#pragma once

#include "common/core/request/request/request.h"

namespace common {
class MeasurementControlResponse : public Request {
  imei_t imei = "";
  unsigned int signal = 0;
  unsigned int bsId = 0;

public:
  MeasurementControlResponse() = default;
  MeasurementControlResponse(const imei_t &imei_, unsigned int signal_,
                             unsigned int bsId_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;

  virtual std::unique_ptr<BaseJsonInfo> getJsonRootInfo() override;

  virtual std::vector<std::unique_ptr<BaseBinaryInfo>>
  getBinaryValuesInfo() override;

  imei_t getImei() const;
  unsigned int getSignal() const;
  unsigned int getBsId() const;
};
} // namespace common
