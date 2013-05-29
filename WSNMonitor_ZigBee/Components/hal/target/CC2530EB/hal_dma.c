#include "hal_types.h"
#include "hal_defs.h"
#include "hal_dma.h"
#include "hal_mcu.h"
#include "hal_uart.h"

#if (defined HAL_IRGEN) && (HAL_IRGEN == TRUE)
#include "hal_irgen.h"
#endif

#if (defined HAL_SPI) && (HAL_SPI == TRUE)
#include "hal_spi.h"
#endif


#if ((defined HAL_DMA) && (HAL_DMA == TRUE))

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

halDMADesc_t dmaCh0;
halDMADesc_t dmaCh1234[4];

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/******************************************************************************
 * @fn      HalDMAInit
 *
 * @brief   DMA Interrupt Service Routine
 *
 * @param   None
 *
 * @return  None
 *****************************************************************************/
void HalDmaInit( void )
{
  HAL_DMA_SET_ADDR_DESC0( &dmaCh0 );
  HAL_DMA_SET_ADDR_DESC1234( dmaCh1234 );
#if (HAL_UART_DMA || \
   ((defined HAL_SPI) && (HAL_SPI == TRUE))  || \
   ((defined HAL_IRGEN) && (HAL_IRGEN == TRUE)))
  DMAIE = 1;
#endif
}

#if (HAL_UART_DMA || \
   ((defined HAL_SPI) && (HAL_SPI == TRUE))  || \
   ((defined HAL_IRGEN) && (HAL_IRGEN == TRUE)))
/******************************************************************************
 * @fn      HalDMAInit
 *
 * @brief   DMA Interrupt Service Routine
 *
 * @param   None
 *
 * @return  None
 *****************************************************************************/
HAL_ISR_FUNCTION( halDmaIsr, DMA_VECTOR )
{
  extern void HalUARTIsrDMA(void);

  DMAIF = 0;

#if HAL_UART_DMA
  if (HAL_DMA_CHECK_IRQ(HAL_DMA_CH_TX))
  {
    HalUARTIsrDMA();
  }
#endif // HAL_UART_DMA

#if (defined HAL_SPI) && (HAL_SPI == TRUE)
  if ( HAL_DMA_CHECK_IRQ( HAL_DMA_CH_RX ) )
  {
    HAL_DMA_CLEAR_IRQ( HAL_DMA_CH_RX );
    HalSpiRxIsr();
  }

  if ( HAL_DMA_CHECK_IRQ( HAL_DMA_CH_TX ) )
  {
    HAL_DMA_CLEAR_IRQ( HAL_DMA_CH_TX );
    HalSpiTxIsr();
  }
#endif // (defined HAL_SPI) && (HAL_SPI == TRUE)

#if (defined HAL_IRGEN) && (HAL_IRGEN == TRUE)
  if ( HAL_IRGEN == TRUE && HAL_DMA_CHECK_IRQ( HAL_IRGEN_DMA_CH ) )
  {
    HAL_DMA_CLEAR_IRQ( HAL_IRGEN_DMA_CH );
    HalIrGenDmaIsr();
  }
#endif // (defined HAL_IRGEN) && (HAL_IRGEN == TRUE)
}
#endif
#endif  // #if ((defined HAL_DMA) && (HAL_DMA == TRUE))
/******************************************************************************
******************************************************************************/
