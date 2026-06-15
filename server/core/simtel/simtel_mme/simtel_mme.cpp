#include "simtel_mme.h"

#include "common/utils/str/str.h"
#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
SimtelMme::SimtelMme(const MmeConfig &config,
                     std::shared_ptr<SimtelRegister> hlr_,
                     std::shared_ptr<SimtelSmsc> smsc_)
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

  auto record = vlr.getRecord(imsi);
  if (!record) {
    auto mTimsi = generateMTimsi();
    vlr.setRecord({mTimsi, imei, "msisdn", nullptr});
    return mTimsi;
  }

  return record->mTimsi;
}

std::optional<std::string>
SimtelMme::handleAuthRequest(const common::imsi_t &mTimsi, unsigned int bsId) {
  MessageHolder::instance().addMsg(
      createLogMsg("received Auth{mTmsi=" + mTimsi +
                   ", bsId=" + std::to_string(bsId) + "}"));

  auto bs = findBsById(bsId);
  if (!bs) {
    return "BS not known by MME";
  }

  if (vlr.changePath(mTimsi, bs)) {
    return std::nullopt;
  }

  return "m-timsi not found in VLR";
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
  return "MME_" + std::to_string(id) + " " + content;
}
} // namespace server
