/*! \file  pm_config_pam.c
 *  
 *  \brief  File Contains the PMIC NPA PAM Data
 *  \details  This file contains the PAM data for the PMIC NPA layer.
 *  
 *    Platform: QCS405.XX.1.0
 *    Revision: NO REVISIONS (Working Set)
 *    Template Version: 
 *
 *    $Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Settings/PMIC/core/pm_config_pam.c#1 $ 
 *    $DateTime: 2018/08/08 05:31:55 $  $Author: pwbldsvc $
 *  
 *  &copy; Copyright April 9, 2018 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        INCLUDE HEADER FILES

===========================================================================*/

#include "pm_npa_device_clk_buff.h"
#include "pm_npa_device_ldo.h"
#include "pm_npa_device_smps.h"
#include "pm_npa.h"
#include "pm_appsbl_proc_npa.h"
#include "pm_appsbl_proc_npa_device.h"
#include "pmapp_npa.h"

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/

#define PMIC_NPA_CLIENT_NODE_DISP_EXT_DP "/node/pmic/client/disp_ext_dp"
#define PMIC_NPA_CLIENT_NODE_DISP_PRIM "/node/pmic/client/disp_prim"
#define PMIC_NPA_CLIENT_NODE_USB_SS1 "/node/pmic/client/usb_ss1"
#define PMIC_NPA_CLIENT_NODE_USB_SS2 "/node/pmic/client/usb_ss2"
/*===========================================================================

                        VARIABLES DEFINITION

===========================================================================*/

/* DISP_EXT_DP Client */
pm_npa_ldo_kvps
pm_pam_disp_ext_dp_a_ldo4 [] =
{
   // Mode: PMIC_NPA_MODE_ID_GENERIC_STANDBY
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 0, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_GENERIC_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 1200000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
};
pm_npa_ldo_kvps
pm_pam_disp_ext_dp_a_ldo5 [] =
{
   // Mode: PMIC_NPA_MODE_ID_GENERIC_STANDBY
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 0, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_GENERIC_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 1200000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
};
pm_npa_pam_client_cfg_type
pm_pam_disp_ext_dp_rails_info [] =
{
   {
      (void*)pm_pam_disp_ext_dp_a_ldo4,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_disp_ext_dp_a_ldo5,
      PM_NPA_VREG_LDO
   },
};

npa_node_dependency
pm_client_dev_deps_disp_ext_dp [] =
{
      PMIC_NPA_NODE_DEP_LIST( A,  ldo, 4 ),
      PMIC_NPA_NODE_DEP_LIST( A,  ldo, 5 ),
};

/* DISP_PRIM Client */
pm_npa_ldo_kvps
pm_pam_disp_prim_a_ldo4 [] =
{
   // Mode: PMIC_NPA_MODE_ID_GENERIC_STANDBY
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 0, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_GENERIC_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 1200000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
};
pm_npa_ldo_kvps
pm_pam_disp_prim_a_ldo5 [] =
{
   // Mode: PMIC_NPA_MODE_ID_GENERIC_STANDBY
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 0, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_GENERIC_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
};
pm_npa_pam_client_cfg_type
pm_pam_disp_prim_rails_info [] =
{
   {
      (void*)pm_pam_disp_prim_a_ldo4,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_disp_prim_a_ldo5,
      PM_NPA_VREG_LDO
   },
};

npa_node_dependency
pm_client_dev_deps_disp_prim [] =
{
      PMIC_NPA_NODE_DEP_LIST( A,  ldo, 4 ),
      PMIC_NPA_NODE_DEP_LIST( A,  ldo, 5 ),
};

/* USB_SS1 Client */
pm_npa_ldo_kvps
pm_pam_usb_ss1_a_ldo3 [] =
{
   // Mode: PMIC_NPA_MODE_ID_USB_OFF
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 0, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_PERPH_SUSPEND
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 0, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_PERPH_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 1052000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_HOST_SUSPEND
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 0, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_HOST_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 1052000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
};
pm_npa_ldo_kvps
pm_pam_usb_ss1_a_ldo5 [] =
{
   // Mode: PMIC_NPA_MODE_ID_USB_OFF
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 0, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_PERPH_SUSPEND
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 0, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_PERPH_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_HOST_SUSPEND
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 0, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_HOST_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
};
pm_npa_ldo_kvps
pm_pam_usb_ss1_a_ldo12 [] =
{
   // Mode: PMIC_NPA_MODE_ID_USB_OFF
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 0, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_PERPH_SUSPEND
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 0, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_PERPH_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 3300000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_HOST_SUSPEND
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 0, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_HOST_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 3300000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
};
pm_npa_pam_client_cfg_type
pm_pam_usb_ss1_rails_info [] =
{
   {
      (void*)pm_pam_usb_ss1_a_ldo3,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_usb_ss1_a_ldo5,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_usb_ss1_a_ldo12,
      PM_NPA_VREG_LDO
   },
};

npa_node_dependency
pm_client_dev_deps_usb_ss1 [] =
{
      PMIC_NPA_NODE_DEP_LIST( A,  ldo, 3 ),
      PMIC_NPA_NODE_DEP_LIST( A,  ldo, 5 ),
      PMIC_NPA_NODE_DEP_LIST( A,  ldo, 12 ),
};

/* USB_SS2 Client */
pm_npa_ldo_kvps
pm_pam_usb_ss2_a_ldo3 [] =
{
   // Mode: PMIC_NPA_MODE_ID_USB_OFF
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 0, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_PERPH_SUSPEND
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_PERPH_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 1052000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_HOST_SUSPEND
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 1052000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_HOST_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 1052000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
};
pm_npa_ldo_kvps
pm_pam_usb_ss2_a_ldo5 [] =
{
   // Mode: PMIC_NPA_MODE_ID_USB_OFF
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 0, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_PERPH_SUSPEND
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 1200000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_PERPH_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_HOST_SUSPEND
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_HOST_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
};
pm_npa_ldo_kvps
pm_pam_usb_ss2_a_ldo12 [] =
{
   // Mode: PMIC_NPA_MODE_ID_USB_OFF
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 0, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_PERPH_SUSPEND
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 3300000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_PERPH_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 3300000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_HOST_SUSPEND
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 3300000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
   // Mode: PMIC_NPA_MODE_ID_USB_HOST_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list. */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list. */
      PM_NPA_KEY_MICRO_VOLT, 4, 3300000, /**< [X uV] -> max aggregation. */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> summed aggregation. */
      PM_NPA_KEY_HEAD_ROOM, 4, 0,  /**< [X uV] -> voltage headroom needed. */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_ACTIVE_FLOOR, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_CORNER_VOLT_LEVEL, 4, 0,  /**< Corner voltage */
      PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR, 4, 0,  /**< Corner Voltage Floor */
   },
};
pm_npa_pam_client_cfg_type
pm_pam_usb_ss2_rails_info [] =
{
   {
      (void*)pm_pam_usb_ss2_a_ldo3,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_usb_ss2_a_ldo5,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_usb_ss2_a_ldo12,
      PM_NPA_VREG_LDO
   },
};

npa_node_dependency
pm_client_dev_deps_usb_ss2 [] =
{
      PMIC_NPA_NODE_DEP_LIST( A,  ldo, 3 ),
      PMIC_NPA_NODE_DEP_LIST( A,  ldo, 5 ),
      PMIC_NPA_NODE_DEP_LIST( A,  ldo, 12 ),
};
/* NPA NODE RESOURCE */
pm_npa_node_resource_info 
pm_npa_appsbl_pam_node_rsrcs [] =
{
   {
      PMIC_NPA_GROUP_ID_DISP_EXT_DP, // Resource Name
      PMIC_NPA_MODE_ID_GENERIC_ACTIVE,   // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm_pam_disp_ext_dp_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_DISP_EXT_DP, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_client_dev_deps_disp_ext_dp, // Node Dependency
      NPA_ARRAY_SIZE(pm_client_dev_deps_disp_ext_dp), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_DISP_PRIM, // Resource Name
      PMIC_NPA_MODE_ID_GENERIC_ACTIVE,   // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm_pam_disp_prim_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_DISP_PRIM, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_client_dev_deps_disp_prim, // Node Dependency
      NPA_ARRAY_SIZE(pm_client_dev_deps_disp_prim), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_USB_SS1, // Resource Name
      PMIC_NPA_MODE_ID_USB_HOST_ACTIVE,   // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm_pam_usb_ss1_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_USB_SS1, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_client_dev_deps_usb_ss1, // Node Dependency
      NPA_ARRAY_SIZE(pm_client_dev_deps_usb_ss1), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_USB_SS2, // Resource Name
      PMIC_NPA_MODE_ID_USB_HOST_ACTIVE,   // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm_pam_usb_ss2_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_USB_SS2, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_client_dev_deps_usb_ss2, // Node Dependency
      NPA_ARRAY_SIZE(pm_client_dev_deps_usb_ss2), // Deps Count & Deps Node Dependency Count
   },
};

uint32 num_of_pm_appsbl_nodes [] = { sizeof(pm_npa_appsbl_pam_node_rsrcs) / sizeof(pm_npa_node_resource_info) };

pm_npa_remote_name_type
pmic_npa_clk_remote_resources [] =
{
};

pm_npa_remote_resource_type
pmic_npa_remote_clk [1] = 
{
   {
      pmic_npa_clk_remote_resources,
      sizeof(pmic_npa_clk_remote_resources) / sizeof(pm_npa_remote_name_type)
   }
};

pm_npa_remote_name_type
pmic_npa_ldo_remote_resources [] =
{
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 3), "ldoa\x03\x00\x00\x00" },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 4), "ldoa\x04\x00\x00\x00" },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 5), "ldoa\x05\x00\x00\x00" },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 12), "ldoa\x0C\x00\x00\x00" },
};

pm_npa_remote_resource_type
pmic_npa_remote_ldo [1] = 
{
   {
      pmic_npa_ldo_remote_resources,
      sizeof(pmic_npa_ldo_remote_resources) / sizeof(pm_npa_remote_name_type)
   }
};

pm_npa_remote_name_type
pmic_npa_smps_remote_resources [] =
{
};

pm_npa_remote_resource_type
pmic_npa_remote_smps [1] = 
{
   {
      pmic_npa_smps_remote_resources,
      sizeof(pmic_npa_smps_remote_resources) / sizeof(pm_npa_remote_name_type)
   }
};
