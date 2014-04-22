#ifndef UIM_COMMON_EFS_H
#define UIM_COMMON_EFS_H
/*===========================================================================


                U I M   C O M M O N   E F S   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2020 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.3.1/common/inc/uim_common_efs.h#4 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/23/23   tq      Modem base IMSI switch
08/28/20   rkg     Suspend timer configuration using EFS
07/28/20   rkg     Add suspend resume support
03/12/19   vm      Set byte 10 in EF-TST as per ATT REQ 13340 v18.3 
02/20/19   ng      Delay local card power up in Modem SSR in BTSAP mode 
11/15/18   tq      Updating the UIM_COMMON_EFS path to use a common value
09/27/17   nc      UIM RMT Secure Channel 
03/15/17   nr      Add support to cache PIN1 of the SIM card when PSM mode enabled
01/16/17   dd      Add test mode logging enabled APIs for sensitive info control
11/10/16   me      Added support for mmode ciot EFS  
09/13/16   sn      Support for CDMALess Device
08/19/16   me      Added support for non uim EDRX EFS and UIM MTC EFS
08/10/16   ar      Support for setting terminal capability from AP
07/04/16   ar      EONS selective logging
06/23/16   nr      ECC optimizations for IoE devices
06/23/16   shr     Deprecate GSTK TAL
03/09/16   vdc     Removed JCDMA related code
12/25/15   bcho    Send Card and session events based on SFS data in PSM mode
11/05/15   ks      Rel12 polling enhancement
09/25/15   vr      Added support for handling multiple profiles enabled cards
09/23/15   bcho    Use MCFG Context enum instead of typecast
09/08/15   ll      UIMDRV migrates to UIM COMMON EFS APIs for all EFS items
09/07/15   bcho    Migrate to mcfg EFS wrappers
08/05/15   nr      Operator specifc NV ECC numbers configuration
07/03/15   tkl     Added support for custom fields
04/30/15   vdc     Added support for APN for bootstrapping
04/30/15   lxu     Use gba_url and gba_port in nv if they are valid
04/30/15   vdc     Added support for GBA
04/30/15   nr      Enhancements in handling of GCF anite flag update
04/27/15   vr      Non UI and non NW proactive command parallel processing
12/19/14   gm      Added support to read envelope retry data from NV
12/10/14   vr      Disable CC env during SEND/RECEIVE DATA based on NV config
11/19/14   av      Fix logic of setting ANITE_NV when card contains CSIM app
11/10/14   akv     Support for user to customize LDO settings
10/31/14   gm      Update qmi_cat_raw_unsupported_features in conf file
10/13/14   tl      Fix compiler warnings
10/12/14   hh      Add support to get Voice Domain Preference
09/09/14   kk      Enhancements to NV handling
07/25/14   av      Auto onchip activation for multi slots
07/15/14   yt      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"
#include "mcfg_fs.h"

/*=============================================================================

                          UIM COMMON DEFINITIONS
                     Definitions used throughout UIM

=============================================================================*/

/* Number of bytes used for storing length of terminal capability EFS file */
#define UIM_COMMON_TERMINAL_CAPABILITY_EFS_LEN_BYTES                 2

/* Paths for EFS Dir */
#define UIM_COMMON_EFS_PATH_CONF_DIR1                            "/nv"
#define UIM_COMMON_EFS_PATH_CONF_DIR2                            "/nv/item_files"
#define UIM_COMMON_EFS_PATH_CONF_DIR3                            "/nv/item_files/conf"

/* Paths for QMI CAT, QMI UIM, UIM dir */
#define UIM_COMMON_EFS_PATH_MODEM                                "/nv/item_files/modem"
#define UIM_COMMON_EFS_PATH_MODEM_QMI                            "/nv/item_files/modem/qmi"
#define UIM_COMMON_EFS_PATH_MODEM_QMI_CAT                        "/nv/item_files/modem/qmi/cat"
#define UIM_COMMON_EFS_PATH_MODEM_QMI_UIM                        "/nv/item_files/modem/qmi/uim"
#define UIM_COMMON_EFS_PATH_MODEM_UIM_MMGSDI                     "/nv/item_files/modem/uim/mmgsdi"
#define UIM_COMMON_EFS_PATH_MODEM_UIM_REMOTE                     "/nv/item_files/modem/uim/remote"
#define UIM_COMMON_EFS_PATH_MODEM_UIM_MMGSDI_SECURE              "/nv/item_files/modem/uim/mmgsdi/secure"
#define UIM_COMMON_EFS_PATH_MODEM_UIM_GSTK                       "/nv/item_files/modem/uim/gstk"
#define UIM_COMMON_EFS_PATH_MODEM_UIM_UIMDRV                     "/nv/item_files/modem/uim/uimdrv"
#define UIM_COMMON_EFS_PATH_MODEM_UIM_GBA                        "/nv/item_files/modem/uim/gba"
#define UIM_COMMON_EFS_PATH_MODEM_UIM_LPA                        "/nv/item_files/modem/uim/lpa"
#define UIM_COMMON_EFS_PATH_CONF                                 "/nv/item_files/conf"
#define UIM_COMMON_EFS_PATH_IMS                                  "/nv/item_files/ims"
#define UIM_COMMON_EFS_PATH_PBM                                  "/nv/item_files/pbm"

/* Paths for MMGSDI EFS items accessed via UIM Common APIs */
#define UIM_COMMON_EFS_PATH_MMGSDI_REFRESH_RETRY                    "refresh_retry"
#define UIM_COMMON_EFS_PATH_MMGSDI_REFRESH_VOTE_OK                   "refresh_vote_ok"
#define UIM_COMMON_EFS_PATH_MMGSDI_FEATURE_STATUS_LIST               "features_status_list"
#define UIM_COMMON_EFS_PATH_MMGSDI_ENS_SLOT_ENABLED                  "ens_slot_enabled"
#define UIM_COMMON_EFS_PATH_MMGSDI_ONCHIP_SUPPORT                    "auto_activate_onchip"
#define UIM_COMMON_EFS_PATH_MMGSDI_ENS_SUB_FEATURES_STATUS_LIST_FILE "ens_sub_features_status_list"
#define UIM_COMMON_EFS_PATH_MMGSDI_DIAG_SUPPORT                          "mmgsdi_diag_support" 
#define UIM_COMMON_EFS_PATH_MMGSDI_HALT_SUB                              "halt_subscription"
#define UIM_COMMON_EFS_PATH_MMGSDI_ENCRYPT_SUB                           "encrypt_sub_ok"
#define UIM_COMMON_EFS_PATH_MMGSDI_CSIM_DEVICE_MODEL                     "csim_device_model"
#define UIM_COMMON_EFS_PATH_MMGSDI_ONCHIP_CONFIG_EFS_FILE_SLOT1      "/mmgsdi/onchip/onchip_slot1"
#define UIM_COMMON_EFS_PATH_MMGSDI_ONCHIP_CONFIG_EFS_FILE_SLOT2      "/mmgsdi/onchip/onchip_slot2"
#define UIM_COMMON_EFS_PATH_MMGSDI_ONCHIP_CONFIG_EFS_FILE_SLOT3      "/mmgsdi/onchip/onchip_slot3"
#define UIM_COMMON_EFS_PATH_MMGSDI_BUILTIN_PLMN_LIST_FILE                "builtin_plmn_list"
#define UIM_COMMON_EFS_PATH_MMGSDI_AUTO_ACTIVATE_MULTI_SLOTS_FILE        "auto_activate_multi_slots"
#define UIM_COMMON_EFS_PATH_MMGSDI_MULTI_PROFILE_FILE                    "multi_profile"
#define UIM_COMMON_EFS_PATH_MMGSDI_GCF_GSTK_FILE                     "gcf_gstk"
#define UIM_COMMON_EFS_PATH_MMGSDI_GPRS_GCF_ANITE_CONTROL            "gprs_gcf_anite_control"
#define UIM_COMMON_EFS_PATH_MMGSDI_PSM                                   "psm_mode"
#define UIM_COMMON_EFS_PATH_MMGSDI_PSM_PIN1_CACHE_SUPPORT                "psm_pin1_cache_support"
#define UIM_COMMON_EFS_PATH_MMGSDI_SELECTIVE_LOGGING                     "enable_selective_logging"
#define UIM_COMMON_EFS_PATH_MMGSDI_MTC                                   "mtc_mode"
#define UIM_COMMON_EFS_PATH_MMGSDI_TEST_MODE_LOGGING_ENABLED             "test_mode_logging_enabled"
#define UIM_COMMON_EFS_PATH_MMGSDI_IMSI_SWITCH_TIMER                     "imsi_switch_timer"
#define UIM_COMMON_EFS_PATH_MMGSDI_IMSI_SWITCH_PING_PONG_TIMER            "ping_pong_timer"
#define UIM_COMMON_EFS_PATH_MMGSDI_IMSI_SWITCH_SUPPORT                    "imsi_switch_support"

/* Paths for GSTK EFS items accessed via UIM Common APIs */
#define UIM_COMMON_EFS_PATH_GSTK_ME_TP_CLIENTS                   "tp_clients"
#define UIM_COMMON_EFS_PATH_GSTK_ME_PROACTIVE_FEATURE_ENABLE_CFG "proactive_feature_enable_cfg"
#define UIM_COMMON_EFS_PATH_GSTK_ME_REDIAL_CFG                   "redial_cfg"
#define UIM_COMMON_EFS_PATH_GSTK_ME_QMI_CAT_SVC_INIT             "qmi_cat_svc_init"
#define UIM_COMMON_EFS_PATH_GSTK_SLOT_CAT_VERSION                "cat_version"
#define UIM_COMMON_EFS_PATH_GSTK_SLOT_RAT_PRIORITY_CONFIG        "rat_priority_config"
#define UIM_COMMON_EFS_PATH_GSTK_SLOT_BIP_APN_MAPPING_TABLE      "bip_apn_mapping_tbl"
#define UIM_COMMON_EFS_PATH_GSTK_SLOT_DEFAULT_APN                "default_apn"
#define UIM_COMMON_EFS_PATH_GSTK_SLOT_FEATURE_BMASK              "feature_bmsk"
#define UIM_COMMON_EFS_PATH_GSTK_SLOT_TOOLKIT_ENV_RETRY_EXT_DATA "envelope_retry"
#define UIM_COMMON_EFS_PATH_GSTK_SUB_BIP_OTASP_CONCURRENCY       "bip_otasp_concurrency"
#define UIM_COMMON_EFS_PATH_GSTK_SUB_ATTACH_CTRL_TIMER           "attach_ctrl_timer"
#define UIM_COMMON_EFS_PATH_GSTK_SLOT_FEATURE_BMASK_2            "feature_bmsk_2"
#define UIM_COMMON_EFS_PATH_GSTK_ME_FEATURE_BMASK                "me_feature_bmsk"

/* Paths for UIMDRV carrier dependent EFS items accessed via UIM Common APIs */
#define UIM_COMMON_EFS_PATH_UIMDRV_PDOWN_NV                      "nv_pdown_uim_consecutive_techproblems"
#define UIM_COMMON_EFS_PATH_HW_LDO_CONFIG                        "uim_hw_ldo_config_efs"
#define UIM_COMMON_EFS_PATH_UIM_PUP_LOGGING_CONFIG               "uim_powerup_logging_config"

/* Paths for UIMDRV non-carrier dependent EFS items accessed via UIM Common APIs */
#define UIM_COMMON_EFS_PATH_UIMDRV_CONF                                "uimdrv.conf"
#define UIM_COMMON_EFS_PATH_UIMDRV_FEATURES_STATUS_LIST                "uim_features_status_list"
#define UIM_COMMON_EFS_PATH_UIMDRV_HOTSWAP_DEBOUNCE_CONFIG             "me_hotswap_configuration"
#define UIM_COMMON_EFS_PATH_UIMDRV_ACTIVE_SLOT                         "nv_active_slot_configuration"
#define UIM_COMMON_EFS_PATH_UIMDRV_DISABLE_POLLING                     "uim_disable_polling"
#define UIM_COMMON_EFS_PATH_UIMDRV_BUSY_RESPONSE                       "sim_response_timers_config"
#define UIM_COMMON_EFS_PATH_UIMDRV_FEATURE_BUSY_RESPONSE_SIMULATE_NULL "uim_busy_response_simulate_null_config"
#define UIM_COMMON_EFS_PATH_UIMDRV_REMOTE_COMMAND_RESP_TIMER           "nv_remote_command_resp_timer"
#define UIM_COMMON_EFS_PATH_UIMDRV_FEATURE_SUPPORT_HOTSWAP             "feature_support_hotswap"
#define UIM_COMMON_EFS_PATH_UIMDRV_HW_CONFIG                           "uim_hw_config"
#define UIM_COMMON_EFS_PATH_UIMDRV_EXTENDED_RECOVERY_FILE              "uim_extended_recovery_config"
#define UIM_COMMON_EFS_PATH_LAST_MODE_AND_CONFIG_DELAY_FOR_REMOTE_CONNECT  	"uim_last_mode_and_config_delay_for_remote_connect"
#define UIM_COMMON_EFS_PATH_UIMDRV_REL12_POLLING_OPTIMIZATION_FILE     "uim_rel12_polling_optimization"
#define UIM_COMMON_EFS_PATH_UIMDRV_ENABLE_SUSPEND_RESUME               "uim_enable_suspend_resume"
#define UIM_COMMON_EFS_PATH_UIMDRV_SUSPEND_TIMER_CONFIG                 "uim_suspend_timer_config"

/* Paths for QMI UIM/CAT EFS items accessed via UIM Common APIs */
#define UIM_COMMON_EFS_PATH_UIMQMI_QMI_CAT_MODE                     "qmi_cat_mode"
#define UIM_COMMON_EFS_PATH_UIMQMI_QMI_CAT_DEFAULT_LANG             "qmi_cat_default_lang"
#define UIM_COMMON_EFS_PATH_UIMQMI_QMI_CAT_CONFIG_TP                "qmi_cat_custom_tp"
#define UIM_COMMON_EFS_PATH_UIMQMI_QMI_CAT_DISPLAY_ALPHA_FILE       "qmi_cat_display_alpha"
#define UIM_COMMON_EFS_PATH_UIMQMI_QMI_CAT_BLOCK_SMS_PP_ENV         "qmi_cat_block_sms_pp_env"
#define UIM_COMMON_EFS_PATH_UIMQMI_QMI_CAT_RAW_UNSUPPORTED_FEATURES "qmi_cat_raw_unsupported_features"

#define UIM_COMMON_EFS_PATH_UIMQMI_QMI_UIM_CONFIG_SAP_SEC_RESTR      "sap_security_restrictions"
#define UIM_COMMON_EFS_PATH_UIMQMI_QMI_UIM_CONFIG_AUTH_SEC_RESTR     "auth_security_restrictions"
#define UIM_COMMON_EFS_PATH_UIMQMI_QMI_UIM_CONFIG_APDU_SEC_RESTR     "apdu_security_restrictions"
#define UIM_COMMON_EFS_PATH_UIMQMI_QMI_UIM_CONFIG_SILENT_PIN1_FILE   "qmi_uim_silent_pin1"
#define UIM_COMMON_EFS_PATH_UIMQMI_QMI_UIM_CONFIG_APDU_SEC_AID_LIST  "apdu_security_aid_list"
#define UIM_COMMON_EFS_PATH_UIMQMI_QMI_UIM_CONFIG_CLOSE_CHANNEL_SYNC "close_channel_sync"
#define UIM_COMMON_EFS_PATH_UIMQMI_QMI_UIM_CONFIG_SILENT_RECOVERY    "silent_recovery"
#define UIM_COMMON_EFS_PATH_UIMQMI_QMI_UIM_TERMINAL_CAPABILITY       "terminal_capability"
#define UIM_COMMON_EFS_PATH_UIMQMI_QMI_UIM_CONFIG_REMOTE_SERVICE     "uim_remote_service_enable"
#define UIM_COMMON_EFS_PATH_UIMQMI_UIM_REMOTE_SECURE_CHANNEL         "qmi_uim_remote_secure_channel"

/* Paths for GBA EFS items access via UIM Common APIs */
#define UIM_COMMON_EFS_PATH_GBA_BOOTSTRAP_BSF_ADDRESS                "bsf_address"
#define UIM_COMMON_EFS_PATH_GBA_BOOTSTRAP_APN                        "access_point_name"
#define UIM_COMMON_EFS_PATH_GBA_BOOTSTRAP_CUSTOM_REALM               "custom_realm"
#define UIM_COMMON_EFS_PATH_GBA_BOOTSTRAP_CUSTOM_USER_AGENT_TMPI     "custom_user_agent_tmpi"
#define UIM_COMMON_EFS_PATH_GBA_BOOTSTRAP_HTTP_HEADER_IMEI           "http_header_imei"

/* Paths for non-UIM EFS items accessed via UIM Common APIs */
#define UIM_COMMON_EFS_PATH_NON_UIM_CSG_SUPPORT_CONFIG           "nas/csg_support_configuration"
#define UIM_COMMON_EFS_PATH_NON_UIM_IMS_ENABLE                   "IMS_enable"
#define UIM_COMMON_EFS_PATH_NON_UIM_RPM_INFO                     "mmode/rpm_info"
#define UIM_COMMON_EFS_PATH_NON_UIM_VOICE_DOMAIN_PREF            "mmode/voice_domain_pref"
#define UIM_COMMON_EFS_PATH_NON_UIM_LTE_3GPP_RELEASE_VER         "lte/lte_3gpp_release_ver"
#define UIM_COMMON_EFS_PATH_NON_UIM_PSM_MODE                     "mmode/psm_config"
#define UIM_COMMON_EFS_PATH_NON_UIM_CDMA_LESS_DEVICE             "mmode/cdma_less"
#define UIM_COMMON_EFS_PATH_NON_UIM_CIOT_LTE_PREFERENCES         "mmode/ciot_lte_preferences"

/* Paths for PBM EFS items accessed via UIM Common APIs */
#define UIM_COMMON_EFS_PATH_PBM_NV_ECC_LIST                      "pbm_ecc_nums"
#define UIM_COMMON_EFS_PATH_PBM_NV_HARDCODED_ECC_CONFIG          "pbm_hardcoded_ecc_config"
#define UIM_COMMON_EFS_PATH_PBM_NV_HARDCODED_ECC_LIST            "pbm_hardcoded_ecc_list"
/* Paths for PBM EFS items accessed via UIM Common APIs */
#define UIM_COMMON_EFS_PATH_PBM_NV_ECC_LIST_PER_SUB              "pbm_nv_ecc_list_per_sub"
#define UIM_COMMON_EFS_PATH_PBM_SIM_ECC_AIRPLANE                 "pbm_sim_ecc_airplane"
#define UIM_COMMON_EFS_PATH_PBM_NV_ECC_SOURCE_CONFIG             "pbm_nv_ecc_config"
#define UIM_COMMON_EFS_PATH_PBM_EFS_SUPPORT_DISABLE              "pbm_efs_support_disable"
#define UIM_COMMON_EFS_PATH_PBM_FEATURES_LIST                    "features_status_list"

/* Config file for multi-context items */
#define UIM_COMMON_EFS_PATH_MMGSDI_MULTI_CONTEXT_CONF            "mmgsdi.conf"

/*=============================================================================
   ENUM:      UIM_COMMON_EFS_ITEM_TYPE

   DESCRIPTION:
     Common enums for EFS items accessed by various UIM modules.
=============================================================================*/
typedef enum
{
  /* Items owned by MMGSDI */
  UIM_COMMON_EFS_MMGSDI_START = 0x0,
  UIM_COMMON_EFS_MMGSDI_REFRESH_RETRY,
  UIM_COMMON_EFS_MMGSDI_REFRESH_VOTE_OK,
  UIM_COMMON_EFS_MMGSDI_FEATURE_STATUS_LIST,
  UIM_COMMON_EFS_MMGSDI_ENS_SLOT_ENABLED,
  UIM_COMMON_EFS_MMGSDI_ONCHIP_SUPPORT,
  UIM_COMMON_EFS_MMGSDI_DIAG_SUPPORT,
  UIM_COMMON_EFS_MMGSDI_HALT_SUBSCRIPTION,
  UIM_COMMON_EFS_MMGSDI_ENCRYPT_SUB_OK,
  UIM_COMMON_EFS_MMGSDI_CSIM_DEVICE_MODEL,
  UIM_COMMON_EFS_MMGSDI_ONCHIP_CONFIG_SLOT1,
  UIM_COMMON_EFS_MMGSDI_ONCHIP_CONFIG_SLOT2,
  UIM_COMMON_EFS_MMGSDI_ONCHIP_CONFIG_SLOT3,
  UIM_COMMON_EFS_MMGSDI_ENS_SUB_FEATURES_STATUS_LIST,
  UIM_COMMON_EFS_MMGSDI_BUILT_IN_PLMN_LIST,
  UIM_COMMON_EFS_MMGSDI_AUTO_ACTIVATE_MULTI_SLOTS_FILE,
  UIM_COMMON_EFS_MMGSDI_GCF_GSTK_FILE,
  UIM_COMMON_EFS_MMGSDI_MULTI_PROFILE_FILE,
  UIM_COMMON_EFS_MMGSDI_GPRS_GCF_ANITE_CONTROL,
  UIM_COMMON_EFS_MMGSDI_PSM,
  UIM_COMMON_EFS_MMGSDI_PSM_PIN1_CACHE_SUPPORT,
  UIM_COMMON_EFS_MMGSDI_SELECTIVE_LOGGING,
  UIM_COMMON_EFS_MMGSDI_MTC,
  UIM_COMMON_EFS_MMGSDI_TEST_MODE_LOGGING_ENABLED,
  UIM_COMMON_EFS_MMGSDI_IMSI_SWITCH_TIMER,
  UIM_COMMON_EFS_MMGSDI_IMSI_SWITCH_PING_PONG_TIMER,
  UIM_COMMON_EFS_MMGSDI_IMSI_SWITCH_SUPPORT,
 
  /* Items owned by GSTK */
  UIM_COMMON_EFS_GSTK_START = 0x100,
  UIM_COMMON_EFS_GSTK_ME_TP_CLIENTS,
  UIM_COMMON_EFS_GSTK_ME_PROACTIVE_FEATURE_ENABLE_CFG,
  /* Deprecated: UIM_COMMON_EFS_GSTK_ME_TAL_CONF, */
  UIM_COMMON_EFS_GSTK_ME_REDIAL_CFG = 0x104,
  UIM_COMMON_EFS_GSTK_ME_QMI_CAT_SVC_INIT,
  UIM_COMMON_EFS_GSTK_SLOT_CAT_VERSION,
  UIM_COMMON_EFS_GSTK_SLOT_RAT_PRIORITY_CONFIG,
  UIM_COMMON_EFS_GSTK_SLOT_BIP_APN_MAPPING_TABLE,
  UIM_COMMON_EFS_GSTK_SLOT_DEFAULT_APN,
  UIM_COMMON_EFS_GSTK_SLOT_FEATURE_BMASK,
  UIM_COMMON_EFS_GSTK_SLOT_TOOLKIT_ENV_RETRY_EXT_DATA,
  UIM_COMMON_EFS_GSTK_SUB_BIP_OTASP_CONCURRENCY,
  UIM_COMMON_EFS_GSTK_SUB_ATTACH_CTRL_TIMER,
  UIM_COMMON_EFS_GSTK_SLOT_FEATURE_BMASK_2,
  UIM_COMMON_EFS_GSTK_ME_FEATURE_BMASK,

  /* Items owned by UIMDRV */
  UIM_COMMON_EFS_UIMDRV_START = 0x200,
  UIM_COMMON_EFS_UIMDRV_CONF,
  UIM_COMMON_EFS_UIMDRV_PDOWN_NV_EF,
  UIM_COMMON_EFS_UIMDRV_LDO_SETTINGS,
  UIM_COMMON_EFS_UIMDRV_FEATURES_STATUS_LIST,
  UIM_COMMON_EFS_UIMDRV_HOTSWAP_DEBOUNCE_CONFIG,
  UIM_COMMON_EFS_UIMDRV_ACTIVE_SLOT,
  UIM_COMMON_EFS_UIMDRV_BUSY_RESPONSE,
  UIM_COMMON_EFS_UIMDRV_FEATURE_BUSY_RESPONSE_SIMULATE_NULL,
  UIM_COMMON_EFS_UIMDRV_REMOTE_COMMAND_RESP_TIMER,
  UIM_COMMON_EFS_UIMDRV_FEATURE_SUPPORT_HOTSWAP,
  UIM_COMMON_EFS_UIMDRV_HW_CONFIG,
  UIM_COMMON_EFS_UIMDRV_DISABLE_POLLING,
  UIM_COMMON_EFS_UIMDRV_EXTENDED_RECOVERY_FILE,
  UIM_COMMON_EFS_LAST_MODE_AND_CONFIG_DELAY_FOR_REMOTE_CONNECT,
  UIM_COMMON_EFS_UIMDRV_REL12_POLLING_OPTIMIZATION_FILE,
  UIM_COMMON_EFS_UIMDRV_ENABLE_SUSPEND_RESUME,
  UIM_COMMON_EFS_UIMDRV_SUSPEND_TIMER_CONFIG,

  /* Items owned by QMI UIM and CAT */
  UIM_COMMON_EFS_UIMQMI_START = 0x300,
  UIM_COMMON_EFS_UIMQMI_QMI_CAT_MODE,
  UIM_COMMON_EFS_UIMQMI_QMI_CAT_DEFAULT_LANG,
  UIM_COMMON_EFS_UIMQMI_QMI_CAT_CONFIG_TP,
  UIM_COMMON_EFS_UIMQMI_QMI_CAT_DISPLAY_ALPHA_FILE,
  UIM_COMMON_EFS_UIMQMI_QMI_CAT_BLOCK_SMS_PP_ENV,
  UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_SAP_SEC_RESTR,
  UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_AUTH_SEC_RESTR,
  UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_APDU_SEC_RESTR,
  UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_SILENT_PIN1_FILE,
  UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_APDU_SEC_AID_LIST,
  UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_CLOSE_CHANNEL_SYNC,
  UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_SILENT_RECOVERY,
  UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_GBA_FQDN_WHITE_LIST,
  UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_GBA_FQDN_BLACK_LIST,
  UIM_COMMON_EFS_UIMQMI_QMI_CAT_RAW_UNSUPPORTED_FEATURES,
  UIM_COMMON_EFS_UIMQMI_QMI_UIM_TERMINAL_CAPABILITY,
  UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_REMOTE_SERVICE,
  UIM_COMMON_EFS_UIMQMI_QMI_UIM_REMOTE_SECURE_CHANNEL,

  /* Items accessed in UIM but not owned by UIM */
  UIM_COMMON_EFS_NON_UIM_START = 0x400,
  UIM_COMMON_EFS_NON_UIM_CSG_SUPPORT_CONFIG,
  UIM_COMMON_EFS_NON_UIM_IMS_ENABLE,
  UIM_COMMON_EFS_NON_UIM_RPM_INFO,
  UIM_COMMON_EFS_NON_UIM_VOICE_DOMAIN_PREF,
  UIM_COMMON_EFS_NON_UIM_LTE_3GPP_RELEASE_VER,
  UIM_COMMON_EFS_NON_UIM_PSM_MODE,
  UIM_COMMON_EFS_NON_UIM_CDMA_LESS_DEVICE,
  UIM_COMMON_EFS_NON_UIM_CIOT_LTE_PREFERENCES, 

  /* Config file for multi-context items */
  UIM_COMMON_EFS_MMGSDI_MULTI_CONTEXT_CONF = 0x500,

  /* Items owned by GBA */
  UIM_COMMON_EFS_GBA_BOOTSTRAP_BSF_ADDRESS = 0x600,
  UIM_COMMON_EFS_GBA_BOOTSTRAP_APN,
  UIM_COMMON_EFS_GBA_BOOTSTRAP_CUSTOM_REALM,
  UIM_COMMON_EFS_GBA_BOOTSTRAP_CUSTOM_USER_AGENT_TMPI,
  UIM_COMMON_EFS_GBA_BOOTSTRAP_HTTP_HEADER_IMEI,

 /* Items owned by PBM */
  UIM_COMMON_EFS_PBM_NV_ECC_LIST = 0x700,
  UIM_COMMON_EFS_PBM_NV_HARDCODED_ECC_CONFIG,
  UIM_COMMON_EFS_PBM_NV_HARDCODED_ECC_LIST,
  UIM_COMMON_EFS_PBM_SIM_ECC_AIRPLANE,
  UIM_COMMON_EFS_PBM_NV_ECC_SOURCE_CONFIG,
  UIM_COMMON_EFS_PBM_EFS_SUPPORT_DISABLE,
  UIM_COMMON_EFS_PBM_FEATURES_LIST,
  UIM_COMMON_EFS_PBM_NV_ECC_LIST_PER_SUB,

  UIM_COMMON_EFS_ITEM_MAX = 0x7FFF
}uim_common_efs_item_type;


/*=============================================================================
   ENUM:      UIM_COMMON_DIR_ITEM_TYPE

   DESCRIPTION:
     Common enums for DIR items accessed by various UIM modules.
=============================================================================*/
typedef enum
{
  /* Items for the directory */
  UIM_COMMON_EFS_MODEM,
  UIM_COMMON_EFS_UIMQMI_QMI,
  UIM_COMMON_EFS_UIMQMI_QMI_CAT,
  UIM_COMMON_EFS_UIMQMI_QMI_UIM,
  UIM_COMMON_EFS_CONF_DIR1,
  UIM_COMMON_EFS_CONF_DIR2,
  UIM_COMMON_EFS_CONF_DIR3,

  UIM_COMMON_DIR_ITEM_MAX = 0x7F
}uim_common_dir_item_type;

/*=============================================================================
   ENUM:      UIM_COMMON_EFS_CONTEXT_TYPE

   DESCRIPTION:
     Context for the EFS items which maps to the index corresponding to a
     subscription or slot.
=============================================================================*/
typedef enum
{
  UIM_COMMON_EFS_DEVICE      = -1, /* used for device specific EFS items */
  UIM_COMMON_EFS_CONTEXT_0   =  0,
  UIM_COMMON_EFS_CONTEXT_1,
  UIM_COMMON_EFS_CONTEXT_2,
  UIM_COMMON_EFS_CONTEXT_MAX = 0x7F
} uim_common_efs_context_type;


/*=============================================================================
   ENUM:      UIM_COMMON_EFS_STATUS_ENUM_TYPE

   DESCRIPTION:
     Return types for EFS utilities.
=============================================================================*/
typedef enum
{
  UIM_COMMON_EFS_SUCCESS,
  UIM_COMMON_EFS_ERROR
} uim_common_efs_status_enum_type;


/*=============================================================================
   ENUM:      UIM_COMMON_EFS_FILE_TYPE

   DESCRIPTION:
     Type of EFS file.
=============================================================================*/
typedef enum
{
  UIM_COMMON_EFS_REGULAR_FILE_TYPE, /* Files of size greater than FS_ITEM_MAX */
  UIM_COMMON_EFS_ITEM_FILE_TYPE     /* Files of size <= FS_ITEM_MAX that are stored
                                       on a special partition. */
} uim_common_efs_file_type;

/*=============================================================================
   ENUM:      UIM_COMMON_CONF_ITEM_LIST_TYPE

   DESCRIPTION:
     Type of conf file list
=============================================================================*/
typedef struct
{
  uim_common_efs_item_type file;
  boolean                  is_multi_context;
}uim_common_conf_item_list_type;

/*=============================================================================
FUNCTION:      UIM_COMMON_EFS_CREATE_DIR

DESCRIPTION:
  This function will create dir.

PARAMETERS
  dir_item: The enum corresponding to the EFS item/file to be read
  mode:      Permissions for the directory

DEPENDENCIES:
  None

LIMITATIONS:
  None.

RETURN VALUE:
  uim_common_efs_status_enum_type

  UIM_COMMON_EFS_SUCCESS:          The EFS operation was successful.
  UIM_COMMON_EFS_ERROR:            The EFS operation was not successful.

SIDE EFFECTS:
  None
=============================================================================*/
uim_common_efs_status_enum_type uim_common_efs_create_dir(
  uim_common_dir_item_type  dir_item,
  uint16                    mode
);

/*=============================================================================
FUNCTION:      UIM_COMMON_EFS_GET_FILE_SIZE

DESCRIPTION:
  This function will retrieve the size of the EFS file using efs_stat().

PARAMETERS
  efs_item:        The enum corresponding to the EFS item/file to be read
  efs_context:     Context for the EFS item
  file_size_ptr:   Output variable to return file size

DEPENDENCIES:
  None

LIMITATIONS:
  The EFS item should a regular file.

RETURN VALUE:
  uim_efs_status_enum_type

  UIM_EFS_SUCCESS:          The EFS operation was successful.
  UIM_EFS_ERROR:            The EFS operation was not successful.

SIDE EFFECTS:
  None
=============================================================================*/
uim_common_efs_status_enum_type uim_common_efs_get_file_size(
  uim_common_efs_item_type       efs_item,
  uim_common_efs_context_type    efs_context,
  uint32                       * file_size_ptr
);


/*=============================================================================
FUNCTION:      UIM_COMMON_EFS_GET_DIR_STAT

DESCRIPTION:
  This function will retrieve the stats of the Dir using efs_stat().

PARAMETERS
  dir_item:        The enum corresponding to the EFS dir to be accessed
  file_size_ptr:   Output variable to return file size

DEPENDENCIES:
  None

LIMITATIONS:
  None.

RETURN VALUE:
  uim_common_efs_status_enum_type

  UIM_COMMON_EFS_SUCCESS:          The EFS operation was successful.
  UIM_COMMON_EFS_ERROR:            The EFS operation was not successful.

SIDE EFFECTS:
  None
=============================================================================*/
uim_common_efs_status_enum_type uim_common_efs_get_dir_stat(
  uim_common_dir_item_type       dir_item,
  uint32                       * file_size_ptr
);

/*=============================================================================
FUNCTION:      UIM_COMMON_EFS_READ

DESCRIPTION:
  This function will read the EFS file using efs_read() or efs_get()
  depending on the type of file.
  If file type is UIM_EFS_REGULAR_FILE_TYPE, the file is read using efs_read().
  This file type is used to read files which may need to be read partially.
  For UIM_EFS_ITEM_FILE_TYPE file type, the entire file is read at once
  using efs_get(). This file type is used to read the 'special item' files
  of small size (<= FS_ITEM_MAX).

PARAMETERS
  efs_item:        The enum corresponding to the EFS item/file to be read
  file_type:       Type of EFS file to be accessed
  context:         Index of the EFS item for multi-slot settings
  data_ptr:        Buffer to hold the data to be read (to be allocated
                   by the caller)
  data_len:        Size of the buffer

DEPENDENCIES:
  The caller of this API should know the file size and pass the data_len
  parameter accordingly.

LIMITATIONS:
  None

RETURN VALUE:
  uim_efs_status_enum_type

  UIM_COMMON_EFS_SUCCESS:          The EFS operation was successful.
  UIM_COMMON_EFS_ERROR:            The EFS operation was not successful.

SIDE EFFECTS:
  None
=============================================================================*/
uim_common_efs_status_enum_type uim_common_efs_read(
  uim_common_efs_item_type       efs_item,
  uim_common_efs_file_type       file_type,
  uim_common_efs_context_type    context,
  uint8                        * data_ptr,
  uint32                         data_len
);


/*=============================================================================
FUNCTION:      UIM_COMMON_EFS_WRITE

DESCRIPTION:
  This function will write the data to the EFS using efs_write() or efs_put()
  depending on the type of file.
  If file type is UIM_EFS_REGULAR_FILE_TYPE, the data is written to a regular
  EFS file using efs_write(). These type of files can be updated partially.
  For UIM_EFS_ITEM_FILE_TYPE file type, the entire file is overwritten using
  efs_put(). This file type is used to write to 'special item' files
  of small size (<= FS_ITEM_MAX).

PARAMETERS
  efs_item:        The enum corresponding to the EFS item/file
  file_type:       Type of EFS file to be accessed
  context:         Index of the EFS item for multi-slot settings
  data_ptr:        Buffer with the data to be written
  data_len:        Size of the buffer

DEPENDENCIES:
  The caller of this API should know the file size and pass the data_len
  parameter accordingly.

LIMITATIONS:
  None

RETURN VALUE:
  uim_efs_status_enum_type

  UIM_COMMON_EFS_SUCCESS:          The EFS operation was successful.
  UIM_COMMON_EFS_ERROR:            The EFS operation was not successful.

SIDE EFFECTS:
  None
=============================================================================*/
uim_common_efs_status_enum_type uim_common_efs_write(
  uim_common_efs_item_type       efs_item,
  uim_common_efs_file_type       file_type,
  uim_common_efs_context_type    context,
  const char                   * data_ptr,
  uint32                         data_len
);

/*===========================================================================
FUNCTION UIM_COMMON_EFS_DELETE

DESCRIPTION:
  This function will delete the EFS file using mcfg_fs_delete().

PARAMETERS
  efs_item:        The enum corresponding to the EFS item/file to be delete
  efs_context:     Context for the EFS item

DEPENDENCIES:
  None

LIMITATIONS:
  None.

RETURN VALUE:
  uim_common_efs_status_enum_type

  UIM_COMMON_EFS_SUCCESS:          The EFS operation was successful.
  UIM_COMMON_EFS_ERROR:            The EFS operation was not successful.

SIDE EFFECTS:
  None
=============================================================================*/
uim_common_efs_status_enum_type uim_common_efs_delete(
  uim_common_efs_item_type      efs_item,
  uim_common_efs_context_type   context
);

/*=============================================================================
FUNCTION:      UIM_COMMON_EFS_CREATE_CONF_FILE

DESCRIPTION:
  This function will create config file for EFS items that are accessed in
  multiple contexts.

PARAMETERS
  None

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
=============================================================================*/
void uim_common_efs_create_conf_file
(
  void
);

/*=============================================================================
FUNCTION:      UIM_COMMON_CONVERT_TO_MCFG_CONTEXT

DESCRIPTION:
  This function will convert uim common context type to mcfg context type.

PARAMETERS
  uim_common_context: UIM COMMON context type, passed in by the callers to API

DEPENDENCIES:
  None

LIMITATIONS:
  None.

RETURN VALUE:
  mcfg_fs_sub_id_e_type - MCFG CONTEXT Enum

SIDE EFFECTS:
  None
=============================================================================*/
mcfg_fs_sub_id_e_type uim_common_convert_to_mcfg_context(
  uim_common_efs_context_type  uim_common_context
);

/*=============================================================================
FUNCTION:      UIM_COMMON_EFS_GET_PATH_FROM_ENUM

DESCRIPTION:
  This function will return the path of an EFS file based on the enum passed.

PARAMETERS
  path_buf:        Buffer to hold the path
  path_buf_len:    Size of the path buffer
  efs_item:        The enum corresponding to the EFS item/file to be read

DEPENDENCIES:
  None

LIMITATIONS:
  None.

RETURN VALUE:
  uim_common_efs_status_enum_type

  UIM_COMMON_EFS_SUCCESS:          The EFS operation was successful.
  UIM_COMMON_EFS_ERROR:            The EFS operation was not successful.

SIDE EFFECTS:
  None
=============================================================================*/
uim_common_efs_status_enum_type uim_common_efs_get_path_from_enum(
  char                         * path_buf,
  uint32                         path_buf_len,
  uim_common_efs_item_type       efs_item
);
#endif /* UIM_COMMON_EFS_H */