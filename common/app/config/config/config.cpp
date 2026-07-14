#include "config.h"

namespace common {
bool Config::isInitialized() const { return port != InvalidPort; }

int Config::getPort() const { return port; }

void Config::setPort(int port_) { port = port_; }
} // namespace common
