#pragma once

#include <csignal>
#include <functional>
#include <unordered_map>

namespace common {
class SignalHandler {
public:
  using handler_t = std::function<void(int)>;
  using handler_map_t = std::unordered_map<int, handler_t>;

private:
  static SignalHandler &getInstance();

  inline static handler_map_t handlers{};

  static void staticHandler(int signal);

public:
  static void setHandler(int signal, const handler_t &handler);

  static handler_map_t getHandlers();
};
} // namespace common
