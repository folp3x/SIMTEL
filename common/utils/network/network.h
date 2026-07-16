#pragma once

#include <expected>

#include "common/network/network_error/network_error/network_error.h"
#include "common/types.h"

namespace common::utils {
std::expected<uint64_t, std::string> parseIp(const std::string &ip);
std::expected<uint16_t, std::string> parsePort(const std::string &port);

template <typename... Binaries>
binary_t mergeBinary(const Binaries &...binaries);

inline bool isNoConnectedError(NetworkError error);
} // namespace common::utils

#include "network_impl.h"
