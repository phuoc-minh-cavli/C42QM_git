/*! \file
*  \n
*  \brief  pm_mpm_volt_rail_cmd_cfg.c
*  \n
*  \n This file contains pmic configuration data specific for PM8994 mpm shutdown/wakeup
*  \n voltage rail cmds.
*  \n
*  \n &copy; Copyright 2014-2015 Qualcomm Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.3/core/systemdrivers/pmic/config/mdm9x05/pm_mpm_volt_rail_cmd_cfg.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/02/16   as      MX parents config has been changed to 1.225V (CR-1043784)
10/01/15   sv      Updated mpm sleep config settings. (CR-902375)
08/01/15   ps      Added support for 9x07 target (CR-883336)
03/16/15   akt     Update the S3 retension for any MX voltage (CR-804591)
01/28.15   akt     Update the retension voltage for S3 and S4 (CR-779395)
12/12/14   akt     Update the PBS trigger to PBS_CLIENT4 in 8909, MX parent
02/12/14   vtw     Updated with 8994 MPM sequence settings.
10/31/13   vtw     Created.
========================================================================== */

/*=========================================================================
      Include Files
==========================================================================*/

#include "pm_mpm.h"
#include "pm_mpm_internal.h"
#include "pm_mpm_target.h"

/*===========================================================================
Data Declarations
===========================================================================*/

/**
  * MPM PMIC sleep register settings, set dynamically by railway driver.
  */
pm_mpm_cfg_type
pm_mpm_sleep_cfg[] =
{
  [PM_MPM_SLEEP_CMD_CX_VSET_LB]        = {0x1, 0x04040, 0x0, 0x0}, /* Set VDD_CX (L1A) retention. */
  [PM_MPM_SLEEP_CMD_CX_VSET_UB]        = {0x1, 0x04041, 0x0, 0x0}, /* Set VDD_CX (L1A)retention. */
  [PM_MPM_SLEEP_CMD_CX_MODE]           = {0x1, 0x04045, 0x3, 0x0},  /* set CX (L1A) mode to RM_NPM  . */
  [PM_MPM_SLEEP_CMD_MX_VSET_LB]        = {0x1, 0x04340, 0x0, 0x0}, /* Set VDD_MX (L4A) retention */
  [PM_MPM_SLEEP_CMD_MX_VSET_UB]        = {0x1, 0x04341, 0x0, 0x0}, /* Set VDD_MX (L4A)retention */
  [PM_MPM_SLEEP_CMD_MX_MODE]           = {0x1, 0x04345, 0x3, 0x0},  /* set MX (L4A) mode to RM_NPM */
  [PM_MPM_SLEEP_CMD_PBS_TRIG_ARG]    = {0x0, 0x07250, 0x1, 0x0}, /* Set PBS sleep trigger argument */
  [PM_MPM_SLEEP_CMD_PBS_TRIG]        = {0x0, 0x07242, 0x80, 0x6}, /* Program PBS SW trigger. */      
  [PM_MPM_SLEEP_CMD_END]             = {0x0, 0x0,     0x0, 0x0}, /* End of sleep sequence. */
  
  
};

/**
  * MPM PMIC active register settings, set dynamically by railway driver.
  */
pm_mpm_cfg_type
pm_mpm_active_cfg[] =
{
  [PM_MPM_ACTIVE_CMD_PBS_TRIG_ARG]      = {0x0, 0x07250, 0x2, 0x0},  /* Set PBS wake trigger argument */
  [PM_MPM_ACTIVE_CMD_PBS_TRIG]          = {0x0, 0x07242, 0x80, 0x6},  /* Program PBS SW trigger. */     
  [PM_MPM_ACTIVE_CMD_MX_MODE]           = {0x1, 0x04345, 0x7, 0x0},  /* Restore MX (L4A) to mode NPM. */
  [PM_MPM_ACTIVE_CMD_MX_VSET_LB]        = {0x1, 0x04340, 0x0, 0x0},  /* Turn on MX (L4A) with active voltage.  */
  [PM_MPM_ACTIVE_CMD_MX_VSET_UB]        = {0x1, 0x04341, 0x0, 0x0},  /* Turn on MX (L4A) with active voltage.  */
  [PM_MPM_ACTIVE_CMD_CX_MODE]           = {0x1, 0x04045, 0x7, 0x0},  /* Restore CX (L1A) to mode NPM. */
  [PM_MPM_ACTIVE_CMD_CX_VSET_LB]        = {0x1, 0x04040, 0x0, 0x0},  /* Turn on CX (L1A) with active voltage.  */
  [PM_MPM_ACTIVE_CMD_CX_VSET_UB]        = {0x1, 0x04041, 0x0, 0x0},  /* Turn on CX (L1A) with active voltage.  */
  [PM_MPM_ACTIVE_CMD_END]               = {0x0, 0x0,     0x0, 0x0},  /* End active sequence. */  
  
};

/**
  * MPM cmd index buffer.
  */
const pm_mpm_cmd_index_type pm_mpm_cmd_index[] =
{
                               /* rail retention index, rail active index, rail sw mode before sleep */
  [PM_MPM_CX_INDEX]           = {PM_MPM_SLEEP_CMD_CX_VSET_LB, PM_MPM_ACTIVE_CMD_CX_VSET_LB, PM_MPM_ACTIVE_CMD_CX_MODE},
  [PM_MPM_MX_INDEX]           = {PM_MPM_SLEEP_CMD_MX_VSET_LB, PM_MPM_ACTIVE_CMD_MX_VSET_LB, PM_MPM_ACTIVE_CMD_MX_MODE},
  [PM_MPM_CX_MX_PARENT_INDEX] = {PM_MPM_INVALID_CMD, PM_MPM_INVALID_CMD, PM_MPM_INVALID_CMD},
};

/**
  * MPM rails info.
  */
pm_mpm_rails_info_type pm_mpm_rails_info[] =
{
                               /* resource_type, resource_index, sleep_headroom_uv */
  [PM_MPM_CX_INDEX]           = {RPM_LDO_A_REQ, 1, 0}, // CX is L1A
  [PM_MPM_MX_INDEX]           = {RPM_LDO_A_REQ, 4, 0}, // MX is L4A
  [PM_MPM_CX_MX_PARENT_INDEX] = {RPM_SMPS_A_REQ, 1, 0}, // Cx_MX parent is S1A
  
};
