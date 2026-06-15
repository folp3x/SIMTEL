#pragma once

#include <expected>
#include <memory>

#include "common/types.h"
#include "server/app/config/mme_config/mme_config.h"
#include "server/core/simtel/simtel_register/simtel_register.h"
#include "server/core/simtel/simtel_smsc/simtel_smsc.h"
#include "server/core/simtel/simtel_visitor_list/simtel_visitor_list.h"

namespace server {
class SimtelMme {
private:
  static constexpr uint64_t MAX_MTIMSI = 999'999'999'999'999;

  const unsigned int id = 0;
  const unsigned int maxVlrSize = 0;

  uint64_t curMTimsi = 0;

  SimtelVisitorList vlr{};

  std::shared_ptr<SimtelRegister> hlr;
  SimtelSmsc *smsc;
  std::unordered_map<unsigned int, std::shared_ptr<SimtelBaseStation>>
      baseStations;

  common::imsi_t generateMTimsi();

  std::shared_ptr<SimtelBaseStation> findBsById(unsigned int id) const;

  std::string createLogMsg(const std::string &content) const;

public:
  SimtelMme(const MmeConfig &config, std::shared_ptr<SimtelRegister> hlr_,
            SimtelSmsc *smsc_);

  void addBs(std::shared_ptr<SimtelBaseStation> bs);

  unsigned int getId() const;

  std::expected<common::imsi_t, std::string>
  handleAttachRequest(const common::imsi_t &imsi, const common::imei_t &imei);

  std::optional<std::string> handleAuthResponse(const common::imsi_t &mTimsi,
                                                unsigned int bsId);
};
} // namespace server
