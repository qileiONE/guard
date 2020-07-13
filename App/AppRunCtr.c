
#define __APPRUN_CTR_C

#include "includes.h"
#include "eeprom.h"
#include "rtc.h"
#include "30205.h"
#include "oled.h"
#include "Ble.h"
#include "RN5T618.H"
#include "adc.h"


volatile    UartCtrStr	PcCmdBuf;

#define	MeasureFilterLen		15

uint32_t display_timeout;

uint8_t last_sendtime;

uint8_t nfccard_flag = 0;

uint8_t measure_flag;
uint16_t measure_pos;
uint16_t measure_buf[60];
uint32_t mensure_timeout;

uint16_t CurrentTemp;
uint8_t  CurrentPoint=10;

uint32_t lastRtcTime;

uint8_t ChargeStatus;
uint32_t BatteryV;
uint16_t BatteryCap;

char displayBuf[50];

uint16_t bleCheckCnt;

float tmpCaliK;
float tmpCaliB;


void  CmdSetDeal(char *buf);
void  UserKeyScan(void);
void  OledDisplay(void);
void  TemperatureGet(void);
void  Error_Handler(void);


void AppProgramme(void)
{
    uint8_t sta;
    ADC1_Init();
    max_init ();
    BleInit();
    OLED_Init();			//初始化OLED  
	OLED_Clear(); 
    getTempCali(&tmpCaliK,&tmpCaliB);
    
	if(!RCC_GetFlagStatus(RCC_FLAG_IWDGRST))	
	{
		OLED_ShowString(21,0,"Ble Watch");
		delay_ms(700);
		delay_ms(500);
        MOTOR_ON;
        delay_ms(100);
        MOTOR_OFF;
		
		secondTick = RTC_GetCounter();
		display_timeout = secondTick + 3;
		OLED_Clear();
	}
	IWDG_Init();
	while(1)
	{  		
		secondTick = RTC_GetCounter();
		if(lastRtcTime != secondTick)//if(alarm_flag == 1)
        {
			lastRtcTime = secondTick;
            bleCheckCnt++;
            //alarm_flag = 0;
            RTC_Get();
//            PrintMsg("%d 年%d 月%d 日 %d:%02d:%02d\r\n",calendar.w_year,calendar.w_month,calendar.w_date,
//                                                    calendar.hour,calendar.min,calendar.sec); 
			TemperatureGet();
            sta = BatteryChargeState();
            if(ChargeStatus != sta)
            {
                ChargeStatus = sta;
                display_timeout = secondTick + 3;
            }
                //BatteryV = GET_BATVOLTAGE();
			IWDG_ReloadCounter();
        }
        
//        if((secondTick % 10)==0)
//        {
//            if(BleSentMaster == BleSendNONE)
//            {
//                BleSentMaster = BleSendAPP;
//                BleSentTemp = BleSendNONE;
//            }
//            else
//            {
//                BleSentTemp = BleSendAPP;
//            }
//        }            
		UserKeyScan();
		OledDisplay();
		BleSendTask();
		if(BleCanSleep())
		{
//            LED_LOW;
//            delay_ms(20);
//            LED_HI;
            
			EnterStopMode();
		}			
		
	}
}

uint8_t key_press_flag;
uint32_t key_press_second;
void UserKeyScan(void)
{
	if(KeyRead() == 0)
	{
		if(key_press_flag == 0)
		{			
			delay_ms(30);
			if(KeyRead() == 0)
			{
				key_press_flag = 1;
				key_press_second = secondTick;
				display_timeout = secondTick + 3;
				BleSendOut((uint8_t *)"AT+STATU",8);
			}
		}
		else
		{
			display_timeout = secondTick + 3;
			if((secondTick - key_press_second) >= 5)//长按时间达到，可以显示提示
			{
				CC_RESET_LOW();
				CC_OFF();

				//CC_UART_WAKE_DIS(); 
				delay_ms(10);
				CC_RESET_HIGH();
				CC_EN();
				delay_ms(20);
				
				at_send_cmd("AT+ADVON","ON",200,3);
				nfccard_flag = 1;
				saveNFCCardFlag();
//                if(BleSentMaster == BleSendNONE)
//				{
//					BleSentMaster = BleSendCARD;
//                    BleSentTemp = BleSendNONE;
//				}
//                else
//                {
//                    BleSentTemp = BleSendCARD;
//                }
			}
		}
	}
	else
	{
		if(key_press_flag == 1)
		{
			if((secondTick - key_press_second) >= 5)// long press up to 3s
			{			

					nfccard_flag = 1;
				saveNFCCardFlag();
//                if(BleSentMaster == BleSendNONE)
//				{
//					BleSentMaster = BleSendCARD;
//                    BleSentTemp = BleSendNONE;
//				}
//                else
//                {
//                    BleSentTemp = BleSendCARD;
//                }
			}
		}
		key_press_flag = 0;
	}
}
uint8_t oled_dis_flag;
uint32_t oled_dis_time;
void OledDisplay(void)
{
    uint32_t tmp32;
    float tmpf;
	if(display_timeout < secondTick)
	{
		if(oled_dis_flag == 1)
		{
			oled_dis_flag = 0;
			OLED_Display_Off();
		}
	}
	else
	{
		if(oled_dis_time != secondTick)// display change every second
		{
			oled_dis_time = secondTick;
			tmpf = GetTemperature1();
            CurrentTemp =tmpf * 10;
            if(tempDevice == 1)
						{
							sprintf(displayBuf,"%02d:%02d n",calendar.hour,calendar.min);
						}
						else 
						{
							sprintf(displayBuf,"%02d:%02d m",calendar.hour,calendar.min);
						}
            font_size = 16;
            OLED_ShowString(10,0,displayBuf);
            sprintf(displayBuf,"%.1f`C",tmpf);
            font_size = 32;
						OLED_ShowString(18,3,displayBuf);
//						if(tempDevice == 1)
//						{
//							OLED_ShowString(10,5,"N");
//						}
//						else 
//						{
//							OLED_ShowString(10,5,"M");
//						}
            if(ChargeStatus == 1)
            {
                OLED_DrawBattery(90,0,0xff);
            }
            else
            {
                BatteryV = GET_BATVOLTAGE();
                tmp32 = (BatteryV - 3600)*100/(4200-3600);
						//	BatteryCap = GET_BATCAP() ;
						//	tmp32 = BatteryCap;
                OLED_DrawBattery(90,0,tmp32);
							
						//	BatteryCap = GET_BATCAP();
            }
//            if(BleSentMaster == BleSendCARD)
						if(nfccard_flag == 1)
            {
                OLED_DrawCard(70,0,1);
            }
            else
            {
                OLED_DrawCard(70,0,0);
            }
			if(oled_dis_flag == 0)
			{
				oled_dis_flag = 1;
				OLED_Display_On();
			}
            
		}
	}
}
void TemperatureGet(void)
{
	uint8_t err;
	uint8_t  num;
	uint16_t tmp16;
	
	if(measure_flag == 0)
	{
		if((calendar.min == 59)||(calendar.min == 14)||(calendar.min == 29)||(calendar.min == 44))
		{
			if(calendar.sec >= 40)
			{
				measure_flag = 1;
				measure_pos = 0;
				mensure_timeout = secondTick + 120;
			}
		}
	}
	else
	{
		if(measure_pos < MeasureFilterLen)
		{
			tmp16 = GetTemperature1() * 100;
			if((tmp16%10)>=5)
			{
				tmp16 = tmp16/10 + 1;
			}
			else 
			{
				tmp16 = tmp16/10;
			}
			measure_buf[measure_pos++] = tmp16;
		}
		if(measure_pos >= MeasureFilterLen)
		{
			err = filterDltaNoMinMax(measure_buf,MeasureFilterLen,4,&CurrentTemp);
			if(err == 0)
			{
				measure_flag = 0;
				measure_pos = 0;
				num = calendar.hour * 4;
				if(calendar.min < 5)
					num = num + 1;
				else if(calendar.min < 20)
					num = num + 2;
				else if(calendar.min < 35)
					num = num + 3;
				else
					num = num + 4;
                CurrentPoint = num;
				saveTempPoint(calendar.w_year,calendar.w_month,calendar.w_date,num,CurrentTemp);
//				if(BleSentMaster == BleSendNONE)
//				{
//					BleSentMaster = BleSendAPP;
//                    BleSentTemp = BleSendNONE;
//				}
//                else
//                {
//                    BleSentTemp = BleSendAPP;
//                }
			}
			else
			{
				measure_pos = 0;
			}
		}
		if(secondTick > mensure_timeout)
		{
			measure_flag = 0;
			measure_pos = 0;
		}
	}
}




void Error_Handler(void)
{
	LED_RUN_ON();
	delay_ms(500);
	LED_RUN_OFF();
	delay_ms(500);	
}



