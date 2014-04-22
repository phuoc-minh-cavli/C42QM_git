/*! \file
*  \n
*  \brief  pm_pvc_config.c
*  \n
*  \n This file contains pmic configuration data specific for SPMI Controller's
      Peripheral for SDM845 device..
*  \n
*  \n &copy; Copyright 2015-2018 Qualcomm Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Settings/PMIC/loader/pm_pvc_config.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/18   hl     Updated to support MDM9205
04/09/18   sv     Updated to support QCS405
09/18/17   sv     Updated to support SDM670
02/01/17   aab    Supported SDM845 target.  Added SPMI,PVC, MGPI and VIOCTL configurations
08/25/15   vtw    Created.
========================================================================== */

/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/

#include "SpmiCfg.h"

/*-------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* ----------------------------------------------------------------------*/

/* Converts a PVC port index to an overall PMIC Arbiter port index */
#define PVC_PORT_TO_PMIC_ARB_PORT(p) (p+1)

/*   Port assignment in 405   */
#define APPS0_PORT      2   /* APPS Port 0  */
// #define APPS1_PORT      2   /* APPS Port 1  */


/*-------------------------------------------------------------------------
* Static Variable Definitions
* ----------------------------------------------------------------------*/

/*
 * pm_pvc_apps0_addr
 *
 * PVC port addr for APPS0 Gold cluster.
 */
SpmiCfg_Ppid pm_pvc_apps0_addr[] =
{
  {0x1, 0x4040}, /*PME9205, L1A_CTRL_VSET_LB */
  {0x1, 0x4041}, /*PME9205, L1A_CTRL_VSET_UB */
  {0x1, 0x4045}, /*PME9205, L1A_CTRL_MODE_CTL */
  {0x1, 0x4046}, /*PME9205, L1A_CTRL_EN_CTL */
};                                 

const SpmiCfg_PvcPortCfg pm_pvc_apps0_cfg = {APPS0_PORT, SPMI_ACCESS_PRIORITY_HIGH, pm_pvc_apps0_addr, sizeof(pm_pvc_apps0_addr)/sizeof(SpmiCfg_Ppid)};

/*
 * pm_pvc_apps1_addr
 *
 * PVC port addr for APPS1 Silver cluster block.
 */
// SpmiCfg_Ppid pm_pvc_apps1_addr[] =
// {
  // {0x1, 0x105}, /* PM660, dummy entry*/
  // {0x1, 0x105}, /* PM660, dummy entry*/
  // {0x1, 0x105}, /* PM660, dummy entry*/
  // {0x1, 0x105}, /* PM660, dummy entry*/
// };

// const SpmiCfg_PvcPortCfg pm_pvc_apps1_cfg = {APPS1_PORT, SPMI_ACCESS_PRIORITY_LOW, pm_pvc_apps1_addr, sizeof(pm_pvc_apps1_addr)/sizeof(SpmiCfg_Ppid)};


/*
 * pm_arb_pvc_cfg
 *
 * PMIC Arbiter PVC ports config.
 */
const SpmiCfg_PvcPortCfg* pm_arb_pvc_port_cfg[] = {&pm_pvc_apps0_cfg, };

uint32 pm_arb_pvc_port_cfg_size[] = {sizeof(pm_arb_pvc_port_cfg) / sizeof(SpmiCfg_PvcPortCfg*)};

