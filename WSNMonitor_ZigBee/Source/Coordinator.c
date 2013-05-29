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
/*****************************************
  *Added by Change_Ty for demonstrating UART. begin.
  ******************************************/
#include "MT_UART.h"
#include "MT.h"
  /*****************************************
  *Added by Change_Ty for demonstrating UART. end.
  ******************************************/
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
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

#define NODE_CPT_SOP                        0xFE
#define ROUTER_CPT_SOP                      0xFD
#define COOR_CPT_SOP                        0xFC
#define SYS_PING_REQUEST                    0x0021
#define SYS_PING_RESPONSE                   0x0161
#define ZB_RECEIVE_DATA_INDICATION          0x8746


// ZB_RECEIVE_DATA_INDICATION frame length
#define ZB_RECV_LENGTH                     23
//定义协调器组网报告消息
#define ZB_WSN_REPORT                      13

// PING response frame length and offset
#define SYS_PING_RSP_LENGTH                 7 
#define SYS_PING_CMD_OFFSET                 1
/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
  uint16              source;//包里该节点自己的网络短地址
  uint16              parent;//包里该节点的父节点的网络短地址
  uint8               temp;
  uint8               voltage;
  uint8            IEEEAdd0;
  uint8            IEEEAdd1;
  uint8            IEEEAdd2;
  uint8            IEEEAdd3;
  uint8            IEEEAdd4;
  uint8            IEEEAdd5;
  uint8            IEEEAdd6;
  uint8            IEEEAdd7;
} gtwData_t;

/*********************************************************************
 * GLOBAL VARIABLES
*/
// This list should be filled with Application specific Cluster IDs.

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

/*********************************************************************
 * LOCAL VARIABLES
 */
    
    static uint16 myReportPeriod = 60000; 
    static gtwData_t gtwData;
    uint8 WSNMonitorApp_TaskID;    
    devStates_t WSNMonitorApp_NwkState;                                                                          
    uint8 WSNMonitorApp_TransID;  
    afAddrType_t WSNMonitorApp_Periodic_DstAddr;
    afAddrType_t WSNMonitorApp_Group_DstAddr;
    afAddrType_t WSNMonitorApp_UnicastData_DstAddr;     
    aps_Group_t WSNMonitorApp_Group;
    uint8 WSNMonitorAppPeriodicCounter = 0;
    uint8 WSNMonitorAppFlashCounter = 0;
    //afAddrType_t WSNMonitorApp_UnicastIEEEAdd_DstAddr; 
/*********************************************************************
 * LOCAL FUNCTIONS
 */
void WSNMonitorApp_HandleKeys( uint8 shift, uint8 keys );
void WSNMonitorApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void WSNMonitorApp_SendPeriodicMessage(void);
void WSNMonitorApp_SendDataToGroup(void);
static uint8 calcFCS(uint8 *pBuf, uint8 len);
static void sendGtwReport(gtwData_t *gtwData);
void sendWSNReport(void);
void initAddAndUnicast(uint16 destAdd,uint8 *unicastData);

/*****************************************
  *Added by Change_Ty for demonstrating UART begin.
  ******************************************/
void  static ProcessUartData(mtOSALSerialData_t * pMsg);  
  /*****************************************
  *Added by Change_Ty for demonstrating UART end.
  ******************************************/

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
  WSNMonitorApp_TaskID = task_id;
  WSNMonitorApp_NwkState = DEV_INIT;
  WSNMonitorApp_TransID = 0;
  
  /*****************************************
  *Added by Change_Ty for demonstrating UART begin.
  ******************************************/
     MT_UartInit();
     MT_UartRegisterTaskID(task_id);
     
  /*****************************************
  *Added by Change_Ty for demonstrating UART end.
  ******************************************/
      
 #if defined ( BUILD_ALL_DEVICES ) //预编译不通过
  // The "Demo" target is setup to have BUILD_ALL_DEVICES and HOLD_AUTO_START
  // We are looking at a jumper (defined in WSNMonitorAppHw.c) to be jumpered
  // together - if they are - we will start up a coordinator. Otherwise,
  // the device will start as a router.
  if ( readCoordinatorJumper() )
    zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
  else
    zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
#endif // BUILD_ALL_DEVICES

#if defined ( HOLD_AUTO_START )//预编译不通过
  ZDOInitDevice(0);
#endif

 // osal_set_event(task_id, ZB_ENTRY_EVENT);
  
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
    //initUart(uartRxCB);
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( WSNMonitorApp_TaskID );
    while ( MSGpkt )  //当有消息传来
    {
      switch ( MSGpkt->hdr.event )
      {
          //串口事件      
          case CMD_SERIAL_MSG:
           ProcessUartData((mtOSALSerialData_t *)MSGpkt);  
           HalLedBlink (HAL_LED_2, 1, 20, 0x1111);
           HalLcdWriteString( "serialMSG", HAL_LCD_LINE_4 );
          break;                        
        // Received when a key is pressed
        case KEY_CHANGE:  //键值改变消息
          WSNMonitorApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        // Received when a messages is received (OTA) for this endpoint
        case AF_INCOMING_MSG_CMD://消息处理
          WSNMonitorApp_MessageMSGCB( MSGpkt );
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

  return 0;
}
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
  //uint8 testComm[8] = {'t','e','s','t','c','o','m','m'};
  (void)shift;  // Intentionally unreferenced parameter
  
  if ( keys & HAL_KEY_SW_1 ) // key up
  {
    //向上键 用来发送组播消息
    WSNMonitorApp_SendDataToGroup();
  }
  if ( keys & HAL_KEY_SW_2 )//keyright
  {
    //向右键用来加入或者离开 group
    aps_Group_t *grp;
    grp = aps_FindGroup( WSNMonitorApp_ENDPOINT, WSNMonitorApp_TEMP_GROUP );
    if ( grp )
    {
      aps_RemoveGroup( WSNMonitorApp_ENDPOINT, WSNMonitorApp_TEMP_GROUP );
      HalLcdWriteString( "RemoveGroup", HAL_LCD_LINE_4 );
    }
    else
    {
      aps_AddGroup( WSNMonitorApp_ENDPOINT, &WSNMonitorApp_Group );
      HalLcdWriteString( "AddGroup", HAL_LCD_LINE_4 );
    }
    
  }
  if ( keys & HAL_KEY_SW_3 ) //keydown
  {      
   // 向下键主要用来测试手动串口发送
   // HalUARTWrite(HAL_UART_PORT_0,testComm, 2);  
  }
   if ( keys & HAL_KEY_SW_4 ) //keyleft
  {   
      //向左键用来测试各种东西
    
  }
   if ( keys & HAL_KEY_SW_5) //keycenter
  {   
    //中间键 暂时不用
  }
   if ( keys & HAL_KEY_SW_6) //大板子的S1
  {   
     sendWSNReport();
  }
   if ( keys & HAL_KEY_SW_7) // 小板子的S2,大板子的S6
  {                        
   // HalLedBlink (HAL_LED_2, 1, 20, 0x1111);
    sendWSNReport();
  }
  if ( keys & HAL_KEY_SW_8)  //S3
  {                        

  }
  if ( keys & HAL_KEY_SW_9) //S1
  {                        

  }
  
}
/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      WSNMonitorApp_MessageMSGCB消息处理函数
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
void WSNMonitorApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{ 
  char temp[16] = "temp:";
  uint16 sourceAddress;
  uint16 parentAddress;
  switch ( pkt->clusterId )
  { 
    case WSNMonitorApp_PERIODIC_CLUSTERID:      
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
      HalLcdWriteString (temp , HAL_LCD_LINE_3);
      HalLcdWriteStringValue( "from:", pkt->srcAddr.addr.shortAddr, 16, HAL_LCD_LINE_4);
      break;
      
     case WSN_PERIOD_REPORT_CMD_ID:   
      zb_ReceiveDataIndication( pkt->srcAddr.addr.shortAddr,  pkt->clusterId, pkt->cmd.DataLength, pkt->cmd.Data );
      break;
  }
}


/*****************************************
  *By Change_Ty 广播函数  begin
  ******************************************/ 
void WSNMonitorApp_SendPeriodicMessage( void )
{ 
  
  //广播都发出自己的短地址，用来说明是谁发出去的广播消息 
  uint16 aaaa = NLME_GetShortAddr(); 
  uint8 buffer[2];
  buffer[0] = LO_UINT16( aaaa );
  buffer[1] = HI_UINT16( aaaa ); 
  
  if ( AF_DataRequest( &WSNMonitorApp_Periodic_DstAddr, &WSNMonitorApp_epDesc,
                       WSNMonitorApp_PERIODIC_CLUSTERID,
                       2, //有效数据长度                           
                       buffer, //指向数据的指针
                       &WSNMonitorApp_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
  else
  {
    // Error occurred in request to send.
  }
}
  /*****************************************
  *By Change_Ty 广播函数 end.
  ******************************************/



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



/******************************************************************************
 * @fn          zb_ReceiveDataIndication
 *
 * @brief       The zb_ReceiveDataIndication callback function is called
 *              asynchronously by the ZigBee stack to notify the application
 *              when data is received from a peer device.
 *
 * @param       source - The short address of the peer device that sent the data
 *              command - The commandId associated with the data
 *              len - The number of bytes in the pData parameter
 *              pData - The data sent by the peer device
 *
 * @return      none
 */
void zb_ReceiveDataIndication( uint16 source, uint16 command, uint16 len, uint8 *pData  )
{ 
  //BUILD_UINT16(loByte, hiByte) 
  //把父节点的地址 封装起来
  gtwData.parent = BUILD_UINT16(pData[SENSOR_PARENT_OFFSET+ 1], pData[SENSOR_PARENT_OFFSET]);
  gtwData.source=source;
  gtwData.temp=*pData;
  gtwData.voltage=*(pData+1);
  gtwData.IEEEAdd0=*(pData+4);
  gtwData.IEEEAdd1=*(pData+5);
  gtwData.IEEEAdd2=*(pData+6);
  gtwData.IEEEAdd3=*(pData+7);
  gtwData.IEEEAdd4=*(pData+8);
  gtwData.IEEEAdd5=*(pData+9);
  gtwData.IEEEAdd6=*(pData+10);
  gtwData.IEEEAdd7=*(pData+11);
  
  // Flash LED 2 once to indicate data reception
  HalLedSet ( HAL_LED_2, HAL_LED_MODE_FLASH );
  
  // Update the display
  #if defined ( LCD_SUPPORTED )
 // HalLcdWriteScreen( "ZjutWSN", "DataRcvd" );
  HalLcdWriteString( "ReportRcvd!", 1 );
  HalLcdWriteStringValue( "ReportFrom:", source, 16, HAL_LCD_LINE_2);
  #endif
  
  // Send gateway report
  sendGtwReport(&gtwData);
}


/******************************************************************************
 * @fn          sendGtwReport
 *
 * @brief       Build and send gateway report
 *
 * @param       none
 *              
 * @return      none
 */
static void sendGtwReport(gtwData_t *gtwData)
{
  uint8 pFrame[ZB_RECV_LENGTH]; 
  // Start of Frame Delimiter
  //区分不同节点的头标志,协调器是COOR_CPT_SOP,终端节点是NODE_CPT_SOP,路由器节点是ROUTER_CPT_SOP
  if(gtwData->temp != 0xFF)
  {
   pFrame[FRAME_SOF_OFFSET] = NODE_CPT_SOP;// Start of Frame Delimiter
  }
  else
  {
    pFrame[FRAME_SOF_OFFSET] = ROUTER_CPT_SOP;
  }
  
  // Length
  pFrame[FRAME_LENGTH_OFFSET] = 18;
  
  // Command type 命令类型  可以自己定义
  pFrame[FRAME_CMD0_OFFSET] = HI_UINT16(ZB_RECEIVE_DATA_INDICATION);   
  pFrame[FRAME_CMD1_OFFSET] = LO_UINT16(ZB_RECEIVE_DATA_INDICATION); 
  
  // Source address
  pFrame[FRAME_DATA_OFFSET+ ZB_RECV_SRC_OFFSET] = HI_UINT16(gtwData->source); 
  pFrame[FRAME_DATA_OFFSET+ ZB_RECV_SRC_OFFSET+ 1] = LO_UINT16(gtwData->source);
  
  // Command ID
  pFrame[FRAME_DATA_OFFSET+ ZB_RECV_CMD_OFFSET] = LO_UINT16(WSN_PERIOD_REPORT_CMD_ID); 
  pFrame[FRAME_DATA_OFFSET+ ZB_RECV_CMD_OFFSET+ 1] = HI_UINT16(WSN_PERIOD_REPORT_CMD_ID);
  
  // Length
  pFrame[FRAME_DATA_OFFSET+ ZB_RECV_LEN_OFFSET] = LO_UINT16(4); 
  pFrame[FRAME_DATA_OFFSET+ ZB_RECV_LEN_OFFSET+ 1] = HI_UINT16(4);
  
  // Data
  pFrame[FRAME_DATA_OFFSET+ ZB_RECV_DATA_OFFSET] = gtwData->temp;
  pFrame[FRAME_DATA_OFFSET+ ZB_RECV_DATA_OFFSET+ 1] = gtwData->voltage; 
  pFrame[FRAME_DATA_OFFSET+ ZB_RECV_DATA_OFFSET+ 2] = HI_UINT16(gtwData->parent); 
  pFrame[FRAME_DATA_OFFSET+ ZB_RECV_DATA_OFFSET+ 3] = LO_UINT16(gtwData->parent);
  pFrame[FRAME_DATA_OFFSET+ ZB_RECV_DATA_OFFSET+ 4] = gtwData->IEEEAdd0;
  pFrame[FRAME_DATA_OFFSET+ ZB_RECV_DATA_OFFSET+ 5] = gtwData->IEEEAdd1;
  pFrame[FRAME_DATA_OFFSET+ ZB_RECV_DATA_OFFSET+ 6] = gtwData->IEEEAdd2;

  pFrame[FRAME_DATA_OFFSET+ ZB_RECV_DATA_OFFSET+ 7] = gtwData->IEEEAdd3;
  pFrame[FRAME_DATA_OFFSET+ ZB_RECV_DATA_OFFSET+ 8] = gtwData->IEEEAdd4;
  pFrame[FRAME_DATA_OFFSET+ ZB_RECV_DATA_OFFSET+ 9] = gtwData->IEEEAdd5;
  pFrame[FRAME_DATA_OFFSET+ ZB_RECV_DATA_OFFSET+ 10] = gtwData->IEEEAdd6;
  pFrame[FRAME_DATA_OFFSET+ ZB_RECV_DATA_OFFSET+ 11] = gtwData->IEEEAdd7;
  
  
  // Frame Check Sequence 
  pFrame[ZB_RECV_LENGTH - 1] = calcFCS(&pFrame[FRAME_LENGTH_OFFSET], (ZB_RECV_LENGTH - 2) );
  
  // Write report to UART
  HalUARTWrite(HAL_UART_PORT_0,pFrame, ZB_RECV_LENGTH);
}
/******************************************************************************
 * @fn          sendWSNReport  
协调器串口初始化以后 发送给服务器的报告  FC 长度 网络地址 IEEE地址
协调器专用(用于服务器 了解网络结构)
 *
 * @brief       Build and send gateway report
 *
 * @param       none
 *              
 * @return      none
 */
void sendWSNReport()
{
  uint8 pData[ZB_WSN_REPORT];
  
  // Start of Frame Delimiter

  pData[FRAME_SOF_OFFSET] = COOR_CPT_SOP;
  
  // Length
  pData[FRAME_LENGTH_OFFSET] = 10;
  
  // 
  pData[FRAME_CMD0_OFFSET] = LO_UINT16(NLME_GetShortAddr());   
  pData[FRAME_CMD1_OFFSET] = HI_UINT16(NLME_GetShortAddr()); 
  
  pData[FRAME_CMD0_OFFSET + 2] = aExtendedAddress[7];
  pData[FRAME_CMD0_OFFSET + 3] = aExtendedAddress[6];
  pData[FRAME_CMD0_OFFSET + 4] = aExtendedAddress[5];
  pData[FRAME_CMD0_OFFSET + 5] = aExtendedAddress[4];
  pData[FRAME_CMD0_OFFSET + 6] = aExtendedAddress[3];
  pData[FRAME_CMD0_OFFSET + 7] = aExtendedAddress[2];
  pData[FRAME_CMD0_OFFSET + 8] = aExtendedAddress[1];
  pData[FRAME_CMD0_OFFSET + 9] = aExtendedAddress[0];
  
  // Frame Check Sequence
  pData[ZB_WSN_REPORT - 1] = calcFCS(&pData[FRAME_LENGTH_OFFSET], (ZB_RECV_LENGTH - 2) );
  
  // Write report to UART
  HalUARTWrite(HAL_UART_PORT_0,pData, ZB_WSN_REPORT);
}
static uint8 calcFCS(uint8 *pBuf, uint8 len)
{
  uint8 rtrn = 0;

  while (len--)
  {
    rtrn ^= *pBuf++;
  }
  return rtrn;
}
    /*****************************************
    *Added by Change_Ty for demonstrating UART begin.
    ******************************************/
void static ProcessUartData(mtOSALSerialData_t * pMsg)
{ 

  uint16 destAddress;
  uint8 *pUartData;
  uint8 *pBuffer;
  uint8 datalength;
  uint8 i;
  uint8 unicastData[2];

  pUartData = pMsg->msg;
  datalength = 3 + *pUartData;  // 3 + （*pUartData） （优先级"*"高于"+",3加上pUartData[0]的那个数）  
  //  datalength = 4 + *pUartData;  
  pBuffer = (uint8 *)osal_mem_alloc(datalength);//定位到有效数据的那位
  if(pBuffer != NULL)
  {
    for(i = 0; i < datalength; i++)
    {
      pBuffer[i] = pUartData[i];

      if(i == 5)// 发送过来的参数里面 将pUartData[5]设给unicastData[0]
      {
        unicastData[0] = pUartData[i];
      }
      if(i == 6)
      {
        unicastData[1] = pUartData[i];
      }
      
    }
     destAddress = BUILD_UINT16(pBuffer[4], pBuffer[3] );  
    
     HalLcdWriteStringValue( "source:", destAddress, 16, HAL_LCD_LINE_3);

     //指令发过去
     initAddAndUnicast(destAddress,unicastData);
     
     HalUARTWrite(HAL_UART_PORT_0,pBuffer,datalength );
    
    osal_mem_free(pBuffer);
  }     
}
    /*****************************************
    *Added by Change_Ty for demonstrating UART end.
    ******************************************/



    /*****************************************
    *Added by Change_Ty for demonstrating UART begin.
    *用于串口回调函数控制发送函数
    *第一个参数是 发送设备的网络短地址
    *第二个参数是  串口发送给他的指令
    ******************************************/
void initAddAndUnicast(uint16 destAdd,uint8 *unicastData)
{
    //显示出目的地址的网络短地址
    HalLcdWriteStringValue( "self:", destAdd, 16, HAL_LCD_LINE_4);

  
    WSNMonitorApp_UnicastData_DstAddr.addrMode = (afAddrMode_t)afAddr16Bit;//点对点的发送 
    WSNMonitorApp_UnicastData_DstAddr.endPoint = WSNMonitorApp_ENDPOINT;
    WSNMonitorApp_UnicastData_DstAddr.addr.shortAddr = destAdd;//destAdd设置为目标设备的网络短地址即可
   
    WSNMonitorApp_epDesc.endPoint = WSNMonitorApp_ENDPOINT;//端点号
    WSNMonitorApp_epDesc.task_id = &WSNMonitorApp_TaskID;
    WSNMonitorApp_epDesc.simpleDesc
    = (SimpleDescriptionFormat_t *)&WSNMonitorApp_SimpleDesc;
    WSNMonitorApp_epDesc.latencyReq = noLatencyReqs;
    
    AF_DataRequest( &WSNMonitorApp_UnicastData_DstAddr, //发送目的地址+端点地址+传送模式
                       &WSNMonitorApp_epDesc, //端点描述符
                       WSN_NODE_CONTROL_CMD_ID ,//簇ID
                       2, //有效数据长度                           
                       unicastData, //第二个参数的数据
                       &WSNMonitorApp_TransID,
                       AF_DISCV_ROUTE, //路由方式选择
                       AF_DEFAULT_RADIUS //路由深度
                         );
}