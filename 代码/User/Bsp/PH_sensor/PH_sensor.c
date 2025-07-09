

#include "./PH_sensor/PH_sensor.h"
#include "bsp_IOT_BC20.h"


void PH_sensor_Init()
{
	Rheostat_Init(PH_sensor_CH);
}

double Get_PH_Value()
{
	u16 ADC_Value;
	double phValue;
	double ph;
	ADC_Value = Get_ADC_Value(PH_sensor_CH);
 
	//因为传感器输出模拟量为5V，所以硬件电路上对模拟量进行了1/2分压
	//实际度数应该要*2再以5V为基准算出电压
	printf("\r\nADC14 = %d\r\n",ADC_Value);
	phValue = (double)ADC_Value / 4096 * 3.3;
 
	ph=-5.753*phValue+18.666;
	
	return ph;
}

