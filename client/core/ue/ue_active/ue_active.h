#pragma once

#include <string>

namespace client {
inline std::string ueActiveToStr(bool inActive) {
  return inActive ? "active" : "not active";
}
} // namespace client
