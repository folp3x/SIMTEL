#pragma once

namespace server {
template <std::derived_from<common::Request> T>
std::expected<T, std::string>
SimtelBaseStation::receiveRequest(std::shared_ptr<SimtelUeContext> ctx) const {
  auto receiveError = ctx->receiveData();
  if (receiveError) {
    return std::unexpected(receiveError->description);
  }

  common::Protocol protocol;
  auto req = parseFromBytes<T>(ctx->takeBuf(), protocol);
  if (!req) {
    return std::unexpected(req.error());
  }

  ctx->setProtocol(protocol);
  MessageHolder::instance().addMsg(
      createLogMsg("request from " + ctx->toStr() + " = " + req->toStr()));

  return req;
}

template <std::derived_from<common::Request> T>
std::expected<T, std::string>
SimtelBaseStation::parseFromBytes(const common::binary_t &bytes,
                                  common::Protocol &protocol) const {
  T req{};
  auto error = req.fromBytes(bytes, protocol);
  if (error) {
    return std::unexpected(*error);
  }
  return req;
}
} // namespace server
