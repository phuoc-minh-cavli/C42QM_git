/*
===========================================================================

FILE:         BIMC_SCMO_v1.c

DESCRIPTION:  
  This is the platform hardware abstraction layer implementation for 
  BIMC SCMO.

Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.3/core/boot/ddr/hw/mdm9x05/controller/BIMC_SCMO_v1.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
20/12/18   ds      Added support for 32 MB SCMO size mask. 
02/09/14   tw      added support for common ddr library
12/03/12   tw      Fixed incorrect SCMO configuration
09/06/12   sl      Removed ddr_target_param.h.
08/24/12   sl      Updated HAL_SDRAM_SCMO_Init().
05/15/12   tw      Added API for rank disable
02/02/12   tw      Initial revision
===========================================================================
*/


/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_params.h"
#include "ddr_target.h"
#include "BIMC.h"
#include "HAL_DDR.h"

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
/* ============================================================================
**  Function : HAL_SDRAM_SCMO_Rank_Disable
** ============================================================================
*/
/*!
*   @brief
*   This function will initialize the disable the rank given in the parameter
*
*   @details
*   This function will initialize the disable the rank given in the parameter
*
*   @param interface   -  [IN] interface to disable the rank for
*   @param chip_select      -  [IN] chipselect for rank disable
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   Any new traffic going to the rank disabled will be blocked and generate an error
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SCMO_Rank_Disable(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  uint32 offset;

  /* Configure for correct offset */
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);

  /* Set SCMO_ADDR_MAP_CSn[RANK_EN] = 0 for the ranks which needs to be disabled */
  if(chip_select & SDRAM_CS0)
  {
    BOOT_HWIO_OUTMI(DDR_ADDR(SCMO_ADDR_MAP_CSn),
        offset,
        0,
        HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MAP_CSn), RANK_EN),
        0);
  }
  if(chip_select & SDRAM_CS1)
  {
    BOOT_HWIO_OUTMI(DDR_ADDR(SCMO_ADDR_MAP_CSn),
        offset,
        1,
        HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MAP_CSn), RANK_EN),
        0);
  }
  /* Flush all buffers */
  BOOT_HWIO_OUTM(DDR_ADDR(SCMO_FLUSH_CMD),
                 offset,
                 HWIO_FMSK(DDR_ADDR(SCMO_FLUSH_CMD), FLUSH_ALL_BUF),
                 0x1 << HWIO_SHFT(DDR_ADDR(SCMO_FLUSH_CMD), FLUSH_ALL_BUF));
  while(BOOT_HWIO_INM(DDR_ADDR(SCMO_FLUSH_CMD), offset,
                      HWIO_FMSK(DDR_ADDR(SCMO_FLUSH_CMD), FLUSH_ALL_BUF)))
  {}

  /* Wait for ranks to become idle */
  while(BOOT_HWIO_INM(DDR_ADDR(SCMO_SLV_STATUS), offset,
                      HWIO_FMSK(DDR_ADDR(SCMO_SLV_STATUS), RANK_IDLE)) == 0)
  {}
}

/* ============================================================================
**  Function : HAL_SDRAM_SCMO_Rank_Enable
** ============================================================================
*/
/*!
*   @brief
*   This function will initialize the enable the rank given in the parameter
*
*   @details
*   This function will initialize the enable the rank given in the parameter
*
*   @param interface   -  [IN] interface to enable the rank for
*   @param chip_select      -  [IN] chipselect for rank enable
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SCMO_Rank_Enable(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  uint32 offset;

  /* Configure for correct offset */
  offset = base + ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);
  
  /* Set SCMO_ADDR_MAP_CSn[RANK_EN] = 0 for the ranks which needs to be disabled */
  if(chip_select & SDRAM_CS0)
  {
    BOOT_HWIO_OUTMI(DDR_ADDR(SCMO_ADDR_MAP_CSn),
        offset,
        0,
        HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MAP_CSn), RANK_EN),
        1 << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_MAP_CSn), RANK_EN));
  }
  if(chip_select & SDRAM_CS1)
  {
    BOOT_HWIO_OUTMI(DDR_ADDR(SCMO_ADDR_MAP_CSn),
        offset,
        1,
        HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MAP_CSn), RANK_EN),
        1 << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_MAP_CSn), RANK_EN));
  }
}
