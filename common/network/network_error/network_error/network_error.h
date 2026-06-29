#pragma once

#include <string>

#include "common/network/network_error/network_error_type/network_error_type.h"

namespace common {
struct NetworkError {
  NetworkErrorType type = NetworkErrorType::OTHER;
  std::string description = "";
};

inline bool isNoConnectedError(NetworkError error) {
  return error.type == NetworkErrorType::CONNECTION_CLOSED ||
         error.type == NetworkErrorType::CONNECTION_RESET ||
         error.type == NetworkErrorType::CONNECTION_CLOSED;
}
}; // namespace common
