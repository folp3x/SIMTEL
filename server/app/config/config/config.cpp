#include "config.h"

namespace server {
bool Config::isInitialized() const {
  return common::Config::isInitialized() && !bsFilePath.empty() &&
         !epcFilePath.empty() && mmeConfig.maxVlrSize != 0 &&
         smscConfig.ttlMs != 0 && !smscConfig.cdrAccessFilePath.empty();
}

std::string Config::getBsFilePath() const { return bsFilePath; }

void Config::setBsFilePath(const std::string &bsFilePath_) {
  bsFilePath = bsFilePath_;
}

std::string Config::getEpcFilePath() const { return epcFilePath; }

void Config::setEpcFilePath(const std::string &epcFilePath_) {
  epcFilePath = epcFilePath;
}

size_t Config::getMmeMaxVlrSize() const { return mmeConfig.maxVlrSize; }

void Config::setMmeMaxVlrSize(size_t maxVlrSize) {
  mmeConfig.maxVlrSize = maxVlrSize;
}

unsigned int Config::getSmscTtlMs() const { return smscConfig.ttlMs; }

void Config::setSmscTtlMs(unsigned int smscTtlMs) {
  smscConfig.ttlMs = smscTtlMs;
}

std::string Config::getSmsCdrAccessFilePath() const {
  return smscConfig.cdrAccessFilePath;
}

void Config::setSmsCdrAccessFilePath(const std::string &smscCdrAccessFilePath) {
  smscConfig.cdrAccessFilePath = smscCdrAccessFilePath;
}
} // namespace server
