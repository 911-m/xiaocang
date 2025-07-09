#ifndef __BSP_SENSORS_H
#define __BSP_SENSORS_H
#include "stm32f4xx.h"

typedef struct sensors_msg
{
	char *latstr;
	char *lonstr;
	u8 battery;
	u8 waterstate;
	double PH;
	u8 temp;
	double turbidity;
	u8 powerswitch;
  uint16_t tds;
	u8 cpu_temp;
	u8 temperature_DHT11; 	    
	u8 humidity_DHT11;
	double Pressure;
	double H;
	u8 light;
	
}SENSORS_MSG;

typedef struct sensors_source
{
	double PH;//PHֵ��6.5-8.5
	int temp;//�¶���0-50���϶�
	double turbidity;//�Ƕ���0-100NTU
    int tds;//0-500ppm
    int service;//0-100
	  u8 greap;
}SOURCE_MSG;
extern SOURCE_MSG Source_Msg;
extern SENSORS_MSG Sensors_Msg;
 
void Sensores_Data_Proessing(void);
#endif
