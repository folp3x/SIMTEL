#include "simtel_base_station.h"

#include "common/core/request/request_serializer/request_serializer.h"
#include "common/core/request/request_type/request_type.h"
#include "common/network/socket/socket_message/socket_message.h"
#include "server/core/simtel/simtel_ue_context/simtel_ue_context.h"

namespace server {
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
    auto msg = std::move(*msgParseResult);

    auto clientReqParseResult =
        common::RequestSerializer::positionRequestFromBinary(
            msg.header.protocol, msg.content);
    if (!clientReqParseResult) {
      continue;
    }
    auto clientReq = std::move(*clientReqParseResult);

    auto protocolConvertResult =
        common::protocolFromNetworkId(msg.header.protocol);
    if (!protocolConvertResult) {
      continue;
    }

    common::PositionRequest req{clientReq.imei, bs->getLocation()};
    auto serializeResult = common::RequestSerializer::positionRequestToBinary(
        *protocolConvertResult, req);
  }
}

common::binary_t SimtelBaseStation::getBuf() const { return buf; }

void SimtelBaseStation::setBuf(const common::binary_t &buf_) { buf = buf_; }

common::Location<> SimtelBaseStation::getLocation() const { return location; }
} // namespace server
