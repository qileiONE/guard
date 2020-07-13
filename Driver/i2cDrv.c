
#include "i2cDrv.h"

#define I2C_DELAY_COUNT		200

#define SCL_H(x)		(x->SCLport->BSRR = x->SCLpin)
#define SCL_L(x)		(x->SCLport->BRR  = x->SCLpin) 
#define SDA_H(x)		(x->SDAport->BSRR = x->SDApin)
#define SDA_L(x)		(x->SDAport->BRR  = x->SDApin)

#define SCL_READ(x)		(x->SCLport->IDR  & x->SCLpin)
#define SDA_READ(x)		(x->SDAport->IDR  & x->SDApin)

void I2C_GPIO_Config(const I2C_STR *I2cStr)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(I2cStr->SCLclk|I2cStr->SDAclk, ENABLE); 
  /* Configure I2C1 pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  I2cStr->SCLpin;//SCL
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //��© 
  GPIO_Init(I2cStr->SCLport, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  I2cStr->SDApin;//SDA
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��©
  GPIO_Init(I2cStr->SDAport, &GPIO_InitStructure);
}

void I2C_delay(void)
{
	volatile uint16_t i = I2C_DELAY_COUNT;

	while(i--);
}

u8 I2C_Start(const I2C_STR *I2cStr)
{
	SDA_H(I2cStr);
	SCL_H(I2cStr);
	I2C_delay();
	if(!SCL_READ(I2cStr))return FALSE;	//SCL��Ϊ�͵�ƽ������æ,�˳�
	SDA_L(I2cStr);
	I2C_delay();
	if(SDA_READ(I2cStr)) return FALSE;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
	SCL_L(I2cStr);
	I2C_delay();
	return TRUE;  
}

void I2C_Stop(const I2C_STR *I2cStr)
{
    SDA_L(I2cStr);
	I2C_delay();
	SCL_L(I2cStr);
	I2C_delay();
	SCL_H(I2cStr);
	I2C_delay();
	SDA_H(I2cStr);
	I2C_delay();
}

void I2C_Ack(const I2C_STR *I2cStr)
{	
	SCL_L(I2cStr);
	I2C_delay();
	SDA_L(I2cStr);
	I2C_delay();
	SCL_H(I2cStr);
	I2C_delay();
	SCL_L(I2cStr);
	I2C_delay();
}

void I2C_NoAck(const I2C_STR *I2cStr)
{	
	SCL_L(I2cStr);
	I2C_delay();
	SDA_H(I2cStr);
	I2C_delay();
	SCL_H(I2cStr);
	I2C_delay();
	SCL_L(I2cStr);
	I2C_delay();
}

u8 I2C_WaitAck(const I2C_STR *I2cStr)	 //����Ϊ:=1��ACK,=0��ACK
{
	SCL_L(I2cStr);
	I2C_delay();
	SDA_H(I2cStr);			 
	I2C_delay();
	SCL_H(I2cStr);
	I2C_delay();
	if(SDA_READ(I2cStr))
	{
      SCL_L(I2cStr);
      return FALSE;
	}
	SCL_L(I2cStr);
	return TRUE;
}

void I2C_SendByte(const I2C_STR *I2cStr,uint8_t SendByte) //���ݴӸ�λ����λ//
{
    uint8_t i=8;
    while(i--)
    {
        SCL_L(I2cStr);
        I2C_delay();
        if(SendByte&0x80)
          SDA_H(I2cStr);  
        else 
          SDA_L(I2cStr);   
        SendByte<<=1;
        I2C_delay();
		SCL_H(I2cStr);
        I2C_delay();
        I2C_delay();
    }
    SCL_L(I2cStr);
}

u8 I2C_ReceiveByte(const I2C_STR *I2cStr)  //���ݴӸ�λ����λ//
{ 
	uint8_t i=8;
    uint8_t ReceiveByte=0;

    SDA_H(I2cStr);				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL_L(I2cStr);
      I2C_delay();
      SCL_H(I2cStr);
      I2C_delay();	
      if(SDA_READ(I2cStr))
      {
        ReceiveByte|=0x01;//���λ��1
      }
    }
    SCL_L(I2cStr);
    return ReceiveByte;
}
//�������������ݵļĴ�����ַ��
u8 I2C_WriteHaveAdd(const I2C_STR *I2cStr,u8 *SendByte,u8 len,u8 WriteAddress, u8 DeviceAddress)
{	
//    FM24_WP_L;	
    if(!I2C_Start(I2cStr))return FALSE;
    I2C_SendByte(I2cStr,DeviceAddress & 0xFE);//���ø���ʼ��ַ+������ַ 
    if(!I2C_WaitAck(I2cStr)){I2C_Stop(I2cStr); return FALSE;}
    I2C_SendByte(I2cStr,WriteAddress);   //���õ���ʼ��ַ      
    I2C_WaitAck(I2cStr);
	while(len--)
	{	
	    I2C_SendByte(I2cStr,*SendByte++);
	    I2C_WaitAck(I2cStr);
	}   
    I2C_Stop(I2cStr); 	

//	FM24_WP_H;
    return TRUE;
}
//���������������ݵļĴ�����ַ��ֱ��д����
u8 I2C_WriteNoAdd(const I2C_STR *I2cStr,u8 *SendByte,u8 len,u8 DeviceAddress)
{	
//    FM24_WP_L;	
    if(!I2C_Start(I2cStr))return FALSE;
    I2C_SendByte(I2cStr,DeviceAddress & 0xFE);//���ø���ʼ��ַ+������ַ 
    if(!I2C_WaitAck(I2cStr)){I2C_Stop(I2cStr); return FALSE;}	
    while(len--)
	{	
	    I2C_SendByte(I2cStr,*SendByte++);
	    I2C_WaitAck(I2cStr);
	}   
    I2C_Stop(I2cStr); 	

//	FM24_WP_H;
    return TRUE;
}
//�������������ݵļĴ�����ַ��
u8 I2C_ReadHaveAdd(const I2C_STR *I2cStr,u8* pBuffer,u8 len,u8 ReadAddress,u8 DeviceAddress)
{
    if(!I2C_Start(I2cStr))return FALSE;
    I2C_SendByte(I2cStr,DeviceAddress & 0xFE);//���ø���ʼ��ַ+������ַ 
    if(!I2C_WaitAck(I2cStr)){I2C_Stop(I2cStr); return FALSE;}
    I2C_SendByte(I2cStr,ReadAddress);   //���õ���ʼ��ַ      
    I2C_WaitAck(I2cStr);
	//I2C_Stop(I2cStr);
    I2C_Start(I2cStr);
    I2C_SendByte(I2cStr,DeviceAddress | 0x01);
    I2C_WaitAck(I2cStr);
    while(len--)
    {
      *pBuffer++ = I2C_ReceiveByte(I2cStr);
      if(len == 0) I2C_NoAck(I2cStr);
      else I2C_Ack(I2cStr); 
    }
    I2C_Stop(I2cStr);
    return TRUE;
}
//���������������ݵļĴ�����ַ��ֱ�Ӷ�ȡ
u8 I2C_ReadNoAdd(const I2C_STR *I2cStr,u8* pBuffer,u8 len,u8 DeviceAddress)
{
    if(!I2C_Start(I2cStr))return FALSE;
    I2C_SendByte(I2cStr,DeviceAddress | 0x01);
    I2C_WaitAck(I2cStr);
    while(len--)
    {
      *pBuffer++ = I2C_ReceiveByte(I2cStr);
      if(len == 0) I2C_NoAck(I2cStr);
      else I2C_Ack(I2cStr); 
    }
    I2C_Stop(I2cStr);
    return TRUE;
}
