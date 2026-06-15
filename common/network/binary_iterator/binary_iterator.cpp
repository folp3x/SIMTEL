#include "binary_iterator.h"

namespace common {
BinaryIterator::BinaryIterator(const common::binary_t binary_)
    : binary(binary_), it(binary.begin()) {}

std::optional<common::binary_t> BinaryIterator::getNext(size_t bytes) {
  auto last = it + bytes;
  if (last > binary.end()) {
    return std::nullopt;
  }

  auto result = common::binary_t(it, last);
  it = last;
  return result;
}

std::optional<common::binary_t> BinaryIterator::getRemaining() {
  if (it == binary.end()) {
    return std::nullopt;
  }

  auto result = common::binary_t(it, binary.end());
  it = binary.end();
  return result;
}
} // namespace common
