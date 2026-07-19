#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>

namespace common::utils {
inline std::string formatTime(std::chrono::system_clock::time_point time) {
  auto time_t = std::chrono::system_clock::to_time_t(time);
  std::tm *tm = std::localtime(&time_t);

  std::ostringstream stream;
  stream << std::put_time(tm, "%d.%m.%Y %H:%M:%S");
  return stream.str();
}

inline auto getNowSeconds() {
  return std::chrono::time_point_cast<std::chrono::seconds>(
      std::chrono::system_clock::now());
}
} // namespace common::utils
