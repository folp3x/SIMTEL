#include "simtel_smsc.h"

namespace server {
SimtelSmsc::SimtelSmsc(const SmscConfig &config)
    : smsTtlMs(config.smsTtlMs), cdrJsonFilePath(config.cdrJsonFilePath) {}
} // namespace server
