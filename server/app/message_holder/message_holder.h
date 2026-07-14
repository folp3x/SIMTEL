#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

#include "common/app/menu/menu_message/menu_message.h"

namespace server {
class MessageHolder {
private:
  bool waiting = false;
  std::mutex messagesMtx;
  std::condition_variable messagesCv;
  std::queue<common::MenuMessage> messages{};

  MessageHolder() = default;

public:
  static MessageHolder &instance() {
    static MessageHolder holder;
    return holder;
  }

  void addMsg(const std::string &content,
              common::MenuMessageType type = common::MenuMessageType::NoColor);

  void addInfoMsg(const std::string &content);

  void addErrorMsg(const std::string &content);

  std::optional<common::MenuMessage> takeMsg();
};
} // namespace server
