#include <Arduino.h>
#include <WiFi.h>
#include "boot/calibration_run.h"
#include "boot/plant_selection.h"
#include "button_decoder.h"
#include "buzzer_pattern.h"
#include "command.h"
#include "autwat_config.h"
#include "hal/air_sensor.h"
#include "hal/button.h"
#include "hal/buzzer.h"
#include "hal/display.h"
#include "hal/moisture_sensor.h"
#include "hal/pump.h"
#include "moisture_scale.h"
#include "net/ble_link.h"
#include "net/status_server.h"
#include "net/wifi_link.h"
#include "screen_text.h"
#include "secrets.h"
#include "status_codec.h"
#include "status_json.h"
#include "watering_controller.h"

using namespace autwat;

namespace {

const ControllerConfig kControllerConfig = {kScheduleIntervalMs, kFlowRateMlPerSec, kEmergencyMaxMs};

ControllerState controllerState = {Mode::Idle, 0, 0};
ButtonState buttonState = {0, false, false};
uint8_t plantIndex = 0;
uint32_t lastTickMs = 0;
bool pumpRunning = false;

Command readCommand(uint32_t nowMs) {
    const ButtonEvent event = decodeButton(buttonState, buttonIsDown(), nowMs);
    return pickCommand(commandForButton(event, pumpRunning), bleLinkTakeCommand(), statusServerTakeCommand());
}

void logModeChange(Mode before, Mode after, uint8_t moisturePct) {
    if (before != after) {
        Serial.printf("[ctrl] %s -> %s (moisture=%u%%)\n", modeName(before), modeName(after),
                      static_cast<unsigned>(moisturePct));
    }
}

StatusSnapshot buildSnapshot(uint8_t moisturePct, const AirReading& air, const ControllerOutput& output,
                             uint32_t nowMs) {
    const int8_t rssi = wifiLinkIsUp() ? static_cast<int8_t>(WiFi.RSSI()) : 0;
    return StatusSnapshot{plantAt(plantIndex).name,
                          plantIndex,
                          moisturePct,
                          air.valid ? air.humidityPct : kInvalidHumidity,
                          air.valid ? air.tempC : kInvalidTemp,
                          output.mode,
                          output.pumpOn,
                          output.alarm,
                          rssi,
                          nowMs / 1000};
}

void showScreen(const StatusSnapshot& snapshot) {
    char sensorLine[kScreenBufferSize];
    char modeLine[kScreenBufferSize];
    char linkLine[kScreenBufferSize];
    formatSensorLine(sensorLine, snapshot.moisturePct, snapshot.humidityPct, snapshot.tempC);
    formatModeLine(modeLine, snapshot.mode);
    formatLinkLine(linkLine, wifiLinkIsUp(), bleLinkIsConnected());
    displayShow(snapshot.plantName, sensorLine, modeLine, linkLine);
}

void publishStatus(const StatusSnapshot& snapshot) {
    uint8_t payload[kStatusBytes];
    encodeStatus(payload, snapshot, healthFor(snapshot.moisturePct, plantAt(snapshot.plantIndex)));
    statusServerPublish(snapshot);
    bleLinkPublish(payload);
}

void controlTick(uint32_t nowMs) {
    const uint8_t moisturePct = adcToPercent(moistureSensorReadRaw(), kRawDry, kRawWet);
    airSensorTick(nowMs);
    const Mode before = controllerState.mode;
    const ControllerOutput output = stepController(
        controllerState, ControllerInput{moisturePct, nowMs, readCommand(nowMs)}, plantAt(plantIndex),
        kControllerConfig);
    logModeChange(before, output.mode, moisturePct);
    pumpRunning = output.pumpOn;
    pumpSet(output.pumpOn);
    buzzerSet(buzzerIsOn(output.alarm, nowMs));
    const StatusSnapshot snapshot = buildSnapshot(moisturePct, airSensorLatest(), output, nowMs);
    showScreen(snapshot);
    publishStatus(snapshot);
    wifiLinkTick(nowMs);
    statusServerTick();
}

}  // namespace

void setup() {
    Serial.begin(115200);
    pumpBegin();
    buzzerBegin();
    buttonBegin();
    moistureSensorBegin();
    airSensorBegin();
    displayBegin();
    Serial.println("[boot] AutWat start");
    displayShow("   AutWat System   ", "", "", "");
    const uint32_t splashStartMs = millis();
    while (millis() - splashStartMs < 2000) {
    }
    plantIndex = selectPlant();
    runCalibrationPump();
    wifiLinkBegin(kWifiSsid, kWifiPassword);
    statusServerBegin();
    bleLinkBegin();
    const uint32_t nowMs = millis();
    controllerState = ControllerState{Mode::Idle, nowMs, nowMs};
    lastTickMs = nowMs;
}

void loop() {
    const uint32_t nowMs = millis();
    if (nowMs - lastTickMs >= kControlTickMs) {
        lastTickMs = nowMs;
        controlTick(nowMs);
    }
}
