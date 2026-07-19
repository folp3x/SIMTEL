#include "simtel_visitor_list.h"

#include "app/menu/message_holder/message_holder.h"
#include "core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
std::string SimtelVisitorList::createLogMsg(const std::string &content) const {
  return "MME_" + std::to_string(mmeId) + "_VLR: " + content;
}

SimtelVisitorList::SimtelVisitorList(unsigned int mmeId_) : mmeId(mmeId_) {}

void SimtelVisitorList::setRecord(const VlrRecord &record) {
  MessageHolder::instance().addMsg(
      createLogMsg("added record: " + record.toStr()));
  records.emplace(record.mTimsi, record);
}

void SimtelVisitorList::removeRecord(const common::imsi_t &mTimsi) {
  size_t removedCount = records.erase(mTimsi);
  if (removedCount > 0) {
    MessageHolder::instance().addMsg(
        createLogMsg("removed record of m-timsi = " + mTimsi));
  }
}

bool SimtelVisitorList::changePath(const common::imsi_t &mTimsi,
                                   std::shared_ptr<SimtelBaseStation> bs) {
  auto it = records.find(mTimsi);
  if (it == records.end()) {
    return false;
  }

  it->second.bs = bs;

  MessageHolder::instance().addMsg(
      createLogMsg("changed path: " + it->second.toStr()));

  return true;
}

size_t SimtelVisitorList::getSize() const { return records.size(); }

std::optional<VlrRecord>
SimtelVisitorList::findByMTimsi(const common::imsi_t &mTimsi) const {
  auto it = records.find(mTimsi);
  if (it == records.end()) {
    return std::nullopt;
  }
  return it->second;
}

std::optional<VlrRecord>
SimtelVisitorList::findByImsi(const common::imsi_t &imsi) const {
  for (const auto &[mTimsi, record] : records) {
    if (record.imsi == imsi) {
      return record;
    }
  }

  return std::nullopt;
}

} // namespace server
