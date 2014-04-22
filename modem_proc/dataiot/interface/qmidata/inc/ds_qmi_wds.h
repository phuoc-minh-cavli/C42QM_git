#ifndef _DS_QMI_WDS_H
#define _DS_QMI_WDS_H
/*===========================================================================

                         D S _ Q M I _ W D S . H

DESCRIPTION

 The Data Services Qualcomm Wireless Data Services lite MSM Interface header file.
EXTERNALIZED FUNCTIONS

   qmi_wds_init()
   
     Initialize the QMI-WDS service and register with QCSI

Copyright (c) 2019 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/qmidata/inc/ds_qmi_wds.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/29/17    mkumarsa     Created module
===========================================================================*/

#include "wireless_data_service_v01.h"
#include "ds_fwk.h"
#include "ds_defs.h"
#include "dstaski.h"

/*===========================================================================
  FUNCTION QMI_WDS_INIT()

  DESCRIPTION
    Initialize the QMI-WDS service and register with QCSI

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_wds_init 
(
  void
);

  
#define WDS_EXT_IPV6_ADDRS_MAX_V01 30

#define WDSI_EMBEDDED_EP_ID 67108865
#define EP_TYPE_BAM_DEMUX  83886080

#define QMI_WDS_INVALID_INST  -1


/*
typedef struct
{
  union
  {
    wds_create_profile_req_msg_ex_v01          *create_req;
    wds_modify_profile_settings_req_msg_ex_v01 *modify_req;
  } profile_type;
} qmi_wdsi_profile_op;*/    

/*
 beyond 512 bytes. In case you need a command buffer larger than 512 bytes,
 * declare a separate structure. */
typedef struct
{
  uint16  cmd_id; /* qmi_cmd_id_e_type */
  union
  {
   int i; //Temporar
    //wds_system_status_ind_msg_v01 system_status;
    //wds_nipd_downlink_data_ind_msg_v01 nipd_downlink_data;
  } data;
} qmi_wdsi_cmd_buf_type;

/*---------------------------------------------------------------------------
	WDS call end reason enum type
---------------------------------------------------------------------------*/
typedef enum
{

  QMI_WDS_CALL_END_REASON_MIN                     = 0,
  QMI_WDS_CALL_END_REASON_UNSPECIFIED             = 1,
  QMI_WDS_CALL_END_REASON_CLIENT_END              = 2,
  QMI_WDS_CALL_END_REASON_NO_SRV                  = 3,
  QMI_WDS_CALL_END_REASON_FADE                    = 4,
  QMI_WDS_CALL_END_REASON_REL_NORMAL              = 5,
  QMI_WDS_CALL_END_REASON_ACC_IN_PROG             = 6,
  QMI_WDS_CALL_END_REASON_ACC_FAIL                = 7,
  QMI_WDS_CALL_END_REASON_REDIR_OR_HANDOFF        = 8,
  QMI_WDS_CALL_END_REASON_CLOSE_IN_PROGRESS                   = 9,
  QMI_WDS_CALL_END_REASON_AUTH_FAILED                         = 10,
  QMI_WDS_CALL_END_REASON_INTERNAL_CALL_END                   = 11,
  QMI_WDS_CALL_END_REASON_INTERNAL_ERROR                      = 12,
  QMI_WDS_CALL_END_REASON_INTERNAL_UNKNOWN_CAUSE_CODE         = 13,

/* GSM/WCDMA specific call-end reasons from CM */
  QMI_WDS_CALL_END_REASON_CONF_FAILED          = 1000,
  QMI_WDS_CALL_END_REASON_INCOM_REJ            = 1001,
  QMI_WDS_CALL_END_REASON_NO_GW_SRV            = 1002,
  QMI_WDS_CALL_END_REASON_NETWORK_END          = 1003,
  QMI_WDS_CALL_END_REASON_LLC_SNDCP_FAILURE    = 1004,
  QMI_WDS_CALL_END_REASON_INSUFFICIENT_RESOURCES = 1005,
  QMI_WDS_CALL_END_REASON_OPTION_TEMP_OOO      = 1006,
  QMI_WDS_CALL_END_REASON_NSAPI_ALREADY_USED   = 1007,
  QMI_WDS_CALL_END_REASON_REGULAR_DEACTIVATION = 1008,
  QMI_WDS_CALL_END_REASON_NETWORK_FAILURE      = 1009,
  QMI_WDS_CALL_END_REASON_UMTS_REATTACH_REQ    = 1010,
  QMI_WDS_CALL_END_REASON_PROTOCOL_ERROR       = 1011,
  QMI_WDS_CALL_END_REASON_OPERATOR_DETERMINED_BARRING         = 1012,
  QMI_WDS_CALL_END_REASON_UNKNOWN_APN                         = 1013,
  QMI_WDS_CALL_END_REASON_UNKNOWN_PDP                         = 1014,
  QMI_WDS_CALL_END_REASON_GGSN_REJECT                         = 1015,
  QMI_WDS_CALL_END_REASON_ACTIVATION_REJECT                   = 1016,
  QMI_WDS_CALL_END_REASON_OPTION_NOT_SUPPORTED                = 1017,
  QMI_WDS_CALL_END_REASON_OPTION_UNSUBSCRIBED                 = 1018,
  QMI_WDS_CALL_END_REASON_QOS_NOT_ACCEPTED                    = 1019,
  QMI_WDS_CALL_END_REASON_TFT_SEMANTIC_ERROR                  = 1020,
  QMI_WDS_CALL_END_REASON_TFT_SYNTAX_ERROR                    = 1021,
  QMI_WDS_CALL_END_REASON_UNKNOWN_PDP_CONTEXT                 = 1022,
  QMI_WDS_CALL_END_REASON_FILTER_SEMANTIC_ERROR               = 1023,
  QMI_WDS_CALL_END_REASON_FILTER_SYNTAX_ERROR                 = 1024,
  QMI_WDS_CALL_END_REASON_PDP_WITHOUT_ACTIVE_TFT              = 1025,
  QMI_WDS_CALL_END_REASON_INVALID_TRANSACTION_ID              = 1026,
  QMI_WDS_CALL_END_REASON_MESSAGE_INCORRECT_SEMANTIC          = 1027,
  QMI_WDS_CALL_END_REASON_INVALID_MANDATORY_INFO              = 1028,
  QMI_WDS_CALL_END_REASON_MESSAGE_TYPE_UNSUPPORTED            = 1029,
  QMI_WDS_CALL_END_REASON_MSG_TYPE_NONCOMPATIBLE_STATE        = 1030,
  QMI_WDS_CALL_END_REASON_UNKNOWN_INFO_ELEMENT                = 1031,
  QMI_WDS_CALL_END_REASON_CONDITIONAL_IE_ERROR                = 1032,
  QMI_WDS_CALL_END_REASON_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE = 1033,
  QMI_WDS_CALL_END_REASON_APN_TYPE_CONFLICT                   = 1034,
  QMI_WDS_CALL_END_REASON_NO_GPRS_CONTEXT                     = 1035,
  QMI_WDS_CALL_END_REASON_FEATURE_NOT_SUPPORTED               = 1036,
  QMI_WDS_CALL_END_REASON_ILLEGAL_MS                          = 1037,
  QMI_WDS_CALL_END_REASON_ILLEGAL_ME                          = 1038,
  QMI_WDS_CALL_END_REASON_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED = 1039,
  QMI_WDS_CALL_END_REASON_GPRS_SERVICES_NOT_ALLOWED           = 1040,
  QMI_WDS_CALL_END_REASON_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK = 1041,
  QMI_WDS_CALL_END_REASON_IMPLICITLY_DETACHED                 = 1042,
  QMI_WDS_CALL_END_REASON_PLMN_NOT_ALLOWED                    = 1043,
  QMI_WDS_CALL_END_REASON_LA_NOT_ALLOWED                      = 1044,
  QMI_WDS_CALL_END_REASON_GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN = 1045,
  QMI_WDS_CALL_END_REASON_PDP_DUPLICATE                       = 1046,
  QMI_WDS_CALL_END_REASON_UE_RAT_CHANGE                       = 1047,
  QMI_WDS_CALL_END_REASON_APP_PREEMPTED                       = 1048,
  QMI_WDS_CALL_END_REASON_CONGESTION                          = 1049,
  QMI_WDS_CALL_END_REASON_NO_PDP_CONTEXT_ACTIVATED            = 1050,
  QMI_WDS_CALL_END_REASON_ACCESS_CLASS_DSAC_REJECTION         = 1051,

  QMI_WDS_CALL_END_REASON_MAX_NB_PDN_REACHED                  = 1052,

  QMI_WDS_CALL_END_REASON_MAX      /* DO NOT USE. Used for bounds check */
}qmi_wds_call_end_reason_e_type;

typedef enum 
{
  WDSI_FORCE_REASON_DEFAULT = 0,
  WDSI_FORCE_REASON_ABORT = 1,
  WDSI_FORCE_REASON_AUTOCONNECT = 2
} qmi_wdsi_force_type;

typedef enum 
{
  WDSI_PROFILE_CREATE = 0,
  WDSI_PROFILE_MODIFY = 1
} qmi_wdsi_profile_op_type;

/*---------------------------------------------------------------------------
  REQUESTED RUNTIME SETTINGS bitmask
---------------------------------------------------------------------------*/
#define QMI_WDS_MASK_PROFILE_ID   (0x01)
#define QMI_WDS_MASK_PROFILE_NAME (0x02)
#define QMI_WDS_MASK_PDP_TYPE     (0x04)
#define QMI_WDS_MASK_APN          (0x08)
#define QMI_WDS_MASK_DNS_ADDR     (0x10)
#define QMI_WDS_MASK_GRANTED_QOS  (0x20)
#define QMI_WDS_MASK_USERNAME     (0x40)
#define QMI_WDS_MASK_AUTH_PROTO   (0x80)
#define QMI_WDS_MASK_IP_ADDR      (0x0100)
#define QMI_WDS_MASK_GATEWAY      (0x0200)
#define QMI_WDS_MASK_PCSCF_PCO    (0x0400)
#define QMI_WDS_MASK_PCSCF_ADDR   (0x0800)
#define QMI_WDS_MASK_PCSCF_NAME   (0x1000)
#define QMI_WDS_MASK_RUNTIME_DEFAULT (0x1FFF) /* Only covers bits 0 to 12 */
#define QMI_WDS_MASK_MTU              (0x2000)
#define QMI_WDS_MASK_DOMAIN_NAME_LIST (0x4000)
#define QMI_WDS_MASK_IP_FAMILY        (0x8000)
#define QMI_WDS_MASK_IM_CN_FLAG       (0x10000)
#define QMI_WDS_MASK_TECH_NAME        (0x20000)
#define QMI_WDS_MASK_OP_RES_PCO       (0x40000)



#define WDSI_PRM_TYPE_PROFILE_TECH_TYPE        (0x10)

#define WDSI_IPV6_DEFAULT_PREFIX_LEN                    (64) // 64 bits

/*===========================================================================
  DESCRIPTION
    function pointer to register for pdn events in WDS

  PARAMETERS
    client_handle : handle associated with the pdn.
    event: pdn event.
    event-info: event payload
    user_data_ptr: user data passed as part of registration

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void wdsi_reg_rmnet_pdn_ev
(  
  ds_fwk_clnt_handle_type    client_handle,
  ds_pdn_event_enum_type     event,
  ds_pdn_event_info_u_type   event_info,
  void                      *user_data_ptr
);

void qmi_wdsi_process_cmd
(
  ds_command_enum_type  cmd,
  void              *cmd_ptr
);

/*---------------------------------------------------------------------------
  WDS Event report indication bitmask values
---------------------------------------------------------------------------*/
typedef enum
{
  WDSI_REPORT_STATUS_EV_CH_RATE          = 0x00000001,
  WDSI_REPORT_STATUS_EV_PKT_STATS        = 0x00000002,
  WDSI_REPORT_STATUS_EV_DORM_STATUS      = 0x00000004,
  WDSI_REPORT_STATUS_EV_CURR_DATA_BEARER = 0x00000010,
  WDSI_REPORT_STATUS_MIP_STATUS          = 0x00000020,
  WDSI_REPORT_STATUS_DATA_CALL_STATUS     = 0x00000080,
  WDSI_REPORT_STATUS_PREFERRED_DATA_STATUS    = 0x00000100,
  WDSI_REPORT_STATUS_EVDO_PAGE_MONITOR_PERIOD_CHANGE  = 0x00000200,
  WDSI_REPORT_STATUS_DATA_SYSTEM_STATUS       = 0x00000400,
  WDSI_REPORT_STATUS_UPLINK_FLOW_CONTROL = 0x00000800,
  WDSI_REPORT_STATUS_DELAY_DORMANCY   = 0x00002000,
  WDSI_REPORT_WIDTH         = 0x7FFFFFFF  /* forces enum values to 32 bits */
} qmi_wdsi_report_status_ev_e_type;

/*---------------------------------------------------------------------------
  Pkt statistics masks
---------------------------------------------------------------------------*/
#define WDSI_STAT_MASK_NONE                             (0x00000000)
#define WDSI_STAT_MASK_TX_GOOD_NW                       (0x00000001)
#define WDSI_STAT_MASK_RX_GOOD_NW                       (0x00000002)
#define WDSI_STAT_MASK_TX_DISCARD_NW                    (0x00000004)
#define WDSI_STAT_MASK_RX_DISCARD_NW                    (0x00000008)
#define WDSI_STAT_MASK_TX_OUT_OF_MEM_NW                 (0x00000010)
#define WDSI_STAT_MASK_RX_OUT_OF_MEM_NW                 (0x00000020)
#define WDSI_STAT_MASK_TX_OK_BYTES                      (0x00000040)
#define WDSI_STAT_MASK_RX_OK_BYTES                      (0x00000080)
#define WDSI_STAT_MASK_TX_DROPPED                       (0X00000100)
#define WDSI_STAT_MASK_RX_DROPPED                       (0x00000200)
#define WDSI_STAT_VALUE_UNAVAILABLE                     (0xFFFFFFFF)

#endif /* _DS_QMI_WDS_H */
