#pragma once

#include "common/app/menu/menu_item/menu_item/menu_item.h"

#include "common/types.h"

namespace client {
class MenuItemSent : public common::MenuItem {
public:
  virtual std::string_view getName() const override;
};
} // namespace client
