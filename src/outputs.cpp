#include "outputs.hpp"
#include "main.h"
#include <algorithm>

extern "C" TIM_HandleTypeDef htim1;
extern "C" TIM_HandleTypeDef htim8;

namespace {
struct Motor {
    GPIO_TypeDef *a_port;
    uint16_t a_pin;
    GPIO_TypeDef *b_port;
    uint16_t b_pin;
    uint32_t channel;
};

const Motor kMotors[] = {
    {motor1_A_GPIO_Port, motor1_A_Pin, motor1_B_GPIO_Port, motor1_B_Pin, TIM_CHANNEL_1},
    {motor2_A_GPIO_Port, motor2_A_Pin, motor2_B_GPIO_Port, motor2_B_Pin, TIM_CHANNEL_2},
    {motor3_A_GPIO_Port, motor3_A_Pin, motor3_B_GPIO_Port, motor3_B_Pin, TIM_CHANNEL_3},
    {motor4_A_GPIO_Port, motor4_A_Pin, motor4_B_GPIO_Port, motor4_B_Pin, TIM_CHANNEL_4}};

constexpr uint32_t servo_channels[] = {TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3, TIM_CHANNEL_4};

int previous_motor_duty[4] = {};
}

void apply_servos(ArmCommand command) {
    // TIM8 每 1 微秒计数一次（72 MHz / 72），因此比较值就是脉宽的微秒数。
    constexpr int kNeutralUs = 1500;
    constexpr int kSpanUs = 1000;
    for (unsigned i = 0; i < 4; ++i) {
        const int percent =
            command.servo[i] < -100 ? -100 : (command.servo[i] > 100 ? 100 : command.servo[i]);
        const int pulse_us = kNeutralUs + kSpanUs * percent / 100;
        __HAL_TIM_SET_COMPARE(&htim8, servo_channels[i], static_cast<uint32_t>(pulse_us));
    }
}

void apply_motors(DriveCommand command) {
    int requested[4];
    bool reversing = false;
    for (unsigned i = 0; i < 4; ++i) {
        requested[i] = std::max(-100, std::min(100, command.motor[i]));
        reversing |= requested[i] * previous_motor_duty[i] < 0;
    }
    if (reversing) {
        for (const Motor &motor : kMotors) {
            __HAL_TIM_SET_COMPARE(&htim1, motor.channel, 0);
            HAL_GPIO_WritePin(motor.a_port, motor.a_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(motor.b_port, motor.b_pin, GPIO_PIN_RESET);
        }
        HAL_Delay(2);
    }
    for (unsigned i = 0; i < 4; ++i) {
        const Motor &motor = kMotors[i];
        const int duty = requested[i];
        if (!reversing && duty == previous_motor_duty[i]) {
            continue;
        }
        HAL_GPIO_WritePin(motor.a_port, motor.a_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(motor.b_port, motor.b_pin, GPIO_PIN_RESET);
        if (duty != 0) {
            HAL_GPIO_WritePin(duty > 0 ? motor.a_port : motor.b_port,
                              duty > 0 ? motor.a_pin : motor.b_pin, GPIO_PIN_SET);
        }
        const uint32_t magnitude = static_cast<uint32_t>(duty < 0 ? -duty : duty);
        const uint32_t period = __HAL_TIM_GET_AUTORELOAD(&htim1);
        const uint32_t scaled = ((period + 1U) * magnitude) / 100U;
        const uint32_t compare = scaled > period ? period : scaled;
        __HAL_TIM_SET_COMPARE(&htim1, motor.channel, compare);
        previous_motor_duty[i] = duty;
    }
}

void init_outputs() {
    // 先写入停止状态，再启动 PWM
    for (unsigned i = 0; i < 4; ++i) {
        const auto &motor = kMotors[i];
        __HAL_TIM_SET_COMPARE(&htim1, motor.channel, 0);
        HAL_GPIO_WritePin(motor.a_port, motor.a_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(motor.b_port, motor.b_pin, GPIO_PIN_RESET);
        previous_motor_duty[i] = 0;
    }
    for (const Motor &motor : kMotors) {
        if (HAL_TIM_PWM_Start(&htim1, motor.channel) != HAL_OK) {
            Error_Handler();
        }
    }
    apply_servos({});
    for (const auto channel : servo_channels) {
        if (HAL_TIM_PWM_Start(&htim8, channel) != HAL_OK) {
            Error_Handler();
        }
    }
}
