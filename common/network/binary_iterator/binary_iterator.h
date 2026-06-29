#pragma once

#include <optional>

#include "common/types.h"

namespace common {
class BinaryIterator {
private:
  common::binary_t binary;
  binary_t::iterator it;

public:
  explicit BinaryIterator(const common::binary_t binary_);

  std::optional<common::binary_t> getNext(size_t bytes);

  std::optional<common::binary_t> getRemaining();
};
} // namespace common
