#pragma once
#include <cstdint>
#include "command.h"
#include "plant_profiles.h"

namespace autwat {

enum class Mode : uint8_t { Idle, Scheduled, Emergency, Lockout, Fault };
enum class Alarm : uint8_t { None, Slow, Fast };

struct ControllerConfig { uint32_t scheduleIntervalMs; float flowRateMlPerSec; uint32_t emergencyMaxMs; };
struct ControllerState { Mode mode; uint32_t modeStartMs; uint32_t lastWateredMs; };
struct ControllerInput { uint8_t moisturePct; uint32_t nowMs; Command command; };
struct ControllerOutput { bool pumpOn; Alarm alarm; Mode mode; };

ControllerOutput stepController(ControllerState& state, const ControllerInput& input,
                                const PlantProfile& plant, const ControllerConfig& config);

}  // namespace autwat
