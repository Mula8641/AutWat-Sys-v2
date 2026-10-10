#pragma once
#include <cstdint>

void wifiLinkBegin(const char* ssid, const char* password);
bool wifiLinkIsUp();
void wifiLinkTick(uint32_t nowMs);
