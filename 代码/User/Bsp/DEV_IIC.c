#include "DEV_IIC.h" 

 
#define SDA_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;} //PB7 输入模式
#define SDA_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //PB7 输出模式

#define SDA_SET   PBout(7)=1
#define SDA_CLR   PBout(7)=0

#define SCK_SET   PBout(6)=1
#define SCK_CLR   PBout(6)=0

#define SDA_START   PBin(7)


#define IIC_GPIO_CLK  RCC_AHB1Periph_GPIOB

#define IIC_SDA_PIN   GPIO_Pin_7
#define IIC_SCL_PIN   GPIO_Pin_6


#define IIC_GPIOx    GPIOB


static void i2c_Delay()
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


//初始化IIC的IO口
void IIC_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(IIC_GPIO_CLK, ENABLE); //使能GPIOB时钟

    //GPIOB6,7初始化设置
    GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN | IIC_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
    GPIO_Init(IIC_GPIOx, &GPIO_InitStructure); //初始化
		SDA_SET;
		SCK_SET;
	  IIC_Stop();
}



void IIC_Start(void)
{
	SDA_SET;
	SCK_SET;
	i2c_Delay();
	SDA_CLR;
	i2c_Delay();
	SCK_CLR;
	i2c_Delay();
}

void IIC_Stop(void)
{
   SDA_OUT();
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	SDA_CLR;
	SCK_SET;
	i2c_Delay();
	SDA_SET;
}

u8 IIC_Wait_Ack(void)
{
	uint8_t re,ucErrTime=0;
   SDA_OUT();
	
	SCK_CLR;
	i2c_Delay();
	SDA_SET;	/* CPU释放SDA总线 */
	i2c_Delay();
	SCK_SET;	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
//   SDA_IN();
    while(SDA_START)  
    {  
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC_Stop();
            return 1;
        }
		  i2c_Delay();
    } 
	SCK_CLR;
	i2c_Delay();
	return 0;
}

void IIC_Ack(void)//在主机产生脉冲之前，拉低SDA表示应答
{

   SDA_OUT();
	
	SDA_CLR;	/* CPU驱动SDA = 1 */
	i2c_Delay();
	SCK_SET;	/* CPU产生1个时钟 */
	i2c_Delay();
	SCK_CLR;
	i2c_Delay();	
	SDA_SET;	 

}
void IIC_No_Ack(void)//在主机产生脉冲之前，拉高SDA表示非应答
{

   SDA_OUT();
	
	SDA_SET;	/* CPU驱动SDA = 1 */
	i2c_Delay();
	SCK_SET;	/* CPU产生1个时钟 */
	i2c_Delay();
	SCK_CLR;
	i2c_Delay();

}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC_Send_Byte(u8 txe)
{
	uint8_t i;
   SDA_OUT();
	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (txe & 0x80)
		{
			SDA_SET;
		}
		else
		{
			SDA_CLR;
		}
		i2c_Delay();
		
		SCK_SET;
		i2c_Delay();	
		SCK_CLR;
		if (i == 7)
		{
			 SDA_SET; // 释放总线
		}
		txe <<= 1;	/* 左移一个bit */
		i2c_Delay();
	}

}
u8 IIC_Read_Byte(u8 ack)
{
	uint8_t i;
	uint8_t value,Data;
//   SDA_IN();
	/* 读到第1个bit为数据的bit7 */
	for (i = 0; i < 8; i++)
	{
		Data<<=1;
		SCK_SET;
		i2c_Delay();
		if (SDA_START==1)
		{
			Data++;
		}
		SCK_CLR;
		i2c_Delay();
	}
 
	if(ack)
	{
	  IIC_Ack();
	}
	else
	{
	  IIC_No_Ack();
	}
	return Data;
}


