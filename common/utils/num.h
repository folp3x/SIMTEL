#pragma once

#include <concepts>
#include <stdexcept>
#include <string>

namespace common {
template <typename T>
  requires std::is_floating_point_v<T>
std::string toStr(T num, int precision = -1) {
  if (precision == -1) {
    return std::to_string(num);
  } else if (precision >= 0) {
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
  throw std::invalid_argument(
      "precision must be > 0 or -1 for default precision");
}
} // namespace common
