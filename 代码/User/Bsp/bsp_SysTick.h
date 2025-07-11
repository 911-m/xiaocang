#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"

void SysTick_Init(void);
void TimingDelay_Decrement(void);
void Delay_ms(__IO u32 nTime);
void Delay_10ms(__IO u32 nTime);

#endif /* __SYSTICK_H */
