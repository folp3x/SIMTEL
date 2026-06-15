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
  std::optional<common::imsi_t>
  getImsiByMTimsi(const common::imsi_t &mTimsi) const;

  void setRecord(const VlrRecord &record);

  bool changePath(const common::imsi_t &mTimsi,
                  std::shared_ptr<SimtelBaseStation> bs);

  size_t getSize() const;
};
} // namespace server
