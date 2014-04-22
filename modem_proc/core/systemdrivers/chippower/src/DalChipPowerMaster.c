/*============================================================================

FILE: DalChipPowerMaster.c

DESCRIPTION:
  This file contains the master code for resetting and powering off the device.

PUBLIC CLASSES:
  ChipPower_Reset
  ChipPower_PowerOff

===============================================================================
  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR

  $Header: //components/rel/core.mpss/3.10/systemdrivers/chippower/src/DalChipPowerMaster.c#2 $
  $DateTime: 2019/03/25 08:08:19 $
  $Author: pwbldsvc $
=============================================================================*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "DalChipPower.h"

#include "dog.h"
#include "pm_pon.h"
#include "err.h"

#include "HALhwio.h"
#include "msmhwiobase.h"


/*=========================================================================
     HWIO 
==========================================================================*/

/*----------------------------------------------------------------------------
 * MODULE: MPM2_PSHOLD
 *--------------------------------------------------------------------------*/

#define MPM2_PSHOLD_REG_BASE                       (MPM2_MPM_BASE      + 0x0000b000)

#define HWIO_MPM2_MPM_PS_HOLD_ADDR                 (MPM2_PSHOLD_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_PS_HOLD_RMSK                        0x1
#define HWIO_MPM2_MPM_PS_HOLD_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PS_HOLD_ADDR, HWIO_MPM2_MPM_PS_HOLD_RMSK)
#define HWIO_MPM2_MPM_PS_HOLD_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PS_HOLD_ADDR, m)
#define HWIO_MPM2_MPM_PS_HOLD_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PS_HOLD_ADDR,v)
#define HWIO_MPM2_MPM_PS_HOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PS_HOLD_ADDR,m,v,HWIO_MPM2_MPM_PS_HOLD_IN)
#define HWIO_MPM2_MPM_PS_HOLD_PSHOLD_BMSK                 0x1
#define HWIO_MPM2_MPM_PS_HOLD_PSHOLD_SHFT                 0x0


/*=========================================================================
      Functions
==========================================================================*/

/*==========================================================================

  FUNCTION      ChipPower_Reset

  DESCRIPTION   Resets the chip.

  PARAMETERS
    h        - DAL device handle.
    nNotUsed - Not Used.

  DEPENDENCIES  None.

  RETURN VALUE  Does not return.

  SIDE EFFECTS  None.

==========================================================================*/
void ChipPower_Reset
(
  DalDeviceHandle *h,
  uint32           nNotUsed
)
{
  ChipPowerDrvCtxt *pDrvCtxt;

  pDrvCtxt = ((ChipPowerClientCtxt *)h->pClientCtxt)->
               pChipPowerDevCtxt->pChipPowerDrvCtxt;

  ChipPower_PerformCallbacks(pDrvCtxt, DALCHIPPOWER_EVENT_RESET);
  pm_app_pon_pshold_cfg(PM_APP_PON_CFG_HARD_RESET);
  ChipPower_PrepareForPowerEvent();

  HWIO_OUTF(MPM2_MPM_PS_HOLD, PSHOLD, 0);

  /* Wait for the chip to reset. */
  while (1)
  {
    dog_kick();
  }

} /* END ChipPower_Reset */


/*==========================================================================

  FUNCTION      ChipPower_PowerOff

  DESCRIPTION   Powers off the chip.

  PARAMETERS
    h        - DAL device handle.
    nNotUsed - Not Used.

  DEPENDENCIES  None.

  RETURN VALUE  Does not return.

  SIDE EFFECTS  None.

==========================================================================*/
void ChipPower_PowerOff
(
  DalDeviceHandle *h,
  uint32           nNotUsed
)
{
  ChipPowerDrvCtxt *pDrvCtxt;

 pDrvCtxt = ((ChipPowerClientCtxt *)h->pClientCtxt)->
              pChipPowerDevCtxt->pChipPowerDrvCtxt;

  ChipPower_PerformCallbacks(pDrvCtxt, DALCHIPPOWER_EVENT_POWEROFF);

  /*-----------------------------------------------------------------------*/
  /* Configure the PMIC for shutdown.                                      */
  /*-----------------------------------------------------------------------*/

  pm_app_pon_pshold_cfg(PM_APP_PON_CFG_NORMAL_SHUTDOWN);

  /* Must come after PMIC operations since it may disable the SPMI bus. */
  ChipPower_PrepareForPowerEvent();

  /*-----------------------------------------------------------------------*/
  /* Turn off PS-HOLD which feeds into the voltage regulator.  Power will  */
  /* turn off in ~50 ms when the capacitor drains.                         */
  /*-----------------------------------------------------------------------*/

  HWIO_OUTF(MPM2_MPM_PS_HOLD, PSHOLD, 0);

  /* Wait for the chip to power off. */
  while (1)
  {
    dog_kick();
  }

} /* END ChipPower_PowerOff */
