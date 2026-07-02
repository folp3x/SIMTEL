#include "attach_accept_response.h"

namespace common {
RequestType AttachAcceptResponse::getType() const {
  return RequestType::Attach_Accept;
}
} // namespace common
