#include "menu_item_dialog.h"

namespace client {
std::string_view MenuItemDialog::getName() const { return "sms"; }

common::msisdn_t MenuItemDialog::getMsisdn() const { return msisdn; }

void MenuItemDialog::setMsisdn(const common::msisdn_t &msisdn_) {
  msisdn = msisdn_;
}

char MenuItemDialog::getSpeedDialNum() const { return speedDialNum; }

void MenuItemDialog::setSpeedDialNum(char speedDialNum_) {
  speedDialNum = speedDialNum_;
}

size_t MenuItemDialog::getArgsCount() { return 1; }
} // namespace client
