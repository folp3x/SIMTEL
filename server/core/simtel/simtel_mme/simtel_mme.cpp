#include "simtel_mme.h"

#include "common/network/binary_serializer/binary_serializer.h"
#include "common/utils/str/str.h"
#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
SimtelMme::SimtelMme(const MmeConfig &config,
                     std::shared_ptr<SimtelRegister> hlr_, SimtelSmsc *smsc_)
    : id(config.id), maxVlrSize(config.maxVlrSize), hlr(hlr_), smsc(smsc_),
      vlr(id) {}

std::optional<common::imsi_t>
SimtelMme::findImsiInHlr(const common::imsi_t &mTimsi) const {
  auto imsi = hlr->getImsiByMTimsi(mTimsi, id);
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

  common::imsi_t realImsi = imsi;
  auto found = findImsiInHlr(imsi);
  if (found) {
    MessageHolder::instance().addMsg(
        createLogMsg("Client sended m-timsi is not real imsi"));
    realImsi = *found;
  }

  if (!found) {
    MessageHolder::instance().addMsg(
        createLogMsg("Client sended m-timsi is real imsi"));

    if (vlr.getSize() >= maxVlrSize) {
      return "VLR cant accept more records";
    }

    auto mTimsi = generateMTimsi();

    auto hlrRecord = hlr->handleAuthInfoRequest(realImsi, imei, mTimsi);
    if (!hlrRecord) {
      return std::unexpected(hlrRecord.error());
    }

    vlr.setRecord({mTimsi, realImsi, imei, hlrRecord->msisdn, nullptr});
    return mTimsi;
  }

  // если клиент уже зарегистрирован и прислал свой m-timsi
  return imsi;
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

  MessageHolder::instance().addMsg(
      createLogMsg("Searching for real IMSI in VLR"));

  MessageHolder::instance().addMsg(createLogMsg("Updating mmeId in HLR"));

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
  MessageHolder::instance().addMsg(createLogMsg("sending SM_Submit to SMSC"));
  return smsc->handleSmSubmit(mtimsi_s, smsId);
}

bool SimtelMme::handleMoForwardSM(const common::imsi_t &mtimsi_s,
                                  unsigned int smsId,
                                  const std::string &smsText) {
  MessageHolder::instance().addMsg(
      createLogMsg("sending MO_Forward_SM to SMSC"));
  return smsc->handleMoForwardSM(mtimsi_s, smsId, smsText);
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
      createLogMsg("sending Routing_Info_SM to SMSC"));

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

  if (!receiverRecord->mmeId) {
    return "Unknown receiver MME id";
  }

  if (!receiverRecord->mTimsi) {
    return "Unknown receiver m-timsi";
  }

  unsigned int receiverMmeId = *receiverRecord->mmeId;
  if (receiverMmeId != id) {
    MessageHolder::instance().addMsg(createLogMsg("changing MME"));
    auto receiverMme = findOtherById(receiverMmeId);
    if (!receiverMme) {
      return "Receiver MME not found";
    }

    return receiverMme->handleChangeAfterSriSm(
        senderRecord->msisdn, smsId, mtimsi_s, *(receiverRecord->mTimsi));
  }

  return handleChangeAfterSriSm(senderRecord->msisdn, smsId, mtimsi_s,
                                *(receiverRecord->mTimsi));
}

std::optional<std::string> SimtelMme::handleChangeAfterSriSm(
    const common::msisdn_t &msisdn_s, unsigned int smsId,
    const common::imsi_t &mtimsi_s, const common::imsi_t &mtimsi_d) {
  MessageHolder::instance().addMsg(createLogMsg("updating SMSC context"));

  bool updated = smsc->updateContextMTimsiD(mtimsi_s, smsId, mtimsi_d);
  if (!updated) {
    return "Error updating SMSC context";
  }

  auto receiverInfo = vlr.findByMTimsi(mtimsi_d);
  if (!receiverInfo) {
    return "Receiver info not found in VLR";
  }

  auto bs = receiverInfo->bs;
  if (!bs) {
    return "Reiver BS not found";
  }

  MessageHolder::instance().addMsg(
      createLogMsg("receiving SMS text from SMSC"));
  auto smsText = smsc->getSmsText(smsId, mtimsi_s);
  if (!smsText) {
    return "SMS text not found in SMSC";
  }
  MessageHolder::instance().addMsg(
      createLogMsg("received SMS text with " + std::to_string(smsText->size()) +
                   " characters"));

  common::binary_t binary = common::BinarySerializer::strToBinary(*smsText);

  unsigned int smsTtlSec = smsc->getSmsTtlMs() / common::constants::MSEC_IN_SEC;
  unsigned int warningPeriodSec = 1;
  TtlManager ttlManager{smsTtlSec, warningPeriodSec};
  ttlManager.update();
  ttlManager.setActive(true);

  MessageHolder::instance().addMsg(createLogMsg("trying to send SM_Delivery"));

  while (true) {
    if (ttlManager.isActive() && ttlManager.isExpired()) {
      MessageHolder::instance().addErrorMsg("SMS TTL expired");
      break;
    }

    auto warningSec = ttlManager.getWarningSec();
    if (warningSec) {
      MessageHolder::instance().addMsg(
          "SMS(mtimsi_s=" + mtimsi_s + ", id=" + std::to_string(smsId) +
              ") TTL: " + std::to_string(*warningSec) + " seconds left",
          common::MenuMessageType::INFO);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(SEND_SMS_SLEEP_MS));

    bool bufAquired = bs->handleForwardSmReq(mtimsi_d, binary.size());
    if (!bufAquired) {
      continue;
    }

    bool bufFilled = bs->handleMtForwardSm(mtimsi_d, binary);
    if (!bufFilled) {
      continue;
    }

    auto preparedReq = bs->prepareSmDelivery(mtimsi_d, smsId, msisdn_s);
    if (!preparedReq) {
      continue;
    }

    bool ueFound = true;
    bs->sendSmDelivery(mtimsi_d, *preparedReq, ueFound);
    if (!ueFound) {
      return "UE not found by BS";
    }

    auto req = bs->receiveSmDeliveryAck(mtimsi_d, ueFound);
    if (!ueFound) {
      return "UE not found by BS";
    }

    if (!req) {
      continue;
    } else {
      break;
    }
  }

  return std::nullopt;
}

common::imsi_t SimtelMme::generateMTimsi() {
  curMTimsi++;
  if (curMTimsi > MAX_MTIMSI) {
    curMTimsi = 0;
  }

  MessageHolder::instance().addMsg("Generated m-timsi: " +
                                   common::imsiToStr(curMTimsi));

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
