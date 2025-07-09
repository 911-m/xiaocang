#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32f4xx.h"
#include <stdio.h>


//���Ŷ���
/*******************************************************/
#define DEBUG_USART                             USART1
#define DEBUG_USART_CLK                         RCC_APB2Periph_USART1
#define DEBUG_USART_BAUDRATE                    115200  //���ڲ�����

#define DEBUG_USART_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART_RX_PIN                      GPIO_Pin_10
#define DEBUG_USART_RX_AF                       GPIO_AF_USART1
#define DEBUG_USART_RX_SOURCE                   GPIO_PinSource10

#define DEBUG_USART_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART_TX_PIN                      GPIO_Pin_9
#define DEBUG_USART_TX_AF                       GPIO_AF_USART1
#define DEBUG_USART_TX_SOURCE                   GPIO_PinSource9

#define DEBUG_USART_IRQHandler                  USART1_IRQHandler
#define DEBUG_USART_IRQ                 				USART1_IRQn
/************************************************************/


/*******************************************************/
#define IOT_USART                             USART3
#define IOT_USART_CLK                         RCC_APB1Periph_USART3
#define IOT_USART_BAUDRATE                    115200  //���ڲ�����

#define IOT_USART_RX_GPIO_PORT                GPIOB
#define IOT_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOB
#define IOT_USART_RX_PIN                      GPIO_Pin_11
#define IOT_USART_RX_AF                       GPIO_AF_USART3
#define IOT_USART_RX_SOURCE                   GPIO_PinSource11

#define IOT_USART_TX_GPIO_PORT                GPIOB
#define IOT_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOB
#define IOT_USART_TX_PIN                      GPIO_Pin_10
#define IOT_USART_TX_AF                       GPIO_AF_USART3
#define IOT_USART_TX_SOURCE                   GPIO_PinSource10

#define IOT_USART_IRQHandler                  USART3_IRQHandler
#define IOT_USART_IRQ                 				USART3_IRQn
/************************************************************/

/*******************************************************/
#define TDS_USART                             USART2
#define TDS_USART_CLK                         RCC_APB1Periph_USART2
#define TDS_USART_BAUDRATE                    9600  //���ڲ�����

#define TDS_USART_RX_GPIO_PORT                GPIOA
#define TDS_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define TDS_USART_RX_PIN                      GPIO_Pin_3
#define TDS_USART_RX_AF                       GPIO_AF_USART2
#define TDS_USART_RX_SOURCE                   GPIO_PinSource3

#define TDS_USART_TX_GPIO_PORT                GPIOA
#define TDS_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define TDS_USART_TX_PIN                      GPIO_Pin_2
#define TDS_USART_TX_AF                       GPIO_AF_USART2
#define TDS_USART_TX_SOURCE                   GPIO_PinSource2

#define TDS_USART_IRQHandler                  USART2_IRQHandler
#define TDS_USART_IRQ                 				USART2_IRQn
/************************************************************/
extern uint8_t Rxflag[7];
extern uint8_t ucTemp[7];


extern uint8_t TDS[6],TDS_flag;

void Debug_USART_Config(void);
void IOT_USART_Config(void);
void TDS_USART_Config(void);
void Dev_USART_Init(void);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, u8 *str);

void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);

/*__VA_ARGS__ ��һ���ɱ�����ĺ�
 ##__VA_ARGS__ ��ǰ�����##���������ڣ����ɱ�����ĸ���Ϊ0ʱ�������##�𵽰�ǰ������","ȥ��������,�����������
 ##__VA_ARGS__��Ӧ�궨�庯�����߱���ĺ���������������Ϊ...
 ����֮�����е�fmt, ��һ����ʱ����, ������ʾ��Ҫ�����Ĳ���.*/

#define      Iot_Usart( fmt, ... )               USART_printf ( IOT_USART, fmt, ##__VA_ARGS__ ) 
#define      PC_Usart( fmt, ... )                printf ( fmt, ##__VA_ARGS__ )





#endif /* __USART1_H */
