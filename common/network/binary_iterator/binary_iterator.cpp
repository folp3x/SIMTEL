#include "binary_iterator.h"

namespace common {
BinaryIterator::BinaryIterator(const binary_t binary_)
    : binary(binary_), it(binary.begin()) {}

std::optional<binary_t> BinaryIterator::getNext(size_t bytes) {
  auto last = it + bytes;
  if (last > binary.end()) {
    return std::nullopt;
  }

  auto result = binary_t(it, last);
  it = last;
  return result;
}

bool BinaryIterator::skip(size_t bytes) {
  auto last = it + bytes;
  if (last > binary.end()) {
    return false;
  }
  it = last;
  return true;
}

std::optional<binary_t> BinaryIterator::getRemaining() {
  if (it == binary.end()) {
    return std::nullopt;
  }

  auto result = binary_t(it, binary.end());
  it = binary.end();
  return result;
}
} // namespace common
