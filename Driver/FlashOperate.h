
#ifndef __FLASHOPERATE_H
#define __FLASHOPERATE_H

#define __DSB()                           __dsb(0)

#define AIRCR_VECTKEY_MASK    ((u32)0x05FA0000)
/* ##################################    Reset function  ############################################ */

/**
 * @brief  Initiate a system reset request.
 *
 * Initiate a system reset request to reset the MCU
 */
static __inline void NVIC_SystemReset(void)
{
  __SETFAULTMASK();
  SCB->AIRCR = AIRCR_VECTKEY_MASK | (u32)0x04;                   /* Keep priority group unchanged */
  __DSB();                                                     /* Ensure completion of memory access */              
  while(1);                                                    /* wait until reset */
}

uint8_t FLASH_DisableWriteProtectionPages(uint32_t Mask);
uint8_t FLASH_EnableWriteProtectionPages(uint32_t Mask);

#endif

