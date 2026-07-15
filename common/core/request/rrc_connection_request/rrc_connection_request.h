#pragma once

#include "common/core/request/request/request.h"

namespace common {
class RrcConnectionRequest : public Request {
private:
  imei_t imei = "";
  coords_t<> coords;

public:
  RrcConnectionRequest() = default;
  RrcConnectionRequest(const imei_t &imei_, const coords_t<> &coords_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;

  virtual std::unique_ptr<BaseJsonInfo> getJsonRootInfo() override;

  virtual std::vector<std::unique_ptr<BaseBinaryInfo>>
  getBinaryValuesInfo() override;

  imei_t getImei() const;
  coords_t<> getCoords() const;
};
} // namespace common
