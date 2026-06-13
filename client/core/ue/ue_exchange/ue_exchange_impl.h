#pragma once

namespace client {
template <std::derived_from<common::Request> T>
std::expected<T, std::string> UeExchange::receiveRequest() const {
  auto bytes = sock.receiveMessage();
  if (!bytes) {
    return std::unexpected(bytes.error().description);
  }

  common::Protocol protocol;
  T req{};
  auto error = req.fromBytes(*bytes, protocol);
  if (error) {
    return std::unexpected(*error);
  }
  if (protocol != curProtocol) {
    return std::unexpected("Invalid protocol");
  }

  return req;
}
} // namespace client
