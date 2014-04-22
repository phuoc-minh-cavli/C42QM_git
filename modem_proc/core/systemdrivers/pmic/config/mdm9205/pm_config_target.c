/*! \file
 *
 *  \brief  pm_config_target.c ----This file contains customizable target specific driver settings & PMIC registers.
 *  \details This file contains customizable target specific
 * driver settings & PMIC registers. This file is generated from database functional
 * configuration information that is maintained for each of the targets.
 *
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Resource Setting Information Version: VU.Please Provide Valid Label - Not Approved
 *    PMIC code generation Software Register Information Version: VU.Please Provide Valid Label - Not Approved
 *    PMIC code generation Processor Allocation Information Version: VU.Please Provide Valid Label - Not Approved
 *    This file contains code for Target specific settings and modes.
 *
 *  &copy; Copyright 2013, 2019 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/config/mdm9205/pm_config_target.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/22/15   ps      Added support for 9x07 target (CR-878990)
12/12/14   mr      Resolved issue of .scons file for MSM8909. (CR-769500)
12/05/14   mr      Added Dual PMIC support for MSM8909. (CR-764010)
10/28/14   akt     PBS client3 is used for BUA in 8909 (CR-736180).
10/22/13   rk      GPIO1 used for BUA in 8916.
10/03/13   rk      Changes required for new ULT BUCKs and LDOs
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/

#include "pm_target_information.h"
#include "pm_npa.h"

#include "pm_mpp_driver.h"
#include "pm_xo_driver.h"
#include "pm_pbs_client.h"
#include "pm_uicc_app.h"
#include "pm_pwr_alg.h"


/*===========================================================================

                    TARGET SPECIFIC DATA DEFINITIONS

===========================================================================*/

/* PMD9x07 Info for VIRTIO/RUMI only */
uint32  pm_model_info[][3] =
{
    /* PMIC_Model, AllLayer_Rev, MetalLayer_Rev */
    {          36,            2,              0 },    /* PMIC_IS_PME9205 */
    {           0,            0,              0 }     /* Array terminator */
};

/* PMD9x07, PMxxxx */
uint32  num_of_smps[]   = {  2, 0 };
uint32  num_of_ldo[]    = {  7, 0 };
uint32  num_of_clkbuf[] = { 13, 0 };
uint32  num_of_gpio[]   = {  4, 0 };
uint32  num_of_mpp[]    = {  0, 0 };
uint32  num_of_rtc[]    = {  1, 0 };
uint32  num_of_talm[]   = {  1, 0 };
uint32  num_of_megaxo[] = {  1, 0 };
uint32  num_of_boost[]  = {  0, 0 };
uint32  num_of_vs[]     = {  0, 0 };

pm_xo_core_specific_info_type xocore_specific[1] =
{
    {0x7FF, 0x3F, 0x3F, 0xFF}
};

/* we don't need this uicc_specific data for MDM9205 as UICC is always on ,and using PM_GPIO_44 as a invalid GPIO */
pm_uicc_specific_info_type uicc_specific[1] =
{
    {PM_GPIO_44, PM_PBS_CLIENT_INVALID, PM_LDO_INVALID, PM_LDO_INVALID, PM_LDO_INVALID, PM_LDO_INVALID}
};
