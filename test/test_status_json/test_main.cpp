#include <cstring>
#include <unity.h>
#include "status_json.h"

using namespace autwat;

void setUp() {}
void tearDown() {}

static StatusSnapshot cactusSnapshot() {
    return StatusSnapshot{"AloeVera", 1, 42, 55, 23, Mode::Idle, false, Alarm::None, -60, 123};
}

void test_exact_json() {
    char out[256];
    int n = writeStatusJson(out, sizeof(out), cactusSnapshot());
    const char* expected =
        "{\"plant\":\"AloeVera\",\"moisture_pct\":42,\"humidity_pct\":55,\"temp_c\":23,\"health\":\"ok\","
        "\"mode\":\"idle\",\"pump\":false,\"alarm\":\"none\",\"wifi_rssi\":-60,\"uptime_s\":123}";
    TEST_ASSERT_EQUAL_STRING(expected, out);
    TEST_ASSERT_EQUAL(strlen(expected), n);
}
void test_invalid_readings_are_null() {
    char out[256];
    StatusSnapshot s = cactusSnapshot();
    s.humidityPct = -1;
    s.tempC = -128;
    writeStatusJson(out, sizeof(out), s);
    TEST_ASSERT_NOT_NULL(strstr(out, "\"humidity_pct\":null,\"temp_c\":null"));
}
void test_running_dry_snapshot_fields() {
    char out[256];
    StatusSnapshot s = cactusSnapshot();
    s.moisturePct = 5;
    s.mode = Mode::Emergency;
    s.pumpOn = true;
    s.alarm = Alarm::Fast;
    writeStatusJson(out, sizeof(out), s);
    TEST_ASSERT_NOT_NULL(strstr(out, "\"health\":\"dry\",\"mode\":\"emergency\",\"pump\":true,\"alarm\":\"fast\""));
}
void test_health_boundaries() {
    const PlantProfile& cactus = plantAt(0);
    TEST_ASSERT_EQUAL(Health::Dry, healthFor(9, cactus));
    TEST_ASSERT_EQUAL(Health::Ok, healthFor(10, cactus));
    TEST_ASSERT_EQUAL(Health::Ok, healthFor(40, cactus));
    TEST_ASSERT_EQUAL(Health::Overwatered, healthFor(41, cactus));
}
void test_too_small_buffer_returns_minus_one_and_stays_in_bounds() {
    char buffer[16 + 4];
    memset(buffer, 'X', sizeof(buffer));
    int n = writeStatusJson(buffer, 16, cactusSnapshot());
    TEST_ASSERT_EQUAL(-1, n);
    for (int i = 16; i < 20; ++i) TEST_ASSERT_EQUAL_CHAR('X', buffer[i]);
}
void test_zero_size_buffer_returns_minus_one() {
    char c = 'X';
    TEST_ASSERT_EQUAL(-1, writeStatusJson(&c, 0, cactusSnapshot()));
    TEST_ASSERT_EQUAL_CHAR('X', c);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_exact_json);
    RUN_TEST(test_invalid_readings_are_null);
    RUN_TEST(test_running_dry_snapshot_fields);
    RUN_TEST(test_health_boundaries);
    RUN_TEST(test_too_small_buffer_returns_minus_one_and_stays_in_bounds);
    RUN_TEST(test_zero_size_buffer_returns_minus_one);
    return UNITY_END();
}
