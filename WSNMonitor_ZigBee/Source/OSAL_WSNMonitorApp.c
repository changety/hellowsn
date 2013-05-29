#include "ZComDef.h"
#include "hal_drivers.h"
#include "OSAL.h"
#include "OSAL_Tasks.h"
#if defined ( MT_TASK )
#include "MT.h"
#include "MT_TASK.h"
#endif
#include "nwk.h"
#include "APS.h"
#include "ZDApp.h"
#if defined ( ZIGBEE_FREQ_AGILITY ) || defined ( ZIGBEE_PANID_CONFLICT )
  #include "ZDNwkMgr.h"
#endif
#if defined ( ZIGBEE_FRAGMENTATION )
  #include "aps_frag.h"
#endif
#include "NodeCommon.h"

/*********************************************************************
 * GLOBAL VARIABLES
 */

// The order in this table must be identical to the task initialization calls below in osalInitTask.
const pTaskEventHandlerFn tasksArr[] = {
  macEventLoop,//mac层任务处理函数
  nwk_event_loop,//网络层
  Hal_ProcessEvent,//硬件抽象层
#if defined( MT_TASK )
  MT_ProcessEvent,
#endif
  APS_event_loop,//应用层任务处理函数，用户不要更改
#if defined ( ZIGBEE_FRAGMENTATION )
  APSF_ProcessEvent,
#endif
  ZDApp_event_loop,//zigbee设备应用层任务处理函数，用户可以根据需要更改
#if defined ( ZIGBEE_FREQ_AGILITY ) || defined ( ZIGBEE_PANID_CONFLICT )
  ZDNwkMgr_event_loop,
#endif
  WSNMonitorApp_ProcessEvent
  //SensorSent_ProcessEvent;
};

const uint8 tasksCnt = sizeof( tasksArr ) / sizeof( tasksArr[0] );
uint16 *tasksEvents;

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn      osalInitTasks
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */
void osalInitTasks( void )
{
  uint8 taskID = 0;

  tasksEvents = (uint16 *)osal_mem_alloc( sizeof( uint16 ) * tasksCnt);
  osal_memset( tasksEvents, 0, (sizeof( uint16 ) * tasksCnt));

  macTaskInit( taskID++ );
  nwk_init( taskID++ );
  Hal_Init( taskID++ );
#if defined( MT_TASK )
  MT_TaskInit( taskID++ );
#endif
  APS_Init( taskID++ );
#if defined ( ZIGBEE_FRAGMENTATION )
  APSF_Init( taskID++ );
#endif
  ZDApp_Init( taskID++ );
#if defined ( ZIGBEE_FREQ_AGILITY ) || defined ( ZIGBEE_PANID_CONFLICT )
  ZDNwkMgr_Init( taskID++ );
#endif
  WSNMonitorApp_Init( taskID++ );
}

/*********************************************************************
*********************************************************************/
