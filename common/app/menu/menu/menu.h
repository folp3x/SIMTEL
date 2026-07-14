#pragma once

#include <queue>

#include "common/app/menu/menu_message/menu_message.h"
#include "common/utils/print/print.h"

namespace common {
class Menu {
protected:
  static constexpr size_t MenuHeaderLineLength = 60;

public:
  virtual ~Menu() = default;

  inline void showMenuHeaderLine() const;

  inline void showMessage(const MenuMessage &msg) const;
  void showMessages(std::queue<MenuMessage> &messages) const;
};
} // namespace common

#include "menu_item_impl.h"
