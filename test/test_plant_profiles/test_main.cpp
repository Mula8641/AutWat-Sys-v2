#include <unity.h>
#include "plant_profiles.h"

using namespace autwat;

void setUp() {}
void tearDown() {}

static void expectProfile(uint8_t index, const char* name, uint8_t low, uint8_t high, uint8_t critHigh, uint16_t ml) {
    const PlantProfile& p = plantAt(index);
    TEST_ASSERT_EQUAL_STRING(name, p.name);
    TEST_ASSERT_EQUAL_UINT8(low, p.criticalLowPct);
    TEST_ASSERT_EQUAL_UINT8(high, p.normalHighPct);
    TEST_ASSERT_EQUAL_UINT8(critHigh, p.criticalHighPct);
    TEST_ASSERT_EQUAL_UINT16(ml, p.waterMl);
}

void test_cactus_row() { expectProfile(0, "Cactus", 10, 20, 40, 300); }
void test_aloe_row() { expectProfile(1, "AloeVera", 10, 30, 50, 300); }
void test_zz_row() { expectProfile(2, "ZamioculcasZamifolia", 15, 40, 70, 250); }
void test_next_wraps_to_zero() { TEST_ASSERT_EQUAL_UINT8(0, nextPlantIndex(2)); }
void test_next_advances() { TEST_ASSERT_EQUAL_UINT8(1, nextPlantIndex(0)); }
void test_index_wraps() { TEST_ASSERT_EQUAL_STRING("ZamioculcasZamifolia", plantAt(5).name); }

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_cactus_row);
    RUN_TEST(test_aloe_row);
    RUN_TEST(test_zz_row);
    RUN_TEST(test_next_wraps_to_zero);
    RUN_TEST(test_next_advances);
    RUN_TEST(test_index_wraps);
    return UNITY_END();
}
