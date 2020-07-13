#define EEPROM_OP_C

#include "stm32f10x_flash.h"
#include "eeprom.h"

extern uint8_t nfccard_flag;
uint32_t readEeprom(uint32_t add);
uint8_t writeEeprom(uint32_t add,uint32_t data);
void ClearEeprom(uint32_t add);


uint32_t readEeprom(uint32_t add)
{
    uint32_t read;
    read = (*(volatile uint32_t*) add);
    return read;
}
uint8_t writeEeprom(uint32_t add,uint32_t data)
{
    uint8_t err;
    uint32_t read;
	
	read = (*(volatile uint32_t*) add);
	if(read != 0xffffffff)
	{
		return 1;
	}
    
    FLASH_Unlock();
    FLASH_ProgramWord(add,data);
    FLASH_Lock();
    
    read = (*(volatile uint32_t*) add);
    if(read == data)
        err = 0;
    else
        err = 1;
    return err;
}
void ClearEeprom(uint32_t add)
{
	FLASH_Unlock();
	FLASH_ErasePage(add);
	FLASH_Lock();
}

uint16_t readTempPoint(uint16_t year,uint16_t month,uint16_t day,uint16_t num)
{
	uint16_t data = 0xffff;
	uint32_t date;
	
	date = ((uint32_t )year << 16) + (month << 8) + day;
	if(readEeprom(EEPROM_BASE1_ADD) == date)
	{
		data = readEeprom(EEPROM_BASE1_ADD + num * 4);
	}
	else if(readEeprom(EEPROM_BASE2_ADD) == date)
	{
		data = readEeprom(EEPROM_BASE2_ADD + num * 4);
	}
	
	return data;
}
uint16_t saveTempPoint(uint16_t year,uint16_t month,uint16_t day,uint16_t num,uint16_t data)
{
	uint8_t err=0;
	uint8_t flag=0;
	uint32_t date;
	uint32_t date1,date2;
	uint32_t add;
	
	if((num == 0)||(num > T_MAX_POINT))
		return 101;
	
	date = ((uint32_t )year << 16) + (month << 8) + day;
	date1 = readEeprom(EEPROM_BASE1_ADD);
	
	if(date1 == date)
	{
		err = writeEeprom(EEPROM_BASE1_ADD + num * 4,data);
	}
	else if(date1 == 0xffffffff)
	{
		writeEeprom(EEPROM_BASE1_ADD,date);
		err = writeEeprom(EEPROM_BASE1_ADD + num * 4,data);
	}
	else
		flag = 1;
	if(flag == 1)//&&(err != 0))
	{
		flag = 0;
		date2 = readEeprom(EEPROM_BASE2_ADD);
		if(date2 == date)
		{
			err = writeEeprom(EEPROM_BASE2_ADD + num * 4,data);
		}
		else if(date2 == 0xffffffff)
		{
			writeEeprom(EEPROM_BASE2_ADD,date);
			err = writeEeprom(EEPROM_BASE2_ADD + num * 4,data);
		}
		else
			flag = 1;
	}
	if(flag == 1)//&&(err != 0))
	{
		if(date1 < date2)
			add = EEPROM_BASE1_ADD;
		else
			add = EEPROM_BASE2_ADD;
		ClearEeprom(add);
		writeEeprom(add,date);
		err = writeEeprom(add + num * 4,data);
	}
	return err;
}

void saveTempCali(float  k,float b)
{
    ClearEeprom(EEPROM_CALI_ADD);
	writeEeprom(EEPROM_CALI_ADD,k*100);
    writeEeprom(EEPROM_CALI_ADD+4,b*100);
}
void getTempCali(float *k,float *b)
{
    int32_t tmp1,tmp2;
    tmp1 = (int32_t)readEeprom(EEPROM_CALI_ADD);
    tmp2 = (int32_t)readEeprom(EEPROM_CALI_ADD+4);
    
    if((tmp1 > 200)||(tmp1 < 50))
        tmp1 = 100;
    if((tmp2 > 200)||(tmp2 < -200))
        tmp2 = 0;
    *k = (float)tmp1/100;
    *b = (float)tmp2/100;
}


void saveNFCCardFlag(void)
{//nfccard_flag
		ClearEeprom(EEPROM_NFCCARD_ADD);
		writeEeprom(EEPROM_NFCCARD_ADD,nfccard_flag);
}

void getNFCCardFlag(void)
{
	int32_t tmp1;
	tmp1 = (int32_t)readEeprom(EEPROM_NFCCARD_ADD);
	
	if((tmp1 != 1) && (tmp1 != 0))
	{
		nfccard_flag = 0;
	}
	else
	{
		nfccard_flag = tmp1;
	}
}
