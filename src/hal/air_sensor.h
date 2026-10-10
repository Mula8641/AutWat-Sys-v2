#pragma once
#include <cstdint>

struct AirReading {
    int16_t humidityPct;
    int16_t tempC;
    bool valid;
};

void airSensorBegin();
void airSensorTick(uint32_t nowMs);
AirReading airSensorLatest();
