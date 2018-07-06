#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

char s_temp[20];
int i;

volatile unsigned int *DWT_CYCCNT     = (volatile unsigned int *)0xE0001004; //address of the register
volatile unsigned int *DWT_CONTROL    = (volatile unsigned int *)0xE0001000; //address of the register
volatile unsigned int *SCB_DEMCR      = (volatile unsigned int *)0xE000EDFC; //address of the register

void init_cycle_counter()
{
  // Init cycle counter
  *SCB_DEMCR = *SCB_DEMCR | 0x01000000;
  *DWT_CYCCNT = 0; // reset the counter
  *DWT_CONTROL = *DWT_CONTROL | 1 ; // enable the counter
}

uint32_t read_cycle_counter()
{
	return *DWT_CYCCNT;
}

void delay_us(uint32_t time_us)
{
  uint32_t cyc1 = *DWT_CYCCNT;
  if (time_us>0)
  {
    while ((*DWT_CYCCNT-cyc1)<=(SystemCoreClock/1000000)*(time_us-1)){};
  }
}

void delay_ms(uint32_t time_ms)
{
    HAL_Delay(time_ms);
}

/*******************************************************************************
* Function Name  : HexToChar.
* Description    : Convert Hex 32Bits value into char.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  i = 0;

  for( i = 0 ; i < len ; i++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* i] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* i] = (value >> 28) + 'A' - 10;
    }

    value = value << 4;

    pbuf[ 2* i + 1] = 0;
  }
}

/************************************************************************/
/*                                                                      */
/* 2compl_to_int (24-bit)                                               */
/*                                                                      */
/* umwandeln von Werten in 2er-Komplementdarstellung nach int			*/
/* das Vorzeichen enthält.                                              */
/*                                                                      */
/************************************************************************/
int32_t compl2_to_int24(uint32_t wert)
{
	return -1*(wert & 0x800000) + (wert & 0x7FFFFF);
}

/************************************************************************/
/*                                                                      */
/* 2compl_to_int (48-bit)                                               */
/*                                                                      */
/* umwandeln von Werten in 2er-Komplementdarstellung nach int			*/
/* das Vorzeichen enthält.                                              */
/*                                                                      */
/************************************************************************/
int64_t compl2_to_int48(uint64_t wert)
{
	return -1*(wert & 0x800000000000) + (wert & 0x7FFFFFFFFFFF);
}

/************************************************************************/
/*                                                                      */
/* 2compl_to_int                                                        */
/*                                                                      */
/* umwandeln von Werten in 2er-Komplementdarstellung nach int				    */
/* das Vorzeichen enthält.                                              */
/*                                                                      */
/************************************************************************/
int16_t compl2_to_int16(uint16_t wert)
{
	return -1*(wert & 0x8000) + (wert & 0x7FFF);
}

/************************************************************************/
/*                                                                      */
/* bytes_to_int                                                         */
/*                                                                      */
/* umwandeln von 2 Bytes in einen Integerwert, wobei das Bit7 von       */
/* byte_h das Vorzeichen enthält.                                       */
/* Wertebereich <wert>: -16383 ... 16383								*/
/*                                                                      */
/************************************************************************/
int16_t bytes_to_int16(uint8_t byte_h, uint8_t byte_l)
{
  if ( (byte_h & 0x80)>0)
    return (-1*(int16_t)(((byte_h & 0x7F)<<8) + byte_l));
  else
    return ((int16_t)(byte_h<<8) + byte_l);
}

/************************************************************************/
/*                                                                      */
/* int_to_bytes                                                         */
/*                                                                      */
/* umwandeln von Integerwerten in 2 Bytes, wobei das Bit7 von byte_h    */
/* das Vorzeichen enthält.                                              */
/* Wertebereich <wert>: -16383 ... 16383								*/
/*                                                                      */
/************************************************************************/
void int16_to_bytes(int16_t wert, uint8_t *byte_h, uint8_t *byte_l)
{
  if (wert<0)
  {
    wert=-1*wert;
    *byte_h = (uint8_t)(0x80 + ((wert>>8) & 0x7F));
  }
  else
  {
    *byte_h = (uint8_t)((wert & 0x7F00)>>8);
  }
  *byte_l = (uint8_t)(wert & 0x00FF);
}

//*****************************************************************************
//
// get_int_param
//
// extrahieren eines Integerwertes aus einen String
//
//*****************************************************************************
int32_t get_int_param(char* s,uint8_t start_index)
{
  i=0;
  for (i=0; i<8; i++)
    s_temp[i]=0;
  for (i=start_index; i<strlen(s); i++)
    s_temp[i-start_index]=s[i];

  return atol(s_temp);
} // *** get_int_param(char *s,unsigned char index)

//*****************************************************************************
//
// get_hex_param
//
// extrahieren eines Hexadezimalwertes aus einen String
//
//*****************************************************************************
uint32_t get_hex_param(char* s,uint8_t start_index)
{
  for (i=0; i<8; i++)
    s_temp[i]=0;
  for (i=start_index; i<strlen(s); i++)
    s_temp[i-start_index]=s[i];

  return strtoul(s_temp,NULL,16);
} // *** get_hex_param(char *s,unsigned char index)

//*****************************************************************************
//
// get_float_param
//
// extrahieren eines Floatwertes aus einen String
//
//*****************************************************************************
double get_double_param(char* s,uint8_t start_index)
{
  for (i=0; i<8; i++)
    s_temp[i]=0;
  for (i=start_index; i<strlen(s); i++)
    s_temp[i-start_index]=s[i];

  return atof(s_temp);
} // *** get_double_param(char *s,unsigned char index)



//*****************************************************************************
//
// get_str_param
//
// extrahieren eines Strings aus einen String
//
//*****************************************************************************
void get_string_param(char* src, char* dest, uint8_t start_index)
{
  for (i=0; i<20; i++)
    dest[i]=0;
  for (i=start_index; i<strlen(src); i++)
    dest[i-start_index]=src[i];
}

uint8_t bitcount (uint8_t n)
{
    uint8_t count=0;
    while (n)
    {
        count += n & 0x01;
        n >>= 1 ;
    }
    return count ;
}


float convert_frac_int(uint32_t val, uint8_t frac_bits)
{
	return ((float)val/(1<<frac_bits));
}

void uint32_to_byte_array(uint32_t val, uint8_t *ba)
{
  ba[0] = (uint8_t)val;
  ba[1] = (uint8_t)(val>>8);
  ba[2] = (uint8_t)(val>>16);
  ba[3] = (uint8_t)(val>>24);
}

