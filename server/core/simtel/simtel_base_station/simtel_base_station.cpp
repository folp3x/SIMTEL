#include "simtel_base_station.h"

#include <iostream>

#include "common/core/request/request_serializer/request_serializer.h"
#include "common/core/request/request_type/request_type.h"
#include "common/network/socket/socket_message/socket_message.h"
#include "server/core/simtel/simtel_ue_context/simtel_ue_context.h"

namespace server {
void SimtelBaseStation::addBs() {
  baseStations.emplace(1, std::make_unique<SimtelBaseStation>());
}

std::queue<std::shared_ptr<SimtelUeContext>>
    SimtelBaseStation::connectionRequests = {};

std::unordered_map<unsigned int, std::unique_ptr<SimtelBaseStation>>
    SimtelBaseStation::baseStations = {};

void SimtelBaseStation::handleConnectionRequest(
    std::shared_ptr<SimtelUeContext> ctx) {
  for (const auto &[id, bs] : baseStations) {
    ctx->setBs(bs.get());
    ctx->receiveLocationUpdate();

    auto msgParseResult = common::socketMessageFromBinary(bs->getBuf());
    if (!msgParseResult) {
      continue;
    }
    auto clientMsg = std::move(*msgParseResult);

    auto clientReqParseResult =
        common::RequestSerializer::positionRequestFromBinary(
            clientMsg.header.protocol, clientMsg.content);
    if (!clientReqParseResult) {
      continue;
    }
    auto clientReq = std::move(*clientReqParseResult);

    std::cout << clientReq.imei << " " << clientReq.loc.toStr() << std::endl;

    auto protocolConvertResult =
        common::protocolFromNetworkId(clientMsg.header.protocol);
    if (!protocolConvertResult) {
      continue;
    }

    uint8_t requestTypeBinary =
        static_cast<uint8_t>(common::RequestType::Measurement_Control);

    common::PositionRequest req{clientReq.imei, bs->getLocation()};
    auto serializeResult = common::RequestSerializer::positionRequestToBinary(
        *protocolConvertResult, req);

    common::SocketMessage msg{{clientMsg.header.protocol,
                               static_cast<uint32_t>(serializeResult->size()),
                               requestTypeBinary},
                              *serializeResult};

    auto msgSerializeResult = common::socketMessagetoBinary(msg);
    if (!msgSerializeResult) {
      continue;
    }

    bs->setBuf(*msgSerializeResult);
    ctx->resendToUe(bs->getBuf());
    bs->clearBuf();
  }
}

common::binary_t SimtelBaseStation::getBuf() const { return buf; }

void SimtelBaseStation::setBuf(const common::binary_t &buf_) { buf = buf_; }

void SimtelBaseStation::clearBuf() { buf.clear(); }

common::Location<> SimtelBaseStation::getLocation() const { return location; }
} // namespace server
