#ifndef ZDCONFIG_H
#define ZDCONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "AF.h"

/*********************************************************************
 * Enable Features
 */
#if defined ( MT_ZDO_FUNC )
  // All of the ZDO functions are enabled for ZTool use.
  #define ZDO_NWKADDR_REQUEST
  #define ZDO_IEEEADDR_REQUEST
  #define ZDO_MATCH_REQUEST
  #define ZDO_NODEDESC_REQUEST
  #define ZDO_POWERDESC_REQUEST
  #define ZDO_SIMPLEDESC_REQUEST
  #define ZDO_ACTIVEEP_REQUEST

  #define ZDO_COMPLEXDESC_REQUEST
  #define ZDO_USERDESC_REQUEST
  #define ZDO_USERDESCSET_REQUEST
  #define ZDO_ENDDEVICEBIND_REQUEST
  #define ZDO_BIND_UNBIND_REQUEST
  #define ZDO_SERVERDISC_REQUEST
  #define ZDO_NETWORKSTART_REQUEST
  #define ZDO_LINK_KEY_MANAGEMENT

  #define ZDO_COMPLEXDESC_RESPONSE
  #define ZDO_USERDESC_RESPONSE
  #define ZDO_USERDESCSET_RESPONSE
  #define ZDO_SERVERDISC_RESPONSE
  #define ZDO_ENDDEVICE_ANNCE

#if defined ( MT_ZDO_MGMT )
  #define ZDO_MGMT_NWKDISC_REQUEST
  #define ZDO_MGMT_LQI_REQUEST
  #define ZDO_MGMT_RTG_REQUEST
  #define ZDO_MGMT_BIND_REQUEST
  #define ZDO_MGMT_LEAVE_REQUEST
  #define ZDO_MGMT_JOINDIRECT_REQUEST
  #define ZDO_MGMT_PERMIT_JOIN_REQUEST
  #define ZDO_MGMT_NWKUPDATE_REQUEST
  #define ZDO_MGMT_NWKDISC_RESPONSE
  #define ZDO_MGMT_LQI_RESPONSE
  #define ZDO_MGMT_RTG_RESPONSE
  #define ZDO_MGMT_BIND_RESPONSE
  #define ZDO_MGMT_LEAVE_RESPONSE
  #define ZDO_MGMT_JOINDIRECT_RESPONSE
  #define ZDO_MGMT_PERMIT_JOIN_RESPONSE
  #define ZDO_MGMT_NWKUPDATE_NOTIFY
#endif

#else // !MT_ZDO_FUNC

  // Normal operation and sample apps only use End Device Bind
  // and Match Request.

  //#define ZDO_NWKADDR_REQUEST
  //#define ZDO_IEEEADDR_REQUEST
  #define ZDO_MATCH_REQUEST
  //#define ZDO_NODEDESC_REQUEST
  //#define ZDO_POWERDESC_REQUEST
  //#define ZDO_SIMPLEDESC_REQUEST
  //#define ZDO_ACTIVEEP_REQUEST
  //#define ZDO_COMPLEXDESC_REQUEST
  //#define ZDO_USERDESC_REQUEST
  //#define ZDO_USERDESCSET_REQUEST
  #define ZDO_ENDDEVICEBIND_REQUEST
  //#define ZDO_BIND_UNBIND_REQUEST
  //#define ZDO_SERVERDISC_REQUEST
  //#define ZDO_NETWORKSTART_REQUEST
  //#define ZDO_LINK_KEY_MANAGEMENT

  //#define ZDO_BIND_UNBIND_RESPONSE
  //#define ZDO_COMPLEXDESC_RESPONSE
  //#define ZDO_USERDESC_RESPONSE
  //#define ZDO_USERDESCSET_RESPONSE
  //#define ZDO_SERVERDISC_RESPONSE
  #define ZDO_ENDDEVICE_ANNCE

  //#define ZDO_MGMT_NWKDISC_REQUEST
  //#define ZDO_MGMT_LQI_REQUEST
  //#define ZDO_MGMT_RTG_REQUEST
  //#define ZDO_MGMT_BIND_REQUEST
  //#define ZDO_MGMT_LEAVE_REQUEST
  //#define ZDO_MGMT_JOINDIRECT_REQUEST
  //#define ZDO_MGMT_PERMIT_JOIN_REQUEST
  //#define ZDO_MGMT_NWKDISC_RESPONSE
  //#define ZDO_MGMT_LQI_RESPONSE
  //#define ZDO_MGMT_RTG_RESPONSE
  //#define ZDO_MGMT_BIND_RESPONSE
  //#define ZDO_MGMT_LEAVE_RESPONSE
  //#define ZDO_MGMT_JOINDIRECT_RESPONSE
  //#define ZDO_MGMT_PERMIT_JOIN_RESPONSE

#if defined ( REFLECTOR  )
  // Binding needs this request to do a 64 to 16 bit conversion
#if !defined(ZDO_NWKADDR_REQUEST)
  #define ZDO_NWKADDR_REQUEST
#endif
#if !defined(ZDO_IEEEADDR_REQUEST)
  #define ZDO_IEEEADDR_REQUEST
#endif
  #define ZDO_BIND_UNBIND_RESPONSE
#endif

#endif  // !MT_ZDO_FUNC

/*********************************************************************
 * Constants
 */

#define MAX_BUFFER_SIZE		        	80
  
#if defined ( ZIGBEE_FRAGMENTATION )
  // The application/profile must fill this field out.
  #define MAX_TRANSFER_SIZE	        	160
#else
  #define MAX_TRANSFER_SIZE	        	80
#endif
  
#define MAX_ENDPOINTS	            	240

// Node Description Bitfields
#define ZDOLOGICALTYPE_MASK		    	0x07
#define ZDOAPSFLAGS_MASK		      	0x07
#define ZDOFREQUENCYBANDS_MASK    	0x1F
#define ZDOAPSFLAGS_BITLEN	    		3

#define SIMPLE_DESC_DATA_SIZE				7
#define NODE_DESC_DATA_SIZE					10

// Simple Description Bitfields
#define ZDOENDPOINT_BITLEN		      5
#define ZDOENDPOINT_MASK		        0x1F
#define ZDOINTERFACE_MASK	      		0x07
#define ZDOAPPFLAGS_MASK	      		0x0F
#define ZDOAPPDEVVER_MASK		      	0x0F
#define ZDOAPPDEVVER_BITLEN		    	4

/*********************************************************************
 * Attributes
 */

extern NodeDescriptorFormat_t ZDO_Config_Node_Descriptor;
extern NodePowerDescriptorFormat_t ZDO_Config_Power_Descriptor;

/*********************************************************************
 * Function Prototypes
 */
extern void ZDConfig_InitDescriptors( void );
extern void ZDConfig_UpdateNodeDescriptor( void );
extern void ZDConfig_UpdatePowerDescriptor( void );



/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ZDCONFIG_H */
