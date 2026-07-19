#pragma once

#include <optional>
#include <string>

namespace server {
enum class SubscriberStatus : uint8_t { Active, Banned };

std::optional<SubscriberStatus> subscriberStatusfromStr(const std::string &str);
std::string subscriberStatusToStr(SubscriberStatus status);
} // namespace server
