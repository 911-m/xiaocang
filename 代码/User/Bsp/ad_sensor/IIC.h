#ifndef _IIC__H
#define _IIC__H
#include "stm32f4xx.h" 
#include "sys.h"		

//IO操作函数	 


void BMP_IIC_Start(void);
void BMP_IIC_Stop(void);
void BMP_IIC_WriteByte(u8 txe);
//u8 BMP_IIC_Read_Byte(void);
void BMP_IIC_GPIO_Init(void);
u8 BMP_IIC_Wait_Ack(void);
u8 BMP_IIC_Read_Byte(u8 ack);
void BMP_IIC_Ack(void);//在主机产生脉冲之前，拉低SDA表示应答
void BMP_IIC_No_Ack(void);//在主机产生脉冲之前，拉高SDA表示非应答

#endif
