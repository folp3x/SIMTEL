#pragma once

#include "common/core/request/empty_request/empty_request.h"

namespace common {
class AttachAcceptRequest : public Request {
public:
  virtual std::string toStr() const override;

  virtual RequestType getType() const override;
};
} // namespace common
