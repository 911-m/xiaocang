#ifndef __MAIN_H
#define __MAIN_H	
#include "stm32f4xx.h"
#include <stdlib.h>
#include "math.h "
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "string.h"
#include "bsp_debug_usart.h"
#include "bsp_SysTick.h"
#include "bsp_Flash.h"
#include "bsp_IOT_BC20.h"
#include "bsp_Led.h"
#include "bsp_sensors.h"
#include "bsp_Compass.h"
#include "ship_route.h"
#include "./fire_phoenix_esc/fire_phoenix_esc.h"
#include "./PH_sensor/PH_sensor.h"
#include "./JXCT_TDS/JXCT_TDS.h"
#include "./adc/bsp_adc.h"
#include "NTU.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "battery.h"
#include "cyc_list.h"
#include "primx.h"
#include "iwdg.h"
#include "water.h"
#include "HC_SR04.h"
#include "BMP280.h"
#include "IIC.h" 
#include "dht11.h"
#include "light.h"

extern SemaphoreHandle_t Iot_Bin_Semaphore;
extern TaskHandle_t Iot_Init_Task_Handle;
extern TaskHandle_t Iot_Connect_Task_Handle;
extern TaskHandle_t Iot_Pubsh_Task_Handle;
extern TaskHandle_t Iot_Recv_Task_Handle;
extern TaskHandle_t Sensors_Processing_Task_Handle;
extern TaskHandle_t Route_Task_Handle;
extern TaskHandle_t Idel_Task_Handle;
extern TaskHandle_t Gps_Send_Task_Handle;
extern TaskHandle_t Gps_Task_Handle;
extern TaskHandle_t Stable_Task_Handle;

extern arm_fir_instance_f32 S;
extern uint32_t blockSize;
#endif
