#pragma once

#include "common/network/protocol/protocol.h"

namespace common {
struct Request {
  common::Protocol protocol = common::Protocol::JSON;
};
} // namespace common
