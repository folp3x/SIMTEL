#include "simtel_mme.h"

#include "common/utils/str/str.h"
#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
SimtelMme::SimtelMme(const MmeConfig &config,
                     std::shared_ptr<SimtelRegister> hlr_, SimtelSmsc *smsc_)
    : id(config.id), maxVlrSize(config.maxVlrSize), hlr(hlr_), smsc(smsc_),
      vlr(id) {}

std::optional<common::imsi_t>
SimtelMme::findImsiInVlr(const common::imsi_t &mTimsi) const {
  return vlr.getImsiByMTimsi(mTimsi);
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

std::optional<common::imsi_t>
SimtelMme::getImsiFromOther(const common::imsi_t &mTimsi) const {
  for (const auto &mme : otherMme) {
    auto found = mme.second->findImsiInVlr(mTimsi);
    if (found) {
      return *found;
    }
  }
  return std::nullopt;
}

std::expected<common::imsi_t, std::string>
SimtelMme::handleAttachRequest(const common::imsi_t &imsi,
                               const common::imei_t &imei) {
  MessageHolder::instance().addMsg(
      createLogMsg("received Attach{imsi=" + imsi + ", imei=" + imei + "}"));

  common::imsi_t realImsi = imsi;
  auto found = vlr.getImsiByMTimsi(imsi);
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

  auto hlrRecord = hlr->handleAuthInfoRequest(realImsi, imei);
  if (!hlrRecord) {
    return std::unexpected(hlrRecord.error());
  }

  if (!found) {
    MessageHolder::instance().addMsg(
        createLogMsg("Client sended m-timsi is real imsi"));

    if (vlr.getSize() >= maxVlrSize) {
      return "VLR cant accept more records";
    }

    auto mTimsi = generateMTimsi();
    vlr.setRecord({mTimsi, realImsi, imei, hlrRecord->msisdn, nullptr});
    return mTimsi;
  }

  // если клиент уже зарегистрирован и прислал свой m-timsi
  return imsi;
}

std::optional<std::string>
SimtelMme::handleAuthResponse(const common::imsi_t &mTimsi, unsigned int bsId) {
  MessageHolder::instance().addMsg(
      createLogMsg("received AuthResponse{mTimsi=" + mTimsi +
                   ", bsId=" + std::to_string(bsId) + "}"));

  auto bs = findBsById(bsId);
  if (!bs) {
    return "BS not known by MME";
  }

  bool changed = vlr.changePath(mTimsi, bs);
  if (!changed) {
    return "m-timsi not found in VLR";
  }

  MessageHolder::instance().addMsg(
      createLogMsg("Searching for real IMSI in VLR"));

  auto imsi = vlr.getImsiByMTimsi(mTimsi);
  if (!imsi) {
    return "IMSI not found in VLR";
  }

  MessageHolder::instance().addMsg(createLogMsg("Updating mmeId in HLR"));

  auto result = hlr->handleUpdateLocationRequest(*imsi, id);
  if (!result) {
    return result.error();
  }

  auto prevMmeId = *result;
  if (prevMmeId) {
    auto prevMme = findOtherById(*prevMmeId);
    if (prevMme) {
      auto ptr = *prevMme;
      ptr->removeFromVlr(mTimsi);
    }
  }

  return std::nullopt;
}

std::optional<std::shared_ptr<SimtelMme>>
SimtelMme::findOtherById(unsigned int id) {
  auto it = otherMme.find(id);
  if (it == otherMme.end()) {
    return std::nullopt;
  }
  return it->second;
}

void SimtelMme::handleSmSubmit(const common::imsi_t &mTimsi,
                               unsigned int smsId) {
  smsc->handleSmSubmit(mTimsi, smsId);
}

bool SimtelMme::handleMoForwardSM(const common::imsi_t &mTimsi,
                                  unsigned int smsId,
                                  const std::string &smsText) {
  return smsc->handleMoForwardSM(mTimsi, smsId, smsText);
}

void SimtelMme::sendRoutingInfoSm(const common::msisdn_t &msisdn_d) {
  auto record = hlr->handleRoutingInfoSM(msisdn_d);
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
