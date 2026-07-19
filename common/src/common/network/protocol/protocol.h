#pragma once

#include <optional>
#include <string>

#include "common/utils/hash/hash.h"

namespace common {
enum class Protocol : size_t {
  Json = utils::getHash("json"),
  Binary = utils::getHash("binary"),
};

std::optional<std::string> protocolNameFromAlias(std::string_view alias);

std::string protocolToStr(Protocol protocol);
std::optional<Protocol> protocolFromStr(std::string_view str);
bool isCorrectProtocolStr(std::string_view str);
std::optional<uint8_t> protocolToNetworkId(Protocol protocol);
std::optional<Protocol> protocolFromNetworkId(uint8_t networkId);
} // namespace common
