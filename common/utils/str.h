#pragma once

#include <optional>
#include <string>

namespace common {
std::string lowercased(const std::string &str);
std::string uppercased(const std::string &str);
std::optional<bool> parseBool(const std::string &str);
bool hasDataAfterPos(const std::string &str, const std::streampos &pos);
bool allDigits(const std::string &str);
std::string ltrimmed(const std::string &str);
} // namespace common
