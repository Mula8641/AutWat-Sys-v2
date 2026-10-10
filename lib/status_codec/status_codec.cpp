#include "status_codec.h"
#include "autwat_config.h"

namespace autwat {

namespace {

constexpr uint8_t kHumidityInvalidByte = 255;

uint8_t humidityByte(int16_t humidityPct) {
    uint8_t byte = kHumidityInvalidByte;
    if (humidityPct != kInvalidHumidity && humidityPct >= 0 && humidityPct <= 100) {
        byte = static_cast<uint8_t>(humidityPct);
    }
    return byte;
}

uint8_t temperatureByte(int16_t tempC) {
    int16_t clamped = tempC;
    if (clamped > 127) clamped = 127;
    if (clamped < -127 && tempC != kInvalidTemp) clamped = -127;
    return static_cast<uint8_t>(static_cast<int8_t>(clamped));
}

}  // namespace

void encodeStatus(uint8_t out[kStatusBytes], const StatusSnapshot& snapshot, Health health) {
    out[0] = snapshot.moisturePct;
    out[1] = humidityByte(snapshot.humidityPct);
    out[2] = temperatureByte(snapshot.tempC);
    out[3] = static_cast<uint8_t>(snapshot.mode);
    out[4] = snapshot.pumpOn ? 1 : 0;
    out[5] = static_cast<uint8_t>(snapshot.alarm);
    out[6] = static_cast<uint8_t>(health);
    out[7] = snapshot.plantIndex;
}

}  // namespace autwat
