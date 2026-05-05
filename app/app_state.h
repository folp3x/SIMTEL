#ifndef APP_STATE_H
#define APP_STATE_H

#include <string>

enum class AppState { ACTIVE, INACTIVE };

std::string appStateToStr(AppState state);

#endif // APP_STATE_H
