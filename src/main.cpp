#include "app.h"
#include "ps2.hpp"
#include "outputs.hpp"
#include "diagnostics.hpp"

extern "C" SPI_HandleTypeDef hspi2;

extern "C" void main_task() {
    init_outputs();
    ps2_init(hspi2);
    uint32_t last_report = HAL_GetTick() - 500U;
    bool emergency_stop = false;
    bool key_sample = false;
    bool key_pressed = false;
    uint32_t key_change_ms = HAL_GetTick();
    while (true) {
        // 20 ms 去抖后按一次锁定急停，再按一次解除
        const bool key_now = HAL_GPIO_ReadPin(Key_GPIO_Port, Key_Pin) == GPIO_PIN_RESET;
        if (key_now != key_sample) {
            key_sample = key_now;
            key_change_ms = HAL_GetTick();
        } else if (key_now != key_pressed && HAL_GetTick() - key_change_ms >= 20U) {
            key_pressed = key_now;
            if (key_pressed) {
                emergency_stop = !emergency_stop;
            }
        }
        if (emergency_stop) {
            apply_motors({});
        }
        ps2_update();
        const auto frame = ps2_read();
        const auto command = emergency_stop ? DriveCommand{} : drive_from_ps2(frame);
        apply_motors(command);
        const auto arm_command = emergency_stop ? ArmCommand{} : arm_from_ps2(frame);
        apply_servos(arm_command);
        if (HAL_GetTick() - last_report >= 500U) {
            report_status(frame, command, emergency_stop, key_pressed);
            last_report = HAL_GetTick();
        }
        HAL_Delay(1);
    }
}
