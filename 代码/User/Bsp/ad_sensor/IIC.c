#include "IIC.h" 
#include "sys.h"



//========================================================
//                   BMP_IIC�ܽ�����
//========================================================
#define BMP_IIC_GPIO_CLK  RCC_AHB1Periph_GPIOC
#define BMP_IIC_GPIOx    GPIOC
#define BMP_IIC_SCL_PIN   GPIO_Pin_6
#define BMP_IIC_SDA_PIN   GPIO_Pin_5

#define BMP_IIC_SCL_SPEED 4				//BMP_IIC����ͨ���ٶȵ�λ��us
//========================================================


#define BMP_IIC_SCL(n) GPIO_WriteBit(BMP_IIC_GPIOx,BMP_IIC_SCL_PIN,(BitAction)n)
#define BMP_IIC_SDA(n) GPIO_WriteBit(BMP_IIC_GPIOx,BMP_IIC_SDA_PIN,(BitAction)n)
#define READ_SDA   GPIO_ReadInputDataBit(BMP_IIC_GPIOx,BMP_IIC_SDA_PIN)  //����SDA 

void SDA_OUT()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
 // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ�� GPIOA, GPIOD ʱ��
  //��ʼ������	
  GPIO_InitStructure.GPIO_Pin = BMP_IIC_SDA_PIN ;         //LED0--PA8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //����
  GPIO_Init(BMP_IIC_GPIOx, &GPIO_InitStructure);              //��ʼ��GPIO
}
void SDA_IN()
{
		  GPIO_InitTypeDef  GPIO_InitStructure;
 // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ�� GPIOA, GPIOD ʱ��
  //��ʼ������
  GPIO_InitStructure.GPIO_Pin = BMP_IIC_SDA_PIN ;         //LED0--PA8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;      //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       //����
  GPIO_Init(BMP_IIC_GPIOx, &GPIO_InitStructure);              //��ʼ��GPIO
}

void BMP_IIC_GPIO_Init(void)
{
	
	GPIO_InitTypeDef Init_GPIO_Structure;
	RCC_AHB1PeriphClockCmd(BMP_IIC_GPIO_CLK,ENABLE);

	Init_GPIO_Structure.GPIO_Pin=BMP_IIC_SDA_PIN|BMP_IIC_SCL_PIN;
	Init_GPIO_Structure.GPIO_Mode=GPIO_Mode_OUT;
	Init_GPIO_Structure.GPIO_OType = GPIO_OType_PP;     //�������
	Init_GPIO_Structure.GPIO_Speed=GPIO_Speed_100MHz;

	GPIO_Init(BMP_IIC_GPIOx,&Init_GPIO_Structure);

	BMP_IIC_SCL(1);
	BMP_IIC_SDA(1);
}

static void delay_us(u8 x)
{
	uint8_t i;

	/*��
	 	�����ʱ����ͨ���߼������ǲ��Եõ��ġ�
    ����������CPU��Ƶ72MHz ��MDK���뻷����1���Ż�
  
		ѭ������Ϊ10ʱ��SCLƵ�� = 205KHz 
		ѭ������Ϊ7ʱ��SCLƵ�� = 347KHz�� SCL�ߵ�ƽʱ��1.5us��SCL�͵�ƽʱ��2.87us 
	 	ѭ������Ϊ5ʱ��SCLƵ�� = 421KHz�� SCL�ߵ�ƽʱ��1.25us��SCL�͵�ƽʱ��2.375us 
	*/
	for (i = 0; i < 30; i++);
}

void BMP_IIC_Start(void)
{
	SDA_OUT();     	//sda�����
	BMP_IIC_SDA(1);
	BMP_IIC_SCL(1);
	delay_us(4);
	BMP_IIC_SDA(0);
	delay_us(4);
	BMP_IIC_SCL(0);
}

void BMP_IIC_Stop(void)
{
	SDA_OUT();     	//sda�����
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
	SDA_IN();      //SDA����Ϊ����  
	BMP_IIC_SDA(1);
	delay_us(1);
	BMP_IIC_SCL(1);//���ߺ�ȴ�SDA����Ӧ��
	delay_us(1);
	while(READ_SDA)
	{
	i++;
	delay_us(5);
		if(i>250)
		{
		BMP_IIC_Stop(); //��Ӧ���Խӷ���
			return 0;
		}
	}
		BMP_IIC_SCL(0);

   return 1;
}

void BMP_IIC_Ack(void)//��������������֮ǰ������SDA��ʾӦ��
{
  //SDA_OUT();
	
 	BMP_IIC_SCL(0);
	SDA_OUT();     	//sda�����
	BMP_IIC_SDA(0);//��������������֮ǰ������SDA��ʾӦ��
	delay_us(2);
	BMP_IIC_SCL(1);
	delay_us(1);
	BMP_IIC_SCL(0);

}
void BMP_IIC_No_Ack(void)//��������������֮ǰ������SDA��ʾ��Ӧ��
{

	BMP_IIC_SCL(0);
	SDA_OUT();     	//sda�����
	BMP_IIC_SDA(1);//��������������֮ǰ������SDA��ʾ��Ӧ��
	delay_us(2);
	BMP_IIC_SCL(1);
	delay_us(1);
	BMP_IIC_SCL(0);

}
//BMP_IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void BMP_IIC_WriteByte(u8 txe)
{
  u8 i;
	SDA_OUT();     	//sda�����
	BMP_IIC_SCL(0);

	for(i=0;i<8;i++)
	{
        BMP_IIC_SDA(((txe&0x80)>>7));//д��
        txe<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		BMP_IIC_SCL(1);  //����������SCL�ߵ�ƽд��
		delay_us(BMP_IIC_SCL_SPEED); 
		BMP_IIC_SCL(0);	
		delay_us(2);
	}
 

}
u8 BMP_IIC_Read_Byte(u8 ack)
{
	uint8_t i;
	uint8_t Data;
	SDA_IN();       //SDA����Ϊ����  
	
	for(i=0;i<8;i++)
	{		
		Data<<=1;
		BMP_IIC_SCL(1);
		delay_us(BMP_IIC_SCL_SPEED);//����������SCL�ߵ�ƽ��ȡ
		

		if(READ_SDA==1)//��ȡ
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


