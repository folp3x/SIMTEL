#include "message_holder.h"

namespace server {
void MessageHolder::addMsg(const std::string &content,
                           common::MenuMessageType type) {
  std::lock_guard lock(messagesMtx);
  messages.push({content, type});
  messagesCv.notify_one();
}

std::optional<common::MenuMessage> MessageHolder::takeMsg() {
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
