#include "subscriber_status.h"

#include <unordered_map>

namespace server {
static std::unordered_map<std::string, SubscriberStatus> statuses = {
    {"active", SubscriberStatus::ACTIVE}, {"banned", SubscriberStatus::BANNED}};

std::optional<SubscriberStatus>
subscriberStatusfromStr(const std::string &str) {
  auto it = statuses.find(str);
  if (it == statuses.end()) {
    return std::nullopt;
  }
  return it->second;
}

std::string subscriberStatusToStr(SubscriberStatus status) {
  for (const auto &[name, curStatus] : statuses) {
    if (curStatus == status) {
      return name;
    }
  }
  return "unknown";
}

} // namespace server
