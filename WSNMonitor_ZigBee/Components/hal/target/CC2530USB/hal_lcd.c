/**************************************************************************************************
  Filename:       hal_lcd.c
  Revised:        $Date: 2010-8-31$
  Author:         $outman from feibit.com$

  Description:    This file contains the interface to the HAL LCD Service. 

  IMPORTANT:   THE ORIGINAL CODE IS FROM TI, AND THE FONTS LIB AND 
                      RELATED CODE IS WRITTEN BY WWW.FEIBIT.COM


  Copyright 2007 - 2009 Texas Instruments Incorporated. All rights reserved.

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
  PROVIDED 揂S IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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

/**************************************************************************************************
 *                                           INCLUDES
 **************************************************************************************************/
#include "hal_types.h"
#include "hal_lcd.h"
#include "OSAL.h"
#include "OnBoard.h"
#include "hal_assert.h"

#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
  #include "DebugTrace.h"
#endif

/**************************************************************************************************
 *                                          CONSTANTS
 **************************************************************************************************/
/*
  LCD pins

  //control
  P0.0 - LCD_MODE
  P1.1 - LCD_FLASH_RESET
  P1.2 - LCD_CS

  //spi
  P1.5 - CLK
  P1.6 - MOSI
  P1.7 - MISO
*/

/* 30/8/2010 ++ by feibit */

#define BIT0              0x01
#define BIT1              0x02
#define BIT2              0x04
#define BIT3              0x08
#define BIT4              0x10
#define BIT5              0x20
#define BIT6              0x40
#define BIT7              0x80

/* The ASCII fonts library, the matrix size is w*h = 8*12, created by outman from feibit.com */
__code const uint8 ASCIITAB_DZK[]=
{
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*""=00H*/ 
0x00,0x30,0x78,0x78,0x78,0x30,0x30,0x00,0x30,0x30,0x00,0x00,/*"!"=01H*/
0x00,0x00,0x66,0x66,0x66,0x24,0x00,0x00,0x00,0x00,0x00,0x00,/*"""=02h*/
0x00,0x00,0x6c,0x6c,0xfe,0x6c,0x6c,0x6c,0xfe,0x6c,0x6c,0x00,/*"#"=03h*/
0x00,0x30,0x30,0x7c,0xc0,0xc0,0x78,0x0c,0x0c,0xf8,0x30,0x30,/*"$"=04h*/
0x00,0x00,0x00,0x00,0xc4,0xcc,0x18,0x30,0x60,0xcc,0x8c,0x00,/*"%"=05h*/
0x00,0x00,0x70,0xd8,0xd8,0x70,0xfa,0xde,0xcc,0xdc,0x76,0x00,/*"&"=06h*/
0x00,0x00,0x30,0x30,0x30,0x60,0x00,0x00,0x00,0x00,0x00,0x00,/*"'"=07h*/
0x00,0x00,0x0c,0x18,0x30,0x60,0x60,0x60,0x30,0x18,0x0c,0x00,/*"("=08h*/
0x00,0x00,0x60,0x30,0x18,0x0c,0x0c,0x0c,0x18,0x30,0x60,0x00,/*")"=09h*/
0x00,0x00,0x00,0x00,0x66,0x3c,0xff,0x3c,0x66,0x00,0x00,0x00,/*"*"=0Ah*/
0x00,0x00,0x00,0x00,0x18,0x18,0x7e,0x18,0x18,0x00,0x00,0x00,/*"+"=0Bh*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x38,0x60,/*";"=0Ch*/
0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0x00,0x00,0x00,0x00,0x00,/*"-"=0Dh*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x38,0x00,/*"."=0Eh*/
0x00,0x00,0x00,0x02,0x06,0x0c,0x18,0x30,0x60,0xc0,0x80,0x00,/*"/"=0Fh*/
0x00,0x00,0x7c,0xc6,0xce,0xde,0xd6,0xf6,0xe6,0xc6,0x7c,0x00,/*"0"=10h*/
0x00,0x00,0x10,0x30,0xf0,0x30,0x30,0x30,0x30,0x30,0xfc,0x00,/*"1"=11h*/
0x00,0x00,0x78,0xcc,0xcc,0x0c,0x18,0x30,0x60,0xcc,0xfc,0x00,/*"2"=12h*/
0x00,0x00,0x78,0xcc,0x0c,0x0c,0x38,0x0c,0x0c,0xcc,0x78,0x00,/*"3"=13h*/
0x00,0x00,0x0c,0x1c,0x3c,0x6c,0xcc,0xfe,0x0c,0x0c,0x1e,0x00,/*"4"=14h*/
0x00,0x00,0xfc,0xc0,0xc0,0xc0,0xf8,0x0c,0x0c,0xcc,0x78,0x00,/*"5"=15h*/
0x00,0x00,0x38,0x60,0xc0,0xc0,0xf8,0xcc,0xcc,0xcc,0x78,0x00,/*"6"=16h*/
0x00,0x00,0xfe,0xc6,0xc6,0x06,0x0c,0x18,0x30,0x30,0x30,0x00,/*"7"=17h*/
0x00,0x00,0x78,0xcc,0xcc,0xec,0x78,0xdc,0xcc,0xcc,0x78,0x00,/*"8"=18h*/
0x00,0x00,0x78,0xcc,0xcc,0xcc,0x7c,0x18,0x18,0x30,0x70,0x00,/*"9"=19h*/
0x00,0x00,0x00,0x00,0x38,0x38,0x00,0x00,0x38,0x38,0x00,0x00,/*":"=1Ah*/
0x00,0x00,0x00,0x00,0x38,0x38,0x00,0x00,0x38,0x38,0x18,0x30,/*";"=1Bh*/
0x00,0x00,0x0c,0x18,0x30,0x60,0xc0,0x60,0x30,0x18,0x0c,0x00,/*"<"=1Ch*/
0x00,0x00,0x00,0x00,0x00,0x7e,0x00,0x7e,0x00,0x00,0x00,0x00,/*"="=1Dh*/
0x00,0x00,0x60,0x30,0x18,0x0c,0x06,0x0c,0x18,0x30,0x60,0x00,/*">"=1Eh*/
0x00,0x00,0x78,0xcc,0x0c,0x18,0x30,0x30,0x00,0x30,0x30,0x00,/*"?"=1Fh*/
0x00,0x00,0x7c,0xc6,0xc6,0xde,0xde,0xde,0xc0,0xc0,0x7c,0x00,/*"@"=20h*/
0x00,0x00,0x30,0x78,0xcc,0xcc,0xcc,0xfc,0xcc,0xcc,0xcc,0x00,/*"A"=21h*/
0x00,0x00,0xfc,0x66,0x66,0x66,0x7c,0x66,0x66,0x66,0xfc,0x00,/*"B"=22h*/
0x00,0x00,0x3c,0x66,0xc6,0xc0,0xc0,0xc0,0xc6,0x66,0x3c,0x00,/*"C"=23h*/
0x00,0x00,0xf8,0x6c,0x66,0x66,0x66,0x66,0x66,0x6c,0xf8,0x00,/*"D"=24h*/
0x00,0x00,0xfe,0x62,0x60,0x64,0x7c,0x64,0x60,0x62,0xfe,0x00,/*"E"=25h*/
0x00,0x00,0xfe,0x66,0x62,0x64,0x7c,0x64,0x60,0x60,0xf0,0x00,/*"F"=26h*/
0x00,0x00,0x3c,0x66,0xc6,0xc0,0xc0,0xce,0xc6,0x66,0x3e,0x00,/*"G"=27h*/
0x00,0x00,0xcc,0xcc,0xcc,0xcc,0xfc,0xcc,0xcc,0xcc,0xcc,0x00,/*"h"=28h*/
0x00,0x00,0x78,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x78,0x00,/*"I"=29h*/
0x00,0x00,0x1e,0x0c,0x0c,0x0c,0x0c,0xcc,0xcc,0xcc,0x78,0x00,/*"J"=2Ah*/
0x00,0x00,0xe6,0x66,0x6c,0x6c,0x78,0x6c,0x6c,0x66,0xe6,0x00,/*"K"=2Bh*/
0x00,0x00,0xf0,0x60,0x60,0x60,0x60,0x62,0x66,0x66,0xfe,0x00,/*"L"=2Ch*/
0x00,0x00,0xc6,0xee,0xfe,0xfe,0xd6,0xc6,0xc6,0xc6,0xc6,0x00,/*"M"=2Dh*/
0x00,0x00,0xc6,0xc6,0xe6,0xf6,0xfe,0xde,0xce,0xc6,0xc6,0x00,/*"N"=2Eh*/
0x00,0x00,0x38,0x6c,0xc6,0xc6,0xc6,0xc6,0xc6,0x6c,0x38,0x00,/*"O"=2Fh*/
0x00,0x00,0xfc,0x66,0x66,0x66,0x7c,0x60,0x60,0x60,0xf0,0x00,/*"P"=30h*/
0x00,0x00,0x38,0x6c,0xc6,0xc6,0xc6,0xce,0xde,0x7c,0x0c,0x1e,/*"Q"=31h*/
0x00,0x00,0xfc,0x66,0x66,0x66,0x7c,0x6c,0x66,0x66,0xe6,0x00,/*"R"=32h*/
0x00,0x00,0x78,0xcc,0xcc,0xc0,0x70,0x18,0xcc,0xcc,0x78,0x00,/*"S"=33h*/
0x00,0x00,0xfc,0xb4,0x30,0x30,0x30,0x30,0x30,0x30,0x78,0x00,/*"T"=34h*/
0x00,0x00,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0x78,0x00,/*"U"=35h*/
0x00,0x00,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0x78,0x30,0x00,/*"V"=36h*/
0x00,0x00,0xc6,0xc6,0xc6,0xc6,0xd6,0xd6,0x6c,0x6c,0x6c,0x00,/*"W"=37h*/
0x00,0x00,0xcc,0xcc,0xcc,0x78,0x30,0x78,0xcc,0xcc,0xcc,0x00,/*"X"=38h*/
0x00,0x00,0xcc,0xcc,0xcc,0xcc,0x78,0x30,0x30,0x30,0x78,0x00,/*"Y"=39h*/
0x00,0x00,0xfe,0xce,0x98,0x18,0x30,0x60,0x62,0xc6,0xfe,0x00,/*"Z"=3Ah*/
0x00,0x00,0x3c,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x3c,0x00,/*"["=3Bh*/
0x00,0x00,0x00,0x80,0xc0,0x60,0x30,0x18,0x0c,0x06,0x02,0x00,/*"\"=3Ch*/
0x00,0x00,0x3c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x3c,0x00,/*"]"=3Dh*/
0x00,0x10,0x38,0x6c,0xc6,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"^"=3Eh*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,/*"_"=3Fh*/
0x00,0x30,0x30,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"`"=40h*/
0x00,0x00,0x00,0x00,0x00,0x78,0x0c,0x7c,0xcc,0xcc,0x76,0x00,/*"a"=41h*/
0x00,0x00,0xe0,0x60,0x60,0x7c,0x66,0x66,0x66,0x66,0xdc,0x00,/*"b"=42h*/
0x00,0x00,0x00,0x00,0x00,0x78,0xcc,0xc0,0xc0,0xcc,0x78,0x00,/*"c"=43h*/
0x00,0x00,0x1c,0x0c,0x0c,0x7c,0xcc,0xcc,0xcc,0xcc,0x76,0x00,/*"d"=44h*/
0x00,0x00,0x00,0x00,0x00,0x78,0xcc,0xfc,0xc0,0xcc,0x78,0x00,/*"e"=45h*/
0x00,0x00,0x38,0x6c,0x60,0x60,0xf8,0x60,0x60,0x60,0xf0,0x00,/*"f"=46h*/
0x00,0x00,0x00,0x00,0x00,0x76,0xcc,0xcc,0xcc,0x7c,0x0c,0xcc,/*"g"=47h*/
0x00,0x00,0xe0,0x60,0x60,0x6c,0x76,0x66,0x66,0x66,0xe6,0x00,/*"h"=48h*/
0x00,0x00,0x18,0x18,0x00,0x78,0x18,0x18,0x18,0x18,0x7e,0x00,/*"i"=49h*/
0x00,0x00,0x0c,0x0c,0x00,0x3c,0x0c,0x0c,0x0c,0x0c,0xcc,0xcc,/*"j"=4Ah*/
0x00,0x00,0xe0,0x60,0x60,0x66,0x6c,0x78,0x6c,0x66,0xe6,0x00,/*"k"=4Bh*/
0x00,0x00,0x78,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x7e,0x00,/*"l"=4Ch*/
0x00,0x00,0x00,0x00,0x00,0xfc,0xd6,0xd6,0xd6,0xd6,0xc6,0x00,/*"m"=4Dh*/
0x00,0x00,0x00,0x00,0x00,0xf8,0xcc,0xcc,0xcc,0xcc,0xcc,0x00,/*"n"=4Eh*/
0x00,0x00,0x00,0x00,0x00,0x78,0xcc,0xcc,0xcc,0xcc,0x78,0x00,/*"o'=4Fh*/
0x00,0x00,0x00,0x00,0x00,0xdc,0x66,0x66,0x66,0x66,0x7c,0x60,/*"p"=50h*/
0xf0,0x00,0x00,0x00,0x00,0x76,0xcc,0xcc,0xcc,0xcc,0x7c,0x0c,/*"q"=51h*/
0x00,0x00,0x00,0x00,0x00,0xec,0x6e,0x76,0x60,0x60,0xf0,0x00,/*"r"=52h*/
0x00,0x00,0x00,0x00,0x00,0x78,0xcc,0x60,0x18,0xcc,0x78,0x00,/*"s"=53h*/
0x00,0x00,0x00,0x20,0x60,0xfc,0x60,0x60,0x60,0x6c,0x38,0x00,/*"t"=54h*/
0x00,0x00,0x00,0x00,0x00,0xcc,0xcc,0xcc,0xcc,0xcc,0x76,0x00,/*"u"=55h*/
0x00,0x00,0x00,0x00,0x00,0xcc,0xcc,0xcc,0xcc,0x78,0x30,0x00,/*"v"=56h*/
0x00,0x00,0x00,0x00,0x00,0xc6,0xc6,0xd6,0xd6,0x6c,0x6c,0x00,/*"w"=57h*/
0x00,0x00,0x00,0x00,0x00,0xc6,0x6c,0x38,0x38,0x6c,0xc6,0x00,/*"x"=58h*/
0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x3c,0x0c,0x18,/*"y"=59h*/
0x00,0x00,0x00,0x00,0x00,0xfc,0x8c,0x18,0x60,0xc4,0xfc,0x00,/*"z"=5Ah*/
0x00,0x00,0x1c,0x30,0x30,0x60,0xc0,0x60,0x30,0x30,0x1c,0x00,/*"{"=5Bh*/
0x00,0x00,0x18,0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x18,0x00,/*"|"=5Ch*/
0x00,0x00,0xe0,0x30,0x30,0x18,0x0c,0x18,0x30,0x30,0xe0,0x00,/*"}"=5Dh*/
0x00,0x00,0x73,0xda,0xce,0x00,0x00,0x00,0x00,0x00,0x00,0x00,           
0x00,0x00,0x00,0x00,0x10,0x38,0x6c,0xc6,0xc6,0xfe,0x00,0x00,/*"~"=5Fh*/
};
 
/* LCD Control lines */
#define HAL_LCD_MODE_PORT 0
#define HAL_LCD_MODE_PIN  0

#define HAL_LCD_RESET_PORT 1
#define HAL_LCD_RESET_PIN  1

#define HAL_LCD_CS_PORT 1
#define HAL_LCD_CS_PIN  2

/* LCD SPI lines */
#define HAL_LCD_CLK_PORT 1
#define HAL_LCD_CLK_PIN  5

#define HAL_LCD_MOSI_PORT 1
#define HAL_LCD_MOSI_PIN  6

/*No output data comes from LCD 30/8/2010 ++ by feibit */
//#define HAL_LCD_MISO_PORT 1
//#define HAL_LCD_MISO_PIN  7

/* LCD backlight control pin */
#define HAL_LCD_BK_PORT 0
#define HAL_LCD_BK_PIN  7

/* SPI settings */
#define HAL_SPI_CLOCK_POL_LO       0x00
#define HAL_SPI_CLOCK_PHA_0        0x00
#define HAL_SPI_TRANSFER_MSB_FIRST 0x20

/* LCD lines */
#define LCD_MAX_LINE_COUNT              3
#define LCD_MAX_LINE_LENGTH             16
#define LCD_MAX_BUF                     25

/* Defines for HW LCD */

/**************************************************************************************************
 *                                           MACROS
 **************************************************************************************************/

#define HAL_IO_SET(port, pin, val)        HAL_IO_SET_PREP(port, pin, val)
#define HAL_IO_SET_PREP(port, pin, val)   st( P##port##_##pin## = val; )

#define HAL_CONFIG_IO_OUTPUT(port, pin, val)      HAL_CONFIG_IO_OUTPUT_PREP(port, pin, val)
#define HAL_CONFIG_IO_OUTPUT_PREP(port, pin, val) st( P##port##SEL &= ~BV(pin); \
                                                      P##port##_##pin## = val; \
                                                      P##port##DIR |= BV(pin); )

#define HAL_CONFIG_IO_PERIPHERAL(port, pin)      HAL_CONFIG_IO_PERIPHERAL_PREP(port, pin)
#define HAL_CONFIG_IO_PERIPHERAL_PREP(port, pin) st( P##port##SEL |= BV(pin); )



/* SPI interface control */
#define LCD_SPI_BEGIN()     HAL_IO_SET(HAL_LCD_CS_PORT,  HAL_LCD_CS_PIN,  0); /* chip select */
#define LCD_SPI_END()                                                         \
{                                                                             \
  asm("NOP");                                                                 \
  asm("NOP");                                                                 \
  asm("NOP");                                                                 \
  asm("NOP");                                                                 \
  HAL_IO_SET(HAL_LCD_CS_PORT,  HAL_LCD_CS_PIN,  1); /* chip select */         \
}
/* clear the received and transmit byte status, write tx data to buffer, wait till transmit done */
#define LCD_SPI_TX(x)                   { U1CSR &= ~(BV(2) | BV(1)); U1DBUF = x; while( !(U1CSR & BV(1)) ); }
#define LCD_SPI_WAIT_RXRDY()            { while(!(U1CSR & BV(1))); }


/* Control macros */
#define LCD_DO_WRITE()        HAL_IO_SET(HAL_LCD_MODE_PORT,  HAL_LCD_MODE_PIN,  1);
#define LCD_DO_CONTROL()      HAL_IO_SET(HAL_LCD_MODE_PORT,  HAL_LCD_MODE_PIN,  0);

#define LCD_ACTIVATE_RESET()  HAL_IO_SET(HAL_LCD_RESET_PORT, HAL_LCD_RESET_PIN, 0);
#define LCD_RELEASE_RESET()   HAL_IO_SET(HAL_LCD_RESET_PORT, HAL_LCD_RESET_PIN, 1);

#define CONTRAST_CTRL_REGISTER          0x81

/**************************************************************************************************
 *                                       GLOBAL VARIABLES
 **************************************************************************************************/
static uint8 *Lcd_Line1;

/**************************************************************************************************
 *                                       FUNCTIONS - API
 **************************************************************************************************/
#if (HAL_LCD == TRUE)
void HalLcd_HW_Init(void);
void HalLcd_HW_WaitUs(uint16 i);
void HalLcd_HW_Clear(void);
void HalLcd_HW_Control(uint8 cmd);
void HalLcd_HW_Write(uint8 data);
void HalLcd_HW_SetContrast(uint8 value);
void HalLcd_HW_WriteChar(uint8 line, uint8 col, char text);
void HalLcd_HW_WriteLine(uint8 line, const char *pText);
/* 30/8/2010 ++ by feibit */
void SET_DDRAM_ADDR(uint8 x, uint8 y);
uint8 HalLcd_Convert_DZK(uint8 index, uint16 addr);

#endif //LCD

/**************************************************************************************************
 * @fn      HalLcdInit
 *
 * @brief   Initilize LCD Service
 *
 * @param   init - pointer to void that contains the initialized value
 *
 * @return  None
 **************************************************************************************************/
void HalLcdInit(void)
{
  Lcd_Line1 = NULL;
#if (HAL_LCD == TRUE)
  HalLcd_HW_Init();
#endif
}

/*************************************************************************************************
 *                    LCD EMULATION FUNCTIONS
 *
 * Some evaluation boards are equipped with Liquid Crystal Displays
 * (LCD) which may be used to display diagnostic information. These
 * functions provide LCD emulation, sending the diagnostic strings
 * to Z-Tool via the RS232 serial port. These functions are enabled
 * when the "LCD_SUPPORTED" compiler flag is placed in the makefile.
 *
 * Most applications update both lines (1 and 2) of the LCD whenever
 * text is posted to the device. This emulator assumes that line 1 is
 * updated first (saved locally) and the formatting and send operation
 * is triggered by receipt of line 2. Nothing will be transmitted if
 * only line 1 is updated.
 *
 *************************************************************************************************/

/* 30/8/2010 ++ by feibit */
void SET_DDRAM_ADDR(uint8 x, uint8 y)
{
	HalLcd_HW_Control(y|0xb0);
	HalLcd_HW_Control((x >> 4)|0x10);
	HalLcd_HW_Control(x&0x0f);
}

/**************************************************************************************************
 * @fn      HalLcdWriteString
 *
 * @brief   Write a string to the LCD
 *
 * @param   str    - pointer to the string that will be displayed
 *          option - display options
 *
 * @return  None
 **************************************************************************************************/
void HalLcdWriteString ( char *str, uint8 option)
{
#if (HAL_LCD == TRUE)

  uint8 strLen = 0;
  uint8 totalLen = 0;
  uint8 *buf;
  uint8 tmpLen;

  if ( Lcd_Line1 == NULL )
  {
    Lcd_Line1 = osal_mem_alloc( HAL_LCD_MAX_CHARS+1 );
    HalLcdWriteString( "TexasInstruments", 1 );
  }

  strLen = (uint8)osal_strlen( (char*)str );

  /* Check boundries */
  if ( strLen > HAL_LCD_MAX_CHARS )
    strLen = HAL_LCD_MAX_CHARS;

  if ( option == HAL_LCD_LINE_1 )
  {
    /* Line 1 gets saved for later */
    osal_memcpy( Lcd_Line1, str, strLen );
    Lcd_Line1[strLen] = '\0';
  }
  else
  {
    /* Line 2 triggers action */
    tmpLen = (uint8)osal_strlen( (char*)Lcd_Line1 );
    totalLen =  tmpLen + 1 + strLen + 1;
    buf = osal_mem_alloc( totalLen );
    if ( buf != NULL )
    {
      /* Concatenate strings */
      osal_memcpy( buf, Lcd_Line1, tmpLen );
      buf[tmpLen++] = ' ';
      osal_memcpy( &buf[tmpLen], str, strLen );
      buf[tmpLen+strLen] = '\0';

      /* Send it out */
#if defined (ZTOOL_P1) || defined (ZTOOL_P2)

#if defined(SERIAL_DEBUG_SUPPORTED)
      debug_str( (uint8*)buf );
#endif //LCD_SUPPORTED

#endif //ZTOOL_P1

      /* Free mem */
      osal_mem_free( buf );
    }
  }

  /* Display the string */
  HalLcd_HW_WriteLine (option, str);

#endif //HAL_LCD

}

/**************************************************************************************************
 * @fn      HalLcdWriteValue
 *
 * @brief   Write a value to the LCD
 *
 * @param   value  - value that will be displayed
 *          radix  - 8, 10, 16
 *          option - display options
 *
 * @return  None
 **************************************************************************************************/
void HalLcdWriteValue ( uint32 value, const uint8 radix, uint8 option)
{
#if (HAL_LCD == TRUE)
  uint8 buf[LCD_MAX_BUF];

  _ltoa( value, &buf[0], radix );
  HalLcdWriteString( (char*)buf, option );
#endif
}

/**************************************************************************************************
 * @fn      HalLcdWriteScreen
 *
 * @brief   Write a value to the LCD
 *
 * @param   line1  - string that will be displayed on line 1
 *          line2  - string that will be displayed on line 2
 *
 * @return  None
 **************************************************************************************************/
void HalLcdWriteScreen( char *line1, char *line2 )
{
#if (HAL_LCD == TRUE)
  HalLcdWriteString( line1, 1 );
  HalLcdWriteString( line2, 2 );
#endif
}

/**************************************************************************************************
 * @fn      HalLcdWriteStringValue
 *
 * @brief   Write a string followed by a value to the LCD
 *
 * @param   title  - Title that will be displayed before the value
 *          value  - value
 *          format - redix
 *          line   - line number
 *
 * @return  None
 **************************************************************************************************/
void HalLcdWriteStringValue( char *title, uint16 value, uint8 format, uint8 line )
{
#if (HAL_LCD == TRUE)
  uint8 tmpLen;
  uint8 buf[LCD_MAX_BUF];
  uint32 err;

  tmpLen = (uint8)osal_strlen( (char*)title );
  osal_memcpy( buf, title, tmpLen );
  buf[tmpLen] = ' ';
  err = (uint32)(value);
  _ltoa( err, &buf[tmpLen+1], format );
  HalLcdWriteString( (char*)buf, line );		
#endif
}

/**************************************************************************************************
 * @fn      HalLcdWriteStringValue
 *
 * @brief   Write a string followed by a value to the LCD
 *
 * @param   title   - Title that will be displayed before the value
 *          value1  - value #1
 *          format1 - redix of value #1
 *          value2  - value #2
 *          format2 - redix of value #2
 *          line    - line number
 *
 * @return  None
 **************************************************************************************************/
void HalLcdWriteStringValueValue( char *title, uint16 value1, uint8 format1,
                                  uint16 value2, uint8 format2, uint8 line )
{

#if (HAL_LCD == TRUE)

  uint8 tmpLen;
  uint8 buf[LCD_MAX_BUF];
  uint32 err;

  tmpLen = (uint8)osal_strlen( (char*)title );
  if ( tmpLen )
  {
    osal_memcpy( buf, title, tmpLen );
    buf[tmpLen++] = ' ';
  }

  err = (uint32)(value1);
  _ltoa( err, &buf[tmpLen], format1 );
  tmpLen = (uint8)osal_strlen( (char*)buf );

  buf[tmpLen++] = ',';
  buf[tmpLen++] = ' ';
  err = (uint32)(value2);
  _ltoa( err, &buf[tmpLen], format2 );

  HalLcdWriteString( (char *)buf, line );		

#endif
}

/**************************************************************************************************
 * @fn      HalLcdDisplayPercentBar
 *
 * @brief   Display percentage bar on the LCD
 *
 * @param   title   -
 *          value   -
 *
 * @return  None
 **************************************************************************************************/
void HalLcdDisplayPercentBar( char *title, uint8 value )
{
#if (HAL_LCD == TRUE)

  uint8 percent;
  uint8 leftOver;
  uint8 buf[17];
  uint32 err;
  uint8 x;

  /* Write the title: */
  HalLcdWriteString( title, HAL_LCD_LINE_1 );

  if ( value > 100 )
    value = 100;

  /* convert to blocks */
  percent = (uint8)(value / 10);
  leftOver = (uint8)(value % 10);

  /* Make window */
  osal_memcpy( buf, "[          ]  ", 15 );

  for ( x = 0; x < percent; x ++ )
  {
    buf[1+x] = '>';
  }

  if ( leftOver >= 5 )
    buf[1+x] = '+';

  err = (uint32)value;
  _ltoa( err, (uint8*)&buf[13], 10 );

  HalLcdWriteString( (char*)buf, HAL_LCD_LINE_2 );

#endif

}


#if (HAL_LCD == TRUE)
/**************************************************************************************************
 *                                    HARDWARE LCD
 **************************************************************************************************/

/**************************************************************************************************
 * @fn      halLcd_ConfigIO
 *
 * @brief   Configure IO lines needed for LCD control.
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
static void halLcd_ConfigIO(void)
{
  /* GPIO configuration */
  HAL_CONFIG_IO_OUTPUT(HAL_LCD_MODE_PORT,  HAL_LCD_MODE_PIN,  1);
  HAL_CONFIG_IO_OUTPUT(HAL_LCD_RESET_PORT, HAL_LCD_RESET_PIN, 1);
  HAL_CONFIG_IO_OUTPUT(HAL_LCD_CS_PORT,    HAL_LCD_CS_PIN,    1);
/* 30/8/2010 ++ by feibit */
  HAL_CONFIG_IO_OUTPUT(HAL_LCD_BK_PORT,    HAL_LCD_BK_PIN,    1);

}

/**************************************************************************************************
 * @fn      halLcd_ConfigSPI
 *
 * @brief   Configure SPI lines needed for talking to LCD.
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
static void halLcd_ConfigSPI(void)
{
  /* UART/SPI Peripheral configuration */

   uint8 baud_exponent;
   uint8 baud_mantissa;

  /* Set SPI on UART 1 alternative 2 */
  PERCFG |= 0x02;

  /* Configure clk, master out and master in lines */
  HAL_CONFIG_IO_PERIPHERAL(HAL_LCD_CLK_PORT,  HAL_LCD_CLK_PIN);
  HAL_CONFIG_IO_PERIPHERAL(HAL_LCD_MOSI_PORT, HAL_LCD_MOSI_PIN);
/* 30/8/2010 ++ by feibit */
//  HAL_CONFIG_IO_PERIPHERAL(HAL_LCD_MISO_PORT, HAL_LCD_MISO_PIN);


  /* Set SPI speed to 1 MHz (the values assume system clk of 32MHz)
   * Confirm on board that this results in 1MHz spi clk.
   */
  baud_exponent = 15;
  baud_mantissa =  0;

  /* Configure SPI */
  U1UCR  = 0x80;      /* Flush and goto IDLE state. 8-N-1. */
  U1CSR  = 0x00;      /* SPI mode, master. */
  U1GCR  = HAL_SPI_TRANSFER_MSB_FIRST | HAL_SPI_CLOCK_PHA_0 | HAL_SPI_CLOCK_POL_LO | baud_exponent;
  U1BAUD = baud_mantissa;
}

/**************************************************************************************************
 * @fn      HalLcd_HW_Init
 *
 * @brief   Initilize HW LCD Driver.
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_Init(void)
{
    /* Initialize LCD IO lines */
    halLcd_ConfigIO();

    /* Initialize SPI */
    halLcd_ConfigSPI();

    HalLcd_HW_Control(0xaf);    //开显示
    HalLcd_HW_Control(0x40);    //显示起始行为0
    HalLcd_HW_Control(0xa0);    //RAM列地址与列驱动同顺序
    HalLcd_HW_Control(0xa6);    //正向显示
    HalLcd_HW_Control(0xa4);    //显示全亮功能关闭
    HalLcd_HW_Control(0xa2);    //LCD偏压比1/9
    HalLcd_HW_Control(0xc8);    //行驱动方向为反向
    HalLcd_HW_Control(0x2f);    //启用内部LCD驱动电源
    HalLcd_HW_Control(0xf8);    //升压电路设置指令代码
    HalLcd_HW_Control(0x00);    //倍压设置为4X

    /* set contrast */
    HalLcd_HW_SetContrast(0x38);
    
    /* Clear the display */
    HalLcd_HW_Clear();

}

/**************************************************************************************************
 * @fn      HalLcd_HW_Control
 *
 * @brief   Write 1 command to the LCD
 *
 * @param   uint8 cmd - command to be written to the LCD
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_Control(uint8 cmd)
{
  LCD_SPI_BEGIN();
  LCD_DO_CONTROL();
  LCD_SPI_TX(cmd);
  LCD_SPI_WAIT_RXRDY();
  LCD_SPI_END();
}

/**************************************************************************************************
 * @fn      HalLcd_HW_Write
 *
 * @brief   Write 1 byte to the LCD
 *
 * @param   uint8 data - data to be written to the LCD
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_Write(uint8 data)
{
  LCD_SPI_BEGIN();
  LCD_DO_WRITE();
  LCD_SPI_TX(data);
  LCD_SPI_WAIT_RXRDY();
  LCD_SPI_END();
}

/**************************************************************************************************
 * @fn          HalLcd_HW_SetContrast
 *
 * @brief       Set display contrast
 *
 * @param       uint8 value - contrast value
 *
 * @return      none
 **************************************************************************************************/
void HalLcd_HW_SetContrast(uint8 value)
{
    HalLcd_HW_Control(CONTRAST_CTRL_REGISTER);    
    HalLcd_HW_Control(value);   
}

/**************************************************************************************************
 * @fn      HalLcd_HW_Clear
 *
 * @brief   Clear the HW LCD
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_Clear(void)
{
	uint8 i,j;
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 132; j++)
		{
			SET_DDRAM_ADDR(j,i);
			HalLcd_HW_Write(0);
		}
	}
}

/**************************************************************************************************
 * @fn      HalLcd_HW_WriteChar
 *
 * @brief   Write one char to the display
 *
 * @param   uint8 line - line number that the char will be displayed
 *          uint8 col - colum where the char will be displayed
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_WriteChar(uint8 line, uint8 col, char text)
{
    uint8 j;
    uint16 addr;
    uint8 cvt_result;

    if (col < LCD_MAX_LINE_LENGTH && line <= HAL_LCD_LINE_4)
    {
            
        //write first line 
        SET_DDRAM_ADDR(col * 8, (line - 1) * 2);
        addr = (text - 0x20)*12;

        for (j = 0; j < 8; j++)
        {
             cvt_result = HalLcd_Convert_DZK(j,addr);
             cvt_result = cvt_result << 2;
        	HalLcd_HW_Write(cvt_result);
        }

        //write second line 
        SET_DDRAM_ADDR(col * 8, (line - 1) * 2 + 1);
        addr = addr + 6;

        for (j = 0; j < 8; j++)
        {
             cvt_result = HalLcd_Convert_DZK(j,addr);
        	HalLcd_HW_Write(cvt_result);
        }

    }
    else
    {
        return;
    }
}

uint8 HalLcd_Convert_DZK(uint8 index, uint16 addr)
{
    uint8 return_val = 0;
    uint8 j;
    uint8 temp;
    uint8 mask_array[8] = {BIT7,BIT6,BIT5,BIT4,BIT3,BIT2,BIT1,BIT0};
    uint8 mask = mask_array[index];

    if (index > 7)
    {
        return return_val;
    }

    for (j = 0; j < 6; j++)
    {
        temp = ASCIITAB_DZK[addr + j];
        temp = temp & mask;
        temp = temp >> (7 - index);
        if (temp)
        {
            return_val |=  mask_array[7-j];
        }
        else
        {
            return_val &=  ~mask_array[7-j];
        }
    }

    return return_val;
}

/**************************************************************************************************
 * @fn          halLcdWriteLine
 *
 * @brief       Write one line on display
 *
 * @param       uint8 line - display line
 *              char *pText - text buffer to write
 *
 * @return      none
 **************************************************************************************************/
void HalLcd_HW_WriteLine(uint8 line, const char *pText)
{
  uint8 count;
  uint8 totalLength = (uint8)osal_strlen( (char *)pText );

  /* Write the content first */
  for (count=0; count<totalLength; count++)
  {
    HalLcd_HW_WriteChar(line, count, (*(pText++)));
  }

  /* Write blank spaces to rest of the line */
  for(count=totalLength; count<LCD_MAX_LINE_LENGTH;count++)
  {
    HalLcd_HW_WriteChar(line, count, ' ');
  }
}

/**************************************************************************************************
 * @fn      HalLcd_HW_WaitUs
 *
 * @brief   wait for x us. @ 32MHz MCU clock it takes 32 "nop"s for 1 us delay.
 *
 * @param   x us. range[0-65536]
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_WaitUs(uint16 microSecs)
{
  while(microSecs--)
  {
    /* 32 NOPs == 1 usecs */
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop");
  }
}
#endif


/**************************************************************************************************
**************************************************************************************************/



