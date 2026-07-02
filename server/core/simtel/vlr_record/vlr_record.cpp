#include "vlr_record.h"

#include "server/core/simtel/simtel_base_station/simtel_base_station.h"

namespace server {
std::string VlrRecord::toStr() const {
  std::string bsIdStr = (!bs) ? "?" : std::to_string(bs->getId());
  return "[mTimsi=" + mTimsi + ", imsi=" + imsi + ", imei=" + imei +
         ", msisdn=" + msisdn + ", bsId=" + bsIdStr + "]";
}
} // namespace server
