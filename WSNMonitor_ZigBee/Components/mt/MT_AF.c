/**************************************************************************************************
  Filename:       MT_AF.c
  Revised:        $Date: 2010-01-08 16:52:37 -0800 (Fri, 08 Jan 2010) $
  Revision:       $Revision: 21471 $


  Description:    MonitorTest functions for the AF layer.


  Copyright 2007-2010 Texas Instruments Incorporated. All rights reserved.

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

/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include "ZComDef.h"
#include "OSAL.h"
#include "MT.h"
#include "MT_AF.h"
#include "MT_ZDO.h"
#include "nwk.h"
#include "OnBoard.h"
#include "MT_UART.h"

#if defined INTER_PAN
#include "stub_aps.h"
#endif

/***************************************************************************************************
 * GLOBAL VARIABLES
 ***************************************************************************************************/

#if defined ( MT_AF_CB_FUNC )
uint16 _afCallbackSub;
#endif

/***************************************************************************************************
 * LOCAL FUNCTIONS
 ***************************************************************************************************/
void MT_AfRegister(uint8 *pBuf);
void MT_AfDataRequest(uint8 *pBuf);

#if defined ( ZIGBEE_SOURCE_ROUTING )
void MT_AfDataRequestSrcRtg(uint8 *pBuf);
#endif

#if defined INTER_PAN
static void MT_AfInterPanCtl(uint8 *pBuf);
#endif

/***************************************************************************************************
 * @fn      MT_afCommandProcessing
 *
 * @brief   Process all the AF commands that are issued by test tool
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  status
 ***************************************************************************************************/
uint8 MT_AfCommandProcessing(uint8 *pBuf)
{
  uint8 status = MT_RPC_SUCCESS;

  switch (pBuf[MT_RPC_POS_CMD1])
  {
    case MT_AF_REGISTER:
      MT_AfRegister(pBuf);
      break;

    case MT_AF_DATA_REQUEST:
    case MT_AF_DATA_REQUEST_EXT:
      MT_AfDataRequest(pBuf);
      break;
      
#if defined ( ZIGBEE_SOURCE_ROUTING )
    case MT_AF_DATA_REQUEST_SRCRTG:
      MT_AfDataRequestSrcRtg(pBuf);
      break;
#endif
      
#if defined INTER_PAN
    case MT_AF_INTER_PAN_CTL:
      MT_AfInterPanCtl(pBuf);
      break; 
#endif
      
    default:
      status = MT_RPC_ERR_COMMAND_ID;
      break;
  }

  return status;
}

/***************************************************************************************************
 * @fn      MT_AfRegister
 *
 * @brief   Process AF Register command
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  none
 ***************************************************************************************************/
void MT_AfRegister(uint8 *pBuf)
{
  uint8 cmdId;
  uint8 retValue = ZMemError;
  endPointDesc_t *epDesc;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  epDesc = (endPointDesc_t *)osal_mem_alloc(sizeof(endPointDesc_t));
  if ( epDesc )
  {
    epDesc->task_id = &MT_TaskID;
    retValue = MT_BuildEndpointDesc( pBuf, epDesc );
    if ( retValue == ZSuccess )
    {
      retValue = afRegister( epDesc );
    }

    if ( retValue != ZSuccess )
    {
      osal_mem_free( epDesc );
    }
  }

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_AF), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_AfDataRequest
 *
 * @brief   Process AF Register command
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  none
 ***************************************************************************************************/
void MT_AfDataRequest(uint8 *pBuf)
{
  uint8 cmd0, cmd1, tempLen = 0;
  uint8 retValue = ZFailure;
  endPointDesc_t *epDesc;
  byte transId;
  afAddrType_t dstAddr;
  cId_t cId;
  byte txOpts, radius, srcEP;

  /* Parse header */
  cmd0 = pBuf[MT_RPC_POS_CMD0];
  cmd1 = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  if (cmd1 == MT_AF_DATA_REQUEST_EXT)
  {
    dstAddr.addrMode = (afAddrMode_t)*pBuf++;

    if (dstAddr.addrMode == afAddr64Bit)
    {
      (void)osal_memcpy(dstAddr.addr.extAddr, pBuf, Z_EXTADDR_LEN);
    }
    else
    {
      dstAddr.addr.shortAddr = BUILD_UINT16(pBuf[0], pBuf[1]);
    }
    pBuf += Z_EXTADDR_LEN;

    dstAddr.endPoint = *pBuf++;
    dstAddr.panId = BUILD_UINT16(pBuf[0], pBuf[1]);
    pBuf += 2;
  }
  else
  {
    /* Destination address */
    dstAddr.addrMode = afAddr16Bit;
    dstAddr.addr.shortAddr = BUILD_UINT16(pBuf[0], pBuf[1]);
    pBuf += 2;

    /* Destination endpoint */
    dstAddr.endPoint = *pBuf++;
    dstAddr.panId = 0;
  }

  /* Source endpoint */
  srcEP = *pBuf++;
  epDesc = afFindEndPointDesc( srcEP );

  /* ClusterId */
  cId = BUILD_UINT16(pBuf[0], pBuf[1]);
  pBuf +=2;

  /* TransId */
  transId = *pBuf++;

  /* TxOption */
  txOpts = *pBuf++;

  /* Radius */
  radius = *pBuf++;

  /* Length */
  tempLen = *pBuf++;

  if ( epDesc == NULL )
  {
    retValue = afStatus_INVALID_PARAMETER;
  }
  else
  {
    retValue = AF_DataRequest( &dstAddr, epDesc, cId, tempLen, pBuf, &transId, txOpts, radius );
  }

  if (MT_RPC_CMD_SREQ == (cmd0 & MT_RPC_CMD_TYPE_MASK))
  {
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP|(uint8)MT_RPC_SYS_AF), cmd1, 1, &retValue);
  }
}


#if defined ( ZIGBEE_SOURCE_ROUTING )

/***************************************************************************************************
 * @fn      MT_AfDataRequestSrcRtg
 *
 * @brief   Process AF Register command
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  none
 ***************************************************************************************************/
void MT_AfDataRequestSrcRtg(uint8 *pBuf)
{
  uint8 cmdId, dataLen = 0;
  uint8 retValue = ZFailure;
  endPointDesc_t *epDesc;
  byte transId;
  afAddrType_t dstAddr;
  cId_t cId;
  byte txOpts, radius, srcEP, relayCnt;
  uint16 *pRelayList;
  uint8 i;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Destination address */
  /* Initialize the panID field to zero to avoid inter-pan */
  osal_memset( &dstAddr, 0, sizeof(afAddrType_t) ); 
  dstAddr.addrMode = afAddr16Bit;
  dstAddr.addr.shortAddr = BUILD_UINT16(pBuf[0], pBuf[1]);
  pBuf += 2;

  /* Destination endpoint */
  dstAddr.endPoint = *pBuf++;

  /* Source endpoint */
  srcEP = *pBuf++;
  epDesc = afFindEndPointDesc( srcEP );

  /* ClusterId */
  cId = BUILD_UINT16(pBuf[0], pBuf[1]);
  pBuf +=2;

  /* TransId */
  transId = *pBuf++;

  /* TxOption */
  txOpts = *pBuf++;

  /* Radius */
  radius = *pBuf++;
  
  /* Source route relay count */
  relayCnt = *pBuf++;
  
  /* Convert the source route relay list */
  if( (pRelayList = osal_mem_alloc( relayCnt * sizeof( uint16 ))) != NULL )
  {
    for( i = 0; i < relayCnt; i++ )
    {
      pRelayList[i]  = BUILD_UINT16( pBuf[0], pBuf[1] );
      pBuf += 2;
    }
  
    /* Data payload Length */
    dataLen = *pBuf++;

    if ( epDesc == NULL )
    {
      retValue = afStatus_INVALID_PARAMETER;
    }
    else
    {
      retValue = AF_DataRequestSrcRtg( &dstAddr, epDesc, cId, dataLen, pBuf, 
                                     &transId, txOpts, radius, relayCnt, pRelayList );
    }
    
    /* Free the memory allocated */
    osal_mem_free( pRelayList );
  }
  else
  {
    retValue = afStatus_MEM_FAIL;
  }
    

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_AF), cmdId, 1, &retValue);
}
#endif

#if defined INTER_PAN
/***************************************************************************************************
 * @fn      MT_AfInterPanCtl
 *
 * @brief   Process the AF Inter Pan control command.
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  none
 ***************************************************************************************************/
static void MT_AfInterPanCtl(uint8 *pBuf)
{
  uint8 cmd, rtrn;
  uint16 panId;
  endPointDesc_t *pEP;
  
  cmd = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  switch (*pBuf++)  // Inter-pan request parameter.
  {
  case InterPanClr:
    rtrn = StubAPS_SetIntraPanChannel();           // Switch channel back to the NIB channel.
    break;

  case InterPanSet:
    rtrn = StubAPS_SetInterPanChannel(*pBuf);      // Set channel for inter-pan communication.
    break;

  case InterPanReg:
    if ((pEP = afFindEndPointDesc(*pBuf)))
    {
      StubAPS_RegisterApp(pEP);
      rtrn = SUCCESS;
    }
    else
    {
      rtrn = FAILURE;
    }
    break;

  case InterPanChk:
    panId = BUILD_UINT16(pBuf[0], pBuf[1]);
    rtrn = (StubAPS_InterPan(panId, pBuf[2])) ? ZSuccess : ZFailure;
    break;

  default:
    rtrn = afStatus_INVALID_PARAMETER;
    break;
  }

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_AF), cmd, 1, &rtrn);
}
#endif

/***************************************************************************************************
 * @fn      MT_AfDataConfirm
 *
 * @brief   Process
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  none
 ***************************************************************************************************/
void MT_AfDataConfirm(afDataConfirm_t *pMsg)
{
  uint8 retArray[3];

  retArray[0] = pMsg->hdr.status;
  retArray[1] = pMsg->endpoint;
  retArray[2] = pMsg->transID;

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_AF), MT_AF_DATA_CONFIRM, 3, retArray);
}

/***************************************************************************************************
 * @fn          MT_AfIncomingMsg
 *
 * @brief       Process the callback subscription for AF Incoming data.
 *
 * @param       pkt - Incoming AF data.
 *
 * @return      none
 ***************************************************************************************************/
void MT_AfIncomingMsg(afIncomingMSGPacket_t *pMsg)
{
  uint8 dataLen = pMsg->cmd.DataLength;  /* Length of the data section in the response packet */
  uint8 respLen = 17 + dataLen;          /* Length of the whole response packet */
  uint8 cmd = MT_AF_INCOMING_MSG;
  uint8 *pRsp, *tempPtr;

#if defined INTER_PAN
  if (StubAPS_InterPan(pMsg->srcAddr.panId, pMsg->srcAddr.endPoint))
  {
    cmd = MT_AF_INCOMING_MSG_EXT;
  }
  else
#endif
  if (pMsg->srcAddr.addrMode == afAddr64Bit)
  {
    cmd = MT_AF_INCOMING_MSG_EXT;
  }

  if (cmd == MT_AF_INCOMING_MSG_EXT)
  {
    respLen += 9;
  }

  // Attempt to allocate memory for the response packet.
  if ((pRsp = osal_mem_alloc(respLen)) == NULL)
  {
    return;
  }
  tempPtr = pRsp;

  /* Fill in the data */

  /* Group ID */
  *tempPtr++ = LO_UINT16(pMsg->groupId);
  *tempPtr++ = HI_UINT16(pMsg->groupId);

  /* Cluster ID */
  *tempPtr++ = LO_UINT16(pMsg->clusterId);
  *tempPtr++ = HI_UINT16(pMsg->clusterId);

  if (cmd == MT_AF_INCOMING_MSG_EXT)
  {
    *tempPtr++ = pMsg->srcAddr.addrMode;

    if (pMsg->srcAddr.addrMode == afAddr64Bit)
    {
      (void)osal_memcpy(tempPtr, pMsg->srcAddr.addr.extAddr, Z_EXTADDR_LEN);
    }
    else
    {
      tempPtr[0] = LO_UINT16(pMsg->srcAddr.addr.shortAddr);
      tempPtr[1] = HI_UINT16(pMsg->srcAddr.addr.shortAddr);
    }
    tempPtr += Z_EXTADDR_LEN;

    *tempPtr++ = pMsg->srcAddr.endPoint;
#if defined INTER_PAN
    *tempPtr++ = LO_UINT16(pMsg->srcAddr.panId);
    *tempPtr++ = HI_UINT16(pMsg->srcAddr.panId);
#else
    *tempPtr++ = 0;
    *tempPtr++ = 0;
#endif
  }
  else
  {
    /* Source Address */
    *tempPtr++ = LO_UINT16(pMsg->srcAddr.addr.shortAddr);
    *tempPtr++ = HI_UINT16(pMsg->srcAddr.addr.shortAddr);

    /* Source EP */
    *tempPtr++ = pMsg->srcAddr.endPoint;
  }

  /* Destination EP */
  *tempPtr++ = pMsg->endPoint;

  /* WasBroadCast */
  *tempPtr++ = pMsg->wasBroadcast;

  /* LinkQuality */
  *tempPtr++ = pMsg->LinkQuality;

  /* SecurityUse */
  *tempPtr++ = pMsg->SecurityUse;

  /* Timestamp */
  *tempPtr++ = BREAK_UINT32(pMsg->timestamp, 0);
  *tempPtr++ = BREAK_UINT32(pMsg->timestamp, 1);
  *tempPtr++ = BREAK_UINT32(pMsg->timestamp, 2);
  *tempPtr++ = BREAK_UINT32(pMsg->timestamp, 3);

  /* Transmit Sequence Number */
  *tempPtr++ = pMsg->cmd.TransSeqNumber;

  /* Data Length */
  *tempPtr++ = dataLen;

  /* Data */
  if (dataLen)
  {
    osal_memcpy(tempPtr, pMsg->cmd.Data, dataLen);
  }

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ|(uint8)MT_RPC_SYS_AF), cmd, respLen, pRsp);

  /* Free memory */
  osal_mem_free(pRsp);
}

/***************************************************************************************************
***************************************************************************************************/
