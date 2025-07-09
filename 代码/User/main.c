#include "main.h"
extern char List_Arr[6];
extern double GPS_arr[6][2];

TaskHandle_t Start_Handle;
TaskHandle_t IOT_Usart_Task_Handle;
TaskHandle_t Iot_Init_Task_Handle;
TaskHandle_t Iot_Connect_Task_Handle;
TaskHandle_t Iot_Pubsh_Task_Handle;
TaskHandle_t Iot_Recv_Task_Handle;
TaskHandle_t Sensors_Processing_Task_Handle;
TaskHandle_t Route_Task_Handle;
TaskHandle_t Idel_Task_Handle;
TaskHandle_t Gps_Task_Handle;
TaskHandle_t Gps_Send_Task_Handle;
TaskHandle_t Stable_Task_Handle;

SemaphoreHandle_t Iot_Bin_Semaphore;//��ֵ�ź���

#define BLOCK_SIZE           1     /* ����һ��arm_fir_f32����Ĳ�������� */
#define NUM_TAPS             19     /* �˲���ϵ������ */

uint32_t blockSize = BLOCK_SIZE;
uint32_t numBlocks = BLOCK_SIZE;            /* ��Ҫ����arm_fir_f32�Ĵ��� */

static float firStateF32[BLOCK_SIZE + NUM_TAPS - 1];        /* ״̬���棬��СnumTaps + blockSize - 1*/

/* ��ͨ�˲���ϵ�� ͨ��fadtool��ȡ*/
const float firCoeffs32LP[NUM_TAPS] = {
  0.001050907278223,0.003900401872444,0.009931421520278, 0.020186987747788,0.034955126760822,
  0.053297734925034, 0.072939024167560, 0.090640193301301, 0.103001497903959,  0.107442624028692,
   0.103001497903959,   0.090640193301301,   0.072939024167560,   0.053297734925034,  0.034955126760822,
     0.020186987747788,   0.009931421520278,   0.003900401872444,  0.001050907278223,
};

arm_fir_instance_f32 S;
u8 volatile IWDG_REST_FLAG=0;

#define START_STACK_SIZE 256
#define START_PRIORITY 2

void Start_Task(void);
void Bsp_Init(void);
void delay(int i);

int main(void)
{
	uint32_t i;
	
  Bsp_Init();
  xTaskCreate(Start_Task,"Start_Task",START_STACK_SIZE,NULL,START_PRIORITY,&Start_Handle);
	if(Start_Handle==NULL)
	{
	  printf("Start_Task����ʧ��");
	}
	else
	{
	  printf("Start_Task�����ɹ�\n");
	}
	vTaskStartScheduler();
}

void Bsp_Init(void)
{
//	float TDS1=445,TDS2;
//	u16 TEMP1=30,TEMP2;
//	u8 temperature_DHT11; 	    
//	u8 humidity_DHT11;
//	double Pressure;
//	double H;
	
	IWDG_Init(IWDG_Prescaler_128,0xfff);//Tout=((4��2^prer) ��rlr) /32
	Dev_USART_Init(); 
	printf("\n\n\n\n\n\n\n\n\n\nϵͳ����\n\n\n\n\n\n\n\n\n\n\n");
	switch_ctrl_init();	
	PWM1_JS_ON;
	PWM2_JS_ON;
	
	Battery_Init();
	PH_sensor_Init();
	Water_Init();
	MCU_temp_init();
 	JXCT_TDS_Init();
	NTU_Init();
	Compass_Init();	
  Sr04_Init();
	BMP_Init();
	DHT11_Init();
	TIM_Cmd(TIM4, DISABLE);	
	Light_Init();
	BUZZER_Init();
	BUZZER_OFF();
	RZ7888_Init();
	water_ctrl_OFF();
  karm_status=karm_run;
  My_List= (cyc_list_t*)malloc(sizeof(cyc_list_t));
//  water_ctrl_pumping();
	PWM1_MCU_ON;
	PWM2_MCU_ON;
 
	
}
 
void Start_Task(void)
{
  int i=0;
	
 while(1)
 {
	 	fire_phonenix_esc_init();

	  vTaskDelay(3000);
	  PWM1_JS_ON;
	  PWM2_JS_ON;
				
//		Gaussian_Parameter.Xoffest =0.005046;
//		Gaussian_Parameter.Yoffest = -0.391743;
//		Gaussian_Parameter.Kx =1;
//		Gaussian_Parameter.Ky =0.985395;
		
		taskENTER_CRITICAL(); //�����ٽ��� 
			 //������ֵ�ź��� Ĭ��Ϊ0
		Iot_Bin_Semaphore=xSemaphoreCreateBinary();
	  xSemaphoreGive(Iot_Bin_Semaphore);
		if(Iot_Bin_Semaphore==NULL)
		{
			printf("Iot_Bin_Semaphore����ʧ��");
		}
		else
		{
			printf("Iot_Bin_Semaphore�����ɹ�\n");
		}

		xTaskCreate(IOT_Usart_Task,"IOT_Usart_Task",IOT_USART_STACK_SIZE,NULL,IOT_USART_PRIORITY,&IOT_Usart_Task_Handle);
		if(IOT_Usart_Task_Handle==NULL)
		{
			printf("IOT_Usart_Task����ʧ��");
		}
		else
		{
			printf("IOT_Usart_Task�����ɹ�\n");
		}

		xTaskCreate(Iot_Init_Task,"Iot_Init_Task",IOT_INIT_STACK_SIZE,NULL,IOT_INIT_PRIORITY,&Iot_Init_Task_Handle);
		if(Iot_Init_Task_Handle==NULL)
		{
			printf("Iot_Init_Task����ʧ��");
		}
		else
		{
			printf("Iot_Init_Task�����ɹ�\n");
		}
		
			xTaskCreate(Idel_Task,"Idel_Task",IOT_RECV_STACK_SIZE,NULL,ROUTE_PRIORITY,&Idel_Task_Handle);
			if(Idel_Task_Handle==NULL)
			{
				printf("Idel_Task����ʧ��");
			}
			else
			{
				printf("Idel_Task�����ɹ�\n");
			}
	
			vTaskDelete(Start_Handle);
			taskEXIT_CRITICAL(); //�˳��ٽ���
		}
}


void delay(int i)
{
 while(i--);
}