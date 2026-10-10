#include <unity.h>
#include "moisture_scale.h"

using namespace autwat;

void setUp() {}
void tearDown() {}

void test_dry_end_is_zero() { TEST_ASSERT_EQUAL_UINT8(0, adcToPercent(0, 0, 4095)); }
void test_wet_end_is_hundred() { TEST_ASSERT_EQUAL_UINT8(100, adcToPercent(4095, 0, 4095)); }
void test_midpoint_is_about_fifty() { TEST_ASSERT_UINT8_WITHIN(1, 50, adcToPercent(2048, 0, 4095)); }
void test_above_wet_clamps_to_hundred() { TEST_ASSERT_EQUAL_UINT8(100, adcToPercent(4095, 0, 3000)); }
void test_below_dry_clamps_to_zero() { TEST_ASSERT_EQUAL_UINT8(0, adcToPercent(500, 1000, 3000)); }
void test_inverted_sensor_dry_end() { TEST_ASSERT_EQUAL_UINT8(0, adcToPercent(3000, 3000, 1200)); }
void test_inverted_sensor_wet_end() { TEST_ASSERT_EQUAL_UINT8(100, adcToPercent(1200, 3000, 1200)); }
void test_inverted_sensor_midpoint() { TEST_ASSERT_EQUAL_UINT8(50, adcToPercent(2100, 3000, 1200)); }
void test_inverted_sensor_clamps() {
    TEST_ASSERT_EQUAL_UINT8(100, adcToPercent(100, 3000, 1200));
    TEST_ASSERT_EQUAL_UINT8(0, adcToPercent(4000, 3000, 1200));
}
void test_equal_calibration_points_return_zero() { TEST_ASSERT_EQUAL_UINT8(0, adcToPercent(2000, 2000, 2000)); }

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_dry_end_is_zero);
    RUN_TEST(test_wet_end_is_hundred);
    RUN_TEST(test_midpoint_is_about_fifty);
    RUN_TEST(test_above_wet_clamps_to_hundred);
    RUN_TEST(test_below_dry_clamps_to_zero);
    RUN_TEST(test_inverted_sensor_dry_end);
    RUN_TEST(test_inverted_sensor_wet_end);
    RUN_TEST(test_inverted_sensor_midpoint);
    RUN_TEST(test_inverted_sensor_clamps);
    RUN_TEST(test_equal_calibration_points_return_zero);
    return UNITY_END();
}
