#include <unity.h>
#include "dose_calculator.h"
#include "watering_controller.h"

using namespace autwat;

namespace {

constexpr uint32_t kInterval = 30000;
constexpr uint32_t kEmergencyMax = 60000;
const ControllerConfig kConfig = {kInterval, 17.54f, kEmergencyMax};

ControllerState state;

const PlantProfile& cactus() { return plantAt(0); }  // 10 / 20 / 40, 300 mL
const PlantProfile& aloe() { return plantAt(1); }    // 10 / 30 / 50, 300 mL
const PlantProfile& zz() { return plantAt(2); }      // 15 / 40 / 70, 250 mL

ControllerOutput step(const PlantProfile& plant, uint8_t moisture, uint32_t now, Command cmd = Command::None) {
    return stepController(state, ControllerInput{moisture, now, cmd}, plant, kConfig);
}

}  // namespace

void setUp() { state = ControllerState{Mode::Idle, 0, 0}; }
void tearDown() {}

void test_rule1_above_critical_high_locks_out() {
    step(cactus(), 41, 100);
    TEST_ASSERT_EQUAL(Mode::Lockout, state.mode);
}
void test_rule2_lockout_holds_above_normal_high() {
    step(cactus(), 41, 100);
    step(cactus(), 30, 200);
    TEST_ASSERT_EQUAL(Mode::Lockout, state.mode);
}
void test_rule3_lockout_leaves_at_normal_high_and_resets_clock() {
    step(cactus(), 41, 100);
    step(cactus(), 20, 5000);
    TEST_ASSERT_EQUAL(Mode::Idle, state.mode);
    TEST_ASSERT_EQUAL_UINT32(5000, state.lastWateredMs);
}
void test_rule4_stop_ends_run_and_resets_clock() {
    step(aloe(), 25, 0, Command::WaterNow);
    TEST_ASSERT_EQUAL(Mode::Scheduled, state.mode);
    step(aloe(), 25, 4000, Command::Stop);
    TEST_ASSERT_EQUAL(Mode::Idle, state.mode);
    TEST_ASSERT_EQUAL_UINT32(4000, state.lastWateredMs);
}
void test_rule5_fault_holds() {
    state = ControllerState{Mode::Fault, 0, 0};
    step(cactus(), 25, 100);
    TEST_ASSERT_EQUAL(Mode::Fault, state.mode);
}
void test_rule6_emergency_timeout_becomes_fault() {
    step(cactus(), 5, 0);
    step(cactus(), 5, kEmergencyMax - 1);
    TEST_ASSERT_EQUAL(Mode::Emergency, state.mode);
    step(cactus(), 5, kEmergencyMax);
    TEST_ASSERT_EQUAL(Mode::Fault, state.mode);
}
void test_rule6_only_stop_clears_fault() {
    state = ControllerState{Mode::Fault, 0, 0};
    step(cactus(), 25, 100, Command::WaterNow);
    TEST_ASSERT_EQUAL(Mode::Fault, state.mode);
    step(cactus(), 25, 200, Command::Stop);
    TEST_ASSERT_EQUAL(Mode::Idle, state.mode);
}
void test_rule7_below_critical_low_enters_emergency_and_sets_start() {
    step(cactus(), 9, 777);
    TEST_ASSERT_EQUAL(Mode::Emergency, state.mode);
    TEST_ASSERT_EQUAL_UINT32(777, state.modeStartMs);
}
void test_rule7_start_not_reset_while_staying_in_emergency() {
    step(cactus(), 9, 777);
    step(cactus(), 9, 1500);
    TEST_ASSERT_EQUAL_UINT32(777, state.modeStartMs);
}
void test_rule8_emergency_holds_between_low_and_normal_high() {
    step(cactus(), 5, 0);
    step(cactus(), 15, 100);
    TEST_ASSERT_EQUAL(Mode::Emergency, state.mode);
}
void test_rule9_emergency_leaves_at_normal_high() {
    step(cactus(), 5, 0);
    step(cactus(), 20, 900);
    TEST_ASSERT_EQUAL(Mode::Idle, state.mode);
    TEST_ASSERT_EQUAL_UINT32(900, state.lastWateredMs);
}
void test_rule10_water_now_in_idle_starts_run() {
    step(aloe(), 25, 1234, Command::WaterNow);
    TEST_ASSERT_EQUAL(Mode::Scheduled, state.mode);
    TEST_ASSERT_EQUAL_UINT32(1234, state.modeStartMs);
}
void test_rule11_run_finishes_after_exact_duration_for_every_plant() {
    const PlantProfile* plants[] = {&cactus(), &aloe(), &zz()};
    const uint8_t moisture[] = {15, 25, 30};  // inside each plant's normal band
    for (int i = 0; i < 3; ++i) {
        setUp();
        const uint32_t runMs = runMsFor(plants[i]->waterMl, kConfig.flowRateMlPerSec);
        step(*plants[i], moisture[i], 1000, Command::WaterNow);
        step(*plants[i], moisture[i], 1000 + runMs - 1);
        TEST_ASSERT_EQUAL(Mode::Scheduled, state.mode);
        step(*plants[i], moisture[i], 1000 + runMs);
        TEST_ASSERT_EQUAL(Mode::Idle, state.mode);
        TEST_ASSERT_EQUAL_UINT32(1000 + runMs, state.lastWateredMs);
    }
}
void test_rule12_scheduled_holds_mid_run() {
    step(aloe(), 25, 0, Command::WaterNow);
    step(aloe(), 25, 5000);
    TEST_ASSERT_EQUAL(Mode::Scheduled, state.mode);
}
void test_rule13_due_schedule_with_wet_soil_is_skipped() {
    step(cactus(), 25, kInterval);
    TEST_ASSERT_EQUAL(Mode::Idle, state.mode);
    TEST_ASSERT_EQUAL_UINT32(kInterval, state.lastWateredMs);
}
void test_rule14_schedule_fires_exactly_at_interval() {
    step(aloe(), 25, kInterval - 1);
    TEST_ASSERT_EQUAL(Mode::Idle, state.mode);
    step(aloe(), 25, kInterval);
    TEST_ASSERT_EQUAL(Mode::Scheduled, state.mode);
    TEST_ASSERT_EQUAL_UINT32(kInterval, state.modeStartMs);
}
void test_rule15_idle_otherwise_stays_idle() {
    step(aloe(), 25, 1000);
    TEST_ASSERT_EQUAL(Mode::Idle, state.mode);
    TEST_ASSERT_EQUAL_UINT32(0, state.lastWateredMs);
}
void test_outputs_for_each_mode() {
    ControllerOutput out = step(aloe(), 25, 1000);
    TEST_ASSERT_FALSE(out.pumpOn);
    TEST_ASSERT_EQUAL(Alarm::None, out.alarm);
    out = step(aloe(), 25, 1100, Command::WaterNow);
    TEST_ASSERT_TRUE(out.pumpOn);
    TEST_ASSERT_EQUAL(Alarm::Slow, out.alarm);
    out = step(aloe(), 5, 1200);
    TEST_ASSERT_TRUE(out.pumpOn);
    TEST_ASSERT_EQUAL(Alarm::Fast, out.alarm);
    out = step(aloe(), 60, 1300);
    TEST_ASSERT_FALSE(out.pumpOn);
    TEST_ASSERT_EQUAL(Alarm::Fast, out.alarm);
    state = ControllerState{Mode::Fault, 0, 0};
    out = step(aloe(), 25, 1400);
    TEST_ASSERT_FALSE(out.pumpOn);
    TEST_ASSERT_EQUAL(Alarm::Fast, out.alarm);
    TEST_ASSERT_EQUAL(Mode::Fault, out.mode);
}
void test_water_now_ignored_in_lockout_emergency_and_fault() {
    step(cactus(), 41, 100, Command::WaterNow);
    TEST_ASSERT_EQUAL(Mode::Lockout, state.mode);
    setUp();
    step(cactus(), 5, 100);
    step(cactus(), 15, 200, Command::WaterNow);
    TEST_ASSERT_EQUAL(Mode::Emergency, state.mode);
    state = ControllerState{Mode::Fault, 0, 0};
    step(cactus(), 25, 300, Command::WaterNow);
    TEST_ASSERT_EQUAL(Mode::Fault, state.mode);
}
void test_water_now_cannot_water_overwatered_plant() {
    const ControllerOutput out = step(zz(), 71, 100, Command::WaterNow);
    TEST_ASSERT_FALSE(out.pumpOn);
}
void test_emergency_to_idle_only_at_normal_high() {
    step(aloe(), 5, 0);
    step(aloe(), 29, 100);
    TEST_ASSERT_EQUAL(Mode::Emergency, state.mode);
    step(aloe(), 30, 200);
    TEST_ASSERT_EQUAL(Mode::Idle, state.mode);
}
void test_lockout_holds_until_normal_high() {
    step(aloe(), 51, 0);
    step(aloe(), 31, 100);
    TEST_ASSERT_EQUAL(Mode::Lockout, state.mode);
    step(aloe(), 30, 200);
    TEST_ASSERT_EQUAL(Mode::Idle, state.mode);
}
void test_each_plant_uses_its_own_thresholds() {
    step(cactus(), 12, 0);
    TEST_ASSERT_EQUAL(Mode::Idle, state.mode);
    setUp();
    step(zz(), 12, 0);
    TEST_ASSERT_EQUAL(Mode::Emergency, state.mode);
    setUp();
    step(aloe(), 45, 0);
    TEST_ASSERT_EQUAL(Mode::Idle, state.mode);
    setUp();
    step(cactus(), 45, 0);
    TEST_ASSERT_EQUAL(Mode::Lockout, state.mode);
}
void test_millis_overflow_schedule_elapsed_time() {
    state = ControllerState{Mode::Idle, 0, 0xFFFFFF00u};
    step(aloe(), 25, 0x000000FFu);
    TEST_ASSERT_EQUAL(Mode::Idle, state.mode);  // only 511 ms elapsed
    ControllerConfig shortInterval = {500, 17.54f, kEmergencyMax};
    stepController(state, ControllerInput{25, 0x000000FFu, Command::None}, aloe(), shortInterval);
    TEST_ASSERT_EQUAL(Mode::Scheduled, state.mode);  // 511 >= 500
}
void test_millis_overflow_run_duration() {
    const uint32_t start = 0xFFFFFF00u;
    step(aloe(), 25, start, Command::WaterNow);
    step(aloe(), 25, start + 17000);
    TEST_ASSERT_EQUAL(Mode::Scheduled, state.mode);
    step(aloe(), 25, start + 17104);
    TEST_ASSERT_EQUAL(Mode::Idle, state.mode);
}
void test_stop_in_idle_resets_schedule_clock() {
    step(aloe(), 25, 20000, Command::Stop);
    step(aloe(), 25, 20000 + kInterval - 1);
    TEST_ASSERT_EQUAL(Mode::Idle, state.mode);
    step(aloe(), 25, 20000 + kInterval);
    TEST_ASSERT_EQUAL(Mode::Scheduled, state.mode);
}
void test_dry_during_scheduled_run_escalates_to_emergency() {
    step(aloe(), 25, 0, Command::WaterNow);
    step(aloe(), 5, 100);
    TEST_ASSERT_EQUAL(Mode::Emergency, state.mode);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_rule1_above_critical_high_locks_out);
    RUN_TEST(test_rule2_lockout_holds_above_normal_high);
    RUN_TEST(test_rule3_lockout_leaves_at_normal_high_and_resets_clock);
    RUN_TEST(test_rule4_stop_ends_run_and_resets_clock);
    RUN_TEST(test_rule5_fault_holds);
    RUN_TEST(test_rule6_emergency_timeout_becomes_fault);
    RUN_TEST(test_rule6_only_stop_clears_fault);
    RUN_TEST(test_rule7_below_critical_low_enters_emergency_and_sets_start);
    RUN_TEST(test_rule7_start_not_reset_while_staying_in_emergency);
    RUN_TEST(test_rule8_emergency_holds_between_low_and_normal_high);
    RUN_TEST(test_rule9_emergency_leaves_at_normal_high);
    RUN_TEST(test_rule10_water_now_in_idle_starts_run);
    RUN_TEST(test_rule11_run_finishes_after_exact_duration_for_every_plant);
    RUN_TEST(test_rule12_scheduled_holds_mid_run);
    RUN_TEST(test_rule13_due_schedule_with_wet_soil_is_skipped);
    RUN_TEST(test_rule14_schedule_fires_exactly_at_interval);
    RUN_TEST(test_rule15_idle_otherwise_stays_idle);
    RUN_TEST(test_outputs_for_each_mode);
    RUN_TEST(test_water_now_ignored_in_lockout_emergency_and_fault);
    RUN_TEST(test_water_now_cannot_water_overwatered_plant);
    RUN_TEST(test_emergency_to_idle_only_at_normal_high);
    RUN_TEST(test_lockout_holds_until_normal_high);
    RUN_TEST(test_each_plant_uses_its_own_thresholds);
    RUN_TEST(test_millis_overflow_schedule_elapsed_time);
    RUN_TEST(test_millis_overflow_run_duration);
    RUN_TEST(test_stop_in_idle_resets_schedule_clock);
    RUN_TEST(test_dry_during_scheduled_run_escalates_to_emergency);
    return UNITY_END();
}
