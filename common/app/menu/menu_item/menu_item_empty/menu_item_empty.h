#pragma once

#include "common/app/menu/menu_item/menu_item/menu_item.h"

namespace common {
class MenuItemEmpty : public MenuItem {
public:
  virtual std::string_view getName() const override;
};
} // namespace common
