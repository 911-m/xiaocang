/**
  ******************************************************************************
  * @file    bsp_bsp_adc.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   adc驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
/*
通道0	PA0
通道1	PA1
通道2	PA2
通道3	PA3
通道4	PA4
通道5	PA5
通道6	PA6
通道7	PA7
通道8	PB0
通道9	PB1
通道10	PC0
通道11	PC1
通道12	PC2
通道13	PC3
通道14	PC4
通道15	PC5
通道16	连接内部温度传感器
通道17	连接内部Vrefint
*/


#include "./adc/bsp_adc.h"

#define ADC_mode 0	//0手动获取  1中断轮询

static void Rheostat_ADC_GPIO_Config(u8 channel)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
	// 使能 GPIO 时钟
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
		
	// 配置 IO
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
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ; //不上拉不下拉
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
	
	
  // 开启ADC时钟
	RCC_APB2PeriphClockCmd(RHEOSTAT_ADC_CLK , ENABLE);
	
	RCC_APB2PeriphResetCmd(RHEOSTAT_ADC_CLK , ENABLE);
		RCC_APB2PeriphResetCmd(RHEOSTAT_ADC_CLK , DISABLE);
	
	ADC_TempSensorVrefintCmd(ENABLE); //使能内部温度传感器

  // -------------------ADC Common 结构体 参数 初始化------------------------
	// 独立ADC模式
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  // 时钟为fpclk x分频	
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
  // 禁止DMA直接访问模式	
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  // 采样时间间隔	
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;  
	
  ADC_CommonInit(&ADC_CommonInitStructure);
	
  // -------------------ADC Init 结构体 参数 初始化--------------------------
	ADC_StructInit(&ADC_InitStructure);
  // ADC 分辨率
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  // 禁止扫描模式，多通道采集才需要	
  ADC_InitStructure.ADC_ScanConvMode = DISABLE; 
	//ADC_InitStructure.ADC_ScanConvMode = ENABLE; 
  // 连续转换	
	#if ADC_mode == 0
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 
	#else
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
	#endif
  //禁止外部边沿触发
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  //外部触发通道，本例子使用软件触发，此值随便赋值即可
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  //数据右对齐	
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  //转换通道 1个
  ADC_InitStructure.ADC_NbrOfConversion = 1;                                    
  ADC_Init(RHEOSTAT_ADC, &ADC_InitStructure);
  //---------------------------------------------------------------------------
	
	#if ADC_mode == 1
	
// 配置 ADC 通道转换顺序为1，第一个转换，采样时间为3个时钟周期
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

  // ADC 转换结束产生中断，在中断服务程序中读取转换值
	

	ADC_ITConfig(RHEOSTAT_ADC, ADC_IT_EOC, ENABLE);


#endif

	// 使能ADC
  ADC_Cmd(RHEOSTAT_ADC, ENABLE);  
  //开始adc转换，软件触发
  //ADC_SoftwareStartConv(RHEOSTAT_ADC);
}

#if ADC_mode == 1
// 配置中断优先级
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
	ADC_RegularChannelConfig(RHEOSTAT_ADC, channel, 1, ADC_SampleTime_480Cycles); //设置通道规则
	ADC_SoftwareStartConv(RHEOSTAT_ADC);									 //开启软件触发
	while (!ADC_GetFlagStatus(RHEOSTAT_ADC, ADC_FLAG_EOC));
	return ADC_GetConversionValue(RHEOSTAT_ADC); //软件触发
}

float Get_Temprate(void)	//获取内部温度传感器温度值
{
		uint32_t adc_dr;
    short result;
    double temperature;
    adc_dr = Get_ADC_Value(ADC_Channel_16);
    temperature = (double)adc_dr / 4096 * 3.3; //电压值
    temperature = (temperature - 0.76) / 0.0025 + 25; //转换为温度值
    result = temperature *= 100; //扩大100倍
		result /= 100;
    return (float)result;
}


////////////////////////////////////////////
//中断轮询模式要把下面这段复制到stm32f4xx_it里面
////////////////////////////////////////////
//__IO uint16_t ADC_ConvertedValue[18];

//// ADC 转换完成中断服务程序
//void ADC_IRQHandler(void)
//{
//	static u8 i = 1;
//	if(ADC_GetITStatus(RHEOSTAT_ADC,ADC_IT_EOC)==SET)
//	{
//  // 读取ADC的转换值
//		ADC_ConvertedValue[i] = ADC_GetConversionValue(RHEOSTAT_ADC);
//		if(i < 17)i++;
//		else i = 1;
//	}
//	ADC_ClearITPendingBit(RHEOSTAT_ADC,ADC_IT_EOC);

//}	


