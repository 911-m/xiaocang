#ifndef _IIC__H
#define _IIC__H
#include "stm32f4xx.h" 
#include "sys.h"		

//IO��������	 


void BMP_IIC_Start(void);
void BMP_IIC_Stop(void);
void BMP_IIC_WriteByte(u8 txe);
//u8 BMP_IIC_Read_Byte(void);
void BMP_IIC_GPIO_Init(void);
u8 BMP_IIC_Wait_Ack(void);
u8 BMP_IIC_Read_Byte(u8 ack);
void BMP_IIC_Ack(void);//��������������֮ǰ������SDA��ʾӦ��
void BMP_IIC_No_Ack(void);//��������������֮ǰ������SDA��ʾ��Ӧ��

#endif
