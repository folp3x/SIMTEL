#include "simtel_smsc.h"

#include "server/core/simtel/simtel_mme/simtel_mme.h"

namespace server {
SimtelSmsc::SimtelSmsc(const SmscConfig &config)
    : smsTtlMs(config.smsTtlMs), cdrJsonFilePath(config.cdrJsonFilePath) {}

void SimtelSmsc::setMmeList(
    const std::unordered_map<unsigned int, std::shared_ptr<SimtelMme>>
        &mmeList_) {
  mmeList = mmeList_;
}
} // namespace server
