#include <unity.h>
#include "button_decoder.h"

using namespace autwat;

static ButtonState state;

void setUp() { state = ButtonState{0, false, false}; }
void tearDown() {}

// Presses at pressAtMs, samples every 50 ms until releaseAtMs, then releases. Returns events seen.
static void holdAndRelease(uint32_t pressAtMs, uint32_t releaseAtMs, int& shortCount, int& longCount) {
    for (uint32_t offset = 0; offset < releaseAtMs - pressAtMs; offset += 50) {
        ButtonEvent e = decodeButton(state, true, pressAtMs + offset);
        shortCount += e == ButtonEvent::ShortPress;
        longCount += e == ButtonEvent::LongPress;
    }
    ButtonEvent e = decodeButton(state, false, releaseAtMs);
    shortCount += e == ButtonEvent::ShortPress;
    longCount += e == ButtonEvent::LongPress;
}

void test_tap_under_200ms_is_ignored() {
    int s = 0, l = 0;
    holdAndRelease(1000, 1100, s, l);
    TEST_ASSERT_EQUAL(0, s);
    TEST_ASSERT_EQUAL(0, l);
}
void test_500ms_press_is_short_press() {
    int s = 0, l = 0;
    holdAndRelease(1000, 1500, s, l);
    TEST_ASSERT_EQUAL(1, s);
    TEST_ASSERT_EQUAL(0, l);
}
void test_long_hold_fires_once_while_held() {
    int s = 0, l = 0;
    for (uint32_t t = 1000; t <= 3100; t += 50) {
        l += decodeButton(state, true, t) == ButtonEvent::LongPress;
    }
    TEST_ASSERT_EQUAL(1, l);
    (void)s;
}
void test_release_after_long_press_is_none() {
    int s = 0, l = 0;
    holdAndRelease(1000, 3100, s, l);
    TEST_ASSERT_EQUAL(1, l);
    TEST_ASSERT_EQUAL(0, s);
}
void test_two_short_presses_give_two_events() {
    int s = 0, l = 0;
    holdAndRelease(1000, 1400, s, l);
    holdAndRelease(2000, 2400, s, l);
    TEST_ASSERT_EQUAL(2, s);
}
void test_idle_released_button_gives_none() {
    TEST_ASSERT_EQUAL(ButtonEvent::None, decodeButton(state, false, 5000));
}
void test_press_works_across_millis_overflow() {
    int s = 0, l = 0;
    holdAndRelease(0xFFFFFF00u, 0xFFFFFF00u + 500u, s, l);
    TEST_ASSERT_EQUAL(1, s);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_tap_under_200ms_is_ignored);
    RUN_TEST(test_500ms_press_is_short_press);
    RUN_TEST(test_long_hold_fires_once_while_held);
    RUN_TEST(test_release_after_long_press_is_none);
    RUN_TEST(test_two_short_presses_give_two_events);
    RUN_TEST(test_idle_released_button_gives_none);
    RUN_TEST(test_press_works_across_millis_overflow);
    return UNITY_END();
}
