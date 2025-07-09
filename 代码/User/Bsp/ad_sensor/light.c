#include "light.h"
#include "./adc/bsp_adc.h"

#define LSENS_READ_TIMES	5		//���������������ȡ����,����ô���,Ȼ��ȡƽ��ֵ
 
//������ADͨ��
#define	LIGHT_CH	11			 

//Ӳ����·����
#define Vref 3.3     //�ο���ѹ����λ��V
 

void Light_Init()
{
  Rheostat_Init(LIGHT_CH);
}




//��ȡLight Sens��ֵ
//0~100:0,�;100,���� 
u8 Get_Light_value(void)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<LSENS_READ_TIMES;t++)
	{
		temp_val+=Get_ADC_Value(LIGHT_CH);	//��ȡADCֵ
	}
	temp_val/=LSENS_READ_TIMES;//�õ�ƽ��ֵ 
	if(temp_val>4000)temp_val=4000;
//	return (u8)(100-(temp_val/40));
	 return (u8)(temp_val/40);
}
