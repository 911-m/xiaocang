/***************************************************************
  *	Name		:	TEMP_LIGHT.H
  *	Data		:	2019.10.5
  *	Author	:	NIEAOWEI
  *	Note		:	温度光照采集
****************************************************************/

#ifndef _WATER_H
#define _WATER_H
#include "stm32f4xx.h"

void Water_Init(void);
double Get_Water_value(void);
u16 Get_EmqxWater_value(void);
#endif

