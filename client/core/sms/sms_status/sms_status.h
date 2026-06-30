#pragma once

#include <rang.hpp>
#include <string>

namespace client {
enum class SmsStatus : uint8_t { PENDING, DELIVERED, NOT_DELIVERED };
} // namespace client
