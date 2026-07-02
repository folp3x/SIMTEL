#pragma once

#include "common/core/location/location/location.h"

#include "common/core/request/measurement_report_request/measurement_report_request.h"
#include "common/core/request/rrc_connection_request/rrc_connection_request.h"
#include "common/core/request/sm_delivery_ack_request/sm_delivery_ack_request.h"
#include "common/core/request/sm_transfer_request/sm_transfer_request.h"

#include "common/core/response/sm_delivery_response/sm_delivery_response.h"
#include "common/network/protocol/protocol.h"
#include "server/app/config/bs_config/bs_config/bs_config.h"

#include "server/core/simtel/simtel_mme/simtel_mme.h"
#include "server/core/simtel/simtel_ue_context/simtel_ue_context.h"
#include "server/core/ttl_manager/ttl_manager.h"

namespace server {
class SimtelBaseStation {
private:
  static std::unordered_map<unsigned int, std::shared_ptr<SimtelBaseStation>>
      baseStations;

  static constexpr unsigned int CONNECTION_HANDLE_RECEIVE_TIMEOUT_MSEC = 10000;

  const float radius;
  const unsigned int id;
  const unsigned int mmeId;
  const size_t maxConnections;
  const common::Location<> location;

  std::weak_ptr<SimtelMme> mme;

  mutable std::mutex connectedUeMtx;
  std::unordered_map<common::imsi_t, std::shared_ptr<SimtelUeContext>>
      connectedUe = {};

  static std::optional<std::string>
  handleLocationUpdate(const common::RrcConnectionRequest &req,
                       std::shared_ptr<SimtelUeContext> ctx,
                       bool firstConnection = true);

  static std::shared_ptr<SimtelBaseStation> findBs(unsigned int id);

  template <std::derived_from<common::Request> T>
  std::expected<T, std::string>
  receiveRequest(std::shared_ptr<SimtelUeContext> ctx) const;

  template <std::derived_from<common::Request> T>
  std::expected<T, std::string>
  parseFromBytes(const common::binary_t &bytes,
                 common::Protocol &protocol) const;

  std::optional<std::string>
  sendResponse(std::shared_ptr<SimtelUeContext> ctx,
               std::unique_ptr<common::Request> req) const;

  std::string createLogMsg(const std::string &content) const;

  unsigned int measureSignal(const common::Location<> &targetLoc) const;

  std::optional<std::string>
  handleConfigureComplete(std::shared_ptr<SimtelUeContext> ctx) const;

  float calculateDistance(const common::Location<> &ueLoc) const;

  std::shared_ptr<SimtelUeContext> findUe(const common::imsi_t &mTimsi) const;

  bool handleForwardSmReq(const common::imsi_t &mTimsi, size_t smsTextSize);

  bool handleMtForwardSm(const common::imsi_t &mTimsi,
                         const common::binary_t &smsText);

  std::expected<common::SmDeliveryResponse, std::string>
  prepareSmDelivery(const common::imsi_t &mTimsi, unsigned int smsId,
                    const common::imsi_t &msisdn);

  std::optional<std::string>
  handleMeasurementReport(const common::MeasurementReportRequest &req,
                          std::shared_ptr<SimtelUeContext> ctx,
                          bool &handover) const;

  std::optional<std::string>
  handleSmTransfer(std::shared_ptr<SimtelUeContext> ctx,
                   const common::SmTransferRequest &req,
                   std::string &ueErrorMsg);

  std::optional<std::string> sendResponse(const common::imsi_t &mTimsi,
                                          std::unique_ptr<common::Request> req);

  void handleUeRequests(std::shared_ptr<SimtelUeContext> ctx);

  std::shared_ptr<SimtelUeContext> copyUe(const common::imsi_t &mTimsi);
  bool removeUe(const common::imsi_t &mTimsi);

public:
  SimtelBaseStation(const BsConfig &config, std::weak_ptr<SimtelMme> mme_);

  static void addBs(std::shared_ptr<SimtelBaseStation> bs);

  static void handleConnection(std::shared_ptr<SimtelUeContext> ctx);

  unsigned int getId() const;

  bool ueConnected(const common::imsi_t &mTimsi) const;

  bool canAcceptConnection() const;

  void addUe(std::shared_ptr<SimtelUeContext> ctx);

  void sendSmDelivery(const common::imsi_t &mTimsi, unsigned int smsId,
                      const common::imsi_t &msisdn,
                      const common::binary_t &smsText);

  std::optional<std::string> sendSmDeliveryReport(const common::imsi_t &mTimsi,
                                                  unsigned int smsId);

  std::optional<std::string>
  sendSmDeliveryError(const common::imsi_t &mTimsi, unsigned int smsId,
                      const std::string &description);

  std::optional<std::string> sendUssdBalance(const common::imsi_t &mTimsi,
                                             double balance);

  std::optional<std::string> sendUssdMsisdn(const common::imsi_t &mTimsi,
                                            const common::msisdn_t &msisdn);
};
} // namespace server

#include "simtel_base_station_impl.h"
