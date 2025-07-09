#include "bsp_Compass.h"
#include "bsp_debug_usart.h"
#include "DEV_IIC.h" 


COMPASS Gaussian_XYZ;
Correction_Parameter Gaussian_Parameter;


u8 Check_Device(void);
void HMC5883L_WriteOneByte(u16 WriteAddr, u8 DataToWrite);
void HMC5883L_Read_XYZ(void);

u8 Compass_Init(void)
{
  IIC_Init();
  if(Check_Device())
	{
	  printf("器件无应答\n");
		return 0;
	}
	else
	{
	  printf("器件应答\n");
	}
    
		HMC5883L_WriteOneByte(HMC5883_REG_CRA,HMC5883_REG_CRA_MS_XYZ_P|HMC5883_REG_CRA_DO_Hz_15|HMC5883_REG_CRA_MA_AVG_8);
		HMC5883L_WriteOneByte(HMC5883_REG_CRB,HMC5883_REG_GN_1_30_G1090);
		HMC5883L_WriteOneByte(HMC5883_REG_MOD,HMC5883_REG_MODE_MR_SIG);
	
//		Gaussian_Parameter.Xoffest = 0.314220;
//		Gaussian_Parameter.Yoffest = 0.668807;
//		Gaussian_Parameter.Kx = 1.000000;
//		Gaussian_Parameter.Ky = 0.564472;
	
}
//	Xoffest =0.314220;
//	Yoffest =0.668807;
//	Kx = 1.000000;
//	Ky = 0.564472;
//		Gaussian_Parameter.Xoffest = 0.077064;
//		Gaussian_Parameter.Yoffest = -0.429817;
//		Gaussian_Parameter.Kx = 1.000000;
//		Gaussian_Parameter.Ky = 0.942828;
u8 Check_Device(void)
{
    IIC_Start();
    IIC_Send_Byte(HMC5883_ADDR); //发送写命令
    if(IIC_Wait_Ack())
		{
		  return 1;
		}	
		  return 0;
}

u8 HMC5883L_ReadNByte(u8 reg,u8 *buf,u8 len)
{ 
		IIC_Start(); 
	
		IIC_Send_Byte((HMC5883_ADDR));//发送器件地址+写命令	
	
    IIC_Wait_Ack();
		
		IIC_Send_Byte(reg);	//写寄存器地址
    IIC_Wait_Ack();
	
		IIC_Start();
		IIC_Send_Byte((HMC5883_ADDR)|1);//发送器件地址+读命令	
    IIC_Wait_Ack();


	
	while(len)
	{
		if(len==1)*buf=IIC_Read_Byte(0);//读数据,发送nACK 
		else *buf=IIC_Read_Byte(1);		//读数据,发送ACK  
		len--;
		buf++; 
	}    
    IIC_Stop();	//产生一个停止条件 
	return 0;	
}

//在AT24C02指定地址写入一个数据
//WriteAddr  :写入数据的目的地址
//DataToWrite:要写入的数据

void HMC5883L_WriteOneByte(u16 WriteAddr, u8 DataToWrite)
{
    IIC_Start();
    IIC_Send_Byte(HMC5883_ADDR); //发送写命令
    IIC_Wait_Ack();
    IIC_Send_Byte(WriteAddr); //发送高地址
    IIC_Wait_Ack();
    IIC_Send_Byte(DataToWrite); //发送字节
    IIC_Wait_Ack();
    IIC_Stop(); //产生一个停止条件
}

void HMC5883L_Read_XYZ(void)
{
	  u8 Buf[6];
	  u16 x,y,z;
	  int x1,y1,z1;
	
    /* 更新寄存器采用新数据 */
		HMC5883L_WriteOneByte(HMC5883_REG_CRA,HMC5883_REG_CRA_MS_NORAL|HMC5883_REG_CRA_DO_Hz_15|HMC5883_REG_CRA_MA_AVG_8);
		HMC5883L_WriteOneByte(HMC5883_REG_CRB,HMC5883_REG_GN_1_30_G1090);
		HMC5883L_WriteOneByte(HMC5883_REG_MOD,HMC5883_REG_MODE_MR_SIG);
//	  Delay_ms(10);
    HMC5883L_ReadNByte(HMC5883_REG_X_MSB,Buf,6);
 
		x = Buf[0];
		x <<= 8;
		x |= Buf[1];

		z = Buf[2];
		z <<= 8;
		z |= Buf[3];

		y = Buf[4];
		y <<= 8;
		y |= Buf[5];
		
	 if(x>32768)
		{
			 x1 = -(0xFFFF - x + 1);
			
		}
		else
		{
		  x1 =x;
		}
		// printf("x1=%d  \n",x1);
		if(y>32768)
		{
			 y1 = -(0xFFFF - y + 1);
			
		}
		else
		{
		  y1 =y;
	}
 // printf("y1=%d  \n",y1);
		Gaussian_XYZ.x = x1/1090.0;//获取高斯增益
		Gaussian_XYZ.y = y1/1090.0;
}

