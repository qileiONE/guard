
#define __tool_c

#include "Ctypedef.h"
#include "stdint.h"
#include "stdlib.h"

/* Exported macro ------------------------------------------------------------*/
/* Common routines */
#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

#define SerialPutString(x) Serial_PutString((uint8_t*)(x))


/**
  * @brief  Convert a string to an integer
  * @param  inputstr: The string to be converted
  * @param  intnum: The intger value
  * @retval 1: Correct
  *         0: Error
  */
uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
  uint32_t i = 0, res = 0;
  uint32_t val = 0;

  if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
  {
    if (inputstr[2] == '\0')
    {
      return 0;
    }
    for (i = 2; i < 11; i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1; */
        res = 1;
        break;
      }
      if (ISVALIDHEX(inputstr[i]))
      {
        val = (val << 4) + CONVERTHEX(inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* over 8 digit hex --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }
  else /* max 10-digit decimal input */
  {
    for (i = 0;i < 11;i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1 */
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
      {
        val = val << 10;
        *intnum = val;
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
      {
        val = val << 20;
        *intnum = val;
        res = 1;
        break;
      }
      else if (ISVALIDDEC(inputstr[i]))
      {
        val = val * 10 + CONVERTDEC(inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* Over 10 digit decimal --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }

  return res;
}
uint32_t StrToHex(char *ch)
{
	uint8_t v=0,i;
	uint32_t da=0;
	
	for(i=0;i<8;i++)
	{
		if((*ch >='0')&&(*ch <= '9'))
		{
			v = *ch - '0';
		}
		else if((*ch >='a')&&(*ch <= 'f'))
		{
			v = *ch - 'a' + 10;
		}
		else if((*ch >='A')&&(*ch <= 'F'))
		{
			v = *ch - 'A' + 10;
		}
		else
		{
			break;
		}
		da = (da << 4) + v;
		ch++;
	}
	return da;
}

extern volatile UINT32 systick_ms;
UINT32	GetDelay(UINT32 base)
{
	UINT32 cur_time;

	cur_time = systick_ms;
	if(cur_time >= base)
	{
		return (cur_time - base);
	}
	else
	{
		return (0xffffffff - base + cur_time + 1);
	}
}

/*******************************************************************************
* Function Name   : ADC_Filter
* Description : ADC_Filter
* Input       : None
* Output       : None
* Return       : ADC Converted Value
*******************************************************************************/
UINT32 Average16_NoMinMax(volatile UINT16 *ad,UINT8 num)
{
  UINT8  i;
  UINT32 result=0;
  UINT32 max_ad=0;
  UINT32 min_ad=0x0fff;
  if(num == 1)
  	return *ad;
  if(num == 2)
  	return (*ad + *(ad +1))/2;
  for(i=num;i>0;i--)
  {
     if(max_ad<(*ad))
	 {
	    max_ad=(*ad);
	 }
	 if(min_ad>(*ad))
	 {
	    min_ad=(*ad);	 
	 }
     result += *ad;
	 ad++;
  }
  result=result-max_ad-min_ad;
  return result/(num-2);
}
//return : 1 err    0 ok
//ad: data ptr
//num: data number
//dlta: max-min must be less than dlta
//dst: output
UINT8 filterDltaNoMinMax(volatile UINT16 *ad,UINT8 num,UINT16 dlta,UINT16 *dst)
{
	UINT16  i;
	UINT32 result=0;
	UINT32 max_ad=0;
	UINT32 min_ad=0x0fff;
	if(num == 1)
	{
		*dst = *ad;
		return 0;
	}
	if(num == 2)
	{
		min_ad = *ad;
		max_ad = *(ad+1);
		if(abs(max_ad-min_ad)>dlta)
		{
			*dst = 0;
			return 1;
		}
		else
		{
			*dst = ((min_ad + max_ad) >> 1);
			return 0;
		}
	}
	for(i=num;i>0;i--)
	{
		if(max_ad<(*ad))
		{
			max_ad=(*ad);
		}
		if(min_ad>(*ad))
		{
			min_ad=(*ad);	 
		}
		result += *ad;
		ad++;
	}
	if(abs(max_ad-min_ad)>dlta)
	{
		*dst = 0;
		return 1;
	}
	else
	{
		result=result-max_ad-min_ad;
		*dst = result/(num-2);		
	}
	return 0;
}
UINT32 GetTimeTick(void)
{
	return 	systick_ms;
}		    
