#include "ue_exchange.h"

#include "common/core/request/request_serializer/request_serializer.h"
#include "common/network/socket/socket_message/socket_message.h"

namespace client {
UeExchange::UeExchange(const common::NetworkAddress &serverAddr_)
    : serverAddr(serverAddr_) {}

void UeExchange::handleRequests() {
  while (running) {
    RequestInfo info{};
    {
      std::unique_lock lock(requestsMtx);
      requestsCv.wait(
          lock, [this] { return !requests.empty() && connected || !running; });

      if (!running) {
        break;
      }

      info = std::move(requests.front());
      requests.pop();
    }

    curProtocol = info.protocol;

    std::string error;
    if (info.type == common::RequestType::Rrc_Connection) {
      if (auto *req =
              dynamic_cast<common::RrcConnectionRequest *>(info.req.get())) {
        auto result = handleLocationUpdate(*req);
        if (!result) {
          info.callback(nullptr, result.error());
        }
      } else {
        error = "Invalid request type";
      }
    } else {
      error = "Unknown request type";
    }

    info.callback(nullptr, error);
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
UeExchange::sendLocationUpdate(const common::RrcConnectionRequest &req) const {
  auto bytes =
      common::RequestSerializer::rrcConnectionToBytes(curProtocol, req);
  if (!bytes) {
    return bytes.error();
  }
  return sock.sendMessage(*bytes);
}

std::expected<common::MeasurementControlRequest, std::string>
UeExchange::receiveSignalLevel() const {
  auto bytes = sock.receiveMessage();
  if (!bytes) {
    return std::unexpected(bytes.error());
  }

  auto req = common::RequestSerializer::measurementControlFromBytes(*bytes);
  if (!req) {
    return std::unexpected(req.error());
  }

  return *req;
}

std::optional<std::string>
UeExchange::sendChosenBsId(const common::MeasurementReportRequest &req) const {
  auto bytes =
      common::RequestSerializer::measurementReportToBytes(curProtocol, req);
  if (!bytes) {
    return bytes.error();
  }
  return sock.sendMessage(*bytes);
}

std::expected<std::unique_ptr<common::Request>, std::string>
UeExchange::handleLocationUpdate(const common::RrcConnectionRequest &req) {
  auto error = sendLocationUpdate(req);
  if (error) {
    return std::unexpected(*error);
  }

  unsigned int bestSignal = 0;
  unsigned int bestBsId = 0;

  bool bsLeft = true;
  while (bsLeft) {
    auto signalResponse = receiveSignalLevel();
    if (!signalResponse) {
      bsLeft = false;
      if (bestSignal == 0) {
        return std::unexpected("BS not found");
      }
    } else {
      if (signalResponse->imei != req.imei) {
        continue;
      }

      if (signalResponse->signal > bestSignal) {
        bestSignal = signalResponse->signal;
        bestBsId = signalResponse->bsId;
      }
    }
  }

  {
    std::lock_guard lock(signalLevelMtx);
    signalLevel = bestSignal;
  }

  return nullptr;
}

void UeExchange::closeConnection() {
  sock.closeSock();
  connected = false;
}

unsigned int UeExchange::getSignalLevel() const {
  std::lock_guard lock(signalLevelMtx);
  return signalLevel;
}

void UeExchange::stop() {
  running = false;
  requestsCv.notify_all();
  closeConnection();
}
} // namespace client
