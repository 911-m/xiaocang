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

  /* GPIO初始化 */
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
  /* 等待超时后的处理,输出错误信息 */
  printf("SPI 等待超时!errorCode = %d",errorCode);
  return 0;
}


uint8_t W25q64_Oprate_Reg(uint8_t byte)
{
  SPITimeout = SPIT_FLAG_TIMEOUT;

  /* 等待发送缓冲区为空，TXE事件 */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE ) == RESET)
   {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
   }

  /* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
  SPI_I2S_SendData(SPI2, byte);
	 
  SPITimeout = SPIT_FLAG_TIMEOUT;

  /* 等待接收缓冲区非空，RXNE事件 */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE ) == RESET)
   {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(1);
   }

  /* 读取数据寄存器，获取接收缓冲区数据 */
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
	while(Size) //读0-256字节
	{
		*Subff=W25q64_Oprate_Reg(W25Q128_DUMMY);
		Size--;
		Subff++;
	}
}


void SPI_FLASH_WaitForWriteEnd(void)
{
  uint8_t FLASH_Status = 0,Cmd;

  /* 选择 FLASH: CS 低 */
	CS_STATE=0;

  /* 发送 读状态寄存器 命令 */
	Cmd=0x05;
	W25q64_Spi_Transmit(&Cmd,1);
	
	Cmd=0xff;
  SPITimeout = SPIT_FLAG_TIMEOUT;
  /* 若FLASH忙碌，则等待 */
  do
  {
    /* 读取FLASH芯片的状态寄存器 */
//	    W25q64_Spi_Transmit(&SPI1_Handle,&Cmd,1,500);//产生虚拟时钟
      FLASH_Status = W25q64_Oprate_Reg(0xff);//读取寄存器时间比较慢，不使用HAL_SPI_Receive
//	  HAL_SPI_Receive(&SPI1_Handle,&FLASH_Status,1,500);
 
    {
      if((SPITimeout--) == 0) 
      {
        SPI_TIMEOUT_UserCallback(4);
        return;
      }
    } 
  }
  while ((FLASH_Status & 0x01) == SET); /* 正在写入标志 */

  /* 停止信号  FLASH: CS 高 */
	CS_STATE=1;
}


void W25q128_Write_Enable(void)
{
	uint8_t Cmd;
	
	Cmd=W25Q128_WRITE_ENABLE;
  /* 通讯开始：CS低 */
	CS_STATE=0;

  /* 发送写使能命令*/
	W25q64_Spi_Transmit(&Cmd,1);

  /*通讯结束：CS高 */
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

//只要注意发送擦除地址时高位在前即可。调用扇区擦除指令时注意输入的地址要对齐到 4KB。
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

//只要注意发送擦除地址时高位在前即可。调用擦除指令时注意输入的地址要对齐到 64KB。
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

//写一页
void W25q128_Write_Page(uint32_t Write_Addr,uint8_t *TxBuff,uint32_t Write_Num)
{
	uint8_t Cmd,Cmd_Value=0;
	
  W25q128_Write_Enable();
  SPI_FLASH_WaitForWriteEnd();//等待写结束
	
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
	
	SPI_FLASH_WaitForWriteEnd();//等待写结束
}

//写不定量的数据
void W25q128_Write_LotData(uint32_t Write_Addr,uint8_t *TxBuff,uint32_t Write_Num)
{
	uint8_t Cmd=0,i;
  uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
	
	Addr=Write_Addr%W25Q128_WRITE_MAX;//初始地址是否与页面地址对其
	
	count = W25Q128_WRITE_MAX - Addr;//里下一页还有多少字节
	
	NumOfPage=Write_Num/W25Q128_WRITE_MAX;//写的页数
	NumOfSingle=Write_Num%W25Q128_WRITE_MAX;//写完页后剩余的字节
	
//初始地址与页面对其	
	if(Addr==0)
	{	
		if(NumOfPage==0)//写不满一页
		{
		   W25q128_Write_Page(Write_Addr,TxBuff,Write_Num);
		}
		else//超出一页
		{

			for(i=0;i<NumOfPage;i++)//先把页写完
			{
				W25q128_Write_Page(Write_Addr,&TxBuff[i*W25Q128_WRITE_MAX],W25Q128_WRITE_MAX);
				Write_Addr+=W25Q128_WRITE_MAX;
			}
			 //写剩下的字节
			 W25q128_Write_Page(Write_Addr,&TxBuff[i*W25Q128_WRITE_MAX],NumOfSingle);		
		}

	}
	else//初始地址与页面不对其
	{
		if(NumOfPage==0)//写不满一页
		{
			if(count>=Write_Num)//不超出当前页
			{
				W25q128_Write_Page(Write_Addr,TxBuff,Write_Num);
			}
			else//超出当前页
			{
				W25q128_Write_Page(Write_Addr,TxBuff,count);//把当前页写完
				Write_Addr+=count;
				
				temp=Write_Num-count;//剩余字节
				W25q128_Write_Page(Write_Addr,&TxBuff[count],temp);		
			}		
		
		}
		else 
		{
			Write_Num-=count;//将数据处理成第一种情况
			
			//重新计算
			NumOfPage=Write_Num/W25Q128_WRITE_MAX;
			NumOfSingle=Write_Num%W25Q128_WRITE_MAX;
			
			//写完当前页
			W25q128_Write_Page(Write_Addr,TxBuff,count);
			Write_Addr+=count;//地址偏移到下一页
			
			for(i=0;i<NumOfPage;i++)
			{
				W25q128_Write_Page(Write_Addr,&TxBuff[count+i*W25Q128_WRITE_MAX],W25Q128_WRITE_MAX);
				Write_Addr+=W25Q128_WRITE_MAX;
			}
				W25q128_Write_Page(Write_Addr,&TxBuff[count+i*W25Q128_WRITE_MAX],NumOfSingle);
		}

	}
	

}
//发送了指令编码及要读的起始地址后，FLASH 芯片就会按地址递增的方式返回存储矩阵的内容， 读取的数据量没有限制，只要没有停止通讯，FLASH 芯片就会一直返回数据

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