#pragma once

#include "common/app/menu/menu_item/menu_item/menu_item.h"

#include "common/types.h"
#include "constants.h"

namespace client {
class MenuItemSMS : public common::MenuItem {
private:
  common::msisdn_t msisdn = "";
  std::string content = "";
  char speedDialNum = constants::EmptySpeedDialNum;

public:
  virtual std::string_view getName() const override;

  common::msisdn_t getMsisdn() const;
  void setMsisdn(const common::msisdn_t &msisdn_);

  std::string getContent() const;
  void setContent(const std::string &content_);

  char getSpeedDialNum() const;
  void setSpeedDialNum(char speedDialNum_);
};
} // namespace client
