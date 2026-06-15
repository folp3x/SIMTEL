#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <rang.hpp>
#include <string>

namespace common {
inline void printColored(std::string_view output, rang::fg color,
                         std::string_view ending = "\n") {
  std::cout << color << output << rang::fg::reset << ending;
}

inline void printTime(std::chrono::system_clock::time_point time) {
  auto time_t = std::chrono::system_clock::to_time_t(time);
  std::tm *tm = std::localtime(&time_t);

  std::cout << std::put_time(tm, "%d-%m-%Y %H:%M:%S");
}
} // namespace common
