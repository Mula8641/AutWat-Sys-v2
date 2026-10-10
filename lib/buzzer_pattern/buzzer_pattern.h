#pragma once
#include <cstdint>
#include "watering_controller.h"

namespace autwat {

bool buzzerIsOn(Alarm alarm, uint32_t nowMs);

}  // namespace autwat
