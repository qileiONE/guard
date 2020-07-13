
#ifndef __tool_h
#define __tool_h

#include "stdint.h"
#include "Ctypedef.h"

uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum);
uint32_t StrToHex(char *ch);
UINT32	 GetDelay(UINT32 base);
UINT32 	 Average16_NoMinMax(volatile UINT16 *ad,UINT8 num);
UINT8 	 filterDltaNoMinMax(volatile UINT16 *ad,UINT8 num,UINT16 dlta,UINT16 *dst);
UINT32 	 GetTimeTick(void);
#endif

