#include "validator.h"

namespace client {
std::string
Validator::isCorrectSpeedDialNumStr(const std::string &speedDialNumStr) {
  if (speedDialNumStr.size() != 1) {
    return "Speed dial number must contain only 1 digit";
  }
  return "";
}
} // namespace client
