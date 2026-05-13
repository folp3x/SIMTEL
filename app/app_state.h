#pragma once

#include <string>

enum class AppState { ACTIVE, INACTIVE };

std::string appStateToStr(AppState state);
