#pragma once

#include <string>

namespace client {
enum class AppState { ACTIVE, INACTIVE };

inline std::string_view appStateToStr(AppState state) {
  switch (state) {
  case AppState::ACTIVE:
    return "active";
  case AppState::INACTIVE:
    return "inactive";
  }

  return "unknown";
}
} // namespace client
