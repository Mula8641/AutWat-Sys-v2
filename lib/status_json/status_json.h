#pragma once
#include <cstddef>
#include <cstdint>
#include "plant_profiles.h"
#include "watering_controller.h"

namespace autwat {

struct StatusSnapshot {
    const char* plantName;
    uint8_t plantIndex;
    uint8_t moisturePct;
    int16_t humidityPct;
    int16_t tempC;
    Mode mode;
    bool pumpOn;
    Alarm alarm;
    int8_t wifiRssi;
    uint32_t uptimeSeconds;
};

enum class Health : uint8_t { Ok = 0, Dry = 1, Overwatered = 2 };

const char* modeName(Mode mode);
Health healthFor(uint8_t moisturePct, const PlantProfile& plant);

// Returns characters written, or -1 if the buffer is too small.
int writeStatusJson(char* out, size_t outSize, const StatusSnapshot& snapshot);

}  // namespace autwat
