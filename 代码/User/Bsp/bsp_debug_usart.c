#include "main.h"
#include <stdarg.h>

static char * itoa( int value, char *string, int radix );
 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 
  NVIC_Init(&NVIC_InitStructure);
}

static void IOT_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
 
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure.NVIC_IRQChannel = IOT_USART_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}

static void TDS_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
 
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure.NVIC_IRQChannel = TDS_USART_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  DEBUG_USART GPIO 配置,工作模式配置。115200 8-N-1 ，中断接收模式
  * @param  无
  * @retval 无
  */
void Debug_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
		
  RCC_AHB1PeriphClockCmd(DEBUG_USART_RX_GPIO_CLK|DEBUG_USART_TX_GPIO_CLK,ENABLE);

  /* 使能 USART 时钟 */
  RCC_APB2PeriphClockCmd(DEBUG_USART_CLK, ENABLE);
  
  /* GPIO初始化 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* 配置Tx引脚为复用功能  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_PIN  ;  
  GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* 配置Rx引脚为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_PIN;
  GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
  
 /* 连接 PXx 到 USARTx_Tx*/
  GPIO_PinAFConfig(DEBUG_USART_RX_GPIO_PORT,DEBUG_USART_RX_SOURCE,DEBUG_USART_RX_AF);

  /*  连接 PXx 到 USARTx__Rx*/
  GPIO_PinAFConfig(DEBUG_USART_TX_GPIO_PORT,DEBUG_USART_TX_SOURCE,DEBUG_USART_TX_AF);
  
  /* 配置串DEBUG_USART 模式 */
  USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  /* 硬件流控制：不使用硬件流 */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(DEBUG_USART, &USART_InitStructure); 
	
	NVIC_Configuration();
  
	/* 使能串口接收中断 */
	USART_ITConfig(DEBUG_USART, USART_IT_RXNE, ENABLE);
	USART_ITConfig ( DEBUG_USART, USART_IT_IDLE, ENABLE ); //使能串口总线空闲中断 
  /* 使能串口 */
  USART_Cmd(DEBUG_USART, ENABLE);
}

void IOT_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	
	 /* 使能时钟 */	
  RCC_AHB1PeriphClockCmd(IOT_USART_RX_GPIO_CLK|IOT_USART_TX_GPIO_CLK,ENABLE);
  RCC_APB1PeriphClockCmd(IOT_USART_CLK, ENABLE);
  
  /* GPIO初始化 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* 配置Tx引脚为复用功能  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = IOT_USART_TX_PIN  ;  
  GPIO_Init(IOT_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* 配置Rx引脚为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = IOT_USART_RX_PIN;
  GPIO_Init(IOT_USART_RX_GPIO_PORT, &GPIO_InitStructure);
  
 /* 连接 PXx 到 USARTx_Tx*/
  GPIO_PinAFConfig(IOT_USART_RX_GPIO_PORT,IOT_USART_RX_SOURCE,IOT_USART_RX_AF);

  /*  连接 PXx 到 USARTx__Rx*/
  GPIO_PinAFConfig(IOT_USART_TX_GPIO_PORT,IOT_USART_TX_SOURCE,IOT_USART_TX_AF);
  

  USART_InitStructure.USART_BaudRate = IOT_USART_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(IOT_USART, &USART_InitStructure); 
	
  /* 嵌套向量中断控制器NVIC配置 */
	IOT_NVIC_Configuration();
  
	/* 使能串口接收中断 */
	USART_ITConfig(IOT_USART, USART_IT_RXNE, ENABLE);
	USART_ITConfig (IOT_USART, USART_IT_IDLE, ENABLE ); //使能串口总线空闲中断 
  /* 使能串口 */
  USART_Cmd(IOT_USART, ENABLE);
}

void TDS_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	
	 /* 使能时钟 */	
  RCC_AHB1PeriphClockCmd(TDS_USART_RX_GPIO_CLK|TDS_USART_TX_GPIO_CLK,ENABLE);
  RCC_APB1PeriphClockCmd(TDS_USART_CLK, ENABLE);
  
  /* GPIO初始化 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* 配置Tx引脚为复用功能  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = TDS_USART_TX_PIN  ;  
  GPIO_Init(TDS_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* 配置Rx引脚为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = TDS_USART_RX_PIN;
  GPIO_Init(TDS_USART_RX_GPIO_PORT, &GPIO_InitStructure);
  
 /* 连接 PXx 到 USARTx_Tx*/
  GPIO_PinAFConfig(TDS_USART_RX_GPIO_PORT,TDS_USART_RX_SOURCE,TDS_USART_RX_AF);

  /*  连接 PXx 到 USARTx__Rx*/
  GPIO_PinAFConfig(TDS_USART_TX_GPIO_PORT,TDS_USART_TX_SOURCE,TDS_USART_TX_AF);
  

  USART_InitStructure.USART_BaudRate = TDS_USART_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(TDS_USART, &USART_InitStructure); 
	
  /* 嵌套向量中断控制器NVIC配置 */
	TDS_NVIC_Configuration();
  
	/* 使能串口接收中断 */
	USART_ITConfig(TDS_USART, USART_IT_RXNE, ENABLE);
	//USART_ITConfig (TDS_USART, USART_IT_IDLE, ENABLE ); //使能串口总线空闲中断 
  /* 使能串口 */
  USART_Cmd(TDS_USART, ENABLE);
}

void Dev_USART_Init(void)
{
	Debug_USART_Config();
	IOT_USART_Config();
}
/*****************  发送一个字符 **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/*****************  发送字符串 **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, u8 *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}


///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口 */
		USART_SendData(DEBUG_USART, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
		while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(DEBUG_USART);
}




void DEBUG_USART_IRQHandler(void)
{
	u8 Data;
 if(USART_GetITStatus(DEBUG_USART,USART_IT_RXNE)==1)
 {
   
    Data=USART_ReceiveData(DEBUG_USART);
	 	 //预留一个作为结束符
	 if(strUSART_Fram_Record.InfBit.FramLength<RX_BUF_MAX_LEN-1)
	 {
		 //储存数据到缓冲区
	  strUSART_Fram_Record.RX_DATA_BUF[strUSART_Fram_Record.InfBit.FramLength++]=Data;
		 
	 }

 }
	if ( USART_GetITStatus( DEBUG_USART, USART_IT_IDLE ) == SET )  //数据帧接收完毕,当空闲时会进入一次中断，在使能了 	USART_ITConfig ( USART3, USART_IT_IDLE, ENABLE );
	{
    strUSART_Fram_Record .InfBit .FramFinishFlag = 1;		
	  
		Data = USART_ReceiveData(DEBUG_USART);        //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)	
  }	 
 
	USART_ClearITPendingBit(DEBUG_USART,USART_IT_RXNE);

}



void IOT_USART_IRQHandler(void)
{
	u8 Data;
	BaseType_t xReturn=pdPASS;
	BaseType_t xHigherPriorityTaskWoken =  pdFALSE;
 if(USART_GetITStatus(IOT_USART,USART_IT_RXNE)==1)
 {
	 	 
    Data=USART_ReceiveData(IOT_USART);
	 //预留一个作为结束符
	 if(strIOT_Fram_Record.InfBit.FramLength<RX_BUF_MAX_LEN-1)
	 {
		 //储存数据到缓冲区
	  strIOT_Fram_Record.RX_DATA_BUF[strIOT_Fram_Record.InfBit.FramLength++]=Data;
		 
	 }

 }
 //数据帧接收完毕,当空闲时会进入一次中断，在使能了 	USART_ITConfig ( USART3, USART_IT_IDLE, ENABLE );
	if ( USART_GetITStatus( IOT_USART, USART_IT_IDLE ) == SET )  
	{
    strIOT_Fram_Record .InfBit .FramFinishFlag = 1;		
		Data = USART_ReceiveData(IOT_USART);        //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)	
  }	 
}

uint8_t Rxflag[7];
uint8_t ucTemp[7];
uint8_t TDS[6],TDS_flag;
void TDS_USART_IRQHandler(void)
{
	static u8 RxCnt = 0;
	if(USART_GetITStatus( USART2, USART_IT_RXNE ) != RESET)
	{		
	  Rxflag[2]=1;
		ucTemp[2] = USART_ReceiveData( USART2 );
		//printf("%x",ucTemp[2]);
		if(TDS_flag == 0)
		{
			if(RxCnt == 0)
			if(ucTemp[2] == 0xFF)
			{
				RxCnt = 1;
			}
			if(RxCnt>=2 && RxCnt<=7)
				 {
						TDS[RxCnt-2] = ucTemp[2];
						//printf("\r\nRXcnt = %d\r\n",RxCnt);
						RxCnt ++;
				 }
			if(RxCnt == 9)
			{
				if(ucTemp[2] == 0xAA)TDS_flag = 1;
				else	TDS_flag = 0;
				RxCnt=0;	
			}
			if(RxCnt == 1)RxCnt++;
			if(RxCnt == 8)RxCnt++;
		}

	} 
	 
}


/*
 * 函数名：USART_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口2，即USART2
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用USART2_printf( USART2, "\r\n this is a demo \r\n" );
 *            		 USART2_printf( USART2, "\r\n %d \r\n", i );
 *            		 USART2_printf( USART2, "\r\n %s \r\n", j );

 "\r\n %s \r\n" 代表的是一个字符串，可通过判断和可变参数机制，将%s视为打印字符串的标志，
  一个%对应一个函数参数
 */
void USART_printf ( USART_TypeDef * USARTx, char * Data, ... )
{
	const char *s;
	int d;   
	char buf[16];

	
	va_list ap;
	va_start(ap, Data);

	while ( * Data != 0 )     // 判断是否到达字符串结束符
	{				                          
		if ( * Data == 0x5c )  //'\'
		{									  
			switch ( *++Data ) //*++Data 取得下个字符
			{
				case 'r':							          //回车符
				Usart_SendByte(USARTx, 0x0d);
				Data ++;
				break;

				case 'n':							          //换行符
				Usart_SendByte(USARTx, 0x0a);	
				Data ++;
				break;

				default:
				Data ++;
				break;
			}			 
		}
		else if ( * Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
				s = va_arg(ap, const char *);
				
				for ( ; *s; s++) 
				{
					Usart_SendByte(USARTx,*s);
					while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
				}
				
				Data++;
				
				break;

				case 'd':			
					//十进制
				d = va_arg(ap, int);//d=(int)ap
				
				itoa(d, buf, 10);
				
				for (s = buf; *s; s++) 
				{
					Usart_SendByte(USARTx,*s);
					while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
				}
				
				Data++;
				
				break;
				
				default:
				Data++;
				
				break;
				
			}		 
		}
		
		else Usart_SendByte(USARTx, *Data++);
		
		while ( USART_GetFlagStatus ( USARTx, USART_FLAG_TXE ) == RESET );
		
	}
}
 
/*
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被USART2_printf()调用
 */
static char * itoa( int value, char *string, int radix )
{
	int     i, d;
	int     flag = 0;
	char    *ptr = string;

	/* This implementation only works for decimal numbers. */
	if (radix != 10)
	{
		*ptr = 0;
		return string;
	}

	if (!value)
	{
		*ptr++ = 0x30;
		*ptr = 0;
		return string;
	}

	/* if this is a negative value insert the minus sign. */
	if (value < 0)
	{
		*ptr++ = '-';

		/* Make the value positive. */
		value *= -1;
		
	}

	for (i = 10000; i > 0; i /= 10)
	{
		d = value / i;

		if (d || flag)
		{
			*ptr++ = (char)(d + 0x30);
			value -= (d * i);
			flag = 1;
		}
	}

	/* Null terminate the string. */
	*ptr = 0;

	return string;

} /* NCL_Itoa */
























/*********************************************END OF FILE**********************/
