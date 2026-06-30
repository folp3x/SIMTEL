#include "simtel_mme.h"

#include "common/network/binary_serializer/binary_serializer.h"
#include "common/utils/str/str.h"
#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
uint64_t SimtelMme::curMTimsi = 0;

SimtelMme::SimtelMme(const MmeConfig &config,
                     std::shared_ptr<SimtelRegister> hlr_,
                     std::weak_ptr<SimtelSmsc> smsc_)
    : id(config.id), maxVlrSize(config.maxVlrSize), hlr(hlr_), smsc(smsc_),
      vlr(id) {}

std::optional<common::imsi_t>
SimtelMme::findImsiInHlr(const common::imsi_t &mTimsi,
                         unsigned int &mmeId) const {
  auto imsi = hlr->getImsiByMTimsi(mTimsi, mmeId);
  if (!imsi) {
    return std::nullopt;
  }
  return *imsi;
}

std::optional<common::imsi_t>
SimtelMme::findImsiInVlr(const common::imsi_t &mTimsi) const {
  auto found = vlr.findByMTimsi(mTimsi);
  if (!found) {
    return std::nullopt;
  }

  return found->imsi;
}

void SimtelMme::addOtherMme(std::shared_ptr<SimtelMme> mme) {
  otherMme.insert({mme->getId(), mme});
}

void SimtelMme::addBs(std::shared_ptr<SimtelBaseStation> bs) {
  baseStations.insert({bs->getId(), bs});
}

unsigned int SimtelMme::getId() const { return id; }

void SimtelMme::removeFromVlr(const common::imsi_t &mTimsi) {
  vlr.removeRecord(mTimsi);
}

std::expected<common::imsi_t, std::string>
SimtelMme::handleAttachRequest(const common::imsi_t &imsi,
                               const common::imei_t &imei) {
  MessageHolder::instance().addMsg(createLogMsg(
      "received AttachRequest(imsi=" + imsi + ", imei=" + imei + ")"));
  unsigned int mmeId;
  auto found = findImsiInHlr(imsi, mmeId);
  if (found) {
    common::imsi_t mTimsi = imsi;

    MessageHolder::instance().addMsg(
        createLogMsg("received m-timsi is not real imsi"));

    if (mmeId != id) {
      auto hlrRecord = hlr->handleAuthInfoRequest(*found, imei, mTimsi);
      if (!hlrRecord) {
        return std::unexpected(hlrRecord.error());
      }

      vlr.setRecord({mTimsi, *found, imei, hlrRecord->msisdn, nullptr});
    }

    return mTimsi;
  } else {
    MessageHolder::instance().addMsg(
        createLogMsg("received m-timsi is real imsi"));

    if (vlr.getSize() >= maxVlrSize) {
      return std::unexpected("VLR cant accept more records");
    }

    common::imsi_t mTimsi = generateMTimsi();

    MessageHolder::instance().addMsg(
        createLogMsg("generated m-timsi: " + mTimsi));

    auto hlrRecord = hlr->handleAuthInfoRequest(imsi, imei, mTimsi);
    if (!hlrRecord) {
      return std::unexpected(hlrRecord.error());
    }

    vlr.setRecord({mTimsi, imsi, imei, hlrRecord->msisdn, nullptr});

    return mTimsi;
  }
}

std::optional<std::string>
SimtelMme::handleAuthResponse(const common::imsi_t &mTimsi, unsigned int bsId) {
  MessageHolder::instance().addMsg(
      createLogMsg("received AuthResponse(mTimsi=" + mTimsi +
                   ", bsId=" + std::to_string(bsId) + ")"));

  auto bs = findBsById(bsId);
  if (!bs) {
    return "BS not known by MME";
  }

  bool changed = vlr.changePath(mTimsi, bs);
  if (!changed) {
    return "IMSI not found in VLR";
  }

  auto realImsi = findImsiInVlr(mTimsi);
  if (!realImsi) {
    return "IMSI not found in VLR";
  }

  auto result = hlr->handleUpdateLocationRequest(*realImsi, id);
  if (!result) {
    return result.error();
  }

  auto prevMmeId = *result;
  if (prevMmeId) {
    auto prevMme = findOtherById(*prevMmeId);
    if (prevMme) {
      prevMme->removeFromVlr(mTimsi);
    }
  }

  return std::nullopt;
}

std::shared_ptr<SimtelMme> SimtelMme::findOtherById(unsigned int id) const {
  auto it = otherMme.find(id);
  if (it == otherMme.end()) {
    return nullptr;
  }
  return it->second;
}

bool SimtelMme::handleSmSubmit(const common::imsi_t &mtimsi_s,
                               unsigned int smsId) {
  MessageHolder::instance().addMsg(createLogMsg("sent SM_Submit to SMSC"));
  return smsc.lock()->handleSmSubmit(mtimsi_s, smsId);
}

bool SimtelMme::handleMoForwardSM(const common::imsi_t &mtimsi_s,
                                  unsigned int smsId,
                                  const std::string &smsText) {
  MessageHolder::instance().addMsg(createLogMsg("sent MO_Forward_SM to SMSC"));
  return smsc.lock()->handleMoForwardSM(mtimsi_s, smsId, smsText);
}

std::optional<std::string>
SimtelMme::sendRoutingInfoSm(const common::msisdn_t &msisdn_d,
                             unsigned int smsId,
                             const common::imsi_t &mtimsi_s) {
  auto senderImsi = findImsiInVlr(mtimsi_s);
  if (!senderImsi) {
    return "Sender IMSI not found in VLR";
  }

  MessageHolder::instance().addMsg(
      createLogMsg("request to HLR: Routing_Info_SM(msisdn=" + msisdn_d) + ")");

  auto senderRecord = hlr->handleRoutingInfoSmSender(*senderImsi);
  if (!senderRecord) {
    return senderRecord.error();
  }

  if (senderRecord->msisdn == msisdn_d) {
    return "SMS cant be sent to same MSISDN";
  }

  auto receiverRecord = hlr->handleRoutingInfoSmReceiver(msisdn_d);
  if (!receiverRecord) {
    return receiverRecord.error();
  }

  if (!receiverRecord->isMtimsiSet()) {
    return "Unknown receiver m-timsi";
  }

  if (receiverRecord->mmeId != id) {
    MessageHolder::instance().addMsg(createLogMsg("changing MME"));
    auto receiverMme = findOtherById(receiverRecord->mmeId);
    if (!receiverMme) {
      return "Receiver MME not found";
    }

    return receiverMme->sendForwardSm(senderRecord->msisdn, smsId, mtimsi_s,
                                      receiverRecord->mTimsi);
  }

  return sendForwardSm(senderRecord->msisdn, smsId, mtimsi_s,
                       receiverRecord->mTimsi);
}

std::optional<std::string>
SimtelMme::sendForwardSm(const common::msisdn_t &msisdn_s, unsigned int smsId,
                         const common::imsi_t &mtimsi_s,
                         const common::imsi_t &mtimsi_d) {
  MessageHolder::instance().addMsg(
      createLogMsg("sent context update request to SMSC"));

  bool updated = smsc.lock()->updateMTimsiD(mtimsi_s, smsId, mtimsi_d);
  if (!updated) {
    return "Error updating SMSC context";
  }

  auto receiverInfo = vlr.findByMTimsi(mtimsi_d);
  if (!receiverInfo) {
    return "Unknown receiver";
  }

  auto bs = receiverInfo->bs;
  if (!bs) {
    return "Receiver BS not found";
  }

  auto smsText = smsc.lock()->getSmsText(smsId, mtimsi_s);
  if (!smsText) {
    return "SMS text not found in SMSC";
  }

  MessageHolder::instance().addMsg(createLogMsg("received sms text from SMSC"));

  std::thread smsSender(&SimtelMme::trySendSms, this, msisdn_s, smsId, mtimsi_s,
                        mtimsi_d, *smsText, bs);
  smsSender.detach();

  return std::nullopt;
}

void SimtelMme::handleSmDeliveryAck(const common::msisdn_t &msisdn_s,
                                    unsigned int smsId,
                                    const common::imsi_t &mtimsi_d) {
  MessageHolder::instance().addMsg(createLogMsg("received SmDeliveryAck"));
  auto mtimsi_s = hlr->getMTimsiByMsisdn(msisdn_s);
  if (!mtimsi_s) {
    MessageHolder::instance().addErrorMsg("mtimsi_s not found in HLR");
    return;
  }

  SmsUid uid = {*mtimsi_s, smsId};
  auto deliveredInfo = smsc.lock()->isDelivered(smsId, *mtimsi_s);
  if (!deliveredInfo) {
    return;
  }

  if (*deliveredInfo) {
    return;
  }

  smsc.lock()->markDelivered(smsId, *mtimsi_s);

  bool reportSent = false;
  while (!reportSent) {
    auto senderMmeId = hlr->getMmeIdByMTimsi(*mtimsi_s);
    if (!senderMmeId) {
      continue;
    }

    if (*senderMmeId == id) {
      reportSent = sendSmDeliveryReport(*mtimsi_s, smsId);
    } else {
      auto senderMme = findOtherById(*senderMmeId);
      if (!senderMme) {
        MessageHolder::instance().addErrorMsg("unknown sender MME");
        return;
      }

      reportSent = senderMme->sendSmDeliveryReport(*mtimsi_s, smsId);
    }

    std::this_thread::sleep_for(
        std::chrono::milliseconds(SEND_REPORT_SLEEP_MS));
  }
}

bool SimtelMme::sendSmDeliveryReport(const common::msisdn_t &mtimsi_s,
                                     unsigned int smsId) {
  auto record = vlr.findByMTimsi(mtimsi_s);
  if (!record) {
    MessageHolder::instance().addErrorMsg("mtimsi_s not found in VLR");
    return false;
  }

  auto senderBs = record->bs;

  MessageHolder::instance().addMsg(
      createLogMsg("trying to send SmDeliveryReport"));

  auto error = senderBs->sendSmDeliveryReport(mtimsi_s, smsId);
  if (error) {
    MessageHolder::instance().addErrorMsg(*error);
    return false;
  }

  return true;
}

void SimtelMme::sendSmDeliveryError(const common::msisdn_t &mtimsi_s,
                                    unsigned int smsId) {
  auto record = vlr.findByMTimsi(mtimsi_s);
  if (!record) {
    MessageHolder::instance().addErrorMsg("mtimsi_s not found in VLR");
    return;
  }

  auto senderBs = record->bs;

  MessageHolder::instance().addMsg(createLogMsg("sent SmDeliveryError"));

  auto error = senderBs->sendSmDeliveryError(mtimsi_s, smsId);
  if (error) {
    MessageHolder::instance().addErrorMsg(*error);
  }
}

void SimtelMme::trySendSms(const common::msisdn_t &msisdn_s, unsigned int smsId,
                           const common::imsi_t &mtimsi_s,
                           const common::imsi_t &mtimsi_d,
                           const std::string &smsText,
                           std::shared_ptr<SimtelBaseStation> bs) {
  common::binary_t binary = common::BinarySerializer::strToBinary(smsText);

  unsigned int smsTtlSec =
      smsc.lock()->getSmsTtlMs() / common::constants::MSEC_IN_SEC;
  unsigned int warningPeriodSec = 1;
  TtlManager ttlManager{smsTtlSec, warningPeriodSec};
  ttlManager.start();

  MessageHolder::instance().addMsg(createLogMsg("trying to send SM_Delivery"));

  SmsUid uid = {mtimsi_s, smsId};
  bool delivered = false;
  while (true) {
    if (ttlManager.isActive() && ttlManager.isExpired()) {
      MessageHolder::instance().addErrorMsg("SMS(" + uid.toStr() +
                                            ") TTL expired");
      break;
    }

    auto warningSec = ttlManager.getWarningSec();
    if (warningSec) {
      MessageHolder::instance().addMsg(
          "SMS(" + uid.toStr() + ") TTL: " + std::to_string(*warningSec) +
              " seconds left",
          common::MenuMessageType::INFO);
    }

    bs->sendSmDelivery(mtimsi_d, smsId, msisdn_s, binary);
    std::this_thread::sleep_for(std::chrono::milliseconds(SEND_SMS_SLEEP_MS));

    auto deliveredInfo = smsc.lock()->isDelivered(smsId, mtimsi_s);
    if (!deliveredInfo) {
      MessageHolder::instance().addErrorMsg("SMS(" + uid.toStr() +
                                            ") not found in SMSC");
      break;
    }

    if (*deliveredInfo) {
      delivered = true;
      break;
    }
  }

  smsc.lock()->removeSms(smsId, mtimsi_s);

  if (!delivered) {
    sendSmDeliveryError(mtimsi_s, smsId);
  }
}

common::imsi_t SimtelMme::generateMTimsi() {
  curMTimsi++;
  if (curMTimsi > MAX_MTIMSI) {
    curMTimsi = 0;
  }

  return common::imsiToStr(curMTimsi);
}

std::shared_ptr<SimtelBaseStation>
SimtelMme::findBsById(unsigned int id) const {
  auto it = baseStations.find(id);
  if (it == baseStations.end()) {
    return nullptr;
  }
  return it->second;
}

std::string SimtelMme::createLogMsg(const std::string &content) const {
  return "MME_" + std::to_string(id) + ": " + content;
}
} // namespace server
