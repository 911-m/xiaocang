
#include "main.h"
#include "JXCT_TDS.h"

void JXCT_TDS_Init(void)
{
TDS_USART_Config();
}
static void tds_delay_ms(uint16_t i)
{
	uint32_t t;
	for (t = 0; t < i*60000; t++);
}

void JXCT_TDS_Get_TDS(float* TDS1,float* TDS2,uint16_t* TEMP1,uint16_t* TEMP2)
{
	//u16 TDS1,TDS2,TEMP1,TEMP2;
	static u8 cnt=0;
	while(!TDS_flag)
	{
	  cnt++;
		if(cnt>200)
		{
			cnt=0;
		  break;
		}
		tds_delay_ms(1);
	}
	//printf("³É¹¦");

	*(TEMP1) = TDS[4];
	*(TDS1) = ((TDS[0]*256 + TDS[1])+
						(*TEMP1-25)*0.0108) / 
						(1+(*TEMP1-25)*0.0388);
	*(TEMP2) = TDS[5];
	*(TDS2) = ((TDS[2]*256 + TDS[3])+
						(*TEMP2-25)*0.0108) / 
						(1+(*TEMP2-25)*0.0388);
}
