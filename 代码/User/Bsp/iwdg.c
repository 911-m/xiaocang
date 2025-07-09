#include "iwdg.h"
#include "main.h"

extern u8 volatile IWDG_REST_FLAG;
void IWDG_Init(u8 prer,u16 rlr)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(prer);
	IWDG_SetReload(rlr);
	IWDG_ReloadCounter();
	
	IWDG_Enable();


}

void My_IWDG_ReloadCounter(void)
{
  if(IWDG_REST_FLAG==0)
	{
	  IWDG_ReloadCounter();
	}
	else
	{
	  Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_System_Restart\r\n");	
	}
}


