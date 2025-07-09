#include "bsp_IOT_BC20.h"

#define QLENGHT 20
#define ITESIZE 60

///////////////��������Ҫ�޸ķ��͵�������������ƽ̨����Ҫ��////////////////////////////////////
#define PRODUCEKEY "k0o8tTtSE5f"        //�޸Ĳ�Ʒ��Կ
#define DEVICENAME "NB-IOT"           //�޸��豸����
#define DEVICESECRET "18de1e9187462a14cce0883a06965131" //�޸��豸��Կ

#define EMQX_PRODUCEKEY "NB-IOT"        //�޸Ĳ�Ʒ��Կ
#define EMQX_DEVICENAME "LJX"           //�޸��豸����
#define EMQX_DEVICESECRET "ljx18617738931" //�޸��豸��Կ

extern volatile u8 Sensors_Check;
extern volatile u8 Compass_Check_t;
SemaphoreHandle_t Iot_Pubsh_Semaphore;//��ֵ�ź���
QueueHandle_t Recv_Mail_Queue =NULL;

//CJ02:A,24.639468,N,118.068542,E
//CJ02:A,24.623709,N,118.087703,E
//CJ02:A,24.633964,N,118.088465,E

link_platfrom_t  current_link_platfrom;
u8 *strx,*extstrx;
u8 *route_strx,*extstrx;
u8 atstr[RX_BUF_MAX_LEN];
u8 send_json[RX_BUF_MAX_LEN]; //�齨�����ݵ�JSON
u8 Gps_GNRMC[]="+QGNSSRD: $GNRMC,091900.00,A,2438.1907,N,11805.0172,E,0.189,,201022,,,A,V*1A";
extern volatile u8 IWDG_REST_FLAG;

char sin_cqr[10];
u8 ship_removeMarkers=pdFAIL;
u8 ship_start=pdFAIL;
u8 ship_removeallmarkers=pdFAIL;

uint16_t gps_flag;
u8 base_cnt=0;
struct STRUCT_ESP_DATA_BUFFR strIOT_Fram_Record = { 0 };
struct STRUCT_ESP_DATA_BUFFR strUSART_Fram_Record = { 0 };

typedef struct gps_data
{
   u8 Latitude[10];
   u8 Longitude[10]; 
   double gpslat;
	 double gpslon;
	
}GPS_Coordin;
GPS_Coordin Gps_Coordin;

u8 Mqttaliyun_Savedata(char *t_payload,char *latstr,char *lonstr,u8 battery,double PH,u8 temp,double turbidity,uint16_t TDS,u8 temperature_DHT11,u8 humidity_DHT11,double Pressure,double H,u8 light);
void Get_GPS_CJ02(u8 *Gps_String);
double wgs84_to_gcj02(double *lng, double *lat);
u8 *sub_str(u8 *str, u8 start, u8 end);
int str_cmp(char *str1, char *str2);
void del_last_char(u8 *str);
u8 find_string(u8 *str, u8 *substr);
void find_and_copy(char *dest, char *src, char *target);
void link_platfrom_f(link_platfrom_t staus);
void removeLastN(char *str, int n);
void Iot_Send_AT(u8*Cmd)
{
  Iot_Usart("%s\r\n",Cmd);
}

void Clear_Buffer(void)//��մ���2����
{
    memset(strIOT_Fram_Record.RX_DATA_BUF,0,RX_BUF_MAX_LEN);
}
//IOTģ���ʼ������
//xSemaphoreTake( Iot_Bin_Semaphore, portMAX_DELAY );�ȴ�IOT_Usart_Task�����ͷ��ź���
void Iot_Init_Task(void)
{
	while(1)
	{
	  vTaskDelay(3000);
		Clear_Buffer();	
	 	printf("�鿴ģ���Ƿ���\n");
		Iot_Send_AT(AT_CPIN);	 
		do
		{
			strx=strstr((const u8*)strIOT_Fram_Record.RX_DATA_BUF,(const u8*)"ERROR");//����OK
			if(str_cmp(strx,"ERROR"))
			{
	    	Iot_Send_AT(AT_OFF_POWER);	
			}
			
		}while(strx!=NULL);
    	printf("OK\n");		 
		
    vTaskDelay(2000);
		
//		printf("�鿴����\n");
//		Clear_Buffer();	
//		Iot_Send_AT(AT_CIMI);
//		xSemaphoreTake( Iot_Bin_Semaphore, portMAX_DELAY );
//		xSemaphoreTake( Iot_Bin_Semaphore, portMAX_DELAY );
//		strx=strstr((const u8*)strIOT_Fram_Record.RX_DATA_BUF,(const u8*)"ERROR");//����OK
    My_IWDG_ReloadCounter();
		
//		if(strx==NULL)
//		{
//				printf("�ҵĿ����� : %s \r\n",strIOT_Fram_Record.RX_DATA_BUF);
//				Clear_Buffer();	
//		}
//		else
//		{
//				printf("������ : %s \r\n",strIOT_Fram_Record.RX_DATA_BUF);
//				Clear_Buffer();
//		}
// 		vTaskDelay(100);
		
		printf("��ȡ�ź�����\n");
		Clear_Buffer();	
		Iot_Send_AT(AT_CSQ);
		xSemaphoreTake( Iot_Bin_Semaphore, portMAX_DELAY );
		xSemaphoreTake( Iot_Bin_Semaphore, portMAX_DELAY );
		//strx=strstr((const u8*)strIOT_Fram_Record.RX_DATA_BUF,(const u8*)"+CSQ");//����OK
		Usart_SendString(DEBUG_USART,strIOT_Fram_Record.RX_DATA_BUF);
//    printf("%s\n",strx);	
		vTaskDelay(100);
		
		printf("����GPS��Դ\n");
		Clear_Buffer();	
		Iot_Send_AT(AT_QGNSSC);
		xSemaphoreTake( Iot_Bin_Semaphore, portMAX_DELAY );
		xSemaphoreTake( Iot_Bin_Semaphore, portMAX_DELAY );
		Usart_SendString(DEBUG_USART,strIOT_Fram_Record.RX_DATA_BUF);
		
		printf("���� GNSSAGPS\n");
		Clear_Buffer();	
		Iot_Send_AT(AT_QGNSSAGPS);
		xSemaphoreTake( Iot_Bin_Semaphore, portMAX_DELAY );
		xSemaphoreTake( Iot_Bin_Semaphore, portMAX_DELAY );
		Usart_SendString(DEBUG_USART,strIOT_Fram_Record.RX_DATA_BUF);
		
		printf("�鿴GPSģ���Ƿ���\n");
		for(u8 i=0;i<20;i++)
		{
			Clear_Buffer();	
			Iot_Send_AT(AT_QGNSSCX);		
			xSemaphoreTake( Iot_Bin_Semaphore, portMAX_DELAY );
			vTaskDelay(300);
		}
		
	  My_IWDG_ReloadCounter();
		
		taskENTER_CRITICAL(); //�����ٽ��� 
		//�������Ӱ���������
		xTaskCreate(Iot_Connet_ALIYUN_Task,"Iot_Connet_ALIYUN_Task",IOT_CONNECT_STACK_SIZE,NULL,IOT_CONNECT_PRIORITY,&Iot_Connect_Task_Handle);
		if(Iot_Connect_Task_Handle==NULL)
		{
//			printf("Iot_Connect_Task_Handle����ʧ��");
		}
		else
		{
//			printf("Iot_Connect_Task_Handle�����ɹ�\n");
		}

		taskEXIT_CRITICAL(); //�˳��ٽ���

		vTaskDelete(Iot_Init_Task_Handle);		
	}


 }
/*
	 Debug_ship_Idel ����
   Debug_ship_forward app�ֶ�����
   Debug_ship_rotate ���н���
   Debug_ship_Gps ���õ�λ
   Debug_ship_run ��������
   Debug_ship_check_water ���ˮ��
   Debug_ship_error_stop ����ֹͣ
   Debug_ship_back ��������
 
*/
//����ƽ̨
void Iot_Connet_ALIYUN_Task(void)
{
	while(1)
	{
		  link_platfrom_f(emqx);
			Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",success link#\r\n");
			Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_Idel#\r\n");
			PWM1_MCU_ON;
			PWM2_MCU_ON;
		  taskENTER_CRITICAL(); //�����ٽ��� 
		  
		  //�������ڷ���������ź���
		  Iot_Pubsh_Semaphore=xSemaphoreCreateBinary();
			//��������Iot_Recv_Task�Ķ���
			Recv_Mail_Queue=xQueueCreate( (UBaseType_t )QLENGHT, (UBaseType_t )ITESIZE );
			if(Recv_Mail_Queue!=NULL)
			{
//				Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",��Ϣ���д����ɹ�\r\n");
//				printf("��Ϣ���д����ɹ�\n");
			}else
			{
//				Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",��Ϣ���д���ʧ��\r\n");
//				printf("��Ϣ���д���ʧ��\n");
			}	
			
		  //�������д�������
			xTaskCreate(Route_Task,"Route_Task",ROUTE_STACK_SIZE,NULL,ROUTE_PRIORITY,&Route_Task_Handle);
			if(Route_Task_Handle==NULL)
			{
		//		Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Route_Task no\r\n");
//				printf("Route_Task����ʧ��");
			}
			else
			{
		//	Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Route_Task yes\r\n");
//				printf("Route_Task�����ɹ�");
			}
			Ship_Status=statusX;
			
		  //�������������մ�������
			xTaskCreate(Sensors_Processing_Task,"Sensors_Processing_Task",IOT_PROCESS_STACK_SIZE,NULL,IOT_PROCESS_PRIORITY,&Sensors_Processing_Task_Handle);
			if(Iot_Connect_Task_Handle==NULL)
			{
			//	Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Sensors_Processing_Task no\r\n");
//				printf("Sensors_Processing_Task����ʧ��");
			}
			else
			{
			//	Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Sensors_Processing_Task yes\r\n");
//				printf("Sensors_Processing_Task�����ɹ�\n");
			}

			
			//����������Ϣ����
			xTaskCreate(Iot_Pubsh_Task,"Iot_Pubsh_Task",IOT_PUBSH_STACK_SIZE,NULL,IOT_PUBSH_PRIORITY,&Iot_Pubsh_Task_Handle);
			if(Iot_Connect_Task_Handle==NULL)
			{
			//	Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Iot_Pubsh_Task no\r\n");
//				printf("Iot_Pubsh_Task����ʧ��");
			}
			else
			{
		//		Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Iot_Pubsh_Task yes\r\n");
//				printf("Iot_Pubsh_Task�����ɹ�\n");
			}

			
			//�������ݽ�������
			xTaskCreate(Iot_Recv_Task,"Iot_Recv_Task",IOT_RECV_STACK_SIZE,NULL,IOT_RECV_PRIORITY,&Iot_Recv_Task_Handle);
			if(Iot_Connect_Task_Handle==NULL)
			{
	//			Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Iot_Recv_Task no\r\n");
//				printf("Iot_Recv_Task����ʧ��");
			}
			else
			{
	//			Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Iot_Recv_Task yes\r\n");
//				printf("Iot_Recv_Task�����ɹ�\n");
			}
			//����GPS��������
			xTaskCreate(Gps_Task,"Gps_Task",IOT_PUBSH_STACK_SIZE,NULL,IOT_PUBSH_PRIORITY,&Gps_Task_Handle);
			if(Gps_Task_Handle==NULL)
			{
	//			Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Gps_Task no\r\n");
//				printf("Gps_Task����ʧ��");
			}
			else
			{
		//		Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Gps_Task yes\r\n");
//				printf("Gps_Task�����ɹ�\n");
			}
			
			xTaskCreate(Gps_Send_Task,"Gps_Send_Task",IOT_PUBSH_STACK_SIZE,NULL,IOT_PUBSH_PRIORITY,&Gps_Send_Task_Handle);
			if(Gps_Task_Handle==NULL)
			{
	//			printf("Gps_Task����ʧ��");
			}
			else
			{
	//			printf("Gps_Task�����ɹ�\n");
			}

			vTaskSuspend(Iot_Pubsh_Task_Handle);
			vTaskSuspend(Sensors_Processing_Task_Handle);
			taskEXIT_CRITICAL(); //�˳��ٽ���

			//	vTaskSuspend(Gps_Task_Handle);
     // vTaskSuspend(Route_Task_Handle);
    // JXCT_TDS_Init();

			
		  vTaskDelay(100);
	}

	
}


 
//���������մ�������
void Sensors_Processing_Task(void)
{
		  u8 Gps_Data[40];
	while(1)
	{
		printf("��������\n");
		//�齨JSON����
		Sensores_Data_Proessing();
		Mqttaliyun_Savedata((u8*)send_json,Sensors_Msg.latstr,Sensors_Msg.lonstr,Sensors_Msg.battery,Sensors_Msg.PH,Sensors_Msg.temp,Sensors_Msg.turbidity,Sensors_Msg.tds,Sensors_Msg.temperature_DHT11,Sensors_Msg.humidity_DHT11,Sensors_Msg.Pressure,Sensors_Msg.H,Sensors_Msg.light);
// 		printf("send_json = %s \r\n",send_json);
		xSemaphoreGive(Iot_Pubsh_Semaphore);//�ͷ��ź�����Iot_Pubsh_Task
		vTaskDelay(1000);
		
	}

}

//������Ϣ
void Iot_Pubsh_Task(void)
{
	static u8 Send_Cnt=1;
	u8 w_t=0;
	 while(1)
	 {

		  //�ȴ�Sensors_Processing_Task�ͷ�
		  xSemaphoreTake( Iot_Pubsh_Semaphore, portMAX_DELAY );
		  //��������
			memset(atstr,0,RX_BUF_MAX_LEN);
			sprintf(atstr,"AT+QMTPUB=0,0,0,0,\"public\",%s\r\n",send_json);
			printf("atstr = %s \r\n",atstr);
			Iot_Send_AT(atstr);
		  Clear_Buffer();
		 
			if(Send_Cnt>=2)
			{
					taskENTER_CRITICAL(); //�����ٽ��� 
					vTaskSuspend(Sensors_Processing_Task_Handle);
					taskEXIT_CRITICAL(); //�˳��ٽ���	
				
				  water_ctrl_off_pumping();
      	  water_ctrl_release();
					while(1)
					{
						 if(Ship_Status==stop)
						 {
							 water_ctrl_OFF();
							 break;
						 }
						if((w_t%6==0))
						{
						Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_release#\r\n");					
						}
						w_t++;		
            if(Ship_Status==forward)
						{
							if((w_t>=23))//(Get_EmqxWater_value()<=1500)||
							{
					      Ship_Status=statusX;
								w_t=0;
								water_ctrl_OFF();
								break;
							}							
						}
						else
						{
							if((w_t>=5))//(Get_EmqxWater_value()<=1500)||
							{
								w_t=0;
								break;
							}	
						}
		
						vTaskDelay(1000);
					}

			    Send_Cnt=0;
					taskENTER_CRITICAL(); //�����ٽ��� 
					vTaskResume(Route_Task_Handle);
					taskEXIT_CRITICAL(); //�˳��ٽ���			
				  Sensors_Check=0;
					taskENTER_CRITICAL(); //�����ٽ��� 
					vTaskSuspend(Iot_Pubsh_Task_Handle);
					taskEXIT_CRITICAL(); //�˳��ٽ���	
			   vTaskDelay(10);					

			}	


			Send_Cnt++;
	 }
}




//������Ϣ
void Iot_Recv_Task(void)
{
	uint16_t i;
	vTaskSuspend(Iot_Connect_Task_Handle);
	u8* search = "+QGNSSRD: $GNRMC";
	u8* search_cj02 = "\"CJ02:";
	u8  result[80];
 
	
	int len;
	
	 while(1)
	 {
     xSemaphoreTake( Iot_Bin_Semaphore, portMAX_DELAY );
//	   printf(" yes \n");
		 memset(result,0,80);
		 strx=strstr(strIOT_Fram_Record.RX_ISO_BUF,"$GNRMC");//�鿴�Ƿ�ΪGPS����

			 if(strx!=NULL)
			 {    
				 find_and_copy(result,strIOT_Fram_Record.RX_ISO_BUF,"$GNRMC");
				 if(find_string(result,"E"))
				{
					gps_flag++;
				 //��GPS���ݵ�+QGNSSRD: $GNRM��������ݷ��ͳ�ȥ
					for (i = 0; i < strlen(strIOT_Fram_Record.RX_ISO_BUF); i++) 
				 {
							if (strncmp(strIOT_Fram_Record.RX_ISO_BUF + i, search, strlen(search)) == 0) 
							{
									strcpy(result, strIOT_Fram_Record.RX_ISO_BUF + i + strlen(search));//bytes + i + strlen(search)=bytes[]=','����ʼ��ַ������������ַ������Ƶ�result
									break;
							}
					}
						xQueueSend( Recv_Mail_Queue, result, portMAX_DELAY );
				}
				else 
				{
					  gps_flag=0;
				 	  xQueueSend( Recv_Mail_Queue, Gps_GNRMC, portMAX_DELAY );
				}
					 
			 }
			 else
			 {
						 strx=strstr(strIOT_Fram_Record.RX_ISO_BUF,"\"CJ02:");//�鿴�Ƿ�ΪGPS����;
						 sub_str(strx, 'C', 'E');
						 strx[0]='C';
						 if(strx!=NULL)
						 {    
							xQueueSend( Recv_Mail_Queue, strx, portMAX_DELAY );
						 }
			 }
		

		 Clear_Buffer();
		 vTaskDelay(10);//��Ҫ
	 }

}


void IOT_Usart_Task(void)
{
	 u8  result[80];
 while(1)
 {
	 if(strIOT_Fram_Record.InfBit.FramFinishFlag==1)
	 {
      
		 //�������������Ϊ�ַ���
		 strIOT_Fram_Record.RX_DATA_BUF[strIOT_Fram_Record.InfBit.FramLength]='\0';
		 //��ӡ��Ӧ��� 
     //Usart_SendString(DEBUG_USART,strIOT_Fram_Record.RX_DATA_BUF);
		  memset(strIOT_Fram_Record.RX_ISO_BUF,0,RX_BUF_MAX_LEN);
		  strcpy(strIOT_Fram_Record.RX_ISO_BUF,strIOT_Fram_Record.RX_DATA_BUF);
  // Usart_SendString(DEBUG_USART,strIOT_Fram_Record.RX_ISO_BUF);
		 
		 if(find_string(strIOT_Fram_Record.RX_ISO_BUF,"+QMTRECV"))
		 {
		   route_strx=strstr(strIOT_Fram_Record.RX_ISO_BUF,"thing.service.run");//�鿴�Ƿ�Ϊ�����ź�
			 if(route_strx!=NULL)
			 {
						find_and_copy(result,strIOT_Fram_Record.RX_ISO_BUF,"params");
					 
						if(find_string(result,"ship_forward"))
						{
							printf("ship_forward");
							Ship_Status=forward;
						}
						else if(find_string(result,"ship_stop"))
						{
        			water_ctrl_OFF();
							printf("ship_stop");
							set_right_speed(0);
							set_left_speed(0);	
							Ship_Status=stop;
						}
						else if(find_string(result,"ship_rotate"))
						{
							printf("ship_rotate");
							Ship_Status=rotate;
						}
						else if(find_string(result,"ship_GPS"))
						{
							printf("ship_GPS");
							Ship_Status=status_Gps;
						}
						else if(find_string(result,"ship_ctrl_mode:0"))
						{
							printf("ship_js");
							Ship_Status=pwm_js;
							PWM1_JS_ON;
							PWM2_JS_ON;
						}		
						else if(find_string(result,"ship_ctrl_mode:1"))
						{
							printf("ship_MCU");
							Ship_Status=mcu_js;
							PWM1_MCU_ON;
							PWM2_MCU_ON;
						}		
						else if(find_string(result,"ship_js"))
						{
							printf("ship_js");
							Ship_Status=pwm_js;
							PWM1_JS_ON;
							PWM2_JS_ON;
						}
						else
						{
						 printf("error cmd\n");
						}
						
						if(find_string(result,"ship_start"))
						{
							ship_start=pdTRUE;
							printf("ship_start\n");
						}
						else if(find_string(result,"removeMarkers"))
						{
							ship_removeMarkers=pdTRUE;

							printf("ship_start\n");
						}
						else if(find_string(result,"removeallmarkers"))
						{
							ship_removeallmarkers=pdTRUE;						
						}	
						
						if(find_string(result,"beats"))
						{
							base_cnt++;
							printf("beats\n");
						}	
						
						if(find_string(result,"restart"))
						{
        			water_ctrl_OFF();
							printf("ship_restart");
							set_right_speed(0);
							set_left_speed(0);	
							IWDG_REST_FLAG=1;
						}	
						
				}
		}else if(find_string(strIOT_Fram_Record.RX_ISO_BUF,"+CSQ"))
		 {
			 find_and_copy(sin_cqr,strIOT_Fram_Record.RX_ISO_BUF,"+CSQ:");
			 removeLastN(sin_cqr,4);			 
		    printf("%s\n",sin_cqr);
		 }
		 //�����Ӧ��־λ
		 strIOT_Fram_Record.InfBit.FramFinishFlag=0;	 
		 strIOT_Fram_Record.InfBit.FramLength=0;
     xSemaphoreGive(Iot_Bin_Semaphore);
		 
	 }
	
 }
}

void link_platfrom_f(link_platfrom_t staus)
{
  current_link_platfrom=staus;
	if(current_link_platfrom==emqx)
	{
		 current_link_platfrom=X_link;
//		  //�Ͽ�MQTT
			Iot_Send_AT(AT_QMTDISC);
			Clear_Buffer();
		  vTaskDelay(100);
		
//			Iot_Send_AT(AT_QMTCLOSE);
//			Clear_Buffer();  
//		  vTaskDelay(100);
		
//			Iot_Send_AT(AT_KEEPALIVE);
//			do
//			{			
//			 strx=strstr((const u8*)strIOT_Fram_Record.RX_DATA_BUF,(const u8*)"OK");//����OK
//			}while(strx==NULL);
//			printf("AT_KEEPALIVE %s\n",strx);
//		  vTaskDelay(50);
//			Clear_Buffer();  
			
		//����EMQX

			Clear_Buffer();	
			Iot_Send_AT(AT_EMQX_QMTOPEN);
			xSemaphoreTake( Iot_Bin_Semaphore, portMAX_DELAY );
			do
			{			
//				vTaskDelay(1000);
//				Iot_Send_AT(AT_EMQX_QMTOPEN);
//			  xSemaphoreTake( Iot_Bin_Semaphore, portMAX_DELAY );
		  	strx=strstr((const u8*)strIOT_Fram_Record.RX_DATA_BUF,(const u8*)"+QMTOPEN:");//����OK
			}while(strx==NULL);
			
			printf(" %s OK\n",strx);
		  vTaskDelay(100);
	   	My_IWDG_ReloadCounter();

		  //����EMQX
			Clear_Buffer();	
			memset(atstr,0,RX_BUF_MAX_LEN);
			sprintf(atstr,"AT+QMTCONN=0,\"%s\",\"%s\",\"%s\"\r\n",EMQX_PRODUCEKEY,EMQX_DEVICENAME,EMQX_DEVICESECRET);
			Iot_Send_AT(atstr);
			xSemaphoreTake( Iot_Bin_Semaphore, portMAX_DELAY );
		  vTaskDelay(100);
			do
			{
				strx=strstr((const u8*)strIOT_Fram_Record.RX_DATA_BUF,(const u8*)"+QMTCONN:");//����OK
			}while(strx==NULL);
			printf(" %s OK\n",strx);
		  vTaskDelay(100);
	  	My_IWDG_ReloadCounter();
		  //��������
			Clear_Buffer();	
			Iot_Send_AT(AT_EMQX_QMTSUB);
			xSemaphoreTake( Iot_Bin_Semaphore, portMAX_DELAY );
			do
			{
				strx=strstr((const u8*)strIOT_Fram_Record.RX_DATA_BUF,(const u8*)"+QMTSUB:");//����OK
			}while(strx==NULL);
			printf("%s OK\n",strx);
		  vTaskDelay(100);
	}
	 My_IWDG_ReloadCounter();
   current_link_platfrom=staus;
}
/*д��ȡ�ַ�����һ���ַ�����һ���ַ��ĺ���*/
u8 *sub_str(u8 *str, u8 start, u8 end)
{
    int i = 0;
    int len = strlen(str);
    int start_flag = 0;
    int end_flag = 0;
    for(i = 0; i < len; i++)
    {
        if(str[i] == start)
        {
            start_flag = 1;
        }
        if(str[i] == end)
        {
            end_flag = 1;
        }
        if(start_flag == 1 && end_flag == 0)
        {
            str[i] = str[i + 1];
        }
        if(start_flag == 1 && end_flag == 1)
        {
            str[i] = '\0';
        }
    }
    return str;
}

int str_cmp(char *str1, char *str2)
{
    int i = 0;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if(len1 != len2)
    {
        return 0;
    }
    for(i = 0; i < len1; i++)
    {
        if(str1[i] != str2[i])
        {
            return 0;
        }
    }
    return 1;
}

u8 find_string(u8 *str, u8 *substr) 
{
    int len1 = strlen(str);    
    int len2 = strlen(substr);    
    
    for (int i = 0; i <= len1 - len2; i++) {   
        if (strncmp(&str[i], substr, len2) == 0) {    
            return 1;    
        }
    }
    
    return 0;    
}
//���ַ�����Ѱ��ָ���ַ��������ƺ����ַ����ĺ���
void find_and_copy(char *dest, char *src, char *target) 
{
    char *ptr = strstr(src, target); // ����Ŀ���ַ���

    if (ptr != NULL) { // ����ҵ���Ŀ���ַ���
        strcpy(dest, ptr + strlen(target)); // ����Ŀ���ַ�������Ĳ��ֵ�Ŀ�껺����
    } else { // ���û���ҵ�Ŀ���ַ���
        strcpy(dest, ""); // Ŀ�껺����Ϊ���ַ���
    }
}

void del_last_char(u8 *str)
{
    int len = strlen(str);
    str[len - 3] = '\0';
}


void removeLastN(char *str, int n) 
	{
    int len = strlen(str);
    
    if (n >= len) {
        str[0] = '\0';  // ����ַ���
    } else {
        str[len - n] = '\0';  // ���� len-n λ֮����ַ��滻Ϊ�ַ���������
    }
}

//////////////////////////////////////////////////
//�ύ����
/////////////////////////////////////////////////
u8 Mqttaliyun_Savedata(char *t_payload,char *latstr,char *lonstr,u8 battery,double PH,u8 temp,double turbidity,uint16_t TDS,u8 temperature_DHT11,u8 humidity_DHT11,double Pressure,double H,u8 light)
{

	  //����json���ܳ���
    int json_len;
	  //���͵�json��ʽ
    u8 json[]= "\"thing.service.run:ship_NTU:%lf;thing.service.run:ship_TDS:%d;thing.service.run:ship_temp:%d;thing.service.run:ship_PH:%lf;thing.service.run:ship_battery:%d%%thing.service.run:ship_locatin:%s,N,%s,thing.service.run:tempe_DHT11:%d;thing.service.run:hum_DHT11:%d;thing.service.run:Pressure:%lf;thing.service.run:H:%lf;thing.service.run:light:%d;\"";	 
		memset(send_json,0,RX_BUF_MAX_LEN);
    sprintf(send_json, json,turbidity,TDS,temp,PH,battery,latstr,lonstr,temperature_DHT11,humidity_DHT11,Pressure,H,light); //�齨��Ҫ���͵�json ����
    json_len = strlen(send_json)/sizeof(u8);
  	memcpy(t_payload, send_json, json_len);
    return json_len;
}


//PGRMC->WGS->CJ02
void Get_GPS_CJ02(u8 *Gps_String)
{
		int i=0;
    strx=strstr((const u8*)Gps_String,(const u8*)"A,");//��ȡγ�ȵ�λ��
       if(strx)
        {
            for(i=0;i<9;i++)
            {
             Gps_Coordin.Latitude[i]=strx[i+2];//��ȡγ��ֵ2603.9576
            }
						strx=strstr((const u8*)Gps_String,(const u8*)"N,");//��ȡ����ֵ
						if(strx)
						{
								 for(i=0;i<10;i++)	//��ȡ���� 11912.4098
								 {
										Gps_Coordin.Longitude[i]=strx[i+2];
								 }
								 
						}  
						
					// printf("Gps_Coordin.Latitude ,%s \r\n",Gps_Coordin.Latitude);
					// printf("Gps_Coordin.longitude ,%s \r\n",Gps_Coordin.Longitude);  
						
					 //�ַ���ת������
					 Gps_Coordin.gpslon=(Gps_Coordin.Longitude[0]-0x30)*10000+(Gps_Coordin.Longitude[1]-0x30)*1000+(Gps_Coordin.Longitude[2]-0x30)*100\
					+((Gps_Coordin.Longitude[3]-0x30)*10+(Gps_Coordin.Longitude[4]-0x30)+(double)(Gps_Coordin.Longitude[6]-0x30)/10.0+\
					(double)(Gps_Coordin.Longitude[7]-0x30)/100.0+(double)(Gps_Coordin.Longitude[8]-0x30)/1000.0+(double)(Gps_Coordin.Longitude[9]-0x30)/10000.0);//��ȡ����������

					 Gps_Coordin.gpslat=(Gps_Coordin.Latitude[0]-0x30)*1000+(Gps_Coordin.Latitude[1]-0x30)*100+(Gps_Coordin.Latitude[2]-0x30)*10.0\
					+((Gps_Coordin.Latitude[3]-0x30)+(double)(Gps_Coordin.Latitude[5]-0x30)/10.0+(double)(Gps_Coordin.Latitude[6]-0x30)/100.0+\
					(double)(Gps_Coordin.Latitude[7]-0x30)/1000.0+(double)(Gps_Coordin.Latitude[8]-0x30)/10000.0);//��ȡ����������

					Gps_Coordin.gpslon = (int)(Gps_Coordin.gpslon/100) + (Gps_Coordin.gpslon/100.0 - (int)(Gps_Coordin.gpslon/100)) *100.0 / 60.0;
					Gps_Coordin.gpslat = (int)(Gps_Coordin.gpslat/100) + (Gps_Coordin.gpslat/100.0 - (int)(Gps_Coordin.gpslat/100)) *100.0 / 60.0;

					wgs84_to_gcj02(&Gps_Coordin.gpslon,&Gps_Coordin.gpslat);
			 	
					memset(Gps_Coordin.Longitude,0,sizeof(Gps_Coordin.Longitude));
					memset(Gps_Coordin.Latitude,0,sizeof(Gps_Coordin.Latitude));

					sprintf(Gps_Coordin.Longitude,"%.6lf",Gps_Coordin.gpslon);//����ת�ַ���
					sprintf(Gps_Coordin.Latitude,"%.6lf",Gps_Coordin.gpslat);

					Sensors_Msg.latstr=Gps_Coordin.Latitude;
					Sensors_Msg.lonstr=Gps_Coordin.Longitude;
				 
	    }
                            
			Clear_Buffer();
}


double pi = 3.1415926535897932384626;
double ee = 0.00669342162296594323;//����������˹������ϵ��
double aa = 6378245.0;

 /*
"""
  * �ж��Ƿ��ڹ��ڣ����ڹ�������ƫ��
  * @param lng
  * @param lat
  * @returns {boolean}
"""
 */

int out_of_china(double lng, double lat)
{
    // γ��3.86~53.55,����73.66~135.05
    if ((73.66 < lng < 135.05) && (3.86 < lat < 53.55))
    {
        return 0;
    }
        return 1;
}



 /*
  * ��γ��ƫ��ת��
 */
double transform_lat(double lng, double lat)
{

    double ret;
    ret = -100.0 + 2.0 * lng + 3.0 * lat + 0.2 * lat * lat + 0.1 * lng * lat + 0.2 *  sqrt( fabs(lng));
    ret += (20.0 *  sin(6.0 * lng * pi) + 20.0 *  sin(2.0 * lng *   pi)) * 2.0 / 3.0;
    ret += (20.0 *  sin(lat *   pi) + 40.0 *  sin(lat / 3.0 *   pi)) * 2.0 / 3.0;
    ret += (160.0 *  sin(lat / 12.0 *   pi) + 320 *  sin(lat *   pi / 30.0)) * 2.0 / 3.0;

    return ret;    
}



double transform_lng(double lng, double lat)
{
    double ret;
    ret = 300.0 + lng + 2.0 * lat + 0.1 * lng * lng + 0.1 * lng * lat + 0.1 *  sqrt( fabs(lng));
    ret += (20.0 *  sin(6.0 * lng *   pi) + 20.0 *  sin(2.0 * lng *   pi)) * 2.0 / 3.0;
    ret += (20.0 *  sin(lng *   pi) + 40.0 *  sin(lng / 3.0 *   pi)) * 2.0 / 3.0;
    ret += (150.0 *  sin(lng / 12.0 *   pi) + 300.0 *  sin(lng / 30.0 *   pi)) * 2.0 / 3.0;
    return ret;

}

/*
"""
  * WGS84תGCj02
  * @param lng
  *  @param lat
  *  @returns {*[]}
"""
*/

double wgs84_to_gcj02(double *lng, double *lat)
{
    if (out_of_china(*lng, *lat))
    {
        return 0;
    }
    else
    {
        double dlat = transform_lat(*lng - 105.0, *lat - 35.0);
        double dlng = transform_lng(*lng - 105.0, *lat - 35.0);
        double radlat = *lat / 180.0 *  pi;
        double magic =  sin(radlat);
        magic = 1 - ee * magic * magic;
        double sqrtmagic =  sqrt(magic);
        dlat = (dlat * 180.0) / ((aa * (1 - ee)) / (magic * sqrtmagic) *  pi);
        dlng = (dlng * 180.0) / (aa / sqrtmagic *  cos(radlat) *  pi);
        *lat = *lat + dlat;
        *lng = *lng + dlng;

    }
 
        return 1;   
}

double gcj02_to_wgs84(double *lng, double *lat)
{
 
    if (out_of_china(*lng, *lat))
    {
        return 0;
    }
    else
    {
        double dlat = transform_lat(*lng - 105.0, *lat - 35.0);
        double dlng = transform_lng(*lng - 105.0, *lat - 35.0);
        double radlat = *lat / 180.0 * pi;
        double magic = sin(radlat);
        magic = 1 - ee * magic * magic;
        double sqrtmagic = sqrt(magic);
        dlat = (dlat * 180.0) / ((aa * (1 - ee)) / (magic * sqrtmagic) * pi);
        dlng = (dlng * 180.0) / (aa / sqrtmagic *  cos(radlat) * pi);
        double mglat = *lat + dlat;
        double mglng = *lng + dlng;
        *lng=*lng * 2 - mglng;
        *lat=*lat * 2 - mglat;

    }

    return 0;
}
