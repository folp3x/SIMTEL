#pragma once

#include "common/core/location/location/location.h"
#include "common/network/network_address/network_address.h"
#include "common/network/protocol/protocol.h"
#include "common/types.h"

namespace client {
class UeContext {
private:
  const common::imsi_t imsi = "";
  const common::imei_t imei = "";

  const common::NetworkAddress serverAddr;

  common::Location<> location;

  common::imsi_t mtimsi = "";

  bool inActive = false;
  common::Protocol protocol = common::Protocol::JSON;

public:
  UeContext(common::Location<> &location_, const common::imsi_t &imsi_,
            const common::imei_t &imei_,
            const common::NetworkAddress &serverAddr_);

  common::imsi_t getImsi() const;

  common::imei_t getImei() const;

  common::imsi_t getMtimsi() const;
  void setMtmsi(const std::string &mtmsi_);

  common::Location<> getLocation() const;
  template <typename T> void updateLocation(const std::vector<T> &coords);

  common::NetworkAddress getServerAddr() const;

  bool isInActive() const;
  void setInActive(bool inActive_);

  common::Protocol getProtocol() const;
  void setProtocol(common::Protocol protocol_);
};
} // namespace client

#include "ue_context_impl.h"
