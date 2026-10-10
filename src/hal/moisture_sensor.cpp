#include "moisture_sensor.h"
#include <Arduino.h>
#include "autwat_config.h"

void moistureSensorBegin() { pinMode(autwat::kPinMoisture, INPUT); }

uint16_t moistureSensorReadRaw() { return static_cast<uint16_t>(analogRead(autwat::kPinMoisture)); }
