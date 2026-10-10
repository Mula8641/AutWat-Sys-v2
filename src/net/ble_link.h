#pragma once
#include <cstdint>
#include "command.h"
#include "status_codec.h"

void bleLinkBegin();
bool bleLinkIsConnected();
void bleLinkPublish(const uint8_t status[autwat::kStatusBytes]);
autwat::Command bleLinkTakeCommand();
