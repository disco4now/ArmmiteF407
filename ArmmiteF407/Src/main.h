/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
void dump(char *p, int nbr,int page);
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
#define IR_Pin GPIO_PIN_2
#define IR_GPIO_Port GPIOE
#define KEY1_Pin GPIO_PIN_3
#define KEY1_GPIO_Port GPIOE
#define KEY0_Pin GPIO_PIN_4
#define KEY0_GPIO_Port GPIOE
#define PWM_3A_Pin GPIO_PIN_5
#define PWM_3A_GPIO_Port GPIOE
#define PWM_3B_Pin GPIO_PIN_6
#define PWM_3B_GPIO_Port GPIOE
#define SDIO_CD_Pin GPIO_PIN_13
#define SDIO_CD_GPIO_Port GPIOC
#define PWM_1A_Pin GPIO_PIN_6
#define PWM_1A_GPIO_Port GPIOA
#define PWM_1B_Pin GPIO_PIN_7
#define PWM_1B_GPIO_Port GPIOA
#define T_PEN_Pin GPIO_PIN_5
#define T_PEN_GPIO_Port GPIOC
#define PWM_1C_F_CS_Pin GPIO_PIN_0
#define PWM_1C_F_CS_GPIO_Port GPIOB
//#define PWM_1D_LCD_BL_Pin GPIO_PIN_1
//#define PWM_1D_LCD_BL_GPIO_Port GPIOB
#define T_CS_Pin GPIO_PIN_12
#define T_CS_GPIO_Port GPIOB
#define Drive_VBUS_FS_Pin GPIO_PIN_11
#define Drive_VBUS_FS_GPIO_Port GPIOD
#define PWM_2A_Pin GPIO_PIN_12
#define PWM_2A_GPIO_Port GPIOD
#define TXD1_Pin GPIO_PIN_9
#define TXD1_GPIO_Port GPIOA
#define RXD1_Pin GPIO_PIN_10
#define RXD1_GPIO_Port GPIOA
#define PA13_TMS_Pin GPIO_PIN_13
#define PA13_TMS_GPIO_Port GPIOA
#define PA14_TCK_Pin GPIO_PIN_14
#define PA14_TCK_GPIO_Port GPIOA
#define KBD_CLK_Pin GPIO_PIN_15
#define KBD_CLK_GPIO_Port GPIOA
#define KBD_DATA_Pin GPIO_PIN_3
#define KBD_DATA_GPIO_Port GPIOD
#define I2C_SCL_NRF_CE_Pin GPIO_PIN_6
#define I2C_SCL_NRF_CE_GPIO_Port GPIOB
#define I2C_SDA_NRF_CS_Pin GPIO_PIN_7
#define I2C_SDA_NRF_CS_GPIO_Port GPIOB
#define PWM_2B_NRF_IRQ_Pin GPIO_PIN_8
#define PWM_2B_NRF_IRQ_GPIO_Port GPIOB
#define PWM_2C_Pin GPIO_PIN_9
#define PWM_2C_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
//#define package (*(volatile unsigned int *)(PACKAGE_BASE) & 0b11111)
#define flashsize *(volatile unsigned int *)(FLASHSIZE_BASE)
#define chipID (DBGMCU->IDCODE & 0x00000FFF)
#define CONSOLE_RX_BUF_SIZE 512
#define CONSOLE_TX_BUF_SIZE 1024                    // this is made a large size so that the serial console does not slow down the USB and LCD consoles
#define BREAK_KEY           3                       // the default value (CTRL-C) for the break key.  Reset at the command prompt.
#define forever 1
#define true	1
#define false	0
// used to determine if the exception occured during setup
#define CAUSE_NOTHING           0
#define CAUSE_DISPLAY           1
#define CAUSE_FILEIO            2
#define CAUSE_KEYBOARD          3
#define CAUSE_RTC               4
#define CAUSE_TOUCH             5
#define CAUSE_MMSTARTUP         6
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
