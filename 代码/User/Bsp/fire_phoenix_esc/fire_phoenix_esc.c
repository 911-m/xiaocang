
#include "./fire_phoenix_esc/fire_phoenix_esc.h"
#include "ship_route.h"

#define JS_1_PWM 2250
void ch_ctrl_init()
{
		GPIO_InitTypeDef GPIO_InitStructure;
		/*����LED��ص�GPIO����ʱ��*/
		RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOA, ENABLE); 

		/*ѡ��Ҫ���Ƶ�GPIO����*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	

		/*��������ģʽΪ���ģʽ*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    
    /*�������ŵ��������Ϊ�������*/
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    
    /*��������Ϊ����ģʽ*/
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		/*������������Ϊ2MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 

		/*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
		
 
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
		
		
		
		/*����LED��ص�GPIO����ʱ��*/
		RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOA, ENABLE); 

		/*ѡ��Ҫ���Ƶ�GPIO����*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	

		/*��������ģʽΪ���ģʽ*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    
    /*�������ŵ��������Ϊ�������*/
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    
    /*��������Ϊ����ģʽ*/
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		/*������������Ϊ2MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 

		/*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
		
		
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
		
		
		GPIO_WriteBit(GPIOA, GPIO_Pin_4, 1);
		GPIO_WriteBit(GPIOA, GPIO_Pin_5, 1);
		
}


void switch_ctrl_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(  RCC_AHB1Periph_GPIOA, ENABLE); 		
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	PWM1_MCU_ON;
	PWM2_MCU_ON;
}

//��ʼ�����������ռ��TIM2��TIM3
void fire_phonenix_esc_init()
{
	

	
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	
	
	
	
	/*������ص�GPIO����ʱ��*/
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); 
  /* ��ʱ��ͨ�����Ÿ��� */
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM2); 
  
	/* ��ʱ��ͨ���������� */															   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);

		
	// ����TIMx_CLK,x[2,3,4,5,12,13,14] 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 

  /* �ۼ� TIM_Period�������һ�����»����ж�*/		
  //����ʱ����0������8399����Ϊ8400�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Period = 30000-1;       
	
	// ͨ�ÿ��ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK/2=84MHz 
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=100KHz
  TIM_TimeBaseStructure.TIM_Prescaler = 56-1;	
  // ����ʱ�ӷ�Ƶ
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // ������ʽ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	// ��ʼ����ʱ��TIMx, x[2,3,4,5,12,13,14] 
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	/*PWMģʽ����*/
	/* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = JS_1_PWM;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	  //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ
  TIM_OC1Init(TIM2, &TIM_OCInitStructure);	 //ʹ��ͨ��1
  
	/*ʹ��ͨ��1����*/
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	
	TIM_ARRPreloadConfig(TIM2,ENABLE);//ARPEʹ�� 
	
	// ʹ�ܶ�ʱ��
	TIM_Cmd(TIM2, ENABLE);	
	
	
	
		/*������ص�GPIO����ʱ��*/
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); 
  /* ��ʱ��ͨ�����Ÿ��� */
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3); 
  
	/* ��ʱ��ͨ���������� */															   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	

	
		// ����TIMx_CLK,x[2,3,4,5,12,13,14] 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 

  /* �ۼ� TIM_Period�������һ�����»����ж�*/		
  //����ʱ����0������8399����Ϊ8400�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Period = 30000-1;       
	
	// ͨ�ÿ��ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK/2=84MHz 
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=10KHz 100us
  TIM_TimeBaseStructure.TIM_Prescaler = 56-1;	
  // ����ʱ�ӷ�Ƶ
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // ������ʽ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	// ��ʼ����ʱ��TIMx, x[2,3,4,5,12,13,14] 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/*PWMģʽ����*/
	/* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = JS_1_PWM;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	  //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);	 //ʹ��ͨ��1
  
	/*ʹ��ͨ��1����*/
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	
		TIM_ARRPreloadConfig(TIM3,ENABLE);//ARPEʹ�� 
		
	// ʹ�ܶ�ʱ��
	TIM_Cmd(TIM3, ENABLE);	
	
	
		ch_ctrl_init();
	
}


void RZ7888_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);//ʹ�� GPIOA, GPIOD ʱ��

  //��ʼ������
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_7 ;         //LED0--PA8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //����
  GPIO_Init(GPIOA, &GPIO_InitStructure);              //��ʼ��GPIO
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9 ;         //LED1--PD2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //����
  GPIO_Init(GPIOB, &GPIO_InitStructure);              //��ʼ��GPIO
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);

}


void BUZZER_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ�� GPIOA, GPIOD ʱ��

  //��ʼ������
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;         //LED0--PA8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //����
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	
	GPIO_SetBits(GPIOB,GPIO_Pin_0);//���øߣ�����
}

void BUZZER_OFF(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  //��ʼ������
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;          
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;    
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;      
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	GPIO_SetBits(GPIOB,GPIO_Pin_0);//���øߣ�����
}

void BUZZER_ON(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;         //LED0--PA8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //����
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
}
static void buzzer_delay_ms(uint16_t i)
{
	uint32_t t;
	for (t = 0; t < i*60000; t++);
}

void buzzer_run(void)
{

  BUZZER=1;
	
}


void water_ctrl_pumping(void)
{
	 RZ7888_BI1=0;
	 RZ7888_FI1=1; //�������Գ�ˮ
}


void water_ctrl_release(void)
{
     RZ7888_BI2=1;
		 RZ7888_FI2=0; //�������Է�ˮ
}

void water_ctrl_OFF(void)
{
		RZ7888_BI1=0;
		RZ7888_FI1=0; //�������Գ�ˮ
	
		RZ7888_BI2=0;
		RZ7888_FI2=0; //�������Է�ˮ
}

void water_ctrl_off_pumping(void)
{
		RZ7888_BI1=0;
		RZ7888_FI1=0; //�������Գ�ˮ
}

void water_ctrl_off_release(void)
{
		RZ7888_BI2=0;
		RZ7888_FI2=0; //�������Է�ˮ
}

void set_left_motor_speed(int8_t speed)
{
	uint16_t temp;
	temp = 150 +  (100 * (speed / 100.0));
	TIM_SetCompare1(TIM2,temp);
	return;
}

void set_right_motor_speed(int8_t speed)
{
	uint16_t temp;
	temp = 150 +  (100 * (speed / 100.0));
	TIM_SetCompare1(TIM3,temp);
	return;
}


void set_right_speed(int speed)
{
	uint16_t temp=JS_1_PWM;
	
	if(speed>290)
	{
	   speed=290;
	}
	else if(speed<-290)
	{
		speed=-290;
	}
		
  if (speed < 0) 
	{
    speed = -speed;
		temp-=(speed);
  }
	else if(speed > 0)
	{
		temp+=(speed);
	}
	else
	{
	
	}

	TIM_SetCompare1(TIM2,temp);
	return;
}

void set_left_speed(int speed)
{
	uint16_t temp=JS_1_PWM;
	
	if(speed>290)
	{
	   speed=290;
	}
	else if(speed<-290)
	{
		speed=-290;
	}
	
  if (speed < 0) 
	{
    speed = -speed;
		temp-=(speed);
  }
	else if (speed >0) 
	{
		temp+=(speed);
	}
	else
	{
	
	}


	TIM_SetCompare1(TIM3,temp);
	return;
}
 