#include "simtel_base_station.h"

namespace server {
void SimtelBaseStation::put(const common::binary_t &data) { buf = data; }

common::binary_t SimtelBaseStation::get() const { return buf; }
} // namespace server
