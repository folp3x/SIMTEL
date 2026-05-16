#pragma once

#include "common/app/menu/menu_item/menu_item/menu_item.h"

namespace common {
class MenuItemInvalid : public common::MenuItem {
private:
  const std::string error = "";

  void logConstructor(const std::string &constructorType,
                      const std::string &error) const;

public:
  explicit MenuItemInvalid(const std::string &error_);

  MenuItemInvalid(const MenuItemInvalid &other);
  MenuItemInvalid(MenuItemInvalid &&other) noexcept;

  std::string_view getName() const override;

  std::string getError() const;
};
} // namespace common
