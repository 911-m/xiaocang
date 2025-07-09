#ifndef __IWGD_H
#define __IWGD_H
#include "stm32f4xx.h"

 
//prer是预分频系数，rlr重载值
void IWDG_Init(u8 prer,u16 rlr);
void My_IWDG_ReloadCounter(void);
#endif
