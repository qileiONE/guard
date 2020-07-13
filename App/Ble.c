
#include "string.h"
#include "stm32f10x.h"
#include "Ble.h"
#include "delay.h"
#include "SetupTimer.h"
#include "uartdeal.h"
#include "Crc16.h"
#include "eeprom.h"
#include "rtc.h"
#include "bsp_init.h"
#include "30205.h"
#include "tool.h"

//#define     CC_UART_WAKE_EN()       GPIOB->BRR  = GPIO_Pin_5
//#define     CC_UART_WAKE_DIS()      GPIOB->BSRR  = GPIO_Pin_5

//#define     CC_RESET_LOW()          GPIOB->BRR  = GPIO_Pin_6
//#define     CC_RESET_HIGH()         GPIOB->BSRR  = GPIO_Pin_6

//#define     CC_EN()                 GPIOB->BRR  = GPIO_Pin_7
//#define     CC_OFF()                GPIOB->BSRR  = GPIO_Pin_7

//#define     CC_DATA_IN              (GPIOB->IDR & GPIO_Pin_4)

//#define     CC_LINK                 (1)


uint8_t BleSentMaster;
uint8_t BleSentTemp;
uint8_t ble_state;

uint8_t ble_recv_flag;
uint8_t nfc_recv_flag;

uint8_t bleWaitLink;
uint8_t bleLinkOk;

uint8_t bleMac[6]= { 0x60 , 0x64, 0x05, 0x8D, 0x3A, 0xB2};
uint8_t bleSendBuf[300];
uint8_t bleRecvBuf[300];
uint8_t bleRecvSize;

uint8_t bleReceOk;
uint8_t bleMasterType;

uint8_t  bleEnflag;
uint32_t bleShutOffTime;

extern   uint16_t CurrentTemp;
extern   uint8_t  CurrentPoint;
extern   uint16_t bleCheckCnt;

extern   float tmpCaliK;
extern   float tmpCaliB;

void BleRx_deal(u8 *pdata,u16 len);
void NFCRx_deal(u8 *pdata,u16 len);
void UpdateTimeControl(uint32_t rtc);
void BLESendCurrentTemp(uint8_t flag);
void BLESendData(uint8_t *pdata,uint16_t len);
void BLESendReply(uint8_t date,uint8_t point,uint8_t num);
void BLESendToCard(void);
void NFCSendToCard(void);
void BLESendRece(uint8_t cmd,uint8_t status);
void CommDeal(void);
void NFCDeal(void);
//void BleSendOut(uint8_t *pdata,UINT16 len);
//uint8_t at_send_cmd(char *cmd,char *res,uint32_t timeOut,uint8_t retime);

volatile UartCtrStr	UartRBuf_Uart1 = { 0,0,0,
								{0xa5,0x5a,0x06,0x00,0x01,0x00,0x00,0x00,0x00,0x06,0xff}};
volatile UartCtrStr	UartRBuf_Uart2 = { 0,0,0,
								{0x02,0x00,0x04,0x02,0x21,0xC8,0xC0,0x2B,0x03}};

								
void BleInit(void)
{
	uint8_t i;
	char buf[3];
	
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB , ENABLE );
    
    //PB5  cc uart wake
    //PB6  cc reset
    //PB7  cc en
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;             
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
  	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;	
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
    //PA4  cc uart int
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;             
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
  	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;	
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    //uart init  is in bspinit.c
    
    CC_RESET_LOW();
    CC_OFF();
    
    //CC_UART_WAKE_DIS(); 
    delay_ms(10);
    CC_RESET_HIGH();
	CC_EN();
    delay_ms(20);
	TIMx_Enable(TIM2); 
    //BleSendOut((uint8_t *)"AT+ADVON",8);
    at_send_cmd("AT+ADVON","ON",200,3);
    at_send_cmd("AT+NAME=BLE123","OK",200,3);
    if(at_send_cmd("AT+MAC?","+MAC",200,3) == 0)//get mac
    {
		for(i=0;i<bleRecvSize;i++)
		{
			if(bleRecvBuf[i] == ':')
			{
				break;
			}
		}
		if((i<bleRecvSize)&&(bleRecvSize - i) >= 12)
		{
			buf[0] = bleRecvBuf[i+1];
			buf[1] = bleRecvBuf[i+2];
			buf[2] = 0;
			bleMac[0] = StrToHex(buf);
			buf[0] = bleRecvBuf[i+3];
			buf[1] = bleRecvBuf[i+4];
			bleMac[1] = StrToHex(buf);
			buf[0] = bleRecvBuf[i+5];
			buf[1] = bleRecvBuf[i+6];
			bleMac[2] = StrToHex(buf);
			buf[0] = bleRecvBuf[i+7];
			buf[1] = bleRecvBuf[i+8];
			bleMac[3] = StrToHex(buf);
			buf[0] = bleRecvBuf[i+9];
			buf[1] = bleRecvBuf[i+10];
			bleMac[4] = StrToHex(buf);
			buf[0] = bleRecvBuf[i+11];
			buf[1] = bleRecvBuf[i+12];
			bleMac[5] = StrToHex(buf);
		}
    }
    
    BleSentMaster = BleSendNONE;
    BleSentTemp = BleSendNONE;
    ble_state = BLE_SEND_IDLE;
	
	bleEnflag = 1;
	bleShutOffTime = 0;
}


void CommDeal(void)
{

	if(UartRBuf_Uart1.receFlag != 0)
	{ 
		BleRx_deal((uint8_t *)UartRBuf_Uart1.data,UartRBuf_Uart1.recePos);
    
        UartRBuf_Uart1.recePos = 0;
        UartRBuf_Uart1.receFlag = 0;
	}
}

void NFCDeal(void)
{
	if(UartRBuf_Uart2.receFlag != 0)
	{ 
			NFCRx_deal((uint8_t *)UartRBuf_Uart2.data,UartRBuf_Uart2.recePos);
        UartRBuf_Uart2.recePos = 0;
        UartRBuf_Uart2.receFlag = 0;
				nfccard_flag = 0;
				saveNFCCardFlag();
			MOTOR_ON;
        delay_ms(100);
        MOTOR_OFF;
	}
}

void CommClear(void)
{
	UartRBuf_Uart1.recePos = 0;
  UartRBuf_Uart1.receFlag = 0;
}

void NFCClear(void)
{
	UartRBuf_Uart2.recePos = 0;
  UartRBuf_Uart2.receFlag = 0;
}

uint8_t BleCanSleep(void)
{
    if((ble_state == BLE_SEND_IDLE)||(ble_state == 30))
        return 1;
    else 
        return 0;
}

void BleSendTask(void)
{
    static uint16_t time_cnt = 0;
    static uint16_t err_cnt = 0;
    static uint8_t  err_master = 0;
	static uint8_t	idle_check_cnt=0;
    static uint8_t  waitforcard=0;
    
    switch(ble_state)
    {
        case BLE_SEND_IDLE:
			
				
 			if(err_cnt >= 10)//(bleEnflag == 0)
			{
                err_cnt = 0;
//				if(secondTick >= bleShutOffTime)
				{
                    CC_RESET_LOW();
                    CC_OFF();
                    CC_UART_WAKE_DIS(); 
                    delay_ms(10);  
                    CC_EN();                    
                    CC_RESET_HIGH();
                    delay_ms(10);
					at_send_cmd("AT+ADVON","ON",200,3);
					bleEnflag = 1;
				}
			}
            else if(bleCheckCnt > 30)//if(BleSentMaster != BleSendNONE)
            {
                bleCheckCnt = 0;
                ble_state = 2;
                time_cnt = 0;
                err_cnt = 0;
                err_master = 0;
                bleLinkOk = 0;
                bleWaitLink = 1;
                waitforcard = 0;
                CC_EN();
                //CC_UART_WAKE_EN();
                TIMx_Enable(TIM2); 
                CommClear();                
            }
			else
			{
                #if(APP_TEST)
                ble_state = 50;
                TIMx_Enable(TIM2);
                CC_EN();
                #else                
				idle_check_cnt++;
				if(ble_recv_flag == 1)//if(idle_check_cnt == 2)//if(CC_DATA_IN == 0)
				{
          ble_recv_flag = 0;
					CommClear();
					CC_EN();
					//CC_UART_WAKE_EN();
					TIMx_Enable(TIM2);
					ble_state = 50;
					bleReceOk = 0;
					time_cnt = 0;
					bleMasterType = BleSendNONE;
				}
//				else if(idle_check_cnt == 2)
//				{
//					CC_EN();
//				}
//				else if(idle_check_cnt > 3)
//				{
//					idle_check_cnt = 0;
//					CC_OFF();
//					CC_UART_WAKE_DIS();
//				}
                #endif
			}
            break;
        case 1://use io get link status
            if(CC_LINK == 0)
            {
                bleLinkOk = 1;
                ble_state = 10;
                time_cnt = 0;
            }
            else
            {
                delay_ms(10); 
                time_cnt++;
                if(time_cnt > 50)
                    ble_state = 99;
            }            
            break;
        case 2:            
            if(err_cnt < 10)
            {
                delay_ms(5);
                time_cnt++;
                if(time_cnt >= 10)
                {
                    bleLinkOk = 0;
                    bleWaitLink = 1;
                    ble_state = 3;
                    time_cnt = 0;
                    //delay_ms(100);
                    BleSendOut((uint8_t *)"AT+STATU",8);
                    err_cnt++;
                }
            }
            else//等待连接失败20次
            {
                ble_state = 0;
//                if(BleSentMaster == BleSendCARD)//只要广播开,可以等待打卡机60秒
//                    ble_state = 30;  
//                else
//                    ble_state = 99;
            }
            
            break;
        case 3://use at get link status
            if(bleLinkOk == 1)
            {
                ble_state = 0;//ble_state = 10;
                err_cnt = 0;
                time_cnt = 0;
            }
            else if(bleLinkOk == 99)
            {
                bleWaitLink = 0;
                BleSendOut((uint8_t *)"AT+ADVON",8);
                //delay_ms(100);
                time_cnt = 0;
                ble_state = 2;
            }
            else if(bleLinkOk == 98)
            {
                //delay_ms(100);
                time_cnt = 0;
                err_cnt = 0;
                ble_state = 0;//ble_state = 2;
            }
            else
            {
                delay_ms(10); 
                time_cnt++;
                if(time_cnt > 50)
                {
                    ble_state = 2;
                    time_cnt = 0;
                }
            }
            break;
        case 10://link ok
            bleWaitLink = 0;
//            if(BleSentMaster == BleSendCARD)
//            {
//            }
//            else
//            {
//            }
            if(err_cnt < 3)
            {
                err_cnt++;
                ble_state = 11;
                bleReceOk = 0;
                time_cnt = 0;
                bleMasterType = BleSendNONE;
                BLESendCurrentTemp(0);
            }
            else
            {
                time_cnt = 0;
                if(BleSentMaster == BleSendCARD)//只要广播开,连接的不是APP,都可以等待打卡机60秒
                    ble_state = 30;  
                else
                    ble_state = 99;
            }
            break;
        case 11:
            if(bleReceOk == 1)
            {
				err_cnt = 0;
                time_cnt = 0;
                if(BleSentMaster == BleSendCARD)
                {					
					if(bleMasterType == BleSendCARD)
					{
						ble_state = 30;
					}
					else
					{
						ble_state = 20;
					}
                }
                else if(BleSentMaster == BleSendAPP)//send to app ok
                {
					if(bleMasterType == BleSendAPP)
					{
						if(BleSentTemp == BleSendCARD)
						{
							BleSentMaster= BleSendCARD;
							BleSentTemp = BleSendNONE;
							ble_state = 20;
						}
						else//exit
						{
							BleSentMaster = BleSendNONE;
							BleSentTemp = BleSendNONE;
							ble_state = BLE_SEND_IDLE;
							idle_check_cnt = 0;
//							CC_OFF();
							CC_UART_WAKE_DIS(); 
						}
					}
					else
					{
						if(BleSentTemp == BleSendCARD)
						{
							BleSentMaster= BleSendCARD;
							BleSentTemp = BleSendAPP;
							ble_state = 30;
						}
						else
							ble_state = 20;
					}
                }
				else
					ble_state = 20;
            }
            else
            {
                delay_ms(10); 
                time_cnt++;
                if(time_cnt > 50)
                {
                    time_cnt = 0;
                    ble_state = 10;
                }
            }
            break;
        case 20://disconnect app link
            err_master++;
            if(err_master <4)
            {
                delay_ms(100);
				BLESendCurrentTemp(1);
				delay_ms(10);
                CC_OFF();
                delay_ms(2);
                CC_EN();
                time_cnt = 0;
                ble_state = 2;
            }
            else
            {
                time_cnt = 0;
                if(BleSentMaster == BleSendCARD)//只要广播开,连接的不是APP,都可以等待打卡机60秒
                    ble_state = 30;  
                else
                    ble_state = 99;
            }
            break;
        case 30://等待打卡,可休眠
            waitforcard++;
            if(waitforcard > 15)
            {
                waitforcard = 0;
                ble_state = 99;
            }
            break;
        case 31://master is card ,send data to card
//            if(err_cnt < 3)
            {
//                err_cnt++;
                time_cnt = 0;
                bleReceOk = 0;
                bleMasterType = BleSendNONE;
                ble_state = 32;
                //BLESendToCard();
            }
//            else
//                ble_state = 99;
            break;
        case 32:
            if((bleReceOk == 1)&&(bleMasterType == BleSendCARD))//ok exit
            {
				err_cnt = 0;

				at_send_cmd("AT+ADVOFF","OFF",200,3);
				bleEnflag = 0;
				bleShutOffTime = secondTick + 5;
                MOTOR_ON;
                delay_ms(100);
                MOTOR_OFF;
//				if(BleSentTemp == BleSendAPP)
//				{
//					BleSentMaster = BleSendAPP;
//					BleSentTemp = BleSendNONE;
//					ble_state = 20;
//				}
//				else
//				{
					BleSentMaster = BleSendNONE;
					BleSentTemp = BleSendNONE;
					ble_state = BLE_SEND_IDLE;
					idle_check_cnt = 0;
//					CC_OFF();
					CC_UART_WAKE_DIS(); 
//				}
				
            }
            else
            {
                delay_ms(10); 
                time_cnt++;
                if(time_cnt > 50*6)
                {
                    ble_state = 30;
                    waitforcard = 12;//wait for 35-30 second to card master
                }
            }
            break;
		case 50:// deal with app senddata,not update to app
            #if(!APP_TEST)
			if(bleReceOk == 1)
			{
				bleReceOk = 0;
				time_cnt = 50;
				bleMasterType = BleSendNONE;
				//ble_state = BLE_SEND_IDLE;
				idle_check_cnt = 0;//休眠等待1S，看是否还有数据
				CC_UART_WAKE_DIS();
			}
			else
            {
                delay_ms(10); 
                time_cnt++;
                if(time_cnt > 100)
				{
                    ble_state = BLE_SEND_IDLE;
					idle_check_cnt = 0;
//					CC_OFF();
					CC_UART_WAKE_DIS(); 
				}
            }
            #endif
			break;
        case 99://error status
            CC_RESET_LOW();
            CC_OFF();
            CC_UART_WAKE_DIS(); 
            delay_ms(1);            
            CC_RESET_HIGH();
        
            BleSentMaster = BleSendNONE;
            BleSentTemp = BleSendNONE;
            ble_state = BLE_SEND_IDLE;
			idle_check_cnt = 0;
            break;
        default:
            BleSentMaster = BleSendNONE;
            BleSentTemp = BleSendNONE;
            ble_state = BLE_SEND_IDLE;
			idle_check_cnt = 0;
            CC_OFF();
            CC_UART_WAKE_DIS(); 
            break;
    }
	CommDeal();
		NFCDeal();
}

void NFCRx_deal(u8 *pdata,u16 len)
{
	*(pdata + len) = 0;
//	crc = CRC16(pdata,len);
//  if(crc != 0)
//      ;//return;
		NFCSendToCard();
}

void BleRx_deal(u8 *pdata,u16 len)
{
    uint16_t crc;
    uint16_t y;
    uint8_t m,d,hour,min,sec;
    uint32_t rtc;
    int16_t tmp16;
    uint8_t buf[2];
    
    *(pdata + len) = 0;
    
    if((bleWaitLink == 1)&&(strstr((char *)pdata,"CONNECT")))
    {
        bleLinkOk = 1;
    }
    else if((bleWaitLink == 1)&&(strstr((char *)pdata,"OFF")))
    {
        bleLinkOk = 99;//广播未开启,重新开启
    }
    else if((bleWaitLink == 1)&&(strstr((char *)pdata,"ON")))
    {
        bleLinkOk = 98;//广播未开启,重新开启
    }
    else
    {
        crc = CRC16(pdata,len);
        if(crc != 0)
            ;//return;
        
        if(pdata[0] == 0x01)
        {
            bleMasterType = BleSendAPP;
            bleReceOk = 1;
            
            if((pdata[1] == 0x10)&&(len > 10)) 
            {
                BLESendReply(pdata[2],pdata[3],pdata[4]);
                y= ((uint16_t)pdata[5]<<8) + pdata[6];
                m = pdata[7];
                d = pdata[8];
                hour = pdata[9];
                min = pdata[10];
                sec = pdata[11];
                if((y > 2010)&&(y < 2050)&&(m>0)&&(m<13)&&(d>0)&&(d<32)&&(hour < 24)&&(min<60)&&(sec<60))
                {
                    rtc = DateTimeToSecond(y,m,d,hour,min,sec);
                    if((rtc - secondTick) > 15)
                    {
						rtc = secondTick;
                        RTC_Set(y,m,d,hour,min,sec);
						secondTick = RTC_GetCounter();
						UpdateTimeControl(rtc);
//						RTC_WaitForLastTask();
//						RTC_SetAlarm(rtcsecond + 2);
                    }
                }
            }
            else if(pdata[1] == 0x01)
            {
                if(len > 13)
                {
                    buf[0] = *(pdata+3);
                    buf[1] = *(pdata+2);
                    tmp16 = *(int16_t*)buf;
                    if((tmp16 < 200)&&(tmp16 > 50))
                        tmpCaliK = (float)tmp16/100;
                    buf[0] = *(pdata+5);
                    buf[1] = *(pdata+4);
                    tmp16 = *(int16_t*)buf;
                    if((tmp16 < 800)&&(tmp16 > -800))
                        tmpCaliB = (float)tmp16/100;
                    saveTempCali(tmpCaliK,tmpCaliB);
                    BLESendRece(pdata[1],0);
                }
            }
            else if(pdata[1] == 0x40)
            {
                BLESendToCard();
								MOTOR_ON;
								delay_ms(100);
								MOTOR_OFF;
                delay_ms(80);
								nfccard_flag = 0;
//                bleSendBuf[0] = 0x03;
//                bleSendBuf[1] = 0x41;
//                BLESendData(bleSendBuf,2);
                
//                BleSentMaster = BleSendCARD;
//                BleSentTemp = BleSendNONE;
//                if(ble_state == 50)
//                {
//                    ble_state = BLE_SEND_IDLE;
//                }
            }
            else if(pdata[1] == 0x21)
            {
            }
        }
        else if(pdata[0] == 0x02)
        {
//            if(pdata[1] == 0x21)
//            {
//                bleMasterType = BleSendCARD;
//                bleReceOk = 1;
//            }
//            else if(pdata[1] == 0x11)
//            {
//                BleSentMaster = BleSendCARD;
//                BleSentTemp = BleSendNONE;
//                ble_state = 31;
//                delay_ms(40);
//                BLESendToCard();
//            }
        }
        
    }
}
extern	uint32_t display_timeout;
extern	uint32_t key_press_second;
void UpdateTimeControl(uint32_t rtcbase)//rtcbase ,the old rtc base
{
	u32 tmp;
	if(bleShutOffTime >= rtcbase)
	{
		tmp = bleShutOffTime - rtcbase;
		bleShutOffTime = secondTick + tmp;
	}
	else
		bleShutOffTime = 0;
	if(display_timeout >= rtcbase)
	{
		tmp = display_timeout - rtcbase;
		display_timeout = secondTick + tmp;
	}
	else
		rtcbase = 0;
	key_press_second = secondTick;
}
void BLESendData(uint8_t *pdata,uint16_t len)
{
    uint16_t crc;//,i;
    
//    for(i=0;i<len;i++)
//    {
//        bleSendBuf[i] = pdata[i];
//    }
    crc = CRC16(pdata,len);
    pdata[len++] = crc >> 8;
    pdata[len++] = crc & 0xff;
    
    BleSendOut(pdata,len);
    
}
void BLESendCurrentTemp(uint8_t flag)
{
    uint16_t crc;
    uint16_t tmp16;
    
    tmp16 = GetTemperature1()*100;
    if((tmp16%10)>=5)
		{
			tmp16 = tmp16/10 + 1;
		}
		else 
		{
			tmp16 = tmp16/10;
		}
    bleSendBuf[0] = 0x03;
    if(flag == 1)
        bleSendBuf[1] = 0x90;
    else
        bleSendBuf[1] = 0x10;
    bleSendBuf[2] = bleMac[0];
    bleSendBuf[3] = bleMac[1];
    bleSendBuf[4] = bleMac[2];
    bleSendBuf[5] = bleMac[3];
    bleSendBuf[6] = bleMac[4];
    bleSendBuf[7] = bleMac[5];
    bleSendBuf[8] = 0;
    bleSendBuf[9] = 0;//CurrentPoint;
    bleSendBuf[10] = 1;
    bleSendBuf[11] = tmp16 >> 8;
    bleSendBuf[12] = tmp16 & 0xff;
    
    crc = CRC16(bleSendBuf,13);
    
    bleSendBuf[13] = crc >> 8;
    bleSendBuf[14] = crc & 0xff;
    
    BleSendOut(bleSendBuf,15);
}
void BLESendRece(uint8_t cmd,uint8_t status)
{
    uint16_t crc;
    
    bleSendBuf[0] = 0x03;
    bleSendBuf[1] = cmd;
    bleSendBuf[2] = bleMac[0];
    bleSendBuf[3] = bleMac[1];
    bleSendBuf[4] = bleMac[2];
    bleSendBuf[5] = bleMac[3];
    bleSendBuf[6] = bleMac[4];
    bleSendBuf[7] = bleMac[5];
    
    crc = CRC16(bleSendBuf,8);
    
    bleSendBuf[8] = crc >> 8;
    bleSendBuf[9] = crc & 0xff;
    
    BleSendOut(bleSendBuf,10);
}
void BLESendReply(uint8_t date,uint8_t point,uint8_t num)
{
    uint16_t crc;
    uint16_t data;
    
    if((point == 0)&&(num == 0))
    {
        BLESendCurrentTemp(0);
        return;
    }
    data = readTempPoint(calendar.w_year,calendar.w_month,calendar.w_date,point);
    
    bleSendBuf[0] = 0x03;
    if(BleSentMaster == BleSendCARD)
        bleSendBuf[1] = 0x90;
    else
        bleSendBuf[1] = 0x10;
    bleSendBuf[2] = bleMac[0];
    bleSendBuf[3] = bleMac[1];
    bleSendBuf[4] = bleMac[2];
    bleSendBuf[5] = bleMac[3];
    bleSendBuf[6] = bleMac[4];
    bleSendBuf[7] = bleMac[5];
    bleSendBuf[8] = 0;
    bleSendBuf[9] = point;
    bleSendBuf[10] = 1;
    bleSendBuf[11] = data >> 8;
    bleSendBuf[12] = data & 0xff;
    
    crc = CRC16(bleSendBuf,13);
    
    bleSendBuf[13] = crc >> 8;
    bleSendBuf[14] = crc & 0xff;
    
    BleSendOut(bleSendBuf,15);
}
void BLESendToCard(void)
{
    uint8_t i;
    uint16_t crc;
    uint16_t data;
    
    bleSendBuf[0] = 0x03;
    bleSendBuf[1] = 0x40;//0x11;
    bleSendBuf[2] = bleMac[0];
    bleSendBuf[3] = bleMac[1];
    bleSendBuf[4] = bleMac[2];
    bleSendBuf[5] = bleMac[3];
    bleSendBuf[6] = bleMac[4];
    bleSendBuf[7] = bleMac[5];
    bleSendBuf[8] = 0;
    bleSendBuf[9] = 1;
    bleSendBuf[10] = 96;
    
    for(i=0;i<96;i++)
    {
        data = readTempPoint(calendar.w_year,calendar.w_month,calendar.w_date,i+1);
        bleSendBuf[11 + i*2] = data >> 8;
        bleSendBuf[12 + i*2] = data & 0xff;
    }
    
    crc = CRC16(bleSendBuf,11 + 96*2);
    
    bleSendBuf[11 + 96*2] = crc >> 8;
    bleSendBuf[12 + 96*2] = crc & 0xff;
    
    BleSendOut(bleSendBuf,13 + 96*2);
}

void NFCSendToCard(void)
{
    uint8_t i;
    uint16_t crc;
		uint8_t lrc = 0;
    uint16_t data;
    //02 00 cf a2 33
		bleSendBuf[0] = 0x02;
		bleSendBuf[1] = 0x00;
		bleSendBuf[2] = 0xcf;
		bleSendBuf[3] = 0xa2;
		bleSendBuf[4] = 0x33;
	
    bleSendBuf[5] = 0x03;
    bleSendBuf[6] = 0x40;//0x11;
    bleSendBuf[7] = bleMac[0];
    bleSendBuf[8] = bleMac[1];
    bleSendBuf[9] = bleMac[2];
    bleSendBuf[10] = bleMac[3];
    bleSendBuf[11] = bleMac[4];
    bleSendBuf[12] = bleMac[5];
    bleSendBuf[13] = 0;
    bleSendBuf[14] = 1;
    bleSendBuf[15] = 96;
    
    for(i=0;i<96;i++)
    {
        data = readTempPoint(calendar.w_year,calendar.w_month,calendar.w_date,i+1);
        bleSendBuf[16 + i*2] = data >> 8;
        bleSendBuf[17 + i*2] = data & 0xff;
    }
    
    //crc = CRC16(bleSendBuf,11 + 96*2);
		crc = 0x0000;
		
    bleSendBuf[16 + 96*2] = crc >> 8;
    bleSendBuf[17 + 96*2] = crc & 0xff;
  //  lrc = bleSendBuf[0];
		for(i=3;i<210;i++)
		{
			lrc ^= bleSendBuf[i];
		}
		bleSendBuf[18 + 96*2] = lrc;
		bleSendBuf[19 + 96*2] = 0x03;
    Uart2SendNoDMA(bleSendBuf,21 + 96*2);
}

void BleSendOut(uint8_t *pdata,UINT16 len)
{
    CC_UART_WAKE_EN();
    delay_ms(1);
    
    UartSendNoDMA(pdata,len);
    
    delay_ms(2);
    CC_UART_WAKE_DIS();
}

uint8_t at_send_cmd(char *cmd,char *res,uint32_t timeOut,uint8_t retime)
{
	uint8_t i = 0,j;
	uint32_t timeout;
											                         // ?????o′?	
	for(i=0;i<retime;i++)
	{
	  	timeout = 0;
        CommClear();
        
        CC_UART_WAKE_EN();
        delay_ms(1);    
		UartSendNoDMA((unsigned char *)cmd, strlen((const char *)cmd));
        delay_ms(2);
        CC_UART_WAKE_DIS();
        
		if((timeOut == 0)||(res == NULL)) //TCP连接建立成功 处于收发状态时，不在此处循环阻塞。另行接收处理
		{
			return 0;
		}
		
		while(timeout < timeOut)
		{
            delay_ms(10);
			if(UartRBuf_Uart1.receFlag != 0)						                  // è?1?′??ú?óê?íê3é
			{
                UartRBuf_Uart1.data[UartRBuf_Uart1.recePos] = 0;
				if(strstr((const char *)UartRBuf_Uart1.data, res) != NULL)		// è?1??ì?÷μ?1??ü′ê
				{
                    if(UartRBuf_Uart1.recePos > 100)
                        UartRBuf_Uart1.recePos = 100;
					//printf("%s",lte_buf);
                    for(j=0;j<UartRBuf_Uart1.recePos;j++)
					{
                        bleRecvBuf[j] = UartRBuf_Uart1.data[j]; 
					}					
                    bleRecvBuf[j] = 0;
					bleRecvSize = UartRBuf_Uart1.recePos;
					CommClear();
					return 0;
				}
			}			
			timeout += 10;
		}
		CommClear();
  }
	return 1;
}

