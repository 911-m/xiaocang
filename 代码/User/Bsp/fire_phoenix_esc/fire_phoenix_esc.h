#ifndef __fire_phoenix_esc_H
#define	__fire_phoenix_esc_H


#include "stm32f4xx.h"
#define PWM1_JS_ON 		GPIO_ResetBits(GPIOA , GPIO_Pin_4)
#define PWM1_MCU_ON 		GPIO_SetBits(GPIOA , GPIO_Pin_4)

#define PWM2_JS_ON 		GPIO_ResetBits(GPIOA , GPIO_Pin_5)
#define PWM2_MCU_ON 		GPIO_SetBits(GPIOA , GPIO_Pin_5)


#define BUZZER PBout(0)	// D0 

//电机一 
#define RZ7888_BI1 PAout(1)
#define RZ7888_FI1 PAout(7)

//		 RZ7888_BI1=1;
//		 RZ7888_FI1=0; //正常可以抽水


//电机二
#define RZ7888_BI2 PBout(8)
#define RZ7888_FI2 PBout(9)

//     RZ7888_BI2=1;
//		 RZ7888_FI2=0; //正常可以抽水


void BUZZER_Init(void);
void BUZZER_OFF(void);
void RZ7888_Init(void);
void fire_phonenix_esc_init(void);
void set_left_motor_speed(int8_t speed);
void set_right_motor_speed(int8_t speed);
void set_left_speed(int speed);
void set_right_speed(int speed);
void switch_ctrl_init(void);

void water_ctrl_off_pumping(void);
void water_ctrl_off_release(void);
void water_ctrl_pumping(void);
void water_ctrl_release(void);
void water_ctrl_OFF(void);
#endif
