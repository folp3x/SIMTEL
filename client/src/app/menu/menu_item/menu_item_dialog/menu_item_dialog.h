#pragma once

#include "common/app/menu/menu_item/menu_item/menu_item.h"

#include "common/types.h"
#include "constants.h"

namespace client {
class MenuItemDialog : public common::MenuItem {
private:
  common::msisdn_t msisdn = "";
  char speedDialNum = constants::EmptySpeedDialNum;

public:
  virtual std::string_view getName() const override;

  common::msisdn_t getMsisdn() const;
  void setMsisdn(const common::msisdn_t &msisdn_);

  char getSpeedDialNum() const;
  void setSpeedDialNum(char speedDialNum_);

  static size_t getArgsCount();
};
} // namespace client
