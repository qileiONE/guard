

#ifndef __GLOBERAM_H
#define __GLOBERAM_H

#define	UARTBUF_MAX		200
#define PROTOL_MAX_SIZE	200
#define PROTOL_MIN_SIZE 2

#define MEAU_PHASE_MAX	5

extern volatile UINT32 	systick_ms;

extern UINT8			deviceAdd;

extern UINT8			SlaverHadTrig;
extern UINT8			KeyValue;
extern volatile UINT8	LedTimeout;

extern UINT8 			checkcount;

extern UINT8			RelayFlag;
extern UINT8			KeyInput[4];

extern UINT8			PC485_ToPC;
extern UINT8			PCWait_Nrf;


#endif
