#pragma once

#include <cstddef>
#include <cstdint>

enum class Ps2Button : uint8_t {
    Select = 0,
    L3,
    R3,
    Start,
    Up,
    Right,
    Down,
    Left,
    L2,
    R2,
    L1,
    R1,
    Triangle,
    Circle,
    Cross,
    Square
};

struct Ps2Frame {
    bool connected = false;
    bool analog = false;
    uint16_t buttons = 0; // 1 表示按下；仅在解码入口转换协议的低有效位。
    uint8_t right_x = 128;
    uint8_t right_y = 128;
    uint8_t left_x = 128;
    uint8_t left_y = 128;
};

// 返回模式对应的完整报文长度；未知模式返回 0。
std::size_t ps2_packet_size(uint8_t mode);
Ps2Frame decode_ps2_frame(const uint8_t *bytes, std::size_t length);
bool ps2_pressed(const Ps2Frame &frame, Ps2Button button);
