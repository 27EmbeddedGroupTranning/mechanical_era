#include "ps2_protocol.hpp"

std::size_t ps2_packet_size(uint8_t mode) {
    switch (mode) {
    case 0x41:
        return 5;
    case 0x73:
        return 9;
    case 0x79:
        return 21;
    default:
        return 0;
    }
}

Ps2Frame decode_ps2_frame(const uint8_t *bytes, std::size_t length) {
    if (bytes == nullptr || length < 3) {
        return {};
    }
    const auto expected = ps2_packet_size(bytes[1]);
    if (expected == 0 || length < expected || bytes[0] != 0xFF || bytes[2] != 0x5A) {
        return {};
    }
    Ps2Frame frame;
    frame.connected = true;
    frame.analog = bytes[1] != 0x41;
    frame.buttons = static_cast<uint16_t>(
        ~(static_cast<uint16_t>(bytes[3]) | (static_cast<uint16_t>(bytes[4]) << 8)));
    if (frame.analog) {
        frame.right_x = bytes[5];
        frame.right_y = bytes[6];
        frame.left_x = bytes[7];
        frame.left_y = bytes[8];
    }
    return frame;
}

bool ps2_pressed(const Ps2Frame &frame, Ps2Button button) {
    const auto bit = static_cast<unsigned>(button);
    return frame.connected && bit < 16 && (frame.buttons & (1U << bit)) != 0;
}
