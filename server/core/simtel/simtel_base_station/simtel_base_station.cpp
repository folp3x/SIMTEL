#include "simtel_base_station.h"

#include "server/core/simtel/simtel_ue_context/simtel_ue_context.h"

namespace server {
std::queue<std::shared_ptr<SimtelUeContext>>
    SimtelBaseStation::connectionRequests = {};

std::unordered_map<unsigned int, std::unique_ptr<SimtelBaseStation>>
    SimtelBaseStation::baseStations = {};

void SimtelBaseStation::handleConnectionRequest(
    std::shared_ptr<SimtelUeContext> ctx) {

    for (const auto &[id, bs] : baseStations) {
  }
}

common::binary_t SimtelBaseStation::getBuf() const { return buf; }

void SimtelBaseStation::setBuf(const common::binary_t &buf_) { buf = buf_; }
} // namespace server
