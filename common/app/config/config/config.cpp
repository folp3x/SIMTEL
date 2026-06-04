#include "config.h"

namespace common {
bool Config::isInitialized() const {
  return locationSet && port != INVALID_PORT;
}

int Config::getPort() const { return port; }

void Config::setPort(int port_) { port = port_; }

common::coords_t<> Config::getLoc() const { return loc; }

void Config::setLoc(const common::coords_t<> &loc_) {
  loc = loc_;
  locationSet = true;
}
} // namespace common
