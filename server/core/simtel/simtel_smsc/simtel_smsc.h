#pragma once

#include <memory>
#include <unordered_map>

#include "server/app/config/smsc_config/smsc_config.h"

namespace server {
class SimtelMme;

class SimtelSmsc {
private:
  const unsigned int smsTtlMs = 0;
  std::string cdrJsonFilePath = "";

  std::unordered_map<unsigned int, std::shared_ptr<SimtelMme>> mmeList;

public:
  explicit SimtelSmsc(const SmscConfig &config);

  void
  setMmeList(const std::unordered_map<unsigned int, std::shared_ptr<SimtelMme>>
                 &mmeList_);
};
} // namespace server
