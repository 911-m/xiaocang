#ifndef __BSP_LED_H
#define __BSP_LED_H
#include "stm32f4xx.h"

#define LED1_ON 		GPIO_ResetBits(GPIOE , GPIO_Pin_13)
#define LED2_ON 		GPIO_ResetBits(GPIOE , GPIO_Pin_14)
#define LED3_ON 		GPIO_ResetBits(GPIOE , GPIO_Pin_15)

#define LED1_OFF 		GPIO_SetBits(GPIOE , GPIO_Pin_13)
#define LED2_OFF 		GPIO_SetBits(GPIOE , GPIO_Pin_14)
#define LED3_OFF 		GPIO_SetBits(GPIOE , GPIO_Pin_15)

#define LED3_TOGGLE GPIO_ToggleBits(GPIOE,GPIO_Pin_15)
#define LED2_TOGGLE GPIO_ToggleBits(GPIOE,GPIO_Pin_14)
#define LED1_TOGGLE GPIO_ToggleBits(GPIOE,GPIO_Pin_13)

void LED_GPIO_Config(void);

#endif
