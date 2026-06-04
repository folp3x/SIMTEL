#include "config.h"

namespace client {
bool Config::isInitialized() const {
  return common::Config::isInitialized() && !imei.empty() && !imsi.empty() &&
         !ip.empty();
}

std::string Config::getImei() const { return imei; }

void Config::setImei(const common::imei_t &imei_) { imei = imei_; }

std::string Config::getImsi() const { return imsi; }

void Config::setImsi(const common::imsi_t &imsi_) { imsi = imsi_; }

std::string Config::getIP() const { return ip; }

void Config::setIP(const std::string &ip_) { ip = ip_; }
} // namespace client
