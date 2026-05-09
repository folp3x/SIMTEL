#pragma once

#include <array>
#include <string>

#include "constants.h"

namespace common {
using imei_t = std::string;
using imsi_t = std::string;

template <typename T>
using coords_t = std::array<T, common::constants::LOCATION_COORDS_COUNT>;
} // namespace common
