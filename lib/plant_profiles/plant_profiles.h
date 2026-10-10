#pragma once
#include <cstdint>

namespace autwat {

struct PlantProfile {
    const char* name;
    uint8_t criticalLowPct;
    uint8_t normalHighPct;
    uint8_t criticalHighPct;
    uint16_t waterMl;
};

constexpr uint8_t kPlantCount = 3;

const PlantProfile& plantAt(uint8_t index);
uint8_t nextPlantIndex(uint8_t currentIndex);

}  // namespace autwat
