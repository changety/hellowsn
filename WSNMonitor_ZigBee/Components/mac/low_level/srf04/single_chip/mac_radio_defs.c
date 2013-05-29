#include "mac_radio_defs.h"
#include "hal_types.h"
#include "hal_assert.h"
#include "mac_pib.h"


/* ------------------------------------------------------------------------------------------------
 *                                        Global Constants
 * ------------------------------------------------------------------------------------------------
 */
#if defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || \
  (!defined HAL_PA_LNA && !defined HAL_PA_LNA_CC2590)

const uint8 CODE macRadioDefsTxPwrBare[] =
{
  3,  /* tramsmit power level of the first entry */
  (uint8)(int8)-22, /* transmit power level of the last entry */
  /*   3 dBm */   0xF5,   /* characterized as  4.5 dBm in datasheet */
  /*   2 dBm */   0xE5,   /* characterized as  2.5 dBm in datasheet */
  /*   1 dBm */   0xD5,   /* characterized as  1   dBm in datasheet */
  /*   0 dBm */   0xD5,   /* characterized as  1   dBm in datasheet */
  /*  -1 dBm */   0xC5,   /* characterized as -0.5 dBm in datasheet */
  /*  -2 dBm */   0xB5,   /* characterized as -1.5 dBm in datasheet */
  /*  -3 dBm */   0xA5,   /* characterized as -3   dBm in datasheet */
  /*  -4 dBm */   0x95,   /* characterized as -4   dBm in datasheet */
  /*  -5 dBm */   0x95,
  /*  -6 dBm */   0x85,   /* characterized as -6   dBm in datasheet */
  /*  -7 dBm */   0x85,
  /*  -8 dBm */   0x75,   /* characterized as -8   dBm in datasheet */
  /*  -9 dBm */   0x75,
  /* -10 dBm */   0x65,   /* characterized as -10  dBm in datasheet */
  /* -11 dBm */   0x65,
  /* -12 dBm */   0x55,   /* characterized as -12  dBm in datasheet */
  /* -13 dBm */   0x55,
  /* -14 dBm */   0x45,   /* characterized as -14  dBm in datasheet */
  /* -15 dBm */   0x45,
  /* -16 dBm */   0x35,   /* characterized as -16  dBm in datasheet */
  /* -17 dBm */   0x35,
  /* -18 dBm */   0x25,   /* characterized as -18  dBm in datasheet */
  /* -19 dBm */   0x25,
  /* -20 dBm */   0x15,   /* characterized as -20  dBm in datasheet */
  /* -21 dBm */   0x15,
  /* -22 dBm */   0x05    /* characterized as -22  dBm in datasheet */
};
#endif

#if defined HAL_PA_LNA || defined MAC_RUNTIME_CC2591
const uint8 CODE macRadioDefsTxPwrCC2591[] =
{
  19,  /* tramsmit power level of the first entry */
  (uint8)(int8)0, /* transmit power level of the last entry */
  /*  19 dBm */   0xF5,   /* characterized as 19.36 dBm in datasheet */
  /*  18 dBm */   0xD5,   /* characterized as 18.45 dBm in datasheet */
  /*  17 dBm */   0xB5,   /* characterized as 17.37 dBm in datasheet */
  /*  16 dBm */   0xA5,   /* characterized as 16.42 dBm in datasheet */
  /*  15 dBm */   0x95,   /* characterized as 15.06 dBm in datasheet */
  /*  14 dBm */   0x85,   /* characterized as 14.08 dBm in datasheet */
  /*  13 dBm */   0x75,
  /*  12 dBm */   0x75,   /* characterized as 12.63 dBm in datasheet */
  /*  11 dBm */   0x65,   /* characterized as 10.97 dBm in datasheet */
  /*  10 dBm */   0x55,
  /*   9 dBm */   0x55,
  /*   8 dBm */   0x55,   /* characterized as 8.94 dBm in datasheet */
  /*   7 dBm */   0x45,
  /*   6 dBm */   0x45,   /* characterized as 6.64 dBm in datasheet */
  /*   5 dBm */   0x35,
  /*   4 dBm */   0x35,   /* characterized as 4.30 dBm in datasheet */
  /*   3 dBm */   0x25,
  /*   2 dBm */   0x25,   /* characterized as 2.71 dBm in datasheet */
  /*   1 dBm */   0x15,   /* characterized as 0.70 dBm in datasheet*/
  /*   0 dBm */   0x05,   /* characterized as -0.8 dBm in datasheet */
};
#endif


/* Placeholder for CC2590 */
#if defined HAL_PA_LNA_CC2590 || defined MAC_RUNTIME_CC2590
const uint8 CODE macRadioDefsTxPwrCC2590[] =
{
  11,  /* tramsmit power level of the first entry */
  (uint8)(int8)-12, /* transmit power level of the last entry */
  /* Note that the characterization is preliminary */
  /* 11 dBm */   0xF5,   /* characterized as 11.8 dBm */
  /* 10 dBm */   0xE5,   /* characterized as 10.6 dBm */
  /*  9 dBm */   0xD5,   /* characterized as 9.5 dBm */
  /*  8 dBm */   0xC5,   /* characterized as 8.5 dBm */
  /*  7 dBm */   0xB5,   /* characterized as 7.6 dBm */
  /*  6 dBm */   0xA5,   /* characterized as 6.4 dBm */
  /*  5 dBm */   0xA5,   /* characterized as 6.4 dBm */
  /*  4 dBm */   0x95,   /* characterized as 4.7 dBm */
  /*  3 dBm */   0x85,   /* characterized as 3.5 dBm */
  /*  2 dBm */   0x75,   /* characterized as 2.0 dBm */
  /*  1 dBm */   0x75,   /* characterized as 2.0 dBm */
  /*  0 dBm */   0x65,   /* characterized as 0.3 dBm */
  /* -1 dBm */   0x65,   /* characterized as 0.3 dBm */
  /* -2 dBm */   0x55,   /* characterized as -1.7 dBm */
  /* -3 dBm */   0x55,   /* characterized as -1.7 dBm */
  /* -4 dBm */   0x45,   /* characterized as -3.9 dBm */
  /* -5 dBm */   0x45,   /* characterized as -3.9 dBm */
  /* -6 dBm */   0x45,   /* characterized as -3.9 dBm */
  /* -7 dBm */   0x35,   /* characterized as -6.3 dBm*/
  /* -8 dBm */   0x25,   /* characterized as -7.8 dBm */
  /* -9 dBm */   0x25,   /* characterized as -7.8 dBm */
  /* -10 dBm */  0x15,   /* characterized as -9.8 dBm */
  /* -11 dBm */  0x15,   /* characterized as -9.8 dBm */
  /* -12 dBm */  0x05,   /* characterized as -11.8 dBm */
};
#endif

#if defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || \
  defined HAL_PA_LNA || defined HAL_PA_LNA_CC2590

/* TX power table array */
const uint8 CODE *const CODE macRadioDefsTxPwrTables[] =
{
#if defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || \
  (!defined HAL_PA_LNA && !defined HAL_PA_LNA_CC2590)
  macRadioDefsTxPwrBare,
#endif

#if defined HAL_PA_LNA || defined MAC_RUNTIME_CC2591
  macRadioDefsTxPwrCC2591,
#endif

#if defined HAL_PA_LNA_CC2590 || defined MAC_RUNTIME_CC2590
  macRadioDefsTxPwrCC2590,
#endif
};

/* RSSI offset adjustment value array */
const int8 CODE macRadioDefsRssiAdj[] =
{
#if defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || \
  (!defined HAL_PA_LNA && !defined HAL_PA_LNA_CC2590)
  0,
#endif

#if defined HAL_PA_LNA || defined MAC_RUNTIME_CC2591
  -9, /* high gain mode */
  4,  /* low gain mode */
#endif

#if defined HAL_PA_LNA_CC2590 || defined MAC_RUNTIME_CC2590
  -10, /* high gain mode */
  0,   /* low gain mode */
#endif
};

#endif /* defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || ... */

#if defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || \
  defined HAL_PA_LNA || defined HAL_PA_LNA_CC2590
uint8 macRadioDefsRefTableId = 0;
#endif

/* RF observable control register value to output PA signal */
#define RFC_OBS_CTRL_PA_PD_INV        0x68

/* RF observable control register value to output LNA signal */
#define RFC_OBS_CTRL_LNAMIX_PD_INV    0x6A

/* OBSSELn register value to select RF observable 0 */
#define OBSSEL_OBS_CTRL0             0xFB

/* OBSSELn register value to select RF observable 1 */
#define OBSSEL_OBS_CTRL1             0xFC


/**************************************************************************************************
 * @fn          MAC_SelectRadioRegTable
 *
 * @brief       Select radio register table in case multiple register tables are included
 *              in the build
 *
 * @param       txPwrTblIdx - TX power register value table index
 * @param       rssiAdjIdx - RSSI adjustment value index
 *
 * @return      none
 **************************************************************************************************
 */
extern void MAC_SetRadioRegTable ( uint8 txPwrTblIdx, uint8 rssiAdjIdx )
{
  /* Depending on compile flags, the parameters may not be used */
  (void) txPwrTblIdx;
  (void) rssiAdjIdx;

#if defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590
  if (txPwrTblIdx >= sizeof(macRadioDefsTxPwrTables)/sizeof(macRadioDefsTxPwrTables[0]))
  {
    txPwrTblIdx = 0;
  }
#endif /* defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 */

#if defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || \
  defined HAL_PA_LNA || defined HAL_PA_LNA_CC2590

  if (rssiAdjIdx >= sizeof(macRadioDefsRssiAdj)/sizeof(macRadioDefsRssiAdj[0]))
  {
    rssiAdjIdx = 0;
  }

  macRadioDefsRefTableId = (txPwrTblIdx << 4) | rssiAdjIdx;

#endif /* defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || .. */
}


/**************************************************************************************************
 * @fn          macRadioTurnOnPower
 *
 * @brief       Logic and sequence for powering up the radio.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macRadioTurnOnPower(void)
{
  /* Enable RF error trap */
  MAC_MCU_RFERR_ENABLE_INTERRUPT();

#if defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590
  /* table ID is referenced only when runtime configuratino is enabled */
  if (macRadioDefsRefTableId & 0xf0)
#endif /* defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 */

#if defined MAC_RUNTIME_CC2591 || defined MAC_RUNTIME_CC2590 || \
  defined HAL_PA_LNA || defined HAL_PA_LNA_CC2590
  { /* either if compound statement or non-conditional compound statement */

    /* (Re-)Configure PA and LNA control signals to RF frontend chips.
    * Note that The register values are not retained during sleep.
    */

    /* P1_1 -> PAEN */
    RFC_OBS_CTRL0 = RFC_OBS_CTRL_PA_PD_INV;
    OBSSEL1       = OBSSEL_OBS_CTRL0;

    /* P1_4 -> EN (LNA control) */
    RFC_OBS_CTRL1 = RFC_OBS_CTRL_LNAMIX_PD_INV;
    OBSSEL4       = OBSSEL_OBS_CTRL1;
  }
#endif /* defined MAC_RUNTIME_CC2591 || ... || defined HAL_PA_LNA_CC2590 */

  if (macChipVersion <= REV_B)
  {
    /* radio initializations for disappearing RAM; PG1.0 and before only */
    MAC_RADIO_SET_PAN_ID(macPib.panId);
    MAC_RADIO_SET_SHORT_ADDR(macPib.shortAddress);
    MAC_RADIO_SET_IEEE_ADDR(macPib.extendedAddress.addr.extAddr);
  }
}


/**************************************************************************************************
 * @fn          macRadioTurnOffPower
 *
 * @brief       Logic and sequence for powering down the radio.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
MAC_INTERNAL_API void macRadioTurnOffPower(void)
{
  /* Disable RF error trap */
  MAC_MCU_RFERR_DISABLE_INTERRUPT();
}


/**************************************************************************************************
 *                                  Compile Time Integrity Checks
 **************************************************************************************************
 */
#if (HAL_CPU_CLOCK_MHZ != 32)
#error "ERROR: The only tested/supported clock speed is 32 MHz."
#endif

#if (MAC_RADIO_RECEIVER_SENSITIVITY_DBM > MAC_SPEC_MIN_RECEIVER_SENSITIVITY)
#error "ERROR: Radio sensitivity does not meet specification."
#endif

#if defined (HAL_PA_LNA) && defined (HAL_PA_LNA_CC2590)
#error "ERROR: HAL_PA_LNA and HAL_PA_LNA_CC2590 cannot be both defined."
#endif

#if defined (HAL_PA_LNA) && defined (MAC_RUNTIME_CC2591)
#error "ERROR: HAL_PA_LNA and MAC_RUNTIME_CC2591 cannot be both defined."
#endif

#if defined (HAL_PA_LNA_CC2590) && defined (MAC_RUNTIME_CC2590)
#error "ERROR: HAL_PA_LNA_CC2590 and MAC_RUNTIME_CC2590 cannot be both defined."
#endif


/**************************************************************************************************
 */
