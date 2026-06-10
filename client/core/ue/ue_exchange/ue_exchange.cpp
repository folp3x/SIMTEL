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

    curProtocol = info.ctx->getProtocol();

    switch (info.type) {
    case common::RequestType::Rrc_Connection: {
      auto error = handleLocationUpdate(info);
      if (error) {
        info.callback(nullptr, *error);
      }
      break;
    }
    default:
      info.callback(nullptr, "Unknown request type");
    }
  }
}

void UeExchange::addRequest(std::shared_ptr<const UeContext> ctx,
                            common::RequestType type,
                            const CallbackType &callback) {
  {
    std::lock_guard lock(requestsMtx);
    requests.push({ctx, type, callback});
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
UeExchange::receiveBsInfo() const {
  auto bytes = sock.receiveMessage();
  if (!bytes) {
    return std::unexpected(bytes.error());
  }
  auto msg = common::socketMessageFromBinary(*bytes);
  if (!msg) {
    return std::unexpected(msg.error());
  }

  auto reqType = static_cast<common::RequestType>(msg->header.msgType);
  if (reqType == common::RequestType::Rrc_Reconfiguration_Keep) {
    auto req =
        common::RequestSerializer::rrcReconfigurationKeepFromBytes(*bytes);
    if (!req) {
      return std::unexpected(req.error());
    }

    return std::make_unique<common::RrcReconfigurationKeepRequest>(*req);
  }

  return std::unexpected("Unexpected request type");
}

std::optional<std::string>
UeExchange::handleLocationUpdate(const RequestInfo &info) {
  common::RrcConnectionRequest locationReq{info.ctx->getImei(),
                                           info.ctx->getLocation()};
  auto locationSendError = sendLocationUpdate(locationReq);
  if (locationSendError) {
    return "Failed to send location - " + *locationSendError;
  }

  common::MeasurementControlRequest bestSignalResponse{"", 0, 0};
  bool bsLeft = true;
  while (bsLeft) {
    auto signalResponse = receiveSignalLevel();
    if (!signalResponse) {
      bsLeft = false;
      if (bestSignalResponse.signal == 0) {
        return "BS not found";
      }
    } else {
      if (signalResponse->imei != info.ctx->getImei()) {
        continue;
      }

      if (signalResponse->signal > bestSignalResponse.signal) {
        bestSignalResponse = std::move(*signalResponse);
      }
    }
  }

  common::MeasurementReportRequest chosenBsReq{
      info.ctx->getImei(), info.ctx->getImsi(), bestSignalResponse.bsId};
  auto bsIdSendError = sendChosenBsId(chosenBsReq);
  if (bsIdSendError) {
    return "Failed to send chosen BS id - " + *bsIdSendError;
  }

  auto bsInfoResponse = receiveBsInfo();
  if (!bsInfoResponse) {
    return "Failed to receive BS info - " + bsInfoResponse.error();
  }
  auto response = std::move(*bsInfoResponse);

  if (auto *bsKeepResponse =
          dynamic_cast<common::RrcReconfigurationKeepRequest *>(
              response.get())) {
    if (bsKeepResponse->bsId == bestSignalResponse.bsId) {
      signalLevel = bestSignalResponse.signal;
      curBsId = bsKeepResponse->bsId;
    } else {
      return "Unexpected BS id in info: " +
             std::to_string(bsKeepResponse->bsId);
    }
  } else {
    return "Unexpected response type";
  }

  return std::nullopt;
}

void UeExchange::closeConnection() {
  sock.closeSock();
  connected = false;
}

unsigned int UeExchange::getSignalLevel() const { return signalLevel; }

void UeExchange::stop() {
  running = false;
  requestsCv.notify_all();
  closeConnection();
}
} // namespace client
