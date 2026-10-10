#include <unity.h>
#include "status_codec.h"

using namespace autwat;

void setUp() {}
void tearDown() {}

static StatusSnapshot baseSnapshot() {
    return StatusSnapshot{"AloeVera", 1, 42, 55, 23, Mode::Scheduled, true, Alarm::Slow, -60, 123};
}

void test_exact_bytes() {
    uint8_t out[kStatusBytes];
    encodeStatus(out, baseSnapshot(), Health::Ok);
    const uint8_t expected[kStatusBytes] = {42, 55, 23, 1, 1, 1, 0, 1};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, out, kStatusBytes);
}
void test_invalid_humidity_is_255() {
    uint8_t out[kStatusBytes];
    StatusSnapshot s = baseSnapshot();
    s.humidityPct = -1;
    encodeStatus(out, s, Health::Ok);
    TEST_ASSERT_EQUAL_UINT8(255, out[1]);
}
void test_invalid_temperature_is_0x80() {
    uint8_t out[kStatusBytes];
    StatusSnapshot s = baseSnapshot();
    s.tempC = -128;
    encodeStatus(out, s, Health::Ok);
    TEST_ASSERT_EQUAL_UINT8(0x80, out[2]);
}
void test_negative_temperature_is_int8() {
    uint8_t out[kStatusBytes];
    StatusSnapshot s = baseSnapshot();
    s.tempC = -5;
    encodeStatus(out, s, Health::Ok);
    TEST_ASSERT_EQUAL_INT8(-5, static_cast<int8_t>(out[2]));
}
void test_modes_map_to_documented_numbers() {
    const Mode modes[] = {Mode::Idle, Mode::Scheduled, Mode::Emergency, Mode::Lockout, Mode::Fault};
    for (uint8_t i = 0; i < 5; ++i) {
        uint8_t out[kStatusBytes];
        StatusSnapshot s = baseSnapshot();
        s.mode = modes[i];
        encodeStatus(out, s, Health::Ok);
        TEST_ASSERT_EQUAL_UINT8(i, out[3]);
    }
}
void test_alarms_map_to_documented_numbers() {
    const Alarm alarms[] = {Alarm::None, Alarm::Slow, Alarm::Fast};
    for (uint8_t i = 0; i < 3; ++i) {
        uint8_t out[kStatusBytes];
        StatusSnapshot s = baseSnapshot();
        s.alarm = alarms[i];
        encodeStatus(out, s, Health::Ok);
        TEST_ASSERT_EQUAL_UINT8(i, out[5]);
    }
}
void test_health_maps_to_documented_numbers() {
    const Health values[] = {Health::Ok, Health::Dry, Health::Overwatered};
    for (uint8_t i = 0; i < 3; ++i) {
        uint8_t out[kStatusBytes];
        encodeStatus(out, baseSnapshot(), values[i]);
        TEST_ASSERT_EQUAL_UINT8(i, out[6]);
    }
}
void test_pump_off_and_plant_index() {
    uint8_t out[kStatusBytes];
    StatusSnapshot s = baseSnapshot();
    s.pumpOn = false;
    s.plantIndex = 2;
    encodeStatus(out, s, Health::Ok);
    TEST_ASSERT_EQUAL_UINT8(0, out[4]);
    TEST_ASSERT_EQUAL_UINT8(2, out[7]);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_exact_bytes);
    RUN_TEST(test_invalid_humidity_is_255);
    RUN_TEST(test_invalid_temperature_is_0x80);
    RUN_TEST(test_negative_temperature_is_int8);
    RUN_TEST(test_modes_map_to_documented_numbers);
    RUN_TEST(test_alarms_map_to_documented_numbers);
    RUN_TEST(test_health_maps_to_documented_numbers);
    RUN_TEST(test_pump_off_and_plant_index);
    return UNITY_END();
}
