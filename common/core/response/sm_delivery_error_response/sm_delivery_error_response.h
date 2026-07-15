#pragma once

#include "common/core/response/sm_delivery_report_response/sm_delivery_report_response.h"

namespace common {
class SmDeliveryErrorResponse : public SmDeliveryReportResponse {
private:
  std::string description = "";

public:
  SmDeliveryErrorResponse() = default;
  SmDeliveryErrorResponse(const imsi_t &mTimsi, unsigned int smsId,
                          const std::string &description_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;

  virtual std::unique_ptr<BaseJsonInfo> getJsonRootInfo() override;

  virtual std::vector<std::unique_ptr<BaseBinaryInfo>>
  getBinaryValuesInfo() override;

  std::string getDescription() const;
};
} // namespace common
