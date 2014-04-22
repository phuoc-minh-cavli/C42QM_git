#ifndef DINS_SERVICE_01_H
#define DINS_SERVICE_01_H
/**
  @file data_internal_service_v01.h

  @brief This is the public header file which defines the dins service Data structures.

  This header file defines the types and structures that were defined in
  dins. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.


  $Header: //components/rel/qmimsgs.mpss/4.3.4/dins/api/data_internal_service_v01.h#2 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Wed Aug 18 2021 (Spin 0)
   From IDL File: data_internal_service_v01.idl */

/** @defgroup dins_qmi_consts Constant values defined in the IDL */
/** @defgroup dins_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup dins_qmi_enums Enumerated types used in QMI messages */
/** @defgroup dins_qmi_messages Structures sent as QMI messages */
/** @defgroup dins_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup dins_qmi_accessor Accessor for QMI service object */
/** @defgroup dins_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"
#include "data_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup dins_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define DINS_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define DINS_V01_IDL_MINOR_VERS 0x09
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define DINS_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define DINS_V01_MAX_MESSAGE_ID 0x002E
/**
    @}
  */


/** @addtogroup dins_qmi_consts
    @{
  */
#define DINS_MAX_RM_INFO_V01 2
#define DINS_MAX_PREFIX_LEN_V01 16
#define DINS_SW_VERSION_MAX_V01 128
#define DINS_IMAGE_VER_MAX_V01 32
#define DINS_IMAGE_VER_LEN_MAX_V01 128
#define QMI_DINS_IPV6_ADDR_LEN_V01 16
/**
    @}
  */

/** @addtogroup dins_qmi_enums
    @{
  */
typedef enum {
  DINS_BIND_SUBSCRIPTION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DINS_PRIMARY_SUBS_V01 = 0x0001, /**<  Primary \n  */
  DINS_SECONDARY_SUBS_V01 = 0x0002, /**<  Secondary \n  */
  DINS_TERTIARY_SUBS_V01 = 0x0003, /**<  Tertiary  */
  DINS_BIND_SUBSCRIPTION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dins_bind_subscription_enum_v01;
/**
    @}
  */

/** @addtogroup dins_qmi_enums
    @{
  */
typedef enum {
  DINS_SYNC_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DINS_DATA_PATH_REG_TYPE_V01 = 0, /**<  SYNC RM and UM de-registration info   
 SYNC RM and UM registration info   */
  DINS_DATA_PATH_DEREG_TYPE_V01 = 1, /**<  SYNC RM and UM de-registration info  */
  DINS_RM_RECNF_REG_RMNET_TYPE_V01 = 2, /**<  SYNC RM Re config to RMNET   */
  DINS_RM_RECNF_REG_NONRMNET_TYPE_V01 = 3, /**<  SYNC RM Re config to Proxy/Non RMNET  */
  DINS_DATA_PATH_ACK_TYPE_V01 = 4, /**<  Ack for data path info  to remote modem  */
  DINS_REMOTE_SERVICE_UNAVAIL_TYPE_V01 = 5, /**<  Remote service is un-available(SSR)  */
  DINS_SYNC_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dins_sync_enum_type_v01;
/**
    @}
  */

/** @addtogroup dins_qmi_enums
    @{
  */
typedef enum {
  DINS_SYNC_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DINS_SYNC_STATUS_SUCCESS_V01 = 0, 
  DINS_SYNC_STATUS_FAILURE_V01 = 1, 
  DINS_SYNC_STATUS_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dins_sync_status_enum_type_v01;
/**
    @}
  */

/** @addtogroup dins_qmi_enums
    @{
  */
typedef enum {
  DINS_IFACE_NAME_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DINS_UMTS_IFACE_V01 = 0x8004, 
  DINS_SIO_IFACE_V01 = 0x8008, 
  DINS_IFACE_NAME_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dins_iface_name_enum_type_v01;
/**
    @}
  */

/** @addtogroup dins_qmi_enums
    @{
  */
typedef enum {
  DINS_RMNET_SET_DATA_FMT_E_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DINS_RMNET_SET_DATA_FORMAT_NONE_V01 = 0, 
  DINS_RMNET_SET_DATA_FORMAT_QMI_CTL_V01 = 1, 
  DINS_RMNET_SET_DATA_FORMAT_QMI_WDA_V01 = 2, 
  DINS_RMNET_SET_DATA_FORMAT_DTR_HGH_V01 = 3, 
  DINS_RMNET_SET_DATA_FORMAT_DTR_LOW_V01 = 4, 
  DINS_RMNET_SET_DATA_FMT_E_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dins_rmnet_set_data_fmt_e_type_v01;
/**
    @}
  */

/** @addtogroup dins_qmi_enums
    @{
  */
typedef enum {
  DINS_RMNET_SM_LINK_PROT_E_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DINS_RMNET_INVALID_MODE_V01 = 0x0, 
  DINS_RMNET_ETHERNET_MODE_V01 = 0x1, 
  DINS_RMNET_IP_MODE_V01 = 0x2, 
  DINS_RMNET_ALL_MODE_V01 = 0x3, 
  DINS_RMNET_SM_LINK_PROT_E_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dins_rmnet_sm_link_prot_e_type_v01;
/**
    @}
  */

/** @addtogroup dins_qmi_enums
    @{
  */
typedef enum {
  DINS_RMNET_DATA_AGG_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DINS_RMNET_DISABLE_DATA_AGG_V01 = 0, 
  DINS_RMNET_ENABLE_DATA_AGG_MBIM_V01 = 3, 
  DINS_RMNET_ENABLE_DATA_AGG_QMAP_V01 = 5, 
  DINS_RMNET_ENABLE_DATA_AGG_QMAP_V2_V01 = 6, 
  DINS_RMNET_ENABLE_DATA_AGG_QMAP_V3_V01 = 7, 
  DINS_RMNET_ENABLE_DATA_AGG_QMAP_V4_V01 = 8, 
  DINS_RMNET_DATA_AGG_MAX_V01 = 0x7FFFFFFF, 
  DINS_RMNET_DATA_AGG_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dins_rmnet_data_agg_enum_type_v01;
/**
    @}
  */

/** @addtogroup dins_qmi_enums
    @{
  */
typedef enum {
  DINS_DATA_POWERSAVE_CONFIG_MASK_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DINS_DATA_POWERSAVE_CONFIG_NOT_SUPPORTED_V01 = 0x00, /**<  Not supported mask \n  */
  DINS_DATA_POWERSAVE_CONFIG_DL_MARKER_V01 = 0x01, /**<  Control downlink marker notification to TE \n  */
  DINS_DATA_POWERSAVE_CONFIG_FLOW_CTL_V01 = 0x02, /**<  Control Flow enable/disable indication \n  */
  DINS_DATA_POWERSAVE_CONFIG_ALL_MASK_V01 = 0x7FFFFFFF, /**<  Control all notifications supported in this mask \n  */
  DINS_DATA_POWERSAVE_CONFIG_MASK_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dins_data_powersave_config_mask_enum_v01;
/**
    @}
  */

/** @addtogroup dins_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t is_valid;

  dins_rmnet_set_data_fmt_e_type_v01 data_format_client;

  dins_rmnet_sm_link_prot_e_type_v01 link_prot;

  uint8_t qos;

  uint8_t mbim_sw_deagg;

  dins_rmnet_data_agg_enum_type_v01 ul_data_agg_protocol;

  dins_rmnet_data_agg_enum_type_v01 dl_data_agg_protocol;

  uint32_t ndp_signature;

  uint32_t ncm_signature;

  uint32_t dl_data_agg_max_num;

  uint32_t dl_data_agg_max_size;

  uint32_t ul_data_agg_max_num;

  uint32_t ul_data_agg_max_size;

  uint32_t dl_min_padding;

  uint8_t te_flow_control;
}dins_rmnet_data_format_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dins_qmi_aggregates
    @{
  */
/** 
 */
typedef struct {

  uint8_t is_valid;

  dins_iface_name_enum_type_v01 name;

  uint8_t instance;
}dins_iface_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dins_qmi_aggregates
    @{
  */
/** 
 */
typedef struct {

  uint8_t is_valid;
  /**<   True: Valid info present. False: Otherwise */

  uint8_t prefix_len;
  /**<   The length of IPv6 prefix in number of bits */

  uint8_t prefix[DINS_MAX_PREFIX_LEN_V01];
  /**<   IPv6 prefix in network byte order */
}dins_addr_mgmt_prefix_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dins_qmi_aggregates
    @{
  */
/** 
 */
typedef struct {

  uint8_t is_valid;
  /**<   True: Valid info present. False: Otherwise */

  uint8_t is_proxy_rm_call;
  /**<   True: for NonRmNet. False: for RmNet */

  uint8_t mux_id;
  /**<   MUX id for RmNet call, Fake MUX Id for othercases */

  dins_rmnet_data_format_type_v01 data_format;
}dins_rm_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dins_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t is_valid;
  /**<   True: Valid info present. False: Otherwise */

  uint32_t uid;
  /**<   Unique identifier assigned for Bearer*/

  uint8_t bearer_id;
  /**<  Bearer ID*/

  dins_iface_info_type_v01 v4_um_iface_name;
  /**<  V4 UM iface name and instance*/

  dins_iface_info_type_v01 v6_um_iface_name;
  /**<  V6 UM iface name and instance*/
}dins_um_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dins_qmi_aggregates
    @{
  */
typedef struct {

  uint64_t prefix;
  /**<   IPv6 prefix.           */

  uint8_t prefix_len;
  /**<   Length of the prefix.  */

  uint64_t iid;
  /**<   IPv6 IID.              */
}dins_ipv6_addr_info_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dins_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t is_valid;
  /**<   True: Valid info present. False: Otherwise */

  dins_sync_enum_type_v01 sync_type;
  /**<   Sync type */

  dins_um_info_type_v01 dpm_um_info;
  /**<   UM Configuration  */

  uint8_t num_rm_info;

  dins_rm_info_type_v01 dpm_rm_info[DINS_MAX_RM_INFO_V01];
  /**<   RM Configuration  */
}dins_data_path_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dins_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t is_valid;
  /**<   true: valid info present. false: otherwise */

  dins_sync_enum_type_v01 sync_type;
  /**<   Sync type */

  dins_rm_info_type_v01 old_rm_info;
  /**<   Deregister old RM info*/

  dins_rm_info_type_v01 new_rm_info;
  /**<   Register new RM info */

  dins_iface_info_type_v01 v4_rm_iface_name;
  /**<  V4 RM iface name and instance*/

  dins_iface_info_type_v01 v6_rm_iface_name;
  /**<  V6 RM iface name and instance*/
}dins_rm_reconfig_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dins_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t is_valid;
  /**<   true: valid info present. false: otherwise */

  uint32_t uid;
  /**<   Unique identifier */

  dins_sync_status_enum_type_v01 status;
  /**<   Staus */

  uint32_t bearer_id;
  /**<  Bearer ID*/

  uint8_t mux_id;
  /**<  Mux ID. in case of re-reg RM it will be new Mux ID */

  dins_sync_enum_type_v01 sync_type;

  dins_iface_info_type_v01 v4_um_iface_name;
  /**<  V4 UM iface name and instance*/

  dins_iface_info_type_v01 v6_um_iface_name;
  /**<  V6 UM iface name and instance*/

  dins_iface_info_type_v01 v4_rm_iface_name;
  /**<  V4 RM iface name and instance*/

  dins_iface_info_type_v01 v6_rm_iface_name;
  /**<  V6 RM iface name and instance*/
}dins_info_status_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dins_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t ipv6_addr[QMI_DINS_IPV6_ADDR_LEN_V01];
  /**<   IPv6 address (in network byte order). The
       address is a 16-element array of 8-bit numbers, each
       of which is in big-endian format.
  */

  uint8_t ipv6_prefix_length;
  /**<   IPv6 prefix length in number of bits. Range: 0 to 128.
  */
}dins_ipv6_addr_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Request Message; Synchronize UM and RM information for data path setup */
typedef struct {

  /* Optional */
  /*  Data Path information */
  uint8_t data_path_info_valid;  /**< Must be set to true if data_path_info is being passed */
  dins_data_path_info_type_v01 data_path_info;

  /* Optional */
  /*  IPV6 Prefix information */
  uint8_t prefix_info_valid;  /**< Must be set to true if prefix_info is being passed */
  uint32_t prefix_info_len;  /**< Must be set to # of elements in prefix_info */
  dins_addr_mgmt_prefix_type_v01 prefix_info[DINS_MAX_RM_INFO_V01];

  /* Optional */
  /*  Bearer ID EX */
  uint8_t bearer_id_ex_valid;  /**< Must be set to true if bearer_id_ex is being passed */
  uint8_t bearer_id_ex;

  /* Optional */
  /*  IPV4 address */
  uint8_t ipv4_addr_valid;  /**< Must be set to true if ipv4_addr is being passed */
  uint32_t ipv4_addr;

  /* Optional */
  /*  IPV6 address */
  uint8_t ipv6_addr_valid;  /**< Must be set to true if ipv6_addr is being passed */
  dins_ipv6_addr_info_v01 ipv6_addr;
}dins_data_path_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Response Message; Synchronize UM and RM information for data path setup */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  Unique Identifier */
  uint8_t uid_valid;  /**< Must be set to true if uid is being passed */
  uint32_t uid;

  /* Optional */
  /*  Bearer Identifier */
  uint8_t bearer_id_valid;  /**< Must be set to true if bearer_id is being passed */
  uint8_t bearer_id;

  /* Optional */
  /*  MUX ID */
  uint8_t mux_id_valid;  /**< Must be set to true if mux_id is being passed */
  uint8_t mux_id;
}dins_data_path_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Request Message; Send Rm reconfiguration info to the remote modem */
typedef struct {

  /* Optional */
  /*  Unique Identifier */
  uint8_t uid_valid;  /**< Must be set to true if uid is being passed */
  uint32_t uid;

  /* Optional */
  /*  RM reconfiguration information */
  uint8_t rm_reconf_info_valid;  /**< Must be set to true if rm_reconf_info is being passed */
  dins_rm_reconfig_info_type_v01 rm_reconf_info;

  /* Optional */
  /*  IPV6 Prefix information for the new RM */
  uint8_t prefix_info_valid;  /**< Must be set to true if prefix_info is being passed */
  dins_addr_mgmt_prefix_type_v01 prefix_info;
}dins_rm_reconfig_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Response Message; Send Rm reconfiguration info to the remote modem */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}dins_rm_reconfig_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Request Message; Send data path setup status information to the remote modem */
typedef struct {

  /* Optional */
  /*  Data Path setup status information */
  uint8_t status_valid;  /**< Must be set to true if status is being passed */
  dins_info_status_type_v01 status;
}dins_data_path_info_status_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Response Message; Send data path setup status information to the remote modem */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}dins_data_path_info_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Request Message; Send Rm reconfiguration status to the remote modem */
typedef struct {

  /* Optional */
  /*  RM reconfiguration status information */
  uint8_t status_valid;  /**< Must be set to true if status is being passed */
  dins_info_status_type_v01 status;
}dins_rm_reconfig_status_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Response Message; Send Rm reconfiguration status to the remote modem */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}dins_rm_reconfig_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Request Message; Send IP v6 prefix reconfiguration info to the remote modem */
typedef struct {

  /* Optional */
  /*  MUX ID */
  uint8_t mux_id_valid;  /**< Must be set to true if mux_id is being passed */
  uint8_t mux_id;

  /* Optional */
  /*  IPV6 Prefix information */
  uint8_t prefix_info_valid;  /**< Must be set to true if prefix_info is being passed */
  dins_addr_mgmt_prefix_type_v01 prefix_info;
}dins_prefix_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Response Message; Send IP v6 prefix reconfiguration info to the remote modem */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}dins_prefix_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Request Message; Sets the powersave configuration. */
typedef struct {

  /* Mandatory */
  /*  Peripheral Endpoint ID */
  data_ep_id_type_v01 ep_id;
  /**<   \n The peripheral endpoint (physical data channel) for which
       the power mode should apply.
  */

  /* Optional */
  /*  Requested Data Powersave Configuration */
  uint8_t req_data_cfg_valid;  /**< Must be set to true if req_data_cfg is being passed */
  dins_data_powersave_config_mask_enum_v01 req_data_cfg;
  /**<   Configure mask for data.
  */
}dins_set_powersave_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Response Message; Sets the powersave configuration. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Data Powersave Configuration */
  uint8_t data_cfg_valid;  /**< Must be set to true if data_cfg is being passed */
  dins_data_powersave_config_mask_enum_v01 data_cfg;
  /**<   Supported data mask.
  */
}dins_set_powersave_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Request Message; Sets the powersave mode. */
typedef struct {

  /* Mandatory */
  /*  Powersave control flag */
  uint8_t powersave_control_flag;
  /**<   Indicates enable/disable powersave. 
       Values:\n
       - 0 -- Disable(Default) \n
       - 1 -- Enable
  */

  /* Optional */
  /*  Requested Data Powersave Configuration */
  uint8_t req_data_cfg_valid;  /**< Must be set to true if req_data_cfg is being passed */
  dins_data_powersave_config_mask_enum_v01 req_data_cfg;
  /**<   Configure mask for data.
  */
}dins_set_powersave_mode_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Response Message; Sets the powersave mode. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}dins_set_powersave_mode_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Request Message; Send updated IP address to the remote modem */
typedef struct {

  /* Optional */
  /*  MUX ID */
  uint8_t mux_id_valid;  /**< Must be set to true if mux_id is being passed */
  uint8_t mux_id;

  /* Optional */
  /*  IPV4 address */
  uint8_t ipv4_addr_valid;  /**< Must be set to true if ipv4_addr is being passed */
  uint32_t ipv4_addr;

  /* Optional */
  /*  IPV6 address */
  uint8_t ipv6_addr_valid;  /**< Must be set to true if ipv6_addr is being passed */
  dins_ipv6_addr_info_v01 ipv6_addr;
}dins_addr_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Response Message; Send updated IP address to the remote modem */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}dins_addr_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_enums
    @{
  */
typedef enum {
  DINS_IMAGE_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DINS_IMAGE_UNKNOWN_V01 = 0, /**<  Unknown \n  */
  DINS_IMAGE_SBL_V01 = 1, /**<  SBL \n  */
  DINS_IMAGE_TZ_V01 = 2, /**<  TZ \n  */
  DINS_IMAGE_TZSECAPP_V01 = 3, /**<  TZSECAPP \n  */
  DINS_IMAGE_RPM_V01 = 4, /**<  RPM \n  */
  DINS_IMAGE_SDI_V01 = 5, /**<  SDI \n  */
  DINS_IMAGE_HYPERVISOR_V01 = 6, /**<  Hypervisor \n  */
  DINS_IMAGE_APPSBL_V01 = 7, /**<  APPSBL \n  */
  DINS_IMAGE_APPS_V01 = 8, /**<  Applications \n  */
  DINS_IMAGE_MPSS_V01 = 9, /**<  MPSS \n  */
  DINS_IMAGE_ADSP_V01 = 10, /**<  ADSP \n  */
  DINS_IMAGE_WCNS_V01 = 11, /**<  WCNS \n  */
  DINS_IMAGE_VENUS_V01 = 12, /**<  Venus  */
  DINS_IMAGE_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dins_image_type_enum_v01;
/**
    @}
  */

/** @addtogroup dins_qmi_aggregates
    @{
  */
typedef struct {

  dins_image_type_enum_v01 image_type;
  /**<   Image type. Values: \n
      - DINS_IMAGE_UNKNOWN (0) --  Unknown \n 
      - DINS_IMAGE_SBL (1) --  SBL \n 
      - DINS_IMAGE_TZ (2) --  TZ \n 
      - DINS_IMAGE_TZSECAPP (3) --  TZSECAPP \n 
      - DINS_IMAGE_RPM (4) --  RPM \n 
      - DINS_IMAGE_SDI (5) --  SDI \n 
      - DINS_IMAGE_HYPERVISOR (6) --  Hypervisor \n 
      - DINS_IMAGE_APPSBL (7) --  APPSBL \n 
      - DINS_IMAGE_APPS (8) --  Applications \n 
      - DINS_IMAGE_MPSS (9) --  MPSS \n 
      - DINS_IMAGE_ADSP (10) --  ADSP \n 
      - DINS_IMAGE_WCNS (11) --  WCNS \n 
      - DINS_IMAGE_VENUS (12) --  Venus 
 */

  char image_ver[DINS_IMAGE_VER_LEN_MAX_V01 + 1];
  /**<   Image version string. The format of the string is 
       [QC version]:[Variant]:[OEM version]
  */
}dins_image_ver_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Request Message; Sends the software version of the device. */
typedef struct {

  /* Optional */
  /*  Image Versions */
  uint8_t image_versions_valid;  /**< Must be set to true if image_versions is being passed */
  uint32_t image_versions_len;  /**< Must be set to # of elements in image_versions */
  dins_image_ver_type_v01 image_versions[DINS_IMAGE_VER_MAX_V01];
  /**<   \n Image versions of the builds loaded on the device.
  */

  /* Optional */
  /*  Software Version Information */
  uint8_t sw_version_valid;  /**< Must be set to true if sw_version is being passed */
  char sw_version[DINS_SW_VERSION_MAX_V01 + 1];
  /**<   String representing the software version information.
  */
}dins_send_sw_version_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Response Message; Sends the software version of the device. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}dins_send_sw_version_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Request Message; Associates the requesting control point to the subscription requested. */
typedef struct {

  /* Mandatory */
  /*  Bind Subscription */
  dins_bind_subscription_enum_v01 bind_subs;
  /**<   Subscription to bind to. Values: \n
      - DINS_PRIMARY_SUBS (0x0001) --  Primary \n 
      - DINS_SECONDARY_SUBS (0x0002) --  Secondary \n 
      - DINS_TERTIARY_SUBS (0x0003) --  Tertiary 
 */
}dins_bind_subscription_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Response Message; Associates the requesting control point to the subscription requested. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
            */
}dins_bind_subscription_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Request Message; Queries the subscription associated with the control point. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}dins_get_bind_subscription_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Response Message; Queries the subscription associated with the control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.       
 Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
   */

  /* Optional */
  /*  Bound Subscription */
  uint8_t bind_subscription_valid;  /**< Must be set to true if bind_subscription is being passed */
  dins_bind_subscription_enum_v01 bind_subscription;
  /**<   Values: \n
      - DINS_PRIMARY_SUBS (0x0001) --  Primary \n 
      - DINS_SECONDARY_SUBS (0x0002) --  Secondary \n 
      - DINS_TERTIARY_SUBS (0x0003) --  Tertiary 
 */
}dins_get_bind_subscription_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_enums
    @{
  */
typedef enum {
  DINS_IWLAN_CAPABILITY_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DINS_IWLAN_CAPABILITY_ENABLE_V01 = 0, /**<  Enable IWLAN Capability (Default)   
 Disable IWLAN Capability   */
  DINS_IWLAN_CAPABILITY_DISABLE_V01 = 1, 
  DINS_IWLAN_CAPABILITY_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dins_iwlan_capability_enum_type_v01;
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Request Message; Set modem iWLAN capability */
typedef struct {

  /* Optional */
  /*  IWLAN S2B Capability Information */
  uint8_t iwlan_s2b_capability_valid;  /**< Must be set to true if iwlan_s2b_capability is being passed */
  dins_iwlan_capability_enum_type_v01 iwlan_s2b_capability;
}dins_set_modem_iwlan_capability_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Response Message; Set modem iWLAN capability */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}dins_set_modem_iwlan_capability_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Request Message; Sends Public V6 IP address to the Control Point */
typedef struct {

  /* Optional */
  /*  Peripheral Endpoint ID */
  uint8_t ep_id_valid;  /**< Must be set to true if ep_id is being passed */
  data_ep_id_type_v01 ep_id;
  /**<   \n(The peripheral endpoint (physical data channel) to which
       the client binds.)
  */

  /* Optional */
  /*  Mux ID */
  uint8_t mux_id_valid;  /**< Must be set to true if mux_id is being passed */
  uint8_t mux_id;
  /**<   The mux ID of the logical data channel to which
       the client binds; default value is 0. 
  */
}dins_get_v6_addr_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Response Message; Sends Public V6 IP address to the Control Point */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  IPV6 address */
  uint8_t ipv6_addr_valid;  /**< Must be set to true if ipv6_addr is being passed */
  dins_ipv6_addr_type_v01 ipv6_addr;
}dins_get_v6_addr_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Request Message; 	Configure optimizations during downlink data transfers for throughputs. */
typedef struct {

  /* Mandatory */
  /*  BSR optimization setting. */
  uint8_t bsr_optimization;
  /**<   Values: \n
       - 0 -- BSR optimization is disabled \n
       - 1 -- BSR optimization is enabled
  */
}dins_set_dl_throughput_optimization_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Response Message; 	Configure optimizations during downlink data transfers for throughputs. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}dins_set_dl_throughput_optimization_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Request Message; 	Query the DL throughput optimization information. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}dins_get_dl_throughput_optimization_info_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup dins_qmi_messages
    @{
  */
/** Response Message; 	Query the DL throughput optimization information. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  BSR optimization setting. */
  uint8_t bsr_optimization_valid;  /**< Must be set to true if bsr_optimization is being passed */
  uint8_t bsr_optimization;
  /**<   Values: \n
       - 0 -- BSR optimization is disabled \n
       - 1 -- BSR optimization is enabled
  */
}dins_get_dl_throughput_optimization_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_DINS_BIND_SUBSCRIPTION_V01
//#define REMOVE_QMI_DINS_DATA_PATH_INFO_V01
//#define REMOVE_QMI_DINS_DATA_PATH_INFO_STATUS_V01
//#define REMOVE_QMI_DINS_GET_BIND_SUBSCRIPTION_V01
//#define REMOVE_QMI_DINS_GET_DL_THROUGHPUT_OPTIMIZATION_INFO_V01
//#define REMOVE_QMI_DINS_GET_V6_ADDR_V01
//#define REMOVE_QMI_DINS_RM_RECONFIG_INFO_V01
//#define REMOVE_QMI_DINS_RM_RECONFIG_STATUS_V01
//#define REMOVE_QMI_DINS_SEND_SW_VERSION_V01
//#define REMOVE_QMI_DINS_SET_DL_THROUGHPUT_OPTIMIZATION_INFO_V01
//#define REMOVE_QMI_DINS_SET_MODEM_IWLAN_CAPABILITY_V01
//#define REMOVE_QMI_DINS_SET_POWERSAVE_CONFIG_V01
//#define REMOVE_QMI_DINS_SET_POWERSAVE_MODE_V01
//#define REMOVE_QMI_DINS_UPDATE_ADDR_INFO_V01
//#define REMOVE_QMI_DINS_UPDATE_PREFIX_INFO_V01

/*Service Message Definition*/
/** @addtogroup dins_qmi_msg_ids
    @{
  */
#define QMI_DINS_DATA_PATH_INFO_REQ_V01 0x0020
#define QMI_DINS_DATA_PATH_INFO_RESP_V01 0x0020
#define QMI_DINS_RM_RECONFIG_INFO_REQ_V01 0x0021
#define QMI_DINS_RM_RECONFIG_INFO_RESP_V01 0x0021
#define QMI_DINS_DATA_PATH_INFO_STATUS_REQ_V01 0x0022
#define QMI_DINS_DATA_PATH_INFO_STATUS_RESP_V01 0x0022
#define QMI_DINS_RM_RECONFIG_STATUS_REQ_V01 0x0023
#define QMI_DINS_RM_RECONFIG_STATUS_RESP_V01 0x0023
#define QMI_DINS_PREFIX_INFO_REQ_V01 0x0024
#define QMI_DINS_PREFIX_INFO_RESP_V01 0x0024
#define QMI_DINS_SET_POWERSAVE_CONFIG_REQ_V01 0x0025
#define QMI_DINS_SET_POWERSAVE_CONFIG_RESP_V01 0x0025
#define QMI_DINS_SET_POWERSAVE_MODE_REQ_V01 0x0026
#define QMI_DINS_SET_POWERSAVE_MODE_RESP_V01 0x0026
#define QMI_DINS_ADDR_INFO_REQ_V01 0x0027
#define QMI_DINS_ADDR_INFO_RESP_V01 0x0027
#define QMI_DINS_SEND_SW_VERSION_REQ_V01 0x0028
#define QMI_DINS_SEND_SW_VERSION_RESP_V01 0x0028
#define QMI_DINS_BIND_SUBSCRIPTION_REQ_V01 0x0029
#define QMI_DINS_BIND_SUBSCRIPTION_RESP_V01 0x0029
#define QMI_DINS_GET_BIND_SUBSCRIPTION_REQ_V01 0x002A
#define QMI_DINS_GET_BIND_SUBSCRIPTION_RESP_V01 0x002A
#define QMI_DINS_SET_MODEM_IWLAN_CAPABILITY_REQ_V01 0x002B
#define QMI_DINS_SET_MODEM_IWLAN_CAPABILITY_RESP_V01 0x002B
#define QMI_DINS_GET_V6_ADDR_REQ_V01 0x002C
#define QMI_DINS_GET_V6_ADDR_RESP_V01 0x002C
#define QMI_DINS_SET_DL_THROUGHPUT_OPTIMIZATION_INFO_REQ_V01 0x002D
#define QMI_DINS_SET_DL_THROUGHPUT_OPTIMIZATION_INFO_RESP_V01 0x002D
#define QMI_DINS_GET_DL_THROUGHPUT_OPTIMIZATION_INFO_REQ_V01 0x002E
#define QMI_DINS_GET_DL_THROUGHPUT_OPTIMIZATION_INFO_RESP_V01 0x002E
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro dins_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type dins_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define dins_get_service_object_v01( ) \
          dins_get_service_object_internal_v01( \
            DINS_V01_IDL_MAJOR_VERS, DINS_V01_IDL_MINOR_VERS, \
            DINS_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

