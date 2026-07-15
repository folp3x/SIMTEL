#pragma once

#include "common/core/location/location/location.h"
#include "common/core/request/request/request.h"

namespace common {
class RrcConnectionRequest : public Request {
private:
  imei_t imei = "";
  Location<> loc{};

public:
  RrcConnectionRequest() = default;
  RrcConnectionRequest(const imei_t &imei_, const Location<> &loc_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::vector<std::unique_ptr<BaseBinaryInfo>>
  getBinaryValuesInfo() override;

  imei_t getImei() const;
  Location<> getLoc() const;
};
} // namespace common
