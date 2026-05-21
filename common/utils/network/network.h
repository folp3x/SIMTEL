#pragma once

#include <cstdint>
#include <expected>
#include <string>

#include "common/types.h"

namespace common {
std::expected<uint64_t, std::string> parseIP(const std::string &ip);
std::expected<uint16_t, std::string> parsePort(const std::string &port);

std::string toStr(const binary_t &binary);
} // namespace common
