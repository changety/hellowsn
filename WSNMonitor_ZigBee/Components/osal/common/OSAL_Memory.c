/**************************************************************************************************
  Filename:       OSAL_Memory.c
  Revised:        $Date: 2009-08-06 14:51:59 -0700 (Thu, 06 Aug 2009) $
  Revision:       $Revision: 20491 $

  Description:    OSAL Heap Memory management functions.


  Copyright 2004-2009 Texas Instruments Incorporated. All rights reserved.

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

#include "comdef.h"
#include "OSAL.h"
#include "OSAL_Memory.h"
#include "OnBoard.h"
#include "hal_assert.h"

#if ( MAXMEMHEAP >= 32768 )
  #error MAXMEMHEAP is too big to manage!
#endif

// Minimum wasted bytes to justify splitting a block before allocation.
#if !defined ( OSALMEM_MIN_BLKSZ )
  #define OSALMEM_MIN_BLKSZ    4
#endif

/* Profiling memory allocations showed that a significant % of very high
 * frequency allocations/frees are for block sizes less than or equal to 16.
 */
#if !defined ( OSALMEM_SMALL_BLKSZ )
  #define OSALMEM_SMALL_BLKSZ  16
#endif

#if !defined ( OSALMEM_NODEBUG )
  #define OSALMEM_NODEBUG      TRUE
#endif

#if !defined ( OSALMEM_PROFILER )
  #define OSALMEM_PROFILER     FALSE
#endif

#if ( OSALMEM_PROFILER )
  #define OSALMEM_INIT   'X'
  #define OSALMEM_ALOC   'A'
  #define OSALMEM_REIN   'F'
#endif

/*********************************************************************
 * MACROS
 */

/*
 *  The MAC_ASSERT macro is for use during debugging.
 *  The given expression must evaluate as "true" or else fatal error occurs.
 *  At that point, the call stack feature of the debugger can pinpoint where
 *  the problem occurred.
 *
 *  To disable this feature and save code size, the project should define
 *  OSALMEM_NODEBUG to TRUE.
 */
#if ( OSALMEM_NODEBUG )
  #define OSALMEM_ASSERT( expr )
  #define OSALMEM_DEBUG( statement )
#else
  #define OSALMEM_ASSERT( expr)        HAL_ASSERT( expr )
  #define OSALMEM_DEBUG( statement)    st( statement )
#endif

/*********************************************************************
 * TYPEDEFS
 */

typedef uint16  osalMemHdr_t;

/*********************************************************************
 * CONSTANTS
 */

#define OSALMEM_IN_USE  0x8000

/* This number sets the size of the small-block bucket. Although profiling
 * shows max simultaneous alloc of 16x18, timing without profiling overhead
 * shows that the best worst case is achieved with the following.
 */
#define SMALLBLKHEAP    232

// To maintain data alignment of the pointer returned, reserve the greater
// space for the memory block header.
#define HDRSZ  ( (sizeof ( halDataAlign_t ) > sizeof( osalMemHdr_t )) ? \
                  sizeof ( halDataAlign_t ) : sizeof( osalMemHdr_t ) )

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static osalMemHdr_t *ff1;  // First free block in the small-block bucket.
static osalMemHdr_t *ff2;  // First free block after the small-block bucket.

#if ( OSALMEM_METRICS )
  static uint16 blkMax;  // Max cnt of all blocks ever seen at once.
  static uint16 blkCnt;  // Current cnt of all blocks.
  static uint16 blkFree; // Current cnt of free blocks.
  static uint16 memAlo;  // Current total memory allocated.
  static uint16 memMax;  // Max total memory ever allocated at once.
#endif

#if ( OSALMEM_PROFILER )
  #define OSALMEM_PROMAX  8
  /* The profiling buckets must differ by at least OSALMEM_MIN_BLKSZ; the
   * last bucket must equal the max alloc size. Set the bucket sizes to
   * whatever sizes necessary to show how your application is using memory.
   */
  static uint16 proCnt[OSALMEM_PROMAX] = {
    OSALMEM_SMALL_BLKSZ, 48, 112, 176, 192, 224, 256, 65535 };
  static uint16 proCur[OSALMEM_PROMAX] = { 0 };
  static uint16 proMax[OSALMEM_PROMAX] = { 0 };
  static uint16 proTot[OSALMEM_PROMAX] = { 0 };
  static uint16 proSmallBlkMiss;
#endif

// Memory Allocation Heap.
#if defined( EXTERNAL_RAM )
  static uint8 *theHeap = (uint8 *)EXT_RAM_BEG;
#else
  static halDataAlign_t _theHeap[ MAXMEMHEAP / sizeof( halDataAlign_t ) ];
  static uint8 *theHeap = (uint8 *)_theHeap;
#endif

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      osal_mem_init
 *
 * @brief   Initialize the heap memory management system.
 *
 * @param   void
 *
 * @return  void
 */
void osal_mem_init( void )
{
  osalMemHdr_t *tmp;

#if ( OSALMEM_PROFILER )
  (void)osal_memset( theHeap, OSALMEM_INIT, MAXMEMHEAP );
#endif

  // Setup a NULL block at the end of the heap for fast comparisons with zero.
  tmp = (osalMemHdr_t *)theHeap + (MAXMEMHEAP / HDRSZ) - 1;
  *tmp = 0;

  // Setup a small-block bucket.
  tmp = (osalMemHdr_t *)theHeap;
  *tmp = SMALLBLKHEAP;

  // Setup the wilderness.
  tmp = (osalMemHdr_t *)theHeap + (SMALLBLKHEAP / HDRSZ);
  *tmp = ((MAXMEMHEAP / HDRSZ) * HDRSZ) - SMALLBLKHEAP - HDRSZ;

  // Setup a NULL block that is never freed so that the small-block bucket
  // is never coalesced with the wilderness.
  ff1 = tmp;
  ff2 = osal_mem_alloc( 0 );
  ff1 = (osalMemHdr_t *)theHeap;

#if ( OSALMEM_METRICS )
  /* Start with the small-block bucket and the wilderness - don't count the
   * end-of-heap NULL block nor the end-of-small-block NULL block.
   */
  blkCnt = blkFree = 2;
#endif
}

/*********************************************************************
 * @fn      osal_mem_kick
 *
 * @brief   Kick the ff1 pointer out past the long-lived OSAL Task blocks.
 *          Invoke this once after all long-lived blocks have been allocated -
 *          presently at the end of osal_init_system().
 *
 * @param   void
 *
 * @return  void
 */
void osal_mem_kick( void )
{
  halIntState_t intState;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  /* Logic in osal_mem_free() will ratchet ff1 back down to the first free
   * block in the small-block bucket.
   */
  ff1 = ff2;

  HAL_EXIT_CRITICAL_SECTION( intState );  // Re-enable interrupts.
}

/*********************************************************************
 * @fn      osal_mem_alloc
 *
 * @brief   Implementation of the allocator functionality.
 *
 * @param   size - number of bytes to allocate from the heap.
 *
 * @return  void * - pointer to the heap allocation; NULL if error or failure.
 */
void *osal_mem_alloc( uint16 size )
{
  osalMemHdr_t *prev = NULL;
  osalMemHdr_t *hdr;
  halIntState_t intState;
  uint16 tmp;
  uint8 coal = 0;

  OSALMEM_ASSERT( size );

  size += HDRSZ;

  // Calculate required bytes to add to 'size' to align to halDataAlign_t.
  if ( sizeof( halDataAlign_t ) == 2 )
  {
    size += (size & 0x01);
  }
  else if ( sizeof( halDataAlign_t ) != 1 )
  {
    const uint8 mod = size % sizeof( halDataAlign_t );

    if ( mod != 0 )
    {
      size += (sizeof( halDataAlign_t ) - mod);
    }
  }

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  // Smaller allocations are first attempted in the small-block bucket.
  if ( size <= OSALMEM_SMALL_BLKSZ )
  {
    hdr = ff1;
  }
  else
  {
    hdr = ff2;
  }
  tmp = *hdr;

  do
  {
    if ( tmp & OSALMEM_IN_USE )
    {
      tmp ^= OSALMEM_IN_USE;
      coal = 0;
    }
    else
    {
      if ( coal != 0 )
      {
#if ( OSALMEM_METRICS )
        blkCnt--;
        blkFree--;
#endif

        *prev += *hdr;

        if ( *prev >= size )
        {
          hdr = prev;
          tmp = *hdr;
          break;
        }
      }
      else
      {
        if ( tmp >= size )
        {
          break;
        }

        coal = 1;
        prev = hdr;
      }
    }

    hdr = (osalMemHdr_t *)((uint8 *)hdr + tmp);

    tmp = *hdr;
    if ( tmp == 0 )
    {
      hdr = NULL;
      break;
    }


  } while ( 1 );

  if ( hdr != NULL )
  {
    tmp -= size;

    // Determine whether the threshold for splitting is met.
    if ( tmp >= OSALMEM_MIN_BLKSZ )
    {
      // Split the block before allocating it.
      osalMemHdr_t *next = (osalMemHdr_t *)((uint8 *)hdr + size);
      *next = tmp;
      *hdr = (size | OSALMEM_IN_USE);

#if ( OSALMEM_METRICS )
      blkCnt++;
      if ( blkMax < blkCnt )
      {
        blkMax = blkCnt;
      }
      memAlo += size;
#endif
    }
    else
    {
#if ( OSALMEM_METRICS )
      memAlo += *hdr;
      blkFree--;
#endif

      *hdr |= OSALMEM_IN_USE;
    }

#if ( OSALMEM_METRICS )
    if ( memMax < memAlo )
    {
      memMax = memAlo;
    }
#endif

#if ( OSALMEM_PROFILER )
  {
    uint8 idx;
    size = *hdr ^ OSALMEM_IN_USE;

    for ( idx = 0; idx < OSALMEM_PROMAX; idx++ )
    {
      if ( size <= proCnt[idx] )
      {
        break;
      }
    }
    proCur[idx]++;
    if ( proMax[idx] < proCur[idx] )
    {
      proMax[idx] = proCur[idx];
    }
    proTot[idx]++;
  }
#endif

    hdr++;

#if ( OSALMEM_PROFILER )
    (void)osal_memset( (uint8 *)hdr, OSALMEM_ALOC, (size - HDRSZ) );

    /* A small-block could not be allocated in the small-block bucket.
     * When this occurs significantly frequently, increase the size of the
     * bucket in order to restore better worst case run times. Set the first
     * profiling bucket size in proCnt[] to the small-block bucket size and
     * divide proSmallBlkMiss by the corresponding proTot[] size to get % miss.
     * Best worst case time on TrasmitApp was achieved at a 0-15% miss rate
     * during steady state Tx load, 0% during idle and steady state Rx load.
     */
    if ( (size <= OSALMEM_SMALL_BLKSZ) && (hdr > ff2) )
    {
      proSmallBlkMiss++;
    }
#endif
  }

  HAL_EXIT_CRITICAL_SECTION( intState );  // Re-enable interrupts.

  return (void *)hdr;
}

/*********************************************************************
 * @fn      osal_mem_free
 *
 * @brief   Implementation of the de-allocator functionality.
 *
 * @param   ptr - pointer to the memory to free.
 *
 * @return  void
 */
void osal_mem_free( void *ptr )
{
  osalMemHdr_t *currHdr;
  halIntState_t intState;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  HAL_ASSERT(((uint8 *)ptr >= (uint8 *)_theHeap) && ((uint8 *)ptr < (uint8 *)_theHeap+MAXMEMHEAP));

  currHdr = (osalMemHdr_t *)ptr - 1;

  HAL_ASSERT(*currHdr & OSALMEM_IN_USE);

  *currHdr &= ~OSALMEM_IN_USE;

#if ( OSALMEM_PROFILER )
  {
    uint16 size = *currHdr;
    uint8 idx;

    for ( idx = 0; idx < OSALMEM_PROMAX; idx++ )
    {
      if ( size <= proCnt[idx] )
      {
        break;
      }
    }

    proCur[idx]--;
  }
#endif

#if ( OSALMEM_METRICS )
  memAlo -= *currHdr;
  blkFree++;
#endif

  if ( ff1 > currHdr )
  {
    ff1 = currHdr;
  }

#if ( OSALMEM_PROFILER )
  (void)osal_memset( (uint8 *)currHdr+HDRSZ, OSALMEM_REIN, (*currHdr - HDRSZ) );
#endif

  HAL_EXIT_CRITICAL_SECTION( intState );  // Re-enable interrupts.
}

#if ( OSALMEM_METRICS )
/*********************************************************************
 * @fn      osal_heap_block_max
 *
 * @brief   Return the maximum number of blocks ever allocated at once.
 *
 * @param   none
 *
 * @return  Maximum number of blocks ever allocated at once.
 */
uint16 osal_heap_block_max( void )
{
  return blkMax;
}

/*********************************************************************
 * @fn      osal_heap_block_cnt
 *
 * @brief   Return the current number of blocks now allocated.
 *
 * @param   none
 *
 * @return  Current number of blocks now allocated.
 */
uint16 osal_heap_block_cnt( void )
{
  return blkCnt;
}

/*********************************************************************
 * @fn      osal_heap_block_free
 *
 * @brief   Return the current number of free blocks.
 *
 * @param   none
 *
 * @return  Current number of free blocks.
 */
uint16 osal_heap_block_free( void )
{
  return blkFree;
}

/*********************************************************************
 * @fn      osal_heap_mem_used
 *
 * @brief   Return the current number of bytes allocated.
 *
 * @param   none
 *
 * @return  Current number of bytes allocated.
 */
uint16 osal_heap_mem_used( void )
{
  return memAlo;
}
#endif

#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
/*********************************************************************
 * @fn      osal_heap_high_water
 *
 * @brief   Return the highest byte ever allocated in the heap.
 *
 * @param   none
 *
 * @return  Highest number of bytes ever used by the stack.
 */
uint16 osal_heap_high_water( void )
{
#if ( OSALMEM_METRICS )
  return memMax;
#else
  return MAXMEMHEAP;
#endif
}
#endif

/*********************************************************************
*********************************************************************/
