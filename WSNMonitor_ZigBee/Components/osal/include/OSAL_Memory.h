/**************************************************************************************************
  Filename:       OSAL_Memory.h
  Revised:        $Date: 2009-07-14 08:43:23 -0700 (Tue, 14 Jul 2009) $
  Revision:       $Revision: 20305 $
    
  Description:    This module defines the OSAL memory control functions. 
    
            
  Copyright 2004-2007 Texas Instruments Incorporated. All rights reserved.

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

#ifndef OSAL_MEMORY_H
#define OSAL_MEMORY_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "comdef.h"
 
/*********************************************************************
 * CONSTANTS
 */

#if !defined ( OSALMEM_METRICS )
  #define OSALMEM_METRICS  FALSE
#endif

/*********************************************************************
 * MACROS
 */
  
#define osal_stack_used()  OnBoard_stack_used()

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
 
/*********************************************************************
 * FUNCTIONS
 */

 /*
  * Initialize memory manager.
  */
  void osal_mem_init( void );

 /*
  * Setup efficient search for the first free block of heap.
  */
  void osal_mem_kick( void );

 /*
  * Allocate a block of memory.
  */
  ONLY NULL_OK void *osal_mem_alloc( uint16 size );

 /*
  * Free a block of memory.
  */
  void osal_mem_free( ONLY void *ptr );

#if ( OSALMEM_METRICS )
 /*
  * Return the maximum number of blocks ever allocated at once.
  */
  uint16 osal_heap_block_max( void );

 /*
  * Return the current number of blocks now allocated.
  */
  uint16 osal_heap_block_cnt( void );

 /*
  * Return the current number of free blocks.
  */
  uint16 osal_heap_block_free( void );

 /*
  * Return the current number of bytes allocated.
  */
  uint16 osal_heap_mem_used( void );
#endif

#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
 /*
  * Return the highest number of bytes ever used in the heap.
  */
  uint16 osal_heap_high_water( void );
#endif

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* #ifndef OSAL_MEMORY_H */
