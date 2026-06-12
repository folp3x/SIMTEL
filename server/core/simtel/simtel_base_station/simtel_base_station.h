#pragma once

#include <array>
#include <memory>
#include <queue>
#include <unordered_map>

#include "common/core/location/location/location.h"
#include "common/core/request/measurement_report_request/measurement_report_request.h"
#include "common/core/request/rrc_connection_request/rrc_connection_request.h"
#include "common/network/protocol/protocol.h"
#include "common/types.h"

namespace server {
class SimtelUeContext;

class SimtelBaseStation {
private:
  static std::unordered_map<unsigned int, std::unique_ptr<SimtelBaseStation>>
      baseStations;

  const float radius = 80;
  const unsigned int id = 0;

  std::unordered_map<common::imsi_t, std::shared_ptr<SimtelUeContext>>
      connectedUe = {};
  common::Location<> location{};

  std::queue<common::Request> requests{};

  std::string createLogMsg(const std::string &content) const;

  unsigned int measureSignal(const common::Location<> &targetLoc) const;

  std::optional<std::string>
  sendSignalLevel(const common::imei_t &imei, unsigned int signalLevel,
                  std::shared_ptr<SimtelUeContext> ctx);

  std::expected<common::RrcConnectionRequest, std::string>
  receiveLocation(std::shared_ptr<SimtelUeContext> ctx);

  std::expected<common::MeasurementReportRequest, std::string>
  receiveChosenBsId(std::shared_ptr<SimtelUeContext> ctx);

  std::optional<std::string> sendBsKeep(const common::imei_t &imei,
                                        std::shared_ptr<SimtelUeContext> ctx);

  std::optional<std::string>
  sendBsHandover(const common::imei_t &mTmsi,
                 std::shared_ptr<SimtelUeContext> ctx);

  static std::optional<std::string>
  handleLocationUpdate(const common::RrcConnectionRequest &req,
                       std::shared_ptr<SimtelUeContext> ctx);

  static SimtelBaseStation *findBs(unsigned int id);

public:
  SimtelBaseStation(unsigned int id_);

  static void handleConnectionRequest(std::shared_ptr<SimtelUeContext> ctx);

  common::Location<> getLocation() const;

  unsigned int getId() const;

  bool ueConnected(const common::imsi_t &mTimsi);

  void handleMeasurementReport(const common::MeasurementReportRequest &req,
                               std::shared_ptr<SimtelUeContext> ctx,
                               common::imsi_t &handoverMTimsi);

  std::shared_ptr<SimtelUeContext> takeUe(const common::imsi_t &mTImsi);
  void addUe(const common::imsi_t &mTimsi,
             std::shared_ptr<SimtelUeContext> ctx);
};
} // namespace server
