#include "config.h"

bool Config::isInitialized() const {
  return ip != "" && port != -1 && imei != "" && imsi != "" && locationSet;
}

std::string Config::getIP() const { return ip; }

void Config::setIP(const std::string &ip_) { ip = ip_; }

int Config::getPort() const { return port; }

void Config::setPort(int port_) { port = port_; }

std::string Config::getImei() const { return imei; }

void Config::setImei(const std::string &imei_) { imei = imei_; }

std::string Config::getImsi() const { return imsi; }

void Config::setImsi(const std::string &imsi_) { imsi = imsi_; }

std::array<double, Constants::LOCATION_COORDS_COUNT> Config::getLoc() const {
  return loc;
}

void Config::setLoc(
    const std::array<double, Constants::LOCATION_COORDS_COUNT> &loc_) {
  loc = loc_;
  locationSet = true;
}
