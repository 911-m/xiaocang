/**
  ******************************************************************************
  * @file    bsp_bsp_adc.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   adc����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
/*
ͨ��0	PA0
ͨ��1	PA1
ͨ��2	PA2
ͨ��3	PA3
ͨ��4	PA4
ͨ��5	PA5
ͨ��6	PA6
ͨ��7	PA7
ͨ��8	PB0
ͨ��9	PB1
ͨ��10	PC0
ͨ��11	PC1
ͨ��12	PC2
ͨ��13	PC3
ͨ��14	PC4
ͨ��15	PC5
ͨ��16	�����ڲ��¶ȴ�����
ͨ��17	�����ڲ�Vrefint
*/


#include "./adc/bsp_adc.h"

#define ADC_mode 0	//0�ֶ���ȡ  1�ж���ѯ

static void Rheostat_ADC_GPIO_Config(u8 channel)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
	// ʹ�� GPIO ʱ��
	switch(channel)
	{
		case 0:	
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);break;
		case 8:
		case 9:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);break;
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);break;
		case 16:
		case 17:break;
		default:break;								
	};
		
	// ���� IO
	//GPIO_InitStructure.GPIO_Pin = ((uint16_t)(0x0001<<channel));
	switch(channel)
	{
		case 0:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;break;
		case 1:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;break;
		case 2:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;break;
		case 3:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;break;
		case 4:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;break;
		case 5:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;break;
		case 6:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;break;
		case 7:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;break;
		case 8:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;break;
		case 9:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;break;
		case 10:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;break;
		case 11:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;break;
		case 12:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;break;
		case 13:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;break;
		case 14:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;break;
		case 15:GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;break;
		case 16:
		case 17:break;
		default:break;
	}
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	    
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ; //������������
	switch(channel)
	{
		case 0:	
		case 1:	
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:GPIO_Init(GPIOA, &GPIO_InitStructure);break;
		case 8:
		case 9:GPIO_Init(GPIOB, &GPIO_InitStructure);break;
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:GPIO_Init(GPIOC, &GPIO_InitStructure);break;
		case 16:
		case 17:break;
		default:break;	
	};

}

static void Rheostat_ADC_Mode_Config()
{
	ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
	
  // ����ADCʱ��
	RCC_APB2PeriphClockCmd(RHEOSTAT_ADC_CLK , ENABLE);
	
	RCC_APB2PeriphResetCmd(RHEOSTAT_ADC_CLK , ENABLE);
		RCC_APB2PeriphResetCmd(RHEOSTAT_ADC_CLK , DISABLE);
	
	ADC_TempSensorVrefintCmd(ENABLE); //ʹ���ڲ��¶ȴ�����

  // -------------------ADC Common �ṹ�� ���� ��ʼ��------------------------
	// ����ADCģʽ
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  // ʱ��Ϊfpclk x��Ƶ	
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
  // ��ֹDMAֱ�ӷ���ģʽ	
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  // ����ʱ����	
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;  
	
  ADC_CommonInit(&ADC_CommonInitStructure);
	
  // -------------------ADC Init �ṹ�� ���� ��ʼ��--------------------------
	ADC_StructInit(&ADC_InitStructure);
  // ADC �ֱ���
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  // ��ֹɨ��ģʽ����ͨ���ɼ�����Ҫ	
  ADC_InitStructure.ADC_ScanConvMode = DISABLE; 
	//ADC_InitStructure.ADC_ScanConvMode = ENABLE; 
  // ����ת��	
	#if ADC_mode == 0
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 
	#else
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
	#endif
  //��ֹ�ⲿ���ش���
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  //�ⲿ����ͨ����������ʹ�������������ֵ��㸳ֵ����
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  //�����Ҷ���	
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  //ת��ͨ�� 1��
  ADC_InitStructure.ADC_NbrOfConversion = 1;                                    
  ADC_Init(RHEOSTAT_ADC, &ADC_InitStructure);
  //---------------------------------------------------------------------------
	
	#if ADC_mode == 1
	
// ���� ADC ͨ��ת��˳��Ϊ1����һ��ת��������ʱ��Ϊ3��ʱ������
	static u8 i = 1;
	switch(channel)
	{
		case 0:ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_Channel_0, i, ADC_SampleTime_3Cycles);break;
		case 1:ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_Channel_1, i, ADC_SampleTime_3Cycles);break;
		case 2:ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_Channel_2, i, ADC_SampleTime_3Cycles);break;
		case 3:ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_Channel_3, i, ADC_SampleTime_3Cycles);break;
		case 4:ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_Channel_4, i, ADC_SampleTime_3Cycles);break;
		case 5:ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_Channel_5, i, ADC_SampleTime_3Cycles);break;
		case 6:ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_Channel_6, i, ADC_SampleTime_3Cycles);break;
		case 7:ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_Channel_7, i, ADC_SampleTime_3Cycles);break;
		case 8:ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_Channel_8, i, ADC_SampleTime_3Cycles);break;
		case 9:ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_Channel_9, i, ADC_SampleTime_3Cycles);break;
		case 10:ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_Channel_10, i, ADC_SampleTime_3Cycles);break;
		case 11:ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_Channel_11, i, ADC_SampleTime_3Cycles);break;
		case 12:ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_Channel_12, i, ADC_SampleTime_3Cycles);break;
		case 13:ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_Channel_13, i, ADC_SampleTime_3Cycles);break;
		case 14:ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_Channel_14, i, ADC_SampleTime_3Cycles);break;
		case 15:ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_Channel_15, i, ADC_SampleTime_3Cycles);break;
		case 16:ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_Channel_16, i, ADC_SampleTime_3Cycles);break;
		case 17:ADC_RegularChannelConfig(RHEOSTAT_ADC, ADC_Channel_17, i, ADC_SampleTime_3Cycles);break;
		default:break;	
	};
	i++;

  // ADC ת�����������жϣ����жϷ�������ж�ȡת��ֵ
	

	ADC_ITConfig(RHEOSTAT_ADC, ADC_IT_EOC, ENABLE);


#endif

	// ʹ��ADC
  ADC_Cmd(RHEOSTAT_ADC, ENABLE);  
  //��ʼadcת�����������
  //ADC_SoftwareStartConv(RHEOSTAT_ADC);
}

#if ADC_mode == 1
// �����ж����ȼ�
static void Rheostat_ADC_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
  NVIC_InitStructure.NVIC_IRQChannel = Rheostat_ADC_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
  NVIC_Init(&NVIC_InitStructure);
}
#endif


void Rheostat_Init(u8 channel)
{
	if(channel < 16)
	Rheostat_ADC_GPIO_Config(channel);
	
	Rheostat_ADC_Mode_Config();
#if ADC_mode == 1
	Rheostat_ADC_NVIC_Config();
#endif
}

void MCU_temp_init()
{
	Rheostat_Init(16);
}
u16 Get_ADC_Value(u8 channel)
{
	ADC_RegularChannelConfig(RHEOSTAT_ADC, channel, 1, ADC_SampleTime_480Cycles); //����ͨ������
	ADC_SoftwareStartConv(RHEOSTAT_ADC);									 //�����������
	while (!ADC_GetFlagStatus(RHEOSTAT_ADC, ADC_FLAG_EOC));
	return ADC_GetConversionValue(RHEOSTAT_ADC); //�������
}

float Get_Temprate(void)	//��ȡ�ڲ��¶ȴ������¶�ֵ
{
		uint32_t adc_dr;
    short result;
    double temperature;
    adc_dr = Get_ADC_Value(ADC_Channel_16);
    temperature = (double)adc_dr / 4096 * 3.3; //��ѹֵ
    temperature = (temperature - 0.76) / 0.0025 + 25; //ת��Ϊ�¶�ֵ
    result = temperature *= 100; //����100��
		result /= 100;
    return (float)result;
}


////////////////////////////////////////////
//�ж���ѯģʽҪ��������θ��Ƶ�stm32f4xx_it����
////////////////////////////////////////////
//__IO uint16_t ADC_ConvertedValue[18];

//// ADC ת������жϷ������
//void ADC_IRQHandler(void)
//{
//	static u8 i = 1;
//	if(ADC_GetITStatus(RHEOSTAT_ADC,ADC_IT_EOC)==SET)
//	{
//  // ��ȡADC��ת��ֵ
//		ADC_ConvertedValue[i] = ADC_GetConversionValue(RHEOSTAT_ADC);
//		if(i < 17)i++;
//		else i = 1;
//	}
//	ADC_ClearITPendingBit(RHEOSTAT_ADC,ADC_IT_EOC);

//}	


