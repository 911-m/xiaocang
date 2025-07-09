#include "battery.h"
#include "./adc/bsp_adc.h"


 
//传感器AD通道
#define	BATTERY_CH	10			 

//硬件电路参数
#define Vref 3.3     //参考电压，单位：V
 

void Battery_Init()
{
  Rheostat_Init(BATTERY_CH);
}


 
double Get_Battery_value()
{
	u16 adcx;
	double temp;
	
	adcx=Get_ADC_Value(BATTERY_CH);
	temp = (((adcx/4096.0)*Vref)/3.3)*100.0;
	return (double)temp;
}

u8 Get_EmqxBattery_value(void)
{
	u16 adcx;
	double temp;
	u8 bartty;
	adcx=Get_ADC_Value(BATTERY_CH);
	temp = ((((adcx/4096.0)*Vref)));
	if(temp>=3.0)
	{
	 bartty=(u8)((1-((3.3-temp)/(3.3-3.0)))*100.0);
	}
	
	return (u8)bartty;
}

