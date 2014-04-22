/*================================================================================

DESCRIPTION


Copyright (c) 2009-2014, 2019 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_cat_decoded.h#1 $$ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
12/13/19   dt      Changing mask size in event indication report
10/23/19   dt      Migration of QMICAT to QCSI/QCCI
09/12/16   shr     Add support for Poll Interval Envelope
11/14/14   hh      Fix compilation warnings
09/24/14   gm      Remove GOBI mode code
08/05/14   hh      Added support to retrieve supported proactive commands
04/07/14   gm      Added support for call type as IMS
03/17/14   gm      Added support for IMS communication control
04/30/13   tkl     Added support for 3rd party IMS client in raw format
02/28/13   tkl     Added support for 3rd party IMS client
01/28/13   tkl     Added BIP status events
10/02/12   tkl     Added SMS-PP download in envelope command
09/09/12   at      Removal of private DS functions and headers
03/21/12   at      Added browser termination event in decoded envelope command
01/10/12   tkl     Added support for set/get configuration
01/10/12   tkl     Added custom TP support
01/03/12   nb      Addded support to send status words for TR
01/03/12   nb      Addded support to retrieve terminal profile
11/16/11   tkl     Added APDU support for raw envelope
10/25/11   tkl     Added Get InKey Yes/No TLV for decoded TR
06/05/11   vs      Decoded call control envelope
06/03/11   yt      Added slot id to SET_EVENT_REPORT and GET_EVENT_REPORT
04/20/11   tkl     Added SCWS support
01/14/11   mib     Review of parsing routines
08/05/10   tkl     QMI UIM & QMI CAT split from data package
06/22/10   tkl     Added support for LANGUAGE INFO tlv for PLI-Language
01/13/10   mib     Initial version
=============================================================================*/

#ifndef QMI_CAT_DECODED_H
#define QMI_CAT_DECODED_H


/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "customer.h"
#include "comdef.h"
#include "gstk_exp.h"
#include "common_v01.h"
#include "card_application_toolkit_v02.h"
#include "qmi_cat.h"

/*---------------------------------------------------------------------------
  Max size used in QMI requests
---------------------------------------------------------------------------*/
#define CAT_RAW_MAX_DATA_LEN                           (258)
#define CAT_TLV_ADDITIONAL_INFO_SIZE_MAX               (10)
#define CAT_TLV_TEXT_STRING_MAX                        (255)
#define CAT_TLV_LANGUAGE                               (2)
#define CAT_TLV_SCWS_DATA_AVAIL_MAX                    (1400)
#define CAT_TLV_SCWS_SEND_DATA_MAX                     (500)
#define CATI_TLV_ADDRESS_MAX                           (200)
#define CATI_TLV_SUBADDRESS_MAX                        (20)
#define CATI_TLV_CCP_MAX                               (16)
#define CATI_TLV_USSD_STRING_MAX                       (255)
#define CATI_TLV_ACT_PARAM_MAX                         (255)
#define CATI_TLV_CUSTOM_TP_MAX                         (80)
#define CATI_TLV_SMS_TPDU_MAX                          (255)
#define CATI_TLV_TX_ID_MAX                             (255)
#define CATI_TLV_CAUSE_MAX                             (30)
#define CATI_TLV_IMS_URI_MAX                           (246)


/*---------------------------------------------------------------------------
  Minimum size of TLVs
---------------------------------------------------------------------------*/
#define QMI_CAT_RAW_TR_TLV_MIN_SIZE                        (6)
#define QMI_CAT_RAW_ENVELOPE_TLV_MIN_SIZE                  (4)
#define QMI_CAT_DECODED_TR_TERM_RESP_TLV_MIN_SIZE          (8)
#define QMI_CAT_DECODED_TR_TEXT_STRING_TLV_MIN_SIZE        (2)
#define QMI_CAT_DECODED_TR_GET_INKEY_TLV_MIN_SIZE          (4)
#define QMI_CAT_SCWS_CHANNEL_DATA_TLV_MIN_SIZE             (6)
#define QMI_CAT_DECODED_ENVELOPE_ADDRESS_TLV_MIN_SIZE      (3)
#define QMI_CAT_DECODED_ENVELOPE_SUBADDRESS_TLV_MIN_SIZE   (1)
#define QMI_CAT_DECODED_ENVELOPE_CCP_TLV_MIN_SIZE          (1)
#define QMI_CAT_DECODED_ENVELOPE_USSD_STR_TLV_MIN_SIZE     (2)
#define QMI_CAT_DECODED_ENVELOPE_ACT_PARAM_TLV_MIN_SIZE    (1)
#define QMI_CAT_DECODED_ENVELOPE_SMS_TPDU_TLV_MIN_SIZE     (1)
#define QMI_CAT_DECODED_ENVELOPE_CAUSE_TLV_MIN_SIZE        (1)
#define QMI_CAT_DECODED_ENVELOPE_TRANS_ID_TLV_MIN_SIZE     (1)
#define QMI_CAT_CUSTOM_TP_TLV_MIN_SIZE                     (1)
#define QMI_CAT_DECODED_ENVELOPE_IMS_REQUEST_URI_MIN_SIZE  (1)

/*---------------------------------------------------------------------------
  Max number of BIP Channel
---------------------------------------------------------------------------*/
#define QMI_CATI_MAX_BIP_CHANNEL                  (7)

/*---------------------------------------------------------------------------
  Defines used for DCS coding
---------------------------------------------------------------------------*/
#define CATI_DCS_7_BIT                                 (0x00)
#define CATI_DCS_8_BIT                                 (0x01)
#define CATI_DCS_UCS2_8_BIT                            (0x02)
#define CATI_DCS_UCS2_7_BIT                            (0x03)

/*---------------------------------------------------------------------------
  Call Control Call type
---------------------------------------------------------------------------*/
#define CATI_CC_CALL_TYPE_VOICE                          0x00
#define CATI_CC_CALL_TYPE_SS                             0x01
#define CATI_CC_CALL_TYPE_USSD                           0x02
#define CATI_CC_CALL_TYPE_SMS                            0x03
#define CATI_CC_CALL_TYPE_IMS                            0x04

/*---------------------------------------------------------------------------
  BIP status enum type
---------------------------------------------------------------------------*/
typedef enum
{
  CATI_BIP_SESSION_IN_PROGRESS          =0,
  CATI_BIP_SESSION_END,
} qmi_cati_bip_status_e_type;

/*---------------------------------------------------------------------------
  Data event enum type
---------------------------------------------------------------------------*/
typedef enum
{
  CATI_DATA_EVENT_NONE                   = 0,
  CATI_DATA_EVENT_OPEN_CHANNEL,
  CATI_DATA_EVENT_CLOSE_CHANNEL,
  CATI_DATA_EVENT_SEND_DATA,
  CATI_DATA_EVENT_RECEIVE_DATA,
} qmi_cati_data_event_e_type;

/*---------------------------------------------------------------------------
  Structure used to parse the request for set event report
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                                    raw_mask_present;
  boolean                                    decoded_mask_present;
  boolean                                    slot_present;
  boolean                                    full_func_mask_present;

  uint32                                     raw_mask;
  uint32                                     decoded_mask;
  uint8                                      slot_mask;
  uint32                                     full_func_mask;
} qmi_cat_in_set_event_report_type;

/*---------------------------------------------------------------------------
  Structure used to parse the request for get event report
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                                    proactive_command_present;

  PACKED struct PACKED_POST
  {
    uint32  cmd_ref_id;
    uint8   format;
  }                                          proactive_command;
} qmi_cat_in_get_event_report_type;

/*---------------------------------------------------------------------------
  Structure used to parse the request for raw TR
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                                    term_resp_present;
  boolean                                    slot_present;

  PACKED struct PACKED_POST
  {
    uint32                                   cmd_ref_id;
    uint16                                   tr_len;
    uint8                                    terminal_response[CAT_RAW_MAX_DATA_LEN];
  }                                          term_resp;
  PACKED struct PACKED_POST
  {
    uint8                                    slot;
  }                                          slot;
} qmi_cat_in_raw_tr_type;

/*---------------------------------------------------------------------------
  Structure used to parse the request for raw envelope
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                                    envelope_present;
  boolean                                    slot_present;

  PACKED struct PACKED_POST
  {
    uint16                                   env_cmd_type;
    uint16                                   env_cmd_len;
    uint8                                    envelope_data[CAT_RAW_MAX_DATA_LEN];
  }                                          envelope;
  PACKED struct PACKED_POST
  {
    uint8                                    slot;
  }                                          slot;
} qmi_cat_in_raw_envelope_type;

/*---------------------------------------------------------------------------
  Structure used to parse the request for decoded TR
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                                    term_resp_present;
  boolean                                    text_string_present;
  boolean                                    item_id_present;
  boolean                                    get_inkey_term_resp_extra_present;
  boolean                                    language_present;
  boolean                                    get_inkey_term_resp_yes_no_present;
  boolean                                    slot_present;

  PACKED struct PACKED_POST
  {
    uint32                                   cmd_detail_ref_id;
    uint8                                    command_number;
    uint8                                    response_cmd;
    uint8                                    general_result;
    uint8                                    additional_info_length;
    uint8                                    additional_info_data[CAT_TLV_ADDITIONAL_INFO_SIZE_MAX];
  }                                          term_resp;
  PACKED struct PACKED_POST
  {
    uint8                                    dcs;
    uint8                                    length_of_string;
    uint8                                    text[CAT_TLV_TEXT_STRING_MAX];
  }                                          text_string;
  PACKED struct PACKED_POST
  {
    uint8                                    item_id;
  }                                          item_id;
  PACKED struct PACKED_POST
  {
    uint8                                    units;
    uint8                                    interval;
    uint8                                    dcs;
    uint8                                    length_of_string;
    uint8                                    text[CAT_TLV_TEXT_STRING_MAX];
  }                                          get_inkey_term_resp_extra;
  PACKED struct PACKED_POST
  {
    uint8                                    language[CAT_TLV_LANGUAGE];
  }                                          language;
  PACKED struct PACKED_POST
  {
    uint8                                    units;
    uint8                                    interval;
    uint8                                    yes_no;
  }                                          get_inkey_term_resp_yes_no;
  PACKED struct PACKED_POST
  {
    uint8                                    slot;
  }                                          slot;
} qmi_cat_in_decoded_tr_type;

/*---------------------------------------------------------------------------
  Structure used to parse the request for decoded Envelope
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                                    env_hdr_present;
  boolean                                    item_id_present;
  boolean                                    help_request_present;
  boolean                                    language_present;
  boolean                                    slot_present;
  boolean                                    address_present;
  boolean                                    subaddress_present;
  boolean                                    ccp1_present;
  boolean                                    ccp2_present;
  boolean                                    ussd_string_present;
  boolean                                    pdp_context_act_present;
  boolean                                    eps_pdn_conn_act_present;
  boolean                                    browser_term_cause_present;
  boolean                                    sms_tpdu_present;
  boolean                                    is_cdma_sms_present;
  boolean                                    rat_present;
  boolean                                    call_type_present;
  boolean                                    transaction_id_present;
  boolean                                    rp_address_present;
  boolean                                    tp_address_present;
  boolean                                    cause_present;
  boolean                                    ims_uri_present;
  boolean                                    poll_interval_duration_present;

  PACKED struct PACKED_POST
  {
    uint8                                    env_cmd_type;
  }                                          env_hdr;
  PACKED struct PACKED_POST
  {
    uint8                                    item_id;
  }                                          item_id;
  PACKED struct PACKED_POST
  {
    uint8                                    help_request;
  }                                          help_request;
  PACKED struct PACKED_POST
  {
    uint8                                    language[CAT_TLV_LANGUAGE];
  }                                          language;
  PACKED struct PACKED_POST
  {
    uint8                                    slot;
  }                                          slot;
  PACKED struct PACKED_POST
  {
    uint8                                     ton;
    uint8                                     npi;
    uint8                                     length;
    uint8                                     address[CATI_TLV_ADDRESS_MAX];
  }                                           address;
  PACKED struct PACKED_POST
  {
    uint8                                     length;
    uint8                                     subaddress[CATI_TLV_SUBADDRESS_MAX];
  }                                           subaddress;
  PACKED struct PACKED_POST
  {
    uint8                                     length;
    uint8                                     ccp[CATI_TLV_CCP_MAX];
  }                                           ccp1;
  PACKED struct PACKED_POST
  {
    uint8                                     length;
    uint8                                     ccp[CATI_TLV_CCP_MAX];
  }                                           ccp2;
  PACKED struct PACKED_POST
  {
    uint8                                     dcs;
    uint8                                     length;
    uint8                                     text[CATI_TLV_USSD_STRING_MAX];
  }                                           ussd_string;
  PACKED struct PACKED_POST
  {
    uint8                                     length;
    uint8                                     pdp_act_param[CATI_TLV_ACT_PARAM_MAX];
  }                                           pdp_context_act;
  PACKED struct PACKED_POST
  {
    uint8                                     length;
    uint8                                     eps_act_param[CATI_TLV_ACT_PARAM_MAX];
  }                                           eps_pdn_connect_act;
  PACKED struct PACKED_POST
  {
    uint32                                    browser_term_cause;
  }                                           browser_term_cause;
  PACKED struct PACKED_POST
  {
    uint8                                     length;
    uint8                                     tpdu[CATI_TLV_SMS_TPDU_MAX];
  }                                           sms_tpdu;
  PACKED struct PACKED_POST
  {
    boolean                                   is_cdma_sms;
  }                                           is_cdma_sms;
  PACKED struct PACKED_POST
  {
    uint32                                    rat;
  }                                           rat;
  PACKED struct PACKED_POST
  {
    uint32                                    call_type;
  }                                           call_type;
  PACKED struct PACKED_POST
  {
    uint8                                     length;
    uint8                                     transaction_id[CATI_TLV_TX_ID_MAX];
  }                                           transaction_id;
  PACKED struct PACKED_POST
  {
    uint8                                     ton;
    uint8                                     npi;
    uint8                                     length;
    uint8                                     address[CATI_TLV_ADDRESS_MAX];
  }                                           rp_address;
  PACKED struct PACKED_POST
  {
    uint8                                     ton;
    uint8                                     npi;
    uint8                                     length;
    uint8                                     address[CATI_TLV_ADDRESS_MAX];
  }                                           tp_address;
  PACKED struct PACKED_POST
  {
    uint8                                     length;
    uint8                                     cause[CATI_TLV_CAUSE_MAX];
  }                                           cause;
  PACKED struct PACKED_POST
  {
    uint8                                     length;
    uint8                                     ims_uri[CATI_TLV_IMS_URI_MAX];
  }                                           ims_uri;
  PACKED struct PACKED_POST
  {
    uint8                                     units;
    uint8                                     interval;
  }                                           poll_interval_duration;  
} qmi_cat_in_decoded_envelope_type;

/*---------------------------------------------------------------------------
  Structure used to parse the request for event confirmation
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                                    user_confirmed_present;
  boolean                                    icon_display_present;
  boolean                                    slot_present;

  PACKED struct PACKED_POST
  {
    uint8                                    user_confirmed;
  }                                          user_confirmed;
  PACKED struct PACKED_POST
  {
    uint8                                    icon_display;
  }                                          icon_display;
  PACKED struct PACKED_POST
  {
    uint8                                    slot;
  }                                          slot;
} qmi_cat_in_event_confirmation_type;

/*---------------------------------------------------------------------------
  Structure used to parse the request for scws open channel state
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                                    channel_state_present;
  boolean                                    slot_present;

  PACKED struct PACKED_POST
  {
    uint32                                   ch_id;
    uint8                                    ch_state;
  }                                          channel_state;
  PACKED struct PACKED_POST
  {
    uint8                                    slot;
  }                                          slot;
} qmi_cat_in_scws_open_channel_state_type;

/*---------------------------------------------------------------------------
  Structure used to parse the request for scws close channel state
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                                    channel_state_present;
  boolean                                    slot_present;

  PACKED struct PACKED_POST
  {
    uint32                                   ch_id;
    uint8                                    ch_state;
  }                                          channel_state;
  PACKED struct PACKED_POST
  {
    uint8                                    slot;
  }                                          slot;
} qmi_cat_in_scws_close_channel_state_type;

/*---------------------------------------------------------------------------
  Structure used to parse the request for scws channel status state
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                                    channel_state_present;
  boolean                                    slot_present;

  PACKED struct PACKED_POST
  {
    uint32                                   ch_id;
    uint8                                    ch_state;
  }                                          channel_state;
  PACKED struct PACKED_POST
  {
    uint8                                    slot;
  }                                          slot;
} qmi_cat_in_scws_channel_status_state_type;

/*---------------------------------------------------------------------------
  Structure used to parse the request for scws send data
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                                    channel_data_result_present;
  boolean                                    slot_present;

  PACKED struct PACKED_POST
  {
    uint32                                   ch_id;
    uint8                                    result;
  }                                          channel_data_result;
  PACKED struct PACKED_POST
  {
    uint8                                    slot;
  }                                          slot;
} qmi_cat_in_scws_send_data_type;

/*---------------------------------------------------------------------------
  Structure used to parse the request for scws data available
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                                    channel_data_present;
  boolean                                    remain_data_len_present;
  boolean                                    slot_present;

  PACKED struct PACKED_POST
  {
    uint32                                   ch_id;
    uint16                                   data_len;
    uint8                                    data[CAT_TLV_SCWS_DATA_AVAIL_MAX];
  }                                          channel_data;
  PACKED struct PACKED_POST
  {
    uint16                                   data_len;
  }                                          remain_data_len;
  PACKED struct PACKED_POST
  {
    uint8                                    slot;
  }                                          slot;
} qmi_cat_in_scws_data_available_type;

/*---------------------------------------------------------------------------
  Structure used to parse the request to retrieve Terminal Profile
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                                    slot_present;

  PACKED struct PACKED_POST
  {
    uint8                                    slot;
  }                                          slot;
} qmi_cat_in_get_terminal_profile_type;

/*---------------------------------------------------------------------------
  Structure used to parse the request for set configuration
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                                    config_mode_present;
  boolean                                    custom_tp_present;

  PACKED struct PACKED_POST
  {
    uint8                                    config_mode;
  }                                          config_mode;
  PACKED struct PACKED_POST
  {
    uint8                                    tp_len;
    uint8                                    tp_bitmask[CATI_TLV_CUSTOM_TP_MAX];
  }                                          custom_tp;
} qmi_cat_in_set_configuration_type;

/*---------------------------------------------------------------------------
  Structure used to parse the request for get cached proactive command
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                                    command_id_present;
  boolean                                    slot_present;

  PACKED struct PACKED_POST
  {
    uint32                                   command_id;
  }                                          command_id;
  PACKED struct PACKED_POST
  {
    uint8                                    slot;
  }                                          slot;
} qmi_cat_in_get_cached_proactive_cmd_type;

/*---------------------------------------------------------------------------
  Structure for decoded BIP Status
---------------------------------------------------------------------------*/
typedef struct
{
  gstk_slot_id_enum_type                slot_id;
  uint8                                 ch_id;
  qmi_cati_bip_status_e_type            bip_status;
} qmi_cat_decoded_bip_status_type;

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_RAW_PROACTIVE_COMMAND()

  DESCRIPTION
    Populates the tlvs from the raw proactive command

  PARAMETERS
    tlv_id          : Tag of the TLV
    ind             : output sdu
    cmd_ref_id      : command reference id
    raw_data_len    : length of the raw proactive command
    raw_data_ptr    : value of the raw proactive command
    slot_id         : slot id

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_cat_response_raw_proactive_command
(
  byte                             tlv_id,
  cat_event_report_ind_msg_v02   * ind_ptr,
  uint32                           cmd_ref_id,
  uint16                           raw_data_len,
  const uint8                    * raw_data_ptr,
  gstk_slot_id_enum_type           slot_id,
  uint8                            response_type,
  uint32                            mask,
  qmi_cati_data_event_e_type       event_type
);


/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_END_PROACTIVE_SESSION()

  DESCRIPTION
    Populates the tlv for the end proactive session

  PARAMETERS
    ind            : output sdu
    end_type       : type of end of proactive session
    slot_id        : slot id

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_cat_response_end_proactive_session
(
  cat_get_event_report_resp_msg_v02 * response_ptr,
  uint8                            end_type,
  gstk_slot_id_enum_type           slot_id
);


/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_SET_EVENT_REPORT()

  DESCRIPTION
    Populates the tlv for the set event report

  PARAMETERS
    ind                 : output sdu
    raw_mask            : raw mask
    decoded_mask        : decoded mask
    full_func_mask      : full function handling mask

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_cat_response_set_event_report
(
  cat_set_event_report_resp_msg_v02  * response_ptr,
  uint32                             raw_mask,
  uint32                             decoded_mask,
  uint32                             full_func_mask
);


/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_GET_SERVICE_STATE()

  DESCRIPTION
    Populates the tlv for the response of get service state

  PARAMETERS
    ind                 : output sdu
    raw_client_mask     : raw mask of this client
    raw_global_mask     : raw mask of all clients
    decoded_client_mask : decoded mask of this client
    decoded_global_mask : decoded mask of all clients
	full_func_mask      : full function mask of all clients

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_cat_response_get_service_state
(
  cat_get_service_state_resp_msg_v02 * response_ptr,
  uint32                           raw_client_mask,
  uint32                           raw_global_mask,
  uint32                           decoded_client_mask,
  uint32                           decoded_global_mask,
  uint32                           full_func_global_mask
);


/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_GET_CONFIGURATION()

  DESCRIPTION
    Populates the tlv for get configuration

  PARAMETERS
    ind                 : output sdu
    tp_len              : len of custom_tp
    custom_tp_ptr       : value of custom_tp

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_cat_response_get_configuration
(
  cat_get_configuration_resp_msg_v02 *response_ptr,
  uint8                            config_mode,
  uint8                            custom_tp_len,
  uint8                          * custom_tp_ptr
);


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_SET_EVENT_REPORT()

  DESCRIPTION
    Parses the TLV content of QMI_UIM message

  PARAMETERS
    req_ptr      : input req msg
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_cat_parse_request_set_event_report
(
  cat_set_event_report_req_msg_v02   * req_ptr,
  qmi_cat_in_set_event_report_type   * parsed_data_ptr
);

/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_GET_EVENT_REPORT()

  DESCRIPTION
    Parses the TLV content of QMI_UIM message

  PARAMETERS
    req_ptr      : input req msg
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_cat_parse_request_get_event_report
(
  cat_get_event_report_req_msg_v02   * req_ptr,
  qmi_cat_in_get_event_report_type   * parsed_data_ptr
);

/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_RAW_TR()

  DESCRIPTION
    Parses the TLV content of QMI_UIM message

  PARAMETERS
    req_ptr      : input req msg
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_cat_parse_request_raw_tr
(
  cat_send_tr_req_msg_v02      * req_ptr,
  qmi_cat_in_raw_tr_type       * parsed_data_ptr
);

/*===========================================================================
  FUNCTION  QMI_CAT_RESPONSE_SEND_TR()

  DESCRIPTION
    Process the TR response

  PARAMETERS
   void *        : Pointer to response message
   gstk_cmd_from_card_type : Pointer to command from card

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_cat_response_send_tr
(
  cat_send_tr_resp_msg_v02              * response_ptr,
  const gstk_term_rsp_sw1_sw2_rpt_type  * msg_ptr
);

/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_RAW_ENVELOPE()

  DESCRIPTION
    Parses the TLV content of QMI_UIM message

  PARAMETERS
    req_ptr      : input req msg
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_cat_parse_request_raw_envelope
(
  cat_send_envelope_cmd_req_msg_v02 * req_ptr,
  qmi_cat_in_raw_envelope_type   * parsed_data_ptr
);

/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_EVENT_CONFIRMATION()

  DESCRIPTION
    Parses the TLV content of QMI_CAT message

  PARAMETERS
    req_ptr      : input req msg
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
 qmi_error_type_v01 qmi_cat_parse_request_event_confirmation
 (
   cat_event_confirmation_req_msg_v02  * req_ptr,
   qmi_cat_in_event_confirmation_type  * parsed_data_ptr
 );

/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_SET_CONFIGURATION()

  DESCRIPTION
    Parses the TLV content of QMI_CAT message

  PARAMETERS
    req_ptr      : input req msg
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
 qmi_error_type_v01 qmi_cat_parse_request_set_configuration
 (
   cat_set_configuration_req_msg_v02   * req_ptr,
   qmi_cat_in_set_configuration_type   * parsed_data_ptr
 );

/*===========================================================================
  FUNCTION  QMI_CAT_RESPONSE_RAW_ENVELOPE()

  DESCRIPTION
    Process the raw evnvelope response

  PARAMETERS
   gstk_cmd_from_card_type : Pointer to command from card
   dsm_item_type **        : Pointer to DSM item pointer

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
 qmi_error_type_v01 qmi_cat_response_raw_envelope
 (
   const gstk_cmd_from_card_type  * env_rsp_ptr,
   cat_send_envelope_cmd_resp_msg_v02          * response_ptr
 );


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_GET_TERMINAL_PROFILE()

  DESCRIPTION
    Parses the TLV content of QMI_CAT message

  PARAMETERS
    req_ptr      : input req msg
    parsed_data_ptr : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_cat_parse_request_get_terminal_profile
(
  cat_get_terminal_profile_req_msg_v02   * req_ptr,
  qmi_cat_in_get_terminal_profile_type   * parsed_data_ptr
);

/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_GET_TERMINAL_PROFILE()

  DESCRIPTION
    Populates the tlvs for TP content

  PARAMETERS
    ind               : output sdu
    buf_len           : buffer length
    buf_ptr           : pointer to data buffer
   
  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_cat_response_get_terminal_profile
(
  cat_get_terminal_profile_resp_msg_v02  * response_ptr,
  uint32                                   buf_len,
  uint8                                  * buf_ptr
);


/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_CACHED_PROACTIVE_CMD()

  DESCRIPTION
    Populates the tlvs from the raw proactive command

  PARAMETERS
    tlv_id          : tag to be used to pack response
    ind             : output sdu
    cmd_ref_id      : command reference id
    raw_data_len    : length of the raw proactive command
    raw_data_ptr    : value of the raw proactive command

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_cat_response_cached_proactive_cmd
(
  uint8                                               command_id,
  cat_get_cached_proactive_cmd_resp_msg_v02         * response_ptr,
  uint32                                              cmd_ref_id,
  uint16                                              raw_data_len,
  const uint8                                       * raw_data_ptr
);

/*===========================================================================
  FUNCTION  QMI_CAT_IND_RAW_ENVELOPE()

  DESCRIPTION
    Process the raw evnvelope response

  PARAMETERS
   gstk_cmd_from_card_type            : Pointer to command from card
   cat_send_envelope_cmd_ind_msg_v02* : Pointer to send envelope command indication msg

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_cat_ind_raw_envelope
(
  const gstk_cmd_from_card_type          * env_rsp_ptr,
  cat_send_envelope_cmd_ind_msg_v02      * response_ptr
);

/*===========================================================================
  FUNCTION  QMI_CAT_IND_SEND_TR()

  DESCRIPTION
    Process the TR response

  PARAMETERS
   cat_send_tr_ind_msg_v02*               : Pointer to send tr indicaton msg
   gstk_term_rsp_sw1_sw2_rpt_type : Pointer to status words from card

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_cat_ind_send_tr
(
  cat_send_tr_ind_msg_v02               * response_ptr,
  const gstk_term_rsp_sw1_sw2_rpt_type  * msg_ptr
);

#endif /* QMI_CAT_DECODED_H */

