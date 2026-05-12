#pragma once

#include "common/app/menu/menu_item/menu_item/menu_item.h"

#include <vector>

namespace client {
class MenuItemMove : public common::MenuItem {
private:
  const std::vector<float> coords = {};

  void logConstructor(const std::string &constructorType,
                      const std::vector<float> &coords) const;

public:
  explicit MenuItemMove(const std::vector<float> &coords_);

  MenuItemMove(const MenuItemMove &other);
  MenuItemMove(MenuItemMove &&other) noexcept;

  virtual std::string_view getName() const override;
  std::vector<float> getCoords() const;
  static int getArgsCount();
};
} // namespace client
