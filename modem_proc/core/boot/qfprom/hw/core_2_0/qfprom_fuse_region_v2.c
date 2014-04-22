/*===========================================================================

                       QFPROM  Driver Source  Code

DESCRIPTION
 Contains target specific defintions and APIs to be used to read and write
 qfprom values for sec ctrl 3.0 (used by 9x07).

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright  2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/boot/qfprom/hw/core_2_0/qfprom_fuse_region_v2.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/13/15   lm      Initial revision for MDM9x07

============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include QFPROM_HWIO_REG_INCLUDE_H
#include QFPROM_TARGET_INCLUDE_H

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
/*
**  Array containing QFPROM data items that can be read and associated
**  registers, mask and shift values for the same.
*/
const QFPROM_REGION_INFO qfprom_region_data[] =
{
  {
    QFPROM_ANTI_ROLLBACK_4_REGION,
    1,
    QFPROM_FEC_NONE,
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_ADDR ,
    HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_ADDR ,
    HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_4_BMSK,
    HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_4_BMSK,
    QFPROM_ROW_LSB,
    TRUE,
    14
  },

 {
    QFPROM_OEM_SPARE_REG30_REGION, 
    1,
    QFPROM_FEC_NONE,
    HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_ADDR(30),
    HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_ADDR(30),
    HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG30_BMSK,
    HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG30_BMSK,
    QFPROM_ROW_MSB,
    TRUE,
    30
  },

  /* Add above this entry */
  {
    QFPROM_LAST_REGION_DUMMY,
    0,
    QFPROM_FEC_NONE,
    0,
    0,
    0,
    0,
    QFPROM_ROW_LSB
  }
};
