#include "signal_handler.h"

namespace common {
std::unordered_map<int, std::function<void(int)>> SignalHandler::handlers{};

SignalHandler &SignalHandler::getInstance() {
  static SignalHandler instance;
  return instance;
}

void SignalHandler::staticHandler(int signal) {
  auto handlers = getInstance().getHandlers();
  if (handlers.find(signal) != handlers.end()) {
    handlers[signal](signal);
  }
}

void SignalHandler::setHandler(int signal,
                               const std::function<void(int)> &handler) {
  handlers[signal] = handler;
  std::signal(signal, staticHandler);
}

std::unordered_map<int, std::function<void(int)>> SignalHandler::getHandlers() {
  return handlers;
}
} // namespace common
