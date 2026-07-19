#pragma once

#include "common/app/menu/menu_item/menu_item/menu_item.h"

namespace client {
class MenuItemProtocol : public common::MenuItem {
private:
  const std::string protocol = "";

public:
  explicit MenuItemProtocol(const std::string &protocol_);

  virtual std::string_view getName() const override;

  std::string getProtocol() const;

  static size_t getArgsCount();
};

} // namespace client
