#include "ps2.hpp"
#include <algorithm>

Ps2Diagnostics ps2_debug;

namespace {
constexpr uint32_t kPollMs = 20;
constexpr uint32_t kConfigMs = 1000;
constexpr uint32_t kTimeoutMs = 100;
SPI_HandleTypeDef *receiver_spi = nullptr;
Ps2Frame latest_frame;
uint32_t last_poll = 0;
uint32_t last_config = 0;
uint32_t last_valid_ms = 0;

void delay_us(uint32_t us) {
    const uint32_t start = DWT->CYCCNT;
    const uint32_t cycles = (SystemCoreClock / 1000000U) * us;
    while (static_cast<uint32_t>(DWT->CYCCNT - start) < cycles) {
    }
}

// 整帧保持 ATT 为低；字节间隔为未接线的 ACK 留出应答时间。
HAL_StatusTypeDef exchange(const uint8_t *tx, uint8_t (&rx)[24], std::size_t &length,
                           bool polling) {
    std::fill_n(rx, sizeof(rx), 0xFF);
    const std::size_t requested = length;
    std::size_t expected = requested;
    length = 0;
    HAL_GPIO_WritePin(PS2_CS_GPIO_Port, PS2_CS_Pin, GPIO_PIN_RESET);
    delay_us(20);
    HAL_StatusTypeDef status = HAL_OK;
    for (std::size_t i = 0; i < expected; ++i) {
        uint8_t out = tx[i];
        status = HAL_SPI_TransmitReceive(receiver_spi, &out, &rx[i], 1, 5);
        if (status != HAL_OK) {
            break;
        }
        ++length;
        if (polling && i == 1) {
            const auto packet_size = ps2_packet_size(rx[1]);
            expected = packet_size == 0 ? requested : packet_size;
        }
        delay_us(20);
    }
    HAL_GPIO_WritePin(PS2_CS_GPIO_Port, PS2_CS_Pin, GPIO_PIN_SET);
    ps2_debug.last_hal_status = status;
    if (status != HAL_OK) {
        ++ps2_debug.spi_errors;
    }
    return status;
}

void configure_analog() {
    constexpr uint8_t commands[3][9] = {{0x01, 0x43, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
                                        {0x01, 0x44, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00},
                                        {0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A}};
    ++ps2_debug.config_attempts;
    for (const auto &command : commands) {
        uint8_t rx[24];
        std::size_t length = sizeof(command);
        const auto status = exchange(command, rx, length, false);
        HAL_Delay(10);
        if (status != HAL_OK) {
            break;
        }
    }
}
}

void ps2_init(SPI_HandleTypeDef &spi) {
    receiver_spi = &spi;
    latest_frame = {};
    last_valid_ms = 0;
    ps2_debug = {};
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    HAL_GPIO_WritePin(PS2_CS_GPIO_Port, PS2_CS_Pin, GPIO_PIN_SET);
    // 拉低 ATT 前确保 SPI 时钟处于高电平空闲状态。
    __HAL_SPI_ENABLE(&spi);
    HAL_Delay(300);
    last_poll = HAL_GetTick() - kPollMs;
    last_config = HAL_GetTick() - kConfigMs;
}

void ps2_update() {
    if (receiver_spi == nullptr) {
        return;
    }
    const uint32_t now = HAL_GetTick();
    if (static_cast<uint32_t>(now - last_poll) < kPollMs) {
        return;
    }
    last_poll = now;
    constexpr uint8_t poll[24] = {0x01, 0x42};
    uint8_t rx[24];
    std::size_t length = 9;
    const auto status = exchange(poll, rx, length, true);
    const auto frame = status == HAL_OK ? decode_ps2_frame(rx, length) : Ps2Frame{};
    ++ps2_debug.polls;
    ps2_debug.raw_length = static_cast<uint32_t>(length);
    std::copy_n(rx, sizeof(rx), ps2_debug.raw);
    ps2_debug.mode = frame.connected ? rx[1] : 0;
    if (frame.connected) {
        if (latest_frame.connected &&
            (frame.buttons != latest_frame.buttons || frame.right_x != latest_frame.right_x ||
             frame.right_y != latest_frame.right_y || frame.left_x != latest_frame.left_x ||
             frame.left_y != latest_frame.left_y)) {
            ++ps2_debug.input_changes;
        }
        last_valid_ms = HAL_GetTick();
        ps2_debug.last_valid_ms = last_valid_ms;
        ++ps2_debug.valid_frames;
    } else {
        ++ps2_debug.invalid_frames;
    }
    latest_frame = frame;
    if ((!frame.connected || !frame.analog) &&
        static_cast<uint32_t>(now - last_config) >= kConfigMs) {
        last_config = now;
        configure_analog();
    }
}

Ps2Frame ps2_read() {
    if (static_cast<uint32_t>(HAL_GetTick() - last_valid_ms) >= kTimeoutMs) {
        return {};
    }
    return latest_frame;
}
