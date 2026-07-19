#pragma once

namespace common {
template <typename T>
std::string Validator::isPositiveNumber(T number, const std::string &name) {
  return (number > 0) ? "" : name + " must be a positive number";
}
} // namespace common
