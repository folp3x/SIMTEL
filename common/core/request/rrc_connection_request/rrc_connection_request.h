#pragma once

#include "common/core/location/location/location.h"
#include "common/core/request/request/request.h"
#include "common/types.h"

namespace common {
struct RrcConnectionRequest : Request {
  imei_t imei = "";
  Location<> loc{};

  RrcConnectionRequest() = default;
  RrcConnectionRequest(const imei_t &imei_, const Location<> &loc_);

  virtual std::string toStr() const override;

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::expected<binary_t, std::string> toBinary() const;
  virtual std::optional<std::string> fromBinary(const common::binary_t &binary);
};
} // namespace common
