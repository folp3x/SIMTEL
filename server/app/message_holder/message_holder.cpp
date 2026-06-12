#include "message_holder.h"

namespace server {
void MessageHolder::put(const common::MenuMessage &msg) {
  std::lock_guard lock(messagesMtx);
  messages.push(msg);
  messagesCv.notify_one();
}

std::optional<common::MenuMessage> MessageHolder::take() {
  std::lock_guard lock(messagesMtx);
  if (messages.empty()) {
    return std::nullopt;
  }

  auto copy = messages.front();
  messages.pop();
  return copy;
}

void MessageHolder::waitForMessages() {
  std::unique_lock lock(messagesMtx);
  messagesCv.wait(lock, [this] { return !messages.empty(); });
}
} // namespace server
