#pragma once

#include <array>
#include <concepts>
#include <string>
#include <vector>

#include "common/constants.h"

namespace common {
using imei_t = std::string;
using imsi_t = std::string;

template <typename T = float,
          size_t S = common::constants::LOCATION_COORDS_COUNT>
  requires std::is_arithmetic_v<T>
using coords_t = std::array<T, S>;

using binary_t = std::vector<uint8_t>;
} // namespace common
