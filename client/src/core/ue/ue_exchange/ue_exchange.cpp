#include "ue_exchange.h"

#include "common/core/request/measurement_report_request/measurement_report_request.h"
#include "common/core/request/rrc_connection_request/rrc_connection_request.h"
#include "common/core/request/rrc_reconfiguration_complete_request/rrc_reconfiguration_complete_request.h"

#include "common/core/response/attach_accept_response/attach_accept_response.h"
#include "common/core/response/error_response/error_response.h"

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

  auto reqType = common::BinarySerializer::parseRequestType(*bytes);
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

    callback_t callback = info.callback;
    curProtocol = info.state.protocol;
    switch (info.req->getType()) {
    case common::RequestType::RrcConnection: {
      std::lock_guard lock(receiveMtx);
      auto response = handleLocationUpdate(std::move(info));
      if (!response) {
        signalLevel = NoSignal;
        callback(nullptr, response.error());
      } else {
        callback(std::move(*response), "");
      }

      break;
    }
    case common::RequestType::SmTransfer: {
      auto sendError = sendRequest(std::move(info.req));
      if (sendError) {
        std::string errorMsg = _("Failed to send sms");
        errorMsg += ": " + *sendError;
        callback(nullptr, errorMsg);
      } else {
        callback(nullptr, "");
      }
      break;
    }
    case common::RequestType::SmDeliveryAck: {
      auto sendError = sendRequest(std::move(info.req));
      if (sendError) {
        std::string errorMsg = _("Failed to send SMS delivery acknowledge");
        errorMsg += ": " + *sendError;
        callback(nullptr, errorMsg);
      }
      break;
    }
    default:
      callback(nullptr, _("Unknown request"));
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
    signalLevel = NoSignal;
    closeConnection();
    return std::nullopt;
  }
}

std::expected<std::unique_ptr<common::Request>, std::string>
UeExchange::handleLocationUpdate(RequestInfo info) {
  auto locationSendError = sendRequest(std::move(info.req));
  if (locationSendError) {
    std::string errorMsg = _("Failed to send location");
    errorMsg += ": " + *locationSendError;
    return std::unexpected(errorMsg);
  }

  auto bestSignalResponse = measureSignalToBs(info.state.imei);
  if (!bestSignalResponse) {
    signalLevel = NoSignal;
    return std::unexpected(_("BS not found"));
  }

  auto chosenBsReq = std::make_unique<common::MeasurementReportRequest>(
      info.state.imei, info.state.mTimsi, bestSignalResponse->getBsId());
  auto bsIdSendError = sendRequest(std::move(chosenBsReq));
  if (bsIdSendError) {
    std::string errorMsg = _("Failed to send chosen BS id");
    errorMsg += ": " + *bsIdSendError;
    return std::unexpected(errorMsg);
  }

  common::RequestType responseType;
  auto data = receiveResponseData(responseType);
  if (!data) {
    return std::unexpected(data.error());
  }

  common::imsi_t newMTimsi = info.state.mTimsi;
  unsigned int newBsId = 0;
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
    return std::unexpected(_("Unexpected request"));
  }

  if (newBsId != bestSignalResponse->getBsId()) {
    std::string errorMsg = _("Unknown BS responded");
    errorMsg += ": " + std::to_string(newBsId);
    return std::unexpected(errorMsg);
  }

  auto error = handleConfigureComplete(newMTimsi);
  if (error) {
    return std::unexpected(*error);
  }

  signalLevel = bestSignalResponse->getSignal();
  return response;
}

std::expected<common::MeasurementControlResponse, std::string>
UeExchange::measureSignalToBs(const common::imei_t &imei) {
  bool set = sock.setReceiveTimeout(ReceiveSignalTimeoutMsec);
  if (!set) {
    return std::unexpected(_("Failed to receive signal level"));
  }

  common::MeasurementControlResponse bestSignalResponse{"", NoSignal, 0};
  bool bsLeft = true;
  while (bsLeft) {
    auto signalResponse = receiveResponse<common::MeasurementControlResponse>();
    if (!signalResponse) {
      bsLeft = false;
      if (bestSignalResponse.getSignal() == NoSignal) {
        return std::unexpected(_("BS not found"));
      }
    } else {
      if (signalResponse->getImei() != imei) {
        continue;
      }

      if (signalResponse->getSignal() > bestSignalResponse.getSignal()) {
        bestSignalResponse = std::move(*signalResponse);
      }
    }
  }
  sock.removeReceiveTimeout();

  return bestSignalResponse;
}

std::optional<std::string>
UeExchange::handleConfigureComplete(const common::imsi_t &mTimsi) {
  auto configureCompleteReq =
      std::make_unique<common::RrcReconfigurationCompleteRequest>(mTimsi);
  auto configureCompleteSendError =
      sendRequest(std::move(configureCompleteReq));
  if (configureCompleteSendError) {
    std::string errorMsg = _("Failed to send configure confirm");
    errorMsg += ": " + *configureCompleteSendError;
    return errorMsg;
  }

  auto acceptResponse = receiveResponse<common::AttachAcceptResponse>();
  if (!acceptResponse) {
    std::string errorMsg = _("Failed to receive accept response");
    errorMsg += ": " + acceptResponse.error();
    return errorMsg;
  }

  return std::nullopt;
}

std::expected<std::unique_ptr<common::Request>, std::string>
UeExchange::parseBackgroundResponse(const common::binary_t &responseData,
                                    common::RequestType type) {

  std::unique_ptr<common::Request> response;
  switch (type) {
  case common::RequestType::SmDelivery: {
    auto receivedResponse =
        parseFromBytes<common::SmDeliveryResponse>(responseData);
    if (!receivedResponse) {
      return std::unexpected(receivedResponse.error());
    }

    return std::make_unique<common::SmDeliveryResponse>(*receivedResponse);
  }
  case common::RequestType::SmDeliveryReport: {
    auto receivedResponse =
        parseFromBytes<common::SmDeliveryReportResponse>(responseData);
    if (!receivedResponse) {
      return std::unexpected(receivedResponse.error());
    }

    return std::make_unique<common::SmDeliveryReportResponse>(
        *receivedResponse);
  }
  case common::RequestType::SmDeliveryError: {
    auto receivedResponse =
        parseFromBytes<common::SmDeliveryErrorResponse>(responseData);
    if (!receivedResponse) {
      return std::unexpected(receivedResponse.error());
    }

    return std::make_unique<common::SmDeliveryErrorResponse>(*receivedResponse);
  }
  case common::RequestType::Error: {
    auto receivedResponse = parseFromBytes<common::ErrorResponse>(responseData);
    if (!receivedResponse) {
      return std::unexpected(receivedResponse.error());
    } else {
      return std::unexpected(receivedResponse->getDescription());
    }
  }
  default:
    return std::unexpected(UnexpectedResponseMsg);
  }
}

std::expected<std::unique_ptr<common::Request>, std::string>
UeExchange::parseUssdResponse(const common::binary_t &responseData,
                              common::RequestType type) {
  switch (type) {
  case common::RequestType::UssdBalance: {
    auto receivedResponse =
        parseFromBytes<common::UssdBalanceResponse>(responseData);
    if (!receivedResponse) {
      return std::unexpected(receivedResponse.error());
    }

    return std::make_unique<common::UssdBalanceResponse>(*receivedResponse);
  }
  case common::RequestType::UssdMsisdn: {
    auto receivedResponse =
        parseFromBytes<common::UssdMsisdnResponse>(responseData);
    if (!receivedResponse) {
      return std::unexpected(receivedResponse.error());
    }

    return std::make_unique<common::UssdMsisdnResponse>(*receivedResponse);
  }
  case common::RequestType::Error: {
    auto receivedResponse = parseFromBytes<common::ErrorResponse>(responseData);
    if (!receivedResponse) {
      return std::unexpected(receivedResponse.error());
    } else {
      return std::unexpected(receivedResponse->getDescription());
    }
  }
  default:
    return std::unexpected(UnexpectedResponseMsg);
  }
}

void UeExchange::closeConnection() {
  sock.closeSock();
  connected = false;
}

unsigned int UeExchange::getSignalLevel() const { return signalLevel; }

bool UeExchange::hasSignal() const { return signalLevel != NoSignal; }

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

      auto response = parseBackgroundResponse(*data, responseType);
      if (!response) {
        callback(nullptr, response.error());
      } else {
        callback(std::move(*response), "");
      }
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
    std::string errorMsg = _("Failed to send ussd");
    errorMsg += ": " + *sendError;
    return std::unexpected(errorMsg);
  }

  bool set = sock.setReceiveTimeout(ReceiveUssdTimeoutMsec);
  if (!set) {
    return std::unexpected(_("Failed to receive ussd response"));
  }

  common::RequestType responseType;
  auto data = receiveResponseData(responseType);
  lock.unlock();
  if (!data) {
    return std::unexpected(_("Failed to process command"));
  }

  return parseUssdResponse(*data, responseType);
}
} // namespace client
