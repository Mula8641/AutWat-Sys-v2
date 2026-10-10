#include "command.h"
#include <cctype>

namespace autwat {

namespace {

bool equalsIgnoreCase(const char* text, const char* word) {
    while (*text != '\0' && *word != '\0' &&
           std::tolower(static_cast<unsigned char>(*text)) == *word) {
        ++text;
        ++word;
    }
    return *text == '\0' && *word == '\0';
}

}  // namespace

Command parseCommandText(const char* text) {
    Command result = Command::None;
    if (text != nullptr) {
        if (equalsIgnoreCase(text, "water")) {
            result = Command::WaterNow;
        } else if (equalsIgnoreCase(text, "stop")) {
            result = Command::Stop;
        }
    }
    return result;
}

Command decodeCommandByte(uint8_t value) {
    Command result = Command::None;
    if (value == static_cast<uint8_t>(Command::WaterNow)) {
        result = Command::WaterNow;
    } else if (value == static_cast<uint8_t>(Command::Stop)) {
        result = Command::Stop;
    }
    return result;
}

Command pickCommand(Command fromButton, Command fromBle, Command fromHttp) {
    Command result = fromButton;
    if (result == Command::None) result = fromBle;
    if (result == Command::None) result = fromHttp;
    return result;
}

Command commandForButton(ButtonEvent event, bool pumpRunning) {
    Command result = Command::None;
    if (event == ButtonEvent::ShortPress) {
        result = pumpRunning ? Command::Stop : Command::WaterNow;
    }
    return result;
}

}  // namespace autwat
