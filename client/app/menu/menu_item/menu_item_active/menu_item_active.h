#pragma once

#include "common/app/menu/menu_item/menu_item/menu_item.h"

namespace client {
class MenuItemActive : public common::MenuItem {
private:
  const bool active = false;

public:
  explicit MenuItemActive(bool active_);

  virtual std::string_view getName() const override;

  bool getActive() const;

  static size_t getArgsCount();
};
} // namespace client
