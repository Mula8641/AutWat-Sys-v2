#include "button.h"
#include <Arduino.h>
#include "autwat_config.h"

void buttonBegin() { pinMode(autwat::kPinButton, INPUT_PULLUP); }

bool buttonIsDown() { return digitalRead(autwat::kPinButton) == LOW; }
