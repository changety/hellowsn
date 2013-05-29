#ifndef WSNMonitorApp_H
#define WSNMonitorApp_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "hal_uart.h"
//#include "WSNMonitorAppCommon.c"
/*********************************************************************
 * CONSTANTS
 */

// These constants are only for example and should be changed to the
// device's needs
#define WSNMonitorApp_ENDPOINT           20

#define WSNMonitorApp_PROFID             0x0F08
#define WSNMonitorApp_DEVICEID           0x0001
#define WSNMonitorApp_DEVICE_VERSION     0
#define WSNMonitorApp_FLAGS              0

#define WSNMonitorApp_MAX_CLUSTERS       6
#define WSNMonitorApp_PERIODIC_CLUSTERID      1
#define WSNMonitorApp_GROUP_CLUSTERID         2
#define WSNMonitorApp_ADDUNICAST_CLUSTERID    3
#define WSNMonitorApp_UNICASTTEMP_CLUSTERID   4
#define WSN_NODE_CONTROL_CMD_ID           5 
#define WSN_PERIOD_REPORT_CMD_ID          6
 
  
  

  
// Send Message Timeout
#define WSNMonitorApp_SEND_PERIODIC_MSG_TIMEOUT   5000     // Every 5 seconds

// Application Events (OSAL) - These are bit weighted definitions.
#define WSNMonitorApp_SEND_PERIODIC_MSG_EVT       0x0001
#define MY_START_EVT                        0x0002
#define MY_REPORT_EVT                       0x0003
#define MY_FIND_COLLECTOR_EVT               0x0004
  
// Group ID for Flash Command
#define WSNMonitorApp_TEMP_GROUP                  0x0001
  
// Flash Command Duration - in milliseconds
#define WSNMonitorApp_FLASH_DURATION               1000

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the Generic Application
 */
extern void WSNMonitorApp_Init( uint8 task_id );
/*
 * Task Event Processor for the Generic Application
 */
extern UINT16 WSNMonitorApp_ProcessEvent( uint8 task_id, uint16 events );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* WSNMonitorApp_H */

/**********************************************************************
******************************************************************************/
// Define the Command ID's used in this application

// Sensor report data format
#define SENSOR_TEMP_OFFSET                0
#define SENSOR_VOLTAGE_OFFSET             1
#define SENSOR_PARENT_OFFSET              2
#define SENSOR_REPORT_LENGTH              12

#define RX_BUF_LEN                        128

/******************************************************************************
 * PUBLIC FUNCTIONS
 */
void initUart(halUARTCBack_t pf);
void uartRxCB( uint8 port, uint8 event );