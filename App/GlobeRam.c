
#include "includes.h"

volatile UINT32 systick_ms;
UINT8			deviceAdd = 0x01;


UINT8			SlaverHadTrig;
UINT8			KeyValue;
volatile UINT8	LedTimeout;

UINT8 			checkcount = 0;

UINT8			RelayFlag;
UINT8			KeyInput[4];

UINT8			PC485_ToPC = 0;
UINT8			PCWait_Nrf = 0;





