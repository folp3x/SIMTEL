#pragma once

namespace client {
template <typename T>
  requires std::is_arithmetic_v<T>
void UeContext::updateLocation(const std::vector<T> &coords) {
  state.location.move(coords);
}
} // namespace client
