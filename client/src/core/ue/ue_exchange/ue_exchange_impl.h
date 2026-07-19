#pragma once

namespace client {
template <std::derived_from<common::Request> T>
std::expected<T, std::string> UeExchange::receiveResponse() const {
  auto bytes = sock.receiveMessage();
  if (!bytes) {
    return std::unexpected(bytes.error().description);
  }

  return parseFromBytes<T>(*bytes);
}

template <std::derived_from<common::Request> T>
std::expected<T, std::string>
UeExchange::parseFromBytes(const common::binary_t &bytes) const {
  common::Protocol protocol;
  T req{};
  auto error = req.fromBytes(bytes, protocol);
  if (error) {
    return std::unexpected(*error);
  }
  if (protocol != curProtocol) {
    return std::unexpected("Invalid protocol");
  }

  return req;
}
} // namespace client
