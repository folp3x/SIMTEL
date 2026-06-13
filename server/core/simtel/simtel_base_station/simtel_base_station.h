#pragma once

#include <concepts>
#include <queue>
#include <unordered_map>

#include "common/core/location/location/location.h"
#include "common/core/request/measurement_report_request/measurement_report_request.h"
#include "common/core/request/rrc_connection_request/rrc_connection_request.h"
#include "common/core/request/rrc_reconfiguration_complete_request/rrc_reconfiguration_complete_request.h"
#include "common/network/protocol/protocol.h"
#include "common/types.h"
#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/simtel_ue_context/simtel_ue_context.h"

namespace server {
class SimtelUeContext;

class SimtelBaseStation {
private:
  static std::unordered_map<unsigned int, std::unique_ptr<SimtelBaseStation>>
      baseStations;

  const float radius;
  const unsigned int id;
  const size_t maxConnections;
  common::Location<> location{};

  std::unordered_map<common::imsi_t, std::shared_ptr<SimtelUeContext>>
      connectedUe = {};

  static std::optional<std::string>
  handleLocationUpdate(const common::RrcConnectionRequest &req,
                       std::shared_ptr<SimtelUeContext> ctx);

  static SimtelBaseStation *findBs(unsigned int id);

  std::string createLogMsg(const std::string &content) const;

  unsigned int measureSignal(const common::Location<> &targetLoc) const;

  template <std::derived_from<common::Request> T>
  std::expected<T, std::string>
  receiveRequest(std::shared_ptr<SimtelUeContext> ctx);

  std::optional<std::string> sendRequest(std::shared_ptr<SimtelUeContext> ctx,
                                         std::unique_ptr<common::Request> req);

  std::optional<std::string>
  sendSignalLevel(const common::imei_t &imei, unsigned int signalLevel,
                  std::shared_ptr<SimtelUeContext> ctx) const;

  std::expected<common::MeasurementReportRequest, std::string>
  receiveChosenBsId(std::shared_ptr<SimtelUeContext> ctx) const;

  std::optional<std::string>
  sendBsKeep(const common::imei_t &imei,
             std::shared_ptr<SimtelUeContext> ctx) const;

  std::optional<std::string>
  sendError(const std::string &description,
            std::shared_ptr<SimtelUeContext> ctx) const;

  std::optional<std::string>
  sendBsHandover(const common::imei_t &mTmsi,
                 std::shared_ptr<SimtelUeContext> ctx) const;

  std::optional<std::string>
  sendAttachAccept(std::shared_ptr<SimtelUeContext> ctx) const;

  std::expected<common::RrcReconfigurationCompleteRequest, std::string>
  receiveBsAccept(std::shared_ptr<SimtelUeContext> ctx) const;

public:
  SimtelBaseStation(unsigned int id_, float radius_, size_t maxConnections_,
                    const common::Location<> &location_);

  static void addBs(std::unique_ptr<SimtelBaseStation> bs);

  static void handleConnectionRequest(std::shared_ptr<SimtelUeContext> ctx);

  common::Location<> getLocation() const;

  unsigned int getId() const;

  bool ueConnected(const common::imsi_t &mTimsi) const;

  bool canAcceptConnection() const;

  std::optional<std::string>
  handleMeasurementReport(const common::MeasurementReportRequest &req,
                          std::shared_ptr<SimtelUeContext> ctx,
                          bool &handover) const;

  void addUe(std::shared_ptr<SimtelUeContext> ctx);
  std::shared_ptr<SimtelUeContext> copyUe(const common::imsi_t &mTImsi);
  bool removeUe(const common::imsi_t &mTImsi);

  void handleUe(std::shared_ptr<SimtelUeContext> ctx);
};
} // namespace server

#include "simtel_base_station_impl.h"
