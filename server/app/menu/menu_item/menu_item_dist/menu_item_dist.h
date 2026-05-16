#pragma once

#include "common/app/menu/menu_item/menu_item/menu_item.h"

#include <vector>

namespace server {
template <typename T = float> class MenuItemDist : public common::MenuItem {
private:
  const std::vector<T> coords = {};

  void logConstructor(const std::string &constructorType,
                      const std::vector<T> &coords) const;

public:
  explicit MenuItemDist(const std::vector<T> &coords_);

  MenuItemDist(const MenuItemDist &other);
  MenuItemDist(MenuItemDist &&other) noexcept;

  virtual std::string_view getName() const override;

  std::vector<T> getCoords() const;
  static int getArgsCount();
};
} // namespace server

#include "menu_item_dist_impl.h"
