#include "simtel_base_station.h"

#include "common/core/request/attach_accept_request/attach_accept_request.h"
#include "common/core/request/error_request/error_request.h"
#include "common/core/request/measurement_control_request/measurement_control_request.h"
#include "common/core/request/rrc_reconfiguration_complete_request/rrc_reconfiguration_complete_request.h"
#include "common/core/request/rrc_reconfiguration_handover_request/rrc_reconfiguration_handover_request.h"
#include "common/core/request/rrc_reconfiguration_keep_request/rrc_reconfiguration_keep_request.h"
#include "common/core/request/sm_delivery_report_request/sm_delivery_report_request.h"
#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/simtel_ue_context/simtel_ue_context.h"

namespace server {
std::unordered_map<unsigned int, std::shared_ptr<SimtelBaseStation>>
    SimtelBaseStation::baseStations = {};

std::string SimtelBaseStation::createLogMsg(const std::string &content) const {
  return "BS_" + std::to_string(id) + ": " + content;
}

void SimtelBaseStation::handleConnection(std::shared_ptr<SimtelUeContext> ctx) {
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

  bs->handleUeRequests(ctx);
}

common::Location<> SimtelBaseStation::getLocation() const { return location; }

unsigned int SimtelBaseStation::getId() const { return id; }

unsigned int
SimtelBaseStation::measureSignal(const common::Location<> &targetLoc) const {
  float distance = calculateDistance(targetLoc);
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

float SimtelBaseStation::calculateDistance(
    const common::Location<> &ueLoc) const {
  auto bsCoords = location.getCoords();
  auto ueCoords = ueLoc.getCoords();
  float dist = 0;

  // вычисление евклидова расстояния
  auto it1 = bsCoords.begin();
  auto it2 = ueCoords.begin();
  while (it1 != bsCoords.end() || it2 != ueCoords.end()) {
    dist += pow(*it1 - *it2, 2);
    ++it1;
    ++it2;
  }

  return sqrt(dist);
}

std::shared_ptr<SimtelUeContext>
SimtelBaseStation::findUe(const common::imsi_t &mTimsi) const {
  std::lock_guard lock(connectedUeMtx);

  auto it = connectedUe.find(mTimsi);
  if (it == connectedUe.end()) {
    return nullptr;
  }

  return it->second;
}

SimtelBaseStation::SimtelBaseStation(const BsConfig &config, SimtelMme *mme_)
    : id(config.id), mmeId(config.mmeId), radius(config.radius),
      maxConnections(config.maxConnections), location(config.loc), mme(mme_) {
  if (!mme) {
    throw std::invalid_argument("MME cant be null");
  }

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
    std::shared_ptr<SimtelUeContext> ctx, bool firstConnection) {
  std::lock_guard<std::mutex> lock(*ctx->getSendMtx().get());

  // имитация измерения UE уровня сигнала до базовых станций
  auto initialBs = ctx->getBs();
  for (const auto &[id, bs] : baseStations) {
    unsigned int signalLevel = bs->measureSignal(locReq.getLoc());

    if (signalLevel == 0) {
      continue;
    }

    MessageHolder::instance().addMsg(
        bs->createLogMsg("signal level = " + std::to_string(signalLevel)));

    auto response = std::make_unique<common::MeasurementControlRequest>(
        locReq.getImei(), signalLevel, bs->getId());
    auto responseSendError = bs->sendResponse(ctx, std::move(response));
    if (responseSendError) {
      return bs->createLogMsg("Error sending signal level: " +
                              *responseSendError);
    }
  }
  ctx->setBs(initialBs);

  MessageHolder::instance().addMsg("Receiving chosen BS id through current BS");

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
    return *handleError;
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

bool SimtelBaseStation::ueConnected(const common::imsi_t &mTimsi) const {
  return findUe(mTimsi) != nullptr;
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
      createLogMsg("response from MME: AuthRequest(m-timsi=" + *mTimsi + ")"));

  bool updated = ctx->setMTimsi(*mTimsi);
  if (!updated && ctx->getMTimsi() != *mTimsi) {
    return "UE IMSI cant be reassigned";
  }

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

std::shared_ptr<SimtelUeContext>
SimtelBaseStation::copyUe(const common::imsi_t &mTimsi) {
  auto ue = findUe(mTimsi);

  if (ue) {
    MessageHolder::instance().addMsg(
        createLogMsg(ue->toStr() + " buffer copied "));
  }

  return ue;
}

bool SimtelBaseStation::removeUe(const common::imsi_t &mTimsi) {
  std::lock_guard lock(connectedUeMtx);

  auto it = connectedUe.find(mTimsi);
  if (it == connectedUe.end()) {
    return false;
  }

  MessageHolder::instance().addMsg(
      createLogMsg(it->second->toStr() + " buffer removed "));

  connectedUe.erase(it);
  return true;
}

void SimtelBaseStation::handleUeRequests(std::shared_ptr<SimtelUeContext> ctx) {
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

    MessageHolder::instance().addMsg("");

    auto receiveError = ctx->receiveData();
    if (receiveError) {
      if (isNoConnectedError(*receiveError)) {
        ctx->setBs(nullptr);
        removeUe(ctx->getMTimsi());

        MessageHolder::instance().addMsg(
            createLogMsg(ctx->toStr() + " disconnected\n"),
            common::MenuMessageType::INFO);

        break;
      }

      MessageHolder::instance().addErrorMsg(receiveError->description);
    } else {
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

        auto updateError = handleLocationUpdate(*req, ctx, false);
        if (updateError) {
          MessageHolder::instance().addErrorMsg(*updateError);
        }

        auto bs = ctx->getBs();
        if (bs->getId() != id) {
          ctx->getBs()->handleUeRequests(ctx);
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

        auto handleError = handleSmTransfer(ctx, *req);
        if (handleError) {
          MessageHolder::instance().addErrorMsg(*handleError);
          auto response =
              std::make_unique<common::ErrorRequest>("Failed to deliver SMS");
          auto sendError = sendResponse(ctx, std::move(response));
          if (sendError) {
            MessageHolder::instance().addErrorMsg("Error sending error info: " +
                                                  *sendError);
          }
        }

        break;
      }
      case common::RequestType::SM_Delivery_Ack: {
        common::Protocol protocol;
        auto req = parseFromBytes<common::SmDeliveryAckRequest>(data, protocol);
        if (!req) {
          MessageHolder::instance().addErrorMsg(req.error());
          ctx->clearBuf();
          continue;
        }

        MessageHolder::instance().addMsg(
            createLogMsg("request from " + ctx->toStr() + " = " + req->toStr()),
            common::MenuMessageType::INFO);

        ctx->setProtocol(protocol);

        mme->handleSmDeliveryAck(req->getMsisdn(), req->getSmsId(),
                                 req->getMTimsi());

        break;
      }
      default:
        MessageHolder::instance().addErrorMsg(
            "Unexpected request: " + common::requestTypeToStr(*reqType));
      }
    }
  }
}

std::optional<std::string>
SimtelBaseStation::handleSmTransfer(std::shared_ptr<SimtelUeContext> ctx,
                                    const common::SmTransferRequest &req) {
  MessageHolder::instance().addMsg(
      createLogMsg("sent SM_Submit to MME (Sm_Transfer without text)"));
  bool contextCreated = mme->handleSmSubmit(req.getMTimsi(), req.getSmsId());
  if (!contextCreated) {
    ctx->clearBuf();
    return "SMSC cant create context for SMS";
  }

  MessageHolder::instance().addMsg(
      createLogMsg("sent MO_Forward_SM to MME (Sm_Transfer text)"));
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

bool SimtelBaseStation::handleForwardSmReq(const common::imsi_t &mTimsi,
                                           size_t smsTextSize) {
  auto ue = findUe(mTimsi);
  if (!ue) {
    return false;
  }

  MessageHolder::instance().addMsg(createLogMsg("received Forward_SM_Request"));

  ue->aquireBuf(smsTextSize);
  return true;
}

bool SimtelBaseStation::handleMtForwardSm(const common::imsi_t &mTimsi,
                                          const common::binary_t &smsText) {
  auto ue = findUe(mTimsi);
  if (!ue) {
    return false;
  }

  MessageHolder::instance().addMsg(
      createLogMsg("received MT_Forward_SM_Request"));

  return ue->fillBuf(smsText);
}

std::expected<common::SmDeliveryRequest, std::string>
SimtelBaseStation::prepareSmDelivery(const common::imsi_t &mTimsi,
                                     unsigned int smsId,
                                     const common::imsi_t &msisdn) {
  auto ue = findUe(mTimsi);
  if (!ue) {
    return std::unexpected("UE with such m-timsi not connected");
  }

  auto buf = ue->takeBuf();
  if (buf.empty()) {
    return std::unexpected("No SMS text in buf");
  }

  std::string smsText = common::BinarySerializer::strFromBinary(buf);
  auto response = common::SmDeliveryRequest{mTimsi, smsId, msisdn, smsText};

  MessageHolder::instance().addMsg(
      createLogMsg("prepared response: " + response.toStr()));

  return response;
}

std::optional<std::string>
SimtelBaseStation::sendResponse(const common::imsi_t &mTimsi,
                                std::unique_ptr<common::Request> req) {
  auto ue = findUe(mTimsi);
  if (!ue) {
    return "UE with such m-timsi not connected";
  }

  return sendResponse(ue, std::move(req));
}

void SimtelBaseStation::sendSmDelivery(const common::imsi_t &mTimsi,
                                       unsigned int smsId,
                                       const common::imsi_t &msisdn,
                                       const common::binary_t &smsText) {
  auto ue = findUe(mTimsi);
  if (!ue) {
    return;
  }

  std::lock_guard lock(*ue->getSendMtx().get());

  if (!handleForwardSmReq(mTimsi, smsText.size())) {
    return;
  }

  if (!handleMtForwardSm(mTimsi, smsText)) {
    return;
  }

  auto preparedReq = prepareSmDelivery(mTimsi, smsId, msisdn);
  if (!preparedReq) {
    return;
  }

  sendResponse(mTimsi,
               std::make_unique<common::SmDeliveryRequest>(*preparedReq));
}

std::optional<std::string>
SimtelBaseStation::sendDeliveryReport(const common::imsi_t &mTimsi,
                                      unsigned int smsId) {
  auto reportReq =
      std::make_unique<common::SmDeliveryReportRequest>(mTimsi, smsId);

  return sendResponse(mTimsi, std::move(reportReq));
}
} // namespace server
