#pragma once

#include "common/app/menu/menu_item/menu_item/menu_item.h"

namespace common {
class MenuItemExit : public MenuItem {
public:
  virtual std::string_view getName() const override;

  static size_t getArgsCount();
};
} // namespace common
