#pragma once

#include "drive_control.hpp"
#include "arm_control.hpp"

void init_outputs();
void apply_motors(DriveCommand command);
void apply_servos(ArmCommand command);
