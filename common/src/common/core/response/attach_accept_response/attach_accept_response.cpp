#include "attach_accept_response.h"

namespace common {
RequestType AttachAcceptResponse::getType() const {
  return RequestType::AttachAccept;
}
} // namespace common
