#pragma once

#include <map>
#include <memory>
#include <optional>
#include <unordered_map>

#include "common/types.h"
#include "server/app/config/smsc_config/smsc_config.h"

namespace server {
class SimtelMme;

class SimtelSmsc {
private:
  // mtimsi_s и sms-id
  using SmsUid = std::pair<common::imsi_t, unsigned int>;

  struct SmsContext {
    common::imsi_t mtimsi_s = "";
    common::imsi_t mtimsi_d = "";
    common::msisdn_t msisdn_s = "";
    common::msisdn_t msisdn_d = "";
    std::string text = "";
    unsigned int smsId = 0;
  };

  static constexpr unsigned int MAX_CONTEXT_SIZE = 10;

  const unsigned int smsTtlMs = 0;

  std::string cdrJsonFilePath = "";

  std::map<SmsUid, SmsContext> context;

  std::string smsUidToStr(const SmsUid &uid);

  std::string createLogMsg(const std::string &content) const;

public:
  explicit SimtelSmsc(const SmscConfig &config);

  bool handleSmSubmit(const common::imsi_t &mtimsi_s, unsigned int smsId);
  bool handleMoForwardSM(const common::imsi_t &mtimsi_s, unsigned int smsId,
                         const std::string &smsText);

  bool updateContextMTimsiD(const common::imsi_t &mtimsi_s, unsigned int smsId,
                            const common::imsi_t &mtimsi_d);

  std::optional<std::string> getSmsText(unsigned int smsId,
                                        const common::imsi_t &mtimsi_s);

  unsigned int getSmsTtlMs() const;
};
} // namespace server
