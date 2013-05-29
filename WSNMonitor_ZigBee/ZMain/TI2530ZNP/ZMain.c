/**************************************************************************************************
  Filename:       ZMain.c
  Revised:        $Date: 2009-08-03 15:07:36 -0700 (Mon, 03 Aug 2009) $
  Revision:       $Revision: 20441 $

  Description:    Startup and shutdown code for ZStack
  Notes:          This version targets the Chipcon CC2530


  Copyright 2005-2009 Texas Instruments Incorporated. All rights reserved.

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

/*********************************************************************
 * INCLUDES
 */

#include "ZComDef.h"
#include "OSAL.h"
#include "OSAL_Nv.h"
#include "OnBoard.h"
#include "ZMAC.h"
#include "znp_app.h"

#ifndef NONWK
  #include "AF.h"
#endif

/* Hal */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_adc.h"
#include "hal_drivers.h"
#include "hal_assert.h"
#include "hal_flash.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// Maximun number of Vdd samples checked before go on
#define MAX_VDD_SAMPLES  3
#define ZMAIN_VDD_LIMIT  HAL_ADC_VDD_LIMIT_4

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

extern bool HalAdcCheckVdd (uint8 limit);

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void zmain_dev_info( void );
static void zmain_ext_addr( void );
static void zmain_vdd_check( void );

#ifdef LCD_SUPPORTED
static void zmain_lcd_init( void );
#endif

/*********************************************************************
 * @fn      main
 * @brief   First function called after startup.
 * @return  don't care
 */
int main( void )
{
  // Turn off interrupts
  osal_int_disable( INTS_ALL );

  // Initialization for board related stuff such as LEDs
  HAL_BOARD_INIT();

  // Make sure supply voltage is high enough to run
  zmain_vdd_check();

  // Initialize board I/O
  InitBoard( OB_COLD );

  // Initialze HAL drivers
  HalDriverInit();

  // Initialize NV System
  osal_nv_init( NULL );

  // Initialize and check the ZNP RF Test Mode NV items.
  znpTestRF();

  // Initialize the MAC
  ZMacInit();

  // Determine the extended address
  zmain_ext_addr();

  // Initialize basic NV items
  zgInit();

#ifndef NONWK
  // Since the AF isn't a task, call it's initialization routine
  afInit();
#endif

  // Initialize the operating system
  osal_init_system();

  // Allow interrupts
  osal_int_enable( INTS_ALL );

  // Final board initialization
  InitBoard( OB_READY );

  // Display information about this device
  zmain_dev_info();

  /* Display the device info on the LCD */
#ifdef LCD_SUPPORTED
  zmain_lcd_init();
#endif

#ifdef WDT_IN_PM1
  /* If WDT is used, this is a good place to enable it. */
  WatchDogEnable( WDTIMX );
#endif

  osal_start_system(); // No Return from here

  return 0;  // Shouldn't get here.
} // main()

/*********************************************************************
 * @fn      zmain_vdd_check
 * @brief   Check if the Vdd is OK to run the processor.
 * @return  Return if Vdd is ok; otherwise, flash LED, then reset
 *********************************************************************/
static void zmain_vdd_check( void )
{
  uint8 vdd_passed_count = 0;
  bool toggle = 0;

  // Repeat getting the sample until number of failures or successes hits MAX
  // then based on the count value, determine if the device is ready or not
  while ( vdd_passed_count < MAX_VDD_SAMPLES )
  {
    if ( HalAdcCheckVdd (ZMAIN_VDD_LIMIT) )
    {
      vdd_passed_count++;    // Keep track # times Vdd passes in a row
      MicroWait (10000);     // Wait 10ms to try again
    }
    else
    {
      vdd_passed_count = 0;  // Reset passed counter
      MicroWait (50000);     // Wait 50ms
      MicroWait (50000);     // Wait another 50ms to try again
    }

    /* toggle LED1 and LED2 */
    if (vdd_passed_count == 0)
    {
      if ((toggle = !(toggle)))
        HAL_TOGGLE_LED1();
      else
        HAL_TOGGLE_LED2();
    }
  }

  /* turn off LED1 */
  HAL_TURN_OFF_LED1();
  HAL_TURN_OFF_LED2();
}

/**************************************************************************************************
 * @fn          zmain_ext_addr
 *
 * @brief       Execute a prioritized search for a valid extended address and write the results
 *              into the OSAL NV system for use by the system. Temporary address not saved to NV.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
static void zmain_ext_addr(void)
{
  uint8 nullAddr[Z_EXTADDR_LEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  uint8 writeNV = TRUE;

  // First check whether a non-erased extended address exists in the OSAL NV.
  if ((SUCCESS != osal_nv_item_init(ZCD_NV_EXTADDR, Z_EXTADDR_LEN, NULL))  ||
      (SUCCESS != osal_nv_read(ZCD_NV_EXTADDR, 0, Z_EXTADDR_LEN, aExtendedAddress)) ||
      (osal_memcmp(aExtendedAddress, nullAddr, Z_EXTADDR_LEN)))
  {
    // Attempt to read the extended address from the location on the lock bits page
    // where the programming tools know to reserve it.
    HalFlashRead(HAL_FLASH_IEEE_PAGE, HAL_FLASH_IEEE_OSET, aExtendedAddress, Z_EXTADDR_LEN);

    if (osal_memcmp(aExtendedAddress, nullAddr, Z_EXTADDR_LEN))
    {
      // Attempt to read the extended address from the designated location in the Info Page.
      if (!osal_memcmp((uint8 *)(P_INFOPAGE+HAL_INFOP_IEEE_OSET), nullAddr, Z_EXTADDR_LEN))
      {
        osal_memcpy(aExtendedAddress, (uint8 *)(P_INFOPAGE+HAL_INFOP_IEEE_OSET), Z_EXTADDR_LEN);
      }
      else  // No valid extended address was found.
      {
        uint8 idx;
        
#if !defined ( NV_RESTORE )
        writeNV = FALSE;  // Make this a temporary IEEE address
#endif

        /* Attempt to create a sufficiently random extended address for expediency.
         * Note: this is only valid/legal in a test environment and
         *       must never be used for a commercial product.
         */
        for (idx = 0; idx < (Z_EXTADDR_LEN - 2);)
        {
          uint16 randy = osal_rand();
          aExtendedAddress[idx++] = LO_UINT16(randy);
          aExtendedAddress[idx++] = HI_UINT16(randy);
        }
        // Next-to-MSB identifies ZigBee devicetype.
#if ZG_BUILD_COORDINATOR_TYPE && !ZG_BUILD_JOINING_TYPE
        aExtendedAddress[idx++] = 0x10;
#elif ZG_BUILD_RTRONLY_TYPE
        aExtendedAddress[idx++] = 0x20;
#else
        aExtendedAddress[idx++] = 0x30;
#endif
        // MSB has historical signficance.
        aExtendedAddress[idx] = 0xF8;
      }
    }

    if (writeNV)
    {
      (void)osal_nv_write(ZCD_NV_EXTADDR, 0, Z_EXTADDR_LEN, aExtendedAddress);
    }
  }

  // Set the MAC PIB extended address according to results from above.
  (void)ZMacSetReq(MAC_EXTENDED_ADDRESS, aExtendedAddress);
}

/**************************************************************************************************
 * @fn          zmain_dev_info
 *
 * @brief       This displays the IEEE (MSB to LSB) on the LCD.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
static void zmain_dev_info(void)
{
#ifdef LCD_SUPPORTED
  uint8 i;
  uint8 *xad;
  uint8 lcd_buf[Z_EXTADDR_LEN*2+1];

  // Display the extended address.
  xad = aExtendedAddress + Z_EXTADDR_LEN - 1;

  for (i = 0; i < Z_EXTADDR_LEN*2; xad--)
  {
    uint8 ch;
    ch = (*xad >> 4) & 0x0F;
    lcd_buf[i++] = ch + (( ch < 10 ) ? '0' : '7');
    ch = *xad & 0x0F;
    lcd_buf[i++] = ch + (( ch < 10 ) ? '0' : '7');
  }
  lcd_buf[Z_EXTADDR_LEN*2] = '\0';
  HalLcdWriteString( "IEEE: ", HAL_LCD_LINE_1 );
  HalLcdWriteString( (char*)lcd_buf, HAL_LCD_LINE_2 );
#endif
}

#ifdef LCD_SUPPORTED
/*********************************************************************
 * @fn      zmain_lcd_init
 * @brief   Initialize LCD at start up.
 * @return  none
 *********************************************************************/
static void zmain_lcd_init ( void )
{
#ifdef SERIAL_DEBUG_SUPPORTED
  {
    HalLcdWriteString( "TexasInstruments", HAL_LCD_LINE_1 );

#if defined( MT_MAC_FUNC )
#if defined( ZDO_COORDINATOR )
      HalLcdWriteString( "MAC-MT Coord", HAL_LCD_LINE_2 );
#else
      HalLcdWriteString( "MAC-MT Device", HAL_LCD_LINE_2 );
#endif // ZDO
#elif defined( MT_NWK_FUNC )
#if defined( ZDO_COORDINATOR )
      HalLcdWriteString( "NWK Coordinator", HAL_LCD_LINE_2 );
#else
      HalLcdWriteString( "NWK Device", HAL_LCD_LINE_2 );
#endif // ZDO
#endif // MT_FUNC
  }
#endif // SERIAL_DEBUG_SUPPORTED
}
#endif

/*********************************************************************
*********************************************************************/
