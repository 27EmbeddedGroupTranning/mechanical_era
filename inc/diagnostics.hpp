#pragma once

#include "drive_control.hpp"

void report_status(const Ps2Frame &frame, DriveCommand command, bool emergency_stop,
                   bool key_pressed);
