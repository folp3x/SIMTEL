#pragma once

#include <string>

namespace common::constants {
constexpr size_t LocationCoordsCount = 1;

constexpr size_t BytesInMb = 1024 * 1024;
constexpr unsigned int MsecInSec = 1000;

constexpr size_t SocketMessageHeaderBytes = 6;

constexpr unsigned int MaxSignalLevel = 100;

constexpr size_t ImeiBinaryBytes = 8;
constexpr size_t ImsiBinaryBytes = 8;
constexpr size_t MsisdnBinaryBytes = 8;

constexpr size_t ImeiDefaultLength = 15;
constexpr size_t ImsiDefaultLength = 15;
} // namespace common::constants
