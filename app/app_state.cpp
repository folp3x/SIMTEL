#include "app_state.h"

// возвращает статус в виде строки
std::string appStateToStr(AppState state) {
  switch (state) {
  case AppState::ACTIVE:
    return "active";
  case AppState::INACTIVE:
    return "inactive";
  }

  return "unknown";
}
