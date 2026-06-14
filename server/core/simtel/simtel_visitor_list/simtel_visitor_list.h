#pragma once

#include <optional>
#include <unordered_map>

#include "server/core/simtel/vlr_record/vlr_record.h"

namespace server {
class SimtelVisitorList {
private:
  std::unordered_map<common::imsi_t, VlrRecord> records = {};

  std::string createLogMsg(const std::string &content) const;

public:
  std::optional<VlrRecord> getRecord(const common::imsi_t &mTimsi) const;

  void setRecord(const common::imsi_t &mTimsi, const common::imei_t &imei,
                 const common::msisdn_t &msisdn, unsigned int bsId);
};
} // namespace server
