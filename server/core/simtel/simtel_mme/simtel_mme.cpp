#include "simtel_mme.h"

#include "common/utils/str/str.h"

namespace server {
SimtelMme::SimtelMme(const MmeConfig &config,
                     std::shared_ptr<SimtelRegister> hlr_,
                     std::shared_ptr<SimtelSmsc> smsc_)
    : id(config.id), maxVlrSize(config.maxVlrSize), hlr(hlr_), smsc(smsc_) {}

unsigned int SimtelMme::getId() const { return id; }

std::expected<common::imsi_t, std::string>
SimtelMme::handleAttachRequest(const common::imsi_t &imsi,
                               const common::imei_t &imei) {
  auto record = vlr.getRecord(imsi);
  if (!record) {
    auto mTimsi = generateMTimsi();
    vlr.setRecord(mTimsi, imei, "", std::nullopt);
    return mTimsi;
  }

  return record->mTimsi;
}

std::optional<std::string>
SimtelMme::handleAuthRequest(const common::imsi_t &mTimsi, unsigned int bsId) {
  if (vlr.changePath(mTimsi, bsId)) {
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
} // namespace server
