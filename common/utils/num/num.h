#pragma once

#include <concepts>
#include <optional>
#include <stdexcept>
#include <string>

namespace common {
template <typename T>
  requires std::is_floating_point_v<T>
std::string toStr(T num, std::optional<int> precision_ = std::nullopt) {
  if (!precision_) {
    return std::to_string(num);
  } else {
    int precision = *precision_;

    if (precision < 0)
      throw std::invalid_argument("precision_ must be > 0");

    size_t size =
        snprintf(nullptr, 0, ("%." + std::to_string(precision) + "f").c_str(),
                 num) +
        1;
    std::string buf(size, '\0');
    snprintf(buf.data(), size, ("%." + std::to_string(precision) + "f").c_str(),
             num);
    // удаление лишнего \0
    buf.pop_back();

    // удаление нулей в конце дробной части
    size_t lastNonZeroInd = buf.find_last_not_of('0');
    if (lastNonZeroInd != std::string::npos && buf[lastNonZeroInd] == '.')
      // в дробной части только нули
      buf.erase(lastNonZeroInd);
    else if (lastNonZeroInd != std::string::npos)
      buf.erase(lastNonZeroInd + 1);
    else
      buf = "";

    return buf;
  }
}
} // namespace common
