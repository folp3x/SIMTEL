#ifndef STR_H
#define STR_H

#include <optional>
#include <string>

std::string lowercase(const std::string &str);
std::optional<bool> parseBool(const std::string &str);
bool hasDataAfterPos(const std::string &str, const std::streampos &pos);

#endif // STR_H
