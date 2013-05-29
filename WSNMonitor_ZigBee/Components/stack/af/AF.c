

/*********************************************************************
 * INCLUDES
 */

#include "OSAL.h"
#include "AF.h"
#include "nwk_globals.h"
#include "nwk_util.h"
#include "aps_groups.h"
#include "ZDProfile.h"
#include "aps_frag.h"
#include "rtg.h"

#if defined ( MT_AF_CB_FUNC )
  #include "MT_AF.h"
#endif

#if defined ( INTER_PAN )
  #include "stub_aps.h"
#endif

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * @fn      afSend
 *
 * @brief   Helper macro for V1 API to invoke V2 API.
 *
 * input parameters
 *
 * @param  *dstAddr - Full ZB destination address: Nwk Addr + End Point.
 * @param   srcEP - Origination (i.e. respond to or ack to) End Point.
 * @param   cID - A valid cluster ID as specified by the Profile.
 * @param   len - Number of bytes of data pointed to by next param.
 * @param  *buf - A pointer to the data bytes to send.
 * @param   options - Valid bit mask of AF Tx Options as defined in AF.h.
 * @param  *transID - A pointer to a byte which can be modified and which will
 *                    be used as the transaction sequence number of the msg.
 *
 * output parameters
 *
 * @param  *transID - Incremented by one if the return value is success.
 *
 * @return  afStatus_t - See previous definition of afStatus_... types.
 */
#define afSend( dstAddr, srcEP, cID, len, buf, transID, options, radius ) \
        AF_DataRequest( (dstAddr), afFindEndPointDesc( (srcEP) ), \
                          (cID), (len), (buf), (transID), (options), (radius) )

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

epList_t *epList;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void afBuildMSGIncoming( aps_FrameFormat_t *aff, endPointDesc_t *epDesc,
                zAddrType_t *SrcAddress, uint16 SrcPanId, NLDE_Signal_t *sig,
                byte SecurityUse, uint32 timestamp );

static epList_t *afFindEndPointDescList( byte EndPoint );

static pDescCB afGetDescCB( endPointDesc_t *epDesc );

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      afInit
 *
 * @brief   Initialization function for the AF.
 *
 * @param   none
 *
 * @return  none
 */
void afInit( void )
{
  // Start with no endpoint defined
  epList = NULL;
}

/*********************************************************************
 * @fn      afRegisterExtended
 *
 * @brief   Register an Application's EndPoint description.
 *
 * @param   epDesc - pointer to the Application's endpoint descriptor.
 * @param   descFn - pointer to descriptor callback function
 *
 * NOTE:  The memory that epDesc is pointing to must exist after this call.
 *
 * @return  Pointer to epList_t on success, NULL otherwise.
 */
epList_t *afRegisterExtended( endPointDesc_t *epDesc, pDescCB descFn )
{
  epList_t *ep;
  epList_t *epSearch;

  ep = osal_mem_alloc( sizeof ( epList_t ) );
  if ( ep )
  {
    // Fill in the new list entry
    ep->epDesc = epDesc;

    // Default to allow Match Descriptor.
    ep->flags = eEP_AllowMatch;
    ep->pfnDescCB = descFn;
    ep->nextDesc = NULL;

    // Does a list exist?
    if ( epList == NULL )
      epList = ep;  // Make this the first entry
    else
    {
      // Look for the end of the list
      epSearch = epList;
      while( epSearch->nextDesc != NULL )
        epSearch = epSearch->nextDesc;

      // Add new entry to end of list
      epSearch->nextDesc = ep;
    }
  }

  return ep;
}

/*********************************************************************
 * @fn      afRegister
 *
 * @brief   Register an Application's EndPoint description.
 *
 * @param   epDesc - pointer to the Application's endpoint descriptor.
 *
 * NOTE:  The memory that epDesc is pointing to must exist after this call.
 *
 * @return  afStatus_SUCCESS - Registered
 *          afStatus_MEM_FAIL - not enough memory to add descriptor
 *          afStatus_INVALID_PARAMETER - duplicate endpoint
 */
afStatus_t afRegister( endPointDesc_t *epDesc ) 
{
  epList_t *ep;
  
  // Look for duplicate endpoint
  if ( afFindEndPointDescList( epDesc->endPoint ) )
    return ( afStatus_INVALID_PARAMETER );
  
  ep = afRegisterExtended( epDesc, NULL );

  return ((ep == NULL) ? afStatus_MEM_FAIL : afStatus_SUCCESS);
}


/*********************************************************************
 * @fn          afDataConfirm
 *
 * @brief       This function will generate the Data Confirm back to
 *              the application.
 *
 * @param       endPoint - confirm end point
 * @param       transID - transaction ID from APSDE_DATA_REQUEST
 * @param       status - status of APSDE_DATA_REQUEST
 *
 * @return      none
 */
void afDataConfirm( uint8 endPoint, uint8 transID, ZStatus_t status )
{
  endPointDesc_t *epDesc;
  afDataConfirm_t *msgPtr;

  // Find the endpoint description
  epDesc = afFindEndPointDesc( endPoint );
  if ( epDesc == NULL )
    return;

  // Determine the incoming command type
  msgPtr = (afDataConfirm_t *)osal_msg_allocate( sizeof(afDataConfirm_t) );
  if ( msgPtr )
  {
    // Build the Data Confirm message
    msgPtr->hdr.event = AF_DATA_CONFIRM_CMD;
    msgPtr->hdr.status = status;
    msgPtr->endpoint = endPoint;
    msgPtr->transID = transID;

#if defined ( MT_AF_CB_FUNC )
    /* If MT has subscribed for this callback, don't send as a message. */
    if ( AFCB_CHECK(CB_ID_AF_DATA_CNF,*(epDesc->task_id)) )
    {
      /* Send callback if it's subscribed */
      MT_AfDataConfirm ((void *)msgPtr);
      /* Release the memory. */
      osal_msg_deallocate( (void *)msgPtr );
    }
    else
#endif
    {
      /* send message through task message */
      osal_msg_send( *(epDesc->task_id), (byte *)msgPtr );
    }
  }
}

/*********************************************************************
 * @fn          afIncomingData
 *
 * @brief       Transfer a data PDU (ASDU) from the APS sub-layer to the AF.
 *
 * @param       aff  - pointer to APS frame format
 * @param       SrcAddress  - Source address
 * @param       sig - incoming message's link quality
 * @param       SecurityUse - Security enable/disable
 *
 * @return      none
 */
void afIncomingData( aps_FrameFormat_t *aff, zAddrType_t *SrcAddress, uint16 SrcPanId,
                     NLDE_Signal_t *sig, byte SecurityUse, uint32 timestamp )
{
  endPointDesc_t *epDesc = NULL;
  uint16 epProfileID = 0xFFFF;  // Invalid Profile ID
  epList_t *pList = epList;
#if !defined ( APS_NO_GROUPS )    
  uint8 grpEp = APS_GROUPS_EP_NOT_FOUND;
#endif  

  if ( ((aff->FrmCtrl & APS_DELIVERYMODE_MASK) == APS_FC_DM_GROUP) )
  {
#if !defined ( APS_NO_GROUPS )    
    // Find the first endpoint for this group
    grpEp = aps_FindGroupForEndpoint( aff->GroupID, APS_GROUPS_FIND_FIRST );
    if ( grpEp == APS_GROUPS_EP_NOT_FOUND )
      return;   // No endpoint found

    epDesc = afFindEndPointDesc( grpEp );
    if ( epDesc == NULL )
      return;   // Endpoint descriptor not found

    pList = afFindEndPointDescList( epDesc->endPoint );
#else
    return; // Not supported
#endif    
  }
  else if ( aff->DstEndPoint == AF_BROADCAST_ENDPOINT )
  {
    // Set the list
    if ( pList != NULL )
    {
      epDesc = pList->epDesc;
    }
  }
  else if ( (epDesc = afFindEndPointDesc( aff->DstEndPoint )) )
  {
    pList = afFindEndPointDescList( epDesc->endPoint );
  }

  while ( epDesc )
  {
    if ( pList->pfnDescCB )
    {
      uint16 *pID = (uint16 *)(pList->pfnDescCB(
                                 AF_DESCRIPTOR_PROFILE_ID, epDesc->endPoint ));
      if ( pID )
      {
        epProfileID = *pID;
        osal_mem_free( pID );
      }
    }
    else if ( epDesc->simpleDesc )
    {
      epProfileID = epDesc->simpleDesc->AppProfId;
    }

    if ( (aff->ProfileID == epProfileID) ||
         ((epDesc->endPoint == ZDO_EP) && (aff->ProfileID == ZDO_PROFILE_ID)) )
    {
      {
        afBuildMSGIncoming( aff, epDesc, SrcAddress, SrcPanId, sig, SecurityUse, timestamp );
      }
    }

    if ( ((aff->FrmCtrl & APS_DELIVERYMODE_MASK) == APS_FC_DM_GROUP) )
    {
#if !defined ( APS_NO_GROUPS )      
      // Find the next endpoint for this group
      grpEp = aps_FindGroupForEndpoint( aff->GroupID, grpEp );
      if ( grpEp == APS_GROUPS_EP_NOT_FOUND )
        return;   // No endpoint found

      epDesc = afFindEndPointDesc( grpEp );
      if ( epDesc == NULL )
        return;   // Endpoint descriptor not found

      pList = afFindEndPointDescList( epDesc->endPoint );
#else
      return;
#endif      
    }
    else if ( aff->DstEndPoint == AF_BROADCAST_ENDPOINT )
    {
      pList = pList->nextDesc;
      if ( pList )
        epDesc = pList->epDesc;
      else
        epDesc = NULL;
    }
    else
      epDesc = NULL;
  }
}

/*********************************************************************
 * @fn          afBuildMSGIncoming
 *
 * @brief       Build the message for the app
 *
 * @param
 *
 * @return      pointer to next in data buffer
 */
static void afBuildMSGIncoming( aps_FrameFormat_t *aff, endPointDesc_t *epDesc,
                 zAddrType_t *SrcAddress, uint16 SrcPanId, NLDE_Signal_t *sig, 
                 byte SecurityUse, uint32 timestamp )
{
  afIncomingMSGPacket_t *MSGpkt;
  const byte len = sizeof( afIncomingMSGPacket_t ) + aff->asduLength;
  byte *asdu = aff->asdu;
  MSGpkt = (afIncomingMSGPacket_t *)osal_msg_allocate( len );

  if ( MSGpkt == NULL )
  {
    return;
  }

  MSGpkt->hdr.event = AF_INCOMING_MSG_CMD;
  MSGpkt->groupId = aff->GroupID;
  MSGpkt->clusterId = aff->ClusterID;
  afCopyAddress( &MSGpkt->srcAddr, SrcAddress );
  MSGpkt->srcAddr.endPoint = aff->SrcEndPoint;
  MSGpkt->endPoint = epDesc->endPoint;
  MSGpkt->wasBroadcast = aff->wasBroadcast;
  MSGpkt->LinkQuality = sig->LinkQuality;
  MSGpkt->correlation = sig->correlation;
  MSGpkt->rssi = sig->rssi;
  MSGpkt->SecurityUse = SecurityUse;
  MSGpkt->timestamp = timestamp;
  MSGpkt->macDestAddr = aff->macDestAddr;
  MSGpkt->srcAddr.panId = SrcPanId;
  MSGpkt->cmd.TransSeqNumber = 0;
  MSGpkt->cmd.DataLength = aff->asduLength;

  if ( MSGpkt->cmd.DataLength )
  {
    MSGpkt->cmd.Data = (byte *)(MSGpkt + 1);
    osal_memcpy( MSGpkt->cmd.Data, asdu, MSGpkt->cmd.DataLength );
  }
  else
  {
    MSGpkt->cmd.Data = NULL;
  }

#if defined ( MT_AF_CB_FUNC )
  // If ZDO or SAPI have registered for this endpoint, dont intercept it here
  if (AFCB_CHECK(CB_ID_AF_DATA_IND, *(epDesc->task_id)))
  {
    MT_AfIncomingMsg( (void *)MSGpkt );
    // Release the memory.
    osal_msg_deallocate( (void *)MSGpkt );
  }
  else
#endif
  {
    // Send message through task message.
    osal_msg_send( *(epDesc->task_id), (uint8 *)MSGpkt );
  }
}

/*********************************************************************
 * @fn      AF_DataRequest
 *
 * @brief   Common functionality for invoking APSDE_DataReq() for both
 *          SendMulti and MSG-Send.
 *
 * input parameters
 *
 * @param  *dstAddr - Full ZB destination address: Nwk Addr + End Point.
 * @param  *srcEP - Origination (i.e. respond to or ack to) End Point Descr.
 * @param   cID - A valid cluster ID as specified by the Profile.
 * @param   len - Number of bytes of data pointed to by next param.
 * @param  *buf - A pointer to the data bytes to send.
 * @param  *transID - A pointer to a byte which can be modified and which will
 *                    be used as the transaction sequence number of the msg.
 * @param   options - Valid bit mask of Tx options.
 * @param   radius - Normally set to AF_DEFAULT_RADIUS.
 *
 * output parameters
 *
 * @param  *transID - Incremented by one if the return value is success.
 *
 * @return  afStatus_t - See previous definition of afStatus_... types.
 */
uint8 AF_DataRequestDiscoverRoute = TRUE;
afStatus_t AF_DataRequest( afAddrType_t *dstAddr, endPointDesc_t *srcEP,
                           uint16 cID, uint16 len, uint8 *buf, uint8 *transID,
                           uint8 options, uint8 radius )
{
  pDescCB pfnDescCB;
  ZStatus_t stat;
  APSDE_DataReq_t req;
  afDataReqMTU_t mtu;

  // Verify source end point
  if ( srcEP == NULL )
  {
    return afStatus_INVALID_PARAMETER;
  }

#if !defined( REFLECTOR )
  if ( dstAddr->addrMode == afAddrNotPresent )
  {
    return afStatus_INVALID_PARAMETER;
  }
#endif

  // Validate broadcasting
  if ( ( dstAddr->addrMode == afAddr16Bit     ) ||
       ( dstAddr->addrMode == afAddrBroadcast )    )
  {
    // Check for valid broadcast values
    if( ADDR_NOT_BCAST != NLME_IsAddressBroadcast( dstAddr->addr.shortAddr )  )
    {
      // Force mode to broadcast
      dstAddr->addrMode = afAddrBroadcast;
    }
    else
    {
      // Address is not a valid broadcast type
      if ( dstAddr->addrMode == afAddrBroadcast )
      {
        return afStatus_INVALID_PARAMETER;
      }
    }
  }
  else if ( dstAddr->addrMode != afAddr64Bit &&
            dstAddr->addrMode != afAddrGroup &&
            dstAddr->addrMode != afAddrNotPresent )
  {
    return afStatus_INVALID_PARAMETER;
  }
  
  // Set destination address
  req.dstAddr.addrMode = dstAddr->addrMode;
  if ( dstAddr->addrMode == afAddr64Bit )
    osal_cpyExtAddr( req.dstAddr.addr.extAddr, dstAddr->addr.extAddr );
  else
    req.dstAddr.addr.shortAddr = dstAddr->addr.shortAddr;
  
  req.profileID = ZDO_PROFILE_ID;

  if ( (pfnDescCB = afGetDescCB( srcEP )) )
  {
    uint16 *pID = (uint16 *)(pfnDescCB(
                                 AF_DESCRIPTOR_PROFILE_ID, srcEP->endPoint ));
    if ( pID )
    {
      req.profileID = *pID;
      osal_mem_free( pID );
    }
  }
  else if ( srcEP->simpleDesc )
  {
    req.profileID = srcEP->simpleDesc->AppProfId;
  }

  req.txOptions = 0;

  if ( ( options & AF_ACK_REQUEST              ) &&
       ( req.dstAddr.addrMode != AddrBroadcast ) &&
       ( req.dstAddr.addrMode != AddrGroup     )    )
  {
    req.txOptions |=  APS_TX_OPTIONS_ACK;
  }

  if ( options & AF_SKIP_ROUTING )
  {
    req.txOptions |=  APS_TX_OPTIONS_SKIP_ROUTING;
  }

  if ( options & AF_EN_SECURITY )
  {
    req.txOptions |= APS_TX_OPTIONS_SECURITY_ENABLE;
    mtu.aps.secure = TRUE;
  }
  else
  {
    mtu.aps.secure = FALSE;
  }

  mtu.kvp = FALSE;

  req.transID       = *transID;
  req.srcEP         = srcEP->endPoint;
  req.dstEP         = dstAddr->endPoint;
  req.clusterID     = cID;
  req.asduLen       = len;
  req.asdu          = buf;
  req.discoverRoute = AF_DataRequestDiscoverRoute;//(uint8)((options & AF_DISCV_ROUTE) ? 1 : 0);
  req.radiusCounter = radius;
#if defined ( INTER_PAN )
  req.dstPanId      = dstAddr->panId;

  if ( StubAPS_InterPan( dstAddr->panId, dstAddr->endPoint ) )
  {
    if ( len > INTERP_DataReqMTU() )
    {
      stat = afStatus_INVALID_PARAMETER;
    }
    else
    {
      stat = INTERP_DataReq( &req );
    }
  }
  else
#endif // INTER_PAN
  {
    if (len > afDataReqMTU( &mtu ) )
    {
      if (apsfSendFragmented)
      {
        stat = (*apsfSendFragmented)( &req );
      }
      else
      {
        stat = afStatus_INVALID_PARAMETER;
      }
    }
    else
    {
      stat = APSDE_DataReq( &req );
    }
  }

  /*
   * If this is an EndPoint-to-EndPoint message on the same device, it will not
   * get added to the NWK databufs. So it will not go OTA and it will not get
   * a MACCB_DATA_CONFIRM_CMD callback. Thus it is necessary to generate the
   * AF_DATA_CONFIRM_CMD here. Note that APSDE_DataConfirm() only generates one
   * message with the first in line TransSeqNumber, even on a multi message.
   * Also note that a reflected msg will not have its confirmation generated
   * here.
   */
  if ( (req.dstAddr.addrMode == Addr16Bit) &&
       (req.dstAddr.addr.shortAddr == NLME_GetShortAddr()) )
  {
    afDataConfirm( srcEP->endPoint, *transID, stat );
  }

  if ( stat == afStatus_SUCCESS )
  {
    (*transID)++;
  }

  return (afStatus_t)stat;
}

#if defined ( ZIGBEE_SOURCE_ROUTING )
/*********************************************************************
 * @fn      AF_DataRequestSrcRtg
 *
 * @brief   Common functionality for invoking APSDE_DataReq() for both
 *          SendMulti and MSG-Send.
 *
 * input parameters
 *
 * @param  *dstAddr - Full ZB destination address: Nwk Addr + End Point.
 * @param  *srcEP - Origination (i.e. respond to or ack to) End Point Descr.
 * @param   cID - A valid cluster ID as specified by the Profile.
 * @param   len - Number of bytes of data pointed to by next param.
 * @param  *buf - A pointer to the data bytes to send.
 * @param  *transID - A pointer to a byte which can be modified and which will
 *                    be used as the transaction sequence number of the msg.
 * @param   options - Valid bit mask of Tx options.
 * @param   radius - Normally set to AF_DEFAULT_RADIUS.
 * @param   relayCnt - Number of devices in the relay list
 * @param   pRelayList - Pointer to the relay list
 *
 * output parameters
 *
 * @param  *transID - Incremented by one if the return value is success.
 *
 * @return  afStatus_t - See previous definition of afStatus_... types.
 */

afStatus_t AF_DataRequestSrcRtg( afAddrType_t *dstAddr, endPointDesc_t *srcEP,
                           uint16 cID, uint16 len, uint8 *buf, uint8 *transID,
                           uint8 options, uint8 radius, uint8 relayCnt, uint16* pRelayList )
{
  uint8 status;
  
  /* Add the source route to the source routing table */
  status = RTG_AddSrcRtgEntry_Guaranteed( dstAddr->addr.shortAddr, relayCnt, 
                                         pRelayList );

  if( status == RTG_SUCCESS)
  {
    /* Call AF_DataRequest to send the data */
    status = AF_DataRequest( dstAddr, srcEP, cID, len, buf, transID, options, radius );
  }
  else if( status == RTG_INVALID_PATH )
  {
    /* The source route relay count is exceeding the network limit */
    status = afStatus_INVALID_PARAMETER;
  }
  else
  {
    /* The guaranteed adding entry fails due to memory failure */
    status = afStatus_MEM_FAIL;
  }
  return status;
}

#endif

/*********************************************************************
 * @fn      afFindEndPointDescList
 *
 * @brief   Find the endpoint description entry from the endpoint
 *          number.
 *
 * @param   EndPoint - Application Endpoint to look for
 *
 * @return  the address to the endpoint/interface description entry
 */
static epList_t *afFindEndPointDescList( byte EndPoint )
{
  epList_t *epSearch;

  // Start at the beginning
  epSearch = epList;

  // Look through the list until the end
  while ( epSearch )
  {
    // Is there a match?
    if ( epSearch->epDesc->endPoint == EndPoint )
    {
      return ( epSearch );
    }
    else
      epSearch = epSearch->nextDesc;  // Next entry
  }

  return ( (epList_t *)NULL );
}

/*********************************************************************
 * @fn      afFindEndPointDesc
 *
 * @brief   Find the endpoint description entry from the endpoint
 *          number.
 *
 * @param   EndPoint - Application Endpoint to look for
 *
 * @return  the address to the endpoint/interface description entry
 */
endPointDesc_t *afFindEndPointDesc( byte EndPoint )
{
  epList_t *epSearch;

  // Look for the endpoint
  epSearch = afFindEndPointDescList( EndPoint );

  if ( epSearch )
    return ( epSearch->epDesc );
  else
    return ( (endPointDesc_t *)NULL );
}

/*********************************************************************
 * @fn      afFindSimpleDesc
 *
 * @brief   Find the Simple Descriptor from the endpoint number.
 *
 * @param   EP - Application Endpoint to look for.
 *
 * @return  Non-zero to indicate that the descriptor memory must be freed.
 */
byte afFindSimpleDesc( SimpleDescriptionFormat_t **ppDesc, byte EP )
{
  epList_t *epItem = afFindEndPointDescList( EP );
  byte rtrn = FALSE;

  if ( epItem )
  {
    if ( epItem->pfnDescCB )
    {
      *ppDesc = epItem->pfnDescCB( AF_DESCRIPTOR_SIMPLE, EP );
      rtrn = TRUE;
    }
    else
    {
      *ppDesc = epItem->epDesc->simpleDesc;
    }
  }
  else
  {
    *ppDesc = NULL;
  }

  return rtrn;
}

/*********************************************************************
 * @fn      afGetDescCB
 *
 * @brief   Get the Descriptor callback function.
 *
 * @param   epDesc - pointer to the endpoint descriptor
 *
 * @return  function pointer or NULL
 */
static pDescCB afGetDescCB( endPointDesc_t *epDesc )
{
  epList_t *epSearch;

  // Start at the beginning
  epSearch = epList;

  // Look through the list until the end
  while ( epSearch )
  {
    // Is there a match?
    if ( epSearch->epDesc == epDesc )
    {
      return ( epSearch->pfnDescCB );
    }
    else
      epSearch = epSearch->nextDesc;  // Next entry
  }

  return ( (pDescCB)NULL );
}

/*********************************************************************
 * @fn      afDataReqMTU
 *
 * @brief   Get the Data Request MTU(Max Transport Unit).
 *
 * @param   fields - afDataReqMTU_t
 *
 * @return  uint8(MTU)
 */
uint8 afDataReqMTU( afDataReqMTU_t* fields )
{
  uint8 len;
  uint8 hdr;

  if ( fields->kvp == TRUE )
  {
    hdr = AF_HDR_KVP_MAX_LEN;
  }
  else
  {
    hdr = AF_HDR_V1_1_MAX_LEN;
  }

  len = (uint8)(APSDE_DataReqMTU(&fields->aps) - hdr);

  return len;
}

/*********************************************************************
 * @fn      afGetMatch
 *
 * @brief   Set the allow response flag.
 *
 * @param   ep - Application Endpoint to look for
 * @param   action - true - allow response, false - no response
 *
 * @return  TRUE allow responses, FALSE no response
 */
uint8 afGetMatch( uint8 ep )
{
  epList_t *epSearch;

  // Look for the endpoint
  epSearch = afFindEndPointDescList( ep );

  if ( epSearch )
  {
    if ( epSearch->flags & eEP_AllowMatch )
      return ( TRUE );
    else
      return ( FALSE );
  }
  else
    return ( FALSE );
}

/*********************************************************************
 * @fn      afSetMatch
 *
 * @brief   Set the allow response flag.
 *
 * @param   ep - Application Endpoint to look for
 * @param   action - true - allow response, false - no response
 *
 * @return  TRUE if success, FALSE if endpoint not found
 */
uint8 afSetMatch( uint8 ep, uint8 action )
{
  epList_t *epSearch;

  // Look for the endpoint
  epSearch = afFindEndPointDescList( ep );

  if ( epSearch )
  {
    if ( action )
    {
      epSearch->flags |= eEP_AllowMatch;
    }
    else
    {
      epSearch->flags &= (eEP_AllowMatch ^ 0xFFFF);
    }
    return ( TRUE );
  }
  else
    return ( FALSE );
}

/*********************************************************************
 * @fn      afNumEndPoints
 *
 * @brief   Returns the number of endpoints defined (including 0)
 *
 * @param   none
 *
 * @return  number of endpoints
 */
byte afNumEndPoints( void )
{
  epList_t *epSearch;
  byte endpoints;

  // Start at the beginning
  epSearch = epList;
  endpoints = 0;

  while ( epSearch )
  {
    endpoints++;
    epSearch = epSearch->nextDesc;
  }

  return ( endpoints );
}

/*********************************************************************
 * @fn      afEndPoints
 *
 * @brief   Fills in the passed in buffer with the endpoint (numbers).
 *          Use afNumEndPoints to find out how big a buffer to supply.
 *
 * @param   epBuf - pointer to mem used
 *
 * @return  void
 */
void afEndPoints( byte *epBuf, byte skipZDO )
{
  epList_t *epSearch;
  byte endPoint;

  // Start at the beginning
  epSearch = epList;

  while ( epSearch )
  {
    endPoint = epSearch->epDesc->endPoint;

    if ( !skipZDO || endPoint != 0 )
      *epBuf++ = endPoint;

    epSearch = epSearch->nextDesc;
  }
}


void afCopyAddress ( afAddrType_t *afAddr, zAddrType_t *zAddr )
{
  afAddr->addrMode = (afAddrMode_t)zAddr->addrMode;
  if ( zAddr->addrMode == Addr64Bit )
    osal_cpyExtAddr( afAddr->addr.extAddr, zAddr->addr.extAddr );
  else
    afAddr->addr.shortAddr = zAddr->addr.shortAddr;
}

/*********************************************************************
*********************************************************************/

