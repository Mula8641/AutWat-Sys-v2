#include "pump.h"
#include <Arduino.h>
#include "autwat_config.h"

namespace {

uint8_t levelFor(bool on) { return (on != autwat::kRelayActiveLow) ? HIGH : LOW; }

}  // namespace

void pumpBegin() {
    digitalWrite(autwat::kPinRelay, levelFor(false));  // before pinMode, so the relay does not pulse at boot
    pinMode(autwat::kPinRelay, OUTPUT);
}

void pumpSet(bool on) { digitalWrite(autwat::kPinRelay, levelFor(on)); }
