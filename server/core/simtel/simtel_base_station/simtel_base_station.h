#pragma once

#include "common/types.h"

namespace server {
class SimtelBaseStation {
private:
  common::binary_t buf = {};

public:
  void put(const common::binary_t &data);
  common::binary_t get() const;
};
} // namespace server
