#pragma once

#include <string>

#include "common/core/ussd/ussd_code.h"

namespace client {
struct UssdInfo {
  common::UssdCode code = common::UssdCode::MAX_VALUE;
  std::string description = "";
};
} // namespace client
