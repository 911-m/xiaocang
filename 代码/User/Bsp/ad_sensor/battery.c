#include "battery.h"
#include "./adc/bsp_adc.h"


 
//������ADͨ��
#define	BATTERY_CH	10			 

//Ӳ����·����
#define Vref 3.3     //�ο���ѹ����λ��V
 

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

