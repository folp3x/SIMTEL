#pragma once

namespace common {
template <typename... Binaries>
common::binary_t mergeBinary(const Binaries &...binaries) {
  common::binary_t result;
  size_t totalSize = (binaries.size() + ...);
  result.reserve(totalSize);
  (result.insert(result.end(), binaries.begin(), binaries.end()), ...);

  return result;
}
} // namespace common
