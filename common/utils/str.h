#pragma once

#include <optional>
#include <string>

#include "num.h"

namespace common {
std::string lowercased(const std::string &str);
std::string uppercased(const std::string &str);
std::optional<bool> parseBool(const std::string &str);
bool hasDataAfterPos(const std::string &str, const std::streampos &pos);
bool allDigits(const std::string &str);
std::string ltrimmed(const std::string &str);

// итератор указывающий на число
// решение найдено здесь
// https://softwareengineering.stackexchange.com/questions/291803/template-function-passing-iterators
template <class Iterator,
          class U = typename std::iterator_traits<Iterator>::value_type>
  requires std::is_arithmetic_v<U>
std::string toStr(Iterator begin, Iterator end, int precision = 4,
                  char leftBorder = '[', char rightBorder = ']') {

  if (begin == end)
    return std::to_string(leftBorder) + std::to_string(rightBorder);

  std::string str(1, leftBorder);
  auto last = std::prev(end);

  for (auto it = begin; it != end; ++it) {
    if constexpr (std::is_floating_point_v<U>) {
      str += common::toStr<U>(*it, precision);
    } else {
      str += std::to_string(*it);
    }
    str += (it == last) ? std::string(1, rightBorder) : ", ";
  }
  return str;
}
} // namespace common
