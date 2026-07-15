#include "protocol.h"

#include <algorithm>
#include <unordered_map>

namespace common {
struct ProtocolInfo {
  std::string name;
  uint8_t networkId;
};

const std::unordered_map<Protocol, ProtocolInfo> protocols = {
    {Protocol::Json, {"json", 1}}, {Protocol::Binary, {"binary", 0}}};

const std::unordered_map<std::string_view, std::string> aliases = {
    {"b", "binary"}, {"j", "json"}};

auto findProtocolByName(std::string_view name) {
  size_t hash = utils::getHash(name);
  auto it = protocols.find(static_cast<Protocol>(hash));
  return it;
}

std::optional<std::string> protocolNameFromAlias(std::string_view alias) {
  auto it = aliases.find(alias);
  if (it != aliases.end()) {
    return it->second;
  }
  return std::nullopt;
}

std::string protocolToStr(Protocol p) {
  auto it = protocols.find(p);
  return (it == protocols.end()) ? "unknown" : it->second.name;
}

std::optional<Protocol> protocolFromStr(std::string_view str) {
  auto it = findProtocolByName(str);
  if (it != protocols.end()) {
    return it->first;
  }
  return std::nullopt;
}

bool isCorrectProtocolStr(std::string_view str) {
  auto it = findProtocolByName(str);
  return it != protocols.end();
}

std::optional<uint8_t> protocolToNetworkId(Protocol p) {
  auto it = protocols.find(p);
  if (it == protocols.end()) {
    return std::nullopt;
  }
  return it->second.networkId;
}

std::optional<Protocol> protocolFromNetworkId(uint8_t networkId) {
  for (const auto &[protocol, info] : protocols) {
    if (info.networkId == networkId) {
      return protocol;
    }
  }
  return std::nullopt;
}
} // namespace common
