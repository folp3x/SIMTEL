#include "args_splitter.h"

#include <sstream>

// разделение аргументов командной строки на пробелу
void ArgsSplitter::split(const std::string &argsStr) {
  std::string curArg = "";
  for (auto ch : argsStr) {
    if (isspace(ch)) {
      if (!curArg.empty()) {
        argsValues.push_back(curArg);
        curArg.clear();
      }
    } else {
      curArg += ch;
    }
  }

  // добавление последнего аргумента
  if (!curArg.empty())
    argsValues.push_back(curArg);

  for (auto &val : argsValues) {
    args.push_back(const_cast<char *>(val.c_str()));
  }
}

ArgsSplitter::ArgsSplitter(const std::string &argsStr) { split(argsStr); }

int ArgsSplitter::argc() { return args.size(); }

char **ArgsSplitter::argv() { return args.data(); }
