
#include "./fire_phoenix_esc/fire_phoenix_esc.h"
#include "ship_route.h"

#define JS_1_PWM 2250
void ch_ctrl_init()
{
		GPIO_InitTypeDef GPIO_InitStructure;
		/*开启LED相关的GPIO外设时钟*/
		RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOA, ENABLE); 

		/*选择要控制的GPIO引脚*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	

		/*设置引脚模式为输出模式*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    
    /*设置引脚的输出类型为推挽输出*/
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    
    /*设置引脚为上拉模式*/
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		/*设置引脚速率为2MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 

		/*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
		
 
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
		
		
		
		/*开启LED相关的GPIO外设时钟*/
		RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOA, ENABLE); 

		/*选择要控制的GPIO引脚*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	

		/*设置引脚模式为输出模式*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    
    /*设置引脚的输出类型为推挽输出*/
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    
    /*设置引脚为上拉模式*/
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		/*设置引脚速率为2MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 

		/*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
		
		
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

//初始化两个电机，占用TIM2和TIM3
void fire_phonenix_esc_init()
{
	

	
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	
	
	
	
	/*开启相关的GPIO外设时钟*/
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); 
  /* 定时器通道引脚复用 */
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM2); 
  
	/* 定时器通道引脚配置 */															   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);

		
	// 开启TIMx_CLK,x[2,3,4,5,12,13,14] 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 

  /* 累计 TIM_Period个后产生一个更新或者中断*/		
  //当定时器从0计数到8399，即为8400次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Period = 30000-1;       
	
	// 通用控制定时器时钟源TIMxCLK = HCLK/2=84MHz 
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=100KHz
  TIM_TimeBaseStructure.TIM_Prescaler = 56-1;	
  // 采样时钟分频
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // 计数方式
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	// 初始化定时器TIMx, x[2,3,4,5,12,13,14] 
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	/*PWM模式配置*/
	/* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = JS_1_PWM;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	  //当定时器计数值小于CCR1_Val时为高电平
  TIM_OC1Init(TIM2, &TIM_OCInitStructure);	 //使能通道1
  
	/*使能通道1重载*/
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	
	TIM_ARRPreloadConfig(TIM2,ENABLE);//ARPE使能 
	
	// 使能定时器
	TIM_Cmd(TIM2, ENABLE);	
	
	
	
		/*开启相关的GPIO外设时钟*/
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); 
  /* 定时器通道引脚复用 */
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3); 
  
	/* 定时器通道引脚配置 */															   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	

	
		// 开启TIMx_CLK,x[2,3,4,5,12,13,14] 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 

  /* 累计 TIM_Period个后产生一个更新或者中断*/		
  //当定时器从0计数到8399，即为8400次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Period = 30000-1;       
	
	// 通用控制定时器时钟源TIMxCLK = HCLK/2=84MHz 
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=10KHz 100us
  TIM_TimeBaseStructure.TIM_Prescaler = 56-1;	
  // 采样时钟分频
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // 计数方式
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	// 初始化定时器TIMx, x[2,3,4,5,12,13,14] 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/*PWM模式配置*/
	/* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = JS_1_PWM;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	  //当定时器计数值小于CCR1_Val时为高电平
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);	 //使能通道1
  
	/*使能通道1重载*/
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	
		TIM_ARRPreloadConfig(TIM3,ENABLE);//ARPE使能 
		
	// 使能定时器
	TIM_Cmd(TIM3, ENABLE);	
	
	
		ch_ctrl_init();
	
}


void RZ7888_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);//使能 GPIOA, GPIOD 时钟

  //初始化设置
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_7 ;         //LED0--PA8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      //普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);              //初始化GPIO
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9 ;         //LED1--PD2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      //普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);              //初始化GPIO
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);

}


void BUZZER_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能 GPIOA, GPIOD 时钟

  //初始化设置
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;         //LED0--PA8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      //普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	
	GPIO_SetBits(GPIOB,GPIO_Pin_0);//设置高，灯灭
}

void BUZZER_OFF(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  //初始化设置
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;          
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;    
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;      
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	GPIO_SetBits(GPIOB,GPIO_Pin_0);//设置高，灯灭
}

void BUZZER_ON(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;         //LED0--PA8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      //普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //上拉
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
	 RZ7888_FI1=1; //正常可以抽水
}


void water_ctrl_release(void)
{
     RZ7888_BI2=1;
		 RZ7888_FI2=0; //正常可以放水
}

void water_ctrl_OFF(void)
{
		RZ7888_BI1=0;
		RZ7888_FI1=0; //正常可以抽水
	
		RZ7888_BI2=0;
		RZ7888_FI2=0; //正常可以放水
}

void water_ctrl_off_pumping(void)
{
		RZ7888_BI1=0;
		RZ7888_FI1=0; //正常可以抽水
}

void water_ctrl_off_release(void)
{
		RZ7888_BI2=0;
		RZ7888_FI2=0; //正常可以放水
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
 