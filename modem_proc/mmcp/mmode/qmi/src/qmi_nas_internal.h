#ifndef _QMI_NAS_INTERNAL_H
#define _QMI_NAS_INTERNAL_H
/*===========================================================================

                         QMI_NAS_INTERNAL.H

DESCRIPTION

 QMI_NAS header file for internal definitions

EXTERNALIZED FUNCTIONS

Copyright (c) 2004-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/qmi/src/qmi_nas_internal.h#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/18/11    hs     Created file
===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"

#include "qm_comdef.h"
#include "qmi_nas_hdr.h"

#include "cm.h"
#include "qmi_mmode_svc.h"
#include "qmi_csi.h"
#include "mmgsdilib_common.h"
#include "sys.h"
#include "rex.h"
#include "qmi_mmode_task_cmd.h"

#include "lte_rrc_ext_msg.h"
#include "msgr_rex.h"
#ifdef FEATURE_DUAL_SIM
  #include "cm_dualsim.h"
#endif
#include "qmi_idl_lib.h"

#include "network_access_service_v01.h"

#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#endif // FEATURE_MODEM_CONFIG_REFRESH
#include "qm_meas.h"

#if defined(FEATURE_MMODE_QMI_LTE_ONLY) || defined(FEATURE_MMODE_QMI_GPRS_LTE)
#define FEATURE_MMODE_DISABLE_MISC_FEATURES
#define FEATURE_MMODE_DISABLE_EMERGENCY
#define FEATURE_MMODE_DISABLE_UIM
#ifdef FEATURE_MMODE_DISABLE_MISC_FEATURES
#define REMOVE_QMI_NAS_SET_EVENT_REPORT_V01
#define REMOVE_QMI_NAS_GET_SIGNAL_STRENGTH_V01
#define REMOVE_QMI_NAS_INITIATE_NETWORK_REGISTER_V01
#define REMOVE_QMI_NAS_INITIATE_ATTACH_V01
#define REMOVE_QMI_NAS_SET_TECHNOLOGY_PREFERENCE_V01
#define REMOVE_QMI_NAS_GET_TECHNOLOGY_PREFERENCE_V01
#define REMOVE_QMI_NAS_GET_CSP_PLMN_MODE_BIT_V01
#define REMOVE_QMI_NAS_GET_CURRENT_ACQ_SYS_MODE_V01
#define REMOVE_QMI_NAS_UPDATE_AKEY_V01
#define REMOVE_QMI_NAS_CONFIG_SIG_INFO_V01
//#define REMOVE_QMI_NAS_GET_PLMN_NAME_V01
#define REMOVE_QMI_NAS_GET_CENTRALIZED_EONS_SUPPORT_STATUS_V01
#define REMOVE_QMI_NAS_GET_FORBIDDEN_NETWORKS_V01
#define REMOVE_QMI_NAS_GET_PREFERRED_NETWORKS_V01
#define REMOVE_QMI_NAS_SET_FORBIDDEN_NETWORKS_V01
#define REMOVE_QMI_NAS_SET_PREFERRED_NETWORKS_V01
//#define REMOVE_QMI_NAS_GET_HOME_NETWORK_V01
#define REMOVE_QMI_NAS_GET_SSAC_INFO_V01
#define REMOVE_QMI_NAS_GET_SERV_CELL_SIB_V01
#define REMOVE_QMI_NAS_SET_APP_PRIORITY_V01
#define REMOVE_QMI_NAS_SET_DRX_V01
#define REMOVE_QMI_NAS_GET_DRX_V01
#endif


#if defined(FEATURE_MMODE_QMI_LTE_ONLY)
#define REMOVE_QMI_NAS_GET_ERR_RATE_V01
#define REMOVE_QMI_NAS_GET_SERVING_SYSTEM_V01
#endif

#ifndef FEATURE_CDMA
#define REMOVE_QMI_NAS_GET_ACCOLC_V01
#define REMOVE_QMI_NAS_SET_ACCOLC_V01
#define REMOVE_QMI_NAS_GET_NETWORK_SYSTEM_PREFERENCE_V01
#define REMOVE_QMI_NAS_GET_DEVICE_CONFIG_V01
#define REMOVE_QMI_NAS_SET_DEVICE_CONFIG_V01
#define REMOVE_QMI_NAS_GET_AN_AAA_STATUS_V01
#define REMOVE_QMI_NAS_SET_DDTM_PREFERENCE_V01
#endif

#ifndef FEATURE_CDMA
#define REMOVE_QMI_NAS_GET_3GPP2_SUBSCRIPTION_INFO_V01
#define REMOVE_QMI_NAS_SET_3GPP2_SUBSCRIPTION_INFO_V01
#define REMOVE_QMI_NAS_GET_MOB_CAI_REV_V01
#define REMOVE_QMI_NAS_GET_RTRE_CONFIG_V01
#define REMOVE_QMI_NAS_SET_RTRE_CONFIG_V01
#endif
#define REMOVE_QMI_NAS_BIND_SUBSCRIPTION_V01
#define REMOVE_QMI_NAS_SET_DUAL_STANDBY_PREF_V01
#ifndef FEATURE_HDR
#define REMOVE_QMI_NAS_GET_HDR_SUBTYPE_V01
#define REMOVE_QMI_NAS_GET_HDR_COLOR_CODE_V01
#endif
#ifndef FEATURE_CDMA
#define REMOVE_QMI_NAS_UPDATE_AKEY_EXT_V01
#endif
#ifndef FEATURE_DUAL_SIM
#define REMOVE_QMI_NAS_GET_DUAL_STANDBY_PREF_V01
#endif
#define REMOVE_QMI_NAS_CONFIG_EMBMS_V01
#define REMOVE_QMI_NAS_GET_EMBMS_STATUS_V01
#ifndef FEATURE_CDMA
#define REMOVE_QMI_NAS_GET_CDMA_POSITION_INFO_V01
#endif
//#define REMOVE_QMI_NAS_GET_MANAGED_ROAMING_CONFIG_V01
#define REMOVE_QMI_NAS_FORCE_NETWORK_SEARCH_V01
#ifndef FEATURE_TDSCDMA
#define REMOVE_QMI_NAS_GET_TDS_CELL_AND_POSITION_INFO_V01
#endif
#define REMOVE_QMI_NAS_GET_EMBMS_SIG_V01
#if !defined(FEATURE_MMODE_CDMA_800) && !defined(FEATURE_MMODE_CDMA_1900)
#define REMOVE_QMI_NAS_CDMA_AVOID_SYSTEM_V01
#define REMOVE_QMI_NAS_GET_CDMA_AVOID_SYSTEM_LIST_V01
#endif
#define REMOVE_QMI_NAS_SET_BUILTIN_PLMN_LIST_V01
#define REMOVE_QMI_NAS_SET_E911_STATE_V01
#define REMOVE_QMI_NAS_GET_EMBMS_SIG_EXT_V01
#define REMOVE_QMI_NAS_GET_SUBSCRIPTION_INFO_V01
#define REMOVE_QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_V01
#define REMOVE_QMI_NAS_IMS_PROCEED_WITH_SUBSCRIPTION_CHANGE_V01
#define REMOVE_QMI_NAS_CSG_SEARCH_SELECTION_CONFIG_V01
#define REMOVE_QMI_NAS_CSG_IMMEDIATE_SEARCH_SELECTION_V01
#define REMOVE_QMI_NAS_GET_CSG_SEARCH_SELECTION_CONFIGURATION_V01
#define REMOVE_QMI_NAS_GET_LTE_EMBMS_INFO_V01
#define REMOVE_QMI_NAS_GET_SERV_CELL_SIB_V01
#define REMOVE_QMI_NAS_GET_SSAC_INFO_V01
#define REMOVE_QMI_NAS_SET_PERIODIC_SEARCH_ALLOWED_V01
#ifndef FEATURE_MMODE_DUAL_SIM
#define REMOVE_QMI_NAS_SET_DATA_SUBS_PRIORITY_V01
#define REMOVE_QMI_NAS_GET_DATA_SUBS_PRIORITY_V01
#endif
#define REMOVE_QMI_NAS_SET_SRVCC_V01
#ifndef FEATURE_CDMA
#define REMOVE_QMI_NAS_SET_BSR_TIMER_V01
#define REMOVE_QMI_NAS_GET_BSR_TIMER_V01
#endif
#ifndef FEATURE_MMODE_DUAL_SIM
#define REMOVE_QMI_NAS_AVOID_TUNEAWAY_V01
#endif
//#define REMOVE_QMI_NAS_SET_MCC_V01
#define REMOVE_QMI_NAS_GET_CALL_MODE_V01
#define REMOVE_QMI_NAS_VOLTE_STATE_NOTIFICATION_V01
#define REMOVE_QMI_NAS_IMS_CALL_STATE_NOTIFICATION_V01
#define REMOVE_QMI_NAS_SET_HPLMN_IRAT_SEARCH_TIMER_V01
#define REMOVE_QMI_NAS_SET_HPLMN_SEARCH_TIMER_V01
#define REMOVE_QMI_NAS_GET_HPLMN_SEARCH_TIMER_V01
#define REMOVE_QMI_NAS_SET_DATA_ROAMING_V01
#define REMOVE_QMI_NAS_GET_DATA_ROAMING_V01
#define REMOVE_QMI_NAS_SET_RX_DIVERSITY_V01
#endif

/*---------------------------------------------------------------------------
  Service management
---------------------------------------------------------------------------*/
#if defined(FEATURE_MMODE_QMI_LTE_ONLY) || defined(FEATURE_MMODE_QMI_GPRS_LTE)
#define NASI_MAX_CLIDS   (4)
#else
#define NASI_MAX_CLIDS   (10)
#endif

#define QMI_NAS_ARR_SIZE( a ) ( sizeof( (a) ) / sizeof( (a[0]) ) )
#define QMI_NAS_XOR(a,b) ((!((a)&&(b))) && ((a)||(b)))

/*--------------------------------------------------------------------------- 
  MMGSDI relaged defines, structs and enums
---------------------------------------------------------------------------*/
#define QMI_NAS_MMGSDI_LEN_EF_SPN      (17)
#define QMI_NAS_MMGSDI_REC_SIZE_EF_OPL (8)
#define QMI_NAS_MMGSDI_LEN_EF_OPL      (512)
#define QMI_NAS_MMGSDI_LEN_EF_PNN      (512)
#define QMI_NAS_MMGSDI_LEN_EF_ONS      (128)
#define QMI_NAS_MMGSDI_LEN_EF_CSP      (2)
#define QMI_NAS_MMGSDI_LEN_EF_AD       (4) // saving only first 4 bytes till length of MNC
#define QMI_NAS_MMGSDI_LEN_EF_IMSI     (9)
#define QMI_NAS_MMGSDI_LEN_EF_CDMA_SPN (35)

#define QMI_NAS_MMGSDI_BITMAP_EF_SPN  0x00000001
#define QMI_NAS_MMGSDI_BITMAP_EF_OPL  0x00000002
#define QMI_NAS_MMGSDI_BITMAP_EF_PNN  0x00000004
#define QMI_NAS_MMGSDI_BITMAP_EF_ONS  0x00000008
#define QMI_NAS_MMGSDI_BITMAP_EF_CSP  0x00000010
#define QMI_NAS_MMGSDI_BITMAP_EF_AD   0x00000020
#define QMI_NAS_MMGSDI_BITMAP_EF_IMSI 0x00000040



/*--------------------------------------------------------------------------
  MMODE QMI NAS msgr definitions
---------------------------------------------------------------------------*/

#define MSGR_MODULE_NAS    0x03
#define MSGR_QMI_NAS       MSGR_TECH_MODULE( MSGR_TECH_QMI, MSGR_MODULE_NAS )

#define JAMMING_STATUS_NOT_FOUND 0
#define JAMMING_STATUS_UNKNOWN 2

struct valid_success_s {
  boolean valid;
  boolean success;
};

enum qmi_nas_mmgsdi_tristate_s
{
  QMI_NAS_MMGSDI_NOT_INIT = 0,
  QMI_NAS_MMGSDI_VALID, // cache valid
  QMI_NAS_MMGSDI_ERROR  // file does not exist, etc.
};

enum qmi_nas_mmgsdi_cache_ef_e
{
  QMI_NAS_MMGSDI_EF_NONE,
  QMI_NAS_MMGSDI_EF_AD,   // moved to front so they get read first
  QMI_NAS_MMGSDI_EF_IMSI, // moved to front so they get read first
  QMI_NAS_MMGSDI_EF_PLMNWACT,
  QMI_NAS_MMGSDI_EF_PLMN,
  QMI_NAS_MMGSDI_EF_SPN,
  QMI_NAS_MMGSDI_EF_OPL,
  QMI_NAS_MMGSDI_EF_PNN,
  QMI_NAS_MMGSDI_EF_ONS,
  QMI_NAS_MMGSDI_EF_CSP,
  QMI_NAS_MMGSDI_EF_CDMA_SPN,
  QMI_NAS_MMGSDI_EF_ALL
};

#ifndef FEATURE_MMODE_DISABLE_UIM
struct qmi_nas_mmgsdi_cache_s
{
  boolean                        updating; // is in the middle of updating cache files

  enum qmi_nas_mmgsdi_tristate_s ef_spn_valid;
  uint32                         ef_spn_size;
  uint8                          ef_spn[QMI_NAS_MMGSDI_LEN_EF_SPN];
  boolean                        ef_spn_changed;

  enum qmi_nas_mmgsdi_tristate_s ef_opl_valid;
  uint32                         ef_opl_num_recs; // # of recs
  uint8                          ef_opl[QMI_NAS_MMGSDI_LEN_EF_OPL];
  boolean                        ef_opl_changed;
  unsigned int                   ef_opl_bytes_read;

  enum qmi_nas_mmgsdi_tristate_s ef_pnn_valid;
  uint32                         ef_pnn_rec_size; // size of a rec
  uint32                         ef_pnn_num_recs; // # of recs
  uint8                          ef_pnn[QMI_NAS_MMGSDI_LEN_EF_PNN];
  boolean                        ef_pnn_changed;
  uint32                         ef_pnn_bytes_read;

  enum qmi_nas_mmgsdi_tristate_s ef_ons_valid;
  uint32                         ef_ons_size;
  uint8                          ef_ons[QMI_NAS_MMGSDI_LEN_EF_ONS];
  boolean                        ef_ons_changed;

  enum qmi_nas_mmgsdi_tristate_s ef_csp_valid;
  uint32                         ef_csp_size;
  uint8                          ef_csp[QMI_NAS_MMGSDI_LEN_EF_CSP];
  boolean                        ef_csp_changed;

  enum qmi_nas_mmgsdi_tristate_s ef_ad_valid;
  uint32                         ef_ad_size;
  uint8                          ef_ad[QMI_NAS_MMGSDI_LEN_EF_AD];
//  boolean                        ef_ad_changed;

  enum qmi_nas_mmgsdi_tristate_s ef_imsi_valid;
  uint32                         ef_imsi_size;
  uint8                          ef_imsi[QMI_NAS_MMGSDI_LEN_EF_IMSI];
//  boolean                        ef_imsi_changed;

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
  enum qmi_nas_mmgsdi_tristate_s ef_cdma_spn_valid;
  uint32                         ef_cdma_spn_size;
  uint8                          ef_cdma_spn[QMI_NAS_MMGSDI_LEN_EF_CDMA_SPN];
#endif
  // EFs for SET_PREFERRED_NETWORKS, only need to cache size
  enum qmi_nas_mmgsdi_tristate_s ef_plmnwact_valid;
  uint32                         ef_plmnwact_size;

  enum qmi_nas_mmgsdi_tristate_s ef_plmn_valid;
  uint32                         ef_plmn_size;
};
#endif

typedef enum
{
  QMI_NAS_MMGSDI_SESSION_MIN    = -1,
  QMI_NAS_MMGSDI_GW_PRI_SESSION = 0,
  #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
  QMI_NAS_MMGSDI_1x_PRI_SESSION,
  #endif
  #ifdef FEATURE_DUAL_SIM
  QMI_NAS_MMGSDI_GW_SEC_SESSION,
  #endif
  #ifdef FEATURE_TRIPLE_SIM    
  QMI_NAS_MMGSDI_GW_TER_SESSION,
  #endif
  QMI_NAS_MMGSDI_SESSION_MAX
} qmi_nas_mmgsdi_session_e_type;


struct qmi_nas_mmgsdi_info_s
{
#ifndef FEATURE_MMODE_DISABLE_UIM
  mmgsdi_client_id_type         client_id;
  mmgsdi_session_id_type        session_id[QMI_NAS_MMGSDI_SESSION_MAX]; // support gw_pri, 1x_pri and gw_sec
  mmgsdi_session_type_enum_type session_type[QMI_NAS_MMGSDI_SESSION_MAX]; // support gw_pri, 1x_pri and gw_sec
  #endif
  mmgsdi_app_enum_type          app_type[QMI_NAS_MMGSDI_SESSION_MAX]; // support gw_pri, 1x_pri and gw_sec
  boolean                       fcn_refresh[QMI_NAS_MMGSDI_SESSION_MAX]; // is the REFRESH due to FCN?
  #ifndef FEATURE_MMODE_DISABLE_UIM
  struct qmi_nas_mmgsdi_cache_s cache[QMI_NAS_MMGSDI_SESSION_MAX]; // cache for files of interest
  #endif
  mmgsdi_events_enum_type       latest_evt[QMI_NAS_MMGSDI_SESSION_MAX]; // last received event to see if the session is usable or not
  boolean                       operation_ready[QMI_NAS_MMGSDI_SESSION_MAX]; // if last received event indicates SIM intialized and ready
  boolean                       session_active[QMI_NAS_MMGSDI_SESSION_MAX];
  #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
  boolean                       csp_req_buffer_valid;
  qmi_nas_mmgsdi_session_e_type csp_req_buffer_session;
  qmi_mmode_qcsi_transaction_type             *csp_req_buffer;
  #endif
};

typedef struct qmi_nas_time_and_time_zone_s
{
  uint8                                     year;       /**< Year field. */
  uint8                                     month;      /**< Month field. */
  uint8                                     day;        /**< Day field. */
  uint8                                     hour;       /**< Hour field. */
  uint8                                     minute;     /**< Minute field. */
  uint8                                     second;     /**< Second field. */
  int8                                     time_zone;  /**< Timezone field. */
} qmi_nas_time_and_time_zone_s_type;


struct qmi_nas_nitz_time_info_s
{
  uint8 radio_if;
  #if defined(FEATURE_CDMA)
  struct
  {
    boolean              is_valid;
    byte                 lp_sec;
    int8                time_zone;
    byte                 daylt_savings;
  } cdma_cache;
  #endif
  #if defined(FEATURE_HDR)
  struct
  {
    boolean             is_valid;
    byte lp_sec;
    int8 time_zone;
    byte daylt_savings;
  } hdr_cache;
  #endif
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_LTE)
  struct
  {
      boolean             is_valid;

      boolean                                   univ_time_and_time_zone_avail;
      qmi_nas_time_and_time_zone_s_type         univ_time_and_time_zone;

      boolean                                   time_zone_avail;
      int8                        time_zone;

      boolean                                   daylight_saving_adj_avail;
      uint8            daylight_saving_adj;

  } umts_cache;
  #endif
};


struct qmi_nas_ssac_change_info_s
{
  lte_rrc_ssac_params_s ssac_cache;
  boolean is_valid;
};

/*--------------------------------------------------------------------------- 
  QMI NAS defines, structs and enums
---------------------------------------------------------------------------*/
#define QMI_NAS_NET_DESC_LEN_MAX  256

#define MAX_NUM_SIG_INFO_THRESHOLDS 16
#define MAX_NUM_SIG_INFO2_THRESHOLDS 32
#define MAX_RSSI_EVENTS 10
#define MAX_CM_SS_EVENTS 50
#define MAX_MODEM_STATS_EVENT 20
#define QMI_NAS_WCDMA_MAX_MONITORED_LTE_CELLS 32

/*---------------------------------------------------------------------------
  3GPP PLMN name encoding
---------------------------------------------------------------------------*/
typedef enum
{
  NASI_PLMN_NAME_ENC_GSM_DEFAULT = 0,
  NASI_PLMN_NAME_ENC_UCS2_16     = 1,
  NASI_PLMN_NAME_ENC_MAX,
  NASI_PLMN_NAME_ENC_MAX32       = 0x10000000
} qmi_nasi_plmn_name_enc_e_type;

/* Need to update this struct whenever tlvs/fields are added/modified in 
   phone msg/ind */
typedef struct qmi_nasi_ph_info_s
{
  uint8       curr_nam;
  uint8       cur_ddtm_status;
  uint8       ddtm_pref;
  uint16      ddtm_act_mask;
  uint8       ddtm_num_srv_opt;
  uint32      ddtm_srv_opt_mask;
} qmi_nasi_ph_info_type; 

#define NASI_MAX_LEN_NETWORK_NAME (255)

typedef struct qmi_nasi_rssi_info_stack_s
{
  uint64              signal_strength_changed_fields;
  sys_sys_mode_e_type sys_mode;
  uint16              rssi;
  byte                sinr;
  #ifndef FEATURE_MMODE_QMI_LTE_ONLY
  uint16              ecio;
  int                 io;
  #endif
  int16               rsrp;
  int8                rsrq;
  uint8               bit_err_rate;
  #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
  uint16              frame_err_rate;
  #endif
  #if defined(FEATURE_HDR)
  boolean             hdr_hybrid;
  uint16              hdr_packet_err_rate;
  #endif
} qmi_nasi_rssi_info_stack_type;

typedef enum
{
  QMI_NASI_MMGSDI_PLMN_UNKNOWN = 0,
  QMI_NASI_MMGSDI_PLMN_EF_NOT_FOUND,
  QMI_NASI_MMGSDI_PLMN_EF_FOUND,
  QMI_NASI_MMGSDI_PLMN_EF_READ_REQUESTED,
  QMI_NASI_MMGSDI_PLMN_EF_FOUND_AND_READ,
  QMI_NASI_MMGSDI_PLMN_EF_WRITE_REQUESTED,
  QMI_NASI_MMGSDI_PLMN_MAX = 0xff
} qmi_nasi_mmgsdi_plmn_ef_status_e_type;

typedef struct
{
  sys_plmn_id_s_type plmn;
#define QMI_NASI_MMGSDI_ACT_LENGTH (2)
  uint8              act[QMI_NASI_MMGSDI_ACT_LENGTH];
} qmi_nasi_mmgsdi_plmn_wact_type;

enum init_state {
  INIT_STATE_NOT_INITIALIZED         = 0,
  INIT_STATE_PENDING_MODE_CAPABILITY,
  INIT_STATE_INITIALIZED
};

enum qmi_nas_subs_e
{
  QMI_NAS_SUBS_NONE      = -1,
  QMI_NAS_SUBS_PRIMARY   = 0,
#ifdef FEATURE_DUAL_SIM
  QMI_NAS_SUBS_SECONDARY = 1,
#endif
#ifdef FEATURE_TRIPLE_SIM
  QMI_NAS_SUBS_TERTIARY  = 2,
#endif
  QMI_NAS_SUBS_MAX
};

enum qmi_nas_stack_e
{
  QMI_NAS_STACK_NONE = -1,
  QMI_NAS_STACK_MAIN = 0,
  QMI_NAS_STACK_HYBRID,
  QMI_NAS_STACK_GW_HYB,
  QMI_NAS_STACK_GW_HYB3
};

/* QMI NAS Client Data Types */
#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
#define MAX_NUM_SIG_INFO_THRESHOLDS 16
#endif
#define MAX_NUM_SIG_INFO2_THRESHOLDS 32

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
struct config_sig_s
{
  uint32 threshold_list_len;
  float  threshold_list[MAX_NUM_SIG_INFO_THRESHOLDS];
};
#endif
struct config_sig2_s
{
  float  delta;
  uint32 threshold_list_len;
  float  threshold_list[MAX_NUM_SIG_INFO2_THRESHOLDS];
};

typedef struct qm_nas_clnt_config_s
{
  boolean use_config_sig2;
  #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
  // config sig info information
  struct config_sig_s config_sig[QM_SIG_TYPE_MAX];
  #endif
  #if defined(FEATURE_TDSCDMA)
  struct config_sig_s tds_sinr_config;
  #endif

  // config_sig_info2 information
  struct config_sig2_s config_sig2[QM_SIG_TYPE_MAX][QM_RAT_MAX];
} qm_nas_clnt_config_s_type;


typedef struct qmi_nasi_rssi_info_s
{
  #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
  uint8 number_of_stacks;
  #endif
  qmi_nasi_rssi_info_stack_type stack_info[STACK_INDEX_MAX];
} qmi_nasi_rssi_info_type;

struct qmi_nas_nw_desc_s
{
  uint8 desc[QMI_NAS_NET_DESC_LEN_MAX];
};

struct current_plmn_name_s
{
  boolean                            current_plmn_valid;
  sys_plmn_id_s_type                 plmn;
  mmgsdi_spn_info_type               srv_prov_name;
  mmgsdi_plmn_short_name_static_type plmn_short_name;
  mmgsdi_plmn_long_name_static_type  plmn_long_name;
};

struct qmi_nas_queue_management_s
{
  // counters signed due to possibility of being -1 for short period of time
  int                rssi_cnt;
  int                cm_ss_event_cnt;
  int                modem_stats_cnt;
  rex_crit_sect_type cm_ss_cs;
  rex_crit_sect_type cm_stats_cs;
};

struct lte_sib16_nw_time_s
{
  boolean                is_valid;
  sys_modem_as_id_e_type asubs_id;
  boolean                lte_sib16_acquired;
  boolean                lte_dayLightSavingTime_enabled;
  uint8                  dayLightSavingTime;
  boolean                lte_leapSeconds_enabled;
  int8                   leapSeconds;
  boolean                lte_localTimeOffset_enabled;
  int8                   localTimeOffset;
} ;

/** @brief structure contains number of builtin plmn ids and 
    mmgsdi_builtin_plmn_id_type list
*/
typedef struct qmi_nas_builtin_plmn_list_s
{
  uint32                       oplmn_list_len;  /**< Number of plmn ids. */
  mmgsdi_builtin_plmn_id_type  oplmn_entry[NAS_MAX_BUILTIN_OPLMN_ENTRIES_V01];    /**< Pointer to plmn id list. */
} qmi_nas_builtin_plmn_list_type;

typedef enum 
{
  QMI_NAS_INCREMENTAL_NET_SCAN_STATUS_NONE = 0,

  /* Search is still in progress, send indication */  
  QMI_NAS_INCREMENTAL_NET_SCAN_STATUS_INPROGRESS, 
  /* Search is aborted, last indication */  
  QMI_NAS_INCREMENTAL_NET_SCAN_STATUS_AS_ABORT,
  /* Search is done, last indication */
  QMI_NAS_INCREMENTAL_NET_SCAN_STATUS_DONE, 
}qmi_nas_incremental_net_scan_status_e_type;

/** @brief Type to hold phone information. received in phone
    events.
*/

typedef struct qmi_cm_ph_info_s {

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  **            Phone State
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sys_oprt_mode_e_type              oprt_mode;
    /**< Indicates the current operating mode. */

  sys_sys_mode_mask_e_type          mode_capability;
    /**< Indicates the mode capability. */

  sys_lte_band_mask_e_type              lte_band_capability;
    /**< Indicates the LTE band capability. */

  sys_ue_usage_setting_e_type       ue_usage_setting;
    /**< UE Usage setting*/

  sys_voice_domain_pref_e_type      voice_domain_pref;
    /**< Voice domain pref*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  **            Preferences per Subscription
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type            asubs_id;
    /**< Subscription ID to which these preferences should apply. */
#endif

  cm_mode_pref_e_type               mode_pref;
    /**< Indicates the current mode preference. */

  cm_pref_term_e_type               pref_term;
    /**< Indicates the current preference term (e.g., one-call, power-cycle).*/

  cm_srv_domain_pref_e_type         srv_domain_pref;
    /**< The service domain preferred by the client. This field is set after a
         call to cm_ph_cmd_sys_sel_pref (GSM/WCDMA/TDS only). */

  sys_lte_band_mask_e_type               lte_band_pref;
    /**< Indicates the current LTE band preference. Used
         with CM_BAND_PREF_LTE_* values. */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref;
    /**< Defines whether the network should be selected automatically
         or manually. */

  sys_plmn_id_s_type                plmn;
    /**< If the network selection is set to manual,
         this specifies the PLMN ID. */

  sys_found_plmn_list_u_type     *p_available_networks;
    /**< Available networks (GSM/WCDMA/TDS only). */
 
  cm_network_list_type_e_type    network_list_type;   
 
  sys_plmn_list_status_e_type       available_networks_list_cnf;
    /**< Provides the status of a PLMS list request (success/abort).\
         Clients must check for CM_API_AVAIL_NET_LIST_CNF
         before using this field. */

  sys_drx_cn_coefficient_s1_e_type  drx_coefficient;
    /**< The DRX value NAS cnf for GET_DRX_REQ */

  sys_radio_access_tech_e_type  rat_type_edrx; 
   /* RAT Type for EDRX */ 
 
  boolean       edrx_enabled; 
      /* Indicates EDRX is enabled or disabled*/ 
          
  uint8        edrx_ptw; 
    /* Paging Time window for EDRX*/ 
    
  uint8        edrx_cycle_length; 
      /* Cycle Length for EDRX*/ 

  
   uint16      change_mask_edrx; 
  /*Mask to indicate which parameter for EDRX has change*/ 

  #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 

  cm_roam_pref_e_type               roam_pref;
    /**< Indicates the current roam preference. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  **            Subscription Info
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_rtre_config_e_type             rtre_config;
    /**< Current RTRE configuration; RUIM, NV, or RUIM fallback NV (CDMA only). */

  cm_rtre_control_e_type            rtre_control;
    /**< Current RTRE control; RUIM or NV (CDMA only). */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  **            DDTM Info
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_ddtm_pref_e_type               ddtm_pref;
    /**< Data Dedicated Tranmission Mode (DDTM) preference. */

  boolean                           cur_ddtm_status;
    /**< DDTM status. */

  sys_ddtm_act_mask_e_type          ddtm_act_mask;
    /**< DDTM action mask settings. */

  uint16                            ddtm_num_srv_opt;
    /**< Number of service options specified in ddtm_srv_opt_list. */


  sys_srv_opt_type             ddtm_srv_opt_list[SYS_DDTM_MAX_SO_LIST_SIZE];
    /**< List of service options whose pages have to be ignored, provided
         ddtm_pref and ddtm_act_mask allow it. */
   
  cm_prl_pref_e_type                prl_pref;
    /**< Indicates the current PRL preferred setting. */

   #endif
 #ifdef FEATURE_DUAL_SIM

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  **            Dual Standby Info
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sys_modem_dual_standby_pref_e_type     standby_pref;
      /**< Standby preference of the phone. */

  sys_modem_as_id_e_type                 active_subs;
      /**< The active subscription in Single Standby mode.\
           Only valid when standby_pref is SYS_MODEM_DS_PREF_SINGLE_STANDBY. */

  sys_modem_as_id_e_type                 default_voice_subs;
      /**< Default voice subscription. */

  sys_modem_as_id_e_type                 default_data_subs;
      /**< Default data subscription. */


  sys_data_priority_e_type  priority_type; 

  sys_modem_as_id_e_type                 priority_subs;
    /**< Priority subscription. */

  #endif

  #ifndef FEATURE_MMODE_QMI_LTE_ONLY

  sys_band_mask_e_type              band_capability;
    /**< Indicates 2G and 3G (excluding TD-SCDMA) band capability. */

#ifdef FEATURE_TDSCDMA
  sys_band_mask_e_type              tds_band_capability;
    /**< Indicates the TD-SCDMA band capability. */
#endif
#ifdef FEATURE_CDMA
  cm_nam_e_type                     curr_nam;
    /**< Indicates the current NAM - needed when NAM selection
         is set to AUTO-NAM.\ CDMA only. */
#endif
  cm_gw_acq_order_pref_e_type       acq_order_pref;
    /**< The acquisition order preferred by the client (GSM/WCDMA/TDS only). */

  cm_band_pref_e_type               band_pref;
    /**< Indicates the current 2G and 3G (excluding TD-SCDMA)
         band preference. Used with CM_BAND_PREF_* values.  */
#ifdef FEATURE_TDSCDMA
  cm_band_pref_e_type               tds_band_pref;
    /**< Indicates the current TD-SCDMA band preference. Used
         with CM_BAND_PREF_TDS_* values. */
#endif
  #if !defined(FEATURE_MMODE_QMI_GPRS_LTE)
  sys_csg_id_type                   csg_id;
    /**< If the MANUAL CSG Selection is successful,
         this specifies the CSG ID selected */

  sys_radio_access_tech_e_type       csg_rat;
    /**< RAT specified for CSG */
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   **            Device Mode
   **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  uint32                                 hplmn_timer;
    /**< hplmn timer value */
  #endif

  cm_acq_pri_order_pref_s_type          acq_pri_order_pref;
    /**< Rat acquisition priority order preference */

  cm_ciot_user_lte_pref_s_type           ciot_lte_pref;

} qmi_cm_ph_info_s_type;

typedef struct qmi_nas_jamming_info{
  uint32 jammer_status;
  boolean soft_jammer_flag;
}qmi_nas_jamming_info_s_type;

enum iot_rat{
 IOT_RAT_GSM,
 IOT_RAT_LTE_M1,
 IOT_RAT_LTE_NB1,
 IOT_RAT_MAX
};

struct qmi_nasi_global_s
{

#ifdef FEATURE_DUAL_SIM
  cm_mm_msim_ss_info_s_type     cm_msim_ss_info[QMI_NAS_SUBS_MAX];
#endif

#ifndef FEATURE_MMODE_DISABLE_UIM
  struct qmi_nas_mmgsdi_info_s mmgsdi;
#else
  //mmgsdi_session_id_type           mmgsdi_session_id;
  //mmgsdi_client_id_type            mmgsdi_client_id;
  //struct qmi_nas_mmgsdi_info_s *mmgsdi;
#endif

#ifdef FEATURE_DUAL_SIM
  // latest CM SUBS information
  cm_subs_info_s_type subs_info[QMI_NAS_SUBS_MAX];
#endif
  struct qmi_nas_nw_desc_s   prev_nw_desc[QMI_NAS_SUBS_MAX]; // last reported network description
  qmi_cm_ph_info_s_type   cm_ph_info;      /* for asid 1 */
#ifdef  FEATURE_DUAL_SIM
  qmi_cm_ph_info_s_type   cm_ph_info2;     /* for asid 2 */
#endif
#ifdef FEATURE_TRIPLE_SIM
  qmi_cm_ph_info_s_type   cm_ph_info3;     /* for asid 2 */
#endif

  struct ims_pref_s
  {
    boolean lte_mask_valid;
    uint64  lte_call_mask;
#if ( defined(FEATURE_LTE) && defined(FEATURE_HDR))
    boolean hdr_mask_valid;
    uint64  hdr_call_mask;
#endif
#if ( defined(FEATURE_LTE) && defined(FEATURE_GSM))
    boolean gsm_mask_valid;
    uint64  gsm_call_mask;
#endif
#if( defined(FEATURE_LTE) && defined(FEATURE_WCDMA))
    boolean wcdma_mask_valid;
    uint64  wcdma_call_mask;
#endif
#if( defined(FEATURE_LTE) && defined(FEATURE_CDMA))
    boolean cdma_mask_valid;
    uint64  cdma_call_mask;
#endif
#if( defined(FEATURE_LTE) && defined(FEATURE_TDSCDMA))
    boolean tds_mask_valid;
    uint64  tds_call_mask;
#endif
  } ims_pref_cache;

#ifndef FEATURE_DUAL_SIM
  cm_mm_ss_info_s_type     cm_ss_info[QMI_NAS_SUBS_MAX];
#endif

// Current PLMN information
  struct current_plmn_name_s current_plmn_name_cache;
  struct current_plmn_name_s current_plmn_name_cache_ignore;
#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
  struct current_plmn_name_s current_plmn_name_cache_sec_stack;
  struct current_plmn_name_s current_plmn_name_cache_sec_stack_ignore;
#endif

  // CM SS Event management
  struct qmi_nas_queue_management_s cm_event_queue;
  struct get_plmn_name_s
  {
    mmgsdi_eons_name_type last_reported_srv_prov_name;
    mmgsdi_plmn_name_type last_reported_plmn_short_name;
    mmgsdi_plmn_name_type last_reported_plmn_long_name;
  } get_plmn_name;

#ifndef FEATURE_MMODE_QMI_LTE_ONLY
#ifndef REMOVE_QMI_NAS_SET_BUILTIN_PLMN_LIST_V01
  struct builtin_plmn_list_s
  {
    qmi_client_handle clnt;
    uint32 list_id;
    uint16 oplmn_len_total; //cumulative when list is recv in multiple msgs
    uint16 total_entries;//Total list entries expected
    boolean list_op_in_prog;
    qmi_nas_builtin_plmn_list_type *list_ptr;
    boolean ind_tok_valid;
    uint32 ind_token;
  }builtin_plmn_list[QMI_NAS_SUBS_MAX];
#endif
#endif

#ifndef FEATURE_MMODE_DISABLE_UIM
  struct
  {
    qmi_nasi_mmgsdi_plmn_ef_status_e_type plmn;
    qmi_nasi_mmgsdi_plmn_ef_status_e_type plmn_wact;
    qmi_nasi_mmgsdi_plmn_ef_status_e_type oplmn_wact;
    sys_plmn_id_s_type *plmn_list;
    qmi_nasi_mmgsdi_plmn_wact_type *plmn_wact_list;
    qmi_nasi_mmgsdi_plmn_wact_type *oplmn_wact_list;
    uint8 plmn_list_size;
    uint8 plmn_wact_list_size;
    uint8 oplmn_wact_list_size;
  } pref_nw_scratch;
#endif


  lte_rrc_ac_barring_info_s lte_rrc_ac_barring_info_cache;

#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
  qmi_nasi_ph_info_type last_reported_nasi_ph_info;
  qmi_nasi_ph_info_type current_nasi_ph_info;
#endif

  struct lte_sib16_nw_time_s lte_sib16_nw_time_cache;
  mmgsdi_sim_busy_evt_info_type sim_busy_status;

  qmi_idl_service_object_type   svc_obj;

  qmi_mmode_qcsi_transaction_type *ims_cmd_buf_p;

  qmi_mmode_qcsi_transaction_type *get_lte_band_p;

  qmi_mmode_qcsi_transaction_type *get_serv_cell_sib;

  qmi_mmode_qcsi_transaction_type *set_drx_scaling_cmd_buf_p;

  qmi_mmode_qcsi_transaction_type *set_cell_lock_config_req_buf_p;

  qmi_mmode_qcsi_transaction_type *set_cell_config_req_buf_p;

  qmi_mmode_qcsi_transaction_type *set_drx_req_buf_p;

  qmi_mmode_qcsi_transaction_type *set_edrx_req_buf_p;
  


#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
  qmi_mmode_qcsi_transaction_type     *p_rtre_set_status_cmd_buf;
  boolean rtre_set_status_pending;
#endif

#ifndef REMOVE_QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_V01
  qmi_client_handle   incremental_net_scan_clnt;
#endif

#ifndef FEATURE_MMODE_DISABLE_UIM

  struct
  {
    qmi_nasi_mmgsdi_plmn_ef_status_e_type plmn;
    qmi_nasi_mmgsdi_plmn_ef_status_e_type plmn_wact;
    qmi_nasi_mmgsdi_plmn_ef_status_e_type oplmn_wact;
  } mmgsdi_ef_info;
#endif

  qmi_mmode_qcsi_transaction_type *get_psm_cfg_params;

  
  qmi_mmode_qcsi_transaction_type *set_lte_band_p;

  qmi_mmode_qcsi_transaction_type *get_drx_cmd_buf_p;
  qmi_mmode_qcsi_transaction_type *get_edrx_cmd_buf_p;
  cm_client_id_type            cm_clnt_id;
  void    *cmd_buf_p;
#ifndef REMOVE_QMI_NAS_GET_EMBMS_SIG_EXT_V01
  void       *embms_ext_cmd_buf_p;
#endif

#ifndef REMOVE_QMI_NAS_SET_EVENT_REPORT_V01
  int16  min_rsrp_delta;
#endif
#if !(defined(FEATURE_MMODE_QMI_LTE_ONLY) || defined(FEATURE_MMODE_QMI_GPRS_LTE))
#ifdef FEATURE_LTE_EMBMS  
  int16  trace_id;
#endif
#endif

  // latest CM SS information
  boolean                    rcvd_ss_info; // whether at least one ss event is received


  //uint8                      prev_nw_desc[QMI_NAS_SUBS_MAX][QMI_NAS_NET_DESC_LEN_MAX]; // last reported network description

  // latest CM PH information
  boolean             rcvd_ph_info; // whether at least one ph event for asid 1 is received
  boolean  net_scan_started;
  boolean  get_pref_nwks_req_pending;

  // save last state before CM moves sim_state to SYS_SIM_STATE_NOT_AVAILABLE
  // [0] for sim_state and [1] for gw_sim_state
  sys_sim_state_e_type sim_state_before_not_avail[QMI_NAS_SUBS_MAX];

#ifndef REMOVE_QMI_NAS_SET_DATA_SUBS_PRIORITY_V01
  sys_data_priority_e_type data_subs_priority[QMI_NAS_SUBS_MAX];
#endif

#if !defined(REMOVE_QMI_NAS_GET_DATA_ROAMING_V01 ) || !defined(REMOVE_QMI_NAS_SET_DATA_ROAMING_V01)
  sys_data_roaming_e_type  data_roam_cache[QMI_NAS_SUBS_MAX];
  boolean                  data_roam_cache_valid[QMI_NAS_SUBS_MAX];
  sys_data_roaming_e_type  data_roam_temp;
#ifdef FEATURE_DUAL_SIM
  enum qmi_nas_subs_e      data_roam_temp_subs;
#endif
#endif


#ifndef FEATURE_MMODE_QMI_DISABLE_IMS_EXT
  boolean          call_mode_valid[QMI_NAS_SUBS_MAX];
  sys_call_mode_t  call_mode[QMI_NAS_SUBS_MAX];
#endif

  enum init_state  inited;

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
  sys_modem_as_id_e_type net_scan_asubs_id;
#endif
  cm_network_list_type_e_type net_scan_type;

#ifndef REMOVE_QMI_NAS_PERFORM_INCREMENTAL_NETWORK_SCAN_V01
  qmi_nas_incremental_net_scan_status_e_type  incremental_net_scan_status;
  
#endif

#ifndef REMOVE_QMI_NAS_SET_EVENT_REPORT_V01
  uint8    min_rssi_delta;
  uint8    min_io_delta;
  uint8    min_rsrq_delta;

#endif

  cm_mode_pref_e_type last_mode_pref;
#ifdef  FEATURE_DUAL_SIM
  boolean        rcvd_ph_info2; // whether at least one ph event for asid 2 is received
  cm_mode_pref_e_type last_mode_pref2;
#endif
#ifdef FEATURE_TRIPLE_SIM
  boolean        rcvd_ph_info3; // whether at least one ph event for asid 2 is received
  cm_mode_pref_e_type last_mode_pref3;
#endif
  // bolleans to ensure that cm_ph_cmd_signal_strength_delta is called only
  // once
  boolean is_set_cm_ph_cmd_signal_strength_delta;
  boolean is_pending_config_sig_info;

#if !(defined(FEATURE_MMODE_QMI_LTE_ONLY) || defined(FEATURE_MMODE_QMI_GPRS_LTE))
#ifdef FEATURE_LTE_EMBMS
  // embms status
  boolean embms_status; // true: enabled, false: disabled
#endif
#endif

#if !defined(REMOVE_QMI_NAS_GET_DATA_ROAMING_V01 ) || !defined(REMOVE_QMI_NAS_SET_DATA_ROAMING_V01)
  boolean           data_roam_temp_valid;
#endif

 qmi_mmode_qcsi_transaction_type * set_sfn_config_buf_p;
 qmi_mmode_qcsi_transaction_type * get_next_sfn_buf_p;

 cm_3gpp_service_status_s_type true_service_status;
 
 qmi_nas_jamming_info_s_type    jamming_status_info[IOT_RAT_MAX];   
#ifdef FEATURE_FSK_TX
 qmi_mmode_qcsi_transaction_type * fsk_data_buf_p;
 qmi_mmode_qcsi_transaction_type * fsk_set_hop_table_buf_p;
 qmi_mmode_qcsi_transaction_type * fsk_get_hop_table_buf_p;
 qmi_mmode_qcsi_transaction_type * fsk_start_buf_p;
 qmi_mmode_qcsi_transaction_type * fsk_set_debug_buf_p;  
 qmi_mmode_qcsi_transaction_type * fsk_get_debug_buf_p;
 qmi_mmode_qcsi_transaction_type * fsk_stop_buf_p;
 qmi_mmode_qcsi_transaction_type * fsk_set_pcl_buf_p;
 qmi_mmode_qcsi_transaction_type * fsk_get_pcl_buf_p;
 #endif
 qmi_mmode_qcsi_transaction_type * get_ce_level_info_buf_p;
 //qmi_mmode_qcsi_transaction_type * get_wwan_sleep_threshold_buf_p;
 //qmi_mmode_qcsi_transaction_type * set_wwan_sleep_threshold_buf_p;
 uint32                            wwan_sleep_theshold ;
 qmi_mmode_qcsi_transaction_type * get_cell_select_info_buf_p;
  #ifdef FEATURE_FMD_SPEED_INFO
 qmi_mmode_qcsi_transaction_type * get_fmd_speed_info_buf_p;  
  #endif
 
};

typedef struct qmi_nasi_cell_select_info_s
{
   boolean is_ue_camped;
   sys_sys_mode_e_type rat;
   uint8 q_hyst;
   int8 q_qualmin;
   int32 qrxlevmin;
   int32 qrxlevmin_ce;
   int32 qrxlevmin_ce1;
   uint8 rxlev_access_min;
} qmi_nasi_cell_select_info_type;
/*---------------------------------------------------------------------------
  QMI Network Access Service instance state definition &
  Network Access Service client state definition
---------------------------------------------------------------------------*/
typedef struct qmi_nasi_client_state_s
{

  q_link_type       link; /*< Data Structure required by the queue 
                              library to add this as part of a queue */
                                
  qmi_client_handle            clnt;   /* QCSI Client Handle */

  qm_nas_clnt_config_s_type    clnt_config_sig_info;
  qm_meas_send_info_type       qm_meas_info_send;
  
  struct report_status_s
  {
    // QMI_NAS_SET_EVENT_REPORT_REQ TLV

    // TLV 0x10 Signal Strength Indicator
    boolean     report_sig_str;
    uint8       num_thresholds;
    signed char sig_str_thresholds[NAS_SIG_STRENGTH_THRESHOLD_LIST_MAX_V01];

    // TLV 0x11 RF Band Information
    boolean     report_rf_band_info;

    // TLV 0x12 Registration Reject Reasons
    boolean     report_reg_reject_reason;

    // TLV 0x13 RSSI Indicator
    boolean     report_rssi;
    uint8       report_rssi_delta;
    signed char last_reported_rssi; 
    uint16      last_reported_rssi_ex; 
    
    #ifndef FEATURE_MMODE_QMI_LTE_ONLY
    #if defined(FEATURE_HDR)
    signed char last_reported_hdr_rssi;
    uint16      last_reported_hdr_rssi_ex; 
    #endif
    // TLV 0x14 and TLV 0x19 ECIO Indicator
    boolean     report_ecio;
    uint8       report_ecio_delta; //TLV 0x14 only
    uint16      last_reported_ecio;
    #if defined(FEATURE_HDR)
    uint16      last_reported_hdr_ecio;
    #endif
    uint8       reporting_mode_ecio;
    uint8       num_thresholds_ecio; //TLV 0x19 only
    int16       ecio_thresholds[NAS_ECIO_THRESHOLD_LIST_MAX_V01]; //TLV 0x19 only

    // TLV 0x15 IO Indicator
    boolean     report_io;
    uint8       report_io_delta;
    int         last_reported_io; 
    #if defined(FEATURE_HDR)
    int         last_reported_hdr_io;
    #endif
    #endif

    // TLV 0x16 and TLV 0x20 SINR Indicator
    boolean     report_sinr;
    uint8       report_sinr_delta; //TLV 0x16 only
    uint8       last_reported_sinr; 
    #if defined(FEATURE_HDR)
    uint8       last_reported_hdr_sinr;
    #endif
    uint8       reporting_mode_sinr; 
    uint8       num_thresholds_sinr;//TLV 0x20 only 
    uint8       sinr_thresholds[NAS_SINR_THRESHOLD_LIST_MAX_V01]; //TLV 0x20 only

    // TLV 0x17 Error Rate Indicator
    boolean     report_error_rate;

    // TLV 0x18 RSRQ Indicator
    boolean     report_rsrq;
    uint8       report_rsrq_delta;
    uint8       last_reported_rsrq; 

    // TLV 0x19
    boolean     report_lte_snr;
    uint8       report_lte_snr_delta;
    byte        last_reported_lte_snr; 

    // TLV 0x1A
    boolean     report_lte_rsrp;
    uint8       report_lte_rsrp_delta;
    int16       last_reported_lte_rsrp; 

    boolean     reg_sys_sel_pref;

    #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
    boolean     reg_current_nam;
    boolean     reg_ddtm_events;
    #endif

    #ifndef REMOVE_QMI_NAS_GET_SERVING_SYSTEM_V01
    boolean     not_report_serving_system; // 0 (default) implies reporting by default
    #endif

    #ifdef FEATURE_DUAL_SIM
    boolean     report_dual_standby_pref_ind;
    boolean     report_subscription_info_ind;
    #endif
    
    #if !(defined(FEATURE_MMODE_QMI_LTE_ONLY) || defined(FEATURE_MMODE_QMI_GPRS_LTE))
    boolean     report_thermal_emergency_state;
    #endif
    boolean     report_network_time;
    boolean     report_sys_info;
    boolean     report_sig_info;
    boolean     report_err_rate;
    #if defined(FEATURE_HDR)
    boolean     report_uati;          // HDR uati indication
    boolean     report_session_close; // HDR session close indication
    #endif

    #ifndef REMOVE_QMI_NAS_GET_MANAGED_ROAMING_CONFIG_V01
    boolean     report_managed_roaming;
    #endif
    
    boolean     report_current_plmn_name;
    #if !(defined(FEATURE_MMODE_QMI_LTE_ONLY) || defined(FEATURE_MMODE_QMI_GPRS_LTE))
    #ifdef FEATURE_LTE_EMBMS
    boolean     report_embms_status;        
    #endif
    #endif
    boolean     report_rf_band_info_ind;
    boolean     report_psm_config_params_change_ind;
    boolean     report_psm_status_ind;
    boolean     report_operating_mode_ind;

    enum qmi_nas_subs_e  bound_subs; // subscription which the client is bound to

    struct network_reject_s
    {
      boolean     report_network_reject_ind;
      boolean     suppress_sys_info_on_reject;
    } network_reject;

    boolean     report_operator_name_data;
    boolean     report_csp_plmn_mode_bit;  

    #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
    boolean     report_rtre_config;
    #endif

    uint64      limit_sys_info_reporting_mask;
    
    boolean     report_ims_pref_status;  
    
    #ifndef FEATURE_MMODE_QMI_DISABLE_IMS_EXT
    boolean     report_e911_state_ready;
    #endif
    
    boolean     report_lte_sib16_network_time;
    boolean     report_lte_cphy_ca_status;
    boolean     ignore_plmn_display_cond_in_ind;

    #ifndef FEATURE_MMODE_QMI_DISABLE_IMS_EXT
    boolean     report_subscription_change;
    #endif
    
    boolean     report_ssac_info;
    boolean     report_t3402_timer_change;
    boolean     reg_acb_info_change;
    boolean     report_edrx_change_info_ind;

    #ifndef REMOVE_QMI_NAS_GET_DATA_SUBS_PRIORITY_V01
    boolean     report_data_priority_subs_change;
    #endif

    boolean     report_t3346_timer_status_change;

    #ifndef FEATURE_MMODE_QMI_DISABLE_IMS_EXT
    boolean     report_call_mode_change;
    #endif

    boolean     report_ssac_change_info;

    boolean		report_lte_rrc_tx_info_ind;

    #if !(defined(FEATURE_MMODE_QMI_LTE_ONLY) || defined(FEATURE_MMODE_QMI_GPRS_LTE))
    boolean     report_manual_scan_fail;
    #endif

    boolean reg_sfn_threshold_ind;
    boolean reg_jamming_status_ind;  
    boolean reg_wwan_sleep_info_ind;
  } report_status; 
  
} qmi_nasi_client_state_type;

extern struct qmi_nasi_global_s qmi_nasi_global;

extern qmi_mmode_svc_info_type qmi_nas_state;

/*---------------------------------------------------------------------------
  BCD digit extraction
---------------------------------------------------------------------------*/
#define QMI_NAS_BCD_LOW_DIGIT(a)   ((a) & 0x0F)
#define QMI_NAS_BCD_HIGH_DIGIT(a) (((a) >> 4) & 0x0F)

struct access_tech_s
{
  int c; // cdma
  int h; // hdr
  int g; // gsm
  int w; // wcdma
  int l; // lte
  int t; // tdscdma
};

/*--------------------------------------------------------------------------- 
  MMGSDI get_operator_name client data information
---------------------------------------------------------------------------*/
enum qmi_nasi_calling_cmd
{
  NAS_CMD_MIN = -1,
  NAS_CMD_PERFORM_NETWORK_SCAN,
  NAS_CMD_GET_PLMN_NAME,
  NAS_CMD_GET_HOME_NETWORK,
  NAS_CMD_MAX
};

struct qmi_nasi_mmgsdi_client_data_s
{
  enum qmi_nasi_calling_cmd reason;
  void                     *cmd_buf_ptr;
  boolean                   send_all_info;
};

#define QMI_NAS_INVALID_LAC  (0xFFFF)

typedef struct
{
  #if defined(FEATURE_WCDMA)
  uint64  umts_band_pref;
  #endif
  uint64  lte_band_pref;
  uint32  search_timer;
  uint32  list_type;
  uint32  network_type;
  uint8  search_mode;
  uint8  asubs_id;
  uint8  select_config;
} qmi_nas_csg_search_select_config_s_type;

#ifndef FEATURE_MMODE_DISABLE_UIM
void qmi_nas_mmgsdi_cmd_rsp_cb( mmgsdi_return_enum_type status, mmgsdi_cnf_enum_type cmd, const mmgsdi_cnf_type *p_info );
#endif
void qmi_nas_cmph_evt_cb( cm_ph_event_e_type evt, const cm_ph_info_s_type *p_info);


//=========================================================================== 
// QMI NAS command handler types
//=========================================================================== 
enum qmi_nas_cmd_handler_type {
   QMI_NAS_CMD_HDR = 1,  // Process an HDR event

   QMI_NAS_CMD_MAX // Max enum value
};

//=========================================================================== 
// QMI NAS command handler payloads
//
// Define payloads here for each command handler type. We have choose to not
// combine these into a union here since some payloads are much larger (4k)
// than most others.
// Note: first parameter of each payload must be qmi_nas_cmd_handler_type
//=========================================================================== 

// HDR payload
struct qmi_nas_cmd_hdr_payload {
   enum qmi_nas_cmd_handler_type id;
#ifdef FEATURE_QMI_NAS_HDR
   hdrmc_cb_params_rsp_union_type parms;
#endif
};

/*===========================================================================
  FUNCTION QMI_NAS_INIT()

  DESCRIPTION
    Register the Wireless Data Service with QMUX for all applicable QMI links

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_nas_init
(
  void
);

/*===========================================================================
  FUNCTION QMI_NAS_GEN_SERVING_SYSTEM_IND()

  DESCRIPTION
    Called when the serving system info changes. 
    Causes serving_system_ind to be sent to all clients
    
  PARAMETERS
    None

  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_nas_generate_serving_system_ind
(
  void
);

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_RSSI_CHANGE_IND()

  DESCRIPTION
    Called when CM reports change in RSSI. It sends event report ind to 
    notify rssi change to registered control points
        
  PARAMETERS
    cur_rssi : current signal strength
    radio_if : radio technology in use

  RETURN VALUE
    None
                
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_nas_generate_rssi_change_ind
( 
  signed char  cur_rssi,
  uint8        radio_if
);

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_SYS_SEL_PREF_IND()

  DESCRIPTION
    Called when the system selection preference changes. 
    Causes serving_system_ind to be sent to all clients
    
  PARAMETERS
    CM PH event

  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_generate_sys_sel_pref_ind
(
  qmi_cm_ph_info_s_type  *p_info
);
 #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_CURRENT_NAM_IND()

  DESCRIPTION
    Called when the current nam changes. 
    Causes qmi_nasi_current_nam_ind to be sent to all clients
    
  PARAMETERS
    CM PH event

  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_generate_current_nam_ind
(
  qmi_cm_ph_info_s_type  *p_info
);

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_DDTM_IND()

  DESCRIPTION
    Called when the ddtm(Data Dedicated Transmission Model)status changes. 
    Causes qmi_nasi_ddtm_ind to be sent to all clients
    
  PARAMETERS
    CM PH event

  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_generate_ddtm_ind
(
  qmi_cm_ph_info_s_type  *p_info
);
#endif
/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_REG_REJECT_IND()

  DESCRIPTION
    Called when we receive a REG_REJECT event from CM to cache the reject
    reason.
    
  PARAMETERS
    None

  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_generate_reg_reject_ind
(
  const cm_reg_reject_info_s_type *   reg_reject_info
);

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_REG_ATTACH_RESP()

  DESCRIPTION
    Called when error_cb() supplied in cm_ph_cmd_sys_sel_pref() is called 
    by CM. Sends response to control point for the correspoding
    Register or Attach requests.
        
  PARAMETERS
    user_data   : user data
    ph_cmd_err  : error type
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_nas_generate_reg_attach_resp
(
  void *                user_data,
  cm_ph_cmd_err_e_type  ph_cmd_err
);

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_NET_SCAN_ERR_RESP()

  DESCRIPTION
    Called when error_cb() supplied in cm_ph_cmd_get_networks() is called 
    by CM. Sends error response to control point if error is reported by CM
    
  PARAMETERS
    user_data   : user data
    ph_cmd_err  : Error type

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_nas_generate_net_scan_err_resp
(
  void *               user_data,
  cm_ph_cmd_err_e_type ph_cmd_err
);

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_ERROR_RATE_IND()

  DESCRIPTION
    Called when we receive a RSSI change event from CM with changes to the
    error rate values.
    
  PARAMETERS
    fer - the frame error rate value

  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_generate_error_rate_ind
(
  uint16              err_rate,
  sys_sys_mode_e_type mode
);

#if defined(FEATURE_MMGSDI_SESSION_LIB)
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
/*===========================================================================
  FUNCTION QMI_NAS_FIND_PLMN_STATUS()

  DESCRIPTION
    Function to trigger GET_FILE_ATTR requests to MMGSDI to verify the
    presence of PLMN and PLMN_WACT files

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_find_plmn_status
(
  void
);
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
#endif /* defined(FEATURE_MMGSDI_SESSION_LIB) */

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_RF_INFO_IND()

  DESCRIPTION
    Called when either the current active band or channel changes.
    
  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_generate_rf_info_ind
(
  void
);

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
void qmi_nas_operator_name_data_ind( mmgsdi_session_type_enum_type session, sys_mm_information_s_type *p_mm_info );
#endif
void qmi_nas_csp_plmn_mode_bit_ind( mmgsdi_session_type_enum_type session );

#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 
void qmi_nas_set_rtre_config_resp(  boolean   result_status );
#endif

/*===========================================================================
  FUNCTION QMI_NAS_CMSS_CMD_CB()

  DESCRIPTION
    Handle CM SS command callback

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_cmss_cmd_cb
( void *p_data, 
  cm_ss_cmd_e_type cmd, 
  cm_ss_cmd_err_e_type err
);

/*===========================================================================
  FUNCTION QMI_NAS_CMPH_CMD_CB()

  DESCRIPTION
    Handle CM PH command callback

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_cmph_cmd_cb( void *p_data, cm_ph_cmd_e_type cmd, cm_ph_cmd_err_e_type err);

void qmi_nas_gen_serving_system_ind( cm_mm_ss_info_s_type *p_ss_info, qmi_cm_ph_info_s_type *p_ph_info, qmi_cm_ph_info_s_type *p_ph_info2 );
void qmi_nas_gen_current_plmn_name_ind( cm_mm_ss_info_s_type *p_ss_info, mmgsdi_session_type_enum_type session );

/*===========================================================================
  FUNCTION qmi_nas_get_subs_from_asid()

  DESCRIPTION
    Retrieve subscription information that is mapped to provided asubs_id

  PARAMETERS
    asubs_id

  RETURN VALUE
    subscription information that is mapped to provided asubs_id.
    QMI_NAS_SUBS_NONE if asubs_id is not operational
===========================================================================*/
enum qmi_nas_subs_e qmi_nas_get_subs_from_asid( sys_modem_as_id_e_type asubs_id );

/*===========================================================================
  FUNCTION qmi_nas_get_asid_from_subs()

  DESCRIPTION
    Retrieve asubs_id information that is mapped to provided subscription type

  PARAMETERS
    subscription type

  RETURN VALUE
    asubs_id information that is mapped to provided subscription information.
    SYS_MODEM_AS_ID_NONE if there is no asubs_id mapped to subscription type
===========================================================================*/
sys_modem_as_id_e_type qmi_nas_get_asid_from_subs( enum qmi_nas_subs_e subs_type );
#ifndef FEATURE_MMODE_DISABLE_UIM
extern boolean qmi_nas_mmgsdi_register_to_amss(void);
#endif
extern void qmi_nas_mmgsdi_init( struct qmi_nas_mmgsdi_info_s *p_data);
#ifndef FEATURE_MMODE_DISABLE_UIM

extern boolean qmi_nas_mmgsdi_ef_ready ( qmi_nas_mmgsdi_session_e_type qmi_session, uint32 bitmap );
extern boolean qmi_nas_mmgsdi_is_valid_session( mmgsdi_session_type_enum_type session );
extern boolean qmi_nas_mmgsdi_is_valid_qmi_session( qmi_nas_mmgsdi_session_e_type session );
extern boolean qmi_nas_mmgsdi_is_valid_3gpp_session( mmgsdi_session_type_enum_type session );
extern mmgsdi_session_type_enum_type qmi_nas_map_mmgsdi_session( qmi_nas_mmgsdi_session_e_type qmi_session );
extern qmi_nas_mmgsdi_session_e_type qmi_nas_map_mmgsdi_session_to_qmi( mmgsdi_session_type_enum_type session );
#endif
#if defined(FEATURE_HDR)

//=========================================================================== 
// FUNCTION: qmi_nas_hdr_process_evt_payload
//
// DESCRIPTION:
//   Process HDR event payload (called once per QMI CLID instance)
//
// RETURN:
//   None
//=========================================================================== 
//
void qmi_nas_hdr_process_evt_payload( qmi_nasi_client_state_type *cl_sp,
                                      const void *payload);
#endif
/*=========================================================================== 
  FUNCTION qmi_nas_process_mmgsdi_get_operator_name()
 
  DESCRIPTION
    Process information from mmgsdi_get_operator_name

  PARAMETERS
    p_nas: pointer to nas structure in command queue item

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_process_mmgsdi_get_operator_name ( 
  const mmgsdi_cnf_type        *cnf_data
);
#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
/*=========================================================================== 
  FUNCTION qmi_nas_process_mmgsdi_get_operator_name()
 
  DESCRIPTION
    Process information from mmgsdi_get_operator_name

  PARAMETERS
    p_nas: pointer to nas structure in command queue item

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nas_process_mmgsdi_get_se13_plmn_names ( 
  const mmgsdi_cnf_type        *cnf_data
);
#endif
extern void qmi_nas_nw_name_init(void);

extern void qmi_nas_nw_name_add(
  mmgsdi_session_type_enum_type session,
  mmgsdi_eons_evt_info_type     evt_info
);

extern mmgsdi_eons_evt_info_type * qmi_nas_nw_name_find(
  mmgsdi_session_type_enum_type session,
  sys_plmn_id_s_type plmn_id,
  sys_lac_type lac_tac,
  mmgsdi_rat_enum_type rat,
  sys_csg_id_type csg_id
);

extern enum qmi_nas_stack_e qmi_nas_nw_name_info_stack(
  mmgsdi_eons_evt_info_type *eons_info,
  sys_plmn_id_s_type plmn_id,
  sys_lac_type lac_tac,
  mmgsdi_rat_enum_type rat,
  sys_csg_id_type csg_id
);
#ifndef FEATURE_MMODE_DISABLE_UIM
extern void qmi_nas_mmgsdi_process_cmd_rsp( qmi_mmode_cmd_enum_type cmd_type, void *user_data_ptr );
#endif
extern void qmi_nas_mmgsdi_process_evt( qmi_mmode_cmd_enum_type cmd_type, void *user_data_ptr );

#if defined(FEATURE_HDR)
//=========================================================================== 
// FUNCTION: qmi_nas_cmd_handler
//
// DESCRIPTION:
//   Generic handler function for QMI NAS event handlers
//
// SIDE-EFFECTS: 
//   Frees memory the payload points to
//   Sends QMI indication messages according to data in payload and
//   may update internal QMI_NAS states depending on the command handler thats
//   invoked
//
// RETURN:
//   None
//=========================================================================== 
//
void qmi_nas_cmd_handler( qmi_mmode_cmd_enum_type cmd, void *payload );
#endif
/*========================================================================
  FUNCTION qmi_nas_msgr_handler

  DESCRIPTION
    process messages received from msgr

  PARAMETERS
    msg pointer in *void. cast to dcc_msgr_type before use.
		*void type is to avoid circular reference of headers

  RETURN VALUE
    None
===========================================================================*/
void qmi_nas_msgr_handler( void *param );

/*========================================================================
  FUNCTION qmi_nas_msgr_register

  DESCRIPTION
    register to msgr messages

  PARAMETERS
    msgr client object pointer

  RETURN VALUE
    None
===========================================================================*/
void qmi_nas_msgr_register( msgr_client_t *msgr_client );


#if defined (FEATURE_CDMA)

/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_CDMA_AVOID_SYSTEM_RESP()

  DESCRIPTION
    Called when cmd_cb() supplied in cm_ph_cmd_avoid_system() is called 
    by CM. Sends response to control point.
        
  PARAMETERS
    user_data   : user data
    ph_cmd_err  : error type
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_nas_generate_cdma_avoid_system_resp
(
  void *                user_data,
  cm_ph_cmd_err_e_type  ph_cmd_err
);
#endif
/*===========================================================================
  FUNCTION QMI_NAS_GENERATE_SET_BUILTIN_PLMN_LIST_IND()

  DESCRIPTION
    
  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_nas_generate_set_builtin_plmn_list_ind
(
  qmi_error_type_v01 err, enum qmi_nas_subs_e as_id
);

/*===========================================================================
  FUNCTION QMI_NASI_SEND_RESPONSE()

  DESCRIPTION
    Send the response to framework

  PARAMETERS
    clid: uint8

  RETURN VALUE
    TRUE if sent successfully, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_nasi_send_response
(
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  dsm_item_type *         msg_ptr
);

/*===========================================================================
  FUNCTION qmi_nasi_send_ind()

  DESCRIPTION
    This function is calls the QMI Framework API to send out the NAS service
    indication to client.
===========================================================================*/
void qmi_nasi_send_ind( qmi_nasi_client_state_type* cl_sp, uint16 cmd, dsm_item_type* ind );

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================

FUNCTION  QMI_NAS_MCFG_REFRESH_EVT_CB

DESCRIPTION
   This function is the callback to process mcfg refresh ind.
   CB returns TRUE- MCFG will wait for DONE api.
   CB returns FALSE- MCFG will not wait for DONE api
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean qmi_nas_mcfg_refresh_evt_cb
(
  mcfg_refresh_info_s_type *mcfg_refresh_info_ptr
);
#endif // FEATURE_MODEM_CONFIG_REFRESH

 
#ifdef CM_DEBUG
#error code not present
#endif
#endif // !_QMI_NAS_INTERNAL_H
