#pragma once

#include <array>
#include <concepts>
#include <string>

#include "common/constants/constants.h"

namespace common {
using imei_t = std::string;
using imsi_t = std::string;

template <typename T>
  requires std::is_arithmetic_v<T>
using coords_t = std::array<T, common::constants::LOCATION_COORDS_COUNT>;
} // namespace common
