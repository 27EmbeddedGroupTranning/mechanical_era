#include "arm_control.hpp"

namespace {
int axis_percent(uint8_t axis) {
    const int delta = static_cast<int>(axis) - 128;
    return delta < 0 ? delta * 100 / 128 : delta * 100 / 127;
}
} // namespace

ArmCommand arm_from_ps2(const Ps2Frame &frame) {
    ArmCommand command;
    if (!frame.connected) {
        return command;
    }
    if (frame.analog) {
        command.servo[0] = axis_percent(frame.right_x);
        command.servo[3] = axis_percent(frame.right_y);
    }
    if (ps2_pressed(frame, Ps2Button::Cross)) {
        command.servo[2] -= 10;
    }
    if (ps2_pressed(frame, Ps2Button::Circle)) {
        command.servo[2] += 10;
    }
    return command;
}
