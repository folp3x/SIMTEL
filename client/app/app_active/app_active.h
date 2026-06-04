#pragma once

#include <string>

inline std::string appActiveToStr(bool inActive) {
  return inActive ? "active" : "not active";
}
