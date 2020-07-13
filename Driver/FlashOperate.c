
#define __FlashOperate_c

#include "stdint.h"
#include "stm32f10x.h"


/**
  * @brief  Disable the write protection of desired pages
  * @param  None
  * @retval None
  */
uint8_t FLASH_DisableWriteProtectionPages(uint32_t Mask)
{
	uint32_t useroptionbyte = 0, WRPR = 0;
	uint16_t var1 = OB_IWDG_SW, var2 = OB_STOP_NoRST, var3 = OB_STDBY_NoRST;
	FLASH_Status status = FLASH_BUSY;
	
	WRPR = FLASH_GetWriteProtectionOptionByte();

  /* Test if user memory is write protected */
  if ((WRPR & Mask) != Mask)
  {
    useroptionbyte = FLASH_GetUserOptionByte();

    Mask |= WRPR;

    status = FLASH_EraseOptionBytes();

    if (Mask != 0xFFFFFFFF)
    {
      status = FLASH_EnableWriteProtection((uint32_t)~Mask);
    }

    /* Test if user Option Bytes are programmed */
    if ((useroptionbyte & 0x07) != 0x07)
    { 
      /* Restore user Option Bytes */
      if ((useroptionbyte & 0x01) == 0x0)
      {
        var1 = OB_IWDG_HW;
      }
      if ((useroptionbyte & 0x02) == 0x0)
      {
        var2 = OB_STOP_RST;
      }
      if ((useroptionbyte & 0x04) == 0x0)
      {
        var3 = OB_STDBY_RST;
      }

      FLASH_UserOptionByteConfig(var1, var2, var3);
    }

    if (status == FLASH_COMPLETE)
    {
//      SerialPutString("Write Protection disabled...\r\n");
//
//      SerialPutString("...and a System Reset will be generated to re-load the new option bytes\r\n");

      /* Generate System Reset to load the new option byte values */
//      NVIC_SystemReset();
		return 0;
    }
    else
    {
		return 1;
//      SerialPutString("Error: Flash write unprotection failed...\r\n");
    }
  }
  else
  {
  		return 2;
//    SerialPutString("Flash memory not write protected\r\n");
  }
}
uint8_t FLASH_EnableWriteProtectionPages(uint32_t Mask)
{
	FLASH_Status status = FLASH_BUSY;
	uint32_t useroptionbyte = 0;
	uint16_t var1 = OB_IWDG_SW, var2 = OB_STOP_NoRST, var3 = OB_STDBY_NoRST;


	useroptionbyte = FLASH_GetUserOptionByte();
	status = FLASH_EraseOptionBytes();

    status = FLASH_EnableWriteProtection((uint32_t)Mask);
	if ((useroptionbyte & 0x07) != 0x07)
    { 
      /* Restore user Option Bytes */
      if ((useroptionbyte & 0x01) == 0x0)
      {
        var1 = OB_IWDG_HW;
      }
      if ((useroptionbyte & 0x02) == 0x0)
      {
        var2 = OB_STOP_RST;
      }
      if ((useroptionbyte & 0x04) == 0x0)
      {
        var3 = OB_STDBY_RST;
      }

      FLASH_UserOptionByteConfig(var1, var2, var3);
    }

    if (status == FLASH_COMPLETE)
    {
		return 0;
    }
    else
    {
		return 1;
    }
}

