/*! \file
*  \n
*  \brief  pm_spmi_config.c
*  \n
*  \n This file contains pmic configuration data specific for SPMI Controller's
      Peripheral for MSM8996 device..
*  \n
*  \n &copy; Copyright 2015-2019 QUALCOMM Technologies, Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Settings/PMIC/loader/pm_spmi_config.c#10 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/16/17   aab     Created to support SDM845
========================================================================== */

/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/

#include "pm_device.h"
#include "SpmiCfg.h"

/*-------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
* Static Variable Definitions
* ----------------------------------------------------------------------*/

/* PMIC MMU/SMMU configuration table. */

SpmiCfg_ChannelCfg pm_spmi_channel_cfg [] =
{
  /* PME9205 */
  {2, 0x08, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* PON_OPTIONS */
  {1, 0xBF, 0, PM_APPS_HLOS_OWNER, PM_APPS_HLOS_OWNER    }, /* PWM4 */
  {1, 0xBE, 0, PM_APPS_HLOS_OWNER, PM_APPS_HLOS_OWNER    }, /* PWM3 */
  {1, 0xBD, 0, PM_APPS_HLOS_OWNER, PM_APPS_HLOS_OWNER    }, /* PWM2 */
  {1, 0xBC, 0, PM_APPS_HLOS_OWNER, PM_APPS_HLOS_OWNER    }, /* PWM1 */
  {1, 0x46, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* LDO07 */
  {1, 0x45, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* LDO06 */
  {1, 0x44, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* LDO05 */
  {1, 0x43, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* LDO04 */
  {1, 0x42, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* LDO03 */
  {1, 0x41, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* LDO02 */
  {1, 0x40, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* LDO01 */
  {1, 0x19, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* S2_FREQ */
  {1, 0x18, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* S2_PS */
  {1, 0x17, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* S2_CTRL */
  {1, 0x16, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* S1_FREQ */
  {1, 0x15, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* S1_PS */
  {1, 0x14, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* S1_CTRL */
  {1, 0x10, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* BUCK_CMN_MOD */
  {1, 0x01, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* REVID1 */
  {0, 0xC3, 0, SPMI_OPEN_OWNER, PM_APPS_HLOS_OWNER    }, /* GPIO04 */
  {0, 0xC2, 0, SPMI_OPEN_OWNER, PM_APPS_HLOS_OWNER    }, /* GPIO03 */
  {0, 0xC1, 0, SPMI_OPEN_OWNER, PM_APPS_HLOS_OWNER    }, /* GPIO02 */
  {0, 0xC0, 0, PM_APPS_HLOS_OWNER, PM_APPS_HLOS_OWNER    }, /* GPIO01 */
  {0, 0x74, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* MISC_PBS2 */
  {0, 0x73, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* MISC_PBS1 */
  {0, 0x72, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* PBS_CLIENT1 */
  {0, 0x71, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* PBS_CLIENT0 */
  {0, 0x70, 0, PM_APPS_TZ_OWNER, PM_APPS_TZ_OWNER    }, /* PBS_CORE */
  {0, 0x62, 0, PM_APPS_HLOS_OWNER, PM_APPS_HLOS_OWNER    }, /* RTC_TIMER */
  {0, 0x61, 0, PM_MSS_OWNER, PM_MSS_OWNER    }, /* RTC_ALARM */
  {0, 0x60, 0, PM_APPS_TZ_OWNER, PM_APPS_TZ_OWNER    }, /* RTC_RW */
  {0, 0x5F, 0, PM_MSS_OWNER, PM_MSS_OWNER    }, /* CALRC3 */
  {0, 0x5E, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* LFRC */
  {0, 0x5B, 0, PM_APPS_HLOS_OWNER, PM_APPS_HLOS_OWNER    }, /* DIV_CLK1 */
  {0, 0x5A, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* SLEEP_CLK1 */
  {0, 0x59, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* CLK_DIST */
  {0, 0x54, 0, PM_MSS_OWNER, PM_MSS_OWNER    }, /* RF_CLK1 */
  {0, 0x51, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* LN_BB_CLK */
  {0, 0x50, 0, PM_MSS_OWNER, PM_MSS_OWNER    }, /* XO */
  {0, 0x3F, 0, PM_MSS_OWNER, PM_MSS_OWNER    }, /* BCL_COMP_LITE */
  {0, 0x3D, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* VREG_XVDD */
  {0, 0x3C, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* VDDGEN */
  {0, 0x39, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* VADC_HM10_CMN */
  {0, 0x37, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* VADC_HM4_CAL */
  {0, 0x36, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* VADC_HM3_PBS */
  {0, 0x32, 0, PM_MSS_OWNER, PM_MSS_OWNER    }, /* VADC_HM2_MDM */
  {0, 0x31, 0, PM_APPS_HLOS_OWNER, PM_APPS_HLOS_OWNER    }, /* VADC_HM1_USR */
  {0, 0x2C, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* MBG1 */
  {0, 0x24, 0, PM_APPS_HLOS_OWNER, PM_APPS_HLOS_OWNER    }, /* TEMP_ALARM */
  {0, 0x0A, 0, PM_MSS_OWNER, PM_MSS_OWNER    }, /* AON_LPI */
  {0, 0x09, 0, PM_MSS_OWNER, PM_MSS_OWNER    }, /* MISC */
  {0, 0x08, 0, PM_APPS_HLOS_OWNER, PM_APPS_HLOS_OWNER    }, /* PON */
  {0, 0x06, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* SPMI */
  {0, 0x05, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* INT */
  {0, 0x04, 0, SPMI_OPEN_OWNER, PM_RPM_OWNER    }, /* BUS */
  {0, 0x01, 0, PM_RPM_OWNER, PM_RPM_OWNER    }, /* REVID */

};

/* Number of spmi channels config entries. . */
uint32 pm_spmi_channel_cfg_size[] = {sizeof(pm_spmi_channel_cfg) / sizeof(SpmiCfg_ChannelCfg)};

