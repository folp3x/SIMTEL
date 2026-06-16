#include "simtel_visitor_list.h"

#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
std::string SimtelVisitorList::createLogMsg(const std::string &content) const {
  return "VLR: " + content;
}

std::optional<common::imsi_t>
SimtelVisitorList::getImsiByMTimsi(const common::imsi_t &mTimsi) const {
  auto it = records.find(mTimsi);
  if (it == records.end()) {
    return std::nullopt;
  }

  MessageHolder::instance().addMsg(
      createLogMsg("found record: " + it->second.toStr()));

  return it->second.imsi;
}

void SimtelVisitorList::setRecord(const VlrRecord &record) {
  MessageHolder::instance().addMsg(
      createLogMsg("set record: " + record.toStr()));
  records.insert({record.mTimsi, record});
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

} // namespace server
