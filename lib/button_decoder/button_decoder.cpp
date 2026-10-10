#include "button_decoder.h"
#include "autwat_config.h"

namespace autwat {

ButtonEvent decodeButton(ButtonState& state, bool isDown, uint32_t nowMs) {
    ButtonEvent event = ButtonEvent::None;
    const uint32_t heldMs = nowMs - state.pressStartMs;
    if (isDown && !state.pressed) {
        state = ButtonState{nowMs, true, false};
    } else if (isDown) {
        if (!state.longFired && heldMs > kLongPressMs) {
            state.longFired = true;
            event = ButtonEvent::LongPress;
        }
    } else if (state.pressed) {
        state.pressed = false;
        if (!state.longFired && heldMs >= kShortPressMs && heldMs < kLongPressMs) {
            event = ButtonEvent::ShortPress;
        }
    }
    return event;
}

}  // namespace autwat
