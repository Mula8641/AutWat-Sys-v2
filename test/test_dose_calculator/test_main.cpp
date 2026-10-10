#include <unity.h>
#include "dose_calculator.h"

using namespace autwat;

void setUp() {}
void tearDown() {}

void test_300ml_at_default_flow() { TEST_ASSERT_EQUAL_UINT32(17104, runMsFor(300, 17.54f)); }
void test_250ml_at_default_flow() { TEST_ASSERT_EQUAL_UINT32(14253, runMsFor(250, 17.54f)); }
void test_zero_flow_returns_zero() { TEST_ASSERT_EQUAL_UINT32(0, runMsFor(300, 0.0f)); }
void test_negative_flow_returns_zero() { TEST_ASSERT_EQUAL_UINT32(0, runMsFor(300, -5.0f)); }
void test_zero_ml_returns_zero() { TEST_ASSERT_EQUAL_UINT32(0, runMsFor(0, 17.54f)); }

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_300ml_at_default_flow);
    RUN_TEST(test_250ml_at_default_flow);
    RUN_TEST(test_zero_flow_returns_zero);
    RUN_TEST(test_negative_flow_returns_zero);
    RUN_TEST(test_zero_ml_returns_zero);
    return UNITY_END();
}
