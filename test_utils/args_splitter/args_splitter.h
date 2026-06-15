#pragma once

#include <string>
#include <vector>

class ArgsSplitter {
private:
  std::vector<std::string> argsValues = {};
  std::vector<char *> args = {};

  void split(const std::string &argsStr);

public:
  explicit ArgsSplitter(const std::string &argsStr);

  int argc() const;
  char **argv() const;
};
