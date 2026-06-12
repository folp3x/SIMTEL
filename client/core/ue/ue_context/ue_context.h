#pragma once

#include "client/core/ue/ue_state/ue_state.h"

namespace client {
class UeContext {
private:
  UeState state;

  const common::NetworkAddress serverAddr;

  bool inActive = false;

public:
  UeContext(const common::imsi_t &imsi, const common::imei_t &imei,
            common::Location<> &location,
            const common::NetworkAddress &serverAddr_);

  common::imsi_t getImsi() const;

  common::imei_t getImei() const;

  common::imsi_t getMTimsi() const;
  bool setMTimsi(const std::string &mtmsi_);

  common::Location<> getLocation() const;
  template <typename T> void updateLocation(const std::vector<T> &coords);

  common::NetworkAddress getServerAddr() const;

  bool isInActive() const;
  void setInActive(bool inActive_);

  common::Protocol getProtocol() const;
  void setProtocol(common::Protocol protocol_);

  UeState getState() const;
};
} // namespace client

#include "ue_context_impl.h"
