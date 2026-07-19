#pragma once

#include "common/types.h"

namespace server {
struct BsConfig {
  unsigned int id = 0;
  unsigned int mmeId = 0;
  float radius = 0;
  common::coords_t<> loc{};
  unsigned int maxConnections = 0;
};
} // namespace server
