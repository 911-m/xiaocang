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
	  printf("������Ӧ��\n");
		return 0;
	}
	else
	{
	  printf("����Ӧ��\n");
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
    IIC_Send_Byte(HMC5883_ADDR); //����д����
    if(IIC_Wait_Ack())
		{
		  return 1;
		}	
		  return 0;
}

u8 HMC5883L_ReadNByte(u8 reg,u8 *buf,u8 len)
{ 
		IIC_Start(); 
	
		IIC_Send_Byte((HMC5883_ADDR));//����������ַ+д����	
	
    IIC_Wait_Ack();
		
		IIC_Send_Byte(reg);	//д�Ĵ�����ַ
    IIC_Wait_Ack();
	
		IIC_Start();
		IIC_Send_Byte((HMC5883_ADDR)|1);//����������ַ+������	
    IIC_Wait_Ack();


	
	while(len)
	{
		if(len==1)*buf=IIC_Read_Byte(0);//������,����nACK 
		else *buf=IIC_Read_Byte(1);		//������,����ACK  
		len--;
		buf++; 
	}    
    IIC_Stop();	//����һ��ֹͣ���� 
	return 0;	
}

//��AT24C02ָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ
//DataToWrite:Ҫд�������

void HMC5883L_WriteOneByte(u16 WriteAddr, u8 DataToWrite)
{
    IIC_Start();
    IIC_Send_Byte(HMC5883_ADDR); //����д����
    IIC_Wait_Ack();
    IIC_Send_Byte(WriteAddr); //���͸ߵ�ַ
    IIC_Wait_Ack();
    IIC_Send_Byte(DataToWrite); //�����ֽ�
    IIC_Wait_Ack();
    IIC_Stop(); //����һ��ֹͣ����
}

void HMC5883L_Read_XYZ(void)
{
	  u8 Buf[6];
	  u16 x,y,z;
	  int x1,y1,z1;
	
    /* ���¼Ĵ������������� */
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
		Gaussian_XYZ.x = x1/1090.0;//��ȡ��˹����
		Gaussian_XYZ.y = y1/1090.0;
}

