#include "message_holder.h"

namespace server {
void MessageHolder::addMsg(const std::string &content,
                           common::MenuMessageType type) {
  {
    std::lock_guard lock(messagesMtx);
    messages.push({content, type});
  }
  messagesCv.notify_one();
}

void MessageHolder::addInfoMsg(const std::string &content) {
  addMsg(content, common::MenuMessageType::Info);
}

void MessageHolder::addErrorMsg(const std::string &content) {
  addMsg(content, common::MenuMessageType::Error);
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
} // namespace server
