#ifndef DS_PDN_NV_MANAGER_H
#define DS_PDN_NV_MANAGER_H

/*===========================================================================
                      DS_PDN_NV_MANAGER.H
DESCRIPTION
This file exposes PDN NV manager through APIs and data structures.

Copyright (c) 2018 - 2020 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_nv_manager.h#5 $
  $DateTime: 2023/06/06 06:49:26 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_pdn_manager.h"
#include "ds_bearer_manager.h"
#include "dsm_queue.h"
#include "ps_ip6_sm.h"
#include "ds_pdn_rate_ctrl_common.h"
#include "ds_3gpp_rmsm_ip_common.h"
#include "ds_pdn_cp_data_hdlr.h"

#define DS_PDN_NV_ENABLED    1
#define DS_EFS_READ_BUFFER_SZ 128

/*------------------------------------------------------------------------------ 
  Read the  NAS EFS
  ------------------------------------------------------------------------------*/
#define DS_PDN_EFNAS_CONFIG_SIZE 120

/*-------------------------------------------------------------------------
  PAP and CHAP definitions used by 3GPP and 3GPP2 MH
-------------------------------------------------------------------------*/
#define DS_PDN_AUTH_PAP   0x0
#define DS_PDN_AUTH_CHAP  0x1

#define DS_PDN_DEFAULT_DATA_UNIT 1500   /* used for MRU and MTU                   */
#define DS_OVERRIDE_APN_FILE     "/data/ds_override_apn.txt"

/* Enumeration type for RAT type, This RAT type enum is
   used only while reading override apn from EFS
   INTERNAL USE ONLY.
*/
typedef enum
{
  DS_3GPP_OVERRIDE_APN_INVALID_RAT = -1,
  DS_3GPP_OVERRIDE_APN_GSM_RAT,
  DS_3GPP_OVERRIDE_APN_LTE_CATM1_RAT,
  DS_3GPP_OVERRIDE_APN_LTE_NB1_RAT,
  DS_3GPP_OVERRIDE_APN_MAX_RAT
} ds_3gpp_override_apn_rat_type;


typedef struct 
{
  uint8	version ;		   /* version of the NV item*/
  boolean bcm_info_validation_enabled; /* NV item value */
  uint8 reserve_bytes[2];    /* reserve bytes for future use*/
}ds_pdn_bcm_info_validation_s;

/*-------------------------------------------------------------------------
  Structure for IPv6 IID type
-------------------------------------------------------------------------*/
typedef struct
{
  byte    iid_enum_type;
  uint64  iid_val;
}ds_pdn_ipv6_iid_type;

/*---------------------------------------------------------------------------
  Structure to store IPv6 config info read from EFS/NV
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                               is_enabled;
  boolean                               pdsn_as_proxy_ipv6_dns_server;
  ip6_sm_config_type                    sm_config;
  struct ps_in6_addr                    primary_dns;
  struct ps_in6_addr                    secondary_dns;
  ds_pdn_ipv6_iid_type                  iid_info;
  ds_pdn_ip_ver_failover_e_type         failover_mode;
}ds_pdn_ipv6_efs_nv_config_info;

/*---------------------------------------------------------------------------
  Struct to read Back_pressure_removal in EFS
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  boolean   back_pressure_removal; /* Back Pressure removal flag */
  boolean   um_wm_params_valid;    /* Whether UM WM params are valid */
  uint32    ul_wm_high_level;      /* UL WM high count */
  uint32    ul_wm_low_level;       /* UL WM low count */
  uint32    ul_wm_dne_level;       /* UL WM dne count */
  uint32    dl_wm_high_level;      /* DL WM high count */
  uint32    dl_wm_low_level;       /* DL WM low count */
  uint32    dl_wm_dne_level;       /* UL WM dne count */
  uint32    timer_val;             /* Dynamic BP timer value */
}ds_bearer_mgr_bp_removal_struct_type;

/*---------------------------------------------------------------------------
  Struct to store throughput optimization parameters
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  boolean   back_pressure_removal; /* Back Pressure removal flag */
  uint32    timer_val;             /* Dynamic BP timer value */
  uint16    tcp_threshold_ack_cnt; /* Threshold ack_cnt for triggerring BP removal */
}ds_bearer_mgr_tput_opt_struct_type;

typedef PACKED struct PACKED_POST
{
  boolean  gprs_wm_levels_valid;
  uint32   gprs_ul_dne;
  uint32   gprs_ul_hi;
  uint32   gprs_ul_lo;
  uint32   gprs_dl_dne;
  uint32   gprs_dl_hi;
  uint32   gprs_dl_lo;
  boolean  wcdma_wm_levels_valid;
  uint32   wcdma_ul_dne;
  uint32   wcdma_ul_hi;
  uint32   wcdma_ul_lo;
  uint32   wcdma_dl_dne;
  uint32   wcdma_dl_hi;
  uint32   wcdma_dl_lo;
  boolean  lte_wm_levels_valid;
  uint32   lte_ul_dne;
  uint32   lte_ul_hi;
  uint32   lte_ul_lo;
  uint32   lte_dl_dne;
  uint32   lte_dl_hi;
  uint32   lte_dl_lo;
}ds_bearer_mgr_um_wm_threshold_struct_type;

/*-----------------------------------------------------------------------------
  Structure used to parse the efs file
-----------------------------------------------------------------------------*/
typedef struct
{
    int read_buffer_size;       /*  For efs_read, value is DS_EFS_READ_BUFFER_SZ
                                    For efs_get, value is got from efs_stat  */
    char seperator;             /*  Seperator(;) to be parsed for            */
    char *buffer;               /*  Buffer to read the file into             */
    char *curr;                 /*  pointer to the current location          */
    char *end_pos;              /*  ponter to the end of the buffer          */
    char *efs_file_path;        /*  Use this when doing an efs_get           */
    boolean eof;                /*  used to indicate the end of file         */
    boolean skip_line;          /*  identifies  comments in the file         */
    boolean eol;                /*  used to indicate end of line             */
    boolean bol;                /*  used to indicate begining of the line    */
    boolean efs_get_performed;  /*  Variable to know if an efs_get has been 
                                    performed                                */
}ds_pdn_nv_efs_token_type;

/*-----------------------------------------------------------------------------
  Enum to specify the various return values 
  SUCCESS : Success
  EOL     : End of line is reached => record end
  EOF     : End of file is reached => file end => feof
  FAILURE : Failed 
-----------------------------------------------------------------------------*/
typedef enum 
{
  DS_PDN_NV_EFS_TOKEN_PARSE_SUCCESS  = 0,
  DS_PDN_NV_EFS_TOKEN_PARSE_EOL      = 1,
  DS_PDN_NV_EFS_TOKEN_PARSE_EOF      = 2,
  DS_PDN_NV_EFS_TOKEN_PARSE_FAILURE  = 3,
  DS_PDN_NV_EFS_TOKEN_PARSE_MAX
}ds_pdn_nv_efs_token_parse_status_type;

/*---------------------------------------------------------------------------
  This structure is used to read the NV item(NV:global_throttling). Other
  modem subsystems (e.g: NAS) can read this NV item using this struct.
---------------------------------------------------------------------------*/
typedef struct
{ 
  uint8   version ;          /* version of the NV item*/
  boolean global_throttling; /* NV item value */
  uint8 reserve_bytes[2];    /* reserve bytes for future use*/
}ds_pdn_nv_global_throttle_s;

typedef struct
{
  boolean                                       ds_eps_enable_send_trm_priority;
  
  ue_nas_rel_compliance_type                    ds_3gpp_nv_umts_rel_version;
  boolean                                       ds_3gpp_nv_bcm_validation_on_bearer_mod;
  uint8                                         ds_3gpp_nv_umts_nw_init_qos_support;
  boolean                                       dsumts_rmsmi_multi_pdn_dun_is_enabled;
  uint8                                         ds_pdn_3gpp_rel_version;
  boolean                                       ds_3gpp_rel12_qci_supported;
  uint16                                        ds_3gpp_ciot_bit_mask;
  boolean                                       apm_apn_switching;
  uint8                                         auth_fb_enabled;
  uint32                                        max_rab_reestab_req_retry_cnt;
  boolean                                       check_ehplmn_list;
  boolean                                       null_apn_support_for_non_attach_pdn;
  boolean                                       is_null_apn_support_set;
  boolean                                       call_orig_before_ps_attach;
  uint8                                         ds_pdn_level_auth;
  uint16                                        mtu_size;
  boolean                                       default_mtu;
  boolean                                       ds_eps_remove_unused_pdn;
  boolean                                       enable_apn_param_chg;
  boolean                                       send_ps_data_avail_on_power_save;
  ds_bearer_mgr_tput_opt_struct_type            ds_bearer_mgr_tput_opt_params;
  ds_bearer_mgr_um_wm_threshold_struct_type     ds_bearer_mgr_um_wm_threshold;
  uint8                                         ignore_inactivity_timer;
  uint8                                         cp_data_window_size;
  boolean                                       apn_rate_control_config;
  boolean                                       additional_apn_rate_control_config;
  boolean                                       allow_multi_pdn_same_apn;
  ds_pdn_ipv6_efs_nv_config_info                ipv6_nv_efs_info;
  boolean                                       ds_eps_allow_def_pdn_teardown;
  boolean                                       ds_eps_always_connect_def_pdn_flag;
  boolean                                       ds_3gpp_redial_for_no_cause_code;
  boolean                                       ds_3gpp_disable_apn_matching;
  boolean                                       ds_eps_switch_apn_on_reject;
  boolean                                       global_throttling;
  uint8                                         apn_rc_intrvl_factor;
  uint8                                         additional_apn_rc_intrvl_factor;
  uint8                                         splmn_rc_intrvl_factor;
  boolean                                       ds_3gpp_reg_pdn_throttling;
  boolean                                       nv_rel10_throttling_per_plmn;
  boolean                                       ds_eps_allow_infinite_throt;
  boolean                                       enable_pdn_throt_per_rat;
  uint32                                        ser_req_throt_sm_nv;  
  byte                                         *efsnas_config;
  boolean                                       ds_3gpp_disable_mtu_request;
  char                                         *override_apn_info[DS_3GPP_OVERRIDE_APN_MAX_RAT];
  ds_pdn_dun_call_config_type                   ds_pdn_allow_single_ip_dun_call_dual_ip_profile;
  boolean                                       ds_pdn_rand_backoff_throt;
  boolean                                       ds_iot_rpm_enabled;
  ds_pdn_rai_config_type                        rai_config;
  uint32                                        ds_3gpp_nv_efnas_read_failure_def_retry_timer;
  boolean                                       ds_gsm_rpm_enabled;
}ds_pdn_mgr_nv_info_type;


/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_EFS_FILE_INIT

DESCRIPTION   The function intializes the state machine and
              also opens the file.

DEPENDENCIES  None.

RETURN VALUE  0  : SUCCESS: The file is good, readable,
                            State Machine Initialized.
              -1 : FAILURE: The file cannot be opened/ readable.

SIDE EFFECTS  None.
===========================================================================*/
int ds_pdn_nv_manager_efs_file_init
(
  const char                 *file_path,
  ds_pdn_nv_efs_token_type   *sm
);

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_EFS_TOKENIZER

DESCRIPTION   The is the function that reads data from the opened file.
              The data read is looked for tokens
              1. Each token is seperated by ';'
              2. Successive ';' means empty token
              3. Line begining with '#' is comment
              4. '\n' is the end of token and record
              5. Empty line is ignored
              6. Insufficient tokens is a record is considered bad record

DEPENDENCIES  File should have already been opened.

RETURN VALUE
              SUCCESS : Success => Found Token.
                        *begin points to the begining of token.
                        *end points to the end of token.
              EOL     : End of line is reached => record end
                        => no token extracted
              END     : End of file is reached => file end => feof
                        => no token extracted
              FAILURE : Failed

SIDE EFFECTS  None.
===========================================================================*/
ds_pdn_nv_efs_token_parse_status_type ds_pdn_nv_manager_efs_tokenizer
(
  ds_pdn_nv_efs_token_type    *sm,
  char                       **begin,
  char                       **end
);

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_EFS_FILE_CLOSE

DESCRIPTION   The function closes file and releases the state machine

DEPENDENCIES  The file should have opened already.

RETURN VALUE  NONE

SIDE EFFECTS  None.
===========================================================================*/
void ds_pdn_nv_manager_efs_file_close
(
  ds_pdn_nv_efs_token_type  *sm
);

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_READ_LEGACY_NV

DESCRIPTION   This function reads legacy NV item.

DEPENDENCIES  NV task is expected to be available.  Must run in the
              DS task context.

RETURN VALUE  NV return code.

SIDE EFFECTS  None.
===========================================================================*/
nv_stat_enum_type ds_pdn_nv_manager_read_legacy_nv
(
  nv_items_enum_type      item,
  nv_item_type           *data_ptr
);

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_READ_EFS_NV

DESCRIPTION   This function reads EFS NV item.
                        data_ptr should be correctly by initailized by clients.

DEPENDENCIES  None.

RETURN VALUE  NV return code.

SIDE EFFECTS  None.
===========================================================================*/
nv_stat_enum_type ds_pdn_nv_manager_read_efs_nv
(
  const char             *item_path,
  void                   *data_ptr,
  uint32                  item_size
);

/*===========================================================================
FUNCTION DS_PDN_NV_PATH_IS_DIRECTORY

DESCRIPTION
  To check if the EFS directory exists

DEPENDENCIES
  None

PARAMETERS
  dirname - Directory path

RETURN VALUE
   0         - success
   efs_errno - EFS error
   -1        - Other error

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_pdn_nv_path_is_directory
(
  const char  *dirname
);

/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_get_nv_lte_rel_version

DESCRIPTION
  This function returns  the LTE release version, read during nv manager
  initialzizing.

PARAMETERS
  void

DEPENDENCIES
  None.

RETURN VALUE
  lte_3gpp_release_ver_e enum type

SIDE EFFECTS
  None

===========================================================================*/
lte_3gpp_release_ver_e ds_pdn_nv_manager_get_nv_lte_rel_version();


#ifdef FEATURE_DATA_LTE

/*===========================================================================
FUNCTION  DS_3GPP_NV_MANAGER_GET_ENABLE_SEND_TRM_PRIORITY

DESCRIPTION
  This function gets the value of NV 73562 ENABLE_SEND_TRM_PRIORITY per
  subscription

PARAMETERS 
  subs_id subscription id
 
DEPENDENCIES
  None.

RETURN VALUE
  True  - ENABLE_SEND_TRM_PRIORITY = 1.
  False - ENABLE_SEND_TRM_PRIORITY = 0.
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_nv_manager_get_enable_send_trm_priority();

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_NULL_APN_SUPPORT_FOR_NON_ATTACH_PDN

DESCRIPTION
  This function gets null_apn_support_for_non_attach_pdn NV item

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_null_apn_support_for_non_attach_pdn
(
  void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_remove_unused_pdn

DESCRIPTION
  This function gets the remove unused pdn NV item  

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/

boolean ds_pdn_nv_manager_get_remove_unused_pdn
(
   void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_apn_param_chg_cfg

DESCRIPTION
  This function gets the apn_param_chg_cfg NV item  

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/

boolean ds_pdn_nv_manager_get_apn_param_chg_cfg
(
   void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_ignore_inactivity_timer

DESCRIPTION
  This function returns true if ignore inactivity timer nv is set to true

PARAMETERS 
  None
 
DEPENDENCIES
  None.

RETURN VALUE
  True  - if call orig is allowed before ps attach
  False - 
 
SIDE EFFECTS
  None.
===========================================================================*/

uint8 ds_pdn_nv_manager_get_ignore_inactivity_timer
(
  void
);

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_CP_DATA_WINDOW_SIZE

DESCRIPTION
  This function gets cp_data_window_size NV item

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS

===========================================================================*/
uint8 ds_pdn_nv_manager_get_cp_data_window_size
(
  void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_apn_rate_control_config

DESCRIPTION
  This function gets the boolean that shows whether or not APN Rate
  control configuration is enabled or not.

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_apn_rate_control_config
(
  void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_additional_apn_rate_control_config

DESCRIPTION
  This function gets the boolean that shows whether or not Additional APN Rate
  control configuration is enabled or not.

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_additional_apn_rate_control_config
(
  void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_allow_default_pdn_teardown

DESCRIPTION
  This function gets the boolean that shows whether default PDN tear down is allowed

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_allow_default_pdn_teardown
(
  void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_always_connect_def_pdn_flag

DESCRIPTION
  This function gets the boolean that shows whether always connect default PDN is set

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  boolean.

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_always_connect_def_pdn_flag
(
  void
);

/*===========================================================================
FUNCTION ds_pdn_nv_manager_get_disable_apn_matching_from_nv

DESCRIPTION
  This function gets the disable_apn_matching NV(73529 ) item 

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_disable_apn_matching_from_nv
(
   void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_switch_apn_on_reject

DESCRIPTION
  This function gets the apn switch on reject  NV item 

PARAMETERS
 None

DEPENDENCIES
  None.

RETURN VALUE  
  boolean

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_switch_apn_on_reject
(
  void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_rai_info

DESCRIPTION
  This function gets the value of rai timer from NV

PARAMETERS
 None

DEPENDENCIES
  None.

RETURN VALUE  
  timer value in seconds

SIDE EFFECTS

===========================================================================*/
ds_pdn_rai_config_type ds_pdn_nv_manager_get_rai_info
(
  void
);

#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION  DS_PDN_NV_MANAGER_GET_RC_INTERVAL_FACTOR

DESCRIPTION
  This function return the APN Rate Control interval factor structure

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
uint8 ds_pdn_nv_manager_get_rc_interval_factor
(
  ds_pdn_rate_ctrl_type    current_rate_ctrl_type
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_efnas_config

DESCRIPTION
  This function gets the NAS configuration

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  byte*

SIDE EFFECTS
  None
===========================================================================*/
byte* ds_pdn_nv_manager_get_efnas_config
(
  void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_global_throttling_nv

DESCRIPTION
  This function reads the global throttling NV item 
 

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
 boolean

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_global_throttling_nv
(
  void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_regular_pdn_throttling_nv

DESCRIPTION
  This function reads the regular_pdn_throttling NV item 
 

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
 boolean

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_regular_pdn_throttling_nv
(
  void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_allow_infinite_throttling_nv

DESCRIPTION
  This function fetches the allow infinite throt NV item 

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
 boolean

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_allow_infinite_throttling_nv
(
  void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_enable_pdn_throt_per_rat

DESCRIPTION
  This function returns true if the enable_pdn_throt_per_rat NV item 
  is set to TRUE

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
 boolean

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_enable_pdn_throt_per_rat
(
  void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_rel_10_throttling_per_plmn_from_nv

DESCRIPTION
  This function reads the rel_10_throttling_per_plmn NV item 
 

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
 boolean

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_rel_10_throttling_per_plmn_from_nv
(
  void
);

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_SER_REQ_THOR_SM_NV

DESCRIPTION
  This function is used to retrieve service request throttle info per
  subscription
 
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  uint32. - Service request throttling info

SIDE EFFECTS
  None.

===========================================================================*/
uint32  ds_pdn_nv_manager_get_ser_req_thor_sm_nv
(
  void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_send_ps_data_avail_on_pwr_save

DESCRIPTION
  This function gets the send PS data avail on power save NV. 

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_nv_manager_get_send_ps_data_avail_on_pwr_save
(
  void
);

/*===========================================================================
FUNCTION ds_pdn_nv_manager_get_nv_umts_rel_version

DESCRIPTION
  This Function reads the NAS Complaince NV item and updates the global variable 
  ds_3gpp_nv_umts_rel_version accordingly for umts specific scenerios.

PARAMETERS
  void

DEPENDENCIES
  None.

RETURN VALUE
  ue_nas_rel_compliance_type enum type

SIDE EFFECTS
  None

===========================================================================*/
ue_nas_rel_compliance_type ds_pdn_nv_manager_get_nv_umts_rel_version();


/*===========================================================================
FUNCTION ds_pdn_nv_manager_get_bcm_info_validation_nv

DESCRIPTION
  This function gets the BCM validation NV setting.

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  TRUE /FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_pdn_nv_manager_get_bcm_info_validation_nv();


/*===========================================================================
FUNCTION ds_pdn_nv_manager_get_umts_nw_init_qos_support_nv

DESCRIPTION
  This function gets the ds_3gpp_nv_umts_nw_init_qos_support NV item
  NV: 70331
  modem/data/3gpp/umts_nw_initiated_qos_support

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  1 - If NV enabled
  0 - otherwise

SIDE EFFECTS
  None

===========================================================================*/
uint8 ds_pdn_nv_manager_get_umts_nw_init_qos_support_nv();

/*===========================================================================
  FUNCTION ds_pdn_nv_manager_get_multi_pdn_dun_nv

  DESCRIPTION
    This function gets the multi pdn dun nv item

  PARAMETERS
    None

  DEPENDENCIES
    None.

  RETURN VALUE  
    TRUE /FALSE

  SIDE EFFECTS
===========================================================================*/
boolean ds_pdn_nv_manager_get_multi_pdn_dun_nv();

/*===========================================================================
FUNCTION ds_pdn_nv_manager_get_3gpp_rel_version

DESCRIPTION 
  This function returns 3GGP_REL_VERSION from cache  
 
PARAMETERS
  None
DEPENDENCIES
  None.

RETURN VALUE
  cached 3GPP_REL_VERSION NV value
SIDE EFFECTS
  None.
===========================================================================*/
uint8 ds_pdn_nv_manager_get_3gpp_rel_version();

/*===========================================================================
FUNCTION  DS_PDN_NV_MANAGER_GET_REL12_QCI_SUPPORT

DESCRIPTION
  This function gets the value of NV Release 12 QCI Support Nv item

PARAMETERS 
  None.
 
DEPENDENCIES
  None.

RETURN VALUE
  True  - Release 12 qci supported
  False - Release 12 qci not supported
 
SIDE EFFECTS
  None.
 
===========================================================================*/
boolean ds_pdn_nv_manager_get_rel12_qci_support();

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_CIOT_CAP

DESCRIPTION
  This function is to get the CIOT capability information from NAS NV item.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  uint16 - Bitmask with CIOT capability

SIDE EFFECTS
  None.
===========================================================================*/
uint16 ds_pdn_nv_manager_get_ciot_cap
(
  void
);

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_APM_APN_SWITCHING

DESCRIPTION
  This function is to get the APM APN switching flag

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean - APM APN switching flag

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_nv_manager_get_apm_apn_switching
(
  void
);

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_AUTH_FALLBACK

DESCRIPTION
  This function is to get the auth fb enabled flag

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean - AUTH FB flag

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_nv_manager_get_auth_fallback
(
  void
);

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_MAX_RAB_REESTAB_RETRY_CNT

DESCRIPTION
  This function is used to retrieve umax rab reestablishment retry count 
 
  PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  uint32 -max rab reestab count

SIDE EFFECTS
  None.

===========================================================================*/

uint32 ds_pdn_nv_manager_get_max_rab_reestab_retry_cnt
(
  void
);

/*===========================================================================
FUNCTION ds_pdn_nv_manager_get_check_ehplmn_list

DESCRIPTION
  This function returns true if check_ehplmn_list NV item is set to TRUE

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_nv_manager_get_check_ehplmn_list
(
  void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_call_orig_before_ps_attach

DESCRIPTION
  This function returns true if call orig is allowed before ps attach.

PARAMETERS 
  None
 
DEPENDENCIES
  None.

RETURN VALUE
  True  - if call orig is allowed before ps attach
  False - 
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_nv_manager_get_call_orig_before_ps_attach
(
  void
);

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_GET_CONFIG_PDN_LEVEL_AUTH

DESCRIPTION   Gets config_pdn_level_auth NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
uint8 ds_pdn_nv_manager_get_config_pdn_level_auth
(
  void
);

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_MTU_FROM_NV

DESCRIPTION
  This function is used to get mtu value from nv item 

  PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 ds_pdn_nv_manager_get_mtu_from_nv 
(
   void
);


/*===========================================================================
FUNCTION ds_pdn_nv_manager_is_rel_10_or_above_supported

DESCRIPTION
  This Function returns the NAS Complaince NV item  read during nv manager
  initialization

PARAMETERS
  void

DEPENDENCIES
  None.

RETURN VALUE
  boolean - True -> Umts Nas release version 10 or above
          - False -> Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_nv_manager_is_rel_10_or_above_supported();

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_nv_lte_rel_10

DESCRIPTION
  This function returns true if the nv item corresponding to the file
  /nv/item_files/modem/lte/lte_3gpp_release_ver is LTE_3GPP_REL10
  and release 10 throttling is enabled

PARAMETERS
  subs_id  subscription 
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_nv_manager_get_nv_lte_rel_10
(
  void
);

/*===========================================================================
FUNCTION ds_pdn_nv_manager_is_rel_11_or_above_supported

DESCRIPTION
  Determine if UE is release 11 ready or not

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE 
   TRUE - if device is release 11 ready
   FALSE - Otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_nv_manager_is_rel_11_or_above_supported
(
   void
);

/*===========================================================================
FUNCTION ds_pdn_nv_manager_is_rel_12_or_above_supported

DESCRIPTION
  Determine if UE is release 12 ready or not

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE 
   TRUE - if device is release 12 ready
   FALSE - Otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_nv_manager_is_rel_12_or_above_supported
(
   void
);

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_IS_REL_13_OR_ABOVE_SUPPORTED

DESCRIPTION
  Determine if UE is release 13 ready or not

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE 
   TRUE - if device is release 13 ready
   FALSE - Otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_nv_manager_is_rel_13_or_above_supported
(
   void
);

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_IS_REL_14_OR_ABOVE_SUPPORTED

DESCRIPTION
  Determine if UE is release 14 ready or not

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE 
   TRUE - if device is release 14 ready
   FALSE - Otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_nv_manager_is_rel_14_or_above_supported
(
   void
);


/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_ONLY_REL_11_SUPPORTED

DESCRIPTION
  Determine if UE has release only 11 support or not

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE 
   TRUE - if device is release 11 ready only
   FALSE - Otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_nv_manager_only_rel_11_supported
(
   void
);

/*===========================================================================
FUNCTION DS_PDN_NV_GET_UM_WM_THRESHOLD

DESCRIPTION
  This function is used to retrieve um watermark threshold values 
 
  PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  ds_bearer_mgr_um_wm_threshold_struct_type - um wm info

SIDE EFFECTS
  None.

===========================================================================*/
ds_bearer_mgr_um_wm_threshold_struct_type  ds_pdn_nv_get_um_wm_threshold
(
  void
);

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_BACK_PRESSURE_PARAMS

DESCRIPTION
  This function is used to retrieve back pressure params
 
  PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  ds_bearer_mgr_tput_opt_struct_type - BP info

SIDE EFFECTS
  None.

===========================================================================*/
ds_bearer_mgr_tput_opt_struct_type  ds_pdn_nv_get_back_pressure_params
(
  void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_multi_pdn_same_apn_nv

DESCRIPTION
  This function gets the multi pdn same apn NV item 

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/

boolean ds_pdn_nv_manager_get_multi_pdn_same_apn_nv
(
  void
);

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_GET_IPV6_SM_CONFIG_INFO

DESCRIPTION   Get IPv6 SM config info configured through NV item

DEPENDENCIES  None

RETURN VALUE  ip6_sm_config_type* - IPv6 SM config info

SIDE EFFECTS  None
===========================================================================*/
ip6_sm_config_type* ds_pdn_nv_manager_get_ipv6_sm_config_info
(
  void
);

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_GET_IPV6_PRIMARY_DNS_ADDR

DESCRIPTION   Get IPv6 primary DNS server address

DEPENDENCIES  None

RETURN VALUE  ps_in6_addr* - Ptr containing the primary DNS server addr

SIDE EFFECTS  None
===========================================================================*/
struct ps_in6_addr* ds_pdn_nv_manager_get_ipv6_primary_dns_addr
(
  void
);

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_GET_IPV6_SECONDARY_DNS_ADDR

DESCRIPTION   Get IPv6 secondary DNS server address

DEPENDENCIES  None

RETURN VALUE  ps_in6_addr* - Ptr containing the secondary DNS server addr

SIDE EFFECTS  None
===========================================================================*/
struct ps_in6_addr* ds_pdn_nv_manager_get_ipv6_secondary_dns_addr
(
  void
);

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_IS_IPV6_ENABLED

DESCRIPTION   Returns whether IPv6 is enabled through NV or not.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  TRUE: IPv6 enabled
              FALSE: IPv6 disabled

SIDE EFFECTS  None
===========================================================================*/
boolean ds_pdn_nv_manager_is_ipv6_enabled( void );

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_IS_PDSN_AS_PROXY_IPv6_DNS_SERVER

DESCRIPTION   Returns whether PDSN acts as proxy IPv6 DNS server

DEPENDENCIES  None

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds_pdn_nv_manager_is_pdsn_as_proxy_ipv6_dns_server
(
  void
);

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_GET_FAILOVER_MODE

DESCRIPTION   Returns the failover mode set in NV

DEPENDENCIES  None.

RETURN VALUE  Boolean.

SIDE EFFECTS  None.
===========================================================================*/
ds_pdn_ip_ver_failover_e_type ds_pdn_nv_manager_get_failover_mode( void );

/*===========================================================================
FUNCTION      DS_PDN_NV_MANAGER_GET_IPV6_IID_INFO

DESCRIPTION   Returns IPv6 iid info

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_pdn_nv_manager_get_ipv6_iid_info
(
  ds_pdn_ipv6_iid_type     *iid_info_p
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_redial_no_cause_code_from_nv

DESCRIPTION
  This function reads the redial for no cause from NV item
  NV item: 72563
 
PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  None

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_redial_no_cause_code_from_nv
(
  void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_read_mode_pref_from_nv

DESCRIPTION
  This function reads the redial for no cause from NV item
  NV item: 72563
 
PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  None

SIDE EFFECTS

===========================================================================*/
void ds_pdn_nv_manager_read_mode_pref_from_nv
(
  void
);

/*===========================================================================
FUNCTION      DS3GSUBSMGR_CONVERT_NV_MODE_TO_CM_MODE

DESCRIPTION   This utility function translates the NV mode enum type to 
              CM mode enum.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
cm_mode_pref_e_type ds_pdn_nv_manager_convert_nv_mode_to_cm_mode
(
  nv_mode_enum_type nv_mode_pref
);

/*===========================================================================
FUNCTION  DS_PDN_NV_MANAGER_GET_OVERRIDE_APN_PER_RAT

DESCRIPTION
  This function returns the override apn for a rat
  if it is configured in EFS

PARAMETERS
  apn_name - container to store apn_name

DEPENDENCIES
  None.

RETURN VALUE
  TRUE:  if apn is correctly returned.
  FALSE: if something goes wrong.

SIDE EFFECTS

===========================================================================*/
boolean ds_pdn_nv_manager_get_override_apn_per_rat
(
  char                          *apn_name,
  sys_sys_mode_e_type            curr_mode
);


/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_ppp_wait_timer

DESCRIPTION
  This function gets the timer value of ppp_up_wait_timer_val present in
  NV item single_ip_dun_call_in_dual_ip_profile for each subscription

PARAMETERS 
  subs_id subscription id
 
DEPENDENCIES
  None.

RETURN VALUE
  uint16 timer value in ms.
 
SIDE EFFECTS
  None.
 
===========================================================================*/
uint16 ds_pdn_nv_manager_get_ppp_wait_timer
(
  void
);

/*===========================================================================
FUNCTION  ds_pdn_nv_manager_get_allow_single_IP_call_dualIP_pf

DESCRIPTION
  This function gets the NV value of ds_3gpp_allow_single_ip_dun_call_dual_ip_profile

PARAMETERS 
 
DEPENDENCIES
  None.

RETURN VALUE
  Boolean Flag indicating whether allow_single_ip_dun_call_dual_ip_profile nv is enabled.
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_nv_manager_get_allow_single_IP_call_dualIP_pf
(
  void
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_INIT

DESCRIPTION
  This function initializes the nv manager module. This function is called
  during boot up. This function is responsible for intializing the nv info
  for primary subscription.
 
	PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_nv_manager_init();


/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_READ_DISABLE_MTU_REQUEST

DESCRIPTION
  This function is used to read disable mtu request value from nv item
  per subscription

PARAMETERS
 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_nv_manager_read_disable_mtu_request();

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_DISABLE_MTU_REQUEST

DESCRIPTION
  This function is used to retrieve disable mtu request value per subscription

PARAMETERS
  
DEPENDENCIES
  None.

RETURN VALUE
  boolean-disable mtu request

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_nv_manager_get_disable_mtu_request();

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_READ_RAND_BACKOFF_THROT

DESCRIPTION
  This function is used to read random backoff throt from nv item.

PARAMETERS


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_nv_manager_read_rand_backoff_throt();

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_GET_RAND_BACKOFF_THROT

DESCRIPTION
  This function is used to get random backoff throttling NV

  PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_nv_manager_get_rand_backoff_throt();

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_IS_IOT_RPM_ENABLED

DESCRIPTION
  This function is used to retrieve it rpm enabled value

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  boolean-iot_rpm_enabled

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_nv_manager_is_iot_rpm_enabled();

/*===========================================================================
FUNCTION DS_PDN_NV_MANAGER_IS_GSM_RPM_ENABLED

DESCRIPTION
  This function is used to retrieve GSM rpm enabled value

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  boolean-gsm_rpm_enabled

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_nv_manager_is_gsm_rpm_enabled();
/*===========================================================================
FUNCTION  DS_GET_NV_MANAGER_GET_EFNAS_READ_FAILURE_DEF_RETRY_TIMER

DESCRIPTION
  This Function reads efnas_read_failure_def_retry_timer

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
uint32 ds_pdn_nv_manager_get_efnas_read_failure_def_retry_timer(void);
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */


#endif /* DS_PDN_NV_MANAGER_H */
