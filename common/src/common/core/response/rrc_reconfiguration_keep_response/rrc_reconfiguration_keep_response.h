#pragma once

#include "common/core/request/request/request.h"

namespace common {
class RrcReconfigurationKeepResponse : public Request {
private:
  imei_t imei = "";
  unsigned int bsId = 0;

  virtual std::unique_ptr<BaseJsonInfo> getJsonRootInfo() override;

  virtual std::vector<std::unique_ptr<BaseBinaryInfo>>
  getBinaryValuesInfo() override;

public:
  RrcReconfigurationKeepResponse() = default;
  RrcReconfigurationKeepResponse(const imei_t &imei_, unsigned int bsId_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;

  imei_t getImei() const;
  unsigned int getBsId() const;
};
} // namespace common
