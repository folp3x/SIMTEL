#pragma once

#include "common/app/menu/menu_item/menu_item/menu_item.h"

namespace client {
class MenuItemUssdCode : public common::MenuItem {
private:
  const unsigned int code = 0;

public:
  explicit MenuItemUssdCode(unsigned int code_);

  virtual std::string_view getName() const override;

  unsigned int getCode() const;
};
} // namespace client
