#include "bsp_Led.h"
/***************  ����LED�õ���I/O�� *******************/
void LED_GPIO_Config(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;
	/* Enable the GPIO_LED Clock */
	RCC_AHB1PeriphClockCmd(  RCC_AHB1Periph_GPIOE, ENABLE); 		
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14| GPIO_Pin_15 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	/*��ʼ����󣬹ر�3��LED*/ 
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
}

