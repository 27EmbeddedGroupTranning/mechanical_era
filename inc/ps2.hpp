#pragma once

#include "main.h"
#include "ps2_protocol.hpp"

// 单个接收器，由主循环调用；不在中断中读写。
// 调试统计只用于观察，不作为控制逻辑的输入。
struct Ps2Diagnostics {
    uint32_t polls = 0;
    uint32_t valid_frames = 0;
    uint32_t invalid_frames = 0;
    uint32_t spi_errors = 0;
    uint32_t input_changes = 0;
    uint32_t last_valid_ms = 0;
    uint32_t last_hal_status = 0;
    uint32_t raw_length = 0;
    uint32_t config_attempts = 0;
    uint8_t mode = 0;
    uint8_t raw[24] = {};
};

extern Ps2Diagnostics ps2_debug;
void ps2_init(SPI_HandleTypeDef &spi);
void ps2_update();
// 超过 100 ms 没有有效报文时返回断线、中位、无按键。
Ps2Frame ps2_read();
