#include "args_splitter.h"

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

  if (!curArg.empty()) {
    argsValues.push_back(curArg);
  }

  for (const auto &val : argsValues) {
    args.push_back(val.c_str());
  }
}

ArgsSplitter::ArgsSplitter(const std::string &argsStr) { split(argsStr); }

int ArgsSplitter::argc() const { return args.size(); }

char **ArgsSplitter::argv() const { return const_cast<char **>(args.data()); }
