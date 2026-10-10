#pragma once
#include <cstddef>
#include <cstdint>
#include "status_json.h"

namespace autwat {

constexpr size_t kStatusBytes = 8;

void encodeStatus(uint8_t out[kStatusBytes], const StatusSnapshot& snapshot, Health health);

}  // namespace autwat
