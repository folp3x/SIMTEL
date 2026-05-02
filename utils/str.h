#pragma once

#include <optional>
#include <string>

std::string lowercase(const std::string &str);
std::string uppercase(const std::string &str);
std::optional<bool> parseBool(const std::string &str);
bool hasDataAfterPos(const std::string &str, const std::streampos &pos);
bool allDigits(const std::string &str);
