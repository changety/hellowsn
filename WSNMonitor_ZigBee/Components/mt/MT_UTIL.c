/***************************************************************************************************
  Filename:       MT_UTIL.c
  Revised:        $Date: 2009-12-29 18:31:22 -0800 (Tue, 29 Dec 2009) $
  Revision:       $Revision: 21416 $

  Description:    MonitorTest Utility Functions

  Copyright 2007 - 2009 Texas Instruments Incorporated. All rights reserved.

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

 ***************************************************************************************************/

/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include "ZComDef.h"

#include "AddrMgr.h"
#include "AssocList.h"
#include "OnBoard.h"   /* This is here because of the key reading */
#include "hal_key.h"
#include "hal_led.h"
#include "OSAL_Nv.h"
#include "NLMEDE.h"
#include "ZDApp.h"
#include "MT.h"
#include "MT_UTIL.h"
#include "MT_ZDO.h"
#include "MT_SAPI.h"
#include "MT_NWK.h"
#include "MT_AF.h"
#include "MT_MAC.h"
#if defined ZCL_KEY_ESTABLISH
#include "zcl_key_establish.h"
#endif
#if defined TC_LINKKEY_JOIN
#include "zcl_se.h"
#endif

/***************************************************************************************************
 * CONSTANTS
 ***************************************************************************************************/
#define MT_UTIL_DEVICE_INFO_RESPONSE_LEN 14

/***************************************************************************************************
 * LOCAL VARIABLES
 ***************************************************************************************************/
#if defined ZCL_KEY_ESTABLISH
uint8 zcl_key_establish_task_id;
#endif

/***************************************************************************************************
 * LOCAL FUNCTIONS
 ***************************************************************************************************/
#if defined (MT_UTIL_FUNC)
void MT_UtilGetDeviceInfo(void);
void MT_UtilGetNvInfo(void);
void MT_UtilSetPanID(uint8 *pBuf);
void MT_UtilSetChannels(uint8 *pBuf);
void MT_UtilSetSecLevel(uint8 *pBuf);
void MT_UtilSetPreCfgKey(uint8 *pBuf);
void MT_UtilCallbackSub(uint8 *pData);
void MT_UtilKeyEvent(uint8 *pBuf);
void MT_UtilTimeAlive(void);
void MT_UtilLedControl(uint8 *pBuf);
static void MT_UtilAddrMgrExtAddrLookup(uint8 *pBuf);
static void MT_UtilAddrMgrEntryLookupNwk(uint8 *pBuf);
static void MT_UtilAPSME_LinkKeyDataGet(uint8 *pBuf);
static void MT_UtilAssocCount(uint8 *pBuf);
static void MT_UtilAssocFindDevice(uint8 *pBuf);
static void MT_UtilAssocGetWithAddress(uint8 *pBuf);
#if defined ZCL_KEY_ESTABLISH
static void MT_UtilzclGeneral_KeyEstablish_InitiateKeyEstablishment(uint8 *pBuf);
static void MT_UtilzclGeneral_KeyEstablishment_ECDSASign(uint8 *pBuf);
#endif
static void MT_UtilSync(void);
#endif /* MT_UTIL_FUNC */

#if defined (MT_UTIL_FUNC)
/***************************************************************************************************
 * @fn      MT_UtilProcessing
 *
 * @brief   Process all the DEBUG commands that are issued by test tool
 *
 * @param   pBuf  - pointer to received SPI data message
 *
 * @return  status
 ***************************************************************************************************/
uint8 MT_UtilCommandProcessing(uint8 *pBuf)
{
  uint8 status = MT_RPC_SUCCESS;

  switch (pBuf[MT_RPC_POS_CMD1])
  {
    case MT_UTIL_GET_DEVICE_INFO:
      MT_UtilGetDeviceInfo();
      break;

    case MT_UTIL_GET_NV_INFO:
      MT_UtilGetNvInfo();
      break;

    case MT_UTIL_SET_PANID:
      MT_UtilSetPanID(pBuf);
      break;

    case MT_UTIL_SET_CHANNELS:
      MT_UtilSetChannels(pBuf);
      break;

    case MT_UTIL_SET_SECLEVEL:
      MT_UtilSetSecLevel(pBuf);
      break;

    case MT_UTIL_SET_PRECFGKEY:
      MT_UtilSetPreCfgKey(pBuf);
      break;

    case MT_UTIL_CALLBACK_SUB_CMD:
      MT_UtilCallbackSub(pBuf);
      break;

    case MT_UTIL_KEY_EVENT:
#if (defined HAL_KEY) && (HAL_KEY == TRUE)
      MT_UtilKeyEvent(pBuf);
#endif
      break;

    case MT_UTIL_LED_CONTROL:
#if (defined HAL_LED) && (HAL_LED == TRUE)
      MT_UtilLedControl(pBuf);
#endif
      break;

    case MT_UTIL_TIME_ALIVE:
      MT_UtilTimeAlive();
      break;

    case MT_UTIL_ADDRMGR_EXT_ADDR_LOOKUP:
      MT_UtilAddrMgrExtAddrLookup(pBuf);
      break;

    case MT_UTIL_ADDRMGR_NWK_ADDR_LOOKUP:
      MT_UtilAddrMgrEntryLookupNwk(pBuf);
      break;

    case MT_UTIL_APSME_LINK_KEY_DATA_GET:
      MT_UtilAPSME_LinkKeyDataGet(pBuf);
      break;

    case MT_UTIL_ASSOC_COUNT:
      MT_UtilAssocCount(pBuf);
      break;

    case MT_UTIL_ASSOC_FIND_DEVICE:
      MT_UtilAssocFindDevice(pBuf);
      break;

    case MT_UTIL_ASSOC_GET_WITH_ADDRESS:
      MT_UtilAssocGetWithAddress(pBuf);
      break;

#if defined ZCL_KEY_ESTABLISH
    case MT_UTIL_ZCL_KEY_EST_INIT_EST:
      MT_UtilzclGeneral_KeyEstablish_InitiateKeyEstablishment(pBuf);
      break;

    case MT_UTIL_ZCL_KEY_EST_SIGN:
      MT_UtilzclGeneral_KeyEstablishment_ECDSASign(pBuf);
      break;
#endif

    case MT_UTIL_SYNC_REQ:
      MT_UtilSync();
      break;

    default:
      status = MT_RPC_ERR_COMMAND_ID;
      break;
  }

  return status;
}

/***************************************************************************************************
 * @fn      MT_UtilGetDeviceInfo
 *
 * @brief   The Get Device Info serial message.
 *
 * @param   void
 *
 * @return  void
 ***************************************************************************************************/
void MT_UtilGetDeviceInfo(void)
{
  uint8  *buf;
  uint8  *pBuf;
  uint8  bufLen = MT_UTIL_DEVICE_INFO_RESPONSE_LEN;
#if !defined NONWK
  uint8  assocCnt = 0;
  uint16 *assocList = NULL;

  if (ZG_DEVICE_RTR_TYPE)
  {
    assocList = AssocMakeList( &assocCnt );
    bufLen += (assocCnt * sizeof(uint16));
  }
#endif

  buf = osal_mem_alloc( bufLen );
  if ( buf )
  {
    pBuf = buf;

    *pBuf++ = ZSUCCESS; // Status

    osal_nv_read( ZCD_NV_EXTADDR, 0, Z_EXTADDR_LEN, pBuf );
    pBuf += Z_EXTADDR_LEN;

#if defined NONWK
    // Skip past ZStack only parameters for NONWK
    *pBuf++ = 0;
    *pBuf++ = 0;
    *pBuf++ = 0;
    *pBuf++ = 0;
    *pBuf = 0;
#else
    {
      uint16 shortAddr = NLME_GetShortAddr();
      *pBuf++ = LO_UINT16( shortAddr );
      *pBuf++ = HI_UINT16( shortAddr );
    }

    /* Return device type */
    *pBuf++ = ZSTACK_DEVICE_BUILD;

    /*Return device state */
    *pBuf++ = (uint8)devState;

    if (ZG_DEVICE_RTR_TYPE)
    {
      *pBuf++ = assocCnt;

      if ( assocCnt )
      {
        uint8 x;
        uint16 *puint16 = assocList;

        for ( x = 0; x < assocCnt; x++, puint16++ )
        {
          *pBuf++ = LO_UINT16( *puint16 );
          *pBuf++ = HI_UINT16( *puint16 );
        }
      }
    }
    else
    {
      *pBuf++ = 0;
    }
#endif

    MT_BuildAndSendZToolResponse( ((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL),
                                 MT_UTIL_GET_DEVICE_INFO,
                                 bufLen, buf );

    osal_mem_free( buf );
  }

  if ( assocList )
  {
    osal_mem_free( assocList );
  }
}

/***************************************************************************************************
 * @fn      MT_UtilGetNvInfo
 *
 * @brief   The Get NV Info serial message.
 *
 * @param   byte *msg - pointer to the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_UtilGetNvInfo(void)
{
  uint8 len;
  uint8 stat;
  uint8 *buf;
  uint8 *pBuf;
  uint16 tmp16;
  uint32 tmp32;

  /*
    Get required length of buffer
    Status + ExtAddr + ChanList + PanID  + SecLevel + PreCfgKey
  */
  len = 1 + Z_EXTADDR_LEN + 4 + 2 + 1 + SEC_KEY_LEN;

  buf = osal_mem_alloc( len );
  if ( buf )
  {
    /* Assume NV not available */
    osal_memset( buf, 0xFF, len );

    /* Skip over status */
    pBuf = buf + 1;

    /* Start with 64-bit extended address */
    stat = osal_nv_read( ZCD_NV_EXTADDR, 0, Z_EXTADDR_LEN, pBuf );
    if ( stat ) stat = 0x01;
    pBuf += Z_EXTADDR_LEN;

    /* Scan channel list (bit mask) */
    if (  osal_nv_read( ZCD_NV_CHANLIST, 0, sizeof( tmp32 ), &tmp32 ) )
      stat |= 0x02;
    else
    {
      pBuf[0] = BREAK_UINT32( tmp32, 3 );
      pBuf[1] = BREAK_UINT32( tmp32, 2 );
      pBuf[2] = BREAK_UINT32( tmp32, 1 );
      pBuf[3] = BREAK_UINT32( tmp32, 0 );
    }
    pBuf += sizeof( tmp32 );

    /* ZigBee PanID */
    if ( osal_nv_read( ZCD_NV_PANID, 0, sizeof( tmp16 ), &tmp16 ) )
      stat |= 0x04;
    else
    {
      pBuf[0] = LO_UINT16( tmp16 );
      pBuf[1] = HI_UINT16( tmp16 );
    }
    pBuf += sizeof( tmp16 );

    /* Security level */
    if ( osal_nv_read( ZCD_NV_SECURITY_LEVEL, 0, sizeof( uint8 ), pBuf++ ) )
      stat |= 0x08;

    /* Pre-configured security key */
    if ( osal_nv_read( ZCD_NV_PRECFGKEY, 0, SEC_KEY_LEN, pBuf ) )
      stat |= 0x10;

    /* Status bit mask - bit=1 indicates failure */
    *buf = stat;

    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), MT_UTIL_GET_NV_INFO,
                                  len, buf );

    osal_mem_free( buf );
  }
}

/***************************************************************************************************
 * @fn      MT_UtilSetPanID
 *
 * @brief   Set PanID message
 *
 * @param   byte *msg - pointer to the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_UtilSetPanID(uint8 *pBuf)
{
  uint16 temp16;
  uint8 retValue = ZFailure;
  uint8 cmdId;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  temp16 = BUILD_UINT16(pBuf[0], pBuf[1]);
  pBuf += sizeof(uint16);

  retValue = osal_nv_write(ZCD_NV_PANID, 0, osal_nv_item_len( ZCD_NV_PANID ), &temp16);

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_UtilSetChannels
 *
 * @brief   Set Channels
 *
 * @param   byte *msg - pointer to the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_UtilSetChannels(uint8 *pBuf)
{
  uint32 tmp32;
  uint8 retValue = ZFailure;
  uint8 cmdId;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  tmp32 = BUILD_UINT32(pBuf[0], pBuf[1], pBuf[2], pBuf[3]);

  retValue = osal_nv_write(ZCD_NV_CHANLIST, 0, osal_nv_item_len( ZCD_NV_CHANLIST ), &tmp32);

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_UtilSetSecLevel
 *
 * @brief   Set Sec Level
 *
 * @param   byte *msg - pointer to the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_UtilSetSecLevel(uint8 *pBuf)
{
  uint8 retValue = ZFailure;
  uint8 cmdId;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  retValue = osal_nv_write( ZCD_NV_SECURITY_LEVEL, 0, osal_nv_item_len( ZCD_NV_SECURITY_LEVEL ), pBuf);

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue );

}

/***************************************************************************************************
 * @fn      MT_UtilSetPreCfgKey
 *
 * @brief   Set Pre Cfg Key
 *
 * @param   byte *msg - pointer to the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_UtilSetPreCfgKey(uint8 *pBuf)
{
  uint8 retValue = ZFailure;
  uint8 cmdId;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  retValue = osal_nv_write( ZCD_NV_PRECFGKEY, 0, osal_nv_item_len( ZCD_NV_PRECFGKEY ), pBuf);

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue );

}

/***************************************************************************************************
 * @fn      MT_UtilCallbackSub
 *
 * @brief   The Callback subscribe.
 *
 * @param   byte *msg - pointer to the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_UtilCallbackSub(uint8 *pBuf)
{
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];
  uint8 retValue = ZFailure;

#if defined(MT_MAC_CB_FUNC) || defined(MT_NWK_CB_FUNC) || defined(MT_ZDO_CB_FUNC) || defined(MT_AF_CB_FUNC) || defined(MT_SAPI_CB_FUNC) || defined(MT_SAPI_CB_FUNC)
  uint8 subSystem;
  uint16 subscribed_command;

  // Move past header
  retValue = ZSuccess;
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Command */
  subscribed_command = BUILD_UINT16(pBuf[0], pBuf[1]);
  pBuf += 2;

  /* Subsystem - 5 bits on the MSB of the command */
  subSystem = HI_UINT16(subscribed_command) & 0x1F ;

  /* What is the action - SUBSCRIBE or !SUBSCRIBE */
  if (*pBuf)
  {
    /* Turn ON */
  #if defined( MT_MAC_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_MAC) || (subscribed_command == 0xFFFF))
      _macCallbackSub = 0xFFFF;
  #endif

  #if defined( MT_NWK_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_NWK) || (subscribed_command == 0xFFFF))
      _nwkCallbackSub = 0xFFFF;
  #endif

  #if defined( MT_ZDO_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_ZDO) || (subscribed_command == 0xFFFF))
      _zdoCallbackSub = 0xFFFFFFFF;
  #endif

  #if defined( MT_AF_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_AF) || (subscribed_command == 0xFFFF))
      _afCallbackSub = 0xFFFF;
  #endif

  #if defined( MT_SAPI_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_SAPI) || (subscribed_command == 0xFFFF))
      _sapiCallbackSub = 0xFFFF;
  #endif
  }
  else
  {
    /* Turn OFF */
  #if defined( MT_MAC_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_MAC) || (subscribed_command == 0xFFFF))
      _macCallbackSub = 0x0000;
  #endif

  #if defined( MT_NWK_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_NWK) || (subscribed_command == 0xFFFF))
      _nwkCallbackSub = 0x0000;
  #endif

  #if defined( MT_ZDO_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_ZDO) || (subscribed_command == 0xFFFF))
      _zdoCallbackSub = 0x00000000;
  #endif

  #if defined( MT_AF_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_AF) || (subscribed_command == 0xFFFF))
      _afCallbackSub = 0x0000;
  #endif

  #if defined( MT_SAPI_CB_FUNC )
    if ((subSystem == MT_RPC_SYS_SAPI) || (subscribed_command == 0xFFFF))
        _sapiCallbackSub = 0x0000;
  #endif
  }
#endif  // MT_MAC_CB_FUNC || MT_NWK_CB_FUNC || MT_ZDO_CB_FUNC || MT_AF_CB_FUNC || MT_SAPI_CB_FUNC || MT_SAPI_CB_FUNC

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue );
}

#if (defined HAL_KEY) && (HAL_KEY == TRUE)
/***************************************************************************************************
 * @fn      MT_UtilKeyEvent
 *
 * @brief   Process Key Event
 *
 * @param   byte *msg - pointer to the data
 *
 * @return  void
 ***************************************************************************************************/
void MT_UtilKeyEvent(uint8 *pBuf)
{
  uint8 x = 0;
  uint8 retValue = ZFailure;
  uint8 cmdId;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Translate between SPI values to device values */
  if ( *pBuf & 0x01 )
    x |= HAL_KEY_SW_1;
  if ( *pBuf & 0x02 )
    x |= HAL_KEY_SW_2;
  if ( *pBuf & 0x04 )
    x |= HAL_KEY_SW_3;
  if ( *pBuf & 0x08 )
  x |= HAL_KEY_SW_4;
#if defined ( HAL_KEY_SW_5 )
  if ( *pBuf & 0x10 )
    x |= HAL_KEY_SW_5;
#endif
#if defined ( HAL_KEY_SW_6 )
  if ( *pBuf & 0x20 )
    x |= HAL_KEY_SW_6;
#endif
#if defined ( HAL_KEY_SW_7 )
  if ( *pBuf & 0x40 )
    x |= HAL_KEY_SW_7;
#endif
#if defined ( HAL_KEY_SW_8 )
  if ( *pBuf & 0x80 )
    x |= HAL_KEY_SW_8;
#endif
  pBuf++;

  retValue = OnBoard_SendKeys(x, *pBuf);

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue );
}
#endif

#if (defined HAL_LED) && (HAL_LED == TRUE)
/***************************************************************************************************
 * @fn      MT_UtilLedControl
 *
 * @brief   Process the LED Control Message
 *
 * @param   pBuf - pointer to the received data
 *
 * @return  None
 ***************************************************************************************************/
void MT_UtilLedControl(uint8 *pBuf)
{
  uint8 iLed, Led, iMode, Mode, cmdId;
  uint8 retValue = ZFailure;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* LED and Mode */
  iLed = *pBuf++;
  iMode = *pBuf;

  if ( iLed == 1 )
    Led = HAL_LED_1;
  else if ( iLed == 2 )
    Led = HAL_LED_2;
  else if ( iLed == 3 )
    Led = HAL_LED_3;
  else if ( iLed == 4 )
    Led = HAL_LED_4;
  else if ( iLed == 0xFF )
    Led = HAL_LED_ALL;
  else
    Led = 0;

  if ( iMode == 0 )
    Mode = HAL_LED_MODE_OFF;
  else if ( iMode == 1 )
    Mode = HAL_LED_MODE_ON;
  else if ( iMode == 2 )
    Mode = HAL_LED_MODE_BLINK;
  else if ( iMode == 3 )
    Mode = HAL_LED_MODE_FLASH;
  else if ( iMode == 4 )
    Mode = HAL_LED_MODE_TOGGLE;
  else
    Led = 0;

  if ( Led != 0 )
  {
    HalLedSet (Led, Mode);
    retValue = ZSuccess;
  }

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, &retValue );

}
#endif /* HAL_LED */

/***************************************************************************************************
 * @fn      MT_UtilTimeAlive
 *
 * @brief   Process Time Alive
 *
 * @param   pBuf - pointer to the received data
 *
 * @return  None
 ***************************************************************************************************/
void MT_UtilTimeAlive(void)
{
  uint8 timeAlive[4];
  uint32 tmp32;

  /* Time since last reset (seconds) */
  tmp32 = osal_GetSystemClock() / 1000;

  /* Convert to high byte first into temp buffer */
  timeAlive[0] = BREAK_UINT32(tmp32, 0);
  timeAlive[1] = BREAK_UINT32(tmp32, 1);
  timeAlive[2] = BREAK_UINT32(tmp32, 2);
  timeAlive[3] = BREAK_UINT32(tmp32, 3);

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), MT_UTIL_TIME_ALIVE,
                               sizeof(tmp32), timeAlive );
}

/***************************************************************************************************
 * @fn      MT_UtilAddrMgrExtAddrLookup
 *
 * @brief   Proxy the AddrMgrExtAddrLookup() function.
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
static void MT_UtilAddrMgrExtAddrLookup(uint8 *pBuf)
{
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  (void)AddrMgrExtAddrLookup(BUILD_UINT16(pBuf[Z_EXTADDR_LEN], pBuf[Z_EXTADDR_LEN+1]), pBuf);
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_NWK), cmdId,
                                                               Z_EXTADDR_LEN, (uint8 *)pBuf);
}

/***************************************************************************************************
 * @fn      MT_UtilAddrMgrEntryLookupNwk
 *
 * @brief   Proxy the AddrMgrEntryLookupNwk() function.
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
static void MT_UtilAddrMgrEntryLookupNwk(uint8 *pBuf)
{
  AddrMgrEntry_t entry;
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  entry.nwkAddr = BUILD_UINT16(pBuf[0], pBuf[1]);
  (void)AddrMgrEntryLookupNwk(&entry);
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_NWK), cmdId,
                                                                  Z_EXTADDR_LEN, entry.extAddr);
}

/***************************************************************************************************
 * @fn      MT_UtilAPSME_LinkKeyDataGet
 *
 * @brief   Proxy the APSME_LinkKeyDataGet() function.
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
static void MT_UtilAPSME_LinkKeyDataGet(uint8 *pBuf)
{
  // Status + LinkKeyDataLen + Tx+Rx Frame counter.
  #define MT_APSME_LINKKEY_GET_RSP_LEN (1 + SEC_KEY_LEN + 4 + 4)
  uint8 rsp[MT_APSME_LINKKEY_GET_RSP_LEN];
  APSME_LinkKeyData_t *pData;
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  *rsp = APSME_LinkKeyDataGet(pBuf, &pData);

  if (SUCCESS == *rsp)
  {
    uint8 *ptr = rsp+1;
    (void)osal_memcpy(ptr, pData->key, SEC_KEY_LEN);
    ptr += SEC_KEY_LEN;
    *ptr++ = BREAK_UINT32(pData->txFrmCntr, 0);
    *ptr++ = BREAK_UINT32(pData->txFrmCntr, 1);
    *ptr++ = BREAK_UINT32(pData->txFrmCntr, 2);
    *ptr++ = BREAK_UINT32(pData->txFrmCntr, 3);
    *ptr++ = BREAK_UINT32(pData->rxFrmCntr, 0);
    *ptr++ = BREAK_UINT32(pData->rxFrmCntr, 1);
    *ptr++ = BREAK_UINT32(pData->rxFrmCntr, 2);
    *ptr++ = BREAK_UINT32(pData->rxFrmCntr, 3);
  }

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_NWK), cmdId, 
                                       MT_APSME_LINKKEY_GET_RSP_LEN, rsp);
}

/***************************************************************************************************
 * @fn      MT_UtilAssocCount
 *
 * @brief   Proxy the AssocCount() function.
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
static void MT_UtilAssocCount(uint8 *pBuf)
{
  uint16 cnt;
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;
  
  cnt = AssocCount(pBuf[0], pBuf[1]);
  pBuf[0] = LO_UINT16(cnt);
  pBuf[1] = HI_UINT16(cnt);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_NWK), cmdId, 2, pBuf);
}

/***************************************************************************************************
 * @fn      MT_UtilAssocFindDevice
 *
 * @brief   Get an associated device by index.
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
static void MT_UtilAssocFindDevice(uint8 *pBuf)
{
  associated_devices_t *pDev;
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];

  pDev = AssocFindDevice(pBuf[MT_RPC_FRAME_HDR_SZ]);

  if (NULL != pDev)
  {
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_NWK), cmdId,
                                   sizeof(associated_devices_t), (uint8 *)pDev);
  }
  else
  {
    associated_devices_t dev;

    dev.shortAddr = INVALID_NODE_ADDR;
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_NWK), cmdId,
                                   sizeof(associated_devices_t), (uint8 *)(&dev));
  }
}

/***************************************************************************************************
 * @fn      MT_UtilAssocGetWithAddress
 *
 * @brief   Get an associated device by address.
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
static void MT_UtilAssocGetWithAddress(uint8 *pBuf)
{
  extern associated_devices_t *AssocGetWithAddress(uint8 *extAddr, uint16 shortAddr);
  associated_devices_t *pDev;
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];

  pBuf += MT_RPC_FRAME_HDR_SZ;
  pDev = AssocGetWithAddress(((AddrMgrExtAddrValid(pBuf)) ? pBuf : NULL),
                               BUILD_UINT16(pBuf[Z_EXTADDR_LEN], pBuf[Z_EXTADDR_LEN+1]));

  if (NULL != pDev)
  {
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_NWK), cmdId,
                                   sizeof(associated_devices_t), (uint8 *)pDev);
  }
  else
  {
    associated_devices_t dev;

    dev.shortAddr = INVALID_NODE_ADDR;
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_NWK), cmdId,
                                   sizeof(associated_devices_t), (uint8 *)(&dev));
  }
}

#if defined ZCL_KEY_ESTABLISH
/***************************************************************************************************
 * @fn      MT_UtilzclGeneral_KeyEstablish_InitiateKeyEstablishment
 *
 * @brief   Proxy the zclGeneral_KeyEstablish_InitiateKeyEstablishment() function.
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
static void MT_UtilzclGeneral_KeyEstablish_InitiateKeyEstablishment(uint8 *pBuf)
{
  afAddrType_t partnerAddr;
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  partnerAddr.panId = 0;  // Not an inter-pan message.
  partnerAddr.endPoint = pBuf[2];
  partnerAddr.addrMode = (afAddrMode_t)pBuf[3];
  if (afAddr64Bit == partnerAddr.addrMode)
  {
    (void)osal_memcpy(partnerAddr.addr.extAddr, pBuf+4, Z_EXTADDR_LEN);
  }
  else
  {
    partnerAddr.addr.shortAddr = BUILD_UINT16(pBuf[4], pBuf[4]);
  }

  zcl_key_establish_task_id = pBuf[0];
  *pBuf = zclGeneral_KeyEstablish_InitiateKeyEstablishment(MT_TaskID, &partnerAddr, pBuf[1]);
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, pBuf);
}

/***************************************************************************************************
 * @fn      MT_UtilzclGeneral_KeyEstablishment_ECDSASign
 *
 * @brief   Proxy the zclGeneral_KeyEstablishment_ECDSASign() function.
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
static void MT_UtilzclGeneral_KeyEstablishment_ECDSASign(uint8 *pBuf)
{
#if defined TC_LINKKEY_JOIN
  uint8 *output = osal_mem_alloc(SE_PROFILE_SIGNATURE_LENGTH+1);
  uint8 cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  if (NULL == output)
  {
    *pBuf = FAILURE;
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId, 1, pBuf);
  }
  else
  {
    *output = zclGeneral_KeyEstablishment_ECDSASign(pBuf+1, *pBuf, output+1);
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_UTIL), cmdId,
                                         SE_PROFILE_SIGNATURE_LENGTH+1, output);
    osal_mem_free(output);
  }
#endif
}

/***************************************************************************************************
 * @fn      MT_UtilKeyEstablishInd
 *
 * @brief   Proxy the ZCL_KEY_ESTABLISH_IND command.
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************************************/
void MT_UtilKeyEstablishInd(keyEstablishmentInd_t *pInd)
{
  uint8 msg[6];

  msg[0] = zcl_key_establish_task_id;
  msg[1] = pInd->hdr.event;
  msg[2] = pInd->hdr.status;
  msg[3] = pInd->waitTime;
  msg[4] = LO_UINT16(pInd->keyEstablishmentSuite);
  msg[5] = HI_UINT16(pInd->keyEstablishmentSuite);

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_UTIL),
                                       MT_UTIL_ZCL_KEY_ESTABLISH_IND, 6, msg);
}
#endif

/***************************************************************************************************
 * @fn      MT_UtilSync
 *
 * @brief   Process the MT_UTIL_SYNC command
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************************************/
static void MT_UtilSync(void)
{
 MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ|(uint8)MT_RPC_SYS_UTIL),MT_UTIL_SYNC_REQ,0,0);
}

#endif /* MT_UTIL_FUNC */
/**************************************************************************************************
 **************************************************************************************************/
