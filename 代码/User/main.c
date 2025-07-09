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

SemaphoreHandle_t Iot_Bin_Semaphore;//二值信号量

#define BLOCK_SIZE           1     /* 调用一次arm_fir_f32处理的采样点个数 */
#define NUM_TAPS             19     /* 滤波器系数个数 */

uint32_t blockSize = BLOCK_SIZE;
uint32_t numBlocks = BLOCK_SIZE;            /* 需要调用arm_fir_f32的次数 */

static float firStateF32[BLOCK_SIZE + NUM_TAPS - 1];        /* 状态缓存，大小numTaps + blockSize - 1*/

/* 低通滤波器系数 通过fadtool获取*/
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
	  printf("Start_Task创建失败");
	}
	else
	{
	  printf("Start_Task创建成功\n");
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
	
	IWDG_Init(IWDG_Prescaler_128,0xfff);//Tout=((4×2^prer) ×rlr) /32
	Dev_USART_Init(); 
	printf("\n\n\n\n\n\n\n\n\n\n系统启动\n\n\n\n\n\n\n\n\n\n\n");
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
		
		taskENTER_CRITICAL(); //进入临界区 
			 //创建二值信号量 默认为0
		Iot_Bin_Semaphore=xSemaphoreCreateBinary();
	  xSemaphoreGive(Iot_Bin_Semaphore);
		if(Iot_Bin_Semaphore==NULL)
		{
			printf("Iot_Bin_Semaphore创建失败");
		}
		else
		{
			printf("Iot_Bin_Semaphore创建成功\n");
		}

		xTaskCreate(IOT_Usart_Task,"IOT_Usart_Task",IOT_USART_STACK_SIZE,NULL,IOT_USART_PRIORITY,&IOT_Usart_Task_Handle);
		if(IOT_Usart_Task_Handle==NULL)
		{
			printf("IOT_Usart_Task创建失败");
		}
		else
		{
			printf("IOT_Usart_Task创建成功\n");
		}

		xTaskCreate(Iot_Init_Task,"Iot_Init_Task",IOT_INIT_STACK_SIZE,NULL,IOT_INIT_PRIORITY,&Iot_Init_Task_Handle);
		if(Iot_Init_Task_Handle==NULL)
		{
			printf("Iot_Init_Task创建失败");
		}
		else
		{
			printf("Iot_Init_Task创建成功\n");
		}
		
			xTaskCreate(Idel_Task,"Idel_Task",IOT_RECV_STACK_SIZE,NULL,ROUTE_PRIORITY,&Idel_Task_Handle);
			if(Idel_Task_Handle==NULL)
			{
				printf("Idel_Task创建失败");
			}
			else
			{
				printf("Idel_Task创建成功\n");
			}
	
			vTaskDelete(Start_Handle);
			taskEXIT_CRITICAL(); //退出临界区
		}
}


void delay(int i)
{
 while(i--);
}