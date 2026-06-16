#pragma once

#include "common/core/request/request/request.h"

namespace common {
class RrcReconfigurationCompleteRequest : public Request {
private:
  imsi_t mTimsi = "";

public:
  RrcReconfigurationCompleteRequest() = default;
  RrcReconfigurationCompleteRequest(const imsi_t &mTimsi_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::expected<binary_t, std::string> toBinary() const;
  virtual std::optional<std::string> fromBinary(const binary_t &binary);

  imsi_t getMTimsi() const;
};
} // namespace common
