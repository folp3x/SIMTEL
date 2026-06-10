#include "simtel_base_station.h"

#include <iostream>

#include "common/core/request/request_serializer/request_serializer.h"
#include "common/network/socket/socket_message/socket_message.h"
#include "server/core/distance_calculator/distance_calculator.h"
#include "server/core/simtel/simtel_ue_context/simtel_ue_context.h"

namespace server {
std::unordered_map<unsigned int, std::unique_ptr<SimtelBaseStation>>
    SimtelBaseStation::baseStations = {};

void SimtelBaseStation::handleConnectionRequest(
    std::shared_ptr<SimtelUeContext> ctx) {
  baseStations.clear();
  baseStations.emplace(1, std::make_unique<SimtelBaseStation>());

  // начальное получение данных через 1ую вышку
  auto *firstBs = baseStations.begin()->second.get();
  ctx->setBs(firstBs);
  auto req = firstBs->receiveLocation(ctx);
  if (!req) {
    std::cout << "Error receiving location: " << req.error() << std::endl;
    return;
  }

  std::cout << "IMEI_" + req->imei + "sent location: " << req->loc.toStr()
            << std::endl;

  for (const auto &[id, bs] : baseStations) {
    bs->handleLocationUpdate(*req, ctx);
  }
}

common::binary_t SimtelBaseStation::getBuf() const { return buf; }

void SimtelBaseStation::setBuf(const common::binary_t &buf_) { buf = buf_; }

void SimtelBaseStation::clearBuf() { buf.clear(); }

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

  setBuf(*bytes);
  ctx->translateToUe(getBuf());
  clearBuf();

  return std::nullopt;
}

std::expected<common::RrcConnectionRequest, std::string>
SimtelBaseStation::receiveLocation(std::shared_ptr<SimtelUeContext> ctx) {
  auto translateError = ctx->translateToBs();
  if (translateError) {
    return std::unexpected(*translateError);
  }

  common::Protocol protocol;
  auto req = common::RequestSerializer::rrcConnectionFromBytes(buf, protocol);
  clearBuf();
  if (req) {
    ctx->setProtocol(protocol);
  }

  return req;
}

std::expected<common::MeasurementReportRequest, std::string>
SimtelBaseStation::receiveChosenBsId(std::shared_ptr<SimtelUeContext> ctx) {
  auto translateError = ctx->translateToBs();
  if (translateError) {
    return std::unexpected(*translateError);
  }

  common::Protocol protocol;
  auto req =
      common::RequestSerializer::measurementReportFromBytes(buf, protocol);
  clearBuf();
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

  setBuf(*bytes);
  ctx->translateToUe(getBuf());
  clearBuf();

  return std::nullopt;
}

void SimtelBaseStation::handleLocationUpdate(
    const common::RrcConnectionRequest &req,
    std::shared_ptr<SimtelUeContext> ctx) {

  common::imei_t ueImei = req.imei;
  unsigned int signalLevel = measureSignal(req.loc);

  auto signalSendError = sendSignalLevel(ueImei, signalLevel, ctx);
  if (signalSendError) {
    std::cout << "Error sending signal level to imei_" << req.imei << ": "
              << *signalSendError << std::endl;
    return;
  }

  auto chosenBsReq = receiveChosenBsId(ctx);
  if (!chosenBsReq) {
    std::cout << "Error receiving BS id from imei_" << req.imei << ": "
              << chosenBsReq.error() << std::endl;
    return;
  }

  if (chosenBsReq->imei != ueImei) {
    std::cout << "Unknown imei received: imei_" << chosenBsReq->imei
              << std::endl;
    return;
  }

  std::cout << "imei_" << ueImei << " chose BS: " << chosenBsReq->bsId
            << std::endl;

  std::cout << 1 << std::endl;

  auto curBsId = ctx->getBsId();
  if (curBsId && chosenBsReq->bsId == *curBsId) {
    std::cout << 2 << std::endl;
    auto bsKeepSendError = sendBsKeep(ueImei, ctx);
    if (bsKeepSendError) {
      std::cout << "Error sending BS keep info: " << *bsKeepSendError
                << std::endl;
    }
    std::cout << 3 << std::endl;
  }
}
} // namespace server
