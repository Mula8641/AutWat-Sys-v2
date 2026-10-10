#pragma once
#include <cstdint>
#include "button_decoder.h"

namespace autwat {

enum class Command : uint8_t { None = 0, WaterNow = 1, Stop = 2 };

Command parseCommandText(const char* text);
Command decodeCommandByte(uint8_t value);
Command pickCommand(Command fromButton, Command fromBle, Command fromHttp);
Command commandForButton(ButtonEvent event, bool pumpRunning);

}  // namespace autwat
