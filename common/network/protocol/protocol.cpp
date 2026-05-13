#include "protocol.h"

#include <algorithm>
#include <unordered_map>

namespace common {
using ProtocolInfoMap = std::unordered_map<Protocol, std::string_view>;
using ProtocolAliasMap = std::unordered_map<std::string_view, std::string_view>;

static const ProtocolInfoMap PROTOCOLS = {{Protocol::BINARY, "binary"},
                                          {Protocol::JSON, "json"}};

static const ProtocolAliasMap ALIASES = {{"b", "binary"}, {"j", "json"}};

auto findProtocolByName(std::string_view name) {
  size_t hash = getHash(name);
  auto it = PROTOCOLS.find(static_cast<Protocol>(hash));
  return it;
}

std::optional<std::string_view>
protocolNameFromAlias(std::string_view alias) {
  auto it = ALIASES.find(alias);
  if (it != ALIASES.end()) {
    return it->second;
  }
  return std::nullopt;
}

std::string_view protocolToStr(Protocol p) {
  auto it = PROTOCOLS.find(p);
  return (it == PROTOCOLS.end()) ? "unknown" : it->second;
}

std::optional<Protocol> protocolFromStr(std::string_view str) {
  auto it = findProtocolByName(str);
  if (it != PROTOCOLS.end()) {
    return it->first;
  }
  return std::nullopt;
}

bool isCorrectProtocolStr(std::string_view str) {
  auto it = findProtocolByName(str);
  return it != PROTOCOLS.end();
}
} // namespace common
