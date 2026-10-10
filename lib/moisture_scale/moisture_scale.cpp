#include "moisture_scale.h"

namespace autwat {

uint8_t adcToPercent(uint16_t rawReading, uint16_t rawDry, uint16_t rawWet) {
    int32_t offset = static_cast<int32_t>(rawReading) - rawDry;
    int32_t span = static_cast<int32_t>(rawWet) - rawDry;
    if (span < 0) {
        offset = -offset;
        span = -span;
    }
    uint8_t percent = 0;
    if (span == 0 || offset <= 0) {
        percent = 0;
    } else if (offset >= span) {
        percent = 100;
    } else {
        percent = static_cast<uint8_t>((offset * 100 + span / 2) / span);
    }
    return percent;
}

}  // namespace autwat
