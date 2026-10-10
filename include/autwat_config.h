#pragma once
#include <cstdint>

namespace autwat {

constexpr uint8_t kPinMoisture = 34;
constexpr uint8_t kPinRelay = 26;
constexpr uint8_t kPinBuzzer = 27;
constexpr uint8_t kPinButton = 25;
constexpr uint8_t kPinDht = 23;
constexpr uint8_t kPinLcdRs = 16;
constexpr uint8_t kPinLcdEnable = 17;
constexpr uint8_t kPinLcdD4 = 18;
constexpr uint8_t kPinLcdD5 = 19;
constexpr uint8_t kPinLcdD6 = 21;
constexpr uint8_t kPinLcdD7 = 22;

// Real SRD relay modules are low level trigger; reconfirm against the Wokwi relay (task 3.2).
constexpr bool kRelayActiveLow = true;

constexpr uint16_t kRawDry = 0;
constexpr uint16_t kRawWet = 4095;

constexpr uint32_t kShortPressMs = 200;
constexpr uint32_t kLongPressMs = 2000;
constexpr uint32_t kCalibrationRunMs = 5000;
constexpr uint32_t kControlTickMs = 100;
constexpr uint32_t kAirSensorPeriodMs = 2000;
constexpr float kFlowRateMlPerSec = 17.54f;
constexpr uint32_t kEmergencyMaxMs = 60000;

constexpr int16_t kInvalidHumidity = -1;
constexpr int16_t kInvalidTemp = -128;

constexpr const char* kBleDeviceName = "AutWat";
constexpr const char* kBleServiceUuid = "23f892a4-78c9-4eaa-bab5-ac4d6f11b74f";
constexpr const char* kBleStatusUuid = "d0331ccb-c3e7-4bda-bd21-229236bad932";
constexpr const char* kBleCommandUuid = "49dc7bbb-ebd5-4830-9c89-7a3a0f1047e3";

#ifdef SIMULATED_TIME
constexpr uint32_t kDayMs = 10000;  // 1 simulated day is 10 s, same as the original and the report
#else
constexpr uint32_t kDayMs = 86400000UL;
#endif
constexpr uint32_t kScheduleIntervalMs = 3 * kDayMs;

}  // namespace autwat
