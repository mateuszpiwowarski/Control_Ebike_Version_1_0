/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin_Pin GPIO_PIN_13
#define LED_Pin_GPIO_Port GPIOC
#define DS18B20_Pin_Pin GPIO_PIN_0
#define DS18B20_Pin_GPIO_Port GPIOA
#define DS18B20_Pin_EXTI_IRQn EXTI0_IRQn
#define GPIO_EXTI1_Pin GPIO_PIN_1
#define GPIO_EXTI1_GPIO_Port GPIOA
#define ADC1_IN3_Pin GPIO_PIN_3
#define ADC1_IN3_GPIO_Port GPIOA
#define ADC1_IN4_Pin GPIO_PIN_4
#define ADC1_IN4_GPIO_Port GPIOA
#define ADC1_IN5_Pin GPIO_PIN_5
#define ADC1_IN5_GPIO_Port GPIOA
#define TIM3_CH2_Pin GPIO_PIN_7
#define TIM3_CH2_GPIO_Port GPIOA
#define ADC1_IN8_Pin GPIO_PIN_0
#define ADC1_IN8_GPIO_Port GPIOB
#define ADC1_IN9_Pin GPIO_PIN_1
#define ADC1_IN9_GPIO_Port GPIOB
#define I2C2_SCL_Pin GPIO_PIN_10
#define I2C2_SCL_GPIO_Port GPIOB
#define BUTTON1_Pin_Pin GPIO_PIN_12
#define BUTTON1_Pin_GPIO_Port GPIOB
#define BUTTON2_Pin_Pin GPIO_PIN_13
#define BUTTON2_Pin_GPIO_Port GPIOB
#define BUTTON3_Pin_Pin GPIO_PIN_14
#define BUTTON3_Pin_GPIO_Port GPIOB
#define USART1_TX_Pin GPIO_PIN_9
#define USART1_TX_GPIO_Port GPIOA
#define USART1_RX_Pin GPIO_PIN_10
#define USART1_RX_GPIO_Port GPIOA
#define TIM2_CH1_Pin GPIO_PIN_15
#define TIM2_CH1_GPIO_Port GPIOA
#define I2C2_SDA_Pin GPIO_PIN_3
#define I2C2_SDA_GPIO_Port GPIOB
#define I2C1_SCL_Pin GPIO_PIN_6
#define I2C1_SCL_GPIO_Port GPIOB
#define I2C1_SDA_Pin GPIO_PIN_7
#define I2C1_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
