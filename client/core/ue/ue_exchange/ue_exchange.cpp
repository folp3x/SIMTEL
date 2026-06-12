#include "ue_exchange.h"

#include "common/core/request/request_serializer/request_serializer.h"
#include "common/network/socket/socket_message/socket_message.h"

namespace client {
UeExchange::UeExchange(const common::NetworkAddress &serverAddr_)
    : serverAddr(serverAddr_) {}

void UeExchange::handleRequests() {
  while (running) {
    std::unique_lock lock(requestsMtx);
    requestsCv.wait(
        lock, [this] { return !requests.empty() && connected || !running; });

    if (!running) {
      break;
    }

    RequestInfo info = requests.front();
    requests.pop();
    lock.unlock();

    if (info.type != common::RequestType::Rrc_Connection && signalLevel == 0) {
      info.callback(nullptr, "Not connected");
      continue;
    }

    curProtocol = info.state.protocol;

    switch (info.type) {
    case common::RequestType::Rrc_Connection: {
      auto result = handleLocationUpdate(info);
      if (!result) {
        info.callback(nullptr, result.error());
      } else {
        info.callback(std::move(*result), "");
      }
      break;
    }
    default:
      info.callback(nullptr, "Unknown request type");
    }
  }
}

void UeExchange::addRequest(const UeState &state, common::RequestType type,
                            const CallbackType &callback) {
  {
    std::lock_guard lock(requestsMtx);
    requests.push({state, type, callback});
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

  common::Protocol protocol;
  auto req =
      common::RequestSerializer::measurementControlFromBytes(*bytes, protocol);
  if (!req) {
    return std::unexpected(req.error());
  }
  if (protocol != curProtocol) {
    return std::unexpected("Invalid protocol");
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

  common::Protocol protocol;
  auto reqType = common::RequestSerializer::parseRequestType(*bytes, protocol);
  if (!reqType) {
    return std::unexpected(reqType.error());
  }
  if (protocol != curProtocol) {
    return std::unexpected("Invalid protocol");
  }

  switch (*reqType) {
  case common::RequestType::Rrc_Reconfiguration_Keep: {
    auto req = common::RequestSerializer::rrcReconfigurationKeepFromBytes(
        *bytes, protocol);
    if (!req) {
      return std::unexpected(req.error());
    }

    return std::make_unique<common::RrcReconfigurationKeepRequest>(*req);
  }
  case common::RequestType::Rrc_Reconfiguration_Handover: {
    auto req = common::RequestSerializer::rrcReconfigurationHandoverFromBytes(
        *bytes, protocol);
    if (!req) {
      return std::unexpected(req.error());
    }
    return std::make_unique<common::RrcReconfigurationHandoverRequest>(*req);
  }
  default:
    return std::unexpected("Unexpected request type");
  }
}

std::expected<std::unique_ptr<common::Request>, std::string>
UeExchange::handleLocationUpdate(const RequestInfo &info) {
  common::RrcConnectionRequest locationReq{info.state.imei,
                                           info.state.location};
  auto locationSendError = sendLocationUpdate(locationReq);
  if (locationSendError) {
    return std::unexpected("Failed to send location - " + *locationSendError);
  }

  common::MeasurementControlRequest bestSignalResponse{"", 0, 0};
  bool bsLeft = true;
  while (bsLeft) {
    auto signalResponse = receiveSignalLevel();
    if (!signalResponse) {
      bsLeft = false;
      if (bestSignalResponse.signal == 0) {
        signalLevel = 0;
        return std::unexpected("BS not found");
      }
    } else {
      if (signalResponse->imei != info.state.imei) {
        continue;
      }

      if (signalResponse->signal > bestSignalResponse.signal) {
        bestSignalResponse = std::move(*signalResponse);
      }
    }
  }

  common::MeasurementReportRequest chosenBsReq{
      info.state.imei, info.state.mTimsi, bestSignalResponse.bsId};
  auto bsIdSendError = sendChosenBsId(chosenBsReq);
  if (bsIdSendError) {
    return std::unexpected("Failed to send chosen BS id - " + *bsIdSendError);
  }

  auto bsInfoResponse = receiveBsInfo();
  if (!bsInfoResponse) {
    return std::unexpected("Failed to receive BS info - " +
                           bsInfoResponse.error());
  }
  auto response = std::move(*bsInfoResponse);

  unsigned int newBsId;
  if (auto *bsKeepResponse =
          dynamic_cast<common::RrcReconfigurationKeepRequest *>(
              response.get())) {
    newBsId = bsKeepResponse->bsId;
  } else if (auto *bsHandoverResponse =
                 dynamic_cast<common::RrcReconfigurationHandoverRequest *>(
                     response.get())) {
    newBsId = bsHandoverResponse->bsId;
  } else {
    return std::unexpected("BS rejected connection");
  }

  if (newBsId == bestSignalResponse.bsId) {
    signalLevel = bestSignalResponse.signal;
  } else {
    return std::unexpected("Unexpected BS id in info: " +
                           std::to_string(newBsId));
  }

  return response;
}

void UeExchange::closeConnection() {
  sock.closeSock();
  connected = false;
}

unsigned int UeExchange::getSignalLevel() const { return signalLevel; }

bool UeExchange::hasSignal() const { return signalLevel != 0; }

void UeExchange::stop() {
  running = false;
  requestsCv.notify_all();
  closeConnection();
}
} // namespace client
