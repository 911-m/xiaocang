#ifndef __BSP_IOT_BC20_H
#define __BSP_IOT_BC20_H

#include "stm32f4xx.h"
#include "main.h"

#define AT_CPIN      "AT+CPIN?"
#define AT_CEREG     "AT+CEREG?"
#define AT_CSQ       "AT+CSQ"
#define AT_CIMI      "AT+CIMI"
#define AT_CGPADDR   "AT+CGPADDR=1"
#define AT_OFF_POWER  "AT+QPOWD=0"

#define AT_QGNSSC      "AT+QGNSSC=1"
#define AT_QGNSSCX      "AT+QGNSSC?"
#define AT_QGNSSAGPS   "AT+QGNSSAGPS=1"
#define AT_RMC         "AT+QGNSSRD=\"NMEA/RMC\""
#define AT_QGNSSDB     "AT+QGNSSDB=1"

#define AT_QMTDISC      "AT+QMTDISC=0"
#define AT_QMTCLOSE     "AT+QMTCLOSE=0"
// 修正 AT_QMTOPEN，添加 contextID
#define AT_QMTOPEN      "AT+QMTOPEN=0,iot-06z00ck4m6d0030.mqtt.iothub.aliyuncs.com,1883"
#define AT_CONNECT      "AT+QMTCONN=0,\"BC20_msg\""
#define AT_QMTSUB       "AT+QMTSUB=0,1,\"/a1wTg87gs70/BC20_msg/user/get\",0"
// 去除主题名称中的空格
#define AT_SVE_QMTSUB   "AT+QMTSUB=0,1,\"/sys/a1wTg87gs70/BC20_msg/thing/service/run\",0"
#define AT_KEEPALIVE     "AT+QMTCFG=\"KEEPALIVE\",0,1800"
#define AT_KEEP_QMTSUB   "AT+QMTSUB=0,1,\"/a1wTg87gs70/BC20_msg/user/keep\",0"

// 修正 AT_EMQX_QMTOPEN，添加 contextID
#define AT_EMQX_QMTOPEN      "AT+QMTOPEN=0,o929fc62.ala.dedicated.aliyun.emqxcloud.cn,1883"
#define AT_EMQX_CONNECT      "AT+QMTCONN=0,"
#define AT_EMQX_QMTSUB       "AT+QMTSUB=0,1,\"public\",0"

#define RX_BUF_MAX_LEN     300  //最大接收缓存字节数

#define IOT_USART_STACK_SIZE 256
#define IOT_INIT_STACK_SIZE 256
#define IOT_CONNECT_STACK_SIZE 256
#define IOT_PUBSH_STACK_SIZE 256
#define IOT_RECV_STACK_SIZE 256
#define IOT_PROCESS_STACK_SIZE 256
#define ROUTE_STACK_SIZE 256

#define IOT_USART_PRIORITY 3
#define IOT_INIT_PRIORITY 3
#define IOT_CONNECT_PRIORITY 3
#define IOT_PUBSH_PRIORITY 3
#define IOT_RECV_PRIORITY 4 //最高
#define IOT_PROCESS_PRIORITY 4
#define ROUTE_PRIORITY 3

struct STRUCT_ESP_DATA_BUFFR
{
		u8 RX_DATA_BUF[RX_BUF_MAX_LEN];
    u8 RX_ISO_BUF[RX_BUF_MAX_LEN];
		__IO u16 InfAll;
		struct {
			__IO u16 FramLength       :15;                               // 14:0 
			__IO u16 FramFinishFlag   :1;                                // 15 
		} InfBit;
 
};

typedef enum link_platfrom
{
	X_link,
	aliyu,
	emqx,
}link_platfrom_t;

extern link_platfrom_t  current_link_platfrom;
extern u8 ship_start;
extern u8 ship_removeMarkers;
extern u8 ship_removeallmarkers;

extern struct STRUCT_ESP_DATA_BUFFR strIOT_Fram_Record;
extern struct STRUCT_ESP_DATA_BUFFR strUSART_Fram_Record;
//extern QueueHandle_t Recv_Mail_Queue;
extern u8 atstr[RX_BUF_MAX_LEN];


void Iot_Init_Task(void);
void IOT_Usart_Task(void);
void Iot_Connet_ALIYUN_Task(void);
void Iot_Pubsh_Task(void);
void Iot_Recv_Task(void);
void Sensors_Processing_Task(void);
void Iot_Send_AT(u8*Cmd);
void Clear_Buffer(void);
void link_platfrom_f(link_platfrom_t staus);
double gcj02_to_wgs84(double *lng, double *lat);
void Get_GPS_CJ02(u8 *Gps_String);
#endif 
