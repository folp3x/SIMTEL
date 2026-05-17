#include "socket.h"

#include "common/json/json_parser/json_parser.h"
#include "common/network/serializer/serializer.h"
#include "common/types.h"
#include "common/utils/network/network.h"

namespace client {
std::expected<int, std::string> Socket::initSock() {
  auto initResult = common::Socket::initSock();
  if (!initResult) {
    return initResult;
  }

  int inited = *initResult;
  timeval tv = {5, 0}; // таймаут 5 секунд на получение
  if (setsockopt(inited, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    close(inited);
    return std::unexpected(getLastError());
  }

  return inited;
}

void Socket::logSendLocation(const std::string &dataStr) const {
  SPDLOG_LOGGER_DEBUG(common::Logger::instance().getInner(),
                      "Sending location: {}", dataStr);
}

void Socket::logReceiveDistance(const std::string &dataStr) const {
  SPDLOG_LOGGER_DEBUG(common::Logger::instance().getInner(),
                      "Received distance: {}", dataStr);
}

std::optional<std::string>
Socket::connectTo(const common::NetworkAddress &address) {
  auto initResult = initSock();
  if (!initResult) {
    return initResult.error();
  }

  sock = *initResult;

  sockaddr_in serverAddr = common::Socket::toSockAddr(address);
  if (connect(sock, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    return common::Socket::getLastError();
  }

  return std::nullopt;
}

std::optional<std::string>
Socket::sendLocation(common::Protocol protocol,
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

    return sendMessage(protocolId, *serializeResult);
  }
  case common::Protocol::JSON: {
    std::string jsonStr = loc.toJson().dump();
    logSendLocation(jsonStr);
    common::binary_t binary = common::binary_t(jsonStr.begin(), jsonStr.end());

    return sendMessage(protocolId, binary);
  }
  }

  throw std::invalid_argument("Unsupported protocol");
}

std::expected<float, std::string>
Socket::receiveDistance(common::Protocol protocol) const {
  auto receiveResult = receiveMessage();
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
        common::Serializer::fromBinary<float>(receiveResult->content);

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
} // namespace client
