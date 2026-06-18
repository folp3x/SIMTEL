#pragma once

#include "common/core/location/location/location.h"
#include "common/core/request/measurement_report_request/measurement_report_request.h"
#include "common/core/request/rrc_connection_request/rrc_connection_request.h"
#include "common/core/request/sm_delivery_ack_request/sm_delivery_ack_request.h"
#include "common/core/request/sm_delivery_request/sm_delivery_request.h"
#include "common/core/request/sm_transfer_request/sm_transfer_request.h"
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
  static constexpr unsigned int SM_DELIVERY_ACK_RECEIVE_TIMEOUT_MSEC = 2000;

  static std::shared_ptr<TtlManager> ttlManager;

  const float radius;
  const unsigned int id;
  const unsigned int mmeId;
  const size_t maxConnections;
  const common::Location<> location;

  SimtelMme *mme;

  mutable std::mutex connectedUeMtx;
  std::unordered_map<common::imsi_t, std::shared_ptr<SimtelUeContext>>
      connectedUe = {};

  std::mutex sendMtxListLock;
  std::unordered_map<common::imsi_t, std::unique_ptr<std::mutex>> sendMtxList{};

  static std::optional<std::string>
  handleLocationUpdate(const common::RrcConnectionRequest &req,
                       std::shared_ptr<SimtelUeContext> ctx,
                       bool firstConnection = true);

  static SimtelBaseStation *findBs(unsigned int id);

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

public:
  SimtelBaseStation(const BsConfig &config, SimtelMme *mme_);

  static void addBs(std::shared_ptr<SimtelBaseStation> bs);

  static void setTtlManager(std::shared_ptr<TtlManager> ttlManager_);

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
  std::mutex *getSendMtx(const common::imsi_t &mTimsi);

  std::shared_ptr<SimtelUeContext> copyUe(const common::imsi_t &mTImsi);
  bool removeUe(const common::imsi_t &mTImsi);

  void handleUe(std::shared_ptr<SimtelUeContext> ctx);

  std::optional<std::string>
  handleSmTransfer(std::shared_ptr<SimtelUeContext> ctx,
                   const common::SmTransferRequest &req);

  bool handleForwardSmReq(const common::imsi_t &imsi, size_t smsTextSize);
  bool handleMtForwardSm(const common::imsi_t &imsi,
                         const common::binary_t &smsText);

  std::expected<common::SmDeliveryRequest, std::string>
  prepareSmDelivery(const common::imsi_t &imsi, unsigned int smsId,
                    const common::imsi_t &msisdn);

  bool sendSmDelivery(const common::imsi_t &imsi,
                      const common::SmDeliveryRequest &response);

  std::optional<common::SmDeliveryAckRequest>
  receiveSmDeliveryAck(const common::imsi_t &imsi);

  std::optional<common::SmDeliveryAckRequest>
  trySendSmsDelivery(const common::imsi_t &imsi, unsigned int smsId,
                     const common::imsi_t &msisdn,
                     const common::binary_t &smsText);

  std::optional<std::string> sendDeliveryReport(const common::imsi_t &imsi,
                                                unsigned int smsId);
};
} // namespace server

#include "simtel_base_station_impl.h"
