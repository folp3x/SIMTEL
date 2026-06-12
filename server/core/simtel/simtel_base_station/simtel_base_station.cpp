#include "simtel_base_station.h"

#include <iostream>

#include "common/core/request/request_serializer/request_serializer.h"
#include "common/network/socket/socket_message/socket_message.h"
#include "common/utils/network/network.h"
#include "server/core/distance_calculator/distance_calculator.h"
#include "server/core/simtel/simtel_ue_context/simtel_ue_context.h"

namespace server {
std::unordered_map<unsigned int, std::unique_ptr<SimtelBaseStation>>
    SimtelBaseStation::baseStations = {};

std::string SimtelBaseStation::createLogMsg(const std::string &content) const {
  return "BS_" + std::to_string(id) + ": " + content;
}

void SimtelBaseStation::handleConnectionRequest(
    std::shared_ptr<SimtelUeContext> ctx) {
  baseStations.clear();
  baseStations.emplace(1, std::make_unique<SimtelBaseStation>(1));
  baseStations.emplace(2, std::make_unique<SimtelBaseStation>(2));

  // начальное получение данных через 1ую вышку
  auto *firstBs = baseStations.begin()->second.get();
  ctx->setBs(firstBs);
  auto req = firstBs->receiveLocation(ctx);
  if (!req) {
    std::cout << "Error receiving location: " << req.error() << std::endl;
    return;
  }

  auto error = handleLocationUpdate(*req, std::move(ctx));
  if (error) {
    std::cout << *error << std::endl;
  }
}

common::Location<> SimtelBaseStation::getLocation() const { return location; }

unsigned int SimtelBaseStation::getId() const { return id; }

unsigned int
SimtelBaseStation::measureSignal(const common::Location<> &targetLoc) const {
  float distance = DistanceCalculator::calc(location, targetLoc);
  float coef = 1 - std::abs(distance) / radius;
  return (coef < 0) ? 0 : std::round(coef * 100);
}

std::optional<std::string>
SimtelBaseStation::sendSignalLevel(const common::imei_t &imei,
                                   unsigned int signalLevel,
                                   std::shared_ptr<SimtelUeContext> ctx) {
  common::MeasurementControlRequest req{imei, signalLevel, id};
  auto bytes = common::RequestSerializer::measurementControlToBytes(
      ctx->getProtocol(), req);
  if (!bytes) {
    return bytes.error();
  }
  ctx->setBuf(*bytes);

  auto sendError = ctx->sendBufToUe();
  if (!sendError) {
    std::cout << createLogMsg("Measurement_Control response to UE_" +
                              ctx->getAddrStr() + " = " + req.toStr())
              << std::endl;
  }

  return sendError;
}

std::expected<common::RrcConnectionRequest, std::string>
SimtelBaseStation::receiveLocation(std::shared_ptr<SimtelUeContext> ctx) {
  auto receiveError = ctx->receiveData();
  if (receiveError) {
    return std::unexpected(*receiveError);
  }

  common::Protocol protocol;
  auto req = common::RequestSerializer::rrcConnectionFromBytes(ctx->takeBuf(),
                                                               protocol);
  if (req) {
    ctx->setProtocol(protocol);
    std::cout << createLogMsg("Rrc_Connection req from UE_" +
                              ctx->getAddrStr() + " = " + req->toStr())
              << std::endl;
  }

  return req;
}

std::expected<common::MeasurementReportRequest, std::string>
SimtelBaseStation::receiveChosenBsId(std::shared_ptr<SimtelUeContext> ctx) {
  auto receiveError = ctx->receiveData();
  if (receiveError) {
    return std::unexpected(*receiveError);
  }

  common::Protocol protocol;
  auto req = common::RequestSerializer::measurementReportFromBytes(
      ctx->takeBuf(), protocol);
  if (req) {
    ctx->setProtocol(protocol);
    std::cout << createLogMsg("Measurement_Report req from UE_" +
                              ctx->getAddrStr() + " = " + req->toStr())
              << std::endl;
  }

  return req;
}

std::optional<std::string>
SimtelBaseStation::sendBsKeep(const common::imei_t &imei,
                              std::shared_ptr<SimtelUeContext> ctx) {
  common::RrcReconfigurationKeepRequest req{imei, id};
  auto bytes = common::RequestSerializer::rrcReconfigurationKeepToBytes(
      ctx->getProtocol(), req);
  if (!bytes) {
    return bytes.error();
  }
  ctx->setBuf(*bytes);

  auto sendError = ctx->sendBufToUe();
  if (!sendError) {
    std::cout << createLogMsg("Rrc_Reconfiguration_Keep response to UE_" +
                              ctx->getAddrStr() + " = " + req.toStr())
              << std::endl;
  }

  return sendError;
}

std::optional<std::string>
SimtelBaseStation::sendBsHandover(const common::imei_t &mTmsi,
                                  std::shared_ptr<SimtelUeContext> ctx) {
  common::RrcReconfigurationHandoverRequest req{mTmsi, id};
  auto bytes = common::RequestSerializer::rrcReconfigurationHandoverToBytes(
      ctx->getProtocol(), req);
  if (!bytes) {
    return bytes.error();
  }
  ctx->setBuf(*bytes);

  auto sendError = ctx->sendBufToUe();
  if (!sendError) {
    std::cout << createLogMsg("Rrc_Reconfiguration_Handover response to UE_" +
                              ctx->getAddrStr() + " = " + req.toStr())
              << std::endl;
  }

  return sendError;
}

std::optional<std::string> SimtelBaseStation::handleLocationUpdate(
    const common::RrcConnectionRequest &locReq,
    std::shared_ptr<SimtelUeContext> ctx) {
  // имитация измерения уровня сигнала до базовых станций
  auto initialBs = ctx->getBs();
  for (const auto &[id, bs] : baseStations) {
    unsigned int signalLevel = bs->measureSignal(locReq.loc);
    std::cout << bs->createLogMsg("measured signal level = " +
                                  std::to_string(signalLevel))
              << std::endl;

    if (signalLevel == 0) {
      continue;
    }

    auto signalSendError = bs->sendSignalLevel(locReq.imei, signalLevel, ctx);
    if (signalSendError) {
      return bs->createLogMsg("Error sending signal level: " +
                              *signalSendError);
    }
  }
  ctx->setBs(initialBs);

  auto receiveError = ctx->receiveData();
  if (receiveError) {
    return *receiveError;
  }
  common::binary_t bytes = ctx->takeBuf();

  common::Protocol protocol;
  auto reqType = common::RequestSerializer::parseRequestType(bytes, protocol);
  if (!reqType) {
    return "Error parsing request type: " + reqType.error();
    ;
  }

  if (*reqType == common::RequestType::Measurement_Report) {
    auto chosenBsReq =
        common::RequestSerializer::measurementReportFromBytes(bytes, protocol);
    if (!chosenBsReq) {
      return "Error receiving BS id" + chosenBsReq.error();
    }

    if (chosenBsReq->imei != locReq.imei) {
      return "Unknown imei received: " + chosenBsReq->imei;
    }

    // имитация получение сообщения о выборе вышки конкретной вышкой
    auto chosenBs = findBs(chosenBsReq->bsId);
    if (!chosenBs) {
      return "Requested BS not found";
    }

    common::imsi_t handoverMTimsi = "";
    chosenBs->handleMeasurementReport(*chosenBsReq, ctx, handoverMTimsi);
    if (!handoverMTimsi.empty()) {
      auto ue = ctx->getBs()->takeUe(handoverMTimsi);
      if (!ue) {
        ue = ctx;
      }

      ue->setBs(chosenBs);
      chosenBs->addUe(handoverMTimsi, std::move(ue));
    }
  } else {
    return "MeasurementReport expected";
  }

  return std::nullopt;
}

SimtelBaseStation *SimtelBaseStation::findBs(unsigned int id) {
  auto it = baseStations.find(id);
  if (it == baseStations.end()) {
    return nullptr;
  }
  return it->second.get();
}

SimtelBaseStation::SimtelBaseStation(unsigned int id_) : id(id_) {}

bool SimtelBaseStation::ueConnected(const common::imsi_t &mTimsi) {
  return connectedUe.find(mTimsi) != connectedUe.end();
}

void SimtelBaseStation::handleMeasurementReport(
    const common::MeasurementReportRequest &req,
    std::shared_ptr<SimtelUeContext> ctx, common::imsi_t &handoverMTimsi) {
  common::imsi_t mTimsi = "000000000000000";
  auto curBs = ctx->getBs();
  if (curBs && curBs->getId() == id && ueConnected(mTimsi)) {
    auto keepSendError = sendBsKeep(req.imei, ctx);
    if (keepSendError) {
      std::cout << createLogMsg("Error sending BS keep info: " + *keepSendError)
                << std::endl;
    }
  } else {
    auto handoverSendError = sendBsHandover(mTimsi, ctx);
    if (handoverSendError) {
      std::cout << createLogMsg("Error sending BS handover info: " +
                                *handoverSendError)
                << std::endl;
    } else {
      handoverMTimsi = mTimsi;
    }
  }
}

std::shared_ptr<SimtelUeContext>
SimtelBaseStation::takeUe(const common::imsi_t &mTImsi) {
  auto it = connectedUe.find(mTImsi);
  if (it == connectedUe.end()) {
    return nullptr;
  }

  auto ue = it->second;
  connectedUe.erase(it);
  return ue;
}

void SimtelBaseStation::addUe(const common::imsi_t &mTimsi,
                              std::shared_ptr<SimtelUeContext> ctx) {
  connectedUe.emplace(mTimsi, ctx);
  std::cout << createLogMsg("UE_" + ctx->getAddrStr() + " buffer added ")
            << std::endl;
}
} // namespace server
