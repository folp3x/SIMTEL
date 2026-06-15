#include "config.h"

namespace common {
bool Config::isInitialized() const { return port != INVALID_PORT; }

int Config::getPort() const { return port; }

void Config::setPort(int port_) { port = port_; }
} // namespace common
