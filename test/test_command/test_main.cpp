#include <unity.h>
#include "command.h"

using namespace autwat;

void setUp() {}
void tearDown() {}

void test_parse_accepts_water_any_case() {
    TEST_ASSERT_EQUAL(Command::WaterNow, parseCommandText("water"));
    TEST_ASSERT_EQUAL(Command::WaterNow, parseCommandText("WATER"));
}
void test_parse_accepts_stop_any_case() { TEST_ASSERT_EQUAL(Command::Stop, parseCommandText("Stop")); }
void test_parse_rejects_bad_text() {
    TEST_ASSERT_EQUAL(Command::None, parseCommandText(""));
    TEST_ASSERT_EQUAL(Command::None, parseCommandText("wat"));
    TEST_ASSERT_EQUAL(Command::None, parseCommandText("water now"));
    TEST_ASSERT_EQUAL(Command::None, parseCommandText(nullptr));
}
void test_decode_byte() {
    TEST_ASSERT_EQUAL(Command::None, decodeCommandByte(0));
    TEST_ASSERT_EQUAL(Command::WaterNow, decodeCommandByte(1));
    TEST_ASSERT_EQUAL(Command::Stop, decodeCommandByte(2));
    TEST_ASSERT_EQUAL(Command::None, decodeCommandByte(3));
}
void test_pick_prefers_button_then_ble_then_http() {
    TEST_ASSERT_EQUAL(Command::Stop, pickCommand(Command::Stop, Command::WaterNow, Command::WaterNow));
    TEST_ASSERT_EQUAL(Command::WaterNow, pickCommand(Command::None, Command::WaterNow, Command::Stop));
    TEST_ASSERT_EQUAL(Command::Stop, pickCommand(Command::None, Command::None, Command::Stop));
}
void test_pick_all_none() { TEST_ASSERT_EQUAL(Command::None, pickCommand(Command::None, Command::None, Command::None)); }
void test_short_press_toggles() {
    TEST_ASSERT_EQUAL(Command::Stop, commandForButton(ButtonEvent::ShortPress, true));
    TEST_ASSERT_EQUAL(Command::WaterNow, commandForButton(ButtonEvent::ShortPress, false));
}
void test_other_button_events_give_none() {
    TEST_ASSERT_EQUAL(Command::None, commandForButton(ButtonEvent::LongPress, true));
    TEST_ASSERT_EQUAL(Command::None, commandForButton(ButtonEvent::None, false));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_parse_accepts_water_any_case);
    RUN_TEST(test_parse_accepts_stop_any_case);
    RUN_TEST(test_parse_rejects_bad_text);
    RUN_TEST(test_decode_byte);
    RUN_TEST(test_pick_prefers_button_then_ble_then_http);
    RUN_TEST(test_pick_all_none);
    RUN_TEST(test_short_press_toggles);
    RUN_TEST(test_other_button_events_give_none);
    return UNITY_END();
}
