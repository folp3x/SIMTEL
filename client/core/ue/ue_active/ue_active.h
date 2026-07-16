#pragma once

#include <libintl.h>
#include <string>

#define _(str) gettext(str)

namespace client {
inline std::string ueActiveToStr(bool inActive) {
  return inActive ? _("active") : _("not active");
}
} // namespace client
