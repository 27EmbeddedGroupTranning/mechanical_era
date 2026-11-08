#include "diagnostics.hpp"
#include "ps2.hpp"
#include <cstdio>

extern "C" UART_HandleTypeDef huart3;

void report_status(const Ps2Frame &frame, DriveCommand command, bool emergency_stop,
                   bool key_pressed) {
    char message[192];
    const int length = std::snprintf(
        message, sizeof(message),
        "PS2 online=%lu mode=%02lX keys=%04lX LX=%lu LY=%lu RX=%lu M=%d,%d,%d,%d STOP=%u KEY=%u "
        "bad=%lu\r\n",
        static_cast<unsigned long>(frame.connected), static_cast<unsigned long>(ps2_debug.mode),
        static_cast<unsigned long>(frame.buttons), static_cast<unsigned long>(frame.left_x),
        static_cast<unsigned long>(frame.left_y), static_cast<unsigned long>(frame.right_x),
        command.motor[0], command.motor[1], command.motor[2], command.motor[3],
        static_cast<unsigned>(emergency_stop), static_cast<unsigned>(key_pressed),
        static_cast<unsigned long>(ps2_debug.invalid_frames));
    if (length > 0 && static_cast<unsigned>(length) < sizeof(message)) {
        HAL_UART_Transmit(&huart3, reinterpret_cast<uint8_t *>(message),
                          static_cast<uint16_t>(length), 20);
    }

    // 读取 M4 的实际硬件配置，便于对照请求的占空比。
    // GPIO 配置字段：PA11=0xA（复用推挽输出），PB10/PB11=0x2
    // （推挽输出）
    const int hardware_length =
        std::snprintf(message, sizeof(message),
                      "M4HW-v1 req=%d ccr=%lu arr=%lu run=%u en=%u moe=%u oc=%lu "
                      "pa11=%lX pb10=%lX pb11=%lX out=%u,%u map=%08lX\r\n",
                      command.motor[3], static_cast<unsigned long>(TIM1->CCR4),
                      static_cast<unsigned long>(TIM1->ARR),
                      static_cast<unsigned>((TIM1->CR1 & TIM_CR1_CEN) != 0U),
                      static_cast<unsigned>((TIM1->CCER & TIM_CCER_CC4E) != 0U),
                      static_cast<unsigned>((TIM1->BDTR & TIM_BDTR_MOE) != 0U),
                      static_cast<unsigned long>((TIM1->CCMR2 >> 12U) & 7U),
                      static_cast<unsigned long>((GPIOA->CRH >> 12U) & 15U),
                      static_cast<unsigned long>((GPIOB->CRH >> 8U) & 15U),
                      static_cast<unsigned long>((GPIOB->CRH >> 12U) & 15U),
                      static_cast<unsigned>((GPIOB->IDR & motor4_A_Pin) != 0U),
                      static_cast<unsigned>((GPIOB->IDR & motor4_B_Pin) != 0U),
                      static_cast<unsigned long>(AFIO->MAPR));
    if (hardware_length > 0 && static_cast<unsigned>(hardware_length) < sizeof(message)) {
        HAL_UART_Transmit(&huart3, reinterpret_cast<uint8_t *>(message),
                          static_cast<uint16_t>(hardware_length), 20);
    }
}
