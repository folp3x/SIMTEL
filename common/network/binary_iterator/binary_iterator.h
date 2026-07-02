#pragma once

#include <optional>

#include "common/types.h"

namespace common {
class BinaryIterator {
private:
  binary_t binary;
  binary_t::iterator it;

public:
  explicit BinaryIterator(const binary_t binary_);

  std::optional<binary_t> getNext(size_t bytes);

  bool skip(size_t bytes);

  std::optional<binary_t> getRemaining();
};
} // namespace common
