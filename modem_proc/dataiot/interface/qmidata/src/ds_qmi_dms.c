/*===========================================================================

                         D S _ Q M I _ D M S . C

DESCRIPTION

 The Data Services Qualcomm MSM Interface Device Management Services source
 file.

EXTERNALIZED FUNCTIONS

  qmi_dms_init()
    Register the DMS service with QMUX for all applicable QMI links
    

Copyright (c) 2004-2020 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/qmidata/src/ds_qmi_dms.c#7 $ $DateTime: 2022/11/08 18:42:56 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
--------    ---    ----------------------------------------------------------
10/06/20    mks     To check CM api if LPM is allowed.
10/06/17    ss     Added support for LTE-U 256 bands.
2/24/17      rg      Added rau and gprs ready timer for CPSMS
11/20/14    vrk    Updated from deprecated TRM APIs to new APIs
06/10/14    vrk    Added support for SRLTE configuration
05/27/14    wc     Image version reporting
05/19/14    vrk    Added support for current device config through policyman
04/11/14    vrk    Modified get_sw_ver to fetch info from global.
03/19/14    sk     Added new message - DMSI_CMD_GET_ENCRYPTED_DEVICE_SERIAL_NUMBERS.
03/10/14    vrk    Added error code QMI_ERR_DEVICE_NOT_READY to QMI_DMS_SET_OPERATING_MODE_RESP
03/13/14    sah    Added support for tertiary subscription
02/18/14    sah    Modified nv items to report correct phone self ESN and MEID
                   after UIM card is inserted.
02/13/14    vrk    Modified handling of get_ck_state_q
01/16/14    sah    Using Sysmon API for poweroff when phone unlock attempts
                   trials exceeded.
01/16/14    sah    Fix for MSISDN read when additional data is present
                   in extension file.
11/28/13    gk     Added support for get_operator info message
11/27/13    sah    Added support for set_ap_sw_version message.
11/26/13    gk     Added support for get_mac message
09/25/13    gk     In SGLTE mode we should report CS and PS active
08/30/13    gk     Added SGLTE support to simul_voice_and_data_capability TLV
07/16/13    gk     Fixed double prepending of message header
07/09/13    az     Implemented TDSCDMA GET/SET test config support
06/07/13    rk     modified get msisdn message to retrieve proper values 
01/25/13    ua     Poweroff mapping to OFFLINE first is revereted. 
                   Only Poweroff is sent. 
05/21/13    gk     Added support for device_multisim_capability TLV 
05/10/13    ab     Add TTL markers for boot up profiling 
03/18/13    ab     Increased number of dms clients available to 32 
02/05/13    sn     Fixing invalid async command buffer access on client release.
12/11/12    sn     DMS Memory Optimization, moving to new QMI Framework.
10/31/12    svj    Allow hex digits in ICCID numbers 
10/26/12    gk     Reset and Shutdown operating mode changes calls CM API for clients 
                   to handle any mode change processes before actually invoking 
                   Sys Mon API's.
09/10/12    sb     Len checks for QMI TLVs
08/13/12    svj    Add a TLV to indicate device service capability
08/07/12    ab     Add a TLV to indicate simultaneous voice and data capability
08/06/12    gk     Added support for QMI service to log what messages/TLVs  
                   are supported on this branch.
07/18/12    sn     Fix to use Sysmon API for shutdown. For reset, if AT
                   cmd is not forwarded to AP, Sysmon API for reset is used.
06/11/12    sb     Fix to move to correct UIM state in case of CARD ERROR
                   event.
05/31/12    sj     Support SYS_OPRT_MODE_NET_TEST_GW in get 
12/23/11    ua     Route the Operating mode Reset command through 
                   DMS->AT->QMI-AT->Apps Proc. 
04/27/12    sb     Added additional check in qmi_dmsi_set_operating_mode()
04/27/12    sb     Removed feature Wrap FEATURE_DATA_NIKEL
04/20/12    sn     Added TLV for CSFB voice support in device capability msg.
02/28/12    sj     Add support for TDSCDMA 
02/17/12    sb     Fixed Compiler and KW Warnings.
01/25/12    sj     Fix qmi_dmsi_get_hw_supported_radio_if_list and 
                   qmi_dmsi_get_max_channel_rate to use CM mode_capability
                   which is runtime determined
12/13/11    wc     Fix get_msisdn returing NOT_PROVISINOED on multi-mode build
11/02/11    wc     Add qmi_dms_get_current_prl_info
10/10/11    sj     Add support for LTE band capability TLV  
10/04/11    sa     Q6 free floating changes for QMI.
12/09/10    ua     Added new message for Set SPC, and 
                   additional TLV for GET Device Serial Numbers to have the IMEI SVN.
08/03/11    sa     GSDI cleanup changes.
07/14/11    sj     Expose UIM status and PIN1 status to other modules 
06/22/11    wc     Send PRL init indication when PRL is loaded 
05/11/11    sa     Added new message for SW version info.
03/08/11    ua     Retreiving WCDMA/HSDPA/HSUPA default data rates from RRC.
02/21/11    kk     Fix to prevent insignificant bytes in GET_UIM_IMSI.
02/15/11    kk     Added support for PRL_ONLY TLV in PRL_INFO message.
01/10/11    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
12/17/10    kk     Added log messages before ASSERTs.
12/01/10    ua     Added support for LTE Data rates. 
11/09/10    ua     Extending support of qmi_dmsi_get_msisdn in LTE Mode. 
08/02/10    ack    Added support for setting user time using QMI
06/29/10    kk     Removing bbver.h dependency for BOOT_BLOCK_VERSION.
06/21/10    kk     Fix to align to new MMGSDI APIs for PERSO requests.
06/21/10    kk     Fix to check UIM status before posting commands to MMGSDI.
06/07/10    kk     Fix to update app_type and uim_state from all possible
                   events.
06/07/10    kk     Fix to include HSDPA and HSUPA channel rates in device
                   capabilities.
06/07/10    kk     Fix to prevent accidental memset of get_ck queue.
04/13/10    kk     Fix for GET_UIM_ICCID to report correct error value.
03/18/10    jee    changed FEATURE_MMGSDI to FEATURE_MMGSDI_SESSION_LIB
11/10/09    jee    Changes to support MMGSDI re-architecture
10/16/09    kk     Windows7 compliance - new messages and ATCoP dependencies.
10/09/09    kk     Segregated the internal and externalized functions.
10/08/09    kk     Windows7 compliance - DMS changes.
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
10/02/06    ks     Fixed PIN1 and PIN2 status TLV types in event_report_ind.
09/26/06    ks     Updated version of DMS to 1.1. Removed pin_id param
                   from pin_status tlvs.
09/06/06    ks     Featurized pin support code, under FEATURE_MMGSDI.
09/05/06    ks     Added support for PIN1/PIN2 operations using QMI.
09/05/06    ks     Added support to handle async responses to DMS.
09/02/06    jd     In production, don't return WLAN in supported radio list
08/23/06    ks     Changes to support multiple qmi/rmnet instances.
07/11/06    ks     Using dsi_get_nv_item() to read from NV. Other clean up.
07/06/06    ks     Removed qmi_dmsi_get_tl(), using qmi_svc_get_tl() instead
06/07/06    ks     Fixed Power state and Battery level reporting
04/05/06    jd     Changed GET_DEVICE_SERIAL_NUMBERS to only return not 
                   provisioned if no serial number is returned.
03/13/06    ks     Changed parsing of requests to return error for fixed 
                   length tlvs when tlv lengh passed is incorrect.
03/13/06    ks     Changed QMI_ERR_INVALID_ARG to QMI_ERR_MALFORMED_MSG.                    
03/06/06    ks     Return QMI_ERR_INVALID_ARG if a malformed TLV is seen.
10/27/05    ks     Removed bat_lvl_lower_limit < BAT_LVL_MIN(0) check as 
                   bat_lvl_lower_limit is unsigned.
09/08/05    ks     Added support for returning imei, MSISDN for UMTS.
08/15/05    ks     Return error when all optional TLVs are absent for messages
                   which don't have any mandatory TLVs.
05/31/05   jd/ks   Code review updates
05/11/05    ks     Code review comments and clean up.
03/14/05    ks     Removed featurization
11/21/04    jd/ks  Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ttl_map.h"
#include <stdio.h>
#include <string.h>
#include <stringl/stringl.h>
#include "mobile.h"
#include "dsm.h"
#include "nv.h"
#include "msg.h"
#include "err.h"
#include "amssassert.h"
#include "ps_in.h"
#include "ps_byte.h"
#include "ds_qmi_task.h"
#include "ds_qmi_if.h"
#include "ds_qmi_dms.h"
#include "ds_qmi_defs.h"
//#include "ds_qmi_svc.h"
#include "secapi.h"
#ifdef FEATURE_MMGSDI_SESSION_LIB
  #include "mmgsdilib.h"
  #include "mmgsdisessionlib.h"
#endif /* FEATURE_MMGSDI_SESSION_LIB */
#include "DDIChipInfo.h"
#include "time.h"
#include "time_genoff.h"
#include "smem.h"
#ifdef FEATURE_PREFERRED_ROAMING
   #include "prl.h"
#endif /* FEATURE_PREFERRED_ROAMING */
#include "qw.h"
#include <stringl/stringl.h>
#include "data_msg.h"
#include "ds_qmi_nv_util.h"
#include "ds_system_heap.h"
#include "trm.h"
#include "dsati.h"
#include "sys_m_reset.h"
#include "qmi_si.h"
#include "qmi_idl_lib.h"
#include "qmi_csi_target_ext.h"
#include "mqcsi_log.h"
#include "qmi_sap.h"
#include "device_management_service_v01.h"
#include "marb.h"
#include "ps_sys.h"
#include "ds_sys_event.h"
#include "policyman.h"
#include "policyman_msg.h"
#include "dsmsgr.h"
#include "cm_msgr_msg.h"
#include "ps_utils.h"
#include "dstaski.h"
#include "ds3gmmgsdiif.h"
#include "cm.h"
#include "cmll_v.h"

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

#define DMSI_MAX_CLIDS    (32)

#define DMSI_QCSI_OS_SIGNAL ((rex_sigs_type)(1 << DS_DMS_RDR_SIGNAL))

#define DMSI_SERIAL_NO_SIZ  (256)

#define DMSI_BASE_DECIMAL   (10)
#define DMSI_BASE_HEX       (16)
#define DMSI_MIN_SIZ        (10)
/*---------------------------------------------------------------------------
  Policyman Max subs
---------------------------------------------------------------------------*/

#define QMI_DMS_POLICYMAN_MAX_SUBS (3)

/*---------------------------------------------------------------------------
  Constants for IMSI
---------------------------------------------------------------------------*/


#define DMSI_BAT_LVL_MIN               (0)
#define DMSI_BAT_LVL_MAX               (100)
#define DMSI_BAT_LVL_INACTIVE          (255)
#define DMSI_PWR_STATE_INACTIVE        (0)
#define DMSI_EVENT_MASK_POWER_STATE    (0x01)

/*---------------------------------------------------------------------------
  UIM IMSI length
---------------------------------------------------------------------------*/
#define DMSI_MMGSDI_IMSI_LEN (15)

/*---------------------------------------------------------------------------
  UIM MSISDN length
---------------------------------------------------------------------------*/
#define DMSI_MMGSDI_MSISDN_LEN (15)

/*---------------------------------------------------------------------------
  Invalid PRL version for RUIM
---------------------------------------------------------------------------*/
#define DMSI_PRL_VERSION_INVALID (0xFFFF)

/*---------------------------------------------------------------------------
  Macro used to determine target support
---------------------------------------------------------------------------*/
#define DMSI_TARGET_SUPPORTS_CDMA(mode_capability)  (mode_capability & SYS_SYS_MODE_MASK_CDMA )
#define DMSI_TARGET_SUPPORTS_HDR(mode_capability)  (mode_capability & SYS_SYS_MODE_MASK_HDR )
#define DMSI_TARGET_SUPPORTS_GSM(mode_capability)  (mode_capability & SYS_SYS_MODE_MASK_GSM )
#define DMSI_TARGET_SUPPORTS_WCDMA(mode_capability)  (mode_capability & SYS_SYS_MODE_MASK_WCDMA )
#define DMSI_TARGET_SUPPORTS_LTE(mode_capability)  (mode_capability & SYS_SYS_MODE_MASK_LTE )
#define DMSI_TARGET_SUPPORTS_TDSCDMA(mode_capability)  (mode_capability & SYS_SYS_MODE_MASK_TDS )

/* invalid SPC used to allow certain service provisioning (defined by carrier)
   without knowing the actual SPC*/
#define SPECIAL_INVALID_SPC "999999"
/*---------------------------------------------------------------------------
  mac_address buffer length
---------------------------------------------------------------------------*/
#define DMSI_MAC_ADDR_LEN                        6

/*---------------------------------------------------------------------------
  Enum values for the PSM state of client
---------------------------------------------------------------------------*/
typedef enum
{
  DMSI_PSM_DEFAULT_STATE     = 0,
  DMSI_PSM_DISABLE_STATE     = 1,
  DMSI_PSM_ENABLE_STATE	     = 2
} qmi_dmsi_psm_state_enum;


//Current threshold is 30 mins
#define MODEM_ACTIVITY_QUERY_THRESHOLD_TIME      1800000 

/*===========================================================================

                                DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  Set Event Report event bitmask
---------------------------------------------------------------------------*/
typedef enum
{
  DMSI_REPORT_STATUS_EV_POWER_STATE     = 0x01,
  DMSI_REPORT_STATUS_EV_ACTIVATION_STATE= 0x04,
  DMSI_REPORT_STATUS_EV_OPRT_MODE       = 0x08,
  DMSI_REPORT_STATUS_EV_WD_STATE        = 0x20,
  DMSI_REPORT_STATUS_EV_PRL_INIT        = 0x40,
  DMSI_REPORT_STATUS_EV_CDMA_LOCK_MODE  = 0x80,
  DMSI_REPORT_STATUS_EV_DEV_MSIM_CAP    = 0x100
} qmi_dmsi_report_status_ev_e_type;

/*---------------------------------------------------------------------------
  DMS client sunscription preference enum
---------------------------------------------------------------------------*/
typedef enum
{
  DMSI_BIND_SUBS_PRIMARY   = 0x00,
#ifdef FEATURE_DUAL_SIM
  DMSI_BIND_SUBS_SECONDARY,
#endif
#ifdef FEATURE_TRIPLE_SIM
  DMSI_BIND_SUBS_TERTIARY,
#endif
  DMSI_BIND_SUBS_MAX
} qmi_dms_client_subs_e_type;


/*---------------------------------------------------------------------------
  QMI DMS Client state info
---------------------------------------------------------------------------*/
typedef struct {
  q_link_type              link;  /*< Data Structure required by the queue 
                                      library to add this as part of a 
                                      queue */                                  
  qmi_req_handle            sns_reset_config_req_handle;
  qmi_client_handle         clnt;
  qmi_req_handle            sns_set_config_req_handle;  
  qmi_req_handle            sns_get_data_req_handle;
  struct 
  {  
    boolean  report_operating_mode;
    boolean  report_device_mode;
    boolean  report_psm_status;
    boolean  report_psm_cfg_change;    
    boolean  report_sensor_policy_met_ind;
  } report_status;  
  qmi_dmsi_psm_state_enum     current_psm_state;  
  qmi_dms_client_subs_e_type subscription_id;
}qmi_dmsi_client_info_type;

typedef struct
{
  q_link_type    link;
  qmi_req_handle req_handle;
  qmi_dmsi_client_info_type* cl_sp;
  int            msg_id;
}ds_qmi_dms_async_cmd_type;

/*---------------------------------------------------------------------------
  QMI DMS Service state info
---------------------------------------------------------------------------*/
typedef struct
{
  q_type                 client_q; /*< Information about all the clients that
                                       have connected to the service. This is queue 
                                       of type qmi_dmsi_client_info_type */
  qmi_csi_service_handle service_handle;
  qmi_sap_client_handle  sap_client_handle;
  uint16                 num_of_clients;  
  q_type                 psm_get_cfg_params_q;
} qmi_dmsi_state_info_type;

dms_psm_version_capability_v01      dms_psm_capability;


typedef struct
{
  uint16 cmd_id;
  uint32 subscription_id;
  union
  {
    struct
    {
      qmi_dmsi_report_status_ev_e_type         evt_type;
      void                                    *evt_value;
      qmi_dmsi_client_info_type*               cl_sp;
    }dms_evt_info;

    struct
    {
      qmi_if_cm_ph_info_type  info;
      cm_ph_event_e_type      event;
    } cm_ph;

    struct
    {
      void *                user_data;
      cm_ph_cmd_err_e_type  err_type;
    } cm_ph_err;

    struct
    {
      void *                 user_data;
      cm_call_cmd_e_type     call_cmd;
      cm_call_cmd_err_e_type err_type;
    } cm_call_err;

    struct
    {
      boolean   enabled;
      uint32    threshold;
      uint32    duration_oos;
      uint32    randomization_win;
      uint32    active_timer;
      uint32    periodic_update_timer;
      uint32    early_wakeup_time;
    } psm_params;

    struct
    {
      cm_psm_status_e_type         status;
      cm_psm_reject_reason_e_type  reject_reason;
      uint32                       periodic_update_timer;
      uint32                       modem_only_timer;
    } psm_status;
    struct
    {
      boolean 	   psm_enabled;		
      uint32	   active_timer;		
      uint32	   periodic_update_timer;		
      uint16	   change_mask;		
    } psm_cfg_changes;
#ifdef FEATURE_REF_TRACKER
    struct
    {
      cm_sns_event_info_s_type sns_indication;
    } sensor_policy_met;
    struct
    {
      cm_qmi_sensor_data_rsp_s_type sns_arr[CM_SNS_TYPE_ENV_SENSOR_MAX];  
      uint8 resp_len;
    } sensor_data_resp;
#endif
    qmi_if_mmgsdi_evt_info_type mmgsdi_evt_info;
	
    //ps_sys_modem_power_stats_info_type  power_stats_info;
  } data;
} qmi_dmsi_cmd_buf_type; //Remove this structure mmgsdi changes are in the MOB build

static qmi_csi_os_params             os_params;
static boolean                       qmi_dmsi_inited = FALSE;

/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS 

===========================================================================*/

/*---------------------------------------------------------------------------
  QMI service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/

static qmi_csi_cb_error  qmi_dmsi_reset(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
static qmi_csi_cb_error  qmi_dmsi_set_event_report(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
static qmi_csi_cb_error  qmi_dmsi_get_device_cap(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
static qmi_csi_cb_error  qmi_dmsi_get_device_mfr(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
static qmi_csi_cb_error  qmi_dmsi_get_device_model_id(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
static qmi_csi_cb_error  qmi_dmsi_get_device_rev_id(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);

static qmi_csi_cb_error  qmi_dmsi_get_msisdn(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
static qmi_csi_cb_error  qmi_dmsi_get_device_serial_numbers(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
static qmi_csi_cb_error  qmi_dmsi_get_device_hardware_rev(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
static qmi_csi_cb_error  qmi_dmsi_get_operating_mode(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
static qmi_csi_cb_error  qmi_dmsi_set_operating_mode(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
static qmi_csi_cb_error  qmi_dmsi_get_time(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
static qmi_csi_cb_error  qmi_dmsi_set_time(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);

#ifdef FEATURE_DATA_QMI_DMS_FACTORY
static qmi_csi_cb_error  qmi_dmsi_restore_factory_defaults(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
#endif /* FEATURE_DATA_QMI_DMS_FACTORY */

static qmi_csi_cb_error  qmi_dmsi_validate_spc(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
static qmi_csi_cb_error  qmi_dmsi_get_band_capability(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);

#ifdef FEATURE_DATA_QMI_DMS_FACTORY
static qmi_csi_cb_error  qmi_dmsi_get_factory_sku(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
#endif /* FEATURE_DATA_QMI_DMS_FACTORY */

static qmi_csi_cb_error  qmi_dmsi_get_sw_version(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
static qmi_csi_cb_error  qmi_dmsi_set_spc(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
static qmi_csi_cb_error qmi_dmsi_trigger_modem_activity_info
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);

static qmi_csi_cb_error qmi_dmsi_get_modem_activity_info
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);

static qmi_csi_cb_error qmi_dmsi_indication_register(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
static qmi_csi_cb_error qmi_dmsi_psm_get_cfg_params(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
static qmi_csi_cb_error qmi_dmsi_psm_enter(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);
static qmi_csi_cb_error qmi_dmsi_psm_set_cfg_params
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);

static qmi_csi_cb_error  qmi_dmsi_get_capability
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);

static qmi_csi_cb_error  qmi_dmsi_set_app_priority
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);


static qmi_csi_cb_error qmi_dmsi_get_app_priority_info(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);

static qmi_error_type_v01 qmi_dmsi_get_device_serial_numbers_ext(
                           boolean,
                           void*,
                           qmi_dmsi_client_info_type *);

static void qmi_dmsi_initialize_client
(
  qmi_dmsi_client_info_type * cl_sp
);

static boolean qmi_dmsi_msg_hdlr_cb
(
  ds_sig_enum_type  sig,
  void              *svc_ptr
);

static qmi_csi_cb_error qmi_dmsi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
);

static void qmi_dmsi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
);

static qmi_csi_cb_error qmi_dmsi_handle_client_req_cb
(
  void                    *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                    *req_c_struct,
  unsigned int             req_c_struct_len,
  void                    *service_handle
);

boolean qmi_dms_check_valid_async_cmd_buf
(
  ds_qmi_dms_async_cmd_type *async_cmd_buf
);
#ifdef FEATURE_REF_TRACKER
static qmi_csi_cb_error qmi_dmsi_set_sensor_configuration
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);

static qmi_csi_cb_error qmi_dmsi_get_sensor_data
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);

static qmi_csi_cb_error qmi_dmsi_reset_sensor_configuration
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);

static qmi_csi_cb_error qmi_dmsi_get_last_policy_met_info
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
);

static qmi_csi_cb_error qmi_dmsi_sensor_policy_met_indication
(
  qmi_dmsi_cmd_buf_type *cmd_buf_ptr
);

static qmi_csi_cb_error qmi_dmsi_sensor_get_data_resp
(
  qmi_dmsi_cmd_buf_type *cmd_buf_ptr
);
#endif
/* Define service handle table for DMS messages */
static qmi_csi_cb_error (* const req_handle_table[])
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
) = 
{
  qmi_dmsi_reset                             ,  /* Request handler for message ID 0x00*/
  qmi_dmsi_set_event_report                  ,  /* Request handler for message ID 0x01*/
  NULL                                       ,  /* Request handler for message ID 0x02*/
  qmi_dmsi_indication_register               ,  /* Request handler for message ID 0x03*/
            NULL,NULL,NULL,NULL,NULL,NULL    ,  /* Request handler for message ID 0x04 to 0x09*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL    ,  /* Request handler for message ID 0x0A to 0x11*/
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL    ,  /* Request handler for message ID 0x12 to 0x19*/
  NULL,NULL,NULL,NULL,NULL,NULL              ,  /* Request handler for message ID 0x1A to 0x1F*/
  qmi_dmsi_get_device_cap                    ,  /* Request handler for message ID 0x20*/
  qmi_dmsi_get_device_mfr                    ,  /* Request handler for message ID 0x21*/
  qmi_dmsi_get_device_model_id               ,  /* Request handler for message ID 0x22*/
  qmi_dmsi_get_device_rev_id                 ,  /* Request handler for message ID 0x23*/
  qmi_dmsi_get_msisdn						 ,	/* Request handler for message ID 0x24*/
  qmi_dmsi_get_device_serial_numbers         ,  /* Request handler for message ID 0x25*/
  NULL					                     ,  /* Request handler for message ID 0x26*/
  NULL,NULL,NULL,NULL,NULL                   ,  /* Request handler for message ID 0x27 to 0x02B*/
  qmi_dmsi_get_device_hardware_rev           ,  /* Request handler for message ID 0x2C*/
  qmi_dmsi_get_operating_mode                ,  /* Request handler for message ID 0x2D*/
  qmi_dmsi_set_operating_mode                ,  /* Request handler for message ID 0x2E*/
  qmi_dmsi_get_time                          ,  /* Request handler for message ID 0x2F*/
  NULL                                       ,  /* Request handler for message ID 0x30*/
  NULL						                 ,  /* Request handler for message ID 0x31*/
  NULL						                 ,  /* Request handler for message ID 0x32*/
  NULL                  					 ,  /* Request handler for message ID 0x33*/
  NULL                                       ,  /* Request handler for message ID 0x34*/
  NULL                                       ,  /* Request handler for message ID 0x35*/
  NULL                                       ,  /* Request handler for message ID 0x36*/
  NULL                                       ,  /* Request handler for message ID 0x37*/
  NULL                                       ,  /* Request handler for message ID 0x38*/
  NULL										 ,  /* Request handler for message ID 0x39*/
#ifdef FEATURE_DATA_QMI_DMS_FACTORY
  qmi_dmsi_restore_factory_defaults          ,  /* Request handler for message ID 0x3A*/
#else
  NULL,
#endif /* FEATURE_DATA_QMI_DMS_FACTORY */
  qmi_dmsi_validate_spc                      ,  /* Request handler for message ID 0x3B*/
  NULL,NULL,NULL                             ,  /* Request handler for message ID 0x3C to 0X3E*/
  NULL                                       ,  /* Request handler for message ID 0x3F*/
  NULL,NULL,NULL,NULL,NULL                   ,  /* Request handler for message ID 0x40 to 0X44*/
  qmi_dmsi_get_band_capability               ,  /* Request handler for message ID 0x45*/
#ifdef FEATURE_DATA_QMI_DMS_FACTORY
  qmi_dmsi_get_factory_sku                   ,  /* Request handler for message ID 0x46*/
#else
  NULL,
#endif /* FEATURE_DATA_QMI_DMS_FACTORY */
  NULL,NULL,NULL,NULL                        ,  /* Request handler for message ID 0x47 to 4A*/
  qmi_dmsi_set_time                          ,  /* Request handler for message ID 0x4B*/
  NULL                                       ,  /* Request handler for message ID 0x4C*/
  NULL                                       ,  /* Request handler for message ID 0x4D*/
  NULL                                       ,  /* Request handler for message ID 0x4E*/
  NULL,NULL                                  ,  /* Request handler for message ID 0x4F to 0x50*/
  qmi_dmsi_get_sw_version                    ,  /* Request handler for message ID 0x51*/
  qmi_dmsi_set_spc                           ,  /* Request handler for message ID 0x52*/
  NULL,
  /*
#ifdef FEATURE_DATA_QMI_DMS_PRL
  qmi_dmsi_get_current_prl_info              ,  /* Request handler for message ID 0x53
#else
  NULL,
#endif /* FEATURE_DATA_QMI_DMS_PRL */
  NULL                                       ,  /* Request handler for message ID 0x54*/
  NULL							             ,  /* Request handler for message ID 0x55*/
  NULL						                 ,  /* Request handler for message ID 0x56*/
  NULL						                 ,  /* Request handler for message ID 0x57*/
  NULL,
  NULL,
  NULL,
  NULL							            ,  /* Request handler for message ID 0x5B*/
  NULL                   					,  /* Request handler for message ID 0x5C*/
  NULL										, /* Request handler for message ID 0x5D*/
  qmi_dmsi_trigger_modem_activity_info       ,  /* Request handler for message ID 0x5E*/
  qmi_dmsi_get_modem_activity_info           ,  /* Request handler for message ID 0x5F*/
  qmi_dmsi_psm_get_cfg_params                ,  /* Request handler for message ID 0x60*/
  qmi_dmsi_psm_enter                         ,  /* Request handler for message ID 0x61*/
  NULL, NULL, NULL, NULL                     ,  /* Request handler for message ID 0x62 - 0x65*/
  qmi_dmsi_psm_set_cfg_params                ,  /* Request handler for message ID 0x66*/
  NULL                                       ,  /* Request handler for message ID 0x67*/
  NULL,NULL,                                    /* Request handler for message ID 0x67*/
  qmi_dmsi_get_capability                    ,  /* Request handler for message ID 0x6A*/
  qmi_dmsi_set_app_priority                  ,  /* Request handler for message ID 0x6B*/
#ifdef FEATURE_REF_TRACKER
  qmi_dmsi_set_sensor_configuration,            /* Request handler for message ID 0x6C*/
  qmi_dmsi_get_sensor_data         ,            /* Request handler for message ID 0x6D*/
  qmi_dmsi_reset_sensor_configuration         , /* Request handler for message ID 0x6E*/
  qmi_dmsi_get_last_policy_met_info           , /* Request handler for message ID 0x6F*/
  NULL                                        , /* Request handler for message ID 0x70*/
#else
  NULL,NULL,NULL,NULL,NULL                    , /* Request handler for message ID 0x6C*/
#endif
  NULL,
  NULL,
  NULL,
  NULL,
  qmi_dmsi_get_app_priority_info                /* Request handler for message ID 0x75*/
};
/*
static void qmi_dmsi_modem_activity_info_ind
(
  qmi_dmsi_client_info_type             *clnt_info,
  ps_sys_modem_power_stats_info_type    *activity_info
);


static void qmi_dmsi_reg_sys_events
( 
  void
);

static void dmsi_power_statistics_info_cb
(
  ps_sys_tech_enum_type                          tech_type,
  ps_sys_event_enum_type                         event_name,
  ps_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
);*/

static int dmsi_compare_qmi_client_handle
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmi_dmsi_client_info_type *clnt_info = (qmi_dmsi_client_info_type*)item_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( (clnt_info->clnt == 
                          (qmi_client_handle)compare_val) ? 1 : 0 );
}

static int dmsi_compare_connection_info_handle
( 
  void *p_ItemPtr,
  void *p_CompareVal 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* item_ptr is an element of the Client Q i.e. Connection Handle, 
     and compare val is also a Connection Handle */
  return ( (p_ItemPtr == p_CompareVal) ? 1 : 0 );
}

static int dmsi_cmd_buf_belongs_to_client
(
  ds_qmi_dms_async_cmd_type *async_cmd_buf,
  qmi_dmsi_client_info_type *cl_sp
)
{
  return (async_cmd_buf->cl_sp == cl_sp)?TRUE:FALSE;
}

static qmi_dmsi_state_info_type  qmi_dms_state;

/*---------------------------------------------------------------------------
  QMI instance configurations
---------------------------------------------------------------------------*/
static void     qmi_dmsi_reset_client(qmi_dmsi_client_info_type *);



typedef enum
{
  QMI_DMSI_ACTIVATION_TYPE_CALL  = 0,
  QMI_DMSI_ACTIVATION_TYPE_OTASP = 1,
  QMI_DMSI_ACTIVATION_TYPE_MAX,
  QMI_DMSI_ACTIVATION_TYPE_MAX32 = 0x10000000
} qmi_dmsi_activation_e_type;

/*---------------------------------------------------------------------------
  QMI_DMS Global state information - initialized in qmi_dms_init()
  Global to all QMI instances
---------------------------------------------------------------------------*/
static struct
{ 
  boolean inited;
#ifdef FEATURE_MMGSDI_SESSION_LIB
  dms_uim_state_enum_v01 uim_state;
#endif /* FEATURE_MMGSDI_SESSION_LIB */

  cm_call_id_type               voice_call_id;
  
  dms_activation_state_enum_v01 activation_state;

  /* used to store NV config values so they only have to be read once 
     at startup */
  boolean nv_allow_invalid_spc;
  boolean nv_restrict_imsi_access;
  boolean nv_use_max_defined_data_rates;
  //boolean nv_use_last_otasp_act_state;
  //boolean cdma_lock_mode;
} qmi_dmsi_global = {FALSE,};

/*---------------------------------------------------------------------------
  Image version reporting
---------------------------------------------------------------------------*/
#define DMSI_PRM_TYPE_IMAGE_VER 0x10

#define IMAGE_INDEX_LEN     2
#define IMAGE_QC_VER_LEN   72
#define IMAGE_VARIANT_LEN  20
#define IMAGE_OEM_VER_LEN  32
#define IMAGE_VER_STR_SIZE 128

/* Image version format 128 bytes */
typedef PACKED struct PACKED_POST
{
  char index[IMAGE_INDEX_LEN];      // Image index
  char colon1;                      // ':'
  char qc_ver[IMAGE_QC_VER_LEN];    // QC image version string
  char variant[IMAGE_VARIANT_LEN];  // Variant
  char colon2;                      // ':'
  char oem_ver[IMAGE_OEM_VER_LEN];  // OEM version string
} qmi_dmsi_image_ver_type;


/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS 

===========================================================================*/
static void qmi_dmsi_event_report_ind
(
  qmi_dmsi_client_info_type *  sp,
  uint32   event_mask,
  uint8    power_status,  /* bitmask */
  uint8    battery_lvl,
  uint8    pin_index,
  uint16   activation_state,
  uint8    operating_mode,
  boolean  wd_state,
  boolean  cdma_lock_mode 
);

static void qmi_dmsi_process_svc_init(void);

static uint8 qmi_dmsi_get_hw_supported_radio_if_list(uint32 *);
static qmi_error_type_v01 qmi_dmsi_get_max_channel_rate(uint32 *, uint32 *);
static uint8 qmi_dmsi_get_sim_capability(void);
static uint64 qmi_dmsi_get_voice_support_capability(void);

#if !(defined(FEATURE_DATA_IOE_LWG) || defined(FEATURE_DATA_IOE_LONLY) || defined(FEATURE_DATA_IOE_LG))
  static uint64 qmi_dmsi_get_simul_voice_and_data_capability(void);
  static uint8 qmi_dmsi_get_voice_capability(void);
  static qmi_error_type_v01 qmi_dmsi_get_device_overall_capability
                          (sys_overall_feature_t*);
#endif
//static void qmi_dmsi_set_battery_level_limits(uint8);
static qmi_error_type_v01 qmi_dmsi_get_multisim_info
                          (dms_multisim_capability_type_v01*);

static qmi_error_type_v01 qmi_dmsi_get_device_config_list
(
  dms_device_config_capability_type_v01* response
);

static qmi_error_type_v01  qmi_dmsi_get_max_subs_static_device_cap
(
  dms_get_device_cap_resp_msg_v01* response
);

void qmi_dms_event_report_req_send_ind
(
   qmi_dmsi_report_status_ev_e_type               event_type,
   void                                         * event_info,
   qmi_dmsi_client_info_type*                     cl_sp
);

#ifdef FEATURE_MMGSDI_SESSION_LIB
  static void qmi_dmsi_generate_read_uim_msisdn_resp
  (
    mmgsdi_read_cnf_type * read_cnf,
    ds_qmi_dms_async_cmd_type* cmd_buf_p
  );
#endif /* FEATURE_MMGSDI_SESSION_LIB */
static qmi_error_type_v01 qmi_dmsi_oprt_mode_from_cm
(
  sys_oprt_mode_e_type,
  dms_operating_mode_enum_v01 *
);

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  static qmi_error_type_v01 qmi_dmsi_get_mdn_min(char *,int,char *,int);
  static boolean qmi_dmsi_minnum_validate(dword, word);
  static dword qmi_dmsi_generate_default_min(dword);
  static int qmi_dmsi_minnum_pcs_to_text(char *, dword, word);
  static void  qmi_dmsi_mobnum_pcs_to_text(char *, nv_mob_dir_data_type *);
  static boolean qmi_dmsi_mobnum_text_to_pcs(char*, uint8, nv_mob_dir_data_type *);
  static boolean qmi_dmsi_min_text_to_min12(char*, uint8, dword*, word*);
  static void qmi_dmsi_generate_current_device_capability_ind( void );

#ifdef FEATURE_DATA_QMI_DMS_FACTORY
  static boolean qmi_dmsi_reset_imsi_components(byte nam);
  static boolean qmi_dmsi_reset_imsi_t_components(byte nam);
#endif /* FEATURE_DATA_QMI_DMS_FACTORY */
/*  static void qmi_dmsi_generate_activation_state_ind(void); */
  static dms_activation_state_enum_v01 qmi_dmsi_calculate_activated_state(
                                                                          void);
#if (defined FEATURE_OTASP)
  static void qmi_dmsi_otasp_forced_call_release(void);
#endif /* (defined FEATURE_OTASP) */
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */


static void qmi_dmsi_generate_current_device_capability_ind( void );

static qmi_error_type_v01 qmi_dmsi_get_device_multisim_current_cap_info
(
  void* response,
  uint8 resp_type

);

static dms_subs_voice_data_capability_enum_v01 qmi_dmsi_is_simul_voice_data 
(
   sys_ue_mode_e_type ue_mode 
);

boolean qmi_dmsi_msgr_hdlr 
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg
);

static void qmi_dmsi_policyman_config_init (void);

static uint64 qmi_dmsi_convert_rat_mask (uint32 rat);


static qmi_error_type_v01 qmi_dmsi_get_subs_device_feature_mode
(
  dms_get_device_cap_resp_msg_v01* response
);

#ifdef FEATURE_DATA_QMI_DMS_MULTISIM
static qmi_error_type_v01 qmi_dmsi_get_max_active_data_subs
(
  uint8_t * response
);
#endif /* FEATURE_DATA_QMI_DMS_MULTISIM */

void qmi_dmsi_send_response
(
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void*                    resp_msg,
  unsigned int             c_struct_len
);

//static void qmi_dmsi_generate_power_stats_ind( ps_sys_modem_power_stats_info_type  *activity_info );

int qmi_dms_is_client_power_stats_query_pending
(
  qmi_dmsi_client_info_type *  q_cl_cp,
  qmi_dmsi_client_info_type *  delete_cl_sp
);

static void qmi_dmsi_generate_psm_status_ind
(
  qmi_dmsi_cmd_buf_type  * cmd_buf_ptr
);

static void qmi_dmsi_generate_psm_get_cfg_params_resp
(
  qmi_dmsi_cmd_buf_type  * cmd_buf_ptr
);
static void qmi_dmsi_generate_psm_cfg_change_ind
(
  qmi_dmsi_cmd_buf_type  * cmd_buf_ptr
);
/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
/*Temp function to resolve the linking error with MOB as ATCOP is not there*/

#define MAX_ITOA_LEN 33 /* Maximum length of ASCII string converted from integer */

void trm_get_info(trm_get_info_input_type *input,trm_get_info_return_type *ret )
{
 return ;
}

/*===========================================================================
  FUNCTION QMI_DMS_CHECK_VALID_ASYNC_CMD_BUF()

  DESCRIPTION
    This Function validates the async command buffer passed.
    It chcks whether cl_sp in async command buffer is valid by comparing it with
    clients queue in dms state.
    
  PARAMETERS
    async_cmd_buf : pointer to the async command buffer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_dms_check_valid_async_cmd_buf
(
  ds_qmi_dms_async_cmd_type *async_cmd_buf
)
{  
  qmi_dmsi_client_info_type * cl_sp = NULL;
  cl_sp = (qmi_dmsi_client_info_type*)q_check(&qmi_dms_state.client_q);
  if ((async_cmd_buf != NULL) && (async_cmd_buf->req_handle != NULL))
  {
    DS_MSG1(MSG_LEGACY_MED,"cl_sp = %0x",async_cmd_buf->cl_sp);
    while(cl_sp != NULL)
    {
      if(cl_sp == async_cmd_buf->cl_sp)
      {
        return TRUE;
      }
      cl_sp = q_next(&qmi_dms_state.client_q,
                           &(cl_sp->link));
    }
  }
  DS_MSG0(MSG_LEGACY_MED,"Invalid Async Command buffer:client might be disconnected");
  return FALSE;
}

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
  FUNCTION QMI_DMS_MMGSDI_EVT_CBACK()

  DESCRIPTION
    Callback function called by mmgsdi to report events

  PARAMETERS
    event  : Info about the event generated

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
#if 0
void qmi_dms_mmgsdi_evt_cback
(
  const mmgsdi_event_data_type *  event
)
{
  qmi_dmsi_cmd_buf_type *  cmd_payload;
  ds_command_type     *cmd_ptr =NULL;
  mmgsdi_slot_id_enum_type slot = MMGSDI_SLOT_AUTOMATIC;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*Received event_cback from MMGSDI , posting cmd to DS */
  
  switch(event->evt)
  {
    case MMGSDI_CARD_INSERTED_EVT:
      slot = event->data.card_inserted.slot;
      break;

    case MMGSDI_CARD_ERROR_EVT:
      slot = event->data.card_error.slot;
      break;

    case MMGSDI_CARD_REMOVED_EVT:
      slot = event->data.card_removed.slot;
      break;

    default:
      break;
  }
  
  switch(event->evt)
  {
    case MMGSDI_CARD_INSERTED_EVT:
    case MMGSDI_CARD_ERROR_EVT:
    case MMGSDI_CARD_REMOVED_EVT:

   /*-----------------------------------------------------------------------
    Get a DS command buffer
   -----------------------------------------------------------------------*/
   cmd_ptr = ds_allocate_cmd_buf(sizeof(qmi_dmsi_cmd_buf_type));
   if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
   {
     DS_ERR_FATAL("Out of memory");
     return ;
    }
    cmd_ptr->hdr.cmd_id = DS_QMI_DMS_SVC_CMD;
    cmd_payload = (qmi_dmsi_cmd_buf_type *)cmd_ptr->cmd_payload_ptr;

	cmd_payload->cmd_id = QMI_CMD_PROCESS_UIM_GET_STATE;
    cmd_payload->data.mmgsdi_evt_info.event = event->evt;
    cmd_payload->data.mmgsdi_evt_info.slot = slot;
    cmd_payload->data.mmgsdi_evt_info.card_err_info = 
                 event->data.card_error.info;
      break;

    default:
      return;
  }

  //dcc_send_cmd_ex(DCC_DMS_SVC_CMD, cmd_ptr);
  ds_put_cmd(cmd_ptr);
} /* qmi_dms_mmgsdi_evt_cback */

#endif

/*===========================================================================
  FUNCTION QMI_DMS_PROCESS_MMGSDI_OPERATION_COMPLETE()

  DESCRIPTION
    To send a response to the QMI pin operation requested by the client.
    
  PARAMETERS
    cnf_type : Type of MMGSDI PIN confirmation
    cnf_data : Response data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dms_process_mmgsdi_operation_complete
(
  mmgsdi_cnf_enum_type       cnf_type,
  mmgsdi_cnf_type*           cnf_data
)
{
  ds_qmi_dms_async_cmd_type* async_cmd_buf = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  async_cmd_buf = (ds_qmi_dms_async_cmd_type*)cnf_data->response_header.client_data;
  if(async_cmd_buf == NULL)
  {
    /* QMI should always pass command buffer for MMGSDI read */
    DS_MSG0(MSG_LEGACY_MED,"Null client_data ptr in callback for MMGSDI read");
    return;
  }
  else
  {
    if (qmi_dms_check_valid_async_cmd_buf(async_cmd_buf))
    {
      /* Reassigning the client data to the original command buffer */
      switch(cnf_type)
      {
        case MMGSDI_READ_CNF:      
          qmi_dmsi_generate_read_uim_msisdn_resp(
              &(cnf_data->read_cnf),async_cmd_buf);
          break;
    
        default:
          DS_MSG1(MSG_LEGACY_MED,"Received unhandled cnf_type %d from mmgsdi", cnf_type);
          break;
      }
    }
    else
    {
      DS_SYSTEM_HEAP_MEM_FREE(async_cmd_buf);
      DS_MSG0(MSG_LEGACY_MED, "Recd unsolicited MMGSDI read_cnf" );
    }
  }
} /* qmi_dms_process_mmgsdi_operation_complete */

/*===========================================================================
  FUNCTION QMI_DMS_PROCESS_UIM_GET_STATE()

  DESCRIPTION
    To process event received for SIM status.
    
  PARAMETERS
    event    : MMGSDI event that was generated
    card_err_info: Contains card error code
    session_activated : Contains if session has been activated

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
#if 0
void qmi_dms_process_uim_get_state
(
  mmgsdi_events_enum_type event,
  mmgsdi_card_err_info_enum_type card_err_info,
  boolean session_activated
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 DS_MSG2(MSG_LEGACY_MED,"DMS received mmgsdi event %d for sim status with card_err_info %d ", event,card_err_info);

  switch(event)
  {
    case MMGSDI_CARD_INSERTED_EVT:
      qmi_dmsi_global.uim_state = DMS_UIM_INITIALIZATION_FAILED_V01;
      break;
    /* SessionChanged replaces SelectAid ev, means init in progress */
    case MMGSDI_SESSION_CHANGED_EVT: 
      if ((DMS_UIM_NOT_PRESENT_V01 == qmi_dmsi_global.uim_state) &&
           (FALSE == session_activated))
      {
        DS_MSG0(MSG_LEGACY_MED,"Ignoring MMGSDI_SESSION_CHANGED_EVT since it is deactivated and SIM is not inserted ");
        /* Dont generate indication since there is no change in state */
        return;
      }
      else
      {
        qmi_dmsi_global.uim_state = DMS_UIM_INITIALIZATION_FAILED_V01;
      }
      break;

    /* Card removed ev is seen only on targets with some special hardware */
    case MMGSDI_CARD_REMOVED_EVT: 
      qmi_dmsi_global.uim_state = DMS_UIM_NOT_PRESENT_V01;
      break;

    case MMGSDI_SUBSCRIPTION_READY_EVT:
      qmi_dmsi_global.uim_state = DMS_UIM_INITIALIZATION_COMPLETED_V01;
      break;

    case MMGSDI_CARD_ERROR_EVT:
      /* Checking if card is present in the slot or not */
      /* If the hardware supports the mechanism to detect presence of the card, 
         MMGSDI_CARD_ERR_CARD_REMOVED will be sent when card gets removed. Else 
         one of the other causes will be sent for card removal or power up 
         without card
      */
      if((MMGSDI_CARD_ERR_NO_ATR_RCVD_AT_MAX_VOLT == card_err_info) ||
         (MMGSDI_CARD_ERR_CARD_REMOVED == card_err_info) ||
         (MMGSDI_CARD_ERR_NO_ATR_RCVD_AFTER_RESET == card_err_info) ||
         (MMGSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES == card_err_info))
      {
        qmi_dmsi_global.uim_state = DMS_UIM_NOT_PRESENT_V01 ;
      }
      else
      {
        qmi_dmsi_global.uim_state = DMS_UIM_INITIALIZATION_FAILED_V01;
      }
      break;

    default:
      return;
  }

} /* qmi_dms_process_uim_get_state */


#endif /* FEATURE_MMGSDI_SESSION_LIB */
#endif
/*===========================================================================
  FUNCTION QMI_DMS_INIT()

  DESCRIPTION
    Register the DMS service with QMI Framework 

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dms_init
(
  void
)
{
  qmi_csi_error         rc;
  qmi_sap_error         src;
  qmi_csi_options       options;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG1(MSG_LEGACY_MED,"QMI DMS service init %d", qmi_dmsi_inited);
  if (!qmi_dmsi_inited)
  {
    /*-------------------------------------------------------------------------
      Setting DCC signal handler for handling QMI messages
    -------------------------------------------------------------------------*/
    (void)reg_set_sig_handler(DS_DMS_RDR_SIGNAL, 
                              qmi_dmsi_msg_hdlr_cb, &qmi_dms_state);

    (void)rex_set_sigs( ds_get_ds_task_ptr(),
                      (rex_sigs_type) 1 << (rex_sigs_type)(DS_DMS_RDR_SIGNAL) ); 
    return;
  }
  os_params.tcb = ds_get_ds_task_ptr();
  os_params.sig = DMSI_QCSI_OS_SIGNAL;
  QMI_CSI_OPTIONS_INIT(options);
  (void)mqcsi_register_log_options(&options);
  
  /*-------------------------------------------------------------------------
    Register QMI dms service with QCSI
  -------------------------------------------------------------------------*/
  rc = qmi_csi_register_with_options(dms_get_service_object_v01(),
                        qmi_dmsi_svc_connect_cb,
                        qmi_dmsi_svc_disconnect_cb, 
                        qmi_dmsi_handle_client_req_cb, 
                        &qmi_dms_state, &os_params, 
			&options,
                        &qmi_dms_state.service_handle);
  //register with SAP
  src = qmi_sap_register(dms_get_service_object_v01(), 
                         NULL, 
                         &qmi_dms_state.sap_client_handle);

  /*Check whether registration is successful with QCSI and QSAP or not*/
  if ( (src != QMI_SAP_NO_ERR) || (rc != QMI_CSI_NO_ERR))
  {
    DS_MSG2(MSG_LEGACY_ERROR,"Unable to register dms service with QSAP or QCSI ! Error src = %d rc = %d",
                     src, rc);
    DS_ASSERT(0);
    return;
  }

  /* Initialize the queue which will hold all handles of clients */
  (void) q_init(&qmi_dms_state.client_q);
  qmi_dms_state.num_of_clients = 0;
  qmi_dmsi_process_svc_init();

  dms_psm_capability = DMS_PSM_VERSION_2_V01;
  
} /* qmi_dms_init */

static boolean qmi_dmsi_msg_hdlr_cb
(
  ds_sig_enum_type  sig,
  void              *svc_ptr
)
{
  qmi_dmsi_state_info_type *dms_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ASSERT((sig == DS_DMS_RDR_SIGNAL) && svc_ptr);

  if (!qmi_dmsi_inited)
  {
    /* Call init again to register with QCSI now
     * Wait for all tasks to be inited */
    qmi_dmsi_inited = TRUE;
    qmi_dms_init();
    return TRUE;
  }

  dms_sp = (qmi_dmsi_state_info_type*) svc_ptr;

  qmi_csi_handle_event(dms_sp->service_handle, &os_params);
  return TRUE;
} /* qmi_dmsi_msg_hdlr_cb */

static qmi_csi_cb_error qmi_dmsi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
)
{
  qmi_dmsi_client_info_type    *clnt_info_ptr =  NULL;
  qmi_dmsi_state_info_type     *dms_svc_ptr;
  qmi_csi_cb_error              rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_ASSERT(connection_handle && service_handle);

  dms_svc_ptr = (qmi_dmsi_state_info_type *) service_handle;
 
  if ( NULL != q_linear_search( &dms_svc_ptr->client_q,
                                dmsi_compare_qmi_client_handle,
                                client_handle ) )
  {
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  if (DMSI_MAX_CLIDS == dms_svc_ptr->num_of_clients)
  {
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  DS_SYSTEM_HEAP_MEM_ALLOC(clnt_info_ptr,
  	                       sizeof(qmi_dmsi_client_info_type),
  	                       qmi_dmsi_client_info_type*);
  if(!clnt_info_ptr)
  {
    rc = QMI_CSI_CB_CONN_REFUSED;
    goto bail;
  }

  qmi_dmsi_initialize_client(clnt_info_ptr);
  (void) q_link ( clnt_info_ptr, &clnt_info_ptr->link );
  clnt_info_ptr->clnt = client_handle;
  /* Add it to the queue of clients */
  q_put( &(dms_svc_ptr->client_q), &(clnt_info_ptr->link) );
  dms_svc_ptr->num_of_clients++;

  *connection_handle = clnt_info_ptr;
  rc = QMI_CSI_CB_NO_ERR;
bail:
  DS_MSG4(MSG_LEGACY_MED,"dmsi_svc_connect_cb:rc (%d) clnt_info_ptr (0x%p),client_handle (0x%p),num_of_clients (%d)",
                  rc,clnt_info_ptr,client_handle,dms_svc_ptr->num_of_clients);
  return rc;
} /* qmi_dmsi_svc_connect_cb() */

static void qmi_dmsi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
)
{
  qmi_dmsi_state_info_type    *dms_svc_ptr;
  qmi_dmsi_client_info_type    *clnt_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(connection_handle && service_handle);
  
  dms_svc_ptr = (qmi_dmsi_state_info_type *)service_handle;

  DS_MSG1(MSG_LEGACY_MED,"dmsi_svc_disconnect_cb: Releasing client 0x%p", 
                connection_handle);

  clnt_info_ptr = q_linear_search( &dms_svc_ptr->client_q,
                                   dmsi_compare_connection_info_handle,
                                   connection_handle );
  if(NULL == clnt_info_ptr)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"dmsi_svc_connect_cb: dms client 0x%p not found",
                  connection_handle);
    return;

  }
  qmi_dmsi_reset_client(clnt_info_ptr);
  if(TRUE != q_delete_ext( &dms_svc_ptr->client_q,
                             &clnt_info_ptr->link ) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"dmsi_svc_connect_cb: dms client 0x%p could not be deleted from queue",
                    clnt_info_ptr);
    return;
  }
  DS_SYSTEM_HEAP_MEM_FREE(clnt_info_ptr);
  dms_svc_ptr->num_of_clients--;
} /* qmi_dmsi_svc_disconnect_cb() */


static qmi_csi_cb_error qmi_dmsi_handle_client_req_cb
(
  void*                    connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void*                    req_c_struct,
  unsigned int             req_c_struct_len,
  void*                    service_handle
)

{
  qmi_csi_cb_error               rc;
  qmi_dmsi_client_info_type     *clnt_info_ptr;
  qmi_dmsi_state_info_type      *dms_svc_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(connection_handle && service_handle);
  
  rc = QMI_CSI_CB_NO_ERR;
  dms_svc_ptr = (qmi_dmsi_state_info_type*) service_handle;

  clnt_info_ptr = q_linear_search( &dms_svc_ptr->client_q,
                                   dmsi_compare_connection_info_handle,
                                   connection_handle );
  while(1)
  {
    if(NULL == clnt_info_ptr)
    {
      rc = QMI_CSI_CB_INTERNAL_ERR;
      break;
    }
    if(msg_id < (sizeof(req_handle_table) / sizeof(*req_handle_table)))
    {
      if(req_handle_table[msg_id])
      {
        DS_MSG1(MSG_LEGACY_MED,"Handling DMS MSG ID: (%d)", msg_id);
        rc = req_handle_table[msg_id] (clnt_info_ptr, req_handle, msg_id,
                                       req_c_struct, req_c_struct_len,
                                       service_handle);
        break;
  	  
      }
      else
      {
        rc = QMI_CSI_CB_UNSUPPORTED_ERR;
        break;
      }
    }
    else
    {
      rc = QMI_CSI_CB_UNSUPPORTED_ERR;
      break;
    }
  }
  DS_MSG2(MSG_LEGACY_MED,"dmsi_handle_req_cb: message ID: (%d) rc (%d)", msg_id,rc);
  return rc;
} /* qmi_dmsi_handle_client_req_cb() */


/*===========================================================================
  FUNCTION QMI_DMS_GENERATE_OPRT_MODE_IND()

  DESCRIPTION
    Called when CM notifies us that the operating mode has changed.  Sends
    an indication with the new operating mode to clients registered for
    the event.
        
  PARAMETERS
    new_oprt_mode : new operating mode (if this is LPM, QMI DMS will
                    determine if it is persistent LPM on its own)
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dms_generate_oprt_mode_ind
(
  sys_oprt_mode_e_type  new_cm_oprt_mode
)
{
  qmi_dmsi_client_info_type *    cl_sp = NULL;
  dms_operating_mode_enum_v01      new_dms_oprt_mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  new_dms_oprt_mode = DMS_OP_MODE_ONLINE_V01;
  if(!qmi_dmsi_inited)
  {
    return;
  }
  if (QMI_ERR_NONE_V01 != qmi_dmsi_oprt_mode_from_cm(new_cm_oprt_mode,
                                                 &new_dms_oprt_mode))
  {
    return;
  }
  cl_sp = (qmi_dmsi_client_info_type*)q_check(&qmi_dms_state.client_q);
  while(cl_sp != NULL)
  {
    if( cl_sp->report_status.report_operating_mode)
    {
      qmi_dmsi_event_report_ind(cl_sp,
                                DMSI_REPORT_STATUS_EV_OPRT_MODE,
                                0,
                                0,
                                0,
                                0,
                                (uint8) new_dms_oprt_mode,
                                0,
                                0);
    }
    cl_sp = q_next(&qmi_dms_state.client_q,
                         &(cl_sp->link));
  }

} /* qmi_dms_generate_oprt_mode_ind() */

/*===========================================================================
  FUNCTION QMI_DMS_GENERATE_SET_OPRT_MODE_RESP()

  DESCRIPTION
    Called when cmd_cb() supplied in cm_ph_cmd_set_oprt_mode() is called 
    by CM. Sends response to control point for the corresponding
    Set Operating Mode Reqs
        
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
void qmi_dms_generate_set_oprt_mode_resp
(
  void *               user_data,
  cm_ph_cmd_err_e_type ph_cmd_err
)
{
  qmi_error_type_v01                  errval = QMI_ERR_NONE_V01 ;
  qmi_result_type_v01                 result;
  ds_qmi_dms_async_cmd_type*          qmi_resp_buf_p = NULL;
  dms_set_operating_mode_resp_msg_v01  resp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  qmi_resp_buf_p = (ds_qmi_dms_async_cmd_type*)(user_data);
  
  if(qmi_resp_buf_p == NULL )
  {
    DS_MSG0(MSG_LEGACY_MED,"Invalid resp_buf_ptr");
    return;
  }
  
  if ( qmi_dms_check_valid_async_cmd_buf(qmi_resp_buf_p) == FALSE )
  {
    DS_MSG0(MSG_LEGACY_MED,"Invalid resp_buf_ptr");
    DS_SYSTEM_HEAP_MEM_FREE(qmi_resp_buf_p);
    return;
  }
  memset(&resp_msg,0,sizeof(dms_set_operating_mode_resp_msg_v01));
  /*-------------------------------------------------------------------------
    Send SUCCESS or FAILURE response to control point based on ph_cmd_err
  -------------------------------------------------------------------------*/
  switch (ph_cmd_err)
  {
    case CM_PH_CMD_ERR_NOERR:
      errval = QMI_ERR_NONE_V01;
      break;

    case CM_PH_CMD_ERR_NO_BUF_L:
      errval = QMI_ERR_NO_MEMORY_V01;
      break;

    case CM_PH_CMD_ERR_IN_USE_S:
      errval = QMI_ERR_DEVICE_IN_USE_V01;
      break;

    case CM_PH_CMD_ERR_OPRT_MODE_S:
      errval = QMI_ERR_INVALID_TRANSITION_V01; //error_code
      break;

    case CM_PH_CMD_ERR_RF_NOT_READY:
      errval = QMI_ERR_DEVICE_NOT_READY_V01; //Temporary Error
      break;

    default:
      errval = QMI_ERR_INTERNAL_V01;
      break;
  }

  result = (errval == QMI_ERR_NONE_V01? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  qmi_dmsi_send_response(qmi_resp_buf_p->req_handle,
                         qmi_resp_buf_p->msg_id,
                         &resp_msg,
                         sizeof(dms_set_operating_mode_resp_msg_v01));
  DS_SYSTEM_HEAP_MEM_FREE(qmi_resp_buf_p);
} /* qmi_dms_generate_set_oprt_mode_resp() */

/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_DMSI_RESET()

  DESCRIPTION
    Reset the issuing DMS client's state

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle


  RETURN VALUE
    qmi_csi_cb_error types.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_reset
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  dms_reset_resp_msg_v01       resp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&resp_msg,0,sizeof(dms_reset_resp_msg_v01));
  /*-------------------------------------------------------------------------
    Reset the client's state
  -------------------------------------------------------------------------*/
  qmi_dmsi_reset_client(clnt_info);
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_reset_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;

} /* qmi_dmsi_reset() */

/*===========================================================================
FUNCTION  QMI_DMSI_CM_PH_CB

DESCRIPTION
  CM phone command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
/* ARGSUSED */
static void qmi_dmsi_cm_ph_cb 
(
  void                         *data_ptr,         /* Data block pointer    */
  cm_ph_cmd_e_type              ph_cmd,              /* Command ID            */
  cm_ph_cmd_err_e_type          ph_cmd_err           /* Command error code    */
)
{
  qmi_dmsi_cmd_buf_type *  cmd_payload =NULL;
  ds_command_type         *cmd_ptr =NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
   Get a DS command buffer
   -----------------------------------------------------------------------*/
   cmd_ptr = ds_allocate_cmd_buf(sizeof(qmi_dmsi_cmd_buf_type));
   if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
   {
     DS_ERR_FATAL("Out of memory");
     return ;
    }
    cmd_ptr->hdr.cmd_id = DS_QMI_DMS_SVC_CMD;
    cmd_payload = (qmi_dmsi_cmd_buf_type *)cmd_ptr->cmd_payload_ptr;

	switch(ph_cmd)
	{
      case CM_PH_CMD_OPRT_MODE:
        cmd_payload->cmd_id = QMI_CMD_SET_OPERATING_MODE_RESP;
        break;

      default:
        DS_MSG0(MSG_LEGACY_MED,"Unexpected ph_cmd received in qmi_dmsi_cm_ph_cb");
    }
	cmd_payload->data.cm_ph_err.user_data = data_ptr;
    cmd_payload->data.cm_ph_err.err_type = ph_cmd_err;
	ds_put_cmd(cmd_ptr);
  /*-------------------------------------------------------------------------
    Post a cmd to DCC to handle this in DCC context
  -------------------------------------------------------------------------*/
  return;
} /* qmi_dmsi_cm_ph_cb */

/*===========================================================================
  FUNCTION QMI_DMSI_GET_DEVICE_CAP()

  DESCRIPTION
    Gives the device capabilities

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle


  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_get_device_cap
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  dms_get_device_cap_resp_msg_v01 resp_msg;
  qmi_error_type_v01 errval = QMI_ERR_NONE_V01;
  qmi_result_type_v01  result;
  uint32 max_tx_channel_rate;
  uint32 max_rx_channel_rate;
#if !(defined(FEATURE_DATA_IOE_LWG) || defined(FEATURE_DATA_IOE_LONLY) || defined(FEATURE_DATA_IOE_LG))  
  uint64 sglte_info = 0;
#endif //!(defined(FEATURE_DATA_IOE_LWG) || defined(FEATURE_DATA_IOE_LONLY))
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&resp_msg,0,sizeof(dms_get_device_cap_resp_msg_v01));

  errval = qmi_dmsi_get_device_config_list(&(resp_msg.device_config_list));
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }
  else
  {
    resp_msg.device_config_list_valid = TRUE;
  }

  errval = qmi_dmsi_get_max_subs_static_device_cap((void*)&resp_msg);
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }
  else
  {
    resp_msg.device_max_subscription_static_capability_valid = TRUE;
  }

  /*-------------------------------------------------------------------------
    Get the device multisim info
  -------------------------------------------------------------------------*/
  errval = qmi_dmsi_get_multisim_info(&(resp_msg.multisim_capability));
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }
  else
  {
    resp_msg.multisim_capability_valid = TRUE;
  }

   /*do call and fill the 3 tlvs requied for this*/
   /*Sending FALSE as second arg for get_device message*/
  errval = qmi_dmsi_get_device_multisim_current_cap_info((void*)&resp_msg,FALSE);
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }
  else
  {
    resp_msg.current_multisim_capability_valid = TRUE;
    resp_msg.current_subscription_capability_valid = TRUE;
    resp_msg.subs_voice_data_capability_valid = TRUE;
  }

  /*-------------------------------------------------------------------------
    Get the device feature mode 
  -------------------------------------------------------------------------*/
  errval = qmi_dmsi_get_subs_device_feature_mode(&resp_msg);
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }
  else
  {
    resp_msg.subs_device_feature_mode_valid = TRUE;
  }

#ifdef FEATURE_DATA_QMI_DMS_MULTISIM
  /*-------------------------------------------------------------------------
    Get the Max active subs
  -------------------------------------------------------------------------*/
  errval = qmi_dmsi_get_max_active_data_subs(&(resp_msg.max_active_data_subscriptions));
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }
  else
  {
    resp_msg.max_active_data_subscriptions_valid = TRUE;
  }
#endif /* FEATURE_DATA_QMI_DMS_MULTISIM */

  /*-------------------------------------------------------------------------
    Get the list of supported radio ifs
  -------------------------------------------------------------------------*/
  resp_msg.device_capabilities.radio_if_list_len = 
    qmi_dmsi_get_hw_supported_radio_if_list( (uint32*)&(resp_msg.device_capabilities.radio_if_list));
  
  /*-------------------------------------------------------------------------                                           
    Get the max TX and RX channel rates
  -------------------------------------------------------------------------*/
  errval = qmi_dmsi_get_max_channel_rate(&max_rx_channel_rate,
                                         &max_tx_channel_rate);
  if(errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  resp_msg.device_capabilities.max_tx_channel_rate = max_tx_channel_rate;
  resp_msg.device_capabilities.max_rx_channel_rate = max_rx_channel_rate;

  /*-------------------------------------------------------------------------
    Get the SIM capability of the device
  -------------------------------------------------------------------------*/
  resp_msg.device_capabilities.sim_capability = qmi_dmsi_get_sim_capability();

#if !(defined(FEATURE_DATA_IOE_LWG) || defined(FEATURE_DATA_IOE_LONLY) || defined(FEATURE_DATA_IOE_LG))

  /*-------------------------------------------------------------------------
    Get the voice capability of the device
  -------------------------------------------------------------------------*/
  resp_msg.device_capabilities.data_service_capability = 
                                 (uint32)qmi_dmsi_get_voice_capability();

  /*-------------------------------------------------------------------------
    Get the Simultaneous Voice and Data capability of the device
  -------------------------------------------------------------------------*/
  resp_msg.simul_voice_and_data_capability =
            qmi_dmsi_get_simul_voice_and_data_capability();
  resp_msg.simul_voice_and_data_capability_valid = TRUE;

  sglte_info = resp_msg.simul_voice_and_data_capability;
  /*-------------------------------------------------------------------------
    Get the Device Service capability of the device
  -------------------------------------------------------------------------*/
  resp_msg.device_service_capability_valid = TRUE;
  resp_msg.device_service_capability  = (dms_device_service_capability_enum_v01)
    ((sglte_info & QMI_DMS_MASK_SGLTE_CAPABLE_V01)?DMS_DEVICE_CAP_SIMUL_VOICE_AND_DATA_V01:
     (uint32)qmi_dmsi_get_voice_capability());
	 
  resp_msg.device_capabilities.data_service_capability = 
    (sglte_info & QMI_DMS_MASK_SGLTE_CAPABLE_V01)?DMS_DATA_CAP_SIMUL_CS_AND_PS_V01:
       resp_msg.device_capabilities.data_service_capability;
#else
 /*-------------------------------------------------------------------------
  As TRM info was not availble, send some defailt value in MANDATORY TLV
 -------------------------------------------------------------------------*/
 resp_msg.device_capabilities.data_service_capability = 
		(uint32)DMS_DEVICE_CAP_DATA_ONLY_V01;
#endif

  /*-------------------------------------------------------------------------
    Get the Voice Support capability of the device
  -------------------------------------------------------------------------*/
  resp_msg.voice_support_capability =
            qmi_dmsi_get_voice_support_capability();
  if(resp_msg.voice_support_capability != 0)
  {
    resp_msg.voice_support_capability_valid = TRUE;
  }

send_result:
  result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_get_device_cap_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
} /* qmi_dmsi_get_device_cap() */



/*===========================================================================
  FUNCTION QMI_DMSI_GET_DEVICE_MFR()

  DESCRIPTION
    Gives the name of the device manufacturer.
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_get_device_mfr
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  char *             mfr_info;
  qmi_error_type_v01   errval = QMI_ERR_NONE_V01;
  qmi_result_type_v01  result;
  nv_stat_enum_type    nv_status;
  nv_item_type         *ds_nv_item_ptr;
  dms_get_device_mfr_resp_msg_v01 resp_msg ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&resp_msg,0,sizeof(dms_get_device_mfr_resp_msg_v01));
  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(ds_nv_item_ptr, sizeof(nv_item_type),
                           nv_item_type*);
  if( ds_nv_item_ptr == NULL )
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }
  
  /*-------------------------------------------------------------------------
    Query NV for Model Identification
  -------------------------------------------------------------------------*/
  nv_status = ds_get_nv_item(NV_UNDP_HSU_MFGSTR_I, ds_nv_item_ptr);
  if ((nv_status == NV_DONE_S) && 
                        strlen((char *)ds_nv_item_ptr->undp_hsu_mfgstr))
  {
    mfr_info =(char *) ds_nv_item_ptr->undp_hsu_mfgstr;
  } 
  else
  {
    mfr_info = qmi_device_mfr_str;
  }
  strlcpy((char*)(resp_msg.device_manufacturer),mfr_info,sizeof(resp_msg.device_manufacturer));
  /*-------------------------------------------------------------------------
    Free the temporary memory allocated for NV item.
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);
send_result:
  result = (errval == QMI_ERR_NONE_V01? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_get_device_mfr_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
  
} /* qmi_dmsi_get_device_mfr() */



/*===========================================================================
  FUNCTION QMI_DMSI_GET_DEVICE_MODEL_ID()

  DESCRIPTION
    Gives the model ID of the device in use
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_get_device_model_id
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  dms_get_device_model_id_resp_msg_v01 resp_msg;
  char               model_id_info[QMI_DMS_DEVICE_MODEL_ID_MAX_V01];
  nv_stat_enum_type  nv_status;
  qmi_error_type_v01   errval =  QMI_ERR_NONE_V01;
  qmi_result_type_v01  result;
  nv_item_type      *ds_nv_item_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(model_id_info, 0, sizeof(char)*QMI_DMS_DEVICE_MODEL_ID_MAX_V01);
  memset(&resp_msg,0,sizeof(dms_get_device_model_id_resp_msg_v01));
  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(ds_nv_item_ptr, sizeof(nv_item_type),
                           nv_item_type*);
  if( ds_nv_item_ptr == NULL )
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Query NV for Model Identification
  -------------------------------------------------------------------------*/
  nv_status = ds_get_nv_item(NV_UNDP_HSU_PRODSTR_I, ds_nv_item_ptr);
  if( nv_status == NV_DONE_S )
  {
    (void) strlcpy((char*)model_id_info,
                    (const char*)ds_nv_item_ptr->undp_hsu_prodstr,
                    sizeof(resp_msg.device_model_id));
  }
  if ((nv_status != NV_DONE_S) || (0 == strlen(model_id_info)))
  {
    nv_status = ds_get_nv_item(NV_MOB_MODEL_I, ds_nv_item_ptr);
    if(nv_status == NV_DONE_S)
    {
#ifdef FEATURE_DATA_ATCOP
      (void) dsatutil_itoa(ds_nv_item_ptr->mob_model, 
                           (unsigned char*)model_id_info,
                           DMSI_BASE_DECIMAL);
#endif /* FEATURE_DATA_ATCOP */
    }
    else
    {
      /*-----------------------------------------------------------------------
        Model not found in NV, hence using the global Model variable
      -----------------------------------------------------------------------*/  
#ifdef FEATURE_DATA_ATCOP      
      (void) dsatutil_itoa(mob_model,
                           (unsigned char*)model_id_info, 
                           DMSI_BASE_DECIMAL);
#endif /* FEATURE_DATA_ATCOP */
    }
  }

  /*-----------------------------------------------------------------------
    Make sure that the string is NULL terminated so that strlen will work
  -----------------------------------------------------------------------*/  
  strlcpy((char*)resp_msg.device_model_id,(char*)model_id_info,sizeof(resp_msg.device_model_id));
  /*-------------------------------------------------------------------------
    Free the temporary memory allocated for NV item.
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);

send_result:
  result = (errval == QMI_ERR_NONE_V01? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_get_device_model_id_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
 
} /* qmi_dmsi_get_device_model_id() */



/*===========================================================================
  FUNCTION QMI_DMSI_GET_DEVICE_REV_ID()

  DESCRIPTION
    Gives the revision ID of the device in use
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_get_device_rev_id
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
/* boot_block_version isn't defined for NAND-based (6k and later) targets.
 * We need to send the default NOR boot block version in our response, for
 * backward compatibility. */
#define QMI_BOOT_BLOCK_VERSION 0x1

  char                 dev_rev_id_info[QMI_DMS_DEVICE_REV_ID_MAX_V01];
  qmi_error_type_v01   errval =  QMI_ERR_NONE_V01;
  qmi_result_type_v01  result;
  dms_get_device_rev_id_resp_msg_v01 resp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&resp_msg,0,sizeof(dms_get_device_rev_id_resp_msg_v01));
  memset(dev_rev_id_info, 0, sizeof(dev_rev_id_info));
  /*-------------------------------------------------------------------------
    Populate the revision ID info
  -------------------------------------------------------------------------*/
  (void) snprintf (dev_rev_id_info, QMI_DMS_DEVICE_REV_ID_MAX_V01,
           "%s  %d  [%s %s]",
           mob_sw_rev,
           QMI_BOOT_BLOCK_VERSION,
           rel_date,
           rel_time);

  strlcpy((char*)&(resp_msg.device_rev_id),dev_rev_id_info,sizeof(resp_msg.device_rev_id));
  result = (errval == QMI_ERR_NONE_V01? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_get_device_rev_id_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
} /* qmi_dmsi_get_device_rev_id() */


/*===========================================================================
  FUNCTION QMI_DMSI_GET_MSISDN()

  DESCRIPTION
    Gives the msisdn information(MDN) of the device
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_get_msisdn
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  qmi_error_type_v01        errval = QMI_ERR_NONE_V01;
  qmi_result_type_v01       result;
  dms_get_msisdn_resp_msg_v01 resp_msg;
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  char                    mob_num[NV_DIR_NUMB_PCS_SIZ + 1];
  char                    min_num[NV_DIR_NUMB_PCS_SIZ+1];
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */

#ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_return_enum_type mmgsdi_status;
  mmgsdi_access_type      sim_filename;
  qmi_if_info_type       *qmi_if_info_ptr;
  qmi_dmsi_client_info_type * dms_cl_sp;
  ds_qmi_dms_async_cmd_type *async_cmd_buf = NULL;
  mmgsdi_session_type_enum_type session_type;
  qmi_if_mmgsd_session_e_type session_index;
#endif /* FEATURE_MMGSDI_SESSION_LIB */
  ds3g_mmgsdi_info_type  *ds_mmgsdi_ptr =NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&resp_msg,0,sizeof(dms_get_msisdn_resp_msg_v01));
#ifdef FEATURE_MMGSDI_SESSION_LIB
  //Get the ds_mmgsdi ptr from the PDNMGR.
  ds_mmgsdi_ptr = ds3g_get_mmgsdi_info_hndl();
  if(ds_mmgsdi_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Can not get the mmgsdi info ptr from the PDNMGR !!");	
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  if(ds_mmgsdi_ptr->session_id == QMI_IF_UNUSED_SESSION_ID)
  {    
     DS_MSG0(MSG_LEGACY_ERROR,"Can not get the valid mmgsdi session  ptr from the PDNMGR !!");	   
     errval = QMI_ERR_INTERNAL_V01;  
     goto send_result;
  }
  dms_cl_sp = (qmi_dmsi_client_info_type*)clnt_info;
  qmi_if_info_ptr = qmi_if_info_get_ptr();
  /* check that returning IMSI is allowed and that the UIM is ready.  if not,
     ignore the IMSI since it is optional. */
  
  session_type = MMGSDI_GW_PROV_PRI_SESSION;
  session_index = QMI_IF_MMGSD_GW_PROV_PRI_SESSION;
  
#ifdef FEATURE_DUAL_SIM  
  if(dms_cl_sp->subscription_id == DMSI_BIND_SUBS_SECONDARY) 
  {
    session_type = MMGSDI_GW_PROV_SEC_SESSION;
    session_index = QMI_IF_MMGSD_GW_PROV_SEC_SESSION;
  }
#endif
#ifdef FEATURE_TRIPLE_SIM
  if(dms_cl_sp->subscription_id == DMSI_BIND_SUBS_TERTIARY)
  {
    session_type = MMGSDI_GW_PROV_TER_SESSION;
    session_index = QMI_IF_MMGSD_GW_PROV_TER_SESSION;
  }
#endif
    
  errval = (qmi_error_type_v01)qmi_dms_get_uim_access_status_ext(session_type);
  if (errval != QMI_ERR_NONE_V01)
  {
    if(dms_cl_sp->subscription_id == DMSI_BIND_SUBS_PRIMARY)
    { 
      session_type = MMGSDI_1X_PROV_PRI_SESSION;
      session_index = QMI_IF_MMGSD_1X_PROV_PRI_SESSION;
      if ((errval = (qmi_error_type_v01)qmi_dms_get_uim_access_status_ext(session_type)) 
                   != QMI_ERR_NONE_V01)
      {
        DS_MSG0(MSG_LEGACY_ERROR,"Primary Subscription's SIM is not initialized");
        goto send_result;
      }
    }
    else
    {
      goto send_result;
    }
   }
    
 /* Read MDN and MIN if this is 1x primary session */
 if(session_type == MMGSDI_1X_PROV_PRI_SESSION)
 { 

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
    memset(mob_num, 0, sizeof(mob_num));
    memset(min_num, 0, sizeof(min_num));
  
    errval = qmi_dmsi_get_mdn_min(mob_num, sizeof(mob_num),
                                  min_num, sizeof(min_num));
    /* add the MIN anytime it is present */
    if (errval == QMI_ERR_NONE_V01)
    {
      if (strlen(min_num))
      {
        resp_msg.mobile_id_number_valid = TRUE;
       	strlcpy((char*)(resp_msg.mobile_id_number),min_num,sizeof(resp_msg.mobile_id_number));
      }
      /* qmi_dmsi_get_mdn_min() above checks for NOT_PROVISIONED case and sets
          errval correctly, no reason to check it again*/
      strlcpy((char*)(resp_msg.voice_number),mob_num,sizeof(resp_msg.voice_number));
    }  
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */ 
  }
  else
  {
    sim_filename.access_method = MMGSDI_EF_ENUM_ACCESS;
    sim_filename.file.file_enum = MMGSDI_NO_FILE_ENUM;
    
  /* GET IMSI, Request goes to MMGSDI, then MSISDN is read */
    if (!qmi_dmsi_global.nv_restrict_imsi_access)
    {    
      //if (qmi_if_info_ptr->session_info[session_index].app_type == MMGSDI_APP_SIM)
      if(ds_mmgsdi_ptr->app_type == MMGSDI_APP_SIM)
      {
        sim_filename.file.file_enum = MMGSDI_GSM_IMSI;
      }
      //else if (qmi_if_info_ptr->session_info[session_index].app_type == MMGSDI_APP_USIM)
	  else if(ds_mmgsdi_ptr->app_type == MMGSDI_APP_USIM)
      {
        sim_filename.file.file_enum = MMGSDI_USIM_IMSI;
      }
      else
      {
        DS_MSG0(MSG_LEGACY_ERROR,"MMGSDI apps type is not supported");
      }
  
      if(sim_filename.file.file_enum != MMGSDI_NO_FILE_ENUM)
      {
        DS_SYSTEM_HEAP_MEM_ALLOC(async_cmd_buf,
                                 sizeof(ds_qmi_dms_async_cmd_type),
                                 ds_qmi_dms_async_cmd_type*);
        if( async_cmd_buf == NULL )
        {
          errval = QMI_ERR_NO_MEMORY_V01;
          goto send_result;
        }
        /* Fill Async command buffer with necessary info to send the response 
           after processing in SIM Files in call back*/
        memset(async_cmd_buf,0,sizeof(ds_qmi_dms_async_cmd_type));
        async_cmd_buf->req_handle = req_handle;
        async_cmd_buf->msg_id = msg_id;
        async_cmd_buf->cl_sp = clnt_info;

        //ds_qmi_cflog_amss_call("mmgsdi_session_read_transparent()");
        mmgsdi_status = mmgsdi_session_read_transparent(
                                          //qmi_if_info_ptr->session_info[session_index].session_id,
                                          ds_mmgsdi_ptr->session_id,
                                          sim_filename,
                                          0,
                                          DMSI_MMGSDI_IMSI_LEN,
                                          qmi_if_mmgsdi_resp_cback,
                                          (uint32) async_cmd_buf);
  
        if(mmgsdi_status == MMGSDI_SUCCESS)
        {
          /* IMSI will be reported back later from MMGSDI, other items will be 
                    processed at that time */
          return QMI_CSI_CB_NO_ERR;
        }
        else
        { 
          /* For any reason IMSI cannot be obtained from MMGSDI, it will be
          skipped and the functon will go forward to process other items */
          DS_MSG1(MSG_LEGACY_MED,"MMGSDI read IMSI status %d", mmgsdi_status);
          DS_SYSTEM_HEAP_MEM_FREE(async_cmd_buf);
        }
      }
      else
      {
        DS_MSG0(MSG_LEGACY_ERROR,"MMGSDI filename not found");
      }
    }
    
    /*going ahead and reading msisdn */
    //if (qmi_if_info_ptr->session_info[session_index].app_type == MMGSDI_APP_SIM)
    if(ds_mmgsdi_ptr->app_type == MMGSDI_APP_SIM)
    {
      sim_filename.file.file_enum = MMGSDI_TELECOM_MSISDN;
    }
    //else if (qmi_if_info_ptr->session_info[session_index].app_type == MMGSDI_APP_USIM)
    else if(ds_mmgsdi_ptr->app_type == MMGSDI_APP_USIM)
    {
      sim_filename.file.file_enum = MMGSDI_USIM_MSISDN;
    }
    else
    {
      errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
      goto send_result;
    }
    DS_SYSTEM_HEAP_MEM_ALLOC(async_cmd_buf,
                             sizeof(ds_qmi_dms_async_cmd_type),
                             ds_qmi_dms_async_cmd_type*);
    if( async_cmd_buf == NULL )
    {
      errval = QMI_ERR_NO_MEMORY_V01;
      goto send_result;
    }
    async_cmd_buf->req_handle = req_handle;
    async_cmd_buf->msg_id = msg_id;
    async_cmd_buf->cl_sp = clnt_info;

    //ds_qmi_cflog_amss_call("mmgsdi_session_read_record()");
    /* Read entire record for EF-MSISDN (x+14), len param = 0 */
    mmgsdi_status = mmgsdi_session_read_record(
                                    //qmi_if_info_ptr->session_info[session_index].session_id,
                                     ds_mmgsdi_ptr->session_id, 
                                     sim_filename,
                                     1,
                                     0,
                                     qmi_if_mmgsdi_resp_cback,
                                     (uint32) async_cmd_buf);

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* MSISDN will be reported back later from MMGSDI, other items will be
    processed at that time */
    return QMI_CSI_CB_NO_ERR;
  }
  else
  {
    /* For any reason MSISDN cannot be obtained from MMGSDI it will be
    skipped and the functon will go forward to process other items */
    DS_MSG1(MSG_LEGACY_ERROR,"MMGSDI read MSISDN status %d", mmgsdi_status);
    DS_SYSTEM_HEAP_MEM_FREE(async_cmd_buf);
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
#endif /* FEATURE_MMGSDI_SESSION_LIB */
}

#ifdef FEATURE_MMGSDI_SESSION_LIB
send_result:
#endif /* FEATURE_MMGSDI_SESSION_LIB */
  result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01 
                                   : QMI_RESULT_FAILURE_V01);

  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_get_msisdn_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;


} /* qmi_dmsi_get_msisdn() */

/*===========================================================================
  FUNCTION QMI_DMSI_GET_DEVICE_SERIAL_NUMBERS()

  DESCRIPTION
    Gives the serial numbers of the device in use
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_get_device_serial_numbers
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  qmi_error_type_v01 errval = QMI_ERR_NONE_V01;
  qmi_result_type_v01 result = QMI_RESULT_SUCCESS_V01;
  dms_get_device_serial_numbers_resp_msg_v01 resp_msg;
  memset(&resp_msg,0,sizeof(dms_get_device_serial_numbers_resp_msg_v01));
  /*FALSE to indicate this is not for encrypted message*/
  errval = qmi_dmsi_get_device_serial_numbers_ext(FALSE,&resp_msg,clnt_info);
  if(errval != QMI_ERR_NONE_V01)
  {
    result = QMI_RESULT_FAILURE_V01;
  }
  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_get_device_serial_numbers_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
} /* qmi_dmsi_get_device_serial_numbers() */



/*===========================================================================
  FUNCTION QMI_DMSI_GET_DEVICE_SERIAL_NUMBERS_EXT()

  DESCRIPTION
    common utility for device serial and encrypted device serial numbers.
    
  PARAMETERS
  is_encrypted - to identify the message
  resp_msg     - response message
  clnt_info    - client information

  RETURN VALUE
    qmi_error_type_v01
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

static qmi_error_type_v01 qmi_dmsi_get_device_serial_numbers_ext
(
  boolean                   is_encrypted,
  void*                     resp_msg,
  qmi_dmsi_client_info_type *clnt_info
  
)
{
  /*Encrypted data would be ~100 bytes more*/
  #define DMSI_ENCRYPTED_SERIAL_NO_SIZ DMSI_SERIAL_NO_SIZ+100
  uint8              dev_serial_num_info[DMSI_SERIAL_NO_SIZ];
  nv_stat_enum_type  nv_status;
  int                i;
  boolean            resp_flag = FALSE;
  dms_get_device_serial_numbers_resp_msg_v01 *dev_ser_msg;
  // used for 3GPP IMEI
  uint8              imei_ascii[(NV_UE_IMEI_SIZE-1)*2];
  uint8              imei_bcd_len = 0;
  uint8              digit = '\0';

  qmi_error_type_v01   errval = QMI_ERR_NONE_V01;
  nv_item_type      *ds_nv_item_ptr;
  uint8              dev_serial_num_encrypted_info[DMSI_ENCRYPTED_SERIAL_NO_SIZ];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   nv_status = NV_DONE_S;
    dev_ser_msg = resp_msg;
  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(ds_nv_item_ptr, sizeof(nv_item_type),
                           nv_item_type*);
  if( ds_nv_item_ptr == NULL )
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }  

  memset(dev_serial_num_info, 0, sizeof(dev_serial_num_info));
  memset(dev_serial_num_encrypted_info, 0, sizeof(dev_serial_num_encrypted_info));
  /*nv_status = ds_get_nv_item_ext( NV_UE_IMEI_I, ds_nv_item_ptr,
  	        (uint16)((qmi_dmsi_client_info_type *)clnt_info)->subscription_id );*/
  nv_status = ds_get_nv_item( NV_UE_IMEI_I, ds_nv_item_ptr);//Only single sim is there in chiltepin so do not need ext functio
  	        
  if (nv_status == NV_DONE_S)
  {
    imei_bcd_len = ds_nv_item_ptr->ue_imei.ue_imei[0];
    if( imei_bcd_len <= (NV_UE_IMEI_SIZE-1) )
    {
      memset(imei_ascii, 0, (NV_UE_IMEI_SIZE-1)*2);
      for( i = 1; i <= imei_bcd_len; i++ )
      {
        digit = QMI_BCD_LOW_DIGIT(ds_nv_item_ptr->ue_imei.ue_imei[i]);
        if( (digit <= 9) || (i <= 1) )
        {
          imei_ascii[(i-1)*2] = digit + '0';
        }
        else
        {
          imei_ascii[(i-1)*2] = '\0';
          break;
        }
        
        digit = QMI_BCD_HIGH_DIGIT(ds_nv_item_ptr->ue_imei.ue_imei[i]);
        if( (digit <= 9) || (i <= 1) )
        {
          imei_ascii[((i-1)*2)+1] = digit + '0';
        }
        else
        {
          imei_ascii[((i-1)*2)+1] = '\0';
          break;
        }
      }
      
      /* Skip the first byte because it is just the ID */
      memscpy( (uint8*)dev_serial_num_info, 
               (NV_UE_IMEI_SIZE-1)*2-1,
               imei_ascii + 1, 
               (NV_UE_IMEI_SIZE-1)*2-1 );
      
      resp_flag = TRUE;
        dev_ser_msg->imei_valid= TRUE;
        strlcpy((char*)&dev_ser_msg->imei,(char*)&dev_serial_num_info,sizeof(dev_ser_msg->imei));
      }
    else
    {
      DS_MSG0(MSG_LEGACY_ERROR, "Invalid IMEI value from NV" );
    }
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR, "No IMEI value from NV" );
  }
  
#if defined(FEATURE_CDMA) || defined(FEATURE_DATA_IS707)

  /*-------------------------------------------------------------------------
    Report ESN if CDMA is provisioned.
    dsatutil_itoa returns a pointer to the null-terminator in the output
    string.
  -------------------------------------------------------------------------*/
  memset(dev_serial_num_info, 0, sizeof(dev_serial_num_info));
  memset(dev_serial_num_encrypted_info, 0, sizeof(dev_serial_num_encrypted_info));
  
  /*---------------------------------------------------------------------------------
    Here NV_ESN_ME_I is being used instead of NV_ESN_I because value of NV_ESN_I 
    is getting changed based on dfferent UIM cards or when the card is not inserted
    while value of NV_ESN_ME_I remains consistent.
  ----------------------------------------------------------------------------------*/
  /*nv_status = ds_get_nv_item_ext (NV_ESN_ME_I, ds_nv_item_ptr,
               (uint16)((qmi_dmsi_client_info_type *)clnt_info)->subscription_id);*/ //Only single sim is supported in chiltepin
    nv_status = ds_get_nv_item(NV_ESN_ME_I, ds_nv_item_ptr);
  
  if(NV_DONE_S == nv_status)
  {
#ifdef FEATURE_DATA_ATCOP  
    (void)dsatutil_itoa(ds_nv_item_ptr->esn.esn,
                        dev_serial_num_info,
                        DMSI_BASE_HEX);      
#endif /* FEATURE_DATA_ATCOP */
    if (strlen((const char *)dev_serial_num_info)||strlen((const char *)dev_serial_num_encrypted_info))
    {
      resp_flag = TRUE;
        dev_ser_msg->esn_valid = TRUE;
        strlcpy((char*)&dev_ser_msg->esn,(char*)&dev_serial_num_info,sizeof(dev_ser_msg->esn));
      }
    }
  else
  {
    DS_MSG0(MSG_LEGACY_MED,"ESN not read from NV");
  }
#endif /* defined(FEATURE_CDMA) || defined(FEATURE_DATA_IS707) */

  #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA)
  /*-------------------------------------------------------------------------
    Report IMEISV SVN if supported
  -------------------------------------------------------------------------*/
  memset((void *)&ds_nv_item, 0, sizeof(ds_nv_item));
  memset(dev_serial_num_info, 0, sizeof(dev_serial_num_info));
  memset(dev_serial_num_encrypted_info, 0, sizeof(dev_serial_num_encrypted_info));
/*  nv_status = ds_get_nv_item_ext ( NV_UE_IMEISV_SVN_I , &ds_nv_item,
  	        (uint16)((qmi_dmsi_client_info_type *)clnt_info)->subscription_id);*/ //Single sim is supported 
  nv_status = ds_get_nv_item( NV_UE_IMEISV_SVN_I , &ds_nv_item);  
  if( NV_DONE_S == nv_status )
  {
#ifdef FEATURE_DATA_ATCOP
    (void)dsatutil_itoa(ds_nv_item.ue_imeisv_svn,
                        dev_serial_num_info,
                        DMSI_BASE_HEX);
#endif /* FEATURE_DATA_ATCOP */     
     resp_flag = TRUE;
      dev_ser_msg->imeisv_svn_valid = TRUE;
      strlcpy((char*)&dev_ser_msg->imeisv_svn,(char*)&dev_serial_num_info,sizeof(dev_ser_msg->imeisv_svn));
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Problem reading IMEISV SVN from NV");
  }
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA) */
  if(resp_flag == FALSE && errval == QMI_ERR_NONE_V01)
  {
    errval = QMI_ERR_NOT_PROVISIONED_V01;
  }
  /*-------------------------------------------------------------------------
    Free the temporary memory allocated for NV item.
  -------------------------------------------------------------------------*/
send_result:
  DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);
  return errval;

} /* qmi_dmsi_get_device_serial_numbers_ext() */



/*===========================================================================
  FUNCTION QMI_DMSI_SET_EVENT_REPORT()

  DESCRIPTION
    To set state for event reporting 
        
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_set_event_report
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  qmi_dmsi_client_info_type *  dms_client_sp;
  ds_command_type     *  cmd_ptr;

   dms_set_event_report_resp_msg_v01 resp_msg;
  dms_set_event_report_req_msg_v01* req_msg;
  qmi_error_type_v01   errval = QMI_ERR_NONE_V01;
  qmi_result_type_v01  result;
  uint8    curr_bat_lvl;
  qmi_dmsi_cmd_buf_type *  cmd_payload;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(req_c_struct && clnt_info);

  memset(&resp_msg,0,sizeof(dms_set_event_report_resp_msg_v01));
  curr_bat_lvl = 0;
  req_msg = (dms_set_event_report_req_msg_v01*)req_c_struct;
  dms_client_sp = (qmi_dmsi_client_info_type *) clnt_info;

  /*-------------------------------------------------------------------------
    If ALL optional TLVs are absent return error or else carry on and
    process the request.
  -------------------------------------------------------------------------*/
  if(!(req_msg->report_power_state_valid ||
       req_msg->lvl_limits_valid ||
       req_msg->report_pin_state_valid || 
       req_msg->report_activation_state_valid ||
       req_msg->report_oprt_mode_state_valid ||
       req_msg->report_uim_state_valid ||
       req_msg->report_wireless_disable_state_valid ||
       req_msg->report_prl_init_valid ||
       req_msg->report_cdma_lock_mode_valid ||
       req_msg->report_device_multisim_info_valid ))
  {
      errval = QMI_ERR_MISSING_ARG_V01;
      goto send_result;
  }

  /*-------------------------------------------------------------------------
    Perform 'supported' error checking on all provided TLV's before processing
  -------------------------------------------------------------------------*/
#ifndef FEATURE_MMGSDI_SESSION_LIB
  if (req_msg->report_pin_state_valid  || req_msg->report_uim_state_valid)
  {
    errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    goto send_result;
  }
#endif /* FEATURE_MMGSDI_SESSION_LIB */

#ifndef FEATURE_OTASP
  if (req_msg->report_activation_state_valid)
  {
    errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    goto send_result;
  }
#endif /* FEATURE_OTASP */

  if (req_msg->report_wireless_disable_state_valid)
  {
    errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    goto send_result;
  }
  
  /*-------------------------------------------------------------------------
    Update setting for reporting the operating mode if TLV present
  -------------------------------------------------------------------------*/
  if (req_msg->report_oprt_mode_state_valid)
  {
    dms_client_sp->report_status.report_operating_mode =
      (0 < req_msg->report_oprt_mode_state)? TRUE: FALSE;
  }
  
  if (req_msg->report_device_multisim_info_valid)
  {
    dms_client_sp->report_status.report_device_mode =
      (0 < req_msg->report_device_multisim_info)? TRUE: FALSE;
    if(dms_client_sp->report_status.report_device_mode == TRUE)
    {
       /*Send the current info related to client*/
        /*-----------------------------------------------------------------------
           Get a DS command buffer
       -----------------------------------------------------------------------*/
       cmd_ptr = ds_allocate_cmd_buf(sizeof(qmi_dmsi_cmd_buf_type));
       if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
       {
         errval = QMI_ERR_NO_MEMORY_V01;
         goto send_result;
       }
       cmd_ptr->hdr.cmd_id = DS_QMI_DMS_SVC_CMD;
       cmd_payload = (qmi_dmsi_cmd_buf_type *)cmd_ptr->cmd_payload_ptr;
	   cmd_payload->cmd_id = QMI_CMD_DMS_EVT_REPORT_REQ_IND;
       cmd_payload->data.dms_evt_info.evt_type = DMSI_REPORT_STATUS_EV_DEV_MSIM_CAP;       
       cmd_payload->data.dms_evt_info.evt_value = NULL;
       cmd_payload->data.dms_evt_info.cl_sp = dms_client_sp;
	   ds_put_cmd(cmd_ptr);

    }
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
    No Failure possible upto here so result is success
  -------------------------------------------------------------------------*/
send_result:
  result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01 
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_set_event_report_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;


} /* qmi_dmsi_set_event_report() */



/*===========================================================================
  FUNCTION QMI_DMSI_EVENT_REPORT_IND()

  DESCRIPTION
    Is called when any of the conditions set in set_event_report becomes true
    Sends an indication to the client
        
  PARAMETERS
    sp           : service provided state pointer (user data)
    clid         : clientID
    event_mask   : mask to indicate what caused the indication to be called
    sig_strength : current signal strength
    radio_if     : radio technology in use
    power_status : power status of the mobile
    battery_lvl  : battery level
    
  RETURN VALUE
    void
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dmsi_event_report_ind
(
  qmi_dmsi_client_info_type *   clnt_info,
  uint32   event_mask,
  uint8    power_status,  /* bitmask */
  uint8    battery_lvl,
  uint8    pin_index,
  uint16   activation_state,
  uint8    operating_mode,
  boolean  wd_state,
  boolean  cdma_lock_mode
)
{
  qmi_error_type_v01            errval  = QMI_ERR_NONE_V01;
  dms_event_report_ind_msg_v01* ind_msg = NULL;
  qmi_csi_error                 rc;

  ASSERT(clnt_info);
  DS_SYSTEM_HEAP_MEM_ALLOC(ind_msg, sizeof(dms_event_report_ind_msg_v01),
                           dms_event_report_ind_msg_v01*);
  if(ind_msg == NULL)
  {
    return;
  }
  memset(ind_msg,0,sizeof(dms_event_report_ind_msg_v01));
  if(event_mask & DMSI_REPORT_STATUS_EV_DEV_MSIM_CAP) 
  {
    do 
    {
     errval = qmi_dmsi_get_device_config_list(&(ind_msg->device_config_list));
     if(errval != QMI_ERR_NONE_V01)
     {
       break;
     }
     else
     {
       ind_msg->device_config_list_valid = TRUE;
     }

     errval = qmi_dmsi_get_multisim_info(&(ind_msg->multisim_capability));
     if(errval != QMI_ERR_NONE_V01)
     {
        break;
     }
     else
     {
       ind_msg->multisim_capability_valid = TRUE;
     }

     /*Do call msim info thing to add 3 tlvs and mention it is ind or response*/
     /*Sending TRUE as second arg for indication*/
     errval =  qmi_dmsi_get_device_multisim_current_cap_info((void*)ind_msg,TRUE);
     if(errval != QMI_ERR_NONE_V01)
     {
        break;
     }
     else
     {
       ind_msg->current_multisim_capability_valid = TRUE;
       ind_msg->current_subscription_capability_valid = TRUE;
       ind_msg->subs_voice_data_capability_valid = TRUE;
     }
	 
#ifdef FEATURE_DATA_QMI_DMS_MULTISIM
      /*get the max_active_data subs for ind or response*/
      errval =  qmi_dmsi_get_max_active_data_subs(&(ind_msg->max_active_data_subscriptions));
      if(errval != QMI_ERR_NONE_V01)
      {
        break;
      }
      else
      {
        ind_msg->max_active_data_subscriptions_valid = TRUE;
      }
#endif /* FEATURE_DATA_QMI_DMS_MULTISIM */
	
    }while(0);
    if(errval != QMI_ERR_NONE_V01)
    {
      DS_MSG1(MSG_LEGACY_HIGH,"Unable to generate Event Report indication! %d",errval);
      DS_SYSTEM_HEAP_MEM_FREE(ind_msg);
      return;
    }
  }

  /*-------------------------------------------------------------------------
    ind was called for power state and battery lvl
  -------------------------------------------------------------------------*/
  if(event_mask & DMSI_REPORT_STATUS_EV_POWER_STATE) 
  {
    ind_msg->power_state_valid = TRUE;
    ind_msg->power_state.power_status = power_status;
    ind_msg->power_state.battery_lvl = battery_lvl;
  }

  if(event_mask & DMSI_REPORT_STATUS_EV_ACTIVATION_STATE) 
  {
    ind_msg->activation_state_valid = TRUE;
    ind_msg->activation_state = activation_state;
  }

  if (event_mask & DMSI_REPORT_STATUS_EV_OPRT_MODE)
  {
    ind_msg->operating_mode_valid = TRUE;
    ind_msg->operating_mode = operating_mode;
  }

  if (event_mask & DMSI_REPORT_STATUS_EV_WD_STATE)
  {
    ind_msg->wireless_disable_state_valid = TRUE;
    ind_msg->wireless_disable_state = (wd_state == TRUE) ? 1 : 0;
  }
#ifdef FEATURE_DATA_QMI_DMS_PRL
  if (event_mask & DMSI_REPORT_STATUS_EV_PRL_INIT)
  {
    ind_msg->prl_init_valid = TRUE;
    ind_msg->prl_init = 1;
  }
#endif /* FEATURE_DATA_QMI_DMS_PRL */

#ifdef FEATURE_DATA_QMI_DMS_CDMA
  if (event_mask & DMSI_REPORT_STATUS_EV_CDMA_LOCK_MODE)
  {
    ind_msg->cdma_lock_mode_state_valid = TRUE;
    ind_msg->cdma_lock_mode_state = (cdma_lock_mode == TRUE) ? 1 : 0;
  }
#endif /* FEATURE_DATA_QMI_DMS_CDMA */

  rc = qmi_csi_send_ind( clnt_info->clnt,
                    QMI_DMS_EVENT_REPORT_IND_V01,
                    (void*)ind_msg, 
                    sizeof(dms_event_report_ind_msg_v01));
  if ( rc != QMI_CSI_NO_ERR )
  {
     DS_MSG1(MSG_LEGACY_ERROR,"qmi_dmsi_event_report_ind error %d", rc);
  }
  DS_SYSTEM_HEAP_MEM_FREE(ind_msg);
} /* qmi_dmsi_event_report_ind() */


/*===========================================================================
  FUNCTION QMI_DMSI_GET_DEVICE_HARDWARE_REV()

  DESCRIPTION
    Gives the device hardware revision
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_get_device_hardware_rev
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  uint32               ver;
  dms_get_device_hardware_rev_resp_msg_v01 resp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&resp_msg,0,sizeof(dms_get_device_hardware_rev_resp_msg_v01));
  /*-------------------------------------------------------------------------
    Populate the HW revision
  -------------------------------------------------------------------------*/
  DS_MSG0(MSG_LEGACY_MED,"dms query device hardware rev");

  ver = DalChipInfo_ChipVersion();
#ifdef FEATURE_DATA_ATCOP
 (void) dsatutil_itoa(ver,(uint8*)&(resp_msg.hardware_rev), DMSI_BASE_HEX);
#endif /* FEATURE_DATA_ATCOP */
  resp_msg.hardware_rev[QMI_DMS_HARDWARE_REV_MAX_V01-1] = '\0';
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_get_device_hardware_rev_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;

  
} /* qmi_dmsi_get_device_hardware_rev() */



/*===========================================================================
  FUNCTION QMI_DMSI_SET_OPERATING_MODE()

  DESCRIPTION
    Sets the operating mode of the device
        
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_set_operating_mode
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  qmi_error_type_v01   errval = QMI_ERR_NONE_V01;
  qmi_result_type_v01  result;
  nv_stat_enum_type  nv_status;
  qmi_dmsi_client_info_type *client_sp;
  dms_set_operating_mode_req_msg_v01* req_msg = NULL;
  dms_set_operating_mode_resp_msg_v01 resp_msg;
  ds_qmi_dms_async_cmd_type *     cmd_buf_p = NULL;
  sys_oprt_mode_e_type            new_oprt_mode;
  dms_operating_mode_enum_v01 curr_oprt_mode;
  boolean                   got_curr_oprt_mode = FALSE;
  qmi_if_info_type         *qmi_if_info_ptr;
  nv_item_type               *ds_nv_item_ptr = NULL;
 ds_command_type            *cmd_ptr;
  qmi_dmsi_cmd_buf_type    * cmd_payload = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(clnt_info && req_c_struct);

  client_sp = (qmi_dmsi_client_info_type *)clnt_info;
  req_msg   = (dms_set_operating_mode_req_msg_v01*)req_c_struct;
  memset(&resp_msg,0,sizeof(dms_set_operating_mode_resp_msg_v01));

  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(ds_nv_item_ptr, sizeof(nv_item_type),
                           nv_item_type*);
  if( ds_nv_item_ptr == NULL )
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Populate the local structure with the values coming in the request and
    also validate the values
  -------------------------------------------------------------------------*/
   DS_MSG0(MSG_LEGACY_HIGH,"dms set operating mode");

  new_oprt_mode = SYS_OPRT_MODE_NONE;

  switch(req_msg->operating_mode)
  {
    case DMS_OP_MODE_SHUTTING_DOWN_V01:
      new_oprt_mode = SYS_OPRT_MODE_PWROFF;
      break;

    case DMS_OP_MODE_FACTORY_TEST_MODE_V01 :
      new_oprt_mode = SYS_OPRT_MODE_FTM;
      break;

    case DMS_OP_MODE_OFFLINE_V01 :
      new_oprt_mode = SYS_OPRT_MODE_OFFLINE;
      break;

    case DMS_OP_MODE_ONLINE_V01 :
      new_oprt_mode = SYS_OPRT_MODE_ONLINE;
      TTLBOOTUP(QMI_DMS_ONLINE);

      break;

    case DMS_OP_MODE_LOW_POWER_V01 :
    case DMS_OP_MODE_PERSISTENT_LOW_POWER_V01 :
    case DMS_OP_MODE_MODE_ONLY_LOW_POWER_V01 :
      new_oprt_mode = SYS_OPRT_MODE_LPM;
      break;

    case DMS_OP_MODE_RESETTING_V01 :
      new_oprt_mode = SYS_OPRT_MODE_RESET ;
      break;
   #ifdef FEATURE_FMD_SPEED_INFO 
    case DMS_OP_MODE_CAMP_ONLY_V01 :
	  new_oprt_mode = SYS_OPRT_MODE_PSEUDO_ONLINE;
	  break;
   #endif
	
    default:
      /* Any other value is considered invalid */
      errval = QMI_ERR_INVALID_ARG_V01;
      goto send_result;
      break;
  }
  /*If new mode is of LPM/RESET/SHUTDOWN check if operation is allowed with CM*/
   if( ((new_oprt_mode == SYS_OPRT_MODE_LPM) ||
        (new_oprt_mode == SYS_OPRT_MODE_RESET) ||
        (new_oprt_mode == SYS_OPRT_MODE_PWROFF)) &&
         !cm_rpm_check_reset_allowed() )
   {
     errval = QMI_ERR_INTERNAL_V01;
     DS_MSG1(MSG_LEGACY_ERROR,"CM denied LPM/Reset/Shutdown cmd %d",new_oprt_mode);
     goto send_result;
   }

  qmi_if_info_ptr = qmi_if_info_get_ptr();
  /* Check wireless disable switch and prevent mode change to ONLINE or FTM */
  if ( qmi_if_info_ptr->ph_info.wd_switch_on )
  {
    if ((DMS_OP_MODE_ONLINE_V01 == req_msg->operating_mode) ||
        (DMS_OP_MODE_FACTORY_TEST_MODE_V01    == req_msg->operating_mode))
    {
      errval = QMI_ERR_HARDWARE_RESTRICTED_V01;
       DS_MSG1(MSG_LEGACY_HIGH,"Could not change mode to %d with wireless disable switch on",
                      req_msg->operating_mode); 
      goto send_result;
    }
  }

  /*-------------------------------------------------------------------------
                  Update NV LPM flag if mode changes to LPM or Persistent LPM 
   -------------------------------------------------------------------------*/
  if ((DMS_OP_MODE_PERSISTENT_LOW_POWER_V01 == req_msg->operating_mode) ||
      (DMS_OP_MODE_LOW_POWER_V01 == req_msg->operating_mode))
  {
    /*Get current operating mode*/
    if( QMI_ERR_NONE_V01 == qmi_dmsi_oprt_mode_from_cm( qmi_if_info_ptr->ph_info.oprt_mode, 
                                                    &curr_oprt_mode ) )
    {
      got_curr_oprt_mode = TRUE;
    }
    else
    {
       DS_MSG1(MSG_LEGACY_HIGH,"get current oprt mode failed sys oprt mode = %d\n",
                      qmi_if_info_ptr->ph_info.oprt_mode);
      got_curr_oprt_mode = FALSE;
    }

    /* Read the nv for LPM persistence flag */
    nv_status = ds_get_nv_item( NV_LPM_PERSISTENCE_I, ds_nv_item_ptr );
    switch (nv_status)
    {
      case NV_DONE_S:
        /* Clear nv for LPM or set it for P_LPM only if the nv doesn't match
           the mode request*/
        if (ds_nv_item_ptr->lpm_persistence != 
              (DMS_OP_MODE_PERSISTENT_LOW_POWER_V01 == req_msg->operating_mode))
        {
          ds_nv_item_ptr->lpm_persistence = 
                 (DMS_OP_MODE_PERSISTENT_LOW_POWER_V01 == req_msg->operating_mode);
          if (NV_DONE_S != ds_put_nv_item(NV_LPM_PERSISTENCE_I, ds_nv_item_ptr))
          {
            errval = QMI_ERR_INTERNAL_V01;
             DS_MSG1(MSG_LEGACY_ERROR,"Could not set NV_LPM_PERSISTENCE_I to %d",
                            ds_nv_item_ptr->lpm_persistence);
            goto send_result;
          }
        }
        break;

      case NV_NOTACTIVE_S:
          ds_nv_item_ptr->lpm_persistence = 
              (DMS_OP_MODE_PERSISTENT_LOW_POWER_V01 == req_msg->operating_mode);
          if (NV_DONE_S != ds_put_nv_item(NV_LPM_PERSISTENCE_I, ds_nv_item_ptr))
          {
            errval = QMI_ERR_INTERNAL_V01;
             DS_MSG1(MSG_LEGACY_ERROR,"Could not set NV_LPM_PERSISTENCE_I to %d",
                            ds_nv_item_ptr->lpm_persistence);
            goto send_result;
          }
          break;

      default:
        errval = QMI_ERR_INTERNAL_V01;
        goto send_result;
        /* no need for break after goto */
    }

    /* if current operating mode is known, check for LPM<->P-LPM changes */
    if( got_curr_oprt_mode )
    {
      if  ( (DMS_OP_MODE_LOW_POWER_V01 == req_msg->operating_mode  && 
             DMS_OP_MODE_PERSISTENT_LOW_POWER_V01 == curr_oprt_mode )|| 
            (DMS_OP_MODE_PERSISTENT_LOW_POWER_V01 == req_msg->operating_mode &&
             DMS_OP_MODE_LOW_POWER_V01 == curr_oprt_mode) )
      { 
        /* if a change has happened, send an IND */
        qmi_dms_generate_oprt_mode_ind(SYS_OPRT_MODE_LPM);
      }
    }
  }

  /* For shutdown/Reset call sys monitor and send immediate response */
  if ( (DMS_OP_MODE_RESETTING_V01 == req_msg->operating_mode) || 
       (DMS_OP_MODE_SHUTTING_DOWN_V01 == req_msg->operating_mode) )
  {
    if(!cm_rpm_check_reset_allowed())
    {
      errval = QMI_ERR_INTERNAL_V01;
      DS_MSG0(MSG_LEGACY_ERROR,"CM denied Reset/Shutdown");
      goto send_result;
    }

    //ds_qmi_cflog_cm_ph_cmd_oprt_mode( new_oprt_mode );
     DS_MSG1(MSG_LEGACY_HIGH,"Triggered DMS OPRT MODE %d by sys mon APIs",req_msg->operating_mode);

    if(DMS_OP_MODE_RESETTING_V01 == req_msg->operating_mode)
    {
      sys_m_initiate_shutdown();
    }
    else
    {
      sys_m_initiate_poweroff();
    }

     /*-----------------------------------------------------------------------
         Get a DS command buffer
     -----------------------------------------------------------------------*/
       cmd_ptr = ds_allocate_cmd_buf(sizeof(qmi_dmsi_cmd_buf_type));
       if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
       {
          DS_MSG0(MSG_LEGACY_ERROR,"Out of cmd buf");          
          errval = QMI_ERR_NO_MEMORY_V01;          
          goto send_result;
       }
       cmd_ptr->hdr.cmd_id = DS_QMI_DMS_SVC_CMD;
       cmd_payload = (qmi_dmsi_cmd_buf_type *)cmd_ptr->cmd_payload_ptr;
	   cmd_payload->cmd_id = QMI_CMD_DMS_RESET_SHUTDOWN;
       cmd_payload->data.cm_ph.info.oprt_mode = new_oprt_mode;
	   ds_put_cmd(cmd_ptr);
       errval = QMI_ERR_NONE_V01;
       goto send_result;
  }

  if (SYS_OPRT_MODE_NONE != new_oprt_mode)

  {

  //ds_qmi_cflog_cm_ph_cmd_oprt_mode( new_oprt_mode );
    /* ----------------------------------------------------------
     Allocated temp memory for resp_buf_type 
    -----------------------------------------------------------*/
    DS_SYSTEM_HEAP_MEM_ALLOC(cmd_buf_p,
                             sizeof(ds_qmi_dms_async_cmd_type),
                             ds_qmi_dms_async_cmd_type*);
    if( cmd_buf_p == NULL )
    {
      errval = QMI_ERR_NO_MEMORY_V01;
      goto send_result;
    }
    memset(cmd_buf_p,0,sizeof(ds_qmi_dms_async_cmd_type));
    cmd_buf_p->req_handle = req_handle;
    cmd_buf_p->msg_id = msg_id;
    cmd_buf_p->cl_sp = clnt_info;
    if ( !cm_ph_cmd_oprt_mode( qmi_dmsi_cm_ph_cb,
                               cmd_buf_p,
                               qmi_if_info_ptr->cm_client_id,
                               new_oprt_mode ) )
    {
      DS_MSG1(MSG_LEGACY_ERROR,"Unable to change operating mode, cm_ph_cmd_oprt_mode"
                       " fails in Set Oper Mode Cmd 0x%x", cmd_buf_p);
      errval = QMI_ERR_INTERNAL_V01;
      goto send_result;
    }
    DS_MSG1(MSG_LEGACY_HIGH,"Response pending for Set Operating Mode %d\t sent to cm",
                    new_oprt_mode);
    DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);
    return QMI_CSI_CB_NO_ERR; 
  }

send_result:
  /*-------------------------------------------------------------------------
    Free the temporary memory allocated for NV item.
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);
  if(cmd_buf_p != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(cmd_buf_p);
  }
  result = (errval == QMI_ERR_NONE_V01)?QMI_RESULT_SUCCESS_V01: 
                                        QMI_RESULT_FAILURE_V01;
  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_set_operating_mode_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;

} /* qmi_dmsi_set_operating_mode() */

/*===========================================================================
  FUNCTION QMI_DMSI_GET_OPERATING_MODE()

  DESCRIPTION
    Gives the operating mode of the device
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_get_operating_mode
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  qmi_error_type_v01   errval = QMI_ERR_NONE_V01;
  qmi_result_type_v01  result;
  dms_get_operating_mode_resp_msg_v01 resp_msg;
  dms_operating_mode_enum_v01  operating_mode;
  qmi_if_info_type  *qmi_if_info_ptr;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&resp_msg,0,sizeof(dms_get_operating_mode_resp_msg_v01));
  qmi_if_info_ptr = qmi_if_info_get_ptr();
  operating_mode = DMS_OP_MODE_ONLINE_V01;

  /*-------------------------------------------------------------------------
    Perform query action
  -------------------------------------------------------------------------*/
  DS_MSG0(MSG_LEGACY_HIGH,"dms query current operating mode");

  errval = qmi_dmsi_oprt_mode_from_cm( qmi_if_info_ptr->ph_info.oprt_mode, 
                                       &operating_mode );
  if (QMI_ERR_NONE_V01 == errval)
  {
    resp_msg.operating_mode = (dms_operating_mode_enum_v01)operating_mode;
  }

  result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01 
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_get_operating_mode_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
} /* qmi_dmsi_get_operating_mode() */


/*===========================================================================
  FUNCTION QMI_DMSI_IMAGE_INDEX_TO_TYPE()

  DESCRIPTION
    Convert image index to image type.
    
  PARAMETERS
    image_index: image index

  RETURN VALUE
    uint32: image type
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint32 qmi_dmsi_image_index_to_type
(
  uint8  image_index
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
   * Passing through of image index is not allowed so we have to
   * convert the image index to the image type defined in the QMI spec.
   * image_index is defined in core\debugtools\version\src\image_versino.h
   * This conversion is a workaround before image_index definition is exposed
   * throught core API
  -------------------------------------------------------------------------*/
  if (image_index <= 5)
  {
    return image_index + 1;
  }
  else if (image_index >= 9 && image_index <= 14)
  {
    return image_index - 2;
  }

  return 0;

} /* qmi_dmsi_image_index_to_type() */

/*===========================================================================
  FUNCTION QMI_DMSI_GET_SW_VERSION()

  DESCRIPTION
    Gives the software version of the device.
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dmsi_get_sw_version
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  char              *sw_version_info = NULL;
  char               sw_ver2[QMI_DMS_SW_VERSION_MAX_V01 + 1]; /*+1 for NULL char*/
  uint16             offset = 0;
  uint16             len = 0;
  qmi_error_type_v01   errval = QMI_ERR_NONE_V01;
  qmi_result_type_v01  result;
  nv_stat_enum_type     nv_status;
  dms_get_sw_version_resp_msg_v01 resp_msg;
  qmi_dmsi_image_ver_type * image_ver = NULL;
  uint32                    image_ver_table_size = 0;
  char                      index_str[IMAGE_INDEX_LEN + 1];
  char                      qc_ver_str[IMAGE_QC_VER_LEN + 1];
  char                      variant_str[IMAGE_VARIANT_LEN + 1];
  char                      oem_ver_str[IMAGE_OEM_VER_LEN + 1];
  char                      image_ver_str[IMAGE_VER_STR_SIZE];
  uint8                     image_index;
  uint32                    image_type;
  uint8                     count = 0;
  int                       i;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  memset(&resp_msg,0,sizeof(dms_get_sw_version_resp_msg_v01));

  /*-------------------------------------------------------------------------
    Populate image versions 
  -------------------------------------------------------------------------*/
  image_ver = (qmi_dmsi_image_ver_type *)
          smem_get_addr(SMEM_IMAGE_VERSION_TABLE, &image_ver_table_size); 
  if (image_ver != NULL)
  {
    resp_msg.image_versions_valid = TRUE;
    for (i = image_ver_table_size / sizeof(qmi_dmsi_image_ver_type) - 1; i >= 0; i--)
    {
      if (image_ver[i].colon1 == ':')
      {
        // Copy strings
        memscpy(index_str, IMAGE_INDEX_LEN, image_ver[i].index, IMAGE_INDEX_LEN);
        memscpy(qc_ver_str, IMAGE_QC_VER_LEN, image_ver[i].qc_ver, IMAGE_QC_VER_LEN);
        memscpy(variant_str, IMAGE_VARIANT_LEN, image_ver[i].variant, IMAGE_VARIANT_LEN);
        memscpy(oem_ver_str, IMAGE_OEM_VER_LEN, image_ver[i].oem_ver, IMAGE_OEM_VER_LEN);

        index_str[IMAGE_INDEX_LEN] = '\0';
        qc_ver_str[IMAGE_QC_VER_LEN] = '\0';
        variant_str[IMAGE_VARIANT_LEN] = '\0';
        oem_ver_str[IMAGE_OEM_VER_LEN] = '\0';

        image_index = (uint8)atoi(index_str);
        if (image_index != i)
        {
          DS_MSG2(MSG_LEGACY_ERROR,"Invalid image index %d %d", image_index, i);
          continue;
        }

        // Number of valid image versions
        count++;

        // Convert image index to image type
        image_type = qmi_dmsi_image_index_to_type(image_index);

        // Image version string format is "QC:Variant:OEM"
        snprintf(image_ver_str, IMAGE_VER_STR_SIZE, "%s:%s:%s",
                              qc_ver_str, variant_str, oem_ver_str);
        resp_msg.image_versions[resp_msg.image_versions_len].image_type = image_type;
        strlcpy(&(resp_msg.image_versions[resp_msg.image_versions_len].image_ver[0]),
                 image_ver_str,sizeof(resp_msg.image_versions[resp_msg.image_versions_len].image_ver));
        resp_msg.image_versions_len++;
      }
    }
  }

  /*-------------------------------------------------------------------------
    Mandatory TLV
  -------------------------------------------------------------------------*/
  memset((void *)&ds_nv_item, 0, sizeof(ds_nv_item));
  nv_status = ds_get_nv_item(NV_SW_VERSION_INFO_I, &ds_nv_item);
  if(nv_status == NV_DONE_S)
  {
    sw_version_info = (char *)ds_nv_item.sw_version_info;
  } 
  else
  {
   /*mob_sw_rev is a global extern variable*/
    sw_version_info = (char*)mob_sw_rev;
  }

  len = (uint16)strlen(sw_version_info);
  
  if((sw_version_info == NULL) || (len == 0))
  {
    errval = QMI_ERR_NOT_PROVISIONED_V01;
    goto send_result;
  }

  /*Max length of sw_version in IDL is 32 bytes only. 
    if exceeds truncate the string from start*/
  if(len > QMI_DMS_SW_VERSION_MAX_V01)
  {
   /* sw_version examples, old: "M9615ACETWTAAM5011116.1" (< 32 bytes)
       new : "MPSS.DI.2.0-01022-M8974AAAAANAZM-1" (>  32 bytes)
      truncate first N characters from start i.e., “MPSS.....” 
      output will be DI.2.0-01022-M8974AAAAANAZM-1 */
     memset(sw_ver2, 0, sizeof(sw_ver2));
     offset  = len - QMI_DMS_SW_VERSION_MAX_V01;
     (void) strlcpy (sw_ver2,
                      (char*)(sw_version_info + offset),
                      (QMI_DMS_SW_VERSION_MAX_V01 + 1) );  /*+1 for null char*/

     /*Send the truncated string in response*/
     sw_version_info = (char*)sw_ver2;

  /*-------------------------------------------------------------------------
    As Mandatory TLV was not enough lengh include optional tlv
  -------------------------------------------------------------------------*/
   resp_msg.sw_version_ext_valid = TRUE;
   strlcpy((char*)&(resp_msg.sw_version_ext),(char*)mob_sw_rev, 
            sizeof(resp_msg.sw_version_ext));

  }
  /*-------------------------------------------------------------------------
    Fill the Mandatory TLV
  -------------------------------------------------------------------------*/
  strlcpy((char*)&(resp_msg.sw_version),(char*)sw_version_info,
           sizeof(resp_msg.sw_version));
  send_result:
  result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01 
                                        : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_get_sw_version_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}/* qmi_dmsi_get_sw_version */
/*===========================================================================
  FUNCTION QMI_DMSI_SET_SPC()

  DESCRIPTION
    Updates (Set) the Service Programming Code. After validating the current 
    SPC code. 
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dmsi_set_spc
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  qmi_error_type_v01   errval = QMI_ERR_NONE_V01;
  qmi_result_type_v01  result;
  qmi_if_spc_result_e_type check_spc_result = QMI_SPC_FAIL;
  qmi_if_info_type       *qmi_if_info_ptr;
  dms_set_spc_req_msg_v01*  req_msg = NULL;
  dms_set_spc_resp_msg_v01 resp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(clnt_info && req_c_struct);
  req_msg = (dms_set_spc_req_msg_v01*)req_c_struct;
  memset(&resp_msg,0,sizeof(dms_set_spc_resp_msg_v01));
  DS_MSG0(MSG_LEGACY_MED,"Set Service Programming Code");
 
   /*-----------------------------------------------------------------------
     Check the supplied SPC is valid
   -----------------------------------------------------------------------*/
   check_spc_result = qmi_if_check_spc((uint8 *)req_msg->curr_spc, FALSE); 
   
   if(check_spc_result == QMI_SPC_FAIL)
   {
     errval = QMI_ERR_AUTHENTICATION_FAILED_V01;
     goto send_result;
   }

   if(check_spc_result == QMI_SPC_LOCKED)
   {
     errval = QMI_ERR_AUTHENTICATION_LOCK_V01;
     goto send_result;
   }

   if(check_spc_result == QMI_SPC_ERR_INTERNAL)
   {
     errval = QMI_ERR_INTERNAL_V01;
     goto send_result;
   }

   /* store the SPC lock code into NV.*/
   
    memscpy((void *)ds_nv_item.sec_code.digits, 
            NV_SEC_CODE_SIZE,
           (char *)req_msg->new_spc,
           NV_SEC_CODE_SIZE);
   
    if (NV_DONE_S != ds_put_nv_item(NV_SEC_CODE_I, &ds_nv_item))
    {
      errval = QMI_ERR_INTERNAL_V01;
      DS_MSG0(MSG_LEGACY_ERROR,"Could not Update new SPC");
      goto send_result;
    }

    /* Update cached value in QMI IF*/
    qmi_if_info_ptr = qmi_if_info_get_ptr();
    qmi_if_info_ptr->nv_sec_code = ds_nv_item.sec_code;
    /*-------------------------------------------------------------------------
    Build the response from back to front
    No Failure possible upto here so result is success
  -------------------------------------------------------------------------*/
send_result:
  result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01 
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_set_spc_resp_msg_v01));  
  return QMI_CSI_CB_NO_ERR;
}/* qmi_dmsi_set_spc */



#ifdef FEATURE_DATA_QMI_DMS_FACTORY
/*===========================================================================
  FUNCTION QMI_DMSI_RESTORE_FACTORY_DEFAULTS()

  DESCRIPTION
    Performs the restore factory defaults procedure
        
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_restore_factory_defaults
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  qmi_error_type_v01   errval = QMI_ERR_NONE_V01;
  qmi_result_type_v01  result;
  dms_restore_factory_defaults_req_msg_v01* req_msg;
  dms_restore_factory_defaults_resp_msg_v01 resp_msg;
  qmi_if_spc_result_e_type	check_spc_result;
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  uint8 i;
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)*/
  nv_item_type      *ds_nv_item_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT((int)req_c_struct);

  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(ds_nv_item_ptr, sizeof(nv_item_type),
                           nv_item_type*);
  if( ds_nv_item_ptr == NULL )
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }
  errval = QMI_ERR_NONE_V01;
  req_msg = (dms_restore_factory_defaults_req_msg_v01*)req_c_struct; 
  memset(&resp_msg, 0, sizeof(dms_restore_factory_defaults_resp_msg_v01));
  
  if(!req_c_struct_len)
  {
    errval = QMI_ERR_MISSING_ARG_V01;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Populate the local structure with the values coming in the request and
    also validate the values
  -------------------------------------------------------------------------*/
  DS_MSG0(MSG_LEGACY_MED,"dms restore factory defaults");

  /* Check SPC first , no need to validate values and give any special err for
     SPC, make it obscure */
  if ((FALSE == qmi_dmsi_global.nv_allow_invalid_spc) ||
      (0 != strncmp((char *)req_msg->spc, 
                        SPECIAL_INVALID_SPC, 
                        strlen(SPECIAL_INVALID_SPC))))
  {
    check_spc_result = qmi_if_check_spc((uint8 *)req_msg->spc, FALSE); 
  
    if(check_spc_result == QMI_SPC_FAIL)
    {
    	errval = QMI_ERR_AUTHENTICATION_FAILED_V01;
    	goto send_result;
    }
  
    if(check_spc_result == QMI_SPC_LOCKED)
    {
    	errval = QMI_ERR_AUTHENTICATION_LOCK_V01;
    	goto send_result;
    }

    if(check_spc_result == QMI_SPC_ERR_INTERNAL)
    {
      errval = QMI_ERR_INTERNAL_V01;
      goto send_result;
    }
  }

  /*-------------------------------------------------------------------------
    Initiate the factory defaults procedure

    For C2K and UMTS:
    
    For C2K, only:
    Set the IMSI Components to 0
    Set the IMSI_T Components to 0
    Set MDN = "0000000000"
    
    For UMTS, only:
    
    Reboot! - this is expected to be intiated by the PC Connection Manager !
   -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Reset items for both C2K and UMTS builds.
   -------------------------------------------------------------------------*/

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  /*-------------------------------------------------------------------------
    Reset items for C2K builds.
   -------------------------------------------------------------------------*/
  
#if defined(FEATURE_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE)
  /*-------------------------------------------------------------------------
    Changing provisioning cannot be done on a RUIM card. Do not change any
    values, only allow the UQCN processing.
  -------------------------------------------------------------------------*/
  if (NV_RTRE_CONTROL_USE_RUIM == nv_rtre_control())
  {
    DS_MSG0(MSG_LEGACY_MED,"Cannot change ADM-protected values on RUIM");
  }
  else
#endif /* defined(FEATURE_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE) */
  {
    nv_item_type *ds_nv_mdn_ptr = NULL;

    /*-------------------------------------------------------------------------
      Allocate temporary memory for the NV item
    -------------------------------------------------------------------------*/
    DS_SYSTEM_HEAP_MEM_ALLOC(ds_nv_mdn_ptr, sizeof(nv_item_type),
                           nv_item_type*);
    if( ds_nv_mdn_ptr == NULL )
    {
      errval = QMI_ERR_NO_MEMORY_V01;
      goto send_result;
    }

    /* setup the MDN NV item (fills with 0 char) once here before loop */
    memset(ds_nv_mdn_ptr, 0, sizeof(nv_item_type));
    memscpy(&ds_nv_item_ptr->dir_number.dir_number[0],
            sizeof(ds_nv_item_ptr->dir_number.dir_number),
            "0000000000", 
            sizeof(ds_nv_item_ptr->dir_number.dir_number));

    /* clear all NAM's of info */
    for( i=0; i < NV_MAX_NAMS; i++ ) 
    {
      /* Set the IMSI Components to 0 */
      if ( qmi_dmsi_reset_imsi_components(i) == FALSE )
      {
        errval = QMI_ERR_INTERNAL_V01;
        DS_MSG1(MSG_LEGACY_ERROR,"Could not reset IMSI components on NAM %d", i);
        DS_SYSTEM_HEAP_MEM_FREE(ds_nv_mdn_ptr);
        goto send_result;
      }
  
#ifdef FEATURE_IS95B_TRUE_IMSI
      /* Set the IMSI_T Components to 0 */
      if ( qmi_dmsi_reset_imsi_t_components(i) == FALSE )
      {
        errval = QMI_ERR_INTERNAL_V01;
        DS_MSG1(MSG_LEGACY_ERROR,"Could not reset IMSI_T components on NAM %d", i);
        DS_SYSTEM_HEAP_MEM_FREE(ds_nv_mdn_ptr);
        goto send_result;
      }
#endif /* FEATURE_IS95B_TRUE_IMSI */

      /* Set MDN = "0000000000" */
      ds_nv_mdn_ptr->dir_number.nam = i;
      if ( NV_DONE_S != ds_put_nv_item( NV_DIR_NUMBER_I, ds_nv_mdn_ptr ) )
      {
        errval = QMI_ERR_INTERNAL_V01;
        DS_SYSTEM_HEAP_MEM_FREE(ds_nv_mdn_ptr);
        goto send_result;
      }

      /* Set SSD-A to 0 */
      memset(ds_nv_item_ptr, 0, sizeof(nv_item_type));
      ds_nv_item_ptr->ssd_a.nam = i;
      if ( NV_DONE_S != ds_put_nv_item( NV_SSD_A_I, ds_nv_item_ptr ) )
      {
        errval = QMI_ERR_INTERNAL_V01;
        DS_SYSTEM_HEAP_MEM_FREE(ds_nv_mdn_ptr);
        goto send_result;
      }

      /* Set SSD-B to 0 */
      memset(ds_nv_item_ptr, 0, sizeof(nv_item_type));
      ds_nv_item_ptr->ssd_b.nam = i;
      if ( NV_DONE_S != ds_put_nv_item( NV_SSD_B_I, ds_nv_item_ptr ) )
      {
        errval = QMI_ERR_INTERNAL_V01;
        DS_SYSTEM_HEAP_MEM_FREE(ds_nv_mdn_ptr);
        goto send_result;
      }
    }

    /*-------------------------------------------------------------------------
      Free the temporary memory allocated for NV item.
    -------------------------------------------------------------------------*/
    DS_SYSTEM_HEAP_MEM_FREE(ds_nv_mdn_ptr);
  }

#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA)
  /*-------------------------------------------------------------------------
    Reset items for UMTS builds.
   -------------------------------------------------------------------------*/

#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA) */

send_result: 
  /*-------------------------------------------------------------------------
    Free the temporary memory allocated for NV item.
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);

  result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01 
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_restore_factory_defaults_resp_msg_v01));  
  return QMI_CSI_CB_NO_ERR;

  
} /* qmi_dmsi_restore_factory_defaults() */
#endif /* FEATURE_DATA_QMI_DMS_FACTORY */

/*===========================================================================
  FUNCTION QMI_DMSI_VALIDATE_SPC()

  DESCRIPTION
    Validates a provided security code.
        
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_validate_spc
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  qmi_error_type_v01   errval = QMI_ERR_NONE_V01;
  qmi_result_type_v01  result;
  dms_validate_service_programming_code_req_msg_v01* req_msg;
  dms_validate_service_programming_code_resp_msg_v01 resp_msg;
  qmi_if_spc_result_e_type	check_spc_result;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT((int)req_c_struct);
  errval = QMI_ERR_NONE_V01;
  memset(&resp_msg, 0, sizeof(dms_validate_service_programming_code_resp_msg_v01));
  
  /* Check the SPC */
  req_msg = (dms_validate_service_programming_code_req_msg_v01*)req_c_struct;
  check_spc_result = qmi_if_check_spc((uint8 *)req_msg->spc, TRUE); 

  if(QMI_SPC_FAIL == check_spc_result)
  {
    errval = QMI_ERR_AUTHENTICATION_FAILED_V01;
  }
  else if (QMI_SPC_LOCKED == check_spc_result)
  {
    errval = QMI_ERR_AUTHENTICATION_LOCK_V01;
  }
  else if (QMI_SPC_ERR_INTERNAL == check_spc_result)
  {
    errval = QMI_ERR_INTERNAL_V01;
  }

  result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01 
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_validate_service_programming_code_resp_msg_v01));  
  return QMI_CSI_CB_NO_ERR;
} /* qmi_dmsi_validate_spc() */

/*===========================================================================
  FUNCTION QMI_DMSI_GET_TIME()

  DESCRIPTION
    Gives the current time from the device
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_get_time
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  dms_get_time_resp_msg_v01 resp_msg;
  uint64 time_count = 0;
  time_genoff_args_struct_type genoff_args;
  time_type                    ts_val;
  time_type                    ts_val_usr;
  time_type                    ts_val_sys;
  time_source_enum_type        ts_source;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&resp_msg, 0, sizeof(dms_get_time_resp_msg_v01));
  memset(&genoff_args, 0, sizeof (genoff_args));
  memset(ts_val, 0, sizeof(ts_val));
  memset(ts_val_usr, 0, sizeof(ts_val_usr));
  memset(ts_val_sys, 0, sizeof(ts_val_sys));
  
  /*-------------------------------------------------------------------------
    Query the current user time
  -------------------------------------------------------------------------*/

  genoff_args.base        = ATS_USER; 
  genoff_args.base_source = TIME_SCLK;
  genoff_args.ts_val      = ts_val_usr;
  genoff_args.unit        = TIME_MSEC;
  genoff_args.operation   = T_GET;
  time_genoff_opr( &genoff_args );

  DS_MSG2(MSG_LEGACY_MED,"Dms query User Time: ts_val_usr[0]=%d : ts_val_usr[1]=%d",
                  ts_val_usr[0], ts_val_usr[1]);
  resp_msg.user_time_in_ms_valid = TRUE;
  memscpy(&resp_msg.user_time_in_ms,sizeof(resp_msg.user_time_in_ms),&ts_val_usr,sizeof(time_type));

  memset(&genoff_args, 0, sizeof (genoff_args));

  /*-------------------------------------------------------------------------
    Query the current sys time
  -------------------------------------------------------------------------*/
  DS_MSG0(MSG_LEGACY_MED,"dms query sys time in ms");

  genoff_args.base        = ATS_TOD; 
  genoff_args.base_source = TIME_SCLK;
  genoff_args.ts_val      = ts_val_sys;
  genoff_args.unit        = TIME_MSEC;
  genoff_args.operation   = T_GET;
  time_genoff_opr( &genoff_args );

  DS_MSG2(MSG_LEGACY_MED,"Dms query Sys Time: ts_val_sys[0]=%d : ts_val_sys[1]=%d",
                  ts_val_sys[0], ts_val_sys[1]);
  resp_msg.sys_time_in_ms_valid = TRUE;
  memscpy(&resp_msg.sys_time_in_ms,sizeof(resp_msg.sys_time_in_ms),&ts_val_sys,sizeof(ts_val_sys));
  /*-------------------------------------------------------------------------
    Query the current timestamp
  -------------------------------------------------------------------------*/
  /*
                             TIMESTAMP FORMAT

       |<------------ 48 bits --------------->|<----- 16 bits ------->|
       +--------------------------------------+-----------------------+
       |      1.25 ms counter                 |   1/32 chip counter   |
       +--------------------------------------+-----------------------+
             (11K years of dynamic range)          (1.25 ms dynamic
                                                    range. Rolls over
                                                    at count 49152)
  */
  ts_source = time_get(ts_val);

  /*-------------------------------------------------------------------------
    Populate the timestamp response
  -------------------------------------------------------------------------*/
  /* copy the timestamp to native C */
  time_count = QW_CVT_Q2N(ts_val);

  /* shift the timestamp to conform with the output alignment */
  time_count = time_count >> 16;

  memscpy(resp_msg.device_time.time_count,sizeof(resp_msg.device_time.time_count),
          &time_count,sizeof(time_count));  

  resp_msg.device_time.time_source = (dms_time_source_enum_v01)ts_source;
  
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_get_time_resp_msg_v01));  
  return QMI_CSI_CB_NO_ERR;

  
} /* qmi_dmsi_get_time() */

/*===========================================================================
  FUNCTION QMI_DMSI_SET_TIME()

  DESCRIPTION
    Sets the current time to the device

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dmsi_set_time
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  qmi_error_type_v01   errval = QMI_ERR_NONE_V01;
  qmi_result_type_v01  result;
  time_genoff_args_struct_type genoff_args;
  dms_set_time_req_msg_v01* req_msg = NULL;
  dms_set_time_resp_msg_v01 resp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT((int)req_c_struct);
  req_msg = (dms_set_time_req_msg_v01*)req_c_struct;
  memset(&resp_msg,0,sizeof(dms_set_time_resp_msg_v01));
  memset(&genoff_args,0,sizeof(genoff_args));

  /*-------------------------------------------------------------------------
    We support only User Time Reference Type (0x00000000)
  -------------------------------------------------------------------------*/
  if(req_msg->time_reference_type_valid && 
  	 req_msg->time_reference_type != DMS_TIME_REF_TYPE_USER_V01)
  {
    errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    goto send_result;
  }
  genoff_args.base        = ATS_USER; 
  genoff_args.base_source = TIME_SCLK;
  genoff_args.ts_val      = (void*)&req_msg->time_in_ms;
  genoff_args.unit        = TIME_MSEC;
  genoff_args.operation   = T_SET;
  time_genoff_opr( &genoff_args );

send_result:
  result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01 
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_set_time_resp_msg_v01)); 
  return QMI_CSI_CB_NO_ERR;


} /* qmi_dmsi_set_time() */

/*===========================================================================
  FUNCTION QMI_DMSI_RESET_CLIENT()

  DESCRIPTION
    Resets the state for the given client
    
  PARAMETERS
    cl_sp_in :  client info handle
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dmsi_reset_client
(
  qmi_dmsi_client_info_type * cl_sp_in
)
{
  qmi_dmsi_client_info_type * cl_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_dmsi_client_info_type *)cl_sp_in;
  /*-------------------------------------------------------------------------
    Free resources for client
    Remove any pending commmands for this client from the pending q's
    Clear pending msg transactions and partially bundled responses
  -------------------------------------------------------------------------*/
  /* (void) q_linear_delete_new(
           &(qmi_dms_state.set_oprt_mode_pending_q),
           (q_compare_func_type) qmi_svc_cmd_buf_belongs_to_client,
           cl_sp,
           (q_action_func_type) NULL,
           NULL);*/
           
  #ifdef FEATURE_TDSCDMA

  (void) q_linear_delete_new(&qmi_dms_state.get_tdscdma_config_pending_q,
                             (q_compare_func_type) dmsi_cmd_buf_belongs_to_client,
                             cl_sp,
                             (q_action_func_type) NULL,
                             NULL);

  (void) q_linear_delete_new(&qmi_dms_state.set_tdscdma_config_pending_q,
                             (q_compare_func_type) dmsi_cmd_buf_belongs_to_client,
                             cl_sp,
                             (q_action_func_type) NULL,
                             NULL);

  #endif
/*
  do 
  {
    // Delete all pending power stats requests of current client
    pending_cl_sp = q_linear_delete_new(&qmi_dms_state.modem_activity_pend_q,
                                       (q_compare_func_type) qmi_dms_is_client_power_stats_query_pending,
                                       cl_sp,
                                       (q_action_func_type) NULL,
                                       NULL);
  }while(pending_cl_sp != NULL);*/ //Not for chiltepin

  // Delete all pending PSM_GET_CFG_PARAMS requests for this client

  while (q_linear_delete_new(&qmi_dms_state.psm_get_cfg_params_q,
                              (q_compare_func_type)dmsi_cmd_buf_belongs_to_client,
                              cl_sp,
                              (q_action_func_type) NULL,
                              NULL) != NULL);

  /*-------------------------------------------------------------------------
    If power state reporting is set for this client then
    - reset it
    - decrement the report power state count by 1
    - If this count becomes 0, turn off the timer for power state reporting
  -------------------------------------------------------------------------*/
  /*
  if(cl_sp->report_status.report_power_state == TRUE)
  {
    cl_sp->report_status.report_power_state  = FALSE;
    if (qmi_dmsi_global.pwr_rpt.report_power_state_client_count > 0)
    {
      qmi_dmsi_global.pwr_rpt.report_power_state_client_count--;
    }

    if(qmi_dmsi_global.pwr_rpt.report_power_state_client_count == 0)
    {
      qmi_charger_set_power_state_reporting(FALSE);
    }
  }*/ //Not for chiltepin
  /*-------------------------------------------------------------------------
    clear event reporting then set non-zero default values
  -------------------------------------------------------------------------*/
//  memset(&(cl_sp->report_status), 0, sizeof(cl_sp->report_status));
  //cl_sp->report_status.bat_lvl_lower_limit = DMSI_BAT_LVL_MIN;
  //cl_sp->report_status.bat_lvl_upper_limit = DMSI_BAT_LVL_MAX;
  //cl_sp->report_status.last_bat_lvl_reported = DMSI_BAT_LVL_INACTIVE;
  //cl_sp->report_status.last_power_state_reported = DMSI_PWR_STATE_INACTIVE;
/*
  if (cl_sp_in == qmi_dmsi_global.prl_info.cl_sp)
  {
    /* end the PRL series because the client that was using it is ending *
    qmi_dmsi_reset_prl_series_info();
  }  */
} /* qmi_dmsi_reset_client() */



/*===========================================================================
  FUNCTION QMI_DMSI_GET_MAX_CHANNEL_RATE()

  DESCRIPTION
    Get the maximum theoretically supported RX and TX channel rate

  PARAMETERS
    max_rx_channel_rate (output parameter)
    max_tx_channel_rate (output parameter)

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_dmsi_get_max_channel_rate
(
  uint32 * max_rx_channel_rate,
  uint32 * max_tx_channel_rate
)
{
  qmi_channel_info_s_type supp_channel_info;
  uint16 mode_capability = 0;
  qmi_if_info_type  *qmi_if_info_ptr;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA)
  uint32  tx_rate = 0;
  uint32  rx_rate = 0;
#endif /* FEATURE_WCDMA || FEATURE_GSM || FEATURE_LTE || FEATURE_TDSCDMA */
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  uint32  tx_rate_3gpp2 = 0;
  uint32  rx_rate_3gpp2 = 0;
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */
  qmi_error_type_v01   errval = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ASSERT (max_rx_channel_rate && max_tx_channel_rate);

  *max_tx_channel_rate = 0;
  *max_rx_channel_rate = 0;

  memset(&supp_channel_info, 0, sizeof(supp_channel_info));

  qmi_if_info_ptr = qmi_if_info_get_ptr();

  mode_capability = (uint16)qmi_if_info_ptr->ph_info.mode_capability;


  /*-------------------------------------------------------------------------
    Determine max supported channel rate based on software feature support
  -------------------------------------------------------------------------*/
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA)
  if (qmi_dmsi_global.nv_use_max_defined_data_rates)
  {
    if(DMSI_TARGET_SUPPORTS_LTE(mode_capability))
    {
      supp_channel_info.supp_channel = QMI_CHANNEL_SUPP_LTE;
    }
    else if(DMSI_TARGET_SUPPORTS_WCDMA(mode_capability))
    {  
      supp_channel_info.supp_channel = QMI_CHANNEL_SUPP_WCDMA;
    }
    else if(DMSI_TARGET_SUPPORTS_TDSCDMA(mode_capability))
    {  
      supp_channel_info.supp_channel = QMI_CHANNEL_SUPP_TDSCDMA;
    }
    else if(DMSI_TARGET_SUPPORTS_GSM(mode_capability))
    {
      #if defined(FEATURE_GSM_EGPRS)
        supp_channel_info.supp_channel = QMI_CHANNEL_SUPP_EDGE;
        supp_channel_info.extend.gprs_edge.multislot_class =
                                            QMI_DEFAULT_GPRS_EDGE_MULTISLOT_CLASS;
      #else
        supp_channel_info.supp_channel = QMI_CHANNEL_SUPP_GPRS;
        supp_channel_info.extend.gprs_edge.multislot_class =
                                            QMI_DEFAULT_GPRS_EDGE_MULTISLOT_CLASS;
      #endif
    }
    /* else supp_channel_info.supp_channel stays at QMI_CHANNEL_SUPP_MIN in
       which case qmi_svc_ext_get_default_channel_rates will return rates as 0 */
  } 
  else 
  {
    if(DMSI_TARGET_SUPPORTS_LTE(mode_capability))
    {   
      supp_channel_info.supp_channel = QMI_CHANNEL_SUPP_LTE;
    }
    else if(DMSI_TARGET_SUPPORTS_WCDMA(mode_capability))
    {  
      supp_channel_info.supp_channel = QMI_CHANNEL_SUPP_WCDMA;
    }
    else if(DMSI_TARGET_SUPPORTS_TDSCDMA(mode_capability))
    {  
      supp_channel_info.supp_channel = QMI_CHANNEL_SUPP_TDSCDMA;
    }
    else if(DMSI_TARGET_SUPPORTS_GSM(mode_capability))
    {
      #if defined(FEATURE_GSM_EGPRS)
        supp_channel_info.supp_channel = QMI_CHANNEL_SUPP_EDGE;
        supp_channel_info.extend.gprs_edge.multislot_class =
                                         qmi_if_info_ptr->gw_info.edge_multislot_class;
      #else
        supp_channel_info.supp_channel = QMI_CHANNEL_SUPP_GPRS;
        supp_channel_info.extend.gprs_edge.multislot_class =
                                         qmi_if_info_ptr->gw_info.gprs_multislot_class;
      #endif
    }
    /* else supp_channel_info.supp_channel stays at QMI_CHANNEL_SUPP_MIN in
       which case qmi_svc_ext_get_default_channel_rates will return rates as 0 */
  }
  qmi_svc_ext_get_default_channel_rates( supp_channel_info,
                                         &tx_rate,
                                         &rx_rate );
#endif /* FEATURE_WCDMA || FEATURE_GSM || FEATURE_LTE || FEATURE_TDSCDMA*/

#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
  if(DMSI_TARGET_SUPPORTS_HDR(mode_capability))
  {  
    #ifdef FEATURE_HDR_REVA_L1
      supp_channel_info.supp_channel = QMI_CHANNEL_SUPP_HDR_REVA;
    #else /* FEATURE_HDR_REVA_L1 */
      supp_channel_info.supp_channel = QMI_CHANNEL_SUPP_HDR_REV0;
    #endif /* FEATURE_HDR_REVA_L1 */
  }
  else if(DMSI_TARGET_SUPPORTS_CDMA(mode_capability))
  {  
    supp_channel_info.supp_channel = QMI_CHANNEL_SUPP_CDMA;
  }
  /* else supp_channel_info.supp_channel stays at QMI_CHANNEL_SUPP_MIN in
     which case qmi_svc_ext_get_default_channel_rates will return rates as 0 */
  qmi_svc_ext_get_default_channel_rates( supp_channel_info,
                                         &tx_rate_3gpp2,
                                         &rx_rate_3gpp2 );
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */

#if (( defined(FEATURE_LTE) || defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) || defined(FEATURE_TDSCDMA)) && \
    (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    *max_tx_channel_rate = MAX(tx_rate, tx_rate_3gpp2);
    *max_rx_channel_rate = MAX(rx_rate, rx_rate_3gpp2);
#elif defined(FEATURE_WCDMA) || defined(FEATURE_GSM)|| defined(FEATURE_LTE) || defined(FEATURE_TDSCDMA)
    *max_tx_channel_rate = tx_rate;
    *max_rx_channel_rate = rx_rate;
#elif defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
    *max_tx_channel_rate = tx_rate_3gpp2;
    *max_rx_channel_rate = rx_rate_3gpp2;
#endif /* defined(FEATURE_LTE) || (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) || defined(FEATURE_TDSCDMA) &&
          (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  return errval;
} /* qmi_dmsi_get_max_channel_rate */

/*===========================================================================
  FUNCTION QMI_DMSI_GET_SIM_CAPABILITY()

  DESCRIPTION
    to get the SIM capability of the mobile
    
  PARAMETERS
    None

  RETURN VALUE
    sim_capability
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint8 qmi_dmsi_get_sim_capability
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS) || defined(FEATURE_MMGSDI_CDMA)
    return(DMS_SIM_SUPPORTED_V01);           
  #else 
    return(DMS_SIM_NOT_SUPPORTED_V01);
  #endif

} /* qmi_dmsi_get_sim_capability */

/*===========================================================================
  FUNCTION QMI_DMSI_GET_HW_SUPPORTED_RADIO_IF_LIST()

  DESCRIPTION
    to get the list of supported radio technologies by the hardware
    
  PARAMETERS
    radio_if_list

  RETURN VALUE
    Number of supported radio_ifs
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint8 qmi_dmsi_get_hw_supported_radio_if_list
(
  uint32 *  radio_if_list
)
{
  uint8 index;
  qmi_if_info_type  *qmi_if_info_ptr;
  uint16 mode_capability = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  index = 0;   

  qmi_if_info_ptr = qmi_if_info_get_ptr();

  mode_capability = (uint16)qmi_if_info_ptr->ph_info.mode_capability;

  if(DMSI_TARGET_SUPPORTS_CDMA(mode_capability))
  {
      radio_if_list[index++] = DMS_RADIO_IF_1X_V01;
  }
   
  if(DMSI_TARGET_SUPPORTS_HDR(mode_capability))
  {  
    radio_if_list[index++] = DMS_RADIO_IF_1X_EVDO_V01;
  }
      
  if(DMSI_TARGET_SUPPORTS_GSM(mode_capability))
  {  
    radio_if_list[index++] = DMS_RADIO_IF_GSM_V01;
  }

  if(DMSI_TARGET_SUPPORTS_WCDMA(mode_capability))
  {  
    radio_if_list[index++] = DMS_RADIO_IF_UMTS_V01;
  }

  if(DMSI_TARGET_SUPPORTS_LTE(mode_capability))
  {  
    radio_if_list[index++] = DMS_RADIO_IF_LTE_V01;
  }

  if(DMSI_TARGET_SUPPORTS_TDSCDMA(mode_capability))
  {
    radio_if_list[index++] = DMS_RADIO_IF_TDS_V01;
  }
  return index;
                
} /* qmi_dmsi_get_hw_supported_radio_if_list */

#if !(defined(FEATURE_DATA_IOE_LWG) || defined(FEATURE_DATA_IOE_LONLY) || defined(FEATURE_DATA_IOE_LG))

/*===========================================================================
  FUNCTION QMI_DMSI_GET_SIMUL_VOICE_AND_DATA_CAPABILITY()

  DESCRIPTION
    Returns the Simultaneous Voice and Data Capability of the device
 
  PARAMETERS
    None
    
  RETURN VALUE
    simul_voice_and_data_capability mask identifying SVLTE and SVDO support
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint64 qmi_dmsi_get_simul_voice_and_data_capability
(
  void
)
{
  uint32 sv_mask = 0;
  uint64 simul_voice_and_data_capability = 0;
  sys_overall_feature_t dev_overall_cap;
  trm_get_info_return_type return_info;
  trm_get_info_input_type  input_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   memset(&return_info, 0x0, sizeof(trm_get_info_return_type));
   memset(&input_info, 0x0, sizeof(trm_get_info_input_type));
  
   input_info.client_id = TRM_CM; /* For techs which don't have a client; use CM */
   input_info.info_type = TRM_SIMUL_CAPABILITY;
    
   trm_get_info(&input_info, &return_info);
  
   //DS_ASSERT(return_info.info_type == TRM_SIMUL_CAPABILITY); as trm_get_ino is not defined 
   sv_mask = (uint32)return_info.info.simul_cap_info;

  if( sv_mask & TRM_SVLTE_IS_ENABLED)
  {  
    simul_voice_and_data_capability |= QMI_DMS_MASK_SVLTE_CAPABLE_V01;
  }
  if( sv_mask & TRM_SVDO_IS_ENABLED)
  {  
    simul_voice_and_data_capability |= QMI_DMS_MASK_SVDO_CAPABLE_V01;
  }
  if(QMI_ERR_NONE_V01 == qmi_dmsi_get_device_overall_capability(&dev_overall_cap))
  {
    if(dev_overall_cap == SYS_OVERALL_FEATURE_MODE_SGLTE)
    {
      simul_voice_and_data_capability |= QMI_DMS_MASK_SGLTE_CAPABLE_V01;
    }
  }
  return simul_voice_and_data_capability;
} /* qmi_dmsi_get_simul_voice_and_data_capability */


/*===========================================================================
  FUNCTION QMI_DMSI_GET_VOICE_CAPABILITY()

  DESCRIPTION
    to get the voice capability of the mobile
    
  PARAMETERS
    None
    
  RETURN VALUE
    voice_capability
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint8 qmi_dmsi_get_voice_capability
(
  void
)
{

  uint32 sv_mask = 0;
  trm_get_info_return_type return_info;
  trm_get_info_input_type  input_info;
  uint8 ret_val = DMS_DATA_CAP_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&return_info, 0x0, sizeof(trm_get_info_return_type));
  memset(&input_info, 0x0, sizeof(trm_get_info_input_type));

  input_info.client_id = TRM_CM; /* For techs which don't have a client; use CM */
  input_info.info_type = TRM_SIMUL_CAPABILITY;
  
  trm_get_info(&input_info, &return_info);

//  DS_ASSERT(return_info.info_type == TRM_SIMUL_CAPABILITY);  as trm_get_info is not defined 
  sv_mask = (uint32)return_info.info.simul_cap_info;

  if( (sv_mask & TRM_SVDO_IS_ENABLED) ||
      (sv_mask & TRM_SVLTE_IS_ENABLED) )
  {
    ret_val = DMS_DEVICE_CAP_SIMUL_VOICE_AND_DATA_V01;
  }
  else 
  {
    if( (MCS_MODEM_CAP_AVAILABLE == mcs_modem_has_capability(MCS_MODEM_CAPABILITY_FEATURE_GSM ))||
        (MCS_MODEM_CAP_AVAILABLE == mcs_modem_has_capability(MCS_MODEM_CAPABILITY_FEATURE_1X)) )
    {
      ret_val = DMS_DEVICE_CAP_VOICE_ONLY_V01;
    }

    if ((MCS_MODEM_CAP_AVAILABLE == mcs_modem_has_capability(MCS_MODEM_CAPABILITY_FEATURE_HSPA ))||
       (MCS_MODEM_CAP_AVAILABLE == mcs_modem_has_capability( MCS_MODEM_CAPABILITY_FEATURE_WCDMA))||
       (MCS_MODEM_CAP_AVAILABLE == mcs_modem_has_capability( MCS_MODEM_CAPABILITY_FEATURE_TDSCDMA)))
    {
      ret_val = DMS_DEVICE_CAP_NONSIMUL_VOICE_AND_DATA_V01;
    }

    if ( (MCS_MODEM_CAP_AVAILABLE == mcs_modem_has_capability(MCS_MODEM_CAPABILITY_FEATURE_LTE )) ||
         (MCS_MODEM_CAP_AVAILABLE == mcs_modem_has_capability(MCS_MODEM_CAPABILITY_FEATURE_DO )) )
    {
      if((DMS_DEVICE_CAP_VOICE_ONLY_V01 == ret_val ) ||
         (DMS_DEVICE_CAP_NONSIMUL_VOICE_AND_DATA_V01 == ret_val ))
      {
        ret_val = DMS_DEVICE_CAP_NONSIMUL_VOICE_AND_DATA_V01;
      }
      else
      {
        ret_val = DMS_DEVICE_CAP_DATA_ONLY_V01;
      }
    }

  }

  if ( DMS_DATA_CAP_NONE_V01 == ret_val )
  {
#if defined(FEATURE_IS2000_REL_A_SVD) || defined(FEATURE_UMTS_CONC_SRVC)
    ret_val= DMS_DEVICE_CAP_SIMUL_VOICE_AND_DATA_V01;
#elif defined(FEATURE_DATA)
    ret_val= DMS_DEVICE_CAP_NONSIMUL_VOICE_AND_DATA_V01;
#else /* FEATURE_DATA */
    ret_val= DMS_DEVICE_CAP_VOICE_ONLY_V01;
#endif
  }

  return ret_val;
} /* qmi_dmsi_get_voice_capability */

#endif

/*===========================================================================
  FUNCTION QMI_DMSI_GET_VOICE_SUPPORT_CAPABILITY()

  DESCRIPTION
    to get the voice support capability of the mobile,
    used to determine the device capability of fallback for voice    
  PARAMETERS
    None
    
  RETURN VALUE
    voice_support_capability mask identifying 3gpp CSFB and 1x CSFB support
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint64 qmi_dmsi_get_voice_support_capability
(
  void
)
{
  uint64 voice_support_capability = 0;

  #if defined(FEATURE_3GPP_CSFB)
    voice_support_capability |= QMI_DMS_MASK_VOICE_SUPPORT_GW_CSFB_CAPABLE_V01;
  #endif

  #if defined(FEATURE_LTE) && defined(FEATURE_LTE_TO_1X)
    voice_support_capability |= QMI_DMS_MASK_VOICE_SUPPORT_1x_CSFB_CAPABLE_V01;
  #endif

  return voice_support_capability;
} /* qmi_dmsi_get_voice_support_capability */

#if 0
/*===========================================================================
  FUNCTION QMI_DMSI_SET_BATTERY_LEVEL_LIMITS()

  DESCRIPTION
    It is used to determine the global high and low battery level limits to
    be set in the charge module.
    
  PARAMETERS
    curr_bat_lvl : The current battery level
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dmsi_set_battery_level_limits
(
  uint8 curr_bat_lvl
)
{
  qmi_dmsi_client_info_type *  cl_sp;
  uint8                        low_limit, high_limit;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_dmsi_client_info_type*)q_check(&qmi_dms_state.client_q);
  low_limit = DMSI_BAT_LVL_MIN;
  high_limit = DMSI_BAT_LVL_MAX;
  
  /*-------------------------------------------------------------------------
    traverse through all the DMS clients to determine the new high and low
    battery limits - the new high and low limts for the charger are the 
    client's upper and lower limts which are closest to the current battery
    level

    Note - It is possible that after looping through the high and low limits 
    to be set in the charger remain unchanged and it will result in again 
    setting the existing charger limits, but that is alright
  -------------------------------------------------------------------------*/
    while( cl_sp != NULL )
    {
      if(  cl_sp->report_status.report_power_state )
      {
        /*-------------------------------------------------------------------
          The new high limit for the charger is the client's upper or lower
          limit closest to the current battery level on the higher side
        -------------------------------------------------------------------*/
        if(cl_sp->report_status.bat_lvl_upper_limit >= curr_bat_lvl)
        {
          high_limit = MIN(cl_sp->report_status.bat_lvl_upper_limit,
                           high_limit);
        }
        if(cl_sp->report_status.bat_lvl_lower_limit >= curr_bat_lvl)
        {
          high_limit = MIN(cl_sp->report_status.bat_lvl_lower_limit,
                           high_limit);
        }

        /*-------------------------------------------------------------------
          The new low limit for the charger is the client's upper or lower
          limit closest to the current battery level on the lower side
        -------------------------------------------------------------------*/
        if(cl_sp->report_status.bat_lvl_lower_limit <= curr_bat_lvl)
        {
          low_limit = MAX(cl_sp->report_status.bat_lvl_lower_limit,
                          low_limit);
        }
        if(cl_sp->report_status.bat_lvl_upper_limit <= curr_bat_lvl)
        {
          low_limit = MAX(cl_sp->report_status.bat_lvl_upper_limit,
                          low_limit);
        }
      }
      cl_sp = q_next(&qmi_dms_state.client_q,
                           &(cl_sp->link));
    }


  /*-------------------------------------------------------------------------
    Set the high and low limits in the charger to the computed global high
    and low limts
  -------------------------------------------------------------------------*/
  qmi_charger_set_battery_level_limits(low_limit, high_limit);

  qmi_dmsi_global.pwr_rpt.low_limit = low_limit;
  qmi_dmsi_global.pwr_rpt.high_limit = high_limit;
} /* dmsi_set_battery_level_limits() */
#endif
/*===========================================================================
  FUNCTION QMI_DMSI_GET_MULTISIM_INFO()

  DESCRIPTION
    Get the multisim capabilities of the device from policyman and populate the response

  PARAMETERS
    response (input parameter)
    response (output parameter)

  RETURN VALUE
    errval

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_dmsi_get_multisim_info
(
  dms_multisim_capability_type_v01* response
)
{  
  policyman_status_t         status;
  policyman_item_t const     *cm_policyman_device_config = NULL;
  policyman_item_id_t        ids[] = { POLICYMAN_ITEM_DEVICE_CONFIGURATION };
  uint32                     max_subs;
  uint32                     config_list_len;
  uint32                     max_active;
  uint32                     rat; 
  uint64                     rat_mask;
  int32                      cfg_index;
  int32                      subs_index;
  qmi_error_type_v01         errval = QMI_ERR_NONE_V01;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(response,0x00,sizeof(dms_multisim_capability_type_v01));

  status = policyman_get_items(
              &ids[0],
              sizeof(ids)/sizeof(policyman_item_id_t),
              &cm_policyman_device_config
              );
  if (POLICYMAN_FAILED(status))
  {
    errval = QMI_ERR_INTERNAL_V01;
    return errval;
  }
  
  status = 
    policyman_device_config_num_sims(cm_policyman_device_config,(size_t*)&max_subs);
  response->max_subscriptions = (uint8)max_subs;

  if (POLICYMAN_FAILED(status))
  {
    errval = QMI_ERR_INTERNAL_V01;
    return errval;
  }

  status = policyman_device_config_max_active(cm_policyman_device_config, (size_t*)&max_active);
 
  if (POLICYMAN_FAILED(status))
  {
    errval = QMI_ERR_INTERNAL_V01;
    return errval;
  }
      
  status = policyman_device_config_num_cfgs(cm_policyman_device_config, (size_t*)&config_list_len);

  if (POLICYMAN_FAILED(status))
  {
    errval = QMI_ERR_INTERNAL_V01;
    return errval;
  }

  if( (max_subs > QMI_DMS_MAX_SUBSCRIPTION_LIST_LEN_V01) || 
      ( config_list_len > QMI_DMS_MAX_CONFIG_LIST_LEN_V01 ) ||
      (max_active > max_subs) )
  {
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  
  DS_MSG3(MSG_LEGACY_MED,"num_sims %d max_active %d num_cfg %d",
                  max_subs, max_active, config_list_len);   
  response->subscription_config_list_len = config_list_len;
  cfg_index  = response->subscription_config_list_len;
  subs_index = response->max_subscriptions;
  for (cfg_index -= 1 ; cfg_index >= 0 ; cfg_index--)
  {
    response->subscription_config_list[cfg_index].subscription_list_len = subs_index;
	DS_MSG2(MSG_LEGACY_MED,"subs config list len for config_index %d %d",subs_index,cfg_index);
    for (subs_index -= 1 ; subs_index >= 0 ; subs_index--)
    {
      status = policyman_device_config_get_config(cm_policyman_device_config, cfg_index, subs_index, &rat);
      if (POLICYMAN_FAILED(status))
      {
        errval = QMI_ERR_INTERNAL_V01;
        goto send_result;
      }
      rat_mask = qmi_dmsi_convert_rat_mask(rat);
      response->subscription_config_list[cfg_index].subscription_list[subs_index] = rat_mask;
      DS_MSG2(MSG_LEGACY_MED,"ratmask for subs_index %d %d",rat_mask,subs_index);  
    }
    response->subscription_config_list[cfg_index].max_active = max_active;
    subs_index = response->max_subscriptions;
  }

  send_result:
  policyman_item_release(cm_policyman_device_config);
  return errval;
}

/*===========================================================================
  FUNCTION QMI_DMSI_GET_DEVICE_CONFIG_LIST()

  DESCRIPTION
    Get the Device configuration list from policyman and populate the response

  PARAMETERS
    response (input parameter)

  RETURN VALUE
    errval

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_dmsi_get_device_config_list
(
  dms_device_config_capability_type_v01* response
)
{  
  policyman_status_t         status;
  policyman_item_t const     *cm_policyman_device_config = NULL;
  policyman_item_id_t        ids[] = { POLICYMAN_ITEM_DEVICE_CONFIGURATION };
  int32                      cfg_index, subs_index;
  size_t                     max_subs, config_list_len, max_active, curr_index;
  uint32                     rat;
  uint64                     rat_mask;
  qmi_error_type_v01         errval = QMI_ERR_NONE_V01;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(response,0x00,sizeof(dms_device_config_capability_type_v01));

  status = policyman_get_items(
              &ids[0],
              sizeof(ids)/sizeof(policyman_item_id_t),
              &cm_policyman_device_config);
  if (POLICYMAN_FAILED(status))
  {
    errval = QMI_ERR_INTERNAL_V01;
    return errval;
  }

  status = policyman_device_config_num_sims(cm_policyman_device_config,
                                            &max_subs);
  if (POLICYMAN_FAILED(status) || (max_subs > QMI_DMS_POLICYMAN_MAX_SUBS))
  {
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  /*Fill the TLV info*/
  response->max_subscriptions = (uint8)max_subs;

  status = policyman_device_config_max_active_subs(cm_policyman_device_config,
                                                   &max_active);
  if (POLICYMAN_FAILED(status) || (max_active > QMI_DMS_POLICYMAN_MAX_SUBS) ||
      (max_active > max_subs))
  {
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  /*Fill the TLV info*/
  response->max_active = (uint8)max_active;
  
  status = policyman_device_config_num_cfgs(cm_policyman_device_config,
                                            &config_list_len);
  if ( POLICYMAN_FAILED(status) || (config_list_len > QMI_DMS_MAX_CONFIG_LIST_LEN_V01) )
  {
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }

  DS_MSG3(MSG_LEGACY_MED,"num_sims %d max_active %d num_cfg %d",
                  max_subs, max_active, config_list_len);
  /*Fill the TLV info*/ 
  response->device_cfg_list_len = config_list_len;
  
  cfg_index  = response->device_cfg_list_len;
  subs_index = response->max_subscriptions;
  for (cfg_index -= 1 ; cfg_index >= 0 ; cfg_index--)
  {
    /*Fill the TLV info : Each entry of the list is configurations of all subs*/
    response->device_cfg_list[cfg_index].subs_cfg_list_len = subs_index;
    for (subs_index -= 1 ; subs_index >= 0 ; subs_index--)
    {
      status = policyman_device_config_get_config(cm_policyman_device_config,
                                                  cfg_index, subs_index, &rat);
      if (POLICYMAN_FAILED(status))
      {
        errval = QMI_ERR_INTERNAL_V01;
        goto send_result;
      }
      rat_mask = qmi_dmsi_convert_rat_mask(rat);
      /*Fill the TLV info*/ 
      response->device_cfg_list[cfg_index].subs_cfg_list[subs_index] = 
                                (dms_subs_capability_mask_type_v01)rat_mask;
      DS_MSG2(MSG_LEGACY_MED,"ratmask for subs_index %d %d",rat_mask,subs_index);
    }
    subs_index = response->max_subscriptions;
  }

  status = policyman_device_config_get_current_config_index(cm_policyman_device_config,
                                                            &curr_index);
  /* Index should point within array indices starting from 0*/
  if (POLICYMAN_FAILED(status) || (curr_index >= config_list_len))
  {
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  /*Fill the TLV info*/ 
  response->current_config_index = (uint8)curr_index;

  send_result:
  policyman_item_release(cm_policyman_device_config);
  return errval;
}/*qmi_dmsi_get_device_config_list()*/

/*===========================================================================
  FUNCTION QMI_DMSI_GET_MAX_SUBS_STATIC_DEVICE_CAP()

  DESCRIPTION
    Get the device static maximum rat capabilities from policyman and 
    populate the response

  PARAMETERS
    dms pointer to the response (input parameter)

  RETURN VALUE
    errval (output parameter)

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01  qmi_dmsi_get_max_subs_static_device_cap
(
  dms_get_device_cap_resp_msg_v01* response
)
{
  qmi_error_type_v01   errval = QMI_ERR_NONE_V01;
  uint32   max_subs,subs_index,max_rat_cap;
  policyman_status_t      status;
  policyman_item_t const  *cm_policyman_device_config = NULL;
  policyman_item_id_t ids[] = { POLICYMAN_ITEM_DEVICE_CONFIGURATION };
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  max_rat_cap = 0;
  /*Get the policyman items intially*/
  status = policyman_get_items_block(
                &ids[0],
                sizeof(ids)/sizeof(policyman_item_id_t),
                &cm_policyman_device_config);
  if (POLICYMAN_FAILED(status))
  {
    errval = QMI_ERR_INTERNAL_V01;
    DS_MSG0(MSG_LEGACY_ERROR,"policyman_get_items_block failed");
    goto send_result;
  }

  status = policyman_device_config_num_sims(cm_policyman_device_config,
                                             (size_t*)&max_subs);
  if (POLICYMAN_FAILED(status) || (max_subs > QMI_DMS_POLICYMAN_MAX_SUBS))
  {
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  response->device_max_subscription_static_capability_len = (uint8)max_subs;

  /*get the max static rat capabiltiy of each subs*/
  for( subs_index = (uint32)SYS_MODEM_AS_ID_1;
       (subs_index < max_subs) && (subs_index <= SYS_MODEM_AS_ID_3);
        subs_index++ )
  {
    status = policyman_device_config_get_max_capability(cm_policyman_device_config,
                                                        subs_index,
                                                        &max_rat_cap);
    if ( POLICYMAN_FAILED(status) )
    {
      errval = QMI_ERR_INTERNAL_V01;
      goto send_result;
    }
    response->device_max_subscription_static_capability[subs_index] = 
                                            qmi_dmsi_convert_rat_mask(max_rat_cap);
  }

  send_result:
    if(NULL != cm_policyman_device_config)
    {
      policyman_item_release(cm_policyman_device_config);
    }
    return errval;

}/*qmi_dmsi_get_max_subs_static_device_cap()*/

#if !(defined(FEATURE_DATA_IOE_LWG) || defined(FEATURE_DATA_IOE_LONLY) || defined(FEATURE_DATA_IOE_LG))
/*===========================================================================
  FUNCTION QMI_DMSI_GET_DEVICE_OVERALL_CAPABILITY()

  DESCRIPTION
    Get the overall capabilities(h/w + configuration) of the device from policyman and populate the response

  PARAMETERS
    device_overall_cap (input parameter)
    errval (output parameter)

  RETURN VALUE
    errval

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_dmsi_get_device_overall_capability
(
  sys_overall_feature_t* device_overall_cap
)
{ 
  qmi_error_type_v01   errval = QMI_ERR_NONE_V01;

  policyman_status_t  status;   
  policyman_item_t const *cm_policyman_device_config = NULL;
  policyman_item_id_t ids[] = { POLICYMAN_ITEM_DEVICE_CONFIGURATION };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(NULL == device_overall_cap)
  {
    errval = QMI_ERR_INTERNAL_V01;
    DS_MSG0(MSG_LEGACY_ERROR,"Invalid input parameter ");
    goto send_result;
  }
  
  status = policyman_get_items(
              &ids[0],
              sizeof(ids)/sizeof(policyman_item_id_t),
              &cm_policyman_device_config
              );
  if (POLICYMAN_FAILED(status))
  {
    errval = QMI_ERR_INTERNAL_V01;
    DS_MSG0(MSG_LEGACY_ERROR,"get_items fail"); 
    goto send_result;

  }
  
  status = 
    policyman_device_config_overall_feature(cm_policyman_device_config,(sys_overall_feature_t*)device_overall_cap);

  if (POLICYMAN_FAILED(status))
  {
    errval = QMI_ERR_INTERNAL_V01;
    DS_MSG0(MSG_LEGACY_ERROR,"device_config_overall_feature fail");
    goto send_result;
  }

  if((sys_overall_feature_t)*device_overall_cap > SYS_OVERALL_FEATURE_MODE_SGLTE)
  {
    errval = QMI_ERR_INTERNAL_V01;
    DS_MSG1(MSG_LEGACY_ERROR,"cap is greater %d", *device_overall_cap);
  }

  send_result:
  if(NULL != cm_policyman_device_config)
  {
    policyman_item_release(cm_policyman_device_config);
  }
  return errval;
}
#endif // !(defined(FEATURE_DATA_IOE_LWG) || defined(FEATURE_DATA_IOE_LONLY))

/*===========================================================================
  FUNCTION QMI_DMSI_GET_SUBS_DEVICE_FEATURE_MODE()

  DESCRIPTION
    Get the device static feature mode of the device from policyman and 
    populate the response

  PARAMETERS
    dms pointer to the response (input parameter)
    errval (output parameter)

  RETURN VALUE
    errval

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_dmsi_get_subs_device_feature_mode
(
  dms_get_device_cap_resp_msg_v01* response
)
{
  qmi_error_type_v01   errval = QMI_ERR_NONE_V01;
  uint32   max_subs;
  uint32   subs_index;
  sys_subs_feature_t      subs_device_feature_mode;
  sys_overall_feature_t   device_overall_cap;
  dms_device_subs_feature_mode_enum_v01  dms_sub_feature_mode;

  policyman_status_t      status;
  policyman_item_t const  *cm_policyman_device_config = NULL;
  policyman_item_id_t ids[] = { POLICYMAN_ITEM_DEVICE_CONFIGURATION };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_LOW,"qmi_dmsi_get_subs_device_feature_mode");

  /*Get the policyman items intially*/
  status = policyman_get_items_block(
              &ids[0],
              sizeof(ids)/sizeof(policyman_item_id_t),
              &cm_policyman_device_config);
  if (POLICYMAN_FAILED(status))
  {
    errval = QMI_ERR_INTERNAL_V01;
    DS_MSG0(MSG_LEGACY_ERROR,"policyman_get_items_block failed");
    goto send_result;
  }

  /*Get the policyman overall feature first and 
    if it returns the MULTISIM then go for per subs feature mode*/
  status = 
    policyman_device_config_overall_feature(cm_policyman_device_config,&device_overall_cap);
  if (POLICYMAN_FAILED(status))
  {
    errval = QMI_ERR_INTERNAL_V01;
    DS_MSG0(MSG_LEGACY_ERROR,"device_config_overall_feature fail");
    goto send_result;
  }

  if ( device_overall_cap != SYS_OVERALL_FEATURE_MODE_MULTISIM )
  {
    /*Single sim case, treat this as the first subs and send info*/
    max_subs = 1;     
    switch(device_overall_cap)
    { 
      case SYS_OVERALL_FEATURE_MODE_NORMAL:
          dms_sub_feature_mode  = DMS_DEVICE_SUBS_FEATURE_MODE_NORMAL_V01;
        break;
      case SYS_OVERALL_FEATURE_MODE_SVLTE:
          dms_sub_feature_mode  = DMS_DEVICE_SUBS_FEATURE_MODE_SVLTE_V01;
        break;
      case SYS_OVERALL_FEATURE_MODE_SGLTE:
          dms_sub_feature_mode  = DMS_DEVICE_SUBS_FEATURE_MODE_SGLTE_V01;
        break;
      case SYS_OVERALL_FEATURE_MODE_SRLTE:
          dms_sub_feature_mode  = DMS_DEVICE_SUBS_FEATURE_MODE_SRLTE_V01;
        break;
      default:
          dms_sub_feature_mode  = DMS_DEVICE_SUBS_FEATURE_MODE_NORMAL_V01;
        break;
    }
    /*Fill the first element of the array*/
    response->subs_device_feature_mode_len = max_subs;
    response->subs_device_feature_mode[max_subs-1] = dms_sub_feature_mode;
  }
  else
  {
    /*Multi sim case, get the per subs info and send it*/
    status = 
      policyman_device_config_num_sims(cm_policyman_device_config,(size_t*)&max_subs);
    if (POLICYMAN_FAILED(status) || (max_subs > SYS_MODEM_AS_ID_3))
    {
      errval = QMI_ERR_INTERNAL_V01;
      goto send_result;
    }
    response->subs_device_feature_mode_len = max_subs;
    for( subs_index = (uint32)SYS_MODEM_AS_ID_1;
         (subs_index < max_subs) && (subs_index < QMI_DMS_POLICYMAN_MAX_SUBS);
         subs_index++ )
    {
      status = policyman_device_config_get_subs_feature(cm_policyman_device_config,0,
                                                        subs_index,
                                                        &subs_device_feature_mode);
      if (POLICYMAN_FAILED(status))
      {  
       errval = QMI_ERR_INTERNAL_V01;
       goto send_result;
      }

      switch(subs_device_feature_mode)
      { 
        case SYS_SUBS_FEATURE_MODE_NORMAL:
             dms_sub_feature_mode = DMS_DEVICE_SUBS_FEATURE_MODE_NORMAL_V01;
          break;
        case SYS_SUBS_FEATURE_MODE_SVLTE:
          dms_sub_feature_mode    = DMS_DEVICE_SUBS_FEATURE_MODE_SVLTE_V01;
          break;
        case SYS_SUBS_FEATURE_MODE_SGLTE:
            dms_sub_feature_mode  = DMS_DEVICE_SUBS_FEATURE_MODE_SGLTE_V01;
          break;
        case SYS_SUBS_FEATURE_MODE_SRLTE:
            dms_sub_feature_mode  = DMS_DEVICE_SUBS_FEATURE_MODE_SRLTE_V01;
          break;
        default:
            dms_sub_feature_mode  = DMS_DEVICE_SUBS_FEATURE_MODE_NORMAL_V01;
          break;
      }
      response->subs_device_feature_mode[subs_index] = (uint32)dms_sub_feature_mode;
    }
  }

  send_result:
  if(NULL != cm_policyman_device_config)
  {
    policyman_item_release(cm_policyman_device_config);
  }
  return errval;
}

#ifdef FEATURE_DATA_QMI_DMS_MULTISIM
/*===========================================================================
  FUNCTION QMI_DMSI_GET_MAX_ACTIVE_DATA_SUBS()

  DESCRIPTION
    Get the max active data subscriptions on device from policyman 
    and populate the response

  PARAMETERS
    response - dsm pointer to the response (input parameter)
    cmd_type - specifies the tlv id (input parameter)
    errval (output parameter)

  RETURN VALUE
    errval

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_dmsi_get_max_active_data_subs
(
  uint8_t*  response
)
{
  policyman_status_t  status = -1; 
  policyman_item_t const *cm_policyman_device_config = NULL;
  policyman_item_id_t id1[] = { POLICYMAN_ITEM_DEVICE_CONFIGURATION };
  uint32   max_data_subs = 0;
  qmi_error_type_v01 errval = QMI_ERR_NONE_V01;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_MSG0(MSG_LEGACY_LOW,"qmi_dmsi_get_max_active_data_subs");
 
  status = policyman_get_items(
                &id1[0],
                sizeof(id1)/sizeof(policyman_item_id_t),
                &cm_policyman_device_config
                );
  if (POLICYMAN_FAILED(status))
  {
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }

  status = 
    policyman_device_config_max_active_data(cm_policyman_device_config,(size_t*)&max_data_subs);
  if (POLICYMAN_FAILED(status) || (max_data_subs > SYS_MODEM_AS_ID_3))
  {
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  *response = max_data_subs;
  send_result:
  if(NULL != cm_policyman_device_config)
  {
    policyman_item_release(cm_policyman_device_config);
  }
  return errval;
}
#endif /* FEATURE_DATA_QMI_DMS_MULTISIM */

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
/*===========================================================================
  FUNCTION QMI_DMSI_MOBNUM_PCS_TO_TEXT()

  DESCRIPTION
    convert MDN NV format to ascii-text representation
    
  PARAMETERS
    dest : will contain the mobile number in ascii-text format
    src  : mobile number in NV format

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dmsi_mobnum_pcs_to_text
(
  char *                  dest, 
  nv_mob_dir_data_type *  src
)
{
  int i;
  int len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(dest && src);

  len = src->n_digits;

  if(len > NV_DIR_NUMB_PCS_SIZ)
  {
    DS_MSG0(MSG_LEGACY_MED,"Ignoring Mobile Number digits 16 and higher");
  }

  /*-------------------------------------------------------------------------
    MDN NV format is sort of like BCD, but 8 bits instead of 4, and 
    '0' is represented as 0x0A.  Stop parsing if a place holder digit (0x0F)
    is found.
  -------------------------------------------------------------------------*/
  for (i=0; (i < MIN(len,NV_DIR_NUMB_PCS_SIZ)) && (0x0F != src->digitn[i]); i++)
  {
    dest[i] = (src->digitn[i]==10) ? (char)'0' : (char)src->digitn[i] + '0';
  }
  dest[i] = '\0';
} /* qmi_dmsi_mobnum_pcs_to_text() */
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */

/*===========================================================================
  FUNCTION QMI_DMSI_GET_BAND_CAPABILITY()

  DESCRIPTION
    Gives the current band class info
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_get_band_capability
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
/* When feature flag LTE_BAND_NUM is disabled at MMCP side,
   define it to 64 so that the behavior is as per legacy implementation*/
#ifndef LTE_BAND_NUM
   #define LTE_BAND_NUM 64
#endif
  qmi_if_info_type  *qmi_if_info_ptr;
  dms_get_band_capability_resp_msg_v01 resp_msg;
#ifdef LTE_BAND_NUM
  uint16                               i;
  uint64                               bitmask = 1;
  uint64                               lte_band_capability;
#endif /* LTE_BAND_NUM */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&resp_msg,0,sizeof(dms_get_band_capability_resp_msg_v01));
  qmi_if_info_ptr = qmi_if_info_get_ptr();

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)\
    || defined(FEATURE_CDMA) || defined(FEATURE_HDR)
  resp_msg.band_capability = (uint64)qmi_if_info_ptr->ph_info.band_capability;
#else
 resp_msg.band_capability = (uint64)0;
#endif

  resp_msg.lte_band_capability_valid = TRUE;
  memscpy(&resp_msg.lte_band_capability,
          sizeof(uint64),
          &qmi_if_info_ptr->ph_info.lte_band_capability,
          sizeof(uint64));
#if defined(FEATURE_TDSCDMA)
  resp_msg.tds_band_capability_valid = TRUE;
  resp_msg.tds_band_capability = (uint64)qmi_if_info_ptr->ph_info.tds_band_capability;
#endif

#ifdef LTE_BAND_NUM
  resp_msg.supported_lte_bands_len = 0;

  /* The following code snippet loops over each band (total bands LTE_BAND_NUM) to get
     the relevant uint64 variable to be used to get the supported band number */
  for (i = 0; i < LTE_BAND_NUM; i++)
  {
    if (i % 64 == 0)
    {
      bitmask = 1;
    }
#if (LTE_BAND_NUM == 256)
    if (i < 64)
  {
      lte_band_capability = qmi_if_info_ptr->ph_info.lte_band_capability.bits_1_64;
  }
    else if (i >= 64 && i < 128)
    {
      lte_band_capability = qmi_if_info_ptr->ph_info.lte_band_capability.bits_65_128;
    }
    else if (i >= 128 && i < 192)
    {
      lte_band_capability = qmi_if_info_ptr->ph_info.lte_band_capability.bits_129_192;
    }
    else
    {
      lte_band_capability = qmi_if_info_ptr->ph_info.lte_band_capability.bits_193_256;
    }
#else
    lte_band_capability = qmi_if_info_ptr->ph_info.lte_band_capability;
#endif /* LTE_BAND_NUM == 256 */
    
    if (bitmask & lte_band_capability)
    {
      resp_msg.supported_lte_bands[resp_msg.supported_lte_bands_len++] = i+1;
    }
    bitmask = bitmask << 1;
  }
  if (resp_msg.supported_lte_bands_len > 0)
  {
    resp_msg.supported_lte_bands_valid = TRUE;
  }
#endif /*LTE_BAND_NUM */

  resp_msg.resp.error = QMI_ERR_NONE_V01;
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_get_band_capability_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;


} /* qmi_dmsi_get_band_capability */

#ifdef FEATURE_DATA_QMI_DMS_FACTORY
/*===========================================================================
  FUNCTION QMI_DMSI_GET_FACTORY_SKU()

  DESCRIPTION
    Gives the factory SKU for the device.
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_get_factory_sku
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  qmi_error_type_v01   errval = QMI_ERR_NONE_V01;
  qmi_result_type_v01  result = QMI_RESULT_SUCCESS_V01;
  char *             factory_sku;
  nv_stat_enum_type  nv_status;
  nv_item_type      *ds_nv_item_ptr;
  dms_get_factory_sku_resp_msg_v01 resp_msg;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&resp_msg,0,sizeof(dms_get_factory_sku_resp_msg_v01));
  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(ds_nv_item_ptr, sizeof(nv_item_type),
                           nv_item_type*);
  if( ds_nv_item_ptr == NULL )
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }
  
  // Query NV For factory SKU
  memset(ds_nv_item_ptr, 0, sizeof(nv_item_type));
  nv_status = ds_get_nv_item(NV_FACTORY_DATA_4_I, ds_nv_item_ptr);
  if (NV_DONE_S == nv_status)
  {
    factory_sku = (char *) ds_nv_item_ptr->factory_data_4;
    strlcpy((char*)&(resp_msg.factory_serial_number),factory_sku,sizeof(resp_msg.factory_serial_number));
  } 
  else if (NV_NOTACTIVE_S == nv_status) 
  {
    errval = QMI_ERR_NOT_PROVISIONED_V01;
  } 
  else 
  {
    errval = QMI_ERR_INTERNAL_V01;
  }

  /*-------------------------------------------------------------------------
    Free the temporary memory allocated for NV item.
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);;
  
send_result:
  result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01 
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_get_factory_sku_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
} /* qmi_dmsi_get_factory_sku */
#endif /* FEATURE_DATA_QMI_DMS_FACTORY */

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
  FUNCTION QMI_DMSI_GENERATE_READ_UIM_MSISDN_RESP()

  DESCRIPTION
    To send a response to the QMI read MSISDN requested by the client.
    
  PARAMETERS
    read_cnf : info for IMSI read from UIM
    cmd_buf_p :Async command buffer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dmsi_generate_read_uim_msisdn_resp
(
  mmgsdi_read_cnf_type * read_cnf,
  ds_qmi_dms_async_cmd_type* cmd_buf_p
)
{
  qmi_error_type_v01        errval = QMI_ERR_NONE_V01;
  qmi_result_type_v01       result = QMI_RESULT_SUCCESS_V01;
  boolean                 output_msisdn;
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  char                    mob_num[NV_DIR_NUMB_PCS_SIZ + 1];
  char                    min_num[NV_DIR_NUMB_PCS_SIZ+1];
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */

  mmgsdi_return_enum_type mmgsdi_status;
  mmgsdi_access_type      sim_filename;
  qmi_if_mmgsd_session_e_type session_index;

  qmi_if_info_type  *qmi_if_info_ptr;
  qmi_dmsi_client_info_type * dms_cl_sp;
  ds_qmi_dms_async_cmd_type* async_cmd_buf = cmd_buf_p;
  static char uim_imsi[DMSI_MMGSDI_IMSI_LEN * 2 + 1];
  static char uim_msisdn[DMSI_MMGSDI_MSISDN_LEN * 2 + 1];
  dms_get_msisdn_resp_msg_v01 resp_msg;
  ds3g_mmgsdi_info_type        *ds_mmgsdi_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mmgsdi_status = read_cnf->response_header.mmgsdi_status;
  qmi_if_info_ptr = qmi_if_info_get_ptr();

  //Get the ds_mmgsdi ptr from the PDNMGR.
  ds_mmgsdi_ptr = ds3g_get_mmgsdi_info_hndl();
  if(ds_mmgsdi_ptr == NULL)
  {
    DS_ERR_FATAL("Can not get the mmgsdi info ptr from the PDNMGR so asserting !!");
	return;
  }
  if(ds_mmgsdi_ptr->session_id == QMI_IF_UNUSED_SESSION_ID)
  {    
     DS_ERR_FATAL("Can not get the valid mmgsdi session  ptr from the PDNMGR so asserting!!");	   
  }
  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  output_msisdn = FALSE;
  dms_cl_sp = cmd_buf_p->cl_sp;
  if(dms_cl_sp == NULL)
  {
    return;
  }
  session_index = QMI_IF_MMGSD_GW_PROV_PRI_SESSION;

#ifdef FEATURE_DUAL_SIM
  if(dms_cl_sp->subscription_id == DMSI_BIND_SUBS_SECONDARY) 
  {
    session_index = QMI_IF_MMGSD_GW_PROV_SEC_SESSION;
  }
#endif
#ifdef FEATURE_TRIPLE_SIM
  if(dms_cl_sp->subscription_id == DMSI_BIND_SUBS_TERTIARY)
  {
    session_index = QMI_IF_MMGSD_GW_PROV_TER_SESSION;
  }
#endif  
  if (read_cnf->access.file.file_enum != MMGSDI_TELECOM_MSISDN &&
      read_cnf->access.file.file_enum != MMGSDI_USIM_MSISDN    &&
      read_cnf->access.file.file_enum != MMGSDI_GSM_IMSI       && 
      read_cnf->access.file.file_enum != MMGSDI_USIM_IMSI      &&
      read_cnf->access.file.file_enum != MMGSDI_USIM_EXT5)
  {
    DS_MSG1(MSG_LEGACY_MED,"Cannot determine EF to read (%d)",
                    read_cnf->access.file.file_enum);
    return;
  }

  if(read_cnf->access.file.file_enum == MMGSDI_GSM_IMSI ||
     read_cnf->access.file.file_enum == MMGSDI_USIM_IMSI)
  {
    memset(uim_imsi, 0, DMSI_MMGSDI_IMSI_LEN * 2 + 1);

    /* Report IMSI only when it is available through MMGSDI */
    if (MMGSDI_SUCCESS == mmgsdi_status) 
    {
      int i;
      uint8 current_byte;
      boolean bcd_valid = FALSE;

      /* Decode MCC1 from Byte 2 (bit 4-7), should always available */
      current_byte = read_cnf->read_data.data_ptr[1];
      if(QMI_BCD_HIGH_DIGIT(current_byte) < 0x0A)
      {
        bcd_valid = TRUE;
        uim_imsi[0] = QMI_BCD_HIGH_DIGIT(current_byte) + '0';

        /* Decode the rest from BCD to ASCII string, stop when out of range */
        for(i = 2; i < DMSI_MMGSDI_IMSI_LEN; i++)
        {
          current_byte = read_cnf->read_data.data_ptr[i];
          if(QMI_BCD_LOW_DIGIT(current_byte) < 0x0A)
            uim_imsi[(i - 2) * 2 + 1] = QMI_BCD_LOW_DIGIT(current_byte) + '0';
          else if(QMI_BCD_LOW_DIGIT(current_byte) == 0x0F)
            break;
          else
          {
            bcd_valid = FALSE;
            break;
          }
        
          if(QMI_BCD_HIGH_DIGIT(current_byte) < 0x0A)
            uim_imsi[(i - 2) * 2 + 2] = QMI_BCD_HIGH_DIGIT(current_byte) + '0';
          else if(QMI_BCD_HIGH_DIGIT(current_byte) == 0x0F)
            break;
          else
          {
            bcd_valid = FALSE;
            break;
          }
        }
      }

      /* Only send IMSI if it is valid*/
      if(bcd_valid != TRUE)
      {
          memset(uim_imsi, 0, DMSI_MMGSDI_IMSI_LEN);
      }
    }

    /* Read MSISDN request to MMGSDI */
    sim_filename.access_method = MMGSDI_EF_ENUM_ACCESS;
    sim_filename.file.file_enum = MMGSDI_NO_FILE_ENUM;
    
    //if (qmi_if_info_ptr->session_info[session_index].app_type == MMGSDI_APP_SIM)
    if(ds_mmgsdi_ptr->app_type == MMGSDI_APP_SIM)
    {
      sim_filename.file.file_enum = MMGSDI_TELECOM_MSISDN;
    }
    //else if (qmi_if_info_ptr->session_info[session_index].app_type == MMGSDI_APP_USIM)
    else if(ds_mmgsdi_ptr->app_type == MMGSDI_APP_USIM)
    {
      sim_filename.file.file_enum = MMGSDI_USIM_MSISDN;
    }
    else
    {
      errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
       goto send_result;
    }

    //ds_qmi_cflog_amss_call("mmgsdi_session_read_record()");
    /* Read entire record for EF-MSISDN (x+14), len param = 0 */
    mmgsdi_status = mmgsdi_session_read_record(
                                     //qmi_if_info_ptr->session_info[session_index].session_id,
                                     ds_mmgsdi_ptr->session_id,
                                     sim_filename,
                                     1,
                                     0,
                                     qmi_if_mmgsdi_resp_cback,
                                     (uint32) async_cmd_buf);

    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      /* MSISDN will be reported back later from MMGSDI, other items will be
      processed at that time */
      return;
    }
    else
    { 
      /* For any reason MSISDN cannot be obtained from MMGSDI it will be
      skipped and the functon will go forward to process other items */
      DS_MSG1(MSG_LEGACY_ERROR,"MMGSDI read MSISDN status %d", mmgsdi_status);
      errval = QMI_ERR_INTERNAL_V01;
      goto send_result;
    }
  
  }
  
  if (read_cnf->access.file.file_enum == MMGSDI_TELECOM_MSISDN ||
      read_cnf->access.file.file_enum == MMGSDI_USIM_MSISDN)
  {
    memset(uim_msisdn, 0, DMSI_MMGSDI_MSISDN_LEN * 2 + 1);
    /* Report MSISDN only when it is available through MMGSDI */
    if (MMGSDI_SUCCESS == mmgsdi_status) 
    {
      uint8 current_byte, i, offset, len;
      boolean bcd_valid = FALSE;
      /* Decode MSISDN, start by finding the start of binary MSISDN data */
      #define DMSI_MSISDN_ENCODED_DATA_OFFSET 14

      /* make sure the overall length is valid (greater than MSISDN start) */
      if (read_cnf->read_data.data_len < DMSI_MSISDN_ENCODED_DATA_OFFSET)
      {
        DS_MSG1(MSG_LEGACY_MED,"UIM read length invalid (%d)",
                        read_cnf->read_data.data_len);
        errval = QMI_ERR_INTERNAL_V01;
        goto send_result;
      }

      /* calculate the MSISDN start (offset is from the end of the EF) */
      offset = (uint8)(read_cnf->read_data.data_len - DMSI_MSISDN_ENCODED_DATA_OFFSET);

      /* Decode MCC1 from Byte 2 (bit 4-7), should always available */
      len = read_cnf->read_data.data_ptr[offset];
      if( len < DMSI_MSISDN_ENCODED_DATA_OFFSET )
      {
        bcd_valid = TRUE;

        /* Decode the rest from BCD to ASCII string, stop when out of range */
        for(i = 2; i <= len; i++)
        {
          current_byte = read_cnf->read_data.data_ptr[offset + i];
          if (QMI_BCD_LOW_DIGIT(current_byte) < 0x0A)
            uim_msisdn[(i - 2) * 2] = QMI_BCD_LOW_DIGIT(current_byte)+'0';
          else if (QMI_BCD_LOW_DIGIT(current_byte) == 0x0F)
            break;
          else
          {
            bcd_valid = FALSE;
            break;
          }

          if (QMI_BCD_HIGH_DIGIT(current_byte) < 0x0A)
            uim_msisdn[(i - 2) * 2 + 1] = QMI_BCD_HIGH_DIGIT(current_byte)+'0';
          else if (QMI_BCD_HIGH_DIGIT(current_byte) == 0x0F)
            break;
          else
          {
            bcd_valid = FALSE;
            break;
          }
        }
      }

      /* Only send MSISDN if it is valid*/
      if(bcd_valid != TRUE)
      {
        DS_MSG0(MSG_LEGACY_ERROR,"uim_msisdn BCD format invalid, clearing string");
        memset(uim_msisdn, 0, DMSI_MMGSDI_MSISDN_LEN * 2 + 1);
      }
      /* Reading Extension file if additional data is there */
      if(bcd_valid == TRUE && read_cnf->read_data.data_ptr[read_cnf->read_data.data_len-1] != 0xFF)
      {
        mmgsdi_access_type  sim_filename_ext;
        mmgsdi_rec_num_type  ext_rec_num;
        /* extension record number is present in the last byte*/
        ext_rec_num = read_cnf->read_data.data_ptr[read_cnf->read_data.data_len-1];

        sim_filename_ext.access_method = MMGSDI_EF_ENUM_ACCESS;
        sim_filename_ext.file.file_enum = MMGSDI_NO_FILE_ENUM;
    
        //if (qmi_if_info_ptr->session_info[session_index].app_type == MMGSDI_APP_USIM)			
        if(ds_mmgsdi_ptr->app_type == MMGSDI_APP_USIM)
        {
          sim_filename_ext.file.file_enum = MMGSDI_USIM_EXT5;
        }
        else
        {
          DS_MSG0(MSG_LEGACY_ERROR,"Extension is present in case of UIM only");
          goto send_result;
        }

        mmgsdi_status = mmgsdi_session_read_record(
                                   //qmi_if_info_ptr->session_info[session_index].session_id,
                                   ds_mmgsdi_ptr->session_id,
                           	       sim_filename_ext,
                                   ext_rec_num,
                                   0,
                                   qmi_if_mmgsdi_resp_cback,
                                   (uint32)async_cmd_buf);
       if (MMGSDI_SUCCESS == mmgsdi_status)
       {
         return;
       }
       else
       {
         /* For any reason MSISDN cannot be obtained from MMGSDI it will be
            skipped and the functon will go forward to process other items */
         DS_MSG1(MSG_LEGACY_ERROR,"MMGSDI read MSISDN EXT failed with status %d", mmgsdi_status);
         errval = QMI_ERR_INTERNAL_V01;        
       }
      }
    }
    else
    {
      errval = (qmi_error_type_v01)qmi_if_map_mmgsdi_status_to_errval(mmgsdi_status,__LINE__);
    }

    /* MSISDN is the last item, continue sending result */
    goto send_result;
  }

  if(read_cnf->access.file.file_enum == MMGSDI_USIM_EXT5)
  {
    /* Report MSISDN only when it is available through MMGSDI */
    if (MMGSDI_SUCCESS == mmgsdi_status) 
    {
      uint8 current_byte, j, len;
      boolean bcd_valid = FALSE;

      /* Length of Ext5 file  */
      #define DMSI_MSISDN_EXTN_LENGTH 13
      
      /* make sure the overall length is valid (greater than MSISDN start) */
      if (read_cnf->read_data.data_len < read_cnf->read_data.data_ptr[1])
      {
         DS_MSG1(MSG_LEGACY_MED,"UIM extension read length invalid (%d)",read_cnf->read_data.data_len);
         goto send_result;
      }
      /* In byte 2, length of additional data is present */
      len = read_cnf->read_data.data_ptr[1];

      #define MSISDN_EXT_ADDITIONAL_DATA_TYPE 0x02
      /* Data type should be additional data*/
      if(read_cnf->read_data.data_ptr[0] == MSISDN_EXT_ADDITIONAL_DATA_TYPE)
      {
         bcd_valid = TRUE;
         /* 10 bytes are already read from msisdn*/
         #define UIM_MSISDN_OFFSET 10

         /* Decode the rest from BCD to ASCII string, stop when out of range */
         for(j=0; j < len; j++)
         {
            current_byte = read_cnf->read_data.data_ptr[j+2];
            if (QMI_BCD_LOW_DIGIT(current_byte) < 0x0A)
              uim_msisdn[(j+UIM_MSISDN_OFFSET) * 2] = QMI_BCD_LOW_DIGIT(current_byte)+'0';
           else if (QMI_BCD_LOW_DIGIT(current_byte) == 0x0F)
              break;
           else
           {
              bcd_valid = FALSE;
              break;
           }

            if (QMI_BCD_HIGH_DIGIT(current_byte) < 0x0A)
              uim_msisdn[(j+UIM_MSISDN_OFFSET) * 2 + 1] = QMI_BCD_HIGH_DIGIT(current_byte)+'0';
            else if (QMI_BCD_HIGH_DIGIT(current_byte) == 0x0F)
               break;
            else
            {
              bcd_valid = FALSE;
              break;
            }
         }
         if(bcd_valid != TRUE)
          {
            DS_MSG0(MSG_LEGACY_ERROR,"uim_msisdn BCD format invalid, clearing string");
            memset(uim_msisdn, 0, DMSI_MMGSDI_MSISDN_LEN * 2 + 1);
          }
       }
     }
     else
     {
       DS_MSG1(MSG_LEGACY_ERROR,"MMGSDI couldn't read MSISDN EXT status %d", mmgsdi_status);
     }
   }

send_result:
  /* add the IMSI anytime it is present */
  memset(&resp_msg,0,sizeof(dms_get_msisdn_resp_msg_v01));
  if(strlen(uim_imsi))
  {
    DS_MSG1(MSG_LEGACY_MED,"Imsi valid len = %d",strlen(uim_imsi));
    strlcpy((char*)resp_msg.imsi,uim_imsi,sizeof(resp_msg.imsi));
    resp_msg.imsi_valid = TRUE;
  }

  /* Read MDN and MIN if this is primary subscription */
  if(dms_cl_sp->subscription_id == DMSI_BIND_SUBS_PRIMARY)
  {
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
    memset(mob_num, 0, sizeof(mob_num));
    memset(min_num, 0, sizeof(min_num));
  
    errval = qmi_dmsi_get_mdn_min(mob_num, sizeof(mob_num),
                                  min_num, sizeof(min_num));
    /* add the MIN anytime it is present */
    if (errval == QMI_ERR_NONE_V01)
    {
      if (strlen(min_num))
      {
        resp_msg.mobile_id_number_valid = TRUE;
        strlcpy((char*)resp_msg.mobile_id_number,min_num,
                 sizeof(resp_msg.mobile_id_number));
      }
      strlcpy((char*)resp_msg.voice_number,mob_num,sizeof(resp_msg.voice_number));
    }

    //Getting MDN and MIN Failed, include MSISDN in the response.
    if(errval != QMI_ERR_NONE_V01)
    {
      output_msisdn = TRUE;
      errval = QMI_ERR_NONE_V01;
    }
#else
    /* Return MSISDN as CDMA feature is disabled in primary subscription */
    output_msisdn = TRUE;
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */
  }
  else /* Return only the MSISDN read as this is secondary subscription */
  {
     output_msisdn = TRUE;
  }
 
  if(output_msisdn) 
  {
    /* MSISDN */
    if(strlen(uim_msisdn))
    {
      strlcpy((char*)resp_msg.voice_number,uim_msisdn,
               sizeof(resp_msg.voice_number));
      DS_MSG1(MSG_LEGACY_MED,"Imsi valid len = %d",strlen(uim_msisdn));
    }
    else if (errval == QMI_ERR_NONE_V01)
    {
      /* No records of MSISDN */
      errval = QMI_ERR_NOT_PROVISIONED_V01;
    }
  }
  DS_MSG0(MSG_LEGACY_MED,"sending response for MSISDN get");
  /*clear the reponse and send only error incase of failure*/
  if(errval != QMI_ERR_NONE_V01)
  {
    result = QMI_RESULT_FAILURE_V01;
    memset(&resp_msg,0,sizeof(resp_msg));
  }
  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  if(qmi_dms_check_valid_async_cmd_buf(async_cmd_buf))
  {
    qmi_dmsi_send_response(async_cmd_buf->req_handle, 
                           async_cmd_buf->msg_id,
                           &resp_msg,  	
                           sizeof(dms_get_msisdn_resp_msg_v01));
  }
  DS_SYSTEM_HEAP_MEM_FREE(async_cmd_buf);
} /* qmi_dmsi_generate_read_uim_msisdn_resp */

#endif /* FEATURE_MMGSDI_SESSION_LIB */

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
#if (defined FEATURE_OTASP)
/*===========================================================================
  FUNCTION QMI_DMSI_OTASP_FORCED_CALL_RELEASE()

  DESCRIPTION
    End all calls to allow power down regostration through after OTASP session

  PARAMETERS
   None
    
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dmsi_otasp_forced_call_release()
{
  cm_end_params_s_type end_params;

  if ( qmi_dmsi_global.voice_call_id != CM_CALL_ID_INVALID )
  {
     end_params.call_id = (cm_call_id_type) qmi_dmsi_global.voice_call_id;
     end_params.info_type = CM_CALL_MODE_INFO_CDMA;
     end_params.end_params.cdma_end.end_reason_included = FALSE;
     end_params.end_params.cdma_end.end_reason = CAI_REL_NORMAL;
     DS_MSG1(MSG_LEGACY_MED,"QMI: Adding call_id %d to the end list",
                      end_params.call_id);

     /*ds_qmi_cflog_cm_call_cmd_end( end_params.call_id, 
                                   end_params.info_type, 
                                   "OTASP forced release" );*/

     (void) cm_mm_call_cmd_end( NULL, NULL, (cm_client_id_type) -1,
                              1, &end_params );
  }
  else
  {
      DS_MSG0(MSG_LEGACY_MED,"QMI: Could not locate call_id ");
  }
} /* qmi_dmsi_otasp_forced_call_release */
#endif /* (defined FEATURE_OTASP) */
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */

/*===========================================================================
FUNCTION  QMI_DMS_CM_CALL_CMD_CB

DESCRIPTION
  CM call command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
#if 0
//Used in the cm callback qmi_dmsi_activate_automatic
void qmi_dms_cm_call_cmd_cb 
(
  void                         *data_ptr,         /* Data block pointer    */
  cm_call_cmd_e_type           call_cmd,          /* Command ID            */
  cm_call_cmd_err_e_type       call_cmd_err       /* Command error code    */
)
{
   qmi_dmsi_cmd_buf_type *  cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Post a cmd to DCC to handle this in DCC context
  -------------------------------------------------------------------------*/

  switch(call_cmd)
  {
    case CM_CALL_CMD_ORIG:
      cmd_ptr = (qmi_dmsi_cmd_buf_type *) qmi_dmsi_get_cmd_buf(QMI_CMD_DMS_CM_CALL_CMD_STATUS);
      if( cmd_ptr != NULL)
      {
        cmd_ptr->cmd_id = QMI_CMD_DMS_CM_CALL_CMD_STATUS;
        cmd_ptr->data.cm_call_err.user_data = data_ptr;
        cmd_ptr->data.cm_call_err.call_cmd  = call_cmd;
        cmd_ptr->data.cm_call_err.err_type  = call_cmd_err;
        dcc_send_cmd_ex(DCC_DMS_SVC_CMD, cmd_ptr);
      }
      break;

    default:
      DS_MSG0(MSG_LEGACY_MED,"Unexpected ph_cmd received in qmi_dmsi_cm_ph_cb");
  }

  return;
  
} /* qmi_dms_cm_call_cmd_cb */



/*===========================================================================
  FUNCTION QMI_DMS_PROCESS_CM_CALL_ERR()

  DESCRIPTION
    Process the CM call event

  PARAMETERS
    user_data :  user data ptr passed to CM
    call_cmd  :  requested CM call command
    err_type  :  result of request
    
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dms_process_cm_call_err
(
  void *                 user_data,
  cm_call_cmd_e_type     call_cmd,
  cm_call_cmd_err_e_type err_type
)
{
  ds_qmi_dms_async_cmd_type *     cmd_buf_p;
  qmi_error_type_v01              errval = QMI_ERR_NONE_V01;
  dms_activate_automatic_resp_msg_v01 resp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd_buf_p = (ds_qmi_dms_async_cmd_type*) user_data;
  if(cmd_buf_p == NULL)
  {
    DS_MSG0(MSG_LEGACY_MED,"Null cmd_buf ptr");
    return;
  }
  memset(&resp_msg,0,sizeof(dms_activate_automatic_resp_msg_v01));
  /* map the CM request error to the proper QMI error value */
  switch (err_type)
  {
    case CM_CALL_CMD_ERR_NOERR:
      errval = QMI_ERR_NONE_V01;
      break;

    case CM_CALL_CMD_ERR_OFFLINE_S:
      errval = QMI_ERR_NO_NETWORK_FOUND_V01;
      break;

    case CM_CALL_CMD_ERR_IN_USE_S:
      errval = QMI_ERR_DEVICE_IN_USE_V01;
      break;

    default:
      errval = QMI_ERR_INTERNAL_V01;
      break;
  }
  resp_msg.resp.error = errval;
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01)?QMI_RESULT_SUCCESS_V01:QMI_RESULT_FAILURE_V01;
  if(qmi_dms_check_valid_async_cmd_buf(cmd_buf_p))
  {
    qmi_dmsi_send_response(cmd_buf_p->req_handle,
                           cmd_buf_p->msg_id,
                           &resp_msg, 
                           sizeof(dms_activate_automatic_resp_msg_v01));
  }
  DS_SYSTEM_HEAP_MEM_FREE(cmd_buf_p);
} /* qmi_dms_process_cm_call_err() */
#endif
#if 0
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
/*===========================================================================
  FUNCTION QMI_DMSI_GET_MDN_MIN()

  DESCRIPTION
    a
    
  PARAMETERS
    None

  RETURN VALUE
    None
        
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_dmsi_get_mdn_min
(
  char *mob_num,
  int   mdn_size,
  char *min_num,
  int   min_size
)
{
  qmi_error_type_v01      errval = QMI_ERR_NONE_V01;
  
  dword                   min1;
  word                    min2;
  int                     min_len;

  nv_stat_enum_type       nv_status;
  nv_item_type           *ds_nv_item_ptr;

  cm_nam_e_type           curr_nam;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(mob_num && min_num);

  min1 = min2 = min_len = 0;

  memset(mob_num, 0, mdn_size);
  memset(min_num, 0, min_size);

#if defined(FEATURE_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE)
#ifdef FEATURE_MMGSDI_SESSION_LIB
  /*-------------------------------------------------------------------------
    Check the RUIM state before querying MDN/MIN
  -------------------------------------------------------------------------*/
  if ((NV_RTRE_CONTROL_USE_RUIM == nv_rtre_control()) &&
      ((errval = (qmi_error_type_v01)
       qmi_dms_get_uim_access_status_ext(MMGSDI_1X_PROV_PRI_SESSION)) != 
       QMI_ERR_NONE_V01))
  {
    DS_MSG1(MSG_LEGACY_MED,"RUIM not initialized (%d)", errval);
  }
#else /* FEATURE_MMGSDI_SESSION_LIB */
    errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
#endif /* FEATURE_MMGSDI_SESSION_LIB */
#endif /* defined(FEATURE_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE) */

  if (QMI_ERR_NONE_V01 == errval)
  {
    /*-------------------------------------------------------------------------
      Allocate temporary memory for the NV item
    -------------------------------------------------------------------------*/
    DS_SYSTEM_HEAP_MEM_ALLOC(ds_nv_item_ptr, sizeof(nv_item_type),
                           nv_item_type*);
    if( ds_nv_item_ptr == NULL )
    {
      return QMI_ERR_NO_MEMORY_V01;
    }
    
    nv_status = ds_get_nv_item(NV_CURR_NAM_I, ds_nv_item_ptr);  
    if(nv_status != NV_DONE_S || ds_nv_item_ptr->curr_nam >= NV_MAX_NAMS)
    {
      /*-----------------------------------------------------------------------
        Default to the first NAM
      -----------------------------------------------------------------------*/
      ds_nv_item_ptr->curr_nam = (byte) CM_NAM_1;
    }
    curr_nam = (cm_nam_e_type)ds_nv_item_ptr->curr_nam;
    ds_nv_item_ptr->mob_dir_number.nam = ds_nv_item_ptr->curr_nam;

    /*-------------------------------------------------------------------------
      Get the Mobile Directory Number from NV
    -------------------------------------------------------------------------*/
    nv_status = ds_get_nv_item(NV_DIR_NUMBER_PCS_I, ds_nv_item_ptr);
    if (NV_DONE_S == nv_status)
    {
      /*-----------------------------------------------------------------------
        Convert the mobile number to text format
      -----------------------------------------------------------------------*/
      qmi_dmsi_mobnum_pcs_to_text(mob_num, &ds_nv_item_ptr->mob_dir_number);
      ds_nv_item_ptr->min1.nam = curr_nam;
      if (NV_DONE_S == ds_get_nv_item(NV_MIN1_I, ds_nv_item_ptr))
      {
        min1 = ds_nv_item_ptr->min1.min1[NV_CDMA_MIN_INDEX];
        if (NV_DONE_S == ds_get_nv_item(NV_MIN2_I, ds_nv_item_ptr))
        {
          min2 = ds_nv_item_ptr->min2.min2[NV_CDMA_MIN_INDEX];
          if (qmi_dmsi_minnum_validate(min1, min2))
          {
            min_len = qmi_dmsi_minnum_pcs_to_text(min_num, min1, min2);
            min_num[min_len] = '\0';
          }
        }
      }
    }
    else
    {
      errval = QMI_ERR_NOT_PROVISIONED_V01;  
    }

    /*-------------------------------------------------------------------------
      Free the temporary memory allocated for NV item.
    -------------------------------------------------------------------------*/
    DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);
  }
  
  return errval;
} /* qmi_dmsi_get_mdn_min() */


/*===========================================================================
  FUNCTION QMI_DMSI_GENERATE_DEFAULT_MIN()

  DESCRIPTION
    Generates the default MIN (IMSI_M_S1 / IMSI_T_S1) for the device, based on 
    the ESN, that is set when the UI defaults the provisioning.
    
  PARAMETERS
    esn : dword containing the ESN in binary

  RETURN VALUE
    dword MIN in encoded format returned
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dword qmi_dmsi_generate_default_min
(
  dword  esn
)
{
  word zero;     /* Encoding of three zero digits */
  word fourth;   /* Fourth from last decimal digit of the ESN */
  word last3;    /* Last three decimal digits of the ESN */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode digits as per JSTD-008 section 2.3.1.1 */
  zero = 1110 - 111;
  last3 = (esn % 1000);
  last3 += ((last3 / 100) == 0) ? 1000 : 0;
  last3 += (((last3 % 100) / 10) == 0) ? 100 : 0;
  last3 += ((last3 % 10) == 0) ? 10 : 0;
  last3 -= 111;
  fourth = ((esn % 10000) / 1000); /* In range 0-9 */
  if (fourth == 0)
  {
    fourth = 10;
  }

  /* Concatenate results and return 24 bit value for imsi_s1 */
  /* Example: esn = 120406
  **      imsi_s1 = 000  0  406
  **  encodes to -> 999  10 395
  **       in hex = 3e7  a  18b
  **    in binary = 1111100111 1010 0110001011
  */
  return (((dword) zero << 14) | (fourth << 10) | last3);
} /* qmi_dmsi_generate_default_min() */

/*===========================================================================
  FUNCTION QMI_DMSI_MINNUM_VALIDATE()

  DESCRIPTION
    validate MIN NV format to make sure no reserved areas are set and that the 
    MIN values are within the propper range
    
  PARAMETERS
    min1 : lower 24 bits of the MIN binary representation
    min2 : upper 10 bits of the MIN binary representation

  RETURN VALUE
    Boolean whether the specified MIN is valid according to the binary MIN 
    encoding format.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_dmsi_minnum_validate
(
  dword  min1,
  word   min2
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ( (   min1 & 0xFF000000 ) != 0 ) ||
       ( ( ( min1 & 0x00FFC000 ) >> 14 ) > 999 ) ||
       ( ( ( min1 & 0x00003C00 ) >> 10 ) > 10 ) ||
       ( ( ( min1 & 0x00003C00 ) >> 10 ) == 0 ) ||
       ( ( ( min1 & 0x000003FF )         > 999 ) ) 
     ||
       ( ( min2 & 0xFC00 ) != 0) || 
       ( ( min2 & 0x03FF)          > 999 )
     )
  {
    return FALSE;
  }
  
  return TRUE;
} /* qmi_dmsi_minnum_validate() */


/*===========================================================================
  FUNCTION QMI_DMSI_MINNUM_PCS_TO_TEXT()

  DESCRIPTION
    convert MIN NV format to ascii-text representation
    
  PARAMETERS
    dest : will contain the mobile number in ascii-text format
    min1 : lower 24 bits of the MIN binary representation
    min2 : upper 10 bits of the MIN binary representation

  RETURN VALUE
    length of ascii-text string that was created
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static int qmi_dmsi_minnum_pcs_to_text
(
  char *  dest, 
  dword  min1,
  word   min2
)
{
  byte * start;
  byte * end;
  dword   temp;
  boolean digit_one_zero;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(dest);

  start = end = (byte *) dest;

  /*-------------------------------------------------------------------------
    MIN binary format is as per JSTD-008 section 2.3.1.1, convert to BCD per
    spec
  -------------------------------------------------------------------------*/
  /* mask off and convert first three digits from MIN2 */
  temp = (min2 & 0x03FF);
  digit_one_zero = FALSE;
  /* add 100 and check for overflows from the 4th digit */
  temp += 100;
  if (temp > 999)
  {
    temp -= 1000;
    end[0] = (char) '0';
    end++;
    digit_one_zero = TRUE;
  }
  /* add 10 and check for overflows from the 5th digit */
  temp += 10;
  if ((temp % 10) == (temp % 100))
  {
    temp -= 100;
    if (digit_one_zero)
    {
    end[0] = (char) '0';
    end++;
    }
  }
  /* add 1 and check for overflows from the 6th digit */
  temp += 1;
  if (0 == (temp % 10))
  {
    temp -= 10;
  }
  end = dsatutil_itoa( temp, end, DMSI_BASE_DECIMAL);

  /* mask off and convert next three digits from MIN1 */
  temp = (min1 & 0x00FFC000) >> 14;
  digit_one_zero = FALSE;
  /* add 100 and check for overflows from the 4th digit */
  temp += 100;
  if (temp > 999)
  {
    temp -= 1000;
    end[0] = (char) '0';
    end++;
    digit_one_zero = TRUE;
  }
  /* add 10 and check for overflows from the 5th digit */
  temp += 10;
  if ((temp % 10) == (temp % 100))
  {
    temp -= 100;
    if (digit_one_zero)
    {
    end[0] = (char) '0';
    end++;
    }
  }
  /* add 1 and check for overflows from the 6th digit */
  temp += 1;
  if (0 == (temp % 10))
  {
    temp -= 10;
  }
  end = dsatutil_itoa( temp, end, DMSI_BASE_DECIMAL);

  /* mask off and convert next digit from MIN1 */
  temp = (min1 & 0x00003C00) >> 10;
  /* check for overflows from the 7th digit */
  if (temp == 10)
  {
    temp = 0;
  }
  end = dsatutil_itoa( temp, end, DMSI_BASE_DECIMAL);

  /* mask off and convert last three digits from MIN1 */
  temp = (min1 & 0x000003FF);
  digit_one_zero = FALSE;
  /* add 100 and check for overflows from the 4th digit */
  temp += 100;
  if (temp > 999)
  {
    temp -= 1000;
    end[0] = (char) '0';
    end++;
    digit_one_zero = TRUE;
  }
  /* add 10 and check for overflows from the 5th digit */
  temp += 10;
  if ((temp % 10) == (temp % 100))
  {
    temp -= 100;
    if (digit_one_zero)
    {
    end[0] = (char) '0';
    end++;
    }
  }
  /* add 1 and check for overflows from the 6th digit */
  temp += 1;
  if (0 == (temp % 10))
  {
    temp -= 10;
  }
  end = dsatutil_itoa( temp, end, DMSI_BASE_DECIMAL);

  /* return the length (end - start) given in number of bytes */
  return ((end - start) / sizeof(byte));
} /* qmi_dmsi_minnum_pcs_to_text() */

/*===========================================================================
  FUNCTION QMI_DMSI_MOBNUM_PCS_TO_TEXT()

  DESCRIPTION
    convert MDN NV format to ascii-text representation
    
  PARAMETERS
    dest : will contain the mobile number in ascii-text format
    src  : mobile number in NV format

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean  qmi_dmsi_mobnum_text_to_pcs
(
  char *                  src,
  uint8                   len,
  nv_mob_dir_data_type *  dest
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(dest && src);

  if ((len == 0) || (len > NV_DIR_NUMB_PCS_SIZ))
  {
  	DS_MSG0(MSG_LEGACY_MED,"Invalid MDN length");
  	return FALSE;
  }
  else
  {
    dest->n_digits = len;
  }

  /*-------------------------------------------------------------------------
    MDN NV format is sort of like BCD, but 8 bits instead of 4, and 
    '0' is represented as 0x0A.
  -------------------------------------------------------------------------*/
  for (i=0; i < len; i++)
  {
    if ('0' == src[i]) 
    {
      dest->digitn[i] = 0x0A;
    } 
    else if ( (src[i] >= '1') || (src[i]  <= '9'))
    {
      dest->digitn[i] = (byte) src[i] - '0';
    } 
    else 
    {
      return FALSE; /* invalid num */
    }
  }

  return TRUE;
} /* qmi_dmsi_mobnum_text_to_pcs() */

/*===========================================================================
  FUNCTION QMI_DMSI_MIN_TEXT_TO_MIN12()

  DESCRIPTION
    convert MDN NV format to ascii-text representation
    
  PARAMETERS
    min_data : (IN)  poitner to text-format MIN
    len      : (IN)  length of text-format MIN
    min1_ptr : (OUT) pointer to binary-format MIN1
    min2_ptr : (OUT) pointer to binary-format MIN2

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean  qmi_dmsi_min_text_to_min12
(
  char *          min_data,
  uint8           len,
  dword *				  min1_ptr, 
  word *				  min2_ptr
)
{
  int       i;
  uint16    digit;
  char *    min_text;
  dword		  min1;
  word 		  min2;

  if ((NULL == min_data) || (NULL == min1_ptr) || (NULL == min2_ptr))
  {
    DS_MSG0(MSG_LEGACY_ERROR,"NULL ptr passed");
    return FALSE;
  }

  min_text = min_data;  
  /* Validate MIN length and content */
  if(len != DMSI_MIN_SIZ)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Invalid MIN length");
    return FALSE;
  }
  else
  {
    for (i=0; i<len; i++ ) 
    {
      if ( (*(min_text+i) <  '0') || (*(min_text+i) > '9') ) 
        return FALSE;
    }
  }

  // Encode the first three digits (IS-95A 6.3.1.1)
  min2 = 0;
  for (i = 0; i < 3; i++) 
  {
    digit = *min_text - '0';
    if (0 == digit) 
      digit = 10;
    min_text++;
    min2 = (uint16) (min2 * 10) + digit;
  }

  min2 -= 111;

  // Encode the last seven digits (IS-95A 6.3.1.1)

  // Encode the second three digits into the ten most
  // significant bits (of the 24-bit number)...
  min1 = 0;
  for (i = 0; i < 3; i++) 
  {
    digit = *min_text - (uint16) '0';
    if (0 == digit) 
      digit = 10;
    min_text++;
    min1 = (uint32) (min1 * 10) + digit;
  }

  min1 -= 111;

  min1 <<= 14;
  min1 &= 0x00FFC000;

  // The fourth last digit is mapping as BCD into four bits
  digit = *min_text - '0';
  if (0 == digit) 
    digit = 10;
  min_text++;

  min1 = min1 | (0x00003C00 & (digit << 10));

  // Encode the last three digits into the ten least significant bits
  {
    uint32 tmp = 0;
    for (i = 0; i < 3; i++) 
    {
      digit = *min_text - '0';
      if (0 == digit) 
        digit = 10;
      min_text++;

      tmp = (uint32) (tmp * 10) + digit;
    }

    tmp -= 111;
    tmp &= 0x000003FF;

    min1 |= tmp;
  }

  // Update the min1_ptr and min2_ptr
  *min1_ptr = min1;
  *min2_ptr = min2;
  return TRUE;
} /* qmi_dmsi_min_text_to_min12() */

#ifdef FEATURE_DATA_QMI_DMS_FACTORY
/*===========================================================================
  FUNCTION QMI_DMSI_RESET_IMSI_COMPONENTS()

  DESCRIPTION
    Resets the IMSI components for restore factory operation
    
  PARAMETERS
    nam

  RETURN VALUE
    TRUE/FALSE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean  qmi_dmsi_reset_imsi_components(byte nam)
{
  nv_item_type      *ds_nv_item_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(ds_nv_item_ptr, sizeof(nv_item_type),
                           nv_item_type*);
  if( ds_nv_item_ptr == NULL )
  {
    return FALSE;
  }
  
  /* NV_IMSI_MCC_I*/
  memset(ds_nv_item_ptr, 0, sizeof(nv_item_type));
  ds_nv_item_ptr->imsi_mcc.nam = nam;
  ds_nv_item_ptr->imsi_mcc.imsi_mcc = DMSI_IMSI_MCC_0;      
  if( NV_DONE_S != ds_put_nv_item( NV_IMSI_MCC_I, ds_nv_item_ptr ) )
  {
    DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);;
    return FALSE;
  }

  /* NV_IMSI_11_12_I */
  /* Default MNC is 00 in ASCII and 99 in min */
  memset(ds_nv_item_ptr, 0, sizeof(nv_item_type));
  ds_nv_item_ptr->imsi_11_12.nam = nam;
  ds_nv_item_ptr->imsi_11_12.imsi_11_12 = DMSI_IMSI_11_12_0;  /* default to '00' */
  if( NV_DONE_S != ds_put_nv_item( NV_IMSI_11_12_I, ds_nv_item_ptr ) )
  {
    DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);;
    return FALSE;
  }

  /* NV_MIN2_I */
  memset(ds_nv_item_ptr, 0, sizeof(nv_item_type));
  ds_nv_item_ptr->min2.nam = nam;
  ds_nv_item_ptr->min2.min2[DMSI_FMMIN] = DMSI_IMSI_S2_0 ;
  ds_nv_item_ptr->min2.min2[DMSI_CDMAMIN] = DMSI_IMSI_S2_0 ;
  if( NV_DONE_S != ds_put_nv_item( NV_MIN2_I, ds_nv_item_ptr ) )
  {
    DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);;
    return FALSE;
  }

  /* NV_MIN1_I */
  memset(ds_nv_item_ptr, 0, sizeof(nv_item_type));
  ds_nv_item_ptr->min1.nam = nam;
  ds_nv_item_ptr->min1.min1[DMSI_FMMIN] = DMSI_IMSI_S1_0 ;
  ds_nv_item_ptr->min1.min1[DMSI_CDMAMIN] = DMSI_IMSI_S1_0;
  if( NV_DONE_S != ds_put_nv_item( NV_MIN1_I, ds_nv_item_ptr ) )
  {
    DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);;
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Free the temporary memory allocated for NV item.
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);;
  
  /* All IMSI components reset as required here, return true */
  return TRUE;

}/* qmi_dmsi_reset_imsi_components() */

#ifdef FEATURE_IS95B_TRUE_IMSI
/*===========================================================================
  FUNCTION QMI_DMSI_RESET_IMSI_T_COMPONENTS()

  DESCRIPTION
    Resets the IMSI_T components for restore factory operation
    
  PARAMETERS
    nam

  RETURN VALUE
    TRUE/FALSE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean  qmi_dmsi_reset_imsi_t_components(byte nam)
{
  nv_item_type      *ds_nv_item_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(ds_nv_item_ptr, sizeof(nv_item_type),
                           nv_item_type*);
  if( ds_nv_item_ptr == NULL )
  {
    return FALSE;
  }
  
  /* NV_IMSI_T_MCC_I */
  memset(ds_nv_item_ptr, 0, sizeof(nv_item_type));
  /* Default MCC is 000 in ASCII and 999 in min */
  ds_nv_item_ptr->imsi_t_mcc.nam = nam;
  ds_nv_item_ptr->imsi_t_mcc.imsi_mcc = DMSI_IMSI_MCC_0;      /* default to '000' */
  if( NV_DONE_S != ds_put_nv_item( NV_IMSI_T_MCC_I, ds_nv_item_ptr ) )
  {
    DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);;
    return FALSE;
  }

  /* NV_IMSI_T_11_12_I */
  memset(ds_nv_item_ptr, 0, sizeof(nv_item_type));
  /* Default MNC is 00 in ASCII and 99 in min */
  ds_nv_item_ptr->imsi_t_11_12.nam = nam;
  ds_nv_item_ptr->imsi_t_11_12.imsi_11_12 = DMSI_IMSI_11_12_0;  /* default to '00' */
  if( NV_DONE_S != ds_put_nv_item( NV_IMSI_T_11_12_I, ds_nv_item_ptr ) )
  {
    DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);;
    return FALSE;
  }

  /* NV_IMSI_T_S2_I */
  memset(ds_nv_item_ptr, 0, sizeof(nv_item_type));
  /* Default S2 is 000 in ASCII and 999 in min */
  ds_nv_item_ptr->imsi_t_s2.nam = nam;
  ds_nv_item_ptr->imsi_t_s2.min2[DMSI_FMMIN] = DMSI_IMSI_S2_0 ;
  ds_nv_item_ptr->imsi_t_s2.min2[DMSI_CDMAMIN] = DMSI_IMSI_S2_0 ;
  if( NV_DONE_S != ds_put_nv_item( NV_IMSI_T_S2_I, ds_nv_item_ptr ) )
  {
    DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);;
    return FALSE;
  }

  /* NV_IMSI_T_S1_I */
  memset(ds_nv_item_ptr, 0, sizeof(nv_item_type));
  ds_nv_item_ptr->imsi_t_s1.nam = nam;
  ds_nv_item_ptr->imsi_t_s1.min1[DMSI_FMMIN] = DMSI_IMSI_S1_0 ;
  ds_nv_item_ptr->imsi_t_s1.min1[DMSI_CDMAMIN] =  DMSI_IMSI_S1_0 ;
  if( NV_DONE_S != ds_put_nv_item( NV_IMSI_T_S1_I, ds_nv_item_ptr ) )
  {
    DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);;
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Free the temporary memory allocated for NV item.
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);;

  /* All IMSI_T components reset as required here, return true */
  return TRUE;

}/* qmi_dmsi_reset_imsi_t_components() */
#endif /* def FEATURE_IS95B_TRUE_IMSI */
#endif /* FEATURE_DATA_QMI_DMS_FACTORY */


/*===========================================================================
  FUNCTION QMI_DMSI_CALCULATE_ACTIVATED_STATE()

  DESCRIPTION
    convert MDN NV format to ascii-text representation
    
  PARAMETERS
    dest : will contain the mobile number in ascii-text format
    src  : mobile number in NV format

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dms_activation_state_enum_v01 qmi_dmsi_calculate_activated_state
(
  void
)
{
  dms_activation_state_enum_v01 act_state;
  
  dword                            min1;
  dword                            min1_default;
  boolean                          otasp_state;
  nv_item_type                    *ds_nv_item_ptr;
  nv_stat_enum_type                nv_status;
  cm_nam_e_type                    curr_nam;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  act_state = DMS_ACTIVATION_STATE_ENUM_MAX_ENUM_VAL_V01;
  
  otasp_state = FALSE;
  min1 = min1_default = 0;

  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(ds_nv_item_ptr, sizeof(nv_item_type),
                           nv_item_type*);
  if( ds_nv_item_ptr == NULL )
  {
    return act_state;
  }

  /* see if build stores 'last' OTASP state to QMI NV item */
  if (qmi_dmsi_global.nv_use_last_otasp_act_state)
  {
    /* start with NOT_ACTIVATED in case 'last' NV not set */
    act_state = DMS_ACTIVATION_NOT_ACTIVATED_V01;
    if (QMI_NV_STATUS_OK == qmi_nv_read(QMI_NV_ITEM_LAST_OTASP_STATE, 0, 0, 
                                        &otasp_state, 
                                        sizeof(otasp_state)))
    {
      /* use 'last' NV only if set and non-zero */
      if (otasp_state)
      {
        act_state = DMS_ACTIVATION_ACTIVATED_V01;
      }
    }
  }

  /* if act_state not set, use the ESN/MIN to determine */
  if (DMS_ACTIVATION_STATE_ENUM_MAX_ENUM_VAL_V01 == act_state)
  {
    act_state = DMS_ACTIVATION_NOT_ACTIVATED_V01;
    if(NV_DONE_S == ds_get_nv_item (NV_ESN_I, ds_nv_item_ptr))
    {
      min1_default = qmi_dmsi_generate_default_min(ds_nv_item_ptr->esn.esn);
    }
  
    nv_status = ds_get_nv_item(NV_CURR_NAM_I, ds_nv_item_ptr);  
    if(nv_status != NV_DONE_S || ds_nv_item_ptr->curr_nam >= NV_MAX_NAMS)
    {
      /*-----------------------------------------------------------------------
        Default to the first NAM
      -----------------------------------------------------------------------*/
      ds_nv_item_ptr->curr_nam = (byte) CM_NAM_1;
    }
    curr_nam = (cm_nam_e_type)ds_nv_item_ptr->curr_nam;
    ds_nv_item_ptr->min1.nam = curr_nam;

    if (NV_DONE_S == ds_get_nv_item(NV_MIN1_I, ds_nv_item_ptr))
    {
      min1 = ds_nv_item_ptr->min1.min1[NV_CDMA_MIN_INDEX];
      if (qmi_dmsi_minnum_validate(min1, 0) && (min1 != min1_default))
      {
        act_state = DMS_ACTIVATION_ACTIVATED_V01;
      }
      else
      {
#ifdef FEATURE_IS95B_TRUE_IMSI
        ds_nv_item_ptr->imsi_t_s1.nam = curr_nam;
        if (NV_DONE_S == ds_get_nv_item(NV_IMSI_T_S1_I, ds_nv_item_ptr))
        {
          min1 = ds_nv_item_ptr->imsi_t_s1.min1[NV_CDMA_MIN_INDEX];
          if (qmi_dmsi_minnum_validate(min1, 0) && (min1 != min1_default))
          {
            act_state = DMS_ACTIVATION_ACTIVATED_V01;
          }
        }
#endif /* FEATURE_IS95B_TRUE_IMSI */
      }
    }
  }

  DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);

  return act_state;
} /* qmi_dmsi_calculate_activated_state */
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */
#endif


/*===========================================================================
  FUNCTION QMI_DMSI_OPRT_MODE_FROM_CM()

  DESCRIPTION
    Maps a CM operating mode enum to a QMI DMS operating mode enum.
    
  PARAMETERS
    cm_oprt_mode  [in]
    dms_oprt_mode [out]

  RETURN VALUE
    QMI_ERR_NONE_V01 on success, QMI_ERR_INTERNAL_V01 if CM operating mode is
    unrecognized.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_dmsi_oprt_mode_from_cm
(
  sys_oprt_mode_e_type        cm_oprt_mode,
  dms_operating_mode_enum_v01  *dms_oprt_mode
)
{
  qmi_error_type_v01   errval = QMI_ERR_NONE_V01;
  nv_stat_enum_type  nv_status;
  nv_item_type      *ds_nv_item_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(ds_nv_item_ptr, sizeof(nv_item_type),
                           nv_item_type*);
  if( ds_nv_item_ptr == NULL )
  {
    return QMI_ERR_NO_MEMORY_V01;
  }

  switch (cm_oprt_mode)
  {
    case SYS_OPRT_MODE_NONE:
      errval = QMI_ERR_INFO_UNAVAILABLE_V01;
      break;

    case SYS_OPRT_MODE_PWROFF:
      *dms_oprt_mode = DMS_OP_MODE_SHUTTING_DOWN_V01;
      break;
  
    case SYS_OPRT_MODE_FTM:
      *dms_oprt_mode = DMS_OP_MODE_FACTORY_TEST_MODE_V01;
      break;
  
    case SYS_OPRT_MODE_OFFLINE:
    case SYS_OPRT_MODE_OFFLINE_AMPS:
    case SYS_OPRT_MODE_OFFLINE_CDMA:
      *dms_oprt_mode = DMS_OP_MODE_OFFLINE_V01;
      break;

    case SYS_OPRT_MODE_ONLINE:
      *dms_oprt_mode = DMS_OP_MODE_ONLINE_V01;
      break;
  
    case SYS_OPRT_MODE_LPM:
      *dms_oprt_mode = DMS_OP_MODE_LOW_POWER_V01;
      break;

    case SYS_OPRT_MODE_RESET:
      *dms_oprt_mode = DMS_OP_MODE_RESETTING_V01;
      break;

    case SYS_OPRT_MODE_NET_TEST_GW:
      *dms_oprt_mode = DMS_OP_MODE_NET_TEST_GW_V01;
      break;

#ifdef FEATURE_FMD_SPEED_INFO 
    case SYS_OPRT_MODE_PSEUDO_ONLINE:
        *dms_oprt_mode = DMS_OP_MODE_CAMP_ONLY_V01;	   
	break;
#endif

    default:
      DS_MSG1(MSG_LEGACY_MED,"Operating mode unknown (%d), can't convert to QMI DMS oprt mode",
                      cm_oprt_mode);
      errval = QMI_ERR_INTERNAL_V01;
      break;
  }

  /*-------------------------------------------------------------------------
    Check if the device is actually in P-LPM instead of LPM
  -------------------------------------------------------------------------*/
  if (DMS_OP_MODE_LOW_POWER_V01 == *dms_oprt_mode)
  {
    /* Read the nv for LPM persistence flag */
    nv_status = ds_get_nv_item(NV_LPM_PERSISTENCE_I, ds_nv_item_ptr);
    if(NV_DONE_S == nv_status)
    {
      /* P-LPM flag set, read the value */
      if (ds_nv_item_ptr->lpm_persistence)
      {
        *dms_oprt_mode = DMS_OP_MODE_PERSISTENT_LOW_POWER_V01;
      }
    }
  } 

  DS_SYSTEM_HEAP_MEM_FREE(ds_nv_item_ptr);
  return errval;
} /* qmi_dmsi_oprt_mode_from_cm() */

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
  FUNCTION QMI_DMSI_GET_UIM_STATUS()

  DESCRIPTION
    Returns the status of the UIM based on the values cached from earlier
    MMGSDI events.

  PARAMETERS
    None

  RETURN VALUE
    QMI_ERR_NONE_V01: UIM present and unlocked
    QMI_ERR_SIM_NOT_INITIALIZED: If the SIM is not initialized yet
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_dmsi_get_uim_status
(
  void
)
{
  qmi_if_info_type  *qmi_if_info_ptr;

  qmi_if_info_ptr = qmi_if_info_get_ptr();
  DS_MSG2(MSG_LEGACY_MED,"qmi_dmsi_get_uim_status(): app_type=0x%x, uim_state=0x%x",
                  qmi_if_info_ptr->session_info[QMI_IF_MMGSD_GW_PROV_PRI_SESSION].app_type,
                  qmi_dmsi_global.uim_state);

  switch (qmi_dmsi_global.uim_state)
  {
    case DMS_UIM_INITIALIZATION_COMPLETED_V01: // i.e. subscription ready ev occured
      return QMI_ERR_NONE;

    case DMS_UIM_NOT_PRESENT_V01: // i.e. sim not present
    case DMS_UIM_STATE_UNAVAILABLE_V01: // i.e. no mmgsdi event yet
      return QMI_ERR_SIM_NOT_INITIALIZED;

    default:
      return QMI_ERR_NONE;
  }
} /* qmi_dmsi_get_uim_status */

/*===========================================================================
  FUNCTION QMI_DMSI_GET_UIM_STATUS_EXT()

  DESCRIPTION
    Returns the status of the UIM based on the values fetched from mmgsdi module.

  PARAMETERS
    None

  RETURN VALUE
    QMI_ERR_NONE_V01: UIM present and unlocked
    QMI_ERR_SIM_NOT_INITIALIZED: If the SIM is not initialized yet
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_dms_get_uim_access_status_ext
(
   uint8 session_type
)
{
  mmgsdi_session_info_query_type    session_info_query;
  mmgsdi_session_get_info_type      session_get_info_ptr;
  mmgsdi_return_enum_type  mmgsdi_status;
  session_info_query.query_type = MMGSDI_SESSION_INFO_QUERY_BY_TYPE;
  session_info_query.query_key.session_type = (mmgsdi_session_type_enum_type)session_type;
  
  mmgsdi_status = mmgsdi_session_get_info_sync(session_info_query, &session_get_info_ptr);
  
  switch(mmgsdi_status)
  {
    case MMGSDI_SUCCESS:
      switch(session_get_info_ptr.app_state)
      {
        case MMGSDI_APP_STATE_READY:
          return QMI_ERR_NONE;
        default:
          return QMI_ERR_SIM_NOT_INITIALIZED;
      }
    default:
      return QMI_ERR_SIM_NOT_INITIALIZED;
  }      
}

/*===========================================================================
  FUNCTION QMI_DMS_GET_UIM_ACCESS_STATUS()

  DESCRIPTION
    Returns the status of the UIM for read/write of PIN1 protected values based
    on the values cached from earlier MMGSDI events.  

  PARAMETERS
    None

  RETURN VALUE
    QMI_ERR_NONE_V01: UIM present and unlocked
    QMI_ERR_SIM_NOT_INITIALIZED: If the UIM is not initialized yet
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_dms_get_uim_access_status
(
  void
)
{
  qmi_error_e_type errval = QMI_ERR_INTERNAL;

  errval = qmi_dmsi_get_uim_status();
  if (QMI_ERR_NONE == errval)
  {
      if (DMS_UIM_INITIALIZATION_FAILED_V01 == qmi_dmsi_global.uim_state)
      {
        /*UIM not ready to read/write*/
        errval = QMI_ERR_SIM_NOT_INITIALIZED;
      }
  }

  return errval;
} /* qmi_dms_get_uim_access_status */

#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*===========================================================================
  FUNCTION qmi_dmsi_policyman_config_init()

  DESCRIPTION  Init function to register to the MESSAGE ROUTER for policyman 
               indication
   
  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    
  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dmsi_policyman_config_init (void)
{
   /*Register to the message router for policyman indicaiton*/
   dsmsgrrcv_reg_hdlr(POLICYMAN_CFG_UPDATE_MSIM_IND,
                      (dsmsgrcv_msg_hdlr_f) qmi_dmsi_msgr_hdlr );
}

/*===========================================================================
  FUNCTION qmi_dmsi_msgr_hdlr()

  DESCRIPTION  call back registered for message router
   
  PARAMETERS
     msgrtype  - message id requested
     dsmsg     - changed data

  RETURN VALUE
    Boolean    - true, if message is handled correctly
               - false, if not

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_dmsi_msgr_hdlr 
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg
)
{
  qmi_dmsi_cmd_buf_type                *cmd_payload = NULL;
  ds_command_type                      *cmd_ptr =NULL;
  boolean result                       = FALSE;  
  cm_psm_get_cfg_params_rsp_s_type     *cm_psm_get_cfg_params_resp;
  cm_psm_status_ind_s_type             *cm_psm_status_ind;
  cm_mm_psm_cfg_change_ind_s_type      *cm_psm_cfg_change_ind;

#ifdef FEATURE_REF_TRACKER
  cm_qmi_sensor_event_ind_s_type       *cm_sensor_policy_met_ind;
  uint8                                 dimension_cnt = 0; 
  cm_qmi_sns_data_collection_rsp_s_type *cm_sensor_data_resp;
  uint8                                 sns_len =0;
  uint8                                 batch_len =0;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG1(MSG_LEGACY_MED,"qmi_dmsi_msgr_hdlr  with msgr type = [%d]",msgrtype);
  switch (msgrtype) 
  {
    case  POLICYMAN_CFG_UPDATE_MSIM_IND:
       /*post a command to DCC*/
       /*Ignore payload here as info will be queried*/
       /*release the payload as it has reference in policyman*/
       policyman_msgr_attachments_release((msgr_hdr_s*)dsmsg);
	   /*-----------------------------------------------------------------------
           Get a DS command buffer
       -----------------------------------------------------------------------*/
       cmd_ptr = ds_allocate_cmd_buf(sizeof(qmi_dmsi_cmd_buf_type));
       if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
       {
         DS_ERR_FATAL("Out of memory");
         return FALSE;
       }
       cmd_ptr->hdr.cmd_id = DS_QMI_DMS_SVC_CMD;
       cmd_payload = (qmi_dmsi_cmd_buf_type *)cmd_ptr->cmd_payload_ptr;
	   cmd_payload->cmd_id = QMI_CMD_DMS_DEV_CURRENT_CAP_CB;
	   ds_put_cmd(cmd_ptr);
        result = TRUE;
      break;

    case MM_CM_PSM_GET_CFG_PARAMS_RSP:

#ifdef FEATURE_DATA_ATCOP
      /* Call ATCoP */
      dsat_msgr_hdlr(msgrtype, dsmsg);
#endif /* FEATURE_DATA_ATCOP */

	  cm_psm_get_cfg_params_resp = (cm_psm_get_cfg_params_rsp_s_type *)dsmsg;
	
	  /*-----------------------------------------------------------------------
          Get a DS command buffer
      -----------------------------------------------------------------------*/
	  cmd_ptr = ds_allocate_cmd_buf(sizeof(qmi_dmsi_cmd_buf_type));
      if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
      {
         DS_ERR_FATAL("Out of memory");
		 return FALSE;
      }
      cmd_ptr->hdr.cmd_id = DS_QMI_DMS_SVC_CMD;
      cmd_payload = (qmi_dmsi_cmd_buf_type *)cmd_ptr->cmd_payload_ptr;
      cmd_payload->cmd_id = QMI_CMD_DMS_PSM_GET_CFG_PARAMS_RESP;
      cmd_payload->data.psm_params.enabled = cm_psm_get_cfg_params_resp->enabled;
      cmd_payload->data.psm_params.threshold = cm_psm_get_cfg_params_resp->threshold;
      cmd_payload->data.psm_params.duration_oos = cm_psm_get_cfg_params_resp->psm_duration_due_to_oos;
      cmd_payload->data.psm_params.randomization_win = cm_psm_get_cfg_params_resp->psm_randomization_window;
      cmd_payload->data.psm_params.active_timer = cm_psm_get_cfg_params_resp->active_timer_value;
      cmd_payload->data.psm_params.periodic_update_timer = cm_psm_get_cfg_params_resp->periodic_tau_timer_value;
      cmd_payload->data.psm_params.early_wakeup_time = cm_psm_get_cfg_params_resp->early_wakeup_time;
      ds_put_cmd(cmd_ptr);

      result = TRUE;
      break;

    case MM_CM_PSM_STATUS_IND:

	 /*-----------------------------------------------------------------------
           Get a DS command buffer
     -----------------------------------------------------------------------*/
     cmd_ptr = ds_allocate_cmd_buf(sizeof(qmi_dmsi_cmd_buf_type));
     if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
     {
       DS_ERR_FATAL("Out of memory");
	   return FALSE;
     }
     cmd_ptr->hdr.cmd_id = DS_QMI_DMS_SVC_CMD;
     cmd_payload = (qmi_dmsi_cmd_buf_type *)cmd_ptr->cmd_payload_ptr;
	 cm_psm_status_ind = (cm_psm_status_ind_s_type *)dsmsg;

      cmd_payload->cmd_id = QMI_CMD_DMS_PSM_STATUS_IND;
      cmd_payload->data.psm_status.status = cm_psm_status_ind->status;
      cmd_payload->data.psm_status.reject_reason = cm_psm_status_ind->reject_reason;
      cmd_payload->data.psm_status.periodic_update_timer = 
                                 cm_psm_status_ind->periodic_update_timer_value;
      cmd_payload->data.psm_status.modem_only_timer = 
                                 cm_psm_status_ind->modem_only_timer_value;
      ds_put_cmd(cmd_ptr);
      result = TRUE;
      break;
   case MM_CM_PSM_CFG_CHANGE_IND:
#ifdef FEATURE_DATA_ATCOP
      /* Call ATCoP*/	  
      dsat_msgr_hdlr( msgrtype, dsmsg );
#endif /* FEATURE_DATA_ATCOP */

     /*-----------------------------------------------------------------------
          Get a DS command buffer
      -----------------------------------------------------------------------*/
     cmd_ptr = ds_allocate_cmd_buf(sizeof(qmi_dmsi_cmd_buf_type));
     if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
     {
       DS_ERR_FATAL("Out of memory");
       return FALSE;
     }
     cmd_ptr->hdr.cmd_id = DS_QMI_DMS_SVC_CMD; 
     cmd_payload = (qmi_dmsi_cmd_buf_type *)cmd_ptr->cmd_payload_ptr;     
     cm_psm_cfg_change_ind = (cm_mm_psm_cfg_change_ind_s_type *)dsmsg;
	 cmd_payload->cmd_id = QMI_CMD_DMS_PSM_CFG_CHANGE_IND;
      cmd_payload->data.psm_cfg_changes.change_mask = cm_psm_cfg_change_ind->change_mask;
      cmd_payload->data.psm_cfg_changes.psm_enabled = cm_psm_cfg_change_ind->psm_enabled;
      cmd_payload->data.psm_cfg_changes.active_timer = cm_psm_cfg_change_ind->active_timer_value;
      cmd_payload->data.psm_cfg_changes.periodic_update_timer = 
      cm_psm_cfg_change_ind->periodic_update_timer_value;

      ds_put_cmd(cmd_ptr);

      result = TRUE;
      break;
#ifdef FEATURE_REF_TRACKER
    case MM_CM_SNS_MGR_SNS_APP_EVENT_IND:
      cmd_ptr = ds_allocate_cmd_buf(sizeof(qmi_dmsi_cmd_buf_type));
      if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
      {
        DS_ERR_FATAL("Out of memory");
        return FALSE;
      }      
      cmd_ptr->hdr.cmd_id = DS_QMI_DMS_SVC_CMD; 
      cmd_payload = (qmi_dmsi_cmd_buf_type *)cmd_ptr->cmd_payload_ptr;           
      cm_sensor_policy_met_ind = (cm_qmi_sensor_event_ind_s_type *)dsmsg;
      cmd_payload->cmd_id = QMI_CMD_SNS_POLICY_MET_INDICATION;
      cmd_payload->data.sensor_policy_met.sns_indication.batch_full_sns_mask =     
                   cm_sensor_policy_met_ind->sns_event_info.batch_full_sns_mask;
      cmd_payload->data.sensor_policy_met.sns_indication.policy_met_sns_mask = 
                   cm_sensor_policy_met_ind->sns_event_info.policy_met_sns_mask;
      ds_put_cmd(cmd_ptr);
      result = TRUE;
      break;      
    case MM_CM_SNS_MGR_DATA_RSP:
      cmd_ptr = ds_allocate_cmd_buf(sizeof(qmi_dmsi_cmd_buf_type));
      if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
      {
        DS_ERR_FATAL("Out of memory");
        return FALSE;
      }      
      cmd_ptr->hdr.cmd_id = DS_QMI_DMS_SVC_CMD; 
      cmd_payload = (qmi_dmsi_cmd_buf_type *)cmd_ptr->cmd_payload_ptr;     
      cmd_payload->cmd_id = QMI_CMD_SNS_POLICY_DATA_RESP;      
      cm_sensor_data_resp = (cm_qmi_sns_data_collection_rsp_s_type *)dsmsg;
      cmd_payload->data.sensor_data_resp.resp_len = cm_sensor_data_resp->sns_len;

      for(sns_len = 0;sns_len< cmd_payload->data.sensor_data_resp.resp_len ;sns_len++)
      {
        cmd_payload->data.sensor_data_resp.sns_arr[sns_len].sensor_id = 
             cm_sensor_data_resp->sns_arr[sns_len].sensor_id;

        cmd_payload->data.sensor_data_resp.sns_arr[sns_len].batch_count = 
             cm_sensor_data_resp->sns_arr[sns_len].batch_count;
        for(batch_len =0;batch_len <cmd_payload->data.sensor_data_resp.sns_arr[sns_len].batch_count ;batch_len++)
        {
          cmd_payload->data.sensor_data_resp.sns_arr[sns_len].sample_batch[batch_len].\
                            dimension_data_len =    cm_sensor_data_resp->sns_arr[sns_len].\
                            sample_batch[batch_len].dimension_data_len;
           for(dimension_cnt =0 ;dimension_cnt<cmd_payload->data.sensor_data_resp.sns_arr[sns_len].sample_batch[batch_len].\
                       dimension_data_len ;dimension_cnt++)
           {
             cmd_payload->data.sensor_data_resp.sns_arr[sns_len].sample_batch[batch_len].\
                       sns_dimension_data[dimension_cnt] = 
                      cm_sensor_data_resp->sns_arr[sns_len].sample_batch[batch_len].sns_dimension_data[dimension_cnt];
           }
           cmd_payload->data.sensor_data_resp.sns_arr[sns_len].sample_batch[batch_len].\
                       timestamp =    cm_sensor_data_resp->sns_arr[sns_len].\
                       sample_batch[batch_len].timestamp;
        }
      }
      ds_put_cmd(cmd_ptr);
      result = TRUE;
      break;     
#endif
    default:
      DS_MSG1(MSG_LEGACY_ERROR,"Not supported msgr type %d", (int)msgrtype);
  }
  return result;
}

/*===========================================================================
  FUNCTION qmi_dms_device_mode_changed()

  DESCRIPTION  Device mode is changed and checks if an indication needs to be
               sent to clients.
    Note: Indication will be sent only in case of single sim SVLTE config
          Rest of the things will be taken care of Policyman indications
  PARAMETERS
    none
  RETURN VALUE
    none
  DEPENDENCIES
    none
  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dms_device_mode_changed
(
  void
)
{
  uint32               max_subs = 0;
  qmi_error_e_type errval = QMI_ERR_NONE;
  policyman_status_t   status = -1; 
  policyman_item_t const *cm_pm_device_config = NULL;
  policyman_item_id_t id1[] = { POLICYMAN_ITEM_DEVICE_CONFIGURATION };
  sys_subs_feature_t      subs_device_feature_mode;  

   /*Get the policyman items intially*/
  status = policyman_get_items(
              &id1[0],
              sizeof(id1)/sizeof(policyman_item_id_t),
              &cm_pm_device_config);
  if (POLICYMAN_FAILED(status) || (cm_pm_device_config == NULL) )
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

  status = 
    policyman_device_config_num_sims(cm_pm_device_config,
                                     (size_t*)&max_subs);
  if (POLICYMAN_FAILED(status) || (max_subs > SYS_MODEM_AS_ID_3))
  {
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }

   /*Indication will be sent only in case of single sim SVLTE config
     Rest of the things will be taken care of Policyman indications*/
  if(max_subs != 1)
  {
    goto send_result;
  }

  status = policyman_device_config_get_subs_feature(cm_pm_device_config,0,
                                                     SYS_MODEM_AS_ID_1,
                                                     &subs_device_feature_mode);
  if (POLICYMAN_FAILED(status))
  { 
    errval = QMI_ERR_INTERNAL;
    goto send_result;    
  }
  
  if(subs_device_feature_mode == SYS_SUBS_FEATURE_MODE_SVLTE)
  {
    qmi_dmsi_generate_current_device_capability_ind();
  }

  send_result:
   policyman_item_release(cm_pm_device_config);
   if(errval != QMI_ERR_NONE)
   {
     DS_MSG2(MSG_LEGACY_ERROR,"qmi_dms_device_mode_changed: failed to send IND errval:%d status: 0x%x",
                      errval, status);
   }
   return;  
}

/*===========================================================================
  FUNCTION qmi_dmsi_generate_current_device_capability_ind()

  DESCRIPTION  Generate the Event report indicaiton for curr dev cap
   
  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    
  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dmsi_generate_current_device_capability_ind ( void )
{
  qmi_dmsi_client_info_type *    cl_sp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_HIGH,"qmi_dmsi_generate_current_device_capability_ind()");

  if (!qmi_dmsi_inited)
  {
    return;
  }

  /* Send the indication for all the registered clients */
  cl_sp = (qmi_dmsi_client_info_type*)q_check(&qmi_dms_state.client_q);
  while(cl_sp != NULL)
  {
    if( cl_sp->report_status.report_device_mode)
    {
      qmi_dmsi_event_report_ind(cl_sp,
                                DMSI_REPORT_STATUS_EV_DEV_MSIM_CAP,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0,
                                0);
    }
    cl_sp = q_next(&qmi_dms_state.client_q,
                           &(cl_sp->link));
  }
}

/*===========================================================================
FUNCTION qmi_dmsi_get_device_multisim_current_cap_info()

  DESCRIPTION - fill all the tlvs related to curr dev cap in given resp pointer
    
  PARAMETERS
     response - dsm_item for response
      uint8   - resp_type, IND or RESP
 
  RETURN VALUE
   qmi_error_type_v01   - error encountered while filling tlv

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_dmsi_get_device_multisim_current_cap_info
(
  void* response,
  uint8 resp_type
)
{
  policyman_status_t  status = -1; 
  policyman_item_t const *cm_policyman_device_config = NULL;
  policyman_item_id_t id1[] = { POLICYMAN_ITEM_DEVICE_CONFIGURATION };
  uint32             max_subs = 0;
  uint32             max_active = 0;
  sys_subs_feature_t      subs_device_feature_mode = 0; 
  qmi_if_info_type       *qmi_if_info_ptr = NULL;
  const policyman_item_collection_t *cm_policyman_dev_curr_cap = NULL;  
  sys_modem_as_id_e_type sub_id;
  const policyman_item_t *pItem = NULL;
  policyman_item_id_t id2[] = { POLICYMAN_ITEM_RAT_CAPABILITY
 #ifdef FEATURE_DATA_POLICYMAN
  	                            ,POLICYMAN_ITEM_UE_MODE
#endif /* FEATURE_DATA_POLICYMAN */
                               };
  uint32             curr_rat_tmp = 0;
  int              numitems = 0;
  int i = 0;
  qmi_error_type_v01 errval = QMI_ERR_NONE_V01;
  dms_get_device_cap_resp_msg_v01* dev_cap_res;
  dms_event_report_ind_msg_v01* ind_res;

  /* resp_type FALSE indicates the caller is get_dev_cap request handler
     TRUE indicates the caller is indication*/
  if(resp_type == FALSE)
  {
    dev_cap_res = (dms_get_device_cap_resp_msg_v01*)response;
  }
  else
  {
    ind_res = (dms_event_report_ind_msg_v01*)response;
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
    DS_MSG1(MSG_LEGACY_MED,"qmi_dmsi_get_device_multisim_current_cap_info resp_type:%d ",resp_type);
  
    status = policyman_get_items(
                  &id1[0],
                  sizeof(id1)/sizeof(policyman_item_id_t),
                  &cm_policyman_device_config
                  );
    if (POLICYMAN_FAILED(status))
    {
      errval = QMI_ERR_INTERNAL_V01;
      goto send_result;
    }
  
    status = 
      policyman_device_config_num_sims(cm_policyman_device_config,(size_t*)&max_subs);
    if (POLICYMAN_FAILED(status))
    {
      errval = QMI_ERR_INTERNAL_V01;
      goto send_result;
    }
    status = policyman_device_config_max_active(cm_policyman_device_config, (size_t*)&max_active);
    if (POLICYMAN_FAILED(status))
    {
      errval = QMI_ERR_INTERNAL_V01;
      goto send_result;
    }
    if(resp_type == FALSE)
    {
      dev_cap_res->current_multisim_capability.max_active = (uint8)max_active;
      dev_cap_res->current_multisim_capability.max_subscriptions = (uint8)max_subs;
    }
    else
    {
      ind_res->current_multisim_capability.max_active = (uint8)max_active;
      ind_res->current_multisim_capability.max_subscriptions = (uint8)max_subs;
    }
  /*Get current RAT Capability and UE mode per subs*/
  status = policyman_get_items_msim(id2,
                            sizeof(id2)/sizeof(policyman_item_id_t),
                            &cm_policyman_dev_curr_cap);
  if ( POLICYMAN_FAILED(status) || (cm_policyman_dev_curr_cap == NULL) || (cm_policyman_dev_curr_cap->numItems == 0))
   {
     errval = QMI_ERR_INTERNAL_V01;
     goto send_result;
   }

  /*check for the number of items it should be max_subs * items asked*/
  status = -1;
  numitems = cm_policyman_dev_curr_cap->numItems;

  DS_MSG3(MSG_LEGACY_MED,"policyman_get_items_msim returned num_items:%d, max_sub:%d, max_active: %d", 
                   numitems, max_subs, max_active);

  for (i=0; i < numitems; i++)
  {
    pItem = (policyman_item_t *)cm_policyman_dev_curr_cap->pItems[i];

    switch (policyman_item_get_id_msim(pItem,&sub_id))
    {
      case POLICYMAN_ITEM_RAT_CAPABILITY:

        status = policyman_get_rat_capability( pItem,
                                            &curr_rat_tmp);
        if ( POLICYMAN_FAILED(status) || (sub_id < SYS_MODEM_AS_ID_1) || 
            (sub_id > SYS_MODEM_AS_ID_3))
        {
          errval = QMI_ERR_INTERNAL_V01;
          goto send_result;
        }
        if(resp_type == FALSE)
     	{
          dev_cap_res->current_subscription_capability_len++;
          dev_cap_res->current_subscription_capability[sub_id] = qmi_dmsi_convert_rat_mask(curr_rat_tmp);
     	}
     	else
     	{
          ind_res->current_subscription_capability_len++;
          ind_res->current_subscription_capability[sub_id] = qmi_dmsi_convert_rat_mask(curr_rat_tmp);
     	}
        
       break;
        
#ifdef FEATURE_DATA_POLICYMAN
     case POLICYMAN_ITEM_UE_MODE:

       status = policyman_get_ue_mode( pItem,
                                       &dev_vd_cap_tmp);
     if ( POLICYMAN_FAILED(status) || (sub_id < SYS_MODEM_AS_ID_1) || 
          (sub_id > SYS_MODEM_AS_ID_3))
       {
         errval = QMI_ERR_INTERNAL_V01;
         goto send_result;
       }
       /*Convert to QMI notations*/
       /*Use new API from policyman to fill this*/
        if(resp_type == FALSE)
        {
     	  dev_cap_res->subs_voice_data_capability_len++;
          dev_cap_res->subs_voice_data_capability[sub_id].simul_voice_data_capable = 
                  policyman_is_svd_operation_allowed(sub_id);
          dev_cap_res->subs_voice_data_capability[sub_id].subs_voice_data_capability = 
                 qmi_dmsi_is_simul_voice_data(dev_vd_cap_tmp);
        }
        else
        {
          ind_res->subs_voice_data_capability_len++;
          ind_res->subs_voice_data_capability[sub_id].simul_voice_data_capable = 
                                policyman_is_svd_operation_allowed(sub_id);
          ind_res->subs_voice_data_capability[sub_id].subs_voice_data_capability = 
                                qmi_dmsi_is_simul_voice_data(dev_vd_cap_tmp);
        }
        break;
#endif /* FEATURE_DATA_POLICYMAN */

     default:
        DS_MSG1(MSG_LEGACY_ERROR,"Invalid policyman item: %d",i);
        break;
    }/*switch*/
  }/*for*/
  /*Policyman dont handle single sim SVLTE case. Get info from CM and update ue_mode*/
   if(max_subs == 1)
   {
     status = policyman_device_config_get_subs_feature(cm_policyman_device_config,0,
                                                     SYS_MODEM_AS_ID_1,
                                                     &subs_device_feature_mode);
     if (POLICYMAN_FAILED(status))
     {
       errval = QMI_ERR_INTERNAL_V01;
       goto send_result;
     }
     if(subs_device_feature_mode == SYS_SUBS_FEATURE_MODE_SVLTE)
     {
       qmi_if_info_ptr = qmi_if_info_get_ptr();
       /*if hybrid stack is opertational it is SVLTE else CSFB*/
       if(qmi_if_info_ptr->ss_info.is_hybr_stack_operational)
       {
        if(resp_type == FALSE)
     	{
     	  dev_cap_res->subs_voice_data_capability[SYS_MODEM_AS_ID_1].subs_voice_data_capability = 
                 qmi_dmsi_is_simul_voice_data(SYS_UE_MODE_SVLTE);
     	}
     	else
     	{
          ind_res->subs_voice_data_capability[SYS_MODEM_AS_ID_1].subs_voice_data_capability = 
                 qmi_dmsi_is_simul_voice_data(SYS_UE_MODE_SVLTE);
     	}
       }
       else
       {
        if(resp_type == FALSE)
        {
          dev_cap_res->subs_voice_data_capability[SYS_MODEM_AS_ID_1].subs_voice_data_capability = 
                 qmi_dmsi_is_simul_voice_data(SYS_UE_MODE_CSFB);
        }
        else
        {
          ind_res->subs_voice_data_capability[SYS_MODEM_AS_ID_1].subs_voice_data_capability = 
                 qmi_dmsi_is_simul_voice_data(SYS_UE_MODE_CSFB);
     	}
       }
     }
     DS_MSG2(MSG_LEGACY_MED,"Single Sim case: Subs feature mode:%d and UE_Mode: %d ",
                   subs_device_feature_mode, 
                    (resp_type == FALSE ? 
                     dev_cap_res->subs_voice_data_capability[SYS_MODEM_AS_ID_1].subs_voice_data_capability:
                     ind_res->subs_voice_data_capability[SYS_MODEM_AS_ID_1].subs_voice_data_capability) );
   }

  send_result:

  /*release the policyman items after use*/
  if(cm_policyman_device_config != NULL)
  {
    policyman_item_release(cm_policyman_device_config);
  }
  if(cm_policyman_dev_curr_cap != NULL)
  {
    policyman_item_collection_release(cm_policyman_dev_curr_cap);
  }

  if(errval != QMI_ERR_NONE_V01)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"qmi_dmsi_get_device_multisim_current_cap_info: Failed to get info.errval:%d Policyman. status:0x%x",
                    errval,
                    status);
  }

  return errval;

}

/*===========================================================================
  FUNCTION qmi_dmsi_is_simul_voice_data()

  DESCRIPTION  - Convert policyman ue_mode to QMI enum
    
  PARAMETERS
     ue_mode - policyman enum for ue_mode

  RETURN VALUE
     dms_subs_voice_data_capability_enum_v01  - qmi enum for ue_mode
   
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dms_subs_voice_data_capability_enum_v01 qmi_dmsi_is_simul_voice_data 
(
   sys_ue_mode_e_type ue_mode
)
{
  dms_subs_voice_data_capability_enum_v01 vd_cap;

  switch(ue_mode)
  {
    case SYS_UE_MODE_NONE:
    case SYS_UE_MODE_NORMAL:
        vd_cap = DMS_SUBS_VOICE_DATA_CAPABILITY_NORMAL_V01;
      break;
  
    case SYS_UE_MODE_SVLTE:
    case SYS_UE_MODE_SVLTE_ONLY:
        vd_cap = DMS_SUBS_VOICE_DATA_CAPABILITY_SVLTE_V01;
      break;

    case SYS_UE_MODE_SGLTE:
    case SYS_UE_MODE_SGLTE_ONLY:
        vd_cap = DMS_SUBS_VOICE_DATA_CAPABILITY_SGLTE_V01;
      break;

    case SYS_UE_MODE_CSFB:
    case SYS_UE_MODE_CSFB_ONLY:
        vd_cap = DMS_SUBS_VOICE_DATA_CAPABILITY_CSFB_V01;
      break;

    case SYS_UE_MODE_1XSRLTE_ONLY:
    case SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED:
    case SYS_UE_MODE_GSMSRLTE_ONLY:
    case SYS_UE_MODE_GSMSRLTE_CSFB_PREFERRED:
        vd_cap =  DMS_SUBS_VOICE_DATA_CAPABILITY_SRLTE_V01;
      break;

    default:
        vd_cap = DMS_SUBS_VOICE_DATA_CAPABILITY_NORMAL_V01;
      break;
  }

  return vd_cap;
}

/*===========================================================================
  FUNCTION qmi_dmsi_convert_rat_mask()

  DESCRIPTION  - convert rat mask from policyman to QMI rat mask
    
  PARAMETERS
    rat  - policyman rat mask

  RETURN VALUE
   uint64 - QMI rat mask

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint64 qmi_dmsi_convert_rat_mask 
(
   uint32 rat
)
{
  uint64 rat_mask = 0;

  if(rat & SYS_SYS_MODE_MASK_CDMA)
  {
    rat_mask |= DMS_SUBS_CAPABILITY_CDMA_V01;
  }
  if(rat & SYS_SYS_MODE_MASK_HDR)
  {
    rat_mask |= DMS_SUBS_CAPABILITY_HDR_V01;
  }
  if(rat & SYS_SYS_MODE_MASK_GSM) 
  {
    rat_mask |= DMS_SUBS_CAPABILITY_GSM_V01;
  }
  if(rat & SYS_SYS_MODE_MASK_WCDMA)
  {
    rat_mask |= DMS_SUBS_CAPABILITY_WCDMA_V01;
  }
  if(rat & SYS_SYS_MODE_MASK_LTE)
  {
    rat_mask |= DMS_SUBS_CAPABILITY_LTE_V01;
  }
  if(rat & SYS_SYS_MODE_MASK_TDS)
  {
    rat_mask |= DMS_SUBS_CAPABILITY_TDS_V01;
  }
  
  return rat_mask;

}

/*===========================================================================
  FUNCTION QMI_DMSI_INITIALIZE_CLIENT()

  DESCRIPTION
    Initialize the new client state for the given client state ptr.
    
  PARAMETERS
    cl_sp :  client state pointer
    
  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dmsi_initialize_client
(
  qmi_dmsi_client_info_type * cl_sp
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset( cl_sp , 0, sizeof(qmi_dmsi_client_info_type) );
} /* qmi_dmsi_initialize_client() */

/*===========================================================================
  FUNCTION QMI_DMSI_PROCESS_CMD()

  DESCRIPTION
    This function processes a QMI DMS command or event.

    It is called by the QMI command handler and will dispatch the
    associated command/event handler function.

  PARAMETERS
    cmd_ptr:  private data buffer containing the QMI DMS command
              information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI DMS must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dmsi_process_cmd
(
  ds_command_enum_type  cmd,
  void *cmd_ptr
)
{
  qmi_dmsi_cmd_buf_type *cmd_buf_ptr = NULL;
/*-------------------------------------------------------------------------*/

  DS_ASSERT((int)cmd_ptr);
  cmd_buf_ptr = (qmi_dmsi_cmd_buf_type *)cmd_ptr;

  switch(cmd_buf_ptr->cmd_id)
  {

#if defined(FEATURE_MMGSDI_SESSION_LIB)
    case QMI_CMD_PROCESS_UIM_GET_STATE:
      /* The mmgsdi_evt_info.activated field is populated and used only for
         MMGSDI_SESSION_CHANGED_EVT */
     /* qmi_dms_process_uim_get_state(cmd_buf_ptr->data.mmgsdi_evt_info.event,
                                       cmd_buf_ptr->data.mmgsdi_evt_info.card_err_info,
                                       cmd_buf_ptr->data.mmgsdi_evt_info.activated);*/
      break;

#endif /* defined(FEATURE_MMGSDI_SESSION_LIB) */

    case QMI_CMD_DMS_CM_CALL_CMD_STATUS:
      /*qmi_dms_process_cm_call_err(cmd_buf_ptr->data.cm_call_err.user_data,
                                     cmd_buf_ptr->data.cm_call_err.call_cmd,
                                     cmd_buf_ptr->data.cm_call_err.err_type);*/
      break;

    case QMI_CMD_SET_OPERATING_MODE_RESP:
      qmi_dms_generate_set_oprt_mode_resp(cmd_buf_ptr->data.cm_ph_err.user_data,
                                             cmd_buf_ptr->data.cm_ph_err.err_type);
      break;

    case QMI_CMD_DMS_EVT_REPORT_REQ_IND:
      qmi_dms_event_report_req_send_ind
                          (cmd_buf_ptr->data.dms_evt_info.evt_type,                           
                           cmd_buf_ptr->data.dms_evt_info.evt_value,
                           cmd_buf_ptr->data.dms_evt_info.cl_sp);
      break;

    case QMI_CMD_DMS_RESET_SHUTDOWN:
      qmi_dms_generate_oprt_mode_ind(cmd_buf_ptr->data.cm_ph.info.oprt_mode);
      break;

    case QMI_CMD_DMS_DEV_CURRENT_CAP_CB:
    {
      qmi_dmsi_generate_current_device_capability_ind();
      break;
    }
    case QMI_CMD_DMS_POWER_STATISTICS_EVT:
    {
//      qmi_dmsi_generate_power_stats_ind(&(cmd_buf_ptr->data.power_stats_info));
      break;
    }

    case QMI_CMD_DMS_PSM_GET_CFG_PARAMS_RESP:
    {
      qmi_dmsi_generate_psm_get_cfg_params_resp(cmd_buf_ptr);
      break;
    }

    case QMI_CMD_DMS_PSM_STATUS_IND:
    {
      qmi_dmsi_generate_psm_status_ind(cmd_buf_ptr);
      break;
    }
    case QMI_CMD_DMS_PSM_CFG_CHANGE_IND:
    {
      qmi_dmsi_generate_psm_cfg_change_ind(cmd_buf_ptr);
      break;
    }
#ifdef FEATURE_REF_TRACKER
    case QMI_CMD_SNS_POLICY_MET_INDICATION:
    {
      qmi_dmsi_sensor_policy_met_indication(cmd_buf_ptr);      
      break;
    }
    case QMI_CMD_SNS_POLICY_DATA_RESP:
    {
      qmi_dmsi_sensor_get_data_resp(cmd_buf_ptr);      
      break;     
    }
#endif
    default:
      DS_MSG1(MSG_LEGACY_ERROR,"Unexpected cmd(%d) recd in QMI DMS cmd handler",
                      cmd_buf_ptr->cmd_id);
  }
  return;
} /* qmi_dmsi_process_cmd() */

/*===========================================================================
  FUNCTION QMI_DMSI_PROCESS_SVC_INIT()

  DESCRIPTION
    This function processes a init callback request and intialize the service
    related infos.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    QMI DMS must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dmsi_process_svc_init
(
  void
)
{
/*-------------------------------------------------------------------------*/

  DS_MSG0(MSG_LEGACY_MED, "qmi_dmsi init callback " );

  /*-------------------------------------------------------------------------
    initialize client state non-zero fields
  -------------------------------------------------------------------------*/

  if(qmi_dmsi_global.inited == FALSE)
  {
    /*-----------------------------------------------------------------------
      Initialize the global DMS service parameters
    -----------------------------------------------------------------------*/  
    memset( &qmi_dmsi_global, 0, sizeof(qmi_dmsi_global) );
  
    qmi_dmsi_global.voice_call_id = CM_CALL_ID_INVALID;
    
    /* init the PRL sequence number to 0 for manual activation PRL's */

    if (QMI_NV_STATUS_OK != qmi_nv_read(QMI_NV_ITEM_ALLOW_INVALID_SPC,
                                  0,
                                  0,
                                  &qmi_dmsi_global.nv_allow_invalid_spc,
                                  sizeof(qmi_dmsi_global.nv_allow_invalid_spc)))
    {
      qmi_dmsi_global.nv_allow_invalid_spc = FALSE;
    }
    if (QMI_NV_STATUS_OK != qmi_nv_read(QMI_NV_ITEM_RESTRICT_IMSI_ACCESS,
                               0,
                               0,
                               &qmi_dmsi_global.nv_restrict_imsi_access,
                               sizeof(qmi_dmsi_global.nv_restrict_imsi_access)))
    {
      qmi_dmsi_global.nv_restrict_imsi_access = FALSE;
    }
    if (QMI_NV_STATUS_OK != qmi_nv_read(
                         QMI_NV_ITEM_USE_MAX_DEFINED_DATA_RATES,
                         0,
                         0,
                         &qmi_dmsi_global.nv_use_max_defined_data_rates,
                         sizeof(qmi_dmsi_global.nv_use_max_defined_data_rates)))
    {
      qmi_dmsi_global.nv_use_max_defined_data_rates = FALSE;
    }
    qmi_dmsi_policyman_config_init();

    /* Initialize PSM */
    q_init(&qmi_dms_state.psm_get_cfg_params_q);
    dsmsgrrcv_reg_hdlr(MM_CM_PSM_GET_CFG_PARAMS_RSP, qmi_dmsi_msgr_hdlr);
    dsmsgrrcv_reg_hdlr(MM_CM_PSM_STATUS_IND, qmi_dmsi_msgr_hdlr);
    dsmsgrrcv_reg_hdlr(MM_CM_PSM_CFG_CHANGE_IND, qmi_dmsi_msgr_hdlr);
#ifdef FEATURE_REF_TRACKER
    dsmsgrrcv_reg_hdlr(MM_CM_SNS_MGR_SNS_APP_EVENT_IND, qmi_dmsi_msgr_hdlr);
    dsmsgrrcv_reg_hdlr(MM_CM_SNS_MGR_DATA_RSP, qmi_dmsi_msgr_hdlr);
#endif

    qmi_dmsi_global.inited = TRUE;
  }

} /* qmi_dmsi_process_svc_init */




/*===========================================================================
  FUNCTION QMI_DMSI_SEND_RESPONSE()

  DESCRIPTION
  sends response using QCSI API.
    
  PARAMETERS
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    c_struct_len       :       length of response


  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dmsi_send_response
(
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void*                    resp_msg,
  unsigned int             c_struct_len
)
{
  qmi_csi_error err = QMI_CSI_NO_ERR;
  err = qmi_csi_send_resp(req_handle, msg_id, resp_msg, c_struct_len);
  DS_MSG2(MSG_LEGACY_MED,"qmi_dmsi_send_response err = %d for msg_id = %d",err,msg_id);
}

/*===========================================================================
  FUNCTION QMI_DMS_EVENT_REPORT_REQ_SEND_IND()

  DESCRIPTION
    Sends the indication of a phone to the client that 
    registered for it.
    
  PARAMETERS
    event        : phone event from cm
    value        : status of the phone event
    clid         : Id of the registering client

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_dms_event_report_req_send_ind
(
  qmi_dmsi_report_status_ev_e_type             event_type,
  void                                       * event_info,
  qmi_dmsi_client_info_type*                   cl_sp
)
{
#ifdef FEATURE_DATA_QMI_DMS_CDMA
  boolean *                                    cdma_lock_mode = NULL;
#endif /* FEATURE_DATA_QMI_DMS_CDMA */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   DS_MSG2(MSG_LEGACY_HIGH,"qmi_dms_send_indication() event %d, clid %0x",
                  event_type, cl_sp);

  switch (event_type)
  {   
   case DMSI_REPORT_STATUS_EV_DEV_MSIM_CAP:
          qmi_dmsi_event_report_ind(cl_sp,
                                    DMSI_REPORT_STATUS_EV_DEV_MSIM_CAP,
                                    0,
                                    0,
                                    0,
                                    0,
                                    0,
                                    0,
                                    0);
    break;

    default:
      break;
  }
} /*qmi_dms_event_report_req_send_ind*/

/*===========================================================================
  FUNCTION QMI_DMSI_TRIGGER_MODEM_ACTIVITY_INFO()

  DESCRIPTION
    Triggers the modem activity info calculation
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_trigger_modem_activity_info
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  qmi_error_type_v01   errval = QMI_ERR_NONE_V01;
  qmi_result_type_v01  result;
  dms_config_modem_activity_info_resp_msg_v01    resp_msg;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
  goto send_result;

  
  DS_ASSERT(clnt_info && req_c_struct);
#if 0
  dms_client_sp = (qmi_dmsi_client_info_type *)clnt_info;
  req_msg = (dms_config_modem_activity_info_req_msg_v01*)req_c_struct;
  memset(&resp_msg, 0, sizeof(dms_config_modem_activity_info_resp_msg_v01));
  
  if ( req_msg->enable_statistics_valid ) 
  {
    stats_configuration.enable_power_statistics = req_msg->enable_statistics;
    return_val = ps_sys_conf_set_ex( PS_SYS_TECH_ALL, 
                                     PS_SYS_CONF_MODEM_POWER_STATS_CONFIGURATION,
                                     PS_SYS_PRIMARY_SUBS, 
                                     &stats_configuration,
                                     &ps_errno );
  }
  else
  {
    errval = QMI_ERR_MISSING_ARG_V01;
    goto send_result;

  }

  if(return_val < 0)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Cannot configure power stats, Err: (%d)", ps_errno);
  
    switch (ps_errno)
    {                                   
      case DS_NOMEMORY:
        errval  = QMI_ERR_NO_MEMORY_V01;
        break;
      
      case DS_EFAULT:
        errval = QMI_ERR_INVALID_ARG_V01;
        break;
        
      default:
        errval = QMI_ERR_INTERNAL_V01;
        break;
    } /*  switch (ps_errno) */
    
    goto send_result;
  }

  if ( req_msg-> enable_statistics == FALSE )
  {
    //power statistic turned off; Reset last query time
    qmi_dms_state.last_modem_activity_query_time = 0;

    //Clear the pending queue
    while ( (pend_stats_q_node = 
            (qmi_dmsi_power_stats_pend_q_type *) q_get(&qmi_dms_state.modem_activity_pend_q)) != NULL )
    {
      DS_SYSTEM_HEAP_MEM_FREE(pend_stats_q_node);   
    }
  }
#endif
send_result:
    result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                     : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_config_modem_activity_info_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;

} /* qmi_dmsi_trigger_modem_activity_info() */

/*===========================================================================
  FUNCTION QMI_DMSI_GET_MODEM_ACTIVITY_INFO()

  DESCRIPTION
    Queries for the modem activity information. The actual statisctics is
    not returned in the response. Client is expected to listen to
    QMI_DMSI_MODEM_ACTIVITY_INFO_IND for the actual statistics
 
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_get_modem_activity_info
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  qmi_error_type_v01   errval = QMI_ERR_NONE_V01;
  qmi_result_type_v01  result;
  dms_get_modem_activity_info_resp_msg_v01 resp_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
  goto send_result;
  
  
  DS_ASSERT((int)clnt_info);
#if 0
  dms_client_sp = (qmi_dmsi_client_info_type *)clnt_info;
  memset(&resp_msg,0,sizeof(dms_get_modem_activity_info_resp_msg_v01));
  memset(&activity_info, 0, sizeof(ps_sys_modem_power_stats_info_type));
  memset(&stats_configuration, 0, sizeof(ps_sys_modem_power_stats_config_type));

  //Check if power stats calculation is enabled 
  return_val = ps_sys_conf_get_ex( PS_SYS_TECH_ALL, 
                                   PS_SYS_CONF_MODEM_POWER_STATS_CONFIGURATION,
                                   PS_SYS_PRIMARY_SUBS,
                                   &stats_configuration,
                                   &ps_errno );

  if(return_val == 0)
  {
    if (stats_configuration.enable_power_statistics == FALSE)
    {
      DS_MSG0(MSG_LEGACY_ERROR,"Power stats not configured, Err: (%d)");
      errval = QMI_ERR_NOT_PROVISIONED_V01;
      goto send_result;
    }
  }
  else
  {
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }

  /*-------------------------------------------------------------------------
    Q has pending responses? Add the new request to Q. Do not invoke MH for
    stats again since, this will reset lower layers stats to 0, for every new
    request
  -------------------------------------------------------------------------*/
  
  pending_cl_sp = (qmi_dmsi_client_info_type *)q_check(&qmi_dms_state.modem_activity_pend_q);
  if ( pending_cl_sp != NULL )
  {
    DS_MSG1(MSG_LEGACY_MED,"Adding cl_sp 0x%p into pending queue ", dms_client_sp);
  }
  else /* Q is empty */
  {
    // Query the current sys time
    current_time = time_get_ms_native();
    if( (current_time - qmi_dms_state.last_modem_activity_query_time) < 
        (MODEM_ACTIVITY_QUERY_THRESHOLD_TIME + 1) )
    {
      /*-------------------------------------------------------------------------
        New request has been made within the minimum threshold time.Don't trigger
        MH for new values, since this will reset lower layers statistics to 0.
        Return the last reported statistics from our cache.
      -------------------------------------------------------------------------*/
      return_val = ps_sys_conf_get_ex( PS_SYS_TECH_ALL, 
                                       PS_SYS_CONF_MODEM_POWER_STATISTICS,
                                       PS_SYS_PRIMARY_SUBS, 
                                       (void*)&activity_info,
                                       &ps_errno );
    
      if(return_val == 0)
      {
        DS_MSG1(MSG_LEGACY_MED,"Invoking cb for cl_sp 0x%p ", dms_client_sp);
        dmsi_power_statistics_info_cb( PS_SYS_TECH_ALL, 
                                       PS_SYS_EVENT_MODEM_POWER_STATISTICS, 
                                       PS_SYS_PRIMARY_SUBS, 
                                       &activity_info, 
                                       NULL);
      }
    }
    else
    {
      //Invoke MH to compute modem power stats
      return_val = ps_sys_conf_set_ex( PS_SYS_TECH_ALL, 
                                       PS_SYS_CONF_QUERY_MODEM_POWER_STATS,
                                       PS_SYS_PRIMARY_SUBS, 
                                       (void*)dms_client_sp,
                                       &ps_errno );

      if(return_val == 0)
      {
        //Update time stamp of last query request
        qmi_dms_state.last_modem_activity_query_time  = current_time;
        DS_MSG2(MSG_LEGACY_MED,"cl_sp 0x%p query time %d ", dms_client_sp, current_time);
      }
    }
                              
    if(return_val < 0)
    {
      DS_MSG1(MSG_LEGACY_ERROR,"Cannot query power stats, Err: (%d)", ps_errno);
      switch (ps_errno)
      {                                   
        case DS_NOMEMORY:
          errval  = QMI_ERR_NO_MEMORY_V01;
          break;
        
        case DS_EFAULT:
          errval = QMI_ERR_INVALID_ARG_V01;
          break;
          
        default:
          errval = QMI_ERR_INTERNAL_V01;
          break;
      } /*  switch (ps_errno) *
      goto send_result;
    }
  }/* Q is empty*/

 /*-------------------------------------------------------------------------
    Queue client state pointer to the pending queue of power stats.
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(pend_stats_q_node, 
                                  sizeof(qmi_dmsi_power_stats_pend_q_type),
                                  qmi_dmsi_power_stats_pend_q_type*);
  if( NULL == pend_stats_q_node )
  {
    DS_ASSERT(0);
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  //Put client into pending queue
  pend_stats_q_node->cl_sp = dms_client_sp;
  q_link(pend_stats_q_node, &(pend_stats_q_node->link));
  q_put( &qmi_dms_state.modem_activity_pend_q, &(pend_stats_q_node->link));
#endif

send_result:
    result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                     : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  resp_msg.resp.result = result;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_get_modem_activity_info_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
  
} /* qmi_dmsi_get_modem_activity_info() */


/*===========================================================================
FUNCTION      DS_QMI_CM_PSM_ENTER_REQ_CN

DESCRIPTION   This callback function is registered with CM to notify DS of
              PSM READY REQUEST. It posts cmd to DS.

DEPENDENCIES  None

RETURN VALUE 
       True if command is posted successfully.
       False if command posting fails

SIDE EFFECTS  None
===========================================================================*/
boolean ds_qmi_cm_psm_ready_req_cb( void )
{
  boolean                            ret_val = TRUE;
  ds_command_type              *cmd_ptr = NULL;
  
  qmi_dmsi_cmd_buf_type        *cmd_payload= NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(qmi_dmsi_cmd_buf_type));
  if ((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Out of memory");
    return FALSE;
  }

  DS_MSG0(MSG_LEGACY_HIGH,
                    "PSM Ready Command Posted");

  cmd_payload = (qmi_dmsi_cmd_buf_type *)cmd_ptr->cmd_payload_ptr;
  
  cmd_payload->cmd_id = QMI_CMD_DMS_PSM_STATUS_IND;
  cmd_payload->data.psm_status.status = CM_PSM_STATUS_READY;
  cmd_payload->data.psm_status.periodic_update_timer  = 0xFFFF;
  cmd_payload->data.psm_status.modem_only_timer = 0xFFFF;
  
  cmd_ptr->hdr.cmd_id = DS_QMI_DMS_SVC_CMD; 

  if (cmd_ptr != NULL)
  {
    ds_put_cmd(cmd_ptr);
  }

  return ret_val;
} 

/*===========================================================================
  FUNCTION QMI_DMSI_INDICATION_REGISTER()

  DESCRIPTION
    Registers the requesting control point for various indications. 

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_indication_register
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  dms_indication_register_req_msg_v01  * req_msg;
  dms_indication_register_resp_msg_v01   resp_msg;
  boolean                                got_one_tlv = FALSE;  
  int8                                   cm_psm_version =0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(req_c_struct !=NULL);
  req_msg = (dms_indication_register_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(resp_msg));

  // Process the request
  if (req_msg->report_psm_status_valid)
  {
    got_one_tlv = TRUE;
    clnt_info->report_status.report_psm_status =
                        req_msg->report_psm_status ? TRUE : FALSE;
	
    //Register with the CM for the PSM callbacks
    
     cm_psm_version = cm_get_psm_sw_ver();
     if(cm_psm_version == DMS_PSM_VERSION_2_V01) 
     {
       if(cm_client_psm_ready_reg(ds_qmi_cm_psm_ready_req_cb,
                                   CM_PSM_MODULE_QMI ) != TRUE)
       {
         DS_MSG0(MSG_LEGACY_HIGH,"Unable to register CM PSM Ready notification cb");
       }
    }
  }
  if (req_msg->report_psm_cfg_change_valid)
  {
    got_one_tlv = TRUE;
    clnt_info->report_status.report_psm_cfg_change =
                        req_msg->report_psm_cfg_change ? TRUE : FALSE;

  }
 if(req_msg->report_sensor_status_valid)
 {
    got_one_tlv = TRUE;
    clnt_info->report_status.report_sensor_policy_met_ind =
                        req_msg->report_sensor_status ? TRUE : FALSE;
  }

  // Check if at least one TLV is present
  if (got_one_tlv)
  {
    resp_msg.resp.error = QMI_ERR_NONE_V01;
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
  }
  else
  {
    resp_msg.resp.error = QMI_ERR_MISSING_ARG_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
  }

  // Send response
  qmi_dmsi_send_response(req_handle, msg_id, &resp_msg, sizeof(resp_msg));
  return QMI_CSI_CB_NO_ERR;

} /* qmi_dmsi_indication_register() */

/*===========================================================================
  FUNCTION QMI_DMSI_PSM_GET_CFG_PARAMS()

  DESCRIPTION
    Query PSM (Power Save Mode) configuration parameters.

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_psm_get_cfg_params
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  dms_psm_get_cfg_params_resp_msg_v01   resp_msg;
  cm_psm_get_cfg_params_req_s_type      cm_psm_get_cfg_params_req;
  ds_qmi_dms_async_cmd_type           * async_cmd_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&resp_msg, 0, sizeof(resp_msg));
  memset(&cm_psm_get_cfg_params_req, 0, sizeof(cm_psm_get_cfg_params_req));

  resp_msg.resp.error = QMI_ERR_NONE_V01;

  // Allocate async cmd buffer
  DS_SYSTEM_HEAP_MEM_ALLOC(async_cmd_p,
                           sizeof(ds_qmi_dms_async_cmd_type),
                           ds_qmi_dms_async_cmd_type*);
  if (async_cmd_p == NULL)
  {
    resp_msg.resp.error = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  memset(async_cmd_p, 0, sizeof(ds_qmi_dms_async_cmd_type));
  async_cmd_p->cl_sp = clnt_info;
  async_cmd_p->msg_id = msg_id;
  async_cmd_p->req_handle = req_handle;
  q_link(async_cmd_p, &async_cmd_p->link);

  // Send command to CM only if no pending request
  if (q_check(&qmi_dms_state.psm_get_cfg_params_q) == NULL)
  {
    cm_psm_get_cfg_params_req.user_config = FALSE;
    if (!ds_msgr_snd_msg_send(
                MM_CM_PSM_GET_CFG_PARAMS_REQ,
                (msgr_hdr_struct_type *)&cm_psm_get_cfg_params_req,
                sizeof(cm_psm_get_cfg_params_req)))
    {
      resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
      goto send_result;
    }
    else
    {
      DS_MSG1(MSG_LEGACY_HIGH,"PSM get_cfg_params cmd [%d] sent",
                               MM_CM_PSM_GET_CFG_PARAMS_REQ);
    }
  }

  // Queue and wait for CM response
  q_put(&qmi_dms_state.psm_get_cfg_params_q, &async_cmd_p->link);
  DS_MSG0(MSG_LEGACY_HIGH,"PSM get_cfg_params queued");

send_result:
  if (resp_msg.resp.error != QMI_ERR_NONE_V01)
  {
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
    qmi_dmsi_send_response(req_handle, msg_id, &resp_msg, sizeof(resp_msg));
    if (async_cmd_p != NULL)
    {
      DS_SYSTEM_HEAP_MEM_FREE(async_cmd_p);
    }
  }

  return QMI_CSI_CB_NO_ERR;

} /* qmi_dmsi_psm_get_cfg_params() */
/*===========================================================================
  FUNCTION QMI_DMSI_PSM_SET_CFG_PARAMS()

  DESCRIPTION
    Set PSM (Power Save Mode) configuration parameters.
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_psm_set_cfg_params
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
   dms_psm_set_cfg_params_resp_msg_v01	 resp_msg;
   dms_psm_set_cfg_params_req_msg_v01*   req_msg = NULL;
   cm_mm_psm_set_cfg_params_req_s_type 	 cm_psm_set_cfg_params_req;
   uint32 input_timer_value = 0;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   memset(&resp_msg, 0, sizeof(resp_msg));
   memset(&cm_psm_set_cfg_params_req, 0, sizeof(cm_psm_set_cfg_params_req));
   req_msg = (dms_psm_set_cfg_params_req_msg_v01*)req_c_struct;
 
   resp_msg.resp.error = QMI_ERR_NONE_V01;
   if ( req_msg->psm_enabled_valid == TRUE )
   {
     cm_psm_set_cfg_params_req.change_mask |= CM_PSM_CONFIG_INFO_MASK_PSM_ENABLED;
	 cm_psm_set_cfg_params_req.psm_enabled = req_msg->psm_enabled? TRUE : FALSE;
   }

   if ( req_msg->active_timer_valid == TRUE )
   {
     input_timer_value = (uint32)req_msg->active_timer;
      /*Validate of the timer as per Spec*/
     if(((input_timer_value <= 62) && (input_timer_value % 2 == 0)) ||		  
        ((input_timer_value >= 120 && input_timer_value <= 1860) && (input_timer_value % 60 == 0)) ||	   
        ((input_timer_value >= 2160 && input_timer_value <= 11160) && (input_timer_value % 360 == 0)))		   
     {
       cm_psm_set_cfg_params_req.change_mask |= CM_PSM_CONFIG_INFO_MASK_ACTIVE_TIMER;
       cm_psm_set_cfg_params_req.active_timer_value = req_msg->active_timer;
     }
     else
     {
       resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
       goto send_result;
     }
   }

   if ( req_msg->periodic_update_timer_valid == TRUE )
   {
      input_timer_value = (uint32)req_msg->periodic_update_timer;
      /*Validate of the timer as per Spec*/
      if(((input_timer_value <= 62) && (input_timer_value % 2 == 0)) ||
       ((input_timer_value >= 90 && input_timer_value <= 930) && (input_timer_value % 30 == 0))||		  
       ((input_timer_value >= 960 && input_timer_value <= 1860) && (input_timer_value % 60 == 0))||		   
       ((input_timer_value >= 2400 && input_timer_value <= 18600) && (input_timer_value % 600 == 0))||
       ((input_timer_value >= 21600 && input_timer_value <= 111600) && (input_timer_value % 3600 == 0))||
       ((input_timer_value >= 144000 && input_timer_value <= 1116000) && (input_timer_value % 36000 == 0))||
       ((input_timer_value >= 1152000 && input_timer_value <= 35712000) && (input_timer_value % 1152000 == 0)))	   
      {
        cm_psm_set_cfg_params_req.change_mask |= CM_PSM_CONFIG_INFO_MASK_PERIODIC_UPDATE_TIMER;
        cm_psm_set_cfg_params_req.periodic_tau_timer_value = req_msg->periodic_update_timer;
      }
      else
      {
        resp_msg.resp.error = QMI_ERR_INVALID_ARG_V01;
        goto send_result;
      }
   }
 
   if (!ds_msgr_snd_msg_send(
                  MM_CM_PSM_SET_CFG_PARAMS_REQ,
                  (msgr_hdr_struct_type *)&cm_psm_set_cfg_params_req,
                  sizeof(cm_psm_set_cfg_params_req)))
   {
     resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
     goto send_result;
   }
   DS_MSG1(MSG_LEGACY_HIGH,"PSM set_cfg_params cmd [%d] sent to CM",
                    MM_CM_PSM_SET_CFG_PARAMS_REQ);
 send_result:
  resp_msg.resp.result = (resp_msg.resp.error == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01 
								  : QMI_RESULT_FAILURE_V01);
  if(resp_msg.resp.error == QMI_ERR_INVALID_ARG_V01)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"qmi_dmsi_psm_set_cfg_params INVALID TIMER: active_timer %d or "
                     "periodic_update_timer: %d",
                     ((req_msg->active_timer_valid == TRUE)?req_msg->active_timer:0),
                     ((req_msg->periodic_update_timer_valid == TRUE)?req_msg->periodic_update_timer:0));
  }
  qmi_dmsi_send_response(req_handle, msg_id, &resp_msg, sizeof(resp_msg));
  return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
FUNCTION      DS_QMI_CM_PSM_READY_RESPONSE

DESCRIPTION   This function notifies CM that DS is ready to enter PSM Mode
              and provide the activate timer    
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_qmi_cm_psm_ready_response
(
  boolean        status,
  uint32         activate_timer
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cm_generic_rpt_u_type    psm_rpt;

  memset(&psm_rpt, 0, sizeof(cm_generic_rpt_u_type));

  DS_MSG0(MSG_LEGACY_MED, "PSM Ready response indication to cm");
  
  psm_rpt.psm_ready_rsp.status = SYS_PSM_STATUS_NOT_READY_NOT_IDLE;

  if(status == TRUE)
  {
     psm_rpt.psm_ready_rsp.status = SYS_PSM_STATUS_READY; 
  }

  psm_rpt.psm_ready_rsp.activate_timer =  activate_timer;
  psm_rpt.psm_ready_rsp.module = CM_PSM_MODULE_QMI;

  cm_client_send_psm_rpt(CM_CLIENT_PSM_READY_RSP,&psm_rpt);

  return;
} 

/*===========================================================================
FUNCTION      DS_QMI_CM_PSM_READY_IND

DESCRIPTION   This function notifies CM that DS is ready to enter PSM Mode
              and provide the activate timer in the indication
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_qmi_cm_psm_ready_ind
(
  uint32          activate_timer
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cm_generic_rpt_u_type  psm_rpt;

  memset(&psm_rpt, 0, sizeof(cm_generic_rpt_u_type));

  DS_MSG0(MSG_LEGACY_MED, "PSM Ready indication to cm");
  
  psm_rpt.psm_ready_ind.activate_timer =  activate_timer;
  psm_rpt.psm_ready_ind.module = CM_PSM_MODULE_QMI;

  cm_client_send_psm_rpt(CM_CLIENT_PSM_READY_IND,&psm_rpt);

  return;

}

/*===========================================================================
  FUNCTION QMI_DMSI_PSM_ENTER()

  DESCRIPTION
    Request to enter PSM (Power Save Mode). 

  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_psm_enter
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  dms_psm_enter_req_msg_v01  * req_msg;
  dms_psm_enter_resp_msg_v01   resp_msg;
  cm_psm_enter_req_s_type      cm_psm_enter_req;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(req_c_struct !=NULL);
  req_msg = (dms_psm_enter_req_msg_v01 *)req_c_struct;
  memset(&resp_msg, 0, sizeof(resp_msg));

  memset(&cm_psm_enter_req, 0, sizeof(cm_psm_enter_req));

  // Get request parameters
  if (req_msg->duration_in_psm_valid)
  {
    cm_psm_enter_req.apps_time = req_msg->duration_in_psm;
  } 
  DS_MSG1(MSG_LEGACY_MED, "qmi_dmsi_psm_enter  %d ",clnt_info->current_psm_state);

  if(dms_psm_capability == DMS_PSM_VERSION_2_V01)
  {
    if(req_msg->status_valid)
    {     
      if(clnt_info->current_psm_state != DMSI_PSM_DISABLE_STATE)
      {
         if(req_msg->status == TRUE) //psm accepted
         {
          //Check what was the old state of the client
           clnt_info->current_psm_state = DMSI_PSM_ENABLE_STATE;
           ds_qmi_cm_psm_ready_response(TRUE,req_msg->duration_in_psm);
         }
         else if(req_msg->status == FALSE)
         {
           clnt_info->current_psm_state = DMSI_PSM_DISABLE_STATE;        
           ds_qmi_cm_psm_ready_response(FALSE,req_msg->duration_in_psm);
         }
      }
      else if(clnt_info->current_psm_state == DMSI_PSM_DISABLE_STATE)
      {
        if(req_msg->status == TRUE) //psm accepted
        {
          clnt_info->current_psm_state = DMSI_PSM_ENABLE_STATE;
          ds_qmi_cm_psm_ready_ind(req_msg-> duration_in_psm);
        }
        else if(req_msg->status == FALSE)
        {
          ds_qmi_cm_psm_ready_response(FALSE,req_msg->duration_in_psm);
        }
      }
    } 
    else
    {
       clnt_info->current_psm_state = DMSI_PSM_ENABLE_STATE;
       ds_qmi_cm_psm_ready_response(TRUE,req_msg->duration_in_psm);
    }
    goto send_result;
  }
  // Send command to CM
  if (!ds_msgr_snd_msg_send(
                MM_CM_PSM_ENTER_REQ,
                (msgr_hdr_struct_type *)&cm_psm_enter_req,
                sizeof(cm_psm_enter_req)))
  {
    resp_msg.resp.error = QMI_ERR_INTERNAL_V01;
    resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
  }
  else
  {
    resp_msg.resp.error = QMI_ERR_NONE_V01;
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
    DS_MSG1(MSG_LEGACY_HIGH,"PSM enter cmd [%d] sent", MM_CM_PSM_ENTER_REQ);
  }
  
  send_result:
  // Send response
  qmi_dmsi_send_response(req_handle, msg_id, &resp_msg, sizeof(resp_msg));
  return QMI_CSI_CB_NO_ERR;

} /* qmi_dmsi_psm_enter() */

/*===========================================================================
  FUNCTION QMI_DMSI_GENERATE_PSM_STATUS_IND()

  DESCRIPTION
    Generate PSM status indication.

  PARAMETERS
    cmd_buf_ptr : PSM status cmd buf ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dmsi_generate_psm_status_ind
(
  qmi_dmsi_cmd_buf_type  * cmd_buf_ptr
)
{
  dms_psm_status_ind_msg_v01   ind_msg;
  qmi_dmsi_client_info_type  * cl_sp;
  qmi_csi_error                rc;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(cmd_buf_ptr && cmd_buf_ptr->cmd_id == QMI_CMD_DMS_PSM_STATUS_IND);
  memset(&ind_msg, 0, sizeof(ind_msg));

  // Construct indication
  switch (cmd_buf_ptr->data.psm_status.status)
  {
    case CM_PSM_STATUS_COMPLETED:
      ind_msg.psm_status = DMS_PSM_STATUS_COMPLETED_V01;
      break;
    case CM_PSM_STATUS_REJECTED:
      ind_msg.psm_status = DMS_PSM_STATUS_REJECTED_V01;
      break;
    case CM_PSM_STATUS_READY:
      ind_msg.psm_status = DMS_PSM_STATUS_READY_V01;
      break;
    case CM_PSM_STATUS_OOS:
      ind_msg.psm_status = DMS_PSM_STATUS_OOS_V01;
      break;
    case CM_PSM_STATUS_LIMITED_SERVICE:
      ind_msg.psm_status = DMS_PSM_STATUS_LIMITED_SERVICE_V01;
      break;
    case CM_PSM_STATUS_AUTO_READY:
      ind_msg.psm_status = DMS_PSM_STATUS_AUTO_READY_V01;
      break;

    default:
      DS_MSG1(MSG_LEGACY_ERROR,"Unknonw CM PSM status %d", ind_msg.psm_status);
      return;
  }

  if (ind_msg.psm_status == DMS_PSM_STATUS_REJECTED_V01)
  {
    ind_msg.reject_reason_valid = TRUE;
    switch (cmd_buf_ptr->data.psm_status.reject_reason)
    {
      case CM_PSM_REJECT_REASON_PSM_NO_ERROR:
        ind_msg.reject_reason = DMS_PSM_REJECT_REASON_NO_ERROR_V01;
        break;
      case CM_PSM_REJECT_REASON_PSM_NOT_ENABLED:
        ind_msg.reject_reason = DMS_PSM_REJECT_REASON_NOT_ENABLED_V01;
        break;
      case CM_PSM_REJECT_REASON_MODEM_NOT_READY:
        ind_msg.reject_reason = DMS_PSM_REJECT_REASON_MODEM_NOT_READY_V01;
        break;
      case CM_PSM_REJECT_REASON_DURATION_TOO_SHORT:
        ind_msg.reject_reason = DMS_PSM_REJECT_REASON_DURATION_TOO_SHORT_V01;
        break;
      case CM_PSM_REJECT_REASON_PSM_DURATION_CHANGED:
        ind_msg.reject_reason = DMS_PSM_REJECT_REASON_PSM_DURATION_CHANGED_V01;
        break;
      default:
        DS_MSG1(MSG_LEGACY_ERROR,"Unknonw CM PSM reject code %d", ind_msg.reject_reason);
        return;
    }
  }
  if(cmd_buf_ptr->data.psm_status.periodic_update_timer != (uint32)0xFFFFFFFF)
  {
    ind_msg.periodic_update_timer_valid = TRUE;
    ind_msg.periodic_update_timer = cmd_buf_ptr->data.psm_status.periodic_update_timer;
  }

  if(dms_psm_capability == DMS_PSM_VERSION_2_V01)
  {
    if(cmd_buf_ptr->data.psm_status.modem_only_timer != (uint32)0xFFFFFFFF)
    {
      ind_msg.modem_only_timer_valid = TRUE;
      ind_msg.modem_only_timer = cmd_buf_ptr->data.psm_status.modem_only_timer;
    }
  }

  // Send to all registered clients
  cl_sp = (qmi_dmsi_client_info_type *)q_check(&qmi_dms_state.client_q);
  while (cl_sp != NULL)
  {
    if (cl_sp->report_status.report_psm_status)
    {
      rc = qmi_csi_send_ind(cl_sp->clnt,
                            QMI_DMS_PSM_STATUS_IND_V01,
                            &ind_msg,
                            sizeof(ind_msg));
      if (rc != QMI_CSI_NO_ERR)
      {
        DS_MSG1(MSG_LEGACY_ERROR,"Failed to report PSM status, error %d", rc);
      }
    }
    cl_sp = q_next(&qmi_dms_state.client_q, &(cl_sp->link));
  }

} /* qmi_dmsi_generate_psm_status_ind() */

/*===========================================================================
  FUNCTION QMI_DMSI_GENERATE_PSM_CFG_CHANGE_IND()

  DESCRIPTION
    Generate PSM cfg changes indication.

  PARAMETERS
    cmd_buf_ptr : PSM cfg changes cmd buf ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dmsi_generate_psm_cfg_change_ind
(
  qmi_dmsi_cmd_buf_type  * cmd_buf_ptr
)
{
  dms_psm_cfg_params_change_ind_msg_v01   ind_msg;
  qmi_dmsi_client_info_type  * cl_sp;
  qmi_csi_error                rc;
  uint16                       change_mask;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(cmd_buf_ptr && cmd_buf_ptr->cmd_id == QMI_CMD_DMS_PSM_CFG_CHANGE_IND);
  memset(&ind_msg, 0, sizeof(ind_msg));

  change_mask = cmd_buf_ptr->data.psm_cfg_changes.change_mask;
  DS_MSG1(MSG_LEGACY_HIGH,"qmi_dmsi_generate_psm_cfg_change_ind: changemask 0x%x",
                     change_mask);

  if(change_mask & CM_PSM_CONFIG_INFO_MASK_PSM_ENABLED)
  {
    ind_msg.psm_enabled_valid = TRUE;
    ind_msg.psm_enabled = cmd_buf_ptr->data.psm_cfg_changes.psm_enabled;
  }
  if(change_mask & CM_PSM_CONFIG_INFO_MASK_ACTIVE_TIMER)
  {
    ind_msg.active_timer_valid = TRUE;
    ind_msg.active_timer = cmd_buf_ptr->data.psm_cfg_changes.active_timer;    
  }
  if(change_mask & CM_PSM_CONFIG_INFO_MASK_PERIODIC_UPDATE_TIMER)
  {
    ind_msg.periodic_update_timer_valid = TRUE;
    ind_msg.periodic_update_timer =
                cmd_buf_ptr->data.psm_cfg_changes.periodic_update_timer;
  }

  // Send to all registered clients
  cl_sp = (qmi_dmsi_client_info_type *)q_check(&qmi_dms_state.client_q);
  while (cl_sp != NULL)
  {
    if (cl_sp->report_status.report_psm_cfg_change)
    {
      rc = qmi_csi_send_ind(cl_sp->clnt,
                            QMI_DMS_PSM_CFG_PARAMS_CHANGE_IND_V01,
                            &ind_msg,
                            sizeof(ind_msg));
      if (rc != QMI_CSI_NO_ERR)
      {
        DS_MSG1(MSG_LEGACY_ERROR,"Failed to report PSM status, error %d", rc);
      }
    }
    cl_sp = q_next(&qmi_dms_state.client_q, &(cl_sp->link));
  }
}

static qmi_csi_cb_error    qmi_dmsi_get_capability
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  dms_get_capability_req_msg_v01       *req_msg = (dms_get_capability_req_msg_v01 *)req_c_struct;
  dms_get_capability_resp_msg_v01       resp_msg;
  int8                                  cm_psm_version =0;
/*-------------------------------------------------------*/
  memset(&resp_msg ,0,sizeof(dms_get_capability_resp_msg_v01));
  resp_msg.psm_capability_valid = TRUE;
  resp_msg.psm_capability = DMS_PSM_VERSION_UNSPEC_V01;
  cm_psm_version = cm_get_psm_sw_ver();

  DS_MSG1(MSG_LEGACY_HIGH,"qmi_dmsi_get_capability cm_psm_version %d",cm_psm_version);

  if(req_msg->te_psm_capability_valid)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"qmi_dmsi_get_capability te_psm_capability %d",req_msg->te_psm_capability);
    
    if(req_msg->te_psm_capability == DMS_PSM_VERSION_UNSPEC_V01)
       goto send_result;
    else if(req_msg->te_psm_capability == DMS_PSM_VERSION_1_V01)
    {
      //Call CM api to check which version the CM supports for PSM 
      //return the lower version of the two.
      if(cm_psm_version == DMS_PSM_VERSION_2_V01)
          resp_msg.psm_capability = DMS_PSM_VERSION_1_V01;
    }
    else if(req_msg->te_psm_capability == DMS_PSM_NOT_SUPPORTED_V01)
    {
      resp_msg.psm_capability = DMS_PSM_NOT_SUPPORTED_V01;
    }
    else if(req_msg->te_psm_capability == DMS_PSM_VERSION_2_V01)
    {
     //Call CM api to check which version the CM supports for PSM
     //return the lower capability of two
      if(cm_psm_version == DMS_PSM_VERSION_2_V01)
          resp_msg.psm_capability = DMS_PSM_VERSION_2_V01;
    }
  }
  dms_psm_capability = resp_msg.psm_capability;
  
send_result:
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                          sizeof(resp_msg));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_DMSI_GENERATE_PSM_GET_CFG_PARAMS_RESP()

  DESCRIPTION
    Generate PSM get cfg params response.

  PARAMETERS
    cmd_buf_ptr : PSM status cmd buf ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_dmsi_generate_psm_get_cfg_params_resp
(
  qmi_dmsi_cmd_buf_type  * cmd_buf_ptr
)
{
  dms_psm_get_cfg_params_resp_msg_v01  resp_msg;
  ds_qmi_dms_async_cmd_type          * async_cmd_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(cmd_buf_ptr &&
         cmd_buf_ptr->cmd_id == QMI_CMD_DMS_PSM_GET_CFG_PARAMS_RESP);
  memset(&resp_msg, 0, sizeof(resp_msg));

  // Construct response
  resp_msg.resp.error = QMI_ERR_NONE_V01;
  resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;

  resp_msg.psm_enabled_valid = TRUE;
  resp_msg.psm_enabled = cmd_buf_ptr->data.psm_params.enabled ? 1 : 0;

  if (resp_msg.psm_enabled)
  {
    resp_msg.duration_threshold_valid = TRUE;
    resp_msg.duration_threshold = cmd_buf_ptr->data.psm_params.threshold;

    resp_msg.duration_due_to_oos_valid = TRUE;
    resp_msg.duration_due_to_oos = cmd_buf_ptr->data.psm_params.duration_oos;

    resp_msg.randomization_window_valid = TRUE;
    resp_msg.randomization_window = cmd_buf_ptr->data.psm_params.randomization_win;

    resp_msg.active_timer_valid = TRUE;
    resp_msg.active_timer = cmd_buf_ptr->data.psm_params.active_timer;
    /*FIXME: 0xFFFFFF --> means it is not valid/default value. Need to send??*/
    resp_msg.periodic_update_timer_valid = TRUE;
    resp_msg.periodic_update_timer = cmd_buf_ptr->data.psm_params.periodic_update_timer;

    resp_msg.early_wakeup_time_valid = TRUE;
    resp_msg.early_wakeup_time = cmd_buf_ptr->data.psm_params.early_wakeup_time;
	
  }

  // Send to all pending requests
  while ((async_cmd_p = (ds_qmi_dms_async_cmd_type *)
                          q_get(&qmi_dms_state.psm_get_cfg_params_q)) != NULL)
  {
    if (qmi_dms_check_valid_async_cmd_buf(async_cmd_p))
    {
      qmi_dmsi_send_response(async_cmd_p->req_handle,
                             QMI_DMS_PSM_GET_CFG_PARAMS_RESP_V01,
                             &resp_msg,
                             sizeof(resp_msg));
    }
    DS_SYSTEM_HEAP_MEM_FREE(async_cmd_p);
  }

} /* qmi_dmsi_generate_psm_get_cfg_params_resp() */

/*===========================================================================
  FUNCTION QMI_DMSI_SET_APP_PRIORITY()

  DESCRIPTION
  Sets the app priority to modem
  
  PARAMETERS
     req_c_struct              :         Request Message with all TLV
     req_handle                :         Request Handle
     clnt_info                 :         Client Info Handle
     msg_id                    :         Message ID
     req_c_struct_len          :         length of request message
     service_handle            :         wds service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error  qmi_dmsi_set_app_priority
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  dms_set_app_priority_req_msg_v01        *req_msg;
  dms_set_app_priority_resp_msg_v01        resp_msg;
  qmi_error_type_v01                       errval = QMI_ERR_NONE_V01;
/*--------------------------------------------------*/
  
  memset(&resp_msg,0,sizeof(dms_set_app_priority_resp_msg_v01));

  DS_ASSERT(req_c_struct &&(req_c_struct_len > 0)&&clnt_info && service_handle);
  
  req_msg  = (dms_set_app_priority_req_msg_v01*)req_c_struct;

  if(!(req_msg->prioritized_app == DMS_MARB_CLI_GROUP_GNSS_V01 ||
            req_msg->prioritized_app == DMS_MARB_CLI_GROUP_WWAN_V01))
  {
     errval = QMI_ERR_INVALID_ARG_V01;
     goto send_result;
  }
 
  DS_MSG1(MSG_LEGACY_HIGH,"APP PRIORITY = %d", req_msg->prioritized_app);
  if ( marb_update_app_priority(req_msg->prioritized_app) != MARB_RESULT_ACCEPT )
  {
    errval = QMI_ERR_INCOMPATIBLE_STATE_V01;
  }

send_result:
  
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval; 
  DS_MSG2(MSG_LEGACY_MED,"qmi_dmsi_set_app_priority result %d errval %d",
          resp_msg.resp.result, errval);
  
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dms_set_app_priority_resp_msg_v01));
  
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_DMSI_GET_APP_PRIORITY_INFO()

  DESCRIPTION
    Gets the loaded technology of the application (GNSS/WWAN)
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_get_app_priority_info(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  dms_get_app_priority_info_resp_msg_v01     resp_msg;
  qmi_error_type_v01                         errval = QMI_ERR_NONE_V01;
  dms_marb_client_group_type_v01             app_priority = DMS_MARB_CLI_GROUP_WWAN_V01;
  dms_loaded_tech_type_v01                   loaded_tech = DMS_LOADED_TECH_NONE_V01;
  marb_get_info_input_t                      marb_input;
  marb_get_info_output_t                     marb_output;
  /*--------------------------------------------------*/

  memset(&resp_msg,0,sizeof(dms_get_app_priority_info_resp_msg_v01));
  memset(&marb_input,0,sizeof(marb_get_info_input_t));
  memset(&marb_output,0,sizeof(marb_get_info_output_t));

  //Get Current App_priority
  marb_input.info_type = MARB_CURRENT_APP_PRIORITY;
  
  if(MARB_RESULT_ACCEPT != marb_get_info(&marb_input,&marb_output) 
  	|| marb_output.info_type != MARB_CURRENT_APP_PRIORITY)
  {
    errval = QMI_ERR_INTERNAL_V01;
	goto send_result;
  }
  app_priority = (dms_marb_client_group_type_v01)marb_output.info.current_app_priority;

  //Get Current Loaded_Tech_State
  memset(&marb_input,0,sizeof(marb_get_info_input_t));
  memset(&marb_output,0,sizeof(marb_get_info_output_t));
  marb_input.info_type = MARB_CURRENT_LOAD_STATE;
  
  if(MARB_RESULT_ACCEPT != marb_get_info(&marb_input,&marb_output) 
  	|| marb_output.info_type != MARB_CURRENT_LOAD_STATE)
  {
    errval = QMI_ERR_INTERNAL_V01;
	goto send_result;
  }
  loaded_tech = (dms_loaded_tech_type_v01)marb_output.info.current_loaded_tech;
  
  resp_msg.prioritized_app_valid = TRUE;
  resp_msg.prioritized_app = app_priority;

  resp_msg.loaded_tech_valid = TRUE;
  resp_msg.loaded_tech = loaded_tech;
  
  send_result:
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                   : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error = errval;
  qmi_dmsi_send_response(req_handle,
                         msg_id,
                         &resp_msg,
                         sizeof(dms_get_app_priority_info_resp_msg_v01));
  DS_MSG4(MSG_LEGACY_HIGH,"APP PRIORITY = %d LOADED TECH = %d result %d error %d", 
  	                       app_priority, loaded_tech, resp_msg.resp.result, errval);
  return QMI_CSI_CB_NO_ERR;
  
}/* dms_get_app_priority_info */

#ifdef FEATURE_REF_TRACKER
static qmi_error_type_v01  qmi_dmsi_convert_cm_err
(
  cm_sns_cmd_err_e_type              cm_sns_err
)
{
  DS_MSG1(MSG_LEGACY_HIGH,"qmi_dmsi_convert_cm_err cm_err %d",cm_sns_err);
  switch(cm_sns_err)
  {  
    case CM_SNS_CMD_ERR_NOERR:
        return QMI_ERR_NONE_V01;
    case CM_SNS_CMD_ERR_NO_MEMORY:
        return QMI_ERR_NO_MEMORY_V01;
    case CM_SNS_CMD_ERR_DATA_REQ_NOT_ALLOW:
        return QMI_ERR_INVALID_OPERATION_V01;
    case CM_SNS_CMD_ERR_CONFIG_INVALID:
        return QMI_ERR_INVALID_ARG_V01;
    case CM_SNS_CMD_ERR_ARRAY_PTR_NULL:
        return QMI_ERR_INTERNAL_V01;
    case CM_SNS_CMD_ERR_OTHER:
    case CM_SNS_CMD_ERR_MAX:        
        return QMI_ERR_INTERNAL_V01;
  }
  return QMI_ERR_NONE_V01;
}

/*===========================================================================
  FUNCTION QMI_DMSI_SENSOR_POLICY_MET_INDICATION()

  DESCRIPTION
    Sends the indication about the policy met
    
  PARAMETERS
  cmd_buf_ptr :  Sensor cfg    cmd buf ptr

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static   void qmi_dmsi_sensor_callback
(
  cm_sns_cmd_e_type                  cm_sns_cmd,
  cm_sns_cmd_err_e_type              cm_sns_err
)
{
  dms_set_sensor_configuration_resp_msg_v01   config_msg;
  dms_get_sensor_data_resp_msg_v01            data_msg;
  qmi_dmsi_client_info_type                  *cl_sp;
  qmi_error_type_v01                          errval = QMI_ERR_NONE_V01;
  dms_reset_sensor_config_data_resp_msg_v01   reset_resp;
  /*---------------------------------------------------------------*/
  errval =  qmi_dmsi_convert_cm_err(cm_sns_err);
  switch(cm_sns_cmd)
  {
    case CM_SNS_CMD_SET_CONFIG:
    {    
      memset(&config_msg,0,sizeof(dms_set_sensor_configuration_resp_msg_v01));
      config_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                            : QMI_RESULT_FAILURE_V01);
      config_msg.resp.error  = errval; 
      // Send to all registered clients
      cl_sp = (qmi_dmsi_client_info_type *)q_check(&qmi_dms_state.client_q);
      while (cl_sp != NULL)
      {
         if (cl_sp->sns_set_config_req_handle != NULL)
         {
            qmi_csi_send_resp(cl_sp->sns_set_config_req_handle,
                              QMI_DMS_SET_SENSOR_CONFIGURATION_RESP_V01,
                              &config_msg, 
                              sizeof(dms_set_sensor_configuration_resp_msg_v01));
            cl_sp->sns_set_config_req_handle= NULL;
         }
         cl_sp = q_next(&qmi_dms_state.client_q, &(cl_sp->link));
      }
    }
    break;
    case CM_SNS_CMD_DATA_REQ:
    {
      if(errval != QMI_RESULT_SUCCESS_V01)
      {
        data_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                                  : QMI_RESULT_FAILURE_V01);
        data_msg.resp.error   = errval; 
      
        // Send to all registered clients
        cl_sp = (qmi_dmsi_client_info_type *)q_check(&qmi_dms_state.client_q);
        while (cl_sp != NULL)
        {
           if (cl_sp->sns_get_data_req_handle != NULL)
           {
              qmi_csi_send_resp(cl_sp->sns_get_data_req_handle,
                                QMI_DMS_GET_SENSOR_DATA_REQ_V01,
                                &data_msg, 
                                sizeof(dms_get_sensor_data_resp_msg_v01));
              cl_sp->sns_get_data_req_handle = NULL;
           }
           cl_sp = q_next(&qmi_dms_state.client_q, &(cl_sp->link));
        } 
      }
    }
    break;
    case CM_SNS_CMD_RESET_CONFIG:
    {
      reset_resp.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                            : QMI_RESULT_FAILURE_V01);
      reset_resp.resp.error  = errval; 
      // Send to all registered clients
      cl_sp = (qmi_dmsi_client_info_type *)q_check(&qmi_dms_state.client_q);
      while (cl_sp != NULL)
      {
         if (cl_sp->sns_reset_config_req_handle != NULL)
         {
            qmi_csi_send_resp(cl_sp->sns_reset_config_req_handle,
                              QMI_DMS_RESET_SENSOR_CONFIG_DATA_REQ_V01,
                              &reset_resp, 
                              sizeof(dms_reset_sensor_config_data_resp_msg_v01));
            cl_sp->sns_reset_config_req_handle = NULL;
         }
         cl_sp = q_next(&qmi_dms_state.client_q, &(cl_sp->link));
      }
    }
    break;

    default:
    break;
  }
}

/*===========================================================================
  FUNCTION QMI_DMSI_SET_SENSOR_CONFIGURATION()

  DESCRIPTION
    Sets the sensor policy information to modem for sensor policy
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_set_sensor_configuration
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  dms_set_sensor_configuration_req_msg_v01   *req_msg;
  dms_set_sensor_configuration_resp_msg_v01   resp_msg;
  qmi_error_type_v01                          errval = QMI_ERR_NONE_V01;
  cm_sns_config_data_s_type                   sensor_config[CM_SNS_TYPE_ENV_SENSOR_MAX];
  uint8                                       policy_len = 0;
  uint8                                       config_list =0; 
/*--------------------------------------------------*/
  
  memset(&resp_msg,0,sizeof(dms_set_sensor_configuration_resp_msg_v01));
  DS_ASSERT(req_c_struct && (req_c_struct_len > 0)&&clnt_info &&service_handle);
  memset(&sensor_config,0,(sizeof(cm_sns_config_data_s_type)*CM_SNS_TYPE_ENV_SENSOR_MAX));
  req_msg = (dms_set_sensor_configuration_req_msg_v01 *)req_c_struct;
  if(clnt_info->sns_set_config_req_handle !=NULL)
  {  
    DS_MSG0(MSG_LEGACY_HIGH,"Set Configuration command is already in progress");
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  if(req_msg->sensor_id != DMS_SENSOR_TYPE_ALL_V01)
  {
     errval = QMI_ERR_INVALID_ARG_V01;
     goto send_result;
  }
  if(req_msg->sensor_config_list_valid)
  {
    policy_len = req_msg->sensor_config_list_len;
    for(config_list =0 ;config_list <req_msg->sensor_config_list_len ;config_list++)
    {
       sensor_config[config_list].sns_id = 
                              req_msg->sensor_config_list[config_list].sensor_id;
       sensor_config[config_list].sns_max_batch_cnt =
                              req_msg->sensor_config_list[config_list].sensor_batch_cnt;
       sensor_config[config_list].sns_interval =
                             req_msg->sensor_config_list[config_list].reporting_interval;
       if(req_msg->sensor_config_list[config_list].sensor_policy_len >CM_MAX_SNS_DIMENSION)
       {
          DS_MSG0(MSG_LEGACY_HIGH,"Max policy allwed at lower layers is 2");
          errval = QMI_ERR_INVALID_ARG_V01;
          goto send_result;
       }
       sensor_config[config_list].sns_policy.sns_policy_elem[0].greater_value =
                 req_msg->sensor_config_list[config_list].sensor_policy[0].greater_value;
       sensor_config[config_list].sns_policy.sns_policy_elem[0].lower_value =
                        req_msg->sensor_config_list[config_list].sensor_policy[0].lower_value;
       sensor_config[config_list].sns_policy.sns_policy_elem[0].delta_value =
                 req_msg->sensor_config_list[config_list].sensor_policy[0].delta_value;
       if(req_msg->sensor_config_list[config_list].sensor_policy_len > 0)
       {
         sensor_config[config_list].sns_policy.sns_policy_elem[1].greater_value =
                 req_msg->sensor_config_list[config_list].sensor_policy[1].greater_value;
         sensor_config[config_list].sns_policy.sns_policy_elem[1].lower_value =
                        req_msg->sensor_config_list[config_list].sensor_policy[1].lower_value;
         sensor_config[config_list].sns_policy.sns_policy_elem[1].delta_value =
                 req_msg->sensor_config_list[config_list].sensor_policy[1].delta_value;
       }
    }
  }
  clnt_info->sns_set_config_req_handle = req_handle;
  if(TRUE !=  cm_sns_cmd_set_config(qmi_dmsi_sensor_callback,
                                        dssysmgr_get_cm_client_id(),
                                       sensor_config,
                                        policy_len
                                       ))
  {
    clnt_info->sns_set_config_req_handle = NULL;
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
 return QMI_CSI_CB_NO_ERR;
 
send_result:
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                            : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval; 
  DS_MSG2(MSG_LEGACY_HIGH,"qmi_dmsi_set_sensor_configuration result %d errval %d",
          resp_msg.resp.result, errval);
          
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dms_set_sensor_configuration_resp_msg_v01));
          
  return QMI_CSI_CB_NO_ERR;
}

dms_sensor_id_mask_v01 dms_convert_cm_sensor_mask
( 
  uint32 cm_sensor_mask
)
{
  dms_sensor_id_mask_v01 ret_mask = 0;
  if(cm_sensor_mask & (1 << CM_SNS_TYPE_ALS))
    ret_mask |= DMS_MASK_SENSOR_ID_ALS_V01;
  if(cm_sensor_mask & (1<<CM_SNS_TYPE_PRESSURE))  	
    ret_mask |= DMS_MASK_SENSOR_ID_PRESSURE_V01;
  if(cm_sensor_mask &(1<< CM_SNS_TYPE_HUMIDITY))
    ret_mask |= DMS_MASK_SENSOR_ID_HUMIDITY_V01;
  if(cm_sensor_mask & (1<<CM_SNS_TYPE_TEMPERATURE))
    ret_mask |= DMS_MASK_SENSOR_ID_TEMPERATURE_V01;
  if(cm_sensor_mask & (1<<CM_SNS_TYPE_AMD))
    ret_mask |= DMS_MASK_SENSOR_ID_AMD_V01;
  if(cm_sensor_mask & (1<<CM_SNS_TYPE_SMD))
    ret_mask |= DMS_MASK_SENSOR_ID_SMD_V01;
  
  DS_MSG1(MSG_LEGACY_HIGH,"qmi_dmsi_set_sensor_configuration ret_mask %d",
           ret_mask);
  
 return ret_mask;
}

uint32 dms_convert_dms_sensor_mask
( 
  dms_sensor_id_mask_v01 qmi_sensor_mask
)
{
  uint32 ret_mask =0;
  if(qmi_sensor_mask & DMS_MASK_SENSOR_ID_ALS_V01)
    ret_mask |= (1<< CM_SNS_TYPE_ALS);
  if(qmi_sensor_mask & DMS_MASK_SENSOR_ID_PRESSURE_V01)  	
    ret_mask |= (1<< CM_SNS_TYPE_PRESSURE);
  if(qmi_sensor_mask & DMS_MASK_SENSOR_ID_HUMIDITY_V01)
    ret_mask |= (1<< CM_SNS_TYPE_HUMIDITY);
  if(qmi_sensor_mask & DMS_MASK_SENSOR_ID_TEMPERATURE_V01)
    ret_mask |= (1<< CM_SNS_TYPE_TEMPERATURE);
  if(qmi_sensor_mask & DMS_MASK_SENSOR_ID_AMD_V01)
    ret_mask |= (1<< CM_SNS_TYPE_AMD);
  if(qmi_sensor_mask & DMS_MASK_SENSOR_ID_SMD_V01)
    ret_mask |= (1<< CM_SNS_TYPE_SMD);
  
 return ret_mask;
}

/*===========================================================================
  FUNCTION QMI_DMSI_GET_SENSOR_DATA()

  DESCRIPTION
    Gets the sensor information from modem based on the sensor ID pass
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_get_sensor_data
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  dms_get_sensor_data_req_msg_v01   *req_msg;
  dms_get_sensor_data_resp_msg_v01   resp_msg;
  qmi_error_type_v01                 errval = QMI_ERR_NONE_V01;
  cm_sns_config_data_s_type          sensor_config;
/*--------------------------------------------------*/

  memset(&resp_msg,0,sizeof(dms_get_sensor_data_resp_msg_v01));
  DS_ASSERT(req_c_struct &&(req_c_struct_len > 0)&&clnt_info && service_handle);
  memset(&sensor_config,0,sizeof(cm_sns_config_data_s_type));
  req_msg = (dms_get_sensor_data_req_msg_v01 *)req_c_struct;
  if(clnt_info->sns_get_data_req_handle != NULL)
  {  
    DS_MSG0(MSG_LEGACY_HIGH,"Command is already in progress");
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  if(req_msg->sensor_id_mask > DMS_MASK_SENSOR_ID_AMD_V01)
  {
     errval = QMI_ERR_INVALID_ARG_V01;
     goto send_result;
  }   
  clnt_info->sns_get_data_req_handle = req_handle;
  if(TRUE !=  cm_sns_cmd_data_req(qmi_dmsi_sensor_callback,
                                    dssysmgr_get_cm_client_id(),
                                    dms_convert_dms_sensor_mask(req_msg->sensor_id_mask)
                                    ))
  {
    clnt_info->sns_get_data_req_handle = NULL;
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  return QMI_CSI_CB_NO_ERR;
  
send_result:
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                            : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval; 
  DS_MSG2(MSG_LEGACY_HIGH,"qmi_dmsi_get_sensor_data result %d errval %d",
          resp_msg.resp.result, errval);
          
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dms_get_sensor_data_resp_msg_v01));
          
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_DMSI_RESET_SENSOR_CONFIGURATION()

  DESCRIPTION
    Gets the sensor information from modem based on the sensor ID pass
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_reset_sensor_configuration
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  dms_reset_sensor_config_data_req_msg_v01   *req_msg;
  dms_reset_sensor_config_data_resp_msg_v01   resp_msg;
  qmi_error_type_v01                          errval = QMI_ERR_NONE_V01;
/*--------------------------------------------------*/
  memset(&resp_msg,0,sizeof(dms_reset_sensor_config_data_resp_msg_v01));

  DS_ASSERT(req_c_struct &&(req_c_struct_len > 0)&&clnt_info && service_handle);
  req_msg = (dms_reset_sensor_config_data_req_msg_v01 *)req_c_struct;  

  if(clnt_info->sns_reset_config_req_handle != NULL)
  {  
    DS_MSG0(MSG_LEGACY_HIGH,"Reset Command is already in progress");
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  clnt_info->sns_reset_config_req_handle = req_handle;
  if(TRUE !=  cm_sns_cmd_reset_config(qmi_dmsi_sensor_callback,
                                      dssysmgr_get_cm_client_id()
                                      ))
  {
    clnt_info->sns_reset_config_req_handle = NULL;
    errval = QMI_ERR_INTERNAL_V01;
    goto send_result;
  }
  return QMI_CSI_CB_NO_ERR;

 send_result:
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                            : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval; 
  DS_MSG2(MSG_LEGACY_HIGH,"qmi_dmsi_reset_sensor_configuration result %d errval %d",
          resp_msg.resp.result, errval);
          
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dms_reset_sensor_config_data_resp_msg_v01));


  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_DMSI_GET_LAST_POLICY_MET_INFO()

  DESCRIPTION
    Gets the last sensor information from modem if missed the sensor indication
    
  PARAMETERS
    req_c_struct       :       Request Message with all TLV
    req_handle         :       Request Handle
    clnt_info          :       Client Info Handle
    msg_id             :       Message ID
    req_c_struct_len   :       length of request message
    service_handle     :       DMS service handle

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_get_last_policy_met_info
(
  qmi_dmsi_client_info_type*  clnt_info,
  qmi_req_handle              req_handle,
  int                         msg_id,
  void*                       req_c_struct,
  int                         req_c_struct_len,
  void*                       service_handle
)
{
  dms_get_last_policy_met_info_req_msg_v01   *req_msg;
  dms_get_last_policy_met_info_resp_msg_v01   resp_msg;
  qmi_error_type_v01                          errval = QMI_ERR_NONE_V01;
  cm_sns_event_info_s_type                    event_info;
/*--------------------------------------------------*/
  memset(&resp_msg,0,sizeof(dms_get_last_policy_met_info_resp_msg_v01));
  DS_ASSERT(req_c_struct &&(req_c_struct_len > 0)&&clnt_info && service_handle);
  
  req_msg = (dms_get_last_policy_met_info_req_msg_v01 *)req_c_struct;
 
  event_info = cm_sns_get_event_info();
  DS_MSG2(MSG_LEGACY_HIGH,"qmi_dmsi_get_last_policy_met_info result %d errval %d",
          event_info.policy_met_sns_mask, event_info.batch_full_sns_mask);
  resp_msg.policy_met_sensor_mask_valid = TRUE;
  resp_msg.policy_met_sensor_mask =  dms_convert_cm_sensor_mask(event_info.policy_met_sns_mask);
  resp_msg.batch_full_sensor_mask_valid =TRUE;
  resp_msg.batch_full_sensor_mask = dms_convert_cm_sensor_mask(event_info.batch_full_sns_mask);
  
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                            : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error  = errval; 
  DS_MSG2(MSG_LEGACY_HIGH,"qmi_dmsi_get_last_policy_met_info result %d errval %d",
          resp_msg.resp.result, errval);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg, 
                    sizeof(dms_get_last_policy_met_info_resp_msg_v01));
  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================
  FUNCTION QMI_DMSI_SENSOR_POLICY_MET_INDICATION()

  DESCRIPTION
    Sends the indication about the policy met
    
  PARAMETERS
  cmd_buf_ptr :  Sensor cfg    cmd buf ptr

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_sensor_policy_met_indication
(
  qmi_dmsi_cmd_buf_type *cmd_buf_ptr
)
{
  dms_sensor_policy_met_ind_msg_v01           indication;
  qmi_dmsi_client_info_type                  *cl_sp;  
  qmi_csi_error                               rc;

  uint32                                      batch_full_sns_mask =0;
  uint32                                      policy_sensor_mask =0;
/*--------------------------------------------------*/
  memset(&indication,0,sizeof(dms_sensor_policy_met_ind_msg_v01));
  DS_ASSERT(cmd_buf_ptr != NULL);
  batch_full_sns_mask   = cmd_buf_ptr->data.sensor_policy_met.sns_indication.batch_full_sns_mask;
  policy_sensor_mask = cmd_buf_ptr->data.sensor_policy_met.sns_indication.policy_met_sns_mask;
  
  DS_MSG2(MSG_LEGACY_HIGH,"qmi_dmsi_sensor_policy_met_indication "
          "policy_sensor_mask %d batch_full_sns_mask %d ",policy_sensor_mask,batch_full_sns_mask);  
 
  indication.policy_met_sensor_mask =  dms_convert_cm_sensor_mask(policy_sensor_mask);

  if(indication.policy_met_sensor_mask)
      indication.policy_met_sensor_mask_valid = TRUE;

  indication.batch_full_sensor_mask = dms_convert_cm_sensor_mask(batch_full_sns_mask);

  if(indication.batch_full_sensor_mask)
     indication.batch_full_sensor_mask_valid =TRUE;      
  if((indication.policy_met_sensor_mask_valid)||(indication.batch_full_sensor_mask_valid))
  {
    // Send to all registered clients
    cl_sp = (qmi_dmsi_client_info_type *)q_check(&qmi_dms_state.client_q);
    while (cl_sp != NULL)
    {
      if (cl_sp->report_status.report_sensor_policy_met_ind)
      {
        rc = qmi_csi_send_ind(cl_sp->clnt,
                              QMI_DMS_SENSOR_POLICY_MET_IND_V01,
                              &indication,
                              sizeof(indication));
        if (rc != QMI_CSI_NO_ERR)
        {
          DS_MSG1(MSG_LEGACY_ERROR,"Failed to report PSM status, error %d", rc);
        }
      }
      cl_sp = q_next(&qmi_dms_state.client_q, &(cl_sp->link));
    }
   }
   return QMI_CSI_CB_NO_ERR;
}


/*===========================================================================
  FUNCTION QMI_DMSI_SNESOR_GET_DATA_RESP()

  DESCRIPTION
    Sends the data      response about the policy met
    
  PARAMETERS
  cmd_buf_ptr :  Sensor cfg    cmd buf ptr

  RETURN VALUE
    qmi_csi_cb_error types.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_dmsi_sensor_get_data_resp
(
  qmi_dmsi_cmd_buf_type *cmd_buf_ptr
)
{
  dms_get_sensor_data_resp_msg_v01               resp_msg;  
  uint8                                          dimension_cnt =0;
  qmi_dmsi_client_info_type                     *cl_sp;      
  cm_qmi_sensor_data_rsp_s_type                 *cm_sensor_data_resp;  
  uint8                                          resp_cnt;
  uint8                                          sns_len =0;
  uint8                                          batch_len =0;  
  qmi_error_type_v01                             errval = QMI_ERR_NONE_V01;
/*----------------------------------------------------------------*/
  memset(&resp_msg,0,sizeof(dms_get_sensor_data_resp_msg_v01));
  DS_ASSERT(cmd_buf_ptr != NULL);
  
  DS_MSG0(MSG_LEGACY_HIGH,"qmi_dmsi_sensor_get_data_resp");
  
  resp_cnt = cmd_buf_ptr->data.sensor_data_resp.resp_len;
  cm_sensor_data_resp = cmd_buf_ptr->data.sensor_data_resp.sns_arr;
  
  resp_msg.resp.result = (errval == QMI_ERR_NONE_V01 ? QMI_RESULT_SUCCESS_V01
                            : QMI_RESULT_FAILURE_V01);
  resp_msg.resp.error      = errval;
  
  resp_msg.sensor_info_valid = TRUE;
  resp_msg.sensor_info_len = resp_cnt;

  for(sns_len = 0;sns_len< resp_msg.sensor_info_len ;sns_len++)
  {
    resp_msg.sensor_info[sns_len].sensor_id = 
         (dms_sensor_type_id_v01)cm_sensor_data_resp->sensor_id;
    
    resp_msg.sensor_info[sns_len].sample_list_len       = 
         cm_sensor_data_resp->batch_count;
    
    for(batch_len = 0;batch_len <resp_msg.sensor_info[sns_len].sample_list_len;batch_len++)
    {
       resp_msg.sensor_info[sns_len].sample_list[batch_len].\
        sensor_dimension_data_len =       cm_sensor_data_resp->sample_batch[batch_len].\
                        dimension_data_len;
       
       for(dimension_cnt =0 ;dimension_cnt < resp_msg.sensor_info[sns_len].sample_list[batch_len].\
                               sensor_dimension_data_len;dimension_cnt++)
       {
         resp_msg.sensor_info[sns_len].sample_list[batch_len].      \
                   sensor_dimension_data[dimension_cnt] = 
                    cm_sensor_data_resp->sample_batch[batch_len].\
                                 sns_dimension_data[dimension_cnt];
       }
       resp_msg.sensor_info[sns_len].sample_list[batch_len].timestamp =
                    cm_sensor_data_resp->sample_batch[batch_len].   timestamp;
    }
  }
 // Send to all registered clients
  cl_sp = (qmi_dmsi_client_info_type *)q_check(&qmi_dms_state.client_q);
  while (cl_sp != NULL)
  {
    if (cl_sp->sns_get_data_req_handle != NULL )
    {
      qmi_csi_send_resp(cl_sp->sns_get_data_req_handle,
                        QMI_DMS_GET_SENSOR_DATA_REQ_V01,
                        &resp_msg, 
                        sizeof(dms_get_sensor_data_resp_msg_v01));
      cl_sp->sns_get_data_req_handle = NULL;
    }
    cl_sp = q_next(&qmi_dms_state.client_q, &(cl_sp->link));
  }   
  return QMI_CSI_CB_NO_ERR;
}
#endif
