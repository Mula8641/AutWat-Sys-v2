#include "screen_text.h"
#include <cstdio>
#include "autwat_config.h"

namespace autwat {

namespace {

void padLine(char* out, const char* text) {
    uint8_t i = 0;
    for (; i < kScreenWidth && text[i] != '\0'; ++i) out[i] = text[i];
    for (; i < kScreenWidth; ++i) out[i] = ' ';
    out[kScreenWidth] = '\0';
}

}  // namespace

void formatSensorLine(char* out, uint8_t moisturePct, int16_t humidityPct, int16_t tempC) {
    char humidity[8] = "--";
    char temperature[8] = "--";
    if (humidityPct != kInvalidHumidity) snprintf(humidity, sizeof(humidity), "%d%%", humidityPct);
    if (tempC != kInvalidTemp) snprintf(temperature, sizeof(temperature), "%dC", tempC);
    char text[48];
    snprintf(text, sizeof(text), "M:%u%% H:%s T:%s", static_cast<unsigned>(moisturePct), humidity, temperature);
    padLine(out, text);
}

void formatModeLine(char* out, Mode mode) {
    static const char* const kLines[] = {"Idle", "Watering", "Dry! Emergency", "Too wet! Locked",
                                         "FAULT: press button"};
    padLine(out, kLines[static_cast<uint8_t>(mode)]);
}

void formatPlantLine(char* out, const PlantProfile& plant) { padLine(out, plant.name); }

void formatLinkLine(char* out, bool wifiUp, bool bleConnected) {
    char text[32];
    snprintf(text, sizeof(text), "WiFi:%s BLE:%s", wifiUp ? "ON " : "OFF", bleConnected ? "CONN" : "ADV");
    padLine(out, text);
}

}  // namespace autwat
