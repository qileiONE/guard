
#ifndef EEPROM_OP_H
#define EEPROM_OP_H

#include "stdint.h"

#define	EEPROM_CALI_ADD	    ((u32)0x0800b000)
#define	EEPROM_BASE1_ADD	((u32)0x0800d000)
#define	EEPROM_BASE2_ADD	((u32)0x0800e000)
#define	EEPROM_NFCCARD_ADD	    ((u32)0x0800f000)

#define FLASH_PAGE_SIZE    ((uint16_t)0x400)

#define	T_MAX_POINT			100


uint16_t readTempPoint(uint16_t year,uint16_t month,uint16_t day,uint16_t num);
uint16_t saveTempPoint(uint16_t year,uint16_t month,uint16_t day,uint16_t num,uint16_t data);
void saveTempCali(float  k,float b);
void getTempCali(float *k,float *b);
void saveNFCCardFlag(void);
void getNFCCardFlag(void);
#endif
