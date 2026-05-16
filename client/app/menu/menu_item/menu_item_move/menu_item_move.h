#pragma once

#include "common/app/menu/menu_item/menu_item/menu_item.h"

#include <vector>

namespace client {

template <typename T = float>
  requires std::is_arithmetic_v<T>
class MenuItemMove : public common::MenuItem {
private:
  const std::vector<T> coords = {};

  void logConstructor(const std::string &constructorType,
                      const std::vector<T> &coords) const;

public:
  explicit MenuItemMove(const std::vector<T> &coords_);

  MenuItemMove(const MenuItemMove &other);
  MenuItemMove(MenuItemMove &&other) noexcept;

  virtual std::string_view getName() const override;

  std::vector<T> getCoords() const;
  static int getArgsCount();
};
} // namespace client

#include "menu_item_move_impl.h"
