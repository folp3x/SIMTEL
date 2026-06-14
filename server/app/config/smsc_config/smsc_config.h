#pragma once

#include <string>

namespace server {
struct SmscConfig {
  unsigned int smsTtlMs = 0;
  std::string cdrJsonFilePath = "";
};
} // namespace server
