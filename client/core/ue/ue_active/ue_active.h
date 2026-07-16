#pragma once

#include <string>

#include "client/app/locale/macro.h"

namespace client {
inline std::string ueActiveToStr(bool inActive) {
  return inActive ? _("active") : _("not active");
}
} // namespace client
