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

  virtual std::unique_ptr<BaseJsonInfo> getJsonRootInfo() override;

  virtual std::vector<std::unique_ptr<BaseBinaryInfo>>
  getBinaryValuesInfo() override;

  imsi_t getMTimsi() const;
};
} // namespace common
