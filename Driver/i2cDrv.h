
#ifndef I2C_DRV_H
#define I2C_DRV_H

#include "stm32f10x.h"

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE   
#define FALSE   0
#endif

typedef struct{
		GPIO_TypeDef 	*SCLport;
		GPIO_TypeDef 	*SDAport;
		uint16_t		SCLpin;
		uint16_t		SDApin;
		uint32_t		SCLclk;
		uint32_t		SDAclk;
}I2C_STR;

void I2C_GPIO_Config(const I2C_STR *I2cStr);
//bool I2C_Start(const I2C_STR *I2cStr);
//void I2C_Stop(const I2C_STR *I2cStr);
//void I2C_Ack(const I2C_STR *I2cStr);
//void I2C_NoAck(const I2C_STR *I2cStr);
//bool I2C_WaitAck(const I2C_STR *I2cStr);	 //返回为:=1有ACK,=0无ACK
//void I2C_SendByte(const I2C_STR *I2cStr,uint8_t SendByte); //数据从高位到低位//
//uint8_t I2C_ReceiveByte(const I2C_STR *I2cStr);  //数据从高位到低位//
u8 I2C_WriteHaveAdd(const I2C_STR *I2cStr,uint8_t *SendByte,uint8_t len,uint8_t WriteAddress, uint8_t DeviceAddress);
u8 I2C_WriteNoAdd(const I2C_STR *I2cStr,uint8_t *SendByte,uint8_t len,uint8_t DeviceAddress);
u8 I2C_ReadHaveAdd(const I2C_STR *I2cStr,uint8_t* pBuffer,uint8_t len,uint8_t ReadAddress,uint8_t DeviceAddress);
u8 I2C_ReadNoAdd(const I2C_STR *I2cStr,uint8_t* pBuffer,uint8_t len,uint8_t DeviceAddress);


#endif
