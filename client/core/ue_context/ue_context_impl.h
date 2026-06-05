#pragma once

namespace client {
template <typename T>
void UeContext::updateLocation(const std::vector<T> &coords) {
  location.move(coords);
}
} // namespace client
