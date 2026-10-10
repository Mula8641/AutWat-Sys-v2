#include <unity.h>
#include "buzzer_pattern.h"

using namespace autwat;

void setUp() {}
void tearDown() {}

void test_none_is_always_off() {
    TEST_ASSERT_FALSE(buzzerIsOn(Alarm::None, 0));
    TEST_ASSERT_FALSE(buzzerIsOn(Alarm::None, 200));
}
void test_fast_flips_every_200ms() {
    TEST_ASSERT_TRUE(buzzerIsOn(Alarm::Fast, 0));
    TEST_ASSERT_TRUE(buzzerIsOn(Alarm::Fast, 199));
    TEST_ASSERT_FALSE(buzzerIsOn(Alarm::Fast, 200));
    TEST_ASSERT_TRUE(buzzerIsOn(Alarm::Fast, 400));
}
void test_slow_flips_every_1000ms() {
    TEST_ASSERT_TRUE(buzzerIsOn(Alarm::Slow, 999));
    TEST_ASSERT_FALSE(buzzerIsOn(Alarm::Slow, 1000));
    TEST_ASSERT_TRUE(buzzerIsOn(Alarm::Slow, 2000));
}
void test_correct_at_large_time() {
    const uint32_t onboundary = 0xFFFFFFFFu - 0xFFFFFFFFu % 400u;
    TEST_ASSERT_TRUE(buzzerIsOn(Alarm::Fast, onboundary));
    TEST_ASSERT_FALSE(buzzerIsOn(Alarm::Fast, onboundary - 200u));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_none_is_always_off);
    RUN_TEST(test_fast_flips_every_200ms);
    RUN_TEST(test_slow_flips_every_1000ms);
    RUN_TEST(test_correct_at_large_time);
    return UNITY_END();
}
