#pragma once
#include <cstdint>

namespace autwat {

// Returns 0 for a non-positive flow rate; the caller treats that as a config error.
uint32_t runMsFor(uint16_t waterMl, float flowRateMlPerSec);

}  // namespace autwat
