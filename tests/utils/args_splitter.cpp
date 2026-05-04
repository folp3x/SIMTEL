#include "args_splitter.h"

#include <sstream>

// разделение аргументов командной строки на пробелу
void ArgsSplitter::split(const std::string &argsStr) {
  std::istringstream stream(argsStr);
  std::string arg;

  int argIndex = 0;
  while (stream >> arg) {
    argsValues.push_back(arg);
  }

  for (auto &val : argsValues) {
    args.push_back(const_cast<char *>(val.c_str()));
  }
}

ArgsSplitter::ArgsSplitter(const std::string &argsStr) { split(argsStr); }

int ArgsSplitter::argc() { return args.size(); }

char **ArgsSplitter::argv() { return args.data(); }
