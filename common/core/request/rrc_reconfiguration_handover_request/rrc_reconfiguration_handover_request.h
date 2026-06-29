#pragma once

#include "common/core/request/request/request.h"

namespace common {
class RrcReconfigurationHandoverRequest : public Request {
private:
  imsi_t mTimsi = "";
  unsigned int bsId = 0;

public:
  RrcReconfigurationHandoverRequest() = default;
  RrcReconfigurationHandoverRequest(const imsi_t &mTimsi_, unsigned int bsId_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::expected<binary_t, std::string> toBinary() const;
  virtual std::optional<std::string> fromBinary(const binary_t &binary);

  imsi_t getMTimsi() const;
  unsigned int getBsId() const;
};
} // namespace common
