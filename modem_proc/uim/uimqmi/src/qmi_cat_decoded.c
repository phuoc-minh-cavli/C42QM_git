/*================================================================================

DESCRIPTION


Copyright (c) 2009-2014, 2016, 2019-2020 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_cat_decoded.c#2 $$ $DateTime: 2020/08/12 07:09:02 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
08/03/20   dt      Fix kw issues in end proactive session fn
01/02/19   dt      Data is incorrect in Get input command
12/13/19   dt      Changing mask size in event indication report
10/23/19   dt      Migration of QMICAT to QCSI/QCCI
09/12/16   shr     Add support for Poll Interval Envelope
03/07/16   lm      Remove unneeded F3's
09/24/14   gm      Remove GOBI mode code
08/29/14   hh      Fix compliation warnings
08/05/14   hh      Added support to retrieve supported proactive commands
07/02/14   hh      Correct value for CATI_GET_EVENT_REPORT_COMMAND_TLV 
05/07/14   gm      Add break statement
04/22/14   at      Critical KW fixes
03/27/14   vr      Item with DCS is wrongly coded if text length is 0
03/17/14   gm      Added support for IMS communication control
01/10/14   df      Use v2.0 diag macros
10/09/13   tkl     Fixed handling of slot 3 in parsing slot tlv
10/08/13   gm      Removed unused comments
10/04/13   gm      F3 message reduction
06/12/13   kb      Added QMI malloc and free wrapper functions
05/17/13   tkl     Added support for Refresh Alpha
05/16/13   gm      Moved memcpy() and memmove() to memscpy() and memsmove()
04/30/13   tkl     Added support for 3rd party IMS client in raw format
04/10/13   tkl     Fix size for bip_status and resp_type TLV
03/13/13   tkl     Filter optional address TLV for cc_sms decoded envelope rsp
02/28/13   tkl     Added BIP status events
02/22/13   yt      Support for slot 3
01/28/13   tkl     Added BIP status events
11/09/12   yt      Move QMI CAT to UIM heap
10/11/12   at      Casting of variables to avoid compiler warnings
10/03/12   tkl     Added support for Browswer Termination event indication
10/02/12   tkl     Added SMS-PP download in envelope command
09/18/12   tkl     Added optional Alpha TLV for SCWS open channel ind
09/09/12   at      Removal of private DS functions and headers
09/07/12   tkl     Fixed Slot ID TLV for SCWS indications
05/23/12   hn      Enable 1-byte alignment packing on Windows environment
04/24/12   at      Correct function call when parsing decoded proactive cmd
03/21/12   at      Added browser termination event in decoded envelope command
03/09/12   tkl     Fixed Browser ID TLV
02/23/12   tkl     Removed dcs from network access name TLV
02/23/12   nmb     QMI QMUX Transition
01/19/12   at      Fixed TLV tag ids for QMI_CAT_SET_CONFIGURATION
01/17/12   mib     Fixed handling of default bearer in open channel command
01/10/12   vv      Fixed slot check for set_event_report request
01/10/12   tkl     Added support for set/get configuration
01/10/12   tkl     Added custom TP support
01/03/12   nb      Addded support to send status words for TR
01/03/11   nb      Addded support to retrieve terminal profile
12/20/11   hn      Add support for off-target testing
11/30/11   tkl     Added slot tlv check for set_event_report request
11/17/11   tkl     Fixed dcs type for alphabet TLV
11/16/11   tkl     Added APDU support for raw envelope
11/08/11   tkl     Added support for Class M and L
10/26/11   tkl     Added Items with dcs TLV
10/25/11   tkl     Added Get InKey Yes/No TLV for decoded TR
10/20/11   tkl     Fixed cc_bc_repeat TLV
06/06/11   tkl     Fixed TON and NPI in Address TLV with RFU values
06/05/11   vs      Decoded call control envelope
06/03/11   yt      Added slot id to QMI_CAT_SET_EVENT_REPORT
05/05/11   tkl     Fixed data_len in ch data TLV for decoded BIP command
04/20/11   tkl     Added SCWS support
04/11/11   tkl     Fixed invalid size for next action indicator TLV
02/20/11   tkl     Added BIP support in decoded format
01/14/11   mib     Review of parsing routines
01/04/11   mib     Fixed invalid TLV in Icon ID list
12/21/10   tkl     Fixed Alpha TLV in Select Item
11/03/10   mib     Fixed compiler warnings
10/06/10   tkl     Fixed extra optional TLV in launch browser indication
09/24/10   mib     Fixed compiler warnings
09/20/10   mib     Fixed Klocwork warnings
08/05/10   tkl     QMI UIM & QMI CAT split from data package
07/27/10   tkl     Removed extra NULL char at the end of USSD & DTMF string.
07/21/10   tkl     Added support to use tag for type of setup call cmd.
07/09/10   mib     Fixed redial TLV as mandatory TLV
07/07/10   tkl     Removed extra NULL char at the end of text string in
                   Select Item & Setup Menu
07/07/10   mib     Added provide local info in next action
07/05/10   mib     Fixed length of bearer list
06/22/10   tkl     Added support for LANGUAGE INFO tlv for PLI-Language and
                   fixed memcpy of item text from GSTK indication into QMI TLV
06/14/10   mib     Removed NULL termination from alpha strings
05/13/10   mib     Initial version
=============================================================================*/


#include "uim_variation.h"
#include "customer.h"
#include "comdef.h"
#include "dsm.h"
#include "amssassert.h"
#include "modem_mem.h"
#include "qmi_svc_utils.h"
#include "qmi_cat_decoded.h"
#include "qmi_uim_cat_common.h"
#include "gstk_p.h"
#include <stringl/stringl.h>
#include "uim_msg.h"

/* These includes need to always be at bottom - offtarget use only */
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*---------------------------------------------------------------------------
  Tags used for proactive commands in decoded format
---------------------------------------------------------------------------*/
#define CATI_PRM_TYPE_END_PROACTIVE_SESSION           (0x1A)
#define CATI_PRM_TYPE_DECODED_HEADER_ID               (0x1B)
#define CATI_PRM_TYPE_TEXT_STRING                     (0x1C)
#define CATI_PRM_TYPE_HIGH_PRIORITY                   (0x1D)
#define CATI_PRM_TYPE_USER_CONTROL                    (0x1E)
#define CATI_PRM_TYPE_ICON                            (0x1F)
#define CATI_PRM_TYPE_DURATION                        (0x20)
#define CATI_PRM_TYPE_RESPONSE_FORMAT                 (0x21)
#define CATI_PRM_TYPE_HELP_AVAILABLE                  (0x22)
#define CATI_PRM_TYPE_RESPONSE_PACKING_FORMAT         (0x23)
#define CATI_PRM_TYPE_RESPONSE_LENGTH                 (0x24)
#define CATI_PRM_TYPE_SHOW_USER_INPUT                 (0x25)
#define CATI_PRM_TYPE_TONE                            (0x26)
#define CATI_PRM_TYPE_SOFT_KEY_SELECTION              (0x27)
#define CATI_PRM_TYPE_MENU_ITEMS                      (0x28)
#define CATI_PRM_TYPE_DEFAULT_ITEM                    (0x29)
#define CATI_PRM_TYPE_NEXT_ACTION_INDICATOR           (0x2A)
#define CATI_PRM_TYPE_ICON_ID_LIST                    (0x2B)
#define CATI_PRM_TYPE_PRESENTATION                    (0x2C)
#define CATI_PRM_TYPE_PACKING_REQUIRED                (0x2D)
#define CATI_PRM_TYPE_SMS_TPDU                        (0x2E)
#define CATI_PRM_TYPE_IS_CDMA_SMS                     (0x2F)
#define CATI_PRM_TYPE_ADDRESS                         (0x30)
#define CATI_PRM_TYPE_CALL_SETUP_REQUIREMENT          (0x31)
#define CATI_PRM_TYPE_REDIAL                          (0x32)
#define CATI_PRM_TYPE_SUBADDRESS                      (0x33)
#define CATI_PRM_TYPE_CAPABILITY_CONFIG               (0x34)
#define CATI_PRM_TYPE_DTMF                            (0x35)
#define CATI_PRM_TYPE_SPECIFIC_LANG_NOTIFY            (0x36)
#define CATI_PRM_TYPE_LANGUAGE                        (0x37)
#define CATI_PRM_TYPE_LAUNCH_MODE                     (0x38)
#define CATI_PRM_TYPE_URL                             (0x39)
#define CATI_PRM_TYPE_BROWSER_ID                      (0x3A)
#define CATI_PRM_TYPE_BEARER_ID                       (0x3B)
#define CATI_PRM_TYPE_NUM_OF_PROVISIONING_FILES       (0x3C)
#define CATI_PRM_TYPE_USSD_STRING                     (0x3D)
#define CATI_PRM_TYPE_DEFAULT_TEXT                    (0x3E)
#define CATI_PRM_TYPE_IMMEDIATE_RESPONSE_REQ          (0x3F)
#define CATI_PRM_TYPE_USER_CNF_ALPHA                  (0x40)
#define CATI_PRM_TYPE_SETUP_CALL_DISPLAY_ALPHA        (0x41)
#define CATI_PRM_TYPE_USER_CONF_ICON                  (0x42)
#define CATI_PRM_TYPE_SETUP_CALL_DISPLAY_ICON         (0x43)
#define CATI_PRM_TYPE_GATEWAY_PROXY                   (0x44)
#define CATI_PRM_TYPE_ALPHA                           (0x45)
#define CATI_PRM_TYPE_REQUIRE_NOTIFICATION            (0x46)
#define CATI_PRM_TYPE_SLOT_ID                         (0x50)
#define CATI_PRM_TYPE_ON_DEMAND_LINK_EST              (0x55)
#define CATI_PRM_TYPE_CDS_BEARER                      (0x56)
#define CATI_PRM_TYPE_GPRS_BEARER                     (0x57)
#define CATI_PRM_TYPE_EUTRAN_EXT_PARAMS_BEARER        (0x58)
#define CATI_PRM_TYPE_EUTRAN_MAPPED_UTRAN_PS_BEARER   (0x59)
#define CATI_PRM_TYPE_BUFFER_SIZE                     (0x5A)
#define CATI_PRM_TYPE_NETWORK_ACCESS_NAME             (0x5B)
#define CATI_PRM_TYPE_OTHER_ADDRESS                   (0x5C)
#define CATI_PRM_TYPE_USER_LOGIN                      (0x5D)
#define CATI_PRM_TYPE_USER_PASSWORD                   (0x5E)
#define CATI_PRM_TYPE_TRANSPORT_LEVEL                 (0x5F)
#define CATI_PRM_TYPE_DATA_DEST_ADDRESS               (0x60)
#define CATI_PRM_TYPE_CHANNEL_DATA_LEN                (0x61)
#define CATI_PRM_TYPE_SEND_DATA_IMMEDIATELY           (0x62)
#define CATI_PRM_TYPE_CHANNEL_DATA                    (0x63)
#define CATI_PRM_TYPE_CHANNEL_ID                      (0x64)
#define CATI_PRM_TYPE_MENU_ITEMS_WITH_DCS             (0x65)
#define CATI_PRM_TYPE_ACTIVATE_TARGET                 (0x67)
#define CATI_PRM_TYPE_RESPONSE_TYPE                   (0x68)
#define CATI_PRM_TYPE_BIP_STATUS                      (0x69)

/*---------------------------------------------------------------------------
  Tags used for scws commands in decoded format
---------------------------------------------------------------------------*/
#define CATI_PRM_TYPE_SCWS_OPEN_CHANNEL_INFO          (0x10)
#define CATI_PRM_TYPE_SCWS_SLOT_ID                    (0x11)
#define CATI_PRM_TYPE_SCWS_ALPHA                      (0x12)
#define CATI_PRM_TYPE_SCWS_CHANNEL_STATE              (0x10)
#define CATI_PRM_TYPE_SCWS_CHANNEL_DATA               (0x10)

/*----------------------------------------------------------
  Tags used for decoded envelope call control response
----------------------------------------------------------*/
#define CATI_PRM_TYPE_CC_RESULT                     (0x10)
#define CATI_PRM_TYPE_CC_ADDRESS                    (0x11)
#define CATI_PRM_TYPE_CC_SUBADDRESS                 (0x12)
#define CATI_PRM_TYPE_CC_CCP1                       (0x13)
#define CATI_PRM_TYPE_CC_CCP2                       (0x14)
#define CATI_PRM_TYPE_CC_USSD_STR                   (0x15)
#define CATI_PRM_TYPE_CC_PDP_ACT                    (0x16)
#define CATI_PRM_TYPE_CC_EPS_PDN_ACT                (0x17)
#define CATI_PRM_TYPE_CC_ALPHA                      (0x18)
#define CATI_PRM_TYPE_CC_BC_REPEAT_IND              (0x19)
#define CATI_PRM_TYPE_SMS_PP_UICC_ACK               (0x1A)
#define CATI_PRM_TYPE_RP_ADDRESS                    (0x1B)
#define CATI_PRM_TYPE_TP_ADDRESS                    (0x1C)
#define CATI_PRM_TYPE_CC_IMS_REQUEST_URI            (0x1D)
#define CATI_PRM_TYPE_POLL_INTERVAL_RESULT          (0x1F)
#define CATI_PRM_TYPE_POLL_INTERVAL_DURATION        (0x20)

/*----------------------------------------------------------
  Tags used for raw envelope response
----------------------------------------------------------*/
#define CATI_PRM_TYPE_RAW_ENV_RSP                   (0x10)

/*---------------------------------------------------------------------------
  Sizes used for TLVs
---------------------------------------------------------------------------*/
#define CATI_TLV_BEARER_LIST_MAX                      (255)
#define CATI_TLV_ICON_SIZE_MAX                        (512)
#define CATI_TLV_FILE_PATH_MAX                        (255)
#define CATI_TLV_URL_MAX                              (255)
#define CATI_TLV_NEXT_ACTION_MAX                      (255)
#define CATI_TLV_DATA_ADDRESS_MAX                     (16)
#define CATI_TLV_DTMF_MAX                             (255)
#define CATI_TLV_LANGUAGE_MAX                         (2)
#define CATI_TLV_BIP_DATA_MAX                         (255)
#define CATI_TLV_RAW_ENV_DATA_MAX                     (255)
#define CATI_TLV_RAW_TR_DATA_MAX                      (255)
#define CATI_TLV_SMS_PP_UICC_ACK_MAX_LENGTH           (128)


/*---------------------------------------------------------------------------
  Response format
---------------------------------------------------------------------------*/
#define CATI_RESPONSE_FORMAT_SMS_DEF_ALPHABET         (0x00)
#define CATI_RESPONSE_FORMAT_YES_NO                   (0x01)
#define CATI_RESPONSE_FORMAT_NUMBERICAL_ONLY          (0x02)
#define CATI_RESPONSE_FORMAT_UCS2_ALPHABET            (0x03)
#define CATI_RESPONSE_FORMAT_IMMEDIATE_DIGIT_RSP      (0x04)
#define CATI_RESPONSE_FORMAT_YES_NO_AND_IMM_DIGT_RSP  (0x05)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SET_EVENT_REPORT
---------------------------------------------------------------------------*/
#define CATI_SET_EVENT_REPORT_RAW_TLV                 (0x10)
#define CATI_SET_EVENT_REPORT_DECODED_TLV             (0x11)
#define CATI_SET_EVENT_REPORT_SLOT_TLV                (0x12)
#define CATI_SET_EVENT_REPORT_FULL_FUNC_TLV           (0x13)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_GET_EVENT_REPORT
---------------------------------------------------------------------------*/
#define CATI_GET_EVENT_REPORT_COMMAND_TLV             (0x01)

/*---------------------------------------------------------------------------
  TLV IDs for SET_EVENT_REPORT
---------------------------------------------------------------------------*/
#define CATI_SET_EVENT_REPORT_RAW_RESP_TLV            (0x10)
#define CATI_SET_EVENT_REPORT_DECODED_RESP_TLV        (0x11)
#define CATI_SET_EVENT_REPORT_FULL_FUNC_RESP_TLV      (0x12)

/*---------------------------------------------------------------------------
  TLV IDs for GET_SERVICE_STATE
---------------------------------------------------------------------------*/
#define CATI_GET_SERVICE_STATE_RAW_TLV                (0x01)
#define CATI_GET_SERVICE_STATE_DECODED_TLV            (0x10)
#define CATI_GET_SERVICE_STATE_FULL_FUNC_TLV          (0x11)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SEND_TR
---------------------------------------------------------------------------*/
#define CATI_RAW_TR_TERMINAL_RESPONSE_TLV             (0x01)
#define CATI_RAW_TR_SLOT_TLV                          (0x10)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SEND_TR_RSP
---------------------------------------------------------------------------*/
#define CATI_RAW_TR_RESPONSE_TLV                      (0x10)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SEND_ENVELOPE_CMD
---------------------------------------------------------------------------*/
#define CATI_RAW_ENVELOPE_ENVELOPE_TLV                (0x01)
#define CATI_RAW_ENVELOPE_SLOT_TLV                    (0x10)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SEND_DECODED_TR
---------------------------------------------------------------------------*/
#define CATI_DECODED_TR_TERMINAL_RESPONSE_TLV         (0x01)
#define CATI_DECODED_TR_TEXT_STRING_TLV               (0x10)
#define CATI_DECODED_TR_ITEM_IDENTIFIER_TLV           (0x11)
#define CATI_DECODED_TR_GET_INKEY_EXTRA_TLV           (0x12)
#define CATI_DECODED_TR_LANGUAGE_TLV                  (0x13)
#define CATI_DECODED_TR_SLOT_TLV                      (0x14)
#define CATI_DECODED_TR_GET_INKEY_YES_NO_TLV          (0x15)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_DECODED_ENVELOPE_CMD
---------------------------------------------------------------------------*/
#define CATI_TYPE_ENVELOPE_REQUEST_REQUIRED           (0x01)
#define CATI_ENV_TYPE_ITEM_CHOSEN                     (0x10)
#define CATI_ENV_TYPE_HELP_REQUESTED                  (0x11)
#define CATI_ENV_TYPE_LANGUAGE                        (0x12)
#define CATI_ENV_TYPE_SLOT                            (0x13)
#define CATI_ENV_TYPE_ADDRESS                         (0x14)
#define CATI_ENV_TYPE_SUBADDRESS                      (0x15)
#define CATI_ENV_TYPE_CAPABILITY_CONF_1               (0x16)
#define CATI_ENV_TYPE_CAPABILITY_CONF_2               (0x17)
#define CATI_ENV_TYPE_USSD_STRING                     (0x18)
#define CATI_ENV_TYPE_PDP_CONTEXT_ACT                 (0x19)
#define CATI_ENV_TYPE_EPS_PDN_CONNECT_ACT             (0x1A)
#define CATI_ENV_TYPE_BROWSER_TERM_CAUSE              (0x1B)
#define CATI_ENV_TYPE_SMS_TPDU                        (0x1C)
#define CATI_ENV_TYPE_IS_CDMA_SMS                     (0x1D)
#define CATI_ENV_TYPE_RAT                             (0x1E)
#define CATI_ENV_TYPE_CALL_TYPE                       (0x1F)
#define CATI_ENV_TYPE_TRANSACTION_ID                  (0x20)
#define CATI_ENV_TYPE_RP_ADDRESS                      (0x21)
#define CATI_ENV_TYPE_TP_ADDRESS                      (0x22)
#define CATI_ENV_TYPE_CAUSE                           (0x23)
#define CATI_ENV_TYPE_IMS_REQUEST_URI                 (0x24)
#define CATI_ENV_TYPE_POLL_INTERVAL_DURATION          (0x27)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_EVENT_CONFIRMATION
---------------------------------------------------------------------------*/
#define CATI_EVENT_CONFIRMATION_USER_CONFIRMED_TLV    (0x10)
#define CATI_EVENT_CONFIRMATION_ICON_DISPLAYED_TLV    (0x11)
#define CATI_EVENT_CONFIRMATION_SLOT_TLV              (0x12)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SCWS_OPEN_CHANNEL
  TLV tag ids for QMI_CAT_SCWS_CLOSE_CHANNEL
  TLV tag ids for QMI_CAT_SCWS_CHANNEL_STATUS
---------------------------------------------------------------------------*/
#define CATI_SCWS_CHANNEL_STATE_TLV                   (0x01)
#define CATI_SCWS_CHANNEL_STATE_SLOT_TLV              (0x10)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SCWS_SEND_DATA
---------------------------------------------------------------------------*/
#define CATI_SCWS_SEND_DATA_CHANNEL_DATA_RESULT_TLV   (0x01)
#define CATI_SCWS_SEND_DATA_SLOT_TLV                  (0x10)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SCWS_DATA_AVAILABLE
---------------------------------------------------------------------------*/
#define CATI_SCWS_DATA_AVAILABLE_CHANNEL_DATA_TLV     (0x01)
#define CATI_SCWS_DATA_AVAILABLE_REMAIN_DATA_LEN_TLV  (0x02)
#define CATI_SCWS_DATA_AVAILABLE_SLOT_TLV             (0x10)

/*---------------------------------------------------------------------------
  TLV tag ids for CATI_GET_TERMINAL_PROFILE_SLOT_TLV
---------------------------------------------------------------------------*/
#define CATI_GET_TERMINAL_PROFILE_SLOT_TLV            (0x10)

/*---------------------------------------------------------------------------
  TLV IDs for GET_TERMINAL_PROFILE
---------------------------------------------------------------------------*/
#define CATI_GET_TERMINAL_PROFILE_DATA_TLV            (0x10)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SET_CONFIGURATION
---------------------------------------------------------------------------*/
#define CATI_SET_CONFIGURATION_CONFIG_MODE_TLV        (0x01)
#define CATI_SET_CONFIGURATION_CUSTOM_TP_TLV          (0x10)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_GET_CONFIGURATION
---------------------------------------------------------------------------*/
#define CATI_GET_CONFIGURATION_CONFIG_MODE_TLV        (0x10)
#define CATI_GET_CONFIGURATION_CUSTOM_TP_TLV          (0x11)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_GET_CACHED_PROACTIVE_CMD
---------------------------------------------------------------------------*/
#define CATI_GET_CACHED_PROACTIVE_CMD_CMD_TLV         (0x01)
#define CATI_GET_CACHED_PROACTIVE_CMD_SLOT_TLV        (0x10)

/*---------------------------------------------------------------------------
  Misc
---------------------------------------------------------------------------*/
#define QMI_CATI_SMS_GENERAL_DATA_CODING_MASK           0xCC
#define QMI_CATI_SMS_8_BIT_GENERAL_DATA_CODING_DCS      0x04
#define QMI_CATI_SMS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS 0x08
#define QMI_CATI_SMS_DATA_CODING_MASK                   0xFC
#define QMI_CATI_SMS_8_BIT_DCS                          0xF4

#define QMI_CATI_CBS_DATA_CODING_MASK                   0xFC
#define QMI_CATI_CBS_8_BIT_DCS                          0xF4
#define QMI_CATI_CBS_GENERAL_DATA_CODING_MASK           0xCC
#define QMI_CATI_CBS_8_BIT_GENERAL_DATA_CODING_DCS      0x44
#define QMI_CATI_CBS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS 0x48
#define QMI_CATI_CBS_UCS2_7_BIT_DCS                     0x11

/*---------------------------------------------------------------------------
  Command id in TLV for decoded proactive command
---------------------------------------------------------------------------*/
#define CATI_EVENT_DISPLAY_TEXT_REQ                     0x01
#define CATI_EVENT_GET_INKEY_REQ                        0x02
#define CATI_EVENT_GET_INPUT_REQ                        0x03
#define CATI_EVENT_LAUNCH_BROWSER_REQ                   0x04
#define CATI_EVENT_PLAY_TONE_REQ                        0x05
#define CATI_EVENT_SELECT_ITEM_REQ                      0x06
#define CATI_EVENT_SEND_SMS_REQ                         0x07
#define CATI_EVENT_SEND_SS_REQ                          0x08
#define CATI_EVENT_SEND_USSD_REQ                        0x09
#define CATI_EVENT_SETUP_CALL_USER_CONFIRMATION_REQ     0x0A
#define CATI_EVENT_SETUP_CALL_ALPHA_DISPLAY_REQ         0x0B
#define CATI_EVENT_SETUP_MENU_REQ                       0x0C
#define CATI_EVENT_SETUP_IDLE_TEXT_REQ                  0x0D
#define CATI_EVENT_PROVIDE_LANGUAGE_INFO_REQ            0x0E
#define CATI_EVENT_SEND_DTMF_REQ                        0x0F
#define CATI_EVENT_LANGUAGE_NOTIFICATION_REQ            0x10
#define CATI_EVENT_SETUP_EVENT_USER_ACTIVITY_REQ        0x11
#define CATI_EVENT_SETUP_EVENT_IDLE_SCREEN_REQ          0x12
#define CATI_EVENT_SETUP_EVENT_LANGUAGE_SEL_REQ         0x13
#define CATI_EVENT_OPEN_CHANNEL_REQ                     0x14
#define CATI_EVENT_CLOSE_CHANNEL_REQ                    0x15
#define CATI_EVENT_RECEIVE_DATA_REQ                     0x16
#define CATI_EVENT_SEND_DATA_REQ                        0x17
#define CATI_EVENT_ACTIVATE_REQ                         0x18
#define CATI_EVENT_SETUP_EVENT_HCI_CONN_REQ             0x19
#define CATI_EVENT_REFRESH_ALPHA_REQ                    0x1A
#define CATI_EVENT_SETUP_EVENT_BROWSER_TERM_REQ         0x20


/*===========================================================================

                             MACROS

===========================================================================*/

/*---------------------------------------------------------------------------
  Macro is used to check slot id TLV
---------------------------------------------------------------------------*/
#define QMI_CAT_CHECK_SLOT_ID_TLV(ptr, result)                               \
        /* Check for Slot id TLV range as defined in QMI CAT ICD*/           \
        if (ptr->slot_present &&                                             \
            ptr->slot.slot != 1 &&                                           \
            ptr->slot.slot != 2 &&                                           \
            ptr->slot.slot != 3 &&                                           \
            ptr->slot.slot != 4 &&                                           \
            ptr->slot.slot != 5 )                                            \
        {                                                                    \
          UIM_MSG_ERR_0("QMI_CAT invalid slot id TLV");                      \
          result = QMI_ERR_INVALID_ARG_V01;                                      \
        }

/*===========================================================================

                          FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_CATI_CONVERT_GSTK_DCS_TO_QMI

  DESCRIPTION
    to convert from GSTK to QMI DCS value

  PARAMETERS
    gstk_dcs : GSTK DCS value
    qmi_dcs  : QMI DCS value
    cb       : Cb dcs
  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cati_convert_gstk_dcs_to_qmi
(
  uint8      gstk_dcs,
  uint8   *  qmi_dcs,
  boolean    cb
)
{
  ASSERT(qmi_dcs != NULL);

  if (!cb)
  {
    if((gstk_dcs & QMI_CATI_SMS_GENERAL_DATA_CODING_MASK) == QMI_CATI_SMS_8_BIT_GENERAL_DATA_CODING_DCS)
    {
      *qmi_dcs = (uint8)CATI_DCS_8_BIT;
      return;
    }
    if((gstk_dcs & QMI_CATI_SMS_GENERAL_DATA_CODING_MASK) == QMI_CATI_SMS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS)
    {
      *qmi_dcs = (uint8)CATI_DCS_UCS2_8_BIT;
       return;
    }
    if((gstk_dcs & QMI_CATI_SMS_DATA_CODING_MASK) == QMI_CATI_SMS_8_BIT_DCS)
    {
      *qmi_dcs = (uint8)CATI_DCS_8_BIT;
      return;
    }
  }
  else
  {
    if ((gstk_dcs & QMI_CATI_CBS_DATA_CODING_MASK) == QMI_CATI_CBS_8_BIT_DCS)
    {
      *qmi_dcs = (uint8)CATI_DCS_8_BIT;
      return;
    }
    if ((gstk_dcs & QMI_CATI_CBS_GENERAL_DATA_CODING_MASK) == QMI_CATI_CBS_8_BIT_GENERAL_DATA_CODING_DCS)
    {
      *qmi_dcs = (uint8)CATI_DCS_8_BIT;
      return;
    }
    if ((gstk_dcs & QMI_CATI_CBS_GENERAL_DATA_CODING_MASK) == QMI_CATI_CBS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS)
    {
      *qmi_dcs = (uint8)CATI_DCS_UCS2_8_BIT;
      return;
    }
    if ((gstk_dcs & QMI_CATI_CBS_UCS2_7_BIT_DCS) == QMI_CATI_CBS_UCS2_7_BIT_DCS)
    {
      *qmi_dcs = (uint8)CATI_DCS_UCS2_7_BIT;
      return;
    }
  }

  *qmi_dcs = (uint8)CATI_DCS_7_BIT;
} /* qmi_cati_convert_gstk_dcs_to_qmi */

/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_RAW_PROACTIVE_COMMAND()

  DESCRIPTION
    Populates the tlvs from the raw proactive command

  PARAMETERS
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
)
{
  qmi_error_type_v01 errval = QMI_ERR_NONE_V01;

  /* Check length */
  if (raw_data_len > CAT_RAW_MAX_DATA_LEN)
  {
    return QMI_ERR_NO_MEMORY_V01;
  }

  if(mask & CATI_REPORT_STATUS_EV_DISPLAY_TEXT)
  {
        ind_ptr->display_text_valid = TRUE;
        ind_ptr->display_text.uim_ref_id = cmd_ref_id;
        ind_ptr->display_text.pc_display_text_len = raw_data_len;
        (void)memscpy(ind_ptr->display_text.pc_display_text,
                  sizeof(ind_ptr->display_text.pc_display_text),
                  raw_data_ptr,
                  raw_data_len);
  }
  else if(mask & CATI_REPORT_STATUS_EV_GET_INKEY)
  {
        ind_ptr->get_inkey_valid = TRUE;
        ind_ptr->get_inkey.uim_ref_id = cmd_ref_id;
        ind_ptr->get_inkey.pc_get_inkey_len = raw_data_len;
        (void)memscpy(ind_ptr->get_inkey.pc_get_inkey,
                  sizeof(ind_ptr->get_inkey.pc_get_inkey),
                  raw_data_ptr,
                  raw_data_len);
  }
  else if(mask & CATI_REPORT_STATUS_EV_GET_INPUT)
  {
        ind_ptr->get_input_valid = TRUE;
        ind_ptr->get_input.uim_ref_id = cmd_ref_id;
        ind_ptr->get_input.pc_get_input_len = raw_data_len;
        (void)memscpy(ind_ptr->get_input.pc_get_input,
                  sizeof(ind_ptr->get_input.pc_get_input),
                  raw_data_ptr,
                  raw_data_len);
  }
  else if(mask & CATI_REPORT_STATUS_EV_SETUP_MENU)
  {
        ind_ptr->setup_menu_valid = TRUE;
        ind_ptr->setup_menu.uim_ref_id = cmd_ref_id;
        ind_ptr->setup_menu.pc_setup_menu_len = raw_data_len;
        (void)memscpy(ind_ptr->setup_menu.pc_setup_menu,
                  sizeof(ind_ptr->setup_menu.pc_setup_menu),
                  raw_data_ptr,
                  raw_data_len);
  }
  else if(mask & CATI_REPORT_STATUS_EV_SELECT_ITEM)
  {
        ind_ptr->select_item_valid = TRUE;
        ind_ptr->select_item.uim_ref_id = cmd_ref_id;
        ind_ptr->select_item.pc_select_item_len = raw_data_len;
        (void)memscpy(ind_ptr->select_item.pc_select_item,
                  sizeof(ind_ptr->select_item.pc_select_item),
                  raw_data_ptr,
                  raw_data_len);
  }  
  else if(mask & CATI_REPORT_STATUS_EV_SENDSMS_ALPHA_ID)
  {
        ind_ptr->send_sms_valid = TRUE;
        ind_ptr->send_sms.uim_ref_id = cmd_ref_id;
        ind_ptr->send_sms.pc_send_sms_len = raw_data_len;
        (void)memscpy(ind_ptr->send_sms.pc_send_sms,
                  sizeof(ind_ptr->send_sms.pc_send_sms),
                  raw_data_ptr,
                  raw_data_len);
  }
  else if(mask & CATI_REPORT_STATUS_EV_SETUP_IDL_MODE_TEXT)
  {
        ind_ptr->idle_mode_text_valid = TRUE;
        ind_ptr->idle_mode_text.uim_ref_id = cmd_ref_id;
        ind_ptr->idle_mode_text.pc_setup_idle_mode_text_len = raw_data_len;
        (void)memscpy(ind_ptr->idle_mode_text.pc_setup_idle_mode_text,
                  sizeof(ind_ptr->idle_mode_text.pc_setup_idle_mode_text),
                  raw_data_ptr,
                  raw_data_len);
  }
  else if(mask & CATI_REPORT_STATUS_EV_LANG_NOTIFICATION)
  {
        ind_ptr->lang_notification_valid = TRUE;
        ind_ptr->lang_notification.uim_ref_id = cmd_ref_id;
        ind_ptr->lang_notification.pc_lang_notification_len = raw_data_len;
        (void)memscpy(ind_ptr->lang_notification.pc_lang_notification,
                  sizeof(ind_ptr->lang_notification.pc_lang_notification),
                  raw_data_ptr,
                  raw_data_len);
  }
  else if(mask & CATI_REPORT_STATUS_EV_PLAY_TONE)
  {
        ind_ptr->play_tone_valid = TRUE;
        ind_ptr->play_tone.uim_ref_id = cmd_ref_id;
        ind_ptr->play_tone.pc_play_tone_len= raw_data_len;
        (void)memscpy(ind_ptr->play_tone.pc_play_tone,
                  sizeof(ind_ptr->play_tone.pc_play_tone),
                  raw_data_ptr,
                  raw_data_len);
  }
  else if(mask & CATI_REPORT_STATUS_EV_SETUP_CALL)
  {
        ind_ptr->setup_call_valid = TRUE;
        ind_ptr->setup_call.uim_ref_id = cmd_ref_id;
        ind_ptr->setup_call.pc_setup_call_len = raw_data_len;
        (void)memscpy(ind_ptr->setup_call.pc_setup_call,
                  sizeof(ind_ptr->setup_call.pc_setup_call),
                  raw_data_ptr,
                  raw_data_len);
  }
  else if(mask & CATI_REPORT_STATUS_EV_SEND_DTMF)
  {
        ind_ptr->send_dtmf_valid = TRUE;
        ind_ptr->send_dtmf.uim_ref_id = cmd_ref_id;
        ind_ptr->send_dtmf.pc_send_dtmf_len = raw_data_len;
        (void)memscpy(ind_ptr->send_dtmf.pc_send_dtmf,
                  sizeof(ind_ptr->send_dtmf.pc_send_dtmf),
                  raw_data_ptr,
                  raw_data_len);
  }
  else if(mask & CATI_REPORT_STATUS_EV_LAUNCH_BROWSER)
  {
        ind_ptr->launch_browser_valid = TRUE;
        ind_ptr->launch_browser.uim_ref_id = cmd_ref_id;
        ind_ptr->launch_browser.pc_launch_browser_len = raw_data_len;
        (void)memscpy(ind_ptr->launch_browser.pc_launch_browser,
                  sizeof(ind_ptr->launch_browser.pc_launch_browser),
                  raw_data_ptr,
                  raw_data_len);
  }
  else if(mask & CATI_REPORT_STATUS_EV_SEND_SS)
  {
        ind_ptr->send_ss_valid = TRUE;
        ind_ptr->send_ss.uim_ref_id = cmd_ref_id;
        ind_ptr->send_ss.pc_send_ss_len = raw_data_len;
        (void)memscpy(ind_ptr->send_ss.pc_send_ss,
                  sizeof(ind_ptr->send_ss.pc_send_ss),
                  raw_data_ptr,
                  raw_data_len);
  }
  else if(mask & CATI_REPORT_STATUS_EV_SEND_USSD)
  {
        ind_ptr->send_ussd_valid = TRUE;
        ind_ptr->send_ussd.uim_ref_id = cmd_ref_id;
        ind_ptr->send_ussd.pc_send_ussd_len = raw_data_len;
        (void)memscpy(ind_ptr->send_ussd.pc_send_ussd,
                  sizeof(ind_ptr->send_ussd.pc_send_ussd),
                  raw_data_ptr,
                  raw_data_len);
  }
  else if(mask & CATI_PROVIDE_LOCAL_INFO_MASK)
  {
        ind_ptr->provide_local_info_valid = TRUE;
        ind_ptr->provide_local_info.uim_ref_id = cmd_ref_id;
        ind_ptr->provide_local_info.pc_provide_local_info_len = raw_data_len;
        (void)memscpy(ind_ptr->provide_local_info.pc_provide_local_info,
                  sizeof(ind_ptr->provide_local_info.pc_provide_local_info),
                  raw_data_ptr,
                  raw_data_len);
  }
  else if(mask & CATI_SETUP_EVT_LIST_MASK)
  {
        ind_ptr->setup_event_list_raw_valid = TRUE;
        ind_ptr->setup_event_list_raw.uim_ref_id = cmd_ref_id;
        ind_ptr->setup_event_list_raw.pc_setup_event_list_len = raw_data_len;
        (void)memscpy(ind_ptr->setup_event_list_raw.pc_setup_event_list,
                  sizeof(ind_ptr->setup_event_list_raw.pc_setup_event_list),
                  raw_data_ptr,
                  raw_data_len);
  }
  else if(mask & CATI_REPORT_STATUS_EV_DATA_EVT)
  {
        switch(event_type)
        {
          case CATI_DATA_EVENT_OPEN_CHANNEL:
            ind_ptr->open_channel_valid = TRUE;
            ind_ptr->open_channel.uim_ref_id = cmd_ref_id;
            ind_ptr->open_channel.pc_open_channel_len = raw_data_len;
            (void)memscpy(ind_ptr->open_channel.pc_open_channel,
                  sizeof(ind_ptr->open_channel.pc_open_channel),
                  raw_data_ptr,
                  raw_data_len);
            break;
          case CATI_DATA_EVENT_CLOSE_CHANNEL:
            ind_ptr->close_channel_valid = TRUE;
            ind_ptr->close_channel.uim_ref_id = cmd_ref_id;
            ind_ptr->close_channel.pc_close_channel_len = raw_data_len;
            (void)memscpy(ind_ptr->close_channel.pc_close_channel,
                  sizeof(ind_ptr->close_channel.pc_close_channel),
                  raw_data_ptr,
                  raw_data_len);
            break;
          case CATI_DATA_EVENT_RECEIVE_DATA:
            ind_ptr->receive_data_valid = TRUE;
            ind_ptr->receive_data.uim_ref_id = cmd_ref_id;
            ind_ptr->receive_data.pc_receive_data_len = raw_data_len;
            (void)memscpy(ind_ptr->receive_data.pc_receive_data,
                  sizeof(ind_ptr->receive_data.pc_receive_data),
                  raw_data_ptr,
                  raw_data_len);
            break;
          case CATI_DATA_EVENT_SEND_DATA:
            ind_ptr->send_data_valid = TRUE;
            ind_ptr->send_data.uim_ref_id = cmd_ref_id;
            ind_ptr->send_data.pc_send_data_len = raw_data_len;
            (void)memscpy(ind_ptr->send_data.pc_send_data,
                  sizeof(ind_ptr->send_data.pc_send_data),
                  raw_data_ptr,
                  raw_data_len);
            break;
          default:
            break;
        }
  }
  else if(mask & CATI_REPORT_STATUS_EV_ACTIVATE)
  {
        ind_ptr->activate_valid = TRUE;
        ind_ptr->activate.uim_ref_id = cmd_ref_id;
        ind_ptr->activate.pc_activate_len = raw_data_len;
        (void)memscpy(ind_ptr->activate.pc_activate,
                  sizeof(ind_ptr->activate.pc_activate),
                  raw_data_ptr,
                  raw_data_len);
  }
  else if(mask & CATI_REPORT_STATUS_EV_REFRESH)
  {
        ind_ptr->refresh_valid = TRUE;
        ind_ptr->refresh.refresh_mode = raw_data_ptr[7];
        ind_ptr->refresh.refresh_stage= 0;
  }
  else
  {
        UIM_MSG_ERR_0("Received unhandled Raw Proactive cmd");
        return QMI_ERR_INTERNAL_V01;
    }

  switch(slot_id)
  {
    case GSTK_SLOT_1:
      ind_ptr->slot_valid = TRUE;
      ind_ptr->slot.slot = 1;
      break;
    case GSTK_SLOT_2:
      ind_ptr->slot_valid = TRUE;
      ind_ptr->slot.slot = 2;
      break;
    case GSTK_SLOT_3:
      ind_ptr->slot_valid = TRUE;
      ind_ptr->slot.slot = 3;
      break;
    default:
      /* Invalid slot id... return SUCCESS, without adding any TLV. */
      errval = QMI_ERR_NONE_V01;
  }

  switch(response_type)
  {
    case GSTK_TAG_ORIGINAL_CMD:
      break;
    case GSTK_TAG_USER_CONF_ALPHA:
    case GSTK_TAG_DISPLAY_ALPHA:
      ind_ptr->user_conf_alpha_valid = TRUE;
      ind_ptr->alpha_valid = TRUE;
      break;
    default:
      return QMI_ERR_INTERNAL_V01;
  }

  return errval;
} /* qmi_cat_response_raw_proactive_command */


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
)
{
  qmi_error_type_v01 errval = QMI_ERR_NONE_V01;

  memset(response_ptr, 0, sizeof(cat_get_event_report_resp_msg_v02));

  response_ptr->proactive_session_end_type.proactive_session_end_type = end_type;
  response_ptr->proactive_session_end_type_valid = TRUE;

  switch(slot_id)
  {
    case GSTK_SLOT_1:
      response_ptr->slot_valid = TRUE;
      response_ptr->slot.slot = 1;
      break;
    case GSTK_SLOT_2:
      response_ptr->slot_valid = TRUE;
      response_ptr->slot.slot = 2;
      break;
    case GSTK_SLOT_3:
      response_ptr->slot_valid = TRUE;
      response_ptr->slot.slot = 3;
      break;
    default:
      /* Invalid slot id... return SUCCESS, without adding any TLV. */
      errval = QMI_ERR_NONE_V01;
  }
  
  return errval;
} /* qmi_cat_response_end_proactive_session */


/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_SET_EVENT_REPORT()

  DESCRIPTION
    Populates the tlv for the set event report

  PARAMETERS
    ind                 : output sdu
    raw_mask            : raw mask
    decoded_mask        : decoded mask
    full_func_mask      : full function mask

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
)
{

  if (raw_mask != 0)
  {
    response_ptr->pc_evt_report_req_err_mask_valid = TRUE;
    response_ptr->pc_evt_report_req_err_mask = raw_mask;
  }

  if (decoded_mask != 0)
  {
    response_ptr->pc_dec_evt_report_req_err_mask_valid = TRUE;
    response_ptr->pc_dec_evt_report_req_err_mask = decoded_mask;
  }

  if (full_func_mask != 0)
  {
    response_ptr->pc_full_func_evt_report_err_mask_valid = TRUE;
    response_ptr->pc_full_func_evt_report_err_mask = decoded_mask;
  }
  return QMI_ERR_NONE_V01;
} /* qmi_cat_response_set_event_report */


/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_GET_SERVICE_STATE()

  DESCRIPTION
    Populates the tlv for the response of get service state

  PARAMETERS
    ind                 : output sdu
    raw_client_mask     : raw mask of this client
    raw_all_mask        : raw mask of all clients
    decoded_client_mask : decoded mask of this client
    decoded_all_mask    : decoded mask of all clients
    full_func_mask      : full function mask for all slots

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
)
{

  response_ptr->cat_service_state.cat_common_evt_reg_state_mask = raw_client_mask;
  response_ptr->cat_service_state.pc_evt_report_mask = raw_global_mask;
  if(decoded_client_mask != 0 || decoded_global_mask != 0)
  {
    response_ptr->decoded_cat_service_state_valid = TRUE;
    response_ptr->decoded_cat_service_state.cat_common_evt_reg_state_mask = decoded_client_mask;
    response_ptr->decoded_cat_service_state.pc_evt_report_mask = decoded_global_mask;
  }
  if(full_func_global_mask != 0)
  {
    response_ptr->pc_full_func_evt_report_mask_valid = TRUE;
    response_ptr->pc_full_func_evt_report_mask= full_func_global_mask;
  }
  return QMI_ERR_NONE_V01;
} /* qmi_cat_response_get_service_state */


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
)
{

  if(buf_len > GSTK_TERMINAL_PROFILE_SIZE)
  {
    return QMI_ERR_INTERNAL_V01;
  }
  
  response_ptr->terminal_profile_data_valid = (uint8_t)TRUE; 
  response_ptr->terminal_profile_data_len = (uint32_t)buf_len;
  (void)memscpy(response_ptr->terminal_profile_data,
                sizeof(response_ptr->terminal_profile_data),
                buf_ptr,
                buf_len);

  return QMI_ERR_NONE_V01;
} /* qmi_cat_response_get_terminal_profile */


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
)
{
    /* Check length */
  if (custom_tp_len > CATI_TLV_CUSTOM_TP_MAX)
  {
    return QMI_ERR_NO_MEMORY_V01;
  }
  
  response_ptr->cat_config_mode_valid = TRUE;
  response_ptr->cat_config_mode = config_mode;
  
  if(custom_tp_len == 0)
  {
    UIM_MSG_HIGH_0("custom_tp not available, skip tlv");
  }
  else if (custom_tp_len > 0 && custom_tp_ptr != NULL)
  {
    response_ptr->custom_tp_len = custom_tp_len;
    (void)memscpy(response_ptr->custom_tp,
                  sizeof(response_ptr->custom_tp),
                  custom_tp_ptr,
                  custom_tp_len);
  }
  else
  {
    UIM_MSG_ERR_0("Unable to generate TLV for custom_tp!");
    return QMI_ERR_INTERNAL_V01;
  }

  return QMI_ERR_NONE_V01;
} /* qmi_cat_response_get_configuration */

/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_SET_EVENT_REPORT()

  DESCRIPTION
    Parses the TLV content of QMI_UIM message

  PARAMETERS
    req_ptr     : input req msg
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
)
{
  qmi_error_type_v01 result = QMI_ERR_NONE_V01;

  ASSERT(req_ptr);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_set_event_report_type));

  if(req_ptr->pc_evt_report_req_mask_valid == TRUE)
  {
    parsed_data_ptr->raw_mask_present = TRUE;
    parsed_data_ptr->raw_mask = req_ptr->pc_evt_report_req_mask;
  }

  if(req_ptr->pc_dec_evt_report_req_mask_valid == TRUE)
  {
    parsed_data_ptr->decoded_mask_present = TRUE;
    parsed_data_ptr->decoded_mask = req_ptr->pc_dec_evt_report_req_mask;
  }

  if(req_ptr->pc_full_func_evt_report_req_mask_valid == TRUE)
  {
    parsed_data_ptr->full_func_mask_present = TRUE;
    parsed_data_ptr->full_func_mask = req_ptr->pc_full_func_evt_report_req_mask;
  }

  if(req_ptr->slot_mask_valid == TRUE)
  {
    parsed_data_ptr->slot_present = TRUE;
    parsed_data_ptr->slot_mask = req_ptr->slot_mask;
  }

  if (result == QMI_ERR_NONE_V01)
  {
    /* Check that at least one of the 3 TLV is received */
    if (parsed_data_ptr->raw_mask_present == FALSE &&
        parsed_data_ptr->decoded_mask_present == FALSE &&
        parsed_data_ptr->full_func_mask_present == FALSE)
    {
      result = QMI_ERR_MISSING_ARG_V01;
    }

    /* Check on slot */
    if (parsed_data_ptr->slot_present &&
        (parsed_data_ptr->slot_mask & 0x07) == 0)
    {
      result = QMI_ERR_INVALID_ARG_V01;
    }
  }

  return result;
} /* qmi_cat_parse_request_set_event_report */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_GET_EVENT_REPORT()

  DESCRIPTION
    Parses the TLV content of QMI_UIM message

  PARAMETERS
    req_ptr     : input req msg
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
)
{
  qmi_error_type_v01 result = QMI_ERR_NONE_V01;

  ASSERT(req_ptr);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_get_event_report_type));

  if ((req_ptr->proactive_command_input.format != 1) && 
      (req_ptr->proactive_command_input.format != 2))
  {
    result = QMI_ERR_MISSING_ARG_V01;
  }
  else
  {
    parsed_data_ptr->proactive_command_present = TRUE;
    parsed_data_ptr->proactive_command.cmd_ref_id = req_ptr->proactive_command_input.cmd_ref_id;
    parsed_data_ptr->proactive_command.format = req_ptr->proactive_command_input.format;
  }

  if (parsed_data_ptr->proactive_command_present &&
      parsed_data_ptr->proactive_command.format != 1 &&
      parsed_data_ptr->proactive_command.format != 2)
  {
    result = QMI_ERR_INVALID_ARG_V01;
  }

  return result;
} /* qmi_cat_parse_request_get_event_report */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_RAW_TR()

  DESCRIPTION
    Parses the TLV content of QMI_UIM message

  PARAMETERS
    req_ptr     : input req msg
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
  cat_send_tr_req_msg_v02  * req_ptr,
  qmi_cat_in_raw_tr_type   * parsed_data_ptr
)
{
  qmi_error_type_v01 result = QMI_ERR_NONE_V01;

  ASSERT(req_ptr);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_raw_tr_type));

  if(req_ptr->terminal_response.terminal_response_len == 0)
  {
    result = QMI_ERR_MISSING_ARG_V01;
  }

  if (req_ptr->terminal_response.terminal_response_len >
          sizeof(req_ptr->terminal_response.terminal_response))
  {
    result = QMI_ERR_MALFORMED_MSG_V01;
  }
  else
  {
    parsed_data_ptr->term_resp_present = TRUE;
    parsed_data_ptr->term_resp.tr_len = (uint16)req_ptr->terminal_response.terminal_response_len;
    parsed_data_ptr->term_resp.cmd_ref_id = req_ptr->terminal_response.uim_ref_id;
    (void)memscpy(parsed_data_ptr->term_resp.terminal_response,
                  parsed_data_ptr->term_resp.tr_len,
                  req_ptr->terminal_response.terminal_response,
                  req_ptr->terminal_response.terminal_response_len);
  }

  /* Check on slot */
  QMI_CAT_CHECK_SLOT_ID_TLV(parsed_data_ptr, result);

  if(req_ptr->slot_valid == TRUE)
  {
    parsed_data_ptr->slot_present = TRUE;
    parsed_data_ptr->slot.slot    = req_ptr->slot.slot;
  }
  return result;
} /* qmi_cat_parse_request_raw_tr */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_RAW_ENVELOPE()

  DESCRIPTION
    Parses the TLV content of QMI_UIM message

  PARAMETERS
    req_ptr     : input req msg
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
  cat_send_envelope_cmd_req_msg_v02  * req_ptr,
  qmi_cat_in_raw_envelope_type   * parsed_data_ptr
)
{
  qmi_error_type_v01 result = QMI_ERR_NONE_V01;

  ASSERT(req_ptr);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_raw_envelope_type));

  /* Check that all mandatory TLV were received */
  if (req_ptr->envelope_cmd.envelope_data_len == 0)
  {
    result = QMI_ERR_MISSING_ARG_V01;
  }
  else
  {
    parsed_data_ptr->envelope_present = TRUE;
    parsed_data_ptr->envelope.env_cmd_type = req_ptr->envelope_cmd.env_cmd_type;
    parsed_data_ptr->envelope.env_cmd_len = (uint16)req_ptr->envelope_cmd.envelope_data_len;
    (void)memscpy(parsed_data_ptr->envelope.envelope_data,
                  sizeof(parsed_data_ptr->envelope.envelope_data),
                  req_ptr->envelope_cmd.envelope_data,
                  req_ptr->envelope_cmd.envelope_data_len);
  }

 
  /* Additional checks on data */
  
  /* Check on terminal response */
  if (parsed_data_ptr->envelope_present &&
      parsed_data_ptr->envelope.env_cmd_len >
        sizeof(parsed_data_ptr->envelope.envelope_data))
  {
    result = QMI_ERR_MALFORMED_MSG_V01;
  }

  if(req_ptr->slot_valid)
  {
    /* Check on slot */
    parsed_data_ptr->slot_present = req_ptr->slot_valid;
    parsed_data_ptr->slot.slot = req_ptr->slot.slot;
  }
  QMI_CAT_CHECK_SLOT_ID_TLV(parsed_data_ptr, result);

  return result;
} /* qmi_cat_parse_request_raw_envelope */

/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_EVENT_CONFIRMATION()

  DESCRIPTION
    Parses the TLV content of QMI_CAT message

  PARAMETERS
    req_ptr     : input req msg
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
)
{
  qmi_error_type_v01 result = QMI_ERR_NONE_V01;

  ASSERT(req_ptr);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_event_confirmation_type));

  if(req_ptr->confirm_valid == TRUE)
  {
    parsed_data_ptr->user_confirmed_present = TRUE;
    parsed_data_ptr->user_confirmed.user_confirmed = req_ptr->confirm.confirm;
  }

  if(req_ptr->display_valid == TRUE)
  {
    parsed_data_ptr->icon_display_present = TRUE;
    parsed_data_ptr->icon_display.icon_display = req_ptr->display.display;
  }

  if(req_ptr->slot_valid == TRUE)
  {
    parsed_data_ptr->slot_present = TRUE;
    parsed_data_ptr->slot.slot = req_ptr->slot.slot;
  }

  /* Check on user confirmation */
  if (parsed_data_ptr->user_confirmed_present &&
      parsed_data_ptr->user_confirmed.user_confirmed != 0 &&
      parsed_data_ptr->user_confirmed.user_confirmed != 1)
  {
    result = QMI_ERR_INVALID_ARG_V01;
  }
  /* Check on icon display */
  if (parsed_data_ptr->icon_display_present &&
      parsed_data_ptr->icon_display.icon_display != 0 &&
      parsed_data_ptr->icon_display.icon_display != 1)
  {
    result = QMI_ERR_INVALID_ARG_V01;
  }
  /* Check on slot */
  QMI_CAT_CHECK_SLOT_ID_TLV(parsed_data_ptr, result);

  return result;
} /* qmi_cat_parse_request_event_confirmation */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_SET_CONFIGURATION()

  DESCRIPTION
    Parses the TLV content of QMI_UIM message

  PARAMETERS
    req_ptr     : input req msg
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
)
{
  qmi_error_type_v01  result = QMI_ERR_NONE_V01;

  ASSERT(req_ptr);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_set_configuration_type));

  if (req_ptr->cat_config_mode < CAT_CONFIG_MODE_DISABLED_V02 && 
      req_ptr->cat_config_mode > CAT_CONFIG_MODE_CUSTOM_DECODED_V02)
  {
    result = QMI_ERR_MISSING_ARG_V01;
  }
  else
  {
    parsed_data_ptr->config_mode_present = TRUE;
    parsed_data_ptr->config_mode.config_mode = req_ptr->cat_config_mode;
  }

  /* Additional checks on custom_tp */
  if (req_ptr->custom_tp_valid &&
        req_ptr->custom_tp_len >
        sizeof(req_ptr->custom_tp))
  {
    result = QMI_ERR_MALFORMED_MSG_V01;
  }
  else if(req_ptr->custom_tp_valid)
  {
    parsed_data_ptr->custom_tp_present = TRUE;
    parsed_data_ptr->custom_tp.tp_len = (uint8)req_ptr->custom_tp_len;
    (void)memscpy(parsed_data_ptr->custom_tp.tp_bitmask,
                  parsed_data_ptr->custom_tp.tp_len,
                  req_ptr->custom_tp,
                  req_ptr->custom_tp_len);
  }
  return result;
} /* qmi_cat_parse_request_set_configuration */


/*===========================================================================
  FUNCTION  QMI_CAT_RESPONSE_RAW_ENVELOPE()

  DESCRIPTION
    Process the raw evnvelope response

  PARAMETERS
   gstk_cmd_from_card_type             : Pointer to command from card
   cat_send_envelope_cmd_resp_msg_v02* : pointer to send envelope response msg

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
  cat_send_envelope_cmd_resp_msg_v02                 * response_ptr
)
{
  ASSERT(env_rsp_ptr != NULL);
  
  response_ptr->env_resp_data.sw1 = env_rsp_ptr->cmd.raw_envelope_rsp.sw1;
  response_ptr->env_resp_data.sw2 = env_rsp_ptr->cmd.raw_envelope_rsp.sw2;
  response_ptr->env_resp_data.env_resp_data_len = (uint8)env_rsp_ptr->cmd.raw_envelope_rsp.length;

  /* Data */
  if (env_rsp_ptr->cmd.raw_envelope_rsp.length > CATI_TLV_RAW_ENV_DATA_MAX)
  {
    UIM_MSG_HIGH_0("qmi_cat_response_envelope CATI_TLV_RAW_ENV_DATA_MAX");
    return QMI_ERR_INTERNAL_V01;
  }

  if (env_rsp_ptr->cmd.raw_envelope_rsp.length > 0 &&
      env_rsp_ptr->cmd.raw_envelope_rsp.rsp_data_ptr != NULL)
  {
    response_ptr->env_resp_data_valid = TRUE;
    UIM_MSG_HIGH_1(" length 0x%x", env_rsp_ptr->cmd.raw_envelope_rsp.length);
    (void)memscpy((void*)response_ptr->env_resp_data.env_resp_data,
                  sizeof(response_ptr->env_resp_data.env_resp_data_len),
                  env_rsp_ptr->cmd.raw_envelope_rsp.rsp_data_ptr,
                  env_rsp_ptr->cmd.raw_envelope_rsp.length);
  }
  //send envelope response
  return QMI_ERR_NONE_V01;
} /* qmi_cat_response_raw_envelope */


/*===========================================================================
  FUNCTION  QMI_CAT_IND_RAW_ENVELOPE()

  DESCRIPTION
    Process the raw evnvelope response

  PARAMETERS
   gstk_cmd_from_card_type            : Pointer to command from card
   cat_send_envelope_cmd_ind_msg_v02* : Pointer to send envelope indication msg

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_cat_ind_raw_envelope
(
  const gstk_cmd_from_card_type  * env_rsp_ptr,
  cat_send_envelope_cmd_ind_msg_v02                 * response_ptr
)
{
  ASSERT(env_rsp_ptr != NULL);
  
  response_ptr->env_resp_data.sw1= env_rsp_ptr->cmd.raw_envelope_rsp.sw1;
  response_ptr->env_resp_data.sw2 = env_rsp_ptr->cmd.raw_envelope_rsp.sw2;
  response_ptr->env_resp_data.env_resp_data_len = (uint8)env_rsp_ptr->cmd.raw_envelope_rsp.length;

  /* Data */
  if (env_rsp_ptr->cmd.raw_envelope_rsp.length > CATI_TLV_RAW_ENV_DATA_MAX)
  {
    UIM_MSG_HIGH_0("qmi_cat_response_envelope CATI_TLV_RAW_ENV_DATA_MAX");
    return QMI_ERR_INTERNAL_V01;
  }

  if (env_rsp_ptr->cmd.raw_envelope_rsp.length > 0 &&
      env_rsp_ptr->cmd.raw_envelope_rsp.rsp_data_ptr != NULL)
  {
    response_ptr->env_resp_data_valid = TRUE;
    UIM_MSG_HIGH_1(" length 0x%x", env_rsp_ptr->cmd.raw_envelope_rsp.length);
    (void)memscpy((void*)response_ptr->env_resp_data.env_resp_data,
                  sizeof(response_ptr->env_resp_data.env_resp_data),
                  env_rsp_ptr->cmd.raw_envelope_rsp.rsp_data_ptr,
                  env_rsp_ptr->cmd.raw_envelope_rsp.length);
  }
  //send envelope response
  return QMI_ERR_NONE_V01;
} /* qmi_cat_response_raw_envelope */

/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_GET_TERMINAL_PROFILE()

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
qmi_error_type_v01 qmi_cat_parse_request_get_terminal_profile
(
  cat_get_terminal_profile_req_msg_v02   * req_ptr,
  qmi_cat_in_get_terminal_profile_type   * parsed_data_ptr
)
{
  qmi_error_type_v01  result = QMI_ERR_NONE_V01;

  ASSERT(req_ptr);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0x00, sizeof(qmi_cat_in_get_terminal_profile_type));
  
  /* Check on slot */
  if (req_ptr->slot_valid     &&
      req_ptr->slot.slot != 0x1 &&
      req_ptr->slot.slot != 0x2)
  {
    result = QMI_ERR_INVALID_ARG_V01;
  }

  /* Slot is optional parameter here */
  if (req_ptr->slot_valid)
  {
    parsed_data_ptr->slot_present = TRUE;
    parsed_data_ptr->slot.slot    = (uint8) req_ptr->slot.slot;
  }

  return result;
} /* qmi_cat_parse_request_get_terminal_profile */


/*===========================================================================
  FUNCTION  QMI_CAT_IND_SEND_TR()

  DESCRIPTION
    Process the TR response

  PARAMETERS
   cat_send_tr_resp_msg_v02*      : Pointer to send TR response msg
   gstk_term_rsp_sw1_sw2_rpt_type : Pointer to status words from card

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
)
{
  qmi_error_type_v01                 errval = QMI_ERR_NONE_V01;
  ASSERT(response_ptr && msg_ptr);
  
  response_ptr->tr_response_data.sw1      = msg_ptr->sw1;
  response_ptr->tr_response_data.sw1      = msg_ptr->sw1;
  response_ptr->tr_response_data_valid    = TRUE;
  
  return errval;
} /* qmi_cat_response_send_tr */


/*===========================================================================
  FUNCTION  QMI_CAT_RESPONSE_SEND_TR()

  DESCRIPTION
    Process the TR response

  PARAMETERS
   cat_send_tr_ind_msg_v02*       : Pointer to send TR indication msg
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
)
{
  qmi_error_type_v01                 errval = QMI_ERR_NONE_V01;
  ASSERT(response_ptr && msg_ptr);

  response_ptr->tr_response_data.sw1      = msg_ptr->sw1;
  response_ptr->tr_response_data.sw1      = msg_ptr->sw1;
  response_ptr->tr_response_data_valid    = TRUE;
  
  return errval;
} /* qmi_cat_ind_send_tr */


/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_CACHED_PROACTIVE_CMD()

  DESCRIPTION
    Populates the tlvs from the raw proactive command

  PARAMETERS
    command_id      : tag to be used to pack response
    response_ptr    : output sdu
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
)
{
  switch (command_id)
  {
    case CATI_CACHED_CMD_TYPE_SETUP_MENU:
      response_ptr->setup_menu_valid = TRUE;
      response_ptr->setup_menu.uim_ref_id = cmd_ref_id;
      response_ptr->setup_menu.pc_setup_menu_len = raw_data_len;
      (void)memscpy(response_ptr->setup_menu.pc_setup_menu,
                sizeof(response_ptr->setup_menu.pc_setup_menu),
                raw_data_ptr,
                raw_data_len);
      return QMI_ERR_NONE_V01;
      break;
    case CATI_CACHED_CMD_TYPE_SETUP_EVT_LIST:
      response_ptr->setup_event_list_raw_valid = TRUE;
      response_ptr->setup_event_list_raw.uim_ref_id = cmd_ref_id;
      response_ptr->setup_event_list_raw.pc_setup_event_list_len = raw_data_len;
      (void)memscpy(response_ptr->setup_event_list_raw.pc_setup_event_list,
                  sizeof(response_ptr->setup_event_list_raw.pc_setup_event_list),
                  raw_data_ptr,
                  raw_data_len);
      return QMI_ERR_NONE_V01;
      break;
    case CATI_CACHED_CMD_TYPE_SETUP_IDLE_MODE_TEXT:
      response_ptr->idle_mode_text_valid = TRUE;
      response_ptr->idle_mode_text.uim_ref_id = cmd_ref_id;
      response_ptr->idle_mode_text.pc_setup_idle_mode_text_len = raw_data_len;
      (void)memscpy(response_ptr->idle_mode_text.pc_setup_idle_mode_text,
                    sizeof(response_ptr->idle_mode_text.pc_setup_idle_mode_text),
                    raw_data_ptr,
                    raw_data_len);
      return QMI_ERR_NONE_V01;
      break;
    default:
      return QMI_ERR_INVALID_ARG_V01;
      break;
  }
} /* qmi_cat_response_cached_proactive_cmd */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
