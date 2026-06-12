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
  void put(const common::MenuMessage &msg);

  std::optional<common::MenuMessage> take();

  void waitForMessages();
};
} // namespace server
