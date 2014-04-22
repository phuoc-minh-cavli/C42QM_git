/*
===========================================================================

FILE:         HALmpmmmmap.c

DESCRIPTION:  Mapping LPASS base address to use various HWIO registers.

              Copyright © 2012 Qualcomm Technologies Incorporated.
              All Rights Reserved.
              QUALCOMM Proprietary/GTDR
===========================================================================

$Header: //components/rel/core.tx/6.0/power/mpm/src/hal/HALmpmmmap.c#1 $

=============================================================================*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include "DALDeviceId.h"
#include "DDIHWIO.h"

/* ==================================================================
                     GLOBAL DATA DECLARATIONS
   ================================================================== */
/**
 * Generating base address to send to HWIO 
 */
uint8 *SS_BASE_MPM_PTR = NULL;
#define SSTR(x) STR(x)
#define STR(x) #x

/* -----------------------------------------------------------------------
**                         FUNCTIONS
** ----------------------------------------------------------------------- */

/*
 * @brief HAL_mpmint_hwio_mmap
 *
 * Maps the LPASS base physical address to virtual address. 
*/
void HAL_mpmint_hwio_mmap(void)
{
  static DalDeviceHandle *mpmDALHandle;
  DALResult eResult;

  eResult = DAL_DeviceAttach(DALDEVICEID_HWIO, &mpmDALHandle);
  if (eResult == DAL_SUCCESS)
  {
    DalHWIO_MapRegion(mpmDALHandle, SSTR(MPM_BASE_NAME), &SS_BASE_MPM_PTR);
  }
}
