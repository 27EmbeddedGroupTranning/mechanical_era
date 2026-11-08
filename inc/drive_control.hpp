#pragma once

#include "ps2_protocol.hpp"
#include "car_config.hpp"

struct DriveCommand {
    // 按实际电机 M1～M4 排列，已包含轮位映射和方向校正。
    int motor[4] = {};
};

// 前进、向右平移、顺时针旋转分别为对应轴的正方向。
// 采用开环归一化 PWM 混合；旋转项为差速指令，单位不是弧度每秒。
DriveCommand mix_drive(int forward, int strafe, int rotation,
                       car_config::MecanumLayout layout = car_config::kMecanumLayout);
DriveCommand drive_from_ps2(const Ps2Frame &frame);
