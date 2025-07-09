#include "IIC.h" 
#include "sys.h"



//========================================================
//                   BMP_IIC管脚配置
//========================================================
#define BMP_IIC_GPIO_CLK  RCC_AHB1Periph_GPIOC
#define BMP_IIC_GPIOx    GPIOC
#define BMP_IIC_SCL_PIN   GPIO_Pin_6
#define BMP_IIC_SDA_PIN   GPIO_Pin_5

#define BMP_IIC_SCL_SPEED 4				//BMP_IIC总线通信速度单位：us
//========================================================


#define BMP_IIC_SCL(n) GPIO_WriteBit(BMP_IIC_GPIOx,BMP_IIC_SCL_PIN,(BitAction)n)
#define BMP_IIC_SDA(n) GPIO_WriteBit(BMP_IIC_GPIOx,BMP_IIC_SDA_PIN,(BitAction)n)
#define READ_SDA   GPIO_ReadInputDataBit(BMP_IIC_GPIOx,BMP_IIC_SDA_PIN)  //输入SDA 

void SDA_OUT()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
 // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能 GPIOA, GPIOD 时钟
  //初始化设置	
  GPIO_InitStructure.GPIO_Pin = BMP_IIC_SDA_PIN ;         //LED0--PA8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      //普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //上拉
  GPIO_Init(BMP_IIC_GPIOx, &GPIO_InitStructure);              //初始化GPIO
}
void SDA_IN()
{
		  GPIO_InitTypeDef  GPIO_InitStructure;
 // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能 GPIOA, GPIOD 时钟
  //初始化设置
  GPIO_InitStructure.GPIO_Pin = BMP_IIC_SDA_PIN ;         //LED0--PA8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;      //普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //上拉
  GPIO_Init(BMP_IIC_GPIOx, &GPIO_InitStructure);              //初始化GPIO
}

void BMP_IIC_GPIO_Init(void)
{
	
	GPIO_InitTypeDef Init_GPIO_Structure;
	RCC_AHB1PeriphClockCmd(BMP_IIC_GPIO_CLK,ENABLE);

	Init_GPIO_Structure.GPIO_Pin=BMP_IIC_SDA_PIN|BMP_IIC_SCL_PIN;
	Init_GPIO_Structure.GPIO_Mode=GPIO_Mode_OUT;
	Init_GPIO_Structure.GPIO_OType = GPIO_OType_PP;     //推挽输出
	Init_GPIO_Structure.GPIO_Speed=GPIO_Speed_100MHz;

	GPIO_Init(BMP_IIC_GPIOx,&Init_GPIO_Structure);

	BMP_IIC_SCL(1);
	BMP_IIC_SDA(1);
}

static void delay_us(u8 x)
{
	uint8_t i;

	/*　
	 	下面的时间是通过逻辑分析仪测试得到的。
    工作条件：CPU主频72MHz ，MDK编译环境，1级优化
  
		循环次数为10时，SCL频率 = 205KHz 
		循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us 
	 	循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us 
	*/
	for (i = 0; i < 30; i++);
}

void BMP_IIC_Start(void)
{
	SDA_OUT();     	//sda线输出
	BMP_IIC_SDA(1);
	BMP_IIC_SCL(1);
	delay_us(4);
	BMP_IIC_SDA(0);
	delay_us(4);
	BMP_IIC_SCL(0);
}

void BMP_IIC_Stop(void)
{
	SDA_OUT();     	//sda线输出
	BMP_IIC_SDA(0);
	BMP_IIC_SCL(0);
	delay_us(4);
	BMP_IIC_SCL(1);
	BMP_IIC_SDA(1);
	delay_us(4);
}

u8 BMP_IIC_Wait_Ack(void)
{
uint16_t i;
	SDA_IN();      //SDA设置为输入  
	BMP_IIC_SDA(1);
	delay_us(1);
	BMP_IIC_SCL(1);//拉高后等待SDA拉低应答
	delay_us(1);
	while(READ_SDA)
	{
	i++;
	delay_us(5);
		if(i>250)
		{
		BMP_IIC_Stop(); //无应答自接返回
			return 0;
		}
	}
		BMP_IIC_SCL(0);

   return 1;
}

void BMP_IIC_Ack(void)//在主机产生脉冲之前，拉低SDA表示应答
{
  //SDA_OUT();
	
 	BMP_IIC_SCL(0);
	SDA_OUT();     	//sda线输出
	BMP_IIC_SDA(0);//在主机产生脉冲之前，拉低SDA表示应答
	delay_us(2);
	BMP_IIC_SCL(1);
	delay_us(1);
	BMP_IIC_SCL(0);

}
void BMP_IIC_No_Ack(void)//在主机产生脉冲之前，拉高SDA表示非应答
{

	BMP_IIC_SCL(0);
	SDA_OUT();     	//sda线输出
	BMP_IIC_SDA(1);//在主机产生脉冲之前，拉高SDA表示非应答
	delay_us(2);
	BMP_IIC_SCL(1);
	delay_us(1);
	BMP_IIC_SCL(0);

}
//BMP_IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void BMP_IIC_WriteByte(u8 txe)
{
  u8 i;
	SDA_OUT();     	//sda线输出
	BMP_IIC_SCL(0);

	for(i=0;i<8;i++)
	{
        BMP_IIC_SDA(((txe&0x80)>>7));//写入
        txe<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		BMP_IIC_SCL(1);  //数据总是在SCL高电平写入
		delay_us(BMP_IIC_SCL_SPEED); 
		BMP_IIC_SCL(0);	
		delay_us(2);
	}
 

}
u8 BMP_IIC_Read_Byte(u8 ack)
{
	uint8_t i;
	uint8_t Data;
	SDA_IN();       //SDA设置为输入  
	
	for(i=0;i<8;i++)
	{		
		Data<<=1;
		BMP_IIC_SCL(1);
		delay_us(BMP_IIC_SCL_SPEED);//数据总是在SCL高电平读取
		

		if(READ_SDA==1)//读取
		{
		Data++;
		}
		BMP_IIC_SCL(0);
		delay_us(1); 

	}
  if(ack)
	{
	  BMP_IIC_Ack();
	}
	else
	{
	  BMP_IIC_No_Ack();
	}
   return Data;
}


