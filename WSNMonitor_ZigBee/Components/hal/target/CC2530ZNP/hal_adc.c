/**************************************************************************************************
  Filename:       hal_adc.c
  Revised:        $Date: 2009-12-03 16:14:15 -0800 (Thu, 03 Dec 2009) $
  Revision:       $Revision: 21272 $

  Description:    This file contains the interface to the HAL ADC.


  Copyright 2006-2009 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/**************************************************************************************************
 *                                           INCLUDES
 **************************************************************************************************/
#include  "hal_mcu.h"
#include  "hal_defs.h"
#include  "hal_types.h"
#include  "hal_adc.h"

/**************************************************************************************************
 *                                            CONSTANTS
 **************************************************************************************************/
#define HAL_ADC_EOC         0x80    /* End of Conversion bit */
#define HAL_ADC_START       0x40    /* Starts Conversion */

#define HAL_ADC_STSEL_EXT   0x00    /* External Trigger */
#define HAL_ADC_STSEL_FULL  0x10    /* Full Speed, No Trigger */
#define HAL_ADC_STSEL_T1C0  0x20    /* Timer1, Channel 0 Compare Event Trigger */
#define HAL_ADC_STSEL_ST    0x30    /* ADCCON1.ST =1 Trigger */

#define HAL_ADC_RAND_NORM   0x00    /* Normal Operation */
#define HAL_ADC_RAND_LFSR   0x04    /* Clock LFSR */
#define HAL_ADC_RAND_SEED   0x08    /* Seed Modulator */
#define HAL_ADC_RAND_STOP   0x0c    /* Stop Random Generator */
#define HAL_ADC_RAND_BITS   0x0c    /* Bits [3:2] */

#define HAL_ADC_DEC_064     0x00    /* Decimate by 64 : 8-bit resolution */
#define HAL_ADC_DEC_128     0x10    /* Decimate by 128 : 10-bit resolution */
#define HAL_ADC_DEC_256     0x20    /* Decimate by 256 : 12-bit resolution */
#define HAL_ADC_DEC_512     0x30    /* Decimate by 512 : 14-bit resolution */
#define HAL_ADC_DEC_BITS    0x30    /* Bits [5:4] */

#define HAL_ADC_STSEL       HAL_ADC_STSEL_ST
#define HAL_ADC_RAND_GEN    HAL_ADC_RAND_STOP
#define HAL_ADC_REF_VOLT    HAL_ADC_REF_AVDD
#define HAL_ADC_DEC_RATE    HAL_ADC_DEC_064
#define HAL_ADC_SCHN        HAL_ADC_CHN_VDD3
#define HAL_ADC_ECHN        HAL_ADC_CHN_GND

/* Vdd limit values */
static __code const uint16 HalAdcVddLimit[] =
{
  0x369C,       /*  VDD Limit - 1.6v  */
  0x3A06,       /*  VDD Limit - 1.7v  */
  0x3D70,       /*  VDD Limit - 1.8v  */
  0x40D9,       /*  VDD Limit - 1.9v  */
  0x4443,       /*  VDD Limit - 2.0v  */
  0x47AD,       /*  VDD Limit - 2.1v  */
  0x4B17,       /*  VDD Limit - 2.2v  */
  0x4E81,       /*  VDD Limit - 2.3v  */
  0x51EA,       /*  VDD Limit - 2.4v  */
};

/**************************************************************************************************
 *                                              MACROS
 **************************************************************************************************/
#define HAL_ADC_CLR_EOC()   asm("PUSH A"); asm("MOV A,ADCL"); asm("MOV A,ADCH"); asm("POP A");

/**************************************************************************************************
 *                                            TYPEDEFS
 **************************************************************************************************/

/**************************************************************************************************
 *                                         GLOBAL VARIABLES
 **************************************************************************************************/

/**************************************************************************************************
 *                                          FUNCTIONS - API
 **************************************************************************************************/
extern bool HalAdcCheckVdd (uint8 limit);

/**************************************************************************************************
 * @fn      HalAdcInit
 *
 * @brief   Initialize ADC Service
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HalAdcInit (void)
{
#if (HAL_ADC == TRUE)
#endif
}

/**************************************************************************************************
 * @fn      HalAdcRead
 *
 * @brief   Read the ADC based on given channel and resolution
 *
 * @param   channel - channel where ADC will be read
 * @param   resolution - the resolution of the value
 *
 * @return  16 bit value of the ADC in offset binary format.
 *          Note that the ADC is "bipolar", which means the GND (0V) level is mid-scale.
 **************************************************************************************************/
uint16 HalAdcRead (uint8 channel, uint8 resolution)
{
  uint16 reading = 0;
#if (HAL_ADC == TRUE)
  uint8 resbits;

  /* If Analog input channel is AIN0..AIN7, make sure corresponing P0 I/O pin is enabled. The code
   * disables the pin at the end of this function. I think it is better to leave the pin enabled
   * because the results will be more accurate. Because of the inherent capacitance on the
   * pin, it takes time for the voltage on the pin to charge up to its steady-state level. If
   * HalAdcRead() has to turn on the pin for every conversion, the results may show a lower voltage
   * than actuality because the pin did not have time to fully charge.
   */
  if (channel < 8)
  {
    APCFG = (1 << channel);
  }
  else if (HAL_ADC_CHN_TEMP == channel)
  {
    ATEST = 0x01;
    TR0  |= 0x01; 
  }  

  switch (resolution)  // Convert resolution to decimation rate.
  {
  case HAL_ADC_RESOLUTION_8:
    resbits = HAL_ADC_DEC_064;
    break;
  case HAL_ADC_RESOLUTION_10:
    resbits = HAL_ADC_DEC_128;
    break;
  case HAL_ADC_RESOLUTION_12:
    resbits = HAL_ADC_DEC_256;
    break;
  case HAL_ADC_RESOLUTION_14:
  default:
    resbits = HAL_ADC_DEC_512;
    break;
  }

  ADCCON3 = HAL_ADC_REF_125V | resbits | channel;
  while (!(ADCCON1 & HAL_ADC_EOC));
  APCFG = 0;  // Disable channel after done conversion.

  reading = (uint16)ADCL;
  reading |= (uint16)ADCH << 8;

  switch (resolution)
  {
    case HAL_ADC_RESOLUTION_8:
      reading >>= 8;
      break;
    case HAL_ADC_RESOLUTION_10:
      reading >>= 6;
      break;
    case HAL_ADC_RESOLUTION_12:
      reading >>= 4;
      break;
    case HAL_ADC_RESOLUTION_14:
    default:
      reading >>= 2;
    break;
  }
#else
  // unused arguments
  (void) channel;
  (void) resolution;
#endif

  return reading;
}

/**************************************************************************************************
 * @fn      HalAdcCheckVdd
 *
 * @brief   Check the Vdd and return TRUE if it greater than or equal the limit
 *
 * @param   limit - limit that needs to be checked with the Vdd
 *
 * @return  TRUE if Vdd >= limit, FALSE otherwise
 *
 **************************************************************************************************/
bool HalAdcCheckVdd (uint8 limit)
{
  uint16 value;

  /* Clear ADC interrupt flag */
  ADCIF = 0;

  /* Setup the new value for conversion */
  ADCCON3 = (HAL_ADC_REF_125V | HAL_ADC_DEC_064 | HAL_ADC_CHN_VDD3);

  /* Wait for the conversion to finish */
  while ( !ADCIF );

  /* Get the result */
  value = ADCL;
  value |= ((uint16) ADCH) << 8;

  /* Check the limit and return */
  return ( value >= HalAdcVddLimit[limit] );
}

/**************************************************************************************************
**************************************************************************************************/
