#ifndef __PMIC_SBL_BOOT_TARGET_H__
#define __PMIC_SBL_BOOT_TARGET_H__

/*! \file pm_sbl_boot_target.h
*  \n
*  \brief This file contains PMIC device initialization function and globals declaration.
*  \n
*  \n &copy; Copyright 2016-2018 Qualcomm Technologies Inc, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/08/18   sv      Updated the configuration of WCSS Retention voltage. (FR45484)
01/12/17   as      L5 retention and active voltage rsgister has been swapped (CR1110540)
02/05/16   aab     Added active/retentsion level config reg address
02/05/16   aab     Creation
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "com_dtypes.h"
#include "pm_err_flags.h"



/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

#define PM_SCRATCH_SLAVE_ADDR 1
#define S5_BASE_ADDRESS  0x2000
#define S2_BASE_ADDRESS  0x1700
#define S5_SCRATCH_RET_ADDR_UB                      (S5_BASE_ADDRESS + 0x7F)
#define S2_SCRATCH_RET_ADDR_LB                      (S2_BASE_ADDRESS + 0x7F)

  //LDO01_MISC1 | 0x00014086   as CX_NOM_LB
  //LDO04_MISC1 | 0x00014386   as CX_NOM_UB
  //MISC_MISC_SPARE_PERPH_RB 0x95A   as MX_NOM_LB
  //MISC_MISC_SPARE_XVDD_RB  0x95C   as MX_NOM_UB

#define PM_LDO_MISC_SLAVE_ID 1
#define PM_MISC_SLAVE_ID 0

#define LDO01_MISC1_ADDR               0x4086
#define LDO04_MISC1_ADDR               0x4386
#define MISC_MISC_SPARE_PERPH_RB_ADDR  0x095A
#define MISC_MISC_SPARE_XVDD_RB_ADDR   0x095C

/*===========================================================================

                        FUNCTION PROTOTYPES 

===========================================================================*/
pm_err_flag_type 
pm_check_pbs_ram_warm_reset_seq_presence(uint32 device_index);

#endif //__PMIC_SBL_BOOT_TARGET_H__
