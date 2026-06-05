#pragma once

#include <string>

namespace client {
class Validator {
private:
public:
  static std::string
  isCorrectSpeedDialNumStr(const std::string &speedDialNumStr);
};
} // namespace client
