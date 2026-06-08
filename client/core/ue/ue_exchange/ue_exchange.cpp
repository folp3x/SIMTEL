#include "ue_exchange.h"

#include "common/core/request/request_serializer/request_serializer.h"
#include "common/core/request/request_type/request_type.h"
#include "common/types.h"

namespace client {
UeExchange::UeExchange(const common::NetworkAddress &serverAddr_)
    : serverAddr(serverAddr_) {}

std::optional<std::string> UeExchange::updateConnection(bool ueActive) {
  if (ueActive) {
    return sock.connectTo(serverAddr);
  } else {
    sock.closeSock();
    return std::nullopt;
  }
}

std::optional<std::string>
UeExchange::sendLocationUpdate(common::Protocol protocol,
                               const common::imei_t &imei,
                               const common::Location<> &loc) const {
  common::PositionRequest req{imei, loc};

  auto serializeResult =
      common::RequestSerializer::positionRequestToBinary(protocol, req);

  auto convertResult = protocolToNetworkId(protocol);
  if (!convertResult) {
    return "Unsupported protocol";
  }

  uint8_t requestTypeBinary =
      static_cast<uint8_t>(common::RequestType::Location_Update);

  return sock.sendMessage(*convertResult, requestTypeBinary, *serializeResult);
}

void UeExchange::closeConnection() { sock.closeSock(); }
} // namespace client
