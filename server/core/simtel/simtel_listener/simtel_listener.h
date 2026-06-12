#pragma once

#include <functional>

#include "server/app/message_holder/message_holder.h"
#include "server/core/simtel/simtel_ue_context/simtel_ue_context.h"
#include "server/network/socket/socket.h"

namespace server {
class SimtelListener {
private:
  const size_t maxUeThreads;

  bool listening = false;

  std::unique_ptr<Socket> sock;

  std::atomic<int> activeThreads{0};

  std::shared_ptr<MessageHolder> msgHolder;

public:
  SimtelListener(const common::NetworkAddress &addr, size_t maxUeThreads_,
                 std::shared_ptr<MessageHolder> msgHolder_);

  void acceptConnections(
      const std::function<void(std::shared_ptr<SimtelUeContext> ctx)> &handler);

  void stop();
};
} // namespace server
