/**************************************************************************************************
  Filename:       zcl_pi.c
  Revised:        $Date: 2007-07-31 09:16:11 -0700 (Tue, 31 Jul 2007) $
  Revision:       $Revision: 14985 $

  Description:    Zigbee Cluster Library - Protocol Interfaces (PI)


  Copyright 2010 Texas Instruments Incorporated. All rights reserved.

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
#include "zcl.h"
#include "zcl_general.h"
#include "zcl_pi.h"

#if defined ( INTER_PAN )
  #include "stub_aps.h"
#endif

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */
typedef struct zclPICBRec
{
  struct zclPICBRec     *next;
  uint8                 endpoint; // Used to link it into the endpoint descriptor
  zclPI_AppCallbacks_t  *CBs;     // Pointer to Callback function
} zclPICBRec_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static zclPICBRec_t *zclPICBs = (zclPICBRec_t *)NULL;
static uint8 zclPIPluginRegisted = FALSE;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static ZStatus_t zclPI_HdlIncoming( zclIncoming_t *pInHdlrMsg );
static ZStatus_t zclPI_HdlInSpecificCommands( zclIncoming_t *pInMsg );
static zclPI_AppCallbacks_t *zclPI_FindCallbacks( uint8 endpoint );

static ZStatus_t zclPI_ProcessIn_GenericTunneServer( zclIncoming_t *pInMsg,
                                                     zclPI_AppCallbacks_t *pCBs );
static ZStatus_t zclPI_ProcessIn_GenericTunneClient( zclIncoming_t *pInMsg,
                                                     zclPI_AppCallbacks_t *pCBs );
static ZStatus_t zclPI_ProcessIn_BACnetTunnelCmds( zclIncoming_t *pInMsg,
                                                   zclPI_AppCallbacks_t *pCBs );
static ZStatus_t zclPI_ProcessIn_11073TunnelCmds( zclIncoming_t *pInMsg,
                                                  zclPI_AppCallbacks_t *pCBs );

/*********************************************************************
 * @fn      zclPI_RegisterCmdCallbacks
 *
 * @brief   Register an applications command callbacks
 *
 * @param   endpoint - application's endpoint
 * @param   callbacks - pointer to the callback record.
 *
 * @return  ZMemError if not able to allocate
 */
ZStatus_t zclPI_RegisterCmdCallbacks( uint8 endpoint, zclPI_AppCallbacks_t *callbacks )
{
  zclPICBRec_t *pNewItem;
  zclPICBRec_t *pLoop;

  // Register as a ZCL Plugin
  if ( !zclPIPluginRegisted )
  {
    zcl_registerPlugin( ZCL_CLUSTER_ID_PI_GENERIC_TUNNEL,
                        ZCL_CLUSTER_ID_PI_11073_PROTOCOL_TUNNEL,
                        zclPI_HdlIncoming );
    zclPIPluginRegisted = TRUE;
  }

  // Fill in the new profile list
  pNewItem = osal_mem_alloc( sizeof( zclPICBRec_t ) );
  if ( pNewItem == NULL )
    return (ZMemError);

  pNewItem->next = (zclPICBRec_t *)NULL;
  pNewItem->endpoint = endpoint;
  pNewItem->CBs = callbacks;

  // Find spot in list
  if ( zclPICBs == NULL )
  {
    zclPICBs = pNewItem;
  }
  else
  {
    // Look for end of list
    pLoop = zclPICBs;
    while ( pLoop->next != NULL )
      pLoop = pLoop->next;

    // Put new item at end of list
    pLoop->next = pNewItem;
  }
  return ( ZSuccess );
}

/*******************************************************************************
 * @fn      zclPI_Send_MatchProtocolAddrCmd
 *
 * @brief   Call to send out a Match Protocol Address Command. This command is
 *          used when an associated protocol specific tunnel wishes to find out
 *          the ZigBee address of the Generic Tunnel server cluster representing
 *          a protocol-specific device with a given protocol address. The 
 *          command is typically multicast to a group of inter-communicating 
 *          Generic Tunnel clusters.
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   len - length of protocol address
 * @param   protocolAddr - protocol address
 * @param   disableDefaultRsp - whether to disable the Default Response command
 * @param   seqNum - sequence number
 *
 * @return  ZStatus_t
 */
ZStatus_t zclPI_Send_MatchProtocolAddrCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                           uint8 len, uint8 *protocolAddr, 
                                           uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 *buf;
  ZStatus_t stat;

  buf = osal_mem_alloc( len+1 );  // 1 for length field
  if ( buf )
  {  
    buf[0] = len;
    osal_memcpy( &(buf[1]), protocolAddr, len );

    stat = zcl_SendCommand( srcEP, dstAddr, ZCL_CLUSTER_ID_PI_GENERIC_TUNNEL,
                            COMMAND_PI_GENERIC_TUNNEL_MATCH_PROTOCOL_ADDR, TRUE, 
                            ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum,
                            (len+1), buf );
    osal_mem_free( buf );
  }
  else
  {
    stat = ZMemError;
  }
  
  return ( stat );
}

/*******************************************************************************
 * @fn      zclPI_Send_MatchProtocolAddrRsp
 *
 * @brief   Call to send out a Match Protocol Address Response. This response
 *          is sent back upon receipt of a Match Protocol Address command to
 *          indicate that the Protocol Address was successfully matched.
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   ieeeAddr - device address
 * @param   len - length of protocol address
 * @param   protocolAddr - protocol address
 * @param   disableDefaultRsp - whether to disable the Default Response command
 * @param   seqNum - sequence number
 *
 * @return  ZStatus_t
 */
ZStatus_t zclPI_Send_MatchProtocolAddrRsp( uint8 srcEP, afAddrType_t *dstAddr,
                                           uint8 *ieeeAddr, uint8 len, uint8 *protocolAddr, 
                                           uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 *buf;
  uint8 msgLen = 8 + 1 + len; // 8 for IEEE Addr + 1 for length field
  ZStatus_t stat;

  buf = osal_mem_alloc( msgLen ); // 1 for length field
  if ( buf )
  {
    // Copy over IEEE Address
    osal_cpyExtAddr( buf, ieeeAddr );

    // Copy over Protocol Address
    buf[8] = len;
    osal_memcpy( &(buf[9]), protocolAddr, len );

    stat = zcl_SendCommand( srcEP, dstAddr, ZCL_CLUSTER_ID_PI_GENERIC_TUNNEL,
                            COMMAND_PI_GENERIC_TUNNEL_MATCH_PROTOCOL_ADDR_RSP, TRUE, 
                            ZCL_FRAME_SERVER_CLIENT_DIR, disableDefaultRsp, 0, seqNum,
                            msgLen, buf );
    osal_mem_free( buf );
  }
  else
  {
    stat = ZMemError;
  }
  
  return ( stat );
}

/*******************************************************************************
 * @fn      zclPI_Send_AdvertiseProtocolAddrCmd
 *
 * @brief   Call to send out an Advertise Protocol Address Command. This command
 *          is sent out typically upon startup or whenever the Protocol Address
 *          attribute changes. It is typically multicast to a group of inter-
 *          communicating Generic Tunnel clusters.
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   len - length of protocol address
 * @param   protocolAddr - protocol address
 * @param   disableDefaultRsp - whether to disable the Default Response command
 * @param   seqNum - sequence number
 *
 * @return  ZStatus_t
 */
ZStatus_t zclPI_Send_AdvertiseProtocolAddrCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                               uint8 len, uint8 *protocolAddr, 
                                               uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 *buf;
  ZStatus_t stat;

  buf = osal_mem_alloc( len+1 ); // 1 for length field
  if ( buf )
  {  
    buf[0] = len;
    osal_memcpy( &(buf[1]), protocolAddr, len );

    stat = zcl_SendCommand( srcEP, dstAddr, ZCL_CLUSTER_ID_PI_GENERIC_TUNNEL,
                            COMMAND_PI_GENERIC_TUNNEL_ADVERTISE_PROTOCOL_ADDR, TRUE, 
                            ZCL_FRAME_SERVER_CLIENT_DIR, disableDefaultRsp, 0, seqNum,
                            (len+1), buf );
    osal_mem_free( buf );
  }
  else
  {
    stat = ZMemError;
  }
  
  return ( stat );
}


/*******************************************************************************
 * @fn      zclPI_Send_11073TransferAPDUCmd
 *
 * @brief   Call to send out a 11073 Transfer APDU Command. This command is 
 *          used when a 11073 network layer wishes to transfer a 11073 APDU 
 *          across a ZigBee tunnel to another 11073 network layer.
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   len - length of APDU
 * @param   apdu - APDU to be sent
 * @param   disableDefaultRsp - whether to disable the Default Response command
 * @param   seqNum - sequence number
 *
 * @return  ZStatus_t
 */
ZStatus_t zclPI_Send_11073TransferAPDUCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                           uint16 len, uint8 *apdu, 
                                           uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 *buf;
  ZStatus_t stat;

  buf = osal_mem_alloc( len+2 ); // 2 for length field
  if ( buf )
  {  
    buf[0] = LO_UINT16( len );
    buf[1] = HI_UINT16( len );
    osal_memcpy( &(buf[2]), apdu, len );

    stat = zcl_SendCommand( srcEP, dstAddr, ZCL_CLUSTER_ID_PI_11073_PROTOCOL_TUNNEL,
                            COMMAND_PI_11073_TUNNEL_TRANSFER_APDU, TRUE, 
                            ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum,
                            (len+2), buf );
    osal_mem_free( buf );
  }
  else
  {
    stat = ZMemError;
  }
  
  return ( stat );
}

/*******************************************************************************
 * @fn      zclPI_Send_11073TransferAPDUMetadataCmd
 *
 * @brief   Call to send out a 11073 Transfer APDU and Metadata Command.
 *          This command is used when a 11073 network layer wishes to 
 *          transfer a 11073 APDU and associated metadata across a ZigBee
 *          tunnel to another 11073 network layer.
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   metadata - metadata to be sent
 * @param   len - length of APDU
 * @param   apdu - APDU to be sent
 * @param   disableDefaultRsp - whether to disable the Default Response command
 * @param   seqNum - sequence number
 *
 * @return  ZStatus_t
 */
ZStatus_t zclPI_Send_11073TransferAPDUMetadataCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                   uint16 metadata, uint16 len, uint8 *apdu, 
                                                   uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 *buf;
  ZStatus_t stat;

  buf = osal_mem_alloc( len+4 ); // Metadata (2 octets) + APDU Length (2 octets)
  if ( buf )
  {
    buf[0] = LO_UINT16( metadata );
    buf[1] = HI_UINT16( metadata );
    buf[2] = LO_UINT16( len );
    buf[3] = HI_UINT16( len );
    osal_memcpy( &(buf[4]), apdu, len );

    stat = zcl_SendCommand( srcEP, dstAddr, ZCL_CLUSTER_ID_PI_11073_PROTOCOL_TUNNEL,
                            COMMAND_PI_11073_TUNNEL_TRANSFER_APDU_METADATA, TRUE, 
                            ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum,
                            (len+4), buf );
    osal_mem_free( buf );
  }
  else
  {
    stat = ZMemError;
  }
  
  return ( stat );
}


/*******************************************************************************
 * @fn      zclPI_Send_11073TransfeMetadataCmd
 *
 * @brief   Call to send out a 11073 Transfer Metadata Command. This command
 *          is used when a 11073 network layer wishes to transfer metadata 
 *          across a ZigBee tunnel to another 11073 network layer.
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   metadata - metadata to be sent
 * @param   disableDefaultRsp - whether to disable the Default Response command
 * @param   seqNum - sequence number
 *
 * @return  ZStatus_t
 */
ZStatus_t zclPI_Send_11073TransferMetadataCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                               uint16 metadata, uint8 disableDefaultRsp,
                                               uint8 seqNum )
{
  uint8 buf[2];

  buf[0] = LO_UINT16( metadata );
  buf[1] = HI_UINT16( metadata );

  return ( zcl_SendCommand( srcEP, dstAddr, ZCL_CLUSTER_ID_PI_11073_PROTOCOL_TUNNEL,
                            COMMAND_PI_11073_TUNNEL_TRANSFER_METADATA, TRUE, 
                            ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum,
                            2, buf ) );
}

/*********************************************************************
 * @fn      zclPI_FindCallbacks
 *
 * @brief   Find the callbacks for an endpoint
 *
 * @param   endpoint
 *
 * @return  pointer to the callbacks
 */
static zclPI_AppCallbacks_t *zclPI_FindCallbacks( uint8 endpoint )
{
  zclPICBRec_t *pCBs;
  
  pCBs = zclPICBs;
  while ( pCBs )
  {
    if ( pCBs->endpoint == endpoint )
      return ( pCBs->CBs );
  }
  return ( (zclPI_AppCallbacks_t *)NULL );
}

/*********************************************************************
 * @fn      zclPI_HdlIncoming
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library or Profile commands for attributes
 *          that aren't in the attribute list
 *
 * @param   pInMsg - pointer to the incoming message
 * @param   logicalClusterID
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclPI_HdlIncoming( zclIncoming_t *pInMsg )
{
  ZStatus_t stat = ZSuccess;

#if defined ( INTER_PAN )
  if ( StubAPS_InterPan( pInMsg->msg->srcAddr.panId, pInMsg->msg->srcAddr.endPoint ) )
    return ( stat ); // Cluster not supported thru Inter-PAN
#endif

  if ( zcl_ClusterCmd( pInMsg->hdr.fc.type ) )
  {
    // Is this a manufacturer specific command?
    if ( pInMsg->hdr.fc.manuSpecific == 0 ) 
    {
      stat = zclPI_HdlInSpecificCommands( pInMsg );
    }
    else
    {
      // We don't support any manufacturer specific command -- ignore it.
      stat = ZFailure;
    }
  }
  else
  {
    // Handle all the normal (Read, Write...) commands
    stat = ZFailure;
  }

  return ( stat );
}

/*********************************************************************
 * @fn      zclPI_HdlInSpecificCommands
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library
 *
 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclPI_HdlInSpecificCommands( zclIncoming_t *pInMsg )
{
  ZStatus_t stat;
  zclPI_AppCallbacks_t *pCBs;

  // make sure endpoint exists
  pCBs = zclPI_FindCallbacks( pInMsg->msg->endPoint );
  if ( pCBs == NULL )
    return ( ZFailure );

  switch ( pInMsg->msg->clusterId )				
  {
    case ZCL_CLUSTER_ID_PI_GENERIC_TUNNEL:
      if ( zcl_ServerCmd( pInMsg->hdr.fc.direction ) )
        stat = zclPI_ProcessIn_GenericTunneServer( pInMsg, pCBs );
      else
        stat = zclPI_ProcessIn_GenericTunneClient( pInMsg, pCBs );
      break;

    case ZCL_CLUSTER_ID_PI_BACNET_PROTOCOL_TUNNEL:
      stat = zclPI_ProcessIn_BACnetTunnelCmds( pInMsg, pCBs );
      break;

    case ZCL_CLUSTER_ID_PI_11073_PROTOCOL_TUNNEL:
      stat = zclPI_ProcessIn_11073TunnelCmds( pInMsg, pCBs );
      break;

    default:
      stat = ZFailure;
      break;
  }

  return ( stat );
}

/*********************************************************************
 * @fn      zclPI_ProcessIn_GenericTunneServer
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library on a command ID basis
 *
 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclPI_ProcessIn_GenericTunneServer( zclIncoming_t *pInMsg,
                                                     zclPI_AppCallbacks_t *pCBs )
{
  if  ( pInMsg->hdr.commandID != COMMAND_PI_GENERIC_TUNNEL_MATCH_PROTOCOL_ADDR )
    return (ZFailure);   // Error ignore the command

  if ( pCBs->pfnPI_MatchProtocolAddr )
  {
    zclPIMatchProtocolAddr_t cmd;
    
    cmd.srcAddr = &(pInMsg->msg->srcAddr);
    cmd.seqNum = pInMsg->hdr.transSeqNum;
    cmd.len = pInMsg->pData[0];
    cmd.protocolAddr = &(pInMsg->pData[1]);
    
    pCBs->pfnPI_MatchProtocolAddr( &cmd );
  }

  return ( ZSuccess );
}

/*********************************************************************
 * @fn      zclPI_ProcessIn_GenericTunneClient
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library on a command ID basis
 *
 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclPI_ProcessIn_GenericTunneClient( zclIncoming_t *pInMsg,
                                                     zclPI_AppCallbacks_t *pCBs )
{
  ZStatus_t stat = ZSuccess;

  switch ( pInMsg->hdr.commandID )				
  {
    case COMMAND_PI_GENERIC_TUNNEL_MATCH_PROTOCOL_ADDR_RSP:
      if ( pCBs->pfnPI_MatchProtocolAddrRsp )
      {
        zclPIMatchProtocolAddrRsp_t cmd;
        
        cmd.srcAddr = &(pInMsg->msg->srcAddr);
        cmd.ieeeAddr = pInMsg->pData;
        cmd.len = pInMsg->pData[8];
        cmd.protocolAddr = &(pInMsg->pData[9]);

        pCBs->pfnPI_MatchProtocolAddrRsp( &cmd );
      }
      break;

    case COMMAND_PI_GENERIC_TUNNEL_ADVERTISE_PROTOCOL_ADDR:
      if ( pCBs->pfnPI_AdvertiseProtocolAddr )
      {
        zclPIAdvertiseProtocolAddr_t cmd;
        
        cmd.srcAddr = &(pInMsg->msg->srcAddr);
        cmd.len = pInMsg->pData[0];
        cmd.protocolAddr = &(pInMsg->pData[1]);
        
        pCBs->pfnPI_AdvertiseProtocolAddr( &cmd );
      }
      break;

    default:
      // Unknown command
      stat = ZFailure;
      break;
  }

  return ( stat );
}

/*********************************************************************
 * @fn      zclPI_ProcessIn_BACnetTunnelCmds
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library on a command ID basis
 *
 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclPI_ProcessIn_BACnetTunnelCmds( zclIncoming_t *pInMsg,
                                                   zclPI_AppCallbacks_t *pCBs )
{
  if  ( pInMsg->hdr.commandID != COMMAND_PI_BACNET_TUNNEL_TRANSFER_NPDU )
    return (ZFailure);   // Error ignore the command

  if ( pCBs->pfnPI_BACnetTransferNPDU )
  {
    zclBACnetTransferNPDU_t cmd;
    
    cmd.srcAddr = &(pInMsg->msg->srcAddr);
    cmd.len = pInMsg->pDataLen;
    cmd.npdu = pInMsg->pData;
    
    pCBs->pfnPI_BACnetTransferNPDU( &cmd );
  }
  
  return ( ZSuccess );
}

/*********************************************************************
 * @fn      zclPI_ProcessIn_11073TunnelCmds
 *
 * @brief   Callback from ZCL to process incoming Commands specific
 *          to this cluster library on a command ID basis
 *
 * @param   pInMsg - pointer to the incoming message
 *
 * @return  ZStatus_t
 */
static ZStatus_t zclPI_ProcessIn_11073TunnelCmds( zclIncoming_t *pInMsg,
                                                  zclPI_AppCallbacks_t *pCBs )
{
  ZStatus_t stat = ZSuccess;

  switch ( pInMsg->hdr.commandID )				
  {
    case COMMAND_PI_11073_TUNNEL_TRANSFER_APDU:
      if ( pCBs->pfnPI_11073TransferAPDU )
      {
        zcl11073TransferAPDU_t cmd;
        
        cmd.srcAddr = &(pInMsg->msg->srcAddr);
        cmd.len = BUILD_UINT16( pInMsg->pData[0], pInMsg->pData[1] );
        cmd.apdu = &(pInMsg->pData[2]);

        pCBs->pfnPI_11073TransferAPDU( &cmd );
      }
      break;

    case COMMAND_PI_11073_TUNNEL_TRANSFER_APDU_METADATA:
      if ( pCBs->pfnPI_11073TransferAPDUMetadata )
      {
        zcl11073TransferAPDUMetadata_t cmd;
        
        cmd.srcAddr = &(pInMsg->msg->srcAddr);
        cmd.metadata = BUILD_UINT16( pInMsg->pData[0], pInMsg->pData[1] );
        cmd.len = BUILD_UINT16( pInMsg->pData[2], pInMsg->pData[3] );
        cmd.apdu = &(pInMsg->pData[4]);
        
        pCBs->pfnPI_11073TransferAPDUMetadata( &cmd );
      }
      break;

    case COMMAND_PI_11073_TUNNEL_TRANSFER_METADATA:
      if ( pCBs->pfnPI_11073TransferMetadata )
      {
        zcl11073TransferMetadata_t cmd;
        
        cmd.srcAddr = &(pInMsg->msg->srcAddr);
        cmd.metadata = BUILD_UINT16( pInMsg->pData[0], pInMsg->pData[1] );
        
        pCBs->pfnPI_11073TransferMetadata( &cmd );
      }
      break;

    default:
      // Unknown command
      stat = ZFailure;
      break;
  }

  return ( stat );
}


/****************************************************************************
****************************************************************************/

