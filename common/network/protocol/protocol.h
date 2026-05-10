#pragma once

#include <optional>
#include <string>

#include "common/utils/hash/hash.h"

namespace common {
enum class Protocol : size_t {
  JSON = getHash("json"),
  BINARY = getHash("binary"),
};

static auto findProtocolByName(std::string_view name);

std::string_view protocolToStr(Protocol protocol);
std::optional<Protocol> protocolFromStr(std::string_view str);
bool isCorrectProtocolStr(std::string_view str);
} // namespace common
