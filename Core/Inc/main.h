/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : main.c 的头文件。
  *                   本文件包含应用程序的公共定义。
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define motor1_A_Pin GPIO_PIN_0
#define motor1_A_GPIO_Port GPIOC
#define motor1_B_Pin GPIO_PIN_1
#define motor1_B_GPIO_Port GPIOC
#define motor2_A_Pin GPIO_PIN_2
#define motor2_A_GPIO_Port GPIOC
#define motor2_B_Pin GPIO_PIN_3
#define motor2_B_GPIO_Port GPIOC
#define motor3_A_input_Pin GPIO_PIN_0
#define motor3_A_input_GPIO_Port GPIOA
#define motor3_B_input_Pin GPIO_PIN_1
#define motor3_B_input_GPIO_Port GPIOA
#define motor2_A_input_Pin GPIO_PIN_6
#define motor2_A_input_GPIO_Port GPIOA
#define motor2_B_input_Pin GPIO_PIN_7
#define motor2_B_input_GPIO_Port GPIOA
#define motor3_A_Pin GPIO_PIN_4
#define motor3_A_GPIO_Port GPIOC
#define motor3_B_Pin GPIO_PIN_5
#define motor3_B_GPIO_Port GPIOC
#define motor4_A_Pin GPIO_PIN_10
#define motor4_A_GPIO_Port GPIOB
#define motor4_B_Pin GPIO_PIN_11
#define motor4_B_GPIO_Port GPIOB
#define PS2_CS_Pin GPIO_PIN_12
#define PS2_CS_GPIO_Port GPIOB
#define PWM1_Pin GPIO_PIN_6
#define PWM1_GPIO_Port GPIOC
#define PWM2_Pin GPIO_PIN_7
#define PWM2_GPIO_Port GPIOC
#define PWM3_Pin GPIO_PIN_8
#define PWM3_GPIO_Port GPIOC
#define PWM4_Pin GPIO_PIN_9
#define PWM4_GPIO_Port GPIOC
#define motor1_PWM_Pin GPIO_PIN_8
#define motor1_PWM_GPIO_Port GPIOA
#define motor2_PWM_Pin GPIO_PIN_9
#define motor2_PWM_GPIO_Port GPIOA
#define motor3_PWM_Pin GPIO_PIN_10
#define motor3_PWM_GPIO_Port GPIOA
#define motor4_PWM_Pin GPIO_PIN_11
#define motor4_PWM_GPIO_Port GPIOA
#define Key_Pin GPIO_PIN_12
#define Key_GPIO_Port GPIOA
#define motor1_A_input_Pin GPIO_PIN_15
#define motor1_A_input_GPIO_Port GPIOA
#define motor1_B_input_Pin GPIO_PIN_3
#define motor1_B_input_GPIO_Port GPIOB
#define motor4_A_input_Pin GPIO_PIN_6
#define motor4_A_input_GPIO_Port GPIOB
#define motor4_B_input_Pin GPIO_PIN_7
#define motor4_B_input_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
