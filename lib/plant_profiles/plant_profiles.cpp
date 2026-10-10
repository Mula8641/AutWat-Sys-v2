#include "plant_profiles.h"

namespace autwat {

namespace {

constexpr PlantProfile kPlants[kPlantCount] = {
    {"Cactus", 10, 20, 40, 300},
    {"AloeVera", 10, 30, 50, 300},
    {"ZamioculcasZamifolia", 15, 40, 70, 250},
};

}  // namespace

const PlantProfile& plantAt(uint8_t index) {
    return kPlants[index % kPlantCount];
}

uint8_t nextPlantIndex(uint8_t currentIndex) {
    return static_cast<uint8_t>((currentIndex + 1) % kPlantCount);
}

}  // namespace autwat
