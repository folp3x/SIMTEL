#pragma once

#include <expected>

#include "common/core/request/request/request.h"

namespace common {
class EmptyRequest : public Request {
private:
  virtual std::unique_ptr<BaseJsonInfo> getJsonRootInfo() override;

  virtual std::vector<std::unique_ptr<BaseBinaryInfo>>
  getBinaryValuesInfo() override;

public:
  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
};
} // namespace common
