#pragma once

#include <expected>
#include <memory>

#include "common/types.h"
#include "server/app/config/mme_config/mme_config.h"
#include "server/core/simtel/simtel_register/simtel_register.h"
#include "server/core/simtel/simtel_smsc/simtel_smsc.h"
#include "server/core/simtel/simtel_visitor_list/simtel_visitor_list.h"

namespace server {
class SimtelMme {
private:
  static constexpr uint64_t MAX_MTIMSI = 999'999'999'999'999;
  static constexpr unsigned int SEND_SMS_SLEEP_MS = 1000;

  const unsigned int id = 0;
  const unsigned int maxVlrSize = 0;

  static uint64_t curMTimsi;

  SimtelVisitorList vlr;

  std::shared_ptr<SimtelRegister> hlr;
  std::weak_ptr<SimtelSmsc> smsc;
  std::unordered_map<unsigned int, std::shared_ptr<SimtelBaseStation>>
      baseStations;

  std::unordered_map<unsigned int, std::shared_ptr<SimtelMme>> otherMme{};

  static common::imsi_t generateMTimsi();

  std::shared_ptr<SimtelBaseStation> findBsById(unsigned int id) const;

  std::string createLogMsg(const std::string &content) const;

  std::shared_ptr<SimtelMme> findOtherById(unsigned int id) const;

  void trySendSms(const common::msisdn_t &msisdn_s, unsigned int smsId,
                  const common::imsi_t &mtimsi_s,
                  const common::imsi_t &mtimsi_d, const std::string &smsText,
                  std::shared_ptr<SimtelBaseStation> bs);

  void sendSmDeliveryReport(const common::msisdn_t &mtimsi_s,
                            unsigned int smsId);

  void sendSmDeliveryError(const common::msisdn_t &mtimsi_s,
                           unsigned int smsId);

  void removeFromVlr(const common::imsi_t &mTimsi);

  std::optional<common::imsi_t> findImsiInHlr(const common::imsi_t &mTimsi,
                                              unsigned int &mmeId) const;

  std::optional<common::imsi_t>
  findImsiInVlr(const common::imsi_t &mTimsi) const;

public:
  SimtelMme(const MmeConfig &config, std::shared_ptr<SimtelRegister> hlr_,
            std::weak_ptr<SimtelSmsc> smsc_);

  void addOtherMme(std::shared_ptr<SimtelMme> mme);

  void addBs(std::shared_ptr<SimtelBaseStation> bs);

  unsigned int getId() const;

  std::expected<common::imsi_t, std::string>
  handleAttachRequest(const common::imsi_t &imsi, const common::imei_t &imei);

  std::optional<std::string> handleAuthResponse(const common::imsi_t &mTimsi,
                                                unsigned int bsId);

  bool handleSmSubmit(const common::imsi_t &mtimsi_s, unsigned int smsId);

  bool handleMoForwardSM(const common::imsi_t &mtimsi_s, unsigned int smsId,
                         const std::string &smsText);

  std::optional<std::string> sendRoutingInfoSm(const common::msisdn_t &msisdn_d,
                                               unsigned int smsId,
                                               const common::imsi_t &mtimsi_s);

  std::optional<std::string> sendForwardSm(const common::msisdn_t &msisdn_s,
                                           unsigned int smsId,
                                           const common::imsi_t &mtimsi_s,
                                           const common::imsi_t &mtimsi_d);

  void handleSmDeliveryAck(const common::msisdn_t &msisdn_s, unsigned int smsId,
                           const common::imsi_t &mtimsi_d);
};
} // namespace server
