#include "ue_exchange.h"

#include "common/core/request/request_serializer/request_serializer.h"
#include "common/core/request/request_type/request_type.h"
#include "common/network/socket/socket_message/socket_message.h"
#include "common/types.h"

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
        auto error = sendLocationUpdate(info.protocol, *posReq);
        if (error) {
          info.callback(std::nullopt, *error);
          continue;
        }

        unsigned int bestSignal = 0;
        unsigned int bestBsId = 0;
        bool bsLeft = true;
        while (bsLeft) {
          auto req = receiveSignalLevel();
          if (!req) {
            bsLeft = false;
            if (bestSignal == 0) {
              info.callback(std::nullopt, "BS search error: " + req.error());
            }
          } else {
            if (req->imei != posReq->imei) {
              continue;
            }

            if (req->signal > bestSignal) {
              bestSignal = req->signal;
              bestBsId = req->bsId;
            }
          }
        }

        {
          std::lock_guard lock(signalLevelMtx);
          signalLevel = bestSignal;
        }

        info.callback(std::nullopt, "");
      }
    } else {
      info.callback(std::nullopt, "Unknown request type");
    }
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
  auto serializedReq =
      common::RequestSerializer::positionRequestToBinary(protocol, req);
  if (!serializedReq) {
    return "Error serizliaing request: " + serializedReq.error();
  }

  auto protocolId = protocolToNetworkId(protocol);
  if (!protocolId) {
    return "Unsupported protocol";
  }

  uint8_t requestTypeBinary =
      static_cast<uint8_t>(common::RequestType::Location_Update);
  common::SocketMessage msg{{static_cast<uint32_t>(serializedReq->size()),
                             *protocolId, requestTypeBinary},
                            *serializedReq};

  auto serializedMsg = common::socketMessagetoBinary(msg);
  if (!serializedMsg) {
    return "Error serializing message: " + serializedMsg.error();
  }

  return sock.sendMessage(*serializedMsg);
}

std::expected<common::SignalRequest, std::string>
UeExchange::receiveSignalLevel() const {
  auto binary = sock.receiveMessage();
  if (!binary) {
    return std::unexpected(binary.error());
  }

  auto msg = common::socketMessageFromBinary(*binary);
  if (!msg) {
    return std::unexpected("Error parsing message: " + msg.error());
  }

  auto protocol = common::protocolFromNetworkId(msg->header.protocol);
  if (!protocol) {
    return std::unexpected("Unknown protocol");
  }

  auto reqType = static_cast<common::RequestType>(msg->header.msgType);
  if (reqType != common::RequestType::Measurement_Control) {
    return std::unexpected("Location_Update message expected");
  }

  auto req = common::RequestSerializer::signalRequestFromBinary(
      msg->header.protocol, msg->content);
  if (!req) {
    return std::unexpected("Error parsing request: " + req.error());
  }

  return *req;
}

void UeExchange::closeConnection() {
  sock.closeSock();
  connected = false;
}

unsigned int UeExchange::getSignalLevel() const {
  std::lock_guard lock(signalLevelMtx);
  return signalLevel;
}
} // namespace client
