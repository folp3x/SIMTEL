#include "simtel_visitor_list.h"

#include "server/app/message_holder/message_holder.h"

namespace server {
std::string SimtelVisitorList::createLogMsg(const std::string &content) const {
  return "VLR: " + content;
}

std::optional<VlrRecord>
SimtelVisitorList::getRecord(const common::imsi_t &mTimsi) const {
  auto it = records.find(mTimsi);
  if (it == records.end()) {
    return std::nullopt;
  }

  MessageHolder::instance().addMsg(
      createLogMsg("found record with m-timsi = " + mTimsi));

  return it->second;
}

void SimtelVisitorList::setRecord(const common::imsi_t &mTimsi,
                                  const common::imei_t &imei,
                                  const common::msisdn_t &msisdn,
                                  std::optional<unsigned int> bsId) {
  MessageHolder::instance().addMsg(
      createLogMsg("set record with m-timsi = " + mTimsi));
  records.insert({mTimsi, {mTimsi, imei, msisdn, bsId}});
}

bool SimtelVisitorList::changePath(const common::imsi_t &mTimsi,
                                   std::optional<unsigned int> bsId) {
  auto it = records.find(mTimsi);
  if (it == records.end()) {
    return false;
  }

  MessageHolder::instance().addMsg(
      createLogMsg("changed path of m-timsi = " + mTimsi + " to BS_" +
                   std::to_string(*bsId)));

  it->second.bsId = *bsId;
  return true;
}

} // namespace server
