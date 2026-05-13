#pragma once

#include "common/app/menu/menu_item/menu_item/menu_item.h"

#include <vector>

namespace server {
class MenuItemDist : public common::MenuItem {
private:
  const std::vector<float> coords = {};

  void logConstructor(const std::string &constructorType,
                      const std::vector<float> &coords) const;

public:
  explicit MenuItemDist(const std::vector<float> &coords_);

  MenuItemDist(const MenuItemDist &other);
  MenuItemDist(MenuItemDist &&other) noexcept;

  virtual std::string_view getName() const override;
  std::vector<float> getCoords() const;
  static int getArgsCount();
};
} // namespace server
