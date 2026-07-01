#pragma once

#include <string>

namespace common::constants {
constexpr size_t LOCATION_COORDS_COUNT = 1;
constexpr size_t BYTES_IN_MB = 1024 * 1024;
constexpr size_t SOCKET_MESSAGE_HEADER_BYTES = 6;
constexpr unsigned int MAX_SIGNAL_LEVEL = 100;
constexpr size_t IMEI_BINARY_BYTES = 8;
constexpr size_t IMSI_BINARY_BYTES = 8;
constexpr size_t IMEI_DEFAULT_LENGTH = 15;
constexpr size_t IMSI_DEFAULT_LENGTH = 15;
constexpr size_t MSISDN_BINARY_BYTES = 8;
constexpr unsigned int MSEC_IN_SEC = 1000;
} // namespace common::constants
