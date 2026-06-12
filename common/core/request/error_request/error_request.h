#pragma once

#include <string>

#include "common/core/request/request/request.h"

namespace common {
struct ErrorRequest : Request {
  std::string description = "";

  ErrorRequest(const std::string &description_) : description(description_) {}

  std::string toStr() { return "{description=" + description + "}"; }
};
} // namespace common
