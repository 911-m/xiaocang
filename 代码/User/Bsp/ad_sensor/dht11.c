#include "dht11.h"
#include "HC_SR04.h"

static void dht11_delay_us(uint16_t i)
{
	uint16_t t;
	for (t = 0; t < i*30; t++);
}

static void dht11_delay_ms(uint16_t i)
{
	uint32_t t;
	for (t = 0; t < i*60000; t++);
}


void DHT11_IO_OUT(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能 GPIOA, GPIOD 时钟
  //初始化设置	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ;         //LED0--PA8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      //普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;     //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);              //初始化GPIO
	
}
void DHT11_IO_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能 GPIOA, GPIOD 时钟
  //初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ;         //LED0--PA8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;      //普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;     //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);              //初始化GPIO
}

//复位DHT11
void DHT11_Rst(void)	   
{                 
	DHT11_IO_OUT(); 	//SET OUTPUT
    DHT11_DQ_OUT=0; 	//拉低DQ
    dht11_delay_ms(20);    	//拉低至少18ms
    DHT11_DQ_OUT=1; 	//DQ=1 
	dht11_delay_us(30);     	//主机拉高20~40us
}
//等待DHT11的回应
//返回1:未检测到DHT11的存在
//返回0:存在
u8 DHT11_Check(void) 	   
{   
	u8 retry=0;
	DHT11_IO_IN();//SET INPUT	 
    while (DHT11_DQ_IN&&retry<100)//DHT11会拉低40~80us
	{
		retry++;
		dht11_delay_us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
    while (!DHT11_DQ_IN&&retry<100)//DHT11拉低后会再次拉高40~80us
	{
		retry++;
		dht11_delay_us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}
//从DHT11读取一个位
//返回值：1/0
u8 DHT11_Read_Bit(void) 			 
{
 	u8 retry=0;
	while(DHT11_DQ_IN&&retry<100)//等待变为低电平
	{
		retry++;
		dht11_delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN&&retry<100)//等待变高电平
	{
		retry++;
		dht11_delay_us(1);
	}
	dht11_delay_us(40);//等待40us
	if(DHT11_DQ_IN)return 1;
	else return 0;		   
}
//从DHT11读取一个字节
//返回值：读到的数据
u8 DHT11_Read_Byte(void)    
{        
    u8 i,dat;
    dat=0;
	for (i=0;i<8;i++) 
	{
   		dat<<=1; 
	    dat|=DHT11_Read_Bit();
    }						    
    return dat;
}
//从DHT11读取一次数据
//temp:温度值(范围:0~50°)
//humi:湿度值(范围:20%~90%)
//返回值：0,正常;1,读取失败
u8 DHT11_Read_Data(u8 *temp,u8 *humi)    
{        
 	u8 buf[5];
	u8 i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//读取40位数据
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[2];
		}
	}else return 1;
	return 0;	    
}
//初始化DHT11的IO口 DQ 同时检测DHT11的存在
//返回1:不存在
//返回0:存在    	 
u8 DHT11_Init(void)
{	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能 GPIOA, GPIOD 时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ;         //LED1--PD2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      //普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD        ;     //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);              //初始化GPIO
	
	GPIO_SetBits(GPIOA,GPIO_Pin_12);//设置高
			    
	DHT11_Rst();  //复位DHT11
	return DHT11_Check();//等待DHT11的回应
} 







