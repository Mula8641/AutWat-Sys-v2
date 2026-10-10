#include "buzzer.h"
#include <Arduino.h>
#include "autwat_config.h"

void buzzerBegin() {
    digitalWrite(autwat::kPinBuzzer, LOW);
    pinMode(autwat::kPinBuzzer, OUTPUT);
}

void buzzerSet(bool on) { digitalWrite(autwat::kPinBuzzer, on ? HIGH : LOW); }
