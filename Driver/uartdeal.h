
#ifndef __UARTDEAL_H 
#define __UARTDEAL_H

#include "stdint.h"
#include "stdio.h"
#include "Ctypedef.h"

#define	_DEBUG_msg
#ifdef	_DEBUG_msg
	#define	PrintMsg 		printf
#else
	#define PrintMsg(x,...)
#endif

#define	UART_INTV		8	//串口数据间隔5ms默认一帧结束

//Modbus   响应主机的错误
#define ErrOk			0
#define	ErrFuncode		1
#define ErrRegAdd		2
#define ErrRegData		3
#define ErrDFault		4
#define ErrDoing		5
#define ErrBusy			6
#define ErrSaveCheck	8
//Modbus 串口数据协议格式错误 不需回发
#define	ErrLenth		21
#define ErrCrc			22




void CommDeal(void);
void NFCDeal(void);
void Usart1DmaSend(uint8_t *pdata,uint16_t len);
void Usart2DmaSend(uint8_t *pdata,uint16_t len);
void Usart3DmaSend(uint8_t *pdata,uint16_t len);
void Uart4DmaSend(uint8_t *pdata,uint16_t len);
UINT8 UartSendNoDMA(uint8_t *pdata,UINT16 len);
UINT8 Uart2SendNoDMA(uint8_t *pdata,UINT16 len);

#endif

