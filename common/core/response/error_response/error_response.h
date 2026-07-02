#pragma once

#include "common/core/request/request/request.h"

namespace common {
class ErrorResponse : public Request {
private:
  std::string description = "";

public:
  ErrorResponse() = default;
  ErrorResponse(const std::string &description_);

  virtual RequestType getType() const override;

  virtual nlohmann::json toJson() const;
  virtual std::optional<std::string> fromJsonStr(const std::string &jsonStr);

  virtual std::expected<binary_t, std::string> toBinary() const;
  virtual std::optional<std::string> fromBinary(const binary_t &binary);

  std::string getDescription() const;
};
} // namespace common
