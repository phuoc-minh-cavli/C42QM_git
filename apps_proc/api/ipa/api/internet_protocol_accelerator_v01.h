#ifndef IPA_SERVICE_01_H
#define IPA_SERVICE_01_H
/**
  @file internet_protocol_accelerator_v01.h

  @brief This is the public header file which defines the ipa service Data structures.

  This header file defines the types and structures that were defined in
  ipa. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2013-2021 Qualcomm Technologies, Inc. All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header: //components/rel/qmimsgs.tx/1.0/ipa/api/internet_protocol_accelerator_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Thu Apr 29 2021 (Spin 0)
   From IDL File: internet_protocol_accelerator_v01.idl */

/** @defgroup ipa_qmi_consts Constant values defined in the IDL */
/** @defgroup ipa_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup ipa_qmi_enums Enumerated types used in QMI messages */
/** @defgroup ipa_qmi_messages Structures sent as QMI messages */
/** @defgroup ipa_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup ipa_qmi_accessor Accessor for QMI service object */
/** @defgroup ipa_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"
#include "data_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ipa_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define IPA_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define IPA_V01_IDL_MINOR_VERS 0x1A
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define IPA_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define IPA_V01_MAX_MESSAGE_ID 0x0046
/**
    @}
  */


/** @addtogroup ipa_qmi_consts
    @{
  */
#define QMI_IPA_IPFLTR_NUM_IHL_RANGE_16_EQNS_V01 2
#define QMI_IPA_IPFLTR_NUM_MEQ_32_EQNS_V01 2
#define QMI_IPA_IPFLTR_NUM_IHL_MEQ_32_EQNS_V01 2
#define QMI_IPA_IPFLTR_NUM_MEQ_128_EQNS_V01 2
#define QMI_IPA_MAX_FILTERS_V01 64
#define QMI_IPA_MAX_FILTERS_EX_V01 128
#define QMI_IPA_MAX_FILTERS_EX2_V01 256
#define QMI_IPA_MAX_PIPES_V01 20
#define QMI_IPA_MAX_APN_V01 8
#define QMI_IPA_MAX_PER_CLIENTS_V01 64
#define QMI_IPA_MAX_CLIENT_DST_PIPES_V01 8
#define QMI_IPA_MAX_UL_FIREWALL_RULES_V01 64
#define QMI_IPA_REMOTE_MHI_CHANNELS_NUM_MAX_V01 6
#define QMI_IPA_REMOTE_MHI_MEMORY_MAPPING_NUM_MAX_V01 6
#define QMI_IPA_ENDP_DESC_NUM_MAX_V01 31
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Registers for indications from a service. */
typedef struct {

  /* Optional */
  /*  Master Driver Initialization Completion */
  uint8_t master_driver_init_complete_valid;  /**< Must be set to true if master_driver_init_complete is being passed */
  uint8_t master_driver_init_complete;
  /**<   If set to TRUE, this field indicates that the client wants to receive 
         indications about the completion of the master driver's initialization
         sequence.

         Setting this field in the request message makes sense only when the 
         QMI_IPA_INDICATION_REGISTER_REQ is being originated from the modem
         driver.
    */

  /* Optional */
  /*  Data Usage Quota Reached */
  uint8_t data_usage_quota_reached_valid;  /**< Must be set to true if data_usage_quota_reached is being passed */
  uint8_t data_usage_quota_reached;
  /**<   If set to TRUE, this field indicates that the client wants to receive 
         indications about reaching the data usage quota that was set
         via QMI_IPA_SET_DATA_USAGE_QUOTA.

         Setting this field in the request message makes sense only when the 
         QMI_IPA_INDICATION_REGISTER_REQ is being originated from the master
         driver.
    */

  /* Optional */
  /*  IPA MHI READY IND */
  uint8_t ipa_mhi_ready_ind_valid;  /**< Must be set to true if ipa_mhi_ready_ind is being passed */
  uint8_t ipa_mhi_ready_ind;
  /**<   If set to TRUE, this field indicates that the client wants to receive 
         indications about MHI ready for Channel allocations.

         Setting this field in the request message makes sense only when the 
         QMI_IPA_INDICATION_REGISTER_REQ is being originated from the master driver.
    */

  /* Optional */
  /*  Endpoint Desc Info Indication */
  uint8_t endpoint_desc_ind_valid;  /**< Must be set to true if endpoint_desc_ind is being passed */
  uint8_t endpoint_desc_ind;
  /**<   If set to TRUE, this field indicates that the client wants to receive 
         indications for Endpoint descriptor information via QMI_IPA_ENDP_DESC_INDICATION.

         Setting this field in the request message makes sense only when the 
         QMI_IPA_INDICATION_REGISTER_REQ is being originated from the master driver.
    */

  /* Optional */
  /*  BW Change Indication */
  uint8_t bw_change_ind_valid;  /**< Must be set to true if bw_change_ind is being passed */
  uint8_t bw_change_ind;
  /**<   If set to TRUE, this field indicates that the client wants to receive 
         indications for BW change notify information via QMI_IPA_BW_CHANGE_INDICATION.

         Setting this field in the request message makes sense only when the 
         QMI_IPA_INDICATION_REGISTER_REQ is being originated from the master driver.
    */
}ipa_indication_reg_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Registers for indications from a service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_indication_reg_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_PLATFORM_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_IPA_PLATFORM_TYPE_INVALID_V01 = 0, /**<  Invalid platform identifier
    */
  QMI_IPA_PLATFORM_TYPE_TN_V01 = 1, /**<  Data card device
   */
  QMI_IPA_PLATFORM_TYPE_LE_V01 = 2, /**<  Data router non MHI device
   */
  QMI_IPA_PLATFORM_TYPE_MSM_ANDROID_V01 = 3, /**<  MSM device with Android HLOS
   */
  QMI_IPA_PLATFORM_TYPE_MSM_WINDOWS_V01 = 4, /**<  MSM device with Windows HLOS
   */
  QMI_IPA_PLATFORM_TYPE_MSM_QNX_V01 = 5, /**<  MSM device with QNX HLOS
   */
  QMI_IPA_PLATFORM_TYPE_LE_MHI_V01 = 6, /**<  Data router device with MHI
   */
  IPA_PLATFORM_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_platform_type_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t modem_offset_start;
  /**<   Offset from the start of IPA shared memory from which the modem driver may 
       insert header table entries.
  */

  uint32_t modem_offset_end;
  /**<   Offset from the start of IPA shared memory beyond which the modem driver
       may not insert header table entries. The space available for the modem
       driver includes modem_offset_start and modem_offset_end.
  */
}ipa_hdr_tbl_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t modem_offset_start;
  /**<   Offset from the start of IPA shared memory from which the modem driver may
       insert header processing context table entries.
  */

  uint32_t modem_offset_end;
  /**<   Offset from the start of IPA shared memory beyond which the modem driver
       may not insert header processing table entries. The space available for the modem
       driver includes modem_offset_start and modem_offset_end.
  */
}ipa_hdr_proc_ctx_tbl_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t modem_offset_start;
  /**<   Offset from the start of IPA shared memory from which the modem driver may
       insert compression/decompression command entries.
  */

  uint32_t modem_offset_end;
  /**<   Offset from the start of IPA shared memory beyond which the modem driver
       may not insert compression/decompression command entries. The space available
       for the modem driver includes modem_offset_start and modem_offset_end.
  */
}ipa_zip_tbl_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t route_tbl_start_addr;
  /**<   Identifies the start of the routing table. Denotes the offset from the 
       start of the IPA shared memory.
  */

  uint32_t num_indices;
  /**<   Number of indices (starting from 0) that are allocated to the modem.
       The number indicated here is also included in the allocation. The value
       of num_indices must not exceed 31 (5 bits used to specify the routing
       table index), unless there is a change in the hardware.
  */
}ipa_route_tbl_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t block_start_addr;
  /**<   Identifies the start of the memory block allocated for the modem.
       Denotes the offset from the start of the IPA shared memory.
  */

  uint32_t size;
  /**<   Size of the block allocated in bytes for the modem driver.
  */
}ipa_modem_mem_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t hw_filter_stats_start_addr;
  /**<   Identifies the starting address of the HW Filter statistics table.
       Denotes the offset from the start of the IPA shared memory.
  */

  uint32_t hw_filter_stats_size;
  /**<   Identifies the size in bytes of the HW Filter statistics table.
  */

  uint8_t hw_filter_stats_start_index;
  /**<   Identifies the start index of the Modem driver managed indices
       in the HW Filter statistics table.
  */

  uint8_t hw_filter_stats_end_index;
  /**<   Identifies the end index of the Modem driver managed indices
       in the HW Filter statistics table.
  */
}ipa_filter_stats_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Requests the modem IPA driver to perform initialization. */
typedef struct {

  /* Optional */
  /*  Platform Information */
  uint8_t platform_type_valid;  /**< Must be set to true if platform_type is being passed */
  ipa_platform_type_enum_v01 platform_type;
  /**<   Provides information about the platform. Values: \n
      - QMI_IPA_PLATFORM_TYPE_INVALID (0) --  Invalid platform identifier
   
      - QMI_IPA_PLATFORM_TYPE_TN (1) --  Data card device
  
      - QMI_IPA_PLATFORM_TYPE_LE (2) --  Data router non MHI device
  
      - QMI_IPA_PLATFORM_TYPE_MSM_ANDROID (3) --  MSM device with Android HLOS
  
      - QMI_IPA_PLATFORM_TYPE_MSM_WINDOWS (4) --  MSM device with Windows HLOS
  
      - QMI_IPA_PLATFORM_TYPE_MSM_QNX (5) --  MSM device with QNX HLOS
  
      - QMI_IPA_PLATFORM_TYPE_LE_MHI (6) --  Data router device with MHI
  
 */

  /* Optional */
  /*  Header Table Information */
  uint8_t hdr_tbl_info_valid;  /**< Must be set to true if hdr_tbl_info is being passed */
  ipa_hdr_tbl_info_type_v01 hdr_tbl_info;

  /* Optional */
  /*  IPv4 Routing Table Information */
  uint8_t v4_route_tbl_info_valid;  /**< Must be set to true if v4_route_tbl_info is being passed */
  ipa_route_tbl_info_type_v01 v4_route_tbl_info;

  /* Optional */
  /*  IPv6 Routing Table Information */
  uint8_t v6_route_tbl_info_valid;  /**< Must be set to true if v6_route_tbl_info is being passed */
  ipa_route_tbl_info_type_v01 v6_route_tbl_info;

  /* Optional */
  /*  IPv4 Filter Table Start Address */
  uint8_t v4_filter_tbl_start_addr_valid;  /**< Must be set to true if v4_filter_tbl_start_addr is being passed */
  uint32_t v4_filter_tbl_start_addr;
  /**<   Identifies the starting address of the IPAv2.x or from QMI_IPA version 1.9 
       IPAv3.x non-hashable IPv4 filter table.
       Denotes the offset from the start of the IPA shared memory.
  */

  /* Optional */
  /*  IPv6 Filter Table Start Address */
  uint8_t v6_filter_tbl_start_addr_valid;  /**< Must be set to true if v6_filter_tbl_start_addr is being passed */
  uint32_t v6_filter_tbl_start_addr;
  /**<   Identifies the starting address of the IPAv2.x or from QMI_IPA version 1.9
       IPAv3.x non-hashable IPv6 filter table.
       Denotes the offset from the start of the IPA shared memory.
  */

  /* Optional */
  /*  Modem Memory Block */
  uint8_t modem_mem_info_valid;  /**< Must be set to true if modem_mem_info is being passed */
  ipa_modem_mem_info_type_v01 modem_mem_info;
  /**<   \n Provides information about the start address (physical address) and the
       size of the memory block that is allocated to the modem driver.
  */

  /* Optional */
  /*  Destination Endpoint for Control Commands from the Modem */
  uint8_t ctrl_comm_dest_end_pt_valid;  /**< Must be set to true if ctrl_comm_dest_end_pt is being passed */
  uint32_t ctrl_comm_dest_end_pt;
  /**<   Provides information about the destination endpoint on the application
       processor to which the modem driver can send control commands. \n
       The value of this parameter cannot exceed 19, since the IPA only supports 20
       endpoints.
  */

  /* Optional */
  /*  Modem Bootup Information */
  uint8_t is_ssr_bootup_valid;  /**< Must be set to true if is_ssr_bootup is being passed */
  uint8_t is_ssr_bootup;
  /**<   Specifies whether the modem is booting up after a modem only subsystem 
       restart. This lets the modem driver know that it is not required
       to re-initialize some of the hardware blocks because IPA has not been reset 
       since the previous initialization.
  */

  /* Optional */
  /*  Header Processing Context Table Information */
  uint8_t hdr_proc_ctx_tbl_info_valid;  /**< Must be set to true if hdr_proc_ctx_tbl_info is being passed */
  ipa_hdr_proc_ctx_tbl_info_type_v01 hdr_proc_ctx_tbl_info;
  /**<   \n Provides information about the header processing context table.
  */

  /* Optional */
  /*  Compression Decompression Table Information */
  uint8_t zip_tbl_info_valid;  /**< Must be set to true if zip_tbl_info is being passed */
  ipa_zip_tbl_info_type_v01 zip_tbl_info;
  /**<   \n Provides information about the zip table.
  */

  /* Optional */
  /*  IPv4 Hashable Routing Table Information */
  uint8_t v4_hash_route_tbl_info_valid;  /**< Must be set to true if v4_hash_route_tbl_info is being passed */
  ipa_route_tbl_info_type_v01 v4_hash_route_tbl_info;

  /* Optional */
  /*  IPv6 Hashable Routing Table Information */
  uint8_t v6_hash_route_tbl_info_valid;  /**< Must be set to true if v6_hash_route_tbl_info is being passed */
  ipa_route_tbl_info_type_v01 v6_hash_route_tbl_info;

  /* Optional */
  /*  IPv4 Hashable Filter Table Start Address */
  uint8_t v4_hash_filter_tbl_start_addr_valid;  /**< Must be set to true if v4_hash_filter_tbl_start_addr is being passed */
  uint32_t v4_hash_filter_tbl_start_addr;
  /**<   Identifies the starting address of the IPAv3.x IPv4 hashable filter table.
       Denotes the offset from the start of the IPA shared memory.
  */

  /* Optional */
  /*  IPv6 Hashable Filter Table Start Address */
  uint8_t v6_hash_filter_tbl_start_addr_valid;  /**< Must be set to true if v6_hash_filter_tbl_start_addr is being passed */
  uint32_t v6_hash_filter_tbl_start_addr;
  /**<   Identifies the starting address of the IPAv3.x IPv6 hashable filter table.
       Denotes the offset from the start of the IPA shared memory.
  */

  /* Optional */
  /*  Modem HW Stats Quota Base address */
  uint8_t hw_stats_quota_base_addr_valid;  /**< Must be set to true if hw_stats_quota_base_addr is being passed */
  uint32_t hw_stats_quota_base_addr;
  /**<     Identifies the starting address of the IPAv4.0 Modem Quota statistics table.
 	     Denotes the offset from the start of the IPA shared memory.
  */

  /* Optional */
  /*  Modem HW Stats Quota Size */
  uint8_t hw_stats_quota_size_valid;  /**< Must be set to true if hw_stats_quota_size is being passed */
  uint32_t hw_stats_quota_size;
  /**<     Identifies the size in bytes of the IPAv4.0 Modem Quota statistics table.
  */

  /* Optional */
  /*  Modem HW Drop Stats Table Start Address */
  uint8_t hw_drop_stats_base_addr_valid;  /**< Must be set to true if hw_drop_stats_base_addr is being passed */
  uint32_t hw_drop_stats_base_addr;
  /**<   Identifies the starting address of the IPAv4.0 Modem HW Drop statistics table.
       Denotes the offset from the start of the IPA shared memory.
  */

  /* Optional */
  /*  Modem HW Drop Stats Table size */
  uint8_t hw_drop_stats_table_size_valid;  /**< Must be set to true if hw_drop_stats_table_size is being passed */
  uint32_t hw_drop_stats_table_size;
  /**<     Identifies the size in bytes of the IPAv4.0 Modem HW Drop statistics table.
  */

  /* Optional */
  uint8_t filter_stats_info_valid;  /**< Must be set to true if filter_stats_info is being passed */
  ipa_filter_stats_info_type_v01 filter_stats_info;
  /**<   \n Provides information about the start address of the HW Filter Stats
       Table and the indices in this table that are allocated for the modem driver usage.
  */

  /* Optional */
  uint8_t smem_info_valid;  /**< Must be set to true if smem_info is being passed */
  ipa_modem_mem_info_type_v01 smem_info;
  /**<   \n Provides information about the start address (physical address) and the
       size of the shared DDR memory block that is allocated to the modem driver.
  */

  /* Optional */
  uint8_t per_stats_smem_info_valid;  /**< Must be set to true if per_stats_smem_info is being passed */
  ipa_modem_mem_info_type_v01 per_stats_smem_info;
  /**<   \n Provides information about the start address (physical address) and the
       size of the shared DDR memory block that is allocated to the modem driver for Peripheral stats.
  */
}ipa_init_modem_driver_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Requests the modem IPA driver to perform initialization. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Destination Endpoint for Control Commands from the Master Driver */
  uint8_t ctrl_comm_dest_end_pt_valid;  /**< Must be set to true if ctrl_comm_dest_end_pt is being passed */
  uint32_t ctrl_comm_dest_end_pt;
  /**<   Provides information about the destination endpoint on the modem
       processor to which the master driver can send control commands.

       The value of this parameter cannot exceed 19, since IPA only supports 20
       endpoints.

       The server does not include this field if the response indicates a failure.
  */

  /* Optional */
  /*  Default Endpoint */
  uint8_t default_end_pt_valid;  /**< Must be set to true if default_end_pt is being passed */
  uint32_t default_end_pt;
  /**<   Provides information about the default endpoint. The master driver may 
       or may not set the register in the hardware with this value.

       The value of this parameter cannot exceed 19, since the IPA only supports 20
       endpoints.

       The server does not include this field if the response indicates a failure.
  */

  /* Optional */
  /*  Modem Driver Initialization Pending */
  uint8_t modem_driver_init_pending_valid;  /**< Must be set to true if modem_driver_init_pending is being passed */
  uint8_t modem_driver_init_pending;
  /**<   Identifies if a second-level message handshake is needed between drivers to
       indicate when IPA HWP loading is completed. If this is set by the modem driver,
       the AP driver must wait for a INIT_MODEM_DRIVER_CMPLT message before 
       communicating with IPA HWP.
  */
}ipa_init_modem_driver_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Request from the modem IPA driver to indicate the completion of the
    modem driver initialization. */
typedef struct {

  /* Mandatory */
  /*  Modem Driver Init Complete Status */
  uint8_t status;
  /**<   Specifies whether the modem driver initialization is complete, including 
      the micro controller image loading.
  */
}ipa_init_modem_driver_cmplt_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Request from the modem IPA driver to indicate the completion of the
    modem driver initialization. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_init_modem_driver_cmplt_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Indication Message; Indication sent to the modem IPA driver from the master IPA driver
           regarding the completion of the initialization. */
typedef struct {

  /* Mandatory */
  /*  Master Driver Initialization Completion Status */
  qmi_response_type_v01 master_driver_init_status;
  /**<   Indicates the status of the initialization. If everything went as expected,
       this field is set to SUCCESS. ERROR is set otherwise.
  */
}ipa_master_driver_init_complt_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t offset;
  /**<   Offset from the Internet Header Length (IHL).
  */

  uint16_t range_low;
  /**<   Lower bound of the range.
  */

  uint16_t range_high;
  /**<   Upper bound of the range.
  */
}ipa_ipfltr_range_eq_16_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t offset;
  /**<   Offset either from the IHL or from the start of the IP packet.
       This depends on the equation in which this structure is used.
  */

  uint32_t mask;
  /**<   Mask that must be used in the comparison. The field is
       ANDed with the mask and compared against the value.
  */

  uint32_t value;
  /**<   32-bit value that is to be used in the comparison.
  */
}ipa_ipfltr_mask_eq_32_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t offset;
  /**<   Offset in the packet.
  */

  uint16_t value;
  /**<   16-bit value that is to be used in the comparison.
  */
}ipa_ipfltr_eq_16_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t offset;
  /**<   Offset in the packet.
  */

  uint32_t value;
  /**<   32-bit value that is to be used in the comparison.
  */
}ipa_ipfltr_eq_32_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t offset;
  /**<   Offset in the packet.
  */

  uint8_t mask[16];
  /**<   Mask that must be used in the comparison. The field is
       ANDed with the mask and compared against the value.
  */

  uint8_t value[16];
  /**<   128-bit value that is to be used in the comparison.
  */
}ipa_ipfltr_mask_eq_128_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t rule_eq_bitmap;
  /**<   16-bit bitmask to indicate how many equations are valid in this rule.
         Equations are the building blocks of a filter rule. The receiver uses
         this value to check if it has constructed the rule correctly out of the
         TLVs in the message. This is a way to build in redundancy.
    */

  uint8_t tos_eq_present;
  /**<   Specifies whether a type of service check rule is present.
    */

  uint8_t tos_eq;
  /**<   Value to check against the type of service (IPv4) field.
    */

  uint8_t protocol_eq_present;
  /**<   Specifies whether a protocol check rule is present.
    */

  uint8_t protocol_eq;
  /**<   Value to check against the protocol field.
    */

  uint8_t num_ihl_offset_range_16;
  /**<   Number of 16-bit range check rules at the location determined by
         the IP header length plus a specified offset in this rule.
         See the definition of the ipa_ipfltr_range_eq_16_type for better 
         understanding.

         The value of this field cannot exceed QMI_IPA_IPFLTR_NUM_IHL_RANGE_16_EQNS,
         which is set as 2.
    */

  ipa_ipfltr_range_eq_16_type_v01 ihl_offset_range_16[QMI_IPA_IPFLTR_NUM_IHL_RANGE_16_EQNS_V01];
  /**<   Array of the registered IP header length offset 16-bit range check 
         rules. 
    */

  uint8_t num_offset_meq_32;
  /**<   Number of 32-bit masked comparison rules present in this rule.
    */

  ipa_ipfltr_mask_eq_32_type_v01 offset_meq_32[QMI_IPA_IPFLTR_NUM_MEQ_32_EQNS_V01];
  /**<   Array of all the possible 32-bit masked comparison rules in this rule.
    */

  uint8_t tc_eq_present;
  /**<   Specifies whether the traffic class rule is present in this rule.
    */

  uint8_t tc_eq;
  /**<   Value against which the IPv4 traffic class field must be checked.
    */

  uint8_t flow_eq_present;
  /**<   Specifies if the "flow equals" rule is present in this rule.
    */

  uint32_t flow_eq;
  /**<   Value against which the IPv6 flow field must be checked.
    */

  uint8_t ihl_offset_eq_16_present;
  /**<   Specifies whether a 16-bit comparison is required at the location in
         the packet determined by (IHL + specified offset).
    */

  ipa_ipfltr_eq_16_type_v01 ihl_offset_eq_16;
  /**<   16-bit comparison equation.
    */

  uint8_t ihl_offset_eq_32_present;
  /**<   Specifies whether a 32-bit comparison is required at the location in
         the packet determined by (IHL + specified offset).
    */

  ipa_ipfltr_eq_32_type_v01 ihl_offset_eq_32;
  /**<   32-bit comparison equation.
    */

  uint8_t num_ihl_offset_meq_32;
  /**<   Number of 32-bit masked comparison equations in this rule. The
         location of the packet to be compared is determined by (IHL +
         the specified offset).
    */

  ipa_ipfltr_mask_eq_32_type_v01 ihl_offset_meq_32[QMI_IPA_IPFLTR_NUM_IHL_MEQ_32_EQNS_V01];
  /**<   Array of 32-bit masked comparison equations.
    */

  uint8_t num_offset_meq_128;
  /**<   Number of 128-bit comparison equations in this rule. 
    */

  ipa_ipfltr_mask_eq_128_type_v01 offset_meq_128[QMI_IPA_IPFLTR_NUM_MEQ_128_EQNS_V01];
  /**<   Array of 128-bit comparison equations. The location in the packet is
         determined by the specified offset.
    */

  uint8_t metadata_meq32_present;
  /**<   Indicates whether the 32-bit masked comparison equation
         is present. The comparison is done against the metadata in the IPA.
         Metadata can either be extracted from the packet header or from the
         metadata register.
    */

  ipa_ipfltr_mask_eq_32_type_v01 metadata_meq32;
  /**<   Metadata 32-bit masked comparison equation.
    */

  uint8_t ipv4_frag_eq_present;
  /**<   Specifies whether the IPv4 fragment equation is present in this rule.
    */
}ipa_filter_rule_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_IP_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_IPA_IP_TYPE_INVALID_V01 = 0, /**<  Invalid IP type identifier \n
   */
  QMI_IPA_IP_TYPE_V4_V01 = 1, /**<  IPv4 type \n
   */
  QMI_IPA_IP_TYPE_V6_V01 = 2, /**<  IPv6 type \n
   */
  QMI_IPA_IP_TYPE_V4V6_V01 = 3, /**<  Applies to both IP types
   */
  IPA_IP_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_ip_type_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_FILTER_ACTION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_IPA_FILTER_ACTION_INVALID_V01 = 0, /**<  Invalid action on a filter hit \n
   */
  QMI_IPA_FILTER_ACTION_SRC_NAT_V01 = 1, /**<  Pass a packet to the NAT block for the source NAT \n
   */
  QMI_IPA_FILTER_ACTION_DST_NAT_V01 = 2, /**<  Pass a packet to the NAT block for the destination NAT \n
   */
  QMI_IPA_FILTER_ACTION_ROUTING_V01 = 3, /**<  Pass a packet to the routing block \n
   */
  QMI_IPA_FILTER_ACTION_EXCEPTION_V01 = 4, /**<  Treat a packet as an exception and send it to the exception pipe
   */
  IPA_FILTER_ACTION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_filter_action_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t filter_spec_identifier;
  /**<   Filter specification in the list of filter 
         specifications sent from the client. This field is applicable only in
         the filter installation request and response.
    */

  ipa_ip_type_enum_v01 ip_type;
  /**<   IP type for which this rule is applicable.
 The driver must identify the filter table (v6 or v4), and this
 field is essential for that. Values: \n
      - QMI_IPA_IP_TYPE_INVALID (0) --  Invalid IP type identifier \n
  
      - QMI_IPA_IP_TYPE_V4 (1) --  IPv4 type \n
  
      - QMI_IPA_IP_TYPE_V6 (2) --  IPv6 type \n
  
      - QMI_IPA_IP_TYPE_V4V6 (3) --  Applies to both IP types
  
 */

  ipa_filter_rule_type_v01 filter_rule;
  /**<   Rules in the filter specification. These rules are the
         ones that are matched against fields in the packet.
    */

  ipa_filter_action_enum_v01 filter_action;
  /**<   Action to be taken when a filter match occurs.
 The remote side must install this information into the hardware along
 with the filter equations. Values:
      - QMI_IPA_FILTER_ACTION_INVALID (0) --  Invalid action on a filter hit \n
  
      - QMI_IPA_FILTER_ACTION_SRC_NAT (1) --  Pass a packet to the NAT block for the source NAT \n
  
      - QMI_IPA_FILTER_ACTION_DST_NAT (2) --  Pass a packet to the NAT block for the destination NAT \n
  
      - QMI_IPA_FILTER_ACTION_ROUTING (3) --  Pass a packet to the routing block \n
  
      - QMI_IPA_FILTER_ACTION_EXCEPTION (4) --  Treat a packet as an exception and send it to the exception pipe
  
 */

  uint8_t is_routing_table_index_valid;
  /**<   Specifies whether the routing table index is present. If the
         action is QMI_IPA_FILTER_ACTION_EXCEPTION, this parameter need not be
         provided.
    */

  uint32_t route_table_index;
  /**<   Routing table index used to route the packets if the filter rule is hit.
    */

  uint8_t is_mux_id_valid;
  /**<   Specifies whether the multiplexer ID is valid.
    */

  uint32_t mux_id;
  /**<   QMAP Mux ID. As a part of the QMAP protocol,
         several data calls may be multiplexed over the same physical transport
         channel. This identifier is used to identify one such data call.
         The maximum value for this identifier is 255.
    */
}ipa_filter_spec_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  ipa_ip_type_enum_v01 ip_type;
  /**<   IP type for which this rule is applicable.
 The driver must identify the filter table (v6 or v4), and this
 field is essential for that. Values: \n
      - QMI_IPA_IP_TYPE_INVALID (0) --  Invalid IP type identifier \n
  
      - QMI_IPA_IP_TYPE_V4 (1) --  IPv4 type \n
  
      - QMI_IPA_IP_TYPE_V6 (2) --  IPv6 type \n
  
      - QMI_IPA_IP_TYPE_V4V6 (3) --  Applies to both IP types
  
 */

  ipa_filter_rule_type_v01 filter_rule;
  /**<   Rules in the filter specification. These rules are the
         ones that are matched against fields in the packet.
    */

  ipa_filter_action_enum_v01 filter_action;
  /**<   Action to be taken when a filter match occurs.
 The remote side must install this information into the hardware along
 with the filter equations. Values:
      - QMI_IPA_FILTER_ACTION_INVALID (0) --  Invalid action on a filter hit \n
  
      - QMI_IPA_FILTER_ACTION_SRC_NAT (1) --  Pass a packet to the NAT block for the source NAT \n
  
      - QMI_IPA_FILTER_ACTION_DST_NAT (2) --  Pass a packet to the NAT block for the destination NAT \n
  
      - QMI_IPA_FILTER_ACTION_ROUTING (3) --  Pass a packet to the routing block \n
  
      - QMI_IPA_FILTER_ACTION_EXCEPTION (4) --  Treat a packet as an exception and send it to the exception pipe
  
 */

  uint8_t is_routing_table_index_valid;
  /**<   Specifies whether the routing table index is present. If the
         action is QMI_IPA_FILTER_ACTION_EXCEPTION, this parameter need not be
         provided.
    */

  uint32_t route_table_index;
  /**<   Routing table index used to route the packets if the filter rule is hit.
    */

  uint8_t is_mux_id_valid;
  /**<   Specifies whether the multiplexer ID is valid.
    */

  uint32_t mux_id;
  /**<   QMAP Mux ID. As a part of the QMAP protocol,
         several data calls may be multiplexed over the same physical transport
         channel. This identifier is used to identify one such data call.
         The maximum value for this identifier is 255.
    */

  uint32_t rule_id;
  /**<   Rule ID of the given filter. The Rule ID is populated in the rule 
         header when installing the rule in IPA.
    */

  uint8_t is_rule_hashable;
  /**<   Specifies whether the given rule is hashable.
    */
}ipa_filter_spec_ex_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t rule_eq_bitmap;
  /**<   16-bit bitmask to indicate how many equations are valid in this rule.
         Equations are the building blocks of a filter rule. The receiver uses
         this value to check if it has constructed the rule correctly out of the
         TLVs in the message. This is a way to build in redundancy.
    */

  uint8_t pure_ack_eq_present;
  /**<   Specifies whether a pure ack rule is present.
    */

  uint8_t pure_ack_eq;
  /**<   Value to check for pure ack rule.
    */

  uint8_t protocol_eq_present;
  /**<   Specifies whether a protocol check rule is present.
    */

  uint8_t protocol_eq;
  /**<   Value to check against the protocol field.
    */

  uint8_t num_ihl_offset_range_16;
  /**<   Number of 16-bit range check rules at the location determined by
         the IP header length plus a specified offset in this rule.
         See the definition of the ipa_ipfltr_range_eq_16_type for better 
         understanding.

         The value of this field cannot exceed QMI_IPA_IPFLTR_NUM_IHL_RANGE_16_EQNS,
         which is set as 2.
    */

  ipa_ipfltr_range_eq_16_type_v01 ihl_offset_range_16[QMI_IPA_IPFLTR_NUM_IHL_RANGE_16_EQNS_V01];
  /**<   Array of the registered IP header length offset 16-bit range check 
         rules. 
    */

  uint8_t num_offset_meq_32;
  /**<   Number of 32-bit masked comparison rules present in this rule.
    */

  ipa_ipfltr_mask_eq_32_type_v01 offset_meq_32[QMI_IPA_IPFLTR_NUM_MEQ_32_EQNS_V01];
  /**<   Array of all the possible 32-bit masked comparison rules in this rule.
    */

  uint8_t tc_eq_present;
  /**<   Specifies whether the traffic class rule is present in this rule.
    */

  uint8_t tc_eq;
  /**<   Value against which the IPv4 traffic class field must be checked.
    */

  uint8_t flow_eq_present;
  /**<   Specifies if the "flow equals" rule is present in this rule.
    */

  uint32_t flow_eq;
  /**<   Value against which the IPv6 flow field must be checked.
    */

  uint8_t ihl_offset_eq_16_present;
  /**<   Specifies whether a 16-bit comparison is required at the location in
         the packet determined by (IHL + specified offset).
    */

  ipa_ipfltr_eq_16_type_v01 ihl_offset_eq_16;
  /**<   16-bit comparison equation.
    */

  uint8_t ihl_offset_eq_32_present;
  /**<   Specifies whether a 32-bit comparison is required at the location in
         the packet determined by (IHL + specified offset).
    */

  ipa_ipfltr_eq_32_type_v01 ihl_offset_eq_32;
  /**<   32-bit comparison equation.
    */

  uint8_t num_ihl_offset_meq_32;
  /**<   Number of 32-bit masked comparison equations in this rule. The
         location of the packet to be compared is determined by (IHL +
         the specified offset).
    */

  ipa_ipfltr_mask_eq_32_type_v01 ihl_offset_meq_32[QMI_IPA_IPFLTR_NUM_IHL_MEQ_32_EQNS_V01];
  /**<   Array of 32-bit masked comparison equations.
    */

  uint8_t num_offset_meq_128;
  /**<   Number of 128-bit comparison equations in this rule. 
    */

  ipa_ipfltr_mask_eq_128_type_v01 offset_meq_128[QMI_IPA_IPFLTR_NUM_MEQ_128_EQNS_V01];
  /**<   Array of 128-bit comparison equations. The location in the packet is
         determined by the specified offset.
    */

  uint8_t metadata_meq32_present;
  /**<   Indicates whether the 32-bit masked comparison equation
         is present. The comparison is done against the metadata in the IPA.
         Metadata can either be extracted from the packet header or from the
         metadata register.
    */

  ipa_ipfltr_mask_eq_32_type_v01 metadata_meq32;
  /**<   Metadata 32-bit masked comparison equation.
    */

  uint8_t ipv4_frag_eq_present;
  /**<   Specifies whether the IPv4 fragment equation is present in this rule.
    */
}ipa_filter_rule_req2_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  ipa_ip_type_enum_v01 ip_type;
  /**<   IP type for which this rule is applicable.
 The driver must identify the filter table (v6 or v4), and this
 field is essential for that. Values: \n
      - QMI_IPA_IP_TYPE_INVALID (0) --  Invalid IP type identifier \n
  
      - QMI_IPA_IP_TYPE_V4 (1) --  IPv4 type \n
  
      - QMI_IPA_IP_TYPE_V6 (2) --  IPv6 type \n
  
      - QMI_IPA_IP_TYPE_V4V6 (3) --  Applies to both IP types
  
 */

  ipa_filter_rule_req2_type_v01 filter_rule;
  /**<   Rules in the filter specification. These rules are the
         ones that are matched against fields in the packet.
    */

  ipa_filter_action_enum_v01 filter_action;
  /**<   Action to be taken when a filter match occurs.
 The remote side must install this information into the hardware along
 with the filter equations. Values:
      - QMI_IPA_FILTER_ACTION_INVALID (0) --  Invalid action on a filter hit \n
  
      - QMI_IPA_FILTER_ACTION_SRC_NAT (1) --  Pass a packet to the NAT block for the source NAT \n
  
      - QMI_IPA_FILTER_ACTION_DST_NAT (2) --  Pass a packet to the NAT block for the destination NAT \n
  
      - QMI_IPA_FILTER_ACTION_ROUTING (3) --  Pass a packet to the routing block \n
  
      - QMI_IPA_FILTER_ACTION_EXCEPTION (4) --  Treat a packet as an exception and send it to the exception pipe
  
 */

  uint8_t is_routing_table_index_valid;
  /**<   Specifies whether the routing table index is present. If the
         action is QMI_IPA_FILTER_ACTION_EXCEPTION, this parameter need not be
         provided.
    */

  uint32_t route_table_index;
  /**<   Routing table index used to route the packets if the filter rule is hit.
    */

  uint8_t is_mux_id_valid;
  /**<   Specifies whether the multiplexer ID is valid.
    */

  uint32_t mux_id;
  /**<   QMAP Mux ID. As a part of the QMAP protocol,
         several data calls may be multiplexed over the same physical transport
         channel. This identifier is used to identify one such data call.
         The maximum value for this identifier is 255.
    */

  uint32_t rule_id;
  /**<   Rule ID of the given filter. The Rule ID is populated in the rule 
         header when installing the rule in IPA.
    */

  uint8_t is_rule_hashable;
  /**<   Specifies whether the given rule is hashable.
    */
}ipa_filter_spec_ex2_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Requests installation of filtering rules in the hardware block on the remote side. */
typedef struct {

  /* Optional */
  /*  Filter Specification */
  uint8_t filter_spec_list_valid;  /**< Must be set to true if filter_spec_list is being passed */
  uint32_t filter_spec_list_len;  /**< Must be set to # of elements in filter_spec_list */
  ipa_filter_spec_type_v01 filter_spec_list[QMI_IPA_MAX_FILTERS_V01];
  /**<   \vspace{4pt} \n List of filter specifications of filters that must be installed in
         the IPAv2.x hardware. The Filter Specification TLV only applies for IPAv2.x 
         versions and is replaced in QMI IPA version 1.9 and onward in favor of the 
         Extended Filter Specification TLV, which is used for IPAv3.x and onward.

         The driver installing these rules must do so in the same order as
         specified in this list.
    */

  /* Optional */
  /*  Pipe Index to Install Rule */
  uint8_t source_pipe_index_valid;  /**< Must be set to true if source_pipe_index is being passed */
  uint32_t source_pipe_index;
  /**<   Pipe index to install the filter rule.
         The requester may not always know the pipe indices. If not specified,
         the receiver must install this rule on all pipes that it controls,
         through which data may be fed into the IPA.
    */

  /* Optional */
  /*  Total Number of IPv4 Filters in the Filter Spec List */
  uint8_t num_ipv4_filters_valid;  /**< Must be set to true if num_ipv4_filters is being passed */
  uint32_t num_ipv4_filters;
  /**<   Number of IPv4 rules included in the filter specification list.
    */

  /* Optional */
  /*  Total Number of IPv6 Filters in the Filter Spec List */
  uint8_t num_ipv6_filters_valid;  /**< Must be set to true if num_ipv6_filters is being passed */
  uint32_t num_ipv6_filters;
  /**<   Number of IPv6 rules included in the filter specification list.
    */

  /* Optional */
  /*  List of XLAT Filter Indices in the Filter Spec List */
  uint8_t xlat_filter_indices_list_valid;  /**< Must be set to true if xlat_filter_indices_list is being passed */
  uint32_t xlat_filter_indices_list_len;  /**< Must be set to # of elements in xlat_filter_indices_list */
  uint32_t xlat_filter_indices_list[QMI_IPA_MAX_FILTERS_V01];
  /**<   List of XLAT filter indices.
         Filter rules at specified indices must be modified by the 
         receiver if the PDN is XLAT before installing them on the associated
         IPA consumer pipe.
    */

  /* Optional */
  /*  Extended Filter Specification  */
  uint8_t filter_spec_ex_list_valid;  /**< Must be set to true if filter_spec_ex_list is being passed */
  uint32_t filter_spec_ex_list_len;  /**< Must be set to # of elements in filter_spec_ex_list */
  ipa_filter_spec_ex_type_v01 filter_spec_ex_list[QMI_IPA_MAX_FILTERS_V01];
  /**<   \n List of filter specifications of filters that must be installed in
         the IPAv3.x hardware.

         The driver installing these rules must do so in the same order as
         specified in this list.
    */

  /* Optional */
  /*  Extended Type 2 Filter Specification  */
  uint8_t filter_spec_ex2_list_valid;  /**< Must be set to true if filter_spec_ex2_list is being passed */
  uint32_t filter_spec_ex2_list_len;  /**< Must be set to # of elements in filter_spec_ex2_list */
  ipa_filter_spec_ex2_type_v01 filter_spec_ex2_list[QMI_IPA_MAX_FILTERS_V01];
  /**<   \n List of filter specifications of filters that must be installed in
         the IPAv4.x hardware.

         The driver installing these rules must do so in the same order as
         specified in this list.
    */

  /* Optional */
  /*  List of modem UL Filters in the Spec List which need be to replicated
 with AP UL firewall filters */
  uint8_t ul_firewall_indices_list_valid;  /**< Must be set to true if ul_firewall_indices_list is being passed */
  uint32_t ul_firewall_indices_list_len;  /**< Must be set to # of elements in ul_firewall_indices_list */
  uint32_t ul_firewall_indices_list[QMI_IPA_MAX_FILTERS_V01];
  /**<   List of UL firewall filter indices.
         Filter rules at specified indices must be replicated across the firewall
         filters by the receiver and installed on the associated IPA consumer pipe.
    */
}ipa_install_fltr_rule_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t filter_spec_identifier;
  /**<   Specification in the list of filter 
         specifications sent from the client. This field is applicable only in
         the filter installation request and response.
    */

  uint32_t filter_handle;
  /**<   Identifies a rule in any subsequent message. This
         is a value that is provided by the server to the control point.
    */
}ipa_filter_rule_identifier_to_handle_map_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Requests installation of filtering rules in the hardware block on the remote side. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */

  /* Optional */
  /*  Filter Handle List */
  uint8_t filter_handle_list_valid;  /**< Must be set to true if filter_handle_list is being passed */
  uint32_t filter_handle_list_len;  /**< Must be set to # of elements in filter_handle_list */
  ipa_filter_rule_identifier_to_handle_map_v01 filter_handle_list[QMI_IPA_MAX_FILTERS_V01];
  /**<   \n List of handles returned to the control point. Each handle is
         mapped to the rule identifier that was specified in the request message.
         Any further reference to the rule is done using the filter handle.
    */

  /* Optional */
  /*  Rule ID List */
  uint8_t rule_id_valid;  /**< Must be set to true if rule_id is being passed */
  uint32_t rule_id_len;  /**< Must be set to # of elements in rule_id */
  uint32_t rule_id[QMI_IPA_MAX_FILTERS_V01];
  /**<   List of rule IDs returned to the control point. 
         Any further reference to the rule is done using the filter rule ID 
         specified in this list.
    */
}ipa_install_fltr_rule_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t filter_handle;
  /**<   Handle to the remote client that requested the rule addition.
    */

  uint32_t filter_index;
  /**<   Denotes the location in a filter table where the filter 
         rule has been installed.
         The maximum value of this field is 64.
    */
}ipa_filter_handle_to_index_map_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Notifies the remote driver of the installation of the filter rule
           supplied earlier by the remote driver. */
typedef struct {

  /* Mandatory */
  /*  Pipe Index */
  uint32_t source_pipe_index;
  /**<   Source pipe on which the filter rule has been installed
         or was attempted to be installed.
    */

  /* Mandatory */
  /*  Installation Status */
  qmi_result_type_v01 install_status;
  /**<   Installation status. If this indicates SUCCESS, other
         optional fields carry additional information. The driver installing
         the rules never installs a subset of the rules. The install status
         indicates the status of the installation of all the rules.
    */

  /* Mandatory */
  /*  List of Filter Indices */
  uint32_t filter_index_list_len;  /**< Must be set to # of elements in filter_index_list */
  ipa_filter_handle_to_index_map_v01 filter_index_list[QMI_IPA_MAX_FILTERS_V01];
  /**<   \n Provides the list of filter indices and the corresponding 
         filter handle. If the installation_status indicates a failure, the 
         filter indices must be set to a reserve index (255).
    */

  /* Optional */
  /*  Embedded Pipe Index */
  uint8_t embedded_pipe_index_valid;  /**< Must be set to true if embedded_pipe_index is being passed */
  uint32_t embedded_pipe_index;
  /**<   Denotes the embedded pipe number on which a call to the same Public
         Data Network (PDN) has been made. If this field is set, it denotes
         that this is a use case where PDN sharing is occuring. The embedded
         pipe is used to send data from the embedded client in the device.
    */

  /* Optional */
  /*  Retain Header Configuration */
  uint8_t retain_header_valid;  /**< Must be set to true if retain_header is being passed */
  uint8_t retain_header;
  /**<   Indicates whether the driver installing the rule has turned on 
         the retain_header bit. If this is TRUE, the header that is removed
         by the IPA is reinserted after the packet processing is completed.
    */

  /* Optional */
  /*  Embedded Call Mux ID */
  uint8_t embedded_call_mux_id_valid;  /**< Must be set to true if embedded_call_mux_id is being passed */
  uint32_t embedded_call_mux_id;
  /**<   Identifies one of the many calls that have been originated on the
         embedded pipe. This is how the PDN gateway to which traffic
         from the source pipe must flow is identified.
    */

  /* Optional */
  /*  Total Number of IPv4 Filters in the Filter Index List */
  uint8_t num_ipv4_filters_valid;  /**< Must be set to true if num_ipv4_filters is being passed */
  uint32_t num_ipv4_filters;
  /**<   Number of IPv4 rules included in the filter index list.
    */

  /* Optional */
  /*  Total Number of IPv6 Filters in the Filter Index List */
  uint8_t num_ipv6_filters_valid;  /**< Must be set to true if num_ipv6_filters is being passed */
  uint32_t num_ipv6_filters;
  /**<   Number of IPv6 rules included in the filter index list.
    */

  /* Optional */
  /*  Start Index on IPv4 Filters Installed on Source Pipe */
  uint8_t start_ipv4_filter_idx_valid;  /**< Must be set to true if start_ipv4_filter_idx is being passed */
  uint32_t start_ipv4_filter_idx;
  /**<   Start index of IPv4 rules in the filter index list.
    */

  /* Optional */
  /*  Start Index on IPv6 Filters Installed on Source Pipe */
  uint8_t start_ipv6_filter_idx_valid;  /**< Must be set to true if start_ipv6_filter_idx is being passed */
  uint32_t start_ipv6_filter_idx;
  /**<   Start index of IPv6 rules in the filter index list.
    */

  /* Optional */
  /*  List of Rule IDs */
  uint8_t rule_id_valid;  /**< Must be set to true if rule_id is being passed */
  uint32_t rule_id_len;  /**< Must be set to # of elements in rule_id */
  uint32_t rule_id[QMI_IPA_MAX_FILTERS_V01];
  /**<   \n Provides the list of Rule IDs of rules added in IPA on the given 
         source pipe index. If the install_status TLV indicates a failure,
         the Rule IDs in this list must be set to a reserved index (255).
    */

  /* Optional */
  /*  List of Destination pipe IDs for a given Source pipe */
  uint8_t dst_pipe_id_valid;  /**< Must be set to true if dst_pipe_id is being passed */
  uint32_t dst_pipe_id_len;  /**< Must be set to # of elements in dst_pipe_id */
  uint32_t dst_pipe_id[QMI_IPA_MAX_CLIENT_DST_PIPES_V01];
  /**<   \n Provides the list of destination pipe IDs on which data can be sent
            for a given source pipe.
    */

  /* Optional */
  /*  List of Rule IDs extended */
  uint8_t rule_id_ex_valid;  /**< Must be set to true if rule_id_ex is being passed */
  uint32_t rule_id_ex_len;  /**< Must be set to # of elements in rule_id_ex */
  uint32_t rule_id_ex[QMI_IPA_MAX_FILTERS_EX2_V01];
  /**<   \n Provides the list of Rule IDs of rules added in IPA on the given 
         source pipe index. If the install_status TLV indicates a failure,
         the Rule IDs in this list must be set to a reserved index (255).
    */
}ipa_fltr_installed_notif_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Notifies the remote driver of the installation of the filter rule
           supplied earlier by the remote driver. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_fltr_installed_notif_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Notifies the remote driver of the need to clear the data path to
           prevent the IPA from being blocked at the head of the processing
           pipeline. */
typedef struct {

  /* Mandatory */
  /*  Pipe Mask */
  uint32_t source_pipe_bitmask;
  /**<   Set of consumer (source) pipes that must be clear of
         active data transfers.Each SET bit in the bitmask refers to selected endpoint.
         Refer to IPA HW architecture documents for endpoint definitions.
    */

  /* Mandatory */
  /*  Request ID */
  uint32_t request_id;
  /**<   Identifies the ID of the request that is sent to
         the server. The same request ID is used in the message to remove
         the force_clear request. The server is expected to keep track of the
         request ID and the source_pipe_bitmask / source_pipe_bitmask_ext so that it can revert as needed.
    */

  /* Optional */
  /*  Source Throttle State */
  uint8_t throttle_source_valid;  /**< Must be set to true if throttle_source is being passed */
  uint8_t throttle_source;
  /**<   Specifies whether the server is to throttle the data from
         these consumer (source) pipes after clearing the existing 
         data present in the IPA that were pulled from these pipes.
         The server is expected to put all the source pipes in the 
         source_pipe_bitmask / source_pipe_bitmask_ext in the same state.
    */

  /* Optional */
  /*  Pipe Mask Ext */
  uint8_t source_pipe_bitmask_ext_valid;  /**< Must be set to true if source_pipe_bitmask_ext is being passed */
  uint32_t source_pipe_bitmask_ext[4];
  /**<   Set of consumer (source) pipes that must be clear of
         active data transfers. To support endpoints more than 32. A maximum of
         128 endpoints can be supported.Each SET bit in the bitmask refers to selected endpoint.
         Refer to IPA HW architecture documents for endpoint definitions.
         If this new "source_pipe_bitmask_ext" argument is used, then old mandatory 'source_pipe_bitmask' would be ignored.
    */
}ipa_enable_force_clear_datapath_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Notifies the remote driver of the need to clear the data path to
           prevent the IPA from being blocked at the head of the processing
           pipeline. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_enable_force_clear_datapath_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Notifies the remote driver that the forceful clearing of the data
           path can be lifted. */
typedef struct {

  /* Mandatory */
  /*  Request ID */
  uint32_t request_id;
  /**<   Identifies the request that was sent to the server to
         forcibly clear the data path. This request simply undoes the operation
         done in that request.
    */
}ipa_disable_force_clear_datapath_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Notifies the remote driver that the forceful clearing of the data
           path can be lifted. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_disable_force_clear_datapath_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_PERIPHERAL_SPEED_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_IPA_PER_USB_FS_V01 = 1, /**<  Full-speed USB connection
   */
  QMI_IPA_PER_USB_HS_V01 = 2, /**<  High-speed USB connection
   */
  QMI_IPA_PER_USB_SS_V01 = 3, /**<  Super-speed USB connection
   */
  IPA_PERIPHERAL_SPEED_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_peripheral_speed_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_PIPE_MODE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_IPA_PIPE_MODE_HW_V01 = 1, /**<  Pipe is connected with a hardware block
   */
  QMI_IPA_PIPE_MODE_SW_V01 = 2, /**<  Pipe is controlled by the software
   */
  IPA_PIPE_MODE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_pipe_mode_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_PERIPHERAL_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_IPA_PERIPHERAL_USB_V01 = 1, /**<  Specifies a USB peripheral
   */
  QMI_IPA_PERIPHERAL_HSIC_V01 = 2, /**<  Specifies an HSIC peripheral
   */
  QMI_IPA_PERIPHERAL_PCIE_V01 = 3, /**<  Specifies a PCIe	peripheral
   */
  QMI_IPA_PERIPHERAL_WLAN_V01 = 4, /**<  WLAN connection
   */
  IPA_PERIPHERAL_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_peripheral_type_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Notifies the remote driver of the configuration information. */
typedef struct {

  /* Optional */
  /*  Peripheral Type */
  uint8_t peripheral_type_valid;  /**< Must be set to true if peripheral_type is being passed */
  ipa_peripheral_type_enum_v01 peripheral_type;
  /**<   Informs the remote driver about the perhipheral for
 which this configuration information is relevant. Values: \n
      - QMI_IPA_PERIPHERAL_USB (1) --  Specifies a USB peripheral
  
      - QMI_IPA_PERIPHERAL_HSIC (2) --  Specifies an HSIC peripheral
  
      - QMI_IPA_PERIPHERAL_PCIE (3) --  Specifies a PCIe	peripheral
  
      - QMI_IPA_PERIPHERAL_WLAN (4) --  WLAN connection
  

 */

  /* Optional */
  /*  HW Deaggregation Support */
  uint8_t hw_deaggr_supported_valid;  /**< Must be set to true if hw_deaggr_supported is being passed */
  uint8_t hw_deaggr_supported;
  /**<   Informs the remote driver whether the local IPA driver  
         allows de-aggregation to be performed in the hardware.
    */

  /* Optional */
  /*  Maximum Aggregation Frame Size */
  uint8_t max_aggr_frame_size_valid;  /**< Must be set to true if max_aggr_frame_size is being passed */
  uint32_t max_aggr_frame_size;
  /**<   Specifies the maximum size of the aggregated frame that
         the remote driver can expect from this execution environment. \n
         - Valid range: 128 bytes to 32768 bytes.

         @note In QMI IPA version 1.9 and onward, this TLV has been replaced by TLVs
         Downlink FIFO Size and Upload FIFO Size.
    */

  /* Optional */
  /*  IPA Ingress Pipe Mode */
  uint8_t ipa_ingress_pipe_mode_valid;  /**< Must be set to true if ipa_ingress_pipe_mode is being passed */
  ipa_pipe_mode_enum_v01 ipa_ingress_pipe_mode;
  /**<   Indicates to the remote driver if the ingress pipe into the
 IPA is in direct connection with another hardware block or if the
 producer of data to this ingress pipe is a software module. Values: \n
      - QMI_IPA_PIPE_MODE_HW (1) --  Pipe is connected with a hardware block
  
      - QMI_IPA_PIPE_MODE_SW (2) --  Pipe is controlled by the software
  
 */

  /* Optional */
  /*  Peripheral Speed Info */
  uint8_t peripheral_speed_info_valid;  /**< Must be set to true if peripheral_speed_info is being passed */
  ipa_peripheral_speed_enum_v01 peripheral_speed_info;
  /**<   Indicates the speed that the peripheral connected to the IPA supports.
 Values: \n
      - QMI_IPA_PER_USB_FS (1) --  Full-speed USB connection
  
      - QMI_IPA_PER_USB_HS (2) --  High-speed USB connection
  
      - QMI_IPA_PER_USB_SS (3) --  Super-speed USB connection
  
 */

  /* Optional */
  /*  Downlink Accumulation Time Limit */
  uint8_t dl_accumulation_time_limit_valid;  /**< Must be set to true if dl_accumulation_time_limit is being passed */
  uint32_t dl_accumulation_time_limit;
  /**<   Informs the remote driver about the time for which data
         is accumulated in the downlink direction before it is pushed into the
         IPA (downlink is with respect to the WWAN air interface). \n
         - Units: milliseconds \n
         - Maximum value: 255
    */

  /* Optional */
  /*  Downlink Accumulation Packet Limit */
  uint8_t dl_accumulation_pkt_limit_valid;  /**< Must be set to true if dl_accumulation_pkt_limit is being passed */
  uint32_t dl_accumulation_pkt_limit;
  /**<   Informs the remote driver about the number of packets
         that are to be accumulated in the downlink direction before it is pushed
         into the IPA. \n
         - Maximum value: 1023
    */

  /* Optional */
  /*  Downlink Accumulation Byte Limit */
  uint8_t dl_accumulation_byte_limit_valid;  /**< Must be set to true if dl_accumulation_byte_limit is being passed */
  uint32_t dl_accumulation_byte_limit;
  /**<   Inform the remote driver about the number of bytes
         that are to be accumulated in the downlink direction before it is pushed
         into the IPA. \n
         - Maximum value: TBD
    */

  /* Optional */
  /*  Uplink Accumulation Time Limit */
  uint8_t ul_accumulation_time_limit_valid;  /**< Must be set to true if ul_accumulation_time_limit is being passed */
  uint32_t ul_accumulation_time_limit;
  /**<   Inform thes remote driver about the time for which data
         is to be accumulated in the uplink direction before it is pushed into
         the IPA (downlink is with respect to the WWAN air interface). \n
         - Units: milliseconds \n
         - Maximum value: 255
    */

  /* Optional */
  /*  HW Control Flags */
  uint8_t hw_control_flags_valid;  /**< Must be set to true if hw_control_flags is being passed */
  uint32_t hw_control_flags;
  /**<   Informs the remote driver about the hardware control flags: \n

         - Bit 0: IPA_HW_FLAG_HALT_SYSTEM_ON_NON_TERMINAL_FAILURE --
         Indicates to the hardware that it must not continue with
         any subsequent operation even if the failure is not terminal.

         - Bit 1: IPA_HW_FLAG_NO_REPORT_MHI_CHANNEL_ERROR --
         Indicates to the hardware that it is not required to report
         channel errors to the host.

         - Bit 2: IPA_HW_FLAG_NO_REPORT_MHI_CHANNEL_WAKE_UP --
         Indicates to the hardware that it is not required to generate wake-up
         events to the host.

         - Bit 4: IPA_HW_FLAG_WORK_OVER_DDR --
         Indicates to the hardware that it is accessing addresses in the DDR
         and not over PCIe.

         - Bit 5: IPA_HW_FLAG_INTERRUPT_MODE_CTRL_FLAG --
         Indicates whether the device must
         raise an event to let the host know that it is going into an interrupt
         mode (no longer polling for data/buffer availability).
    */

  /* Optional */
  /*  Uplink MSI Event Threshold */
  uint8_t ul_msi_event_threshold_valid;  /**< Must be set to true if ul_msi_event_threshold is being passed */
  uint32_t ul_msi_event_threshold;
  /**<   Informs the remote driver about the threshold that will
         cause an interrupt (MSI) to be fired to the host. This ensures
         that the remote driver does not accumulate an excesive number of
         events before firing an interrupt.

         This threshold is applicable for data that is moved in the UL direction. \n
         - Maximum value: 65535
    */

  /* Optional */
  /*  Downlink MSI Event Threshold */
  uint8_t dl_msi_event_threshold_valid;  /**< Must be set to true if dl_msi_event_threshold is being passed */
  uint32_t dl_msi_event_threshold;
  /**<   Informs the remote driver about the threshold that will
         cause an interrupt (MSI) to be fired to the host. This ensures
         that the remote driver does not accumulate an excesive number of
         events before firing an interrupt.

         This threshold is applicable for data that is moved in the DL direction. \n
         - Maximum value: 65535
    */

  /* Optional */
  /*  Uplink FIFO Size */
  uint8_t ul_fifo_size_valid;  /**< Must be set to true if ul_fifo_size is being passed */
  uint32_t ul_fifo_size;
  /**<   Informs the remote driver about the total Uplink xDCI buffer size 
         that holds the complete aggregated frame or BAM data FIFO size of the 
         peripheral channel/pipe (in bytes). This TLV replaces the
         max_aggr_frame_size TLV in version 1.9 and onward, and the
         max_aggr_frame_size TLV will be ignored in the presence 
         of this TLV. */

  /* Optional */
  /*  Downlink FIFO Size */
  uint8_t dl_fifo_size_valid;  /**< Must be set to true if dl_fifo_size is being passed */
  uint32_t dl_fifo_size;
  /**<   Informs the remote driver about the total Downlink xDCI buffering 
             capacity or BAM data FIFO size of the peripheral channel/pipe
             (in bytes). dl_fifo_size = n * dl_buf_size. This TLV replaces the
         max_aggr_frame_size TLV in version 1.9 and onward, and the
         max_aggr_frame_size TLV will be ignored in the presence 
         of this TLV. */

  /* Optional */
  /*  Downlink Buffer Size */
  uint8_t dl_buf_size_valid;  /**< Must be set to true if dl_buf_size is being passed */
  uint32_t dl_buf_size;
  /**<   Informs the remote driver about the single xDCI buffer size (in bytes. 
         This is applicable only in GSI mode. */
}ipa_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Notifies the remote driver of the configuration information. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Message exchanged between the control point and the service to notify the 
           remote driver to prevent the transport link
           from going into a Low Power state because low latency traffic
           is in progress.  */
typedef struct {

  /* Optional */
  /*  Request ID */
  uint8_t request_id_valid;  /**< Must be set to true if request_id is being passed */
  uint32_t request_id;
  /**<   ID of the request that is being sent to
         the server. The same request ID will be used in the message to remove
         the enable_link_low_power_state request. 
         The server is expected to keep track of the request ID and the 
         link_inactivity_threshold that it can revert as needed.
    */

  /* Optional */
  /*  Link Endpoint ID */
  uint8_t link_ep_id_valid;  /**< Must be set to true if link_ep_id is being passed */
  data_ep_id_type_v01 link_ep_id;
  /**<   \n Specifies the link endpoint ID on which the requested 
         action must be taken. This endpoint ID is a global identifier 
         used to identify the hardware-accelerated channel. */
}ipa_disable_link_low_pwr_state_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Message exchanged between the control point and the service to notify the 
           remote driver to prevent the transport link
           from going into a Low Power state because low latency traffic
           is in progress.  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_disable_link_low_pwr_state_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Message exchanged between the control point and the service to notify the 
           remote driver about lifting the requirement to prevent a transport
           link Low Power state becuase low latency traffic has ended. */
typedef struct {

  /* Optional */
  /*  Request ID */
  uint8_t request_id_valid;  /**< Must be set to true if request_id is being passed */
  uint32_t request_id;
  /**<   Identifies the request that was sent to the server to
         disbale the link Low Power state. 
         This request simply undoes the operation done in that request.
    */
}ipa_enable_link_low_pwr_state_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Message exchanged between the control point and the service to notify the 
           remote driver about lifting the requirement to prevent a transport
           link Low Power state becuase low latency traffic has ended. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_enable_link_low_pwr_state_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_STATS_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_IPA_STATS_TYPE_INVALID_V01 = 0, /**<  Invalid stats type identifier 
   */
  QMI_IPA_STATS_TYPE_PIPE_V01 = 1, /**<  Pipe stats type
   */
  QMI_IPA_STATS_TYPE_FILTER_RULES_V01 = 2, /**<  Filter rule stats type 
   */
  IPA_STATS_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_stats_type_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t pipe_index;
  /**<   Pipe index for statistics to be retrieved.
  */

  uint64_t num_ipv4_packets;
  /**<   Accumulated number of IPv4 packets over this pipe.
  */

  uint64_t num_ipv4_bytes;
  /**<   Accumulated number of IPv4 bytes over this pipe.
  */

  uint64_t num_ipv6_packets;
  /**<   Accumulated number of IPv6 packets over this pipe.
  */

  uint64_t num_ipv6_bytes;
  /**<   Accumulated number of IPv6 bytes over this pipe.
  */
}ipa_pipe_stats_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t filter_rule_index;
  /**<   Filter rule index for statistics to be retrieved.
  */

  uint64_t num_packets;
  /**<   Accumulated number of packets over this filter rule.
  */
}ipa_stats_type_filter_rule_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Retrieves the data statistics collected at the modem IPA driver. */
typedef struct {

  /* Mandatory */
  /*  Stats Type  */
  ipa_stats_type_enum_v01 ipa_stats_type;
  /**<   Indicates the type of statistics to be retrieved.
    */

  /* Optional */
  /*  Reset Statistics  */
  uint8_t reset_stats_valid;  /**< Must be set to true if reset_stats is being passed */
  uint8_t reset_stats;
  /**<   Option to reset the specific type of data statistics currently 
         collected.
    */
}ipa_get_data_stats_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Retrieves the data statistics collected at the modem IPA driver. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Stats Type  */
  uint8_t ipa_stats_type_valid;  /**< Must be set to true if ipa_stats_type is being passed */
  ipa_stats_type_enum_v01 ipa_stats_type;
  /**<   Indicates the type of statistics that are retrieved.
    */

  /* Optional */
  /*  Uplink Source Pipe Statistics List */
  uint8_t ul_src_pipe_stats_list_valid;  /**< Must be set to true if ul_src_pipe_stats_list is being passed */
  uint32_t ul_src_pipe_stats_list_len;  /**< Must be set to # of elements in ul_src_pipe_stats_list */
  ipa_pipe_stats_info_type_v01 ul_src_pipe_stats_list[QMI_IPA_MAX_PIPES_V01];
  /**<   \n List of all uplink pipe statistics that are retrieved.
    */

  /* Optional */
  /*  Downlink Destination Pipe Statistics List */
  uint8_t dl_dst_pipe_stats_list_valid;  /**< Must be set to true if dl_dst_pipe_stats_list is being passed */
  uint32_t dl_dst_pipe_stats_list_len;  /**< Must be set to # of elements in dl_dst_pipe_stats_list */
  ipa_pipe_stats_info_type_v01 dl_dst_pipe_stats_list[QMI_IPA_MAX_PIPES_V01];
  /**<   \n List of all downlink pipe statistics that are retrieved.
    */

  /* Optional */
  /*  Downlink Filter Rule Stats List */
  uint8_t dl_filter_rule_stats_list_valid;  /**< Must be set to true if dl_filter_rule_stats_list is being passed */
  uint32_t dl_filter_rule_stats_list_len;  /**< Must be set to # of elements in dl_filter_rule_stats_list */
  ipa_stats_type_filter_rule_v01 dl_filter_rule_stats_list[QMI_IPA_MAX_FILTERS_V01];
  /**<   \n List of all downlink filter rule statistics retrieved.
    */
}ipa_get_data_stats_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t mux_id;
  /**<   Indicates the mux ID associated with the APN for which the data usage
       statistics is queried.
  */

  uint64_t num_ul_packets;
  /**<   Accumulated number of uplink packets corresponding to this mux ID. 
  */

  uint64_t num_ul_bytes;
  /**<   Accumulated number of uplink bytes corresponding to this mux ID.
  */

  uint64_t num_dl_packets;
  /**<   Accumulated number of downlink packets corresponding to this mux ID. 
  */

  uint64_t num_dl_bytes;
  /**<   Accumulated number of downlink bytes corresponding to this mux ID. 
  */
}ipa_apn_data_stats_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Retrieves the APN data statistics collected from the modem. */
typedef struct {

  /* Optional */
  /*  Mux ID List */
  uint8_t mux_id_list_valid;  /**< Must be set to true if mux_id_list is being passed */
  uint32_t mux_id_list_len;  /**< Must be set to # of elements in mux_id_list */
  uint32_t mux_id_list[QMI_IPA_MAX_APN_V01];
  /**<   List of mux IDs associated with APNs for which the data usage
         statistics are being retrieved.
    */
}ipa_get_apn_data_stats_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Retrieves the APN data statistics collected from the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  APN Data Statistics List */
  uint8_t apn_data_stats_list_valid;  /**< Must be set to true if apn_data_stats_list is being passed */
  uint32_t apn_data_stats_list_len;  /**< Must be set to # of elements in apn_data_stats_list */
  ipa_apn_data_stats_info_type_v01 apn_data_stats_list[QMI_IPA_MAX_APN_V01];
  /**<   \n List of APN data statistics retrieved as per the request on mux_id. \n
         @note Currently, only one APN monitoring is supported on the modem driver.
         It will be expanded to support more APNs in the future.
    */
}ipa_get_apn_data_stats_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t mux_id;
  /**<   Indicates the mux ID associated with the APN for which the data usage
       quota is to be set.
  */

  uint64_t num_Mbytes;
  /**<   Number of bytes that transferred on the APN associated with this
       mux ID when the quota value was reached.
  */
}ipa_data_usage_quota_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Master driver sets a data usage quota value on the modem driver.  */
typedef struct {

  /* Optional */
  /*  APN Quota List */
  uint8_t apn_quota_list_valid;  /**< Must be set to true if apn_quota_list is being passed */
  uint32_t apn_quota_list_len;  /**< Must be set to # of elements in apn_quota_list */
  ipa_data_usage_quota_info_type_v01 apn_quota_list[QMI_IPA_MAX_APN_V01];
  /**<   \n The list of APNs on which a data usage quota is to be set on the modem driver. \n
         @note Currently, only one APN monitoring is supported on the modem driver.
         It will be expanded to support more APNs in the future.
    */

  /* Optional */
  /*  APN Warning List */
  uint8_t apn_warning_list_valid;  /**< Must be set to true if apn_warning_list is being passed */
  uint32_t apn_warning_list_len;  /**< Must be set to # of elements in apn_warning_list */
  ipa_data_usage_quota_info_type_v01 apn_warning_list[QMI_IPA_MAX_APN_V01];
  /**<   \n The list of APNs on which a data usage warning is to be set on the modem driver. \n
         @note Currently, only one APN monitoring is supported on the modem driver.
         It will be expanded to support more APNs in the future.
    */
}ipa_set_data_usage_quota_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Master driver sets a data usage quota value on the modem driver.  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_set_data_usage_quota_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Indication Message; Modem driver sends this indication to the master driver when the data
           usage quota or warning limit is reached. */
typedef struct {

  /* Mandatory */
  /*  APN Quota or Warning List */
  ipa_data_usage_quota_info_type_v01 apn;
  /**<   \n This message indicates which APN has reached the previously set quota or warning. \n
         @note Currently, only one APN monitoring is supported on the modem driver.
    */

  /* Optional */
  /*  Warning Limit reached indication */
  uint8_t is_warning_limit_valid;  /**< Must be set to true if is_warning_limit is being passed */
  uint8_t is_warning_limit;
  /**<   \n If set to TRUE, Warning Limit is reached. \n
            If set to FALSE, Quota Limit is reached.
    */
}ipa_data_usage_quota_reached_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Master driver requests the modem driver to terminate the current data
           usage quota or warning limit monitoring session. */
typedef struct {

  /* Optional */
  /*  Stop monitoring Quota Limit */
  uint8_t is_quota_limit_valid;  /**< Must be set to true if is_quota_limit is being passed */
  uint8_t is_quota_limit;
  /**<   \n If set to TRUE, Quota Limit will not be monitored
    */

  /* Optional */
  /*  Stop monitoring Warning Limit */
  uint8_t is_warning_limit_valid;  /**< Must be set to true if is_warning_limit is being passed */
  uint8_t is_warning_limit;
  /**<   \n If set to TRUE, Warning Limit will not be monitored
    */
}ipa_stop_data_usage_quota_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Master driver requests the modem driver to terminate the current data
           usage quota or warning limit monitoring session. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_stop_data_usage_quota_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Request from Modem IPA driver to set DPL peripheral pipe */
typedef struct {

  /* Mandatory */
  /*  Modem Driver set DPL peripheral pipe; */
  ipa_peripheral_type_enum_v01 peripheral_type;
  /**<   Identifies the peripheral type to be used for DPL
	*/
}ipa_set_dpl_peripheral_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Request from Modem IPA driver to set DPL peripheral pipe */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_set_dpl_peripheral_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Indication Message; Request from Modem IPA driver to set DPL peripheral pipe */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  ipa_peripheral_type_enum_v01 peripheral_type;
  /**<   Identifies the peripheral type that is used for DPL
  */
}ipa_set_dpl_peripheral_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Requests installation of filtering rules in the hardware block on the remote side. */
typedef struct {

  /* Optional */
  /*  Extended Filter Specification  */
  uint8_t filter_spec_ex_list_valid;  /**< Must be set to true if filter_spec_ex_list is being passed */
  uint32_t filter_spec_ex_list_len;  /**< Must be set to # of elements in filter_spec_ex_list */
  ipa_filter_spec_ex_type_v01 filter_spec_ex_list[QMI_IPA_MAX_FILTERS_EX_V01];
  /**<   \n List of filter specifications of filters that must be installed in
         the IPAv3.x hardware.
         The driver installing these rules must do so in the same order as
         specified in this list.
    */

  /* Optional */
  /*  Pipe Index to Install Rule */
  uint8_t source_pipe_index_valid;  /**< Must be set to true if source_pipe_index is being passed */
  uint32_t source_pipe_index;
  /**<   Pipe index to install the filter rule.
         The requester may not always know the pipe indices. If not specified,
         the receiver must install this rule on all pipes that it controls,
         through which data may be fed into the IPA.
    */

  /* Optional */
  /*  Total Number of IPv4 Filters in the Filter Spec List */
  uint8_t num_ipv4_filters_valid;  /**< Must be set to true if num_ipv4_filters is being passed */
  uint32_t num_ipv4_filters;
  /**<   Number of IPv4 rules included in the filter specification list.
    */

  /* Optional */
  /*  Total Number of IPv6 Filters in the Filter Spec List */
  uint8_t num_ipv6_filters_valid;  /**< Must be set to true if num_ipv6_filters is being passed */
  uint32_t num_ipv6_filters;
  /**<   Number of IPv6 rules included in the filter specification list.
    */

  /* Optional */
  /*  List of XLAT Filter Indices in the Filter Spec List */
  uint8_t xlat_filter_indices_list_valid;  /**< Must be set to true if xlat_filter_indices_list is being passed */
  uint32_t xlat_filter_indices_list_len;  /**< Must be set to # of elements in xlat_filter_indices_list */
  uint32_t xlat_filter_indices_list[QMI_IPA_MAX_FILTERS_EX_V01];
  /**<   List of XLAT filter indices.
         Filter rules at specified indices must be modified by the 
         receiver if the PDN is XLAT before installing them on the associated
         IPA consumer pipe.
    */

  /* Optional */
  /*  Extended Type 2 Filter Specification  */
  uint8_t filter_spec_ex2_list_valid;  /**< Must be set to true if filter_spec_ex2_list is being passed */
  uint32_t filter_spec_ex2_list_len;  /**< Must be set to # of elements in filter_spec_ex2_list */
  ipa_filter_spec_ex2_type_v01 filter_spec_ex2_list[QMI_IPA_MAX_FILTERS_V01];
  /**<   \n List of filter specifications of filters that must be installed in
         the IPAv4.x hardware.

         The driver installing these rules must do so in the same order as
         specified in this list.
    */

  /* Optional */
  /*  List of modem UL Filters in the Spec List which need be to replicated
 with AP UL firewall filters */
  uint8_t ul_firewall_indices_list_valid;  /**< Must be set to true if ul_firewall_indices_list is being passed */
  uint32_t ul_firewall_indices_list_len;  /**< Must be set to # of elements in ul_firewall_indices_list */
  uint32_t ul_firewall_indices_list[QMI_IPA_MAX_FILTERS_V01];
  /**<   List of UL firewall filter indices.
         Filter rules at specified indices must be replicated across the firewall
         filters by the receiver and installed on the associated IPA consumer pipe.
    */
}ipa_install_fltr_rule_req_ex_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Requests installation of filtering rules in the hardware block on the remote side. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */

  /* Optional */
  /*  Rule ID List */
  uint8_t rule_id_valid;  /**< Must be set to true if rule_id is being passed */
  uint32_t rule_id_len;  /**< Must be set to # of elements in rule_id */
  uint32_t rule_id[QMI_IPA_MAX_FILTERS_EX_V01];
  /**<   List of rule IDs returned to the control point. 
         Any further reference to the rule is done using the filter rule ID 
         specified in this list.
    */
}ipa_install_fltr_rule_resp_ex_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Requests the modem IPA driver to enable or disable collection of per
           client statistics. */
typedef struct {

  /* Mandatory */
  /*  Collect statistics per client. */
  uint8_t enable_per_client_stats;
  /**<   Indicates whether to start or stop collecting per client statistics.
  */
}ipa_enable_per_client_stats_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Requests the modem IPA driver to enable or disable collection of per
           client statistics. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_enable_per_client_stats_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t client_id;
  /**<   Identifier of the client for which remote driver needs to send uplink/
       downlink statistics.
  */

  uint32_t src_pipe_id;
  /**<   IPA consumer pipe on which client on AP sent uplink data to modem.
  */

  uint64_t num_ul_ipv4_bytes;
  /**<   Accumulated number of uplink IPv4 bytes for a client.
  */

  uint64_t num_ul_ipv6_bytes;
  /**<   Accumulated number of uplink IPv6 bytes for a client.
  */

  uint64_t num_dl_ipv4_bytes;
  /**<   Accumulated number of downlink IPv4 bytes for a client.
  */

  uint64_t num_dl_ipv6_bytes;
  /**<   Accumulated number of downlink IPv6 bytes for a client.
  */

  uint32_t num_ul_ipv4_pkts;
  /**<   Accumulated number of uplink IPv4 packets for a client.
  */

  uint32_t num_ul_ipv6_pkts;
  /**<   Accumulated number of uplink IPv6 packets for a client.
  */

  uint32_t num_dl_ipv4_pkts;
  /**<   Accumulated number of downlink IPv4 packets for a client.
  */

  uint32_t num_dl_ipv6_pkts;
  /**<   Accumulated number of downlink IPv6 packets for a client.
  */
}ipa_per_client_stats_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Requests the remote IPA driver to provide statistics for a given 
           client. */
typedef struct {

  /* Mandatory */
  /*  Client id */
  uint32_t client_id;
  /**<   Identifier of the client for which remote driver needs to send its 
         uplink/downlink statistics.
    */

  /* Mandatory */
  /*  Source pipe id */
  uint32_t src_pipe_id;
  /**<   IPA consumer pipe on which client sent uplink data to 
         modem driver.
    */

  /* Optional */
  /*  Reset Client statistics */
  uint8_t reset_stats_valid;  /**< Must be set to true if reset_stats is being passed */
  uint8_t reset_stats;
  /**<   Option to reset the statistics currently collected by modem driver 
         for a particular client. Default value is 0.
    */
}ipa_get_stats_per_client_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Requests the remote IPA driver to provide statistics for a given 
           client. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Per clients Statistics List */
  uint8_t per_client_stats_list_valid;  /**< Must be set to true if per_client_stats_list is being passed */
  uint32_t per_client_stats_list_len;  /**< Must be set to # of elements in per_client_stats_list */
  ipa_per_client_stats_info_type_v01 per_client_stats_list[QMI_IPA_MAX_PER_CLIENTS_V01];
  /**<   \n List of all per client statistics that are retrieved.
    */
}ipa_get_stats_per_client_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  ipa_ip_type_enum_v01 ip_type;
  /**<   IP type for which this rule is applicable.
 The driver must identify the filter table (v6 or v4), and this
 field is essential for that. Values: \n
      - QMI_IPA_IP_TYPE_INVALID (0) --  Invalid IP type identifier \n
  
      - QMI_IPA_IP_TYPE_V4 (1) --  IPv4 type \n
  
      - QMI_IPA_IP_TYPE_V6 (2) --  IPv6 type \n
  
      - QMI_IPA_IP_TYPE_V4V6 (3) --  Applies to both IP types
  
 */

  ipa_filter_rule_type_v01 filter_rule;
  /**<   Rules in the filter specification. These rules are the
         ones that are matched against fields in the packet.
    */
}ipa_ul_firewall_rule_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Requestes remote IPA driver to configure uplink firewall rules. */
typedef struct {

  /* Mandatory */
  /*  Uplink Firewall Rules list */
  uint32_t firewall_rules_list_len;  /**< Must be set to # of elements in firewall_rules_list */
  ipa_ul_firewall_rule_type_v01 firewall_rules_list[QMI_IPA_MAX_UL_FIREWALL_RULES_V01];
  /**<   \n List of uplink firewall rules that must be installed.
    */

  /* Mandatory */
  /*  Mux id */
  uint32_t mux_id;
  /**<   QMAP Mux ID. As a part of the QMAP protocol,
         several data calls may be multiplexed over the same physical transport
         channel. This identifier is used to identify one such data call.
         The maximum value for this identifier is 255.
    */

  /* Optional */
  /*  Uplink Firewall disable  */
  uint8_t disable_valid;  /**< Must be set to true if disable is being passed */
  uint8_t disable;
  /**<   Indicates whether uplink firewall needs to be disabled.
    */

  /* Optional */
  /*  Are blacklist filters  */
  uint8_t are_blacklist_filters_valid;  /**< Must be set to true if are_blacklist_filters is being passed */
  uint8_t are_blacklist_filters;
  /**<   Indicates whether the filters recieved as part of this message are 
         blacklist filters. i.e. drop uplink packets matching these rules. 
         Default value is 0.
    */
}ipa_configure_ul_firewall_rules_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Requestes remote IPA driver to configure uplink firewall rules. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */
}ipa_configure_ul_firewall_rules_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_UL_FIREWALL_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_IPA_UL_FIREWALL_STATUS_SUCCESS_V01 = 0, /**<  Indicates that the uplink firewall rules are configured successfully.
   */
  QMI_IPA_UL_FIREWALL_STATUS_FAILURE_V01 = 1, /**<  Indicates that the uplink firewall rules are not configured successfully.
   */
  IPA_UL_FIREWALL_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_ul_firewall_status_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  ipa_ul_firewall_status_enum_v01 is_success;
  /**<   Indicates whether the uplink firewall rules are configured 
         successfully.
    */

  uint32_t mux_id;
  /**<   Mux id on which uplink firewall rules are configured.
    */
}ipa_ul_firewall_config_result_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Indication Message; Requestes remote IPA driver to configure uplink firewall rules. */
typedef struct {

  /* Mandatory */
  /*  Result */
  ipa_ul_firewall_config_result_type_v01 result;
}ipa_configure_ul_firewall_rules_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_CLOCK_RATE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_IPA_CLOCK_RATE_INVALID_V01 = 0, /**<  Invalid clock rate
    */
  QMI_IPA_CLOCK_RATE_LOW_SVS_V01 = 1, /**<  Low SVS clock rate
    */
  QMI_IPA_CLOCK_RATE_SVS_V01 = 2, /**<  SVS clock rate
   */
  QMI_IPA_CLOCK_RATE_NOMINAL_V01 = 3, /**<  Nominal clock rate
   */
  QMI_IPA_CLOCK_RATE_TURBO_V01 = 4, /**<  Turbo clock rate
   */
  IPA_CLOCK_RATE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_clock_rate_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Requests the AP IPA driver to vote/devote PCIe clocks. */
typedef struct {

  /* Mandatory */
  /*  MHI Vote */
  uint8_t mhi_vote;
  /**<   MHI vote request
       TRUE  - ON
       FALSE - OFF
  */

  /* Optional */
  /*  Throughput Value */
  uint8_t tput_value_valid;  /**< Must be set to true if tput_value is being passed */
  uint32_t tput_value;
  /**<   Throughput value in MBPS
  */

  /* Optional */
  /*  IPA Clock Rate */
  uint8_t clk_rate_valid;  /**< Must be set to true if clk_rate is being passed */
  ipa_clock_rate_enum_v01 clk_rate;
  /**<   IPA Clock Rate
  */
}ipa_mhi_clk_vote_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Requests the AP IPA driver to vote/devote PCIe clocks. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */
}ipa_mhi_clk_vote_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t ch_id;
  /**<   Remote MHI channel ID.
  */

  uint8_t er_id;
  /**<   Remote MHI Event ring ID.
  */

  uint32_t ch_doorbell_addr;
  /**<   TR Channel Doorbell addr.
  */

  uint32_t er_doorbell_addr;
  /**<   Event ring Doorbell addr.
  */

  uint32_t direction_type;
  /**<   Direction type.
  */
}ipa_mhi_ch_init_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint64_t iova_ctl_base_addr;
  /**<   IOVA mapped Control Region base address
  */

  uint64_t iova_ctl_size;
  /**<   IOVA Control region size in bytes
  */

  uint64_t iova_data_base_addr;
  /**<   IOVA mapped Data Region base address
  */

  uint64_t iova_data_size;
  /**<   IOVA Data Region size in bytes
  */
}ipa_mhi_smmu_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Indication Message; Requests the modem IPA driver to initiate Remote MHI Channel initialization. */
typedef struct {

  /* Mandatory */
  /*  ch_info_arr */
  uint32_t ch_info_arr_len;  /**< Must be set to # of elements in ch_info_arr */
  ipa_mhi_ch_init_info_type_v01 ch_info_arr[QMI_IPA_REMOTE_MHI_CHANNELS_NUM_MAX_V01];
  /**<   Channel Information array
  */

  /* Optional */
  /*  smmu_info */
  uint8_t smmu_info_valid;  /**< Must be set to true if smmu_info is being passed */
  ipa_mhi_smmu_info_type_v01 smmu_info;
  /**<   SMMU Information
  */
}ipa_mhi_ready_indication_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint64_t pa;
  /**<   Memory region start physical addr
  */

  uint64_t iova;
  /**<   Memory region start iova mapped addr
  */

  uint64_t size;
  /**<   Memory region size in bytes
  */
}ipa_mhi_mem_addr_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_MHI_BRST_MODE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_IPA_BURST_MODE_DEFAULT_V01 = 0, /**<  Default - burst mode enabled for hardware channels, 
        disabled for software channels
   */
  QMI_IPA_BURST_MODE_ENABLED_V01 = 1, /**<  Burst mode is enabled for this channel
   */
  QMI_IPA_BURST_MODE_DISABLED_V01 = 2, /**<  Burst mode is disabled for this channel
   */
  IPA_MHI_BRST_MODE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_mhi_brst_mode_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t ch_id;
  /**<   TR Channel ID
  */

  uint16_t poll_cfg;
  /**<   Poll Configuration -
       Deafult or timer to poll the MHI context in milliseconds 
  */

  ipa_mhi_brst_mode_enum_v01 brst_mode_type;
  /**<   Burst mode configuration
  */

  uint64_t ring_iova;
  /**<   IOVA mapped ring base address
  */

  uint64_t ring_len;
  /**<   Ring Length in bytes
  */

  uint64_t rp;
  /**<   IOVA mapped Read pointer address
  */

  uint64_t wp;
  /**<   IOVA mapped write pointer address
  */
}ipa_mhi_tr_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t er_id;
  /**<   Event ring ID
  */

  uint32_t intmod_cycles;
  /**<   Interrupt moderation cycles
  */

  uint32_t intmod_count;
  /**<   Interrupt moderation count
  */

  uint32_t msi_addr;
  /**<   IOVA mapped MSI address for this ER
  */

  uint64_t ring_iova;
  /**<   IOVA mapped ring base address
  */

  uint64_t ring_len;
  /**<   Ring length in bytes
  */

  uint64_t rp;
  /**<   IOVA mapped Read pointer address
  */

  uint64_t wp;
  /**<   IOVA mapped Write pointer address
  */
}ipa_mhi_er_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Requests the AP IPA driver to perform Remote MHI Channel allocation */
typedef struct {

  /* Mandatory */
  /*  tr_info_arr */
  uint32_t tr_info_arr_len;  /**< Must be set to # of elements in tr_info_arr */
  ipa_mhi_tr_info_type_v01 tr_info_arr[QMI_IPA_REMOTE_MHI_CHANNELS_NUM_MAX_V01];
  /**<   Array of TR context information for Remote MHI channels
  */

  /* Mandatory */
  /*  er_info_arr */
  uint32_t er_info_arr_len;  /**< Must be set to # of elements in er_info_arr */
  ipa_mhi_er_info_type_v01 er_info_arr[QMI_IPA_REMOTE_MHI_CHANNELS_NUM_MAX_V01];
  /**<   Array of ER context information for Remote MHI channels
  */

  /* Mandatory */
  /*  ctrl_addr_map_info */
  uint32_t ctrl_addr_map_info_len;  /**< Must be set to # of elements in ctrl_addr_map_info */
  ipa_mhi_mem_addr_info_type_v01 ctrl_addr_map_info[QMI_IPA_REMOTE_MHI_MEMORY_MAPPING_NUM_MAX_V01];
  /**<   List of PA-IOVA address mappings for control regions used by Modem
  */

  /* Mandatory */
  /*  data_addr_map_info */
  uint32_t data_addr_map_info_len;  /**< Must be set to # of elements in data_addr_map_info */
  ipa_mhi_mem_addr_info_type_v01 data_addr_map_info[QMI_IPA_REMOTE_MHI_MEMORY_MAPPING_NUM_MAX_V01];
  /**<   List of PA-IOVA address mappings for data regions used by Modem
  */
}ipa_mhi_alloc_channel_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t ch_id;
  /**<   Remote MHI channel ID.
  */

  uint8_t is_success;
  /**<   Channel Allocation Status.
  */
}ipa_mhi_ch_alloc_resp_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Requests the AP IPA driver to perform Remote MHI Channel allocation */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described
                          in the error codes section of each message definition.
  */

  /* Optional */
  /*  alloc_resp_arr */
  uint8_t alloc_resp_arr_valid;  /**< Must be set to true if alloc_resp_arr is being passed */
  uint32_t alloc_resp_arr_len;  /**< Must be set to # of elements in alloc_resp_arr */
  ipa_mhi_ch_alloc_resp_type_v01 alloc_resp_arr[QMI_IPA_REMOTE_MHI_CHANNELS_NUM_MAX_V01];
  /**<   MHI channel allocation response array
  */
}ipa_mhi_alloc_channel_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Requests the Modem IPA driver to perform SSR cleanup */
typedef struct {

  /* Optional */
  /*  cleanup */
  uint8_t cleanup_valid;  /**< Must be set to true if cleanup is being passed */
  uint8_t cleanup;
  /**<   a Flag to indicate the type of action
       0 - Reject Request
       1 - Cleanup Request
  */
}ipa_mhi_cleanup_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Requests the Modem IPA driver to perform SSR cleanup */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */
}ipa_mhi_cleanup_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_EP_DESC_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DATA_EP_DESC_TYPE_RESERVED_V01 = 0x00, /**<   Reserved \n  
   */
  DATA_EP_DESC_TYPE_EMB_CONS_V01 = 0x01, /**<  Embedded consumer \n  
   */
  DATA_EP_DESC_TYPE_EMB_PROD_V01 = 0x02, /**<  Embedded producer \n  
   */
  DATA_EP_DESC_TYPE_RSC_PROD_V01 = 0x03, /**<   RSC producer \n  
   */
  DATA_EP_DESC_TYPE_QDSS_PROD_V01 = 0x04, /**<   QDSS producer \n  
   */
  DATA_EP_DESC_TYPE_DPL_PROD_V01 = 0x05, /**<   DPL producer \n  
   */
  DATA_EP_DESC_TYPE_TETH_CONS_V01 = 0x06, /**<   Tethered consumer \n  
   */
  DATA_EP_DESC_TYPE_TETH_PROD_V01 = 0x07, /**<   Tethered producer \n  
   */
  DATA_EP_DESC_TYPE_TETH_RMNET_CONS_V01 = 0x08, /**<   Tethered RMNET consumer \n  
   */
  DATA_EP_DESC_TYPE_TETH_RMNET_PROD_V01 = 0x09, /**<   Tethered RMNET producer \n  
   */
  DATA_EP_DESC_TYPE_EMB_FLOW_CTL_CONS_V01 = 0x0A, /**<   Embedded Flow Control consumer \n  
   */
  DATA_EP_DESC_TYPE_EMB_FLOW_CTL_PROD_V01 = 0x0B, /**<   Embedded Flow Control producer \n  
   */
  DATA_EP_DESC_TYPE_EMB_LL_CONS_V01 = 0x0C, /**<  Embedded consumer \n  
   */
  DATA_EP_DESC_TYPE_EMB_LL_PROD_V01 = 0x0D, /**<  Embedded producer \n  
   */
  IPA_EP_DESC_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_ep_desc_type_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_IC_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DATA_IC_TYPE_RESERVED_V01 = 0x00, /**<   Reserved \n  
   */
  DATA_IC_TYPE_MHI_V01 = 0x01, /**<  MHI \n  
   */
  DATA_IC_TYPE_MHI_PRIME_V01 = 0x02, /**<   MHI Prime \n  
   */
  DATA_IC_TYPE_USB_V01 = 0x03, /**<   USB \n  
   */
  DATA_IC_TYPE_AP_V01 = 0x04, /**<   AP \n  
   */
  DATA_IC_TYPE_Q6_V01 = 0x05, /**<   Q6 \n  
   */
  DATA_IC_TYPE_UC_V01 = 0x06, /**<   UC \n  
   */
  IPA_IC_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_ic_type_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_EP_STATUS_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DATA_EP_STATUS_RESERVED_V01 = 0x00, /**<   Reserved \n  
   */
  DATA_EP_STATUS_STATIC_V01 = 0x01, /**<  There is no information about status change \n  
   */
  DATA_EP_STATUS_CONNECTED_V01 = 0x02, /**<  Endpoint is connected \n  
   */
  DATA_EP_STATUS_DISCONNECTED_V01 = 0x03, /**<  Endpoint is disconnected \n  
   */
  IPA_EP_STATUS_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_ep_status_type_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  ipa_ic_type_enum_v01 ic_type;
  /**<     Interconnect type. Values: See ipa_ic_desc_type_enum type\n
 All other values are reserved and will be ignored by service or clients.
 */

  ipa_ep_desc_type_enum_v01 ep_type;
  /**<     Peripheral end point type. Values: See ipa_ep_status_type type\n
 All other values are reserved and will be ignored by service or clients.
 */

  uint32_t ep_id;
  /**<     Peripheral interface number. \n
  */

  ipa_ep_status_type_v01 ep_status;
  /**<     Status of endpoint. \n
  */
}ipa_ep_id_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Indication Message; Informs modem about role of endpoint or/and notifies about status change of the endpoints. */
typedef struct {

  /* Optional */
  /*  Enpoint Information Array */
  uint8_t ep_info_valid;  /**< Must be set to true if ep_info is being passed */
  uint32_t ep_info_len;  /**< Must be set to # of elements in ep_info */
  ipa_ep_id_type_v01 ep_info[QMI_IPA_ENDP_DESC_NUM_MAX_V01];
  /**<   Endpoint Information array
  */

  /* Optional */
  /*  Number of endpoints */
  uint8_t num_eps_valid;  /**< Must be set to true if num_eps is being passed */
  uint32_t num_eps;
  /**<     Number of endpoints contained in the message. \n
  */
}ipa_endp_desc_indication_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_AGGR_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DATA_AGGR_TYPE_RESERVED_V01 = 0x00, /**<   Reserved \n  
   */
  DATA_AGGR_TYPE_QMAP_V01 = 0x01, /**<  QMAP \n  
   */
  DATA_AGGR_TYPE_QMAPv5_V01 = 0x02, /**<   QMAP v5 \n  
   */
  DATA_AGGR_TYPE_INHERITED_V01 = 0x03, /**<   Inherited \n  
   */
  IPA_AGGR_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_aggr_enum_type_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  ipa_ic_type_enum_v01 ic_type;
  /**<     Interconnect type. Values: See ipa_ic_desc_type_enum type\n
 All other values are reserved and will be ignored by service or clients.
 */

  ipa_ep_desc_type_enum_v01 ep_type;
  /**<     Peripheral end point type. Values: See ipa_ep_status_type type\n
 All other values are reserved and will be ignored by service or clients.
 */

  uint32_t bytes_count;
  /**<     Bytes count in KB . \n
  */

  uint32_t pkt_count;
  /**<     Packet count 
  */

  ipa_aggr_enum_type_v01 aggr_type;
  /**<     Aggregation Type  
  */
}ipa_mhi_prime_aggr_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Indication Message; Sends Aggregation info to modem IPA driver to setup MHI prime pipes */
typedef struct {

  /* Optional */
  /*  Aggregation information */
  uint8_t aggr_info_valid;  /**< Must be set to true if aggr_info is being passed */
  uint32_t aggr_info_len;  /**< Must be set to # of elements in aggr_info */
  ipa_mhi_prime_aggr_info_type_v01 aggr_info[QMI_IPA_ENDP_DESC_NUM_MAX_V01];
  /**<   Aggregration information for MHI prime
  */

  /* Optional */
  /*  Number of Endpoints */
  uint8_t num_eps_valid;  /**< Must be set to true if num_eps is being passed */
  uint32_t num_eps;
  /**<     Number of endpoints contained in the message. \n
  */
}ipa_mhi_prime_aggr_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Sends Aggregation info to modem IPA driver to setup MHI prime pipes */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */
}ipa_mhi_prime_aggr_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Requests installation of filtering rules in the hardware block on the remote side. */
typedef struct {

  /* Optional */
  /*  Total Number of IPv4 Filters in the Filter Spec List */
  uint8_t num_ipv4_filters_valid;  /**< Must be set to true if num_ipv4_filters is being passed */
  uint32_t num_ipv4_filters;
  /**<   Number of IPv4 rules included in the filter specification list.
    */

  /* Optional */
  /*  Total Number of IPv6 Filters in the Filter Spec List */
  uint8_t num_ipv6_filters_valid;  /**< Must be set to true if num_ipv6_filters is being passed */
  uint32_t num_ipv6_filters;
  /**<   Number of IPv6 rules included in the filter specification list.
    */

  /* Optional */
  /*  List of XLAT Filter Indices in the Filter Spec List */
  uint8_t xlat_filter_indices_list_valid;  /**< Must be set to true if xlat_filter_indices_list is being passed */
  uint32_t xlat_filter_indices_list_len;  /**< Must be set to # of elements in xlat_filter_indices_list */
  uint32_t xlat_filter_indices_list[QMI_IPA_MAX_FILTERS_V01];
  /**<   List of XLAT filter indices.
         Filter rules at specified indices must be modified by the 
         receiver if the PDN is XLAT before installing them on the associated
         IPA consumer pipe.
    */

  /* Optional */
  /*  Extended Filter Specification  */
  uint8_t filter_spec_ex2_list_valid;  /**< Must be set to true if filter_spec_ex2_list is being passed */
  uint32_t filter_spec_ex2_list_len;  /**< Must be set to # of elements in filter_spec_ex2_list */
  ipa_filter_spec_ex2_type_v01 filter_spec_ex2_list[QMI_IPA_MAX_FILTERS_V01];
  /**<   \n List of filter specifications of filters that must be installed in
         the IPAv4.x hardware.

         The driver installing these rules must do so in the same order as
         specified in this list.
    */

  /* Optional */
  /*  Mux ID for embedded call */
  uint8_t embedded_call_mux_id_valid;  /**< Must be set to true if embedded_call_mux_id is being passed */
  uint32_t embedded_call_mux_id;
  /**<   Mux ID for the new embedded call
    */

  /* Optional */
  /*  Default MHI Path */
  uint8_t default_mhi_path_valid;  /**< Must be set to true if default_mhi_path is being passed */
  uint8_t default_mhi_path;
  /**<   Default MHI path flag used to indicate defaut mhi pipe to used for routing rule
       0 - FALSE - MHI Prime path (Default)
       1 - TRUE - Default MHI path
    */
}ipa_add_offload_connection_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Requests installation of filtering rules in the hardware block on the remote side. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */

  /* Optional */
  /*  Filter Handle List */
  uint8_t filter_handle_list_valid;  /**< Must be set to true if filter_handle_list is being passed */
  uint32_t filter_handle_list_len;  /**< Must be set to # of elements in filter_handle_list */
  ipa_filter_rule_identifier_to_handle_map_v01 filter_handle_list[QMI_IPA_MAX_FILTERS_V01];
  /**<   \n List of handles returned to the control point. Each handle is
         mapped to the rule identifier that was specified in the request message.
         Any further reference to the rule is done using the filter handle.
    */
}ipa_add_offload_connection_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Requests removal of filtering rules in the hardware block on the remote side. */
typedef struct {

  /* Optional */
  /*  Filter Handle List */
  uint8_t filter_handle_list_valid;  /**< Must be set to true if filter_handle_list is being passed */
  uint32_t filter_handle_list_len;  /**< Must be set to # of elements in filter_handle_list */
  ipa_filter_rule_identifier_to_handle_map_v01 filter_handle_list[QMI_IPA_MAX_FILTERS_V01];
  /**<   \n List of filter handle list for which filtering rules need to be removed
            from the hardware
    */

  /* Optional */
  /*  Clean All Rules */
  uint8_t clean_all_rules_valid;  /**< Must be set to true if clean_all_rules is being passed */
  uint8_t clean_all_rules;
  /**<   Clean All rules flag used to remove all active MHIP rules
       0 - FALSE - Clean only requested MHIP rules (Default)
       1 - TRUE - Clean all MHIP rules
    */
}ipa_remove_offload_connection_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Requests removal of filtering rules in the hardware block on the remote side. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */
}ipa_remove_offload_connection_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Sends Data bandwidth information */
typedef struct {

  /* Optional */
  /*  Peak Uplink bandwidth */
  uint8_t peak_bw_ul_valid;  /**< Must be set to true if peak_bw_ul is being passed */
  uint32_t peak_bw_ul;
  /**<   Peak bandwidth over uplink in Kbps
    */

  /* Optional */
  /*  Peak Downlink bandwidth */
  uint8_t peak_bw_dl_valid;  /**< Must be set to true if peak_bw_dl is being passed */
  uint32_t peak_bw_dl;
  /**<   Peak bandwidth over downlink in Kbps
    */

  /* Optional */
  /*  Burst Frequency  */
  uint8_t burst_freq_valid;  /**< Must be set to true if burst_freq is being passed */
  uint32_t burst_freq;
  /**<   Number of bursts per milisecond
    */

  /* Optional */
  /*  Average Uplink bandwidth */
  uint8_t avg_bw_ul_valid;  /**< Must be set to true if avg_bw_ul is being passed */
  uint32_t avg_bw_ul;
  /**<   Average bandwidth over uplink in Kbps
    */

  /* Optional */
  /*  Average Downlink bandwidth */
  uint8_t avg_bw_dl_valid;  /**< Must be set to true if avg_bw_dl is being passed */
  uint32_t avg_bw_dl;
  /**<   Average bandwidth over downlink in Kbps
    */
}ipa_bw_change_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Sends Data bandwidth information */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */
}ipa_bw_change_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Indication Message; Modem driver sends data bandwidth information indication to the master driver. */
typedef struct {

  /* Optional */
  /*  Peak Uplink bandwidth */
  uint8_t peak_bw_ul_valid;  /**< Must be set to true if peak_bw_ul is being passed */
  uint32_t peak_bw_ul;
  /**<   Peak bandwidth over uplink in Kbps
    */

  /* Optional */
  /*  Peak Downlink bandwidth */
  uint8_t peak_bw_dl_valid;  /**< Must be set to true if peak_bw_dl is being passed */
  uint32_t peak_bw_dl;
  /**<   Peak bandwidth over downlink in Kbps
    */
}ipa_bw_change_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_LOW_LATENCY_TRAFFIC_INFO_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IPA_LOW_LAT_TRAFFIC_STATUS_INVALID_V01 = 0x00, /**<  Invalid low latency traffic status
    */
  IPA_LOW_LAT_TRAFFIC_STATUS_START_V01 = 0x01, /**<  low latency traffic start
   */
  IPA_LOW_LAT_TRAFFIC_STATUS_STOP_V01 = 0x02, /**<  low latency traffic stop
   */
  IPA_LOW_LAT_TRAFFIC_STATUS_MAX_V01 = 0xFF, /**<  max low latency traffic status
   */
  IPA_LOW_LATENCY_TRAFFIC_INFO_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_low_latency_traffic_info_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Sends IPA low latency traffic information */
typedef struct {

  /* Mandatory */
  /*  Low latency traffic information */
  ipa_low_latency_traffic_info_enum_v01 info;
  /**<   Provides information about the low latency traffic. Values: \n
      - IPA_LOW_LAT_TRAFFIC_STATUS_INVALID (0x00) --  Invalid low latency traffic status
   
      - IPA_LOW_LAT_TRAFFIC_STATUS_START (0x01) --  low latency traffic start
  
      - IPA_LOW_LAT_TRAFFIC_STATUS_STOP (0x02) --  low latency traffic stop
  
      - IPA_LOW_LAT_TRAFFIC_STATUS_MAX (0xFF) --  max low latency traffic status
  
 */

  /* Optional */
  /*  Valid end points */
  uint8_t endp_bitmap_valid;  /**< Must be set to true if endp_bitmap is being passed */
  uint32_t endp_bitmap;
  /**<   32-bit bitmask to indicate which end points are valid in the message. \n
     In the absence of this TLV, the low latency traffic info is applied to all pipes\n  
    */
}ipa_low_latency_traffic_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Sends IPA low latency traffic information */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
  */
}ipa_low_latency_traffic_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_MOVE_NAT_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_IPA_MOVE_NAT_TO_DDR_V01 = 0, /**<   Move NAT to DDR \n  
   */
  QMI_IPA_MOVE_NAT_TO_SRAM_V01 = 1, /**<   Move NAT to SRAM \n  
   */
  IPA_MOVE_NAT_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_move_nat_type_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Sends move NAT command */
typedef struct {

  /* Mandatory */
  /*  NAT move direction */
  ipa_move_nat_type_v01 nat_move_direction;
  /**<   NAT move direction
    */
}ipa_move_nat_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Sends move NAT command */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */
}ipa_move_nat_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Indication Message; Indicates completion of NAT table moving
	       Apps driver sends indication to the modem driver that NAT 
	       table was removed successfully. */
typedef struct {

  /* Mandatory */
  /*  NAT table move status */
  qmi_response_type_v01 nat_table_move_status;
  /**<   Standard response type.
       Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
                            Apps shall always populate the field with QMI_ERR_NONE
  */
}ipa_nat_move_table_complt_ind_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_IPA_ADD_OFFLOAD_CONNECTION_V01
//#define REMOVE_QMI_IPA_BW_CHANGE_V01
//#define REMOVE_QMI_IPA_BW_CHANGE_INDICATION_V01
//#define REMOVE_QMI_IPA_CONFIG_V01
//#define REMOVE_QMI_IPA_CONFIGURE_UL_FIREWALL_RULES_V01
//#define REMOVE_QMI_IPA_DATA_USAGE_QUOTA_REACHED_V01
//#define REMOVE_QMI_IPA_DISABLE_FORCE_CLEAR_DATAPATH_V01
//#define REMOVE_QMI_IPA_DISABLE_LINK_LOW_PWR_STATE_V01
//#define REMOVE_QMI_IPA_ENABLE_FORCE_CLEAR_DATAPATH_V01
//#define REMOVE_QMI_IPA_ENABLE_LINK_LOW_PWR_STATE_V01
//#define REMOVE_QMI_IPA_ENABLE_PER_CLIENT_STATS_V01
//#define REMOVE_QMI_IPA_ENDP_DESC_INDICATION_V01
//#define REMOVE_QMI_IPA_FILTER_INSTALLED_NOTIF_V01
//#define REMOVE_QMI_IPA_GET_APN_DATA_STATS_V01
//#define REMOVE_QMI_IPA_GET_DATA_STATS_V01
//#define REMOVE_QMI_IPA_GET_STATS_PER_CLIENT_V01
//#define REMOVE_QMI_IPA_INDICATION_REGISTER_V01
//#define REMOVE_QMI_IPA_INIT_MODEM_DRIVER_V01
//#define REMOVE_QMI_IPA_INIT_MODEM_DRIVER_CMPLT_V01
//#define REMOVE_QMI_IPA_INSTALL_FILTER_RULE_V01
//#define REMOVE_QMI_IPA_INSTALL_FILTER_RULE_EX_V01
//#define REMOVE_QMI_IPA_LOW_LATENCY_TRAFFIC_INFO_V01
//#define REMOVE_QMI_IPA_MASTER_DRIVER_INIT_COMPLETE_V01
//#define REMOVE_QMI_IPA_MHI_ALLOC_CHANNEL_V01
//#define REMOVE_QMI_IPA_MHI_CLEANUP_V01
//#define REMOVE_QMI_IPA_MHI_CLK_VOTE_V01
//#define REMOVE_QMI_IPA_MHI_PRIME_AGGR_INFO_V01
//#define REMOVE_QMI_IPA_MHI_READY_INDICATION_V01
//#define REMOVE_QMI_IPA_NAT_TABLE_MOVE_COMPLT_IND_V01
//#define REMOVE_QMI_IPA_REMOVE_OFFLOAD_CONNECTION_V01
//#define REMOVE_QMI_IPA_SET_DATA_USAGE_QUOTA_V01
//#define REMOVE_QMI_IPA_SET_DPL_PERIPHERAL_V01
//#define REMOVE_QMI_IPA_STOP_DATA_USAGE_QUOTA_V01

/*Service Message Definition*/
/** @addtogroup ipa_qmi_msg_ids
    @{
  */
#define QMI_IPA_INDICATION_REGISTER_REQ_V01 0x0020
#define QMI_IPA_INDICATION_REGISTER_RESP_V01 0x0020
#define QMI_IPA_INIT_MODEM_DRIVER_REQ_V01 0x0021
#define QMI_IPA_INIT_MODEM_DRIVER_RESP_V01 0x0021
#define QMI_IPA_MASTER_DRIVER_INIT_COMPLETE_IND_V01 0x0022
#define QMI_IPA_INSTALL_FILTER_RULE_REQ_V01 0x0023
#define QMI_IPA_INSTALL_FILTER_RULE_RESP_V01 0x0023
#define QMI_IPA_FILTER_INSTALLED_NOTIF_REQ_V01 0x0024
#define QMI_IPA_FILTER_INSTALLED_NOTIF_RESP_V01 0x0024
#define QMI_IPA_ENABLE_FORCE_CLEAR_DATAPATH_REQ_V01 0x0025
#define QMI_IPA_ENABLE_FORCE_CLEAR_DATAPATH_RESP_V01 0x0025
#define QMI_IPA_DISABLE_FORCE_CLEAR_DATAPATH_REQ_V01 0x0026
#define QMI_IPA_DISABLE_FORCE_CLEAR_DATAPATH_RESP_V01 0x0026
#define QMI_IPA_CONFIG_REQ_V01 0x0027
#define QMI_IPA_CONFIG_RESP_V01 0x0027
#define QMI_IPA_DISABLE_LINK_LOW_PWR_STATE_REQ_V01 0x0028
#define QMI_IPA_DISABLE_LINK_LOW_PWR_STATE_RESP_V01 0x0028
#define QMI_IPA_ENABLE_LINK_LOW_PWR_STATE_REQ_V01 0x0029
#define QMI_IPA_ENABLE_LINK_LOW_PWR_STATE_RESP_V01 0x0029
#define QMI_IPA_GET_DATA_STATS_REQ_V01 0x0030
#define QMI_IPA_GET_DATA_STATS_RESP_V01 0x0030
#define QMI_IPA_GET_APN_DATA_STATS_REQ_V01 0x0031
#define QMI_IPA_GET_APN_DATA_STATS_RESP_V01 0x0031
#define QMI_IPA_SET_DATA_USAGE_QUOTA_REQ_V01 0x0032
#define QMI_IPA_SET_DATA_USAGE_QUOTA_RESP_V01 0x0032
#define QMI_IPA_DATA_USAGE_QUOTA_REACHED_IND_V01 0x0033
#define QMI_IPA_STOP_DATA_USAGE_QUOTA_REQ_V01 0x0034
#define QMI_IPA_STOP_DATA_USAGE_QUOTA_RESP_V01 0x0034
#define QMI_IPA_INIT_MODEM_DRIVER_CMPLT_REQ_V01 0x0035
#define QMI_IPA_INIT_MODEM_DRIVER_CMPLT_RESP_V01 0x0035
#define QMI_IPA_SET_DPL_PERIPHERAL_REQ_V01 0x0036
#define QMI_IPA_SET_DPL_PERIPHERAL_RESP_V01 0x0036
#define QMI_IPA_SET_DPL_PERIPHERAL_IND_V01 0x0036
#define QMI_IPA_INSTALL_FILTER_RULE_EX_REQ_V01 0x0037
#define QMI_IPA_INSTALL_FILTER_RULE_EX_RESP_V01 0x0037
#define QMI_IPA_ENABLE_PER_CLIENT_STATS_REQ_V01 0x0038
#define QMI_IPA_ENABLE_PER_CLIENT_STATS_RESP_V01 0x0038
#define QMI_IPA_GET_STATS_PER_CLIENT_REQ_V01 0x0039
#define QMI_IPA_GET_STATS_PER_CLIENT_RESP_V01 0x0039
#define QMI_IPA_CONFIGURE_UL_FIREWALL_RULES_REQ_V01 0x003A
#define QMI_IPA_CONFIGURE_UL_FIREWALL_RULES_RESP_V01 0x003A
#define QMI_IPA_CONFIGURE_UL_FIREWALL_RULES_IND_V01 0x003A
#define QMI_IPA_MHI_CLK_VOTE_REQ_V01 0x003B
#define QMI_IPA_MHI_CLK_VOTE_RESP_V01 0x003B
#define QMI_IPA_MHI_READY_IND_V01 0x003C
#define QMI_IPA_MHI_ALLOC_CHANNEL_REQ_V01 0x003D
#define QMI_IPA_MHI_ALLOC_CHANNEL_RESP_V01 0x003D
#define QMI_IPA_MHI_CLEANUP_REQ_V01 0x003E
#define QMI_IPA_MHI_CLEANUP_RESP_V01 0x003E
#define QMI_IPA_ENDP_DESC_INDICATION_V01 0x003F
#define QMI_IPA_MHI_PRIME_AGGR_INFO_REQ_V01 0x0040
#define QMI_IPA_MHI_PRIME_AGGR_INFO_RESP_V01 0x0040
#define QMI_IPA_ADD_OFFLOAD_CONNECTION_REQ_V01 0x0041
#define QMI_IPA_ADD_OFFLOAD_CONNECTION_RESP_V01 0x0041
#define QMI_IPA_REMOVE_OFFLOAD_CONNECTION_REQ_V01 0x0042
#define QMI_IPA_REMOVE_OFFLOAD_CONNECTION_RESP_V01 0x0042
#define QMI_IPA_BW_CHANGE_REQ_V01 0x0043
#define QMI_IPA_BW_CHANGE_RESP_V01 0x0043
#define QMI_IPA_BW_CHANGE_INDICATION_V01 0x0044
#define QMI_IPA_LOW_LATENCY_TRAFFIC_INFO_REQ_V01 0x0045
#define QMI_IPA_LOW_LATENCY_TRAFFIC_INFO_RESP_V01 0x0045
#define QMI_IPA_MOVE_NAT_REQ_V01 0x0046
#define QMI_IPA_MOVE_NAT_RESP_V01 0x0046
#define QMI_IPA_NAT_TABLE_MOVE_COMPLT_IND_V01 0x0046
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro ipa_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type ipa_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define ipa_get_service_object_v01( ) \
          ipa_get_service_object_internal_v01( \
            IPA_V01_IDL_MAJOR_VERS, IPA_V01_IDL_MINOR_VERS, \
            IPA_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

