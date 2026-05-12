#pragma once

#include "common/app/menu/menu_item/menu_item/menu_item.h"

#include <vector>

namespace client {
class MenuItemProtocol : public common::MenuItem {
private:
  const std::string protocol = "";

  void logConstructor(const std::string &constructorType,
                      const std::string &protocol) const;

public:
  explicit MenuItemProtocol(const std::string &protocol_);

  MenuItemProtocol(const MenuItemProtocol &other);
  MenuItemProtocol(MenuItemProtocol &&other) noexcept;

  virtual std::string_view getName() const override;
  std::string getProtocol() const;
  static int getArgsCount();
};

} // namespace client
