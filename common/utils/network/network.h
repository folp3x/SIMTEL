#pragma once

#include <cstdint>
#include <expected>
#include <string>

#include "common/types.h"

namespace common {
std::expected<uint64_t, std::string> parseIp(const std::string &ip);
std::expected<uint16_t, std::string> parsePort(const std::string &port);

std::string toStr(const binary_t &binary);

template <typename... Binaries>
binary_t mergeBinary(const Binaries &...binaries);
} // namespace common

#include "network_impl.h"
