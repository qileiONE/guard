/*
config list
ldo1	3.3V 	(default)
ldo2	3.3V	(CC2541)
ldo3	3.3V  MAX30101(LED)
ldo4	3.3V 	LEDA(LED SCREEN BACKLIGHT )
ldo5	2.8V 	LCD������Դ
DCDC3	1.8V 	MMA9555L���� ��MAX30112������

*/


#include "i2cDrv.h"
#include "RN5T618.h"

//#include "usart.h"
//#include "FONT_PIC.h"
/***************************************
MMA9555L���磺LDO3(1.8v)+LDO2(2.8v)+LDO1          	1.8V��оƬ�������� 2.8V+3.3V�ǵ�Ƭ�������ݹ���
Һ��������  ��LDO2(2.8v)+LDO3(1.8v)  +   LDO4(����)+LDO1                 Һ������������2.8V+1.8V   ��ʾ����3.3V
�������磺    LDO5			 +	 LDO1        										��������C2640_3.3V				��Ƭ�������ݹ���3.3V
������			��DCDC3      +	 LDO1                         3.5V
*****************************************/
uint8_t system_wakeup_flag=0;

const I2C_STR RN5T618_I2C ={	GPIOB,	//SCL
							GPIOA,	//SDA
							GPIO_Pin_3,
							GPIO_Pin_15,
							RCC_APB2Periph_GPIOB,							
							RCC_APB2Periph_GPIOA
						};

void PowerRN5T618_init(void)
{
    #if(0)
    uint8_t	RN5t618_wr_buf[1];
    #endif

	I2C_GPIO_Config(&RN5T618_I2C);

    SYS_POWER();
//	err = I2C_ReadHaveAdd(&RN5T618_I2C,&Counts,1,5,RN5T618_Add);
//	if(err == FALSE)
//	{
//		return;
//	}
//	I2C_WriteHaveAdd(&RN5T618_I2C,&Counts,1,5,FM24DEVICE_ID);
    
    #if(0)

    RN5t618_wr_buf[0] = 0x01;
    I2C_WriteHaveAdd(&RN5T618_I2C,&RN5t618_wr_buf[0],1,PWRIREN,RN5T618_Add);
	RN5t618_wr_buf[0] = 0x00;
    I2C_WriteHaveAdd(&RN5T618_I2C,&RN5t618_wr_buf[0],1,PWRIRQ,RN5T618_Add);

    //��Դ���� �жϳ�ʼ��
	//1.�����жϴ���ģʽ
	//2.���жϱ�־λ
	RN5t618_wr_buf[0] = 0x00;
    I2C_WriteHaveAdd(&RN5T618_I2C,&RN5t618_wr_buf[0],1,CHGCTRL_IRR,RN5T618_Add);
    I2C_WriteHaveAdd(&RN5T618_I2C,&RN5t618_wr_buf[0],1,CHGSTAT_IRR1,RN5T618_Add);
    I2C_WriteHaveAdd(&RN5T618_I2C,&RN5t618_wr_buf[0],1,CHGSTAT_IRR2,RN5T618_Add);
    I2C_WriteHaveAdd(&RN5T618_I2C,&RN5t618_wr_buf[0],1,CHGERR_IRR,RN5T618_Add);

    //3.�����ж�����λ
	RN5t618_wr_buf[0] = 0x00;
    I2C_WriteHaveAdd(&RN5T618_I2C,&RN5t618_wr_buf[0],1,CHGCTRL_IRFMASK,RN5T618_Add);
    
    //4.���ж�ʹ��
	RN5t618_wr_buf[0] = 0x41;
    I2C_WriteHaveAdd(&RN5T618_I2C,&RN5t618_wr_buf[0],1,INTEN,RN5T618_Add);

	RN5t618_wr_buf[0] = 0x05;//������600ma
    I2C_WriteHaveAdd(&RN5T618_I2C,&RN5t618_wr_buf[0],1,USB_Limit_Current,RN5T618_Add);
    #endif
}


//set mcu voltage 2.8V
void SYS_POWER(void)
{
	uint8_t buffer[3];
	buffer[2] = 0x60;//0x4c-2.8V
	
    //I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[0],1,LDOEN1,RN5T618_Add);

	//buffer[1] = buffer[0]|0x01;
    buffer[1] = 0x01;
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[2],1,LDO1DAC,RN5T618_Add);
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[1],1,LDOEN1,RN5T618_Add);
    
    I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[0],1,LDOEN1,RN5T618_Add);
    
//    I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[0],1,BATSET2,RN5T618_Add);
//    buffer[0] |= 0x30;
//    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[0],1,BATSET2,RN5T618_Add);
    I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[0],1,VSYSSET,RN5T618_Add);
    
    buffer[0] = 0x05;//������600ma
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[0],1,USB_Limit_Current,RN5T618_Add);
    buffer[0] = 0x05;//������600ma
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[0],1,USB_Charge_Current,RN5T618_Add);
    buffer[0] = 0x87;//0xa7;//USB CHARGE
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[0],1,CHGCONTROL,RN5T618_Add);
    
    I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[0],1,USB_Charge_Current,RN5T618_Add);
    I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[0],1,CHGCONTROL,RN5T618_Add);
    
    
}


//��3.3V��Դ LDO2 CC32541
void POWERON_CC2541(void)
{
	uint8_t buffer[3];
	buffer[2] = 0x60;//0x4c;3.3V(CC2541)
	//HAL_I2C_Mem_Read(&hi2c3,RN5T618_Add_RD,PWRONTIMSET,1,&buffer[0],1,100);
	I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[0],1,LDOEN1,RN5T618_Add);

	buffer[1] = buffer[0]|0x02;
    
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[2],1,LDO2DAC,RN5T618_Add);
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[1],1,LDOEN1,RN5T618_Add);
}
//��2.8V��Դ LDO2
//void POWERON_LDO28(void)
//{
//	uint8_t buffer[3];
//	buffer[2] = 0x60;//0x4c;3.3V(CC2541)
//	//HAL_I2C_Mem_Read(&hi2c3,RN5T618_Add_RD,PWRONTIMSET,1,&buffer[0],1,100);
//	
//	HAL_I2C_Mem_Read(&hi2c1,RN5T618_Add_RD,LDOEN1,1,&buffer[0],1,100);
//	buffer[1] = buffer[0]|0x02;
//	HAL_I2C_Mem_Write(&hi2c1,RN5T618_Add_WR,LDO2DAC,1,&buffer[2],1,100);//set voltage = 2.8
//	HAL_I2C_Mem_Write(&hi2c1,RN5T618_Add_WR,LDOEN1,1,&buffer[1],1,100);
//}
//�򿪵�Դ	LDO3 3.3V
void POWERON_MAX30112(void)
{
	uint8_t buffer[3];
	buffer[2] = 0x74;//60;
    I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[0],1,LDOEN1,RN5T618_Add);

	buffer[1] = buffer[0]|0x04;
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[2],1,LDO3DAC,RN5T618_Add);
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[1],1,LDOEN1,RN5T618_Add);
	
}
//��3.3V��Դ LDO4
void POWERON_backlight(void)
{
	uint8_t buffer[3];
	buffer[2] = 0x4C;//60-3.3v 4C-2.8v
	//HAL_I2C_Mem_Read(&hi2c3,RN5T618_Add_RD,PWRONTIMSET,1,&buffer[0],1,100);
	
	I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[0],1,LDOEN1,RN5T618_Add);
	buffer[1] = buffer[0]|0x08;
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[2],1,LDO4DAC,RN5T618_Add);
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[1],1,LDOEN1,RN5T618_Add);
}
//��2.8V��Դ LDO5
void POWERON_LCD(void)
{
	uint8_t buffer[3];
	buffer[2] = 0x4C;//2.8V LCD������Դ �� MMA9555L I2C��ƽת����Դ
	//HAL_I2C_Mem_Read(&hi2c3,RN5T618_Add_RD,PWRONTIMSET,1,&buffer[0],1,100);
	
	I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[0],1,LDOEN1,RN5T618_Add);
    
	buffer[1] = buffer[0]|0x10;
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[2],1,LDO5DAC,RN5T618_Add);
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[1],1,LDOEN1,RN5T618_Add);
}
//��1.8V��Դ���������ã�DCDC3
void POWERON_MMA9555L(void)
{
	uint8_t buffer[1];
	buffer[0] = 0x60;
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[0],1,DC3DAC,RN5T618_Add);
	buffer[0] = 0x03;
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[0],1,DC3CTL,RN5T618_Add);

}
//�ر�2.8V��ԴLDO5
void POWEROFF_LCD(void)
{
	uint8_t buffer[2];	
    I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[0],1,LDOEN1,RN5T618_Add);
	buffer[1] = buffer[0]&0XEF;
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[1],1,LDOEN1,RN5T618_Add);
}
//�ر�1.8V��Դ LDO3
void POWEROFF_MAX30112(void)
{
	uint8_t buffer[2];	
	I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[0],1,LDOEN1,RN5T618_Add);
	buffer[1] = buffer[0]&0XFB;
	I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[1],1,LDOEN1,RN5T618_Add);
}
//�ر�1.8V��Դ LDO4
void POWEROFF_backlight(void)
{
	uint8_t buffer[2];	
	I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[0],1,LDOEN1,RN5T618_Add);
	buffer[1] = buffer[0]&0XF7;
	I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[1],1,LDOEN1,RN5T618_Add);
}
//�ر�1.8V��Դ LDO5//Ӧ�ùر�LDO2
void POWEROFF_CC2541(void)
{
//	uint8_t buffer[2];	
//	HAL_I2C_Mem_Read(&hi2c1,RN5T618_Add_RD,LDOEN1,1,&buffer[0],1,100);
//	buffer[1] = buffer[0]&0XEF;
//	HAL_I2C_Mem_Write(&hi2c1,RN5T618_Add_WR,LDOEN1,1,&buffer[1],1,100);
	uint8_t buffer[2];	
	I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[0],1,LDOEN1,RN5T618_Add);
	buffer[1] = buffer[0]&0XFD;
	I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[1],1,LDOEN1,RN5T618_Add);
}
//�ر�1.8V��Դ DCDC3
void POWEROFF_MMA9555L(void)
{
	uint8_t buffer[1];
	buffer[0] = 0x02;
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[0],1,DC3CTL,RN5T618_Add);
}
//�ػ���ϵͳ�µ�
void SHUTDOWN_SYS(void)
{
	uint8_t buffer[1];
	buffer[0] = 0x01;
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[0],1,SLPCNT,RN5T618_Add);//POWER OFF	
}
//��ȡ��ص�ѹ
float GET_BATVOLTAGE(void)
{
    float tmpf;
    
	uint8_t buffer[3];
	buffer[2] = 0x01;
    I2C_WriteHaveAdd(&RN5T618_I2C,&buffer[2],1,FG_CTRL,RN5T618_Add);// enable fuel gauge
    I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[0],1,VOLTAGE_1,RN5T618_Add);
    I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[1],1,VOLTAGE_0,RN5T618_Add);
	tmpf = (buffer[0]*256+buffer[1])*1.22f;
    
    return tmpf;
}

unsigned int GET_BATCAP(void)
{
	unsigned int fullcapvalue;
	unsigned int avaicapvalue;
	float batcapvalue;
	
	uint8_t	buffer[2];
	I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[0],1,FACAP_H,RN5T618_Add);
  I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[1],1,FACAP_L,RN5T618_Add);
	fullcapvalue = ((buffer[0]&0X00FF)<<8) + buffer[1];
	fullcapvalue -= 2000;
	
	I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[0],1,RECAP_H,RN5T618_Add);
  I2C_ReadHaveAdd(&RN5T618_I2C,&buffer[1],1,RECAP_H,RN5T618_Add);
	avaicapvalue = ((buffer[0]&0X00FF)<<8) + buffer[1];
	avaicapvalue -= 2000;
	
	batcapvalue = (float)(avaicapvalue * 100)/fullcapvalue;
	return (unsigned int )batcapvalue;
}

//��ȡ����¶ȡ�û���������裬������ֵ�ǲ��Ե�
//uint16_t GET_BATTEMP(void)
//{
//	uint16_t TEMPERATURE;
//	uint8_t buffer[3];
//	buffer[2] = 0x01;
//	HAL_I2C_Mem_Write(&hi2c1,RN5T618_Add_WR,FG_CTRL,1,&buffer[2],1,100);
//	HAL_I2C_Mem_Read(&hi2c1,RN5T618_Add_RD,BATTEMP_1,1,&buffer[0],1,100);
//	HAL_I2C_Mem_Read(&hi2c1,RN5T618_Add_RD,BATTEMP_0,1,&buffer[1],1,100);	
//	TEMPERATURE = (buffer[0]*256+buffer[1])*0.0625;
//	return TEMPERATURE;
//}

uint8_t BatteryChargeState(void)
{
	uint8_t RD_BUF[1];
    uint8_t sta;
    I2C_ReadHaveAdd(&RN5T618_I2C,&RD_BUF[0],1,CHGSTATE,RN5T618_Add);
	if((RD_BUF[0]&0x1f) == 0x00)
		sta = 0;
	else
		sta = 1;
    return sta;
}


void MMA9555L_POWERON()
{
//		POWERON_LCD();
		POWERON_MMA9555L();
}
void MMA9555L_POWEROFF()
{
//		POWEROFF_LCD();
		POWEROFF_MMA9555L();
}

