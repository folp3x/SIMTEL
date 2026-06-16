#pragma once

#include <expected>

#include "common/core/request/request/request.h"

namespace common {
class EmptyRequest : public Request {
public:
  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::expected<binary_t, std::string> toBinary() const;
  virtual std::optional<std::string> fromBinary(const common::binary_t &binary);
};
} // namespace common
