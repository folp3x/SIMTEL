#include "attach_accept_request.h"

namespace common {
RequestType AttachAcceptRequest::getType() const {
  return RequestType::Attach_Accept;
}
} // namespace common
