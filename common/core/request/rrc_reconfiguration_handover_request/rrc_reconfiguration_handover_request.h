#pragma once

#include "common/core/request/request/request.h"
#include "common/types.h"

namespace common {
struct RrcReconfigurationHandoverRequest : Request {
  imsi_t mTimsi = "";
  unsigned int bsId = 0;

  RrcReconfigurationHandoverRequest() = default;
  RrcReconfigurationHandoverRequest(const imsi_t &mTimsi_, unsigned int bsId_);

  virtual std::string toStr() const override;

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::expected<binary_t, std::string> toBinary() const;
  virtual std::optional<std::string> fromBinary(const common::binary_t &binary);
};
} // namespace common
