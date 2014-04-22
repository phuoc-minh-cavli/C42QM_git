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


  $Header: //components/rel/qmimsgs.tx/1.0/ims_iot/api/ims_iot_qmi_service_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Mon Jan  7 2019 (Spin 0)
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
#define IMS_IOT_V01_IDL_MINOR_VERS 0x02
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
/** Request Message; Register for indications triggered from service. */
typedef struct {

  /* Optional */
  /*  Flag to enable/disable configuration change indications */
  uint8_t config_change_ind_valid;  /**< Must be set to true if config_change_ind is being passed */
  uint8_t config_change_ind;
  /**<   Flag to enable/disable configuration change indications.
       Clients Should register for indication to receive them in case of any change.
       Default value is disabled for all new client. 
        Values:
        - TRUE  -- Register for config change indication
        - FALSE -- De-Register for config change indication
  */

  /* Optional */
  /*  Flag to enable/disable service status change indications */
  uint8_t srv_status_change_ind_valid;  /**< Must be set to true if srv_status_change_ind is being passed */
  uint8_t srv_status_change_ind;
  /**<    Flag to enable/disable service status change indications
        Clients Should register for indication to receive them in case of any change.
        Default value is disabled for all new client.
        Values:
        - TRUE  -- Register for service change indication
        - FALSE -- De-Register for service change indication
  */
}qmi_ims_iot_indication_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Register for indications triggered from service. */
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
/** Request Message; Fetch IMS Framework configurations from modem. */
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
/** Response Message; Fetch IMS Framework configurations from modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  User Agent String */
  uint8_t User_Agent_valid;  /**< Must be set to true if User_Agent is being passed */
  char User_Agent[IMS_IOT_USER_AGENT_LEN_V01 + 1];
  /**<   UserAgent String which will be used in SIP headers.
    */
}qmi_ims_iot_get_fw_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Updates IMS Framework configurations at modem
     */
typedef struct {

  /* Optional */
  /*  User Agent String */
  uint8_t User_Agent_valid;  /**< Must be set to true if User_Agent is being passed */
  char User_Agent[IMS_IOT_USER_AGENT_LEN_V01 + 1];
  /**<   UserAgent String which will be used in SIP headers.
    */
}qmi_ims_iot_set_fw_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Updates IMS Framework configurations at modem
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
/** Indication Message; Indicates when Framework Configurations gets changed */
typedef struct {

  /* Optional */
  /*  User Agent String */
  uint8_t User_Agent_valid;  /**< Must be set to true if User_Agent is being passed */
  char User_Agent[IMS_IOT_USER_AGENT_LEN_V01 + 1];
  /**<   UserAgent String which will be used in SIP headers.
    */
}qmi_ims_iot_fw_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Fetch IMS SIP Configurations from Modem. */
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
/** Response Message; Fetch IMS SIP Configurations from Modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  SIP Timer T1 is millisecs */
  uint8_t SIP_Timer_T1_valid;  /**< Must be set to true if SIP_Timer_T1 is being passed */
  uint32_t SIP_Timer_T1;
  /**<    Timer T1 has valid range from 500 to 3000
    */

  /* Optional */
  /*  SIP Timer T2 is secs */
  uint8_t SIP_Timer_T2_valid;  /**< Must be set to true if SIP_Timer_T2 is being passed */
  uint32_t SIP_Timer_T2;
  /**<    Timer T2 has valid range from 4 to 16
    */

  /* Optional */
  /*  SIP Timer T4 is secs  */
  uint8_t SIP_Timer_T4_valid;  /**< Must be set to true if SIP_Timer_T4 is being passed */
  uint32_t SIP_Timer_T4;
  /**<    Timer T4 has valid range from 5 to 17
    */

  /* Optional */
  /*  SIP Timer TA is millisecs  */
  uint8_t SIP_Timer_TA_valid;  /**< Must be set to true if SIP_Timer_TA is being passed */
  uint32_t SIP_Timer_TA;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TB is secs  */
  uint8_t SIP_Timer_TB_valid;  /**< Must be set to true if SIP_Timer_TB is being passed */
  uint32_t SIP_Timer_TB;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TC is secs  */
  uint8_t SIP_Timer_TC_valid;  /**< Must be set to true if SIP_Timer_TC is being passed */
  uint32_t SIP_Timer_TC;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TD is secs  */
  uint8_t SIP_Timer_TD_valid;  /**< Must be set to true if SIP_Timer_TD is being passed */
  uint32_t SIP_Timer_TD;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TE is millisecs  */
  uint8_t SIP_Timer_TE_valid;  /**< Must be set to true if SIP_Timer_TE is being passed */
  uint32_t SIP_Timer_TE;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TF is secs  */
  uint8_t SIP_Timer_TF_valid;  /**< Must be set to true if SIP_Timer_TF is being passed */
  uint32_t SIP_Timer_TF;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TG is secs  */
  uint8_t SIP_Timer_TG_valid;  /**< Must be set to true if SIP_Timer_TG is being passed */
  uint32_t SIP_Timer_TG;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TI is secs  */
  uint8_t SIP_Timer_TI_valid;  /**< Must be set to true if SIP_Timer_TI is being passed */
  uint32_t SIP_Timer_TI;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TJ is secs  */
  uint8_t SIP_Timer_TJ_valid;  /**< Must be set to true if SIP_Timer_TJ is being passed */
  uint32_t SIP_Timer_TJ;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TK is secs  */
  uint8_t SIP_Timer_TK_valid;  /**< Must be set to true if SIP_Timer_TK is being passed */
  uint32_t SIP_Timer_TK;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer Response retry  */
  uint8_t SIP_Timer_RelResponse_Retry_valid;  /**< Must be set to true if SIP_Timer_RelResponse_Retry is being passed */
  uint32_t SIP_Timer_RelResponse_Retry;
  /**<    Timer to retransmit Invite provisional response.
          There is no default value for this NV.
    */

  /* Optional */
  /*  SIP Stop Timer B on 1xx */
  uint8_t SIP_Stop_TimerB_On1xx_valid;  /**< Must be set to true if SIP_Stop_TimerB_On1xx is being passed */
  uint32_t SIP_Stop_TimerB_On1xx;
  /**<    Used as toggle to stop timerB on 1xx reception.
          default value is 0
    */

  /* Optional */
  /*  SIP SIP Listening Port */
  uint8_t SIP_Listening_Port_valid;  /**< Must be set to true if SIP_Listening_Port is being passed */
  uint32_t SIP_Listening_Port;
  /**<    SIP UE server port number.
          default value is 5060
    */

  /* Optional */
  /*  SIP TCP Max Backoff timer */
  uint8_t SIP_TCP_MaxBackOff_Timer_valid;  /**< Must be set to true if SIP_TCP_MaxBackOff_Timer is being passed */
  uint32_t SIP_TCP_MaxBackOff_Timer;
  /**<    SIP TCP Max Backoff timer, there is no default value for this NV.
    */

  /* Optional */
  /*  SIP Enable TCP threshold  */
  uint8_t SIP_Enable_TCP_Threshold_valid;  /**< Must be set to true if SIP_Enable_TCP_Threshold is being passed */
  uint32_t SIP_Enable_TCP_Threshold;
  /**<   Enable or disable TCP threshold.
     Values:
       - 1  -- Enabled
       - 0  -- Disabled
    */

  /* Optional */
  /*  SIP TCP threshold value */
  uint8_t SIP_TCP_Threshold_valid;  /**< Must be set to true if SIP_TCP_Threshold is being passed */
  uint32_t SIP_TCP_Threshold;
  /**<    SIP TCP threshold value
          when set to 0, TCP transport will be disabled.
          Non-Zero value will be used as MSS.
    */

  /* Optional */
  /*  SIP Enable Transport switch */
  uint8_t SIP_Enable_Transport_Switch_valid;  /**< Must be set to true if SIP_Enable_Transport_Switch is being passed */
  uint32_t SIP_Enable_Transport_Switch;
  /**<   Enable or disable SIP Transport Switch from TCP to UDP
     Values:
       - 1  -- Enabled
       - 0  -- Disabled
    */

  /* Optional */
  /*  SIP IPsec scheme  */
  uint8_t SIP_IPSec_Int_Scheme_valid;  /**< Must be set to true if SIP_IPSec_Int_Scheme is being passed */
  uint32_t SIP_IPSec_Int_Scheme;
  /**<   SIP IPsec Integrity algorithm 
         Values:
         1 -- hmac-sha-1-96
         2 -- hmac-md5-96			
         3 -- hmac-sha-1-96 and hmac-md5-96			
    */

  /* Optional */
  /*  SIP IPsec Encoder algo */
  uint8_t SIP_IPSec_Enc_Algo_valid;  /**< Must be set to true if SIP_IPSec_Enc_Algo is being passed */
  uint32_t SIP_IPSec_Enc_Algo;
  /**<   SIP IPsec Encryption algorithm bitmask 
         Values:
         1 -- NULL 
         2 -- aes-cbc
         3 -- NULL and aes-cbc
         4 -- des-ede3-cbc
         5 -- NULL and des-ede3-cbc
         6 -- aes-cbc and des-ede3-cbc
         7 -- NULL, aes-cbc and des-ede3-cbc
    */

  /* Optional */
  /*  SIP Enable Compact header */
  uint8_t SIP_Enable_Compact_Header_valid;  /**< Must be set to true if SIP_Enable_Compact_Header is being passed */
  uint32_t SIP_Enable_Compact_Header;
  /**<   Enable or disable compact header in SIP.
     Values:
       - 1  -- Enabled
       - 0  -- Disabled
    */

  /* Optional */
  /*  Enable Sip instance  */
  uint8_t SIP_Enable_Sip_Instance_valid;  /**< Must be set to true if SIP_Enable_Sip_Instance is being passed */
  uint32_t SIP_Enable_Sip_Instance;
  /**<   Enable or disable add SIP instance in contact header
     Values:
       - 1  -- Enabled
       - 0  -- Disabled
    */

  /* Optional */
  /*  SIP Enable route header  */
  uint8_t SIP_Enable_Route_Header_valid;  /**< Must be set to true if SIP_Enable_Route_Header is being passed */
  uint32_t SIP_Enable_Route_Header;
  /**<   Enable or disable SIP Route Header.
     Values:
       - 1  -- Enabled
       - 0  -- Disabled
    */

  /* Optional */
  /*  SIP Enable IMSI in contact header */
  uint8_t SIP_Enable_Imsi_In_Contact_valid;  /**< Must be set to true if SIP_Enable_Imsi_In_Contact is being passed */
  uint32_t SIP_Enable_Imsi_In_Contact;
  /**<   Enable or disable IMSI in SIP Contact header.
     Values:
       - 1  -- Enabled
       - 0  -- Disabled
    */

  /* Optional */
  /*  SIP Enable Keep Alive */
  uint8_t SIP_Enable_Keep_Alive_valid;  /**< Must be set to true if SIP_Enable_Keep_Alive is being passed */
  uint32_t SIP_Enable_Keep_Alive;
  /**<   Enable or disable SIP Keep Alive packet.
     Values:
       - 1  -- Enabled
       - 0  -- Disabled
    */
}qmi_ims_iot_get_sip_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Set IMS SIP Configurations at modem. */
typedef struct {

  /* Optional */
  /*  SIP Timer T1 is millisecs */
  uint8_t SIP_Timer_T1_valid;  /**< Must be set to true if SIP_Timer_T1 is being passed */
  uint32_t SIP_Timer_T1;
  /**<    Timer T1 has valid range from 500 to 3000
    */

  /* Optional */
  /*  SIP Timer T2 is secs */
  uint8_t SIP_Timer_T2_valid;  /**< Must be set to true if SIP_Timer_T2 is being passed */
  uint32_t SIP_Timer_T2;
  /**<    Timer T2 has valid range from 4 to 16
    */

  /* Optional */
  /*  SIP Timer T4 is secs  */
  uint8_t SIP_Timer_T4_valid;  /**< Must be set to true if SIP_Timer_T4 is being passed */
  uint32_t SIP_Timer_T4;
  /**<    Timer T4 has valid range from 5 to 17
    */

  /* Optional */
  /*  SIP Timer TA is millisecs  */
  uint8_t SIP_Timer_TA_valid;  /**< Must be set to true if SIP_Timer_TA is being passed */
  uint32_t SIP_Timer_TA;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TB is secs  */
  uint8_t SIP_Timer_TB_valid;  /**< Must be set to true if SIP_Timer_TB is being passed */
  uint32_t SIP_Timer_TB;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TC is secs  */
  uint8_t SIP_Timer_TC_valid;  /**< Must be set to true if SIP_Timer_TC is being passed */
  uint32_t SIP_Timer_TC;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TD is secs  */
  uint8_t SIP_Timer_TD_valid;  /**< Must be set to true if SIP_Timer_TD is being passed */
  uint32_t SIP_Timer_TD;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TE is millisecs  */
  uint8_t SIP_Timer_TE_valid;  /**< Must be set to true if SIP_Timer_TE is being passed */
  uint32_t SIP_Timer_TE;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TF is secs  */
  uint8_t SIP_Timer_TF_valid;  /**< Must be set to true if SIP_Timer_TF is being passed */
  uint32_t SIP_Timer_TF;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TG is secs  */
  uint8_t SIP_Timer_TG_valid;  /**< Must be set to true if SIP_Timer_TG is being passed */
  uint32_t SIP_Timer_TG;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TI is secs  */
  uint8_t SIP_Timer_TI_valid;  /**< Must be set to true if SIP_Timer_TI is being passed */
  uint32_t SIP_Timer_TI;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TJ is secs  */
  uint8_t SIP_Timer_TJ_valid;  /**< Must be set to true if SIP_Timer_TJ is being passed */
  uint32_t SIP_Timer_TJ;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TK is secs  */
  uint8_t SIP_Timer_TK_valid;  /**< Must be set to true if SIP_Timer_TK is being passed */
  uint32_t SIP_Timer_TK;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer Response retry  */
  uint8_t SIP_Timer_RelResponse_Retry_valid;  /**< Must be set to true if SIP_Timer_RelResponse_Retry is being passed */
  uint32_t SIP_Timer_RelResponse_Retry;
  /**<    Timer to retransmit Invite provisional response.
          There is no default value for this NV.
    */

  /* Optional */
  /*  SIP Stop Timer B on 1xx */
  uint8_t SIP_Stop_TimerB_On1xx_valid;  /**< Must be set to true if SIP_Stop_TimerB_On1xx is being passed */
  uint32_t SIP_Stop_TimerB_On1xx;
  /**<    Used as toggle to stop timerB on 1xx reception.
          default value is 0
    */

  /* Optional */
  /*  SIP SIP Listening Port */
  uint8_t SIP_Listening_Port_valid;  /**< Must be set to true if SIP_Listening_Port is being passed */
  uint32_t SIP_Listening_Port;
  /**<    SIP UE server port number.
          default value is 5060
    */

  /* Optional */
  /*  SIP TCP Max Backoff timer */
  uint8_t SIP_TCP_MaxBackOff_Timer_valid;  /**< Must be set to true if SIP_TCP_MaxBackOff_Timer is being passed */
  uint32_t SIP_TCP_MaxBackOff_Timer;
  /**<    SIP TCP Max Backoff timer, there is no default value for this NV.
    */

  /* Optional */
  /*  SIP Enable TCP threshold  */
  uint8_t SIP_Enable_TCP_Threshold_valid;  /**< Must be set to true if SIP_Enable_TCP_Threshold is being passed */
  uint32_t SIP_Enable_TCP_Threshold;
  /**<   Enable or disable TCP threshold.
     Values:
       - 1  -- Enable
       - 0  -- Disable
       only above values are supported, anything other than this might be treated as enable.
    */

  /* Optional */
  /*  SIP TCP threshold value */
  uint8_t SIP_TCP_Threshold_valid;  /**< Must be set to true if SIP_TCP_Threshold is being passed */
  uint32_t SIP_TCP_Threshold;
  /**<    SIP TCP threshold value
          when set to 0, TCP transport will be disabled.
          Non-Zero value will be used as MSS.
    */

  /* Optional */
  /*  SIP Enable Transport switch */
  uint8_t SIP_Enable_Transport_Switch_valid;  /**< Must be set to true if SIP_Enable_Transport_Switch is being passed */
  uint32_t SIP_Enable_Transport_Switch;
  /**<   Enable or disable SIP Transport Switch from TCP to UDP
     Values:
       - 1  -- Enabled
       - 0  -- Disabled
       only above values are supported, anything other than this might be treated as enable.
    */

  /* Optional */
  /*  SIP IPsec scheme  */
  uint8_t SIP_IPSec_Int_Scheme_valid;  /**< Must be set to true if SIP_IPSec_Int_Scheme is being passed */
  uint32_t SIP_IPSec_Int_Scheme;
  /**<   SIP IPsec Integrity algorithm 
         Values:
         1 -- hmac-sha-1-96
         2 -- hmac-md5-96			
         3 -- hmac-sha-1-96 and hmac-md5-96			
    */

  /* Optional */
  /*  SIP IPsec Encoder algo */
  uint8_t SIP_IPSec_Enc_Algo_valid;  /**< Must be set to true if SIP_IPSec_Enc_Algo is being passed */
  uint32_t SIP_IPSec_Enc_Algo;
  /**<   SIP IPsec Encryption algorithm bitmask 
         Values:
         1 -- NULL 
         2 -- aes-cbc
         3 -- NULL and aes-cbc
         4 -- des-ede3-cbc
         5 -- NULL and des-ede3-cbc
         6 -- aes-cbc and des-ede3-cbc
         7 -- NULL, aes-cbc and des-ede3-cbc
    */

  /* Optional */
  /*  SIP Enable Compact header */
  uint8_t SIP_Enable_Compact_Header_valid;  /**< Must be set to true if SIP_Enable_Compact_Header is being passed */
  uint32_t SIP_Enable_Compact_Header;
  /**<   Enable or disable compact header in SIP.
     Values:
       - 1  -- Enable
       - 0  -- Disable
       only above values are supported, anything other than this might be treated as enable.
    */

  /* Optional */
  /*  Enable Sip instance  */
  uint8_t SIP_Enable_Sip_Instance_valid;  /**< Must be set to true if SIP_Enable_Sip_Instance is being passed */
  uint32_t SIP_Enable_Sip_Instance;
  /**<   Enable or disable SIP instance.
     Values:
       - 1  -- Enable
       - 0  -- Disable
       only above values are supported, anything other than this might be treated as enable.
    */

  /* Optional */
  /*  SIP Enable route header  */
  uint8_t SIP_Enable_Route_Header_valid;  /**< Must be set to true if SIP_Enable_Route_Header is being passed */
  uint32_t SIP_Enable_Route_Header;
  /**<   Enable or disable SIP Route Header.
     Values:
       - 1  -- Enable
       - 0  -- Disable
       only above values are supported, anything other than this might be treated as enable.
    */

  /* Optional */
  /*  SIP Enable IMSI in contact header */
  uint8_t SIP_Enable_Imsi_In_Contact_valid;  /**< Must be set to true if SIP_Enable_Imsi_In_Contact is being passed */
  uint32_t SIP_Enable_Imsi_In_Contact;
  /**<   Enable or disable IMSI in SIP Contact header.
     Values:
       - 1  -- Enable
       - 0 -- Disable
       only above values are supported, anything other than this might be treated as enable.
    */

  /* Optional */
  /*  SIP Enable Keep Alive */
  uint8_t SIP_Enable_Keep_Alive_valid;  /**< Must be set to true if SIP_Enable_Keep_Alive is being passed */
  uint32_t SIP_Enable_Keep_Alive;
  /**<   Enable or disable SIP Keep Alive packet.
     Values:
       - 1  -- Enable
       - 0  -- Disable
       only above values are supported, anything other than this might be treated as enable.
    */
}qmi_ims_iot_set_sip_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Set IMS SIP Configurations at modem. */
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
/** Indication Message; Indicates when SIP related Configurations gets changed */
typedef struct {

  /* Optional */
  /*  SIP Timer T1 is millisecs */
  uint8_t SIP_Timer_T1_valid;  /**< Must be set to true if SIP_Timer_T1 is being passed */
  uint32_t SIP_Timer_T1;
  /**<    Timer T1 has valid range from 500 to 3000
    */

  /* Optional */
  /*  SIP Timer T2 is secs */
  uint8_t SIP_Timer_T2_valid;  /**< Must be set to true if SIP_Timer_T2 is being passed */
  uint32_t SIP_Timer_T2;
  /**<    Timer T2 has valid range from 4 to 16
    */

  /* Optional */
  /*  SIP Timer T4 is secs  */
  uint8_t SIP_Timer_T4_valid;  /**< Must be set to true if SIP_Timer_T4 is being passed */
  uint32_t SIP_Timer_T4;
  /**<    Timer T4 has valid range from 5 to 17
    */

  /* Optional */
  /*  SIP Timer TA is millisecs  */
  uint8_t SIP_Timer_TA_valid;  /**< Must be set to true if SIP_Timer_TA is being passed */
  uint32_t SIP_Timer_TA;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TB is secs  */
  uint8_t SIP_Timer_TB_valid;  /**< Must be set to true if SIP_Timer_TB is being passed */
  uint32_t SIP_Timer_TB;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TC is secs  */
  uint8_t SIP_Timer_TC_valid;  /**< Must be set to true if SIP_Timer_TC is being passed */
  uint32_t SIP_Timer_TC;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TD is secs  */
  uint8_t SIP_Timer_TD_valid;  /**< Must be set to true if SIP_Timer_TD is being passed */
  uint32_t SIP_Timer_TD;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TE is millisecs  */
  uint8_t SIP_Timer_TE_valid;  /**< Must be set to true if SIP_Timer_TE is being passed */
  uint32_t SIP_Timer_TE;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TF is secs  */
  uint8_t SIP_Timer_TF_valid;  /**< Must be set to true if SIP_Timer_TF is being passed */
  uint32_t SIP_Timer_TF;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TG is secs  */
  uint8_t SIP_Timer_TG_valid;  /**< Must be set to true if SIP_Timer_TG is being passed */
  uint32_t SIP_Timer_TG;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TI is secs  */
  uint8_t SIP_Timer_TI_valid;  /**< Must be set to true if SIP_Timer_TI is being passed */
  uint32_t SIP_Timer_TI;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TJ is secs  */
  uint8_t SIP_Timer_TJ_valid;  /**< Must be set to true if SIP_Timer_TJ is being passed */
  uint32_t SIP_Timer_TJ;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer TK is secs  */
  uint8_t SIP_Timer_TK_valid;  /**< Must be set to true if SIP_Timer_TK is being passed */
  uint32_t SIP_Timer_TK;
  /**<    if not set, default timer value will be as per RFC
    */

  /* Optional */
  /*  SIP Timer Response retry  */
  uint8_t SIP_Timer_RelResponse_Retry_valid;  /**< Must be set to true if SIP_Timer_RelResponse_Retry is being passed */
  uint32_t SIP_Timer_RelResponse_Retry;
  /**<    Timer to retransmit Invite provisional response.
          There is no default value for this NV.
    */

  /* Optional */
  /*  SIP Stop Timer B on 1xx */
  uint8_t SIP_Stop_TimerB_On1xx_valid;  /**< Must be set to true if SIP_Stop_TimerB_On1xx is being passed */
  uint32_t SIP_Stop_TimerB_On1xx;
  /**<    Used as toggle to stop timerB on 1xx reception.
          default value is 0
    */

  /* Optional */
  /*  SIP Listening Port */
  uint8_t SIP_Listening_Port_valid;  /**< Must be set to true if SIP_Listening_Port is being passed */
  uint32_t SIP_Listening_Port;
  /**<    SIP UE server port number.
          default value is 5060
    */

  /* Optional */
  /*  SIP TCP Max Backoff timer */
  uint8_t SIP_TCP_MaxBackOff_Timer_valid;  /**< Must be set to true if SIP_TCP_MaxBackOff_Timer is being passed */
  uint32_t SIP_TCP_MaxBackOff_Timer;
  /**<    SIP TCP Max Backoff timer, there is no default value for this NV.
    */

  /* Optional */
  /*  SIP Enable TCP threshold  */
  uint8_t SIP_Enable_TCP_Threshold_valid;  /**< Must be set to true if SIP_Enable_TCP_Threshold is being passed */
  uint32_t SIP_Enable_TCP_Threshold;
  /**<   Enable or disable TCP threshold.
     Values:
       - 1  -- Enabled
       - 0  -- Disabled
    */

  /* Optional */
  /*  SIP TCP threshold value */
  uint8_t SIP_TCP_Threshold_valid;  /**< Must be set to true if SIP_TCP_Threshold is being passed */
  uint32_t SIP_TCP_Threshold;
  /**<    SIP TCP threshold value
          when set to 0, TCP transport will be disabled.
          Non-Zero value will be used as MSS.
    */

  /* Optional */
  /*  SIP Enable Transport switch */
  uint8_t SIP_Enable_Transport_Switch_valid;  /**< Must be set to true if SIP_Enable_Transport_Switch is being passed */
  uint32_t SIP_Enable_Transport_Switch;
  /**<   Enable or disable SIP Transport Switch from TCP to UDP
     Values:
       - 1  -- Enabled
       - 0  -- Disabled
    */

  /* Optional */
  /*  SIP IPsec Integrity algorithm  */
  uint8_t SIP_IPSec_Int_Scheme_valid;  /**< Must be set to true if SIP_IPSec_Int_Scheme is being passed */
  uint32_t SIP_IPSec_Int_Scheme;
  /**<   SIP IPsec Integrity algorithm 
         Values:
         1 -- hmac-sha-1-96
         2 -- hmac-md5-96			
         3 -- hmac-sha-1-96 and hmac-md5-96			
    */

  /* Optional */
  /*  SIP IPsec Encryption algorithm bitmask  */
  uint8_t SIP_IPSec_Enc_Algo_valid;  /**< Must be set to true if SIP_IPSec_Enc_Algo is being passed */
  uint32_t SIP_IPSec_Enc_Algo;
  /**<   SIP IPsec Encryption algorithm bitmask 
         Values:
         1 -- NULL 
         2 -- aes-cbc
         3 -- NULL and aes-cbc
         4 -- des-ede3-cbc
         5 -- NULL and des-ede3-cbc
         6 -- aes-cbc and des-ede3-cbc
         7 -- NULL, aes-cbc and des-ede3-cbc
    */

  /* Optional */
  /*  SIP Enable Compact header */
  uint8_t SIP_Enable_Compact_Header_valid;  /**< Must be set to true if SIP_Enable_Compact_Header is being passed */
  uint32_t SIP_Enable_Compact_Header;
  /**<   Enable or disable compact header in SIP.
     Values:
       - 1  -- Enabled
       - 0  -- Disabled
    */

  /* Optional */
  /*  Enable Sip instance  */
  uint8_t SIP_Enable_Sip_Instance_valid;  /**< Must be set to true if SIP_Enable_Sip_Instance is being passed */
  uint32_t SIP_Enable_Sip_Instance;
  /**<   Enable or disable SIP instance.
     Values:
       - 1  -- Enabled
       - 0  -- Disabled
    */

  /* Optional */
  /*  SIP Enable route header  */
  uint8_t SIP_Enable_Route_Header_valid;  /**< Must be set to true if SIP_Enable_Route_Header is being passed */
  uint32_t SIP_Enable_Route_Header;
  /**<   Enable or disable SIP Route Header.
     Values:
       - 1  -- Enabled
       - 0  -- Disabled
    */

  /* Optional */
  /*  SIP Enable IMSI in contact header */
  uint8_t SIP_Enable_Imsi_In_Contact_valid;  /**< Must be set to true if SIP_Enable_Imsi_In_Contact is being passed */
  uint32_t SIP_Enable_Imsi_In_Contact;
  /**<   Enable or disable IMSI in SIP Contact header.
     Values:
       - 1  -- Enabled
       - 0  -- Disabled
    */

  /* Optional */
  /*  SIP Enable Keep Alive */
  uint8_t SIP_Enable_Keep_Alive_valid;  /**< Must be set to true if SIP_Enable_Keep_Alive is being passed */
  uint32_t SIP_Enable_Keep_Alive;
  /**<   Enable or disable SIP Keep Alive packet.
     Values:
       - 1  -- Enabled
       - 0  -- Disabled
    */
}qmi_ims_iot_sip_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Fetch IMS Register Manager related Configurations from modem (service). */
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
/** Response Message; Fetch IMS Register Manager related Configurations from modem (service). */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Authentication Registration Mode */
  uint8_t Auth_Reg_mode_valid;  /**< Must be set to true if Auth_Reg_mode is being passed */
  uint32_t Auth_Reg_mode;
  /**<    Enabling protected registration mode.
          Values:
          1 -- IPSec Enabled mode
          2 -- Unprotected mode
    */

  /* Optional */
  /*  Registration Retry Base time */
  uint8_t Reg_Retry_Base_Time_valid;  /**< Must be set to true if Reg_Retry_Base_Time is being passed */
  uint32_t Reg_Retry_Base_Time;
  /**<     Registration retry base time in seconds.
           If not set, the default is 30.
    */

  /* Optional */
  /*  Registration Retry Max time */
  uint8_t Reg_Retry_Max_Time_valid;  /**< Must be set to true if Reg_Retry_Max_Time is being passed */
  uint32_t Reg_Retry_Max_Time;
  /**<     Registration retry maximum time in seconds.
           If not set, the default is 7200.
    */

  /* Optional */
  /*  PCSCF server Address  */
  uint8_t PCSCF_Server_Address_valid;  /**< Must be set to true if PCSCF_Server_Address is being passed */
  char PCSCF_Server_Address[IMS_IOT_PCSCF_ADDR_LEN_V01 + 1];
  /**<     Registration preconfigured server base address.
           If not set, the default is NULL.
    */

  /* Optional */
  /*  PSCSF Server Port  */
  uint8_t PCSCF_Server_Port_valid;  /**< Must be set to true if PCSCF_Server_Port is being passed */
  uint32_t PCSCF_Server_Port;
  /**<     Allows configuration of the PSCSF port.
           If not set, the default is 5060.
    */
}qmi_ims_iot_get_reg_mgr_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Sets IMS Register Manager related Configurations at modem (service). */
typedef struct {

  /* Optional */
  /*  Authentication Registration Mode */
  uint8_t Auth_Reg_mode_valid;  /**< Must be set to true if Auth_Reg_mode is being passed */
  uint32_t Auth_Reg_mode;
  /**<    Enabling protected registration mode.
          Values:
          1 -- IPSec Enabled mode
          2 -- Unprotected mode
          only above values are supported.
    */

  /* Optional */
  /*  Registration Retry Base time */
  uint8_t Reg_Retry_Base_Time_valid;  /**< Must be set to true if Reg_Retry_Base_Time is being passed */
  uint32_t Reg_Retry_Base_Time;
  /**<     Registration retry base time in seconds.
           If not set, the default is 30.
    */

  /* Optional */
  /*  Registration Retry Max time */
  uint8_t Reg_Retry_Max_Time_valid;  /**< Must be set to true if Reg_Retry_Max_Time is being passed */
  uint32_t Reg_Retry_Max_Time;
  /**<     Registration retry maximum time in seconds.
           If not set, the default is 7200.
    */

  /* Optional */
  /*  PCSCF server Address  */
  uint8_t PCSCF_Server_Address_valid;  /**< Must be set to true if PCSCF_Server_Address is being passed */
  char PCSCF_Server_Address[IMS_IOT_PCSCF_ADDR_LEN_V01 + 1];
  /**<     Registration preconfigured server base address.
           If not set, the default is NULL.
    */

  /* Optional */
  /*  PSCSF Server Port  */
  uint8_t PCSCF_Server_Port_valid;  /**< Must be set to true if PCSCF_Server_Port is being passed */
  uint32_t PCSCF_Server_Port;
  /**<     Allows configuration of the PSCSF port.
           If not set, the default is 5060.
    */
}qmi_ims_iot_set_reg_mgr_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Sets IMS Register Manager related Configurations at modem (service). */
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
/** Indication Message; Indicates when Register Manager related Configurations gets changed */
typedef struct {

  /* Optional */
  /*  Authentication Registration Mode */
  uint8_t Auth_Reg_mode_valid;  /**< Must be set to true if Auth_Reg_mode is being passed */
  uint32_t Auth_Reg_mode;
  /**<    Enabling protected registration mode.
          Values: 
            1 -- IPSec Enabled mode  
            2 -- Unprotected mode
    */

  /* Optional */
  /*  Registration Retry Base time */
  uint8_t Reg_Retry_Base_Time_valid;  /**< Must be set to true if Reg_Retry_Base_Time is being passed */
  uint32_t Reg_Retry_Base_Time;
  /**<     Registration retry base time in seconds.
           If not set, the default is 30.
    */

  /* Optional */
  /*  Registration Retry Max time */
  uint8_t Reg_Retry_Max_Time_valid;  /**< Must be set to true if Reg_Retry_Max_Time is being passed */
  uint32_t Reg_Retry_Max_Time;
  /**<     Registration retry maximum time in seconds.
           If not set, the default is 7200.
    */

  /* Optional */
  /*  PCSCF server Address  */
  uint8_t PCSCF_Server_Address_valid;  /**< Must be set to true if PCSCF_Server_Address is being passed */
  char PCSCF_Server_Address[IMS_IOT_PCSCF_ADDR_LEN_V01 + 1];
  /**<     Registration preconfigured server base address.
           If not set, the default is NULL.
    */

  /* Optional */
  /*  PSCSF Server Port  */
  uint8_t PCSCF_Server_Port_valid;  /**< Must be set to true if PCSCF_Server_Port is being passed */
  uint32_t PCSCF_Server_Port;
  /**<     Allows configuration of the PSCSF port.
           If not set, the default is 5060.
    */
}qmi_ims_iot_reg_mgr_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Fetch IMS Service status Configurations from modem (service). */
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
/** Response Message; Fetch IMS Service status Configurations from modem (service). */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  VoLTE enable/disable . */
  uint8_t VOLTE_Enable_valid;  /**< Must be set to true if VOLTE_Enable is being passed */
  uint32_t VOLTE_Enable;
  /**<    Enable/Disable toggle for VoLTE feature.
          Values: 
          -1 -- Enable  
          -0 -- Disable 
    */

  /* Optional */
  /*  SMS over IMS enable/disable .  */
  uint8_t SMS_Over_IP_Enable_valid;  /**< Must be set to true if SMS_Over_IP_Enable is being passed */
  uint32_t SMS_Over_IP_Enable;
  /**<    Enable/Disable toggle for SMS over IMS feature. 
          Values: 
          -1 -- Enable  
          -0 -- Disable 
    */
}qmi_ims_iot_get_service_status_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Sets IMS Service status Configurations at modem(service). */
typedef struct {

  /* Optional */
  /*  VoLTE enable/disable. */
  uint8_t VOLTE_Enable_valid;  /**< Must be set to true if VOLTE_Enable is being passed */
  uint32_t VOLTE_Enable;
  /**<    Enable/Disable toggle for VoLTE feature.
          Values:
          -1 -- Enable
          -0 -- Disable
          only above values are supported, anything other than this might be treated as enable.
    */

  /* Optional */
  /*  SMS over IMS enable/disable.  */
  uint8_t SMS_Over_IP_Enable_valid;  /**< Must be set to true if SMS_Over_IP_Enable is being passed */
  uint32_t SMS_Over_IP_Enable;
  /**<    Enable/Disable toggle for SMS over IMS feature.
          Values:
          -1 -- Enable
          -0 -- Disable
          only above values are supported, anything other than this might be treated as enable.
    */
}qmi_ims_iot_set_service_status_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Sets IMS Service status Configurations at modem(service). */
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
/** Indication Message; Indicates when IMS Service status gets changed */
typedef struct {

  /* Optional */
  /*  VoLTE enable/disable. */
  uint8_t VOLTE_Enable_valid;  /**< Must be set to true if VOLTE_Enable is being passed */
  uint32_t VOLTE_Enable;
  /**<    Enable/Disable toggle for VoLTE feature.
          Values:
          -1 -- Enable
          -0 -- Disable
    */

  /* Optional */
  /*  SMS over IMS enable/disable.  */
  uint8_t SMS_Over_IP_Enable_valid;  /**< Must be set to true if SMS_Over_IP_Enable is being passed */
  uint32_t SMS_Over_IP_Enable;
  /**<    Enable/Disable toggle for SMS over IMS feature.
          Values:
          -1 -- Enable
          -0 -- Disable
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
/** Request Message; Fetch IMS SMS related Configurations from modem. */
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
/** Response Message; Fetch IMS SMS related Configurations from modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  SMS Format, either 3gpp/3gpp2 */
  uint8_t SMS_Format_valid;  /**< Must be set to true if SMS_Format is being passed */
  ims_iot_sms_format_e_type_v01 SMS_Format;

  /* Optional */
  /*  SMS PSI Value */
  uint8_t Psi_Value_valid;  /**< Must be set to true if Psi_Value is being passed */
  char Psi_Value[IMS_IOT_SMS_PSI_LEN_V01 + 1];
  /**<     PSI string for SMS
           If not set, the default is NULL.
    */
}qmi_ims_iot_get_sms_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Sets IMS SMS Configurations at modem. */
typedef struct {

  /* Optional */
  /*  SMS Format, either 3gpp/3gpp2 */
  uint8_t SMS_Format_valid;  /**< Must be set to true if SMS_Format is being passed */
  ims_iot_sms_format_e_type_v01 SMS_Format;

  /* Optional */
  /*  SMS PSI Value */
  uint8_t Psi_Value_valid;  /**< Must be set to true if Psi_Value is being passed */
  char Psi_Value[IMS_IOT_SMS_PSI_LEN_V01 + 1];
  /**<     PSI string for SMS
           If not set, the default is NULL.
    */
}qmi_ims_iot_set_sms_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Sets IMS SMS Configurations at modem. */
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
/** Indication Message; Indicates when IMS SMS Configuration gets changed */
typedef struct {

  /* Optional */
  /*  SMS Format, either 3gpp/3gpp2 */
  uint8_t SMS_Format_valid;  /**< Must be set to true if SMS_Format is being passed */
  ims_iot_sms_format_e_type_v01 SMS_Format;

  /* Optional */
  /*  SMS PSI Value */
  uint8_t Psi_Value_valid;  /**< Must be set to true if Psi_Value is being passed */
  char Psi_Value[IMS_IOT_SMS_PSI_LEN_V01 + 1];
  /**<     PSI string for SMS
           If not set, the default is NULL.
    */
}qmi_ims_iot_sms_config_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Fetch IMS Voice/VoLTE related Configurations from modem. */
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
/** Response Message; Fetch IMS Voice/VoLTE related Configurations from modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  Display Text Priority */
  uint8_t Display_Text_Priority_valid;  /**< Must be set to true if Display_Text_Priority is being passed */
  uint32_t Display_Text_Priority;
  /**<   Originating party identity determination from PAI or from header for incoming sip messages. Default value is 0
        Values: \n
        - 0 --  PAI\n
        - 1 --  From\n
        - 2 --  Fallback Enabled\n
    */

  /* Optional */
  /*  Caller ID mode */
  uint8_t CallerId_Mode_valid;  /**< Must be set to true if CallerId_Mode is being passed */
  uint32_t CallerId_Mode;
  /**<   Caller id mode to allow presentation/restriction. Default value 16
        Values: \n
        - 0  -- OIP_OIR_NONE  \n
        - 1  -- OIP_MODE  \n
        - 2  -- OIR_PERM_MODE_PRESENTATION_RESTRICTED  \n
        - 4  -- OIR_PERM_MODE_PRESENTATION_NOT_RESTRICTED  \n
        - 8  -- OIR_TEMP_MODE_PRESENTATION_RESTRICTED  \n
        - 16 -- OIR_TEMP_MODE_PRESENTATION_NOT_RESTRICTED  \n
        - 32 -- OIR_TEMP_MODE_PRESENTATION_RESTRICTED_OVERRIDE  \n
    */

  /* Optional */
  /*  SIP Timer TCall in sec */
  uint8_t T_CALL_valid;  /**< Must be set to true if T_CALL is being passed */
  uint32_t T_CALL;
  /**<   Duration of the TCALL timer for outgoing call. Default value is 10.
    */

  /* Optional */
  /*  Ringing Timer in sec */
  uint8_t Ringing_Timer_valid;  /**< Must be set to true if Ringing_Timer is being passed */
  uint32_t Ringing_Timer;
  /**<   Duration of the ringing timer, in seconds. The ringing timer 
         starts on the ringing event. If the call is not answered within 
         the duration of this timer, the call is disconnected.
         Default value is 45 sec.
    */

  /* Optional */
  /*  Ring back timer in sec */
  uint8_t RingBack_Timer_valid;  /**< Must be set to true if RingBack_Timer is being passed */
  uint32_t RingBack_Timer;
  /**<   Duration of the ringback timer, in seconds. The ringback timer
         starts on the ringback event. If the call is not answered within
         the duration of this timer, the call is disconnected.
         Default value is 90 sec.
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
  /*  Session Expiry timer in sec */
  uint8_t Session_Expires_valid;  /**< Must be set to true if Session_Expires is being passed */
  uint32_t Session_Expires;
  /**<   Session duration, in seconds. 
         Default value is 1800 sec
    */

  /* Optional */
  /*  Min Session Expiry timer in sec */
  uint8_t Min_Session_Expires_valid;  /**< Must be set to true if Min_Session_Expires is being passed */
  uint32_t Min_Session_Expires;
  /**<   Minimum allowed value for session timer, in seconds for outgoing INVITE.
         Default value is 1800 sec 
    */

  /* Optional */
  /*  RTCP Link aliveness timer in sec */
  uint8_t Rtcp_Link_Aliveness_Timer_valid;  /**< Must be set to true if Rtcp_Link_Aliveness_Timer is being passed */
  uint32_t Rtcp_Link_Aliveness_Timer;
  /**<   In a connected media session if RTCP is not received for timer value, indication will be published.
         Default value is 20 sec 
    */

  /* Optional */
  /*  AMR Octet Aligned Dynamic PT  */
  uint8_t Amr_OctetAligned_DynamicPt_valid;  /**< Must be set to true if Amr_OctetAligned_DynamicPt is being passed */
  uint32_t Amr_OctetAligned_DynamicPt;
  /**<   Payload type for AMR octate aligned codec. 
         Default value is 104 
    */

  /* Optional */
  /*  AMR Bandwidth Efficient Dynamic PT */
  uint8_t Amr_BandWidthEfficient_DynamicPt_valid;  /**< Must be set to true if Amr_BandWidthEfficient_DynamicPt is being passed */
  uint32_t Amr_BandWidthEfficient_DynamicPt;
  /**<   Payload type for AMR bandwidth efficient codec. 
         Default value is 102 
    */

  /* Optional */
  /*  DTMF NB Dynamic PT */
  uint8_t Dtmf_Nb_DynamicPt_valid;  /**< Must be set to true if Dtmf_Nb_DynamicPt is being passed */
  uint32_t Dtmf_Nb_DynamicPt;
  /**<   Payload type for DTMF narrow band codec. 
         Default value is 96 
    */

  /* Optional */
  /*  AMR Mode Set  */
  uint8_t Amr_ModeSet_valid;  /**< Must be set to true if Amr_ModeSet is being passed */
  uint32_t Amr_ModeSet;
  /**<   Bitmask for AMR NB modes allowed. \n
        Values: \n
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
  /**<   Enable/disable Source Controlled Rate (SCR) for AMR narrowband (NB). 
        Values: \n
        -1 -- Enable \n
        -0 -- Disable  
    */

  /* Optional */
  /*  Speech Start Port  */
  uint8_t Speech_Start_Port_valid;  /**< Must be set to true if Speech_Start_Port is being passed */
  uint32_t Speech_Start_Port;
  /**<   
         Smallest RTP port number for a speech codec.
         If not set, the default value is 50000.
    */

  /* Optional */
  /*  Speech Stop Port  */
  uint8_t Speech_Stop_Port_valid;  /**< Must be set to true if Speech_Stop_Port is being passed */
  uint32_t Speech_Stop_Port;
  /**<   
         Largest RTP port number for a speech codec.
         If not set, the default value is 50002.
    */
}qmi_ims_iot_get_voice_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Sets IMS Voice/VoLTE related Configurations at modem. */
typedef struct {

  /* Optional */
  /*  Display Text Priority */
  uint8_t Display_Text_Priority_valid;  /**< Must be set to true if Display_Text_Priority is being passed */
  uint32_t Display_Text_Priority;
  /**<   Originating party identity determination from PAI or from header for incoming sip messages. Default value is 0
        Values: \n
        - 0 --  PAI\n
        - 1 --  From\n
        - 2 --  Fallback Enabled \n
        only above values are supported.
    */

  /* Optional */
  /*  Caller ID mode */
  uint8_t CallerId_Mode_valid;  /**< Must be set to true if CallerId_Mode is being passed */
  uint32_t CallerId_Mode;
  /**<   Caller id mode to allow presentation/restriction. Default value 16
        Values: \n
        - 0  -- OIP_OIR_NONE  \n
        - 1  -- OIP_MODE  \n
        - 2  -- OIR_PERM_MODE_PRESENTATION_RESTRICTED  \n
        - 4  -- OIR_PERM_MODE_PRESENTATION_NOT_RESTRICTED  \n
        - 8  -- OIR_TEMP_MODE_PRESENTATION_RESTRICTED  \n
        - 16 -- OIR_TEMP_MODE_PRESENTATION_NOT_RESTRICTED  \n
        - 32 -- OIR_TEMP_MODE_PRESENTATION_RESTRICTED_OVERRIDE  \n
        only above values are supported.
    */

  /* Optional */
  /*  SIP Timer TCall in sec */
  uint8_t T_CALL_valid;  /**< Must be set to true if T_CALL is being passed */
  uint32_t T_CALL;
  /**<   Duration of the TCALL timer for outgoing call. Default value is 10.
    */

  /* Optional */
  /*  Ringing Timer in sec */
  uint8_t Ringing_Timer_valid;  /**< Must be set to true if Ringing_Timer is being passed */
  uint32_t Ringing_Timer;
  /**<   Duration of the ringing timer, in seconds. The ringing timer 
         starts on the ringing event. If the call is not answered within 
         the duration of this timer, the call is disconnected.
         Default value is 45 sec.
    */

  /* Optional */
  /*  Ring back timer in sec */
  uint8_t RingBack_Timer_valid;  /**< Must be set to true if RingBack_Timer is being passed */
  uint32_t RingBack_Timer;
  /**<   Duration of the ringback timer, in seconds. The ringback timer
         starts on the ringback event. If the call is not answered within
         the duration of this timer, the call is disconnected.
         Default value is 90 sec.
    */

  /* Optional */
  /*  Session Timer Enable */
  uint8_t Session_Timer_Enabled_valid;  /**< Must be set to true if Session_Timer_Enabled is being passed */
  uint32_t Session_Timer_Enabled;
  /**<   Indicates whether the VoIP session is timer enabled. Default value is 1.
         Values: \n
          -1 -- Enable \n
          -0 -- Disable \n
          only above values are supported, anything other than this might be treated as enable.    
    */

  /* Optional */
  /*  Session Expiry timer in sec */
  uint8_t Session_Expires_valid;  /**< Must be set to true if Session_Expires is being passed */
  uint32_t Session_Expires;
  /**<   Session duration, in seconds. 
         Default value is 1800 sec
    */

  /* Optional */
  /*  Min Session Expiry timer in sec */
  uint8_t Min_Session_Expires_valid;  /**< Must be set to true if Min_Session_Expires is being passed */
  uint32_t Min_Session_Expires;
  /**<   Minimum allowed value for session timer, in seconds for outgoing INVITE.
         Default value is 1800 sec 
    */

  /* Optional */
  /*  RTCP Link aliveness timer in sec */
  uint8_t Rtcp_Link_Aliveness_Timer_valid;  /**< Must be set to true if Rtcp_Link_Aliveness_Timer is being passed */
  uint32_t Rtcp_Link_Aliveness_Timer;
  /**<   In a connected media session if RTCP is not received for timer value, indication will be published.
         Default value is 20 sec 
    */

  /* Optional */
  /*  AMR Octet Aligned Dynamic PT  */
  uint8_t Amr_OctetAligned_DynamicPt_valid;  /**< Must be set to true if Amr_OctetAligned_DynamicPt is being passed */
  uint32_t Amr_OctetAligned_DynamicPt;
  /**<   Payload type for AMR octate aligned codec. 
         Default value is 104 
    */

  /* Optional */
  /*  AMR Bandwidth Efficient Dynamic PT */
  uint8_t Amr_BandWidthEfficient_DynamicPt_valid;  /**< Must be set to true if Amr_BandWidthEfficient_DynamicPt is being passed */
  uint32_t Amr_BandWidthEfficient_DynamicPt;
  /**<   Payload type for AMR bandwidth efficient codec. 
         Default value is 102 
    */

  /* Optional */
  /*  DTMF NB Dynamic PT */
  uint8_t Dtmf_Nb_DynamicPt_valid;  /**< Must be set to true if Dtmf_Nb_DynamicPt is being passed */
  uint32_t Dtmf_Nb_DynamicPt;
  /**<   Payload type for DTMF narrow band codec. 
         Default value is 96 
    */

  /* Optional */
  /*  AMR Mode Set  */
  uint8_t Amr_ModeSet_valid;  /**< Must be set to true if Amr_ModeSet is being passed */
  uint32_t Amr_ModeSet;
  /**<   Bitmask for AMR NB modes allowed. \n
        Values: \n
        - 0x1 -- 4.75 kbps \n
        - 0x2 -- 5.15 kbps \n
        - 0x4 -- 5.9 kbps \n
        - 0x8 -- 6.17 kbps \n
        - 0x10 -- 7.4 kbps \n
        - 0x20 -- 7.95 kbps \n
        - 0x40 -- 10.2 kbps \n
        - 0x80 -- 12.2 kbps \n
        only above values are supported.
    */

  /* Optional */
  /*  SCR AMR Enable  */
  uint8_t Scr_AMR_Enable_valid;  /**< Must be set to true if Scr_AMR_Enable is being passed */
  uint32_t Scr_AMR_Enable;
  /**<   Enable/disable Source Controlled Rate (SCR) for AMR narrowband (NB). \n
        Values: \n
        -1 -- Enable \n
        -0 -- Disable \n
        only above values are supported, anything other than this might be treated as enable.
    */

  /* Optional */
  /*  Speech Start Port  */
  uint8_t Speech_Start_Port_valid;  /**< Must be set to true if Speech_Start_Port is being passed */
  uint32_t Speech_Start_Port;
  /**<   
         Smallest RTP port number for a speech codec.
         If not set, the default value is 50000.
    */

  /* Optional */
  /*  Speech Stop Port  */
  uint8_t Speech_Stop_Port_valid;  /**< Must be set to true if Speech_Stop_Port is being passed */
  uint32_t Speech_Stop_Port;
  /**<   
         Largest RTP port number for a speech codec.
         If not set, the default value is 50002.
    */
}qmi_ims_iot_set_voice_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Response Message; Sets IMS Voice/VoLTE related Configurations at modem. */
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
/** Indication Message; Indicates when IMS Voice/VoLTE Configuration gets changed */
typedef struct {

  /* Optional */
  /*  Display Text Priority */
  uint8_t Display_Text_Priority_valid;  /**< Must be set to true if Display_Text_Priority is being passed */
  uint32_t Display_Text_Priority;
  /**<   Originating party identity determination from PAI or from header for incoming sip messages. Default value is 0
        Values: \n
        - 0 --  PAI\n
        - 1 --  From\n
        - 2 --  Fallback Enabled\n
    */

  /* Optional */
  /*  Caller ID mode */
  uint8_t CallerId_Mode_valid;  /**< Must be set to true if CallerId_Mode is being passed */
  uint32_t CallerId_Mode;
  /**<   Caller id mode to allow presentation/restriction. Default value 16
        Values: \n
        - 0  -- OIP_OIR_NONE  \n
        - 1  -- OIP_MODE  \n
        - 2  -- OIR_PERM_MODE_PRESENTATION_RESTRICTED  \n
        - 4  -- OIR_PERM_MODE_PRESENTATION_NOT_RESTRICTED  \n
        - 8  -- OIR_TEMP_MODE_PRESENTATION_RESTRICTED  \n
        - 16 -- OIR_TEMP_MODE_PRESENTATION_NOT_RESTRICTED  \n
        - 32 -- OIR_TEMP_MODE_PRESENTATION_RESTRICTED_OVERRIDE  \n
    */

  /* Optional */
  /*  SIP Timer TCall in sec */
  uint8_t T_CALL_valid;  /**< Must be set to true if T_CALL is being passed */
  uint32_t T_CALL;
  /**<   Duration of the TCALL timer for outgoing call. Default value is 10.
    */

  /* Optional */
  /*  Ringing Timer in sec */
  uint8_t Ringing_Timer_valid;  /**< Must be set to true if Ringing_Timer is being passed */
  uint32_t Ringing_Timer;
  /**<   Duration of the ringing timer, in seconds. The ringing timer 
         starts on the ringing event. If the call is not answered within 
         the duration of this timer, the call is disconnected.
         Default value is 45 sec.
    */

  /* Optional */
  /*  Ring back timer in sec */
  uint8_t RingBack_Timer_valid;  /**< Must be set to true if RingBack_Timer is being passed */
  uint32_t RingBack_Timer;
  /**<   Duration of the ringback timer, in seconds. The ringback timer
         starts on the ringback event. If the call is not answered within
         the duration of this timer, the call is disconnected.
         Default value is 90 sec.
    */

  /* Optional */
  /*  Session Timer Enable */
  uint8_t Session_Timer_Enabled_valid;  /**< Must be set to true if Session_Timer_Enabled is being passed */
  uint32_t Session_Timer_Enabled;
  /**<   Indicates whether the VoIP session is timer enabled. Default value is 1.
         Values: \n
          -1 -- Enable \n
          -0 -- Disable \n 
    */

  /* Optional */
  /*  Session Expiry timer in sec */
  uint8_t Session_Expires_valid;  /**< Must be set to true if Session_Expires is being passed */
  uint32_t Session_Expires;
  /**<   Session duration, in seconds. 
         Default value is 1800 sec
    */

  /* Optional */
  /*  Min Session Expiry timer in sec */
  uint8_t Min_Session_Expires_valid;  /**< Must be set to true if Min_Session_Expires is being passed */
  uint32_t Min_Session_Expires;
  /**<   Minimum allowed value for session timer, in seconds for outgoing INVITE.
         Default value is 1800 sec 
    */

  /* Optional */
  /*  RTCP Link aliveness timer in sec */
  uint8_t Rtcp_Link_Aliveness_Timer_valid;  /**< Must be set to true if Rtcp_Link_Aliveness_Timer is being passed */
  uint32_t Rtcp_Link_Aliveness_Timer;
  /**<   In a connected media session if RTCP is not received for timer value, indication will be published.
         Default value is 20 sec 
    */

  /* Optional */
  /*  AMR Octet Aligned Dynamic PT  */
  uint8_t Amr_OctetAligned_DynamicPt_valid;  /**< Must be set to true if Amr_OctetAligned_DynamicPt is being passed */
  uint32_t Amr_OctetAligned_DynamicPt;
  /**<   Payload type for AMR octate aligned codec. 
         Default value is 104 
    */

  /* Optional */
  /*  AMR Bandwidth Efficient Dynamic PT */
  uint8_t Amr_BandWidthEfficient_DynamicPt_valid;  /**< Must be set to true if Amr_BandWidthEfficient_DynamicPt is being passed */
  uint32_t Amr_BandWidthEfficient_DynamicPt;
  /**<   Payload type for AMR bandwidth efficient codec. 
         Default value is 102 
    */

  /* Optional */
  /*  DTMF NB Dynamic PT */
  uint8_t Dtmf_Nb_DynamicPt_valid;  /**< Must be set to true if Dtmf_Nb_DynamicPt is being passed */
  uint32_t Dtmf_Nb_DynamicPt;
  /**<   Payload type for DTMF narrow band codec. 
         Default value is 96 
    */

  /* Optional */
  /*  AMR Mode Set  */
  uint8_t Amr_ModeSet_valid;  /**< Must be set to true if Amr_ModeSet is being passed */
  uint32_t Amr_ModeSet;
  /**<   Bitmask for AMR NB modes allowed. \n
        Values: \n
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
  /**<   Enable/disable Source Controlled Rate (SCR) for AMR narrowband (NB). \n
        Values: \n
        -1 -- Enable \n
        -0 -- Disable  
    */

  /* Optional */
  /*  Speech Start Port  */
  uint8_t Speech_Start_Port_valid;  /**< Must be set to true if Speech_Start_Port is being passed */
  uint32_t Speech_Start_Port;
  /**<   
         Smallest RTP port number for a speech codec.
         If not set, the default value is 50000.
    */

  /* Optional */
  /*  Speech Stop Port  */
  uint8_t Speech_Stop_Port_valid;  /**< Must be set to true if Speech_Stop_Port is being passed */
  uint32_t Speech_Stop_Port;
  /**<   
         Largest RTP port number for a speech codec.
         If not set, the default value is 50002.
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
  IMS_IOT_STATUS_MIN_V01 = 0, /**<  Invalid registration status for IMS  */
  IMS_IOT_STATUS_NOT_REGISTERED_V01 = 1, /**<  Not registered or registration failed state for IMS  */
  IMS_IOT_STATUS_REGISTERING_V01 = 2, /**<  Registering for IMS   */
  IMS_IOT_STATUS_REGISTERED_V01 = 3, /**<  Registered for IMS  */
  IMS_IOT_STATUS_LIMITED_REGISTERED_V01 = 4, /**<  Limited registration for IMS  */
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

 structure for storing URI

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
  IMS_IOT_SERVICE_MIN_V01 = 0, /**<  Invalid Service status  */
  IMS_IOT_NO_SERVICE_V01 = 1, /**<  No Service available  */
  IMS_IOT_FULL_SERVICE_V01 = 2, /**<  Service available  */
  IMS_IOT_LIMITED_SERVICE_V01 = 3, /**<  Limited Service available  */
  IMS_IOT_SERVICE_MAX_V01 = 4, /**<  Invalid Service status  */
  IMS_IOT_SERVICE_STATUS_E_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_iot_service_status_e_type_v01;
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Gets the registration status for various IMS services for the
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
/** Response Message; Gets the registration status for various IMS services for the
             requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  IMS Registration status */
  uint8_t registration_status_valid;  /**< Must be set to true if registration_status is being passed */
  ims_iot_reg_status_e_type_v01 registration_status;

  /* Optional */
  /*  IMS Service status for SMS */
  uint8_t sms_service_status_valid;  /**< Must be set to true if sms_service_status is being passed */
  ims_iot_service_status_e_type_v01 sms_service_status;

  /* Optional */
  /*  IMS Service status for VOICE */
  uint8_t voice_service_status_valid;  /**< Must be set to true if voice_service_status is being passed */
  ims_iot_service_status_e_type_v01 voice_service_status;

  /* Optional */
  /*  IMS Registration Error Code in case of failure */
  uint8_t registration_failure_reason_code_valid;  /**< Must be set to true if registration_failure_reason_code is being passed */
  uint32_t registration_failure_reason_code;
  /**<   An reason/error code is returned when the IMS
       registration status is IMS_IOT_STATUS_NOT_REGISTERED. \n
       Values: \n
       - 3xx -- Redirection responses \n
       - 4xx -- Client failure responses \n
       - 5xx -- Server failure responses \n
       - 6xx -- Global failure responses
    */

  /* Optional */
  /*  Registration Error String in case of failure */
  uint8_t registration_failure_reason_valid;  /**< Must be set to true if registration_failure_reason is being passed */
  char registration_failure_reason[IMS_IOT_REGISTRATION_FAILURE_LEN_V01 + 1];
  /**<   Registration failure error string when the registration_status is IMS_IOT_STATUS_NOT_REGISTERED 
         otherwise NULL string.
    */

  /* Optional */
  /*  P-Associated URI List */
  uint8_t uri_list_valid;  /**< Must be set to true if uri_list is being passed */
  uint32_t uri_list_len;  /**< Must be set to # of elements in uri_list */
  ims_iot_uri_s_type_v01 uri_list[IMS_IOT_MAX_URI_LIST_V01];
  /**<   P-Associated URI List configured.
    */
}qmi_ims_iot_get_registration_service_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Indication Message; Indication sent when the IMS service registration status changes. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  IMS Registration status */
  uint8_t registration_status_valid;  /**< Must be set to true if registration_status is being passed */
  ims_iot_reg_status_e_type_v01 registration_status;

  /* Optional */
  /*  IMS Service status for SMS */
  uint8_t sms_service_status_valid;  /**< Must be set to true if sms_service_status is being passed */
  ims_iot_service_status_e_type_v01 sms_service_status;

  /* Optional */
  /*  IMS Service status for VOICE */
  uint8_t voice_service_status_valid;  /**< Must be set to true if voice_service_status is being passed */
  ims_iot_service_status_e_type_v01 voice_service_status;

  /* Optional */
  /*  IMS Registration Error Code in case of failure */
  uint8_t registration_failure_reason_code_valid;  /**< Must be set to true if registration_failure_reason_code is being passed */
  uint32_t registration_failure_reason_code;
  /**<   An reason/error code is returned when the IMS
       registration status is IMS_IOT_STATUS_NOT_REGISTERED. \n
       Values: \n
       - 3xx -- Redirection responses \n
       - 4xx -- Client failure responses \n
       - 5xx -- Server failure responses \n
       - 6xx -- Global failure responses
    */

  /* Optional */
  /*  Registration Error String in case of failure */
  uint8_t registration_failure_reason_valid;  /**< Must be set to true if registration_failure_reason is being passed */
  char registration_failure_reason[IMS_IOT_REGISTRATION_FAILURE_LEN_V01 + 1];
  /**<   Registration failure error string when the registration_status is IMS_IOT_STATUS_NOT_REGISTERED
         Otherwise NULL string.
    */

  /* Optional */
  /*  P-Associated URI List */
  uint8_t uri_list_valid;  /**< Must be set to true if uri_list is being passed */
  uint32_t uri_list_len;  /**< Must be set to # of elements in uri_list */
  ims_iot_uri_s_type_v01 uri_list[IMS_IOT_MAX_URI_LIST_V01];
  /**<   P-Associated URI List configured.
    */
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
  IMS_IOT_STATUS_UNKNOWN_PDN_V01 = 2, /**<  PDN was unknown  */
  IMS_IOT_STATUS_REASON_NOT_SPECIFIED_V01 = 3, /**<  Reason not specified \n */
  IMS_IOT_STATUS_CONNECTION_BRINGUP_FAILURE_V01 = 4, /**<  Connection bring-up failure  */
  IMS_IOT_STATUS_CONNECTION_IKE_AUTH_FAILURE_V01 = 5, /**<  IKE authentication failure  */
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
  IMS_IOT_TYPE_IPV6_V01 = 0x06, /**<  IPv6 \n  */
  IMS_IOT_PDN_FAIL_REASON_E_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ims_iot_pdn_fail_reason_e_type_v01;
/**
    @}
  */

/** @addtogroup ims_iot_qmi_aggregates
    @{
  */
/**  Enumeration for various PDN failure error code

Enumeration for a verbose call end reason (VCER) type. 
 Strucute for various PDN failure error code

 */
typedef struct {

  ims_iot_pdn_fail_reason_e_type_v01 pdn_fail_reason_type;
  /**<   Values: \n
      - IMS_IOT_TYPE_UNSPECIFIED (0x00) --  Unspecified \n 
      - IMS_IOT_TYPE_MOBILE_IP (0x01) --  Mobile IP \n 
      - IMS_IOT_TYPE_INTERNAL (0x02) --  Internal \n 
      - IMS_IOT_TYPE_CALL_MANAGER_DEFINED (0x03) --  Call manager is defined \n 
      - IMS_IOT_TYPE_3GPP_SPEC_DEFINED (0x04) --  3GPP specification is defined \n 
      - IMS_IOT_TYPE_PPP (0x05) --  PPP \n 
      - IMS_IOT_TYPE_IPV6 (0x06) --  IPv6 \n 
 */

  uint32_t pdn_fail_reason;
  /**<   Failure reason (verbose); see Spec 24.008/24.007/C.S0017
       for the definition of these values.
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
  /*  IMS PDN Connection status */
  uint8_t pdn_status_valid;  /**< Must be set to true if pdn_status is being passed */
  uint8_t pdn_status;
  /**<   IMS PDN connection state information.
         Values:
           - TRUE  -- IMS PDN is UP or Connected 
           - FALSE -- IMS PDN is Down or not connected
    */

  /* Optional */
  /*  PDN failure Error String in case of failure  */
  uint8_t pdn_failure_reason_valid;  /**< Must be set to true if pdn_failure_reason is being passed */
  char pdn_failure_reason[IMS_IOT_PDN_FAILURE_LEN_V01 + 1];
  /**<    PDN failure error string when the pdn_status is FALSE. 
          Otherwise NULL string
    */

  /* Optional */
  /*  IMS PDP Failure Error Code */
  uint8_t pdn_fail_code_valid;  /**< Must be set to true if pdn_fail_code is being passed */
  ims_iot_pdn_failure_code_e_type_v01 pdn_fail_code;
  /**<   IMS PDP connection failure reason when the IMS PDP Connection status TLV is FALSE 
         (that is, IMS PDP is not in the Connected state).
    */

  /* Optional */
  /*  IMS PDN failure reason if pdn_status is set to 0 */
  uint8_t pdn_fail_cause_valid;  /**< Must be set to true if pdn_fail_cause is being passed */
  ims_iot_pdn_failure_reason_s_type_v01 pdn_fail_cause;
  /**<   IMS PDN connection failure error cause code when the IMS PDN Connection status TLV is FALSE 
         (that is, IMS PDN is not in the Connected state).
    */
}qmi_ims_iot_get_pdn_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Indication Message; Indication sent when the status of a PDN connection changes. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  IMS PDN Connection status */
  uint8_t pdn_status_valid;  /**< Must be set to true if pdn_status is being passed */
  uint8_t pdn_status;
  /**<   IMS PDN connection state information.\n
         Values:\n
           - TRUE  -- IMS PDN is UP or Connected \n
           - FALSE -- IMS PDN is Down or not connected
    */

  /* Optional */
  /*  PDN failure Error String in case of failure  */
  uint8_t pdn_failure_reason_valid;  /**< Must be set to true if pdn_failure_reason is being passed */
  char pdn_failure_reason[IMS_IOT_PDN_FAILURE_LEN_V01 + 1];
  /**<    PDN failure error string when the pdn_status is FALSE.
          Otherwise NULL string
    */

  /* Optional */
  /*  IMS PDP Failure Error Code */
  uint8_t pdn_fail_code_valid;  /**< Must be set to true if pdn_fail_code is being passed */
  ims_iot_pdn_failure_code_e_type_v01 pdn_fail_code;
  /**<   IMS PDP connection failure reason when the IMS PDP Connection status TLV is FALSE
         (that is, IMS PDP is not in the Connected state).
    */

  /* Optional */
  /*  IMS PDN failure reason if pdn_status is set to 0 */
  uint8_t pdn_fail_cause_valid;  /**< Must be set to true if pdn_fail_cause is being passed */
  ims_iot_pdn_failure_reason_s_type_v01 pdn_fail_cause;
  /**<   IMS PDN connection failure error cause code when the IMS PDN Connection status TLV is FALSE
         (that is, IMS PDN is not in the Connected state).
    */
}qmi_ims_iot_pdn_status_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_enums
    @{
  */
typedef enum {
  IMS_IOT_BLOCK_STATUS_E_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IMS_IOT_NOT_BLOCKED_V01 = 0, /**<  IMS Registration/PDN is not Blocked  */
  IMS_IOT_TEMP_BLOCKED_V01 = 1, /**<  IMS Registration/PDN is Blocked Temporarily  */
  IMS_IOT_PERM_BLOCKED_V01 = 2, /**<  IMS Registration/PDN is Blocked Permanently  */
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
  /*  IMS PDN Connection status */
  uint8_t block_status_valid;  /**< Must be set to true if block_status is being passed */
  ims_iot_block_status_e_type_v01 block_status;
  /**<   IMS Service status information.\n
         It can have have below values :
          0 - Not Blocked
          1 - Blocked Temporarily
          2 - Blocked Permanently
    */
}qmi_ims_iot_get_registration_block_status_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Indication Message; Indication sent when IMS block status changes because of PDN and/or Registration failure */
typedef struct {

  /* Optional */
  /*  IMS PDN Connection status */
  uint8_t block_status_valid;  /**< Must be set to true if block_status is being passed */
  ims_iot_block_status_e_type_v01 block_status;
  /**<   IMS PDN connection status information.\n
         It can have have below values :
          0 - Not Blocked
          1 - Blocked Temporarily
          2 - Blocked Permanently
    */
}qmi_ims_iot_registration_block_status_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ims_iot_qmi_messages
    @{
  */
/** Request Message; Resets/unblock the IMS blocking status. */
typedef struct {

  /* Mandatory */
  /*  PDN Status unblock */
  uint8_t pdn_unblock;
  /**<    Request to unblock IMS PDN at modem in case if its blocked previously.
        Values: \n
         - TRUE  -- Unblock \n
         - FALSE -- No action 
   */

  /* Mandatory */
  /*  Registration Status unblock */
  uint8_t registration_unblock;
  /**<   Request to unblock IMS Registration at modem in case if its blocked previously.
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
/** Response Message; Resets/unblock the IMS blocking status. */
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
/** Indication Message; Indication sent when LTE attach parameters change. */
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

