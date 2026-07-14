#include "socket_message_header.h"

#include <arpa/inet.h>
#include <zpp_bits.h>

#include "common/utils/network/network.h"

namespace common {
std::optional<SocketMessageHeader>
socketMessageHeaderFromBinary(const binary_t &binary) {
  SocketMessageHeader header;

  zpp::bits::in in(binary, zpp::bits::endian::big{});
  auto result = in(header.msgSize, header.protocol, header.reqType);
  if (zpp::bits::failure(result)) {
    return std::nullopt;
  }

  return header;
}

std::optional<binary_t>
socketMessageHeaderToBinary(const SocketMessageHeader &header) {
  common::binary_t data{};
  zpp::bits::out out(data, zpp::bits::endian::big{});

  auto result = out(header.msgSize, header.protocol, header.reqType);
  if (zpp::bits::failure(result)) {
    return std::nullopt;
  }

  return data;
}
} // namespace common
