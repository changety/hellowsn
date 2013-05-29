/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "sapi.h"
#include "aps_groups.h"
#include "ZDApp.h"
#include "NodeCommon.h"
#include "WSNMonitorAppHw.h"
#include "OnBoard.h"
/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "ds18b20.c"
#include <string.h>

//#include "mac_radio_defs.h"
/*********************************************************************
 * MACROS
 */

/* CONSTANTS
 */
// General UART frame offsets
// Stack Profile
#define ZIGBEE_2007                         0x0040
#define ZIGBEE_PRO_2007                     0x0041
#ifdef ZIGBEEPRO
#define STACK_PROFILE                       ZIGBEE_PRO_2007             
#else 
#define STACK_PROFILE                       ZIGBEE_2007
#endif

#define ACK_REQ_INTERVAL                    5 

#define FRAME_SOF_OFFSET                    0
#define FRAME_LENGTH_OFFSET                 1 
#define FRAME_CMD0_OFFSET                   2
#define FRAME_CMD1_OFFSET                   3
#define FRAME_DATA_OFFSET                   4

// ZB_RECEIVE_DATA_INDICATION offsets
#define ZB_RECV_SRC_OFFSET                  0
#define ZB_RECV_CMD_OFFSET                  2
#define ZB_RECV_LEN_OFFSET                  4
#define ZB_RECV_DATA_OFFSET                 6
#define ZB_RECV_FCS_OFFSET                  8

#define CPT_SOP                             0xFE

#define SYS_PING_REQUEST                    0x0021
#define SYS_PING_RESPONSE                   0x0161
#define ZB_RECEIVE_DATA_INDICATION          0x8746


// ZB_RECEIVE_DATA_INDICATION frame length
#define ZB_RECV_LENGTH                      15

// PING response frame length and offset
#define SYS_PING_RSP_LENGTH                 7 
#define SYS_PING_CMD_OFFSET                 1
/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
  uint16              source;
  uint16              parent;
  uint8               temp;
  uint8               voltage;
} gtwData_t;
/*********************************************************************
 * GLOBAL VARIABLES
 */
const  cId_t WSNMonitorApp_ClusterList[WSNMonitorApp_MAX_CLUSTERS] =
{
  WSNMonitorApp_PERIODIC_CLUSTERID,
  WSNMonitorApp_GROUP_CLUSTERID,
  WSNMonitorApp_ADDUNICAST_CLUSTERID,
  WSNMonitorApp_UNICASTTEMP_CLUSTERID,
  WSN_NODE_CONTROL_CMD_ID,
  WSN_PERIOD_REPORT_CMD_ID           
};
const SimpleDescriptionFormat_t WSNMonitorApp_SimpleDesc =
{
  WSNMonitorApp_ENDPOINT,              //  int Endpoint;
  WSNMonitorApp_PROFID,                //  uint16 AppProfId[2];
  WSNMonitorApp_DEVICEID,              //  uint16 AppDeviceId[2];
  WSNMonitorApp_DEVICE_VERSION,        //  int   AppDevVer:4;
  WSNMonitorApp_FLAGS,                 //  int   AppFlags:4;
  WSNMonitorApp_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)WSNMonitorApp_ClusterList,  //  uint8 *pAppInClusterList;
  WSNMonitorApp_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)WSNMonitorApp_ClusterList   //  uint8 *pAppInClusterList;
};
// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in WSNMonitorApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t WSNMonitorApp_epDesc;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
#define ROUTER_REPORT_CYCLE_SLOW 0x01
#define ROUTER_REPORT_CYCLE_NORMAL 0x02
#define ROUTER_REPORT_CYCLE_FREQUENCE 0x03

#define CYCLE_SLOW 60000
#define CYCLE_NORMAL 30000
#define CYCLE_FREQUENCE 10000 
/*********************************************************************
 * LOCAL VARIABLES
 */
    //static uint16 parentShortAddr = NLME_GetCoordShortAddr();
    static uint16 myReportPeriod = CYCLE_SLOW;   
    uint8 WSNMonitorApp_TaskID;   
    devStates_t WSNMonitorApp_NwkState;                                                                
    uint8 WSNMonitorApp_TransID;  // This is the unique message ID (counter)
    afAddrType_t WSNMonitorApp_Periodic_DstAddr;
    afAddrType_t WSNMonitorApp_Group_DstAddr;
    afAddrType_t WSNMonitorApp_UnicastAdd_DstAddr;
    afAddrType_t WSNMonitorApp_UnicastTemp_DstAddr;
    //afAddrType_t WSNMonitorApp_UnicastIEEEAdd_DstAddr;  
    aps_Group_t WSNMonitorApp_Group;
    uint8 WSNMonitorAppPeriodicCounter = 0;
    uint8 WSNMonitorAppFlashCounter = 0;
    static uint8 sleep = 0xFC;
    static uint8 voltageSetting = 0x21;  //3.3v
    

    

/*********************************************************************
 * LOCAL FUNCTIONS
 */
void WSNMonitorApp_HandleKeys( uint8 shift, uint8 keys );
void WSNApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void WSNMonitorApp_SendDataToGroup( void );
void WSNMonitorApp_SendAddDataUnicast( void );
void WSNMonitorApp_SendTempDataUnicast( void );
void send_Router_PeriodReport(void);
/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      WSNMonitorApp_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void WSNMonitorApp_Init( uint8 task_id )
{ 
   //HAL_PA_LNA_RX_HGM();
  WSNMonitorApp_TaskID = task_id;
  WSNMonitorApp_NwkState = DEV_INIT;
  WSNMonitorApp_TransID = 0;
 

 #if defined ( BUILD_ALL_DEVICES ) //预编译不通过

  if ( readCoordinatorJumper() )
    zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
  else
    zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
#endif // BUILD_ALL_DEVICES

#if defined ( HOLD_AUTO_START )//
  ZDOInitDevice(0);
#endif
  
  //osal_set_event(task_id, ZB_ENTRY_EVENT);
  
  
  osal_set_event(task_id, MY_REPORT_EVT);
  
  //广播模式
  WSNMonitorApp_Periodic_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  WSNMonitorApp_Periodic_DstAddr.endPoint = WSNMonitorApp_ENDPOINT;
  WSNMonitorApp_Periodic_DstAddr.addr.shortAddr = 0xFFFF;   //广播模式的一种
  
  //组播模式
  WSNMonitorApp_Group_DstAddr.addrMode = (afAddrMode_t)afAddrGroup;
  WSNMonitorApp_Group_DstAddr.endPoint = WSNMonitorApp_ENDPOINT;
  WSNMonitorApp_Group_DstAddr.addr.shortAddr = WSNMonitorApp_TEMP_GROUP;
  WSNMonitorApp_epDesc.endPoint = WSNMonitorApp_ENDPOINT;
  WSNMonitorApp_epDesc.task_id = &WSNMonitorApp_TaskID;
  WSNMonitorApp_epDesc.simpleDesc = (SimpleDescriptionFormat_t *)&WSNMonitorApp_SimpleDesc;
  WSNMonitorApp_epDesc.latencyReq = noLatencyReqs;
 
//点对点的发送地址方式
WSNMonitorApp_UnicastAdd_DstAddr.addrMode = (afAddrMode_t)afAddr16Bit;
WSNMonitorApp_UnicastAdd_DstAddr.endPoint = WSNMonitorApp_ENDPOINT;
//WSNMonitorApp_UnicastAdd_DstAddr.addr.shortAddr = NLME_GetCoordShortAddr(); //发送到该节点的父节点
WSNMonitorApp_UnicastAdd_DstAddr.addr.shortAddr = 0x0000; //发送到协调器上
WSNMonitorApp_epDesc.endPoint = WSNMonitorApp_ENDPOINT;
WSNMonitorApp_epDesc.task_id = &WSNMonitorApp_TaskID;
WSNMonitorApp_epDesc.simpleDesc
= (SimpleDescriptionFormat_t *)&WSNMonitorApp_SimpleDesc;
WSNMonitorApp_epDesc.latencyReq = noLatencyReqs;

//点对点的发送温度方式
WSNMonitorApp_UnicastTemp_DstAddr.addrMode = (afAddrMode_t)afAddr16Bit;
WSNMonitorApp_UnicastTemp_DstAddr.endPoint = WSNMonitorApp_ENDPOINT;
WSNMonitorApp_UnicastTemp_DstAddr.addr.shortAddr = 0x0000;
WSNMonitorApp_epDesc.endPoint = WSNMonitorApp_ENDPOINT;
WSNMonitorApp_epDesc.task_id = &WSNMonitorApp_TaskID;
WSNMonitorApp_epDesc.simpleDesc
= (SimpleDescriptionFormat_t *)&WSNMonitorApp_SimpleDesc;
WSNMonitorApp_epDesc.latencyReq = noLatencyReqs;


  // Register the endpoint description with the AF
  afRegister( &WSNMonitorApp_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( WSNMonitorApp_TaskID );
  // By default, all devices start out in Group 1
  WSNMonitorApp_Group.ID = 0x0001;
  osal_memcpy( WSNMonitorApp_Group.name, "Group 0x0001", 7  );
  aps_AddGroup( WSNMonitorApp_ENDPOINT, &WSNMonitorApp_Group );

#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "WSNApp", HAL_LCD_LINE_1 );
#endif
}

/*********************************************************************
 * @fn      WSNMonitorApp_ProcessEvent
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 WSNMonitorApp_ProcessEvent( uint8 task_id, uint16 events ) //事件处理函数
{
  afIncomingMSGPacket_t *MSGpkt;
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG ) //当有系统消息传来
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( WSNMonitorApp_TaskID );
    while ( MSGpkt )  //当有消息传来
    {
      switch ( MSGpkt->hdr.event )
      {
        // Received when a key is pressed
        case KEY_CHANGE:  //键值改变消息
          WSNMonitorApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;
        // Received when a messages is received (OTA) for this endpoint
        case AF_INCOMING_MSG_CMD://消息处理
          WSNApp_MessageMSGCB( MSGpkt );
          break;
        // Received whenever the device changes state in the network
        case ZDO_STATE_CHANGE: //ZDO状态改变
          WSNMonitorApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          //此处可以分开if，然后根据不同设备处理不同函数。
          if ( (WSNMonitorApp_NwkState == DEV_ZB_COORD)
              || (WSNMonitorApp_NwkState == DEV_ROUTER)
              || (WSNMonitorApp_NwkState == DEV_END_DEVICE) )
          {
            // Start sending the periodic message in a regular interval.  周期性发送数据
            osal_start_timerEx( WSNMonitorApp_TaskID,
                              WSNMonitorApp_SEND_PERIODIC_MSG_EVT,  //设置周期性广播事件
                              WSNMonitorApp_SEND_PERIODIC_MSG_TIMEOUT );//设置周期
          }
          else
          {
            // Device is no longer in the network
          }
          break;
        default:
          break;
      }
      // Release the memory
     osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next - if one is available
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( WSNMonitorApp_TaskID );
    }
    // return unprocessed events
    return (events ^ SYS_EVENT_MSG); //再次传回系统事件消息，重新判断，直到没有为止。
  }

  
  if ( events & MY_REPORT_EVT )
  {   
    //如果使能发送
   if (sleep == 0xFC) 
   {
      send_Router_PeriodReport();
   }
      osal_start_timerEx(WSNMonitorApp_TaskID, MY_REPORT_EVT, myReportPeriod);
  }

  // Discard unknown events
  return 0;
}



/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      WSNMonitorApp_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
void WSNMonitorApp_HandleKeys( uint8 shift, uint8 keys )
{
  
  (void)shift;  // Intentionally unreferenced parameter
  
  if ( keys & HAL_KEY_SW_1 ) // key up
  {  
   // WSNMonitorApp_SendDataToGroup();
  }

  if ( keys & HAL_KEY_SW_2 )//keyright
  {
    aps_Group_t *grp;
    grp = aps_FindGroup( WSNMonitorApp_ENDPOINT, WSNMonitorApp_TEMP_GROUP );
    if ( grp )
    {
      // Remove from the group
      aps_RemoveGroup( WSNMonitorApp_ENDPOINT, WSNMonitorApp_TEMP_GROUP );
      HalLcdWriteString( "RemoveGroup", HAL_LCD_LINE_4 );
    }
    else
    {
      // Add to the flash group
      aps_AddGroup( WSNMonitorApp_ENDPOINT, &WSNMonitorApp_Group );
      HalLcdWriteString( "AddGroup", HAL_LCD_LINE_4 );
    }    
  }
  if ( keys & HAL_KEY_SW_3 ) //keydown
  {  
  }
   if ( keys & HAL_KEY_SW_4 ) //keyleft
  {   

  }
   if ( keys & HAL_KEY_SW_5) //keycenter
  {   

  }
   if ( keys & HAL_KEY_SW_6) //大板子的S1是灵光的
  {   
    WSNMonitorApp_SendAddDataUnicast();
  }
   if ( keys & HAL_KEY_SW_7) // 小板子的S2  
  {  
    WSNMonitorApp_SendTempDataUnicast();
    // WSNMonitorApp_SendAddDataUnicast();
    // HalLedBlink (HAL_LED_2, 2, 20, 0x1111);
  //  WSNMonitorApp_SendAddDataUnicast();
  }
  if ( keys & HAL_KEY_SW_8)  //小板子S3 大板子的S6
  {       
   // WSNMonitorApp_SendAddDataUnicast();
  }
  if ( keys & HAL_KEY_SW_9) //S1
  {     
    //WSNMonitorApp_SendAddDataUnicast();
  }
}
/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      WSNApp_MessageMSGCB
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
//消息处理函数
void WSNApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{ 
   char temp[16] = "temp:";
  uint16 sourceAddress;
  uint16 parentAddress;
  uint8 unicastData[2];
  switch ( pkt->clusterId )
  {  
    case WSNMonitorApp_PERIODIC_CLUSTERID:
      HalLedBlink( HAL_LED_1, 4, 50, (5000 / 4) );
      break;

    case WSNMonitorApp_GROUP_CLUSTERID:     
          //再第四行显示出   发送组播消息设备的网络地址
      sourceAddress = BUILD_UINT16(pkt->cmd.Data[0], pkt->cmd.Data[1] );
      
      HalLcdWriteStringValue( "groupM:", sourceAddress, 16, HAL_LCD_LINE_4);
      
      break;
    case WSNMonitorApp_ADDUNICAST_CLUSTERID:
     //当收到来自节点的 子父节点信息的时候， 在3 4 行显示出节点自身 以及 父节点的网络短地址信息    
      sourceAddress = BUILD_UINT16(pkt->cmd.Data[0], pkt->cmd.Data[1] );
      parentAddress = BUILD_UINT16(pkt->cmd.Data[2], pkt->cmd.Data[3] );
      HalLcdWriteStringValue( "source:", sourceAddress, 16, HAL_LCD_LINE_3);
      HalLcdWriteStringValue( "parent:", parentAddress, 16, HAL_LCD_LINE_4);
       break;
     case WSNMonitorApp_UNICASTTEMP_CLUSTERID: 
       //当收到来自节点的 温度信息的时候， 在3,4 行显示出节 温度信息 以及 网络短地址信息 
      strcat(temp,(char*)pkt->cmd.Data);
 //     unicastData = pkt->cmd.Data[0];//unicastData  就是接收到的数据

      HalLcdWriteString (temp , HAL_LCD_LINE_3);
      HalLcdWriteStringValue( "source:", pkt->srcAddr.addr.shortAddr, 16, HAL_LCD_LINE_4);
      break;
     case WSN_NODE_CONTROL_CMD_ID: 
       
     unicastData[0] = pkt->cmd.Data[0];//unicastData  就是接收到的数据
     unicastData[1] = pkt->cmd.Data[1];
     if(unicastData[0] == 0xDA)
     {
       if(unicastData[1] == 0xFA)
       {
         HalLedSet (HAL_LED_2, HAL_LED_MODE_OFF);
       }
       else if (unicastData[1] == 0xFB)
       {
         HalLedSet (HAL_LED_2, HAL_LED_MODE_ON);
       }
     }
     else if(unicastData[0] == 0xDC)
     {
       voltageSetting = unicastData[1];
     }
     else if(unicastData[0] == 0xDD)
     {
        sleep = 0xFD;
     }
     else if(unicastData[0] == 0xDE)
     {  
        sleep = 0xFC;
        switch ( unicastData[1] )
        {  
           case ROUTER_REPORT_CYCLE_SLOW:
             myReportPeriod = CYCLE_SLOW;
             break;
           case ROUTER_REPORT_CYCLE_NORMAL:     
             myReportPeriod = CYCLE_NORMAL;
             break;
           case ROUTER_REPORT_CYCLE_FREQUENCE:
             myReportPeriod = CYCLE_FREQUENCE;
             break;
           default :
             myReportPeriod = CYCLE_SLOW;
             break;
       }  
     }
     break; 
      
  }
}
/*********************************************************************
 * @fn      WSNMonitorApp_SendPeriodicMessage
 *
 * @brief   Send the periodic message.
 *
 * @param   none
 *
 * @return  none
 */

/*****************************************
  *By Change_Ty 组播函数  begin
  ******************************************/ 
void WSNMonitorApp_SendDataToGroup( void )
{ 
  //组播都发出自己的短地址，用来说明是谁发出去的组播消息 
  uint16 sourceAddr = NLME_GetShortAddr(); 
  uint8 buffer[2];
  buffer[0] = LO_UINT16(sourceAddr);
  buffer[1] = HI_UINT16(sourceAddr);
  
  if ( AF_DataRequest( &WSNMonitorApp_Group_DstAddr, //发送目的地址+端点地址+传送模式
                       &WSNMonitorApp_epDesc, //简单描述符
                       WSNMonitorApp_GROUP_CLUSTERID,//簇ID
                       2, //有效数据长度                           
                       buffer, //数据
                       &WSNMonitorApp_TransID,
                       AF_DISCV_ROUTE, //路由方式选择
                       AF_DEFAULT_RADIUS //路由深度
                         ) == afStatus_SUCCESS )
  {
    
  }
  else
  {
    // Error occurred in request to send.
  }
}
  /*****************************************
  *By Change_Ty 组播函数 end.
  ******************************************/

/*****************************************
  *By Change_Ty 发送地址消息函数  begin
  ******************************************/
void WSNMonitorApp_SendAddDataUnicast( void )
{
 
  uint16 sourceAdd = NLME_GetShortAddr();
  uint16 parentAdd = NLME_GetCoordShortAddr();
  uint8 buffer[4];
  buffer[0] = LO_UINT16( sourceAdd );
  buffer[1] = HI_UINT16( sourceAdd );
  buffer[2] = LO_UINT16( parentAdd );
  buffer[3] = HI_UINT16( parentAdd );
  if ( AF_DataRequest( &WSNMonitorApp_UnicastAdd_DstAddr, //发送目的地址+端点地址+传送模式
                       &WSNMonitorApp_epDesc, //端点描述符
                       WSNMonitorApp_ADDUNICAST_CLUSTERID ,//簇ID
                       4,
                       buffer,
                       &WSNMonitorApp_TransID,
                       AF_DISCV_ROUTE, //路由方式选择
                       AF_DEFAULT_RADIUS //路由深度
                         ) == afStatus_SUCCESS )
  {
    
  }
  else
  {
    // Error occurred in request to send.
  }
}
/*****************************************
  *By Change_Ty 发送地址消息函数 end
  ******************************************/

/*****************************************
  *By Change_Ty 发送温度消息函数  begin
  ******************************************/
void WSNMonitorApp_SendTempDataUnicast( void )
{
  
  Init_DS18B20();
  ReadTemperature();
  gettempstr();
 
  if ( AF_DataRequest( &WSNMonitorApp_UnicastTemp_DstAddr, //发送目的地址+端点地址+传送模式
                       &WSNMonitorApp_epDesc, //端点描述符
                       WSNMonitorApp_UNICASTTEMP_CLUSTERID ,//簇ID
                      (byte)osal_strlen( ch ) + 1, //有效数据长度                           
                      (byte *)&ch, //数据
                       &WSNMonitorApp_TransID,
                       AF_DISCV_ROUTE, //路由方式选择
                       AF_DEFAULT_RADIUS //路由深度
                         ) == afStatus_SUCCESS )
  {
    
  }
  else
  {
    // Error occurred in request to send.
  }
}

/*****************************************
  *By Change_Ty 发送温度消息函数  end
  ******************************************/


void send_Router_PeriodReport(void)
{
  uint8 pData[SENSOR_REPORT_LENGTH];
  //static uint8 reportNr=0;
  //uint8 txOptions;
  
  // Read and report temperature value
  pData[SENSOR_TEMP_OFFSET] =  0xFF;
  pData[SENSOR_VOLTAGE_OFFSET] = 0xFF; 
    
  pData[SENSOR_PARENT_OFFSET] =  HI_UINT16(NLME_GetCoordShortAddr());
  pData[SENSOR_PARENT_OFFSET+ 1] =  LO_UINT16(NLME_GetCoordShortAddr());
  pData[SENSOR_PARENT_OFFSET + 2] = aExtendedAddress[7];
  pData[SENSOR_PARENT_OFFSET + 3] = aExtendedAddress[6];
  pData[SENSOR_PARENT_OFFSET + 4] = aExtendedAddress[5];
  pData[SENSOR_PARENT_OFFSET + 5] = aExtendedAddress[4];
  pData[SENSOR_PARENT_OFFSET + 6] = aExtendedAddress[3];
  pData[SENSOR_PARENT_OFFSET + 7] = aExtendedAddress[2];
  pData[SENSOR_PARENT_OFFSET + 8] = aExtendedAddress[1];
  pData[SENSOR_PARENT_OFFSET + 9] = aExtendedAddress[0];
  
  
  AF_DataRequest( &WSNMonitorApp_UnicastAdd_DstAddr, //发送目的地址+端点地址+传送模式
                       &WSNMonitorApp_epDesc, //端点描述符
                       WSN_PERIOD_REPORT_CMD_ID ,//报告 CMD_ID 区别一般的 CMD_ID
                       SENSOR_REPORT_LENGTH,
                       pData,
                       &WSNMonitorApp_TransID,
                       AF_DISCV_ROUTE, //路由方式选择
                       AF_DEFAULT_RADIUS //路由深度
                         ) ;
}

