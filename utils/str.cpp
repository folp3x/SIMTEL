#include "str.h"

#include <algorithm>
#include <sstream>

// возвращает строку в нижнем регистре
std::string lowercase(const std::string &str) {
  std::string strCopy = str;
  for (auto &ch : strCopy) {
    ch = std::tolower(ch);
  }

  return strCopy;
}

// возвращает строку в верхнем регистре
std::string uppercase(const std::string &str) {
  std::string strCopy = str;
  for (auto &ch : strCopy) {
    ch = std::toupper(ch);
  }

  return strCopy;
}

// преобразует строку с булевым значением в bool
std::optional<bool> parseBool(const std::string &str) {
  std::string strLower = lowercase(str);
  if (strLower == "true" || strLower == "1") {
    return true;
  } else if (strLower == "false" || strLower == "0") {
    return false;
  } else {
    return std::nullopt;
  }
}

// проверяет есть ли данные в потоке после позиции pos
bool hasDataAfterPos(const std::string &str, const std::streampos &pos) {
  std::istringstream streamCopy(str);
  streamCopy.seekg(pos);
  streamCopy >> std::ws;

  return streamCopy.peek() != EOF;
}

// проверяет что все символы строки - цифры
bool allDigits(const std::string &str) {
  return std::all_of(str.begin(), str.end(), ::isdigit);
}
