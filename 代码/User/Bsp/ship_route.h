#ifndef __SHIP_ROUTE_H
#define __SHIP_ROUTE_H
#include "stm32f4xx.h"
#include "main.h"
 
typedef struct route_msg
{
		u8 Latitude[10];
		u8 Longitude[10]; 
	volatile	double wgs_gpslat;//GPS模块
	volatile	double wgs_gpslon;

//		double wgs_maplat;//高德地图
//		double wgs_maplon;

		double cj02_maplat;//高德地图
		double cj02_maplon;

		double map_destination_x;//目标高斯坐标
		double map_destination_y;

		double map_origin_x;//源高斯坐标
		double map_origin_y;

		double map_disance;//目标距离
		double map_angle;//角度

		double origin_gpslat;//源GPS坐标
		double origin_gpslon;

		double wgs_f_maplat;//GPS模块
		double wgs_f_maplon;
	 
}ROUTE_MSG;

typedef enum route_behavior
{
	statusX,
	forward,
	stop,
	rotate,
  status_Gps,
	pwm_js,
	mcu_js,
}Route_Behavior;

typedef enum auto_route_behavior
{
	set_off,
	brack,
}Auto_Behavior_t;

typedef enum start
{
	pid_forward,
	pid_rotate,
}pid_status_t;

typedef enum karm
{
	karm_run,
	karm_static,
}karm_status_t;

//typedef struct pid
//{
//    double Kp;
//    double Ki;
//    double Kd;
//    double angle_err_sum;
//    double angle_err_last;
//    double init_sleep;
//	  double error_ave;
//	  double error_cnt;
//	  double f_sleep;
//	  double error;
//    double ave_err_sum;
//	  u8 start;
//	
//} pid_t;

typedef enum
{
    positive_greater_than_180,
    positive_less_than_180,
    negative_greater_than_180,
    negative_less_than_180,
} angle_state;

extern pid_status_t Pid_Status;
extern Route_Behavior Ship_Status;
extern angle_state angle_error_status;
//extern pid_t ship_pid;
extern Auto_Behavior_t  Auto_Behavior;
extern karm_status_t karm_status;
extern Correction_Parameter Gaussian_Parameter;
extern ROUTE_MSG Route_Msg;


void Idel_Task(void);
void Route_Task(void);
void Gps_Task(void);
void Gps_Send_Task(void);
void Stable_Task(void);

double Angle(double X1,double Y1,double X2,double Y2);
double Distance(double X1,double Y1,double X2,double Y2);
int LongLat2XY(double longitude,double latitude,double *X,double *Y);
void convert_to_compass(double *angle);
void Con_Pwm(void);

#endif
