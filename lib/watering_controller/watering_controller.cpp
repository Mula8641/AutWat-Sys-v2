#include "watering_controller.h"
#include "dose_calculator.h"

namespace autwat {

namespace {

struct Transition {
    Mode next;
    bool markWatered;  // lastWateredMs = nowMs
    bool markStart;    // modeStartMs = nowMs
};

uint32_t elapsedMs(uint32_t nowMs, uint32_t sinceMs) { return nowMs - sinceMs; }

// Rules 1-6: overwatering lockout, Stop, Fault hold, emergency timeout.
bool decideSafety(const ControllerState& state, const ControllerInput& input,
                  const PlantProfile& plant, const ControllerConfig& config, Transition& out) {
    bool matched = true;
    const bool inLockout = state.mode == Mode::Lockout;
    if (input.moisturePct > plant.criticalHighPct) {
        out = {Mode::Lockout, false, false};
    } else if (inLockout && input.moisturePct > plant.normalHighPct) {
        out = {Mode::Lockout, false, false};
    } else if (inLockout) {
        out = {Mode::Idle, true, false};
    } else if (input.command == Command::Stop) {
        out = {Mode::Idle, true, false};
    } else if (state.mode == Mode::Fault) {
        out = {Mode::Fault, false, false};
    } else if (state.mode == Mode::Emergency &&
               elapsedMs(input.nowMs, state.modeStartMs) >= config.emergencyMaxMs) {
        out = {Mode::Fault, false, false};
    } else {
        matched = false;
    }
    return matched;
}

// Rules 7-9: dry emergency.
bool decideEmergency(const ControllerState& state, const ControllerInput& input,
                     const PlantProfile& plant, Transition& out) {
    bool matched = true;
    const bool inEmergency = state.mode == Mode::Emergency;
    if (input.moisturePct < plant.criticalLowPct) {
        out = {Mode::Emergency, false, !inEmergency};
    } else if (inEmergency && input.moisturePct < plant.normalHighPct) {
        out = {Mode::Emergency, false, false};
    } else if (inEmergency) {
        out = {Mode::Idle, true, false};
    } else {
        matched = false;
    }
    return matched;
}

// Rules 10-15: manual run, run completion, scheduled run, skip when soil is already wet.
Transition decideSchedule(const ControllerState& state, const ControllerInput& input,
                          const PlantProfile& plant, const ControllerConfig& config) {
    Transition out = {state.mode, false, false};
    const bool idle = state.mode == Mode::Idle;
    const bool due = idle && elapsedMs(input.nowMs, state.lastWateredMs) >= config.scheduleIntervalMs;
    if (idle && input.command == Command::WaterNow) {
        out = {Mode::Scheduled, false, true};
    } else if (state.mode == Mode::Scheduled) {
        const uint32_t runMs = runMsFor(plant.waterMl, config.flowRateMlPerSec);
        if (elapsedMs(input.nowMs, state.modeStartMs) >= runMs) out = {Mode::Idle, true, false};
    } else if (due && input.moisturePct >= plant.normalHighPct) {
        out = {Mode::Idle, true, false};
    } else if (due) {
        out = {Mode::Scheduled, false, true};
    }
    return out;
}

Transition nextTransition(const ControllerState& state, const ControllerInput& input,
                          const PlantProfile& plant, const ControllerConfig& config) {
    Transition out = {state.mode, false, false};
    if (!decideSafety(state, input, plant, config, out) && !decideEmergency(state, input, plant, out)) {
        out = decideSchedule(state, input, plant, config);
    }
    return out;
}

void applyEntryEffects(ControllerState& state, const Transition& transition, uint32_t nowMs) {
    state.mode = transition.next;
    if (transition.markWatered) state.lastWateredMs = nowMs;
    if (transition.markStart) state.modeStartMs = nowMs;
}

ControllerOutput outputFor(Mode mode) {
    ControllerOutput out = {false, Alarm::None, mode};
    if (mode == Mode::Scheduled) {
        out = {true, Alarm::Slow, mode};
    } else if (mode == Mode::Emergency) {
        out = {true, Alarm::Fast, mode};
    } else if (mode == Mode::Lockout || mode == Mode::Fault) {
        out = {false, Alarm::Fast, mode};
    }
    return out;
}

}  // namespace

ControllerOutput stepController(ControllerState& state, const ControllerInput& input,
                                const PlantProfile& plant, const ControllerConfig& config) {
    applyEntryEffects(state, nextTransition(state, input, plant, config), input.nowMs);
    return outputFor(state.mode);
}

}  // namespace autwat
