#include "bsp_Flash.h"
#include "sys.h"
#include "bsp_debug_usart.h"

uint32_t SPITimeout;

#define SPIT_FLAG_TIMEOUT 0x1000

#define CS_STATE PEout(3)

uint8_t W25q_Id[8];
#define W25Q128_WRITE_MAX  256

#define W25Q128_WRITE_ENABLE  0x06

#define W25Q128_IDCMD  0x4B
#define W25Q128_DUMMY  0xFF
#define W25Q128_JEDEC  0x9F
#define W25Q128_SECTOR_ERASE  0x20
#define W25Q128_WRITE_BYTE  0x02
#define W25Q128_REG1  0x05
#define W25Q128_READ_DATA  0x03
#define W25Q128_BLOCK_ERASE  0xD8
#define SPIT_FLAG_TIMEOUT 0x1000;


void Spi_Gpio_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
		
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOE,ENABLE);

  /* GPIO��ʼ�� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* SCK  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* MISO */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
  /* MOSI */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
  /* CS */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;  
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	
 /* */
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource2,GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource3,GPIO_AF_SPI2);	
  CS_STATE=1;
}

void Spi_Init_Config(void)
{
	
	SPI_InitTypeDef SPI_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
   Spi_Gpio_Init();
	
	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode=SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial=7;

	SPI_Init(SPI2,&SPI_InitStructure);
	SPI_Cmd(SPI2,ENABLE);
 
}
static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* �ȴ���ʱ��Ĵ���,���������Ϣ */
  printf("SPI �ȴ���ʱ!errorCode = %d",errorCode);
  return 0;
}


uint8_t W25q64_Oprate_Reg(uint8_t byte)
{
  SPITimeout = SPIT_FLAG_TIMEOUT;

  /* �ȴ����ͻ�����Ϊ�գ�TXE�¼� */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE ) == RESET)
   {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
   }

  /* д�����ݼĴ�������Ҫд�������д�뷢�ͻ����� */
  SPI_I2S_SendData(SPI2, byte);
	 
  SPITimeout = SPIT_FLAG_TIMEOUT;

  /* �ȴ����ջ������ǿգ�RXNE�¼� */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE ) == RESET)
   {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(1);
   }

  /* ��ȡ���ݼĴ�������ȡ���ջ��������� */
  return SPI_I2S_ReceiveData(SPI2);
}

void W25q64_Spi_Transmit(uint8_t*Subff,uint16_t Size)
{
  while(Size)
  {
		W25q64_Oprate_Reg(*Subff);
		Subff++;
		Size--;
  }
}

void W25q64_Spi_Receive(uint8_t*Subff,uint16_t Size)
{
	while(Size) //��0-256�ֽ�
	{
		*Subff=W25q64_Oprate_Reg(W25Q128_DUMMY);
		Size--;
		Subff++;
	}
}


void SPI_FLASH_WaitForWriteEnd(void)
{
  uint8_t FLASH_Status = 0,Cmd;

  /* ѡ�� FLASH: CS �� */
	CS_STATE=0;

  /* ���� ��״̬�Ĵ��� ���� */
	Cmd=0x05;
	W25q64_Spi_Transmit(&Cmd,1);
	
	Cmd=0xff;
  SPITimeout = SPIT_FLAG_TIMEOUT;
  /* ��FLASHæµ����ȴ� */
  do
  {
    /* ��ȡFLASHоƬ��״̬�Ĵ��� */
//	    W25q64_Spi_Transmit(&SPI1_Handle,&Cmd,1,500);//��������ʱ��
      FLASH_Status = W25q64_Oprate_Reg(0xff);//��ȡ�Ĵ���ʱ��Ƚ�������ʹ��HAL_SPI_Receive
//	  HAL_SPI_Receive(&SPI1_Handle,&FLASH_Status,1,500);
 
    {
      if((SPITimeout--) == 0) 
      {
        SPI_TIMEOUT_UserCallback(4);
        return;
      }
    } 
  }
  while ((FLASH_Status & 0x01) == SET); /* ����д���־ */

  /* ֹͣ�ź�  FLASH: CS �� */
	CS_STATE=1;
}


void W25q128_Write_Enable(void)
{
	uint8_t Cmd;
	
	Cmd=W25Q128_WRITE_ENABLE;
  /* ͨѶ��ʼ��CS�� */
	CS_STATE=0;

  /* ����дʹ������*/
	W25q64_Spi_Transmit(&Cmd,1);

  /*ͨѶ������CS�� */
	CS_STATE=1;
}

void W25q128_Read_ID(void)
{
	uint8_t Cmd,i;
	
	Cmd=W25Q128_IDCMD;
	
	CS_STATE=0;
	W25q64_Spi_Transmit(&Cmd,1);
	
	Cmd=W25Q128_DUMMY;
	
	W25q64_Spi_Transmit(&Cmd,1);
	
	W25q64_Spi_Transmit(&Cmd,1);
	
	W25q64_Spi_Transmit(&Cmd,1);
	
   W25q64_Spi_Receive(W25q_Id,8);
	
	CS_STATE=1;
	
	for(i=0;i<8;i++)
		{
			printf("%x ",W25q_Id[i]);
		}
	
}

void W25q128_Read_JEDG(void)
{
	uint8_t Cmd,i;
	
	Cmd=W25Q128_JEDEC;
	
	CS_STATE=0;
	W25q64_Spi_Transmit(&Cmd,1);
   W25q64_Spi_Receive(W25q_Id,3);
	
	CS_STATE=1;
	
	for(i=0;i<3;i++)
		{
			printf("%x ",W25q_Id[i]);
		}
	
}

//ֻҪע�ⷢ�Ͳ�����ַʱ��λ��ǰ���ɡ�������������ָ��ʱע������ĵ�ַҪ���뵽 4KB��
void W25q128_Sector_Erase(uint32_t Erase_Addr,uint16_t Erase_Num)
{
	  uint8_t Cmd,Cmd_Value=0,i;
	
		W25q128_Write_Enable();
  	  SPI_FLASH_WaitForWriteEnd();
	
		Cmd=W25Q128_SECTOR_ERASE;	
		CS_STATE=0;	
	
	for(i=0;i<Erase_Num;i++)
		{
			W25q64_Spi_Transmit(&Cmd,1);
			
			Cmd_Value=((Erase_Addr>>16)&0xff);
			W25q64_Spi_Transmit(&Cmd_Value,1);
			
			Cmd_Value=((Erase_Addr>>8)&0xff);
			W25q64_Spi_Transmit(&Cmd_Value,1);
			
			Cmd_Value=(Erase_Addr&0xff);
			W25q64_Spi_Transmit(&Cmd_Value,1);	
			Erase_Addr+=0x1000;

		}

	 CS_STATE=1;
	 SPI_FLASH_WaitForWriteEnd();	
}

//ֻҪע�ⷢ�Ͳ�����ַʱ��λ��ǰ���ɡ����ò���ָ��ʱע������ĵ�ַҪ���뵽 64KB��
void W25q128_Sector_Block(uint32_t Erase_Addr)
{
	  uint8_t Cmd,Cmd_Value=0;
	
		W25q128_Write_Enable();
		SPI_FLASH_WaitForWriteEnd();

		Cmd=W25Q128_BLOCK_ERASE;	
		CS_STATE=0;	

		W25q64_Spi_Transmit(&Cmd,1);

		Cmd_Value=((Erase_Addr>>16)&0xff);
		W25q64_Spi_Transmit(&Cmd_Value,1);

		Cmd_Value=((Erase_Addr>>8)&0xff);
		W25q64_Spi_Transmit(&Cmd_Value,1);

		Cmd_Value=(Erase_Addr&0xff);
		W25q64_Spi_Transmit(&Cmd_Value,1);	

		CS_STATE=1;
		SPI_FLASH_WaitForWriteEnd();	
}

//дһҳ
void W25q128_Write_Page(uint32_t Write_Addr,uint8_t *TxBuff,uint32_t Write_Num)
{
	uint8_t Cmd,Cmd_Value=0;
	
  W25q128_Write_Enable();
  SPI_FLASH_WaitForWriteEnd();//�ȴ�д����
	
	CS_STATE=0;	
	
	Cmd=W25Q128_WRITE_BYTE;	
	W25q64_Spi_Transmit(&Cmd,1);
	
	Cmd_Value=((Write_Addr>>16)&0xff);
	W25q64_Spi_Transmit(&Cmd_Value,1);
	
	Cmd_Value=((Write_Addr>>8)&0xff);
	W25q64_Spi_Transmit(&Cmd_Value,1);
	
	Cmd_Value=(Write_Addr&0xff);
	W25q64_Spi_Transmit(&Cmd_Value,1);	
 
	W25q64_Spi_Transmit(TxBuff,Write_Num);	

	CS_STATE=1;
	
	SPI_FLASH_WaitForWriteEnd();//�ȴ�д����
}

//д������������
void W25q128_Write_LotData(uint32_t Write_Addr,uint8_t *TxBuff,uint32_t Write_Num)
{
	uint8_t Cmd=0,i;
  uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
	
	Addr=Write_Addr%W25Q128_WRITE_MAX;//��ʼ��ַ�Ƿ���ҳ���ַ����
	
	count = W25Q128_WRITE_MAX - Addr;//����һҳ���ж����ֽ�
	
	NumOfPage=Write_Num/W25Q128_WRITE_MAX;//д��ҳ��
	NumOfSingle=Write_Num%W25Q128_WRITE_MAX;//д��ҳ��ʣ����ֽ�
	
//��ʼ��ַ��ҳ�����	
	if(Addr==0)
	{	
		if(NumOfPage==0)//д����һҳ
		{
		   W25q128_Write_Page(Write_Addr,TxBuff,Write_Num);
		}
		else//����һҳ
		{

			for(i=0;i<NumOfPage;i++)//�Ȱ�ҳд��
			{
				W25q128_Write_Page(Write_Addr,&TxBuff[i*W25Q128_WRITE_MAX],W25Q128_WRITE_MAX);
				Write_Addr+=W25Q128_WRITE_MAX;
			}
			 //дʣ�µ��ֽ�
			 W25q128_Write_Page(Write_Addr,&TxBuff[i*W25Q128_WRITE_MAX],NumOfSingle);		
		}

	}
	else//��ʼ��ַ��ҳ�治����
	{
		if(NumOfPage==0)//д����һҳ
		{
			if(count>=Write_Num)//��������ǰҳ
			{
				W25q128_Write_Page(Write_Addr,TxBuff,Write_Num);
			}
			else//������ǰҳ
			{
				W25q128_Write_Page(Write_Addr,TxBuff,count);//�ѵ�ǰҳд��
				Write_Addr+=count;
				
				temp=Write_Num-count;//ʣ���ֽ�
				W25q128_Write_Page(Write_Addr,&TxBuff[count],temp);		
			}		
		
		}
		else 
		{
			Write_Num-=count;//�����ݴ���ɵ�һ�����
			
			//���¼���
			NumOfPage=Write_Num/W25Q128_WRITE_MAX;
			NumOfSingle=Write_Num%W25Q128_WRITE_MAX;
			
			//д�굱ǰҳ
			W25q128_Write_Page(Write_Addr,TxBuff,count);
			Write_Addr+=count;//��ַƫ�Ƶ���һҳ
			
			for(i=0;i<NumOfPage;i++)
			{
				W25q128_Write_Page(Write_Addr,&TxBuff[count+i*W25Q128_WRITE_MAX],W25Q128_WRITE_MAX);
				Write_Addr+=W25Q128_WRITE_MAX;
			}
				W25q128_Write_Page(Write_Addr,&TxBuff[count+i*W25Q128_WRITE_MAX],NumOfSingle);
		}

	}
	

}
//������ָ����뼰Ҫ������ʼ��ַ��FLASH оƬ�ͻᰴ��ַ�����ķ�ʽ���ش洢��������ݣ� ��ȡ��������û�����ƣ�ֻҪû��ֹͣͨѶ��FLASH оƬ�ͻ�һֱ��������

void W25q128_Read_LotData(uint32_t Read_Addr,uint8_t *RxBuff,uint16_t Read_Num)
{
	uint8_t Cmd,Cmd_Value=0;
	
	CS_STATE=0;	
	Cmd=W25Q128_READ_DATA;	
	W25q64_Spi_Transmit(&Cmd,1);
	
	Cmd_Value=((Read_Addr>>16)&0xff);
	W25q64_Spi_Transmit(&Cmd_Value,1);
	
	Cmd_Value=((Read_Addr>>8)&0xff);
	W25q64_Spi_Transmit(&Cmd_Value,1);
	
	Cmd_Value=(Read_Addr&0xff);
	W25q64_Spi_Transmit(&Cmd_Value,1);	
 
	W25q64_Spi_Receive(RxBuff,Read_Num);	

	CS_STATE=1;

}