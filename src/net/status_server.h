#pragma once
#include "command.h"
#include "status_json.h"

void statusServerBegin();
void statusServerTick();
void statusServerPublish(const autwat::StatusSnapshot& snapshot);
autwat::Command statusServerTakeCommand();
