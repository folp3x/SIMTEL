#include "simtel_ue_context.h"

#include "common/network/binary_serializer/binary_serializer.h"
#include "common/utils/network/network.h"
#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
void SimtelUeContext::logReceiveLocation(const std::string &dataStr) const {
  SPDLOG_LOGGER_DEBUG(common::Logger::instance().getInner(),
                      "Received location: {}", dataStr);
}

void SimtelUeContext::logSendDistance(const std::string &dataStr) const {
  SPDLOG_LOGGER_DEBUG(common::Logger::instance().getInner(),
                      "Sending distance: {}", dataStr);
}

SimtelUeContext::SimtelUeContext(std::unique_ptr<Socket> sock_)
    : sock(std::move(sock_)) {}

void SimtelUeContext::setImsi(const common::imsi_t &imsi_) { imsi = imsi_; }

Socket *SimtelUeContext::getSock() const { return sock.get(); }

void SimtelUeContext::setProtocol(common::Protocol protocol_) {
  protocol = protocol_;
}

std::expected<common::Location<>, std::string>
SimtelUeContext::receiveLocation(common::Protocol &clientProtocol) const {
  auto receiveResult = sock->receiveMessage();
  if (!receiveResult) {
    return std::unexpected(receiveResult.error());
  }

  auto protocolSearchResult =
      common::protocolFromNetworkId(receiveResult->protocol);
  if (!protocolSearchResult) {
    throw std::invalid_argument("Unsupported protocol");
  }

  clientProtocol = *protocolSearchResult;
  switch (clientProtocol) {
  case common::Protocol::BINARY: {
    logReceiveLocation(common::toStr(receiveResult->content));
    return common::Location<>::fromBinary(receiveResult->content);
  }
  case common::Protocol::JSON: {
    std::string jsonStr = std::string(receiveResult->content.begin(),
                                      receiveResult->content.end());
    logReceiveLocation(jsonStr);

    auto parseResult = common::Location<>::fromJsonStr(jsonStr);
    if (!parseResult) {
      return std::unexpected(parseResult.error());
    }

    return *parseResult;
  }
  }

  throw std::invalid_argument("Unsupported protocol");
}

std::optional<std::string>
SimtelUeContext::sendDistance(common::Protocol protocol, float distance) const {
  auto convertResult = protocolToNetworkId(protocol);
  if (!convertResult) {
    throw std::invalid_argument("Unsupported protocol");
  }
  uint8_t protocolId = *convertResult;

  switch (protocol) {
  case common::Protocol::BINARY: {
    auto serializeResult = common::BinarySerializer::toBinary<float>(distance);
    if (!serializeResult) {
      return "Failed to serializeq distance";
    }
    logSendDistance(common::toStr(*serializeResult));

    return sock->sendMessage(protocolId, *serializeResult);
  }
  case common::Protocol::JSON: {
    std::string jsonStr = nlohmann::json{{"dist", distance}}.dump();
    logSendDistance(jsonStr);
    common::binary_t binary = common::binary_t(jsonStr.begin(), jsonStr.end());

    return sock->sendMessage(protocolId, binary);
  }
  }

  throw std::invalid_argument("Unsupported protocol");
}
} // namespace server
