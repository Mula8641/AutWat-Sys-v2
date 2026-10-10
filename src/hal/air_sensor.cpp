#include "air_sensor.h"
#include <Arduino.h>
#include <DHT.h>
#include <cmath>
#include "autwat_config.h"

namespace {

constexpr uint8_t kFailuresBeforeInvalid = 3;

DHT dht(autwat::kPinDht, DHT22);
AirReading latest = {autwat::kInvalidHumidity, autwat::kInvalidTemp, false};
uint32_t lastReadMs = 0;
uint8_t consecutiveFailures = 0;
bool firstReadDone = false;

void recordFailure() {
    ++consecutiveFailures;
    if (consecutiveFailures == kFailuresBeforeInvalid) {
        latest.valid = false;
        Serial.println("[air] DHT22 read failed 3 times, readings invalid");
    }
}

}  // namespace

void airSensorBegin() { dht.begin(); }

void airSensorTick(uint32_t nowMs) {
    if (firstReadDone && nowMs - lastReadMs < autwat::kAirSensorPeriodMs) return;
    firstReadDone = true;
    lastReadMs = nowMs;
    const float humidity = dht.readHumidity();
    const float temperature = dht.readTemperature();
    if (std::isnan(humidity) || std::isnan(temperature)) {
        recordFailure();
    } else {
        latest = {static_cast<int16_t>(lroundf(humidity)), static_cast<int16_t>(lroundf(temperature)), true};
        consecutiveFailures = 0;
    }
}

AirReading airSensorLatest() { return latest; }
