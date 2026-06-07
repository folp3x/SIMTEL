#pragma once

#include <csignal>
#include <functional>
#include <unordered_map>

namespace common {
class SignalHandler {
private:
  static SignalHandler &getInstance();

  static std::unordered_map<int, std::function<void(int)>> handlers;

  static void staticHandler(int signal);

public:
  static void setHandler(int signal, const std::function<void(int)> &handler);

  static std::unordered_map<int, std::function<void(int)>> getHandlers();
};
} // namespace common
