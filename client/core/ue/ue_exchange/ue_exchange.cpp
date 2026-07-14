#include "ue_exchange.h"

#include "common/core/request/measurement_report_request/measurement_report_request.h"
#include "common/core/request/rrc_connection_request/rrc_connection_request.h"
#include "common/core/request/rrc_reconfiguration_complete_request/rrc_reconfiguration_complete_request.h"

#include "common/core/response/attach_accept_response/attach_accept_response.h"
#include "common/core/response/error_response/error_response.h"
#include "common/core/response/measurement_control_response/measurement_control_response.h"

#include "common/core/response/rrc_reconfiguration_handover_response/rrc_reconfiguration_handover_response.h"
#include "common/core/response/rrc_reconfiguration_keep_response/rrc_reconfiguration_keep_response.h"

#include "common/core/response/sm_delivery_error_response/sm_delivery_error_response.h"
#include "common/core/response/sm_delivery_report_response/sm_delivery_report_response.h"
#include "common/core/response/sm_delivery_response/sm_delivery_response.h"

#include "common/core/response/ussd_balance_response/ussd_balance_response.h"
#include "common/core/response/ussd_msisdn_response/ussd_msisdn_response.h"

namespace client {
std::optional<std::string>
UeExchange::sendRequest(std::unique_ptr<common::Request> req) {
  auto bytes = req->toBytes(curProtocol);
  if (!bytes) {
    return bytes.error();
  }

  auto error = sock.sendMessage(*bytes);
  if (!error) {
    return std::nullopt;
  }

  return error->description;
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

void UeExchange::sendRequests() {
  while (running) {
    std::unique_lock lock(requestsMtx);
    requestsCv.wait(
        lock, [this] { return !requests.empty() && connected || !running; });

    if (!running) {
      lock.unlock();
      break;
    }

    RequestInfo info = std::move(requests.front());
    requests.pop();
    lock.unlock();

    if (info.req->getType() != common::RequestType::RrcConnection &&
        signalLevel == 0) {
      info.callback(nullptr, "Not connected");
      continue;
    }

    callback_t callback = info.callback;
    curProtocol = info.state.protocol;
    switch (info.req->getType()) {
    case common::RequestType::RrcConnection: {
      std::lock_guard lock(receiveMtx);
      auto response = handleLocationUpdate(std::move(info));
      if (!response) {
        signalLevel = 0;
        callback(nullptr, response.error());
      } else {
        callback(std::move(*response), "");
      }

      break;
    }
    case common::RequestType::SmTransfer: {
      auto sendError = sendRequest(std::move(info.req));
      if (sendError) {
        callback(nullptr, "Failed to send sms - " + *sendError);
      } else {
        callback(nullptr, "");
      }
      break;
    }
    case common::RequestType::SmDeliveryAck: {
      auto sendError = sendRequest(std::move(info.req));
      if (sendError) {
        callback(nullptr, "Failed to send sms deelivery ack - " + *sendError);
      }
      break;
    }
    default:
      callback(nullptr, "Unknown request type");
    }
  }
}

void UeExchange::addRequest(const UeState &state,
                            std::unique_ptr<common::Request> req,
                            const callback_t &callback) {
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
    signalLevel = 0;
    closeConnection();
    return std::nullopt;
  }
}

std::expected<std::unique_ptr<common::Request>, std::string>
UeExchange::handleLocationUpdate(RequestInfo info) {
  auto locationSendError = sendRequest(std::move(info.req));
  if (locationSendError) {
    return std::unexpected("Failed to send location - " + *locationSendError);
  }

  bool set = sock.setReceiveTimeout(ReceiveSignalTimeoutMsec);
  if (!set) {
    return std::unexpected("Error setting receive timeout");
  }

  common::MeasurementControlResponse bestSignalResponse{"", 0, 0};
  bool bsLeft = true;
  while (bsLeft) {
    auto signalResponse = receiveResponse<common::MeasurementControlResponse>();
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
  sock.removeReceiveTimeout();

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
  case common::RequestType::RrcReconfigurationKeep: {
    auto receivedResponse =
        parseFromBytes<common::RrcReconfigurationKeepResponse>(*data);
    if (!receivedResponse) {
      return std::unexpected(receivedResponse.error());
    }
    newBsId = receivedResponse->getBsId();
    response = std::make_unique<common::RrcReconfigurationKeepResponse>(
        *receivedResponse);
    break;
  }
  case common::RequestType::RrcReconfigurationHandover: {
    auto receivedResponse =
        parseFromBytes<common::RrcReconfigurationHandoverResponse>(*data);
    if (!receivedResponse) {
      return std::unexpected(receivedResponse.error());
    }
    newBsId = receivedResponse->getBsId();
    newMTimsi = receivedResponse->getMTimsi();
    response = std::make_unique<common::RrcReconfigurationHandoverResponse>(
        *receivedResponse);
    break;
  }
  case common::RequestType::Error: {
    auto error = parseFromBytes<common::ErrorResponse>(*data);
    return std::unexpected(error ? error->getDescription() : error.error());
  }
  default:
    return std::unexpected(
        "Unexpected request type - Keep or Handover expected");
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

  auto acceptResponse = receiveResponse<common::AttachAcceptResponse>();
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

void UeExchange::receiveFromBsInBackground(const callback_t &callback) {
  while (running) {
    {
      std::this_thread::sleep_for(ReceiveFromBsSleepTime);

      std::unique_lock lock(receiveMtx);
      bool set = sock.setReceiveTimeout(ReceiveFromBsTimeoutMsec);
      if (!set) {
        lock.unlock();
        continue;
      }

      common::RequestType responseType;
      auto data = receiveResponseData(responseType);
      lock.unlock();
      if (!data) {
        continue;
      }

      std::unique_ptr<common::Request> response;
      switch (responseType) {
      case common::RequestType::SmDelivery: {
        auto receivedResponse =
            parseFromBytes<common::SmDeliveryResponse>(*data);
        if (!receivedResponse) {
          callback(nullptr, receivedResponse.error());
          continue;
        }

        response =
            std::make_unique<common::SmDeliveryResponse>(*receivedResponse);
        break;
      }
      case common::RequestType::SmDeliveryReport: {
        auto receivedResponse =
            parseFromBytes<common::SmDeliveryReportResponse>(*data);
        if (!receivedResponse) {
          callback(nullptr, receivedResponse.error());
          continue;
        }

        response = std::make_unique<common::SmDeliveryReportResponse>(
            *receivedResponse);
        break;
      }
      case common::RequestType::SmDeliveryError: {
        auto receivedResponse =
            parseFromBytes<common::SmDeliveryErrorResponse>(*data);
        if (!receivedResponse) {
          callback(nullptr, receivedResponse.error());
          continue;
        }

        response = std::make_unique<common::SmDeliveryErrorResponse>(
            *receivedResponse);
        break;
      }
      case common::RequestType::Error: {
        auto receivedResponse = parseFromBytes<common::ErrorResponse>(*data);
        if (!receivedResponse) {
          callback(nullptr, receivedResponse.error());
        } else {
          callback(nullptr, receivedResponse->getDescription());
        }
        continue;
      }
      default:
        callback(nullptr, "Unexpected response received from BS: " +
                              common::requestTypeToStr(responseType));
        continue;
      }

      callback(std::move(response), "");
    }
  }
}

std::expected<std::unique_ptr<common::Request>, std::string>
UeExchange::sendUssd(const UeState &state,
                     std::unique_ptr<common::UssdCodeRequest> req) {
  curProtocol = state.protocol;

  std::unique_lock lock(receiveMtx);

  auto sendError = sendRequest(std::move(req));
  if (sendError) {
    return std::unexpected("Failed to send ussd - " + *sendError);
  }

  bool set = sock.setReceiveTimeout(ReceiveUssdTimeoutMsec);
  if (!set) {
    return std::unexpected("Error setting receive timeout");
  }

  common::RequestType responseType;
  auto data = receiveResponseData(responseType);
  lock.unlock();
  if (!data) {
    return std::unexpected("Unable to process command");
  }

  switch (responseType) {
  case common::RequestType::UssdBalance: {
    auto receivedResponse = parseFromBytes<common::UssdBalanceResponse>(*data);
    if (!receivedResponse) {
      return std::unexpected(receivedResponse.error());
    }

    return std::make_unique<common::UssdBalanceResponse>(*receivedResponse);
  }
  case common::RequestType::UssdMsisdn: {
    auto receivedResponse = parseFromBytes<common::UssdMsisdnResponse>(*data);
    if (!receivedResponse) {
      return std::unexpected(receivedResponse.error());
    }

    return std::make_unique<common::UssdMsisdnResponse>(*receivedResponse);
  }
  case common::RequestType::Error: {
    auto receivedResponse = parseFromBytes<common::ErrorResponse>(*data);
    if (!receivedResponse) {
      return std::unexpected(receivedResponse.error());
    } else {
      return std::unexpected(receivedResponse->getDescription());
    }
  }
  default:
    return std::unexpected("Unexpected response received from BS: " +
                           common::requestTypeToStr(responseType));
  }
}
} // namespace client
