#include "simtel_mme.h"

#include "common/core/ussd/ussd_code.h"
#include "common/network/binary_serializer/binary_serializer.h"
#include "common/utils/num/num.h"

#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
SimtelMme::SimtelMme(const MmeConfig &config,
                     std::shared_ptr<SimtelRegister> reg_,
                     std::weak_ptr<SimtelSmsc> smsc_,
                     std::shared_ptr<SimtelPcrf> pcrf_)
    : id(config.id), maxVlrSize(config.maxVlrSize), reg(reg_), smsc(smsc_),
      pcrf(pcrf_), vlr(id) {}

std::optional<common::imsi_t>
SimtelMme::findImsiInOther(const common::imsi_t &mTimsi,
                           unsigned int &mmeId) const {
  for (const auto &other : otherMme) {
    auto found = other.second->findImsiInVlr(mTimsi);
    if (found) {
      mmeId = other.first;
      return *found;
    }
  }

  return std::nullopt;
}

std::optional<common::imsi_t>
SimtelMme::findImsiInVlr(const common::imsi_t &mTimsi) const {
  auto found = vlr.findByMTimsi(mTimsi);
  if (!found) {
    return std::nullopt;
  }

  return found->imsi;
}

std::optional<common::imsi_t>
SimtelMme::findMTimsiInVlr(const common::imsi_t &imsi) const {
  auto found = vlr.findByImsi(imsi);
  if (!found) {
    return std::nullopt;
  }

  return found->mTimsi;
}

void SimtelMme::trySendReport(unsigned int smsId,
                              const common::imsi_t &imsi_s) {
  auto mtimsi_s = findMTimsiInVlr(imsi_s);
  if (!mtimsi_s) {
    MessageHolder::instance().addErrorMsg("Sender not known by MME");
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

  smsc.lock()->handleMtForwardSmAck(smsId, *mtimsi_s);

  auto ttlManager = smsc.lock()->getTtlManager(smsId, *mtimsi_s);
  if (!ttlManager) {
    MessageHolder::instance().addErrorMsg("SMS(" + uid.toStr() +
                                          ") not found in SMSC");
    return;
  }

  bool reportSent = false;
  while (!reportSent) {
    if (ttlManager->isActive() && ttlManager->isExpired()) {
      MessageHolder::instance().addErrorMsg("SMS(" + uid.toStr() +
                                            ") TTL expired");
      break;
    }

    auto warningSec = ttlManager->getWarningSec();
    if (warningSec) {
      MessageHolder::instance().addInfoMsg(
          "SMS(" + uid.toStr() + ") TTL: " + std::to_string(*warningSec) +
          " seconds left");
    }

    MessageHolder::instance().addMsg(
        createLogMsg("trying to send SM_Delivery_Report"));

    auto senderMmeId = reg->getMmeIdByImsi(imsi_s);
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

    std::this_thread::sleep_for(SendReportSleepTime);
  }
}

void SimtelMme::addOtherMme(std::shared_ptr<SimtelMme> mme) {
  otherMme.emplace(mme->getId(), mme);
}

void SimtelMme::addBs(std::shared_ptr<SimtelBaseStation> bs) {
  baseStations.emplace(bs->getId(), bs);
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
  common::imsi_t realImsi;
  bool isRealImsi = true;
  auto imsiInCurrent = findImsiInVlr(imsi);
  if (imsiInCurrent) {
    mmeId = id;
    realImsi = *imsiInCurrent;
    isRealImsi = false;
  } else {
    auto imsiInOther = findImsiInOther(imsi, mmeId);
    if (imsiInOther) {
      realImsi = *imsiInOther;
      isRealImsi = false;
    }
  }

  if (!isRealImsi) {
    common::imsi_t mTimsi = imsi;

    MessageHolder::instance().addMsg(
        createLogMsg("received imsi is m-timsi, real imsi is " + realImsi));

    if (mmeId != id) {
      auto regRecord = reg->handleAuthInfoRequest(realImsi, imei);
      if (!regRecord) {
        return std::unexpected(regRecord.error());
      }

      vlr.setRecord({mTimsi, realImsi, imei, regRecord->msisdn});
    }

    return mTimsi;
  } else {
    MessageHolder::instance().addMsg(
        createLogMsg("received imsi is real imsi"));

    if (vlr.getSize() >= maxVlrSize) {
      return std::unexpected("VLR cant accept more records");
    }

    common::imsi_t mTimsi = generateMTimsi();

    MessageHolder::instance().addMsg(
        createLogMsg("generated m-timsi: " + mTimsi));

    auto regRecord = reg->handleAuthInfoRequest(imsi, imei);
    if (!regRecord) {
      return std::unexpected(regRecord.error());
    }

    vlr.setRecord({mTimsi, imsi, imei, regRecord->msisdn});

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
    return "UE not known by MME";
  }

  auto realImsi = findImsiInVlr(mTimsi);
  if (!realImsi) {
    return "UE not known by MME";
  }

  auto result = reg->handleUpdateLocationRequest(*realImsi, id);
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

std::optional<std::string>
SimtelMme::handleSmSubmit(const common::imsi_t &mtimsi_s, unsigned int smsId,
                          std::string &ueErrorMsg) {
  auto imsi_s = findImsiInVlr(mtimsi_s);
  if (!imsi_s) {
    return "UE not known by MME";
  }

  auto hasBalanceValue = pcrf->hasEnoughBalanceForSms(*imsi_s);
  if (!hasBalanceValue) {
    return "UE not known by PCRF";
  }

  auto reserved = pcrf->reserveMoneyForSms(*imsi_s);
  if (!reserved) {
    return "UE not known by PCRF";
  }

  bool hasBalance = *hasBalanceValue;
  if (!hasBalance) {
    ueErrorMsg = "Not enough balance";
    return "Service not available for UE";
  }

  MessageHolder::instance().addMsg(createLogMsg("sent SM_Submit to SMSC"));

  auto ctxCreated = smsc.lock()->handleSmSubmit(mtimsi_s, smsId);
  if (!ctxCreated) {
    return "Failed to create SMS context";
  }

  return std::nullopt;
}

bool SimtelMme::handleMoForwardSM(const common::imsi_t &mtimsi_s,
                                  unsigned int smsId,
                                  const std::string &smsText) {
  MessageHolder::instance().addMsg(createLogMsg("sent MO_Forward_SM to SMSC"));
  return smsc.lock()->handleMoForwardSM(mtimsi_s, smsId, smsText);
}

std::optional<std::string>
SimtelMme::sendRoutingInfoSm(const common::msisdn_t &msisdn_d,
                             unsigned int smsId, const common::imsi_t &mtimsi_s,
                             std::string &ueErrorMsg) {
  auto senderImsi = findImsiInVlr(mtimsi_s);
  if (!senderImsi) {
    return "Sender not known by MME";
  }

  MessageHolder::instance().addMsg(
      createLogMsg("request to HLR/EIR: Routing_Info_SM(msisdn=" + msisdn_d) +
      ")");

  auto senderRecord = reg->handleRoutingInfoSmSender(*senderImsi);
  if (!senderRecord) {
    return senderRecord.error();
  }

  if (senderRecord->msisdn == msisdn_d) {
    return "SMS cant be sent to same MSISDN";
  }

  auto receiverRecord = reg->handleRoutingInfoSmReceiver(msisdn_d);
  if (!receiverRecord) {
    return receiverRecord.error();
  }

  if (receiverRecord->mmeId != id) {
    MessageHolder::instance().addMsg(createLogMsg("changing MME"));
    auto receiverMme = findOtherById(receiverRecord->mmeId);
    if (!receiverMme) {
      return "Receiver MME not found";
    }

    return receiverMme->sendForwardSm(senderRecord->msisdn, smsId, mtimsi_s,
                                      receiverRecord->imsi, ueErrorMsg);
  }

  return sendForwardSm(senderRecord->msisdn, smsId, mtimsi_s,
                       receiverRecord->imsi, ueErrorMsg);
}

std::optional<std::string>
SimtelMme::sendForwardSm(const common::msisdn_t &msisdn_s, unsigned int smsId,
                         const common::imsi_t &mtimsi_s,
                         const common::imsi_t &imsi_d,
                         std::string &ueErrorMsg) {
  auto mtimsi_d = findMTimsiInVlr(imsi_d);
  if (!mtimsi_d) {
    ueErrorMsg = "No subscriber with such MSISDN";
    return "Receiver not known by MME";
  }

  MessageHolder::instance().addMsg(
      createLogMsg("sent context update request to SMSC"));

  bool updated = smsc.lock()->updateMTimsiD(mtimsi_s, smsId, *mtimsi_d);
  if (!updated) {
    return "Error updating SMSC context";
  }

  auto receiverInfo = vlr.findByMTimsi(*mtimsi_d);
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
                        *mtimsi_d, *smsText, bs);
  smsSender.detach();

  return std::nullopt;
}

void SimtelMme::handleSmDeliveryAck(const common::msisdn_t &msisdn_s,
                                    unsigned int smsId,
                                    const common::imsi_t &mtimsi_d) {
  MessageHolder::instance().addMsg(createLogMsg("received SmDeliveryAck"));

  unsigned int senderMmeId;
  auto imsi_s = reg->getImsiByMsisdn(msisdn_s, senderMmeId);
  if (!imsi_s) {
    MessageHolder::instance().addErrorMsg("Sender record not found in HLR");
    return;
  }

  auto deducted = pcrf->deductReservedMoney(*imsi_s);
  if (!deducted) {
    MessageHolder::instance().addErrorMsg("UE not known by PCRF");
  }

  if (senderMmeId == id) {
    trySendReport(smsId, *imsi_s);
  } else {
    auto senderMme = findOtherById(senderMmeId);
    if (!senderMme) {
      MessageHolder::instance().addErrorMsg("unknown sender MME");
      return;
    }

    senderMme->trySendReport(smsId, *imsi_s);
  }
}

std::optional<std::string> SimtelMme::handleUssd(const common::imsi_t &mTimsi,
                                                 uint8_t code) {
  MessageHolder::instance().addMsg(
      createLogMsg("received USSD: " + std::to_string(code)));

  auto ussd = common::utils::ussdCodeFromNum(code);
  if (!ussd) {
    return "Unknown command";
  }

  auto ueInfo = vlr.findByMTimsi(mTimsi);
  if (!ueInfo) {
    return "Unknown UE";
  }

  auto bs = ueInfo->bs;
  if (!bs) {
    return "UE BS not found";
  }

  switch (*ussd) {
  case common::UssdCode::GetBalance: {
    auto balance = pcrf->getBalance(ueInfo->imsi);
    if (!balance) {
      return "UE not known by PCRF";
    }

    auto error = bs->sendUssdBalance(mTimsi, *balance);
    if (error) {
      MessageHolder::instance().addErrorMsg(*error);
    }

    return std::nullopt;
  }
  case common::UssdCode::GetPhoneNumber: {
    auto msisdn = reg->getMsisdnByImsi(ueInfo->imsi);
    if (!msisdn) {
      return "MSISDN not found in HLR";
    }

    auto error = bs->sendUssdMsisdn(mTimsi, *msisdn);
    if (error) {
      MessageHolder::instance().addErrorMsg(*error);
    }

    return std::nullopt;
  }
  default:
    return "Unknown command";
  }
}

bool SimtelMme::sendSmDeliveryReport(const common::msisdn_t &mtimsi_s,
                                     unsigned int smsId) {
  auto record = vlr.findByMTimsi(mtimsi_s);
  if (!record) {
    MessageHolder::instance().addErrorMsg("Sender not known by MME");
    return false;
  }

  auto senderBs = record->bs;

  auto error = senderBs->sendSmDeliveryReport(mtimsi_s, smsId);
  if (error) {
    return false;
  }

  return true;
}

void SimtelMme::sendSmDeliveryError(const common::msisdn_t &mtimsi_s,
                                    unsigned int smsId) {
  auto record = vlr.findByMTimsi(mtimsi_s);
  if (!record) {
    MessageHolder::instance().addErrorMsg("Sender not known by MME");
    return;
  }

  auto senderBs = record->bs;

  MessageHolder::instance().addMsg(createLogMsg("sending SmDeliveryError"));

  auto error = senderBs->sendSmDeliveryError(mtimsi_s, smsId, "");
  if (error) {
    MessageHolder::instance().addErrorMsg(*error);
  }
}

void SimtelMme::trySendSms(const common::msisdn_t &msisdn_s, unsigned int smsId,
                           const common::imsi_t &mtimsi_s,
                           const common::imsi_t &mtimsi_d,
                           const std::string &smsText,
                           std::shared_ptr<SimtelBaseStation> bs) {
  common::binary_t binaryText =
      common::BinarySerializer::strToBinaryUnsized(smsText);
  SmsUid uid = {mtimsi_s, smsId};

  auto ttlManager = smsc.lock()->getTtlManager(smsId, mtimsi_s);
  if (!ttlManager) {
    MessageHolder::instance().addErrorMsg("SMS(" + uid.toStr() +
                                          ") not found in SMSC");
    return;
  }

  ttlManager->start();

  bool delivered = false;
  while (true) {
    if (ttlManager->isActive() && ttlManager->isExpired()) {
      MessageHolder::instance().addErrorMsg("SMS(" + uid.toStr() +
                                            ") TTL expired");
      break;
    }

    auto warningSec = ttlManager->getWarningSec();
    if (warningSec) {
      MessageHolder::instance().addInfoMsg(
          "SMS(" + uid.toStr() + ") TTL: " + std::to_string(*warningSec) +
          " seconds left");
    }

    MessageHolder::instance().addMsg(
        createLogMsg("trying to send SM_Delivery"));

    bs->sendSmDelivery(mtimsi_d, smsId, msisdn_s, binaryText);

    std::this_thread::sleep_for(SendSmsSleepTime);

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
    auto imsi_s = findImsiInVlr(mtimsi_s);
    if (!imsi_s) {
      MessageHolder::instance().addErrorMsg("UE not known by MME");
      return;
    }

    auto returned = pcrf->returnReservedMoney(*imsi_s);
    if (!returned) {
      MessageHolder::instance().addErrorMsg("UE not known by PCRF");
    }

    sendSmDeliveryError(mtimsi_s, smsId);
  }
}

common::imsi_t SimtelMme::generateMTimsi() {
  curMTimsi++;
  if (curMTimsi > MaxMtimsi) {
    curMTimsi = 0;
  }

  return common::utils::imsiToStr(curMTimsi);
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
