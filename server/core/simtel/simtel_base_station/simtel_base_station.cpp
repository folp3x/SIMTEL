#include "simtel_base_station.h"

#include "common/core/request/measurement_control_request/measurement_control_request.h"
#include "common/core/request/rrc_connection_request/rrc_connection_request.h"
#include "common/core/request/rrc_reconfiguration_handover_request/rrc_reconfiguration_handover_request.h"
#include "common/core/request/rrc_reconfiguration_keep_request/rrc_reconfiguration_keep_request.h"
#include "common/utils/network/network.h"
#include "server/core/distance_calculator/distance_calculator.h"

namespace server {
std::unordered_map<unsigned int, std::unique_ptr<SimtelBaseStation>>
    SimtelBaseStation::baseStations = {};

std::string SimtelBaseStation::createLogMsg(const std::string &content) const {
  return "BS_" + std::to_string(id) + ": " + content;
}

void SimtelBaseStation::handleConnectionRequest(
    std::shared_ptr<SimtelUeContext> ctx) {
  bool timeoutSet = ctx->setReceiveTimeout();
  if (!timeoutSet) {
    MessageHolder::instance().addErrorMsg(ctx->toStr() +
                                          " error setting receive timeout");
    return;
  }

  // начальное получение данных через 1ую вышку
  auto *firstBs = baseStations.begin()->second.get();
  ctx->setBs(firstBs);

  MessageHolder::instance().addMsg(ctx->toStr() +
                                   " receiving location through first BS");
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

  bool timeoutRemoved = ctx->removeReceiveTimeout();
  if (!timeoutRemoved) {
    MessageHolder::instance().addErrorMsg(ctx->toStr() +
                                          " error removing receive timeout");
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
        createLogMsg("response to " + ctx->toStr() + " = " + req->toStr()));
    return std::nullopt;
  }

  return sendError->description;
}

SimtelBaseStation::SimtelBaseStation(const BsConfig &config)
    : id(config.id), mmeId(config.mmeId), radius(config.radius),
      maxConnections(maxConnections), location(config.loc) {}

std::optional<std::string> SimtelBaseStation::handleLocationUpdate(
    const common::RrcConnectionRequest &locReq,
    std::shared_ptr<SimtelUeContext> ctx) {
  // имитация измерения уровня сигнала до базовых станций
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

  MessageHolder::instance().addMsg("Receiving BS id through initial BS");
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
  auto error = chosenBs->handleMeasurementReport(*chosenBsReq, ctx, handover);
  if (error) {
    return error;
  }

  if (handover) {
    auto ue = ctx->getBs()->copyUe(ctx->getMTimsi());
    // если UE еще не подключен к какой-либо вышке
    if (!ue) {
      ue = ctx;
    }

    // rrc_reconfigure_complete

    ctx->getBs()->removeUe(ctx->getMTimsi());
    ue->setBs(chosenBs);
    chosenBs->addUe(std::move(ue));
  }

  // attach_accept

  return std::nullopt;
}

SimtelBaseStation *SimtelBaseStation::findBs(unsigned int id) {
  auto it = baseStations.find(id);
  if (it == baseStations.end()) {
    return nullptr;
  }
  return it->second.get();
}

void SimtelBaseStation::addBs(std::unique_ptr<SimtelBaseStation> bs) {
  baseStations.emplace(bs->getId(), std::move(bs));
}

bool SimtelBaseStation::ueConnected(const common::imsi_t &mTimsi) const {
  return connectedUe.find(mTimsi) != connectedUe.end();
}

bool SimtelBaseStation::canAcceptConnection() const {
  return connectedUe.size() < maxConnections;
}

std::optional<std::string> SimtelBaseStation::handleMeasurementReport(
    const common::MeasurementReportRequest &req,
    std::shared_ptr<SimtelUeContext> ctx, bool &handover) const {

  // auto errorSendError = sendError("MME error", ctx);
  // return "MME error";

  common::imsi_t mTimsi = "000000000000000";
  MessageHolder::instance().addMsg(
      createLogMsg("received t-imsi from MME: " + mTimsi));

  bool updated = ctx->setMTimsi(mTimsi);
  if (!updated && ctx->getMTimsi() != mTimsi) {
    return "UE IMSI cant be reassigned";
  }

  auto curBs = ctx->getBs();
  bool connectedToCur =
      curBs && curBs->getId() == id && ueConnected(ctx->getMTimsi());
  if (connectedToCur || !canAcceptConnection()) {
    auto response =
        std::make_unique<common::RrcReconfigurationKeepRequest>(mTimsi, id);
    auto responseSendError = sendResponse(ctx, std::move(response));
    if (responseSendError) {
      return createLogMsg("Error sending BS keep info: " + *responseSendError);
    }
  } else {
    auto response =
        std::make_unique<common::RrcReconfigurationHandoverRequest>(mTimsi, id);
    auto responseSendError = sendResponse(ctx, std::move(response));
    if (responseSendError) {
      return createLogMsg("Error sending BS handover info: " +
                          *responseSendError);
    } else {
      handover = true;
    }
  }

  return std::nullopt;
}

void SimtelBaseStation::addUe(std::shared_ptr<SimtelUeContext> ctx) {
  connectedUe.emplace(ctx->getMTimsi(), ctx);
  MessageHolder::instance().addMsg(
      createLogMsg(ctx->toStr() + " buffer added "));
}

std::shared_ptr<SimtelUeContext>
SimtelBaseStation::copyUe(const common::imsi_t &mTImsi) {
  auto it = connectedUe.find(mTImsi);
  if (it == connectedUe.end()) {
    return nullptr;
  }

  MessageHolder::instance().addMsg(
      createLogMsg(it->second->toStr() + " buffer copied "));

  return it->second;
}

bool SimtelBaseStation::removeUe(const common::imsi_t &mTImsi) {
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
      createLogMsg("started handling requests from " + ctx->toStr()));
  while (true) {
    auto receiveError = ctx->receiveData();

    if (receiveError) {
      if (isNoConnectedError(*receiveError)) {
        ctx->setBs(nullptr);
        removeUe(ctx->getMTimsi());

        MessageHolder::instance().addMsg(
            createLogMsg(ctx->toStr() + " disconnected"),
            common::MenuMessageType::INFO);
        break;
      } else {
        MessageHolder::instance().addErrorMsg(receiveError->description);
      }
    } else {
      common::binary_t data = ctx->takeBuf();
      auto reqType = common::parseRequestType(data);
      if (!reqType) {
        MessageHolder::instance().addErrorMsg(reqType.error());
      }

      switch (*reqType) {
      case common::RequestType::Rrc_Connection: {
        common::Protocol protocol;
        auto req = parseFromBytes<common::RrcConnectionRequest>(data, protocol);
        if (!req) {
          MessageHolder::instance().addErrorMsg(req.error());
        }

        ctx->setProtocol(protocol);
        MessageHolder::instance().addMsg(
            createLogMsg("req from " + ctx->toStr() + " = " + req->toStr()));

        auto updateError = handleLocationUpdate(*req, ctx);
        if (updateError) {
          MessageHolder::instance().addErrorMsg(*updateError);
          return;
        }
        break;
      }
      default:
        MessageHolder::instance().addErrorMsg("Unexpected request type");
      }
    }
  }
}
} // namespace server
