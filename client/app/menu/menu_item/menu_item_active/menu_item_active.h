#pragma once

#include "common/app/menu/menu_item/menu_item/menu_item.h"

namespace client {
class MenuItemActive : public common::MenuItem {
private:
  const bool active = false;

  void logConstructor(const std::string &constructorType, bool active) const;

public:
  explicit MenuItemActive(bool active_);

  MenuItemActive(const MenuItemActive &other);
  MenuItemActive(MenuItemActive &&other) noexcept;

  virtual std::string_view getName() const override;
  bool getActive() const;
  static int getArgsCount();
};
} // namespace client
