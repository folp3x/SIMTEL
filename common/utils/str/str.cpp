#include "str.h"

#include <algorithm>
#include <ranges>

#include "common/types.h"

namespace common::utils {
std::string lowercased(std::string_view str) {
  std::string result{};
  result.resize(str.size());
  std::ranges::transform(str, result.begin(),
                         [](char ch) { return std::tolower(ch); });
  return result;
}

std::string uppercased(std::string_view str) {
  std::string result{};
  result.resize(str.size());
  std::ranges::transform(str, result.begin(),
                         [](char ch) { return std::toupper(ch); });
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
  return std::ranges::all_of(str, ::isdigit);
}

std::vector<std::string> split(const std::string &str) {
  std::vector<std::string> tokens{};
  std::string curToken = "";
  for (auto ch : str) {
    if (std::isspace(ch)) {
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
  return addLeadingZeroes(std::to_string(imsi), constants::ImsiDefaultLength);
}

std::string imeiToStr(uint64_t imei) {
  return addLeadingZeroes(std::to_string(imei), constants::ImeiDefaultLength);
}

std::optional<uint64_t> identifierFromStr(const std::string &str) {
  return toOptional(fromString<uint64_t>(str));
}

std::string firstWord(const std::string &str) {
  std::string curToken = "";
  for (auto ch : str) {
    if (std::isspace(ch)) {
      if (!curToken.empty()) {
        return curToken;
      }
    } else {
      curToken += ch;
    }
  }

  return curToken;
}

std::string ignoreWords(const std::string &str, size_t count) {
  if (count == 0) {
    return str;
  }

  size_t i = 0;
  size_t len = str.size();
  size_t wordCount = 0;

  while (wordCount < count) {
    while (i < len && std::isspace(str[i])) {
      i++;
    }
    if (i >= len) {
      break;
    }

    while (i < len && !std::isspace(str[i])) {
      i++;
    }
    wordCount++;
  }

  while (i < len && std::isspace(str[i])) {
    i++;
  }

  if (i >= len) {
    return "";
  }

  return str.substr(i);
}

std::string addLeadingZeroes(const std::string &str, size_t length) {
  return std::string(length - str.length(), '0') + str;
}
} // namespace common::utils
