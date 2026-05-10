#pragma once

#include <frozen/string.h>
#include <string>

inline constexpr size_t getHash(std::string_view str) {
  return frozen::elsa<frozen::string>{}(frozen::string(str));
}
