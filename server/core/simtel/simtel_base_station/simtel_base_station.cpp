#include "simtel_base_station.h"

#include "common/core/request/attach_accept_request/attach_accept_request.h"
#include "common/core/request/error_request/error_request.h"
#include "common/core/request/measurement_control_request/measurement_control_request.h"
#include "common/core/request/rrc_reconfiguration_complete_request/rrc_reconfiguration_complete_request.h"
#include "common/core/request/rrc_reconfiguration_handover_request/rrc_reconfiguration_handover_request.h"
#include "common/core/request/rrc_reconfiguration_keep_request/rrc_reconfiguration_keep_request.h"
#include "common/core/request/sm_delivery_report_request/sm_delivery_report_request.h"
#include "server/app/message_holder/message_holder.h"
#include "server/core/distance_calculator/distance_calculator.h"
#include "server/core/simtel/simtel_ue_context/simtel_ue_context.h"

namespace server {
std::unordered_map<unsigned int, std::shared_ptr<SimtelBaseStation>>
    SimtelBaseStation::baseStations = {};

std::shared_ptr<TtlManager> SimtelBaseStation::ttlManager = nullptr;

std::string SimtelBaseStation::createLogMsg(const std::string &content) const {
  return "BS_" + std::to_string(id) + ": " + content;
}

void SimtelBaseStation::handleConnectionRequest(
    std::shared_ptr<SimtelUeContext> ctx) {
  if (!ttlManager) {
    throw std::runtime_error("TTL manager not set");
  }

  ttlManager->setActive(false);

  bool timeoutSet =
      ctx->setReceiveTimeout(CONNECTION_HANDLE_RECEIVE_TIMEOUT_MSEC);
  if (!timeoutSet) {
    MessageHolder::instance().addErrorMsg(ctx->toStr() +
                                          " error setting receive timeout");
    return;
  }

  // начальное получение данных через первую вышку
  auto *firstBs = baseStations.begin()->second.get();
  ctx->setBs(firstBs);

  MessageHolder::instance().addMsg("Receiving location through first BS");

  auto req = firstBs->receiveRequest<common::RrcConnectionRequest>(ctx);
  if (!req) {
    MessageHolder::instance().addErrorMsg("Error receiving location: " +
                                          req.error());
    return;
  }

  auto updateError = handleLocationUpdate(*req, ctx);
  if (updateError) {
    MessageHolder::instance().addErrorMsg(*updateError);
    return;
  }

  auto bs = ctx->getBs();
  if (!bs) {
    MessageHolder::instance().addErrorMsg("UE not connected to BS");
    return;
  }

  bs->handleUe(ctx);
}

common::Location<> SimtelBaseStation::getLocation() const { return location; }

unsigned int SimtelBaseStation::getId() const { return id; }

unsigned int
SimtelBaseStation::measureSignal(const common::Location<> &targetLoc) const {
  float distance = DistanceCalculator::calc(location, targetLoc);
  float coef = 1 - std::abs(distance) / radius;
  return (coef < 0) ? 0
                    : std::round(coef * common::constants::MAX_SIGNAL_LEVEL);
}

std::optional<std::string> SimtelBaseStation::handleConfigureComplete(
    std::shared_ptr<SimtelUeContext> ctx) const {
  auto changePathError = mme->handleAuthResponse(ctx->getMTimsi(), id);
  if (changePathError) {
    return "Error changing path: " + *changePathError;
  }

  auto response = std::make_unique<common::AttachAcceptRequest>();
  auto responseSendError = sendResponse(ctx, std::move(response));
  if (responseSendError) {
    return "Error sending attach accept: " + *responseSendError;
  }

  return std::nullopt;
}

SimtelBaseStation::SimtelBaseStation(const BsConfig &config, SimtelMme *mme_)
    : id(config.id), mmeId(config.mmeId), radius(config.radius),
      maxConnections(config.maxConnections), location(config.loc), mme(mme_) {
  std::lock_guard lock(connectedUeMtx);
  connectedUe.reserve(maxConnections);
}

std::optional<std::string>
SimtelBaseStation::sendResponse(std::shared_ptr<SimtelUeContext> ctx,
                                std::unique_ptr<common::Request> req) const {
  auto bytes = req->toBytes(ctx->getProtocol());
  if (!bytes) {
    return bytes.error();
  }
  ctx->setBuf(*bytes);

  auto sendError = ctx->sendBufToUe();
  if (!sendError) {
    MessageHolder::instance().addMsg(
        createLogMsg("response to " + ctx->toStr() + " = " + req->toStr()),
        common::MenuMessageType::SUCCESS);
    return std::nullopt;
  }

  return sendError->description;
}

std::optional<std::string> SimtelBaseStation::handleLocationUpdate(
    const common::RrcConnectionRequest &locReq,
    std::shared_ptr<SimtelUeContext> ctx) {
  // имитация измерения UE уровня сигнала до базовых станций
  auto initialBs = ctx->getBs();
  for (const auto &[id, bs] : baseStations) {
    unsigned int signalLevel = bs->measureSignal(locReq.getLoc());
    MessageHolder::instance().addMsg(bs->createLogMsg(
        "measured signal level = " + std::to_string(signalLevel)));

    if (signalLevel == 0) {
      continue;
    }

    auto response = std::make_unique<common::MeasurementControlRequest>(
        locReq.getImei(), signalLevel, bs->getId());
    auto responseSendError = bs->sendResponse(ctx, std::move(response));
    if (responseSendError) {
      return bs->createLogMsg("Error sending signal level: " +
                              *responseSendError);
    }
  }
  ctx->setBs(initialBs);

  MessageHolder::instance().addMsg("Receiving BS id through current BS");
  auto chosenBsReq =
      initialBs->receiveRequest<common::MeasurementReportRequest>(ctx);
  if (!chosenBsReq) {
    return "Error receiving BS id: " + chosenBsReq.error();
  }

  if (chosenBsReq->getImei() != locReq.getImei()) {
    return "Unknown imei received: " + chosenBsReq->getImei();
  }
  auto chosenBs = findBs(chosenBsReq->getBsId());
  if (!chosenBs) {
    return "Requested BS not found";
  }

  bool handover = false;
  auto handleError =
      chosenBs->handleMeasurementReport(*chosenBsReq, ctx, handover);
  if (handleError) {
    return handleError;
  }

  std::shared_ptr<SimtelUeContext> ue = nullptr;
  if (handover) {
    ue = ctx->getBs()->copyUe(ctx->getMTimsi());
    // если UE еще не подключен к какой-либо вышке
    if (!ue) {
      ue = ctx;
    }
  }

  auto configureConfirm =
      chosenBs->receiveRequest<common::RrcReconfigurationCompleteRequest>(ctx);
  if (!configureConfirm) {
    return "Error receiving configure confirm: " + configureConfirm.error();
  }
  if (configureConfirm->getMTimsi() != ctx->getMTimsi()) {
    return "Unknown m-timsi received: " + configureConfirm->getMTimsi();
  }

  if (handover) {
    ctx->getBs()->removeUe(ctx->getMTimsi());
    ue->setBs(chosenBs);
    chosenBs->addUe(std::move(ue));
  }

  std::lock_guard lock(*chosenBs->getSendMtx(ctx->getMTimsi()));
  return chosenBs->handleConfigureComplete(ctx);
}

SimtelBaseStation *SimtelBaseStation::findBs(unsigned int id) {
  auto it = baseStations.find(id);
  if (it == baseStations.end()) {
    return nullptr;
  }
  return it->second.get();
}

void SimtelBaseStation::addBs(std::shared_ptr<SimtelBaseStation> bs) {
  baseStations.emplace(bs->getId(), bs);
}

void SimtelBaseStation::setTtlManager(std::shared_ptr<TtlManager> ttlManager_) {
  ttlManager = ttlManager_;
}

bool SimtelBaseStation::ueConnected(const common::imsi_t &mTimsi) const {
  std::lock_guard lock(connectedUeMtx);
  return connectedUe.find(mTimsi) != connectedUe.end();
}

bool SimtelBaseStation::canAcceptConnection() const {
  std::lock_guard lock(connectedUeMtx);
  return connectedUe.size() < maxConnections;
}

std::optional<std::string> SimtelBaseStation::handleMeasurementReport(
    const common::MeasurementReportRequest &req,
    std::shared_ptr<SimtelUeContext> ctx, bool &handover) const {
  if (!canAcceptConnection()) {
    std::string error = "BS busy";
    auto response = std::make_unique<common::ErrorRequest>(error);
    auto responseSendError = sendResponse(ctx, std::move(response));
    if (responseSendError) {
      return "Error sending error info: " + *responseSendError;
    }
    return error;
  }

  auto mTimsi = mme->handleAttachRequest(req.getImsi(), req.getImei());
  if (!mTimsi) {
    auto response = std::make_unique<common::ErrorRequest>(mTimsi.error());
    auto responseSendError = sendResponse(ctx, std::move(response));
    if (responseSendError) {
      return "Error sending error info: " + *responseSendError;
    }
    return mTimsi.error();
  }

  MessageHolder::instance().addMsg(
      createLogMsg("received m-timsi from MME: " + *mTimsi));

  bool updated = ctx->setMTimsi(*mTimsi);
  if (!updated && ctx->getMTimsi() != *mTimsi) {
    return "UE IMSI cant be reassigned";
  }

  MessageHolder::instance().addMsg("Sending AuthRequest through chosen BS");

  auto curBs = ctx->getBs();
  bool connectedToCur =
      curBs && curBs->getId() == id && ueConnected(ctx->getMTimsi());
  if (connectedToCur) {
    auto response = std::make_unique<common::RrcReconfigurationKeepRequest>(
        req.getImei(), id);
    auto responseSendError = sendResponse(ctx, std::move(response));
    if (responseSendError) {
      return "Error sending BS keep info: " + *responseSendError;
    }
  } else {
    auto response = std::make_unique<common::RrcReconfigurationHandoverRequest>(
        *mTimsi, id);
    auto responseSendError = sendResponse(ctx, std::move(response));
    if (responseSendError) {
      return "Error sending BS handover info: " + *responseSendError;
    } else {
      handover = true;
    }
  }

  return std::nullopt;
}

void SimtelBaseStation::addUe(std::shared_ptr<SimtelUeContext> ctx) {
  {
    std::lock_guard lock(connectedUeMtx);
    connectedUe.emplace(ctx->getMTimsi(), ctx);
  }
  MessageHolder::instance().addMsg(
      createLogMsg(ctx->toStr() + " buffer added"));
}

std::mutex *SimtelBaseStation::getSendMtx(const common::imsi_t &mTimsi) {
  std::lock_guard<std::mutex> lock(sendMtxListLock);

  auto it = sendMtxList.find(mTimsi);
  if (it == sendMtxList.end()) {
    auto [newIt, inserted] =
        sendMtxList.emplace(mTimsi, std::make_unique<std::mutex>());
    return newIt->second.get();
  }
  return it->second.get();
}

std::shared_ptr<SimtelUeContext>
SimtelBaseStation::copyUe(const common::imsi_t &mTImsi) {
  std::lock_guard lock(connectedUeMtx);
  auto it = connectedUe.find(mTImsi);
  if (it == connectedUe.end()) {
    return nullptr;
  }

  MessageHolder::instance().addMsg(
      createLogMsg(it->second->toStr() + " buffer copied "));

  return it->second;
}

bool SimtelBaseStation::removeUe(const common::imsi_t &mTImsi) {
  std::lock_guard lock(connectedUeMtx);
  auto it = connectedUe.find(mTImsi);
  if (it == connectedUe.end()) {
    return false;
  }

  MessageHolder::instance().addMsg(
      createLogMsg(it->second->toStr() + " buffer removed "));

  connectedUe.erase(it);

  return true;
}

void SimtelBaseStation::handleUe(std::shared_ptr<SimtelUeContext> ctx) {
  MessageHolder::instance().addMsg(
      createLogMsg("started handling requests from " + ctx->toStr()),
      common::MenuMessageType::INFO);

  while (true) {
    bool timeoutRemoved = ctx->removeReceiveTimeout();
    if (!timeoutRemoved) {
      MessageHolder::instance().addErrorMsg(ctx->toStr() +
                                            " error removing receive timeout");
      return;
    }

    ttlManager->update();
    ttlManager->setActive(true);

    MessageHolder::instance().addMsg("");

    auto receiveError = ctx->receiveData();

    if (receiveError) {
      if (isNoConnectedError(*receiveError)) {
        ctx->setBs(nullptr);
        removeUe(ctx->getMTimsi());

        MessageHolder::instance().addMsg(
            createLogMsg(ctx->toStr() + " disconnected"),
            common::MenuMessageType::INFO);

        break;
      }

      MessageHolder::instance().addErrorMsg(receiveError->description);
    } else {
      ttlManager->setActive(false);

      common::binary_t data = ctx->copyBuf();
      auto reqType = common::parseRequestType(data);
      if (!reqType) {
        MessageHolder::instance().addErrorMsg(reqType.error());
        ctx->clearBuf();
        continue;
      }

      switch (*reqType) {
      case common::RequestType::Rrc_Connection: {
        common::Protocol protocol;
        auto req = parseFromBytes<common::RrcConnectionRequest>(data, protocol);
        if (!req) {
          MessageHolder::instance().addErrorMsg(req.error());
          ctx->clearBuf();
          continue;
        }

        ctx->setProtocol(protocol);

        MessageHolder::instance().addMsg(
            createLogMsg("request from " + ctx->toStr() + " = " + req->toStr()),
            common::MenuMessageType::INFO);

        ctx->clearBuf();

        auto updateError = handleLocationUpdate(*req, ctx);
        if (updateError) {
          MessageHolder::instance().addErrorMsg(*updateError);
        }

        break;
      }
      case common::RequestType::SM_Transfer: {
        common::Protocol protocol;
        auto req = parseFromBytes<common::SmTransferRequest>(data, protocol);
        if (!req) {
          MessageHolder::instance().addErrorMsg(req.error());
          ctx->clearBuf();
          continue;
        }

        MessageHolder::instance().addMsg(
            createLogMsg("request from " + ctx->toStr() + " = " + req->toStr()),
            common::MenuMessageType::INFO);

        ctx->setProtocol(protocol);

        std::string error = "";
        {
          std::lock_guard lock(*getSendMtx(ctx->getMTimsi()));

          auto handleError = handleSmTransfer(ctx, *req);
          if (handleError) {
            error = *handleError;
          }

          if (!error.empty()) {
            MessageHolder::instance().addErrorMsg(error);
            auto response =
                std::make_unique<common::ErrorRequest>("Failed to deliver SMS");
            auto responseSendError = sendResponse(ctx, std::move(response));
            if (responseSendError) {
              MessageHolder::instance().addErrorMsg(
                  "Error sending error info: " + *responseSendError);
            }
          }
        }

        break;
      }
      default:
        MessageHolder::instance().addErrorMsg(
            "Unexpected request type - Rrc_Connection or SM_Transfer sxpected");
      }
    }
  }
}

std::optional<std::string>
SimtelBaseStation::handleSmTransfer(std::shared_ptr<SimtelUeContext> ctx,
                                    const common::SmTransferRequest &req) {
  MessageHolder::instance().addMsg(createLogMsg("sending SM_Submit to MME"));
  bool contextCreated = mme->handleSmSubmit(req.getMTimsi(), req.getSmsId());
  if (!contextCreated) {
    ctx->clearBuf();
    return "SMSC cant create context for SMS";
  }

  MessageHolder::instance().addMsg(
      createLogMsg("sending MO_Forward_SM to MME"));
  bool smsMoved =
      mme->handleMoForwardSM(req.getMTimsi(), req.getSmsId(), req.getText());
  if (!smsMoved) {
    ctx->clearBuf();
    return "SMSC cant move SMS to context";
  }

  ctx->clearBuf();

  return mme->sendRoutingInfoSm(req.getMsisdn(), req.getSmsId(),
                                req.getMTimsi());
}

bool SimtelBaseStation::handleForwardSmReq(const common::imsi_t &imsi,
                                           size_t smsTextSize) {
  std::lock_guard lock(connectedUeMtx);
  auto it = connectedUe.find(imsi);
  if (it == connectedUe.end()) {
    return false;
  }

  MessageHolder::instance().addMsg(createLogMsg("received Forward_SM_Request"));

  it->second->aquireBuf(smsTextSize);
  return true;
}

bool SimtelBaseStation::handleMtForwardSm(const common::imsi_t &imsi,
                                          const common::binary_t &smsText) {
  std::lock_guard lock(connectedUeMtx);
  auto it = connectedUe.find(imsi);
  if (it == connectedUe.end()) {
    return false;
  }

  MessageHolder::instance().addMsg(
      createLogMsg("received MT_Forward_SM_Request"));

  return it->second->fillBuf(smsText);
}

std::expected<common::SmDeliveryRequest, std::string>
SimtelBaseStation::prepareSmDelivery(const common::imsi_t &imsi,
                                     unsigned int smsId,
                                     const common::imsi_t &msisdn) {
  std::shared_ptr<SimtelUeContext> ctx;
  {
    std::lock_guard lock(connectedUeMtx);
    auto it = connectedUe.find(imsi);
    if (it == connectedUe.end()) {
      return std::unexpected("UE with such imsi not connected");
    }
    ctx = it->second;
  }

  auto buf = ctx->takeBuf();
  if (buf.empty()) {
    return std::unexpected("No SMS text in buf");
  }

  std::string smsText = common::BinarySerializer::strFromBinary(buf);
  auto response = common::SmDeliveryRequest{imsi, smsId, msisdn, smsText};

  MessageHolder::instance().addMsg(
      createLogMsg("prepared response: " + response.toStr()));

  return response;
}

void SimtelBaseStation::sendSmDelivery(
    const common::imsi_t &imsi, const common::SmDeliveryRequest &response,
    bool &ueFound) {
  std::shared_ptr<SimtelUeContext> ctx;

  {
    std::lock_guard lock(connectedUeMtx);
    auto it = connectedUe.find(imsi);
    if (it == connectedUe.end()) {
      ueFound = false;
      return;
    }
    ctx = it->second;
  }

  std::lock_guard lock(*getSendMtx(imsi));
  sendResponse(ctx, std::make_unique<common::SmDeliveryRequest>(response));
}

std::optional<common::SmDeliveryAckRequest>
SimtelBaseStation::receiveSmDeliveryAck(const common::imsi_t &imsi,
                                        bool &ueFound) {

  std::shared_ptr<SimtelUeContext> ctx;
  {
    std::lock_guard lock(connectedUeMtx);
    auto it = connectedUe.find(imsi);
    if (it == connectedUe.end()) {
      ueFound = false;
      return std::nullopt;
    }
    ctx = it->second;
  }

  ctx->setReceiveTimeout(SM_DELIVERY_ACK_RECEIVE_TIMEOUT_MSEC);
  auto ackReq = receiveRequest<common::SmDeliveryAckRequest>(ctx);
  if (!ackReq) {
    return std::nullopt;
  }

  return *ackReq;
}
} // namespace server
