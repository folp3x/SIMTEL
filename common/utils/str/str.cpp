#include "str.h"

#include <algorithm>
#include <sstream>

namespace common {
// возвращает строку в нижнем регистре
std::string lowercased(std::string_view str) {
  std::string result{};
  result.resize(str.size());
  std::transform(str.begin(), str.end(), result.begin(),
                 [](char c) { return std::tolower(c); });
  return result;
}

// возвращает строку в верхнем регистре
std::string uppercased(std::string_view str) {
  std::string result{};
  result.resize(str.size());
  std::transform(str.begin(), str.end(), result.begin(),
                 [](char c) { return std::toupper(c); });
  return result;
}

// преобразует строку с булевым значением в bool
std::optional<bool> parseBool(std::string_view str) {
  std::string strLower = lowercased(str);
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
bool allDigits(std::string_view str) {
  return std::all_of(str.begin(), str.end(), ::isdigit);
}

// возвращает строку с убранными слева непечатными символами
std::string ltrimmed(std::string_view str) {
  auto it = std::find_if(str.begin(), str.end(), isprint);

  return std::string(it, str.end());
}
} // namespace common
