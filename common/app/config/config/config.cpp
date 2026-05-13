#include "config.h"

namespace common {
bool Config::isInitialized() const {
  return ip != "" && port != -1 && locationSet;
}

std::string Config::getIP() const { return ip; }

void Config::setIP(const std::string &ip_) { ip = ip_; }

int Config::getPort() const { return port; }

void Config::setPort(int port_) { port = port_; }

common::coords_t<float> Config::getLoc() const { return loc; }

void Config::setLoc(const common::coords_t<float> &loc_) {
  loc = loc_;
  locationSet = true;
}
} // namespace common
