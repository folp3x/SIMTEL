#include "ue_exchange.h"

#include "common/core/request/request_serializer/request_serializer.h"
#include "common/core/request/request_type/request_type.h"
#include "common/network/socket/socket_message/socket_message.h"
#include "common/types.h"

#include "common/utils/network/network.h"
#include <iostream>

namespace client {
UeExchange::UeExchange(const common::NetworkAddress &serverAddr_)
    : serverAddr(serverAddr_) {}

void UeExchange::handleRequests() {
  while (true) {
    RequestInfo info{};
    {
      std::unique_lock lock(requestsMtx);
      requestsCv.wait(lock, [this] { return !requests.empty() && connected; });
      info = std::move(requests.front());
      requests.pop();
    }

    if (info.type == common::RequestType::Location_Update) {
      if (auto *posReq =
              dynamic_cast<common::PositionRequest *>(info.req.get())) {
        sendLocationUpdate(info.protocol, *posReq);
      }
    }

    info.callback(std::nullopt, "");
  }
}

void UeExchange::addRequest(common::Protocol protocol, common::RequestType type,
                            std::unique_ptr<common::Request> req,
                            const CallbackType &callback) {
  {
    std::lock_guard lock(requestsMtx);
    requests.push({protocol, type, std::move(req), callback});
  }
  requestsCv.notify_one();
}

std::optional<std::string> UeExchange::updateConnection(bool ueActive) {
  if (ueActive) {
    auto error = sock.connectTo(serverAddr);
    if (!error) {
      connected = true;
    }
    return error;
  } else {
    closeConnection();
    return std::nullopt;
  }
}

std::optional<std::string>
UeExchange::sendLocationUpdate(common::Protocol protocol,
                               const common::PositionRequest &req) const {
  auto serializeResult =
      common::RequestSerializer::positionRequestToBinary(protocol, req);

  auto convertResult = protocolToNetworkId(protocol);
  if (!convertResult) {
    return "Unsupported protocol";
  }

  uint8_t requestTypeBinary =
      static_cast<uint8_t>(common::RequestType::Location_Update);
  common::SocketMessage msg{{static_cast<uint32_t>(serializeResult->size()),
                             *convertResult, requestTypeBinary},
                            *serializeResult};

  auto msgSerializeResult = common::socketMessagetoBinary(msg);
  if (!msgSerializeResult) {
    return msgSerializeResult.error();
  }

  std::cout << "Sent binary: " << common::toStr(*msgSerializeResult)
            << std::endl;

  return sock.sendMessage(*msgSerializeResult);
}

void UeExchange::closeConnection() {
  sock.closeSock();
  connected = false;
}

unsigned int UeExchange::getSignalLevel() const { return signalLevel; }

void UeExchange::setSignalLevel(unsigned int signalLevel_) {
  signalLevel = signalLevel_;
}
} // namespace client
