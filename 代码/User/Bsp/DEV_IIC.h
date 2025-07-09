#ifndef _DEV_IIC__H
#define _DEV_IIC__H
 
#include "stm32f4xx.h"
#include "sys.h"		

  

#define NO_ACK    0   
#define ACK       1   

void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(u8 txe);
u8 IIC_Read_Byte(u8 ack);
void IIC_GPIO_Init(void);
u8 IIC_Wait_Ack(void);

#endif
