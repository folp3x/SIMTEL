#pragma once

namespace server {
template <std::derived_from<common::Request> T>
std::expected<T, std::string>
SimtelBaseStation::receiveRequest(std::shared_ptr<SimtelUeContext> ctx) {
  auto receiveError = ctx->receiveData();
  if (receiveError) {
    return std::unexpected(receiveError->description);
  }

  common::Protocol protocol;
  T req{};
  auto error = req.fromBytes(ctx->takeBuf(), protocol);
  if (error) {
    return std::unexpected(*error);
  }

  ctx->setProtocol(protocol);
  MessageHolder::instance().addMsg(
      createLogMsg("req from " + ctx->toStr() + " = " + req.toStr()));

  return req;
}
} // namespace server
