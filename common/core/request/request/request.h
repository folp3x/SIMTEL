#pragma once

#include "common/core/request/request_type/request_type.h"

namespace common {
struct Request {
  virtual ~Request() = default;
};
} // namespace common
