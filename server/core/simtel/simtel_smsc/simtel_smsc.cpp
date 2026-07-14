#include "simtel_smsc.h"

#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/simtel_mme/simtel_mme.h"

namespace server {
std::string SimtelSmsc::createLogMsg(const std::string &content) const {
  return "SMSC: " + content;
}

SimtelSmsc::SimtelSmsc(const SmscConfig &config) : smsTtlMs(config.smsTtlMs) {}

bool SimtelSmsc::handleSmSubmit(const common::imsi_t &mtimsi_s,
                                unsigned int smsId) {
  SmsUid uid = {mtimsi_s, smsId};

  std::lock_guard lock(contextMtx);
  if (context.size() < MaxContextSize) {
    MessageHolder::instance().addMsg(
        createLogMsg("created SMS context for " + uid.toStr()));

    SmsContext ctx{mtimsi_s};
    unsigned int warningPeriodSec = 1;
    ctx.ttlManager = std::make_shared<TtlManager>(
        smsTtlMs / common::constants::MsecInSec, warningPeriodSec);

    context.emplace(uid, std::move(ctx));
    return true;
  }

  return false;
}

bool SimtelSmsc::handleMoForwardSM(const common::imsi_t &mtimsi_s,
                                   unsigned int smsId,
                                   const std::string &smsText) {
  SmsUid uid = {mtimsi_s, smsId};

  std::lock_guard lock(contextMtx);
  auto it = context.find(uid);
  if (it == context.end()) {
    return false;
  }

  it->second.text = smsText;

  MessageHolder::instance().addMsg(
      createLogMsg("moved sms text from BS of " + uid.toStr()));

  return true;
}

bool SimtelSmsc::updateMTimsiD(const common::imsi_t &mtimsi_s,
                               unsigned int smsId,
                               const common::imsi_t &mtimsi_d) {
  SmsUid uid = {mtimsi_s, smsId};

  std::lock_guard lock(contextMtx);
  auto it = context.find(uid);
  if (it == context.end()) {
    return false;
  }

  it->second.mtimsi_d = mtimsi_d;

  MessageHolder::instance().addMsg(createLogMsg("updated " + uid.toStr()) +
                                   ": mtimsi_d=" + it->second.mtimsi_d);

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

void SimtelSmsc::removeSms(unsigned int smsId, const common::imsi_t &mtimsi_s) {
  SmsUid uid = {mtimsi_s, smsId};
  size_t removedCount = 0;

  {
    std::lock_guard lock(contextMtx);
    removedCount = context.erase(uid);
  }

  if (removedCount > 0) {
    MessageHolder::instance().addMsg(
        createLogMsg("removed sms with " + uid.toStr()));
  }
}

std::optional<bool>
SimtelSmsc::isDelivered(unsigned int smsId,
                        const common::imsi_t &mtimsi_s) const {
  std::lock_guard lock(contextMtx);
  auto it = context.find({mtimsi_s, smsId});
  if (it == context.end()) {
    return std::nullopt;
  }

  return it->second.delivered->load();
}

bool SimtelSmsc::handleMtForwardSmAck(unsigned int smsId,
                                      const common::imsi_t &mtimsi_s) {
  MessageHolder::instance().addMsg(createLogMsg("received MtForwardSmAck"));

  std::lock_guard lock(contextMtx);
  auto it = context.find({mtimsi_s, smsId});
  if (it == context.end()) {
    return false;
  }

  it->second.delivered->store(true);
  return true;
}

std::shared_ptr<TtlManager>
SimtelSmsc::getTtlManager(unsigned int smsId, const common::imsi_t &mtimsi_s) {
  std::lock_guard lock(contextMtx);
  auto it = context.find({mtimsi_s, smsId});
  if (it == context.end()) {
    return nullptr;
  }

  return it->second.ttlManager;
}
} // namespace server
