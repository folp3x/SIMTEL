#include "validator.h"

namespace client {
std::string
Validator::isCorrectSpeedDialNumStr(const std::string &speedDialNumStr) {
  if (speedDialNumStr.length() != 1) {
    return "Speed dial number must contain only 1 digit";
  }
  return "";
}
} // namespace client
