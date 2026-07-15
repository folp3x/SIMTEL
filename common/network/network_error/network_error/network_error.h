#pragma once

#include <string>

#include "common/network/network_error/network_error_type/network_error_type.h"

namespace common {
struct NetworkError {
  NetworkErrorType type = NetworkErrorType::Other;
  std::string description = "";
};
}; // namespace common
