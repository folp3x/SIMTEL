#pragma once

#include "common/core/location/location/location.h"
#include "common/network/network_address/network_address.h"
#include "common/network/protocol/protocol.h"
#include "common/types.h"

namespace client {
struct UeState {
  const common::imsi_t imsi;
  const common::imei_t imei;
  common::imsi_t mTimsi = "";

  common::Location<> location;
  common::Protocol protocol = common::Protocol::JSON;

  UeState(const common::imsi_t &imsi_, const common::imei_t &imei_,
          const common::Location<> location_)
      : imsi(imsi_), imei(imei_), location(location_) {}
};
} // namespace client
