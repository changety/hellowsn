/**************************************************************************************************
  Filename:       hal_mcu.h
  Revised:        $Date: 2009-03-18 10:57:06 -0700 (Wed, 18 Mar 2009) $
  Revision:       $Revision: 19445 $

  Description:    Describe the purpose and contents of the file.


  Copyright 2006-2007 Texas Instruments Incorporated. All rights reserved.

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

#ifndef _HAL_MCU_H
#define _HAL_MCU_H

/*
 *  Target : Texas Instruments CC2530 (8051 core)
 *
 */


/* ------------------------------------------------------------------------------------------------
 *                                           Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "hal_defs.h"
#include "hal_types.h"


/* ------------------------------------------------------------------------------------------------
 *                                        Target Defines
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_MCU_CC2530


/* ------------------------------------------------------------------------------------------------
 *                                     Compiler Abstraction
 * ------------------------------------------------------------------------------------------------
 */

/* ---------------------- IAR Compiler ---------------------- */
#ifdef __IAR_SYSTEMS_ICC__
#include <ioCC2530.h>
#define HAL_COMPILER_IAR
#define HAL_MCU_LITTLE_ENDIAN()   __LITTLE_ENDIAN__
#define _PRAGMA(x) _Pragma(#x)
#define HAL_ISR_FUNC_DECLARATION(f,v)   _PRAGMA(vector=v) __near_func __interrupt void f(void)
#define HAL_ISR_FUNC_PROTOTYPE(f,v)     _PRAGMA(vector=v) __near_func __interrupt void f(void)
#define HAL_ISR_FUNCTION(f,v)           HAL_ISR_FUNC_PROTOTYPE(f,v); HAL_ISR_FUNC_DECLARATION(f,v)

/* ---------------------- Keil Compiler ---------------------- */
#elif defined __KEIL__
#include <CC2530.h>
#define HAL_COMPILER_KEIL
#define HAL_MCU_LITTLE_ENDIAN()   0
#define HAL_ISR_FUNC_DECLARATION(f,v)   void f(void) interrupt v
#define HAL_ISR_FUNC_PROTOTYPE(f,v)     void f(void)
#define HAL_ISR_FUNCTION(f,v)           HAL_ISR_FUNC_PROTOTYPE(f,v); HAL_ISR_FUNC_DECLARATION(f,v)

/* ------------------ Unrecognized Compiler ------------------ */
#else
#error "ERROR: Unknown compiler."
#endif


/* ------------------------------------------------------------------------------------------------
 *                                        Interrupt Macros
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_ENABLE_INTERRUPTS()         st( EA = 1; )
#define HAL_DISABLE_INTERRUPTS()        st( EA = 0; )
#define HAL_INTERRUPTS_ARE_ENABLED()    (EA)

typedef unsigned char halIntState_t;
#define HAL_ENTER_CRITICAL_SECTION(x)   st( x = EA;  HAL_DISABLE_INTERRUPTS(); )
#define HAL_EXIT_CRITICAL_SECTION(x)    st( EA = x; )
#define HAL_CRITICAL_STATEMENT(x)       st( halIntState_t _s; HAL_ENTER_CRITICAL_SECTION(_s); x; HAL_EXIT_CRITICAL_SECTION(_s); )


/* ------------------------------------------------------------------------------------------------
 *                                        Reset Macro
 * ------------------------------------------------------------------------------------------------
 */
#define WD_EN               BV(3)
#define WD_MODE             BV(2)
#define WD_INT_1900_USEC    (BV(0) | BV(1))
#define WD_RESET1           (0xA0 | WD_EN | WD_INT_1900_USEC)
#define WD_RESET2           (0x50 | WD_EN | WD_INT_1900_USEC)
#define WD_KICK()           st( WDCTL = (0xA0 | WDCTL & 0x0F); WDCTL = (0x50 | WDCTL & 0x0F); )

/* disable interrupts, set watchdog timer, wait for reset */
#define HAL_SYSTEM_RESET()  st( HAL_DISABLE_INTERRUPTS(); WDCTL = WD_RESET1; WDCTL = WD_RESET2; for(;;); )

/* ------------------------------------------------------------------------------------------------
 *                                        CC2530 rev numbers
 * ------------------------------------------------------------------------------------------------
 */
#define REV_A          0x00    /* workaround turned off */
#define REV_B          0x11    /* PG1.1 */
#define REV_C          0x20    /* PG2.0 */
#define REV_D          0x21    /* PG2.1 */

/* ------------------------------------------------------------------------------------------------
 *                                        CC2530 sleep common code
 * ------------------------------------------------------------------------------------------------
 */

/* PCON bit definitions */
#define PCON_IDLE  BV(0)            /* Writing 1 to force CC2530 to enter sleep mode */

/* SLEEPCMD bit definitions */
#define OSC_PD     BV(2)            /* Idle Osc: powered down=1 */
#define PMODE     (BV(1) | BV(0))   /* Power mode bits */

/* SLEEPSTA bit definitions */
#define XOSC_STB   BV(6)  /* XOSC: powered, stable=1 */
#define HFRC_STB   BV(5)  /* HFRCOSC: powered, stable=1 */

/* SLEEPCMD and SLEEPSTA bit definitions */
#define OSC_PD     BV(2)  /* 0: Both oscillators powered up and stable
                           * 1: oscillators not stable */

/* CLKCONCMD bit definitions */
#define OSC              BV(6)
#define TICKSPD(x)       (x << 3)
#define CLKSPD(x)        (x << 0)
#define CLKCONCMD_32MHZ  (0)
#define CLKCONCMD_16MHZ  (CLKSPD(1) | TICKSPD(1) | OSC)

/* STLOAD */
#define LDRDY            BV(0) /* Load Ready. This bit is 0 while the sleep timer
                                * loads the 24-bit compare value and 1 when the sleep
                                * timer is ready to start loading a newcompare value. */

/* All ISR that are used to wake up the chip shall have this macro called */
#ifdef POWER_SAVING
  #define CLEAR_SLEEP_MODE() st(SLEEPCMD &= ~PMODE;)  /* Wake up to Power Mode 0 */
#else
  #define CLEAR_SLEEP_MODE()
#endif

/**************************************************************************************************
 */
#endif
