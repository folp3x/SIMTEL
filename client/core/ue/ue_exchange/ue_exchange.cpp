#include "ue_exchange.h"

#include "common/json/json_parser/json_parser.h"
#include "common/network/binary_serializer/binary_serializer.h"
#include "common/types.h"
#include "common/utils/network/network.h"

namespace client {
void UeExchange::logSendLocation(const std::string &dataStr) const {
  SPDLOG_LOGGER_DEBUG(common::Logger::instance().getInner(),
                      "Sending location: {}", dataStr);
}

void UeExchange::logReceiveDistance(const std::string &dataStr) const {
  SPDLOG_LOGGER_DEBUG(common::Logger::instance().getInner(),
                      "Received distance: {}", dataStr);
}

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
UeExchange::sendLocation(common::Protocol protocol,
                         const common::Location<> &loc) const {
  auto convertResult = protocolToNetworkId(protocol);
  if (!convertResult) {
    throw std::invalid_argument("Unsupported protocol");
  }
  uint8_t protocolId = *convertResult;

  switch (protocol) {
  case common::Protocol::BINARY: {
    auto serializeResult = loc.toBinary();
    if (!serializeResult) {
      return serializeResult.error();
    }
    logSendLocation(common::toStr(*serializeResult));

    return sock.sendMessage(protocolId, *serializeResult);
  }
  case common::Protocol::JSON: {
    std::string jsonStr = loc.toJson().dump();
    logSendLocation(jsonStr);
    common::binary_t binary = common::binary_t(jsonStr.begin(), jsonStr.end());

    return sock.sendMessage(protocolId, binary);
  }
  }

  throw std::invalid_argument("Unsupported protocol");
}

std::expected<float, std::string>
UeExchange::receiveDistance(common::Protocol protocol) const {
  auto receiveResult = sock.receiveMessage();
  if (!receiveResult) {
    return std::unexpected(receiveResult.error());
  }

  auto convertResult = protocolToNetworkId(protocol);
  if (!convertResult) {
    throw std::invalid_argument("Unsupported protocol");
  }
  uint8_t protocolId = *convertResult;

  if (receiveResult->protocol != protocolId) {
    return std::unexpected("Invalid message protocol. ProtocolId=" +
                           std::to_string(protocolId) + " expected");
  }

  switch (protocol) {
  case common::Protocol::BINARY: {
    logReceiveDistance(common::toStr(receiveResult->content));
    auto deserializeResult =
        common::BinarySerializer::fromBinary<float>(receiveResult->content);

    if (!deserializeResult) {
      return std::unexpected("Failed to deserialize distance");
    }

    return *deserializeResult;
  }
  case common::Protocol::JSON: {
    float dist;
    auto distInfo = std::make_unique<common::JsonFieldInfo<float>>(
        "dist", [&](float dist_) { dist = dist_; },
        nlohmann::json::value_t::number_float);

    std::string jsonStr = std::string(receiveResult->content.begin(),
                                      receiveResult->content.end());
    logReceiveDistance(jsonStr);

    auto error =
        common::JsonParser<float>::parseField(std::move(distInfo), jsonStr);
    if (error) {
      return std::unexpected(*error);
    }

    return dist;
  }
  }

  throw std::invalid_argument("Unsupported protocol");
}

void UeExchange::closeConnection() { sock.closeSock(); }
} // namespace client
