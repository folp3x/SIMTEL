#include "vlr_record.h"

#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
std::string VlrRecord::toStr() const {
  std::string bsIdStr =
      (bs == nullptr) ? "unknown" : "BS_" + std::to_string(bs->getId());
  return "[m-timsi=" + mTimsi + ", imei=" + imei + ", msisdn=" + msisdn +
         ", bs=" + bsIdStr + "]";
}
} // namespace server
