#ifndef __BSP_COMPASS_H
#define __BSP_COMPASS_H
#include "stm32f4xx.h"

typedef struct compass
{
 double x;
 double y;
 double z;
 double Curent_Angle;
	
}COMPASS;

typedef struct correction_parameter
{
 double Xoffest;
 double Yoffest;
 double Kx;
 double Ky;
	
}Correction_Parameter;

extern Correction_Parameter Gaussian_Parameter;
extern COMPASS Gaussian_XYZ;

#define HMC5883_REG_X_MSB 0x03 //���X�Ĵ���A�д洢��������е�MSB����λ���ݣ�
#define HMC5883_REG_X_LSB 0x04 //���X�Ĵ���B�д洢��������е�MSB����λ���ݣ�
#define HMC5883_REG_Z_MSB 0x05 //���Y�Ĵ���A�д洢��������е�MSB����λ���ݣ�
#define HMC5883_REG_Z_LSB 0x06 //���Y�Ĵ���B�д洢��������е�MSB����λ���ݣ�
#define HMC5883_REG_Y_MSB 0x07 //���Z�Ĵ���A�д洢��������е�MSB����λ���ݣ� 
#define HMC5883_REG_Y_LSB 0x08 //���Z�Ĵ���B�д洢��������е�MSB����λ���ݣ� 
 
/* CRA */
#define HMC5883_REG_CRA              0x00

#define HMC5883_REG_CRA_MS_MASK      0x03
#define HMC5883_REG_CRA_MS_NORAL     0x00 // default
#define HMC5883_REG_CRA_MS_XYZ_P     0x01
#define HMC5883_REG_CRA_MS_XYZ_N     0x02
#define HMC5883_REG_CRA_MS_REV       0x03
 
#define HMC5883_REG_CRA_DO_Hz_MASK   0x1C
#define HMC5883_REG_CRA_DO_Hz_0_75   0x00
#define HMC5883_REG_CRA_DO_Hz_1_5    0x04
#define HMC5883_REG_CRA_DO_Hz_3      0x08
#define HMC5883_REG_CRA_DO_Hz_7_5    0x0C
#define HMC5883_REG_CRA_DO_Hz_15     0x10 // default
#define HMC5883_REG_CRA_DO_Hz_30     0x14
#define HMC5883_REG_CRA_DO_Hz_75     0x18
#define HMC5883_REG_CRA_DO_Hz_NONE   0x1C
 
#define HMC5883_REG_CRA_MA_MASK      0x60
#define HMC5883_REG_CRA_MA_AVG_1     0x00
#define HMC5883_REG_CRA_MA_AVG_2     0x20
#define HMC5883_REG_CRA_MA_AVG_4     0x40
#define HMC5883_REG_CRA_MA_AVG_8     0x60 // default
 
#define HMC5883_REG_CRA_RUN_MASK     0x80
 
/* CRB */
#define HMC5883_REG_CRB              0x01

#define HMC5883_REG_GN_GA_MASK       0xE0
#define HMC5883_REG_GN_0_88_G1370    0x00
#define HMC5883_REG_GN_1_30_G1090    0x20 // default
#define HMC5883_REG_GN_1_90_G820     0x40
#define HMC5883_REG_GN_2_50_G660     0x60
#define HMC5883_REG_GN_4_00_G440     0x80
#define HMC5883_REG_GN_4_70_G390     0xA0
#define HMC5883_REG_GN_5_60_G330     0xC0
#define HMC5883_REG_GN_8_10_G230     0xE0
 
/* MODE */
#define HMC5883_REG_MOD              0x02

#define HMC5883_REG_MODE_MR_MASK     0x03    
#define HMC5883_REG_MODE_MR_CON      0x00    
#define HMC5883_REG_MODE_MR_SIG      0x01 // default   
#define HMC5883_REG_MODE_MR_IDLE1    0x02    
#define HMC5883_REG_MODE_MR_IDLE2    0x03    
 
/* read addr*/
#define HMC5883_ADDR                 0X3C
/*-------------------------------------------------------------
IIC��ַ��0X3C                   
���üĴ���A��0X00 ���������ø�װ�����õ�����������ʺͲ������ã�
���üĴ���B: 0X01 ����������װ�õ����棩
ģʽ�Ĵ����� 0X02 �������趨װ�õĲ���ģʽ��
---------------------------------------------------------------*/

 


u8 Compass_Init(void);
void HMC5883L_Read_XYZ(void);
#endif
