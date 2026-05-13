#include "str.h"

#include <algorithm>

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

bool allDigits(std::string_view str) {
  return std::all_of(str.begin(), str.end(), ::isdigit);
}

// возвращает строку с убранными слева непечатными символами
std::string ltrimmed(std::string_view str) {
  auto it = std::find_if(str.begin(), str.end(), isprint);

  return std::string(it, str.end());
}

std::vector<std::string> split(const std::string &str) {
  std::vector<std::string> tokens = {};
  std::string curToken = "";
  for (auto ch : str) {
    if (isspace(ch)) {
      if (!curToken.empty()) {
        tokens.push_back(curToken);
        curToken.clear();
      }
    } else {
      curToken += ch;
    }
  }

  // добавление последнего токена
  if (!curToken.empty())
    tokens.push_back(curToken);

  return tokens;
}
} // namespace common
