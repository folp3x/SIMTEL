#pragma once

#include <optional>
#include <unordered_map>

#include "server/core/simtel/vlr_record/vlr_record.h"

namespace server {
class SimtelVisitorList {
private:
  const unsigned int mmeId;

  std::unordered_map<common::imsi_t, VlrRecord> records = {};

  std::string createLogMsg(const std::string &content) const;

public:
  explicit SimtelVisitorList(unsigned int mmeId_);

  void setRecord(const VlrRecord &record);

  void removeRecord(const common::imsi_t &imsi);

  bool changePath(const common::imsi_t &imsi,
                  std::shared_ptr<SimtelBaseStation> bs);

  size_t getSize() const;

  std::optional<VlrRecord> findByImsi(const common::imsi_t &imsi) const;
};
} // namespace server
