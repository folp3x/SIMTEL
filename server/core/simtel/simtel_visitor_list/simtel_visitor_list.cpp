#include "simtel_visitor_list.h"

#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
std::string SimtelVisitorList::createLogMsg(const std::string &content) const {
  return "VLR of MME_" + std::to_string(mmeId) + ": " + content;
}

SimtelVisitorList::SimtelVisitorList(unsigned int mmeId_) : mmeId(mmeId_) {}

void SimtelVisitorList::setRecord(const VlrRecord &record) {
  MessageHolder::instance().addMsg(
      createLogMsg("set record: " + record.toStr()));
  records.insert({record.mTimsi, record});
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

  MessageHolder::instance().addMsg(createLogMsg("changed path of m-timsi " +
                                                mTimsi + " to BS_" +
                                                std::to_string(bs->getId())));

  it->second.bs = bs;
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

} // namespace server
