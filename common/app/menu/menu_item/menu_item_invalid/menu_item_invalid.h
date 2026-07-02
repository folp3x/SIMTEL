#pragma once

#include "common/app/menu/menu_item/menu_item/menu_item.h"

namespace common {
class MenuItemInvalid : public MenuItem {
private:
  const std::string error = "";

public:
  explicit MenuItemInvalid(const std::string &error_);

  std::string_view getName() const override;

  std::string getError() const;
};
} // namespace common
