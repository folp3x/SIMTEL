#include "str.h"

#include <algorithm>

#include "common/types.h"

namespace common {
std::string lowercased(std::string_view str) {
  std::string result{};
  result.resize(str.size());
  std::transform(str.begin(), str.end(), result.begin(),
                 [](char c) { return std::tolower(c); });
  return result;
}

std::string uppercased(std::string_view str) {
  std::string result{};
  result.resize(str.size());
  std::transform(str.begin(), str.end(), result.begin(),
                 [](char c) { return std::toupper(c); });
  return result;
}

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

  if (!curToken.empty()) {
    tokens.push_back(curToken);
  }

  return tokens;
}

std::string imsiToStr(uint64_t imsi) {
  imsi_t str = std::to_string(imsi);
  size_t lenDiff = common::constants::IMSI_DEFAULT_LENGTH - str.length();
  if (lenDiff > 0) {
    return std::string(lenDiff, '0') + str;
  }
  return str;
}
} // namespace common
