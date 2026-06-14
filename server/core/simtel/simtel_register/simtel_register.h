#pragma once

#include <string>

namespace server {
class SimtelRegister {
private:
  std::string hlrSqliteFilePath = "";

public:
  explicit SimtelRegister(const std::string &hlrSqliteFilePath_);
};
} // namespace server
