#include "simtel_base_station.h"

#include "common/core/request/request_serializer/request_serializer.h"
#include "common/network/socket/socket_message/socket_message.h"
#include "common/utils/network/network.h"
#include "server/core/distance_calculator/distance_calculator.h"
#include "server/core/simtel/simtel_ue_context/simtel_ue_context.h"

namespace server {
std::unordered_map<unsigned int, std::unique_ptr<SimtelBaseStation>>
    SimtelBaseStation::baseStations = {};

std::string SimtelBaseStation::createLogMsg(const std::string &content) const {
  return "BS_" + std::to_string(id) + ": " + content;
}

void SimtelBaseStation::handleConnectionRequest(
    std::shared_ptr<SimtelUeContext> ctx) {
  // начальное получение данных через 1ую вышку
  auto *firstBs = baseStations.begin()->second.get();
  ctx->setBs(firstBs);

  MessageHolder::instance().addMsg(ctx->toStr() +
                                   " receiving location through first BS");
  auto req = firstBs->receiveLocation(ctx);
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

std::optional<std::string>
SimtelBaseStation::sendSignalLevel(const common::imei_t &imei,
                                   unsigned int signalLevel,
                                   std::shared_ptr<SimtelUeContext> ctx) const {
  common::MeasurementControlRequest req{imei, signalLevel, id};
  auto bytes = common::RequestSerializer::measurementControlToBytes(
      ctx->getProtocol(), req);
  if (!bytes) {
    return bytes.error();
  }
  ctx->setBuf(*bytes);

  auto sendError = ctx->sendBufToUe();
  if (!sendError) {
    MessageHolder::instance().addMsg(
        createLogMsg("Measurement_Control response to " + ctx->toStr() + " = " +
                     req.toStr()));
    return std::nullopt;
  }

  return sendError->description;
}

std::expected<common::RrcConnectionRequest, std::string>
SimtelBaseStation::receiveLocation(std::shared_ptr<SimtelUeContext> ctx) const {
  auto receiveError = ctx->receiveData();
  if (receiveError) {
    return std::unexpected(receiveError->description);
  }

  common::Protocol protocol;
  auto req = common::RequestSerializer::rrcConnectionFromBytes(ctx->takeBuf(),
                                                               protocol);
  if (req) {
    ctx->setProtocol(protocol);
    MessageHolder::instance().addMsg(createLogMsg(
        "Rrc_Connection req from " + ctx->toStr() + " = " + req->toStr()));
  }

  return req;
}

std::expected<common::MeasurementReportRequest, std::string>
SimtelBaseStation::receiveChosenBsId(
    std::shared_ptr<SimtelUeContext> ctx) const {
  auto receiveError = ctx->receiveData();
  if (receiveError) {
    return std::unexpected(receiveError->description);
  }

  common::Protocol protocol;
  auto req = common::RequestSerializer::measurementReportFromBytes(
      ctx->takeBuf(), protocol);
  if (req) {
    ctx->setProtocol(protocol);
    MessageHolder::instance().addMsg(createLogMsg(
        "Measurement_Report req from " + ctx->toStr() + " = " + req->toStr()));
  }

  return req;
}

std::optional<std::string>
SimtelBaseStation::sendBsKeep(const common::imei_t &imei,
                              std::shared_ptr<SimtelUeContext> ctx) const {
  common::RrcReconfigurationKeepRequest req{imei, id};
  auto bytes = common::RequestSerializer::rrcReconfigurationKeepToBytes(
      ctx->getProtocol(), req);
  if (!bytes) {
    return bytes.error();
  }
  ctx->setBuf(*bytes);

  auto sendError = ctx->sendBufToUe();
  if (!sendError) {
    MessageHolder::instance().addMsg(
        createLogMsg("Rrc_Reconfiguration_Keep response to " + ctx->toStr() +
                     " = " + req.toStr()));
    return std::nullopt;
  }

  return sendError->description;
}

std::optional<std::string>
SimtelBaseStation::sendError(const std::string &description,
                             std::shared_ptr<SimtelUeContext> ctx) const {
  common::ErrorRequest req{description};
  auto bytes = common::RequestSerializer::errorToBytes(ctx->getProtocol(), req);
  if (!bytes) {
    return bytes.error();
  }
  ctx->setBuf(*bytes);

  auto sendError = ctx->sendBufToUe();
  if (!sendError) {
    MessageHolder::instance().addMsg(createLogMsg(
        "Error response to " + ctx->toStr() + " = " + req.toStr()));
    return std::nullopt;
  }

  return sendError->description;
}

std::optional<std::string>
SimtelBaseStation::sendBsHandover(const common::imei_t &mTmsi,
                                  std::shared_ptr<SimtelUeContext> ctx) const {
  common::RrcReconfigurationHandoverRequest req{mTmsi, id};
  auto bytes = common::RequestSerializer::rrcReconfigurationHandoverToBytes(
      ctx->getProtocol(), req);
  if (!bytes) {
    return bytes.error();
  }
  ctx->setBuf(*bytes);

  auto sendError = ctx->sendBufToUe();
  if (!sendError) {
    MessageHolder::instance().addMsg(
        createLogMsg("Rrc_Reconfiguration_Handover response to " +
                     ctx->toStr() + " = " + req.toStr()));
    return std::nullopt;
  }

  return sendError->description;
}

std::optional<std::string> SimtelBaseStation::handleLocationUpdate(
    const common::RrcConnectionRequest &locReq,
    std::shared_ptr<SimtelUeContext> ctx) {
  // имитация измерения уровня сигнала до базовых станций
  bool signalLevelSent = false;
  auto initialBs = ctx->getBs();
  for (const auto &[id, bs] : baseStations) {
    unsigned int signalLevel = bs->measureSignal(locReq.loc);
    MessageHolder::instance().addMsg(bs->createLogMsg(
        "measured signal level = " + std::to_string(signalLevel)));

    if (signalLevel == 0) {
      continue;
    }

    auto signalSendError = bs->sendSignalLevel(locReq.imei, signalLevel, ctx);
    if (signalSendError) {
      return bs->createLogMsg("Error sending signal level: " +
                              *signalSendError);
    }
    signalLevelSent = true;
  }
  if (!signalLevelSent) {
    return "All BS cant receive signal from client";
  }

  ctx->setBs(initialBs);

  MessageHolder::instance().addMsg("Receiving BS id through initial BS");
  auto chosenBsReq = initialBs->receiveChosenBsId(ctx);
  if (!chosenBsReq) {
    return "Error receiving BS id: " + chosenBsReq.error();
  }
  if (chosenBsReq->imei != locReq.imei) {
    return "Unknown imei received: " + chosenBsReq->imei;
  }
  auto chosenBs = findBs(chosenBsReq->bsId);
  if (!chosenBs) {
    return "Requested BS not found";
  }

  bool handover = false;
  auto error = chosenBs->handleMeasurementReport(*chosenBsReq, ctx, handover);
  if (error) {
    return error;
  }

  if (handover) {
    auto ue = ctx->getBs()->takeUe(ctx->getMTimsi());
    // если UE еще не подключен к какой-либо вышке
    if (!ue) {
      ue = ctx;
    }

    ue->setBs(chosenBs);
    chosenBs->addUe(std::move(ue));
  }

  return std::nullopt;
}

SimtelBaseStation *SimtelBaseStation::findBs(unsigned int id) {
  auto it = baseStations.find(id);
  if (it == baseStations.end()) {
    return nullptr;
  }
  return it->second.get();
}

SimtelBaseStation::SimtelBaseStation(unsigned int id_, float radius_,
                                     size_t maxConnections_,
                                     const common::Location<> &location_)
    : id(id_), radius(radius_), maxConnections(maxConnections_),
      location(location_) {}

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

  auto errorSendError = sendError("MME error", ctx);
  return "MME error";

  common::imsi_t mTimsi = "000000000000000";
  MessageHolder::instance().addMsg(
      createLogMsg("received t-imsi from MME: " + mTimsi));

  bool set = ctx->setMTimsi(mTimsi);
  if (!set) {
    return "UE imsi already set";
  }

  auto curBs = ctx->getBs();
  bool connectedToCur =
      curBs && curBs->getId() == id && ueConnected(ctx->getMTimsi());
  if (connectedToCur || !canAcceptConnection()) {
    auto keepSendError = sendBsKeep(req.imei, ctx);
    if (keepSendError) {
      return createLogMsg("Error sending BS keep info: " + *keepSendError);
    }
  } else {
    auto handoverSendError = sendBsHandover(ctx->getMTimsi(), ctx);
    if (handoverSendError) {
      return createLogMsg("Error sending BS handover info: " +
                          *handoverSendError);
    } else {
      handover = true;
    }
  }

  return std::nullopt;
}

std::shared_ptr<SimtelUeContext>
SimtelBaseStation::takeUe(const common::imsi_t &mTImsi) {
  auto it = connectedUe.find(mTImsi);
  if (it == connectedUe.end()) {
    return nullptr;
  }

  auto ue = it->second;
  connectedUe.erase(it);
  return ue;
}

void SimtelBaseStation::addUe(std::shared_ptr<SimtelUeContext> ctx) {
  connectedUe.emplace(ctx->getMTimsi(), ctx);
  MessageHolder::instance().addMsg(
      createLogMsg(ctx->toStr() + " buffer added "));
}

void SimtelBaseStation::handleUe(std::shared_ptr<SimtelUeContext> ctx) {
  MessageHolder::instance().addMsg(
      createLogMsg("started handling requests from " + ctx->toStr()));
  while (true) {
    auto receiveError = ctx->receiveData();

    if (receiveError) {
      if (common ::isNoConnectedError(*receiveError)) {
        takeUe(ctx->getMTimsi());

        MessageHolder::instance().addMsg(
            createLogMsg(ctx->toStr() + " disconnected"),
            common::MenuMessageType::INFO);
        break;
      } else {
        MessageHolder::instance().addErrorMsg(receiveError->description);
      }
    } else {
      common::binary_t bytes = ctx->takeBuf();
      common::Protocol protocol;
      auto reqType =
          common::RequestSerializer::parseRequestType(bytes, protocol);
      if (!reqType) {
        MessageHolder::instance().addErrorMsg(reqType.error());
      }

      switch (*reqType) {
      case common::RequestType::Rrc_Connection: {
        auto req =
            common::RequestSerializer::rrcConnectionFromBytes(bytes, protocol);
        if (!req) {
          MessageHolder::instance().addErrorMsg(req.error());
        }

        ctx->setProtocol(protocol);
        MessageHolder::instance().addMsg(createLogMsg(
            "Rrc_Connection req from " + ctx->toStr() + " = " + req->toStr()));

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
