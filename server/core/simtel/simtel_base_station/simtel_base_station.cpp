#include "simtel_base_station.h"

#include "common/core/request/rrc_reconfiguration_complete_request/rrc_reconfiguration_complete_request.h"
#include "common/core/request/ussd_code_request/ussd_code_request.h"

#include "common/core/response/attach_accept_response/attach_accept_response.h"
#include "common/core/response/error_response/error_response.h"
#include "common/core/response/measurement_control_response/measurement_control_response.h"

#include "common/core/response/rrc_reconfiguration_handover_response/rrc_reconfiguration_handover_response.h"
#include "common/core/response/rrc_reconfiguration_keep_response/rrc_reconfiguration_keep_response.h"

#include "common/core/response/sm_delivery_error_response/sm_delivery_error_response.h"
#include "common/core/response/sm_delivery_report_response/sm_delivery_report_response.h"

#include "common/core/response/ussd_balance_response/ussd_balance_response.h"
#include "common/core/response/ussd_msisdn_response/ussd_msisdn_response.h"

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

  // начальное получение данных через первую базовую станцию
  auto firstBs = baseStations.begin()->second;
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
  if (auto ptr = bs.lock()) {
    ptr->handleUeRequests(ctx);
  } else {
    MessageHolder::instance().addErrorMsg("UE not connected to BS");
  }
}

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
  auto handleAuthError = mme.lock()->handleAuthResponse(ctx->getMTimsi(), id);
  if (handleAuthError) {
    return "Error changing path: " + *handleAuthError;
  }

  auto response = std::make_unique<common::AttachAcceptResponse>();
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
    dist += std::pow(*it1 - *it2, 2);
    ++it1;
    ++it2;
  }

  return std::sqrt(dist);
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

SimtelBaseStation::SimtelBaseStation(const BsConfig &config,
                                     std::weak_ptr<SimtelMme> mme_)
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

  auto error = ctx->sendBufToUe();
  if (!error) {
    MessageHolder::instance().addMsg(
        createLogMsg("response to " + ctx->toStr() + " = " + req->toStr()),
        common::MenuMessageType::SUCCESS);
    return std::nullopt;
  }

  return error->description;
}

std::optional<std::string> SimtelBaseStation::handleLocationUpdate(
    const common::RrcConnectionRequest &locReq,
    std::shared_ptr<SimtelUeContext> ctx, bool firstConnection) {
  std::lock_guard<std::mutex> lock(*ctx->getSendMtx().get());

  // имитация измерения UE уровня сигнала до базовых станций
  auto initialBs = ctx->getBs();
  if (initialBs.expired()) {
    return "UE not connected to BS";
  }
  auto initialBsPtr = initialBs.lock();

  for (const auto &[id, bs] : baseStations) {
    unsigned int signalLevel = bs->measureSignal(locReq.getLoc());

    if (signalLevel == 0) {
      continue;
    }

    MessageHolder::instance().addMsg(
        bs->createLogMsg("signal level = " + std::to_string(signalLevel)));

    auto response = std::make_unique<common::MeasurementControlResponse>(
        locReq.getImei(), signalLevel, bs->getId());
    auto measurementControlSendError =
        bs->sendResponse(ctx, std::move(response));
    if (measurementControlSendError) {
      return bs->createLogMsg("Error sending signal level: " +
                              *measurementControlSendError);
    }
  }
  ctx->setBs(initialBs);

  MessageHolder::instance().addMsg("Receiving chosen BS id through current BS");

  auto chosenBsReq =
      initialBsPtr->receiveRequest<common::MeasurementReportRequest>(ctx);
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
    if (auto ptr = ctx->getBs().lock()) {
      ue = ptr->copyUe(ctx->getMTimsi());
      // если UE еще не подключен к какой-либо базовой станции
      if (!ue) {
        ue = ctx;
      }
    } else {
      return "UE not connected to BS";
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

  std::shared_ptr<SimtelBaseStation> prevBs = nullptr;
  if (handover) {
    if (auto ptr = ctx->getBs().lock()) {
      prevBs = ptr;
      ue->setBs(chosenBs);
      chosenBs->addUe(std::move(ue));
    } else {
      return "UE not connected to BS";
    }
  }

  auto configureCompleteError = chosenBs->handleConfigureComplete(ctx);

  if (handover && prevBs) {
    prevBs->removeUe(ctx->getMTimsi());
  }

  if (configureCompleteError) {
    return *configureCompleteError;
  }

  return std::nullopt;
}

std::shared_ptr<SimtelBaseStation> SimtelBaseStation::findBs(unsigned int id) {
  auto it = baseStations.find(id);
  if (it == baseStations.end()) {
    return nullptr;
  }
  return it->second;
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
    std::string error = "BS is busy";
    auto response = std::make_unique<common::ErrorResponse>(error);
    auto sendError = sendResponse(ctx, std::move(response));
    if (sendError) {
      return "Error sending error info: " + *sendError;
    }
    return error;
  }

  auto mTimsi = mme.lock()->handleAttachRequest(req.getImsi(), req.getImei());
  if (!mTimsi) {
    auto response =
        std::make_unique<common::ErrorResponse>("Connection failed");
    auto sendError = sendResponse(ctx, std::move(response));
    if (sendError) {
      return "Error sending error info: " + *sendError;
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
  bool connectedToCur = !curBs.expired() && curBs.lock()->getId() == id &&
                        ueConnected(ctx->getMTimsi());
  if (connectedToCur) {
    auto response = std::make_unique<common::RrcReconfigurationKeepResponse>(
        req.getImei(), id);
    auto sendError = sendResponse(ctx, std::move(response));
    if (sendError) {
      return "Error sending BS keep info: " + *sendError;
    }
  } else {
    auto response =
        std::make_unique<common::RrcReconfigurationHandoverResponse>(*mTimsi,
                                                                     id);
    auto sendError = sendResponse(ctx, std::move(response));
    if (sendError) {
      return "Error sending BS handover info: " + *sendError;
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

    auto receiveError = ctx->receiveData();
    if (receiveError) {
      if (isNoConnectedError(*receiveError)) {
        ctx->setBs(std::weak_ptr<SimtelBaseStation>());
        bool removed = removeUe(ctx->getMTimsi());

        if (removed) {
          MessageHolder::instance().addMsg(
              createLogMsg(ctx->toStr() + " disconnected\n"),
              common::MenuMessageType::INFO);
        }

        break;
      }

      MessageHolder::instance().addErrorMsg(receiveError->description);
    } else {
      MessageHolder::instance().addMsg("");

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
        ctx->clearBuf();
        if (!req) {
          MessageHolder::instance().addErrorMsg(req.error());
          continue;
        }

        ctx->setProtocol(protocol);

        MessageHolder::instance().addMsg(
            createLogMsg("request from " + ctx->toStr() + " = " + req->toStr()),
            common::MenuMessageType::INFO);

        auto updateError = handleLocationUpdate(*req, ctx, false);
        if (updateError) {
          MessageHolder::instance().addErrorMsg(*updateError);
        }

        auto bs = ctx->getBs();
        if (auto ptr = bs.lock()) {
          if (ptr->getId() != id) {
            ptr->handleUeRequests(ctx);
          }
        }

        break;
      }
      case common::RequestType::SM_Transfer: {
        common::Protocol protocol;
        auto req = parseFromBytes<common::SmTransferRequest>(data, protocol);
        ctx->clearBuf();
        if (!req) {
          MessageHolder::instance().addErrorMsg(req.error());
          continue;
        }

        MessageHolder::instance().addMsg(
            createLogMsg("request from " + ctx->toStr() + " = " + req->toStr()),
            common::MenuMessageType::INFO);

        ctx->setProtocol(protocol);

        std::string ueErrorMsg = "";
        auto handleError = handleSmTransfer(ctx, *req, ueErrorMsg);
        if (handleError) {
          MessageHolder::instance().addErrorMsg(*handleError);
          auto response = std::make_unique<common::SmDeliveryErrorResponse>(
              req->getMTimsi(), req->getSmsId(), ueErrorMsg);

          std::lock_guard lock(*ctx->getSendMtx().get());
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
        ctx->clearBuf();
        if (!req) {
          MessageHolder::instance().addErrorMsg(req.error());
          continue;
        }

        MessageHolder::instance().addMsg(
            createLogMsg("request from " + ctx->toStr() + " = " + req->toStr()),
            common::MenuMessageType::INFO);

        ctx->setProtocol(protocol);

        mme.lock()->handleSmDeliveryAck(req->getMsisdn(), req->getSmsId(),
                                        req->getMTimsi());

        break;
      }
      case common::RequestType::UssdCode: {
        common::Protocol protocol;
        auto req = parseFromBytes<common::UssdCodeRequest>(data, protocol);
        ctx->clearBuf();
        if (!req) {
          MessageHolder::instance().addErrorMsg(req.error());
          continue;
        }

        MessageHolder::instance().addMsg(
            createLogMsg("request from " + ctx->toStr() + " = " + req->toStr()),
            common::MenuMessageType::INFO);

        ctx->setProtocol(protocol);

        auto error = mme.lock()->handleUssd(req->getMTimsi(), req->getCode());
        if (error) {
          auto response = std::make_unique<common::ErrorResponse>(*error);

          std::lock_guard lock(*ctx->getSendMtx().get());
          auto sendError = sendResponse(ctx, std::move(response));
          if (sendError) {
            MessageHolder::instance().addErrorMsg("Error sending error info: " +
                                                  *sendError);
          }
        }

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
                                    const common::SmTransferRequest &req,
                                    std::string &ueErrorMsg) {
  MessageHolder::instance().addMsg(
      createLogMsg("sent SM_Submit to MME (Sm_Transfer without text)"));

  auto ctxCreateError =
      mme.lock()->handleSmSubmit(req.getMTimsi(), req.getSmsId(), ueErrorMsg);
  if (ctxCreateError) {
    ctx->clearBuf();
    return *ctxCreateError;
  }

  MessageHolder::instance().addMsg(
      createLogMsg("sent MO_Forward_SM to MME (Sm_Transfer text)"));
  bool smsMoved = mme.lock()->handleMoForwardSM(req.getMTimsi(), req.getSmsId(),
                                                req.getText());
  if (!smsMoved) {
    ctx->clearBuf();
    return "SMSC cant move SMS to context";
  }

  ctx->clearBuf();

  return mme.lock()->sendRoutingInfoSm(req.getMsisdn(), req.getSmsId(),
                                       req.getMTimsi(), ueErrorMsg);
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

std::expected<common::SmDeliveryResponse, std::string>
SimtelBaseStation::prepareSmDelivery(const common::imsi_t &mTimsi,
                                     unsigned int smsId,
                                     const common::imsi_t &msisdn) {
  auto ue = findUe(mTimsi);
  if (!ue) {
    return std::unexpected("UE with such m-timsi not connected");
  }

  common::binary_t buf = ue->takeBuf();
  if (buf.empty()) {
    return std::unexpected("No SMS text in buf");
  }

  std::string smsText = common::BinarySerializer::strFromBinary(buf);
  auto response = common::SmDeliveryResponse{mTimsi, smsId, msisdn, smsText};

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
               std::make_unique<common::SmDeliveryResponse>(*preparedReq));
}

std::optional<std::string>
SimtelBaseStation::sendSmDeliveryReport(const common::imsi_t &mTimsi,
                                        unsigned int smsId) {
  auto response =
      std::make_unique<common::SmDeliveryReportResponse>(mTimsi, smsId);

  auto ue = findUe(mTimsi);
  if (!ue) {
    return "UE not connected to BS";
  }

  std::lock_guard lock(*ue->getSendMtx().get());

  return sendResponse(mTimsi, std::move(response));
}

std::optional<std::string>
SimtelBaseStation::sendSmDeliveryError(const common::imsi_t &mTimsi,
                                       unsigned int smsId,
                                       const std::string &description) {
  auto response = std::make_unique<common::SmDeliveryErrorResponse>(
      mTimsi, smsId, description);

  auto ue = findUe(mTimsi);
  if (!ue) {
    return "UE not connected to BS";
  }

  std::lock_guard lock(*ue->getSendMtx().get());

  return sendResponse(mTimsi, std::move(response));
}

std::optional<std::string>
SimtelBaseStation::sendUssdBalance(const common::imsi_t &mTimsi,
                                   double balance) {
  auto response = std::make_unique<common::UssdBalanceResponse>(balance);

  auto ue = findUe(mTimsi);
  if (!ue) {
    return "UE not connected to BS";
  }

  std::lock_guard lock(*ue->getSendMtx().get());

  return sendResponse(mTimsi, std::move(response));
}

std::optional<std::string>
SimtelBaseStation::sendUssdMsisdn(const common::imsi_t &mTimsi,
                                  const common::msisdn_t &msisdn) {
  auto response = std::make_unique<common::UssdMsisdnResponse>(msisdn);

  auto ue = findUe(mTimsi);
  if (!ue) {
    return "UE not connected to BS";
  }

  std::lock_guard lock(*ue->getSendMtx().get());

  return sendResponse(mTimsi, std::move(response));
}
} // namespace server
