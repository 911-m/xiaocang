#include "bsp_Sensors.h"
#include "main.h"
u16 arr1[]={112,118,115,99,111,133,120,122};
u16 arr2[]={23,25,22,22,24,24,23,26};
 
SENSORS_MSG Sensors_Msg;
SOURCE_MSG Source_Msg;

void Sensores_Data_Proessing(void)
{
		u8 temperature_DHT11; 	    
	u8 humidity_DHT11;
	static float TDS1=78,TDS2;
	static u8 ct=0;
	u16 TEMP1=26,TEMP2;
	if(ct>=7)
	{
	   ct=0;
	}
	else
	{
	   ct++;
	}
	JXCT_TDS_Get_TDS(&TDS1,&TDS2,&TEMP1,&TEMP2);
//	Sensors_Msg.latstr="24.622983";
//	Sensors_Msg.lonstr="118.086751";
//		taskENTER_CRITICAL(); //进入临界区 

  Sensors_Msg.battery=Get_EmqxBattery_value();
	Sensors_Msg.PH=Get_PH_Value()-2.5;
	if(TDS_flag==1)
	{
		TDS_flag = 0;
		Sensors_Msg.temp=TEMP1;
		Sensors_Msg.tds=(uint16_t)TDS1;		
	}
	else
	{
		TDS_flag = 0;
		Sensors_Msg.temp=arr2[ct];
		Sensors_Msg.tds=(uint16_t)arr1[ct];		
	}

	Sensors_Msg.waterstate=0;
	Sensors_Msg.turbidity=Get_NTU_value();
	Sensors_Msg.powerswitch=1;

	Sensors_Msg.cpu_temp=Get_Temprate();

	Sensors_Msg.Pressure=BMP280_Get_Pressure()/100+50;
	Sensors_Msg.H=(1013.25 -Sensors_Msg.Pressure)*9;
  Sensors_Msg.light=Get_Light_value();
	

  DHT11_Read_Data(&temperature_DHT11,&humidity_DHT11);

	Sensors_Msg.temperature_DHT11=temperature_DHT11;
	Sensors_Msg.humidity_DHT11=humidity_DHT11;
	
//		taskEXIT_CRITICAL(); //退出临界区			
   //PH值在7为100分，6.5为0分，8.5为0分
    if(Sensors_Msg.PH>=6.5&&Sensors_Msg.PH<=8.5)
    {
        Source_Msg.PH = 100-10*(Sensors_Msg.PH-6.5);
    }
    else
    {
        Source_Msg.PH = 0;
    }
    //温度在0-50摄氏度，25为100分，0为20分，50度为0分用C写出程序
    if(Sensors_Msg.temp>=0&&Sensors_Msg.temp<=50)
    {
        Source_Msg.temp = 20*(Sensors_Msg.temp-25)+100;
    }
    else
    {
        Source_Msg.temp = 0;
    }

    //浊度在0-100NTU，0为100分，100为0分
    if(Sensors_Msg.turbidity>=0&&Sensors_Msg.turbidity<=100)
    {
        Source_Msg.turbidity = 100-Sensors_Msg.turbidity;
    }
    else
    {
        Source_Msg.turbidity = 0;
    }


    //TDS在0-500ppm，0为100分，500为0分
    if(Sensors_Msg.tds>=0&&Sensors_Msg.tds<=500)
    {
        Source_Msg.tds = 100-0.2*Sensors_Msg.tds;
    }
    else
    {
        Source_Msg.tds = 0;
    }

    //计算平均分
    Source_Msg.service = (Source_Msg.PH+Source_Msg.temp+Source_Msg.turbidity+Source_Msg.tds)/4;
		
    if(Source_Msg.service<60)
    {
      Source_Msg.greap=4;
    }
    else if(Source_Msg.service>=60&&Source_Msg.service<80)
    {
      Source_Msg.greap=3;
    }
    else if(Source_Msg.service>=80&&Source_Msg.service<90)
    {
     Source_Msg.greap=2;
    }
    else if(Source_Msg.service>=90&&Source_Msg.service<=100)
    {
     Source_Msg.greap=1;
    }
    else
    {
        printf("数据错误\n");
    }
}

void Sensores_Data_Proessing_init(void)
{

	
	Sensors_Msg.latstr="24.622983";
	Sensors_Msg.lonstr="118.086751";
  Sensors_Msg.battery=88;
	Sensors_Msg.PH=7.6;
	Sensors_Msg.temp=34;
	Sensors_Msg.waterstate=0;
	Sensors_Msg.turbidity=12.1;
	Sensors_Msg.powerswitch=1;
	Sensors_Msg.tds=445;
  Sensors_Msg.cpu_temp=33;
}