#include "plant_selection.h"
#include <Arduino.h>
#include "button_decoder.h"
#include "hal/button.h"
#include "hal/display.h"
#include "plant_profiles.h"
#include "screen_text.h"

// Blocking by design: nothing else runs until a plant is confirmed.
uint8_t selectPlant() {
    using namespace autwat;
    uint8_t index = 0;
    ButtonState buttonState = {0, false, false};
    char plantLine[kScreenBufferSize];
    bool confirmed = false;
    while (!confirmed) {
        formatPlantLine(plantLine, plantAt(index));
        displayShow("Select plant:", plantLine, "Short press: next", "Long press: confirm");
        const ButtonEvent event = decodeButton(buttonState, buttonIsDown(), millis());
        if (event == ButtonEvent::ShortPress) index = nextPlantIndex(index);
        confirmed = event == ButtonEvent::LongPress;
    }
    Serial.printf("[boot] plant selected: %s\n", plantAt(index).name);
    return index;
}
