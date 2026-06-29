#pragma once

#include "common/core/request/empty_request/empty_request.h"

namespace common {
class AttachAcceptRequest : public EmptyRequest {
public:
  virtual RequestType getType() const override;
};
} // namespace common
