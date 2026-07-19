#pragma once

#include "common/core/request/request/request.h"

namespace common {
class SmDeliveryReportResponse : public Request {
private:
  imsi_t mTimsi = "";
  unsigned int smsId = 0;

protected:
  virtual std::unique_ptr<BaseJsonInfo> getJsonRootInfo() override;

  virtual std::vector<std::unique_ptr<BaseBinaryInfo>>
  getBinaryValuesInfo() override;

public:
  SmDeliveryReportResponse() = default;
  SmDeliveryReportResponse(const common::imsi_t &mTimsi_, unsigned int smsId_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;

  imsi_t getMTimsi() const;
  unsigned int getSmsId() const;
};
} // namespace common
