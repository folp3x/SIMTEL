#include "config.h"

#include <iostream>

namespace server {
bool Config::isInitialized() const {
  std::cout << smscConfig.ttlMs << std::endl;
  std::cout << smscConfig.cdrAccessJsonFilePath << std::endl;
  return common::Config::isInitialized() && !bsFilePath.empty() &&
         !epcFilePath.empty();

  // && mmeConfig.maxVlrSize != 0 &&
  // smscConfig.ttlMs != 0 && !smscConfig.cdrAccessFilePath.empty();
}

std::string Config::getBsFilePath() const { return bsFilePath; }

void Config::setBsFilePath(const std::string &bsFilePath_) {
  bsFilePath = bsFilePath_;
}

std::string Config::getEpcFilePath() const { return epcFilePath; }

void Config::setEpcFilePath(const std::string &epcFilePath_) {
  epcFilePath = epcFilePath_;
}

size_t Config::getMmeMaxVlrSize() const { return mmeConfig.maxVlrSize; }

void Config::setMmeMaxVlrSize(size_t maxVlrSize) {
  mmeConfig.maxVlrSize = maxVlrSize;
}

unsigned int Config::getSmscTtlMs() const { return smscConfig.ttlMs; }

void Config::setSmscTtlMs(unsigned int smscTtlMs) {
  smscConfig.ttlMs = smscTtlMs;
}

std::string Config::getSmsCdrAccessJsonFilePath() const {
  return smscConfig.cdrAccessJsonFilePath;
}

void Config::setSmsCdrAccessJsonFilePath(
    const std::string &smscCdrAccessJsonFilePath_) {
  smscConfig.cdrAccessJsonFilePath = smscCdrAccessJsonFilePath_;
}
} // namespace server
