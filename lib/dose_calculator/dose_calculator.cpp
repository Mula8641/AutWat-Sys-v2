#include "dose_calculator.h"

namespace autwat {

uint32_t runMsFor(uint16_t waterMl, float flowRateMlPerSec) {
    uint32_t runMs = 0;
    if (flowRateMlPerSec > 0.0f) {
        runMs = static_cast<uint32_t>(waterMl * 1000.0 / flowRateMlPerSec + 0.5);
    }
    return runMs;
}

}  // namespace autwat
