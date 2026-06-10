#pragma once

#include <array>
#include <memory>
#include <queue>
#include <unordered_map>

#include "common/core/location/location/location.h"
#include "common/core/request/rrc_connection_request/rrc_connection_request.h"
#include "common/network/protocol/protocol.h"
#include "common/types.h"

namespace server {
class SimtelUeContext;

class SimtelBaseStation {
private:
  static std::unordered_map<unsigned int, std::unique_ptr<SimtelBaseStation>>
      baseStations;

  common::binary_t buf = {};
  std::unordered_map<common::imsi_t, std::unique_ptr<SimtelUeContext>>
      connectedUe = {};
  common::Location<> location{};
  const float radius = 0;
  const unsigned int id = 0;

  std::queue<common::Request> requests{};

  unsigned int measureSignal(const common::Location<> &targetLoc) const;

  std::optional<std::string>
  sendSignalLevel(const common::imei_t &imei, unsigned int signalLevel,
                  std::shared_ptr<SimtelUeContext> ctx);

public:
  static void handleConnectionRequest(std::shared_ptr<SimtelUeContext> ctx);

  common::binary_t getBuf() const;
  void setBuf(const common::binary_t &buf_);
  void clearBuf();

  common::Location<> getLocation() const;

  void handleLocationUpdate(const common::RrcConnectionRequest &req,
                            std::shared_ptr<SimtelUeContext> ctx);
};
} // namespace server
