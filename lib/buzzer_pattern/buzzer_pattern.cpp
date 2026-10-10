#include "buzzer_pattern.h"

namespace autwat {

bool buzzerIsOn(Alarm alarm, uint32_t nowMs) {
    uint32_t intervalMs = 0;
    if (alarm == Alarm::Fast) intervalMs = 200;
    if (alarm == Alarm::Slow) intervalMs = 1000;
    return intervalMs != 0 && (nowMs / intervalMs) % 2 == 0;
}

}  // namespace autwat
