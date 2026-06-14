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
                               const common::imei_t &imei, unsigned int bsId) {
  auto record = vlr.getRecord(imsi);
  if (!record) {
    auto mTimsi = generateMTimsi();
    vlr.setRecord(mTimsi, imei, "", bsId);
    return mTimsi;
  }

  return record->mTimsi;
}

common::imsi_t SimtelMme::generateMTimsi() {
  curMTimsi++;
  if (curMTimsi > MAX_MTIMSI) {
    curMTimsi = 0;
  }

  return common::imsiToStr(curMTimsi);
}
} // namespace server
