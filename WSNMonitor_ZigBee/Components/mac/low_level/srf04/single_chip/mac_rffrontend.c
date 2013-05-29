#include "hal_types.h"
#include "hal_board_cfg.h"
#include "hal_assert.h"
#include "mac_api.h"


/* ------------------------------------------------------------------------------------------------
 *                                        Global Constants
 * ------------------------------------------------------------------------------------------------
 */

/* The following table index definitions are specific to a particular MAC build */
#define MAC_CC2591_TX_PWR_TABLE_IDX   1
#define MAC_CC2590_TX_PWR_TABLE_IDX   2
#define MAC_CC2591_HGM_RSSI_ADJ_IDX   1
#define MAC_CC2591_LGM_RSSI_ADJ_IDX   2
#define MAC_CC2590_HGM_RSSI_ADJ_IDX   3
#define MAC_CC2590_LGM_RSSI_ADJ_IDX   4

/* ------------------------------------------------------------------------------------------------
 *                                           MACROS
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_PA_LNA_RX_HGM()                           st( P0_7 = 1; )
#define HAL_PA_LNA_RX_LGM()                           st( P0_7 = 0; )


/* ------------------------------------------------------------------------------------------------
 *                                       Function Prototypes
 * ------------------------------------------------------------------------------------------------
 */
void MAC_RfFrontendSetup(void);

/**************************************************************************************************
 * @fn          MAC_RfFrontendSetup
 *
 * @brief       Setup RF frontend.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
void MAC_RfFrontendSetup(void)
{
  /* AGCCTRL1 for CC2590 or CC2591 */
  AGCCTRL1 = 0x16;

  /* CC2591 HGM pin control configuration
   *   P0_7 -> HGM
   */
  HAL_PA_LNA_RX_HGM();

  /* Select power register value table and RSSI adjustment value table */
  MAC_SetRadioRegTable(MAC_CC2591_TX_PWR_TABLE_IDX, MAC_CC2591_HGM_RSSI_ADJ_IDX);
}

/**************************************************************************************************
 */
