#include "simtel_smsc.h"

#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/simtel_mme/simtel_mme.h"

namespace server {
std::string SimtelSmsc::smsUidToStr(const SmsUid &uid) {
  return "uid{iTimsi=" + uid.first + ", smsId=" + std::to_string(uid.second) +
         "}";
}

std::string SimtelSmsc::createLogMsg(const std::string &content) const {
  return "SMSC: " + content;
}

SimtelSmsc::SimtelSmsc(const SmscConfig &config)
    : smsTtlMs(config.smsTtlMs), cdrJsonFilePath(config.cdrJsonFilePath) {}

bool SimtelSmsc::handleSmSubmit(const common::imsi_t &mtimsi_s,
                                unsigned int smsId) {
  MessageHolder::instance().addMsg(createLogMsg(
      "created SMS context for " + smsUidToStr({mtimsi_s, smsId})));

  std::lock_guard lock(contextMtx);
  if (context.size() < MAX_CONTEXT_SIZE) {
    context.insert({{mtimsi_s, smsId}, {mtimsi_s, "", "", "", "", 0}});
    return true;
  }

  return false;
}

bool SimtelSmsc::handleMoForwardSM(const common::imsi_t &mtimsi_s,
                                   unsigned int smsId,
                                   const std::string &smsText) {
  std::lock_guard lock(contextMtx);
  auto it = context.find({mtimsi_s, smsId});
  if (it == context.end()) {
    return false;
  }

  MessageHolder::instance().addMsg(
      createLogMsg("moved sms text from BS of " + smsUidToStr(it->first)));

  it->second.text = smsText;
  return true;
}

bool SimtelSmsc::updateContextMTimsiD(const common::imsi_t &mtimsi_s,
                                      unsigned int smsId,
                                      const common::imsi_t &mtimsi_d) {
  std::lock_guard lock(contextMtx);
  auto it = context.find({mtimsi_s, smsId});
  if (it == context.end()) {
    return false;
  }

  MessageHolder::instance().addMsg(
      createLogMsg("updated mtimsi_d of " + smsUidToStr(it->first)));

  it->second.mtimsi_d = mtimsi_d;
  return true;
}

std::optional<std::string>
SimtelSmsc::getSmsText(unsigned int smsId, const common::imsi_t &mtimsi_s) {
  std::lock_guard lock(contextMtx);
  auto it = context.find({mtimsi_s, smsId});
  if (it == context.end()) {
    return std::nullopt;
  }

  return it->second.text;
}

unsigned int SimtelSmsc::getSmsTtlMs() const { return smsTtlMs; }

void SimtelSmsc::removeSms(unsigned int smsId, const common::imsi_t &mtimsi_s) {
  size_t removedCount = 0;
  {
    std::lock_guard lock(contextMtx);
    removedCount = context.erase({mtimsi_s, smsId});
  }
  if (removedCount > 0) {
    MessageHolder::instance().addMsg(
        createLogMsg("removed sms with " + smsUidToStr({mtimsi_s, smsId})));
  }
}
} // namespace server
