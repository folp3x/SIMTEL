#include "ue_exchange.h"

#include "common/core/request/request_serializer/request_serializer.h"
#include "common/network/socket/socket_message/socket_message.h"

namespace client {
std::optional<std::string>
UeExchange::sendRequest(std::unique_ptr<common::Request> req) {
  auto bytes = req->toBytes(curProtocol);
  if (!bytes) {
    return bytes.error();
  }

  auto sendError = sock.sendMessage(*bytes);
  if (!sendError) {
    return std::nullopt;
  }

  return sendError->description;
}

std::expected<common::binary_t, std::string>
UeExchange::receiveRequestData(common::RequestType &type) const {
  auto bytes = sock.receiveMessage();
  if (!bytes) {
    return std::unexpected(bytes.error().description);
  }

  auto reqType = common::parseRequestType(*bytes);
  if (!reqType) {
    return std::unexpected(reqType.error());
  }
  type = *reqType;

  return *bytes;
}

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
        signalLevel = 0;
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

std::expected<common::MeasurementControlRequest, std::string>
UeExchange::receiveSignalLevel() const {
  auto bytes = sock.receiveMessage();
  if (!bytes) {
    return std::unexpected(bytes.error().description);
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

  auto sendError = sock.sendMessage(*bytes);
  if (!sendError) {
    return std::nullopt;
  }
  return sendError->description;
}

std::expected<std::unique_ptr<common::Request>, std::string>
UeExchange::receiveBsInfo() const {
  common::RequestType type;
  auto data = receiveRequestData(type);
  if (!data) {
    return std::unexpected(data.error());
  }

  common::Protocol protocol;
  switch (type) {
  case common::RequestType::Rrc_Reconfiguration_Keep: {
    auto req = common::RequestSerializer::rrcReconfigurationKeepFromBytes(
        *data, protocol);
    if (!req) {
      return std::unexpected(req.error());
    } else if (protocol != curProtocol) {
      return std::unexpected("Unknown protocol");
    }

    return std::make_unique<common::RrcReconfigurationKeepRequest>(*req);
  }
  case common::RequestType::Rrc_Reconfiguration_Handover: {
    auto req = common::RequestSerializer::rrcReconfigurationHandoverFromBytes(
        *data, protocol);
    if (!req) {
      return std::unexpected(req.error());
    } else if (protocol != curProtocol) {
      return std::unexpected("Unknown protocol");
    }

    return std::make_unique<common::RrcReconfigurationHandoverRequest>(*req);
  }
  case common::RequestType::Error: {
    auto req = common::RequestSerializer::errorFromBytes(*data, protocol);
    if (!req) {
      return std::unexpected(req.error());
    } else if (protocol != curProtocol) {
      return std::unexpected("Unknown protocol");
    }
    return std::make_unique<common::ErrorRequest>(*req);
  }
  default:
    return std::unexpected("Unexpected request type");
  }
}

std::expected<common::AttachAcceptRequest, std::string>
UeExchange::receiveAttachAccept() const {
  auto bytes = sock.receiveMessage();
  if (!bytes) {
    return std::unexpected(bytes.error().description);
  }

  common::Protocol protocol;
  auto req = common::RequestSerializer::attachAcceptFromBytes(*bytes, protocol);
  if (!req) {
    return std::unexpected(req.error());
  }
  if (protocol != curProtocol) {
    return std::unexpected("Invalid protocol");
  }

  return *req;
}

std::optional<std::string> UeExchange::sendBsAccept(
    const common::RrcReconfigurationCompleteRequest &req) const {
  auto bytes = common::RequestSerializer::rrcReconfigurationCompleteToBytes(
      curProtocol, req);
  if (!bytes) {
    return bytes.error();
  }

  auto sendError = sock.sendMessage(*bytes);
  if (!sendError) {
    return std::nullopt;
  }
  return sendError->description;
}

std::expected<std::unique_ptr<common::Request>, std::string>
UeExchange::handleLocationUpdate(const RequestInfo &info) {
  auto locationReq = std::make_unique<common::RrcConnectionRequest>(
      info.state.imei, info.state.location);
  auto locationSendError = sendRequest(std::move(locationReq));
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
  if (auto *resp = dynamic_cast<common::RrcReconfigurationKeepRequest *>(
          response.get())) {
    newBsId = resp->bsId;
  } else if (auto *resp =
                 dynamic_cast<common::RrcReconfigurationHandoverRequest *>(
                     response.get())) {
    newBsId = resp->bsId;
  } else if (auto *errorResponse =
                 dynamic_cast<common::ErrorRequest *>(response.get())) {
    return std::unexpected("Error response: " + errorResponse->description);
  } else {
    return std::unexpected("Invalid response received");
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
