#include "HC_SR04.h"
#include "main.h"
/*******************************
����˵����
PA2  -- TRIG(���)
PA3  -- ECHO(����)
*******************************/
#define Sr04_GPIO GPIOB
#define Sr04_GPIO_RCC RCC_AHB1Periph_GPIOB

volatile uint32_t millis = 0;
// ���붨ʱ����
void Tim_delay_ms(uint32_t ms) {
    uint32_t start = millis;
    while ((millis - start) < ms);
}

void Tim_delay_us(uint32_t us) {
    uint32_t start = TIM4->CNT;
    while ((TIM4->CNT - start) < us);
}

static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 
  NVIC_Init(&NVIC_InitStructure);
}

static void TRIG_delay_us(uint16_t i)
{
	uint16_t t;
	for (t = 0; t < i*50; t++);
}

void Sr04_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseInitStruct;
	//1���ܶ�ʱ��ʱ�ӡ�
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_TimeBaseInitStruct.TIM_Period = 1000; // ��ʱ����Ϊ1ms
    TIM_TimeBaseInitStruct.TIM_Prescaler = 84 - 1; // Ԥ��Ƶ��Ϊ168MHz
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
	//2����ʼ����ʱ��������ARR,PSC��
  NVIC_Configuration();	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
   // ʹ�ܶ�ʱ�������ж�
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	//5����ʹ�ܶ�ʱ����
	TIM_Cmd(TIM4, ENABLE);	

}

void TIM4_IRQHandler(void) {
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
        // �����ﴦ��ʱ�ж��¼�
        millis++;
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}

