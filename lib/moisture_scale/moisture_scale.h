#pragma once
#include <cstdint>

namespace autwat {

uint8_t adcToPercent(uint16_t rawReading, uint16_t rawDry, uint16_t rawWet);

}  // namespace autwat
