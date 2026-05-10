#include "protocol.h"

#include <algorithm>
#include <unordered_map>

namespace common {
using ProtocolInfoMap = std::unordered_map<Protocol, std::string_view>;

static const ProtocolInfoMap PROTOCOLS = {{Protocol::BINARY, "binary"},
                                          {Protocol::JSON, "json"}};

auto findProtocolByName(std::string_view name) {
  size_t hash = getHash(name);
  auto it = PROTOCOLS.find(static_cast<Protocol>(hash));
  return it;
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
