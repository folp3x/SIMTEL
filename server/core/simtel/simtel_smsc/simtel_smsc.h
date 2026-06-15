#pragma once

#include <map>
#include <memory>
#include <unordered_map>

#include "common/types.h"
#include "server/app/config/smsc_config/smsc_config.h"

namespace server {
class SimtelMme;

class SimtelSmsc {
private:
  using SmsUid = std::pair<common::imsi_t, unsigned int>;

  const unsigned int smsTtlMs = 0;

  std::string cdrJsonFilePath = "";

  std::unordered_map<unsigned int, std::shared_ptr<SimtelMme>> mmeList;

  std::map<SmsUid, std::string> context;

  std::string smsUidToStr(const SmsUid &uid);

  std::string createLogMsg(const std::string &content) const;

public:
  explicit SimtelSmsc(const SmscConfig &config);

  void
  setMmeList(const std::unordered_map<unsigned int, std::shared_ptr<SimtelMme>>
                 &mmeList_);

  void handleSmSubmit(const common::imsi_t &miTmsi, unsigned int smsId);
  bool handleMoForwardSM(const common::imsi_t &mTimsi, unsigned int smsId,
                         const std::string &smsText);
};
} // namespace server
