#include "drive_control.hpp"

namespace {
int magnitude(int value) { return value < 0 ? -value : value; }

constexpr bool valid_configuration() {
    unsigned seen = 0;
    for (unsigned i = 0; i < 4; ++i) {
        const unsigned wheel = static_cast<unsigned>(car_config::kMotorWheel[i]);
        if (wheel >= 4 || (seen & (1U << wheel)) ||
            (car_config::kMotorPolarity[i] != 1 && car_config::kMotorPolarity[i] != -1)) {
            return false;
        }
        seen |= 1U << wheel;
    }
    return seen == 15U;
}
static_assert(valid_configuration(), "Assign every wheel once and use polarity +1/-1");

int axis(uint8_t value, bool invert) {
    const int delta = static_cast<int>(value) - 128;
    const int distance = magnitude(delta);
    if (distance <= car_config::kAxisDeadzone) {
        return 0;
    }
    const int full_scale = delta < 0 ? 128 : 127;
    int scaled = (distance - car_config::kAxisDeadzone) * car_config::kMaxDutyPercent /
                 (full_scale - car_config::kAxisDeadzone);
    if (delta < 0) {
        scaled = -scaled;
    }
    return invert ? -scaled : scaled;
}
} // namespace

DriveCommand mix_drive(int forward, int strafe, int rotation, car_config::MecanumLayout layout) {
    // O 型与 X 型的横向滚子方向相反。轮序为左前、右前、左后、右后。
    // 旋转指令仍表示归一化的左右差速量，不是角速度。
    if (layout == car_config::MecanumLayout::O) {
        strafe = -strafe;
    }
    int wheel[4] = {forward + strafe + rotation, forward - strafe - rotation,
                    forward - strafe + rotation, forward + strafe - rotation};
    int peak = car_config::kMaxDutyPercent;
    for (int speed : wheel) {
        if (magnitude(speed) > peak) {
            peak = magnitude(speed);
        }
    }
    DriveCommand result{};
    for (unsigned i = 0; i < 4; ++i) {
        result.motor[i] = wheel[static_cast<unsigned>(car_config::kMotorWheel[i])] *
                          car_config::kMaxDutyPercent / peak * car_config::kMotorPolarity[i];
    }
    return result;
}

DriveCommand drive_from_ps2(const Ps2Frame &frame) {
    if (!frame.connected || ps2_pressed(frame, Ps2Button::Cross) ||
        ps2_pressed(frame, Ps2Button::Select)) {
        return {};
    }
    const bool up = ps2_pressed(frame, Ps2Button::Up);
    const bool down = ps2_pressed(frame, Ps2Button::Down);
    const bool left = ps2_pressed(frame, Ps2Button::Left);
    const bool right = ps2_pressed(frame, Ps2Button::Right);
    if (ps2_pressed(frame, Ps2Button::Start)) {
        // 按住 START 和且仅一个方向键，可单独测试一个电机。
        // 上=M1、右=M2、下=M3、左=M4；同时按住 R2 可反转。
        DriveCommand result{};
        if (static_cast<int>(up) + down + left + right != 1) {
            return result;
        }
        const unsigned motor = up ? 0 : right ? 1 : down ? 2 : 3;
        result.motor[motor] =
            car_config::kCalibrationDutyPercent * (ps2_pressed(frame, Ps2Button::R2) ? -1 : 1);
        return result;
    }
    int forward = frame.analog ? axis(frame.left_y, true) : 0;
    int strafe = frame.analog ? axis(frame.left_x, false) : 0;
    int rotation =
        ps2_pressed(frame, Ps2Button::L3) && frame.analog ? axis(frame.right_x, false) : 0;

    if (up || down || left || right) {
        forward = (static_cast<int>(up) - static_cast<int>(down)) * car_config::kButtonDutyPercent;
        strafe =
            (static_cast<int>(right) - static_cast<int>(left)) * car_config::kButtonDutyPercent;
    }
    const bool l1 = ps2_pressed(frame, Ps2Button::L1);
    const bool r1 = ps2_pressed(frame, Ps2Button::R1);
    if (l1 || r1) {
        rotation = (static_cast<int>(r1) - static_cast<int>(l1)) * car_config::kButtonDutyPercent;
    }
    return mix_drive(forward, strafe, rotation);
}
