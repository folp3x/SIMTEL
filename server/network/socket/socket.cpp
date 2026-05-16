#include "socket.h"

#include <arpa/inet.h>
#include <stdexcept>

#include "common/network/serializer/serializer.h"

namespace server {
Socket::Socket(int sock_, const sockaddr_in &addr_)
    : common::Socket(sock_), sockAddr(addr_) {}

std::expected<std::unique_ptr<Socket>, std::string>
Socket::create(const common::NetworkAddress &address) {
  auto initResult = initSock();
  if (!initResult) {
    return std::unexpected(initResult.error());
  }

  int sock = *initResult;
  sockaddr_in sockAddr = toSockAddr(address);

  if (bind(sock, reinterpret_cast<sockaddr *>(&sockAddr), sizeof(sockAddr)) <
      0) {
    return std::unexpected(getLastError());
  }

  return std::make_unique<Socket>(sock, sockAddr);
}

std::optional<std::string> Socket::listenForConnections() const {
  if (listen(sock, MAX_WAITING_CONNECTIONS) < 0) {
    return getLastError();
  }
  return std::nullopt;
}

std::expected<std::unique_ptr<Socket>, std::string>
Socket::acceptConnection() const {
  sockaddr_in clientAddr;
  socklen_t addrLen = sizeof(clientAddr);

  int clientSock =
      accept(sock, reinterpret_cast<sockaddr *>(&clientAddr), &addrLen);
  if (clientSock < 0) {
    return std::unexpected(getLastError());
  }

  return std::make_unique<Socket>(clientSock, clientAddr);
}

std::expected<common::Location<>, std::string>
Socket::receiveLocation(common::Protocol &clientProtocol) const {
  auto receiveResult = receiveMessage();
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
    return common::Location<>::fromBinary(receiveResult->content);
  }
  case common::Protocol::JSON: {
    std::string jsonStr = std::string(receiveResult->content.begin(),
                                      receiveResult->content.end());

    auto parseResult = common::Location<>::fromJsonStr(jsonStr);
    if (!parseResult) {
      return std::unexpected(parseResult.error());
    }

    return *parseResult;
  }
  }

  throw std::invalid_argument("Unsupported protocol");
}

std::optional<std::string> Socket::sendDistance(common::Protocol protocol,
                                                float distance) const {
  auto convertResult = protocolToNetworkId(protocol);
  if (!convertResult) {
    throw std::invalid_argument("Unsupported protocol");
  }
  uint8_t protocolId = *convertResult;

  switch (protocol) {
  case common::Protocol::BINARY: {
    auto serializeResult = common::Serializer::toBinary<float>(distance);

    if (!serializeResult) {
      return "Error serializing distance";
    }

    return sendMessage(protocolId, *serializeResult);
  }
  case common::Protocol::JSON: {
    std::string jsonStr = nlohmann::json{{"dist", distance}}.dump();
    common::binary_t binary = common::binary_t(jsonStr.begin(), jsonStr.end());
    return sendMessage(protocolId, binary);
  }
  }

  throw std::invalid_argument("Unsupported protocol");
}

std::string Socket::getAddrStr() const {
  char buffer[INET_ADDRSTRLEN];

  if (inet_ntop(AF_INET, &sockAddr.sin_addr, buffer, INET_ADDRSTRLEN) ==
      nullptr) {
    return "invalid";
  }

  return std::string(buffer) + ":" + std::to_string(ntohs(sockAddr.sin_port));
}
} // namespace server
