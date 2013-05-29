/**************************************************************************************************
  Filename:       zcl_pi.h
  Revised:        $Date: 2007-07-31 09:16:11 -0700 (Tue, 31 Jul 2007) $
  Revision:       $Revision: 14985 $

  Description:    This file contains the ZCL Protocol Interfaces Definitions


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

#ifndef ZCL_PI_H
#define ZCL_PI_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "zcl.h"

/*********************************************************************
 * CONSTANTS
 */

/*************************************************/
/***   Protocol Interface Cluster Attributes   ***/
/*************************************************/
  // Attributes of the Generic Tunnel cluster
#define ATTRID_PI_GENERIC_TUNNEL_MAX_IN_TRANSFER_SIZE      0x0001
#define ATTRID_PI_GENERIC_TUNNEL_MAX_OUT_TRANSFER_SIZE     0x0002
#define ATTRID_PI_GENERIC_TUNNEL_PROTOCOL_ADDR             0x0003

  // The BACnet Protocol Tunnel cluster does not contain any attributes

/*************************************************/
/***    Protocol Interface Cluster Commands    ***/
/*************************************************/
  // Command IDs for the Generic Tunnel Cluster
#define COMMAND_PI_GENERIC_TUNNEL_MATCH_PROTOCOL_ADDR      0x00

#define COMMAND_PI_GENERIC_TUNNEL_MATCH_PROTOCOL_ADDR_RSP  0x00
#define COMMAND_PI_GENERIC_TUNNEL_ADVERTISE_PROTOCOL_ADDR  0x01

  // Command IDs for the BACnet Protocol Tunnel Cluster
#define COMMAND_PI_BACNET_TUNNEL_TRANSFER_NPDU             0x00

  // Command IDs for the 11073 Protocol Tunnel Cluster
#define COMMAND_PI_11073_TUNNEL_TRANSFER_APDU              0x00
#define COMMAND_PI_11073_TUNNEL_TRANSFER_APDU_METADATA     0x01
#define COMMAND_PI_11073_TUNNEL_TRANSFER_METADATA          0x02

  // Attributes of the 11073 Protocol Tunnel cluster
#define ATTRID_PI_11073_TUNNEL_METADATA                    0x0000

/*** Metadata Attribute End-to-end Latency values ***/
#define METADATA_LANTENCY_VERY_HI                          0x00
#define METADATA_LANTENCY_HI                               0x01
#define METADATA_LANTENCY_MID                              0x02
#define METADATA_LANTENCY_LOW                              0x03

/*** Metadata Attribute Reliablity values ***/
#define METADATA_RELIABLITY_GOOD                           0x00
#define METADATA_RELIABLITY_BETTER                         0x01
#define METADATA_RELIABLITY_BEST                           0x02

/************************************************************************************
 * MACROS
 */


/****************************************************************************
 * TYPEDEFS
 */
/*** Structures used for callback functions ***/

/*** ZCL Generic Tunnel Cluster: Match Protocol Address command ***/
typedef struct
{
  afAddrType_t *srcAddr;  // requestor's address
  uint8 seqNum;           // sequence number received with command
  uint8 len;              // length of address protocol
  uint8 *protocolAddr;    // protocol address
} zclPIMatchProtocolAddr_t;

/*** ZCL Generic Tunnel Cluster: Match Protocol Address Response ***/
typedef struct
{
  afAddrType_t *srcAddr;  // responder's address
  uint8 *ieeeAddr;        // device address
  uint8 len;              // length of address protocol
  uint8 *protocolAddr;    // protocol address
} zclPIMatchProtocolAddrRsp_t;

/*** ZCL Generic Tunnel Cluster: Advertise Protocol Address command ***/
typedef struct
{
  afAddrType_t *srcAddr;  // requestor's address
  uint8 len;              // length of address protocol
  uint8 *protocolAddr;    // protocol address
} zclPIAdvertiseProtocolAddr_t;

/*** ZCL BACnet Protocol Tunnel Cluster: Transfer NPDU command ***/
typedef struct
{
  afAddrType_t *srcAddr;  // requestor's address
  uint16 len;             // length of BACnet NPDU
  uint8 *npdu;            // BACnet NPDU
} zclBACnetTransferNPDU_t;

/*** ZCL 11073 Protocol Tunnel Cluster: Transfer APDU command ***/
typedef struct
{
  afAddrType_t *srcAddr;  // requestor's address
  uint16 len;             // length of 11073 APDU
  uint8 *apdu;            // 11073 APDU
} zcl11073TransferAPDU_t;

/*** ZCL 11073 Protocol Tunnel Cluster: Transfer APDU and Metadata command ***/
typedef struct
{
  afAddrType_t *srcAddr;  // requestor's address
  uint16 metadata;        // 11073 Metadata
  uint16 len;             // length of 11073 APDU
  uint8 *apdu;            // 11073 APDU
} zcl11073TransferAPDUMetadata_t;

/*** ZCL 11073 Protocol Tunnel Cluster: Transfer Metadata command ***/
typedef struct
{
  afAddrType_t *srcAddr;  // requestor's address
  uint16 metadata;        // 11073 Metadata
} zcl11073TransferMetadata_t;

// This callback is called to process a Match Protocol Address command
//  pCmd - received command data
typedef void (*zclPICB_MatchProtocolAddr_t)( zclPIMatchProtocolAddr_t *pCmd );

// This callback is called to process a Match Protocol Address response
//  pCmd - received command data
typedef void (*zclPICB_MatchProtocolAddrRsp_t)( zclPIMatchProtocolAddrRsp_t *pRsp );

// This callback is called to process a Advertise Protocol Address command
//  pCmd - received command data
typedef void (*zclPICB_AdvertiseProtocolAddr_t)( zclPIAdvertiseProtocolAddr_t *pCmd );

// This callback is called to process a BACnet Transfer NPDU command
//  pCmd - received command data
typedef void (*zclPICB_BACnetTransferNPDU_t)( zclBACnetTransferNPDU_t *pCmd );

// This callback is called to process a 11037 Transfer APDU command
//  pCmd - received command data
typedef void (*zclPICB_11073TransferAPDU_t)( zcl11073TransferAPDU_t *pCmd );

// This callback is called to process a 11037 Transfer APDU and Metdata command
//  pCmd - received command data
typedef void (*zclPICB_11073TransferAPDUMetadata_t)( zcl11073TransferAPDUMetadata_t *pCmd );

// This callback is called to process a 11037 Transfer Metadata command
//  pCmd - received command data
typedef void (*zclPICB_11073TransferMetadata_t)( zcl11073TransferMetadata_t *pCmd );

// Register Callbacks table entry - enter function pointers for callbacks that
// the application would like to receive
typedef struct			
{
  zclPICB_MatchProtocolAddr_t      pfnPI_MatchProtocolAddr;
  zclPICB_MatchProtocolAddrRsp_t   pfnPI_MatchProtocolAddrRsp;
  zclPICB_AdvertiseProtocolAddr_t  pfnPI_AdvertiseProtocolAddr;
  zclPICB_BACnetTransferNPDU_t     pfnPI_BACnetTransferNPDU;
  zclPICB_11073TransferAPDU_t      pfnPI_11073TransferAPDU;
  zclPICB_11073TransferAPDUMetadata_t pfnPI_11073TransferAPDUMetadata;
  zclPICB_11073TransferMetadata_t  pfnPI_11073TransferMetadata;
} zclPI_AppCallbacks_t;

/*********************************************************************
 * FUNCTION MACROS
 */

/*
 *  Send a BACnet Transfer NPDU Command. This command is used when a
 *  BACnet network layer wishes to transfer a BACnet NPDU across a 
 *  ZigBee tunnel to another BACnet network layer.
 *
 *  Use like:
 *      ZStatus_t zclPI_Send_BACnetTransferNPDUCmd( uint16 srcEP, afAddrType_t *dstAddr, uint16 len, uint8 *npdu, uint8 disableDefaultRsp, uint8 seqNum );
 */
#define zclPI_Send_BACnetTransferNPDUCmd(a,b,c,d,e,f) zcl_SendCommand( (a), (b), ZCL_CLUSTER_ID_PI_BACNET_PROTOCOL_TUNNEL,\
                                                                       COMMAND_PI_BACNET_TUNNEL_TRANSFER_NPDU, TRUE,\
                                                                       ZCL_FRAME_CLIENT_SERVER_DIR, (e), 0, (f), (c), (d) )

/****************************************************************************
 * VARIABLES
 */


/****************************************************************************
 * FUNCTIONS
 */

 /*
  * Register for callbacks from this cluster library
  */
extern ZStatus_t zclPI_RegisterCmdCallbacks( uint8 endpoint, zclPI_AppCallbacks_t *callbacks );

/*
 * Send out a Match Protocol Address Command
 */
extern ZStatus_t zclPI_Send_MatchProtocolAddrCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                  uint8 len, uint8 *protocolAddr, 
                                                  uint8 disableDefaultRsp, uint8 seqNum );
/*
 *  Send a Match Protocol Address Response
*/
extern ZStatus_t zclPI_Send_MatchProtocolAddrRsp( uint8 srcEP, afAddrType_t *dstAddr,
                                                  uint8 *ieeeAddr, uint8 len, uint8 *protocolAddr, 
                                                  uint8 disableDefaultRsp, uint8 seqNum );
/*
 * Send out an Advertise Protocol Address Command
 */
extern ZStatus_t zclPI_Send_AdvertiseProtocolAddrCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                      uint8 len, uint8 *protocolAddr, 
                                                      uint8 disableDefaultRsp, uint8 seqNum );
/*
 * Send out a 11073 Transfer APDU Command
 */
extern ZStatus_t zclPI_Send_11073TransferAPDUCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                  uint16 len, uint8 *apdu, 
                                                  uint8 disableDefaultRsp, uint8 seqNum );
/*
 * Send out a 11073 Transfer APDU and Metadata Command
 */
extern ZStatus_t zclPI_Send_11073TransferAPDUMetadataCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                                   uint16 metadata, uint16 len, uint8 *apdu, 
                                                   uint8 disableDefaultRsp, uint8 seqNum );
/*
 * Send out a 11073 Transfer Metadata Command
 */
extern ZStatus_t zclPI_Send_11073TransferMetadataCmd( uint8 srcEP, afAddrType_t *dstAddr,
                                               uint16 metadata, uint8 disableDefaultRsp,
                                               uint8 seqNum );

#ifdef __cplusplus
}
#endif

#endif /* ZCL_PI_H */
