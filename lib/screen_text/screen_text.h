#pragma once
#include <cstdint>
#include "plant_profiles.h"
#include "watering_controller.h"

namespace autwat {

constexpr uint8_t kScreenWidth = 20;
constexpr uint8_t kScreenBufferSize = kScreenWidth + 1;

void formatSensorLine(char* out, uint8_t moisturePct, int16_t humidityPct, int16_t tempC);
void formatModeLine(char* out, Mode mode);
void formatPlantLine(char* out, const PlantProfile& plant);
void formatLinkLine(char* out, bool wifiUp, bool bleConnected);

}  // namespace autwat
