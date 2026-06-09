#include "socket_message_header.h"

#include <arpa/inet.h>
#include <zpp_bits.h>

namespace common {
std::expected<SocketMessageHeader, std::string>
socketMessageHeaderFromBinary(const binary_t &binary) {
  SocketMessageHeader header;

  zpp::bits::in in(binary);

  if (in(header.msgSize, header.protocol, header.msgType) !=
      zpp::bits::errc{}) {
    return std::unexpected("Failed to deserialize header");
  }

  header.msgSize = ntohl(header.msgSize);
  return header;
}

std::expected<binary_t, std::string>
socketMessageHeaderToBinary(const SocketMessageHeader &header) {
  auto [data, in, out] = zpp::bits::data_in_out();

  uint32_t msgSize = htonl(header.msgSize);

  if (out(msgSize, header.protocol, header.msgType) != zpp::bits::errc{}) {
    return std::unexpected("Failed to serialize header");
  }

  return data;
}
} // namespace common
