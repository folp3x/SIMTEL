#pragma once

namespace client {
template <typename T>
void UeContext::updateLocation(const std::vector<T> &coords) {
  state.location.move(coords);
}
} // namespace client
