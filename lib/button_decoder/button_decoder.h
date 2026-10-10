#pragma once
#include <cstdint>

namespace autwat {

enum class ButtonEvent : uint8_t { None, ShortPress, LongPress };
struct ButtonState { uint32_t pressStartMs; bool pressed; bool longFired; };

ButtonEvent decodeButton(ButtonState& state, bool isDown, uint32_t nowMs);

}  // namespace autwat
