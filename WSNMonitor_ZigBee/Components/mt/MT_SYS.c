/***************************************************************************************************
  Filename:       MT_SYS.c
  Revised:        $Date: 2009-12-22 17:20:57 -0800 (Tue, 22 Dec 2009) $
  Revision:       $Revision: 21402 $

  Description:   MonitorTest

  Copyright 2007-2009 Texas Instruments Incorporated. All rights reserved.

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

 ***************************************************************************************************/

/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include "ZComDef.h"
#include "MT.h"
#include "MT_SYS.h"
#include "MT_VERSION.h"
#include "nwk_util.h"
#include "OSAL.h"
#include "OSAL_NV.h"
#include "Onboard.h"  /* This is here because RAM read/write macros need it */
#include "hal_adc.h"
#include "ZGlobals.h"

/***************************************************************************************************
 * MACROS
 ***************************************************************************************************/

#define MT_SYS_DEVICE_INFO_RESPONSE_LEN 14
#define MT_NV_ITEM_MAX_LENGTH           250

#if !defined HAL_GPIO || !HAL_GPIO
#define GPIO_DIR_IN(IDX)
#define GPIO_DIR_OUT(IDX)
#define GPIO_TRI(IDX)
#define GPIO_PULL_UP(IDX)
#define GPIO_PULL_DN(IDX)
#define GPIO_SET(IDX)
#define GPIO_CLR(IDX)
#define GPIO_TOG(IDX)
#define GPIO_GET(IDX) 0
#define GPIO_HiD_SET() (val = 0)
#define GPIO_HiD_CLR() (val = 0)
#endif

/***************************************************************************************************
 * CONSTANT
 ***************************************************************************************************/
const uint16 MT_SysOsalEventId [] = {
                                      MT_SYS_OSAL_EVENT_0,
                                      MT_SYS_OSAL_EVENT_1,
                                      MT_SYS_OSAL_EVENT_2,
                                      MT_SYS_OSAL_EVENT_3
                                    };

typedef enum {
  GPIO_DIR,
  GPIO_TRI,
  GPIO_SET,
  GPIO_CLR,
  GPIO_TOG,
  GPIO_GET,
  GPIO_HiD = 0x12
} GPIO_Op_t;

#define GPIO_MASK  0x33

/***************************************************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************************************************/
extern uint16 HalAdcReadVoltage(void);
extern uint16 HalAdcReadTemperature(void);

/***************************************************************************************************
 * LOCAL FUNCTIONS
 ***************************************************************************************************/
#if defined (MT_SYS_FUNC)
void MT_SysReset(uint8 *pBuf);
void MT_SysPing(void);
void MT_SysVersion(void);
void MT_SysSetExtAddr(uint8 *pBuf);
void MT_SysGetExtAddr(void);
void MT_SysOsalNVWrite(uint8 *pBuf);
void MT_SysOsalNVRead(uint8 *pBuf);
void MT_SysOsalStartTimer(uint8 *pBuf);
void MT_SysOsalStopTimer(uint8 *pBuf);
void MT_SysRandom(void);
void MT_SysAdcRead(uint8 *pBuf);
void MT_SysGpio(uint8 *pBuf);
void MT_SysStackTune(uint8 *pBuf);
void MT_SysGetDeviceInfo(uint8 *pBuf);
#endif /* MT_SYS_FUNC */

#if defined (MT_SYS_FUNC)
/***************************************************************************************************
 * @fn      MT_SysProcessing
 *
 * @brief   Process all the SYS commands that are issued by test tool
 *
 * @param   pBuf - pointer to the msg buffer
 *
 *          | LEN  | CMD0  | CMD1  |  DATA  |
 *          |  1   |   1   |   1   |  0-255 |
 *
 * @return  status
 ***************************************************************************************************/
uint8 MT_SysCommandProcessing(uint8 *pBuf)
{
  uint8 status = MT_RPC_SUCCESS;

  switch (pBuf[MT_RPC_POS_CMD1])
  {
    case MT_SYS_RESET_REQ:
      MT_SysReset(pBuf);
      break;

    case MT_SYS_PING:
      MT_SysPing();
      break;

    case MT_SYS_VERSION:
      MT_SysVersion();
      break;

    case MT_SYS_SET_EXTADDR:
      MT_SysSetExtAddr(pBuf);
      break;

    case MT_SYS_GET_EXTADDR:
      MT_SysGetExtAddr();
      break;

    case MT_SYS_OSAL_NV_READ:
      MT_SysOsalNVRead(pBuf);
      break;

    case MT_SYS_OSAL_NV_WRITE:
      MT_SysOsalNVWrite(pBuf);
      break;

    case MT_SYS_OSAL_START_TIMER:
      MT_SysOsalStartTimer(pBuf);
      break;

    case MT_SYS_OSAL_STOP_TIMER:
      MT_SysOsalStopTimer(pBuf);
      break;

    case MT_SYS_RANDOM:
      MT_SysRandom();
      break;

    case MT_SYS_ADC_READ:
      MT_SysAdcRead(pBuf);
      break;

    case MT_SYS_GPIO:
      MT_SysGpio(pBuf);
      break;

    case MT_SYS_STACK_TUNE:
      MT_SysStackTune(pBuf);
      break;

    case MT_SYS_RESET_IND:
      //TBD
      break;

    default:
      status = MT_RPC_ERR_COMMAND_ID;
      break;
  }

  return status;
}

/***************************************************************************************************
 * @fn      MT_SysReset
 *
 * @brief   Reset/reprogram the device.
 * @param   typID: 0=reset, 1=serial bootloader
 *
 * @return  None
 ***************************************************************************************************/
void MT_SysReset(uint8 *pBuf)
{
  (void)pBuf;  // Intentionally unreferenced parameter
  SystemReset();  /* Restart this program */
}

/***************************************************************************************************
 * @fn      MT_SysPing
 *
 * @brief   Process the Ping command
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************************************/
void MT_SysPing(void)
{
  uint16 tmp16;
  uint8 retArray[2];

  /* Build Capabilities */
  tmp16 = MT_CAP_SYS | MT_CAP_MAC | MT_CAP_NWK | MT_CAP_AF | MT_CAP_ZDO|
          MT_CAP_SAPI | MT_CAP_UTIL | MT_CAP_DEBUG | MT_CAP_APP | MT_CAP_ZOAD;

  /* Convert to high byte first into temp buffer */
  retArray[0] = LO_UINT16( tmp16 );
  retArray[1] = HI_UINT16( tmp16 );

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_SYS), MT_SYS_PING,
                                sizeof (tmp16), retArray );
}

/***************************************************************************************************
 * @fn      MT_SysVersion
 *
 * @brief   Process the Version command
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************************************/
void MT_SysVersion(void)
{
  byte *verStr = (byte *)MTVersionString;
  uint8 respLen = sizeof(MTVersionString);

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_SYS), MT_SYS_VERSION,
                               respLen, verStr);
}

/***************************************************************************************************
 * @fn      MT_SysSetExtAddr
 *
 * @brief   Set the Extended Address
 *
 * @param   pBuf
 *
 * @return  None
 ***************************************************************************************************/
void MT_SysSetExtAddr(uint8 *pBuf)
{
  uint8 retValue = ZFailure;
  uint8 cmdId;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  if ( ZMacSetReq(ZMacExtAddr, pBuf) == ZMacSuccess )
  {
    retValue = osal_nv_write(ZCD_NV_EXTADDR, 0, Z_EXTADDR_LEN, pBuf);
  }

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_SYS), cmdId, 1, &retValue);

}

/***************************************************************************************************
 * @fn      MT_SysGetExtAddr
 *
 * @brief   Get the Extended Address
 *
 * @param   None
 *
 * @return  None
 ***************************************************************************************************/
void MT_SysGetExtAddr(void)
{
  uint8 extAddr[Z_EXTADDR_LEN];

  ZMacGetReq( ZMacExtAddr, extAddr );

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_SYS), MT_SYS_GET_EXTADDR,
                               Z_EXTADDR_LEN, extAddr);
}

/***************************************************************************************************
 * @fn      MT_SysOsalNVRead
 *
 * @brief  Read a NV value
 *
 * @param  uint8 pBuf - pointer to the data
 *
 * @return  None
 ***************************************************************************************************/
void MT_SysOsalNVRead(uint8 *pBuf)
{
  uint16 nvId;
  uint8 nvItemLen=0, nvItemOffset=0;
  uint8 *pRetBuf=NULL;
  uint8 respLen, cmdId;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Get the ID */
  nvId = BUILD_UINT16(pBuf[0], pBuf[1]);
  pBuf += sizeof(uint16);

  /* Calculate the offset */
  nvItemOffset = *pBuf;

  nvItemLen = osal_nv_item_len(nvId);

  /* Return only 250 bytes max */
  if (nvItemLen > MT_NV_ITEM_MAX_LENGTH)
  {
    nvItemLen = MT_NV_ITEM_MAX_LENGTH;
  }

  if ((nvItemLen > 0) && ((nvItemLen - nvItemOffset) > 0))
  {
    respLen = nvItemLen - nvItemOffset + 2;
  }
  else
  {
    respLen = 2;
  }

  pRetBuf = osal_mem_alloc(respLen);

  if (pRetBuf != NULL)
  {
    osal_memset(pRetBuf, 0, respLen);

    /* Default to ZFailure */
    pRetBuf[0] = ZFailure;

    if (respLen > 2)
    {
      if (((osal_nv_read( nvId, (uint16)nvItemOffset, (uint16)nvItemLen, &pRetBuf[2])) == ZSUCCESS) && (respLen > 2))
      {
        pRetBuf[0] = ZSuccess;
      }
      pRetBuf[1] = nvItemLen - nvItemOffset;
    }
    else
    {
      pRetBuf[1] = 0;
    }

    /* Build and send back the response */
    MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_SYS), cmdId, respLen, pRetBuf );

    osal_mem_free(pRetBuf);
  }


}

/***************************************************************************************************
 * @fn      MT_SysOsalNVWrite
 *
 * @brief
 *
 * @param   uint8 pData - pointer to the data
 *
 * @return  None
 ***************************************************************************************************/
void MT_SysOsalNVWrite(uint8 *pBuf)
{
  uint16 nvId;
  uint8 nvItemLen=0, nvItemOffset=0;
  uint8 rtrn, cmd;

  /* parse header */
  cmd = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Get the ID */
  nvId = BUILD_UINT16(pBuf[0], pBuf[1]);
  pBuf += sizeof(uint16);

  /* Calculate the offset */
  nvItemOffset = *pBuf++;

  /* Calculate the length */
  nvItemLen = *pBuf++;

  /* Default to ZFailure */
  rtrn = ZFailure;

  /* Set the Z-Globals value of this NV item. */
  zgSetItem( nvId, (uint16)nvItemLen, pBuf );

  if ((osal_nv_write(nvId, (uint16)nvItemOffset, (uint16)nvItemLen, pBuf)) == ZSUCCESS)
  {
    if (nvId == ZCD_NV_EXTADDR)
    {
      rtrn = ZMacSetReq(ZMacExtAddr, pBuf);
    }
    else
    {
      rtrn = ZSuccess;
    }
  }

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_SYS), cmd, 1, &rtrn);
}

/***************************************************************************************************
 * @fn      MT_SysOsalStartTimer
 *
 * @brief
 *
 * @param   uint8 pData - pointer to the data
 *
 * @return  None
 ***************************************************************************************************/
void MT_SysOsalStartTimer(uint8 *pBuf)
{
  uint16 eventId;
  uint8 retValue = ZFailure;
  uint8 cmdId;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  if (*pBuf <= 3)
  {
    eventId = (uint16) MT_SysOsalEventId[*pBuf];
    retValue = osal_start_timerEx(MT_TaskID, eventId, BUILD_UINT16(pBuf[1], pBuf[2]));
  }
  else
  {
    retValue = ZInvalidParameter;
  }

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_SYS), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_SysOsalStopTimer
 *
 * @brief
 *
 * @param   uint8 pData - pointer to the data
 *
 * @return  None
 ***************************************************************************************************/
void MT_SysOsalStopTimer(uint8 *pBuf)
{
  uint16 eventId;
  uint8 retValue = ZFailure;
  uint8 cmdId;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  if (*pBuf <= 3)
  {
    eventId = (uint16) MT_SysOsalEventId[*pBuf];
    retValue = osal_stop_timerEx(MT_TaskID, eventId);
  }
  else
  {
    retValue = ZInvalidParameter;
  }

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_SYS), cmdId, 1, &retValue );
}

/***************************************************************************************************
 * @fn      MT_SysRandom
 *
 * @brief
 *
 * @param   uint8 pData - pointer to the data
 *
 * @return  None
 ***************************************************************************************************/
void MT_SysRandom()
{
  uint16 randValue = Onboard_rand();
  uint8 retArray[2];

  retArray[0] = LO_UINT16(randValue);
  retArray[1] = HI_UINT16(randValue);

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_SYS), MT_SYS_RANDOM, 2, retArray );
}

/***************************************************************************************************
 * @fn      MT_SysAdcRead
 *
 * @brief   Reading ADC value, temperature sensor and voltage
 *
 * @param   uint8 pData - pointer to the data
 *
 * @return  None
 ***************************************************************************************************/
void MT_SysAdcRead(uint8 *pBuf)
{
  uint8 channel, resolution;
  uint16 tempValue;
  uint8 retArray[2];
  uint8 cmdId;

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Channel */
  channel = *pBuf++;

  /* Resolution */
  resolution = *pBuf++;

  /* Voltage reading */
  switch (channel)
  {
    /* Analog input channel */
    case HAL_ADC_CHANNEL_0:
    case HAL_ADC_CHANNEL_1:
    case HAL_ADC_CHANNEL_2:
    case HAL_ADC_CHANNEL_3:
    case HAL_ADC_CHANNEL_4:
    case HAL_ADC_CHANNEL_5:
    case HAL_ADC_CHANNEL_6:
    case HAL_ADC_CHANNEL_7:
      tempValue = HalAdcRead(channel, resolution);
      break;

    /* Temperature sensor */
    case(HAL_ADC_CHANNEL_TEMP):
      tempValue = HalAdcRead(HAL_ADC_CHANNEL_TEMP, HAL_ADC_RESOLUTION_14);
      break;

    /* Voltage reading */
    case(HAL_ADC_CHANNEL_VDD):
      tempValue = HalAdcRead(HAL_ADC_CHANNEL_VDD, HAL_ADC_RESOLUTION_14);
      break;

    /* Undefined channels */
    default:
      tempValue = 0x00;
      break;
  }

  retArray[0] = LO_UINT16(tempValue);
  retArray[1] = HI_UINT16(tempValue);

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_SYS), cmdId, 2, retArray);
}

/**************************************************************************************************
 * @fn      MT_SysGpio
 *
 * @brief   ZAccel RPC interface for controlling the available GPIO pins.
 *
 * @param   uint8 pData - Pointer to the data.
 *
 * @return  None
 *************************************************************************************************/
void MT_SysGpio(uint8 *pBuf)
{
  uint8 cmd, val;
  GPIO_Op_t op;

  cmd = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  op = (GPIO_Op_t)(*pBuf++);
  val = *pBuf & GPIO_MASK;

  switch (op)
  {
    case GPIO_DIR:
      if (val & BV(0)) {GPIO_DIR_OUT(0);} else {GPIO_DIR_IN(0);}
      if (val & BV(1)) {GPIO_DIR_OUT(1);} else {GPIO_DIR_IN(1);}
      if (val & BV(2)) {GPIO_DIR_OUT(2);} else {GPIO_DIR_IN(2);}
      if (val & BV(3)) {GPIO_DIR_OUT(3);} else {GPIO_DIR_IN(3);}
      break;

    case GPIO_TRI:
      if(val & BV(0)) {GPIO_TRI(0);} else if(val & BV(4)) {GPIO_PULL_DN(0);} else {GPIO_PULL_UP(0);}
      if(val & BV(1)) {GPIO_TRI(1);} else if(val & BV(5)) {GPIO_PULL_DN(1);} else {GPIO_PULL_UP(1);}
      if(val & BV(2)) {GPIO_TRI(2);} else if(val & BV(6)) {GPIO_PULL_DN(2);} else {GPIO_PULL_UP(2);}
      if(val & BV(3)) {GPIO_TRI(3);} else if(val & BV(7)) {GPIO_PULL_DN(3);} else {GPIO_PULL_UP(3);}
      break;

    case GPIO_SET:
      if (val & BV(0)) {GPIO_SET(0);}
      if (val & BV(1)) {GPIO_SET(1);}
      if (val & BV(2)) {GPIO_SET(2);}
      if (val & BV(3)) {GPIO_SET(3);}
      break;

    case GPIO_CLR:
      if (val & BV(0)) {GPIO_CLR(0);}
      if (val & BV(1)) {GPIO_CLR(1);}
      if (val & BV(2)) {GPIO_CLR(2);}
      if (val & BV(3)) {GPIO_CLR(3);}
      break;

    case GPIO_TOG:
      if (val & BV(0)) {GPIO_TOG(0);}
      if (val & BV(1)) {GPIO_TOG(1);}
      if (val & BV(2)) {GPIO_TOG(2);}
      if (val & BV(3)) {GPIO_TOG(3);}
      break;

    case GPIO_GET:
      break;

    case GPIO_HiD:
      (val) ? GPIO_HiD_SET() :  GPIO_HiD_CLR();
      break;

    default:
      break;
  }

  val  = (GPIO_GET(0)) ? BV(0) : 0;
  val |= (GPIO_GET(1)) ? BV(1) : 0;
  val |= (GPIO_GET(2)) ? BV(2) : 0;
  val |= (GPIO_GET(3)) ? BV(3) : 0;

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_SYS), cmd, 1, &val);
}

/**************************************************************************************************
 * @fn      MT_SysStackTune
 *
 * @brief   ZAccel RPC interface for tuning the stack parameters to adjust performance
 *
 * @param   uint8 pData - Pointer to the data.
 *
 * @return  None
 *************************************************************************************************/
void MT_SysStackTune(uint8 *pBuf)
{
  uint8 cmd, rtrn;

  cmd = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  switch (*pBuf++)
  {
  case STK_TX_PWR:
    rtrn = ZMacSetReq(ZMacPhyTransmitPower, pBuf);
    break;

  case STK_RX_ON_IDLE:
    if ((*pBuf != TRUE) && (*pBuf != FALSE))
    {
      (void)ZMacGetReq(ZMacRxOnIdle, &rtrn);
    }
    else
    {
      rtrn = ZMacSetReq(ZMacRxOnIdle, pBuf);
    }
    break;

  default:
    rtrn = ZInvalidParameter;
    break;
  }

  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_SYS), cmd, 1, &rtrn);
}
#endif /* MT_SYS_FUNC */

/***************************************************************************************************
 * SUPPORT
 ***************************************************************************************************/

/***************************************************************************************************
 * @fn      MT_SysResetInd()
 *
 * @brief   Sends a ZTOOL "reset response" message.
 *
 * @param   None
 *
 * @return  None
 *
 ***************************************************************************************************/
void MT_SysResetInd(void)
{
  uint8 retArray[6];

  retArray[0] = ResetReason();   /* Reason */
  osal_memcpy( &retArray[1], MTVersionString, 5 );   /* Revision info */

  /* Send out Reset Response message */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_SYS), MT_SYS_RESET_IND,
                                sizeof(retArray), retArray);
}

/***************************************************************************************************
 * @fn      MT_SysOsalTimerExpired()
 *
 * @brief   Sends a SYS Osal Timer Expired
 *
 * @param   None
 *
 * @return  None
 *
 ***************************************************************************************************/
void MT_SysOsalTimerExpired(uint8 Id)
{
  uint8 retValue;
  retValue = Id;
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_AREQ | (uint8)MT_RPC_SYS_SYS), MT_SYS_OSAL_TIMER_EXPIRED, 1, &retValue);
}

/***************************************************************************************************
 ***************************************************************************************************/
