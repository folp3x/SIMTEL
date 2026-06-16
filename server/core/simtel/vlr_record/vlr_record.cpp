#include "vlr_record.h"

#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
std::string VlrRecord::toStr() const {
  std::string bsIdStr = (bs == nullptr) ? "?" : std::to_string(bs->getId());
  return "[imsi=" + imsi + ", imei=" + imei + ", msisdn=" + msisdn +
         ", bsId=" + bsIdStr + "]";
}
} // namespace server
