#include "ZComDef.h"
#include "MT_TASK.h"
#include "MT.h"
#include "MT_DEBUG.h"
#include "MT_UART.h"
#include "MT_UTIL.h"
#include "MT_SYS.h"
#include "MT_ZDO.h"

#include "hal_uart.h"

#include "OSAL_Memory.h"

/***************************************************************************************************
 * LOCAL FUNCTIONS
 ***************************************************************************************************/
void MT_ProcessIncomingCommand( mtOSALSerialData_t *msg );

/***************************************************************************************************
 * GLOBALS
 ***************************************************************************************************/

/***************************************************************************************************
 * @fn      MT_TaskInit
 *
 * @brief  MonitorTest Task Initialization.  This function is put into the
 *         task table.
 *
 * @param   byte task_id - task ID of the MT Task
 *
 * @return  void
 ***************************************************************************************************/
void MT_TaskInit(uint8 task_id)
{
  /* Initialize the Serial port */
  MT_UartInit();

  /* Register taskID - Do this after UartInit() because it will reset the taskID */
  MT_UartRegisterTaskID(task_id);

  /* Initialize MT */
  MT_Init(task_id);
}

/***************************************************************************************************
 * @fn      MT_ProcessEvent
 *
 * @brief MonitorTest Task Event Processor.  This task is put into the task table.
 *
 * @param   byte task_id - task ID of the MT Task
 * @param   UINT16 events - event(s) for the MT Task
 *
 * @return  void
 ***************************************************************************************************/
UINT16 MT_ProcessEvent(uint8 task_id, uint16 events)
{
  uint8 *msg_ptr;
  
  (void)task_id;  // Intentionally unreferenced parameter

  /* Could be multiple events, so switch won't work */
  if ( events & SYS_EVENT_MSG )
  {
    while ( (msg_ptr = osal_msg_receive( MT_TaskID )) )
    {
      MT_ProcessIncomingCommand((mtOSALSerialData_t *)msg_ptr);
    }

    /* Return unproccessed events */
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & MT_ZTOOL_SERIAL_RCV_BUFFER_FULL )
  {
    /* Return unproccessed events */
    return (events ^ MT_ZTOOL_SERIAL_RCV_BUFFER_FULL);
  }

  /* Handle MT_SYS_OSAL_START_TIMER callbacks */
#if defined MT_SYS_FUNC
  if ( events & (MT_SYS_OSAL_EVENT_MASK))
  {
    if (events & MT_SYS_OSAL_EVENT_0)
    {
      MT_SysOsalTimerExpired(0x00);
      events ^= MT_SYS_OSAL_EVENT_0;
    }

    if (events & MT_SYS_OSAL_EVENT_1)
    {
      MT_SysOsalTimerExpired(0x01);
      events ^= MT_SYS_OSAL_EVENT_1;
    }

    if (events & MT_SYS_OSAL_EVENT_2)
    {
      MT_SysOsalTimerExpired(0x02);
      events ^= MT_SYS_OSAL_EVENT_2;
    }

    if (events & MT_SYS_OSAL_EVENT_3)
    {
      MT_SysOsalTimerExpired(0x03);
      events ^= MT_SYS_OSAL_EVENT_3;
    }

    return events;
  }
#endif

  /* Discard or make more handlers */
  return 0;

} /* MT_ProcessEvent() */

/***************************************************************************************************
 * @fn      MT_ProcessIncomingCommand
 *
 * @brief
 *
 *   Process Event Messages.
 *
 * @param   byte *msg - pointer to event message
 *
 * @return
 ***************************************************************************************************/
void MT_ProcessIncomingCommand( mtOSALSerialData_t *msg )
{
  byte deallocate;
  byte *msg_ptr;
  byte len;

  /* A little setup for AF, CB_FUNC and MT_SYS_APP_RSP_MSG */
  msg_ptr = msg->msg;

  deallocate = true;

  /* Use the first byte of the message as the command ID */
  switch ( msg->hdr.event )
  {
    case CMD_SERIAL_MSG:
      MT_ProcessIncoming(msg->msg);
      break;

    case CMD_DEBUG_MSG:
      MT_ProcessDebugMsg( (mtDebugMsg_t *)msg );
      break;

    case CB_FUNC:
      /*
        Build SPI message here instead of redundantly calling MT_BuildSPIMsg
        because we have copied data already in the allocated message
      */

      /* msg_ptr is the beginning of the intended SPI message */
      len = SPI_0DATA_MSG_LEN + msg_ptr[DATALEN_FIELD];

      /*
        FCS goes to the last byte in the message and is calculated over all
        the bytes except FCS and SOP
      */
      msg_ptr[len-1] = MT_UartCalcFCS(msg_ptr + 1, (byte)(len-2));

#ifdef MT_UART_DEFAULT_PORT
     // HalUARTWrite ( MT_UART_DEFAULT_PORT, msg_ptr, len );
#endif
      break;

    case CMD_DEBUG_STR:
      MT_ProcessDebugStr( (mtDebugStr_t *)msg );
      break;

#if !defined ( NONWK )
    case MT_SYS_APP_RSP_MSG:
      len = SPI_0DATA_MSG_LEN + msg_ptr[DATALEN_FIELD];
      MTProcessAppRspMsg( msg_ptr, len );
      break;
#endif  // NONWK

#if defined (MT_UTIL_FUNC)
#if defined ZCL_KEY_ESTABLISH
    case ZCL_KEY_ESTABLISH_IND:
      MT_UtilKeyEstablishInd((keyEstablishmentInd_t *)msg);
      break;
#endif        
#endif        
#ifdef MT_ZDO_CB_FUNC
    case ZDO_STATE_CHANGE:
      MT_ZdoStateChangeCB((osal_event_hdr_t *)msg);
      break;
#endif

    default:
      break;
  }

  if ( deallocate )
  {
    osal_msg_deallocate( (uint8 *)msg );
  }
}

#ifdef MT_TASK
/***************************************************************************************************
 * @fn      MT_TransportAlloc
 *
 * @brief   Allocate memory for transport msg
 *
 * @param   uint8 cmd0 - The first byte of the MT command id containing the command type and subsystem.
 *          uint8 len - length
 *
 * @return  pointer the allocated memory or NULL if fail to allocate the memory
 ***************************************************************************************************/
uint8 *MT_TransportAlloc(uint8 cmd0, uint8 len)
{
  uint8 *p;

  (void)cmd0;  // Intentionally unreferenced parameter
  
  /* Allocate a buffer of data length + SOP+CMD+FCS (5bytes) */
  p = osal_msg_allocate(len + SPI_0DATA_MSG_LEN);

  if (p)
  {
    p++; /* Save space for SOP_VALUE, msg structure */
    return p;
  }
  else
  {
    return NULL;
  }
}

/***************************************************************************************************
 * @fn      MT_TransportSend
 *
 * @brief   Fill in SOP and FCS then send out the msg
 *
 * @param   uint8 *pBuf - pointer to the message that contains CMD, length, data and FCS
 *
 * @return  None
 ***************************************************************************************************/
void MT_TransportSend(uint8 *pBuf)
{
  uint8 *msgPtr;
  uint8 dataLen = pBuf[0]; /* Data length is on byte #1 from the pointer */

  /* Move back to the SOP */
  msgPtr = pBuf-1;

  /* Insert SOP */
  msgPtr[0] = MT_UART_SOF;

  /* Insert FCS */
  msgPtr[SPI_0DATA_MSG_LEN - 1 + dataLen] = MT_UartCalcFCS (pBuf, (3 + dataLen));

  /* Send to UART */
#ifdef MT_UART_DEFAULT_PORT
  //HalUARTWrite(MT_UART_DEFAULT_PORT, msgPtr, dataLen + SPI_0DATA_MSG_LEN);
#endif

  /* Deallocate */
  osal_msg_deallocate(msgPtr);
}
#endif /* MT_TASK */
/***************************************************************************************************
 ***************************************************************************************************/
