#ifndef __30205_H
#define __30205_H
#include "sys.h"
#include "IIC.h"
#include "adc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//����������30205�����´���������							  
//////////////////////////////////////////////////////////////////////////////////
 #define MAX30205_ADDRESS  0X90    //8bit address converted to 7bit
 //һЩ�Ĵ�����ַ
 #define MAX30205_TEMPERATURE 0X00
 #define MAX30205_CONFIGURATION 0X01
 #define MAX30205_THYST         0X02
 #define MAX30205_TOS           0X03
 #define IIC_WRITE 0   //���ݷ��� д��
 #define IIC_READ   1   //���ݷ��� ��ȡ
 
 #define    TempBase    2.35f
 //extern float temperature;
 extern __IO u16 ADC_ConvertedValue;
 
 extern unsigned char tempDevice ; //0 : MAX    1: NTC
 
 void shutdown(void);
 void max_init (void);
 float GetTemperature(void);
 void I2CwriteByte(u8 address,u8 subAddress,u8 data);
 void I2Cwriteaddr(u8 address,u8 subAddress);
 u8 I2CreadByte(u8 address,u8 subAddress);
 void I2CreadBytes(u8 address,u8 subAddress,u8*dest,u8 count);
int16_t Read_max16(u8 address,u8 subAddress);
float GetTemperature1(void);
 #endif

