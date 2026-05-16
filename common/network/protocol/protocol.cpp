#include "protocol.h"

#include <algorithm>
#include <unordered_map>

namespace common {
using ProtocolInfoMap = std::unordered_map<Protocol, ProtocolInfo>;
using ProtocolAliasMap = std::unordered_map<std::string_view, std::string>;

static const ProtocolInfoMap PROTOCOLS = {{Protocol::JSON, {"json", 1}},
                                          {Protocol::BINARY, {"binary", 0}}};

static const ProtocolAliasMap ALIASES = {{"b", "binary"}, {"j", "json"}};

auto findProtocolByName(std::string_view name) {
  size_t hash = getHash(name);
  auto it = PROTOCOLS.find(static_cast<Protocol>(hash));
  return it;
}

std::optional<std::string> protocolNameFromAlias(std::string_view alias) {
  auto it = ALIASES.find(alias);
  if (it != ALIASES.end()) {
    return it->second;
  }
  return std::nullopt;
}

std::string protocolToStr(Protocol p) {
  auto it = PROTOCOLS.find(p);
  return (it == PROTOCOLS.end()) ? "unknown" : it->second.name;
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

std::optional<uint8_t> protocolToNetworkId(Protocol p) {
  auto it = PROTOCOLS.find(p);
  if (it == PROTOCOLS.end()) {
    return std::nullopt;
  }

  return it->second.networkId;
}

std::optional<Protocol> protocolFromNetworkId(uint8_t networkId) {
  for (const auto &[protocol, info] : PROTOCOLS) {
    if (info.networkId == networkId) {
      return protocol;
    }
  }
  return std::nullopt;
}
} // namespace common
