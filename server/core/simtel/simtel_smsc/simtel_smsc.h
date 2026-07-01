#pragma once

#include <atomic>
#include <map>
#include <memory>
#include <optional>
#include <unordered_map>

#include "server/app/config/smsc_config/smsc_config.h"
#include "server/core/simtel/sms_uid/sms_uid.h"
#include "server/core/ttl_manager/ttl_manager.h"

namespace server {
class SimtelMme;

class SimtelSmsc {
private:
  struct SmsContext {
    common::imsi_t mtimsi_s = "";
    common::imsi_t mtimsi_d = "";
    common::msisdn_t msisdn_s = "";
    common::msisdn_t msisdn_d = "";
    std::string text = "";
    unsigned int smsId = 0;

    std::unique_ptr<std::atomic<bool>> delivered =
        std::make_unique<std::atomic<bool>>(false);

    std::shared_ptr<TtlManager> ttlManager = nullptr;
  };

  static constexpr unsigned int MAX_CONTEXT_SIZE = 10;

  const unsigned int smsTtlMs = 0;

  mutable std::mutex contextMtx;
  std::map<SmsUid, SmsContext> context;

  std::string createLogMsg(const std::string &content) const;

public:
  explicit SimtelSmsc(const SmscConfig &config);

  bool handleSmSubmit(const common::imsi_t &mtimsi_s, unsigned int smsId);
  bool handleMoForwardSM(const common::imsi_t &mtimsi_s, unsigned int smsId,
                         const std::string &smsText);

  bool updateMTimsiD(const common::imsi_t &mtimsi_s, unsigned int smsId,
                     const common::imsi_t &mtimsi_d);

  std::optional<std::string> getSmsText(unsigned int smsId,
                                        const common::imsi_t &mtimsi_s);

  void removeSms(unsigned int smsId, const common::imsi_t &mtimsi_s);

  std::optional<bool> isDelivered(unsigned int smsId,
                                  const common::imsi_t &mtimsi_s) const;
  bool markDelivered(unsigned int smsId, const common::imsi_t &mtimsi_s);

  std::shared_ptr<TtlManager> getTtlManager(unsigned int smsId,
                                            const common::imsi_t &mtimsi_s);
};
} // namespace server
