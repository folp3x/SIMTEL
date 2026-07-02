#pragma once

#include <string>

namespace client {
struct UssdInfo {
  unsigned int code = 0;
  std::string description = "";
};
} // namespace client
