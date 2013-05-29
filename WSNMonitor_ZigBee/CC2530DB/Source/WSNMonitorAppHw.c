/**************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include "ZComDef.h"
#include "hal_mcu.h"
#include "hal_defs.h"

#include "WSNMonitorAppHw.h"


/**************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/
/* NOTES:   Jumper should be between Header P18 pin 7 and header P18 pin 9 on the SmartRF05
 *          P0_2 - P18 pin 7
 *          P0_3 - P18 pin 9
 */
#define JUMPERIN_BIT  BV(2)  //P0.2
#define JUMPERIN_SEL  P0SEL
#define JUMPERIN_DIR  P0DIR
#define JUMPERIN      P0

#define JUMPEROUT_BIT BV(3)  //P0.3
#define JUMPEROUT_SEL P0SEL
#define JUMPEROUT_DIR P0DIR
#define JUMPEROUT     P0


/**************************************************************************************************
 * @fn      readCoordinatorJumper
 *
 * @brief   Checks for a jumper to determine if the device should
 *          become a coordinator
 *
 * @return  TRUE if the jumper is there, FALSE if not
 **************************************************************************************************/
uint8 readCoordinatorJumper( void )
{

  uint8 jumpered = TRUE;
  uint8 result;
  uint8 x,y;

  JUMPERIN_SEL  &= ~(JUMPERIN_BIT);
  JUMPEROUT_SEL &= ~(JUMPEROUT_BIT);
  JUMPERIN_DIR  &= ~(JUMPERIN_BIT);
  JUMPEROUT_DIR |= JUMPEROUT_BIT;

  jumpered = TRUE;

  for ( x = 0; x < 8; x++ )
  {
    if ( x & 0x01 )
    {
      JUMPEROUT |= JUMPEROUT_BIT;
      for ( y = 0; y < 8; y++ );
      result = JUMPERIN & JUMPERIN_BIT;
      if ( result != JUMPERIN_BIT )
        jumpered = FALSE;
    }
    else
    {
      JUMPEROUT &= ~(JUMPEROUT_BIT);
      for ( y = 0; y < 8; y++ );
      result = JUMPERIN & JUMPERIN_BIT;
      if ( result != 0x00 )
        jumpered = FALSE;
    }
  }

  return ( jumpered );

}

/**************************************************************************************************
**************************************************************************************************/
