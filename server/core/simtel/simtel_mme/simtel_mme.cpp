#include "simtel_mme.h"

#include "common/utils/str/str.h"
#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
SimtelMme::SimtelMme(const MmeConfig &config,
                     std::shared_ptr<SimtelRegister> hlr_, SimtelSmsc *smsc_)
    : id(config.id), maxVlrSize(config.maxVlrSize), hlr(hlr_), smsc(smsc_) {}

void SimtelMme::addBs(std::shared_ptr<SimtelBaseStation> bs) {
  baseStations.insert({bs->getId(), bs});
}

unsigned int SimtelMme::getId() const { return id; }

std::expected<common::imsi_t, std::string>
SimtelMme::handleAttachRequest(const common::imsi_t &imsi,
                               const common::imei_t &imei) {
  MessageHolder::instance().addMsg(
      createLogMsg("received Attach{imsi=" + imsi + ", imei=" + imei + "}"));

  common::imsi_t realImsi = imsi;
  auto found = vlr.getImsiByMTimsi(imsi);
  if (found) {
    realImsi = *found;
  }

  auto error = hlr->handleAuthInfoRequest(realImsi, imei);
  if (error) {
    return std::unexpected(*error);
  }

  if (!found) {
    if (vlr.getSize() >= maxVlrSize) {
      return "VLR cant accept more records";
    }

    auto mTimsi = generateMTimsi();
    vlr.setRecord({mTimsi, imsi, imei, "msisdn", nullptr});
    return mTimsi;
  }

  // если клиент уже зарегистрирован и прислал свой m-timsi
  return imsi;
}

std::optional<std::string>
SimtelMme::handleAuthResponse(const common::imsi_t &mTimsi, unsigned int bsId) {
  MessageHolder::instance().addMsg(
      createLogMsg("received AuthResponse{mTmsi=" + mTimsi +
                   ", bsId=" + std::to_string(bsId) + "}"));

  auto bs = findBsById(bsId);
  if (!bs) {
    return "BS not known by MME";
  }

  if (vlr.changePath(mTimsi, bs)) {
    auto imsi = vlr.getImsiByMTimsi(mTimsi);
    if (!imsi) {
      return "IMSI not found in VLR";
    }

    return hlr->handleUpdateLocationRequest(*imsi, id);
  }

  return "m-timsi not found in VLR";
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

  MessageHolder::instance().addMsg("generated T-IMSI: " +
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
