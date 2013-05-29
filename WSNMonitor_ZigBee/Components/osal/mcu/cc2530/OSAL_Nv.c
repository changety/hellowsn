/**************************************************************************************************
  Filename:       OSAL_Nv.c
  Revised:        $Date: 2008-08-06 17:17:11 -0700 (Wed, 06 Aug 2008) $
  Revision:       $Revision: 17715 $

  Description:    This module contains the OSAL non-volatile memory functions.


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
  Notes:
    - A trick buried deep in initPage() requires that the MSB of the NV Item Id is to
      be reserved for use by this module.
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "hal_adc.h"
#include "hal_flash.h"
#include "hal_types.h"
#include "ZComdef.h"
#include "OSAL_Nv.h"

/*********************************************************************
 * CONSTANTS
 */

#define OSAL_NV_PAGE_SIZE       HAL_FLASH_PAGE_SIZE
#define OSAL_NV_PAGES_USED      HAL_NV_PAGE_CNT
#define OSAL_NV_PAGE_BEG        HAL_NV_PAGE_BEG
#define OSAL_NV_PAGE_END       (OSAL_NV_PAGE_BEG + OSAL_NV_PAGES_USED - 1)

#define OSAL_NV_ACTIVE          0x00
#define OSAL_NV_ERASED          0xFF
#define OSAL_NV_ERASED_ID       0xFFFF
#define OSAL_NV_ZEROED_ID       0x0000
// Reserve MSB of Id to signal a search for the "old" source copy (new write interrupted/failed.)
#define OSAL_NV_SOURCE_ID       0x8000

#define OSAL_NV_PAGE_FREE       HAL_FLASH_PAGE_SIZE

// In case pages 0-1 are ever used, define a null page value.
#define OSAL_NV_PAGE_NULL       0

// In case item Id 0 is ever used, define a null item value.
#define OSAL_NV_ITEM_NULL       0

#define OSAL_NV_WORD_SIZE       HAL_FLASH_WORD_SIZE

#define OSAL_NV_PAGE_HDR_OFFSET 0

/*********************************************************************
 * MACROS
 */

#define  OSAL_NV_CHECK_BUS_VOLTAGE  (HalAdcCheckVdd( HAL_ADC_VDD_LIMIT_4 ))

#define OSAL_NV_DATA_SIZE( LEN )  \
   ((((LEN) + OSAL_NV_WORD_SIZE - 1) / OSAL_NV_WORD_SIZE) * OSAL_NV_WORD_SIZE)

#define OSAL_NV_ITEM_SIZE( LEN )  \
  (((((LEN) + OSAL_NV_WORD_SIZE - 1) / OSAL_NV_WORD_SIZE) * OSAL_NV_WORD_SIZE) + OSAL_NV_HDR_SIZE)

/*********************************************************************
 * TYPEDEFS
 */

typedef struct
{
  uint16 id;
  uint16 len;   // Enforce Flash-WORD size on len.
  uint16 chk;   // Byte-wise checksum of the 'len' data bytes of the item.
  uint16 stat;  // Item status.
} osalNvHdr_t;
// Struct member offsets.
#define OSAL_NV_HDR_ID    0
#define OSAL_NV_HDR_LEN   2
#define OSAL_NV_HDR_CHK   4
#define OSAL_NV_HDR_STAT  6

#define OSAL_NV_HDR_ITEM  2  // Length of any item of a header struct.
#define OSAL_NV_HDR_SIZE  8
#define OSAL_NV_HDR_HALF (OSAL_NV_HDR_SIZE / 2)

typedef struct
{
  uint16 active;
  uint16 inUse;
  uint16 xfer;
  uint16 spare;
} osalNvPgHdr_t;
// Struct member offsets.
#define OSAL_NV_PG_ACTIVE 0
#define OSAL_NV_PG_INUSE  2
#define OSAL_NV_PG_XFER   4
#define OSAL_NV_PG_SPARE  6

#define OSAL_NV_PAGE_HDR_SIZE  8
#define OSAL_NV_PAGE_HDR_HALF (OSAL_NV_PAGE_HDR_SIZE / 2)

typedef enum
{
  eNvXfer,
  eNvZero
} eNvHdrEnum;

typedef enum
{
  ePgActive,
  ePgInUse,
  ePgXfer,
  ePgSpare
} ePgHdrEnum;

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

extern bool HalAdcCheckVdd(uint8 limit);

/*********************************************************************
 * GLOBAL VARIABLES
 */

#ifndef OAD_KEEP_NV_PAGES
// When NV pages are to remain intact during OAD download,
// the image itself should not include NV pages.
#pragma location="ZIGNV_ADDRESS_SPACE"
__no_init uint8 _nvBuf[OSAL_NV_PAGES_USED * OSAL_NV_PAGE_SIZE];
#pragma required=_nvBuf
#pragma location="IEEE_ADDRESS_SPACE"
__no_init uint8 _nvIEEE[Z_EXTADDR_LEN];
#pragma required=_nvIEEE
#endif // OAD_KEEP_NV_PAGES

/*********************************************************************
 * LOCAL VARIABLES
 */

// Offset into the page of the first available erased space.
static uint16 pgOff[OSAL_NV_PAGES_USED];

// Count of the bytes lost for the zeroed-out items.
static uint16 pgLost[OSAL_NV_PAGES_USED];

static uint8 pgRes;  // Page reserved for item compacting transfer.

// Saving ~100 code bytes to move a uint8* parameter/return value from findItem() to a global.
static uint8 findPg;

/* Immediately before the voltage critical operations of a page erase or
 * a word write, check bus voltage. If less than min, set global flag & abort.
 * Since this is to be done at the lowest level, many void functions would have to be changed to
 * return a value and code added to check that value before proceeding, resulting in a very
 * expensive code size hit for implementing this properly. Therefore, use this global as follows:
 * at the start of osal_nv_item_init/osal_nv_write, set to FALSE, and at the end, before returning,
 * check the value. Thus, the global is an accumulator of any error that occurred in any of the
 * attempts to modify Flash with a low bus voltage during the complicated sequence of events that
 * may occur on any item init or write. This is much more expedient and code saving than adding
 * return values and checking return values to early out. No matter which method is used, an NV
 * data record may end up mangled due to the low VCC conditions. The strategy is that the headers
 * and checksums will detect and allow recovery from such a condition.
 *
 * One unfortunate side-effect of using the global fail flag vice adding and checking return
 * values, is that when setting an item Id to zero has failed due to the low VCC check,
 * the page lost-bytes counter is still updated. Having an artificially high lost-byte
 * count makes it look like there are more bytes to recover from compacting a page than there may
 * actually be. The easy work-around implemented is it to invoke initNV() from osal_nv_item_init or
 * osal_nv_write anytime that the failF gets set - this will re-walk all of the pages and set the
 * page offset count and page lost bytes count to their actual values.
 */
static uint8 failF;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static uint8  initNV( void );

static void   setPageUse( uint8 pg, uint8 inUse );
static uint16 initPage( uint8 pg, uint16 id, uint8 findDups );
static void   erasePage( uint8 pg );
static void   compactPage( uint8 pg );

static uint16 findItem( uint16 id );
static uint8  initItem( uint8 flag, uint16 id, uint16 len, void *buf );
static void   setItem( uint8 pg, uint16 offset, eNvHdrEnum stat );

static uint16 calcChkB( uint16 len, uint8 *buf );
static uint16 calcChkF( byte pg, uint16 offset, uint16 len );

static void   writeWord( uint8 pg, uint16 offset, uint8 *buf );
static void   writeWordH( uint8 pg, uint16 offset, uint8 *buf );
static void   writeWordM( uint8 pg, uint16 offset, uint8 *buf, uint16 cnt );
static void   writeBuf( uint8 pg, uint16 offset, uint16 len, uint8 *buf );
static void   xferBuf( uint8 srcPg, uint16 srcOff, uint8 dstPg, uint16 dstOff, uint16 len );

static uint8  writeItem( uint8 pg, uint16 id, uint16 len, void *buf, uint8 flag );

/*********************************************************************
 * @fn      initNV
 *
 * @brief   Initialize the NV flash pages.
 *
 * @param   none
 *
 * @return  TRUE
 */
static uint8 initNV( void )
{
  osalNvPgHdr_t pgHdr;
  uint8 oldPg = OSAL_NV_PAGE_NULL;
  uint8 newPg = OSAL_NV_PAGE_NULL;
  uint8 findDups = FALSE;
  uint8 pg;

  pgRes = OSAL_NV_PAGE_NULL;

  for ( pg = OSAL_NV_PAGE_BEG; pg <= OSAL_NV_PAGE_END; pg++ )
  {
    HalFlashRead(pg, OSAL_NV_PAGE_HDR_OFFSET, (uint8 *)(&pgHdr), OSAL_NV_HDR_SIZE);

    if ( pgHdr.active == OSAL_NV_ERASED_ID )
    {
      if ( pgRes == OSAL_NV_PAGE_NULL )
      {
        pgRes = pg;
      }
      else
      {
        setPageUse( pg, TRUE );
      }
    }
    else  // Page is active.
    {
      // If the page is not yet in use, it is the tgt of items from an xfer.
      if ( pgHdr.inUse == OSAL_NV_ERASED_ID )
      {
        newPg = pg;
      }
      // An Xfer from this page was in progress.
      else if ( pgHdr.xfer != OSAL_NV_ERASED_ID )
      {
        oldPg = pg;
      }
    }

    // Calculate page offset and lost bytes - any "old" item triggers an N^2 re-scan from start.
    if ( initPage( pg, OSAL_NV_ITEM_NULL, findDups ) != OSAL_NV_ITEM_NULL )
    {
      findDups = TRUE;
      pg = OSAL_NV_PAGE_BEG-1;
      continue;
    }
  }  // for ( pg = OSAL_NV_PAGE_BEG; pg <= OSAL_NV_PAGE_END; pg++ )

  /* First the old page is erased, and then the new page is put into use.
   * So if a transfer was in progress, the new page will always not yet be
   * marked as in use, since that is the last step to ending a transfer.
   */
  if ( newPg != OSAL_NV_PAGE_NULL )
  {
    /* If there is already a fallow page reserved, keep it and put the newPg in use.
     * An unfinished compaction will finish to the new reserve page and the old page
     * will be erased and reserved.
     */
    if ( pgRes != OSAL_NV_PAGE_NULL )
    {
      setPageUse( newPg, TRUE );
    }
    else if ( oldPg != OSAL_NV_PAGE_NULL )
    {
      pgRes = newPg;
    }

    /* If a page compaction was interrupted and the page being compacted is not
     * yet erased, then there may be items remaining to xfer before erasing.
     */
    if ( oldPg != OSAL_NV_PAGE_NULL )
    {
      compactPage( oldPg );
    }
  }

  /* If no page met the criteria to be the reserve page:
   *  - A compactPage() failed or board reset before doing so.
   *  - Perhaps the user changed which Flash pages are dedicated to NV and downloaded the code
   *    without erasing Flash?
   */
  if ( pgRes == OSAL_NV_PAGE_NULL )
  {
    for ( pg = OSAL_NV_PAGE_BEG; pg <= OSAL_NV_PAGE_END; pg++ )
    {
      erasePage( pg );
    }
    initNV();
  }

  return TRUE;
}

/*********************************************************************
 * @fn      setPageUse
 *
 * @brief   Set page header active/inUse state according to 'inUse'.
 *
 * @param   pg - Valid NV page to verify and init.
 * @param   inUse - Boolean TRUE if inUse, FALSE if only active.
 *
 * @return  none
 */
static void setPageUse( uint8 pg, uint8 inUse )
{
  osalNvPgHdr_t pgHdr;

  pgHdr.active = OSAL_NV_ZEROED_ID;

  if ( inUse )
  {
    pgHdr.inUse = OSAL_NV_ZEROED_ID;
  }
  else
  {
    pgHdr.inUse = OSAL_NV_ERASED_ID;
  }

  writeWord( pg, OSAL_NV_PAGE_HDR_OFFSET, (uint8*)(&pgHdr) );
}

/*********************************************************************
 * @fn      initPage
 *
 * @brief   Walk the page items; calculate checksums, lost bytes & page offset.
 *
 * @param   pg - Valid NV page to verify and init.
 * @param   id - Valid NV item Id to use function as a "findItem".
 *               If set to NULL then just perform the page initialization.
 *
 * @return  If 'id' is non-NULL and good checksums are found, return the offset
 *          of the data corresponding to item Id; else OSAL_NV_ITEM_NULL.
 */
static uint16 initPage( uint8 pg, uint16 id, uint8 findDups )
{
  uint16 offset = OSAL_NV_PAGE_HDR_SIZE;
  uint16 sz, lost = 0;
  osalNvHdr_t hdr;

  do
  {
    HalFlashRead(pg, offset, (uint8 *)(&hdr), OSAL_NV_HDR_SIZE);

    if ( hdr.id == OSAL_NV_ERASED_ID )
    {
      break;
    }
    offset += OSAL_NV_HDR_SIZE;
    sz = OSAL_NV_DATA_SIZE( hdr.len );

    // A bad 'len' write has blown away the rest of the page.
    if ( (offset + sz) > OSAL_NV_PAGE_FREE )
    {
      lost += (OSAL_NV_PAGE_FREE - offset + OSAL_NV_HDR_SIZE);
      offset = OSAL_NV_PAGE_FREE;
      break;
    }

    if ( hdr.id != OSAL_NV_ZEROED_ID )
    {
      /* This trick allows function to do double duty for findItem() without
       * compromising its essential functionality at powerup initialization.
       */
      if ( id != OSAL_NV_ITEM_NULL )
      {
        /* This trick allows asking to find the old/transferred item in case
         * of a successful new item write that gets interrupted before the
         * old item can be zeroed out.
         */
        if ( (id & 0x7fff) == hdr.id )
        {
          if ( (((id & OSAL_NV_SOURCE_ID) == 0) && (hdr.stat == OSAL_NV_ERASED_ID)) ||
               (((id & OSAL_NV_SOURCE_ID) != 0) && (hdr.stat != OSAL_NV_ERASED_ID)) )
          {
            return offset;
          }
        }
      }
      // When invoked from the osal_nv_init(), verify checksums and find & zero any duplicates.
      else
      {
        if ( hdr.chk == calcChkF( pg, offset, hdr.len ) )
        {
          if ( findDups )
          {
            if ( hdr.stat == OSAL_NV_ERASED_ID )
            {
              /* The trick of setting the MSB of the item Id causes the logic
               * immediately above to return a valid page only if the header 'stat'
               * indicates that it was the older item being transferred.
               */
              uint16 off = findItem( (hdr.id | OSAL_NV_SOURCE_ID) );

              if ( off != OSAL_NV_ITEM_NULL )
              {
                setItem( findPg, off, eNvZero );  // Mark old duplicate as invalid.
              }
            }
          }
          // Any "old" item immediately exits and triggers the N^2 exhaustive initialization.
          else if ( hdr.stat != OSAL_NV_ERASED_ID )
          {
            return OSAL_NV_ERASED_ID;
          }
        }
        else
        {
          setItem( pg, offset, eNvZero );  // Mark bad checksum as invalid.
          lost += (OSAL_NV_HDR_SIZE + sz);
        }
      }
    }
    else
    {
      lost += (OSAL_NV_HDR_SIZE + sz);
    }
    offset += sz;

  } while ( TRUE );

  pgOff[pg - OSAL_NV_PAGE_BEG] = offset;
  pgLost[pg - OSAL_NV_PAGE_BEG] = lost;

  return OSAL_NV_ITEM_NULL;
}

/*********************************************************************
 * @fn      erasePage
 *
 * @brief   Erases a page in Flash.
 *
 * @param   pg - Valid NV page to erase.
 *
 * @return  none
 */
static void erasePage( uint8 pg )
{
  if ( !OSAL_NV_CHECK_BUS_VOLTAGE )
  {
    failF = TRUE;
    return;
  }

  HalFlashErase(pg);

  pgOff[pg - OSAL_NV_PAGE_BEG] = OSAL_NV_PAGE_HDR_SIZE;
  pgLost[pg - OSAL_NV_PAGE_BEG] = 0;
}

/*********************************************************************
 * @fn      compactPage
 *
 * @brief   Compacts the page specified.
 *
 * @param   srcPg - Valid NV page to erase.
 *
 * @return  none
 */
static void compactPage( uint8 srcPg )
{
  uint16 dstOff = pgOff[pgRes-OSAL_NV_PAGE_BEG];
  uint16 srcOff = OSAL_NV_ZEROED_ID;
  osalNvHdr_t hdr;

  // Mark page as being in process of compaction.
  writeWordH( srcPg, OSAL_NV_PG_XFER, (uint8*)(&srcOff) );

  srcOff = OSAL_NV_PAGE_HDR_SIZE;

  do
  {
    uint16 sz;
    HalFlashRead(srcPg, srcOff, (uint8 *)(&hdr), OSAL_NV_HDR_SIZE);

    if ( hdr.id == OSAL_NV_ERASED_ID )
    {
      break;
    }

    srcOff += OSAL_NV_HDR_SIZE;

    if ( (srcOff + hdr.len) > OSAL_NV_PAGE_FREE )
    {
      break;
    }

    sz = OSAL_NV_DATA_SIZE( hdr.len );

    if ( hdr.id != OSAL_NV_ZEROED_ID )
    {
      if ( hdr.chk == calcChkF( srcPg, srcOff, hdr.len ) )
      {
        setItem( srcPg, srcOff, eNvXfer );
        writeBuf( pgRes, dstOff, OSAL_NV_HDR_SIZE, (byte *)(&hdr) );
        dstOff += OSAL_NV_HDR_SIZE;
        xferBuf( srcPg, srcOff, pgRes, dstOff, sz );
        dstOff += sz;
      }

      setItem( srcPg, srcOff, eNvZero );  // Mark old location as invalid.
    }

    srcOff += sz;

  } while ( TRUE );

  pgOff[pgRes-OSAL_NV_PAGE_BEG] = dstOff;

  /* In order to recover from a page compaction that is interrupted,
   * the logic in osal_nv_init() depends upon the following order:
   * 1. Compacted page is erased.
   * 2. State of the target of compaction is changed ePgActive to ePgInUse.
   */
  erasePage( srcPg );

  // Mark the reserve page as being in use.
  setPageUse( pgRes, TRUE );

  // Set the reserve page to be the newly erased page.
  pgRes = srcPg;
}

/*********************************************************************
 * @fn      findItem
 *
 * @brief   Find an item Id in NV and return the page and offset to its data.
 *
 * @param   id - Valid NV item Id.
 *
 * @return  Offset of data corresponding to item Id, if found;
 *          otherwise OSAL_NV_ITEM_NULL.
 *
 *          The page containing the item, if found;
 *          otherwise no valid assignment made - left equal to item Id.
 *
 */
static uint16 findItem( uint16 id )
{
  uint16 off;
  uint8 pg;

  for ( pg = OSAL_NV_PAGE_BEG; pg <= OSAL_NV_PAGE_END; pg++ )
  {
    if ( (off = initPage( pg, id, FALSE )) != OSAL_NV_ITEM_NULL )
    {
      findPg = pg;
      return off;
    }
  }

  // Now attempt to find the item as the "old" item of a failed/interrupted NV write.
  if ( (id & OSAL_NV_SOURCE_ID) == 0 )
  {
    return findItem( id | OSAL_NV_SOURCE_ID );
  }
  else
  {
    findPg = OSAL_NV_PAGE_NULL;
    return OSAL_NV_ITEM_NULL;
  }
}

/*********************************************************************
 * @fn      initItem
 *
 * @brief   An NV item is created and initialized with the data passed to the function, if any.
 *
 * @param   id  - Valid NV item Id.
 * @param   len - Item data length.
 * @param  *buf - Pointer to item initalization data. Set to NULL if none.
 *
 * @return  TRUE if item write and read back checksums ok; FALSE otherwise.
 */
static uint8 initItem( uint8 flag, uint16 id, uint16 len, void *buf )
{
  uint16 sz = OSAL_NV_ITEM_SIZE( len );
  uint8 rtrn = FALSE;
  uint8 cnt = OSAL_NV_PAGES_USED;
  uint8 pg = pgRes+1;  // Set to 1 after the reserve page to even wear across all available pages.
  uint8 idx = 0;

  do {
    if (pg >= OSAL_NV_PAGE_BEG+OSAL_NV_PAGES_USED)
    {
      pg = OSAL_NV_PAGE_BEG;
    }
    if ( pg != pgRes )
    {
      idx = pg - OSAL_NV_PAGE_BEG;
      if ( (pgOff[idx] - pgLost[idx] + sz) <= OSAL_NV_PAGE_FREE )
      {
        break;
      }
    }
    pg++;
  } while (--cnt);

  if (cnt)
  {
    // Item fits if an old page is compacted.
    if ( (pgOff[idx] + sz) > OSAL_NV_PAGE_FREE )
    {
      pg = pgRes;
    }

    // New item is the first one written to the reserved page, then the old page is compacted.
    rtrn = writeItem( pg, id, len, buf, flag );

    if ( pg == pgRes )
    {
      if ( flag )
      {
        compactPage( OSAL_NV_PAGE_BEG+idx );
      }
      else
      {
        *(uint8 *)buf = OSAL_NV_PAGE_BEG+idx;
      }
    }
  }

  return rtrn;
}

/*********************************************************************
 * @fn      setItem
 *
 * @brief   Set an item Id or status to mark its state.
 *
 * @param   pg - Valid NV page.
 * @param   offset - Valid offset into the page of the item data - the header
 *                   offset is calculated from this.
 * @param   stat - Valid enum value for the item status.
 *
 * @return  none
 */
static void setItem( uint8 pg, uint16 offset, eNvHdrEnum stat )
{
  osalNvHdr_t hdr;

  offset -= OSAL_NV_HDR_SIZE;
  HalFlashRead(pg, offset, (uint8 *)(&hdr), OSAL_NV_HDR_SIZE);

  if ( stat == eNvXfer )
  {
    hdr.stat = OSAL_NV_ACTIVE;
    writeWord( pg, offset+OSAL_NV_HDR_CHK, (uint8*)(&(hdr.chk)) );
}
  else // if ( stat == eNvZero )
  {
    uint16 sz = ((hdr.len + (OSAL_NV_WORD_SIZE-1)) / OSAL_NV_WORD_SIZE) * OSAL_NV_WORD_SIZE +
                                                                          OSAL_NV_HDR_SIZE;
    hdr.id = 0;
    writeWord( pg, offset, (uint8 *)(&hdr) );
    pgLost[pg-OSAL_NV_PAGE_BEG] += sz;
  }
}

/*********************************************************************
 * @fn      calcChkB
 *
 * @brief   Calculates the data checksum over the 'buf' parameter.
 *
 * @param   pg - A valid NV Flash page.
 * @param   offset - A valid offset into the page.
 * @param   len - Byte count of the data to be checksummed.
 *
 * @return  Calculated checksum of the data bytes.
 */
static uint16 calcChkB( uint16 len, uint8 *buf )
{
  uint8 fill = len % OSAL_NV_WORD_SIZE;
  uint16 chk;

  if ( !buf )
  {
    chk = len * OSAL_NV_ERASED;
  }
  else
  {
    chk = 0;
    while ( len-- )
    {
      chk += *buf++;
    }
  }

  // calcChkF() will calculate over OSAL_NV_WORD_SIZE alignment.
  if ( fill )
  {
    chk += (OSAL_NV_WORD_SIZE - fill) * OSAL_NV_ERASED;
  }

  return chk;
}

/*********************************************************************
 * @fn      calcChkF
 *
 * @brief   Calculates the data checksum by reading the data bytes from NV.
 *
 * @param   pg - A valid NV Flash page.
 * @param   offset - A valid offset into the page.
 * @param   len - Byte count of the data to be checksummed.
 *
 * @return  Calculated checksum of the data bytes.
 */
static uint16 calcChkF( byte pg, uint16 offset, uint16 len )
{
  uint16 chk = 0;

  len = (len + (OSAL_NV_WORD_SIZE-1)) / OSAL_NV_WORD_SIZE;

  while ( len-- )
  {
    uint8 cnt, tmp[OSAL_NV_WORD_SIZE];

    HalFlashRead(pg, offset, tmp, OSAL_NV_WORD_SIZE);
    offset += OSAL_NV_WORD_SIZE;

    for ( cnt = 0; cnt < OSAL_NV_WORD_SIZE; cnt++ )
    {
      chk += tmp[cnt];
    }
  }

  return chk;
}

/*********************************************************************
 * @fn      writeWord
 *
 * @brief   Writes a Flash-WORD to NV.
 *
 * @param   pg - A valid NV Flash page.
 * @param   offset - A valid offset into the page.
 * @param   buf - Pointer to source buffer.
 *
 * @return  none
 */
static void writeWord( uint8 pg, uint16 offset, uint8 *buf )
{
  offset = (offset >> 2) + ((uint16)pg << 9);

  if ( OSAL_NV_CHECK_BUS_VOLTAGE )
  {
    HalFlashWrite(offset, buf, 1);
  }
  else
  {
    failF = TRUE;
  }
}

/*********************************************************************
 * @fn      writeWordM
 *
 * @brief   Writes multiple Flash-WORDs to NV.
 *
 * @param   pg - A valid NV Flash page.
 * @param   offset - A valid offset into the page.
 * @param   buf - Pointer to source buffer.
 * @param   cnt - Number of 4-byte blocks to write.
 *
 * @return  none
 */
static void writeWordM( uint8 pg, uint16 offset, uint8 *buf, uint16 cnt )
{
  offset = (offset >> 2) + ((uint16)pg << 9);

  if ( OSAL_NV_CHECK_BUS_VOLTAGE )
  {
    HalFlashWrite(offset, buf, cnt);
  }
  else
  {
    failF = TRUE;
  }
}

/*********************************************************************
 * @fn      writeWordH
 *
 * @brief   Writes the 1st half of a Flash-WORD to NV (filling 2nd half with 0xffff).
 *
 * @param   pg - A valid NV Flash page.
 * @param   offset - A valid offset into the page.
 * @param   buf - Pointer to source buffer.
 *
 * @return  none
 */
static void writeWordH( uint8 pg, uint16 offset, uint8 *buf )
{
  uint8 tmp[4];

  tmp[0] = buf[0];
  tmp[1] = buf[1];
  tmp[2] = OSAL_NV_ERASED;
  tmp[3] = OSAL_NV_ERASED;

  writeWord( pg, offset, tmp );
}

/*********************************************************************
 * @fn      writeBuf
 *
 * @brief   Writes a data buffer to NV.
 *
 * @param   dstPg - A valid NV Flash page.
 * @param   offset - A valid offset into the page.
 * @param   len  - Byte count of the data to write.
 * @param   buf  - The data to write.
 *
 * @return  TRUE if data buf checksum matches read back checksum, else FALSE.
 */
static void writeBuf( uint8 dstPg, uint16 dstOff, uint16 len, uint8 *buf )
{
  uint8 rem = dstOff % OSAL_NV_WORD_SIZE;
  uint8 tmp[OSAL_NV_WORD_SIZE];

  if ( rem )
  {
    dstOff = (dstOff / OSAL_NV_WORD_SIZE) * OSAL_NV_WORD_SIZE;
    HalFlashRead(dstPg, dstOff, tmp, OSAL_NV_WORD_SIZE);

    while ( (rem < OSAL_NV_WORD_SIZE) && len )
    {
      tmp[rem++] = *buf++;
      len--;
    }

    writeWord( dstPg, dstOff, tmp );
    dstOff += OSAL_NV_WORD_SIZE;
  }

  rem = len % OSAL_NV_WORD_SIZE;
  len /= OSAL_NV_WORD_SIZE;

  if ( len )
  {
    writeWordM( dstPg, dstOff, buf, len );
    dstOff += OSAL_NV_WORD_SIZE * len;
    buf += OSAL_NV_WORD_SIZE * len;
  }

  if ( rem )
  {
    uint8 idx = 0;
    HalFlashRead(dstPg, dstOff, tmp, OSAL_NV_WORD_SIZE);
    while ( rem-- )
    {
      tmp[idx++] = *buf++;
    }
    writeWord( dstPg, dstOff, tmp );
  }
}

/*********************************************************************
 * @fn      xferBuf
 *
 * @brief   Xfers an NV buffer from one location to another, enforcing OSAL_NV_WORD_SIZE writes.
 *
 * @return  none
 */
static void xferBuf( uint8 srcPg, uint16 srcOff, uint8 dstPg, uint16 dstOff, uint16 len )
{
  uint8 rem = dstOff % OSAL_NV_WORD_SIZE;
  uint8 tmp[OSAL_NV_WORD_SIZE];

  if ( rem )
  {
    dstOff -= rem;
    HalFlashRead(dstPg, dstOff, tmp, OSAL_NV_WORD_SIZE);

    while ( (rem < OSAL_NV_WORD_SIZE) && len )
    {
      HalFlashRead(srcPg, srcOff, tmp+rem, 1);
      srcOff++;
      rem++;
      len--;
    }

    writeWord( dstPg, dstOff, tmp );
    dstOff += OSAL_NV_WORD_SIZE;
  }

  rem = len % OSAL_NV_WORD_SIZE;
  len /= OSAL_NV_WORD_SIZE;

  while ( len-- )
  {
    HalFlashRead(srcPg, srcOff, tmp, OSAL_NV_WORD_SIZE);
    srcOff += OSAL_NV_WORD_SIZE;
    writeWord( dstPg, dstOff, tmp );
    dstOff += OSAL_NV_WORD_SIZE;
  }

  if ( rem )
  {
    uint8 idx = 0;
    HalFlashRead(dstPg, dstOff, tmp, OSAL_NV_WORD_SIZE);
    while ( rem-- )
    {
      HalFlashRead(srcPg, srcOff, tmp+idx, 1);
      srcOff++;
      idx++;
    }
    writeWord( dstPg, dstOff, tmp );
  }
}

/*********************************************************************
 * @fn      writeItem
 *
 * @brief   Writes an item header/data combo to the specified NV page.
 *
 * @param   pg - Valid NV Flash page.
 * @param   id - Valid NV item Id.
 * @param   len  - Byte count of the data to write.
 * @param   buf  - The data to write. If NULL, no data/checksum write.
 * @param   flag - TRUE if the checksum should be written, FALSE otherwise.
 *
 * @return  TRUE if header/data to write matches header/data read back, else FALSE.
 */
static uint8 writeItem( uint8 pg, uint16 id, uint16 len, void *buf, uint8 flag )
{
  uint16 offset = pgOff[pg-OSAL_NV_PAGE_BEG];
  uint8 rtrn = FALSE;
  osalNvHdr_t hdr;

  if ( pg == pgRes )
  {
    /* Mark reserve page as being active, in process of receiving items.
     * Invoking function must effect a page compaction.
     */
    setPageUse( pg, FALSE );
  }

  hdr.id = id;
  hdr.len = len;

  writeWord( pg, offset, (uint8 *)&hdr );
  HalFlashRead(pg, offset, (uint8 *)(&hdr), OSAL_NV_HDR_SIZE);

  if ( (hdr.id == id) && (hdr.len == len) )
  {
    if ( flag )
    {
      uint16 chk = calcChkB( len, buf );

      offset += OSAL_NV_HDR_SIZE;
      if ( buf != NULL )
      {
        writeBuf( pg, offset, len, buf );
      }

      if ( chk == calcChkF( pg, offset, len ) )
      {
        writeWordH(pg, (offset-OSAL_NV_WORD_SIZE), (uint8 *)&chk);
        HalFlashRead(pg, (offset-OSAL_NV_HDR_SIZE), (uint8 *)(&hdr), OSAL_NV_HDR_SIZE);

        if ( chk == hdr.chk )
        {
          rtrn = pg;
        }
      }
    }
    else
    {
      rtrn = pg;
    }
  }

  len = OSAL_NV_ITEM_SIZE( hdr.len );
  pgOff[pg-OSAL_NV_PAGE_BEG] += len;

  return rtrn;
}

/*********************************************************************
 * @fn      osal_nv_init
 *
 * @brief   Initialize NV service.
 *
 * @param   p - Not used.
 *
 * @return  none
 */
void osal_nv_init( void *p )
{
  (void)p;  // Suppress Lint warning.
  (void)initNV();  // Always returns TRUE after pages have been erased.
}

/*********************************************************************
 * @fn      osal_nv_item_init
 *
 * @brief   If the NV item does not already exist, it is created and
 *          initialized with the data passed to the function, if any.
 *          This function must be called before calling osal_nv_read() or
 *          osal_nv_write().
 *
 * @param   id  - Valid NV item Id.
 * @param   len - Item length.
 * @param  *buf - Pointer to item initalization data. Set to NULL if none.
 *
 * @return  NV_ITEM_UNINIT - Id did not exist and was created successfully.
 *          ZSUCCESS       - Id already existed, no action taken.
 *          NV_OPER_FAILED - Failure to find or create Id.
 */
uint8 osal_nv_item_init( uint16 id, uint16 len, void *buf )
{
  /* Global fail flag for fail due to low bus voltage has less impact on code
   * size than passing back a return value all the way from the lowest level.
   */
  failF = FALSE;

  if ( (findItem( id ) != OSAL_NV_ITEM_NULL) )
  {
    return ZSUCCESS;
  }
  else if ( initItem( TRUE, id, len, buf ) )
  {
    if ( failF )
    {
      (void)initNV();  // See comment at the declaration of failF.
      return NV_OPER_FAILED;
    }
    else
    {
      return NV_ITEM_UNINIT;
    }
  }
  else
  {
    return NV_OPER_FAILED;
  }
}

/*********************************************************************
 * @fn      osal_nv_item_len
 *
 * @brief   Get the data length of the item stored in NV memory.
 *
 * @param   id  - Valid NV item Id.
 *
 * @return  Item length, if found; zero otherwise.
 */
uint16 osal_nv_item_len( uint16 id )
{
  uint16 offset = findItem( id );

  if ( offset == OSAL_NV_ITEM_NULL )
  {
    return 0;
  }
  else
  {
    osalNvHdr_t hdr;
    HalFlashRead(findPg, (offset - OSAL_NV_HDR_SIZE), (uint8 *)(&hdr), OSAL_NV_HDR_SIZE);
    return hdr.len;
  }
}

/*********************************************************************
 * @fn      osal_nv_write
 *
 * @brief   Write a data item to NV. Function can write an entire item to NV or
 *          an element of an item by indexing into the item with an offset.
 *
 * @param   id  - Valid NV item Id.
 * @param   ndx - Index offset into item
 * @param   len - Length of data to write.
 * @param  *buf - Data to write.
 *
 * @return  ZSUCCESS if successful, NV_ITEM_UNINIT if item did not
 *          exist in NV and offset is non-zero, NV_OPER_FAILED if failure.
 */
uint8 osal_nv_write( uint16 id, uint16 ndx, uint16 len, void *buf )
{
  uint8 rtrn = ZSUCCESS;

  /* Global fail flag for fail due to low bus voltage has less impact on code
   * size than passing back a return value all the way from the lowest level.
   */
  failF = FALSE;

  if ( len != 0 )
  {
    osalNvHdr_t hdr;
    uint16 origOff, srcOff;
    uint16 cnt;
    uint8 *ptr;

    origOff = srcOff = findItem( id );
    if ( srcOff == OSAL_NV_ITEM_NULL )
    {
      return NV_ITEM_UNINIT;
    }

    HalFlashRead(findPg, (srcOff - OSAL_NV_HDR_SIZE), (uint8 *)(&hdr), OSAL_NV_HDR_SIZE);
    if ( hdr.len < (ndx + len) )
    {
      return NV_OPER_FAILED;
    }

    srcOff += ndx;
    ptr = buf;
    cnt = len;
    while ( cnt )
    {
      uint8 tmp;
      HalFlashRead(findPg, srcOff++, &tmp, 1);
      if ( tmp != *ptr++ )
      {
        break;
      }
      cnt--;
    }

    if ( cnt != 0 )  // If the buffer to write is different in one or more bytes.
    {
      uint8 comPg, dstPg = initItem( FALSE, id, hdr.len, &comPg );

      if ( dstPg != OSAL_NV_PAGE_NULL )
      {
        uint16 tmp = OSAL_NV_DATA_SIZE( hdr.len );
        uint16 dstOff = pgOff[dstPg-OSAL_NV_PAGE_BEG] - tmp;
        uint8 srcPg = findPg;
        srcOff = origOff;

        setItem( srcPg, srcOff, eNvXfer );

        xferBuf( srcPg, srcOff, dstPg, dstOff, ndx );
        srcOff += ndx;
        dstOff += ndx;

        writeBuf( dstPg, dstOff, len, buf );
        srcOff += len;
        dstOff += len;

        xferBuf( srcPg, srcOff, dstPg, dstOff, (hdr.len-ndx-len) );

        // Calculate and write the new checksum.
        dstOff = pgOff[dstPg-OSAL_NV_PAGE_BEG] - tmp;
        tmp = calcChkF( dstPg, dstOff, hdr.len );
        dstOff -= OSAL_NV_HDR_SIZE;
        writeWordH( dstPg, (dstOff+OSAL_NV_HDR_CHK), (uint8 *)&tmp );
        HalFlashRead(dstPg, dstOff, (uint8 *)(&hdr), OSAL_NV_HDR_SIZE);

        if ( tmp == hdr.chk )
        {
          setItem( srcPg, origOff, eNvZero );
        }
        else
        {
          rtrn = NV_OPER_FAILED;
        }

        if ( dstPg == pgRes )
        {
          compactPage( comPg );
        }
      }
      else
      {
        rtrn = NV_OPER_FAILED;
      }
    }
  }

  if ( failF )
  {
    (void)initNV();  // See comment at the declaration of failF.
    rtrn = NV_OPER_FAILED;
  }

  return rtrn;
}

/*********************************************************************
 * @fn      osal_nv_read
 *
 * @brief   Read data from NV. This function can be used to read an entire item from NV or
 *          an element of an item by indexing into the item with an offset.
 *          Read data is copied into *buf.
 *
 * @param   id  - Valid NV item Id.
 * @param   ndx - Index offset into item
 * @param   len - Length of data to read.
 * @param  *buf - Data is read into this buffer.
 *
 * @return  ZSUCCESS if NV data was copied to the parameter 'buf'.
 *          Otherwise, NV_OPER_FAILED for failure.
 */
uint8 osal_nv_read( uint16 id, uint16 ndx, uint16 len, void *buf )
{
  uint16 offset;

  offset = findItem( id );
  if ( offset == OSAL_NV_ITEM_NULL )
  {
    return NV_OPER_FAILED;
  }

  HalFlashRead(findPg, offset+ndx, buf, len);

  return ZSUCCESS;
}

/*********************************************************************
*********************************************************************/
