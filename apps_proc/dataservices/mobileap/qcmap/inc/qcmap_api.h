#ifndef QCMAP_API_H
#define QCMAP_API_H
/******************************************************************************
  @file    qcmap_api.h
  @brief   The QCMAP API Header file.

  DESCRIPTION
  Header files and function declaration of Mobile AP.

  ---------------------------------------------------------------------------
  Copyright (c) 2008-2023 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/

#include "qapi_dss.h"
#include "qmi_client.h"
#include "qcmap_ext_api.h"
#include "qapi_socket.h"
#include "timer.h"
#include "qapi_diag.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Describe handles including theory of operation */
/* Private opaque handles */
typedef struct         qcmap_client_handle_struct      *qcmap_client_handle;
typedef struct         qcmap_req_handle_struct         *qcmap_req_handle;

/* Service handle */
typedef struct         qcmap_csi_service_struct        *qcmap_csi_service_handle;

#define QCMAP_MAX_CLIENT_HANDLES 5
#define QCMAP_SOFTAP_HANDLE 0x65432
#define QCMAP_QMI_TIMEOUT_VALUE     15000
#define MAX_STRING_LENGTH 100
#define MAX_IPV6_PREFIX 40
#define QCMAP_MSGR_MAC_ADDR_LEN 6

typedef enum
{
  QCMAP_CSI_NO_ERR = 0,
  QCMAP_CSI_CONN_REFUSED,
  QCMAP_CSI_CONN_BUSY,
  QCMAP_CSI_INVALID_HANDLE,
  QCMAP_CSI_INVALID_ARGS,
  QCMAP_CSI_ENCODE_ERR,
  QCMAP_CSI_DECODE_ERR,
  QCMAP_CSI_NO_MEM,
  QCMAP_CSI_INTERNAL_ERR,
  QCMAP_CSI_NOT_SUPP_ERR,
  QCMAP_CSI_DEFERRED
}qcmap_csi_error;


typedef enum
{
  QCMAP_CSI_CB_NO_ERR = 0,
  QCMAP_CSI_CB_CONN_REFUSED,
  QCMAP_CSI_CB_NO_MEM,
  QCMAP_CSI_CB_INTERNAL_ERR,
  QCMAP_CSI_CB_UNSUPPORTED_ERR,
  QCMAP_CSI_CB_REQ_HANDLED,
}qcmap_csi_cb_error;

typedef enum {
  QCMAP_RESULT_TYPE_MIN_ENUM_VAL = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QCMAP_RESULT_SUCCESS = 0, 
  QCMAP_RESULT_FAILURE = 1, 
  QCMAP_RESULT_TYPE_MAX_ENUM_VAL = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qcmap_result_type;

typedef enum {
  QCMAP_ERROR_TYPE_MIN_ENUM_VAL = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QCMAP_ERR_NONE = 0x0000, 
  QCMAP_ERR_INTERNAL = 0x003,
  QCMAP_ERR_INVALID_HANDLE = 0x0009,
  QCMAP_ERR_NO_NETWORK_FOUND = 0x000D,
  QCMAP_ERR_NO_EFFECT = 0x001A,
  QCMAP_ERR_INTERFACE_NOT_FOUND = 0x002B,
  QCMAP_ERR_INVALID_IP_FAMILY_PREF = 0x0040,
  QCMAP_ERROR_TYPE_MAX_ENUM_VAL = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qcmap_error_type;


typedef struct {
  qcmap_result_type result;
  qcmap_error_type error;
}qcmap_response_type;


/*---------------------------------------------------------------------------
           WAN Configuration.
---------------------------------------------------------------------------*/
#define QCMAP_WAN_INVALID_QCMAP_HANDLE 0xFFFFFFFF
#define QCMAP_WAN_MAX_ERI_DATA_SIZE    256
#define QCMAP_WAN_TECH_ANY             0
#define QCMAP_WAN_TECH_3GPP            1
#define QCMAP_WAN_TECH_3GPP2           2
#define MAX_WAN_CON_TIMEOUT            120

/*---------------------------------------------------------------------------
                    Return values indicating error status
---------------------------------------------------------------------------*/
#define QCMAP_SUCCESS               0         /* Successful operation   */
#define QCMAP_ERROR                -1         /* Unsuccessful operation */
#define QCMAP_DHCP_ASSIGN_FAIL 1
#define QCMAP_DHCP_ASSIGN_SUCCESS  0

/*---------------------------------------------------------------------------
           Error Condition Values
---------------------------------------------------------------------------*/
#define QCMAP_ENOERROR              0        /* No error                */
#define QCMAP_EWOULDBLOCK           1        /* Operation would block   */
#define QCMAP_EINVAL                2        /* Invalid operation       */
#define QCMAP_EOPNOTSUPP            3        /* Operation not supported */
#define QCMAP_EBADAPP               4        /* Invalid application ID  */
#define QCMAP_ENOWWAN               5        /* WWAN not connected      */
#define QCMAP_EALDCONN              6        /* Already connected  */
#define QCMAP_EALDDISCONN           7        /* Already disconnected  */
#define QCMAP_DISCONNECTED          8        /* Disconnected */
#define QCMAP_MSGR_ENTRY_PRESENT       -8
#define QCMAP_MSGR_ENTRY_FULL          -9
#define QCMAP_MSGR_INVALID_PARAM       -10

/*---------------------------------------------------------------------------
           Mobile AP Events
---------------------------------------------------------------------------*/
typedef enum
{
  QCMAP_EVENT_ENABLED = 0,
  QCMAP_EVENT_WAN_CONNECTING,
  QCMAP_EVENT_WAN_CONNECTING_FAIL,
  QCMAP_EVENT_WAN_IPv6_CONNECTING_FAIL,
  QCMAP_EVENT_WAN_CONNECTED,
  QCMAP_EVENT_WAN_IPv6_CONNECTED,
  QCMAP_EVENT_WAN_DISCONNECTED,
  QCMAP_EVENT_WAN_IPv6_DISCONNECTED,
  QCMAP_EVENT_WAN_ADDR_RECONF,
  QCMAP_EVENT_WAN_IPv6_ADDR_RECONF,
  QCMAP_EVENT_DISABLED
} qcmap_event_e;

/*---------------------------------------------------------------------------
   Type representing enumeration of QCMAP CM states
---------------------------------------------------------------------------*/
typedef enum
{
  QCMAP_DISABLE = 0,
  QCMAP_ENABLE,
  QCMAP_WAN_CONNECTING,
  QCMAP_WAN_DISCONNECTING,
  QCMAP_WAN_CONNECTED
} qcmap_state_e;

typedef enum
{
  QCMAP_V6_DISABLE = 0,
  QCMAP_V6_ENABLE,
  QCMAP_V6_WAN_CONNECTING,
  QCMAP_V6_WAN_DISCONNECTING,
  QCMAP_V6_WAN_CONNECTED
} qcmap_v6_state_e;

typedef enum {
  QCMAP_MSGR_WWAN_CALL_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01 = 0x01, /**<  IPv4 call  */
  QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 = 0x02, /**<  IPv6 call  */
  QCMAP_MSGR_WWAN_CALL_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qcmap_msgr_wwan_call_type_v01;

#define QCMAP_BRING_UP_WWAN_REQ_V01 0x0022
#define QCMAP_BRING_UP_WWAN_IND_V01 0x0022
#define QCMAP_TEAR_DOWN_WWAN_REQ_V01 0x0023
#define QCMAP_TEAR_DOWN_WWAN_IND_V01 0x0023
#define QCMAP_WWAN_STATUS_IND_V01 0x0025
#define QCMAP_MOBILE_AP_STATUS_IND_V01 0x0029

/*---------------------------------------------------------------------------
           Mobile AP call back declarations.
---------------------------------------------------------------------------*/
typedef void (*qcmap_cb_fcn)
(
  int                 handle,                  /* Mobile AP Application id */
  qcmap_event_e       event,                   /* Type of Mobile AP Event  */
  void               *qcmap_cb_user_data,    /* Call back User data      */
  qapi_DSS_CE_Reason_t    *callend_reason
);

/*---------------------------------------------------------------------------
 QCMAP Client state info
---------------------------------------------------------------------------*/
typedef struct {
  qcmap_client_handle               clnt;
  boolean                           wwan_status_ind_regd;
  boolean                           qcmap_status_ind_regd;
  unsigned int                      wwan_op_ind_msg_id;
  boolean                           wwan_op_ind_pending;
  boolean                           client_enabled;
}qcmap_client_info_type;

/*---------------------------------------------------------------------------
 QCMAP Service state info
---------------------------------------------------------------------------*/
typedef struct
{
  qcmap_csi_service_handle service_handle;
  int                  num_clients;
  void*                client_handle_list[QCMAP_MAX_CLIENT_HANDLES];
  int                  qmi_instance;
  int                  client_ref_count;
} qcmap_state_info_type;

/*---------------------------------------------------------------------------
   Call info for a DSS Call
---------------------------------------------------------------------------*/
typedef struct {
  qapi_DSS_Hndl_t handle;
  char tech[8];
  char family[7];
  int profile;
} dss_call_info_t;

typedef struct qcmap_nas_buffer_s {
  qmi_client_type user_handle;                    /* QMI user handle       */
  unsigned int    msg_id;                         /* Indicator message ID  */
  void           *ind_buf;                        /* Raw indication data   */
  unsigned int    ind_buf_len;                    /* Raw data length       */
  void           *ind_cb_data;                     /* User call back handle */
} qcmap_nas_buffer_t;

typedef struct qcmap_dss_buffer_s {
  qapi_DSS_Hndl_t            dss_nethandle;
  void                  *user_data;
  qapi_DSS_Net_Evt_t         evt;
  qapi_DSS_Evt_Payload_t     *payload_ptr;
} qcmap_dss_buffer_t;

typedef enum {
  QCMAP_MSGR_WWAN_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QCMAP_MSGR_WWAN_STATUS_CONNECTING_V01 = 0x01, /**<  IPv4 WWAN is in connecting state  */
  QCMAP_MSGR_WWAN_STATUS_CONNECTING_FAIL_V01 = 0x02, /**<  IPv4 connection to WWAN failed  */
  QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01 = 0x03, /**<  IPv4 WWAN is in connected state  */
  QCMAP_MSGR_WWAN_STATUS_DISCONNECTING_V01 = 0x04, /**<  IPv4 WWAN is disconnecting  */
  QCMAP_MSGR_WWAN_STATUS_DISCONNECTING_FAIL_V01 = 0x05, /**<  IPv4 WWAN failed to disconnect  */
  QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01 = 0x06, /**<  IPv4 WWAN is disconnected  */
  QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_V01 = 0x07, /**<  IPv6 WWAN is in connecting state  */
  QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_FAIL_V01 = 0x08, /**<  IPv6 connection to WWAN failed  */
  QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01 = 0x09, /**<  IPv6 WWAN is in connected state  */
  QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTING_V01 = 0x0A, /**<  IPv6 WWAN is disconnecting  */
  QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTING_FAIL_V01 = 0x0B, /**<  IPv6 WWAN failed to disconnect  */
  QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01 = 0x0C, /**<  IPv6 WWAN is disconnected  */
  QCMAP_MSGR_WWAN_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qcmap_msgr_wwan_status_enum_v01;


typedef enum {
  QCMAP_MSGR_WWAN_CALL_END_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QCMAP_MSGR_WWAN_CALL_END_TYPE_INVALID_V01 = 0xFF, /**<  Unknown  */
  QCMAP_MSGR_WWAN_CALL_END_TYPE_MOBILE_IP_V01 = 0x01, /**<  Mobile IP  */
  QCMAP_MSGR_WWAN_CALL_END_TYPE_INTERNAL_V01 = 0x02, /**<  Internal  */
  QCMAP_MSGR_WWAN_CALL_END_TYPE_CALL_MANAGER_DEFINED_V01 = 0x03, /**<  Call manager defined  */
  QCMAP_MSGR_WWAN_CALL_END_TYPE_3GPP_SPEC_DEFINED_V01 = 0x06, /**<  3GPP specification defined  */
  QCMAP_MSGR_WWAN_CALL_END_TYPE_PPP_V01 = 0x07, /**<  PPP  */
  QCMAP_MSGR_WWAN_CALL_END_TYPE_EHRPD_V01 = 0x08, /**<  EHRPD  */
  QCMAP_MSGR_WWAN_CALL_END_TYPE_IPV6_V01 = 0x09, /**<  IPv6  */
  QCMAP_MSGR_WWAN_CALL_END_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qcmap_msgr_wwan_call_end_type_enum_v01;


typedef enum {
  QCMAP_MSGR_MOBILE_AP_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QCMAP_MSGR_MOBILE_AP_STATUS_CONNECTED_V01 = 0x01, /**<  Mobile AP is connected  */
  QCMAP_MSGR_MOBILE_AP_STATUS_DISCONNECTED_V01 = 0x02, /**<  Mobile AP is disconnected  */
  QCMAP_MSGR_MOBILE_AP_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qcmap_msgr_mobile_ap_status_enum_v01;


typedef struct {

  qcmap_msgr_wwan_call_end_type_enum_v01 wwan_call_end_reason_type;
  int32_t wwan_call_end_reason_code;

}qcmap_msgr_wwan_call_end_reason_v01;

typedef struct {
  uint32_t mobile_ap_handle;
  qcmap_msgr_wwan_status_enum_v01 wwan_status;
  uint8_t wwan_call_end_reason_valid;
  qcmap_msgr_wwan_call_end_reason_v01 wwan_call_end_reason;
}qcmap_msgr_wwan_status_ind_msg_v01;

typedef struct {
  uint32_t mobile_ap_handle;
  qcmap_msgr_wwan_status_enum_v01 conn_status;
  uint8_t wwan_call_end_reason_valid;
  qcmap_msgr_wwan_call_end_reason_v01 wwan_call_end_reason;
}qcmap_msgr_bring_up_wwan_ind_msg_v01;

typedef struct {
  uint32_t mobile_ap_handle;
  qcmap_msgr_wwan_status_enum_v01 conn_status;
  uint8_t wwan_call_end_reason_valid;
  qcmap_msgr_wwan_call_end_reason_v01 wwan_call_end_reason;
}qcmap_msgr_tear_down_wwan_ind_msg_v01;

typedef struct {
  uint32_t mobile_ap_handle;
  qcmap_msgr_mobile_ap_status_enum_v01 mobile_ap_status;
}qcmap_msgr_mobile_ap_status_ind_msg_v01;

typedef enum {
  QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QCMAP_MSGR_IP_FAMILY_V4_V01 = 0x04, /**<  IPv4 version  */
  QCMAP_MSGR_IP_FAMILY_V6_V01 = 0x06, /**<  IPv6 version  */
  QCMAP_MSGR_IP_FAMILY_V4V6_V01 = 0x0A, /**<  Dual mode version  */
  QCMAP_MSGR_IP_FAMILY_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qcmap_msgr_ip_family_enum_v01;

typedef struct {
  uint32_t mobile_ap_handle;
}qcmap_msgr_mobile_ap_disable_req_msg_v01;

typedef struct {
  qcmap_response_type resp;
}qcmap_msgr_mobile_ap_disable_resp_msg_v01;

typedef struct {
  uint32_t mobile_ap_handle;
  uint8_t call_type_valid;
  qcmap_msgr_wwan_call_type_v01 call_type;
}qcmap_msgr_bring_up_wwan_req_msg_v01;

typedef struct {
  qcmap_response_type resp;
  uint8_t conn_status_valid;
  qcmap_msgr_wwan_status_enum_v01 conn_status;
}qcmap_msgr_bring_up_wwan_resp_msg_v01;

typedef struct {
  uint32_t mobile_ap_handle;
  uint8_t call_type_valid;
  qcmap_msgr_wwan_call_type_v01 call_type;
}qcmap_msgr_tear_down_wwan_req_msg_v01;

typedef struct {
  qcmap_response_type resp;
  uint8_t conn_status_valid;
  qcmap_msgr_wwan_status_enum_v01 conn_status;
}qcmap_msgr_tear_down_wwan_resp_msg_v01;

typedef struct {
  uint32_t mobile_ap_handle;
  uint8_t enable;
}qcmap_msgr_set_auto_connect_req_msg_v01;

typedef struct {
  qcmap_response_type resp;
}qcmap_msgr_set_auto_connect_resp_msg_v01;

typedef struct {
  uint32_t mobile_ap_handle;
  uint8_t dhcp_enable;
}qcmap_msgr_set_dhcp_req_msg_v01;

typedef struct {
  qcmap_response_type resp;
}qcmap_msgr_set_dhcp_resp_msg_v01;

typedef struct
{
    boolean flag;

    /* AutoConnect */
    boolean  auto_connect;                              /* Auto connect flag is set ot not */
    boolean  auto_connect_timer_running_v4;             /* Auto connect v4 timer is running or not */

    timer_group_type ipv4_timer_group;                  /* Ipv4 autoconnect timer group*/

    timer_type ipv4_auto_connect_timer;                 /* Ipv4 autoconnect timer type*/

    int      auto_timer_value_v4;

    boolean  auto_connect_timer_running_v6;             /* Auto connect v6 timer is running or not */

    timer_group_type ipv6_timer_group;                  /* Ipv6 autoconnect timer group*/

    timer_type ipv6_auto_connect_timer;                 /* Ipv6 autoconnect timer type*/

    int      auto_timer_value_v6;

    /* Use for Signal the LAN/WAN connected */
    qurt_mutex_t               cm_mutex;
    qurt_signal_t              cm_cond;

    /*To track qcmap tear down*/
     boolean         qcmap_tear_down_in_progress;        /* Call tear down in progress or not*/

    /* QCMAP CM Config */
    boolean           qcmap_enable;
    int    qcmap_cm_handle;

    /* QCMAP CM CB and Client Information Ref Ptr */
    unsigned int* qcmap_cb_handle;

    qcmap_client_resp_ind_cb cb_arr[QCMAP_MAX_CLIENT_HANDLES];

    uint8 client_ref_count;

    sio_stream_id_type          qcmap_ph_stream_id;

    boolean        dtr_enabled;

    unsigned long dmz_addr;
}qcmap_connection_manager;

typedef struct{
  void *data;
  uint8 conn_handle;
}client_data_s;

typedef struct
{
  int  profile;
  char call_tech[8];
  char gateway_ip[128];
  char subnet_mask[128];
  char dhcp_start[128];
  char dhcp_end[128];
  int  dhcp_lease;
  int  autoconnect;
  int  roaming;
  int  dhcp_enable;
  int  stdecm_publicIP;
  int  apps_port_start;
  int  apps_port_range;
  int  nat_tcp_tmo;
  int  nat_udp_tmo;
}qcmap_file;

typedef struct
{
  struct ip46addr gateway_addr;
  struct ip46addr subnet_addr;
  struct ip46addr dhcp_start_addr;
  struct ip46addr dhcp_end_addr;
}qcmap_ip;

#ifndef ip_addr
#define ip_addr unsigned long
#endif   /* ip_addr */

typedef struct
{
  boolean client_enabled[QCMAP_MAX_CLIENT_HANDLES];

}qcmap_client_info_t;

/*---------------------------------------------------------------------------
   Type representing an QCMAP CM CB (QCMAP CM control block); this captures
   all state and other data related to the QCMAP CM
---------------------------------------------------------------------------*/
typedef struct
{
    qcmap_state_e          state;

    qcmap_v6_state_e       ipv6_state;
    int                       handle;
    /* QMI QCMAP IPv4v6 handles */
    uint32_t                  mobile_ap_handle;
    uint32_t                  ipv6_mobile_ap_handle;
    /* libqcmap_cm library callback info */
    qcmap_cb_fcn           qcmap_callback;
    void                     *qcmap_callback_user_data;

    /* Whether it is init */
    boolean                   init;
    /* Whether it has backhaul service  */
    boolean                   backhaul_service;
    uint8                     roaming_indicator;

    uint16                    eri_roam_data_len;
    /*Whether disable is in progress*/
    boolean                   disable_in_process;


    qurt_mutex_t           qcmap_mutex;
    uint8                     eri_roam_data[QCMAP_WAN_MAX_ERI_DATA_SIZE];

    /* dss related info */
    dss_call_info_t           dss_net_hndl;
    char dss_device_name[QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN + 2];

    /* dss ipv6 related info */
    dss_call_info_t           ipv6_dss_net_hndl;
    char ipv6_dss_device_name[QAPI_DSS_CALL_INFO_DEVICE_NAME_MAX_LEN + 2];

    /* QMI NAS service info */
    qmi_client_type           qmi_nas_handle;
    qmi_client_type           qmi_nas_notifier;
    qmi_cci_os_signal_type    qmi_nas_os_params;

}qcmap_cb_t;

/*---------------------------------------------------------------------------
   Mobile AP CM control block
---------------------------------------------------------------------------*/
#if 1
static qcmap_cb_t qcmap_cb = {
 QCMAP_DISABLE,//state
 QCMAP_V6_DISABLE,//ipv6_state
 1,//handle
 QCMAP_WAN_INVALID_QCMAP_HANDLE,//mobile_ap_handle
 QCMAP_WAN_INVALID_QCMAP_HANDLE,//ipv6_mobile_ap_handle
 NULL,//qcmap_cm_callback
 NULL,//qcmap_cm_callback_user_data
 false,//init
 false,//backhaul_service
 0,//roaming_indicator
 0,//eri_roam_data_len
 false,//disable_in_process
 };
#endif

typedef struct
{
  struct
  {
    int  umts_profile_index;
    int  cdma_profile_index;
  }v4;
  struct
  {
    int  umts_profile_index;
    int  cdma_profile_index;
  }v6;
}qcmap_profile_index;

typedef struct {
  uint8 conn_handle;
  void *data;
  qcmap_response_type resp;
  uint8_t mobile_ap_handle_valid; 
  uint32_t mobile_ap_handle;
  uint8_t  call_type_valid;
  uint8_t conn_status_valid;
  qcmap_msgr_wwan_status_enum_v01 wwan_status;
  qcmap_msgr_wwan_status_enum_v01 conn_status;
  uint8_t wwan_call_end_reason_valid;
  qcmap_msgr_wwan_call_end_reason_v01 wwan_call_end_reason;
  qcmap_msgr_mobile_ap_status_enum_v01 mobile_ap_status;
}mobile_ap_resp;

typedef struct
{
  boolean  roaming;
}qcmap_wwan_conf_t;


typedef void (*qcmap_msgr_wwan_ind_cb_fcn)
(
  void                           *user_data,              /* Call back User data */
  qcmap_msgr_wwan_status_enum_v01      conn_status,             /* Connection Status enum */
  qcmap_msgr_wwan_call_end_type_enum_v01 call_end,
  int call_end_reason_code
);

typedef void (*qcmap_msgr_qcmap_ind_cb_fcn)
(
  void                           *user_data,              /* Call back User data */
  qcmap_msgr_mobile_ap_status_enum_v01      conn_status             /* Connection Status enum */
);

typedef struct
{
  qcmap_wwan_conf_t wwan_cfg;
}qcmap_backhaul_wwan;

/*---------------------------------------------------------------------------
  status callback data
---------------------------------------------------------------------------*/
typedef struct
{
  uint8                        conn_handle;
  unsigned int                 map_instance;
  qcmap_msgr_wwan_ind_cb_fcn   wwan_cb;
  qcmap_msgr_qcmap_ind_cb_fcn  qcmap_cb;
}qcmap_status_cb_data;


/*---------------------------------------------------------------------------
  QCMAP QMI SERVICE
----------------------------------------------------------------------------*/
typedef enum
{
  QCMAP_QMI_NAS_IN_SERVICE = 0,
  QCMAP_QMI_NAS_NOT_IN_SERVICE,
}qcmap_qmi_service_t;


typedef struct qcmap_qmi_service_buffer_s {
  qcmap_qmi_service_t  qmi_service_id;
} qcmap_qmi_service_buffer_t;


typedef struct
{
  int                           handle;
  qcmap_msgr_wwan_status_enum_v01     wwan_status;
  qcmap_status_cb_data     *cb_ptr;
} qcmap_softap_handle_type;

  typedef struct {
  qcmap_response_type resp;
  uint8_t mobile_ap_handle_valid; 
  uint32_t mobile_ap_handle;
}qcmap_msgr_mobile_ap_enable_resp_msg_v01;

typedef enum{
  NO_BACKHAUL = 0,
  BACKHAUL_TYPE_WWAN,
}qcmap_backhaul_type;

typedef struct 
{
  boolean flag;
/* ----------------------MobileAP Backhaul Execution--------------------*/

  //ALL VARIABLES HERE//

   /*Priority list for Backhaul Switching*/
   qcmap_backhaul_type	   preffered_backhaul_first;
   qcmap_backhaul_type	   preffered_backhaul_second;
   qcmap_backhaul_type	   preffered_backhaul_third;
   /*Current Backhaul*/
   qcmap_backhaul_type	   current_backhaul;

   boolean enable_ipv6;
   boolean enable_ipv4;

}qcmap_backhaul_mgr;


typedef struct {
  uint32_t mobile_ap_handle;
}qcmap_msgr_enable_ipv4_req_msg_v01;

typedef struct {
  qcmap_response_type resp;
}qcmap_msgr_enable_ipv4_resp_msg_v01;

typedef struct {
  uint32_t mobile_ap_handle;
}qcmap_msgr_disable_ipv4_req_msg_v01;

typedef struct {
  qcmap_response_type resp;
}qcmap_msgr_disable_ipv4_resp_msg_v01;

typedef struct {
  uint32_t mobile_ap_handle;
}qcmap_msgr_enable_ipv6_req_msg_v01;

typedef struct {
  qcmap_response_type resp;
}qcmap_msgr_enable_ipv6_resp_msg_v01;

typedef struct {
  uint32_t mobile_ap_handle;
}qcmap_msgr_disable_ipv6_req_msg_v01;

typedef struct {
  qcmap_response_type resp;
}qcmap_msgr_disable_ipv6_resp_msg_v01;

typedef struct {
  uint32_t mobile_ap_handle;
}qcmap_msgr_get_auto_connect_req_msg_v01;

typedef struct {
  qcmap_response_type resp;
  uint8_t auto_conn_flag_valid;
  uint8_t auto_conn_flag;
}qcmap_msgr_get_auto_connect_resp_msg_v01;

typedef struct {
  uint32_t mobile_ap_handle;
}qcmap_msgr_get_roaming_pref_req_msg_v01;

typedef struct {
  qcmap_response_type resp;
  uint8_t allow_wwan_calls_while_roaming_valid;
  uint8_t allow_wwan_calls_while_roaming;
}qcmap_msgr_get_roaming_pref_resp_msg_v01;

typedef struct {
  uint32_t mobile_ap_handle;
  uint8_t allow_wwan_calls_while_roaming;
}qcmap_msgr_set_roaming_pref_req_msg_v01;

typedef struct {
  qcmap_response_type resp;
}qcmap_msgr_set_roaming_pref_resp_msg_v01;

typedef struct {
  uint32_t mobile_ap_handle;
  unsigned long dmz_ip_addr;
}qcmap_msgr_set_dmz_req_msg_v01;

typedef struct {
  qcmap_response_type resp;
}qcmap_msgr_set_dmz_resp_msg_v01;

typedef struct {
  uint32_t mobile_ap_handle;
  unsigned long dmz_ip_addr;
}qcmap_msgr_delete_dmz_req_msg_v01;

typedef struct {
  qcmap_response_type resp;
}qcmap_msgr_delete_dmz_resp_msg_v01;


/*===========================================================================
  FUNCTION QCMAP_MOBILE_AP_ENABLE()

  DESCRIPTION
    Enable SoftAP functionality on modem.

  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/

void  qcmap_mobile_ap_enable(
  void* data,
  uint8 conn_handle
);

/*===========================================================================
  FUNCTION QCMAP_MOBILE_AP_DISABLE()

  DESCRIPTION
    Disable SoftAP functionality on modem.

  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/

void  qcmap_mobile_ap_disable(
  void* data,
  uint8 conn_handle
);

/*=====================================================
  FUNCTION Disable
======================================================*/
/*!
@brief
  Disable Mobile ap will bring down the backhaul.

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/

boolean Disable
(
  int *err_num,
  qcmap_error_type *qcmap_err_num
);

/*=====================================================
  FUNCTION DisableHandle
======================================================*/
/*!
@brief
 Initialize mobileap handle to zero.

@return
  None.

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/

void DisableHandle(void);

/*===========================================================================

FUNCTION QCMAP_DISABLE()

DESCRIPTION

  Disable MobileAP.
  It will configure Modem in non-MobileAP mode.

DEPENDENCIES
  None.

RETURN VALUE

  qcmap_errno Values
  ----------------
  QCMAP_EBADAPP           invalid application ID specified

SIDE EFFECTS

===========================================================================*/

int qcmap_disable
(
  int  qcmap_handle,                          /* Handle for MobileAP CM */
  int *qcmap_errno,                           /* Error condition value  */
  qcmap_error_type *qcmap_err_num
);

/*=====================================================
  FUNCTION Enable
======================================================*/
/*!
@brief
  Enables MobileAP based on the configuration.
  As a part of enable mobileap, registers mobileap event callback.
  It will bring up RmNet.
  If AutoConnect is enabled then initiates connect to backhaul.

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean Enable
(
  int *handle,
  void *cb_user_data,
  qcmap_error_type *qcmap_err_num
);

/*===========================================================================

FUNCTION QCMAP_ENABLE()

DESCRIPTION

  Enable MobileAP based on the user input.
  It will register MobileAP event callback.
  It will configure Modem in MobileAP Mode and bring up RmNet.

DEPENDENCIES
  None.

RETURN VALUE
  Returns MobileAP application ID on success.

  On error, return 0 and places the error condition value in
  *qcmap_errno.

SIDE EFFECTS

===========================================================================*/

int qcmap_enable
(
  qcmap_cb_fcn  qcmap_callback,             /* Callback function     */
  void            *qcmap_callback_user_data,   /* Callback user data    */
  int             *qcmap_errno,                /* Error condition value */
  qcmap_error_type *qcmap_err_num
);

/*===========================================================================

FUNCTION QCMAP_CONNECT_BACKHAUL()

DESCRIPTION

  It will bringup WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  If WAN is already connected, returns QCMAP_SUCCESS.
  Otherwise, return QCMAP_ERROR and places the error condition value in
  *qcmap_errno.

  qcmap_errno Values
  ----------------
  QCMAP_EBADAPP           invalid application ID specified
  QCMAP_EWOULDBLOCK       the operation would block
  QCMAP_EOPNOTSUPP        backhaul bringup/teardown in progress

SIDE EFFECTS
===========================================================================*/

int qcmap_connect_backhaul
(
  int  qcmap_handle,                          /* Handle for MobileAP CM */
  qcmap_msgr_wwan_call_type_v01    call_type, /* Call type to be brought UP. */
  int *qcmap_errno,                            /* Error condition value  */
  qcmap_error_type *qcmap_err_num
);

/*===========================================================================

FUNCTION QCMAP_DISCONNECT_BACKHAUL()

DESCRIPTION

  It will teardown WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  If WAN is already disconnected, returns QCMAP_SUCCESS.
  return QCMAP_ERROR and places the error condition value in
  *qcmap_errno.

  qcmap_errno Values
  ----------------
  QCMAP_EBADAPP           invalid application ID specified
  QCMAP_EWOULDBLOCK       the operation would block
  QCMAP_EOPNOTSUPP        backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/

int qcmap_disconnect_backhaul
(
  int  qcmap_handle,                          /* Handle for MobileAP CM */
  qcmap_msgr_wwan_call_type_v01    call_type, /* Call type to be brought down. */
  int *qcmap_errno,                           /* Error condition value  */
  qcmap_error_type *qcmap_err_num
);

/*===========================================================================

FUNCTION BRING_UP_IPV4_WWAN()

DESCRIPTION

  It will bring up IPV4 WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  Conncets WAN for IPV4 and returns QCMAP_SUCCESS if sucessful
  return QCMAP_ERROR and places the error condition value in
  *qcmap_errno.

  qcmap_errno Values
  ----------------
  QCMAP_ERR_INVALID_HANDLE           invalid handle received
  QCMAP_EWOULDBLOCK                  the operation would block
  QCMAP_EOPNOTSUPP                   backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/

int bring_up_ipv4_wwan
(
  int *qcmap_errno,
  qcmap_error_type *qcmap_err_num
);

/*===========================================================================

FUNCTION BRING_UP_IPV6_WWAN()

DESCRIPTION

  It will bring up IPV6 WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  Conncets WAN for IPV4 and returns QCMAP_SUCCESS if sucessful
  return QCMAP_ERROR and places the error condition value in
  *qcmap_errno.

  qcmap_errno Values
  ----------------
  QCMAP_ERR_INVALID_HANDLE           invalid handle received
  QCMAP_EWOULDBLOCK                  the operation would block
  QCMAP_EOPNOTSUPP                   backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/

int bring_up_ipv6_wwan
(
  int *qcmap_errno,
  qcmap_error_type *qcmap_err_num
);

/*===========================================================================

FUNCTION BRING_DOWN_IPV4_WWAN()

DESCRIPTION

  It will teardown IPV4 WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  If WAN is already disconnected, returns QCMAP_SUCCESS.
  return QCMAP_ERROR and places the error condition value in
  *qcmap_errno.

  qcmap_errno Values
  ----------------
  QCMAP_EBADAPP           invalid application ID specified
  QCMAP_EWOULDBLOCK       the operation would block
  QCMAP_EOPNOTSUPP        backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/

int bring_down_ipv4_wwan
(
  int    qcmap_handle,                      /* Handle for Mobile AP CM  */
  int    *qcmap_errno,                      /* error condition value    */
  qcmap_error_type *qcmap_err_num
);

/*===========================================================================

FUNCTION bring_down_ipv6_wwan()

DESCRIPTION

  It will teardown IPV6 WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  If WAN is already disconnected, returns QCMAP_SUCCESS.
  return QCMAP_ERROR and places the error condition value in
  *qcmap_errno.

  qcmap_errno Values
  ----------------
  QCMAP_EBADAPP           invalid application ID specified
  QCMAP_EWOULDBLOCK       the operation would block
  QCMAP_EOPNOTSUPP        backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/

int bring_down_ipv6_wwan
(
  int    qcmap_handle,                      /* Handle for Mobile AP CM  */
  int    *qcmap_errno,                      /* error condition value    */
  qcmap_error_type *qcmap_err_num
);

int get_nas_config
(
  int  qcmap_handle,   /* Handle for MobileAP CM */
  int *qcmap_errno,     /* Error condition value  */
  qcmap_error_type *qcmap_err_num
);
/*===========================================================================

FUNCTION QCMAP_GET_DEV_NAME()

DESCRIPTION
  This Function get RMNET device name.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_SUCCESS.
  On error, return QCMAP_ERROR and places the error condition value in
  *qcmap_errno.

SIDE EFFECTS

===========================================================================*/

int qcmap_get_dev_name
(
  int     qcmap_handle,                     /* Handle for Mobile AP CM  */
  qcmap_msgr_ip_family_enum_v01
          qcmap_dev_type,                      /* Dev type                 */
  char   *qcmap_dev_name,                      /* Device name              */
  int    *qcmap_errno                       /* Error condition value    */
);

/*=====================================================
  FUNCTION QCMAP_ConnectionManager_callback
======================================================*/
/*!
@brief
  Handles the mobile ap events and invokes the callback function
  for the respective indications to be sent.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*====================================================*/

void qcmap_connectionmanager_callback
(
  int               handle,               /* MobileAP Application id */
  qcmap_event_e     event,                /* Type of MobileAP Event  */
  void              *qcmap_msgr_cb_user_data, /* Call back user data     */
  qapi_DSS_CE_Reason_t   *callend_reason
);

/*===========================================================================
  FUNCTION QCMAP_WWAN_STATUS_IND()

  DESCRIPTION
    Dispatches a unicast indication to notify registered QCMobileAP client
      about WWAN status.

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_mobile_ap_enable() must have been called

  SIDE EFFECTS
    None
===========================================================================*/
void qcmap_wwan_status_ind
(
  void                           *user_data,
  qcmap_msgr_wwan_status_enum_v01  conn_status,
  qcmap_msgr_wwan_call_end_type_enum_v01 call_end_type,
  int call_end_reason_code
);

/*===========================================================================
  FUNCTION QCMAP_Mobile_AP_STATUS_IND()

  DESCRIPTION
    Dispatches a unicast indication to notify registered QCMobileAP client
      about Mobile AP status.

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_mobile_ap_enable() must have been called

  SIDE EFFECTS
    None
===========================================================================*/

void qcmap_mobile_ap_status_ind
(
  void                           *user_data,
  qcmap_msgr_mobile_ap_status_enum_v01  conn_status
);

/*============================================================
  FUNCTION qcmap_nas_init
==============================================================
@brief
  API to register QMI NAS Service available call back

@return
  QCMAP_ERROR - Could not register QMI NAS service callback
  QCMAP_SUCCESS - Successfully registered QMI NAS service call back
@note

  - Dependencies
    - None

  - Side Effects
    - None
==========================================================*/
int qcmap_nas_init(void);

/*============================================================
  FUNCTION qmi_nas_service_available_cb
==============================================================
@brief
 QMI NAS Service Available callback handler

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
==========================================================*/

void qmi_nas_service_available_cb
(
  qmi_client_type                user_handle,
  qmi_idl_service_object_type    service_obj,
  qmi_client_notify_event_type   service_event,
  void                           *notify_cb_data
);

/*=============================================================================

FUNCTION QCMAP_SEND_MSG

DESCRIPTION
  API to send response/indication back to the QCMAP CLIENT
DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void qcmap_send_msg
(
  qcmap_cmd_id_e evt,
  void * data,
  uint8  connection_handle
);

/*===========================================================================
  FUNCTION QCMAP_BRING_UP_WWAN()

  DESCRIPTION
    Bring up WWAN connection in MobileAP mode.

  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/

void qcmap_bring_up_wwan
(
  void* data,
  uint8 conn_handle
);

/*===========================================================================
  FUNCTION QCMAP_TEAR_DOWN_WWAN()

  DESCRIPTION
    Tear down currently active WWAN connection.

  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/

void qcmap_tear_down_wwan
(
  void* data,
  uint8 conn_handle
);

/*===========================================================================
  FUNCTION ConnectBackHaul
==========================================================================*/
/*!
@brief
  Tries to connect to backhaul. As a part of connect to backhaul checks for the
  network service and nas configuration.
  If the network is in roaming mode,  connects to backhaul based on the roaming flag configuration.

@parameters
  qcmap_msgr_wwan_call_type_v01 call_type
  int                           *err_num

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean ConnectBackHaul
(
  qcmap_msgr_wwan_call_type_v01 call_type,
  int *err_num,
  qcmap_error_type *qcmap_err_num
);

/*===========================================================================
  FUNCTION DisconnectBackHaul
==========================================================================*/
/*!
@brief
  Brings down the bachaul service.

@parameters
  qcmap_msgr_wwan_call_type_v01 call_type
  int                           *err_num

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean DisconnectBackHaul
(
  qcmap_msgr_wwan_call_type_v01 call_type,
  int *err_num,
  qcmap_error_type *qcmap_err_num
);

/*===========================================================================
  FUNCTION  qcmap_process_dss_net_evt
===========================================================================*/
/*!
@brief
    QCMAP DSS net callback

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

void qcmap_process_dss_net_evt
( 
  qcmap_dss_buffer_t *qcmap_dss_buffer
);

/*===========================================================================
  FUNCTION start_auto_connect
===========================================================================*/
/*!
@brief
  This Function is used to send connect to backhaul request incase the
  previous connect to backhal request failed.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

void start_auto_connect(qcmap_msgr_ip_family_enum_v01 family);

/*===========================================================================
  FUNCTION stop_auto_timer
===========================================================================*/
/*!
@brief
 This Fuction will stop the running timer and reset the timer vaules.
@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

void stop_auto_timer(qcmap_msgr_ip_family_enum_v01 family);

/*===========================================================================
  FUNCTION QCMAP_GET_AUTO_CONNECT()

  DESCRIPTION
    Get Autoconnect flag value on Apps.

  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/
void  qcmap_get_auto_connect(
  void* data,
  uint8 conn_handle
);

/*===========================================================================
  FUNCTION QCMAP_SET_AUTO_CONNECT()

  DESCRIPTION
    Set Autoconnect flag value on Apps.

  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/
void  qcmap_set_auto_connect(
  void* data,
  uint8 conn_handle
);

/*===========================================================================
  FUNCTION SetAutoconnect
==========================================================================*/
/*!
@brief
  This function will enable the autoconnect flag.
  If the mobileap and auto connect is enabled, connect to backhaul is triggered.
  In case connect to backhaul fails, then a autoconnect timer is triggered. After the expiry of
  autoconnect timer a new request to connect to backhaul is initiated.

@parameters
  boolean enable

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean SetAutoconnect
(
  boolean enable,
  qcmap_error_type *qcmap_err_num
);

/*===========================================================================
  FUNCTION GetAutoconnect
==========================================================================*/
/*!
@brief
  Returns autoconnect flag value.

@parameters
  value

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean GetAutoconnect(void);

/*===========================================================================
  FUNCTION SetRoaming
==========================================================================*/
/*!
@brief
  This function will set the roaming flag value. Enabling the roaming flag
  will enable connecting to a Network in roaming mode.

@parameters
  boolean enable

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean SetRoaming
(
boolean enable,
 qcmap_error_type *qcmap_err_num
);

/*===========================================================================
  FUNCTION GetRoaming
==========================================================================*/
/*!
@brief
  Returns roaming flag value.

@parameters
  void

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean GetRoaming(void);

/*===========================================================================

FUNCTION QCMAP_CM_SET_AUTO_CONNECT()

DESCRIPTION

  It will set autoconnect.

DEPENDENCIES
  None.

RETURN VALUE

  qcmap_errno Values
  ----------------
  On success, returns QCMAP_SUCCESS.
  On error, return QCMAP_ERROR and places the error condition value in
  *qcmap_errno.


SIDE EFFECTS

===========================================================================*/
int qcmap_cm_set_auto_connect
(
 int      qcmap_handle,                   /* Handle for MobileAP CM    */
 int     *qcmap_errno,                    /* Error condition value     */
 boolean  auto_connect,                      /* Autoconnect Enable or Not */
 qcmap_error_type *qcmap_err_num
 );

/*===========================================================================

FUNCTION QCMAP_CM_SET_ROAMING()

DESCRIPTION

  It will set roaming.

DEPENDENCIES
  None.

RETURN VALUE

  qcmap_errno Values
  ----------------
  On success, returns QCMAP_SUCCESS.
  On error, return QCMAP_ERROR and places the error condition value in
  *qcmap_errno.


SIDE EFFECTS

===========================================================================*/
int qcmap_cm_set_roaming
(
  int      qcmap_handle,                      /* Handle for MobileAP CM */
  qcmap_error_type *qcmap_err_num,               /* Error condition value  */
  boolean  roaming                               /* Roaming Enable or Not  */
);

/*===========================================================================
  FUNCTION QCMAP_GET_ROAMING()

  DESCRIPTION
    Gets Roaming flag value on Apps.

  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/

void qcmap_get_roaming
(
  void* data,
  uint8 conn_handle
);

/*===========================================================================
  FUNCTION QCMAP_SET_ROAMING()

  DESCRIPTION
    Set roaming flag value on Apps.  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/
void qcmap_set_roaming
(
  void* data,
  uint8 conn_handle
);


/*===========================================================================
  FUNCTION QCMAP_SET_DMZIP()

  DESCRIPTION
    Set DMZ IP

  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/
void qcmap_set_dmzip
(
  void* data,
  uint8 conn_handle
);

/*===========================================================================
  FUNCTION QCMAP_DELETE_DMZIP()

  DESCRIPTION
    Set DMZ IP

  PARAMETERS
    data - Data from client
    conn_handle: Used to identify client

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/
void qcmap_delete_dmzip
(
  void* data,
  uint8 conn_handle
);


/*===========================================================================
  FUNCTION QCMAP_UPDATE_DTR_REG()

  DESCRIPTION
    The function updates the DTR callback registration with the peripheral

  RETURN VALUE
    None

  DEPENDENCIES
    qcmap_client_reg_cb should be called

  SIDE EFFECTS
    None
===========================================================================*/
void qcmap_update_dtr_reg(uint8 stream_id);


/*================================================================

FUNCTION qcmap_ph_dtr_changed_cb

DESCRIPTION
  Callback function to handle DTR signal notification from peripheral

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==================================================================*/
void qcmap_ph_dtr_changed_cb (void);

/*===========================================================================
  FUNCTION QCMAP_GET_LINE_STATE()

  DESCRIPTION
    The function gets the Line state from the peripheral

  RETURN VALUE
    boolean with the line state value

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qcmap_get_line_state(void);

/*============================================================
  FUNCTION  qcmap_cm_process_qmi_service_availabilty
=============================================================
@brief
 Initiate QMI Client for the QMI service which is available

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None

 ==========================================================*/
void qcmap_cm_process_qmi_service_availabilty
(
  qcmap_qmi_service_t  qmi_service_id
);


/*============================================================
  FUNCTION qmi_nas_client_init
==============================================================
@brief
 QMI NAS Client Init

@return
  QCMAP_ERROR in case of error
  QCMAP_SUCCESS for sucess

@note

  - Dependencies
    - None

  - Side Effects
    - None
==========================================================*/

int qmi_nas_client_init(void);


/*============================================================
  FUNCTION qmi_nas_not_in_service
=============================================================

@brief
   Function invoked during SSR functionality, when modem is not in service.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
==========================================================*/
int qmi_nas_not_in_service(void);



/*===========================================================================
  FUNCTION  qcmap_cm_qmi_nas_ind
===========================================================================*/
/*!
@brief
 Receives an incoming QMI NAS Indication.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
qcmap_cm_qmi_nas_ind
(
  qmi_client_type user_handle,                    /* QMI user handle       */
  unsigned int    msg_id,                         /* Indicator message ID  */
  void           *ind_buf,                        /* Raw indication data   */
  unsigned int    ind_buf_len,                    /* Raw data length       */
  void           *ind_cb_data                     /* User call back handle */
);

/*===========================================================================
  FUNCTION  qcmap_cm_process_qmi_nas_ind
===========================================================================*/
/*!
@brief
 Processes an incoming QMI NAS Indication.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
qcmap_cm_process_qmi_nas_ind
(
  qcmap_nas_buffer_t *qcmap_nas_buffer
);

/*===========================================================================
  FUNCTION  qcmap_create_timers
===========================================================================*/
/*!
@brief
 Creates Timers for Autoconnect functionality

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void qcmap_create_timers(void);


/*===========================================================================
  FUNCTION  qcmap_v4_timer_cb
===========================================================================*/
/*!
@brief
 Callback invoked when v4 timer expires

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void qcmap_v4_timer_cb(void* unused);

/*===========================================================================
  FUNCTION  qcmap_v6_timer_cb
===========================================================================*/
/*!
@brief
 Callback invoked when v6 timer expires

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void qcmap_v6_timer_cb(void* unused);

/*===========================================================================
  FUNCTION  QCMAP_AUTO_CONNECT_TIMEOUT
===========================================================================*/
/*!
@brief
  Function to handle the auto connect timeout
@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void qcmap_auto_connect_timeout(qcmap_cmd_id_e  cmd_id);

/*===========================================================================
  FUNCTION  qcmap_bring_up_std_ecm
===========================================================================*/
/*!
@brief
 Function for registering qti client and enabling mobile ap functionality.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

void qcmap_bring_up_std_ecm( void);

/*===========================================================================
  FUNCTION  qcmap_bring_down_std_ecm
===========================================================================*/
/*!
@brief
 Function for disabling mobile ap functionality for qti client.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

void qcmap_bring_down_std_ecm(void );

/*===========================================================================
  FUNCTION  qcmap_qti_client_cb
===========================================================================*/
/*!
@brief
 Callback function to process different response events received from qcmap server

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

void qcmap_qti_client_cb(uint8 evt, void* data, uint8 idx);

/*===========================================================================
  FUNCTION ()

  DESCRIPTION
   Function to get the mac address for ecm interface

  RETURN VALUE
    boolean with the line state value

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qcmap_get_mac_address(void);

/*===========================================================================
  FUNCTION qcmap_enable_nat_on_apps()

  DESCRIPTION
   Function to enable the NAT functionality in the network stack

  RETURN VALUE
   None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qcmap_enable_nat_on_apps(void);

/*===========================================================================
  FUNCTION ()

  DESCRIPTION
   Function to disable the NAT functionality in the network stack

  RETURN VALUE
    boolean with the line state value

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qcmap_disable_nat_on_apps(void);

/*===========================================================================
  FUNCTION  qcmap_set_dhcp_address_range
===========================================================================*/
/*!
@brief
 Function to set address pool to be used by DHCP clients.

@return
  QCMAP_DHCP_ASSIGN_SUCCESS -Successful in setting up DHCP address pool
  QCMAP_DHCP_ASSIGN_FAIL - Failed to set DHCP address pool

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int32_t qcmap_set_dhcp_address_range(void);

/*===========================================================================
  FUNCTION  qcmap_start_dnsc
===========================================================================*/
/*!
@brief
 Function to start the DNSc
@return
  0 -Successful in starting dnsc
  -1 - Failed starting dnsc

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int qcmap_start_dnsc(void);

/*===========================================================================
  FUNCTION  qcmap_add_v4_dns_svr
===========================================================================*/
/*!
@brief
 Function to add v4 DNS servers
 @param
 dss_addr_info_t 
@return
  0 -Successful adding v4 DNS servers
  -1 - Failed adding v4 DNS servers

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=====================*/
int qcmap_add_v4_dns_svr(qapi_DSS_Addr_Info_t *v4addr);

/*===========================================================================
  FUNCTION  qcmap_add_v6_dns_svr
===========================================================================*/
/*!
@brief
 Function to add v6 DNS servers
 @param
 dss_addr_info_t 
@return
  0 -Successful adding v6 DNS servers
  -1 - Failed adding v6 DNS servers

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=====================*/
int qcmap_add_v6_dns_svr(qapi_DSS_Addr_Info_t *v6addr);

void qcmap_set_dhcp_command
(
  void* data,
  uint8 conn_handle
);

/*===========================================================================
  FUNCTION  set_dhcp_command
===========================================================================*/
/*!
@brief
 Function to enable/disable DHCP server
 @param
 uint8_t
 0-disable
 1-enable
@return
 None

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=====================*/
void set_dhcp_command(uint8_t cmd);


#ifdef __cplusplus
}
#endif
#endif /* QCMAP_API_H */

