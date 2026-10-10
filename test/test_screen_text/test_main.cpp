#include <cstring>
#include <unity.h>
#include "screen_text.h"

using namespace autwat;

void setUp() {}
void tearDown() {}

static char line[kScreenBufferSize];

static void expectLine(const char* expectedPrefix) {
    TEST_ASSERT_EQUAL(kScreenWidth, strlen(line));
    TEST_ASSERT_EQUAL_INT(0, strncmp(line, expectedPrefix, strlen(expectedPrefix)));
    for (size_t i = strlen(expectedPrefix); i < kScreenWidth; ++i) TEST_ASSERT_EQUAL_CHAR(' ', line[i]);
}

void test_sensor_line_normal() {
    formatSensorLine(line, 42, 55, 23);
    expectLine("M:42% H:55% T:23C");
}
void test_sensor_line_invalid_humidity_and_temp() {
    formatSensorLine(line, 42, -1, -128);
    expectLine("M:42% H:-- T:--");
}
void test_sensor_line_moisture_extremes() {
    formatSensorLine(line, 0, 55, 23);
    expectLine("M:0% H:55% T:23C");
    formatSensorLine(line, 7, 55, 23);
    expectLine("M:7% H:55% T:23C");
    formatSensorLine(line, 100, 100, -10);
    expectLine("M:100% H:100% T:-10C");
}
void test_mode_lines() {
    formatModeLine(line, Mode::Idle);
    expectLine("Idle");
    formatModeLine(line, Mode::Scheduled);
    expectLine("Watering");
    formatModeLine(line, Mode::Emergency);
    expectLine("Dry! Emergency");
    formatModeLine(line, Mode::Lockout);
    expectLine("Too wet! Locked");
    formatModeLine(line, Mode::Fault);
    expectLine("FAULT: press button");
}
void test_plant_line_longest_name_fits() {
    formatPlantLine(line, plantAt(2));
    expectLine("ZamioculcasZamifolia");
}
void test_plant_line_short_name_padded() {
    formatPlantLine(line, plantAt(0));
    expectLine("Cactus");
}
void test_link_lines() {
    formatLinkLine(line, true, true);
    expectLine("WiFi:ON  BLE:CONN");
    formatLinkLine(line, false, false);
    expectLine("WiFi:OFF BLE:ADV");
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_sensor_line_normal);
    RUN_TEST(test_sensor_line_invalid_humidity_and_temp);
    RUN_TEST(test_sensor_line_moisture_extremes);
    RUN_TEST(test_mode_lines);
    RUN_TEST(test_plant_line_longest_name_fits);
    RUN_TEST(test_plant_line_short_name_padded);
    RUN_TEST(test_link_lines);
    return UNITY_END();
}
