/***************************************************************
  *	Name		:	TEMP_LIGHT.H
  *	Data		:	2019.10.5
  *	Author	:	NIEAOWEI
  *	Note		:	温度光照采集
****************************************************************/

#ifndef _LIGHT_H
#define _LIGHT_H
#include "stm32f4xx.h"

void Light_Init(void);
u8 Get_Light_value(void);
#endif

