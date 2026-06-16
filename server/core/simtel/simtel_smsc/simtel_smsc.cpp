#include "simtel_smsc.h"

#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/simtel_mme/simtel_mme.h"

namespace server {
std::string SimtelSmsc::smsUidToStr(const SmsUid &uid) {
  return "sms-uid{iTimsi=" + uid.first +
         ", smsId=" + std::to_string(uid.second) + "}";
}

std::string SimtelSmsc::createLogMsg(const std::string &content) const {
  return "SMSC: " + content;
}

SimtelSmsc::SimtelSmsc(const SmscConfig &config)
    : smsTtlMs(config.smsTtlMs), cdrJsonFilePath(config.cdrJsonFilePath) {}

void SimtelSmsc::handleSmSubmit(const common::imsi_t &mTimsi,
                                unsigned int smsId) {
  MessageHolder::instance().addMsg(
      createLogMsg("created sms context for " + smsUidToStr({mTimsi, smsId})));

  context.insert({{mTimsi, smsId}, ""});
}

bool SimtelSmsc::handleMoForwardSM(const common::imsi_t &mTimsi,
                                   unsigned int smsId,
                                   const std::string &smsText) {
  auto it = context.find({mTimsi, smsId});
  if (it == context.end()) {
    return false;
  }

  MessageHolder::instance().addMsg(
      createLogMsg("moved sms text of " + smsUidToStr({mTimsi, smsId})));

  it->second = smsText;
  return true;
}
} // namespace server
