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
  baseStations.emplace(0, std::make_unique<SimtelBaseStation>());

  // начальное получение данных через 1ую вышку
  auto *firstBs = baseStations.begin()->second.get();
  ctx->setBs(firstBs);
  ctx->translateMessage();

  auto msg = common::socketMessageFromBinary(firstBs->getBuf());
  firstBs->clearBuf();
  if (!msg) {
    std::cout << "Error parsing message: " << msg.error() << std::endl;
    return;
  }

  auto protocol = common::protocolFromNetworkId(msg->header.protocol);
  if (!protocol) {
    std::cout << "Unsupported protocol" << std::endl;
    return;
  }

  auto reqType = static_cast<common::RequestType>(msg->header.msgType);
  if (reqType != common::RequestType::Location_Update) {
    std::cout << "Location_Update message expected" << std::endl;
    return;
  }

  auto req = common::RequestSerializer::positionRequestFromBinary(
      msg->header.protocol, msg->content);
  if (!req) {
    std::cout << "Error parsing request: " << req.error() << std::endl;
  }

  std::cout << "Location received from client: " << req->loc.toStr()
            << std::endl;

  for (const auto &[id, bs] : baseStations) {
    bs->handleLocationUpdate(*protocol, *req, ctx);
  }
}

common::binary_t SimtelBaseStation::getBuf() const { return buf; }

void SimtelBaseStation::setBuf(const common::binary_t &buf_) { buf = buf_; }

void SimtelBaseStation::clearBuf() { buf.clear(); }

common::Location<> SimtelBaseStation::getLocation() const { return location; }

unsigned int
SimtelBaseStation::measureSignal(const common::Location<> &targetLoc) const {
  float distance = DistanceCalculator::calc(location, targetLoc);
  float coef = 1 - std::abs(distance) / radius;
  return (coef < 0) ? 0 : std::round(coef * 100);
}

std::optional<std::string> SimtelBaseStation::sendSignalLevel(
    common::Protocol clientProtocol, const common::imei_t &imei,
    unsigned int signalLevel, std::shared_ptr<SimtelUeContext> ctx) {
  uint8_t requestTypeBinary =
      static_cast<uint8_t>(common::RequestType::Measurement_Control);

  auto protocolId = common::protocolToNetworkId(clientProtocol);
  if (!protocolId) {
    return "Unknown protocol";
  }

  common::SignalRequest req{imei, signalLevel, id};
  auto serializedReq =
      common::RequestSerializer::signalRequestToBinary(clientProtocol, req);
  if (!serializedReq) {
    return "Error serializing request: " + serializedReq.error();
  }

  common::SocketMessage msg{{static_cast<uint32_t>(serializedReq->size()),
                             *protocolId, requestTypeBinary},
                            *serializedReq};

  auto serializedMsg = common::socketMessagetoBinary(msg);
  if (!serializedMsg) {
    return "Error serializing message" + serializedMsg.error();
  }

  setBuf(*serializedMsg);
  ctx->resendToUe(getBuf());
  clearBuf();

  return std::nullopt;
}

void SimtelBaseStation::handleLocationUpdate(
    common::Protocol clientProtocol, const common::PositionRequest &clientReq,
    std::shared_ptr<SimtelUeContext> ctx) {
  unsigned int signalLevel = measureSignal(clientReq.loc);
  std::cout << "Signal level to IMSI" << clientReq.imei << ": " << signalLevel
            << std::endl;

  auto error =
      sendSignalLevel(clientProtocol, clientReq.imei, signalLevel, ctx);
  if (error) {
    std::cout << *error << std::endl;
    return;
  }
}
} // namespace server
