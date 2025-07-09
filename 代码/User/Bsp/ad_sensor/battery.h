/***************************************************************
  *	Name		:	TEMP_LIGHT.H
  *	Data		:	2019.10.5
  *	Author	:	NIEAOWEI
  *	Note		:	温度光照采集
****************************************************************/

#ifndef _BATTERY_H
#define _BATTERY_H
#include "stm32f4xx.h"

void Battery_Init(void);
double Get_Battery_value(void);
u8 Get_EmqxBattery_value(void);
#endif

