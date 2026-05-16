#pragma once

#include <stdexcept>

namespace common {
template <typename T>
  requires std::is_floating_point_v<T>
std::string toStr(T num, std::optional<int> precision_) {
  if (!precision_) {
    return std::to_string(num);
  } else {
    int precision = *precision_;

    if (precision < 0) {
      throw std::invalid_argument("precision_ must be > 0");
    }

    std::string formatStr = "%." + std::to_string(precision) + "f";
    size_t size = snprintf(nullptr, 0, formatStr.c_str(), num) + 1;
    std::string buf(size, '\0');
    snprintf(buf.data(), size, formatStr.c_str(), num);
    // удаление лишнего \0
    buf.pop_back();

    // удаление нулей в конце дробной части
    size_t lastNonZeroInd = buf.find_last_not_of('0');
    if (lastNonZeroInd != std::string::npos && buf[lastNonZeroInd] == '.') {
      // в дробной части только нули
      buf.erase(lastNonZeroInd);
    } else if (lastNonZeroInd != std::string::npos) {
      buf.erase(lastNonZeroInd + 1);
    } else {
      buf = "";
    }

    return buf;
  }
}

template <class Iterator, class U>
  requires std::is_arithmetic_v<U>
std::string toStr(Iterator begin, Iterator end, std::optional<int> precision,
                  char leftBorder, char rightBorder) {
  if (begin == end) {
    return std::to_string(leftBorder) + std::to_string(rightBorder);
  }

  std::string str(1, leftBorder);
  auto last = std::prev(end);

  for (auto it = begin; it != end; ++it) {
    if constexpr (std::is_floating_point_v<U>) {
      str += common::toStr<U>(*it, precision);
    } else {
      str += std::to_string(*it);
    }

    if (it != last) {
      str += ", ";
    }
  }
  str += std::string(1, rightBorder);

  return str;
}

template <typename T>
  requires std::is_arithmetic_v<T>
std::expected<T, std::string> fromString(const std::string &str) {
  try {
    if constexpr (std::is_unsigned_v<T>)
      return std::stoull(str);
    else if constexpr (std::is_integral_v<T>)
      return std::stoll(str);
    else
      return stold(str);
  } catch (std::invalid_argument &e) {
    return std::unexpected("not a valid number");
  } catch (std::out_of_range &e) {
    return std::unexpected("value out of range");
  }
}
} // namespace common
