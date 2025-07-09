#include "light.h"
#include "./adc/bsp_adc.h"

#define LSENS_READ_TIMES	5		//定义光敏传感器读取次数,读这么多次,然后取平均值
 
//传感器AD通道
#define	LIGHT_CH	11			 

//硬件电路参数
#define Vref 3.3     //参考电压，单位：V
 

void Light_Init()
{
  Rheostat_Init(LIGHT_CH);
}




//读取Light Sens的值
//0~100:0,最暗;100,最亮 
u8 Get_Light_value(void)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<LSENS_READ_TIMES;t++)
	{
		temp_val+=Get_ADC_Value(LIGHT_CH);	//读取ADC值
	}
	temp_val/=LSENS_READ_TIMES;//得到平均值 
	if(temp_val>4000)temp_val=4000;
//	return (u8)(100-(temp_val/40));
	 return (u8)(temp_val/40);
}
