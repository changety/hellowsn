/**************************************************************************************************
  Filename:       zcl_closures.h
  Revised:        $Date: 2006-09-08 15:51:21 -0700 (Fri, 08 Sep 2006) $
  Revision:       $Revision: 11934 $

  Description:    This file contains the ZCL Closures definitions.


  Copyright 2006-2007 Texas Instruments Incorporated. All rights reserved.

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

#ifndef ZCL_CLOSURES_H
#define ZCL_CLOSURES_H

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

/**********************************************/
/*** Shade Configuration Cluster Attributes ***/
/**********************************************/
  // Shade information attributes set
#define ATTRID_CLOSURES_PHYSICAL_CLOSED_LIMIT                          0x0000
#define ATTRID_CLOSURES_MOTOR_STEP_SIZE                                0x0001
#define ATTRID_CLOSURES_STATUS                                         0x0002
/*** Status attribute bit values ***/
#define CLOSURES_STATUS_SHADE_IS_OPERATIONAL                           0x01
#define CLOSURES_STATUS_SHADE_IS_ADJUSTING                             0x02
#define CLOSURES_STATUS_SHADE_DIRECTION                                0x04
#define CLOSURES_STATUS_SHADE_MOTOR_FORWARD_DIRECTION                  0x08
  // Shade settings attributes set
#define ATTRID_CLOSURES_CLOSED_LIMIT                                   0x0010
#define ATTRID_CLOSURES_MODE                                           0x0012
/*** Mode attribute values ***/
#define CLOSURES_MODE_NORMAL_MODE                                      0x00
#define CLOSURES_MODE_CONFIGURE_MODE                                   0x01
// cluster has no specific commands

/**********************************************/
/*** Logical Cluster ID - for mapping only ***/
/***  These are not to be used over-the-air ***/
/**********************************************/
#define ZCL_CLOSURES_LOGICAL_CLUSTER_ID_SHADE_CONFIG                   0x0010

/*********************************************************************
 * TYPEDEFS
 */
// This callback is just a place holder
//

typedef void (*zclClosures_PlaceHolder_t)( void );

// Register Callbacks table entry - enter function pointers for callbacks that
// the application would like to receive
typedef struct			
{
  zclClosures_PlaceHolder_t               pfnPlaceHolder; // Place Holder
//  NULL
} zclClosures_AppCallbacks_t;


 /*
  * Register for callbacks from this cluster library
  */
extern ZStatus_t zclClosures_RegisterCmdCallbacks( uint8 endpoint, zclClosures_AppCallbacks_t *callbacks );



/*********************************************************************
 * VARIABLES
 */


/*********************************************************************
 * FUNCTIONS
 */



/*********************************************************************
 * FUNCTION MACROS
 */



#ifdef __cplusplus
}
#endif

#endif /* ZCL_CLOSURES_H */
