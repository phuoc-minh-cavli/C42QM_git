#ifndef WDS_EXT_SERVICE_01_H
#define WDS_EXT_SERVICE_01_H
/**
  @file wireless_data_service_ext_v01.h

  @brief This is the public header file which defines the wds_ext service Data structures.

  This header file defines the types and structures that were defined in
  wds_ext. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2006-2015 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.


  $Header: //components/rel/qmimsgs.mpss/4.3.4/wds_ext/api/wireless_data_service_ext_v01.h#2 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Thu Jan 17 2019 (Spin 0)
   From IDL File: wireless_data_service_ext_v01.idl */

/** @defgroup wds_ext_qmi_consts Constant values defined in the IDL */
/** @defgroup wds_ext_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup wds_ext_qmi_enums Enumerated types used in QMI messages */
/** @defgroup wds_ext_qmi_messages Structures sent as QMI messages */
/** @defgroup wds_ext_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup wds_ext_qmi_accessor Accessor for QMI service object */
/** @defgroup wds_ext_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"
#include "data_common_v01.h"
#include "wireless_data_service_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup wds_ext_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define WDS_EXT_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define WDS_EXT_V01_IDL_MINOR_VERS 0x05
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define WDS_EXT_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define WDS_EXT_V01_MAX_MESSAGE_ID 0xFFF2
/**
    @}
  */


/** @addtogroup wds_ext_qmi_consts
    @{
  */
/**
    @}
  */

/** @addtogroup wds_ext_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t iface_handle;
  /**<   IFACE handle that best matches the policy provided in the request. 
       The QMI_WDS_START_NETWORK_INTERFACE command must specify the IFACE handle.
  */

  uint32_t priority;
  /**<   Priority of the interface returned.
  */

  uint16_t technology_name;
  /**<   Technology name corresponding to this interface handle.
  */
}wds_iface_handle_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wds_ext_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t iface_priority_mask;
  /**<   IFACE priority mask.
  */

  uint8_t is_iface_arb_candiate;
  /**<   IFACE arbitration candidate flag.
  */
}wds_arb_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wds_ext_qmi_enums
    @{
  */
typedef enum {
  WDS_ROUTE_LOOK_UP_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  WDS_RL_TYPE_BRINGUP_V01 = 0, /**<  Bringup \n */
  WDS_RL_TYPE_IFACE_V01 = 1, /**< IFACE \n  */
  WDS_RL_TYPE_DATAPATH_V01 = 2, /**<  Data path  */
  WDS_ROUTE_LOOK_UP_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}wds_route_look_up_enum_v01;
/**
    @}
  */

/** @addtogroup wds_ext_qmi_messages
    @{
  */
/** Request Message; Determines the interface to be used.  */
typedef struct {

  /* Mandatory */
  /*  Route Lookup Type */
  wds_route_look_up_enum_v01 route_look_up_type;
  /**<   Route lookup type. Values: \n
      - WDS_RL_TYPE_BRINGUP (0) --  Bringup \n
      - WDS_RL_TYPE_IFACE (1) -- IFACE \n 
      - WDS_RL_TYPE_DATAPATH (2) --  Data path 
 */

  /* Optional */
  /*  APN Name */
  uint8_t apn_name_valid;  /**< Must be set to true if apn_name is being passed */
  char apn_name[QMI_WDS_APN_NAME_MAX_V01 + 1];
  /**<   APN name.
  */

  /* Optional */
  /*  IP Family */
  uint8_t ip_family_pref_valid;  /**< Must be set to true if ip_family_pref is being passed */
  wds_ip_family_preference_enum_v01 ip_family_pref;
  /**<   IP family preference. Values: \n
      - WDS_IP_FAMILY_PREF_NON_IP (0x02) -- 
      - WDS_IP_FAMILY_PREF_IPV4 (0x04) -- 
      - WDS_IP_FAMILY_PREF_IPV6 (0x06) -- 
      - WDS_IP_FAMILY_PREF_UNSPEC (0x08) -- 
 */

  /* Optional */
  /*  UMTS Profile */
  uint8_t umts_profile_valid;  /**< Must be set to true if umts_profile is being passed */
  uint8_t umts_profile;
  /**<   3GPP profile.
  */

  /* Optional */
  /*  CDMA Profile */
  uint8_t cdma_profile_valid;  /**< Must be set to true if cdma_profile is being passed */
  uint8_t cdma_profile;
  /**<   3GPP2 profile.
  */

  /* Optional */
  /*  Technology Preference */
  uint8_t ext_tech_pref_valid;  /**< Must be set to true if ext_tech_pref is being passed */
  wds_ext_tech_pref_enum_v01 ext_tech_pref;
  /**<   Extended technology preference. Values: \n
      - WDS_EXT_TECH_PREF_CDMA (-32767) -- 
      - WDS_EXT_TECH_PREF_UMTS (-32764) -- 
      - WDS_EXT_TECH_PREF_EMBMS (-30590) -- 
      - WDS_EXT_TECH_PREF_MODEM_LINK_LOCAL (-30584) -- 
      - WDS_EXT_TECH_PREF_NON_IP (-30588) -- 
 */

  /* Optional */
  /*  Enable MO Exceptional Data Capability */
  uint8_t mo_exceptional_data_capability_valid;  /**< Must be set to true if mo_exceptional_data_capability is being passed */
  uint8_t mo_exceptional_data_capability;
  /**<   - 1 -- TRUE -- Device attempts to lookup up
       for a data call that supports mo exceptional data \n
	   - 0 -- FALSE -- Default
  */

  /* Optional */
  /*  APN Type Enum */
  uint8_t apn_type_valid;  /**< Must be set to true if apn_type is being passed */
  wds_apn_type_enum_v01 apn_type;
  /**<   Values: \n 
      - WDS_APN_TYPE_UNSPECIFIED (0) --  APN type unspecified \n 
      - WDS_APN_TYPE_INTERNET (1) --  APN type for internet traffic \n 
      - WDS_APN_TYPE_IMS (2) --  APN type for IMS \n 
      - WDS_APN_TYPE_VSIM (3) --  APN type for virtual or remote SIM   
      - WDS_APN_TYPE_MMS (4) --  APN type for MMS   
      - WDS_APN_TYPE_DUN (5) --  APN type for DUN   
      - WDS_APN_TYPE_SUPL (6) --  APN type for SUPL   
      - WDS_APN_TYPE_HIPRI (7) --  APN type for HIPRI   
      - WDS_APN_TYPE_FOTA (8) --  APN type for FOTA   
      - WDS_APN_TYPE_CBS (9) --  APN type for CBS   
      - WDS_APN_TYPE_IA (10) --  APN type for IA  
      - WDS_APN_TYPE_EMERGENCY (11) --  APN type for Emergency   
 */

  /* Optional */
  /*  Bring Up By APN Name */
  uint8_t bring_up_by_apn_name_valid;  /**< Must be set to true if bring_up_by_apn_name is being passed */
  uint8_t bring_up_by_apn_name;
  /**<   Values: \n
       - 1 -- TRUE -- Device attempts to look up
             with the APN name. \n
	   - 0 -- FALSE -- Default.
  */

  /* Optional */
  /*  Bring Up By APN Type */
  uint8_t bring_up_by_apn_type_valid;  /**< Must be set to true if bring_up_by_apn_type is being passed */
  uint8_t bring_up_by_apn_type;
  /**<   Values: \n
       - 1 -- TRUE -- Device attempts to look up
             with the APN TYPE. \n
	   - 0 -- FALSE -- Default.
  */
}wds_ext_route_look_up_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wds_ext_qmi_messages
    @{
  */
/** Response Message; Determines the interface to be used.  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Mandatory */
  /*  Interface Handle */
  wds_iface_handle_type_v01 iface_handle;

  /* Optional */
  /*  QMI Instance */
  uint8_t qmi_instance_valid;  /**< Must be set to true if qmi_instance is being passed */
  uint8_t qmi_instance;
  /**<   Specifies the QMI link that has a data call up
       on the interface indicated by the Interface Handle TLV. 
	   This TLV is returned only if a call is up on the interface determined
       by the route lookup operation. 
   */

  /* Optional */
  /*  Arbitration Information */
  uint8_t arb_info_valid;  /**< Must be set to true if arb_info is being passed */
  wds_arb_info_type_v01 arb_info;

  /* Optional */
  /*  Peripheral End Point ID */
  uint8_t ep_id_valid;  /**< Must be set to true if ep_id is being passed */
  data_ep_id_type_v01 ep_id;
  /**<   \n Specifies the peripheral end point of the RmNet instance where a data call is
       already present. This TLV is returned only if a call is up on the
       interface determined by the route lookup operation.
  */

  /* Optional */
  /*  Mux ID */
  uint8_t mux_id_valid;  /**< Must be set to true if mux_id is being passed */
  uint8_t mux_id;
  /**<   The Mux ID of the RmNet instance where a data call is already present.
       This TLV is returned only if a call is up on the interface determined
       by the route lookup operation.
  */
}wds_ext_route_look_up_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wds_ext_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t max;
  /**<   Maximum queueing delay (in 3.4 milliseconds precision). */

  uint32_t min;
  /**<   Minimum queueing delay (in 3.4 milliseconds precision). */

  uint32_t avg;
  /**<   Average queueing delay (in 3.4 milliseconds precision). */

  uint32_t num_samples;
  /**<   Number of samples. */
}wds_delay_metrics_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wds_ext_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t bearer_id;
  /**<   Bearer ID. */

  wds_delay_metrics_type_v01 rm_l2_delay;
  /**<   RM to L2 delay. */

  wds_delay_metrics_type_v01 l2_tx_delay;
  /**<   L2 Tx delay. */
}wds_bearer_delay_metrics_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup wds_ext_qmi_messages
    @{
  */
/** Request Message; Retrieves the uplink packet delay metrics. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}wds_get_ul_delay_metrics_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup wds_ext_qmi_messages
    @{
  */
/** Response Message; Retrieves the uplink packet delay metrics. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  UL Delay Metrics */
  uint8_t ul_delay_metrics_valid;  /**< Must be set to true if ul_delay_metrics is being passed */
  uint32_t ul_delay_metrics_len;  /**< Must be set to # of elements in ul_delay_metrics */
  wds_bearer_delay_metrics_type_v01 ul_delay_metrics[QMI_WDS_MAX_BEARER_CONTEXT_V01];
}wds_get_ul_delay_metrics_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wds_ext_qmi_messages
    @{
  */
/** Request Message; Resets the uplink packet delay metrics. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}wds_reset_ul_delay_metrics_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup wds_ext_qmi_messages
    @{
  */
/** Response Message; Resets the uplink packet delay metrics. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}wds_reset_ul_delay_metrics_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wds_ext_qmi_messages
    @{
  */
/** Request Message; Registers for generating uplink/downlink throughput 
           information. */
typedef struct {

  /* Optional */
  /*  Uplink/Downlink Throughput Interval */
  uint8_t ul_dl_throughput_interval_valid;  /**< Must be set to true if ul_dl_throughput_interval is being passed */
  uint32_t ul_dl_throughput_interval;
  /**<   Period at which throughput information 
       is generated, in milliseconds.
  */
}wds_ul_dl_throughput_ind_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wds_ext_qmi_messages
    @{
  */
/** Response Message; Registers for generating uplink/downlink throughput 
           information. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}wds_ul_dl_throughput_ind_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup wds_ext_qmi_messages
    @{
  */
/** Indication Message; Indicates uplink/downlink throughput information. */
typedef struct {

  /* Optional */
  /*  Uplink Achievable Throughput */
  uint8_t ul_achievable_throughput_valid;  /**< Must be set to true if ul_achievable_throughput is being passed */
  uint32_t ul_achievable_throughput;
  /**<   Uplink achievable rate per UE in kbps.
  */

  /* Optional */
  /*  Downlink Achievable Throughput */
  uint8_t dl_achievable_throughput_valid;  /**< Must be set to true if dl_achievable_throughput is being passed */
  uint32_t dl_achievable_throughput;
  /**<   Downlink achievable rate per UE in kbps.
  */

  /* Optional */
  /*  Uplink Actual Throughput */
  uint8_t ul_actual_throughput_valid;  /**< Must be set to true if ul_actual_throughput is being passed */
  uint32_t ul_actual_throughput;
  /**<   Uplink rate per UE in kbps.
  */

  /* Optional */
  /*  Downlink Actual Throughput */
  uint8_t dl_actual_throughput_valid;  /**< Must be set to true if dl_actual_throughput is being passed */
  uint32_t dl_actual_throughput;
  /**<   Downlink rate per UE in kbps.
  */
}wds_ul_dl_throughput_info_ind_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_WDS_EXT_ROUTE_LOOK_UP_V01 
//#define REMOVE_QMI_WDS_GET_UL_DELAY_METRICS_V01 
//#define REMOVE_QMI_WDS_RESET_UL_DELAY_METRICS_V01 
//#define REMOVE_QMI_WDS_UL_DL_THROUGHPUT_IND_REGISTER_V01 
//#define REMOVE_QMI_WDS_UL_DL_THROUGHPUT_INFO_IND_V01 

/*Service Message Definition*/
/** @addtogroup wds_ext_qmi_msg_ids
    @{
  */
#define QMI_WDS_UL_DL_THROUGHPUT_IND_REGISTER_REQ_V01 0xFFEE
#define QMI_WDS_UL_DL_THROUGHPUT_IND_REGISTER_RESP_V01 0xFFEE
#define QMI_WDS_UL_DL_THROUGHPUT_INFO_IND_V01 0xFFEF
#define QMI_WDS_GET_UL_DELAY_METRICS_REQ_V01 0xFFF0
#define QMI_WDS_GET_UL_DELAY_METRICS_RESP_V01 0xFFF0
#define QMI_WDS_RESET_UL_DELAY_METRICS_REQ_V01 0xFFF1
#define QMI_WDS_RESET_UL_DELAY_METRICS_RESP_V01 0xFFF1
#define QMI_WDS_EXT_ROUTE_LOOK_UP_REQ_V01 0xFFF2
#define QMI_WDS_EXT_ROUTE_LOOK_UP_RESP_V01 0xFFF2
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro wds_ext_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type wds_ext_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define wds_ext_get_service_object_v01( ) \
          wds_ext_get_service_object_internal_v01( \
            WDS_EXT_V01_IDL_MAJOR_VERS, WDS_EXT_V01_IDL_MINOR_VERS, \
            WDS_EXT_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

