#pragma once

#include <expected>
#include <memory>

#include "common/types.h"
#include "server/app/config/mme_config/mme_config.h"

#include "server/core/simtel/simtel_pcrf/simtel_pcrf.h"
#include "server/core/simtel/simtel_register/simtel_register.h"
#include "server/core/simtel/simtel_smsc/simtel_smsc.h"
#include "server/core/simtel/simtel_visitor_list/simtel_visitor_list.h"

namespace server {
class SimtelMme {
private:
  static constexpr uint64_t MaxMtimsi = 999'999'999'999'999;
  static constexpr std::chrono::milliseconds SendSmsSleepTime{1000};
  static constexpr std::chrono::milliseconds SendReportSleepTime{1000};

  const unsigned int id = 0;
  const unsigned int maxVlrSize = 0;

  static uint64_t curMTimsi;

  SimtelVisitorList vlr;

  std::shared_ptr<SimtelRegister> reg;
  std::weak_ptr<SimtelSmsc> smsc;
  std::unordered_map<unsigned int, std::shared_ptr<SimtelBaseStation>>
      baseStations;

  std::unordered_map<unsigned int, std::shared_ptr<SimtelMme>> otherMme{};

  std::shared_ptr<SimtelPcrf> pcrf;

  static common::imsi_t generateMTimsi();

  std::shared_ptr<SimtelBaseStation> findBsById(unsigned int id) const;

  std::string createLogMsg(const std::string &content) const;

  std::shared_ptr<SimtelMme> findOtherById(unsigned int id) const;

  void trySendSms(const common::msisdn_t &msisdn_s, unsigned int smsId,
                  const common::imsi_t &mtimsi_s,
                  const common::imsi_t &mtimsi_d, const std::string &smsText,
                  std::shared_ptr<SimtelBaseStation> bs);

  bool sendSmDeliveryReport(const common::msisdn_t &mtimsi_s,
                            unsigned int smsId);

  void sendSmDeliveryError(const common::msisdn_t &mtimsi_s,
                           unsigned int smsId);

  void removeFromVlr(const common::imsi_t &mTimsi);

  std::optional<common::imsi_t> findImsiInOther(const common::imsi_t &mTimsi,
                                                unsigned int &mmeId) const;

  std::optional<common::imsi_t>
  findImsiInVlr(const common::imsi_t &mTimsi) const;

  std::optional<common::imsi_t>
  findMTimsiInVlr(const common::imsi_t &imsi) const;

  void trySendReport(unsigned int smsId, const common::imsi_t &imsi_s);

public:
  SimtelMme(const MmeConfig &config, std::shared_ptr<SimtelRegister> reg_,
            std::weak_ptr<SimtelSmsc> smsc_, std::shared_ptr<SimtelPcrf> pcrf_);

  void addOtherMme(std::shared_ptr<SimtelMme> mme);

  void addBs(std::shared_ptr<SimtelBaseStation> bs);

  unsigned int getId() const;

  std::expected<common::imsi_t, std::string>
  handleAttachRequest(const common::imsi_t &imsi, const common::imei_t &imei);

  std::optional<std::string> handleAuthResponse(const common::imsi_t &mTimsi,
                                                unsigned int bsId);

  std::optional<std::string> handleSmSubmit(const common::imsi_t &mtimsi_s,
                                            unsigned int smsId,
                                            std::string &ueErrorMsg);

  bool handleMoForwardSM(const common::imsi_t &mtimsi_s, unsigned int smsId,
                         const std::string &smsText);

  std::optional<std::string> sendRoutingInfoSm(const common::msisdn_t &msisdn_d,
                                               unsigned int smsId,
                                               const common::imsi_t &mtimsi_s,
                                               std::string &ueErrorMsg);

  std::optional<std::string> sendForwardSm(const common::msisdn_t &msisdn_s,
                                           unsigned int smsId,
                                           const common::imsi_t &mtimsi_s,
                                           const common::imsi_t &imsi_d,
                                           std::string &ueErrorMsg);

  void handleSmDeliveryAck(const common::msisdn_t &msisdn_s, unsigned int smsId,
                           const common::imsi_t &mtimsi_d);

  std::optional<std::string> handleUssd(const common::imsi_t &mTimsi,
                                        uint8_t code);
};
} // namespace server
