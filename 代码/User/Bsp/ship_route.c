#include "ship_route.h"
#include <stdlib.h> 
#include "cyc_list.h"

extern char List_Arr[V_MAX];
extern double GPS_arr[V_MAX][2];
extern u8 *strx,*extstrx;
extern QueueHandle_t Recv_Mail_Queue;//接收信息队列
extern char sin_cqr[10];
#define MAX 4
u8 send_gps[40]; //组建带数据的JSON
u8 send_battry[40]; 
u8 send_cpu_temp[40]; 
u8 gps_atstr[70];

/*-------------gps定位------------*/
volatile  double wgs_maplat;
volatile  double wgs_maplon;
volatile	double wgs_gpslat;//GPS模块
volatile	double wgs_gpslon;
volatile	double map_angle;
volatile	double origin_gpslat;
volatile	double origin_gpslon;
volatile	double stale_angle;

/*-------------卡尔曼------------*/
volatile  double latQ=0.000001;
volatile  double lonQ=0.000001;
volatile	double latR=2.5;//GPS模块
volatile	double lonR=2.5;
volatile double latX, latP,latK;
volatile double lonX, lonP,lonK;
/*-------------pid------------*/
volatile double Kp;
volatile double Ki;
volatile double Kd;
volatile double angle_err_sum;
volatile double angle_err_last;
volatile double init_sleep;
volatile double error_ave;
volatile double error_cnt;
volatile double f_sleep;
volatile double pid_error;
volatile double ave_err_sum;
volatile u8 pid_start;

/*-------------其他控制变量------------*/
volatile double	Curent_Angle;
volatile u8 Compass_Check_t=0;
volatile u8 Sensors_Check=0;
volatile u8 Obstacle_Make=0;

volatile u8 MAX_V;
volatile float disance;
volatile uint16_t cnt=0;

karm_status_t karm_status;
extern uint16_t gps_flag;
extern u8 base_cnt;
//double PI = 3.1415926535897932384626;

/*-------------结构体变量------------*/
angle_state angle_error_status;
//pid_t ship_pid;
ROUTE_MSG Route_Msg;//地图信息
Route_Behavior Ship_Status;//船体状态
pid_status_t Pid_Status;//pid状态
Auto_Behavior_t  Auto_Behavior;//出发状态


void Pid_Frow(void);//pid直行控制
int Pid_Rotate(void);//pid找角度
void Compass_check(void);//指南针校验
void Con_Pwm(void);//遥控器控制PWM

void pid_init(void);
void pid_f_init(void);
double pid_control(double target, double actual);
double kalman_filter_lat(double latZ);
double kalman_filter_lon(double lonZ);

double atofx(u8 *str);
void Prgmc_to_Wgs(u8 *Gps_String);
void Map_Cj02_to_Wgs(u8 *Gps_String);

u8 Mqttaemqx_gpsdata(u8 *t_payload,char *latstr,char *lonstr)
{
	  //发送json的总长度
    int json_len;
	  //发送的json格式
    u8 json[]= "thing.service.run:ship_locatin:%s,N,%s,E";	 
		memset(send_gps,0,RX_BUF_MAX_LEN);
    sprintf(send_gps, json, latstr,lonstr); //组建需要发送的json 数据
    json_len = strlen(send_gps)/sizeof(u8);
  	memcpy(t_payload, send_gps, json_len);
    return json_len;
}

u8 Mqttaemqx_battry(u8 *t_payload,u8 battry)
{
	  //发送json的总长度
    int json_len;
	  //发送的json格式
    u8 json[]= "thing.service.run:ship_battery:%d%%";	 
		memset(send_battry,0,40);
    sprintf(send_battry, json,battry); //组建需要发送的json 数据
    json_len = strlen(send_battry)/sizeof(u8);
  	memcpy(t_payload, send_battry, json_len);
    return json_len;
}
u8 Mqttaemqx_cpu_temp(u8 *t_payload,u8 cpu_temp)
{
	  //发送json的总长度
    int json_len;
	  //发送的json格式
    u8 json[]= "thing.service.run:ship_cpu_temp:%d#";	 
		memset(send_cpu_temp,0,40);
    sprintf(send_cpu_temp, json,cpu_temp); //组建需要发送的json 数据
    json_len = strlen(send_cpu_temp)/sizeof(u8);
  	memcpy(t_payload, send_cpu_temp, json_len);
    return json_len;
}

void Idel_Task(void)
{
	static uint32_t t=0;
	u8 at,c_temp;
  while(1)
	{
   if(current_link_platfrom==emqx)
		{
			if(Ship_Status==mcu_js)
			{
				PWM1_MCU_ON;
				PWM2_MCU_ON;
			}
			else if(Ship_Status==pwm_js)
			{
				PWM1_JS_ON;
				PWM2_JS_ON;
			}

			printf("emqx\n");	
	    t++;
			if(Sensors_Check==0)
			{
        memset(send_gps,0,40);
				Mqttaemqx_gpsdata((u8*)send_gps,Sensors_Msg.latstr,Sensors_Msg.lonstr);
				memset(gps_atstr,0,60);
				sprintf(gps_atstr,"AT+QMTPUB=0,0,0,0,\"public\",%s\r\n",send_gps);
				if((t%14)==0)
				{
					memset(send_battry,0,40);
					at=Get_EmqxBattery_value();
					Mqttaemqx_battry((u8*)send_battry,at);
\
					if(at<=10)
					{
						Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_powerlose_Warning#\r\n");	
					}

					
					memset(gps_atstr,0,70);
					sprintf(gps_atstr,"AT+QMTPUB=0,0,0,0,\"public\",%s\r\n",send_battry);		
	
				}		
				else if((t%15)==0)
				{
					memset(send_cpu_temp,0,40);
					c_temp=Get_Temprate();
					Mqttaemqx_cpu_temp((u8*)send_cpu_temp,c_temp);
					
					if(c_temp>=75)
					{
						Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_hight_temp_warning#\r\n");	
					}
					memset(gps_atstr,0,70);
					sprintf(gps_atstr,"AT+QMTPUB=0,0,0,0,\"public\",%s\r\n",send_cpu_temp);	
					
				}
         printf("%s\n",gps_atstr);		
				 Iot_Send_AT(gps_atstr);
			}
			if((t%4)==0)
			{
				Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",thing.service.run:beats1\r\n");	
			}
			if((t%5)==0)
			{
			  if(base_cnt>0)
				{
				  base_cnt=0;
				}
				else
				{	
					if((Sensors_Check==0)&&(Ship_Status!=rotate))
					{
							PWM1_JS_ON;
							PWM2_JS_ON;
							/*-------------重新连接平台------------*/
							taskENTER_CRITICAL(); //进入临界区 
							vTaskSuspend(Iot_Recv_Task_Handle);
							vTaskSuspend(Gps_Send_Task_Handle);
							vTaskSuspend(Gps_Task_Handle);
							taskEXIT_CRITICAL(); //退出临界区
							link_platfrom_f(emqx);
							taskENTER_CRITICAL(); //进入临界区 
							vTaskResume(Iot_Recv_Task_Handle);
							vTaskResume(Gps_Send_Task_Handle);
							vTaskResume(Gps_Task_Handle);
							taskEXIT_CRITICAL(); //退出临界区		
							PWM1_MCU_ON;
							PWM2_MCU_ON;
					}
				}
			}

			Clear_Buffer();
		}
		else
		{
			printf("no link\n");	
			PWM1_JS_ON;
			PWM2_JS_ON;
		}
		vTaskDelay(5000);

	}
}

void Gps_Send_Task(void)
{
	static uint32_t t=0;
  while(1)
	{
		t++;
		if(t%15==0)
		{
		  Iot_Send_AT(AT_CSQ);
		}
		else if(t%20==0)
		{
			 if((current_link_platfrom==emqx))
			 {
			 sprintf(gps_atstr,"AT+QMTPUB=0,0,0,0,\"public\",Signal quality %s\r\n",sin_cqr);
       Iot_Send_AT(gps_atstr);
			 			 
			 }
		}
		else
		{
	  Iot_Send_AT(AT_RMC);		
		}

		printf("AT_RMC\n");
		vTaskDelay(1000);	
	}
}

void Gps_Task(void)
{
  u8 Gps_Data[40];
	uint16_t static t=0;
  while(1)
	{
		xQueueReceive( Recv_Mail_Queue, (void*)&Gps_Data, portMAX_DELAY );//等待Iot_Recv_Task推送GPS队列消息
	  My_IWDG_ReloadCounter();
		Prgmc_to_Wgs(Gps_Data);
		Get_GPS_CJ02(Gps_Data);
		if(gps_flag>=15)
		{
			if(t==0)
			{
			 latP=0.3;
			 latX=wgs_gpslat;
			 lonP=0.3;
			 lonX=wgs_gpslon;
			}
			else
			{
			 if(karm_status==karm_static)
			 {
				latQ=0.000001;
				lonQ=0.000001;
				latR=2.5;//GPS模块
				lonR=2.5;
			 }
			 else if(karm_status==karm_run)
			 {
				latQ=0.0009;
				lonQ=0.0009;
				latR=0.05;//GPS模块
				lonR=0.05;
			 }
			 wgs_gpslat=kalman_filter_lat(wgs_gpslat);
			 wgs_gpslon=kalman_filter_lon(wgs_gpslon);
			}
			t++;
		}
  	printf("%lf,%lf \n",(wgs_gpslon),(wgs_gpslat));
		vTaskDelay(10);			
	}
}

void Route_Task(void)
{
	u8 Gps_Data[40];
	u8 res;
	u8 w_t=0,d_t=0;
	int i=0;
	double H_angle=0;
	int out_disance;
  Auto_Behavior=set_off;
	pid_init();
	
while(1)
{
			if((Ship_Status!=pwm_js)&&(Ship_Status!=statusX))
		 {
			 PWM1_MCU_ON;
			 PWM2_MCU_ON;
		 }
		 
	   if(Ship_Status==forward)//app手动控制
		 {
			 if(Sensors_Check==0)
			 {
			    Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_check_water#\r\n");
				  Sensors_Check=1;
					water_ctrl_pumping();
					while(1)
					{
					 My_IWDG_ReloadCounter();
					 if(Ship_Status==stop)
					 {
						 water_ctrl_OFF();
						 Sensors_Check=0;
						 break;
					 }
	
						if((w_t%6==0))
						{
						Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_pumping#\r\n");								
						}
						w_t++;					
						if((w_t>=23))
						{
							w_t=0;
        			water_ctrl_OFF();
							break;
						}			
						vTaskDelay(1000);
					}
				
						if(Ship_Status!=stop)
						{
							/*-------------传感器定时上报------------*/
							Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_check_water#\r\n");													
							taskENTER_CRITICAL(); //进入临界区 
							vTaskResume(Iot_Pubsh_Task_Handle);
							vTaskResume(Sensors_Processing_Task_Handle);
							taskEXIT_CRITICAL(); //退出临界区		
							taskENTER_CRITICAL(); //进入临界区 
							vTaskSuspend(Route_Task_Handle);
							taskEXIT_CRITICAL(); //退出临界区						
							vTaskDelay(100);
							Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_Idel#\r\n");		
						}
						else
						{
							 Sensors_Check=0;
						}	
			 }
		 }
		 else if(Ship_Status==stop)
		 {
			  Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_stop#\r\n");
				PWM1_MCU_ON;
				PWM2_MCU_ON;
				Ship_Status=statusX;
				set_right_speed(0);
				set_left_speed(0);

				printf("stop\n");
		 }
		 else if(Ship_Status==rotate)//矫正指南针
		 {
//			 if(Compass_Check_t==0)
//			 {
			  Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",,Debug_ship_rotate#\r\n");

				PWM1_MCU_ON;
				PWM2_MCU_ON;			 
				Compass_check();
			  Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_Idel#\r\n");		
				Ship_Status=statusX;		 
//			 }

		 } 
		 else if(Ship_Status==status_Gps)
		 {
				Ship_Status=statusX;
				if(Compass_Check_t==0)
				{	
					Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_Please_Compass_Check!#\r\n");
				}
				else
				{
				   Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_Gps#\r\n");
				}
				if((current_link_platfrom==emqx)&&(Compass_Check_t==1))
				{					
					PWM1_MCU_ON;
					PWM2_MCU_ON;
					MAX_V=0; 
					
					if(Auto_Behavior==set_off)//出发状态
					{			
						vTaskSuspend(Gps_Send_Task_Handle);
						vTaskSuspend(Gps_Task_Handle);
						
						MAX_V+=1;
						ex_route[MAX_V]=MAX_V;
						printf("MAX_V %d",MAX_V);
						GPS_arr[MAX_V][0]=wgs_gpslon;
						GPS_arr[MAX_V][1]=wgs_gpslat;			
						
						Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",ffffffff\r\n");
						if(ship_start==pdTRUE)
						{
							ship_start=pdFAIL;
						}	
						if(ship_removeMarkers==pdTRUE)
						{
							ship_removeMarkers=pdFAIL;
						}	
						
						if(ship_removeallmarkers==pdTRUE)
						{
							ship_removeallmarkers=pdFAIL;
						}	
						
						while(1)
						{
			  			My_IWDG_ReloadCounter();//刷新看门狗
							res=xQueueReceive( Recv_Mail_Queue, (void*)&Gps_Data, 1000 );//等待Iot_Recv_Task推送GPS队列消息
							if(res==pdTRUE)
							{
								MAX_V+=1;
								printf("MAX_V %d",MAX_V);
								Map_Cj02_to_Wgs(Gps_Data);	
								GPS_arr[MAX_V][0]=wgs_maplon;//默认当前点为出发点
								GPS_arr[MAX_V][1]=wgs_maplat;			
							}
								if(ship_start==pdTRUE)
								{
									break;
								}
								if(ship_removeMarkers==pdTRUE)
								{
									ship_removeMarkers=pdFAIL;
									MAX_V-=1;	
								}						
								if(ship_removeallmarkers==pdTRUE)
								{
									ship_removeallmarkers=pdFAIL;
									MAX_V=1;
								}			
						}
						    Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_run#\r\n");
//								printf("MAX_V_cnt %d\n",MAX_V);
							for (int i = 1; i <= MAX_V; i++)
							{
								  ex_route[i]=i;
							}
							
							if(MAX_V>=5)
							{
								/*-------------贪心算法规划------------*/
									Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_primx#\r\n");
									generateCompleteGraph(MAX_V);
									printf("邻接矩阵\n");
									print_matrix_graph();//打印邻接矩阵
									init_prim();//初始化prim
									prim(MAX_V);//得出最小权值
									Minimum_Spanning_Tree(MAX_V);//通过最小权值生成最小生成树
									dfs_init(&un_greph,1);//深度优先搜索
									print_dfs_result(&un_greph);//打印深度优先搜索结果
									free_graph();
									free(un_greph.adj_list);
									free(dfs_t.marked);
									free(dfs_t.edge_to);	
									My_List = Creat_List(MAX_V);//创建一n个成员的链表	

							}
							else if((MAX_V<5)&&(MAX_V>1))
							{
								/*-----------固定点规划------------*/
									Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_list#\r\n");
									My_List = Creat_List(MAX_V);
							}
							else
							{
								
							}
 
						vTaskResume(Gps_Send_Task_Handle);
						vTaskResume(Gps_Task_Handle);

						origin_gpslat=wgs_gpslat;//记录源坐标
						origin_gpslon=wgs_gpslon;
       /*----------------------------------多点寻路-----------------------------------------*/
						if(MAX_V>1)
						{
							for (int i = 0; i <MAX_V; i++)
							{
								karm_status=karm_static;
								map_angle=My_List->next_angle;
								wgs_maplon=My_List->next_maplon;
								wgs_maplat=My_List->next_maplat;
								
								if(i==(MAX_V-1))
								{
									 Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_back#\r\n");
								}
								pid_init();
								while(1)
								{
									if(Ship_Status!=stop)
									{
											if(Pid_Rotate())
												{
													break;
												}
											PWM1_MCU_ON;
											PWM2_MCU_ON;
									}
									else
									{
										break;
									}
									
									vTaskDelay(100);
								}
								
									pid_f_init();
									karm_status=karm_run;
									while(1)
									{	 
									 if(Ship_Status!=stop)
										{
											LongLat2XY(wgs_gpslon,wgs_gpslat,&Route_Msg.map_origin_x,&Route_Msg.map_origin_y);
											LongLat2XY(wgs_maplon,wgs_maplat,&Route_Msg.map_destination_x,&Route_Msg.map_destination_y);
											
											disance=(float)Distance(Route_Msg.map_origin_x,Route_Msg.map_origin_y,Route_Msg.map_destination_x,Route_Msg.map_destination_y)/1000;
											map_angle=Angle(Route_Msg.map_origin_x,Route_Msg.map_origin_y,Route_Msg.map_destination_x,Route_Msg.map_destination_y)/PI*180;		
											convert_to_compass(&map_angle);
											Pid_Frow();			
											
											if(d_t>=20)
											{
											if(out_disance<=5)
											{
												 if(i!=(MAX_V-1))
												{
													water_ctrl_pumping();
													water_ctrl_off_release();														
												}
											}
												d_t=0;
												Iot_Usart("AT+QMTPUB=0,0,0,0,\"public\",%dm\r\n",(int)(disance*1000));
												printf("disance %dm %lf\n",(int)(disance*1000),map_angle);										
											}
											else
											{
											 d_t++;
											}

											out_disance=(int)(disance*1000);
											if(out_disance<=2)
											{
												set_right_speed(0);
												set_left_speed(0);
												if(i!=(MAX_V-1))
													{
													 Sensors_Check=1;
													 water_ctrl_pumping();
														while(1)
														{
														 if(Ship_Status==stop)
														 {
															water_ctrl_OFF();
															 break;
														 }

															if((w_t%6==0))
															{
						                  Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_pumping#\r\n");			
															}
															w_t++;
															if((w_t>=10))
															{
																w_t=0;
																break;
															}			
															vTaskDelay(1000);
														}
													
														if(Ship_Status!=stop)
														{
															/*-------------传感器定时上报------------*/
															Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_check_water#\r\n");													
															taskENTER_CRITICAL(); //进入临界区 
															vTaskResume(Iot_Pubsh_Task_Handle);
															vTaskResume(Sensors_Processing_Task_Handle);
															taskEXIT_CRITICAL(); //退出临界区		
															taskENTER_CRITICAL(); //进入临界区 
															vTaskSuspend(Route_Task_Handle);
															taskEXIT_CRITICAL(); //退出临界区						
															vTaskDelay(100);
															Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_run#\r\n");
															 break;				
														}
													 else
													 {
														 Sensors_Check=0;
													 }
													 
													}
													else
													{
													  Sensors_Check=0;
													  water_ctrl_OFF();
													}
														 break;												
											}	
											else if(out_disance>2000)
											{
													latX=0;
													latP=0;
													latK=0;
												
													lonX=0;
													lonP=0;
													lonK=0;
//												Ship_Status=stop;
//												set_right_speed(0);
//												set_left_speed(0);	
												Iot_Usart("AT+QMTPUB=0,0,0,0,\"public\",disance %dm error#\r\n",(int)(disance*1000));
////												Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",\r\n");
//												break;
											}
																		
											/*-------------中途停止直接返回出发点------------*/
											if(Ship_Status==stop)
											{
												 break;
											}
								
									 }
										else
										{
												break;
										}

										vTaskDelay(100);
									 }
										/*-------------中途停止直接返回出发点------------*/
										if(Ship_Status==stop)
										{
										 break;
										}   

										My_List = My_List->Next;
							}
						}

							if(Ship_Status==stop)
							{
								Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_error_stop#\r\n");
							}
							else
							{

								Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_ok#\r\n");
							}
					}
					else if(Auto_Behavior==brack)//返回状态
					{
						wgs_maplon=origin_gpslon;//返回时目标坐标为源坐标
						wgs_maplat=origin_gpslat;
						LongLat2XY(wgs_gpslon,wgs_gpslat,&Route_Msg.map_origin_x,&Route_Msg.map_origin_y);
						LongLat2XY(wgs_maplon,wgs_maplat,&Route_Msg.map_destination_x,&Route_Msg.map_destination_y);
						
						disance=(float)Distance(Route_Msg.map_origin_x,Route_Msg.map_origin_y,Route_Msg.map_destination_x,Route_Msg.map_destination_y)/1000;
						map_angle=Angle(Route_Msg.map_origin_x,Route_Msg.map_origin_y,Route_Msg.map_destination_x,Route_Msg.map_destination_y)/PI*180;		
						convert_to_compass(&map_angle);

						//坐标系转指南针
						convert_to_compass(&map_angle);
						printf("%lf,%lf\n",wgs_maplat,wgs_maplon);
						printf("distance=%lfkm,Angle=%lf\n",Route_Msg.map_disance,map_angle);
      		 //转角
						pid_init();
						while(1)
						{
							
							if(Ship_Status!=stop)
							{
									if(Pid_Rotate())
										{
											break;
										}
									PWM1_MCU_ON;
									PWM2_MCU_ON;
							}
							else
							{
								break;
							}
							vTaskDelay(100);
						}
						pid_f_init();				
						karm_status=karm_run;
						while(1)
						{
	
						 if(Ship_Status!=stop)
							{
									Pid_Frow();
								
									LongLat2XY(wgs_gpslon,wgs_gpslat,&Route_Msg.map_origin_x,&Route_Msg.map_origin_y);
									LongLat2XY(wgs_maplon,wgs_maplat,&Route_Msg.map_destination_x,&Route_Msg.map_destination_y);

									disance=(float)Distance(Route_Msg.map_origin_x,Route_Msg.map_origin_y,Route_Msg.map_destination_x,Route_Msg.map_destination_y)/1000;

									map_angle=Angle(Route_Msg.map_origin_x,Route_Msg.map_origin_y,Route_Msg.map_destination_x,Route_Msg.map_destination_y)/PI*180;		
									convert_to_compass(&map_angle); 

									out_disance=(int)(disance*1000);
									if(d_t>=20)
									{
										d_t=0;
										Iot_Usart("AT+QMTPUB=0,0,0,0,\"public\",%dm %lf\r\n",(int)(disance*1000),map_angle);
										printf("disance %dm %lf\n",(int)(disance*1000),map_angle);			
									}
									else
									{
									 d_t++;
									}
									
									if(out_disance<=2)
									{
										set_right_speed(0);
										set_left_speed(0);
										Sensors_Check=0;
										water_ctrl_OFF();
										Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_over#\r\n");
										break;
									}
									
						 }
							else
							{
								break;
							}
							
							vTaskDelay(100);
						 }	
					}
					
         karm_status=karm_static;

				if((Ship_Status==stop)||(Auto_Behavior==brack))
				{
						if(Auto_Behavior==set_off)
						{ 
							if(MAX_V>1)
							{
								Free_List(My_List,MAX_V);						
							}
							Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_back#\r\n");
							Auto_Behavior=brack;
							Ship_Status=status_Gps;							
						}
						else if(Auto_Behavior==brack)
						{
						  Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_Idel#\r\n");
							Auto_Behavior=set_off;
						}		
				}
				else
				{
					  if(MAX_V>1)
						{
							Free_List(My_List,MAX_V);						
						}
					  Auto_Behavior=set_off;
						Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_Idel#\r\n");
				}
		
			}

		}

		 
		 if(Ship_Status==pwm_js)
		 {
			Ship_Status=statusX;
			PWM1_JS_ON;
			PWM2_JS_ON;
		 } 
		 
		 vTaskDelay(20);
  }

}

int Pid_Rotate(void)
{
	  double sleep;
		HMC5883L_Read_XYZ();
		Gaussian_XYZ.x = (Gaussian_XYZ.x - Gaussian_Parameter.Xoffest) * Gaussian_Parameter.Kx;
		Gaussian_XYZ.y = (Gaussian_XYZ.y - Gaussian_Parameter.Yoffest) * Gaussian_Parameter.Ky;

		Gaussian_XYZ.Curent_Angle = (atan2(Gaussian_XYZ.y,Gaussian_XYZ.x) * (180.0 / 3.14159265) + 180.0);  //实际水平角度

		if(Gaussian_XYZ.Curent_Angle>=360)
		{
			Gaussian_XYZ.Curent_Angle=Gaussian_XYZ.Curent_Angle-360;
		}
		  printf("Gaussian_XYZ.Curent_Angle=%lf\n",Gaussian_XYZ.Curent_Angle);	
			double pwm_shift = pid_control(map_angle, Gaussian_XYZ.Curent_Angle); // 计算PI的系数

			if(pid_error<0)
			{
				pid_error=-pid_error;
			}
//							ave_err_sum+=pid_error;
//						  error_ave=ave_err_sum/error_cnt;
			
			if((pid_error<=20))
			{
				set_right_speed(0);
				set_left_speed(0);							
				return 1;
			}
			
//			if(error_cnt>=12)
//			{
//				set_right_speed(0);
//				set_left_speed(0);							
//				return 1;
//			}
			//printf("pwm_shift = %lf\n", pwm_shift);
			//
			if (pwm_shift > 0)
			{
					sleep = init_sleep * pwm_shift;
			}
			else if (pwm_shift < 0)
			{
					sleep = init_sleep * pwm_shift;
					sleep = -sleep;
			}
   	// printf("%lf,%lf,%lf\n", Gaussian_XYZ.Curent_Angle,sleep,angle_err_sum);
			
			if (angle_error_status == negative_greater_than_180)
			{

					//printf("----顺----\n");
					int left_pwm = (int)(0); // 左侧电机逆转，右侧电机顺转，转速相等
					int right_pwm = (int)(sleep);
					set_left_speed(left_pwm);
					set_right_speed(right_pwm);

			}
			else if (angle_error_status == negative_less_than_180)
			{

					//printf("----逆----\n");
					int left_pwm = (int)sleep; // 左侧电机顺转，右侧电机逆转，转速相等
					int right_pwm = (int)(0) ;
					set_left_speed(left_pwm);
					set_right_speed(right_pwm);

			}
			else if (angle_error_status == positive_less_than_180)
			{

					//printf("----顺----\n");
					int left_pwm = (int)(0); // 左侧电机逆转，右侧电机顺转，转速相等
					int right_pwm = (int)(sleep);
					set_left_speed(left_pwm);
					set_right_speed(right_pwm);
			}
			else if (angle_error_status == positive_greater_than_180)
			{
					// 往左转
					//printf("----逆----\n");
					int left_pwm = (int)sleep; // 左侧电机顺转，右侧电机逆转，转速相等
					int right_pwm = (int)(0) ;
					set_left_speed(left_pwm);
					set_right_speed(right_pwm);
			}
			 return 0;
			
}

void Pid_Frow(void)
{
		 double sleep;
		HMC5883L_Read_XYZ();
		Gaussian_XYZ.x = (Gaussian_XYZ.x - Gaussian_Parameter.Xoffest) * Gaussian_Parameter.Kx;
		Gaussian_XYZ.y = (Gaussian_XYZ.y - Gaussian_Parameter.Yoffest) * Gaussian_Parameter.Ky;

		Gaussian_XYZ.Curent_Angle = (atan2(Gaussian_XYZ.y,Gaussian_XYZ.x) * (180.0 / 3.14159265) + 180.0);  //实际水平角度
	 // printf("Gaussian_XYZ.Curent_Angle=%lf\n",Gaussian_XYZ.Curent_Angle);
		if(Gaussian_XYZ.Curent_Angle>=360)
		{
			Gaussian_XYZ.Curent_Angle=Gaussian_XYZ.Curent_Angle-360;
		}
		
			double pwm_shift = pid_control(map_angle, Gaussian_XYZ.Curent_Angle); // 计算PI的系数
			if (pwm_shift > 0)
			{
					sleep = init_sleep * pwm_shift;
			}
			else if (pwm_shift < 0)
			{
					sleep = init_sleep * pwm_shift;
					sleep = -sleep;
			}
			if(sleep>=f_sleep)
			{
				sleep=f_sleep;
			}
				
		if (angle_error_status == negative_greater_than_180)
		{
					//	printf("----顺时针----\n");
						int left_pwm = (int)(-(f_sleep+sleep)); 
						int right_pwm = (int)(-(f_sleep)) ;
						//printf("left=%d,right=%d\n", left_pwm,right_pwm);
						set_left_speed(left_pwm);
						set_right_speed(right_pwm);
		}
		else if (angle_error_status == negative_less_than_180)
		{
					//	printf("----逆时针----\n");
						int left_pwm = (int)(-( f_sleep)); 
						int right_pwm = (int)(-(f_sleep+sleep));
					//	printf("left=%d,right=%d\n", left_pwm,right_pwm);
						set_left_speed(left_pwm);
						set_right_speed(right_pwm);
			
		}
		else if (angle_error_status == positive_less_than_180)
		{
					//	printf("----顺时针----\n");
						int left_pwm = (int)(-(f_sleep+sleep) ); 
						int right_pwm = (int)(-(f_sleep)) ;
						set_left_speed(left_pwm);
						set_right_speed(right_pwm);
						//printf("left=%d,right=%d\n", left_pwm,right_pwm);
		}
		else if (angle_error_status == positive_greater_than_180)
		{
					//	printf("----逆时针----\n");
						int left_pwm = (int)(-( f_sleep)); 
						int right_pwm = (int)(-(f_sleep+sleep));
						set_left_speed(left_pwm);
						set_right_speed(right_pwm);
						//printf("left=%d,right=%d\n", left_pwm,right_pwm);
		}
			
}

void Con_Pwm(void)
{
	 static u8 flag=0;
	 flag+=1;
	 printf("flag=%d\n",flag);
	if(flag==1)
	{
		set_left_speed(-100);				
		set_right_speed(-100);	

	}
	else if(flag==2)
	{
		set_left_speed(100);				
		set_right_speed(0);	
	}
	else if(flag==3)
	{
		set_left_speed(0);				
		set_right_speed(100);	
	}
	else if(flag==4)
	{
		set_left_speed(-100);				
		set_right_speed(0);	
	}
	else if(flag==5)
	{
		set_left_speed(0);				
		set_right_speed(-100);	
	}
	else if(flag==6)
	{
		set_left_speed(100);				
		set_right_speed(100);	
	  flag=0;
	}
	
}

void Compass_check(void)
{
	double GaX,GaY,GaXmax=0,GaXmin=0,GaYmax=0,GaYmin=0;
	u16 compass_check_flag=0;
	int i=0;
	
	set_left_speed(-140);
	while(i <= 100)
	{
		HMC5883L_Read_XYZ();

		GaXmax = GaXmax < Gaussian_XYZ.x? Gaussian_XYZ.x:GaXmax;

		GaXmin = GaXmin > Gaussian_XYZ.x? Gaussian_XYZ.x:GaXmin;

		GaYmax = GaYmax < Gaussian_XYZ.y? Gaussian_XYZ.y:GaYmax;

		GaYmin = GaYmin > Gaussian_XYZ.y? Gaussian_XYZ.y:GaYmin;
		printf("check.....");	
		i++;
		vTaskDelay(100);
	}
	set_left_speed(0);
	Gaussian_Parameter.Xoffest = (GaXmax+GaXmin)/2;
	Gaussian_Parameter.Yoffest = (GaYmax+GaYmin)/2;
	Gaussian_Parameter.Kx =1;
	Gaussian_Parameter.Ky =(GaXmax - GaXmin)/(GaYmax - GaYmin);
	
  i=0;
	set_left_speed(-140);
	while(i <= 100)
	{
    My_IWDG_ReloadCounter();
	  HMC5883L_Read_XYZ();
		Gaussian_XYZ.x = (Gaussian_XYZ.x - Gaussian_Parameter.Xoffest) * Gaussian_Parameter.Kx;
		Gaussian_XYZ.y = (Gaussian_XYZ.y - Gaussian_Parameter.Yoffest) * Gaussian_Parameter.Ky;
		
		Gaussian_XYZ.Curent_Angle = (atan2(Gaussian_XYZ.y,Gaussian_XYZ.x) * (180.0 / 3.14159265) + 180.0);  //实际水平角度
		if(Gaussian_XYZ.Curent_Angle>=360)
		{
			Gaussian_XYZ.Curent_Angle=Gaussian_XYZ.Curent_Angle-360;
		}
		
		if((Gaussian_XYZ.Curent_Angle>0)&&(Gaussian_XYZ.Curent_Angle<30))
		{
		  compass_check_flag=(compass_check_flag|0x01);
		}
		
    if((Gaussian_XYZ.Curent_Angle>30)&&(Gaussian_XYZ.Curent_Angle<60))
		{
			compass_check_flag=(compass_check_flag|0x02);	
		}
		
		if((Gaussian_XYZ.Curent_Angle>60)&&(Gaussian_XYZ.Curent_Angle<90))
		{
			compass_check_flag=(compass_check_flag|0x04);	
		}
		
		if((Gaussian_XYZ.Curent_Angle>90)&&(Gaussian_XYZ.Curent_Angle<120))
		{
			compass_check_flag=(compass_check_flag|0x08);	
		}
		
		 if((Gaussian_XYZ.Curent_Angle>120)&&(Gaussian_XYZ.Curent_Angle<150))
		{
			compass_check_flag=(compass_check_flag|0x10);	
		}
		
		if((Gaussian_XYZ.Curent_Angle>150)&&(Gaussian_XYZ.Curent_Angle<180))
		{
			compass_check_flag=(compass_check_flag|0x20);	
		}
	
		if((Gaussian_XYZ.Curent_Angle>250)&&(Gaussian_XYZ.Curent_Angle<280))
		{
			compass_check_flag=(compass_check_flag|0x40);	
		}
		
		if((Gaussian_XYZ.Curent_Angle>300)&&(Gaussian_XYZ.Curent_Angle<330))
		{
			compass_check_flag=(compass_check_flag|0x80);	
		}
		
		  printf("Curent_Angle=%lf\n",Gaussian_XYZ.Curent_Angle);
		i++;
		vTaskDelay(100);
	}
	
	if((compass_check_flag==0xff))
	{ 
	   Compass_Check_t=1;
	   Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_Compass_Successful#\r\n");
	}
	else
	{
		 Iot_Send_AT("AT+QMTPUB=0,0,0,0,\"public\",Debug_ship_Compass_error#\r\n");
	   Compass_Check_t=0;
	}
	compass_check_flag=0;
	set_left_speed(0);	
}

//处理WGS字符串
void Prgmc_to_Wgs(u8 *Gps_String)
{
		int i=0;
    strx=strstr((const u8*)Gps_String,(const u8*)"A,");//获取纬度的位置
       if(strx)
        {
            for(i=0;i<9;i++)
            {
             Route_Msg.Latitude[i]=strx[i+2];//获取纬度值2603.9576
            }
						strx=strstr((const u8*)Gps_String,(const u8*)"N,");//获取经度值
						if(strx)
						{
								 for(i=0;i<10;i++)	//获取经度 11912.4098
								 {
										Route_Msg.Longitude[i]=strx[i+2];
								 }
								 
						}  
						
					 //字符串转浮点数
						wgs_gpslat=atofx(Route_Msg.Latitude);
						wgs_gpslon=atofx(Route_Msg.Longitude);
						
						wgs_gpslon = (int)(wgs_gpslon/100) + (wgs_gpslon/100.0 - (int)(wgs_gpslon/100)) *100.0 / 60.0;
						wgs_gpslat = (int)(wgs_gpslat/100) + (wgs_gpslat/100.0 - (int)(wgs_gpslat/100)) *100.0 / 60.0;
 
				 
	    }
                            
			Clear_Buffer();
}

//CJ02-WGS
void Map_Cj02_to_Wgs(u8 *Gps_String)
{
		int i=0;
    strx=strstr((const u8*)Gps_String,(const u8*)"A,");//获取纬度的位置
//	  printf("strx=%s\n",strx);
       if(strx)
        {
            for(i=0;i<9;i++)
            {
             Route_Msg.Latitude[i]=strx[i+2];//获取纬度值2603.9576
            }
						strx=strstr((const u8*)Gps_String,(const u8*)"N,");//获取经度值
						if(strx)
						{
								 for(i=0;i<10;i++)	//获取经度 11912.4098
								 {
										Route_Msg.Longitude[i]=strx[i+2];
								 }
								 
						}  
						Route_Msg.cj02_maplat=atofx(Route_Msg.Latitude);
						Route_Msg.cj02_maplon=atofx(Route_Msg.Longitude);
						//printf("cj02_maplat %lf cj02_maplon %lf\n",Route_Msg.cj02_maplat,Route_Msg.cj02_maplon);
						
						wgs_maplat=Route_Msg.cj02_maplat;
						wgs_maplon=Route_Msg.cj02_maplon;
						gcj02_to_wgs84(&wgs_maplon,&wgs_maplat);
				   	//printf("wgs_maplat %lf wgs_maplon %lf\n",wgs_maplat,wgs_maplon);
 		 
	    }
				
                   
			Clear_Buffer();
}

/*写一个字符串转浮点数的函数*/
double atofx(u8 *str)
{
    double result = 0.0;
    int i = 0;
    int flag = 0;
    int point = 0;
    int point_num = 0;
    int len = strlen(str);
    for(i = 0; i < len; i++)
    {
        if(str[i] == '-')
        {
            flag = 1;
        }
        else if(str[i] == '.')
        {
            point = 1;
        }
        else if(str[i] >= '0' && str[i] <= '9')
        {
            if(point == 0)
            {
                result = result * 10 + (str[i] - '0');
            }
            else
            {
                point_num++;
                result = result + (str[i] - '0') * pow(0.1, point_num);
            }
        }
        else
        {
            break;
        }
    }
    if(flag == 1)
    {
        result = -result;
    }
    return result;
}

double Distance(double X1,double Y1,double X2,double Y2)
{
    
    return (sqrt(pow((X2 - X1), 2) + pow((Y2 - Y1), 2)));  
}

double Angle(double X1,double Y1,double X2,double Y2)
{
   return atan2((Y2-Y1),(X2-X1));
}

void convert_to_compass(double *A_angle) 
{
    double compass_angle;
    compass_angle = 90 - *A_angle;
    if(compass_angle < 0) {
        compass_angle += 360;
    }
		*A_angle=compass_angle;
}

int LongLat2XY(double longitude,double latitude,double *X,double *Y)
{
    int ProjNo=0; int ZoneWide; //带宽
    double longitude1,latitude1, longitude0,latitude0, X0,Y0, xval,yval;
    double a, f, e2, ee, NN, T, C, A, M, iPI;
    iPI = 0.0174532925199433;  //3.1415926535898/180.0;
    ZoneWide = 3;  //3度带宽
    //ZoneWide = 6; 6度带宽
    //a=6378245.0; f=1.0/298.3; //54年北京坐标系参数
    //a=6378140.0; f=1/298.257; //80年西安坐标系参数
    a = 6378137.0; f = 1.0/298.257223563;//WGS84坐标系参数
    //ProjNo = (int)(longitude / ZoneWide) ;      //6度带
    //longitude0 = ProjNo * ZoneWide + ZoneWide / 2; //6度带
    ProjNo = (int)(longitude / ZoneWide+0.5) ;
    // ProjNo = (int)(longitude / ZoneWide) ; //--带号
    longitude0 = ProjNo * ZoneWide ; //--中央子午线
    longitude0 = longitude0 * iPI ;//--中央子午线转化为弧度
    latitude0=0;
    longitude1 = longitude * iPI ; //经度转换为弧度
    latitude1 = latitude * iPI ; //纬度转换为弧度
    e2=2*f-f*f;
    ee=e2*(1.0-e2);
    NN=a/sqrt(1.0-e2*sin(latitude1)*sin(latitude1));
    T=tan(latitude1)*tan(latitude1);
    C=ee*cos(latitude1)*cos(latitude1);
    A=(longitude1-longitude0)*cos(latitude1);

    M=a*((1-e2/4-3*e2*e2/64-5*e2*e2*e2/256)*latitude1-(3*e2/8+3*e2*e2/32+45*e2*e2*e2/1024)*sin(2*latitude1)
         +(15*e2*e2/256+45*e2*e2*e2/1024)*sin(4*latitude1)-(35*e2*e2*e2/3072)*sin(6*latitude1));

    xval = NN*(A+(1-T+C)*A*A*A/6+(5-18*T+T*T+72*C-58*ee)*A*A*A*A*A/120);
    yval = M+NN*tan(latitude1)*(A*A/2+(5-T+9*C+4*C*C)*A*A*A*A/24
                                +(61-58*T+T*T+600*C-330*ee)*A*A*A*A*A*A/720);
    //X0 = 1000000L*(ProjNo+1)+500000L; //6度带
    X0 = 1000000L*ProjNo+500000L;  //3度带
    Y0 = 0;
    xval = xval+X0; yval = yval+Y0;

    *X= xval;
    *Y= yval;
    //printf("%lf   %lf\r\n",xval,yval);
    return 1;
}

 // 航向角进行PID 控制，
// 调速范围 0-260

void pid_init(void)
	{
    Kp = 0.0010;// 比例系数 0.020
    Ki = 0.0042;// 积分系数 0.0042
    Kd = 0.0001; //  微分系数
    angle_err_sum = 0.0;// 航向角误差的累计值
    angle_err_last = 0.0;// 上一次航向角误差
    init_sleep = 60.0;// 初始PWM值 60
	  error_cnt=0;
	  error_ave=0;
	  ave_err_sum=0;
	  pid_start=pid_rotate;
}

void pid_f_init(void)
{
    Kp = 2.5;// 比例系数
    //Ki = 0.0066;// 积分系数
    //Kd = 0.03; // 微分系数
    angle_err_sum = 0.0;//  
    angle_err_last = 0.0;// 
    init_sleep = 1.0;// 
    f_sleep=80;
	
	  error_cnt=0;
	  error_ave=0;
	  pid_start=pid_forward;
}
double get_angle_error(double target, double actual) // 计算航向角误差
{
    return target - actual;
}

double pid_control(double target, double actual) // PID控制器
{
	  double pid_out;
    double angle_error = get_angle_error(target, actual); // 计算航向角误差
    pid_error=angle_error;
    if ((angle_error <= 180) && (angle_error > 0))
    {
        angle_error_status = positive_less_than_180;
    }
    else if (angle_error > 180)
    {
        angle_error = 360 - angle_error;
        angle_error_status = positive_greater_than_180;
    }
    else if ((angle_error >= -180) && (angle_error < 0))
    {
        //printf("negative_less_than_180 angle_error = %lf\n", angle_error);
        angle_error_status = negative_less_than_180;
    }
    else if (angle_error < -180)
    {
        angle_error = 360 + angle_error;
        //printf("negative_greater_than_180 angle_error = %lf\n", angle_error);
        angle_error_status = negative_greater_than_180;
    }
      angle_err_sum += angle_error*0.1; // t=10ms
		if(pid_start==pid_rotate)
		{
			if (angle_err_sum > 8000) // 积分限幅
			{
					angle_err_sum = 8000;
			}
			else if (angle_err_sum < -8000) // 积分限幅
			{
					angle_err_sum = -8000;
			}
			  pid_out = Kp * angle_error + Ki * angle_err_sum + ((angle_error - angle_err_last)/0.1) * Kd;
		}
		else if(pid_start==pid_forward)
		{
			if (angle_err_sum > 1000) // 积分限幅
			{
					angle_err_sum = 1000;
			}
			else if (angle_err_sum < -1000) // 积分限幅
			{
					angle_err_sum = -1000;
			}
			   pid_out = Kp * angle_error;
		}

    //double pid_out = Kp * angle_error + Ki * angle_err_sum + ((angle_error - angle_err_last)/0.1) * Kd;

//    printf("angle_error = %lf, angle_err_sum = %lf, pid_out = %lf,Ki * angle_err_sum= %lf,(angle_err_last-angle_error)*Kd=%lf\n", angle_error, angle_err_sum, pid_out, Ki * angle_err_sum, (angle_err_last - angle_error) * Kd);
  // printf("angle_error = %lf, angle_err_sum = %lf, pid_out = %lf,Ki * angle_err_sum= %lf\n", angle_error, angle_err_sum, pid_out, Ki * angle_err_sum);
		angle_err_last = angle_error; // 上一次航向角误差
    return pid_out;
}




double kalman_filter_lat(double latZ)
{
    double latX_, latP_;
    latX_ = latX;
    latP_ = latP + latQ;
    latK = latP_ / (latP_ + latR);
    latX = latX_ + latK * (latZ - latX_);
    latP = (1 - latK) * latP_;
    return latX;
}

double kalman_filter_lon(double lonZ)
{
    double lonX_, lonP_;
    lonX_ = lonX;
    lonP_ = lonP + lonQ;
    lonK = lonP_ / (lonP_ + lonR);
    lonX = lonX_ + lonK * (lonZ - lonX_);
    lonP = (1 - lonK) * lonP_;
    return lonX;
}