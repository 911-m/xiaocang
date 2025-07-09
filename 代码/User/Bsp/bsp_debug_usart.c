#include "main.h"
#include <stdarg.h>

static char * itoa( int value, char *string, int radix );
 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
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
  * @brief  DEBUG_USART GPIO ����,����ģʽ���á�115200 8-N-1 ���жϽ���ģʽ
  * @param  ��
  * @retval ��
  */
void Debug_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
		
  RCC_AHB1PeriphClockCmd(DEBUG_USART_RX_GPIO_CLK|DEBUG_USART_TX_GPIO_CLK,ENABLE);

  /* ʹ�� USART ʱ�� */
  RCC_APB2PeriphClockCmd(DEBUG_USART_CLK, ENABLE);
  
  /* GPIO��ʼ�� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* ����Tx����Ϊ���ù���  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_PIN  ;  
  GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* ����Rx����Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_PIN;
  GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
  
 /* ���� PXx �� USARTx_Tx*/
  GPIO_PinAFConfig(DEBUG_USART_RX_GPIO_PORT,DEBUG_USART_RX_SOURCE,DEBUG_USART_RX_AF);

  /*  ���� PXx �� USARTx__Rx*/
  GPIO_PinAFConfig(DEBUG_USART_TX_GPIO_PORT,DEBUG_USART_TX_SOURCE,DEBUG_USART_TX_AF);
  
  /* ���ô�DEBUG_USART ģʽ */
  USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  /* Ӳ�������ƣ���ʹ��Ӳ���� */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(DEBUG_USART, &USART_InitStructure); 
	
	NVIC_Configuration();
  
	/* ʹ�ܴ��ڽ����ж� */
	USART_ITConfig(DEBUG_USART, USART_IT_RXNE, ENABLE);
	USART_ITConfig ( DEBUG_USART, USART_IT_IDLE, ENABLE ); //ʹ�ܴ������߿����ж� 
  /* ʹ�ܴ��� */
  USART_Cmd(DEBUG_USART, ENABLE);
}

void IOT_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	
	 /* ʹ��ʱ�� */	
  RCC_AHB1PeriphClockCmd(IOT_USART_RX_GPIO_CLK|IOT_USART_TX_GPIO_CLK,ENABLE);
  RCC_APB1PeriphClockCmd(IOT_USART_CLK, ENABLE);
  
  /* GPIO��ʼ�� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* ����Tx����Ϊ���ù���  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = IOT_USART_TX_PIN  ;  
  GPIO_Init(IOT_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* ����Rx����Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = IOT_USART_RX_PIN;
  GPIO_Init(IOT_USART_RX_GPIO_PORT, &GPIO_InitStructure);
  
 /* ���� PXx �� USARTx_Tx*/
  GPIO_PinAFConfig(IOT_USART_RX_GPIO_PORT,IOT_USART_RX_SOURCE,IOT_USART_RX_AF);

  /*  ���� PXx �� USARTx__Rx*/
  GPIO_PinAFConfig(IOT_USART_TX_GPIO_PORT,IOT_USART_TX_SOURCE,IOT_USART_TX_AF);
  

  USART_InitStructure.USART_BaudRate = IOT_USART_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(IOT_USART, &USART_InitStructure); 
	
  /* Ƕ�������жϿ�����NVIC���� */
	IOT_NVIC_Configuration();
  
	/* ʹ�ܴ��ڽ����ж� */
	USART_ITConfig(IOT_USART, USART_IT_RXNE, ENABLE);
	USART_ITConfig (IOT_USART, USART_IT_IDLE, ENABLE ); //ʹ�ܴ������߿����ж� 
  /* ʹ�ܴ��� */
  USART_Cmd(IOT_USART, ENABLE);
}

void TDS_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	
	 /* ʹ��ʱ�� */	
  RCC_AHB1PeriphClockCmd(TDS_USART_RX_GPIO_CLK|TDS_USART_TX_GPIO_CLK,ENABLE);
  RCC_APB1PeriphClockCmd(TDS_USART_CLK, ENABLE);
  
  /* GPIO��ʼ�� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* ����Tx����Ϊ���ù���  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = TDS_USART_TX_PIN  ;  
  GPIO_Init(TDS_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* ����Rx����Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = TDS_USART_RX_PIN;
  GPIO_Init(TDS_USART_RX_GPIO_PORT, &GPIO_InitStructure);
  
 /* ���� PXx �� USARTx_Tx*/
  GPIO_PinAFConfig(TDS_USART_RX_GPIO_PORT,TDS_USART_RX_SOURCE,TDS_USART_RX_AF);

  /*  ���� PXx �� USARTx__Rx*/
  GPIO_PinAFConfig(TDS_USART_TX_GPIO_PORT,TDS_USART_TX_SOURCE,TDS_USART_TX_AF);
  

  USART_InitStructure.USART_BaudRate = TDS_USART_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(TDS_USART, &USART_InitStructure); 
	
  /* Ƕ�������жϿ�����NVIC���� */
	TDS_NVIC_Configuration();
  
	/* ʹ�ܴ��ڽ����ж� */
	USART_ITConfig(TDS_USART, USART_IT_RXNE, ENABLE);
	//USART_ITConfig (TDS_USART, USART_IT_IDLE, ENABLE ); //ʹ�ܴ������߿����ж� 
  /* ʹ�ܴ��� */
  USART_Cmd(TDS_USART, ENABLE);
}

void Dev_USART_Init(void)
{
	Debug_USART_Config();
	IOT_USART_Config();
}
/*****************  ����һ���ַ� **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/*****************  �����ַ��� **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, u8 *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* �ȴ�������� */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}


///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ����� */
		USART_SendData(DEBUG_USART, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		/* �ȴ������������� */
		while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(DEBUG_USART);
}




void DEBUG_USART_IRQHandler(void)
{
	u8 Data;
 if(USART_GetITStatus(DEBUG_USART,USART_IT_RXNE)==1)
 {
   
    Data=USART_ReceiveData(DEBUG_USART);
	 	 //Ԥ��һ����Ϊ������
	 if(strUSART_Fram_Record.InfBit.FramLength<RX_BUF_MAX_LEN-1)
	 {
		 //�������ݵ�������
	  strUSART_Fram_Record.RX_DATA_BUF[strUSART_Fram_Record.InfBit.FramLength++]=Data;
		 
	 }

 }
	if ( USART_GetITStatus( DEBUG_USART, USART_IT_IDLE ) == SET )  //����֡�������,������ʱ�����һ���жϣ���ʹ���� 	USART_ITConfig ( USART3, USART_IT_IDLE, ENABLE );
	{
    strUSART_Fram_Record .InfBit .FramFinishFlag = 1;		
	  
		Data = USART_ReceiveData(DEBUG_USART);        //�������������жϱ�־λ(�ȶ�USART_SR��Ȼ���USART_DR)	
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
	 //Ԥ��һ����Ϊ������
	 if(strIOT_Fram_Record.InfBit.FramLength<RX_BUF_MAX_LEN-1)
	 {
		 //�������ݵ�������
	  strIOT_Fram_Record.RX_DATA_BUF[strIOT_Fram_Record.InfBit.FramLength++]=Data;
		 
	 }

 }
 //����֡�������,������ʱ�����һ���жϣ���ʹ���� 	USART_ITConfig ( USART3, USART_IT_IDLE, ENABLE );
	if ( USART_GetITStatus( IOT_USART, USART_IT_IDLE ) == SET )  
	{
    strIOT_Fram_Record .InfBit .FramFinishFlag = 1;		
		Data = USART_ReceiveData(IOT_USART);        //�������������жϱ�־λ(�ȶ�USART_SR��Ȼ���USART_DR)	
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
 * ��������USART_printf
 * ����  ����ʽ�������������C���е�printf��������û���õ�C��
 * ����  ��-USARTx ����ͨ��������ֻ�õ��˴���2����USART2
 *		     -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *			   -...    ��������
 * ���  ����
 * ����  ���� 
 * ����  ���ⲿ����
 *         ����Ӧ��USART2_printf( USART2, "\r\n this is a demo \r\n" );
 *            		 USART2_printf( USART2, "\r\n %d \r\n", i );
 *            		 USART2_printf( USART2, "\r\n %s \r\n", j );

 "\r\n %s \r\n" �������һ���ַ�������ͨ���жϺͿɱ�������ƣ���%s��Ϊ��ӡ�ַ����ı�־��
  һ��%��Ӧһ����������
 */
void USART_printf ( USART_TypeDef * USARTx, char * Data, ... )
{
	const char *s;
	int d;   
	char buf[16];

	
	va_list ap;
	va_start(ap, Data);

	while ( * Data != 0 )     // �ж��Ƿ񵽴��ַ���������
	{				                          
		if ( * Data == 0x5c )  //'\'
		{									  
			switch ( *++Data ) //*++Data ȡ���¸��ַ�
			{
				case 'r':							          //�س���
				Usart_SendByte(USARTx, 0x0d);
				Data ++;
				break;

				case 'n':							          //���з�
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
				case 's':										  //�ַ���
				s = va_arg(ap, const char *);
				
				for ( ; *s; s++) 
				{
					Usart_SendByte(USARTx,*s);
					while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
				}
				
				Data++;
				
				break;

				case 'd':			
					//ʮ����
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
 * ��������itoa
 * ����  ������������ת�����ַ���
 * ����  ��-radix =10 ��ʾ10���ƣ��������Ϊ0
 *         -value Ҫת����������
 *         -buf ת������ַ���
 *         -radix = 10
 * ���  ����
 * ����  ����
 * ����  ����USART2_printf()����
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
