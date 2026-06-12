#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

#include "common/app/menu/menu_message/menu_message.h"

namespace server {
class MessageHolder {
private:
  std::mutex messagesMtx;
  std::condition_variable messagesCv;
  std::queue<common::MenuMessage> messages{};

public:
  void addMsg(const std::string &content,
              common::MenuMessageType type = common::MenuMessageType::NOCOLOR);

  std::optional<common::MenuMessage> takeMsg();

  void waitForMessages();
};
} // namespace server
