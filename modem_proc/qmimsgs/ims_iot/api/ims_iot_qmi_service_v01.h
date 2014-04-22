#ifndef IMS_IOT_SERVICE_01_H
#define IMS_IOT_SERVICE_01_H
/**
  @file ims_iot_qmi_service_v01.h

  @brief This is the public header file which defines the ims_iot service Data structures.

  This header file defines the types and structures that were defined in
  ims_iot. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.mpss/4.3.4/ims_iot/api/ims_iot_qmi_service_v01.h#3 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Tue Jun 23 2020 (Spin 0)
   From IDL File: ims_iot_qmi_service_v01.idl */

/** @defgroup ims_iot_qmi_consts Constant values defined in the IDL */
/** @defgroup ims_iot_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup ims_iot_qmi_enums Enumerated types used in QMI messages */
/** @defgroup ims_iot_qmi_messages Structures sent as QMI messages */
/** @defgroup ims_iot_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup ims_iot_qmi_accessor Accessor for QMI service object */
/** @defgroup ims_iot_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ims_iot_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define IMS_IOT_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define IMS_IOT_V01_IDL_MINOR_VERS 0x03
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define IMS_IOT_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define IMS_IOT_V01_MAX_MESSAGE_ID 0x003C
/**
    @}
  */


/** @addtogroup ims_iot_qmi_consts
    @{
  */
#define IMS_IOT_USER_AGENT_LEN_V01 128
#define IMS_IOT_PCSCF_ADDR_LEN_V01 256
#define IMS_IOT_SMS_PSI_LEN_V01 128
#define IMS_IOT_REGISTRATION_FAILURE_LEN_V01 255
#define IMS_IOT_URI_LEN_V01 255
#define IMS_IOT_MAX_URI_LIST_V01 5
#define IMS_IOT_PDN_FAILURE_LEN_V01 128
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Registers for indications triggered from the service. */
typedef struct {

  /* Optional */
  /*  Configuration Change Indication Flag */
  uint8_t config_change_ind_valid;  /**< Must be set to true if config_change_ind is being passed */
  uint8_t config_change_ind;
  /**<   Flag to register for configuration change indications.
       Clients should register for indications to receive them in case of any changes.
       Default value is FALSE for all new clients. Values: \n
        - TRUE  -- Registers configuration change indications \n
        - FALSE -- Deregisters configuration change indications
  */

  /* Optional */
  /*  Service Status Change Indication Flag */
  uint8_t srv_status_change_ind_valid;  /**< Must be set to true if srv_status_change_ind is being passed */
  uint8_t srv_status_change_ind;
  /**<    Flag to register for service status change indications.
        Clients should register for indication to receive them in case of any changes.
        Default value is FALSE for all new clients. Values: \n
        - TRUE  -- Registers for service change indications \n
        - FALSE -- Deregisters service change indications
  */
}qmi_ims_iot_indication_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Registers for indications triggered from the service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}qmi_ims_iot_indication_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Fetches IMS framework configurations from the modem. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_ims_iot_get_fw_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Fetches IMS framework configurations from the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  User Agent String */
  uint8_t User_Agent_valid;  /**< Must be set to true if User_Agent is being passed */
  char User_Agent[IMS_IOT_USER_AGENT_LEN_V01 + 1];
  /**<   String that is used in SIP headers.
    */
}qmi_ims_iot_get_fw_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Updates IMS framework configurations at the modem
     */
typedef struct {

  /* Optional */
  /*  User Agent String */
  uint8_t User_Agent_valid;  /**< Must be set to true if User_Agent is being passed */
  char User_Agent[IMS_IOT_USER_AGENT_LEN_V01 + 1];
  /**<   String that is used in SIP headers.
    */
}qmi_ims_iot_set_fw_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Updates IMS framework configurations at the modem
     */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}qmi_ims_iot_set_fw_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Indication Message; Indicates when framework configurations change. */
typedef struct {

  /* Optional */
  /*  User Agent String */
  uint8_t User_Agent_valid;  /**< Must be set to true if User_Agent is being passed */
  char User_Agent[IMS_IOT_USER_AGENT_LEN_V01 + 1];
  /**<   String that is used in SIP headers.
    */
}qmi_ims_iot_fw_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Fetches IMS SIP configurations from the modem. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_ims_iot_get_sip_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Fetches IMS SIP configurations from the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  SIP Timer T1 */
  uint8_t SIP_Timer_T1_valid;  /**< Must be set to true if SIP_Timer_T1 is being passed */
  uint32_t SIP_Timer_T1;
  /**<    Timer T1 has a valid range from 500 to 3000 milliseconds.
    */

  /* Optional */
  /*  SIP Timer T2 */
  uint8_t SIP_Timer_T2_valid;  /**< Must be set to true if SIP_Timer_T2 is being passed */
  uint32_t SIP_Timer_T2;
  /**<    Timer T2 has a valid range from 4 to 16 seconds.
    */

  /* Optional */
  /*  SIP Timer T4  */
  uint8_t SIP_Timer_T4_valid;  /**< Must be set to true if SIP_Timer_T4 is being passed */
  uint32_t SIP_Timer_T4;
  /**<    Timer T4 has a valid range from 5 to 17 seconds.
    */

  /* Optional */
  /*  SIP Timer TA  */
  uint8_t SIP_Timer_TA_valid;  /**< Must be set to true if SIP_Timer_TA is being passed */
  uint32_t SIP_Timer_TA;
  /**<    If the value (in milliseconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TB  */
  uint8_t SIP_Timer_TB_valid;  /**< Must be set to true if SIP_Timer_TB is being passed */
  uint32_t SIP_Timer_TB;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TC */
  uint8_t SIP_Timer_TC_valid;  /**< Must be set to true if SIP_Timer_TC is being passed */
  uint32_t SIP_Timer_TC;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TD  */
  uint8_t SIP_Timer_TD_valid;  /**< Must be set to true if SIP_Timer_TD is being passed */
  uint32_t SIP_Timer_TD;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TE  */
  uint8_t SIP_Timer_TE_valid;  /**< Must be set to true if SIP_Timer_TE is being passed */
  uint32_t SIP_Timer_TE;
  /**<    If the value (in milliseconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TF  */
  uint8_t SIP_Timer_TF_valid;  /**< Must be set to true if SIP_Timer_TF is being passed */
  uint32_t SIP_Timer_TF;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TG */
  uint8_t SIP_Timer_TG_valid;  /**< Must be set to true if SIP_Timer_TG is being passed */
  uint32_t SIP_Timer_TG;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TI */
  uint8_t SIP_Timer_TI_valid;  /**< Must be set to true if SIP_Timer_TI is being passed */
  uint32_t SIP_Timer_TI;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TJ  */
  uint8_t SIP_Timer_TJ_valid;  /**< Must be set to true if SIP_Timer_TJ is being passed */
  uint32_t SIP_Timer_TJ;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TK  */
  uint8_t SIP_Timer_TK_valid;  /**< Must be set to true if SIP_Timer_TK is being passed */
  uint32_t SIP_Timer_TK;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer Response Retry  */
  uint8_t SIP_Timer_RelResponse_Retry_valid;  /**< Must be set to true if SIP_Timer_RelResponse_Retry is being passed */
  uint32_t SIP_Timer_RelResponse_Retry;
  /**<    Timer to retransmit invite the provisional response.
          There is no default value for this TLV.
    */

  /* Optional */
  /*  SIP Stop Timer B On 1xx */
  uint8_t SIP_Stop_TimerB_On1xx_valid;  /**< Must be set to true if SIP_Stop_TimerB_On1xx is being passed */
  uint32_t SIP_Stop_TimerB_On1xx;
  /**<    Used as a toggle to stop timer B on 1xx reception.
          Default value is 0.
    */

  /* Optional */
  /*  SIP Listening Port */
  uint8_t SIP_Listening_Port_valid;  /**< Must be set to true if SIP_Listening_Port is being passed */
  uint32_t SIP_Listening_Port;
  /**<    Sets the UE server port number. Default value is 5060.
    */

  /* Optional */
  /*  SIP TCP Maximum Backoff Timer */
  uint8_t SIP_TCP_MaxBackOff_Timer_valid;  /**< Must be set to true if SIP_TCP_MaxBackOff_Timer is being passed */
  uint32_t SIP_TCP_MaxBackOff_Timer;
  /**<    Sets the TCP maximum backoff timer. There is no default value for this TLV.
    */

  /* Optional */
  /*  SIP Enable TCP Threshold  */
  uint8_t SIP_Enable_TCP_Threshold_valid;  /**< Must be set to true if SIP_Enable_TCP_Threshold is being passed */
  uint32_t SIP_Enable_TCP_Threshold;
  /**<   Enables TCP threshold.
     Values: \n
       - 1  -- Enable \n
       - 0  -- Disable
    */

  /* Optional */
  /*  SIP TCP Threshold */
  uint8_t SIP_TCP_Threshold_valid;  /**< Must be set to true if SIP_TCP_Threshold is being passed */
  uint32_t SIP_TCP_Threshold;
  /**<    If TCP threshold value is set to 0, TCP transport is disabled.
          Then a nonzero value is used as MSS.
    */

  /* Optional */
  /*  SIP Enable Transport Switch */
  uint8_t SIP_Enable_Transport_Switch_valid;  /**< Must be set to true if SIP_Enable_Transport_Switch is being passed */
  uint32_t SIP_Enable_Transport_Switch;
  /**<   Enables the transport switch from TCP to UDP.
     Values: \n
       - 1  -- Enable \n
       - 0  -- Disable
    */

  /* Optional */
  /*  SIP IPSec Integrity Scheme  */
  uint8_t SIP_IPSec_Int_Scheme_valid;  /**< Must be set to true if SIP_IPSec_Int_Scheme is being passed */
  uint32_t SIP_IPSec_Int_Scheme;
  /**<   Sets the IPSec integrity algorithm. 
         Values: \n
         - 1 -- hmac-sha-1-96 \n
         - 2 -- hmac-md5-96	\n		
         - 3 -- hmac-sha-1-96 and hmac-md5-96			
    */

  /* Optional */
  /*  SIP IPSec Encoder Algorithm */
  uint8_t SIP_IPSec_Enc_Algo_valid;  /**< Must be set to true if SIP_IPSec_Enc_Algo is being passed */
  uint32_t SIP_IPSec_Enc_Algo;
  /**<   Sets the IPSec encryption algorithm bitmask. 
         Values: \n
         - 1 -- NULL \n
         - 2 -- aes-cbc \n
         - 3 -- NULL and aes-cbc \n
         - 4 -- des-ede3-cbc \n
         - 5 -- NULL and des-ede3-cbc \n
         - 6 -- aes-cbc and des-ede3-cbc \n
         - 7 -- NULL, aes-cbc and des-ede3-cbc
    */

  /* Optional */
  /*  SIP Enable Compact Header */
  uint8_t SIP_Enable_Compact_Header_valid;  /**< Must be set to true if SIP_Enable_Compact_Header is being passed */
  uint32_t SIP_Enable_Compact_Header;
  /**<   Enables the compact header in SIP.
     Values: \n
       - 1  -- Enable \n
       - 0  -- Disable
    */

  /* Optional */
  /*  SIP Enable SIP Instance  */
  uint8_t SIP_Enable_Sip_Instance_valid;  /**< Must be set to true if SIP_Enable_Sip_Instance is being passed */
  uint32_t SIP_Enable_Sip_Instance;
  /**<   Enables the SIP instance in the contact header.
     Values: \n
       - 1  -- Enable \n
       - 0  -- Disable
    */

  /* Optional */
  /*  SIP Enable Route Header  */
  uint8_t SIP_Enable_Route_Header_valid;  /**< Must be set to true if SIP_Enable_Route_Header is being passed */
  uint32_t SIP_Enable_Route_Header;
  /**<   Enables route header.
     Values: \n
       - 1  -- Enable \n
       - 0  -- Disable
    */

  /* Optional */
  /*  SIP Enable IMSI In Contact Header */
  uint8_t SIP_Enable_Imsi_In_Contact_valid;  /**< Must be set to true if SIP_Enable_Imsi_In_Contact is being passed */
  uint32_t SIP_Enable_Imsi_In_Contact;
  /**<   Enables IMSI in contact header.
     Values: \n
       - 1  -- Enable \n
       - 0  -- Disable
    */

  /* Optional */
  /*  SIP Enable Keep Alive */
  uint8_t SIP_Enable_Keep_Alive_valid;  /**< Must be set to true if SIP_Enable_Keep_Alive is being passed */
  uint32_t SIP_Enable_Keep_Alive;
  /**<   Enables keep alive packet.
     Values: \n
       - 1  -- Enable \n
       - 0  -- Disable
    */
}qmi_ims_iot_get_sip_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Sets IMS SIP configurations at the modem. */
typedef struct {

  /* Optional */
  /*  SIP Timer T1 */
  uint8_t SIP_Timer_T1_valid;  /**< Must be set to true if SIP_Timer_T1 is being passed */
  uint32_t SIP_Timer_T1;
  /**<    Timer T1 has a valid range from 500 to 3000 milliseconds.
    */

  /* Optional */
  /*  SIP Timer T2 */
  uint8_t SIP_Timer_T2_valid;  /**< Must be set to true if SIP_Timer_T2 is being passed */
  uint32_t SIP_Timer_T2;
  /**<    Timer T2 has a valid range from 4 to 16 seconds.
    */

  /* Optional */
  /*  SIP Timer T4 */
  uint8_t SIP_Timer_T4_valid;  /**< Must be set to true if SIP_Timer_T4 is being passed */
  uint32_t SIP_Timer_T4;
  /**<    Timer T4 has a valid range from 5 to 17 seconds.
    */

  /* Optional */
  /*  SIP Timer TA */
  uint8_t SIP_Timer_TA_valid;  /**< Must be set to true if SIP_Timer_TA is being passed */
  uint32_t SIP_Timer_TA;
  /**<    If the value (in milliseconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TB */
  uint8_t SIP_Timer_TB_valid;  /**< Must be set to true if SIP_Timer_TB is being passed */
  uint32_t SIP_Timer_TB;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TC  */
  uint8_t SIP_Timer_TC_valid;  /**< Must be set to true if SIP_Timer_TC is being passed */
  uint32_t SIP_Timer_TC;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TD */
  uint8_t SIP_Timer_TD_valid;  /**< Must be set to true if SIP_Timer_TD is being passed */
  uint32_t SIP_Timer_TD;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TE  */
  uint8_t SIP_Timer_TE_valid;  /**< Must be set to true if SIP_Timer_TE is being passed */
  uint32_t SIP_Timer_TE;
  /**<    If the value (in milliseconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TF  */
  uint8_t SIP_Timer_TF_valid;  /**< Must be set to true if SIP_Timer_TF is being passed */
  uint32_t SIP_Timer_TF;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TG */
  uint8_t SIP_Timer_TG_valid;  /**< Must be set to true if SIP_Timer_TG is being passed */
  uint32_t SIP_Timer_TG;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TI */
  uint8_t SIP_Timer_TI_valid;  /**< Must be set to true if SIP_Timer_TI is being passed */
  uint32_t SIP_Timer_TI;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TJ */
  uint8_t SIP_Timer_TJ_valid;  /**< Must be set to true if SIP_Timer_TJ is being passed */
  uint32_t SIP_Timer_TJ;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TK */
  uint8_t SIP_Timer_TK_valid;  /**< Must be set to true if SIP_Timer_TK is being passed */
  uint32_t SIP_Timer_TK;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer Response Retry  */
  uint8_t SIP_Timer_RelResponse_Retry_valid;  /**< Must be set to true if SIP_Timer_RelResponse_Retry is being passed */
  uint32_t SIP_Timer_RelResponse_Retry;
  /**<    Timer to retransmit invite the provisional response.
          There is no default value for this TLV.
    */

  /* Optional */
  /*  SIP Stop Timer B On 1xx */
  uint8_t SIP_Stop_TimerB_On1xx_valid;  /**< Must be set to true if SIP_Stop_TimerB_On1xx is being passed */
  uint32_t SIP_Stop_TimerB_On1xx;
  /**<    Used as a toggle to stop timer B on 1xx reception.
          Default value is 0
    */

  /* Optional */
  /*  SIP Listening Port */
  uint8_t SIP_Listening_Port_valid;  /**< Must be set to true if SIP_Listening_Port is being passed */
  uint32_t SIP_Listening_Port;
  /**<    Sets the UE server port number. Default value is 5060.
    */

  /* Optional */
  /*  SIP TCP Maximum Backoff Timer */
  uint8_t SIP_TCP_MaxBackOff_Timer_valid;  /**< Must be set to true if SIP_TCP_MaxBackOff_Timer is being passed */
  uint32_t SIP_TCP_MaxBackOff_Timer;
  /**<    Sets the TCP maximum backoff timer. There is no default value for this TLV.
    */

  /* Optional */
  /*  SIP Enable TCP Threshold  */
  uint8_t SIP_Enable_TCP_Threshold_valid;  /**< Must be set to true if SIP_Enable_TCP_Threshold is being passed */
  uint32_t SIP_Enable_TCP_Threshold;
  /**<   Enables TCP threshold.
     Values: \n
       - 1  -- Enable \n
       - 0  -- Disable \n
       Only the above values are supported. Any other value defaults to enabled.
    */

  /* Optional */
  /*  SIP TCP Threshold */
  uint8_t SIP_TCP_Threshold_valid;  /**< Must be set to true if SIP_TCP_Threshold is being passed */
  uint32_t SIP_TCP_Threshold;
  /**<    If TCP threshold value is set to 0, TCP transport is disabled.
          Then a nonzero value is used as MSS.
    */

  /* Optional */
  /*  SIP Enable Transport Switch */
  uint8_t SIP_Enable_Transport_Switch_valid;  /**< Must be set to true if SIP_Enable_Transport_Switch is being passed */
  uint32_t SIP_Enable_Transport_Switch;
  /**<   Enables the transport switch from TCP to UDP.
     Values: \n
       - 1  -- Enable \n
       - 0  -- Disable \n
       Only the above values are supported. Any other value defaults to enabled.
    */

  /* Optional */
  /*  SIP IPSec Integrity Scheme  */
  uint8_t SIP_IPSec_Int_Scheme_valid;  /**< Must be set to true if SIP_IPSec_Int_Scheme is being passed */
  uint32_t SIP_IPSec_Int_Scheme;
  /**<   Sets the SIP IPSec integrity algorithm. 
         Values: \n
         - 1 -- hmac-sha-1-96 \n
         - 2 -- hmac-md5-96 \n			
         - 3 -- hmac-sha-1-96 and hmac-md5-96			
    */

  /* Optional */
  /*  SIP IPSec Encoder Algotithm */
  uint8_t SIP_IPSec_Enc_Algo_valid;  /**< Must be set to true if SIP_IPSec_Enc_Algo is being passed */
  uint32_t SIP_IPSec_Enc_Algo;
  /**<   Sets the IPSec encryption algorithm bitmask. 
         Values: \n
         - 1 -- NULL  \n
         - 2 -- aes-cbc  \n
         - 3 -- NULL and aes-cbc  \n
         - 4 -- des-ede3-cbc  \n
         - 5 -- NULL and des-ede3-cbc  \n
         - 6 -- aes-cbc and des-ede3-cbc  \n
         - 7 -- NULL, aes-cbc and des-ede3-cbc
    */

  /* Optional */
  /*  SIP Enable Compact Header */
  uint8_t SIP_Enable_Compact_Header_valid;  /**< Must be set to true if SIP_Enable_Compact_Header is being passed */
  uint32_t SIP_Enable_Compact_Header;
  /**<   Enables the compact header in SIP. 
	 Values: \n
       - 1  -- Enable \n
       - 0  -- Disable \n
       Only the above values are supported. Any other value defaults to enabled.
    */

  /* Optional */
  /*  SIP Enable SIP Instance  */
  uint8_t SIP_Enable_Sip_Instance_valid;  /**< Must be set to true if SIP_Enable_Sip_Instance is being passed */
  uint32_t SIP_Enable_Sip_Instance;
  /**<   Enables the SIP instance in the contact header.
     Values: \n
       - 1  -- Enable \n
       - 0  -- Disable \n
       Only the above values are supported. Any other value defaults to enabled.
    */

  /* Optional */
  /*  SIP Enable Route Header  */
  uint8_t SIP_Enable_Route_Header_valid;  /**< Must be set to true if SIP_Enable_Route_Header is being passed */
  uint32_t SIP_Enable_Route_Header;
  /**<   Enables route header.
     Values: \n
       - 1  -- Enable \n
       - 0  -- Disable \n
       Only the above values are supported. Any other value defaults to enabled.
    */

  /* Optional */
  /*  SIP Enable IMSI In Contact Header */
  uint8_t SIP_Enable_Imsi_In_Contact_valid;  /**< Must be set to true if SIP_Enable_Imsi_In_Contact is being passed */
  uint32_t SIP_Enable_Imsi_In_Contact;
  /**<   Enables IMSI in contact header.
     Values: \n
       - 1 -- Enable \n
       - 0 -- Disable \n
       Only the above values are supported. Any other value defaults to enabled.
    */

  /* Optional */
  /*  SIP Enable Keep Alive */
  uint8_t SIP_Enable_Keep_Alive_valid;  /**< Must be set to true if SIP_Enable_Keep_Alive is being passed */
  uint32_t SIP_Enable_Keep_Alive;
  /**<   Enables keep alive packet.
     Values: \n
       - 1  -- Enable \n
       - 0  -- Disable \n
       Only the above values are supported. Any other value defaults to enabled.
    */
}qmi_ims_iot_set_sip_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Sets IMS SIP configurations at the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}qmi_ims_iot_set_sip_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Indication Message; Indicates when SIP configurations change. */
typedef struct {

  /* Optional */
  /*  SIP Timer T1 */
  uint8_t SIP_Timer_T1_valid;  /**< Must be set to true if SIP_Timer_T1 is being passed */
  uint32_t SIP_Timer_T1;
  /**<    Timer T1 has a valid range from 500 to 3000 milliseconds.
    */

  /* Optional */
  /*  SIP Timer T2 */
  uint8_t SIP_Timer_T2_valid;  /**< Must be set to true if SIP_Timer_T2 is being passed */
  uint32_t SIP_Timer_T2;
  /**<    Timer T2 has a valid range from 4 to 16 seconds.
    */

  /* Optional */
  /*  SIP Timer T4  */
  uint8_t SIP_Timer_T4_valid;  /**< Must be set to true if SIP_Timer_T4 is being passed */
  uint32_t SIP_Timer_T4;
  /**<    Timer T4 has a valid range from 5 to 17 seconds.
    */

  /* Optional */
  /*  SIP Timer TA  */
  uint8_t SIP_Timer_TA_valid;  /**< Must be set to true if SIP_Timer_TA is being passed */
  uint32_t SIP_Timer_TA;
  /**<    If the value (in milliseconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TB */
  uint8_t SIP_Timer_TB_valid;  /**< Must be set to true if SIP_Timer_TB is being passed */
  uint32_t SIP_Timer_TB;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TC */
  uint8_t SIP_Timer_TC_valid;  /**< Must be set to true if SIP_Timer_TC is being passed */
  uint32_t SIP_Timer_TC;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TD  */
  uint8_t SIP_Timer_TD_valid;  /**< Must be set to true if SIP_Timer_TD is being passed */
  uint32_t SIP_Timer_TD;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TE  */
  uint8_t SIP_Timer_TE_valid;  /**< Must be set to true if SIP_Timer_TE is being passed */
  uint32_t SIP_Timer_TE;
  /**<    If the value (in milliseconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TF  */
  uint8_t SIP_Timer_TF_valid;  /**< Must be set to true if SIP_Timer_TF is being passed */
  uint32_t SIP_Timer_TF;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TG */
  uint8_t SIP_Timer_TG_valid;  /**< Must be set to true if SIP_Timer_TG is being passed */
  uint32_t SIP_Timer_TG;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TI */
  uint8_t SIP_Timer_TI_valid;  /**< Must be set to true if SIP_Timer_TI is being passed */
  uint32_t SIP_Timer_TI;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TJ */
  uint8_t SIP_Timer_TJ_valid;  /**< Must be set to true if SIP_Timer_TJ is being passed */
  uint32_t SIP_Timer_TJ;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer TK */
  uint8_t SIP_Timer_TK_valid;  /**< Must be set to true if SIP_Timer_TK is being passed */
  uint32_t SIP_Timer_TK;
  /**<    If the value (in seconds) is not set, the default timer value 
		is set by RFC.
    */

  /* Optional */
  /*  SIP Timer Response Retry  */
  uint8_t SIP_Timer_RelResponse_Retry_valid;  /**< Must be set to true if SIP_Timer_RelResponse_Retry is being passed */
  uint32_t SIP_Timer_RelResponse_Retry;
  /**<    Timer to retransmit invite the provisional response.
          There is no default value for this TLV.
    */

  /* Optional */
  /*  SIP Stop Timer B On 1xx */
  uint8_t SIP_Stop_TimerB_On1xx_valid;  /**< Must be set to true if SIP_Stop_TimerB_On1xx is being passed */
  uint32_t SIP_Stop_TimerB_On1xx;
  /**<    Used as a toggle to stop timer B on 1xx reception.
          Default value is 0.
    */

  /* Optional */
  /*  SIP Listening Port */
  uint8_t SIP_Listening_Port_valid;  /**< Must be set to true if SIP_Listening_Port is being passed */
  uint32_t SIP_Listening_Port;
  /**<    Sets the UE server port number. Default value is 5060.
    */

  /* Optional */
  /*  SIP TCP Maximum Backoff Timer */
  uint8_t SIP_TCP_MaxBackOff_Timer_valid;  /**< Must be set to true if SIP_TCP_MaxBackOff_Timer is being passed */
  uint32_t SIP_TCP_MaxBackOff_Timer;
  /**<    Sets the TCP maximum backoff timer. There is no default value for this TLV.
    */

  /* Optional */
  /*  SIP Enable TCP Threshold  */
  uint8_t SIP_Enable_TCP_Threshold_valid;  /**< Must be set to true if SIP_Enable_TCP_Threshold is being passed */
  uint32_t SIP_Enable_TCP_Threshold;
  /**<   Enables TCP threshold.
     Values: \n
       - 1  -- Enable \n
       - 0  -- Disable
    */

  /* Optional */
  /*  SIP TCP Threshold */
  uint8_t SIP_TCP_Threshold_valid;  /**< Must be set to true if SIP_TCP_Threshold is being passed */
  uint32_t SIP_TCP_Threshold;
  /**<    If TCP threshold value is set to 0, TCP transport is disabled.
          Then a nonzero value is used as MSS.
    */

  /* Optional */
  /*  SIP Enable Transport Switch */
  uint8_t SIP_Enable_Transport_Switch_valid;  /**< Must be set to true if SIP_Enable_Transport_Switch is being passed */
  uint32_t SIP_Enable_Transport_Switch;
  /**<   Enables the transport switch from TCP to UDP.
     Values: \n
       - 1  -- Enable \n
       - 0  -- Disable
    */

  /* Optional */
  /*  SIP IPSec Integrity Scheme  */
  uint8_t SIP_IPSec_Int_Scheme_valid;  /**< Must be set to true if SIP_IPSec_Int_Scheme is being passed */
  uint32_t SIP_IPSec_Int_Scheme;
  /**<   Sets the IPSec integrity algorithm. 
         Values: \n
         - 1 -- hmac-sha-1-96 \n
         - 2 -- hmac-md5-96 \n			
         - 3 -- hmac-sha-1-96 and hmac-md5-96			
    */

  /* Optional */
  /*  SIP IPSec Encoder Algorithm  */
  uint8_t SIP_IPSec_Enc_Algo_valid;  /**< Must be set to true if SIP_IPSec_Enc_Algo is being passed */
  uint32_t SIP_IPSec_Enc_Algo;
  /**<   Sets the IPSec encryption algorithm bitmask. 
         Values: \n
         - 1 -- NULL \n
         - 2 -- aes-cbc \n
         - 3 -- NULL and aes-cbc \n
         - 4 -- des-ede3-cbc \n
         - 5 -- NULL and des-ede3-cbc \n
         - 6 -- aes-cbc and des-ede3-cbc \n
         - 7 -- NULL, aes-cbc and des-ede3-cbc
    */

  /* Optional */
  /*  SIP Enable Compact Header */
  uint8_t SIP_Enable_Compact_Header_valid;  /**< Must be set to true if SIP_Enable_Compact_Header is being passed */
  uint32_t SIP_Enable_Compact_Header;
  /**<   Enables the compact header in SIP. Values: \n
       - 1  -- Enable \n
       - 0  -- Disable
    */

  /* Optional */
  /*  SIP Enable SIP Instance  */
  uint8_t SIP_Enable_Sip_Instance_valid;  /**< Must be set to true if SIP_Enable_Sip_Instance is being passed */
  uint32_t SIP_Enable_Sip_Instance;
  /**<   Enables the SIP instance in the contact header.
     Values: \n
       - 1  -- Enable \n
       - 0  -- Disable
    */

  /* Optional */
  /*  SIP Enable Route Header  */
  uint8_t SIP_Enable_Route_Header_valid;  /**< Must be set to true if SIP_Enable_Route_Header is being passed */
  uint32_t SIP_Enable_Route_Header;
  /**<   Enables route header.
     Values: \n
       - 1  -- Enable \n
       - 0  -- Disable
    */

  /* Optional */
  /*  SIP Enable IMSI In Contact Header */
  uint8_t SIP_Enable_Imsi_In_Contact_valid;  /**< Must be set to true if SIP_Enable_Imsi_In_Contact is being passed */
  uint32_t SIP_Enable_Imsi_In_Contact;
  /**<   Enables IMSI in contact header.
     Values: \n
       - 1  -- Enable \n
       - 0  -- Disable
    */

  /* Optional */
  /*  SIP Enable Keep Alive */
  uint8_t SIP_Enable_Keep_Alive_valid;  /**< Must be set to true if SIP_Enable_Keep_Alive is being passed */
  uint32_t SIP_Enable_Keep_Alive;
  /**<   Enables keep alive packet.
     Values: \n
       - 1  -- Enable \n
       - 0  -- Disable
    */
}qmi_ims_iot_sip_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Fetches IMS register manager configurations from the modem 
			(service). */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_ims_iot_get_reg_mgr_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Fetches IMS register manager configurations from the modem 
			(service). */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Authentication Registration Mode */
  uint8_t Auth_Reg_mode_valid;  /**< Must be set to true if Auth_Reg_mode is being passed */
  uint32_t Auth_Reg_mode;
  /**<    Enables protected registration mode.
          Only the following values are supported: \n
          - 1 -- IPSec Enabled mode \n
          - 2 -- Unprotected mode
    */

  /* Optional */
  /*  Registration Retry Base Time */
  uint8_t Reg_Retry_Base_Time_valid;  /**< Must be set to true if Reg_Retry_Base_Time is being passed */
  uint32_t Reg_Retry_Base_Time;
  /**<     Sets registration retry base time in seconds.
    */

  /* Optional */
  /*  Registration Retry Maximum Time */
  uint8_t Reg_Retry_Max_Time_valid;  /**< Must be set to true if Reg_Retry_Max_Time is being passed */
  uint32_t Reg_Retry_Max_Time;
  /**<     Sets registration retry maximum time in seconds.
    */

  /* Optional */
  /*  PCSCF Server Address  */
  uint8_t PCSCF_Server_Address_valid;  /**< Must be set to true if PCSCF_Server_Address is being passed */
  char PCSCF_Server_Address[IMS_IOT_PCSCF_ADDR_LEN_V01 + 1];
  /**<     Sets registration preconfigured server base address.
           If not set, the default is NULL.
    */

  /* Optional */
  /*  PSCSF Server Port  */
  uint8_t PCSCF_Server_Port_valid;  /**< Must be set to true if PCSCF_Server_Port is being passed */
  uint32_t PCSCF_Server_Port;
  /**<     Sets the PSCSF port. If not set, the default is 5060.
    */
}qmi_ims_iot_get_reg_mgr_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Sets IMS register manager configurations at the modem (service). */
typedef struct {

  /* Optional */
  /*  Authentication Registration Mode */
  uint8_t Auth_Reg_mode_valid;  /**< Must be set to true if Auth_Reg_mode is being passed */
  uint32_t Auth_Reg_mode;
  /**<    Enables protected registration mode.
          Only the following values are supported: \n
          - 1 -- IPSec Enabled mode \n
          - 2 -- Unprotected mode
    */

  /* Optional */
  /*  Registration Retry Base Time */
  uint8_t Reg_Retry_Base_Time_valid;  /**< Must be set to true if Reg_Retry_Base_Time is being passed */
  uint32_t Reg_Retry_Base_Time;
  /**<     Sets registration retry base time in seconds.
    */

  /* Optional */
  /*  Registration Retry Maximum Time */
  uint8_t Reg_Retry_Max_Time_valid;  /**< Must be set to true if Reg_Retry_Max_Time is being passed */
  uint32_t Reg_Retry_Max_Time;
  /**<     Sets registration retry maximum time in seconds.
    */

  /* Optional */
  /*  PCSCF Server Address  */
  uint8_t PCSCF_Server_Address_valid;  /**< Must be set to true if PCSCF_Server_Address is being passed */
  char PCSCF_Server_Address[IMS_IOT_PCSCF_ADDR_LEN_V01 + 1];
  /**<     Sets registration preconfigured server base address.
           If not set, the default is NULL.
    */

  /* Optional */
  /*  PSCSF Server Port  */
  uint8_t PCSCF_Server_Port_valid;  /**< Must be set to true if PCSCF_Server_Port is being passed */
  uint32_t PCSCF_Server_Port;
  /**<     Sets the PSCSF port. If not set, the default is 5060.
    */
}qmi_ims_iot_set_reg_mgr_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Sets IMS register manager configurations at the modem (service). */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}qmi_ims_iot_set_reg_mgr_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Indication Message; Indicates when the register manager configurations change. */
typedef struct {

  /* Optional */
  /*  Authentication Registration Mode */
  uint8_t Auth_Reg_mode_valid;  /**< Must be set to true if Auth_Reg_mode is being passed */
  uint32_t Auth_Reg_mode;
  /**<    Enables protected registration mode.
          Only the following values are supported: \n
          - 1 -- IPSec Enabled mode \n
          - 2 -- Unprotected mode
    */

  /* Optional */
  /*  Registration Retry Base Time */
  uint8_t Reg_Retry_Base_Time_valid;  /**< Must be set to true if Reg_Retry_Base_Time is being passed */
  uint32_t Reg_Retry_Base_Time;
  /**<     Sets registration retry base time in seconds.
    */

  /* Optional */
  /*  Registration Retry Maximum Time */
  uint8_t Reg_Retry_Max_Time_valid;  /**< Must be set to true if Reg_Retry_Max_Time is being passed */
  uint32_t Reg_Retry_Max_Time;
  /**<     Sets registration retry maximum time in seconds.
    */

  /* Optional */
  /*  PCSCF Server Address  */
  uint8_t PCSCF_Server_Address_valid;  /**< Must be set to true if PCSCF_Server_Address is being passed */
  char PCSCF_Server_Address[IMS_IOT_PCSCF_ADDR_LEN_V01 + 1];
  /**<     Sts registration preconfigured server base address.
           If not set, the default is NULL.
    */

  /* Optional */
  /*  PSCSF Server Port  */
  uint8_t PCSCF_Server_Port_valid;  /**< Must be set to true if PCSCF_Server_Port is being passed */
  uint32_t PCSCF_Server_Port;
  /**<     Sets the PSCSF port. If not set, the default is 5060.
    */
}qmi_ims_iot_reg_mgr_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Fetches IMS service status configurations from the modem 
			(service). */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_ims_iot_get_service_status_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Fetches IMS service status configurations from the modem 
			(service). */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  VoLTE Enable */
  uint8_t VOLTE_Enable_valid;  /**< Must be set to true if VOLTE_Enable is being passed */
  uint32_t VOLTE_Enable;
  /**<    Enables the VoLTE feature.
          Values: \n
          - 1 -- Enable \n 
          - 0 -- Disable 
    */

  /* Optional */
  /*  SMS Over IP Enable  */
  uint8_t SMS_Over_IP_Enable_valid;  /**< Must be set to true if SMS_Over_IP_Enable is being passed */
  uint32_t SMS_Over_IP_Enable;
  /**<    Enables SMS over IP feature. 
          Values: \n
          - 2 -- Limited \n
		  - 1 -- Enable \n  
          - 0 -- Disable 
    */

  /* Optional */
  /*  USSD Enable */
  uint8_t USSD_Enable_valid;  /**< Must be set to true if USSD_Enable is being passed */
  uint32_t USSD_Enable;
  /**<    Enables USSD feature. 
          Values: \n
          - 1 -- Enable \n 
          - 0 -- Disable 
    */
}qmi_ims_iot_get_service_status_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Sets IMS service status configurations at the modem (service). */
typedef struct {

  /* Optional */
  /*  VoLTE Enable */
  uint8_t VOLTE_Enable_valid;  /**< Must be set to true if VOLTE_Enable is being passed */
  uint32_t VOLTE_Enable;
  /**<    Enables the VoLTE feature.
          Values: \n
          - 1 -- Enable \n
          - 0 -- Disable \n
          Only the above values are supported. Any other value defaults to enabled.
    */

  /* Optional */
  /*  SMS Over IP Enable  */
  uint8_t SMS_Over_IP_Enable_valid;  /**< Must be set to true if SMS_Over_IP_Enable is being passed */
  uint32_t SMS_Over_IP_Enable;
  /**<    Enables the SMS over IP feature.
          Values: \n
          - 2 -- Limited \n
		  - 1 -- Enable \n
          - 0 -- Disable \n
          Only the above values are supported. Any other value defaults to enabled.
    */

  /* Optional */
  /*  USSD Enable  */
  uint8_t USSD_Enable_valid;  /**< Must be set to true if USSD_Enable is being passed */
  uint32_t USSD_Enable;
  /**<    Enables the USSD feature.
          Values: \n
          - 1 -- Enable \n
          - 0 -- Disable \n
          Only the above values are supported. Any other value defaults to enabled.
    		*/
}qmi_ims_iot_set_service_status_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Sets IMS service status configurations at the modem (service). */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}qmi_ims_iot_set_service_status_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Indication Message; Indicates when IMS service status configurations change. */
typedef struct {

  /* Optional */
  /*  VoLTE Enable */
  uint8_t VOLTE_Enable_valid;  /**< Must be set to true if VOLTE_Enable is being passed */
  uint32_t VOLTE_Enable;
  /**<    Enables the VoLTE feature.
          Values: \n
          - 1 -- Enable \n
          - 0 -- Disable
    */

  /* Optional */
  /*  SMS Over IP Enable  */
  uint8_t SMS_Over_IP_Enable_valid;  /**< Must be set to true if SMS_Over_IP_Enable is being passed */
  uint32_t SMS_Over_IP_Enable;
  /**<    Enables the SMS over IP feature.
          Values: \n
          - 2 -- Limited \n
		  - 1 -- Enable \n
          - 0 -- Disable
    */

  /* Optional */
  /*  USSD Enable  */
  uint8_t USSD_Enable_valid;  /**< Must be set to true if USSD_Enable is being passed */
  uint32_t USSD_Enable;
  /**<    Enables the USSD feature.
          Values: \n
          - 1 -- Enable \n
          - 0 -- Disable
    	*/
}qmi_ims_iot_service_status_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Sets the APCS_COMPLETE status 
             for the requesting control point. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_ims_iot_set_apcs_config_complete_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Sets the APCS_COMPLETE status 
             for the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code     */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
         - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
         - qmi_error_type  -- Error code. Possible error code values are 
                              described in the error codes section of each 
                              message definition.
    */
}qmi_ims_iot_set_apcs_config_complete_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_enums
    @{
  */
typedef enum {
  IMS_IOT_SMS_FORMAT_E_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_IOT_SMS_FORMAT_MIN_V01 = 0, /**<  Invalid SMS format  */
  IMS_IOT_SMS_FORMAT_3GPP_V01 = 1, /**<  SMS format of type 3GPP  */
  IMS_IOT_SMS_FORMAT_3GPP2_V01 = 2, /**<  SMS format of type 3GPP2  */
  IMS_IOT_SMS_FORMAT_MAX_V01 = 3, /**<  Invalid SMS format  */
  IMS_IOT_SMS_FORMAT_E_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_iot_sms_format_e_type_v01;
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Fetches IMS SMS configurations from the modem. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_ims_iot_get_sms_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Fetches IMS SMS configurations from the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  SMS Format */
  uint8_t SMS_Format_valid;  /**< Must be set to true if SMS_Format is being passed */
  ims_iot_sms_format_e_type_v01 SMS_Format;
  /**<     Sets the SMS format to either 3gpp or 3gpp2. Only 3gpp is supported 
	  now, 3gpp2 will be supported in the future if there is a requirement.
    */

  /* Optional */
  /*  SMS PSI Value */
  uint8_t Psi_Value_valid;  /**< Must be set to true if Psi_Value is being passed */
  char Psi_Value[IMS_IOT_SMS_PSI_LEN_V01 + 1];
  /**<     Sets PSI string for SMS. If not set, the default is NULL.
    */
}qmi_ims_iot_get_sms_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Sets IMS SMS configuration at the modem. */
typedef struct {

  /* Optional */
  /*  SMS Format */
  uint8_t SMS_Format_valid;  /**< Must be set to true if SMS_Format is being passed */
  ims_iot_sms_format_e_type_v01 SMS_Format;
  /**<     Sets the SMS format to either 3gpp or 3gpp2. Only 3gpp is supported 
	  now, 3gpp2 will be supported in the future if there is a requirement.
    */

  /* Optional */
  /*  SMS PSI Value */
  uint8_t Psi_Value_valid;  /**< Must be set to true if Psi_Value is being passed */
  char Psi_Value[IMS_IOT_SMS_PSI_LEN_V01 + 1];
  /**<     Set PSI the string for SMS. If not set, the default is NULL.
    */
}qmi_ims_iot_set_sms_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Sets IMS SMS configuration at the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}qmi_ims_iot_set_sms_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Indication Message; Indicates when IMS SMS configurations change. */
typedef struct {

  /* Optional */
  /*  SMS Format */
  uint8_t SMS_Format_valid;  /**< Must be set to true if SMS_Format is being passed */
  ims_iot_sms_format_e_type_v01 SMS_Format;
  /**<     Sets the SMS format to either 3gpp or 3gpp2. Only 3gpp is supported 
	  now, 3gpp2 will be supported in the future if there is a requirement.
    */

  /* Optional */
  /*  SMS PSI Value */
  uint8_t Psi_Value_valid;  /**< Must be set to true if Psi_Value is being passed */
  char Psi_Value[IMS_IOT_SMS_PSI_LEN_V01 + 1];
  /**<     Sets PSI string for SMS. If not set, the default is NULL.
    */
}qmi_ims_iot_sms_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Fetches IMS voice or VoLTE configurations when there is a change 
	in IMS registration or PDN status on the modem. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_ims_iot_get_voice_config_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Fetches IMS voice or VoLTE configurations when there is a change 
	in IMS registration or PDN status on the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Display Text Priority */
  uint8_t Display_Text_Priority_valid;  /**< Must be set to true if Display_Text_Priority is being passed */
  uint32_t Display_Text_Priority;
  /**<   Sets originating party identity determination from PAI or from the 
		header for incoming SIP messages. Default value is 0. Values: \n
        - 0 --  PAI\n
        - 1 --  From\n
        - 2 --  Fallback-enabled
    */

  /* Optional */
  /*  Caller ID Mode */
  uint8_t CallerId_Mode_valid;  /**< Must be set to true if CallerId_Mode is being passed */
  uint32_t CallerId_Mode;
  /**<   Sets caller ID mode to presentation or restriction. 
		Default value is 16. Values: \n
        - 0  -- OIP_OIR_NONE  \n
        - 1  -- OIP_MODE  \n
        - 2  -- OIR_PERM_MODE_PRESENTATION_RESTRICTED  \n
        - 4  -- OIR_PERM_MODE_PRESENTATION_NOT_RESTRICTED  \n
        - 8  -- OIR_TEMP_MODE_PRESENTATION_RESTRICTED  \n
        - 16 -- OIR_TEMP_MODE_PRESENTATION_NOT_RESTRICTED  \n
        - 32 -- OIR_TEMP_MODE_PRESENTATION_RESTRICTED_OVERRIDE 
    */

  /* Optional */
  /*  SIP Timer TCall */
  uint8_t T_CALL_valid;  /**< Must be set to true if T_CALL is being passed */
  uint32_t T_CALL;
  /**<   Sets duration of the TCALL timer for an outgoing call in seconds. 
		Default value is 0 seconds.
    */

  /* Optional */
  /*  Ringing Timer */
  uint8_t Ringing_Timer_valid;  /**< Must be set to true if Ringing_Timer is being passed */
  uint32_t Ringing_Timer;
  /**<   Sets duration of the ringing timer in seconds. The ringing timer 
         starts on the ringing event. If the call is not answered within 
         the duration of this timer, the call is disconnected.
         Default value is 45 seconds.
    */

  /* Optional */
  /*  Ringback Timer */
  uint8_t RingBack_Timer_valid;  /**< Must be set to true if RingBack_Timer is being passed */
  uint32_t RingBack_Timer;
  /**<   Sets duration of the ringback timer in seconds. The ringback timer
         starts on the ringback event. If the call is not answered within
         the duration of this timer, the call is disconnected.
         Default value is 90 seconds.
    */

  /* Optional */
  /*  Session Timer Enable */
  uint8_t Session_Timer_Enabled_valid;  /**< Must be set to true if Session_Timer_Enabled is being passed */
  uint32_t Session_Timer_Enabled;
  /**<   Indicates whether the VoIP session is timer enabled. Default value is 1.
         Values: \n
          -1 -- Enable \n
          -0 -- Disable          
    */

  /* Optional */
  /*  Session Expire Timer */
  uint8_t Session_Expires_valid;  /**< Must be set to true if Session_Expires is being passed */
  uint32_t Session_Expires;
  /**<   Sets session duration in seconds. 
         Default value is 1800 seconds.
    */

  /* Optional */
  /*  Minimum Session Expire Timer */
  uint8_t Min_Session_Expires_valid;  /**< Must be set to true if Min_Session_Expires is being passed */
  uint32_t Min_Session_Expires;
  /**<   Sets the minimum value for a session timer in seconds for 
		outgoing INVITE. Default value is 90 seconds. 
    */

  /* Optional */
  /*  RTCP Link Aliveness Timer */
  uint8_t Rtcp_Link_Aliveness_Timer_valid;  /**< Must be set to true if Rtcp_Link_Aliveness_Timer is being passed */
  uint32_t Rtcp_Link_Aliveness_Timer;
  /**<   If RTCP is not received during a connected media session, sets the timer 
		(in seconds) for when an indication is published. Default value is 20 seconds. 
    */

  /* Optional */
  /*  AMR Octet Aligned Dynamic PT  */
  uint8_t Amr_OctetAligned_DynamicPt_valid;  /**< Must be set to true if Amr_OctetAligned_DynamicPt is being passed */
  uint32_t Amr_OctetAligned_DynamicPt;
  /**<   Sets payload type for AMR octate aligned codec. 
         Default value is 104. 
    */

  /* Optional */
  /*  AMR Bandwidth Efficient Dynamic PT */
  uint8_t Amr_BandWidthEfficient_DynamicPt_valid;  /**< Must be set to true if Amr_BandWidthEfficient_DynamicPt is being passed */
  uint32_t Amr_BandWidthEfficient_DynamicPt;
  /**<   Sets payload type for AMR bandwidth efficient codec. 
         Default value is 102. 
    */

  /* Optional */
  /*  DTMF NB Dynamic PT */
  uint8_t Dtmf_Nb_DynamicPt_valid;  /**< Must be set to true if Dtmf_Nb_DynamicPt is being passed */
  uint32_t Dtmf_Nb_DynamicPt;
  /**<   Sets payload type for DTMF narrowband (NB) codec. 
         Default value is 96. 
    */

  /* Optional */
  /*  AMR Mode Set  */
  uint8_t Amr_ModeSet_valid;  /**< Must be set to true if Amr_ModeSet is being passed */
  uint32_t Amr_ModeSet;
  /**<   Sets the bitmask for AMR NB modes. Values: \n
        - 0x1 -- 4.75 kbps \n
        - 0x2 -- 5.15 kbps \n
        - 0x4 -- 5.9 kbps \n
        - 0x8 -- 6.17 kbps \n
        - 0x10 -- 7.4 kbps \n
        - 0x20 -- 7.95 kbps \n
        - 0x40 -- 10.2 kbps \n
        - 0x80 -- 12.2 kbps
    */

  /* Optional */
  /*  SCR AMR Enable  */
  uint8_t Scr_AMR_Enable_valid;  /**< Must be set to true if Scr_AMR_Enable is being passed */
  uint32_t Scr_AMR_Enable;
  /**<   Enables source controlled rate (SCR) for AMR narrowband. 
        Values: \n
        -1 -- Enable \n
        -0 -- Disable  
    */

  /* Optional */
  /*  Speech Start Port  */
  uint8_t Speech_Start_Port_valid;  /**< Must be set to true if Speech_Start_Port is being passed */
  uint32_t Speech_Start_Port;
  /**<   
         Sets the smallest RTP port number for a speech codec.
         If not set, the default value is 50000.
    */

  /* Optional */
  /*  Speech Stop Port  */
  uint8_t Speech_Stop_Port_valid;  /**< Must be set to true if Speech_Stop_Port is being passed */
  uint32_t Speech_Stop_Port;
  /**<   
         Sets the largest RTP port number for a speech codec.
         If not set, the default value is 50002. Currently, this TLV is 
		 not supported, since there is only support for one call in IOT 
		 products. 
    */
}qmi_ims_iot_get_voice_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Sets IMS voice or VoLTE configurations at the modem. */
typedef struct {

  /* Optional */
  /*  Display Text Priority */
  uint8_t Display_Text_Priority_valid;  /**< Must be set to true if Display_Text_Priority is being passed */
  uint32_t Display_Text_Priority;
  /**<   Sets originating party identity determination from PAI or from the 
		header for incoming SIP messages. Default value is 0. Values: \n
        - 0 --  PAI\n
        - 1 --  From\n
        - 2 --  Fallback-enabled \n
        Only the above values are supported.
    */

  /* Optional */
  /*  Caller ID Mode */
  uint8_t CallerId_Mode_valid;  /**< Must be set to true if CallerId_Mode is being passed */
  uint32_t CallerId_Mode;
  /**<   Sets caller ID mode to presentation or restriction. 
	Default value is 16. Values: \n
        - 0  -- OIP_OIR_NONE  \n
        - 1  -- OIP_MODE  \n
        - 2  -- OIR_PERM_MODE_PRESENTATION_RESTRICTED  \n
        - 4  -- OIR_PERM_MODE_PRESENTATION_NOT_RESTRICTED  \n
        - 8  -- OIR_TEMP_MODE_PRESENTATION_RESTRICTED  \n
        - 16 -- OIR_TEMP_MODE_PRESENTATION_NOT_RESTRICTED  \n
        - 32 -- OIR_TEMP_MODE_PRESENTATION_RESTRICTED_OVERRIDE  \n
        Only the above values are supported.
    */

  /* Optional */
  /*  SIP Timer TCall */
  uint8_t T_CALL_valid;  /**< Must be set to true if T_CALL is being passed */
  uint32_t T_CALL;
  /**<   Sets duration of the TCALL timer for outgoing call in seconds.
	Default value is 0 seconds.
    */

  /* Optional */
  /*  Ringing Timer */
  uint8_t Ringing_Timer_valid;  /**< Must be set to true if Ringing_Timer is being passed */
  uint32_t Ringing_Timer;
  /**<   Sets duration of the ringing timer in seconds. The ringing timer 
         starts on the ringing event. If the call is not answered within 
         the duration of this timer, the call is disconnected.
         Default value is 45 seconds.
    */

  /* Optional */
  /*  Ringback Timer */
  uint8_t RingBack_Timer_valid;  /**< Must be set to true if RingBack_Timer is being passed */
  uint32_t RingBack_Timer;
  /**<   Sets duration of the ringback timer in seconds. The ringback timer
         starts on the ringback event. If the call is not answered within
         the duration of this timer, the call is disconnected.
         Default value is 90 seconds.
    */

  /* Optional */
  /*  Session Timer Enable */
  uint8_t Session_Timer_Enabled_valid;  /**< Must be set to true if Session_Timer_Enabled is being passed */
  uint32_t Session_Timer_Enabled;
  /**<   Indicates whether the VoIP session is timer enabled. Default value is 1.
         Values: \n
          -1 -- Enable \n
          -0 -- Disable \n
          Only the above values are supported. Any other value defaults to enabled.    
    */

  /* Optional */
  /*  Session Expire Timer */
  uint8_t Session_Expires_valid;  /**< Must be set to true if Session_Expires is being passed */
  uint32_t Session_Expires;
  /**<   Sets session duration in seconds. 
         Default value is 1800 seconds.
    */

  /* Optional */
  /*  Minimum Session Expire Timer */
  uint8_t Min_Session_Expires_valid;  /**< Must be set to true if Min_Session_Expires is being passed */
  uint32_t Min_Session_Expires;
  /**<   Sets the minimum value for session timer in seconds for outgoing 
		INVITE. Default value is 90 seconds. 
    */

  /* Optional */
  /*  RTCP Link Aliveness Timer */
  uint8_t Rtcp_Link_Aliveness_Timer_valid;  /**< Must be set to true if Rtcp_Link_Aliveness_Timer is being passed */
  uint32_t Rtcp_Link_Aliveness_Timer;
  /**<   If RTCP is not received during a connected media session, sets the timer
		(in seconds) for when an indication is published. 
		Default value is 20 seconds. 
    */

  /* Optional */
  /*  AMR Octet Aligned Dynamic PT  */
  uint8_t Amr_OctetAligned_DynamicPt_valid;  /**< Must be set to true if Amr_OctetAligned_DynamicPt is being passed */
  uint32_t Amr_OctetAligned_DynamicPt;
  /**<   Sets payload type for AMR octate aligned codec. 
         Default value is 104. 
    */

  /* Optional */
  /*  AMR Bandwidth Efficient Dynamic PT */
  uint8_t Amr_BandWidthEfficient_DynamicPt_valid;  /**< Must be set to true if Amr_BandWidthEfficient_DynamicPt is being passed */
  uint32_t Amr_BandWidthEfficient_DynamicPt;
  /**<   Sets payload type for AMR bandwidth efficient codec. 
         Default value is 102. 
    */

  /* Optional */
  /*  DTMF NB Dynamic PT */
  uint8_t Dtmf_Nb_DynamicPt_valid;  /**< Must be set to true if Dtmf_Nb_DynamicPt is being passed */
  uint32_t Dtmf_Nb_DynamicPt;
  /**<   Sets payload type for DTMF narrow band codec. 
         Default value is 96. 
    */

  /* Optional */
  /*  AMR Mode Set  */
  uint8_t Amr_ModeSet_valid;  /**< Must be set to true if Amr_ModeSet is being passed */
  uint32_t Amr_ModeSet;
  /**<   Sets the bitmask for AMR NB modes. Values: \n
        - 0x1 -- 4.75 kbps \n
        - 0x2 -- 5.15 kbps \n
        - 0x4 -- 5.9 kbps \n
        - 0x8 -- 6.17 kbps \n
        - 0x10 -- 7.4 kbps \n
        - 0x20 -- 7.95 kbps \n
        - 0x40 -- 10.2 kbps \n
        - 0x80 -- 12.2 kbps \n
        Only above values are supported.
    */

  /* Optional */
  /*  SCR AMR Enable  */
  uint8_t Scr_AMR_Enable_valid;  /**< Must be set to true if Scr_AMR_Enable is being passed */
  uint32_t Scr_AMR_Enable;
  /**<   Enables source controlled rate (SCR) for AMR narrowband. \n
        Values: \n
        -1 -- Enable \n
        -0 -- Disable \n
        Only the above values are supported. Any other value defaults to enabled.
    */

  /* Optional */
  /*  Speech Start Port  */
  uint8_t Speech_Start_Port_valid;  /**< Must be set to true if Speech_Start_Port is being passed */
  uint32_t Speech_Start_Port;
  /**<   
         Sets the smallest RTP port number for a speech codec.
         If not set, the default value is 50000.
    */

  /* Optional */
  /*  Speech Stop Port  */
  uint8_t Speech_Stop_Port_valid;  /**< Must be set to true if Speech_Stop_Port is being passed */
  uint32_t Speech_Stop_Port;
  /**<   
         Sets the largest RTP port number for a speech codec.
         If not set, the default value is 50002. Currently, this TLV is 
		 not supported, since there is only support for one call in IOT 
		 products. 
    */
}qmi_ims_iot_set_voice_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Sets IMS voice or VoLTE configurations at the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}qmi_ims_iot_set_voice_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Indication Message; Indicates when IMS voice or VoLTE configurations change. */
typedef struct {

  /* Optional */
  /*  Display Text Priority */
  uint8_t Display_Text_Priority_valid;  /**< Must be set to true if Display_Text_Priority is being passed */
  uint32_t Display_Text_Priority;
  /**<   Sets originating party identity determination from PAI or from header
		for incoming SIP messages. Default value is 0. Values: \n
        - 0 --  PAI\n
        - 1 --  From\n
        - 2 --  Fallback Enabled
    */

  /* Optional */
  /*  Caller ID Mode */
  uint8_t CallerId_Mode_valid;  /**< Must be set to true if CallerId_Mode is being passed */
  uint32_t CallerId_Mode;
  /**<   Sets Caller ID mode to presentation or restriction. 
		Default value is 16. Values: \n
        - 0  -- OIP_OIR_NONE  \n
        - 1  -- OIP_MODE  \n
        - 2  -- OIR_PERM_MODE_PRESENTATION_RESTRICTED  \n
        - 4  -- OIR_PERM_MODE_PRESENTATION_NOT_RESTRICTED  \n
        - 8  -- OIR_TEMP_MODE_PRESENTATION_RESTRICTED  \n
        - 16 -- OIR_TEMP_MODE_PRESENTATION_NOT_RESTRICTED  \n
        - 32 -- OIR_TEMP_MODE_PRESENTATION_RESTRICTED_OVERRIDE
    */

  /* Optional */
  /*  SIP Timer TCall */
  uint8_t T_CALL_valid;  /**< Must be set to true if T_CALL is being passed */
  uint32_t T_CALL;
  /**<   Sets duration of the TCALL timer for outgoing call in seconds. 
	Default value is 0 seconds.
    */

  /* Optional */
  /*  Ringing Timer */
  uint8_t Ringing_Timer_valid;  /**< Must be set to true if Ringing_Timer is being passed */
  uint32_t Ringing_Timer;
  /**<   Sets duration of the ringing timer in seconds. The ringing timer 
         starts on the ringing event. If the call is not answered within 
         the duration of this timer, the call is disconnected.
         Default value is 45 seconds.
    */

  /* Optional */
  /*  Ringback Timer */
  uint8_t RingBack_Timer_valid;  /**< Must be set to true if RingBack_Timer is being passed */
  uint32_t RingBack_Timer;
  /**<   Duration of the ringback timer in seconds. The ringback timer
         starts on the ringback event. If the call is not answered within
         the duration of this timer, the call is disconnected.
         Default value is 90 seconds.
    */

  /* Optional */
  /*  Session Timer Enable */
  uint8_t Session_Timer_Enabled_valid;  /**< Must be set to true if Session_Timer_Enabled is being passed */
  uint32_t Session_Timer_Enabled;
  /**<   Indicates whether the VoIP session is timer enabled. Default value is 1.
         Values: \n
          -1 -- Enable \n
          -0 -- Disable 
    */

  /* Optional */
  /*  Session Expire Timer */
  uint8_t Session_Expires_valid;  /**< Must be set to true if Session_Expires is being passed */
  uint32_t Session_Expires;
  /**<   Sets session duration in seconds. 
         Default value is 1800 seconds.
    */

  /* Optional */
  /*  Minimum Session Expire */
  uint8_t Min_Session_Expires_valid;  /**< Must be set to true if Min_Session_Expires is being passed */
  uint32_t Min_Session_Expires;
  /**<   Sets the minimum value for session timer in seconds for outgoing INVITE.
		Default value is 90 seconds. 
    */

  /* Optional */
  /*  RTCP Link Aliveness Timer */
  uint8_t Rtcp_Link_Aliveness_Timer_valid;  /**< Must be set to true if Rtcp_Link_Aliveness_Timer is being passed */
  uint32_t Rtcp_Link_Aliveness_Timer;
  /**<   If RTCP is not received during a connected media session, sets the timer
		(in seconds) for when an indication is published. 
		Default value is 20 seconds.
    */

  /* Optional */
  /*  AMR Octet Aligned Dynamic PT  */
  uint8_t Amr_OctetAligned_DynamicPt_valid;  /**< Must be set to true if Amr_OctetAligned_DynamicPt is being passed */
  uint32_t Amr_OctetAligned_DynamicPt;
  /**<   Sets payload type for AMR octate aligned codec. 
         Default value is 104. 
    */

  /* Optional */
  /*  AMR Bandwidth Efficient Dynamic PT */
  uint8_t Amr_BandWidthEfficient_DynamicPt_valid;  /**< Must be set to true if Amr_BandWidthEfficient_DynamicPt is being passed */
  uint32_t Amr_BandWidthEfficient_DynamicPt;
  /**<   Sets payload type for AMR bandwidth efficient codec. 
         Default value is 102. 
    */

  /* Optional */
  /*  DTMF NB Dynamic PT */
  uint8_t Dtmf_Nb_DynamicPt_valid;  /**< Must be set to true if Dtmf_Nb_DynamicPt is being passed */
  uint32_t Dtmf_Nb_DynamicPt;
  /**<   Sets payload type for DTMF narrowband codec. 
         Default value is 96. 
    */

  /* Optional */
  /*  AMR Mode Set  */
  uint8_t Amr_ModeSet_valid;  /**< Must be set to true if Amr_ModeSet is being passed */
  uint32_t Amr_ModeSet;
  /**<   Sets the bitmask for AMR NB modes. Values: \n
        - 0x1 -- 4.75 kbps \n
        - 0x2 -- 5.15 kbps \n
        - 0x4 -- 5.9 kbps \n
        - 0x8 -- 6.17 kbps \n
        - 0x10 -- 7.4 kbps \n
        - 0x20 -- 7.95 kbps \n
        - 0x40 -- 10.2 kbps \n
        - 0x80 -- 12.2 kbps
    */

  /* Optional */
  /*  SCR AMR Enable  */
  uint8_t Scr_AMR_Enable_valid;  /**< Must be set to true if Scr_AMR_Enable is being passed */
  uint32_t Scr_AMR_Enable;
  /**<   Enables source controlled rate (SCR) for AMR narrowband.
        Values: \n
        -1 -- Enable \n
        -0 -- Disable  
    */

  /* Optional */
  /*  Speech Start Port  */
  uint8_t Speech_Start_Port_valid;  /**< Must be set to true if Speech_Start_Port is being passed */
  uint32_t Speech_Start_Port;
  /**<   
         Sets the smallest RTP port number for a speech codec.
         If not set, the default value is 50000.
    */

  /* Optional */
  /*  Speech Stop Port  */
  uint8_t Speech_Stop_Port_valid;  /**< Must be set to true if Speech_Stop_Port is being passed */
  uint32_t Speech_Stop_Port;
  /**<   
         Sets the largest RTP port number for a speech codec.
         If not set, the default value is 50002. Currently, this TLV is 
		 not supported, since there is only support for one call in IOT 
		 products. 
    */
}qmi_ims_iot_voice_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_enums
    @{
  */
typedef enum {
  IMS_IOT_REG_STATUS_E_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_IOT_STATUS_MIN_V01 = 0, /**<  Invalid registration status for IMS \n  */
  IMS_IOT_STATUS_NOT_REGISTERED_V01 = 1, /**<  Not registered or registration failed state for IMS \n  */
  IMS_IOT_STATUS_REGISTERING_V01 = 2, /**<  Registering for IMS \n   */
  IMS_IOT_STATUS_REGISTERED_V01 = 3, /**<  Registered for IMS \n  */
  IMS_IOT_STATUS_LIMITED_REGISTERED_V01 = 4, /**<  Limited registration for IMS \n  */
  IMS_IOT_STATUS_MAX_V01 = 5, /**<  Invalid registration status for IMS  */
  IMS_IOT_REG_STATUS_E_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_iot_reg_status_e_type_v01;
/**
    @}
  */

/** @addtogroup ims_iot_qmi_aggregates
    @{
  */
/**  Enum for registration status

 Structure for storing URI

 */
typedef struct {

  char ims_uri[IMS_IOT_URI_LEN_V01 + 1];
  /**<   URI String */
}ims_iot_uri_s_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_enums
    @{
  */
typedef enum {
  IMS_IOT_SERVICE_STATUS_E_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_IOT_SERVICE_MIN_V01 = 0, /**<  Invalid service status \n  */
  IMS_IOT_NO_SERVICE_V01 = 1, /**<  No service available \n  */
  IMS_IOT_FULL_SERVICE_V01 = 2, /**<  Service available \n  */
  IMS_IOT_LIMITED_SERVICE_V01 = 3, /**<  Limited service available \n  */
  IMS_IOT_SERVICE_MAX_V01 = 4, /**<  Invalid service status  */
  IMS_IOT_SERVICE_STATUS_E_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_iot_service_status_e_type_v01;
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Gets the registration status related to IMS services for the
	         requesting control point. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_ims_iot_get_registration_service_status_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Gets the registration status related to IMS services for the
	         requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Registration Status */
  uint8_t registration_status_valid;  /**< Must be set to true if registration_status is being passed */
  ims_iot_reg_status_e_type_v01 registration_status;
  /**<   Gets the IMS registration status.
 Values: \n
      - IMS_IOT_STATUS_MIN (0) --  Invalid registration status for IMS \n 
      - IMS_IOT_STATUS_NOT_REGISTERED (1) --  Not registered or registration failed state for IMS \n 
      - IMS_IOT_STATUS_REGISTERING (2) --  Registering for IMS \n  
      - IMS_IOT_STATUS_REGISTERED (3) --  Registered for IMS \n 
      - IMS_IOT_STATUS_LIMITED_REGISTERED (4) --  Limited registration for IMS \n 
      - IMS_IOT_STATUS_MAX (5) --  Invalid registration status for IMS 
 */

  /* Optional */
  /*  SMS Service Status */
  uint8_t sms_service_status_valid;  /**< Must be set to true if sms_service_status is being passed */
  ims_iot_service_status_e_type_v01 sms_service_status;
  /**<   Gets the SMS service status.
 Values: \n
      - IMS_IOT_SERVICE_MIN (0) --  Invalid service status \n 
      - IMS_IOT_NO_SERVICE (1) --  No service available \n 
      - IMS_IOT_FULL_SERVICE (2) --  Service available \n 
      - IMS_IOT_LIMITED_SERVICE (3) --  Limited service available \n 
      - IMS_IOT_SERVICE_MAX (4) --  Invalid service status 
 */

  /* Optional */
  /*  Voice Service Status */
  uint8_t voice_service_status_valid;  /**< Must be set to true if voice_service_status is being passed */
  ims_iot_service_status_e_type_v01 voice_service_status;
  /**<   Gets the voice service status.
 Values: \n
      - IMS_IOT_SERVICE_MIN (0) --  Invalid service status \n 
      - IMS_IOT_NO_SERVICE (1) --  No service available \n 
      - IMS_IOT_FULL_SERVICE (2) --  Service available \n 
      - IMS_IOT_LIMITED_SERVICE (3) --  Limited service available \n 
      - IMS_IOT_SERVICE_MAX (4) --  Invalid service status 
 */

  /* Optional */
  /*  Registration Error Code */
  uint8_t registration_failure_reason_code_valid;  /**< Must be set to true if registration_failure_reason_code is being passed */
  uint32_t registration_failure_reason_code;
  /**<   In case of failure, a reason or error code is returned when 
		the Registration Status TLV is IMS_IOT_STATUS_NOT_REGISTERED. 
		Values: \n
       - 3xx -- Redirection responses \n
       - 4xx -- Client failure responses \n
       - 5xx -- Server failure responses \n
       - 6xx -- Global failure responses
    */

  /* Optional */
  /*  Registration Error String */
  uint8_t registration_failure_reason_valid;  /**< Must be set to true if registration_failure_reason is being passed */
  char registration_failure_reason[IMS_IOT_REGISTRATION_FAILURE_LEN_V01 + 1];
  /**<   In case of failure, a registration failure error string when 
		Registration Status TLV is IMS_IOT_STATUS_NOT_REGISTERED. 
        Otherwise, it is a NULL string.
    */

  /* Optional */
  /*  P-Associated URI List */
  uint8_t uri_list_valid;  /**< Must be set to true if uri_list is being passed */
  uint32_t uri_list_len;  /**< Must be set to # of elements in uri_list */
  ims_iot_uri_s_type_v01 uri_list[IMS_IOT_MAX_URI_LIST_V01];
}qmi_ims_iot_get_registration_service_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Indication Message; Indicates when the IMS service registration status changes. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Registration Status */
  uint8_t registration_status_valid;  /**< Must be set to true if registration_status is being passed */
  ims_iot_reg_status_e_type_v01 registration_status;
  /**<   Gets the IMS registration status.
 Values: \n
      - IMS_IOT_STATUS_MIN (0) --  Invalid registration status for IMS \n 
      - IMS_IOT_STATUS_NOT_REGISTERED (1) --  Not registered or registration failed state for IMS \n 
      - IMS_IOT_STATUS_REGISTERING (2) --  Registering for IMS \n  
      - IMS_IOT_STATUS_REGISTERED (3) --  Registered for IMS \n 
      - IMS_IOT_STATUS_LIMITED_REGISTERED (4) --  Limited registration for IMS \n 
      - IMS_IOT_STATUS_MAX (5) --  Invalid registration status for IMS 
 */

  /* Optional */
  /*  SMS Service Status */
  uint8_t sms_service_status_valid;  /**< Must be set to true if sms_service_status is being passed */
  ims_iot_service_status_e_type_v01 sms_service_status;
  /**<   Gets the SMS service status.
 Values: \n
      - IMS_IOT_SERVICE_MIN (0) --  Invalid service status \n 
      - IMS_IOT_NO_SERVICE (1) --  No service available \n 
      - IMS_IOT_FULL_SERVICE (2) --  Service available \n 
      - IMS_IOT_LIMITED_SERVICE (3) --  Limited service available \n 
      - IMS_IOT_SERVICE_MAX (4) --  Invalid service status 
 */

  /* Optional */
  /*  Voice Service Status */
  uint8_t voice_service_status_valid;  /**< Must be set to true if voice_service_status is being passed */
  ims_iot_service_status_e_type_v01 voice_service_status;
  /**<   Gets the voice service status.
 Values: \n
      - IMS_IOT_SERVICE_MIN (0) --  Invalid service status \n 
      - IMS_IOT_NO_SERVICE (1) --  No service available \n 
      - IMS_IOT_FULL_SERVICE (2) --  Service available \n 
      - IMS_IOT_LIMITED_SERVICE (3) --  Limited service available \n 
      - IMS_IOT_SERVICE_MAX (4) --  Invalid service status 
 */

  /* Optional */
  /*  Registration Error Code */
  uint8_t registration_failure_reason_code_valid;  /**< Must be set to true if registration_failure_reason_code is being passed */
  uint32_t registration_failure_reason_code;
  /**<   In case of failure, a reason or error code is returned when 
		the Registration Status TLV is IMS_IOT_STATUS_NOT_REGISTERED. 
		Values: \n
       - 3xx -- Redirection responses \n
       - 4xx -- Client failure responses \n
       - 5xx -- Server failure responses \n
       - 6xx -- Global failure responses
    */

  /* Optional */
  /*  Registration Error String */
  uint8_t registration_failure_reason_valid;  /**< Must be set to true if registration_failure_reason is being passed */
  char registration_failure_reason[IMS_IOT_REGISTRATION_FAILURE_LEN_V01 + 1];
  /**<   In case of failure. a registration failure error string when 
		Registration Status TLV is IMS_IOT_STATUS_NOT_REGISTERED. 
        Otherwise, it is a NULL string.
    */

  /* Optional */
  /*  P-Associated URI List */
  uint8_t uri_list_valid;  /**< Must be set to true if uri_list is being passed */
  uint32_t uri_list_len;  /**< Must be set to # of elements in uri_list */
  ims_iot_uri_s_type_v01 uri_list[IMS_IOT_MAX_URI_LIST_V01];
}qmi_ims_iot_registration_service_status_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_enums
    @{
  */
typedef enum {
  IMS_IOT_PDN_FAILURE_CODE_E_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_IOT_STATUS_OTHER_FAILURE_V01 = 0, /**<  Generic failure reason for other than specified \n  */
  IMS_IOT_STATUS_OPTION_UNSUBSCRIBED_V01 = 1, /**<  Option is unsubscribed \n  */
  IMS_IOT_STATUS_UNKNOWN_PDN_V01 = 2, /**<  PDN was unknown \n  */
  IMS_IOT_STATUS_REASON_NOT_SPECIFIED_V01 = 3, /**<  Reason not specified \n */
  IMS_IOT_STATUS_CONNECTION_BRINGUP_FAILURE_V01 = 4, /**<  Connection bring-up failure \n  */
  IMS_IOT_STATUS_CONNECTION_IKE_AUTH_FAILURE_V01 = 5, /**<  IKE authentication failure \n  */
  IMS_IOT_STATUS_USER_AUTH_FAILED_V01 = 6, /**<  User authentication failed    */
  IMS_IOT_PDN_FAILURE_CODE_E_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_iot_pdn_failure_code_e_type_v01;
/**
    @}
  */

/** @addtogroup ims_iot_qmi_enums
    @{
  */
typedef enum {
  IMS_IOT_PDN_FAIL_REASON_E_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_IOT_TYPE_UNSPECIFIED_V01 = 0x00, /**<  Unspecified \n  */
  IMS_IOT_TYPE_MOBILE_IP_V01 = 0x01, /**<  Mobile IP \n  */
  IMS_IOT_TYPE_INTERNAL_V01 = 0x02, /**<  Internal \n  */
  IMS_IOT_TYPE_CALL_MANAGER_DEFINED_V01 = 0x03, /**<  Call manager is defined \n  */
  IMS_IOT_TYPE_3GPP_SPEC_DEFINED_V01 = 0x04, /**<  3GPP specification is defined \n  */
  IMS_IOT_TYPE_PPP_V01 = 0x05, /**<  PPP \n  */
  IMS_IOT_TYPE_IPV6_V01 = 0x06, /**<  IPv6  */
  IMS_IOT_PDN_FAIL_REASON_E_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_iot_pdn_fail_reason_e_type_v01;
/**
    @}
  */

/** @addtogroup ims_iot_qmi_aggregates
    @{
  */
/**  Enumeration for various PDN failure code

Enumeration for a verbose call end reason (VCER) type. 
 Structure for various PDN failure error code

 */
typedef struct {

  ims_iot_pdn_fail_reason_e_type_v01 pdn_fail_reason_type;
  /**<   Gets the PDN connection failure reason type when the PDN 
 Connection Status TLV is FALSE. Values: \n
      - IMS_IOT_TYPE_UNSPECIFIED (0x00) --  Unspecified \n 
      - IMS_IOT_TYPE_MOBILE_IP (0x01) --  Mobile IP \n 
      - IMS_IOT_TYPE_INTERNAL (0x02) --  Internal \n 
      - IMS_IOT_TYPE_CALL_MANAGER_DEFINED (0x03) --  Call manager is defined \n 
      - IMS_IOT_TYPE_3GPP_SPEC_DEFINED (0x04) --  3GPP specification is defined \n 
      - IMS_IOT_TYPE_PPP (0x05) --  PPP \n 
      - IMS_IOT_TYPE_IPV6 (0x06) --  IPv6 
 */

  uint32_t pdn_fail_reason;
  /**<   Failure reason (verbose); see the TS 24.008, TS 24.007, and C.S0017
       specificaitons for the definition of these values.
  */
}ims_iot_pdn_failure_reason_s_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Gets the IMS PDN status.  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_ims_iot_get_pdn_status_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Gets the IMS PDN status.  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  PDN Connection Status */
  uint8_t pdn_status_valid;  /**< Must be set to true if pdn_status is being passed */
  uint8_t pdn_status;
  /**<   Checks the PDN connection status.
         Values: \n
           - TRUE -- PDN is up or connected \n 
           - FALSE -- PDN is down or not connected
    */

  /* Optional */
  /*  PDN Failure Reason String */
  uint8_t pdn_failure_reason_valid;  /**< Must be set to true if pdn_failure_reason is being passed */
  char pdn_failure_reason[IMS_IOT_PDN_FAILURE_LEN_V01 + 1];
  /**<    Gets the PDN failure reason string when the PDN Connection 
		Status TLV is FALSE. Otherwise string is NULL.
    */

  /* Optional */
  /*  PDN Failure Code */
  uint8_t pdn_fail_code_valid;  /**< Must be set to true if pdn_fail_code is being passed */
  ims_iot_pdn_failure_code_e_type_v01 pdn_fail_code;
  /**<   Gets the PDN connection failure code when the PDN 
 Connection Status TLV is FALSE. Values: \n
      - IMS_IOT_STATUS_OTHER_FAILURE (0) --  Generic failure reason for other than specified \n 
      - IMS_IOT_STATUS_OPTION_UNSUBSCRIBED (1) --  Option is unsubscribed \n 
      - IMS_IOT_STATUS_UNKNOWN_PDN (2) --  PDN was unknown \n 
      - IMS_IOT_STATUS_REASON_NOT_SPECIFIED (3) --  Reason not specified \n
      - IMS_IOT_STATUS_CONNECTION_BRINGUP_FAILURE (4) --  Connection bring-up failure \n 
      - IMS_IOT_STATUS_CONNECTION_IKE_AUTH_FAILURE (5) --  IKE authentication failure \n 
      - IMS_IOT_STATUS_USER_AUTH_FAILED (6) --  User authentication failed   
 */

  /* Optional */
  /*  PDN Failure Reason Type */
  uint8_t pdn_fail_cause_valid;  /**< Must be set to true if pdn_fail_cause is being passed */
  ims_iot_pdn_failure_reason_s_type_v01 pdn_fail_cause;
}qmi_ims_iot_get_pdn_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Indication Message; Indicates when the status of a PDN connection changes. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  PDN Connection Status */
  uint8_t pdn_status_valid;  /**< Must be set to true if pdn_status is being passed */
  uint8_t pdn_status;
  /**<   Checks the PDN connection status.
         Values: \n
           - TRUE -- PDN is up or connected \n 
           - FALSE -- PDN is down or not connected
    */

  /* Optional */
  /*  PDN Failure Reason String  */
  uint8_t pdn_failure_reason_valid;  /**< Must be set to true if pdn_failure_reason is being passed */
  char pdn_failure_reason[IMS_IOT_PDN_FAILURE_LEN_V01 + 1];
  /**<    PDN failure reason string when the PDN Connection Status 
		TLV is FALSE. Otherwise it is NULL.
    */

  /* Optional */
  /*  PDN Failure Code */
  uint8_t pdn_fail_code_valid;  /**< Must be set to true if pdn_fail_code is being passed */
  ims_iot_pdn_failure_code_e_type_v01 pdn_fail_code;
  /**<   Gets the PDN connection failure code when the PDN 
 Connection Status TLV is FALSE. Values: \n
      - IMS_IOT_STATUS_OTHER_FAILURE (0) --  Generic failure reason for other than specified \n 
      - IMS_IOT_STATUS_OPTION_UNSUBSCRIBED (1) --  Option is unsubscribed \n 
      - IMS_IOT_STATUS_UNKNOWN_PDN (2) --  PDN was unknown \n 
      - IMS_IOT_STATUS_REASON_NOT_SPECIFIED (3) --  Reason not specified \n
      - IMS_IOT_STATUS_CONNECTION_BRINGUP_FAILURE (4) --  Connection bring-up failure \n 
      - IMS_IOT_STATUS_CONNECTION_IKE_AUTH_FAILURE (5) --  IKE authentication failure \n 
      - IMS_IOT_STATUS_USER_AUTH_FAILED (6) --  User authentication failed   
 */

  /* Optional */
  /*  PDN Failure Reason Type */
  uint8_t pdn_fail_cause_valid;  /**< Must be set to true if pdn_fail_cause is being passed */
  ims_iot_pdn_failure_reason_s_type_v01 pdn_fail_cause;
}qmi_ims_iot_pdn_status_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_enums
    @{
  */
typedef enum {
  IMS_IOT_BLOCK_STATUS_E_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_IOT_NOT_BLOCKED_V01 = 0, /**<  IMS registration and/or PDN is not blocked \n  */
  IMS_IOT_TEMP_BLOCKED_V01 = 1, /**<  IMS Registrationand/or PDN is blocked temporarily \n  */
  IMS_IOT_PERM_BLOCKED_V01 = 2, /**<  IMS Registration and/or PDN is blocked permanently \n  */
  IMS_IOT_BLOCK_STATUS_MAX_V01 = 3, /**<  Invalid block status  */
  IMS_IOT_BLOCK_STATUS_E_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_iot_block_status_e_type_v01;
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Retrieves the IMS block status. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_ims_iot_get_registration_block_status_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Retrieves the IMS block status. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Block Status */
  uint8_t block_status_valid;  /**< Must be set to true if block_status is being passed */
  ims_iot_block_status_e_type_v01 block_status;
  /**<   Gets the block status information. Values: \n
      - IMS_IOT_NOT_BLOCKED (0) --  IMS registration and/or PDN is not blocked \n 
      - IMS_IOT_TEMP_BLOCKED (1) --  IMS Registrationand/or PDN is blocked temporarily \n 
      - IMS_IOT_PERM_BLOCKED (2) --  IMS Registration and/or PDN is blocked permanently \n 
      - IMS_IOT_BLOCK_STATUS_MAX (3) --  Invalid block status 
 */
}qmi_ims_iot_get_registration_block_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Indication Message; Indicates when IMS block status changes because of PDN 
	          and/or registration failure. */
typedef struct {

  /* Optional */
  /*  Block Status */
  uint8_t block_status_valid;  /**< Must be set to true if block_status is being passed */
  ims_iot_block_status_e_type_v01 block_status;
  /**<   Gets the block status information. Values: \n
      - IMS_IOT_NOT_BLOCKED (0) --  IMS registration and/or PDN is not blocked \n 
      - IMS_IOT_TEMP_BLOCKED (1) --  IMS Registrationand/or PDN is blocked temporarily \n 
      - IMS_IOT_PERM_BLOCKED (2) --  IMS Registration and/or PDN is blocked permanently \n 
      - IMS_IOT_BLOCK_STATUS_MAX (3) --  Invalid block status 
 */
}qmi_ims_iot_registration_block_status_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Resets/unblocks the IMS blocking status. */
typedef struct {

  /* Mandatory */
  /*  PDN Unblock */
  uint8_t pdn_unblock;
  /**<    Request to unblock PDN at modem in case it is blocked previously.
        Values: \n
         - TRUE  -- Unblock \n
         - FALSE -- No action 
   */

  /* Mandatory */
  /*  Registration Unblock */
  uint8_t registration_unblock;
  /**<   Request to unblock registration at modem in case it is blocked previously.
        Values: \n
         - TRUE  -- Unblock \n
         - FALSE -- No Action
   */
}qmi_ims_iot_registration_unblock_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Resets/unblocks the IMS blocking status. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}qmi_ims_iot_registration_unblock_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Gets the LTE attach parameters.  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_ims_iot_get_lte_attach_parameters_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Gets the LTE attach parameters.  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  LTE Attach Parameters  */
  uint8_t operator_reserved_pco_valid;  /**< Must be set to true if operator_reserved_pco is being passed */
  uint8_t operator_reserved_pco;
  /**<   Operator-reserved PCO value for subscription status. Preconditions: \n
       - Operator must always have an operator-reserved PCO container in the Attach response. \n
       - Applications should query to get the API only when the PDP is connected.
      */
}qmi_ims_iot_get_lte_attach_parameters_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Indication Message; Indicates when LTE attach parameters change. */
typedef struct {

  /* Mandatory */
  /*  LTE Attach Parameters */
  uint8_t operator_reserved_pco;
  /**<   Operator-reserved PCO value for subscription status.
    */
}qmi_ims_iot_lte_attach_parameters_ind_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_IMS_IOT_FW_CONFIG_IND_V01
//#define REMOVE_QMI_IMS_IOT_GET_FW_CONFIG_V01
//#define REMOVE_QMI_IMS_IOT_GET_LTE_ATTACH_PARAMETERS_V01
//#define REMOVE_QMI_IMS_IOT_GET_PDN_STATUS_V01
//#define REMOVE_QMI_IMS_IOT_GET_REGISTRATION_BLOCK_STATUS_V01
//#define REMOVE_QMI_IMS_IOT_GET_REGISTRATION_SERVICE_STATUS_V01
//#define REMOVE_QMI_IMS_IOT_GET_REG_MGR_CONFIG_V01
//#define REMOVE_QMI_IMS_IOT_GET_SERVICE_STATUS_CONFIG_V01
//#define REMOVE_QMI_IMS_IOT_GET_SIP_CONFIG_V01
//#define REMOVE_QMI_IMS_IOT_GET_SMS_CONFIG_V01
//#define REMOVE_QMI_IMS_IOT_GET_VOICE_CONFIG_V01
//#define REMOVE_QMI_IMS_IOT_INDICATION_REGISTER_V01
//#define REMOVE_QMI_IMS_IOT_LTE_ATTACH_PARAMETERS_IND_V01
//#define REMOVE_QMI_IMS_IOT_PDN_STATUS_IND_V01
//#define REMOVE_QMI_IMS_IOT_REGISTRATION_BLOCK_STATUS_IND_V01
//#define REMOVE_QMI_IMS_IOT_REGISTRATION_SERVICE_STATUS_IND_V01
//#define REMOVE_QMI_IMS_IOT_REGISTRATION_UNBLOCK_V01
//#define REMOVE_QMI_IMS_IOT_REG_MGR_CONFIG_IND_V01
//#define REMOVE_QMI_IMS_IOT_SERVICE_STATUS_CONFIG_IND_V01
//#define REMOVE_QMI_IMS_IOT_SET_APCS_CONFIG_COMPLETE_V01
//#define REMOVE_QMI_IMS_IOT_SET_FW_CONFIG_V01
//#define REMOVE_QMI_IMS_IOT_SET_REG_MGR_CONFIG_V01
//#define REMOVE_QMI_IMS_IOT_SET_SERVICE_STATUS_CONFIG_V01
//#define REMOVE_QMI_IMS_IOT_SET_SIP_CONFIG_V01
//#define REMOVE_QMI_IMS_IOT_SET_SMS_CONFIG_V01
//#define REMOVE_QMI_IMS_IOT_SET_VOICE_CONFIG_V01
//#define REMOVE_QMI_IMS_IOT_SIP_CONFIG_IND_V01
//#define REMOVE_QMI_IMS_IOT_SMS_CONFIG_IND_V01
//#define REMOVE_QMI_IMS_IOT_VOICE_CONFIG_IND_V01

/*Service Message Definition*/
/** @addtogroup ims_iot_qmi_msg_ids
    @{
  */
#define QMI_IMS_IOT_GET_SUPPORTED_MSGS_REQ_V01 0x001E
#define QMI_IMS_IOT_GET_SUPPORTED_MSGS_RESP_V01 0x001E
#define QMI_IMS_IOT_INDICATION_REGISTER_REQ_V01 0x0020
#define QMI_IMS_IOT_INDICATION_REGISTER_RESP_V01 0x0020
#define QMI_IMS_IOT_GET_FW_CONFIG_REQ_V01 0x0021
#define QMI_IMS_IOT_GET_FW_CONFIG_RESP_V01 0x0021
#define QMI_IMS_IOT_SET_FW_CONFIG_REQ_V01 0x0022
#define QMI_IMS_IOT_SET_FW_CONFIG_RESP_V01 0x0022
#define QMI_IMS_IOT_FW_CONFIG_IND_V01 0x0023
#define QMI_IMS_IOT_GET_SIP_CONFIG_REQ_V01 0x0024
#define QMI_IMS_IOT_GET_SIP_CONFIG_RESP_V01 0x0024
#define QMI_IMS_IOT_SET_SIP_CONFIG_REQ_V01 0x0025
#define QMI_IMS_IOT_SET_SIP_CONFIG_RESP_V01 0x0025
#define QMI_IMS_IOT_SIP_CONFIG_IND_V01 0x0026
#define QMI_IMS_IOT_GET_REG_MGR_CONFIG_REQ_V01 0x0027
#define QMI_IMS_IOT_GET_REG_MGR_CONFIG_RESP_V01 0x0027
#define QMI_IMS_IOT_SET_REG_MGR_CONFIG_REQ_V01 0x0028
#define QMI_IMS_IOT_SET_REG_MGR_CONFIG_RESP_V01 0x0028
#define QMI_IMS_IOT_REG_MGR_CONFIG_IND_V01 0x0029
#define QMI_IMS_IOT_GET_SERVICE_STATUS_CONFIG_REQ_V01 0x002A
#define QMI_IMS_IOT_GET_SERVICE_STATUS_CONFIG_RESP_V01 0x002A
#define QMI_IMS_IOT_SET_SERVICE_STATUS_CONFIG_REQ_V01 0x002B
#define QMI_IMS_IOT_SET_SERVICE_STATUS_CONFIG_RESP_V01 0x002B
#define QMI_IMS_IOT_SERVICE_STATUS_CONFIG_IND_V01 0x002C
#define QMI_IMS_IOT_SET_APCS_CONFIG_COMPLETE_REQ_V01 0x002D
#define QMI_IMS_IOT_SET_APCS_CONFIG_COMPLETE_RESP_V01 0x002D
#define QMI_IMS_IOT_GET_SMS_CONFIG_REQ_V01 0x002E
#define QMI_IMS_IOT_GET_SMS_CONFIG_RESP_V01 0x002E
#define QMI_IMS_IOT_SET_SMS_CONFIG_REQ_V01 0x002F
#define QMI_IMS_IOT_SET_SMS_CONFIG_RESP_V01 0x002F
#define QMI_IMS_IOT_SMS_CONFIG_IND_V01 0x0030
#define QMI_IMS_IOT_GET_VOICE_CONFIG_REQ_V01 0x0031
#define QMI_IMS_IOT_GET_VOICE_CONFIG_RESP_V01 0x0031
#define QMI_IMS_IOT_SET_VOICE_CONFIG_REQ_V01 0x0032
#define QMI_IMS_IOT_SET_VOICE_CONFIG_RESP_V01 0x0032
#define QMI_IMS_IOT_VOICE_CONFIG_IND_V01 0x0033
#define QMI_IMS_IOT_GET_REGISTRATION_SERVICE_STATUS_REQ_V01 0x0034
#define QMI_IMS_IOT_GET_REGISTRATION_SERVICE_STATUS_RESP_V01 0x0034
#define QMI_IMS_IOT_REGISTRATION_SERVICE_STATUS_IND_V01 0x0035
#define QMI_IMS_IOT_GET_PDN_STATUS_REQ_V01 0x0036
#define QMI_IMS_IOT_GET_PDN_STATUS_RESP_V01 0x0036
#define QMI_IMS_IOT_PDN_STATUS_IND_V01 0x0037
#define QMI_IMS_IOT_GET_REGISTRATION_BLOCK_STATUS_REQ_V01 0x0038
#define QMI_IMS_IOT_GET_REGISTRATION_BLOCK_STATUS_RESP_V01 0x0038
#define QMI_IMS_IOT_REGISTRATION_BLOCK_STATUS_IND_V01 0x0039
#define QMI_IMS_IOT_REGISTRATION_UNBLOCK_REQ_V01 0x003A
#define QMI_IMS_IOT_REGISTRATION_UNBLOCK_RESP_V01 0x003A
#define QMI_IMS_IOT_GET_LTE_ATTACH_PARAMETERS_REQ_V01 0x003B
#define QMI_IMS_IOT_GET_LTE_ATTACH_PARAMETERS_RESP_V01 0x003B
#define QMI_IMS_IOT_LTE_ATTACH_PARAMETERS_IND_V01 0x003C
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro ims_iot_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type ims_iot_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define ims_iot_get_service_object_v01( ) \
          ims_iot_get_service_object_internal_v01( \
            IMS_IOT_V01_IDL_MAJOR_VERS, IMS_IOT_V01_IDL_MINOR_VERS, \
            IMS_IOT_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

