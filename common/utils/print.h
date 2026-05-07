#pragma once

#include <rang.hpp>
#include <string>

namespace common {
void printColored(const std::string &output, rang::fg color);
void printlnColored(const std::string &output, rang::fg color);
} // namespace common
