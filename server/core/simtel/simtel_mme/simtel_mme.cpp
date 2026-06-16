#include "simtel_mme.h"

#include "common/network/binary_serializer/binary_serializer.h"
#include "common/utils/str/str.h"
#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

#include <iostream>

namespace server {
SimtelMme::SimtelMme(const MmeConfig &config,
                     std::shared_ptr<SimtelRegister> hlr_, SimtelSmsc *smsc_)
    : id(config.id), maxVlrSize(config.maxVlrSize), hlr(hlr_), smsc(smsc_),
      vlr(id) {}

std::optional<common::imsi_t>
SimtelMme::findImsiInHlr(const common::imsi_t &mTimsi) const {
  auto imsi = hlr->getImsiByMTimsi(mTimsi);
  if (!imsi) {
    return std::nullopt;
  }
  return *imsi;
}

void SimtelMme::addOtherMme(std::shared_ptr<SimtelMme> mme) {
  otherMme.insert({mme->getId(), mme});
}

void SimtelMme::addBs(std::shared_ptr<SimtelBaseStation> bs) {
  baseStations.insert({bs->getId(), bs});
}

unsigned int SimtelMme::getId() const { return id; }

void SimtelMme::removeFromVlr(const common::imsi_t &imsi) {
  vlr.removeRecord(imsi);
}

std::optional<common::imsi_t>
SimtelMme::getImsiFromOther(const common::imsi_t &mTimsi) const {
  for (const auto &mme : otherMme) {
    auto found = mme.second->findImsiInHlr(mTimsi);
    if (found) {
      return *found;
    }
  }
  return std::nullopt;
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
  } else {
    // запрос постоянного IMSI у других MME
    auto foundInOther = getImsiFromOther(imsi);
    if (foundInOther) {
      realImsi = *foundInOther;
    }
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

    vlr.setRecord({realImsi, imei, hlrRecord->msisdn, nullptr});
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
  auto imsi = hlr->getImsiByMTimsi(mTimsi);
  if (!imsi) {
    return "IMSI not found in HLR";
  }

  auto bs = findBsById(bsId);
  if (!bs) {
    return "BS not known by MME";
  }

  bool changed = vlr.changePath(*imsi, bs);
  if (!changed) {
    return "m-timsi not found in VLR";
  }

  MessageHolder::instance().addMsg(
      createLogMsg("Searching for real IMSI in VLR"));

  MessageHolder::instance().addMsg(createLogMsg("Updating mmeId in HLR"));

  auto result = hlr->handleUpdateLocationRequest(*imsi, id);
  if (!result) {
    return result.error();
  }

  auto prevMmeId = *result;
  if (prevMmeId) {
    auto prevMme = findOtherById(*prevMmeId);
    if (prevMme) {
      prevMme->removeFromVlr(*imsi);
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
  MessageHolder::instance().addMsg(createLogMsg("searching for IMSI in VLR"));

  auto senderImsi = findImsiInHlr(mtimsi_s);
  if (!senderImsi) {
    return "Sender IMSI not found in VLR";
  }

  MessageHolder::instance().addMsg(
      createLogMsg("sending Routing_Info_SM to SMSC"));

  auto senderRecord = hlr->handleRoutingInfoSmReceiver(*senderImsi);
  if (!senderRecord) {
    return senderRecord.error();
  }

  if (senderRecord->msisdn == msisdn_d) {
    return "SMS cant be sent to same MSISDN";
  }

  auto receiverRecord = hlr->handleRoutingInfoSmSender(msisdn_d);
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

  auto imsi = findImsiInHlr(mtimsi_d);
  if (!imsi) {
    std::cout << mtimsi_d << std::endl;
    return "Receiver IMSI not found in HLR";
  }

  auto receiverInfo = vlr.findByImsi(*imsi);
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

  bool bufAquired = bs->handleForwardSmReq(mtimsi_d, binary.size());
  if (!bufAquired) {
    return "Failed to aquire buf for SMS text on BS";
  }

  bool bufFilled = bs->handleMtForwardSm(mtimsi_d, binary);
  if (!bufFilled) {
    return "Failed to copy SMS text to BS buf";
  }

  auto prepareError = bs->prepareSmDelivery(mtimsi_d, smsId, msisdn_s);
  if (prepareError) {
    return *prepareError;
  }

  auto sendError = bs->sendSmDelivery(mtimsi_d);
  if (sendError) {
    return *sendError;
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
