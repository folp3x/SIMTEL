#pragma once

#include <array>
#include <concepts>
#include <string>
#include <vector>

#include "constants.h"

namespace common {
using imei_t = std::string;
using imsi_t = std::string;
using msisdn_t = std::string;

template <typename T = float, size_t S = constants::LocationCoordsCount>
  requires std::is_arithmetic_v<T>
using coords_t = std::array<T, S>;

using binary_t = std::vector<std::byte>;
} // namespace common
