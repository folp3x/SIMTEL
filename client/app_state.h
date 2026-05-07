#pragma once

#include <string>

namespace client {
enum class AppState { ACTIVE, INACTIVE };

std::string appStateToStr(AppState state);
} // namespace client
