#include "ble_link.h"
#include <Arduino.h>
#include <NimBLEDevice.h>
#include "autwat_config.h"

using namespace autwat;

namespace {

constexpr uint32_t kNotifyPeriodMs = 2000;

NimBLECharacteristic* statusCharacteristic = nullptr;
bool connected = false;
Command pending = Command::None;
uint32_t lastNotifyMs = 0;

class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer*, NimBLEConnInfo&) override {
        connected = true;
        Serial.println("[ble] connected");
    }
    void onDisconnect(NimBLEServer*, NimBLEConnInfo&, int) override {
        connected = false;
        Serial.println("[ble] disconnected, advertising");
        NimBLEDevice::startAdvertising();
    }
};

class CommandCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* characteristic, NimBLEConnInfo&) override {
        const NimBLEAttValue value = characteristic->getValue();
        if (value.size() == 1) {
            pending = decodeCommandByte(value[0]);
            Serial.printf("[ble] command byte: %u\n", static_cast<unsigned>(value[0]));
        }
    }
};

ServerCallbacks serverCallbacks;
CommandCallbacks commandCallbacks;

}  // namespace

void bleLinkBegin() {
    NimBLEDevice::init(kBleDeviceName);
    NimBLEServer* server = NimBLEDevice::createServer();
    server->setCallbacks(&serverCallbacks);
    NimBLEService* service = server->createService(kBleServiceUuid);
    statusCharacteristic =
        service->createCharacteristic(kBleStatusUuid, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    NimBLECharacteristic* command = service->createCharacteristic(kBleCommandUuid, NIMBLE_PROPERTY::WRITE);
    command->setCallbacks(&commandCallbacks);
    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    advertising->addServiceUUID(kBleServiceUuid);
    advertising->setName(kBleDeviceName);
    advertising->start();
    Serial.println("[ble] init done, advertising as AutWat");
}

bool bleLinkIsConnected() { return connected; }

void bleLinkPublish(const uint8_t status[kStatusBytes]) {
    statusCharacteristic->setValue(status, kStatusBytes);
    const uint32_t nowMs = millis();
    if (connected && nowMs - lastNotifyMs >= kNotifyPeriodMs) {
        lastNotifyMs = nowMs;
        statusCharacteristic->notify();
    }
}

Command bleLinkTakeCommand() {
    const Command taken = pending;
    pending = Command::None;
    return taken;
}
