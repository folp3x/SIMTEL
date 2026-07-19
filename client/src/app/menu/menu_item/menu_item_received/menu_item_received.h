#pragma once

#include "common/app/menu/menu_item/menu_item/menu_item.h"

namespace client {
class MenuItemReceived : public common::MenuItem {
public:
  virtual std::string_view getName() const override;
};
} // namespace client
