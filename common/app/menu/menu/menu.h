#pragma once

#include <queue>
#include <string>

#include "common/app/menu/menu_message/menu_message.h"

namespace common {
// базовый класс для вывода меню
class Menu {
private:
  static constexpr int MENU_HEADER_LINE_LENGTH = 60;

public:
  virtual ~Menu() = default;

  inline void showMenuHeaderLine() const;

  inline void showMessage(const MenuMessage &msg) const;
  void showMessages(std::priority_queue<MenuMessage> &messages) const;
};
} // namespace common

#include "menu_impl.h"
