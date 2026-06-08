#pragma once

#include "common/core/request/request_type/request_type.h"
#include "common/network/protocol/protocol.h"

namespace common {
struct Request {
  Protocol protocol = Protocol::JSON;
  RequestType type = RequestType::Location_Update;
};
} // namespace common
