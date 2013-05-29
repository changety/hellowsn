#include "ZComdef.h"
#include "AF.h"
#include "ZDObject.h"
#include "ZDConfig.h"

 /*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
NodeDescriptorFormat_t ZDO_Config_Node_Descriptor;
NodePowerDescriptorFormat_t ZDO_Config_Power_Descriptor;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * @fn      ZDConfig_InitDescriptors()
 *
 * @brief   Setup the ZDO descriptors 
 *             Node, Power
 *
 * @param   none
 *
 * @return  none
 */
void ZDConfig_InitDescriptors( void )
{
  ZDConfig_UpdateNodeDescriptor();
  ZDConfig_UpdatePowerDescriptor();
}

/*********************************************************************
 * @fn      ZDConfig_UpdateNodeDescriptor()
 *
 * @brief   Update the ZDO Node Descriptor 
 *
 * @param   none
 *
 * @return  none
 */
void ZDConfig_UpdateNodeDescriptor( void )
{
  // Build the Node Descriptor
  if ( ZG_BUILD_COORDINATOR_TYPE && ZG_DEVICE_COORDINATOR_TYPE )
    ZDO_Config_Node_Descriptor.LogicalType = NODETYPE_COORDINATOR;
  else if ( ZSTACK_ROUTER_BUILD )
    ZDO_Config_Node_Descriptor.LogicalType = NODETYPE_ROUTER;
  else if ( ZSTACK_END_DEVICE_BUILD )
    ZDO_Config_Node_Descriptor.LogicalType = NODETYPE_DEVICE;
  
  ZDO_Config_Node_Descriptor.ComplexDescAvail = FALSE;      // set elsewhere
  ZDO_Config_Node_Descriptor.UserDescAvail = FALSE;         // set elsewhere
  ZDO_Config_Node_Descriptor.Reserved = 0;                  // Reserved
  ZDO_Config_Node_Descriptor.APSFlags = 0;                  // NO APS flags
  ZDO_Config_Node_Descriptor.FrequencyBand = NODEFREQ_2400; // Frequency Band
  
  // MAC Capabilities
  if ( ZSTACK_ROUTER_BUILD )
  {
    ZDO_Config_Node_Descriptor.CapabilityFlags 
              = (CAPINFO_DEVICETYPE_FFD | CAPINFO_POWER_AC | CAPINFO_RCVR_ON_IDLE);
    
    if ( ZG_BUILD_COORDINATOR_TYPE && ZG_DEVICE_COORDINATOR_TYPE )
      ZDO_Config_Node_Descriptor.CapabilityFlags |= CAPINFO_ALTPANCOORD;
  }
  else if ( ZSTACK_END_DEVICE_BUILD )
  {
    ZDO_Config_Node_Descriptor.CapabilityFlags = (CAPINFO_DEVICETYPE_RFD
  #if ( RFD_RCVC_ALWAYS_ON == TRUE)
            | CAPINFO_RCVR_ON_IDLE
  #endif
        );
  }
  
  // Manufacturer Code - *YOU FILL IN*
  ZDO_Config_Node_Descriptor.ManufacturerCode[0] = 0;
  ZDO_Config_Node_Descriptor.ManufacturerCode[1] = 0;
  
  // Maximum Buffer Size
  ZDO_Config_Node_Descriptor.MaxBufferSize = MAX_BUFFER_SIZE;

  // Maximum Incoming Transfer Size Field
  ZDO_Config_Node_Descriptor.MaxInTransferSize[0] = LO_UINT16( MAX_TRANSFER_SIZE );
  ZDO_Config_Node_Descriptor.MaxInTransferSize[1] = HI_UINT16( MAX_TRANSFER_SIZE );
  
  // Maximum Outgoing Transfer Size Field
  ZDO_Config_Node_Descriptor.MaxOutTransferSize[0] = LO_UINT16( MAX_TRANSFER_SIZE );
  ZDO_Config_Node_Descriptor.MaxOutTransferSize[1] = HI_UINT16( MAX_TRANSFER_SIZE );
  
  // Server Mask
  ZDO_Config_Node_Descriptor.ServerMask = 0;
  
  // Descriptor Capability Field - extended active endpoint list and 
  // extended simple descriptor are not supported.
  ZDO_Config_Node_Descriptor.DescriptorCapability = 0;
}

/*********************************************************************
 * @fn      ZDConfig_UpdatePowerDescriptor()
 *
 * @brief   Update the ZDO Power Descriptor 
 *
 * @param   none
 *
 * @return  none
 */
void ZDConfig_UpdatePowerDescriptor( void )
{
  // Build the Power Descriptor  
  if ( ZSTACK_ROUTER_BUILD )
  {
    ZDO_Config_Power_Descriptor.PowerMode = NODECURPWR_RCVR_ALWAYS_ON;
    ZDO_Config_Power_Descriptor.AvailablePowerSources = NODEAVAILPWR_MAINS;
    ZDO_Config_Power_Descriptor.CurrentPowerSource = NODEAVAILPWR_MAINS;
    ZDO_Config_Power_Descriptor.CurrentPowerSourceLevel = NODEPOWER_LEVEL_100;
  }
  else if ( ZSTACK_END_DEVICE_BUILD )
  {
    if ( zgPollRate )
      ZDO_Config_Power_Descriptor.PowerMode = NODECURPWR_RCVR_AUTO;
    else
      ZDO_Config_Power_Descriptor.PowerMode = NODECURPWR_RCVR_STIM;
      
    ZDO_Config_Power_Descriptor.AvailablePowerSources = NODEAVAILPWR_RECHARGE;
    ZDO_Config_Power_Descriptor.CurrentPowerSource = NODEAVAILPWR_RECHARGE;
    ZDO_Config_Power_Descriptor.CurrentPowerSourceLevel = NODEPOWER_LEVEL_66;
  }
}

/*********************************************************************
*********************************************************************/


