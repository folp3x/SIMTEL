#pragma once

#include <frozen/string.h>
#include <string>

namespace common::utils {
inline constexpr size_t getHash(std::string_view str) {
  return frozen::elsa<frozen::string>{}(frozen::string(str));
}
} // namespace common::utils
