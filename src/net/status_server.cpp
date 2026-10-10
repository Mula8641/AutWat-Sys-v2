#include "status_server.h"
#include <Arduino.h>
#include <WebServer.h>

using namespace autwat;

namespace {

constexpr size_t kJsonBufferSize = 256;

WebServer server(80);
StatusSnapshot latest = {"", 0, 0, -1, -128, Mode::Idle, false, Alarm::None, 0, 0};
Command pending = Command::None;

void handleStatus() {
    char json[kJsonBufferSize];
    if (writeStatusJson(json, sizeof(json), latest) < 0) {
        Serial.println("[http] status buffer too small");
        server.send(500, "text/plain", "status buffer too small");
    } else {
        server.send(200, "application/json", json);
    }
}

void handleCommand(Command command, const char* name) {
    pending = command;
    Serial.printf("[http] command: %s\n", name);
    server.send(200, "application/json", "{\"ok\":true}");
}

}  // namespace

void statusServerBegin() {
    server.on("/status", HTTP_GET, handleStatus);
    server.on("/water", HTTP_POST, [] { handleCommand(Command::WaterNow, "water"); });
    server.on("/stop", HTTP_POST, [] { handleCommand(Command::Stop, "stop"); });
    server.begin();
}

void statusServerTick() { server.handleClient(); }

void statusServerPublish(const StatusSnapshot& snapshot) { latest = snapshot; }

Command statusServerTakeCommand() {
    const Command taken = pending;
    pending = Command::None;
    return taken;
}
