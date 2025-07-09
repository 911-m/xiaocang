#ifndef __JXCT_TDS_H
#define	__JXCT_TDS_H

#include "stm32f4xx.h"

#define JXCT_TDS_USARTx 2

void JXCT_TDS_Init(void);
void JXCT_TDS_Get_TDS(float* TDS1,float* TDS2,uint16_t* TEMP1,uint16_t* TEMP2);
#endif
