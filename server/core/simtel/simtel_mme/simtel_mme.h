#pragma once

#include <memory>

#include "server/app/config/mme_config/mme_config.h"
#include "server/core/simtel/simtel_register/simtel_register.h"
#include "server/core/simtel/simtel_smsc/simtel_smsc.h"
#include "server/core/simtel/simtel_visitor_list/simtel_visitor_list.h"

namespace server {
class SimtelMme {
private:
  const unsigned int id = 0;
  const unsigned int maxVlrSize = 0;

  SimtelVisitorList vlr{};

  std::shared_ptr<SimtelRegister> hlr;
  std::shared_ptr<SimtelSmsc> smsc;

public:
  SimtelMme(const MmeConfig &config, std::shared_ptr<SimtelRegister> hlr_,
            std::shared_ptr<SimtelSmsc> smsc_);

  unsigned int getId() const;
};
} // namespace server
