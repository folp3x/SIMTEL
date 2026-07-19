#pragma once

#include "common/core/request/request/request.h"

namespace common {
class ErrorResponse : public Request {
private:
  std::string description = "";

  virtual std::unique_ptr<BaseJsonInfo> getJsonRootInfo() override;

  virtual std::vector<std::unique_ptr<BaseBinaryInfo>>
  getBinaryValuesInfo() override;

public:
  ErrorResponse() = default;
  ErrorResponse(const std::string &description_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;

  std::string getDescription() const;
};
} // namespace common
