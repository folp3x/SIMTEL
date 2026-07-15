#pragma once

#include <string>

namespace client {
enum class SmsStatus : uint8_t { Pending, Delivered, NotDelivered };
} // namespace client
