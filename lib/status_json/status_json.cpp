#include "status_json.h"
#include <cstdio>
#include "autwat_config.h"

namespace autwat {

namespace {

const char* alarmName(Alarm alarm) {
    static const char* const kNames[] = {"none", "slow", "fast"};
    return kNames[static_cast<uint8_t>(alarm)];
}

const char* healthName(Health health) {
    static const char* const kNames[] = {"ok", "dry", "overwatered"};
    return kNames[static_cast<uint8_t>(health)];
}

void formatNullable(char* out, size_t outSize, int16_t value, int16_t invalid) {
    if (value == invalid) {
        snprintf(out, outSize, "null");
    } else {
        snprintf(out, outSize, "%d", value);
    }
}

}  // namespace

const char* modeName(Mode mode) {
    static const char* const kNames[] = {"idle", "scheduled", "emergency", "lockout", "fault"};
    return kNames[static_cast<uint8_t>(mode)];
}

Health healthFor(uint8_t moisturePct, const PlantProfile& plant) {
    Health health = Health::Ok;
    if (moisturePct < plant.criticalLowPct) {
        health = Health::Dry;
    } else if (moisturePct > plant.criticalHighPct) {
        health = Health::Overwatered;
    }
    return health;
}

int writeStatusJson(char* out, size_t outSize, const StatusSnapshot& snapshot) {
    int written = -1;
    if (out != nullptr && outSize > 0) {
        char humidity[8];
        char temperature[8];
        formatNullable(humidity, sizeof(humidity), snapshot.humidityPct, kInvalidHumidity);
        formatNullable(temperature, sizeof(temperature), snapshot.tempC, kInvalidTemp);
        const Health health = healthFor(snapshot.moisturePct, plantAt(snapshot.plantIndex));
        const int needed = snprintf(
            out, outSize,
            "{\"plant\":\"%s\",\"moisture_pct\":%u,\"humidity_pct\":%s,\"temp_c\":%s,"
            "\"health\":\"%s\",\"mode\":\"%s\",\"pump\":%s,\"alarm\":\"%s\","
            "\"wifi_rssi\":%d,\"uptime_s\":%lu}",
            snapshot.plantName, static_cast<unsigned>(snapshot.moisturePct), humidity, temperature,
            healthName(health), modeName(snapshot.mode), snapshot.pumpOn ? "true" : "false",
            alarmName(snapshot.alarm), snapshot.wifiRssi,
            static_cast<unsigned long>(snapshot.uptimeSeconds));
        if (needed >= 0 && static_cast<size_t>(needed) < outSize) written = needed;
    }
    return written;
}

}  // namespace autwat
