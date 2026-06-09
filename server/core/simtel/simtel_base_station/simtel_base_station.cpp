#include "simtel_base_station.h"

#include <iostream>

#include "common/core/request/request_serializer/request_serializer.h"
#include "common/core/request/request_type/request_type.h"
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
  ctx->receiveLocationUpdate();

  auto msgParseResult = common::socketMessageFromBinary(firstBs->getBuf());
  if (!msgParseResult) {
    std::cout << "Error parsing message: " << msgParseResult.error()
              << std::endl;
    return;
  }
  auto clientMsg = std::move(*msgParseResult);

  auto protocolConvertResult =
      common::protocolFromNetworkId(clientMsg.header.protocol);
  if (!protocolConvertResult) {
    std::cout << "Unknown protocol" << std::endl;
    return;
  }

  auto clientReqType =
      static_cast<common::RequestType>(clientMsg.header.msgType);
  if (clientReqType != common::RequestType::Location_Update) {
    std::cout << "Location_Update message expected" << std::endl;
    return;
  }

  auto clientReqParseResult =
      common::RequestSerializer::positionRequestFromBinary(
          clientMsg.header.protocol, clientMsg.content);

  if (!clientReqParseResult) {
    std::cout << "Error parsing request: " << clientReqParseResult.error()
              << std::endl;
  }
  auto clientReq = std::move(*clientReqParseResult);
  firstBs->clearBuf();

  std::cout << "Location received from client: " << clientReq.loc.toStr()
            << std::endl;

  for (const auto &[id, bs] : baseStations) {
    bs->handleLocationUpdate(*protocolConvertResult, clientReq, ctx);
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
  return std::round(coef) * 100;
}

void SimtelBaseStation::handleLocationUpdate(
    common::Protocol clientProtocol, const common::PositionRequest &clientReq,
    std::shared_ptr<SimtelUeContext> ctx) {
  unsigned int signalLevel = measureSignal(clientReq.loc);
  std::cout << "Signal level to " << clientReq.imei << signalLevel << std::endl;

  uint8_t requestTypeBinary =
      static_cast<uint8_t>(common::RequestType::Measurement_Control);

  auto protocolConvertResult = common::protocolToNetworkId(clientProtocol);
  if (!protocolConvertResult) {
    std::cout << "Unknown protocol" << std::endl;
    return;
  }

  common::PositionRequest req{clientReq.imei, getLocation()};
  auto serializeResult =
      common::RequestSerializer::positionRequestToBinary(clientProtocol, req);
  if (!serializeResult) {
    std::cout << "Error serializing request: " << serializeResult.error()
              << std::endl;
    return;
  }

  common::SocketMessage msg{{static_cast<uint32_t>(serializeResult->size()),
                             *protocolConvertResult, requestTypeBinary},
                            *serializeResult};

  auto msgSerializeResult = common::socketMessagetoBinary(msg);
  if (!msgSerializeResult) {
    std::cout << "Error serializing message" << msgSerializeResult.error()
              << std::endl;
    return;
  }

  setBuf(*msgSerializeResult);
  ctx->resendToUe(getBuf());
  clearBuf();
}
} // namespace server
