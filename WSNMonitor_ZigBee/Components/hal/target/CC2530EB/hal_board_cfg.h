#ifndef HAL_BOARD_CFG_H
#define HAL_BOARD_CFG_H

/*
 *     =============================================================
 *     |        Chipcon SmartRF05EB Evaluation Board, plus         |
 *     |            Texas Instruments CC2530EM Evaluation Module   |
 *     | --------------------------------------------------------- |
 *     |  mcu   : 8051 core                                        |
 *     |  clock : 32MHz                                            |
 *     =============================================================
 */


/* ------------------------------------------------------------------------------------------------
 *                                           Includes
 * ------------------------------------------------------------------------------------------------
 */

#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_types.h"

/* ------------------------------------------------------------------------------------------------
 *                                       CC2590/CC2591 support
 *
 *                        Define HAL_PA_LNA_CC2590 if CC2530+CC2590EM is used
 *                        Define HAL_PA_LNA if CC2530+CC2591EM is used
 *                        Note that only one of them can be defined
 * ------------------------------------------------------------------------------------------------
 */
#define xHAL_PA_LNA
#define xHAL_PA_LNA_CC2590

//定义这个就会初始化cc2590
//#define HAL_PA_LNA


/* ------------------------------------------------------------------------------------------------
 *                                       Board Indentifier
 *
 *      Define the Board Identifier to HAL_BOARD_CC2530EB_REV13 for SmartRF05 EB 1.3 board
 * ------------------------------------------------------------------------------------------------
 */

#if !defined (HAL_BOARD_CC2530EB_REV17) && !defined (HAL_BOARD_CC2530EB_REV13)
  #define HAL_BOARD_CC2530EB_REV17
#endif

/* ------------------------------------------------------------------------------------------------
 *                                          Clock Speed
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_CPU_CLOCK_MHZ     32

/* 32 kHz clock source select in CLKCONCMD */
//#if !defined (OSC32K_CRYSTAL_INSTALLED) || (defined (OSC32K_CRYSTAL_INSTALLED) && (OSC32K_CRYSTAL_INSTALLED == TRUE))
 //#define OSC_32KHZ  0x00 /* external 32 KHz xosc */
//#else
  #define OSC_32KHZ  0x80 /* internal 32 KHz rcosc */
//#endif

/* ------------------------------------------------------------------------------------------------
 *                                       LED Configuration
 * ------------------------------------------------------------------------------------------------
 */

#if defined (HAL_BOARD_CC2530EB_REV17) && !defined (HAL_PA_LNA) && !defined (HAL_PA_LNA_CC2590)
  #define HAL_NUM_LEDS            3
#elif defined (HAL_BOARD_CC2530EB_REV13) || defined (HAL_PA_LNA) || defined (HAL_PA_LNA_CC2590)
  #define HAL_NUM_LEDS            1
#else
  #error Unknown Board Indentifier
#endif

#define HAL_LED_BLINK_DELAY()   st( { volatile uint32 i; for (i=0; i<0x5800; i++) { }; } )

/* 1 - Green */
#define LED1_BV           BV(0)
#define LED1_SBIT         P1_0
#define LED1_DDR          P1DIR
#define LED1_POLARITY     ACTIVE_HIGH

#if defined (HAL_BOARD_CC2530EB_REV17)
  /* 2 - Red */
  #define LED2_BV           BV(1)
  #define LED2_SBIT         P1_1
  #define LED2_DDR          P1DIR
  #define LED2_POLARITY     ACTIVE_HIGH

  /* 3 - Yellow */
  #define LED3_BV           BV(4)
  #define LED3_SBIT         P1_4
  #define LED3_DDR          P1DIR
  #define LED3_POLARITY     ACTIVE_HIGH
#endif

/* Realy*/
#define REALY_BV           BV(2)
#define REALY_SBIT         P1_2
#define REALY_DDR          P1DIR
#define REALY_POLARITY     ACTIVE_HIGH




/* ------------------------------------------------------------------------------------------------
 *                                    Push Button Configuration
 * ------------------------------------------------------------------------------------------------
 */

#define ACTIVE_LOW        !
#define ACTIVE_HIGH       !!    /* double negation forces result to be '1' */

//相当于大板的S1 和小板的S2
#define PUSH1_BV          BV(1)   
#define PUSH1_SBIT        P0_1      
//大板子的S6 
#define PUSH3_BV          BV(2)
#define PUSH3_SBIT        P1_2
 //大板子的S2
#define PUSH4_BV          BV(5)
#define PUSH4_SBIT        P0_5
//小板子的S3
#define PUSH5_BV          BV(6)
#define PUSH5_SBIT        P0_6
//小板子的S1
#define PUSH6_BV          BV(0)
#define PUSH6_SBIT        P0_0



#if defined (HAL_BOARD_CC2530EB_REV17)
  #define PUSH1_POLARITY    ACTIVE_HIGH
  #define PUSH3_POLARITY    ACTIVE_LOW
  #define PUSH6_POLARITY    ACTIVE_LOW
  #define PUSH5_POLARITY    ACTIVE_LOW

#elif defined (HAL_BOARD_CC2530EB_REV13)
  #define PUSH1_POLARITY    ACTIVE_LOW
#else
  #error Unknown Board Indentifier
#endif

/* Joystick Center Press */
#define PUSH2_BV          BV(0)
#define PUSH2_SBIT        P2_0
#define PUSH2_POLARITY    ACTIVE_HIGH

/* ------------------------------------------------------------------------------------------------
 *                         OSAL NV implemented by internal flash pages.
 * ------------------------------------------------------------------------------------------------
 */

// Flash is partitioned into 8 banks of 32 KB or 16 pages.
#define HAL_FLASH_PAGE_PER_BANK    16
// Flash is constructed of 128 pages of 2 KB.
#define HAL_FLASH_PAGE_SIZE        2048
#define HAL_FLASH_WORD_SIZE        4

// CODE banks get mapped into the XDATA range 8000-FFFF.
#define HAL_FLASH_PAGE_MAP         0x8000

// The last 16 bytes of the last available page are reserved for flash lock bits.
#if defined NON_BANKED
#define HAL_FLASH_LOCK_BITS        16
#define HAL_NV_PAGE_END            30
#define HAL_NV_PAGE_CNT            2
#else
#define HAL_FLASH_LOCK_BITS        16
#define HAL_NV_PAGE_END            126
#define HAL_NV_PAGE_CNT            6
#endif

// Re-defining Z_EXTADDR_LEN here so as not to include a Z-Stack .h file.
#define HAL_FLASH_IEEE_SIZE        8
#define HAL_FLASH_IEEE_PAGE       (HAL_NV_PAGE_END+1)
#define HAL_FLASH_IEEE_OSET       (HAL_FLASH_PAGE_SIZE - HAL_FLASH_LOCK_BITS - HAL_FLASH_IEEE_SIZE)
#define HAL_INFOP_IEEE_OSET        0xC

#define HAL_NV_PAGE_BEG           (HAL_NV_PAGE_END-HAL_NV_PAGE_CNT+1)

// Used by DMA macros to shift 1 to create a mask for DMA registers.
#define HAL_NV_DMA_CH              0
#define HAL_DMA_CH_RX              3
#define HAL_DMA_CH_TX              4

#define HAL_NV_DMA_GET_DESC()      HAL_DMA_GET_DESC0()
#define HAL_NV_DMA_SET_ADDR(a)     HAL_DMA_SET_ADDR_DESC0((a))


/* ------------------------------------------------------------------------------------------------
 *                                            Macros
 * ------------------------------------------------------------------------------------------------
 */

/* ----------- RF-frontend Connection Initialization ---------- */
#if defined HAL_PA_LNA || defined HAL_PA_LNA_CC2590
extern void MAC_RfFrontendSetup(void);
#define HAL_BOARD_RF_FRONTEND_SETUP() MAC_RfFrontendSetup()
#else
#define HAL_BOARD_RF_FRONTEND_SETUP()
#endif

/* ----------- Cache Prefetch control ---------- */
#define PREFETCH_ENABLE()     st( FCTL = 0x08; )
#define PREFETCH_DISABLE()    st( FCTL = 0x04; )

/* ----------- Board Initialization ---------- */
#if defined (HAL_BOARD_CC2530EB_REV17) && !defined (HAL_PA_LNA) && !defined (HAL_PA_LNA_CC2590)

#define HAL_BOARD_INIT()                                         \
{                                                                \
  uint16 i;                                                      \
                                                                 \
  SLEEPCMD &= ~OSC_PD;                       /* turn on 16MHz RC and 32MHz XOSC */                \
  while (!(SLEEPSTA & XOSC_STB));            /* wait for 32MHz XOSC stable */                     \
  asm("NOP");                                /* chip bug workaround */                            \
  for (i=0; i<504; i++) asm("NOP");          /* Require 63us delay for all revs */                \
  CLKCONCMD = (CLKCONCMD_32MHZ | OSC_32KHZ); /* Select 32MHz XOSC and the source for 32K clock */ \
  while (CLKCONSTA != (CLKCONCMD_32MHZ | OSC_32KHZ)); /* Wait for the change to be effective */   \
  SLEEPCMD |= OSC_PD;                        /* turn off 16MHz RC */                              \
                                                                 \
  /* Turn on cache prefetch mode */                              \
  PREFETCH_ENABLE();                                             \
                                                                 \
  /* set direction for GPIO outputs  */                          \
  LED1_DDR |= LED1_BV;                                           \
  LED2_DDR |= LED2_BV;                                           \
  LED3_DDR |= LED3_BV;                                           \
                                                                 \
  /* configure tristates */                                      \
  P0INP |= PUSH2_BV;                                             \
}

#elif defined (HAL_BOARD_CC2530EB_REV13) || defined (HAL_PA_LNA) || defined (HAL_PA_LNA_CC2590)

#define HAL_BOARD_INIT()                                         \
{                                                                \
  uint16 i;                                                      \
                                                                 \
  SLEEPCMD &= ~OSC_PD;                       /* turn on 16MHz RC and 32MHz XOSC */                \
  while (!(SLEEPSTA & XOSC_STB));            /* wait for 32MHz XOSC stable */                     \
  asm("NOP");                                /* chip bug workaround */                            \
  for (i=0; i<504; i++) asm("NOP");          /* Require 63us delay for all revs */                \
  CLKCONCMD = (CLKCONCMD_32MHZ | OSC_32KHZ); /* Select 32MHz XOSC and the source for 32K clock */ \
  while (CLKCONSTA != (CLKCONCMD_32MHZ | OSC_32KHZ)); /* Wait for the change to be effective */   \
  SLEEPCMD |= OSC_PD;                        /* turn off 16MHz RC */                              \
                                                                 \
  /* Turn on cache prefetch mode */                              \
  PREFETCH_ENABLE();                                             \
                                                                 \
  /* set direction for GPIO outputs  */                          \
  LED1_DDR |= LED1_BV;                                           \
                                                                 \
  /* Set PA/LNA HGM control P0_7 */                              \
  P0DIR |= BV(7);                                                \
                                                                 \
  /* configure tristates */                                      \
  P0INP |= PUSH2_BV;                                             \
                                                                 \
  /* setup RF frontend if necessary */                           \
  HAL_BOARD_RF_FRONTEND_SETUP();                                 \
}

#endif

/* ----------- Debounce ---------- */
#define HAL_DEBOUNCE(expr)    { int i; for (i=0; i<500; i++) { if (!(expr)) i = 0; } }

/* ----------- Push Buttons ---------- */
#define HAL_PUSH_BUTTON1()        (PUSH1_POLARITY (PUSH1_SBIT))
#define HAL_PUSH_BUTTON2()        (PUSH2_POLARITY (PUSH2_SBIT))
#define HAL_PUSH_BUTTON3()        (PUSH3_POLARITY (PUSH3_SBIT))
#define HAL_PUSH_BUTTON4()        (0)
#define HAL_PUSH_BUTTON5()        (PUSH5_POLARITY (PUSH5_SBIT))
#define HAL_PUSH_BUTTON6()        (PUSH6_POLARITY (PUSH6_SBIT))


/* ----------- REALY's ---------- */
  #define HAL_TURN_ON_RELAY()       st( REALY_SBIT = REALY_POLARITY (1); )
  #define HAL_TURN_OFF_RELAY()      st( REALY_SBIT = REALY_POLARITY (0); )

/* ----------- LED's ---------- */
#if defined (HAL_BOARD_CC2530EB_REV17) && !defined (HAL_PA_LNA) && !defined (HAL_PA_LNA_CC2590)

  #define HAL_TURN_OFF_LED1()       st( LED1_SBIT = LED1_POLARITY (0); )
  #define HAL_TURN_OFF_LED2()       st( LED2_SBIT = LED2_POLARITY (0); )
  #define HAL_TURN_OFF_LED3()       st( LED3_SBIT = LED3_POLARITY (0); )
  #define HAL_TURN_OFF_LED4()       HAL_TURN_OFF_LED1()

  #define HAL_TURN_ON_LED1()        st( LED1_SBIT = LED1_POLARITY (1); )
  #define HAL_TURN_ON_LED2()        st( LED2_SBIT = LED2_POLARITY (1); )
  #define HAL_TURN_ON_LED3()        st( LED3_SBIT = LED3_POLARITY (1); )
  #define HAL_TURN_ON_LED4()        HAL_TURN_ON_LED1()

  #define HAL_TOGGLE_LED1()         st( if (LED1_SBIT) { LED1_SBIT = 0; } else { LED1_SBIT = 1;} )
  #define HAL_TOGGLE_LED2()         st( if (LED2_SBIT) { LED2_SBIT = 0; } else { LED2_SBIT = 1;} )
  #define HAL_TOGGLE_LED3()         st( if (LED3_SBIT) { LED3_SBIT = 0; } else { LED3_SBIT = 1;} )
  #define HAL_TOGGLE_LED4()         HAL_TOGGLE_LED1()

  #define HAL_STATE_LED1()          (LED1_POLARITY (LED1_SBIT))
  #define HAL_STATE_LED2()          (LED2_POLARITY (LED2_SBIT))
  #define HAL_STATE_LED3()          (LED3_POLARITY (LED3_SBIT))
  #define HAL_STATE_LED4()          HAL_STATE_LED1()

#elif defined (HAL_BOARD_CC2530EB_REV13) || defined (HAL_PA_LNA) || defined (HAL_PA_LNA_CC2590)

  #define HAL_TURN_OFF_LED1()       st( LED1_SBIT = LED1_POLARITY (0); )
  #define HAL_TURN_OFF_LED2()       HAL_TURN_OFF_LED1()
  #define HAL_TURN_OFF_LED3()       HAL_TURN_OFF_LED1()
  #define HAL_TURN_OFF_LED4()       HAL_TURN_OFF_LED1()

  #define HAL_TURN_ON_LED1()        st( LED1_SBIT = LED1_POLARITY (1); )
  #define HAL_TURN_ON_LED2()        HAL_TURN_ON_LED1()
  #define HAL_TURN_ON_LED3()        HAL_TURN_ON_LED1()
  #define HAL_TURN_ON_LED4()        HAL_TURN_ON_LED1()

  #define HAL_TOGGLE_LED1()         st( if (LED1_SBIT) { LED1_SBIT = 0; } else { LED1_SBIT = 1;} )
  #define HAL_TOGGLE_LED2()         HAL_TOGGLE_LED1()
  #define HAL_TOGGLE_LED3()         HAL_TOGGLE_LED1()
  #define HAL_TOGGLE_LED4()         HAL_TOGGLE_LED1()

  #define HAL_STATE_LED1()          (LED1_POLARITY (LED1_SBIT))
  #define HAL_STATE_LED2()          HAL_STATE_LED1()
  #define HAL_STATE_LED3()          HAL_STATE_LED1()
  #define HAL_STATE_LED4()          HAL_STATE_LED1()

#endif



/* ----------- XNV ---------- */
#define XNV_SPI_BEGIN()             st(P1_3 = 0;)
#define XNV_SPI_TX(x)               st(U1CSR &= ~0x02; U1DBUF = (x);)
#define XNV_SPI_RX()                U1DBUF
#define XNV_SPI_WAIT_RXRDY()        st(while (!(U1CSR & 0x02));)
#define XNV_SPI_END()               st(P1_3 = 1;)

// The TI reference design uses UART1 Alt. 2 in SPI mode.
#define XNV_SPI_INIT() \
st( \
  /* Mode select UART1 SPI Mode as master. */\
  U1CSR = 0; \
  \
  /* Setup for 115200 baud. */\
  U1GCR = 11; \
  U1BAUD = 216; \
  \
  /* Set bit order to MSB */\
  U1GCR |= BV(5); \
  \
  /* Set UART1 I/O to alternate 2 location on P1 pins. */\
  PERCFG |= 0x02;  /* U1CFG */\
  \
  /* Select peripheral function on I/O pins but SS is left as GPIO for separate control. */\
  P1SEL |= 0xE0;  /* SELP1_[7:4] */\
  /* P1.1,2,3: reset, LCD CS, XNV CS. */\
  P1SEL &= ~0x0E; \
  P1 |= 0x0E; \
  P1_1 = 0; \
  P1DIR |= 0x0E; \
  \
  /* Give UART1 priority over Timer3. */\
  P2SEL &= ~0x20;  /* PRI2P1 */\
  \
  /* When SPI config is complete, enable it. */\
  U1CSR |= 0x40; \
  /* Release XNV reset. */\
  P1_1 = 1; \
)

/* ------------------------------------------------------------------------------------------------
 *                                     Driver Configuration
 * ------------------------------------------------------------------------------------------------
 */

/* Set to TRUE enable H/W TIMER usage, FALSE disable it */
#ifndef HAL_TIMER
#define HAL_TIMER FALSE
#endif

/* Set to TRUE enable ADC usage, FALSE disable it */
#ifndef HAL_ADC
#define HAL_ADC TRUE
#endif

/* Set to TRUE enable DMA usage, FALSE disable it */
#ifndef HAL_DMA
#define HAL_DMA TRUE
#endif

/* Set to TRUE enable Flash access, FALSE disable it */
#ifndef HAL_FLASH
#define HAL_FLASH TRUE
#endif

/* Set to TRUE enable AES usage, FALSE disable it */
#ifndef HAL_AES
#define HAL_AES TRUE
#endif

#ifndef HAL_AES_DMA
#define HAL_AES_DMA TRUE
#endif

/* Set to TRUE enable LCD usage, FALSE disable it */
#ifndef HAL_LCD
#define HAL_LCD TRUE
#endif


/* Set to TRUE enable LED usage, FALSE disable it */
#ifndef HAL_LED
#define HAL_LED TRUE
#endif
#if (!defined BLINK_LEDS) && (HAL_LED == TRUE)
#define BLINK_LEDS
#endif

/* Set to TRUE enable KEY usage, FALSE disable it */
#ifndef HAL_KEY
#define HAL_KEY TRUE
#endif

/* Set to TRUE enable UART usage, FALSE disable it */
#ifndef HAL_UART
#if (defined ZAPP_P1) || (defined ZAPP_P2) || (defined ZTOOL_P1) || (defined ZTOOL_P2)
#define HAL_UART TRUE
#else
#define HAL_UART FALSE
#endif
#endif

#if HAL_UART
// Always prefer to use DMA over ISR.
#if HAL_DMA
#ifndef HAL_UART_DMA
#if (defined ZAPP_P1) || (defined ZTOOL_P1)
#define HAL_UART_DMA  1
#elif (defined ZAPP_P2) || (defined ZTOOL_P2)
#define HAL_UART_DMA  2
#else
#define HAL_UART_DMA  1
#endif
#endif
#define HAL_UART_ISR  0
#else
#ifndef HAL_UART_ISR
#if (defined ZAPP_P1) || (defined ZTOOL_P1)
#define HAL_UART_ISR  1
#elif (defined ZAPP_P2) || (defined ZTOOL_P2)
#define HAL_UART_ISR  2
#else
#define HAL_UART_ISR  1
#endif
#endif
#define HAL_UART_DMA  0
#endif

// Used to set P2 priority - USART0 over USART1 if both are defined.
#if ((HAL_UART_DMA == 1) || (HAL_UART_ISR == 1))
#define HAL_UART_PRIPO             0x00
#else
#define HAL_UART_PRIPO             0x40
#endif

#else
#define HAL_UART_DMA  0
#define HAL_UART_ISR  0
#endif

/* USB is not used for CC2530 configuration */
#define HAL_UART_USB  0

/*******************************************************************************************************
*/
#endif
