#pragma once

#include "common/validator/validator.h"

namespace server {
class Validator : common::Validator {
public:
  static std::string isCorrectNodesPath(const std::string &filePath);
};
} // namespace server
