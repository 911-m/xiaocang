/***************************************************************
  *	Name		:	TEMP_LIGHT.H
  *	Data		:	2019.10.5
  *	Author	:	NIEAOWEI
  *	Note		:	温度光照采集
****************************************************************/

#ifndef _HC_SR04_H
#define _HC_SR04_H
#include "stm32f4xx.h"
#include "sys.h"		

#define TRIG   PBout(4)
#define ECHO   PBin(5)
void Tim_delay_ms(uint32_t ms);
void Tim_delay_us(uint32_t us);
void Sr04_Init(void);
#endif

