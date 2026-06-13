#include "attach_accept_request.h"

namespace common {
std::string AttachAcceptRequest::toStr() const { return "Attach_Accept"; }

RequestType AttachAcceptRequest::getType() const {
  return RequestType::Attach_Accept;
}
} // namespace common
