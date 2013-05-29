#ifndef MT_UART_H
#define MT_UART_H

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************************************
 *                                               INCLUDES
 ***************************************************************************************************/
#include "Onboard.h"
#include "OSAL.h"

/***************************************************************************************************
 *                                             CONSTANTS
 ***************************************************************************************************/

/* Start-of-frame delimiter for UART transport */
#define MT_UART_SOF                     0xFE

/* UART frame overhead for SOF and FCS bytes */
#define MT_UART_FRAME_OVHD              2

/* Default values */
#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
  #define MT_UART_DEFAULT_PORT           ZTOOL_PORT
#elif defined (ZAPP_P1) || defined (ZAPP_P2)
  #define MT_UART_DEFAULT_PORT           ZAPP_PORT
#endif

#if !defined( MT_UART_DEFAULT_OVERFLOW )
  #define MT_UART_DEFAULT_OVERFLOW       FALSE
#endif

#if !defined MT_UART_DEFAULT_BAUDRATE
#define MT_UART_DEFAULT_BAUDRATE         HAL_UART_BR_38400
#endif
#define MT_UART_DEFAULT_THRESHOLD        MT_UART_THRESHOLD
#define MT_UART_DEFAULT_MAX_RX_BUFF      MT_UART_RX_BUFF_MAX
#if !defined( MT_UART_DEFAULT_MAX_TX_BUFF )
  #define MT_UART_DEFAULT_MAX_TX_BUFF    MT_UART_TX_BUFF_MAX
#endif
#define MT_UART_DEFAULT_IDLE_TIMEOUT     MT_UART_IDLE_TIMEOUT

/* Application Flow Control */
#define MT_UART_ZAPP_RX_NOT_READY         0x00
#define MT_UART_ZAPP_RX_READY             0x01

typedef struct
{
  osal_event_hdr_t  hdr;
  uint8             *msg;
} mtOSALSerialData_t;

/*
 * Initialization
 */
extern void MT_UartInit (void);

/*
 * Process ZTool Rx Data
 */
void MT_UartProcessZToolData ( uint8 port, uint8 taskId );

/*
 * Process ZApp Rx Data
 */
void MT_UartProcessZAppData ( uint8 port, uint8 event );

/*
 * Calculate the check sum
 */
extern uint8 MT_UartCalcFCS( uint8 *msg_ptr, uint8 length );

/*
 * Register TaskID for the application
 */
extern void MT_UartRegisterTaskID( uint8 taskID );

/*
 * Register max length that application can take
 */
extern void MT_UartZAppBufferLengthRegister ( uint16 maxLen );

/*
 * Turn Application flow control ON/OFF
 */
extern void MT_UartAppFlowControl ( uint8 status );

/***************************************************************************************************
***************************************************************************************************/

#endif  /* MT_UART_H */
