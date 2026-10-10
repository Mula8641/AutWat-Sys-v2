#include "wifi_link.h"
#include <Arduino.h>
#include <WiFi.h>

namespace {

constexpr uint32_t kRetryPeriodMs = 10000;

const char* wifiSsid = "";
const char* wifiPassword = "";
bool wasUp = false;
uint32_t lastAttemptMs = 0;

}  // namespace

void wifiLinkBegin(const char* ssid, const char* password) {
    wifiSsid = ssid;
    wifiPassword = password;
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiSsid, wifiPassword);
    lastAttemptMs = millis();
}

bool wifiLinkIsUp() { return WiFi.status() == WL_CONNECTED; }

void wifiLinkTick(uint32_t nowMs) {
    const bool isUp = wifiLinkIsUp();
    if (isUp != wasUp) {
        wasUp = isUp;
        Serial.println(isUp ? "[wifi] connected" : "[wifi] down");
    }
    if (!isUp && nowMs - lastAttemptMs >= kRetryPeriodMs) {
        lastAttemptMs = nowMs;
        WiFi.disconnect();
        WiFi.begin(wifiSsid, wifiPassword);
    }
}
