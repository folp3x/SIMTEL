#pragma once

#include "common/app/menu/menu/menu.h"

namespace server {
class Menu : public common::Menu {
public:
  void showStatus() const;
};
} // namespace server
