#pragma once

#include "common/core/request/request/request.h"

namespace common {
class UssdCodeRequest : public Request {
private:
  imsi_t mTimsi = "";
  uint8_t code = 0;

  virtual std::unique_ptr<BaseJsonInfo> getJsonRootInfo() override;

  virtual std::vector<std::unique_ptr<BaseBinaryInfo>>
  getBinaryValuesInfo() override;

public:
  UssdCodeRequest() = default;
  UssdCodeRequest(const imsi_t &mTimsi_, uint8_t code_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;

  imsi_t getMTimsi() const;
  uint8_t getCode() const;
};
} // namespace common
