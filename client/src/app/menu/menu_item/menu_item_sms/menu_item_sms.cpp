#include "menu_item_sms.h"

namespace client {
std::string_view MenuItemSMS::getName() const { return "sms"; }

common::msisdn_t MenuItemSMS::getMsisdn() const { return msisdn; }

void MenuItemSMS::setMsisdn(const common::msisdn_t &msisdn_) {
  msisdn = msisdn_;
}

std::string MenuItemSMS::getContent() const { return content; }

void MenuItemSMS::setContent(const std::string &content_) {
  content = content_;
}

char MenuItemSMS::getSpeedDialNum() const { return speedDialNum; }

void MenuItemSMS::setSpeedDialNum(char speedDialNum_) {
  speedDialNum = speedDialNum_;
}
} // namespace client
