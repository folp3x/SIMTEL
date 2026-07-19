#include "signal_handler.h"

namespace common {
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

void SignalHandler::setHandler(int signal, const handler_t &handler) {
  handlers[signal] = handler;
  std::signal(signal, staticHandler);
}

SignalHandler::handler_map_t SignalHandler::getHandlers() { return handlers; }
} // namespace common
