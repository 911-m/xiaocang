#include "water.h"
#include "./adc/bsp_adc.h"
#include "bsp_IOT_BC20.h"

 
//������ADͨ��
#define	Water_CH	9			 

//Ӳ����·����
#define Vref 3.3     //�ο���ѹ����λ��V
 

void Water_Init()
{
  Rheostat_Init(Water_CH);
}


 
double Get_Water_value()
{
	u16 adcx;
	double temp;
	
	adcx=Get_ADC_Value(Water_CH);
	temp = (((adcx/4096.0)*Vref)/3.3)*100.0;
	return (double)temp;
}

u16 Get_EmqxWater_value(void)
{
	u16 adcx;
	double temp;
	u8 bartty;
	adcx=Get_ADC_Value(Water_CH);
	printf("adcx %d\n",adcx);
//	temp = ((((adcx/4096.0)*Vref)));
//	if(temp>=3.0)
//	{
//	 bartty=(u8)((1-((3.3-temp)/(3.3-3.0)))*100.0);
//	}
	
	return adcx;
}

