#include "ue_exchange.h"

#include "common/core/request/attach_accept_request/attach_accept_request.h"
#include "common/core/request/error_request/error_request.h"
#include "common/core/request/measurement_control_request/measurement_control_request.h"
#include "common/core/request/measurement_report_request/measurement_report_request.h"
#include "common/core/request/rrc_connection_request/rrc_connection_request.h"
#include "common/core/request/rrc_reconfiguration_complete_request/rrc_reconfiguration_complete_request.h"
#include "common/core/request/rrc_reconfiguration_handover_request/rrc_reconfiguration_handover_request.h"
#include "common/core/request/rrc_reconfiguration_keep_request/rrc_reconfiguration_keep_request.h"
#include "common/core/request/sm_delivery_report_request/sm_delivery_report_request.h"
#include "common/core/request/sm_delivery_request/sm_delivery_request.h"

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
UeExchange::receiveResponseData(common::RequestType &type) const {
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

    RequestInfo info = std::move(requests.front());
    requests.pop();

    if (info.req->getType() != common::RequestType::Rrc_Connection &&
        signalLevel == 0) {
      info.callback(nullptr, "Not connected");
      continue;
    }

    CallbackType callback = info.callback;
    curProtocol = info.state.protocol;
    switch (info.req->getType()) {
    case common::RequestType::Rrc_Connection: {
      auto result = handleLocationUpdate(std::move(info));
      if (!result) {
        signalLevel = 0;
        callback(nullptr, result.error());
      } else {
        callback(std::move(*result), "");
      }
      break;
    }
    case common::RequestType::SM_Transfer: {
      auto sendError = sendRequest(std::move(info.req));
      if (sendError) {
        callback(nullptr, "Failed to send sms - " + *sendError);
      }
      break;
    }
    default:
      callback(nullptr, "Unknown request type");
    }

    callback(nullptr, "");
  }
}

void UeExchange::addRequest(const UeState &state,
                            std::unique_ptr<common::Request> req,
                            const CallbackType &callback) {
  {
    std::lock_guard lock(requestsMtx);
    requests.push({state, std::move(req), callback});
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

std::expected<std::unique_ptr<common::Request>, std::string>
UeExchange::handleLocationUpdate(RequestInfo info) {
  sock.setReceiveTimeout(HANDLE_LOCATION_UPDATE_TIMEOUT_MSEC);

  auto locationSendError = sendRequest(std::move(info.req));
  if (locationSendError) {
    return std::unexpected("Failed to send location - " + *locationSendError);
  }

  common::MeasurementControlRequest bestSignalResponse{"", 0, 0};
  bool bsLeft = true;
  while (bsLeft) {
    auto signalResponse = receiveResponse<common::MeasurementControlRequest>();
    if (!signalResponse) {
      bsLeft = false;
      if (bestSignalResponse.getSignal() == 0) {
        signalLevel = 0;
        return std::unexpected("BS not found");
      }
    } else {
      if (signalResponse->getImei() != info.state.imei) {
        continue;
      }

      if (signalResponse->getSignal() > bestSignalResponse.getSignal()) {
        bestSignalResponse = std::move(*signalResponse);
      }
    }
  }

  auto chosenBsReq = std::make_unique<common::MeasurementReportRequest>(
      info.state.imei, info.state.mTimsi, bestSignalResponse.getBsId());
  auto bsIdSendError = sendRequest(std::move(chosenBsReq));
  if (bsIdSendError) {
    return std::unexpected("Failed to send chosen BS id - " + *bsIdSendError);
  }

  common::RequestType responseType;
  auto data = receiveResponseData(responseType);
  if (!data) {
    return std::unexpected(data.error());
  }

  common::imsi_t newMTimsi = info.state.mTimsi;
  unsigned int newBsId;
  std::unique_ptr<common::Request> response;
  switch (responseType) {
  case common::RequestType::Rrc_Reconfiguration_Keep: {
    auto receivedResponse =
        parseFromBytes<common::RrcReconfigurationKeepRequest>(*data);
    if (!receivedResponse) {
      return std::unexpected(receivedResponse.error());
    }
    newBsId = receivedResponse->getBsId();
    response = std::make_unique<common::RrcReconfigurationKeepRequest>(
        *receivedResponse);
    break;
  }
  case common::RequestType::Rrc_Reconfiguration_Handover: {
    auto receivedResponse =
        parseFromBytes<common::RrcReconfigurationHandoverRequest>(*data);
    if (!receivedResponse) {
      return std::unexpected(receivedResponse.error());
    }
    newBsId = receivedResponse->getBsId();
    newMTimsi = receivedResponse->getMTimsi();
    response = std::make_unique<common::RrcReconfigurationHandoverRequest>(
        *receivedResponse);
    break;
  }
  case common::RequestType::Error: {
    auto error = parseFromBytes<common::ErrorRequest>(*data);
    return std::unexpected(error ? error->getDescription() : error.error());
  }
  default:
    return std::unexpected("Unexpected request type");
  }

  if (newBsId != bestSignalResponse.getBsId()) {
    return std::unexpected("Unexpected BS id in info: " +
                           std::to_string(newBsId));
  }

  auto configureCompleteReq =
      std::make_unique<common::RrcReconfigurationCompleteRequest>(newMTimsi);
  auto configureCompleteSendError =
      sendRequest(std::move(configureCompleteReq));
  if (configureCompleteSendError) {
    return std::unexpected("Failed to send configure confirm - " +
                           *configureCompleteSendError);
  }

  auto acceptResponse = receiveResponse<common::AttachAcceptRequest>();
  if (!acceptResponse) {
    return std::unexpected("Failed to receive accept response - " +
                           acceptResponse.error());
  }

  signalLevel = bestSignalResponse.getSignal();
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

void UeExchange::receiveSmsInfo(const CallbackType &callback) {
  while (running) {
    {
      std::lock_guard lock(receiveMtx);
      common::RequestType responseType;

      sock.setReceiveTimeout(RECEIVE_SMS_INFO_TIMEOUT_MSEC);
      auto data = receiveResponseData(responseType);

      if (!data) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(NO_SMS_INFO_SLEEP_MS));
        continue;
      }

      std::unique_ptr<common::Request> response;
      switch (responseType) {
      case common::RequestType::SM_Delivery: {
        auto receivedResponse =
            parseFromBytes<common::SmDeliveryRequest>(*data);
        if (!receivedResponse) {
          callback(nullptr, receivedResponse.error());
          continue;
        }

        response =
            std::make_unique<common::SmDeliveryRequest>(*receivedResponse);
        break;
      }
      case common::RequestType::SM_Delivery_Report: {
        auto receivedResponse =
            parseFromBytes<common::SmDeliveryReportRequest>(*data);
        if (!receivedResponse) {
          callback(nullptr, receivedResponse.error());
          continue;
        }

        response = std::make_unique<common::SmDeliveryReportRequest>(
            *receivedResponse);
        break;
      }
      default:
        callback(nullptr,
                 "Unexpected request type received while receiving SMS info");
        continue;
      }

      callback(std::move(response), "");
    }
  }
}
} // namespace client
