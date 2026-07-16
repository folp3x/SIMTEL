#pragma once

namespace common::utils {
template <typename... Binaries>
binary_t mergeBinary(const Binaries &...binaries) {
  binary_t result;
  size_t totalSize = (binaries.size() + ...);
  result.reserve(totalSize);
  (result.insert(result.end(), binaries.begin(), binaries.end()), ...);

  return result;
}

bool isNoConnectedError(NetworkError error) {
  return error.type == NetworkErrorType::ConnectionClosed ||
         error.type == NetworkErrorType::ConnectionReset ||
         error.type == NetworkErrorType::BadFileDescriptor;
}
} // namespace common::utils
