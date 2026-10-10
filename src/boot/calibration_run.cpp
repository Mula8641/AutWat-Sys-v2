#include "calibration_run.h"
#include <Arduino.h>
#include "autwat_config.h"
#include "hal/display.h"
#include "hal/pump.h"

// Blocking by design: boot only, the pump primes the tubing before control starts.
void runCalibrationPump() {
    displayShow("Calibrating pump...", "", "", "");
    Serial.println("[boot] calibration run start");
    const uint32_t startMs = millis();
    pumpSet(true);
    while (millis() - startMs < autwat::kCalibrationRunMs) {
    }
    pumpSet(false);
    Serial.println("[boot] calibration run done");
}
