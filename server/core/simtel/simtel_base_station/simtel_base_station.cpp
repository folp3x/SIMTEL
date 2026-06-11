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
  baseStations.clear();
  baseStations.emplace(1, std::make_unique<SimtelBaseStation>(1));
  baseStations.emplace(2, std::make_unique<SimtelBaseStation>(2));

  // начальное получение данных через 1ую вышку
  auto *firstBs = baseStations.begin()->second.get();
  ctx->setBs(firstBs);
  auto req = firstBs->receiveLocation(ctx);
  if (!req) {
    std::cout << "Error receiving location: " << req.error() << std::endl;
    return;
  }

  std::cout << "All BS: req from UE_" << ctx->getAddrStr()
            << " = {imei=" + req->imei << ", loc=" << req->loc.toStr() << "}"
            << std::endl;

  handleLocationUpdate(*req, ctx);
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
                                   std::shared_ptr<SimtelUeContext> ctx) {
  common::MeasurementControlRequest req{imei, signalLevel, id};
  auto bytes = common::RequestSerializer::measurementControlToBytes(
      ctx->getProtocol(), req);
  if (!bytes) {
    return bytes.error();
  }
  ctx->setBuf(*bytes);

  return ctx->sendBufToUe();
}

std::expected<common::RrcConnectionRequest, std::string>
SimtelBaseStation::receiveLocation(std::shared_ptr<SimtelUeContext> ctx) {
  auto receiveError = ctx->receiveData();
  if (receiveError) {
    return std::unexpected(*receiveError);
  }

  common::Protocol protocol;
  auto req = common::RequestSerializer::rrcConnectionFromBytes(ctx->takeBuf(),
                                                               protocol);
  if (req) {
    ctx->setProtocol(protocol);
  }

  return req;
}

std::expected<common::MeasurementReportRequest, std::string>
SimtelBaseStation::receiveChosenBsId(std::shared_ptr<SimtelUeContext> ctx) {
  auto receiveError = ctx->receiveData();
  if (receiveError) {
    return std::unexpected(*receiveError);
  }

  common::Protocol protocol;
  auto req = common::RequestSerializer::measurementReportFromBytes(
      ctx->takeBuf(), protocol);
  if (req) {
    ctx->setProtocol(protocol);
  }

  return req;
}

std::optional<std::string>
SimtelBaseStation::sendBsKeep(const common::imei_t &imei,
                              std::shared_ptr<SimtelUeContext> ctx) {
  common::RrcReconfigurationKeepRequest req{imei, id};
  auto bytes = common::RequestSerializer::rrcReconfigurationKeepToBytes(
      ctx->getProtocol(), req);
  if (!bytes) {
    return bytes.error();
  }
  ctx->setBuf(*bytes);

  return ctx->sendBufToUe();
}

std::optional<std::string>
SimtelBaseStation::sendBsHandover(const common::imei_t &mTmsi,
                                  std::shared_ptr<SimtelUeContext> ctx) {
  common::RrcReconfigurationHandoverRequest req{mTmsi, id};
  auto bytes = common::RequestSerializer::rrcReconfigurationHandoverToBytes(
      ctx->getProtocol(), req);
  if (!bytes) {
    return bytes.error();
  }
  ctx->setBuf(*bytes);

  return ctx->sendBufToUe();
}

void SimtelBaseStation::handleLocationUpdate(
    const common::RrcConnectionRequest &locReq,
    std::shared_ptr<SimtelUeContext> ctx) {
  // имитация измерения уровня сигнала до базовых станций
  auto initialBs = ctx->getBs();
  for (const auto &[id, bs] : baseStations) {
    ctx->setBs(bs.get());
    unsigned int signalLevel = bs->measureSignal(locReq.loc);
    std::cout << bs->createLogMsg("measured signal level=" +
                                  std::to_string(signalLevel))
              << std::endl;

    if (signalLevel == 0) {
      continue;
    }

    auto signalSendError = bs->sendSignalLevel(locReq.imei, signalLevel, ctx);
    if (signalSendError) {
      std::cout << bs->createLogMsg("Error sending signal level: " +
                                    *signalSendError)
                << std::endl;
      continue;
    }
  }
  ctx->setBs(initialBs);

  auto receiveError = ctx->receiveData();
  if (receiveError) {
    std::cout << *receiveError << std::endl;
    return;
  }
  common::binary_t bytes = ctx->takeBuf();

  common::Protocol protocol;
  auto reqType = common::RequestSerializer::parseRequestType(bytes, protocol);
  if (!reqType) {
    std::cout << "Error parsing request type: " << reqType.error() << std::endl;
    return;
  }

  if (*reqType == common::RequestType::Measurement_Report) {
    auto chosenBsReq =
        common::RequestSerializer::measurementReportFromBytes(bytes, protocol);
    if (!chosenBsReq) {
      std::cout << "Error receiving BS id" + chosenBsReq.error() << std::endl;
      return;
    }

    if (chosenBsReq->imei != locReq.imei) {
      std::cout << "Unknown imei received: " << chosenBsReq->imei << std::endl;
      return;
    }

    std::cout << "Chosen BS: req from UE_" << ctx->getAddrStr()
              << " = {imei=" << chosenBsReq->imei
              << ", imsi=" << chosenBsReq->imsi
              << ", bsId=" << std::to_string(chosenBsReq->bsId) << "}"
              << std::endl;

    // имитация получение сообщения о выборе вышки конкретной вышкой
    auto bs = findBs(chosenBsReq->bsId);
    if (!bs) {
      std::cout << "Requested BS not found" << std::endl;
      return;
    }

    ctx->setBs(bs);
    bs->handleMeasurementReport(*chosenBsReq, ctx);
  } else {
    std::cout << "MeasurementReport expected" << std::endl;
  }
}

SimtelBaseStation *SimtelBaseStation::findBs(unsigned int id) {
  auto it = baseStations.find(id);
  if (it == baseStations.end()) {
    return nullptr;
  }
  return it->second.get();
}

SimtelBaseStation::SimtelBaseStation(unsigned int id_) : id(id_) {}

bool SimtelBaseStation::ueConnected(const common::imsi_t &imsi) {
  return connectedUe.find(imsi) != connectedUe.end();
}

void SimtelBaseStation::handleMeasurementReport(
    const common::MeasurementReportRequest &req,
    std::shared_ptr<SimtelUeContext> ctx) {
  if (ueConnected(req.imsi)) {
    auto keepSendError = sendBsKeep(req.imsi, ctx);
    if (keepSendError) {
      std::cout << createLogMsg("Error sending BS keep info: " + *keepSendError)
                << std::endl;
    }
  } else {
    auto handoverSendError = sendBsHandover(req.imsi, ctx);
    if (handoverSendError) {
      std::cout << createLogMsg("Error sending BS handover info: " +
                                *handoverSendError)
                << std::endl;
    }
  }
}
} // namespace server
