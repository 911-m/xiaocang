#include "DEV_IIC.h" 

 
#define SDA_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;} //PB7 ����ģʽ
#define SDA_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //PB7 ���ģʽ

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

	/*��
	 	�����ʱ����ͨ���߼������ǲ��Եõ��ġ�
    ����������CPU��Ƶ72MHz ��MDK���뻷����1���Ż�
  
		ѭ������Ϊ10ʱ��SCLƵ�� = 205KHz 
		ѭ������Ϊ7ʱ��SCLƵ�� = 347KHz�� SCL�ߵ�ƽʱ��1.5us��SCL�͵�ƽʱ��2.87us 
	 	ѭ������Ϊ5ʱ��SCLƵ�� = 421KHz�� SCL�ߵ�ƽʱ��1.25us��SCL�͵�ƽʱ��2.375us 
	*/
	for (i = 0; i < 30; i++);
}


//��ʼ��IIC��IO��
void IIC_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(IIC_GPIO_CLK, ENABLE); //ʹ��GPIOBʱ��

    //GPIOB6,7��ʼ������
    GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN | IIC_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
    GPIO_Init(IIC_GPIOx, &GPIO_InitStructure); //��ʼ��
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
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź� */
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
	SDA_SET;	/* CPU�ͷ�SDA���� */
	i2c_Delay();
	SCK_SET;	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
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

void IIC_Ack(void)//��������������֮ǰ������SDA��ʾӦ��
{

   SDA_OUT();
	
	SDA_CLR;	/* CPU����SDA = 1 */
	i2c_Delay();
	SCK_SET;	/* CPU����1��ʱ�� */
	i2c_Delay();
	SCK_CLR;
	i2c_Delay();	
	SDA_SET;	 

}
void IIC_No_Ack(void)//��������������֮ǰ������SDA��ʾ��Ӧ��
{

   SDA_OUT();
	
	SDA_SET;	/* CPU����SDA = 1 */
	i2c_Delay();
	SCK_SET;	/* CPU����1��ʱ�� */
	i2c_Delay();
	SCK_CLR;
	i2c_Delay();

}
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void IIC_Send_Byte(u8 txe)
{
	uint8_t i;
   SDA_OUT();
	/* �ȷ����ֽڵĸ�λbit7 */
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
			 SDA_SET; // �ͷ�����
		}
		txe <<= 1;	/* ����һ��bit */
		i2c_Delay();
	}

}
u8 IIC_Read_Byte(u8 ack)
{
	uint8_t i;
	uint8_t value,Data;
//   SDA_IN();
	/* ������1��bitΪ���ݵ�bit7 */
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


