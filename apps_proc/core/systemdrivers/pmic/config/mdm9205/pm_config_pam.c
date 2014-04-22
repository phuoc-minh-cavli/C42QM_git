
/*! \file  pm_config_pam.c
 *  
 *  \brief  File Contains the PMIC NPA PAM Data
 *  \details  This file contains the PAM data for the PMIC NPA layer.
 *  
 *    Platform: MDM9205.XX.1.0
 *    Revision: NO REVISIONS (Working Set)
 *    Template Version: 
 *
 *    $Header: //components/rel/core.tx/6.0/systemdrivers/pmic/config/mdm9205/pm_config_pam.c#1 $ 
 *    $DateTime: 2020/01/30 22:49:35 $  $Author: pwbldsvc $
 *  
 *  &copy; Copyright Jan. 3, 2019 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        INCLUDE HEADER FILES

===========================================================================*/

#include "pm_npa_device_clk_buff.h"
#include "pm_npa_device_ldo.h"
#include "pm_npa_device_smps.h"
#include "pm_npa.h"
#include "pm_apps_proc_npa.h"
#include "pm_apps_proc_npa_device.h"
#include "pmapp_npa.h"

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/

#define PMIC_NPA_CLIENT_NODE_CODEC "/node/pmic/client/codec"
#define PMIC_NPA_CLIENT_NODE_USB_HS1 "/node/pmic/client/usb_hs1"
/*===========================================================================

                        VARIABLES DEFINITION

===========================================================================*/

/* CODEC Client */
pm_npa_ldo_kvps
pm_pam_codec_a_ldo5 [] =
{
   // Mode: PMIC_NPA_MODE_ID_AUDIO_STANDBY
   // Comments: DISCONNECT: Can use for slotted sleep. LDOs OFF.
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
   // Mode: PMIC_NPA_MODE_ID_AUDIO_ACTIVE_LOW
   // Comments: NPM: Cable connected and not suspended, Fully powered. LDOs in NPM.
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
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
   // Mode: PMIC_NPA_MODE_ID_AUDIO_ACTIVE_MED
   // Comments: SUSPEND: For bus suspend conditions. LDOs in LPM. Can use in Airplane sleep mode.
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
pm_pam_codec_rails_info [] =
{
   {
      (void*)pm_pam_codec_a_ldo5,
      PM_NPA_VREG_LDO
   },
};

npa_node_dependency
pm_client_dev_deps_codec [] =
{
      PMIC_NPA_NODE_DEP_LIST( A,  ldo, 5 ),
};

/* USB_HS1 Client */
pm_npa_ldo_kvps
pm_pam_usb_hs1_a_ldo2 [] =
{
   // Mode: PMIC_NPA_MODE_ID_USB_HS_POWER_OFF
   // Comments: DISCONNECT: Can use for slotted sleep. LDOs OFF.
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
   // Mode: PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_0
   // Comments: NPM: Cable connected and not suspended, Fully powered. LDOs in NPM.
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
   // Mode: PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_1
   // Comments: SUSPEND: For bus suspend conditions. LDOs in LPM. Can use in Airplane sleep mode.
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
pm_pam_usb_hs1_a_ldo5 [] =
{
   // Mode: PMIC_NPA_MODE_ID_USB_HS_POWER_OFF
   // Comments: DISCONNECT: Can use for slotted sleep. LDOs OFF.
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
   // Mode: PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_0
   // Comments: NPM: Cable connected and not suspended, Fully powered. LDOs in NPM.
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right). */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right). */
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
   // Mode: PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_1
   // Comments: SUSPEND: For bus suspend conditions. LDOs in LPM. Can use in Airplane sleep mode.
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
pm_pam_usb_hs1_rails_info [] =
{
   {
      (void*)pm_pam_usb_hs1_a_ldo2,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_usb_hs1_a_ldo5,
      PM_NPA_VREG_LDO
   },
};

npa_node_dependency
pm_client_dev_deps_usb_hs1 [] =
{
      PMIC_NPA_NODE_DEP_LIST( A,  ldo, 2 ),
      PMIC_NPA_NODE_DEP_LIST( A,  ldo, 5 ),
};
/* NPA NODE RESOURCE */
pm_npa_node_resource_info 
pm_npa_pam_node_rsrcs [] =
{
   {
      PMIC_NPA_GROUP_ID_CODEC, // Resource Name
      PMIC_NPA_MODE_ID_AUDIO_ACTIVE_MED,   // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm_pam_codec_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_CODEC, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_client_dev_deps_codec, // Node Dependency
      NPA_ARRAY_SIZE(pm_client_dev_deps_codec), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_USB_HS1, // Resource Name
      PMIC_NPA_MODE_ID_USB_HS_PERI_LPM_1,   // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm_pam_usb_hs1_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_USB_HS1, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_client_dev_deps_usb_hs1, // Node Dependency
      NPA_ARRAY_SIZE(pm_client_dev_deps_usb_hs1), // Deps Count & Deps Node Dependency Count
   },
};

uint32 num_of_pam_nodes [] = { sizeof(pm_npa_pam_node_rsrcs) / sizeof(pm_npa_node_resource_info) };

pm_npa_remote_name_type
pmic_npa_clk_remote_resources [] =
{
    {NULL , NULL}
};

pm_npa_remote_resource_type
pmic_npa_remote_clk [1] = 
{
   {
      pmic_npa_clk_remote_resources,
      0, //sizeof(pmic_npa_clk_remote_resources) / sizeof(pm_npa_remote_name_type)
   }
};

pm_npa_remote_name_type
pmic_npa_ldo_remote_resources [] =
{
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 2), "ldoa\x02\x00\x00\x00" },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 5), "ldoa\x05\x00\x00\x00" },
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
  {NULL , NULL}
};

pm_npa_remote_resource_type
pmic_npa_remote_smps [1] = 
{
   {
      pmic_npa_smps_remote_resources,
      0,//sizeof(pmic_npa_smps_remote_resources) / sizeof(pm_npa_remote_name_type)
   }
};
