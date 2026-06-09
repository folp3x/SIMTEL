#pragma once

#include <array>
#include <memory>
#include <queue>
#include <unordered_map>

#include "common/core/location/location/location.h"
#include "common/types.h"

namespace server {
class SimtelUeContext;

class SimtelBaseStation {
private:
  static std::unordered_map<unsigned int, std::unique_ptr<SimtelBaseStation>>
      baseStations;

  common::binary_t buf = {};
  std::unordered_map<common::imsi_t, std::unique_ptr<SimtelUeContext>> ueInfo =
      {};
  common::Location<> location{};
  const float radius = 0;

  static std::queue<std::shared_ptr<SimtelUeContext>> connectionRequests;

public:
  static void handleConnectionRequest(std::shared_ptr<SimtelUeContext> ctx);

  common::binary_t getBuf() const;
  void setBuf(const common::binary_t &buf_);
  void clearBuf();

  common::Location<> getLocation() const;
};
} // namespace server
