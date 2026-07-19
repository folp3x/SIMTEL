#pragma once

#include <string>

namespace common::constants {
constexpr size_t LocationCoordsCount = 1;

constexpr size_t BytesInMb = 1024 * 1024;
constexpr unsigned int MsecInSec = 1000;

constexpr size_t SocketMessageHeaderBytes = 6;

constexpr unsigned int MaxSignalLevel = 100;

constexpr size_t ImeiDefaultLength = 15;
constexpr size_t ImsiDefaultLength = 15;

constexpr unsigned int RealNumPrecision = 4;
constexpr unsigned int PriceNumPrecision = 2;
} // namespace common::constants
