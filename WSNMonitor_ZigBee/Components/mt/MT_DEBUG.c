/***************************************************************************************************
  Filename:       MT.c
  Revised:        $Date: 2008-02-12 16:28:45 -0800 (Tue, 12 Feb 2008) $
  Revision:       $Revision: 16392 $

  Description:

  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

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
#include "MT.h"
#include "MT_DEBUG.h"
#include "MT_UART.h"

/***************************************************************************************************
 * LOCAL FUNCTIONs
 ***************************************************************************************************/
#if defined (MT_DEBUG_FUNC)
void MT_DebugSetThreshold(uint8 *pBuf);
#endif

#if defined (MT_DEBUG_FUNC)
/***************************************************************************************************
 * @fn      MT_DebugProcessing
 *
 * @brief   Process all the DEBUG commands that are issued by test tool
 *
 * @param   pBuf - pointer to received buffer
 *
 * @return  status
 ***************************************************************************************************/
uint8 MT_DebugCommandProcessing(uint8 *pBuf)
{
  uint8 status = MT_RPC_SUCCESS;

  switch (pBuf[MT_RPC_POS_CMD1])
  {
    case MT_DEBUG_SET_THRESHOLD:
      MT_DebugSetThreshold(pBuf);
      break;

    default:
      status = MT_RPC_ERR_COMMAND_ID;
      break;
  }

  return status;
}

/***************************************************************************************************
 * @fn      MT_DebugSetThreshold
 *
 * @brief   Process Debug Set Threshold
 *
 * @param   pBuf - pointer to received buffer
 *
 * @return  void
 ***************************************************************************************************/
void MT_DebugSetThreshold(uint8 *pBuf)
{
  uint8 retValue = ZSuccess;
  uint8 cmdId;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Populate info */
  debugCompId = *pBuf++;
  debugThreshold = *pBuf++;

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_DBG), cmdId, 1, &retValue);
}
#endif /* MT_DEBUG_FUNC */

/***************************************************************************************************
 * @fn      MT_ProcessDebugMsg
 *
 * @brief   Build and send a debug message.
 *
 * @param   byte *data - pointer to the data portion of the debug message
 *
 * @return  void
 ***************************************************************************************************/
void MT_ProcessDebugMsg( mtDebugMsg_t *msg )
{
  byte *msg_ptr;
  byte dataLen;
  uint8 buf[11];
  uint8 *pBuf;

  /* Calculate the data length based */
  dataLen = 5 + (msg->numParams * sizeof ( uint16 ));

  /* Get a message buffer to build the debug message */
  msg_ptr = osal_msg_allocate( (byte)(SPI_0DATA_MSG_LEN + dataLen + 1) );
  if ( msg_ptr )
  {
    /* Build the message */
    pBuf = buf;
    *pBuf++ = msg->compID;
    *pBuf++ = msg->severity;
    *pBuf++ = msg->numParams;

    if ( msg->numParams >= 1 )
    {
      *pBuf++ = LO_UINT16( msg->param1 );
      *pBuf++ = HI_UINT16( msg->param1 );
    }

    if ( msg->numParams >= 2 )
    {
      *pBuf++ = LO_UINT16( msg->param2 );
      *pBuf++ = HI_UINT16( msg->param2 );
    }

    if ( msg->numParams == 3 )
    {
      *pBuf++ = LO_UINT16( msg->param3 );
      *pBuf++ = HI_UINT16( msg->param3 );
    }

    *pBuf++ = LO_UINT16( msg->timestamp );
    *pBuf++ = HI_UINT16( msg->timestamp );

#ifdef MT_UART_DEFAULT_PORT
    /* Debug message is set to AREQ CMD 0x80 for now */
    /* Build and send back the response */
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_DBG), 0x80, dataLen, buf);
#endif
    osal_msg_deallocate( msg_ptr );
  }
}

/***************************************************************************************************
 * @fn      MT_ProcessDebugStr
 *
 * @brief   Build and send a debug string.
 *
 * @param   byte *dstr - pointer to the data portion of the debug message
 *
 * @return  void
 ***************************************************************************************************/
void MT_ProcessDebugStr(mtDebugStr_t *dstr)
{
  byte *msg_ptr;

  /* Get a message buffer to build the debug message */
  msg_ptr = osal_mem_alloc( (byte)(SPI_0DATA_MSG_LEN + dstr->strLen) );
  if ( msg_ptr )
  {
#ifdef MT_UART_DEFAULT_PORT
    /* Debug message is set to AREQ CMD 0x80 for now */
    /* Build and send back the response */
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_DBG), MT_DEBUG_MSG, dstr->strLen, dstr->pString);
#endif
    osal_mem_free( msg_ptr );
  }
}
/***************************************************************************************************
 ***************************************************************************************************/
