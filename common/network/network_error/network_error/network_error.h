#pragma once

#include <string>

#include "common/network/network_error/network_error_type/network_error_type.h"

namespace common {
struct NetworkError {
  NetworkErrorType type = NetworkErrorType::Other;
  std::string description = "";
};

inline bool isNoConnectedError(NetworkError error) {
  return error.type == NetworkErrorType::ConnectionClosed ||
         error.type == NetworkErrorType::ConnectionReset ||
         error.type == NetworkErrorType::BadFileDescriptor;
}
}; // namespace common
