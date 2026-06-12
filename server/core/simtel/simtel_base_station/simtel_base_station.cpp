#include "simtel_base_station.h"

#include <iostream>

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

  std::cout << "UE_" << ctx->getAddrStr()
            << " receiving location through first BS" << std::endl;
  auto req = firstBs->receiveLocation(ctx);
  if (!req) {
    std::cout << "Error receiving location: " << req.error() << std::endl;
    return;
  }

  auto updateError = handleLocationUpdate(*req, ctx);
  if (updateError) {
    std::cout << *updateError << std::endl;
    return;
  }

  auto bs = ctx->getBs();
  if (!bs) {
    std::cout << "UE not connected to BS" << std::endl;
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
  return (coef < 0) ? 0 : std::round(coef * 100);
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
    std::cout << createLogMsg("Measurement_Control response to UE_" +
                              ctx->getAddrStr() + " = " + req.toStr())
              << std::endl;
  }

  return sendError;
}

std::expected<common::RrcConnectionRequest, std::string>
SimtelBaseStation::receiveLocation(std::shared_ptr<SimtelUeContext> ctx) const {
  auto receiveError = ctx->receiveData();
  if (receiveError) {
    return std::unexpected(*receiveError);
  }

  common::Protocol protocol;
  auto req = common::RequestSerializer::rrcConnectionFromBytes(ctx->takeBuf(),
                                                               protocol);
  if (req) {
    ctx->setProtocol(protocol);
    std::cout << createLogMsg("Rrc_Connection req from UE_" +
                              ctx->getAddrStr() + " = " + req->toStr())
              << std::endl;
  }

  return req;
}

std::expected<common::MeasurementReportRequest, std::string>
SimtelBaseStation::receiveChosenBsId(
    std::shared_ptr<SimtelUeContext> ctx) const {
  auto receiveError = ctx->receiveData();
  if (receiveError) {
    return std::unexpected(*receiveError);
  }

  common::Protocol protocol;
  auto req = common::RequestSerializer::measurementReportFromBytes(
      ctx->takeBuf(), protocol);
  if (req) {
    ctx->setProtocol(protocol);
    std::cout << createLogMsg("Measurement_Report req from UE_" +
                              ctx->getAddrStr() + " = " + req->toStr())
              << std::endl;
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
    std::cout << createLogMsg("Rrc_Reconfiguration_Keep response to UE_" +
                              ctx->getAddrStr() + " = " + req.toStr())
              << std::endl;
  }

  return sendError;
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
    std::cout << createLogMsg("Rrc_Reconfiguration_Handover response to UE_" +
                              ctx->getAddrStr() + " = " + req.toStr())
              << std::endl;
  }

  return sendError;
}

std::optional<std::string> SimtelBaseStation::handleLocationUpdate(
    const common::RrcConnectionRequest &locReq,
    std::shared_ptr<SimtelUeContext> ctx) {
  // имитация измерения уровня сигнала до базовых станций
  auto initialBs = ctx->getBs();
  for (const auto &[id, bs] : baseStations) {
    unsigned int signalLevel = bs->measureSignal(locReq.loc);
    std::cout << bs->createLogMsg("measured signal level = " +
                                  std::to_string(signalLevel))
              << std::endl;

    if (signalLevel == 0) {
      continue;
    }

    auto signalSendError = bs->sendSignalLevel(locReq.imei, signalLevel, ctx);
    if (signalSendError) {
      return bs->createLogMsg("Error sending signal level: " +
                              *signalSendError);
    }
  }
  ctx->setBs(initialBs);

  std::cout << "Receiving BS id through initial BS" << std::endl;
  auto chosenBsReq = initialBs->receiveChosenBsId(ctx);
  if (!chosenBsReq) {
    return "Error receiving BS id" + chosenBsReq.error();
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
  bool set = ctx->setMTimsi("000000000000000");
  // if (!set) {
  //   return "UE imsi already set";
  // }
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
  std::cout << createLogMsg("UE_" + ctx->getAddrStr() + " buffer added ")
            << std::endl;
}

void SimtelBaseStation::handleUe(std::shared_ptr<SimtelUeContext> ctx) {
  std::cout << createLogMsg("started handling requests from UE_" +
                            ctx->getAddrStr())
            << std::endl;
  while (true) {
    auto receiveError = ctx->receiveData();

    if (receiveError == "Connection closed" ||
        receiveError == "Connection reset" ||
        receiveError == "Bad file descriptor") {
      takeUe(ctx->getMTimsi());

      std::cout << createLogMsg("UE_" + ctx->getAddrStr() + " disconnected")
                << std::endl;
      break;
    } else {
      common::binary_t bytes = ctx->takeBuf();

      common::Protocol protocol;
      auto reqType =
          common::RequestSerializer::parseRequestType(bytes, protocol);
      if (!reqType) {
        std::cout << reqType.error() << std::endl;
      }

      switch (*reqType) {
      case common::RequestType::Rrc_Connection: {
        auto req =
            common::RequestSerializer::rrcConnectionFromBytes(bytes, protocol);
        if (!req) {
          std::cout << req.error() << std::endl;
        }

        ctx->setProtocol(protocol);
        std::cout << createLogMsg("Rrc_Connection req from UE_" +
                                  ctx->getAddrStr() + " = " + req->toStr())
                  << std::endl;

        auto updateError = handleLocationUpdate(*req, ctx);
        if (updateError) {
          std::cout << *updateError << std::endl;
          return;
        }
      }
      }
    }
  }
}
} // namespace server
