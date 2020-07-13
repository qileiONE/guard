
#ifndef __BLE_H
#define __BLE_H

#include "stdint.h"
#include "Ctypedef.h"

#define     CC_UART_WAKE_EN()       GPIOB->BRR  = GPIO_Pin_5
#define     CC_UART_WAKE_DIS()      GPIOB->BSRR  = GPIO_Pin_5

#define     CC_RESET_LOW()          GPIOB->BRR  = GPIO_Pin_6
#define     CC_RESET_HIGH()         GPIOB->BSRR  = GPIO_Pin_6

#define     CC_EN()                 GPIOB->BRR  = GPIO_Pin_7
#define     CC_OFF()                GPIOB->BSRR  = GPIO_Pin_7

#define     CC_DATA_IN              (GPIOB->IDR & GPIO_Pin_4)

#define     CC_LINK                 (1)


#define	BleSendNONE		0
#define	BleSendAPP		1
#define	BleSendCARD		2


#define     APP_TEST            0

#define		UART_MAX_LEN		100
typedef struct {

	uint8_t	receFlag; 	
	uint8_t	recePos;
	uint8_t	receTime;
	uint8_t	data[UART_MAX_LEN];
}UartCtrStr;

#define BLE_SEND_IDLE       0


extern  uint8_t BleSentMaster;
extern  uint8_t BleSentTemp;

extern uint8_t nfccard_flag;
extern uint8_t nfc_recv_flag;

void BleInit(void);
void BleSendTask(void);
uint8_t BleCanSleep(void);
uint8_t at_send_cmd(char *cmd,char *res,uint32_t timeOut,uint8_t retime);
void BleSendOut(uint8_t *pdata,UINT16 len);
#endif

