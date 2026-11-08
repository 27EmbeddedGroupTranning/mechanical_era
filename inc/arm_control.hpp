#pragma once

#include "ps2_protocol.hpp"

struct ArmCommand {
    // 行程百分比 -100..100，0 为中位；硬件层换算成舵机脉宽。
    int servo[4] = {};
};

ArmCommand arm_from_ps2(const Ps2Frame &frame);
