// Feeds a scripted moisture sequence into stepController and prints every mode change.
// Build: g++ -std=c++17 -Iinclude -Ilib/* tools/scripted_run.cpp lib/*/*.cpp -o scripted_run
#include <cstdio>
#include "status_json.h"
#include "watering_controller.h"

using namespace autwat;

struct Phase { const char* label; uint8_t moisturePct; uint32_t durationMs; };

int main() {
    const PlantProfile& plant = plantAt(1);  // AloeVera
    const ControllerConfig config = {30000, 17.54f, 60000};
    ControllerState state = {Mode::Idle, 0, 0};
    const Phase script[] = {
        {"normal", 25, 20000}, {"dry", 5, 5000}, {"recovered", 35, 5000}, {"flooded", 60, 3000},
        {"drained", 25, 40000}, {"stuck sensor low", 5, 65000},
    };
    uint32_t nowMs = 0;
    for (const Phase& phase : script) {
        for (uint32_t end = nowMs + phase.durationMs; nowMs < end; nowMs += 100) {
            const Mode before = state.mode;
            const ControllerOutput out = stepController(state, ControllerInput{phase.moisturePct, nowMs, Command::None}, plant, config);
            if (out.mode != before) {
                std::printf("t=%6.1fs  %-16s moisture=%2u%%  %s -> %s  pump=%d\n", nowMs / 1000.0, phase.label,
                            phase.moisturePct, modeName(before), modeName(out.mode), out.pumpOn);
            }
        }
    }
    return 0;
}
