/*==========================================================================

                         Q M I _ C A T . C

DESCRIPTION

 The Data Services Qualcomm MSM Interface Card Application Toolkit Service source
 file.

                        COPYRIGHT INFORMATION

Copyright (c) 2005 - 2016, 2018 - 2020 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_cat.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/13/20    ykr    Fix compilation warnings.
12/13/19    dt     Changing mask size in event indication report
10/23/19    dt     Migration of QMICAT to QCSI/QCCI
11/15/18    tq     Updating the UIM_COMMON_EFS path to use a common value
10/29/18    me     Deprecated MMGSDI_TERMINAL_PROFILE_DL_EVT and MMGSDI_CARD_REMOVED_EVT 
10/12/18    ks     Split ME and slot specific NV
11/21/16    dt     QMICAT registration with GSTK during task start-up
12/05/16    bcho   Remove compilation error
10/10/16    shr    Enable Data Card Config. if QMI CAT is not supported
10/11/16    shr    Add support for Poll Interval Envelope
09/22/16    vr     Cache the GET_INPUT command
09/15/16    gs     Changes in TP based on RAT and App Type
05/26/16    tkl    Remove reference to uim_get_service_object_v01
05/25/16    vdc    Remove F3 messages for memory allocation failure
03/08/16    vr     Common function for result tlv and send response
03/08/16    vr     Remove duplicated code between QMI CAT and QMI UIM
03/07/16    lm     Remove unneeded F3's
12/09/15    vdc    Fix card state update when process TP DL indication
11/25/15    sp     Updated copyright
11/23/15    vdc    Multi-profile switch handling
10/14/15    vr     Fix for STK not displayed with security activated
09/07/15    bcho   Migrate to mcfg EFS wrappers
08/27/15    vdc    Do not retry service registration if it fails once
04/14/15    vr     Fix custom TP update failure
02/18/15    vr     qmi_cati_read_configuration in pre_task_startup
02/13/15    hh     Split qmi_cat_init into pre and post startup
01/09/15    hh     Set correct message type for SCWS_OPEN_CH_REQ
11/14/14    hh     Fix compilation warnings
10/30/14    gm     Move enable/disable of raw unsupported features to NV
10/30/14    hh     Support for GSTK_CFG_FEATURE_ME_PLI_DATE_TIME_APPS removal 
10/22/14    am     Fix compile warning 
10/08/14    tkl    Fix KW error
10/06/14    hh     Set full_func_evt[] for new UICC proactive command only 
09/30/14    gm     Remove GOBI mode code
09/29/14    av     Featurize all QMI messages
09/16/14    hh     Correct message type for raw proactive command
09/15/14    tkl    Clear BIP status when End session with error during BIP session
09/11/14    hh     Call rex_undef_timer() when timer is no longer needed 
09/09/14    bd     Moved an enum to header file and clean up
08/29/14    hh     Fix compliation warnings
08/26/14    tkl    Fixed handling of PLI Lang cmd in check_last_gstk_cmd
08/26/14    hh     Fix array boundary checking
08/25/14    hh     Correctly handle Setup Evt List proactive command
08/22/14    hh     Moved qmi_cati_custom_profile_value_list to heap
08/05/14    hh     Added support to retrieve supported proactive commands
07/02/14    shr    Add Support for NV Refresh
07/02/14    hh     Removed check of cmd_ref_id against CATI_GSTK_PROACTIVE_COMMANDS_MAX
06/18/14    tkl    Update to use BIP channel ID in CLOSE CH CNF instead of REQ
06/17/14    hh     Clear and/or Overwrite cached proactive command with new one
06/02/14    vr     Off target support for qmi cat NV
05/15/14    tkl    Add duration TP bitmask for Decoded format
03/27/14    gs     Removed the GSTK featurization
04/29/14    dy     Added ext browser HTML support bit to TP
04/16/16    vr     Discard invalid TR or event confirmation
04/09/14    yt     Make RAT TLV mandatory for Call Control envelope
04/07/14    gm     Added support for call type as IMS
04/01/14    av     Rename QMI_CAT_FREE_IF_NULL to QMI_CAT_FREE_IF_NOT_NULL
03/27/14    at     Usage of common QMI APIs from DS header
03/19/14    tkl    Added refresh alpha TP for android profile
03/17/14    gm     Added support for IMS communication control
03/12/14    gm     Moved qmi_cat_state and qmi_cati_global to heap
03/06/14    tkl    Update address/sub-address as optional TLVs for MT call env
03/06/14    gm     Moved to new API for client alloc/dealloc result
02/24/14    av     Do not rely on CATI_CMD_MAX when traversing through cmd cb
02/11/14    tkl    Free callback user data after sending response
01/28/14    df     Use q_cnt() instead directly accessing cnt
01/10/14    kb     Add de-init function when gstk task is stopped
01/10/14    df     Use v2.0 diag macros
09/27/13    sw     Corrected the BIP channel Ids
12/02/13    tkl    Use global slot count for looping instead of max card count
10/21/13    gm     Extended ref_command_id for multi card support
10/09/12    tkl    Fixed handling of slot 3 in parsing slot tlv
10/08/13    gm     Removed unused comments
10/04/13    gm     F3 message reduction
09/10/13    tkl    Removed duplicate free of cc_sms_ind_ptr in decoded env
09/04/13    tkl    Moved to new MO SMS envelope API with proper LS filler.
07/27/13    tkl    Fixed merge error
07/26/13    tkl    Add NV support for enabling use of SMS-PP envelope command
07/22/13    gs     Added mapping of GSTK_UNSUPPORTED_COMMAND to QMI error code
07/02/13    yt     Dynamically determine number of slots from UIMDRV
06/12/13    kb     Added QMI malloc and free wrapper functions
05/20/13    tkl    Fix to ensure Close Ch REQ is processed before BIP CNF checks
05/17/13    tkl    Added support for Refresh Alpha
05/15/13    gm     Moved memcpy() and memmove() to memscpy() and memsmove()
05/06/13    tkl    Set location info data for CC envelope with filler
05/03/13    tkl    Return appropriate error code when TR timer expires
04/30/13    tkl    Added support for 3rd party IMS client in raw format
04/23/13    tkl    Fixed sending of BIP status with correct event report bitmask
04/11/13    tkl    Add NV suport for SETUP Call display alpha value
04/02/13    av     Move UIM tasks to group4 of RCINIT
03/18/13    at     Fix for NULL pointer dereference of cmd_buf_p
02/28/13    tkl    Added support for 3rd party IMS client
02/22/13    yt     Support for slot 3
02/19/13    av     Merge mmgsi,gstk,qmiuim/cat conf files to mmgsdi.conf
02/14/13    tl     Move QMI CAT to GSTK context from QMI modem context
01/28/13    tkl    Added BIP status events
11/15/12    xz     Move SCWS TP bit setting into ESTK
11/09/12    yt     Move QMI CAT to UIM heap
10/21/12    av     Update conf file only if necessary
10/03/12    tkl    Added support for Browswer Termination event indication
10/03/12    tkl    Added raw hdr tag to handle GSTK_TAG_SETUP_CALL_DISPLAY_ALPHA
10/02/12    tkl    Added SMS-PP download in envelope command
09/27/12    tkl    Added Browser Status evt bitmask for TP
09/20/12    at     Fix for sending TR when language code NV is set
09/18/12    tkl    Added optional Alpha TLV for SCWS open channel ind
09/09/12    at     Removal of private DS functions and headers
09/07/12    tkl    Fix compliation error when BIP server mode is enabled
09/07/12    tl     Added support for SGLTE dual slots
07/20/12    tkl    Added display alpha support for Custom TP raw format
07/16/12    at     Read NV to send language code in default TR
07/16/12    at     Fix to clean the client queues before cl_id is deallocated
05/23/12    hn     Enable 1-byte alignment packing on Windows environment
05/10/12    nmb    Correct dangling pointer when deallocating clients
03/26/12    at     Extra checks when parsing custom tp values from NV
03/21/12    at     Added browser termination event in decoded envelope command
02/23/12    tkl    MPSS/QMI support interfaces logging
02/23/12    tkl    Added envelope cmd for Call Control & HCI Conn
02/23/12    nmb    QMI QMUX Transition
02/02/12    at     Updation of custom_tp NV with max bytes
02/02/12    nb     Featurized support for "User Activity" and "Language Settings"
                   in android TP
01/12/12    at     Fixed compilation errors
01/10/12    tkl    Added support for set/get configuration
01/10/12    tkl    Added custom TP support
01/05/12    vv     Removed unneeded ASSERT statements
01/03/12    nb     Addded support to send status words for TR
01/03/12    nb     Addded support to retrieve terminal profile
12/22/11    tkl    Fixed crash in SCWS data available with fuzz string
12/21/11    nb     GSDI Cleanup
12/20/11    hn     Add support for off-target testing
12/13/11    mib    Fixed compilation error
12/06/11    tkl    Removed TP bits & evt registration for network related cmd
12/05/11    vv     Send error response for OPEN CHANNEL when cache is busy
11/17/11    tkl    Caching PLI language and PLI time cmd
11/16/11    tkl    Added APDU support for raw envelope
11/08/11    tkl    Added support for Class M and L
11/03/11    tkl    Sent event conf when indication is not sent & not cached
10/25/11    tkl    Added Get InKey Yes/No TLV for decoded TR
10/24/11    tkl    Add duration TP bitmask for Android format
10/20/11    tkl    Fixed KW error
10/11/11    tkl    Disabled Language Notification in TP bitmask for Android
09/26/11    nmb    Maintain command buffers internally
07/11/11    tkl    Disabled User Activity in Terminal Profile for Android
07/07/11    tkl    Fixed uim_ref_id in the QMI CAT cache
06/07/11    yt     Added NV item for CAT VS service
06/06/11    tkl    Added 7 bit DCS encoding for text string in decoded TR
06/05/11    vs     Decoded call control envelope
06/03/11    yt     Added slot id to QMI_CAT_SET_EVENT_REPORT
05/31/11    tkl    Updated read configuration for QCN restore
05/24/11    tkl    Added support for CATI_CONFIG_MODE_DISABLED mode
05/12/11    tkl    Updated SCWS Data Avail reqest as asynchronous
05/10/11    tkl    Fixed terminal_response_cnf for GSTK_PROVIDE_LANG_INFO_REQ
04/20/11    tkl    Added SCWS support
04/14/11    tkl    Updated icon conf value for Setup call display request
03/22/11    tkl    Fixed covert raw mask to TLV id for Send USSD
03/16/11    tkl    Added caching for Display Text cmd
03/12/11    tkl    Fixed compliation errors
03/08/11    tkl    Added support for PLI-Time proactive cmd
02/20/11    tkl    Added BIP support in decoded format
02/07/11    tkl    Fixed qmux register service for vendor specific ID
01/24/11    tkl    Added support for slot id
01/14/11    mib    Review of parsing routines
01/19/10    tkl    Mainlined service anywhere feature
12/21/10    tkl    Added support for End proactive session in PULL only mode
11/30/10    yt     Including private GSTK header
11/05/10    tkl    Fixed incorrect data type in proactive_session_end_type
11/03/10    tkl    Fixed raw_data_len to support size of CATI_TLV_PC_LEN_MAX
10/06/10    bd     Fixed wrong handling of NW commands when no client exists
10/06/10    tkl    Fixed incorrect error code returned from qmi event confirmation
10/01/10    tkl    Fixed soft key max number not set in terminal profile
09/24/10    mib    Fixed compiler warnings
09/24/10    tkl    Added support for BIP related proactive command.
09/21/10    mib    Added .conf file for QMI CAT NV items
09/20/10    tkl    Added support for setup evt list browser termination in
                   raw format
09/10/10    tkl    Fixed send TR when Ind is not sent and event is not cached.
09/09/10    tkl    Fixed len compare in qmi_cati_get_event_report
26/08/10    bd     Fixed wrong terminal response when client is not registered
08/27/10    dd     Fix Fusion TP
08/26/10    dd     Fixed default TP
08/20/10    bd     Fixed CATI_TLV_PC_LEN_MAX value
08/05/10    tkl    QMI UIM & QMI CAT split from data package
08/02/10    js     Modified CLASS0 cmd mask not to include
                   GSTK_REG_SETUP_EVENT_LIST_EVT in GSTK registration
07/28/10    tkl    Fixed deep copy of icon_list for Setup Menu & Select item
07/26/10    tkl    Allowed use of client id only in Gobi mode.
07/21/10    tkl    Added support to use tag for type of setup call cmd.
07/15/10    tkl    Added support for Setup evt list cmd in raw format
07/14/10    tkl    Added support for playtone & network cmd in raw format
                   & fix typo for config_mode.
07/08/10    tkl    Added support for Setup call user alpha & icon conf
07/08/10    mib    Fixed size of config file
06/25/10    mib    Added support for unknown envelope command
06/22/10    tkl    Added support for LANGUAGE INFO tlv for PLI-Language
06/16/10    tkl    Added support for user confirmation
06/14/10    mib    Fixed duration for GET INKEY in decoded TR
03/25/10    dd     Adding decoded tlvs
11/01/09    mj     Initial check-in of QMI_CAT
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stdio.h>
#include <stringl/stringl.h>

#include "nv.h"
#include "uim_msg.h"
#include "err.h"
#include "amssassert.h"
#include "fs_lib.h"
#include "fs_public.h"

#include "qmi_framework.h"
#include "qmi_svc_utils.h"

#include "gstk_exp.h"
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"

/* needed for int conversion functions */
#include "intconv.h"

#include "qmi_cat.h"
#include "qmi_cat_decoded.h"
#include "qmi_uim_cat_common.h"
#include "ds_qmi_fw_common.h"
#include "qmi_uim_util.h"

#include "modem_mem.h"

/* Temporarily include gstk_p.h for Setup Call user & icon confirmation */
#include "gstk_nv_p.h"
#include "gstk_exp_v.h"

#include "qmi_si.h"
#include "qmi_idl_lib.h"
#include "card_application_toolkit_v02.h"
#include "card_application_toolkit_impl_v02.h"

#include "uim_p.h"
#include "uim_common_efs.h"
#include "ipc_policy.h"
#include "qmi_csi.h"

/* These includes need to always be at bottom - offtarget use only */
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Service management
---------------------------------------------------------------------------*/
#define CATI_RETRY_INTERVAL           (75)

/*---------------------------------------------------------------------------
  Major and Minor Version Nos for CAT
---------------------------------------------------------------------------*/
#define CATI_BASE_VER_MAJOR          (2)
#define CATI_BASE_VER_MINOR          (24)
#define CATI_ADDENDUM_VER_MAJOR      (0)
#define CATI_ADDENDUM_VER_MINOR      (0)

/*---------------------------------------------------------------------------
  Number of cards
---------------------------------------------------------------------------*/
#define QMI_CAT_MAX_CARD_COUNT                    (3)

/*---------------------------------------------------------------------------
  Max number of SCWS Channel
---------------------------------------------------------------------------*/
#define QMI_CATI_MAX_SCWS_CHANNEL                 (7)

/*---------------------------------------------------------------------------
  Max value of the number of TP DL type sent to GSTK
---------------------------------------------------------------------------*/
#define QMI_CAT_MODE_CUSTOM_TP_DL_MAX_VALUE      (GSTK_MAX_TP_DATA_SIZE / 4)

/*---------------------------------------------------------------------------
  Configuration file
---------------------------------------------------------------------------*/
#define CATI_CONFIG_DIR1             "/nv/item_files/modem"
#define CATI_CONFIG_DIR2             "/nv/item_files/modem/qmi"
#define CATI_CONFIG_DIR3             "/nv/item_files/modem/qmi/cat"

/*---------------------------------------------------------------------------
  Event report TLV definitions
---------------------------------------------------------------------------*/
#define CATI_PRM_TYPE_DISPLAY_TEXT                  (0x10)
#define CATI_PRM_TYPE_GET_INKEY                     (0x11)
#define CATI_PRM_TYPE_GET_INPUT                     (0x12)
#define CATI_PRM_TYPE_SETUP_MENU                    (0x13)
#define CATI_PRM_TYPE_SELECT_ITEM                   (0x14)
#define CATI_PRM_TYPE_SETUP_IDLE_MODE_TEXT          (0x17)
#define CATI_PRM_TYPE_LANG_NOTIFICATION             (0x18)
#define CATI_PRM_TYPE_PLAY_TONE_EVENT               (0x47)
#define CATI_PRM_TYPE_SETUP_CALL_EVENT              (0x48)
#define CATI_PRM_TYPE_SEND_DTMF_EVENT               (0x49)
#define CATI_PRM_TYPE_LAUNCH_BROWSER_EVENT          (0x4A)
#define CATI_PRM_TYPE_SEND_SMS_EVENT                (0x4B)
#define CATI_PRM_TYPE_SEND_SS_EVENT                 (0x4C)
#define CATI_PRM_TYPE_SEND_USSD_EVENT               (0x4D)
#define CATI_PRM_TYPE_PROVIDE_LOCAL_INFO            (0x4E)
#define CATI_PRM_TYPE_SETUP_EVENT_LIST_RAW          (0x4F)
#define CATI_PRM_TYPE_SLOT                          (0x50)
#define CATI_PRM_TYPE_OPEN_CHANNEL                  (0x51)
#define CATI_PRM_TYPE_CLOSE_CHANNEL                 (0x52)
#define CATI_PRM_TYPE_SEND_DATA                     (0x53)
#define CATI_PRM_TYPE_RECEIVE_DATA                  (0x54)
#define CATI_PRM_TYPE_ACTIVATE                      (0x66)
#define CATI_PRM_TYPE_REFRESH_ALPHA                 (0x6A)

/*---------------------------------------------------------------------------
  Tags used for cached proactive commands in raw format
---------------------------------------------------------------------------*/
#define CATI_PRM_TYPE_CACHED_SETUP_MENU             (0x10)
#define CATI_PRM_TYPE_CACHED_SETUP_EVT_LIST         (0x11)
#define CATI_PRM_TYPE_CACHED_SETUP_IDLE_MODE_TEXT   (0x12)

/*---------------------------------------------------------------------------
  CAT Proactive Cmd length
---------------------------------------------------------------------------*/
#define CATI_CMD_DETAILS_LEN    (5)

/*---------------------------------------------------------------------------
  Envelope content tag
---------------------------------------------------------------------------*/
#define CATI_SMS_PP_DOWNLOAD_TAG     (0xD1)

/*---------------------------------------------------------------------------
  Macro used in command handlers (common)
---------------------------------------------------------------------------*/
#define CHECK_RETVAL()  if (FALSE == retval) { dsm_free_packet(&response); \
                                               return NULL; }

/*---------------------------------------------------------------------------
  Macro used to free pointers
---------------------------------------------------------------------------*/
#define QMI_CAT_FREE_IF_NOT_NULL(ptr)                       \
      if (ptr)                                              \
      {                                                     \
        uimqmi_free(ptr);                                   \
        ptr = NULL;                                         \
      }

/*---------------------------------------------------------------------------
  Values used for cache (raw)
---------------------------------------------------------------------------*/
#define CATI_CACHED_EVT_SETUP_MENU                          (0)
#define CATI_CACHED_EVT_SETUP_IDLE_MODE_TEXT                (1)
#define CATI_CACHED_EVT_SETUP_EVT_LIST                      (2)
#define CATI_CACHED_EVT_DISPLAY_TEXT                        (3)
#define CATI_CACHED_EVT_PROVIDE_LOCAL_INFO                  (4)
#define CATI_CACHED_EVT_GET_INPUT                           (5)
#define CATI_MAX_RAW_CACHED_EVTS                            (6)

/*---------------------------------------------------------------------------
  Values used for recovery cache (raw)
---------------------------------------------------------------------------*/
#define CATI_QMI_CLIENT_CACHED_EVT_SETUP_MENU                (0)
#define CATI_QMI_CLIENT_CACHED_EVT_SETUP_IDLE_MODE_TEXT      (1)
#define CATI_QMI_CLIENT_CACHED_EVT_SETUP_EVT_LIST            (2)
#define CATI_MAX_RAW_QMI_CLIENT_CACHED_EVTS                  (3)

/*---------------------------------------------------------------------------
  Values used for DCS conversions
---------------------------------------------------------------------------*/
#define QMI_CATI_SMS_7_BIT_PACKED_GENERAL_DATA_CODING_DCS (0x00)
#define QMI_CATI_SMS_8_BIT_GENERAL_DATA_CODING_DCS        (0x04)
#define QMI_CATI_SMS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS   (0x08)

/*---------------------------------------------------------------------------
  Values used for Access Technology
---------------------------------------------------------------------------*/
#define QMI_CATI_ACCESS_NONE            (0x00)
#define QMI_CATI_ACCESS_TECH_GSM        (0x01)
#define QMI_CATI_ACCESS_TECH_UTRAN      (0x02)
#define QMI_CATI_ACCESS_TECH_CDMA       (0x03)
#define QMI_CATI_ACCESS_TECH_LTE        (0x04)
#define QMI_CATI_ACCESS_TECH_WLAN       (0x05)

/*===========================================================================

                                DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  Set Event Report Full Function registration event bitmask
---------------------------------------------------------------------------*/
#define CATI_REPORT_STATUS_EV_FULL_FUNC_NONE                   (0x000000)
#define CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SMS               (0x000001)
#define CATI_REPORT_STATUS_EV_FULL_FUNC_SETUP_CALL             (0x000002)
#define CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_DTMF              (0x000004)
#define CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS                (0x000008)
#define CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_USSD              (0x000010)

/*---------------------------------------------------------------------------
  Event list indication
---------------------------------------------------------------------------*/
#define CATI_SETUP_EVT_LIST_MASK_USER_ACT                       0x01
#define CATI_SETUP_EVT_LIST_MASK_SCR_NOTIFY                     0x02
#define CATI_SETUP_EVT_LIST_MASK_LANGSEL_NOTIFY                 0x04
#define CATI_SETUP_EVT_LIST_MASK_HCI_CONNECITVITY               0x08

/*---------------------------------------------------------------------------
  Raw Envelope Cmd type
---------------------------------------------------------------------------*/
#define CATI_ENV_CMD_TYPE_MENU_SEL                              0x01
#define CATI_ENV_CMD_TYPE_EVT_DL_USER_ACT                       0x02
#define CATI_ENV_CMD_TYPE_EVT_DL_IDL_SCREEN_AVAIL               0x03
#define CATI_ENV_CMD_TYPE_EVT_DL_LANG_SEL                       0x04
#define CATI_ENV_CMD_TYPE_UNKNOWN                               0x05
#define CATI_ENV_CMD_TYPE_EVT_DL_BROWSER_TERM                   0x06
#define CATI_ENV_CMD_TYPE_SEND_CALL_CONTROL                     0x07
#define CATI_ENV_CMD_TYPE_EVT_DL_HCI_CONN                       0x08
#define CATI_ENV_CMD_TYPE_SMS_PP_DOWNLOAD                       0x09
#define CATI_ENV_CMD_TYPE_EVT_DL_MT_CALL                        0x0A
#define CATI_ENV_CMD_TYPE_EVT_DL_CALL_CONNECTED                 0x0B
#define CATI_ENV_CMD_TYPE_EVT_DL_CALL_DISCONNECTED              0x0C
#define CATI_ENV_CMD_TYPE_EVT_DL_POLL_INTERVAL                  0x0E

/*---------------------------------------------------------------------------
  Type of QMI CAT messages posted to DCC task
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_CAT_MESSAGE_RAW_PROACTIVE_COMMAND,
  QMI_CAT_MESSAGE_ENVELOPE_RESPONSE,
  QMI_CAT_MESSAGE_END_PROACTIVE_SESSION,
  QMI_CAT_MESSAGE_TERM_RSP_SW1_SW2,
  QMI_CAT_MESSAGE_MMGSDI_CNF,
  QMI_CAT_MESSAGE_MMGSDI_EVT,
  QMI_CAT_MESSAGE_RECOVERY_COMPLETE,
  QMI_CAT_MESSAGE_TP_DL_IND
} qmi_cat_message_enum_type;

/*---------------------------------------------------------------------------
  Structure for QMI CAT messages
---------------------------------------------------------------------------*/
typedef struct
{
  /* This needs to be in first position */
  q_link_type                               link_next;

  qmi_cat_message_enum_type                 message_type;

  union
  {
    struct
    {
      gstk_slot_id_enum_type                slot_id;
      uint32                                cmd_ref_id;
      uint8                                 raw_hdr_tag;
      int32                                 raw_data_len;
      uint8                                 raw_data_buffer[ CAT_RAW_MAX_DATA_LEN ];
      boolean                               is_cached_cmd;
    }                                       raw_proactive_cmd;
    struct
    {
      gstk_cmd_from_card_type               decoded_cmd;
      boolean                               is_cached_cmd;
    }                                       decoded_proactive_cmd;
    struct
    {
      qmi_cat_decoded_bip_status_type       bip_status;
    }                                       decoded_bip_status;
    struct
    {
      gstk_cmd_from_card_type               scws_cmd;
    }                                       scws_cmd;
    struct
    {
      gstk_cmd_from_card_type               envelope_rsp;
      void *                                user_data;
    }                                       envelope_rsp;
    struct
    {
      gstk_cmd_from_card_type               envelope_rsp;
    }                                       decoded_envelope_rsp;
    struct
    {
      gstk_slot_id_enum_type                slot_id;
      uint8                                 proactive_session_end_type;
      gstk_tag_enum_type                    tag;
    }                                       end_proactive_cmd;
    struct
    {
      gstk_cmd_from_card_type               sw1_sw2_data;
    }                                       terminal_response_sw1_sw2_info;
    struct
    {
      qmi_framework_common_msg_hdr_type     cmd_hdr;
    }                                       clid_cb;
    struct
    {
      uimqmi_framework_cmd_type             cmd;
    }                                       framework_cmd;
    struct
    {
      mmgsdi_cnf_enum_type                  cnf_type;
      mmgsdi_cnf_type                       cnf_value;
    }                                       mmgsdi_cnf;
    struct
    {
      mmgsdi_events_enum_type               evt;
      mmgsdi_slot_id_enum_type              slot;
    }                                       mmgsdi_evt;
    struct
    {
      mmgsdi_slot_id_enum_type              slot;
    }                                       recovery_ind;
    struct
    {
      gstk_slot_id_enum_type                slot_id;
    }                                       tp_dl_ind;
  }                                         data;
} qmi_cat_message_type;

/*---------------------------------------------------------------------------
  QMI CAT dispaly alpha confirmation type
---------------------------------------------------------------------------*/
typedef enum
{
  CATI_DISPLAY_ALPHA_NO_AUTOMATIC_CONF                   = 0x00,
  CATI_DISPLAY_ALPHA_AUTOMATIC_FALSE                     = 0x01,
  CATI_DISPLAY_ALPHA_AUTOMATIC_TRUE                      = 0x02
} qmi_cati_display_alpha_conf_e_type;

/*---------------------------------------------------------------------------
  Send TR command type
---------------------------------------------------------------------------*/
#define CATI_EVENT_DISPLAY_TEXT_CNF                      0x01
#define CATI_EVENT_GET_INKEY_CNF                         0x02
#define CATI_EVENT_GET_INPUT_CNF                         0x03
#define CATI_EVENT_LAUNCH_BROWSER_CNF                    0x04
#define CATI_EVENT_PLAY_TONE_CNF                         0x05
#define CATI_EVENT_SELECT_ITEM_CNF                       0x06
#define CATI_EVENT_SETUP_MENU_CNF                        0x07
#define CATI_EVENT_SETUP_IDLE_TEXT_CNF                   0x08
#define CATI_EVENT_PROVIDE_LANGUAGE_INFO_CNF             0x09
#define CATI_EVENT_USER_ACTIVITY_CNF                     0x0A
#define CATI_EVENT_IDLE_SCREEN_CNF                       0x0B
#define CATI_EVENT_LANGUAGE_SEL_CNF                      0x0C
#define CATI_EVENT_LANGUAGE_NOTIFICATION_CNF             0x0D
#define CATI_EVENT_ACTIVATE_CNF                          0x0E
#define CATI_EVENT_HCI_CONNECTIVITY_CNF                  0x0F
#define CATI_EVENT_BROWSER_TERM_CNF                      0x10
#define CATI_EVENT_SEND_SMS_CNF                          0x11
#define CATI_EVENT_SETUP_CALL_CNF                        0x12
#define CATI_EVENT_SEND_DTMF_CNF                         0x13
#define CATI_EVENT_SEND_SS_CNF                           0x14
#define CATI_EVENT_SEND_USSD_CNF                         0x15

/*---------------------------------------------------------------------------
  QMI CAT card states
---------------------------------------------------------------------------*/
typedef enum
{
  /* when card is not present in the device or any card error */
  QMI_CAT_CARD_STATE_ERROR,
  /* card state between tp_dl and card insert event */
  QMI_CAT_CARD_STATE_INIT_IN_PROGRESS,
  /* card state after card insert event */
  QMI_CAT_CARD_STATE_PRESENT,
  /* card state during profile switch */
  QMI_CAT_CARD_STATE_INVALID
} qmi_cati_card_state_e_type;

#define QMI_CAT_GSTK_EVT_MASK_CLASSC (GSTK_REG_LAUNCH_BROWSER_EVT                 |     \
                                      GSTK_REG_SETUP_EVT_BROWSER_TERM_EVT)

#define QMI_CAT_GSTK_EVT_MASK_CLASS0_FULL (GSTK_REG_DISPLAY_TEXT_EVT              |     \
                                           GSTK_REG_GET_INKEY_EVT                 |     \
                                           GSTK_REG_GET_INPUT_EVT                 |     \
                                           GSTK_REG_PLAY_TONE_EVT                 |     \
                                           GSTK_REG_SELECT_ITEM_EVT               |     \
                                           GSTK_REG_SETUP_MENU_EVT                |     \
                                           GSTK_REG_SETUP_IDLE_TEXT_EVT           |     \
                                           GSTK_REG_PROVIDE_LANG_INFO_EVT         |     \
                                           GSTK_REG_LANG_NOTIFY_EVT               |     \
                                           GSTK_REG_SETUP_EVT_USER_ACT_EVT        |     \
                                           GSTK_REG_SETUP_EVT_IDLE_MENU_EVT       |     \
                                           GSTK_REG_SETUP_EVT_LANG_SEL_EVT        |     \
                                           GSTK_REG_EVT_DOWNLOAD_RSP_EVT          |     \
                                           GSTK_REG_MENU_SEL_RSP_EVT)

#define QMI_CAT_GSTK_EVT_MASK_CLASS0_FULL_DATE_TIME (GSTK_REG_PROVIDE_DATE_TIME_EVT)

#define QMI_CAT_GSTK_EVT_MASK_CLASS0_DISPLAY_REFRESH (GSTK_REG_REFRESH_EVT)

#define QMI_CAT_GSTK_EVT_MASK_CLASS0_DISPLAY_WMS (GSTK_REG_SEND_SMS_EVT)

#define QMI_CAT_GSTK_EVT_MASK_CLASS0_DISPLAY_CM_SUPPS ( GSTK_REG_SEND_SS_EVT               |     \
                                                        GSTK_REG_SEND_USSD_EVT)

#define QMI_CAT_GSTK_EVT_MASK_CLASS0_DISPLAY_CM_VOICE_CALL ( GSTK_REG_SETUP_CALL_EVT             |     \
                                                             GSTK_REG_SEND_DTMF_EVT)

#define QMI_CAT_GSTK_EVT_MASK_CLASSE_DISPLAY (GSTK_REG_DATA_EVT)

#define QMI_CAT_GSTK_EVT_MASK_SCWS_DISPLAY (GSTK_REG_SCWS_EVT)

#define QMI_CAT_GSTK_EVT_MASK_CLASSL_DISPLAY (GSTK_REG_ACTIVATE_EVT)

#define QMI_CAT_GSTK_EVT_MASK_CLASSM_DISPLAY (GSTK_REG_SETUP_EVT_HCI_CONN_EVT)

static gstk_toolkit_cmd_reg_info_type  qmi_cati_complete_reg_list[] =
{
  { { GSTK_REG_CATEGORY_C_CMD, QMI_CAT_GSTK_EVT_MASK_CLASSC}, GSTK_HANDLE_ALL_FUNC},
  { { GSTK_REG_CATEGORY_0_CMD, QMI_CAT_GSTK_EVT_MASK_CLASS0_FULL}, GSTK_HANDLE_ALL_FUNC},
  { { GSTK_REG_CATEGORY_0_CMD, QMI_CAT_GSTK_EVT_MASK_CLASS0_DISPLAY_REFRESH}, GSTK_HANDLE_DISPLAY_FUNC_ONLY},
  { { GSTK_REG_CATEGORY_0_CMD, QMI_CAT_GSTK_EVT_MASK_CLASS0_DISPLAY_WMS}, GSTK_HANDLE_DISPLAY_FUNC_ONLY},
  { { GSTK_REG_CATEGORY_0_CMD, QMI_CAT_GSTK_EVT_MASK_CLASS0_DISPLAY_CM_SUPPS}, GSTK_HANDLE_DISPLAY_FUNC_ONLY},
  { { GSTK_REG_CATEGORY_0_CMD, QMI_CAT_GSTK_EVT_MASK_CLASS0_DISPLAY_CM_VOICE_CALL}, GSTK_HANDLE_DISPLAY_FUNC_ONLY},
  { { GSTK_REG_CATEGORY_E_CMD, QMI_CAT_GSTK_EVT_MASK_CLASSE_DISPLAY}, GSTK_HANDLE_DISPLAY_FUNC_ONLY},
  { { GSTK_REG_CATEGORY_M_CMD, QMI_CAT_GSTK_EVT_MASK_CLASSM_DISPLAY}, GSTK_HANDLE_ALL_FUNC},
  { { GSTK_REG_CATEGORY_L_CMD, QMI_CAT_GSTK_EVT_MASK_CLASSL_DISPLAY}, GSTK_HANDLE_ALL_FUNC}
};

static gstk_toolkit_cmd_reg_info_type  qmi_cati_scws_reg_list[] =
{
  { { GSTK_REG_CATEGORY_E_CMD, QMI_CAT_GSTK_EVT_MASK_SCWS_DISPLAY}, GSTK_HANDLE_DISPLAY_FUNC_ONLY}
};

static gstk_toolkit_cmd_reg_info_type  qmi_cati_date_time_reg_list[] =
{
  { { GSTK_REG_CATEGORY_0_CMD, QMI_CAT_GSTK_EVT_MASK_CLASS0_FULL_DATE_TIME}, GSTK_HANDLE_ALL_FUNC}
};

/* Empty TERMINAL PROFILE support from QMI-CAT */
static gstk_profile_dl_support_ext_type qmi_cati_empty_profile_value_list[1] = {{0}};

/* Custom raw/decoded format TERMINAL PROFILE support from QMI-CAT */
static struct
{
  gstk_profile_dl_support_ext_type  *value_list_ptr;
  uint8                              tp_count;
}qmi_cati_custom_profile = {NULL, 0};

/* Decoded format TERMINAL PROFILE support from QMI-CAT */
static gstk_profile_dl_support_ext_type qmi_cati_decoded_profile_value_list[] =
{
  {1, GSTK_MENU_SEL_SUPPORT,                     (uint8)GSTK_APP_NONE},
  {1, GSTK_SUPPORT_FOR_ALPHA_IN_CC,              (uint8)GSTK_APP_SIM},
  {1, GSTK_UCS2_ENTRY_SUPPORT,                   (uint8)GSTK_APP_NONE},
  {1, GSTK_UCS2_DISPLAY_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_DISPLAY_OF_EXTN_TEXT,                 (uint8)GSTK_APP_SIM},
  {1, GSTK_USAT_DISPLAY_SUPPORT_BYTE_2_BIT_8,    (uint8)GSTK_APP_USIM},
  {1, GSTK_DISPLAY_TEXT_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_GET_INKEY_SUPPORT,                    (uint8)GSTK_APP_NONE},
  {1, GSTK_GET_INPUT_SUPPORT,                    (uint8)GSTK_APP_NONE},
  {1, GSTK_SELECT_ITEM_SUPPORT,                  (uint8)GSTK_APP_NONE},
  {1, GSTK_PLAY_TONE_SUPPORT,                    (uint8)GSTK_APP_NONE},
  {1, GSTK_SET_UP_MENU_SUPPORT,                  (uint8)GSTK_APP_NONE},
  {1, GSTK_IDLE_SCRN_AVAIL_EVT_SUPPORT,          (uint8)GSTK_APP_NONE},
  {1, GSTK_LANG_SEL_EVT_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_SET_UP_IDLE_MODE_TEXT_SUPPORT,        (uint8)GSTK_APP_NONE},
  {1, GSTK_USAT_DISP_TEXT_SUPPORT_BYTE_9_BIT_1,  (uint8)GSTK_APP_USIM},
  {1, GSTK_SUSTAINED_DISP_TEXT_SUPPORT,          (uint8)GSTK_APP_SIM},
  {1, GSTK_LAUNCH_BROWSER_SUPPORT,               (uint8)GSTK_APP_NONE},
  {1, GSTK_BROWSER_STATUS_EVT_SUPPORT,           (uint8)GSTK_APP_USIM},
  {1, GSTK_PROVIDE_LOCAL_INFO_LANG_SUPPORT,      (uint8)GSTK_APP_NONE},
  {1, GSTK_SK_SUPPORT_IN_SEL_ITEM,               (uint8)GSTK_APP_NONE},
  {1, GSTK_SK_SUPPORT_IN_SET_UP_MENU,            (uint8)GSTK_APP_NONE},  
  {1, GSTK_BROWSER_TERM_EVT_SUPPORT,             (uint8)GSTK_APP_NONE},
  {2, GSTK_MAX_SK_SUPPORT,                       (uint8)GSTK_APP_NONE},
  {1, GSTK_LANGUAGE_NOTIFICATION_SUPPORT,        (uint8)GSTK_APP_NONE},
  {1, GSTK_USER_ACT_EVT_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_ACTIVATE_SUPPORT,                     (uint8)GSTK_APP_USIM},
  {1, GSTK_HCI_CONNECTIVITY_EVT_SUPPORT,         (uint8)GSTK_APP_USIM},
  {1, GSTK_ALPHA_REFRESH_SUPPORT,                (uint8)GSTK_APP_USIM},
  {1, GSTK_EXT_LAUNCH_BROWSER_HTML_SUPPORT,      (uint8)GSTK_APP_USIM},
  {1, GSTK_DISPLAY_TXT_VARIABLE_TIMEOUT_SUPPORT, (uint8)GSTK_APP_USIM},
  {1, GSTK_INKEY_HELP_FOR_VARIABLE_TIMEOUT_AND_IMMEDIATE_DIGIT_RSP,
                                                 (uint8)GSTK_APP_USIM},
  {1, GSTK_INKEY_VARIABLE_TIMEOUT_SUPPORT,       (uint8)GSTK_APP_USIM}
};

/* Android TERMINAL PROFILE support from QMI-CAT */
static gstk_profile_dl_support_ext_type qmi_cati_android_profile_value_list[] =
{
  {1, GSTK_MENU_SEL_SUPPORT,                     (uint8)GSTK_APP_NONE},
  {1, GSTK_SUPPORT_FOR_ALPHA_IN_CC,              (uint8)GSTK_APP_SIM},
  {1, GSTK_UCS2_ENTRY_SUPPORT,                   (uint8)GSTK_APP_NONE},
  {1, GSTK_UCS2_DISPLAY_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_DISPLAY_OF_EXTN_TEXT,                 (uint8)GSTK_APP_SIM},
  {1, GSTK_USAT_DISPLAY_SUPPORT_BYTE_2_BIT_8,    (uint8)GSTK_APP_USIM},
  {1, GSTK_DISPLAY_TEXT_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_GET_INKEY_SUPPORT,                    (uint8)GSTK_APP_NONE},
  {1, GSTK_GET_INPUT_SUPPORT,                    (uint8)GSTK_APP_NONE},
  {1, GSTK_SELECT_ITEM_SUPPORT,                  (uint8)GSTK_APP_NONE},
  {1, GSTK_PLAY_TONE_SUPPORT,                    (uint8)GSTK_APP_NONE},
  {1, GSTK_SET_UP_MENU_SUPPORT,                  (uint8)GSTK_APP_NONE},
  {1, GSTK_IDLE_SCRN_AVAIL_EVT_SUPPORT,          (uint8)GSTK_APP_NONE},
  {1, GSTK_LANG_SEL_EVT_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_SET_UP_IDLE_MODE_TEXT_SUPPORT,        (uint8)GSTK_APP_NONE},
  {1, GSTK_USAT_DISP_TEXT_SUPPORT_BYTE_9_BIT_1,  (uint8)GSTK_APP_USIM},
  {1, GSTK_SUSTAINED_DISP_TEXT_SUPPORT,          (uint8)GSTK_APP_SIM},
  {1, GSTK_LAUNCH_BROWSER_SUPPORT,               (uint8)GSTK_APP_NONE},
  {1, GSTK_BROWSER_STATUS_EVT_SUPPORT,           (uint8)GSTK_APP_USIM},
  {1, GSTK_PROVIDE_LOCAL_INFO_LANG_SUPPORT,      (uint8)GSTK_APP_NONE},
  {1, GSTK_PROVIDE_LOCAL_INFO_TIME_SUPPORT,      (uint8)GSTK_APP_NONE},
  {1, GSTK_SK_SUPPORT_IN_SEL_ITEM,               (uint8)GSTK_APP_NONE},
  {1, GSTK_SK_SUPPORT_IN_SET_UP_MENU,            (uint8)GSTK_APP_NONE},  
  {1, GSTK_BROWSER_TERM_EVT_SUPPORT,             (uint8)GSTK_APP_NONE},
  {2, GSTK_MAX_SK_SUPPORT,                       (uint8)GSTK_APP_NONE},
  {1, GSTK_DISPLAY_TXT_VARIABLE_TIMEOUT_SUPPORT, (uint8)GSTK_APP_USIM},
  {1, GSTK_INKEY_HELP_FOR_VARIABLE_TIMEOUT_AND_IMMEDIATE_DIGIT_RSP,
                                                 (uint8)GSTK_APP_USIM},
  {1, GSTK_INKEY_VARIABLE_TIMEOUT_SUPPORT,       (uint8)GSTK_APP_USIM},
  {1, GSTK_ACTIVATE_SUPPORT,                     (uint8)GSTK_APP_USIM},
  {1, GSTK_HCI_CONNECTIVITY_EVT_SUPPORT,         (uint8)GSTK_APP_USIM},
  {1, GSTK_ALPHA_REFRESH_SUPPORT,                (uint8)GSTK_APP_USIM},
  {1, GSTK_LANGUAGE_NOTIFICATION_SUPPORT,        (uint8)GSTK_APP_NONE},
  {1, GSTK_USER_ACT_EVT_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_EXT_LAUNCH_BROWSER_HTML_SUPPORT,      (uint8)GSTK_APP_USIM}
};

/*===========================================================================

                               INTERNAL DATA

===========================================================================*/
/*---------------------------------------------------------------------------
  QMI Card Application Toolkit Service (CAT) instance state definition &
  CAT client state definition
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_client_handle                  client_handle; /* client handle provided 
                                                       by QCSI - QCCI, unique for each client*/
  q_type                             cmd_q;         /* This will contain the commands in 
                                                        progress for each client*/
  uint32                         evt_report_mask[QMI_CAT_MAX_CARD_COUNT];
} qmi_cati_client_state_type;

/*---------------------------------------------------------------------------
  Watermark queue to hold deferred responses
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_csi_service_handle       service_handle;    /* provided by QCSI while registering QMI UIM 
                                                     with QCSI. Unique for each service */ 
  qmi_sap_client_handle        sap_handle;

  qmi_cati_client_state_type * client[UIMQMI_SVC_MAX_CLIDS];
  uint32                       client_reg_count[UIMQMI_SVC_MAX_CLIDS];
} qmi_cati_state_type;

/*---------------------------------------------------------------------------
  Proactive Cmd Evt Report type
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                  mask;
  gstk_slot_id_enum_type  slot_id;
  uint8                   response_type;

  /* Proactive command evt */
  union
  {
    /* raw evt */
    struct
    {
      uint32                       cmd_ref_id;
      uint16                       raw_data_len;
      const uint8*                 raw_data_ptr;
      qmi_cati_data_event_e_type   data_evt_type;
    }cati_raw_pc_evt;

    /* end proactive session evt */
    struct
    {
      uint8     proactive_session_end_type;
    }cati_end_proactive_cmd_evt;

  } cati_evt_rpt;
} qmi_cati_evt_rpt_info_s_type;

/*---------------------------------------------------------------------------
  Structure holding cached proactive command (raw format)
---------------------------------------------------------------------------*/
typedef struct
{
  union
  {
    /* raw evt (setup menu or setup evt list) */
    struct
    {
      uint32    cmd_ref_id;
      uint16    raw_data_len;
      uint8     raw_data[CAT_RAW_MAX_DATA_LEN];
    } cati_raw_pc_evt;

    /* setup evt list */
    struct
    {
      uint32     pc_setup_evt_list;
    } cati_setup_evt_list_cmd_evt;
  } cati_evt_rpt;
} qmi_cati_cached_evt_info_type;

/*---------------------------------------------------------------------------
  Structure holding cached proactive command
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_cati_cached_evt_info_type     cached_event;
} qmi_cati_raw_cache_type;

/*---------------------------------------------------------------------------
  Token information extracted from a command
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                    is_valid;
  uint32                     token;
} qmi_cati_ind_token_info_type;

/*---------------------------------------------------------------------------
  Structure holding userdata required during GSTK response
---------------------------------------------------------------------------*/
typedef struct
{
  uint16                                request_id;
  qmi_client_handle                     client_handle;
  uint32                                client_reg_count;
  qmi_req_handle                        req_handle;
  qmi_cati_ind_token_info_type          ind_token;
} qmi_cati_userdata_type;

/*---------------------------------------------------------------------------
  Structure holding dynamically allocated cache
---------------------------------------------------------------------------*/
struct qmi_cati_cache_node
{
  struct qmi_cati_cache_node      * next_ptr;
  qmi_cati_userdata_type          * cb_userdata_ptr;
};

/*===========================================================================
  TYPEDEF QMI_CAT_REQ_HANDLER_TYPE

  DESCRIPTION
    cl_sp             : Service handle as given by the service in qmi_csi_connect
    req_handle Handle : Provided by the infrastructure to identify this particular transaction and message
    req_c_struct      : C structure with the decoded message
    req_c_struct_len: : Size of the C data structure
---------------------------------------------------------------------------*/
typedef qmi_csi_error (* const qmi_cat_req_handler_type)
(
  qmi_cati_client_state_type * cl_sp,
  qmi_req_handle               req_handle,
  void                       * req_c_struct,
  int                          req_c_struct_len
);
 
/*===========================================================================
  TYPEDEF QMI_CAT_CMD_HDLR_TYPE
 
  DESCRIPTION
    This type can be used for each qmi msg command req
===========================================================================*/
typedef struct
{
  uint16                    cmd_type;
  qmi_cat_req_handler_type  request_hdlr;
} qmi_cat_cmd_hdlr_type;

/*===========================================================================
  TYPEDEF QMI_CAT_CMD_BUF_S
  
  DESCRIPTION
    The QMI command buf type.
    Each command buf a link to Transaction and client handle.
    It has a command type and response status.
  ===========================================================================*/
typedef struct
{
  q_link_type                   link_next;    /* must be first */
  uint16                        cmd_type;     /* Command Type  */
  qmi_req_handle                req_handle;   /* req handle */
  boolean                       resp_pending; /* response pending */
  qmi_client_handle             client_handle; /* client handle */
} qmi_cat_cmd_buf_s;

/*===========================================================================
  TYPEDEF QMI_CAT_COMPARE_S
  
  DESCRIPTION
    It contains req_handle and client handle
  ===========================================================================*/
typedef struct
{
  qmi_req_handle                req_handle;    /* req handle */
  qmi_client_handle             client_handle; /* client handle */
} qmi_cat_compare_s;


typedef struct qmi_cati_cache_node qmi_cati_cmd_buf_cache_type;

/*---------------------------------------------------------------------------
  QMI service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/
static qmi_csi_error qmi_cati_reset(qmi_cati_client_state_type *, qmi_req_handle, void*, int);
static qmi_csi_error qmi_cati_set_event_report(qmi_cati_client_state_type *, qmi_req_handle, void*, int);
static qmi_csi_error qmi_cati_get_service_state(qmi_cati_client_state_type *, qmi_req_handle, void*, int);
static qmi_csi_error qmi_cati_send_tr(qmi_cati_client_state_type *, qmi_req_handle, void*, int);
static qmi_csi_error qmi_cati_send_envelope_cmd(qmi_cati_client_state_type *, qmi_req_handle, void*, int);
static qmi_csi_error qmi_cati_get_event_report(qmi_cati_client_state_type *, qmi_req_handle, void*, int);
static qmi_csi_error qmi_cati_event_confirmation(qmi_cati_client_state_type *, qmi_req_handle, void*, int);
static qmi_csi_error qmi_cati_scws_open_channel(qmi_cati_client_state_type *, qmi_req_handle, void*, int);
static qmi_csi_error qmi_cati_scws_close_channel(qmi_cati_client_state_type *, qmi_req_handle, void*, int);
static qmi_csi_error qmi_cati_scws_send_data(qmi_cati_client_state_type *, qmi_req_handle, void*, int);
static qmi_csi_error qmi_cati_scws_data_available(qmi_cati_client_state_type *, qmi_req_handle, void*, int);
static qmi_csi_error qmi_cati_scws_change_status(qmi_cati_client_state_type *, qmi_req_handle, void*, int);
static qmi_csi_error qmi_cati_get_terminal_profile(qmi_cati_client_state_type *, qmi_req_handle, void*, int);
static qmi_csi_error qmi_cati_set_configuration(qmi_cati_client_state_type *, qmi_req_handle, void*, int);
static qmi_csi_error qmi_cati_get_configuration(qmi_cati_client_state_type *, qmi_req_handle, void*, int);
static qmi_csi_error qmi_cati_get_cached_proactive_cmd(qmi_cati_client_state_type *, qmi_req_handle, void*, int);

static qmi_cat_cmd_hdlr_type  qmi_cati_cmd_callbacks[] =
{
  {QMI_CAT_RESET_REQ_V02,                     qmi_cati_reset},
  {QMI_CAT_SET_EVENT_REPORT_REQ_V02,          qmi_cati_set_event_report},
  {QMI_CAT_GET_SERVICE_STATE_REQ_V02,         qmi_cati_get_service_state},
  {QMI_CAT_SEND_TR_REQ_V02,                   qmi_cati_send_tr},
  {QMI_CAT_SEND_ENVELOPE_CMD_REQ_V02,         qmi_cati_send_envelope_cmd},
  {QMI_CAT_GET_EVENT_REPORT_REQ_V02,          qmi_cati_get_event_report},
  {QMI_CAT_EVENT_CONFIRMATION_REQ_V02,        qmi_cati_event_confirmation},
  {QMI_CAT_GET_TERMINAL_PROFILE_REQ_V02,      qmi_cati_get_terminal_profile},
  {QMI_CAT_SET_CONFIGURATION_REQ_V02,         qmi_cati_set_configuration},
  {QMI_CAT_GET_CONFIGURATION_REQ_V02,         qmi_cati_get_configuration},
  {QMI_CAT_GET_CACHED_PROACTIVE_CMD_REQ_V02,  qmi_cati_get_cached_proactive_cmd}
};

/*---------------------------------------------------------------------------
  QMI instance configurations
---------------------------------------------------------------------------*/
static void     qmi_cati_reset_client(qmi_cati_client_state_type *);
/* Prototype for QMI CAT generic message processing */
static void     qmi_cat_process_message(void);

static qmi_cati_state_type *qmi_cat_state_ptr = NULL;

/*---------------------------------------------------------------------------
  QMI_CAT Global state information - initialized in qmi_cat_init()
  Global to all QMI instances
---------------------------------------------------------------------------*/
typedef struct
{
  /* Message queue */
  q_type                                     qmi_cat_q;
  /* Client ids */
  gstk_client_id_type                        cat_gstk_client_id[QMI_CAT_MAX_CARD_COUNT];
  gstk_client_id_type                        cat_gstk_client_id_external[QMI_CAT_MAX_CARD_COUNT];
  /* Number of slots device supports */
  uint8                                      slot_count;
  /* Global masks of registered events */
  uint32                                     evt_reg_mask[QMI_CAT_MAX_CARD_COUNT];
  uint32                                     full_func_evt_reg_mask[QMI_CAT_MAX_CARD_COUNT];
  /* Flag to set if the event received is registered as a full functionality for the ongoing proactive command */
  boolean                                    full_func_evt[QMI_CAT_MAX_CARD_COUNT];
  /* Cache of proactive commands */
  qmi_cati_raw_cache_type                  * raw_cache[QMI_CAT_MAX_CARD_COUNT][CATI_MAX_RAW_CACHED_EVTS];
  qmi_cati_raw_cache_type                  * recovery_raw_cache[QMI_CAT_MAX_CARD_COUNT][CATI_MAX_RAW_QMI_CLIENT_CACHED_EVTS];
  gstk_cmd_enum_type                         last_gstk_cmd[QMI_CAT_MAX_CARD_COUNT];
  mmgsdi_client_id_type                      cat_mmgsdi_client_id;
  /* Configuration mode */
  qmi_cati_config_mode_e_type                config_mode;
  /* Flag to indication return value for display alpha confirmation */
  qmi_cati_display_alpha_conf_e_type         display_alpha_conf;
  /* Flag to control use of sms_pp_envelope commands */
  boolean                                    sms_pp_envelope_blocked;
  /* Reference command id for event confirmation */
  uint32                                     ref_command_id[QMI_CAT_MAX_CARD_COUNT];
  /* BIP channel status */
  qmi_cati_bip_status_e_type                 bip_status_cache[QMI_CAT_MAX_CARD_COUNT][QMI_CATI_MAX_BIP_CHANNEL];
  /* Track numbers of SETUP_EVENT_XX_NOTIFY that is waiting for TR */
  uint8                                      decoded_setup_evt_in_progress[QMI_CAT_MAX_CARD_COUNT];
  /* command buffer of pending scws requests*/
  qmi_cati_cmd_buf_cache_type              * scws_cmd_buf[QMI_CAT_MAX_CARD_COUNT][QMI_CATI_MAX_SCWS_CHANNEL];
  /* card state */
  qmi_cati_card_state_e_type                 card_state[QMI_CAT_MAX_CARD_COUNT];
}qmi_cati_global_type;

static qmi_cati_global_type                  *qmi_cati_global_ptr = NULL;

typedef struct
{
  uint8  command_id;  /* command id */
  uint8  cache_index; /* index to qmi_cati_global_type.raw_cache[] */
}qmi_cat_raw_cache_cmd_index_type;

const static qmi_cat_raw_cache_cmd_index_type  qmi_cat_raw_cache_cmd_index[] =
{
  {GSTK_CMD_STK_SET_UP_MENU,            CATI_CACHED_EVT_SETUP_MENU},
  {GSTK_CMD_STK_SET_UP_IDLE_MODE_TEXT,  CATI_CACHED_EVT_SETUP_IDLE_MODE_TEXT},
  {GSTK_CMD_STK_SET_UP_EVENT_LIST,      CATI_CACHED_EVT_SETUP_EVT_LIST},
  {GSTK_CMD_STK_DISPLAY_TEXT,           CATI_CACHED_EVT_DISPLAY_TEXT},
  {GSTK_CMD_STK_PROVIDE_LOCAL_INFO,     CATI_CACHED_EVT_PROVIDE_LOCAL_INFO},
  {GSTK_CMD_STK_GET_INPUT,              CATI_CACHED_EVT_GET_INPUT}
};

const static qmi_cat_raw_cache_cmd_index_type  qmi_recovery_cat_raw_cache_cmd_index[] =
{
  {GSTK_CMD_STK_SET_UP_MENU,            CATI_QMI_CLIENT_CACHED_EVT_SETUP_MENU},
  {GSTK_CMD_STK_SET_UP_IDLE_MODE_TEXT,  CATI_QMI_CLIENT_CACHED_EVT_SETUP_IDLE_MODE_TEXT},
  {GSTK_CMD_STK_SET_UP_EVENT_LIST,      CATI_QMI_CLIENT_CACHED_EVT_SETUP_EVT_LIST},
};

typedef struct
{
  uint8               stk_cmd_id;
  gstk_cmd_enum_type  gstk_cmd_id;
}qmi_cat_stk_cmd_to_gstk_cmd_type;

const static qmi_cat_stk_cmd_to_gstk_cmd_type qmi_cat_stk_cmd_to_gstk_cmd[] =
{
  {GSTK_CMD_STK_DISPLAY_TEXT,           GSTK_DISPLAY_TEXT_REQ},
  {GSTK_CMD_STK_GET_INKEY,              GSTK_GET_INKEY_REQ},
  {GSTK_CMD_STK_GET_INPUT,              GSTK_GET_INPUT_REQ},
  {GSTK_CMD_STK_SET_UP_MENU,            GSTK_SETUP_MENU_REQ},
  {GSTK_CMD_STK_SELECT_ITEM,            GSTK_SELECT_ITEM_REQ},
  {GSTK_CMD_STK_SEND_SHORT_MESSAGE,     GSTK_SEND_SMS_REQ},
  {GSTK_CMD_STK_SET_UP_IDLE_MODE_TEXT,  GSTK_SETUP_IDLE_TEXT_REQ},
  {GSTK_CMD_STK_LANG_NOTIFICATION,      GSTK_LANG_NOTIFICATION_REQ},
  {GSTK_CMD_STK_PLAY_TONE,              GSTK_PLAY_TONE_REQ},
  {GSTK_CMD_STK_SET_UP_CALL,            GSTK_SETUP_CALL_REQ},
  {GSTK_CMD_STK_SEND_DTMF,              GSTK_SEND_DTMF_REQ},
  {GSTK_CMD_STK_LAUNCH_BROWSER,         GSTK_LAUNCH_BROWSER_REQ},
  {GSTK_CMD_STK_SEND_SS,                GSTK_SEND_SS_REQ},
  {GSTK_CMD_STK_USSD,                   GSTK_SEND_USSD_REQ},
  {GSTK_CMD_STK_PROVIDE_LOCAL_INFO,     GSTK_PROVIDE_LOCAL_INFO_REQ},
  {GSTK_CMD_STK_SET_UP_EVENT_LIST,      GSTK_SETUP_EVENT_LIST_REQ},
  {GSTK_CMD_STK_OPEN_CHANNEL,           GSTK_OPEN_CH_REQ},
  {GSTK_CMD_STK_CLOSE_CHANNEL,          GSTK_CLOSE_CH_REQ},
  {GSTK_CMD_STK_RECEIVE_DATA,           GSTK_RECEIVE_DATA_REQ},
  {GSTK_CMD_STK_SEND_DATA,              GSTK_SEND_DATA_REQ},
  {GSTK_CMD_STK_ACTIVATE,               GSTK_ACTIVATE_REQ},
  {GSTK_CMD_STK_REFRESH,                GSTK_REFRESH_REQ}
};

/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS

===========================================================================*/

/* GSTK CB's*/
static void qmi_cati_gstk_init_cb ( gstk_status_enum_type      gstk_reg_status,
                                    gstk_client_id_type        client_id,
                                    gstk_client_ref_data_type  user_data );

static void qmi_cati_gstk_init_cb_external( gstk_status_enum_type      gstk_reg_status,
                                            gstk_client_id_type        client_id,
                                            gstk_client_ref_data_type  user_data );

static void qmi_cati_gstk_cmd_cb( gstk_status_enum_type        gstk_reg_status,
                                  gstk_client_ref_data_type    user_data );

static void qmi_cati_gstk_evt_cb( gstk_client_pro_cmd_reg_data_type *req_ptr );

static void qmi_cati_gstk_raw_env_cb( gstk_cmd_from_card_type  *env_rsp_ptr );

static void qmi_cat_mmgsdi_response_cb (mmgsdi_return_enum_type status,
                                        mmgsdi_cnf_enum_type    cnf_type,
                                        const mmgsdi_cnf_type  *cnf_ptr);

static void qmi_cat_mmgsdi_evt_cb(const mmgsdi_event_data_type   *mmgsdi_event);

/* Local utility functions */
static uint8 qmi_cati_find_length_of_length_value(const uint8 * length_value);

static qmi_csi_error qmi_cat_send_response_ind
(
  qmi_cat_cmd_buf_s       *cmd_buf_ptr,
  void                    *response_ptr,
  uint16                   cmd_type,
  unsigned int             response_size,
  boolean                  ind_required,
  qmi_client_handle        cli_handle
);

static qmi_csi_error qmi_cat_send_immediate_resp
(
  qmi_cat_cmd_buf_s       *cmd_buf_ptr,
  void                    *response_ptr,
  unsigned int             response_size,
  qmi_client_handle        cli_handle
);


/*===========================================================================

                             LOCAL FUNCTIONS

===========================================================================*/


/*===========================================================================
  FUNCTION QMI_CATI_CLONE_DECODED_PROACTIVE_COMMAND

  DESCRIPTION
    Makes a deep copy of a proactive command in decoded format

  PARAMETERS
    gstk_cmd_from_card_type: source procative command
    gstk_cmd_from_card_type: destination procative command

  RETURN VALUE
    boolean: result of the copy

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_cati_clone_decoded_proactive_command
(
  gstk_cmd_from_card_type       * dst,
  const gstk_cmd_from_card_type * src
)
{
  ASSERT(src != NULL);
  ASSERT(dst != NULL);

  /* Copy header */
  (void)memscpy(&dst->hdr_cmd,
                sizeof(gstk_exp_hdr_type),
                &src->hdr_cmd,
                sizeof(gstk_exp_hdr_type));

  switch(src->hdr_cmd.command_id)
  {
    case GSTK_TERM_RSP_SW1_SW2_RPT:
      (void)memscpy(&dst->cmd.term_rsp_sw1_sw2_rpt,
                    sizeof(gstk_term_rsp_sw1_sw2_rpt_type),
                    &src->cmd.term_rsp_sw1_sw2_rpt,
                    sizeof(gstk_term_rsp_sw1_sw2_rpt_type));
      break;
    default:
      UIM_MSG_ERR_1("QMI_CAT: Invalid proactive command: 0x%x",
                    src->hdr_cmd.command_id);
      return FALSE;
  }

  return TRUE;
} /* qmi_cati_clone_decoded_proactive_command */


/*===========================================================================
FUNCTION  QMI_CAT_IS_COMMAND_DISABLED()
    
DESCRIPTION
Utility function to determine if the command is disabled at compile
        time or not
    
PARAMETERS
cmd_type : QMI CAT command
    
RETURN VALUE
boolean
    
DEPENDENCIES
None
    
SIDE EFFECTS
None
===========================================================================*/
static boolean qmi_cat_is_command_disabled
(
  uint16 cmd_type
)
{
  boolean is_disabled = FALSE;
 /* if((
#ifdef REMOVE_QMI_CAT_RESET_V02
           cmd_type != QMI_CAT_RESET_REQ_V02
#endif /* REMOVE_QMI_CAT_RESET_V02 
#ifdef REMOVE_QMI_CAT_SET_EVENT_REPORT_V02
         && cmd_type != QMI_CAT_SET_EVENT_REPORT_REQ_V02
#endif /* REMOVE_QMI_CAT_SET_EVENT_REPORT_V02 
#ifdef REMOVE_QMI_CAT_GET_SERVICE_STATE_V02
         && cmd_type != QMI_CAT_GET_SERVICE_STATE_REQ_V02
#endif /* REMOVE_QMI_CAT_GET_SERVICE_STATE_V02 
#ifdef REMOVE_QMI_CAT_SEND_TR_V02
         && cmd_type != QMI_CAT_SEND_TR_REQ_V02
#endif /* REMOVE_QMI_CAT_SEND_TR_V02 
#ifdef REMOVE_QMI_CAT_SEND_ENVELOPE_CMD_V02
         && cmd_type != QMI_CAT_SEND_ENVELOPE_CMD_REQ_V02
#endif /* REMOVE_QMI_CAT_SEND_ENVELOPE_CMD_V02 
#ifdef REMOVE_QMI_CAT_GET_EVENT_REPORT_V02
         && cmd_type != QMI_CAT_GET_EVENT_REPORT_REQ_V02
#endif /* REMOVE_QMI_CAT_GET_EVENT_REPORT_V02 
#ifdef REMOVE_QMI_CAT_EVENT_CONFIRMATION_V02
         && cmd_type != QMI_CAT_EVENT_CONFIRMATION_REQ_V02
#endif /* REMOVE_QMI_CAT_EVENT_CONFIRMATION_V02 
#ifdef REMOVE_QMI_CAT_GET_TERMINAL_PROFILE_V02
         && cmd_type != QMI_CAT_GET_TERMINAL_PROFILE_REQ_V02
#endif /* REMOVE_QMI_CAT_GET_TERMINAL_PROFILE_V02 
#ifdef REMOVE_QMI_CAT_SET_CONFIGURATION_V02
         && cmd_type != QMI_CAT_SET_CONFIGURATION_REQ_V02
#endif /* REMOVE_QMI_CAT_SET_CONFIGURATION_V02 
#ifdef REMOVE_QMI_CAT_GET_CONFIGURATION_V02
         && cmd_type != QMI_CAT_GET_CONFIGURATION_REQ_V02
#endif /* REMOVE_QMI_CAT_GET_CONFIGURATION_V02 
#ifdef REMOVE_QMI_CAT_GET_CACHED_PROACTIVE_CMD_V02
         && cmd_type != QMI_CAT_GET_CACHED_PROACTIVE_CMD_REQ_V02
#endif /* QMI_CAT_GET_CACHED_PROACTIVE_CMD_REQ_V02 
        ))
  {
    is_disabled = TRUE;
  }*/
  return is_disabled;
} /* qmi_cat_is_command_disabled */

/*===========================================================================
  FUNCTION QMI_CATI_CLONE_ENVELOPE_RSP

  DESCRIPTION
    Makes a deep copy of an envelope rsp in raw format

  PARAMETERS
    gstk_cmd_from_card_type: source env rsp
    gstk_cmd_from_card_type: destination env rsp

  RETURN VALUE
    boolean: result of the copy

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_cati_clone_envelope_rsp
(
  gstk_cmd_from_card_type       * dst,
  const gstk_cmd_from_card_type * src
)
{
  ASSERT(src != NULL);
  ASSERT(dst != NULL);

  memset(dst, 0, sizeof(gstk_exp_hdr_type));

  /* Copy header */
  (void)memscpy(&dst->hdr_cmd,
                sizeof(gstk_exp_hdr_type),
                &src->hdr_cmd,
                sizeof(gstk_exp_hdr_type));

  if (src->hdr_cmd.command_id == GSTK_RAW_ENVELOPE_RSP)
  {
    (void)memscpy(&dst->cmd.raw_envelope_rsp,
                  sizeof(gstk_raw_envelope_rsp_type),
                  &src->cmd.raw_envelope_rsp,
                  sizeof(gstk_raw_envelope_rsp_type));
    uimqmi_deep_copy((void**)&dst->cmd.raw_envelope_rsp.rsp_data_ptr,
                     (void*)src->cmd.raw_envelope_rsp.rsp_data_ptr,
                     src->cmd.raw_envelope_rsp.length);
  }
  else
  {
    UIM_MSG_ERR_1("qmi_cati_clone_envelope_rsp:Invalid cmd id 0x%x",
                  src->hdr_cmd.command_id);
    return FALSE;
  }
  return TRUE;
} /* qmi_cati_clone_envelope_rsp */

/*===========================================================================
  FUNCTION QMI_CAT_UTIL_GET_CLI_INDEX()

  DESCRIPTION
    This function checks if client already registered with 
    QMI UIM service, if yes then it return client index

  PARAMETERS
    cli_handle[in]: Client Handle
    cli_index[out]: Client Index

  RETURN VALUE
    TRUE:  if already registered
    FALSE: otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_cat_util_get_cli_index
(
  qmi_client_handle             cli_handle,
  uint16                       *cli_index
)
{
  uint16    client_index = 0;

  if(qmi_cat_state_ptr == NULL)
  {
    return FALSE;
  }
  for(client_index = 0; client_index < (int)UIMQMI_SVC_MAX_CLIDS; client_index++)
  {
    if ((qmi_cat_state_ptr->client[client_index] != NULL) &&
        (qmi_cat_state_ptr->client[client_index]->client_handle == cli_handle))
    {
      if(cli_index)
      {
        *cli_index = client_index;
      }
      return TRUE;
    }
  }
  UIM_MSG_ERR_1("not able to find client index for client handle 0x%x", cli_handle);

  return FALSE;
} /* qmi_cat_util_get_cli_index */

/*===========================================================================
FUNCTION QMI_CAT_COMPARE_QMI_CMD

DESCRIPTION
This function will be called by q_linear_delete() to check if particular cmd
is present in the queue or not.

PARAMETER
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  qmi_cat_compare_s

DEPENDENCIES
  None

RETURN VALUE
  1 - if there is a match
  0 - if there is no match

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
int qmi_cat_util_compare_qmi_cmd
(
  void *item_ptr,
  void *param_ptr
)
{
  qmi_cat_compare_s       *param_cmd_buf_ptr    = NULL;
  qmi_cat_cmd_buf_s       *item_cmd_buf_ptr     = NULL;
  
  item_cmd_buf_ptr  = (qmi_cat_cmd_buf_s*)item_ptr;
  param_cmd_buf_ptr = (qmi_cat_compare_s*)param_ptr;

  if (!item_cmd_buf_ptr || !param_cmd_buf_ptr)
  {
    return 0;
  }

  if(param_cmd_buf_ptr->client_handle == item_cmd_buf_ptr->client_handle &&
     param_cmd_buf_ptr->req_handle == item_cmd_buf_ptr->req_handle)
  {
    return 1;
  }
  return 0;
} /* qmi_cat_util_compare_qmi_cmd */

/*===========================================================================
  FUNCTION QMI_CAT_UTIL_GET_BUF_PTR()

  DESCRIPTION
   This function uses client handle and request handle to check
   if any cmd is available in the client cmd q corresponding
   to provided clinet handle.

  PARAMETERS
    qmi_req_handle        req_handle
    qmi_client_handle     cli_handle

  RETURN VALUE
    qmi_cat_cmd_buf_s

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_cat_cmd_buf_s* qmi_cat_util_get_cmd_buf_ptr
(
  qmi_req_handle        req_handle,
  qmi_client_handle     cli_handle
)
{
  qmi_cat_compare_s       compare_s   = {0, };
  uint16                  cli_index   = 0;
  qmi_cat_cmd_buf_s      *cmd_buf_ptr = NULL;

  ASSERT(qmi_cat_state_ptr);
  
  if(!qmi_cat_util_get_cli_index( cli_handle, &cli_index))
  { 
    return NULL;
  }
  
  compare_s.client_handle = cli_handle;
  compare_s.req_handle    = req_handle;
  
  cmd_buf_ptr =   q_linear_search(&qmi_cat_state_ptr->client[cli_index]->cmd_q , 
                                  qmi_cat_util_compare_qmi_cmd, 
                                  (void*)&compare_s);

  return cmd_buf_ptr;
} /* qmi_cat_util_get_cmd_buf_ptr */

/*===========================================================================
  FUNCTION QMI_CATI_FREE_ENVELOPE_RSP

  DESCRIPTION
    Frees previously allocated memory for envelope response

  PARAMETERS
    gstk_cmd_from_card_type: Envelope response

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cati_free_envelope_rsp
(
  gstk_cmd_from_card_type       * command_ptr
)
{
  ASSERT(command_ptr != NULL);

  switch(command_ptr->hdr_cmd.command_id)
  {
    case GSTK_RAW_ENVELOPE_RSP:
      QMI_CAT_FREE_IF_NOT_NULL(command_ptr->cmd.raw_envelope_rsp.rsp_data_ptr);
      break;

    default:
      UIM_MSG_ERR_1("qmi_cati_free_envelope_rsp: Invalid cmd id 0x%x",
                    command_ptr->hdr_cmd.command_id);
      break;
  }
  /* Zero the command, as it's no longer required */
  memset(command_ptr, 0, sizeof(gstk_cmd_from_card_type));
} /* qmi_cati_free_envelope_rsp */


/*===========================================================================
QMI_CAT_SIG_HANDLER

DESCRIPTION
  QMI CAT signal handler for GSTK signals

PARAMETERS
  None.

  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared.
===========================================================================*/
boolean qmi_cat_sig_handler
(
  void
)
{
  ASSERT(qmi_cati_global_ptr);

  qmi_cat_process_message();

  /* This signal is set asyncronously however the framework handles
     concurrency issues so no need to worry about it here */
  if (q_cnt(&qmi_cati_global_ptr->qmi_cat_q) == 0)
  {
    return TRUE;
  }

  return FALSE;
} /* qmi_cat_sig_handler */


/*===========================================================================
QMI_CAT_SIG_HANDLER

DESCRIPTION
  QMI CAT signal handler for GSTK signals

PARAMETERS
  None.

  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared.
===========================================================================*/
void qmi_cat_handle_qcsi_evt
(
  rex_tcb_type * task_tcb, 
  rex_sigs_type  sigs
)
{
    qmi_csi_error     err        = QMI_CSI_NO_ERR;
    qmi_csi_os_params os_params;
    
    memset(&os_params, 0x00, sizeof(qmi_csi_os_params));
    
    if(qmi_cat_state_ptr == NULL || task_tcb == NULL)
    {
      UIM_MSG_ERR_0("Cannot handle qcsi sig as qmi uim is not init");
      return;
    }
    
    os_params.tcb = task_tcb;
    os_params.sig = sigs;
    
    /* below function will call qmi uim cb registred at the time of QCSI registration */
    err = qmi_csi_handle_event(qmi_cat_state_ptr->service_handle,
                               &os_params);
    
    if (err != QMI_CSI_NO_ERR)
    {
      UIM_MSG_ERR_1("qmi_cat_handle_qcsi_evt: failed with error = 0x%x", err);
    }

} /* qmi_cat_sig_handler */



/*===========================================================================
FUNCTION QMI_CAT_FREE_MESSAGE()

DESCRIPTION
  This function frees cat_message_ptr

PARAMETERS
  message_type: type of QMI CAT message

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void qmi_cat_free_message
(
  qmi_cat_message_type * cat_message_ptr
)
{
  if(cat_message_ptr != NULL)
  {
    switch(cat_message_ptr->message_type)
    {
      case QMI_CAT_MESSAGE_ENVELOPE_RESPONSE:
        qmi_cati_free_envelope_rsp(&(cat_message_ptr->data.envelope_rsp.envelope_rsp));
        break;
      default:
        /* Nothing to free here */
        break;
    }
    uimqmi_free(cat_message_ptr);
  }
} /* qmi_cat_free_message */


/*===========================================================================
  FUNCTION QMI_CAT_CREATE_MESSAGE

  DESCRIPTION
    Allocates the memory for a QMI CAT message and initializes it.

  PARAMETERS
    message_type  : type of QMI CAT message

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_cat_message_type * qmi_cat_create_message
(
  qmi_cat_message_enum_type message_type
)
{
  qmi_cat_message_type * msg_ptr = NULL;

  msg_ptr = (qmi_cat_message_type*)uimqmi_malloc(sizeof(qmi_cat_message_type));
  if (msg_ptr != NULL)
  {
    memset(msg_ptr, 0, sizeof(qmi_cat_message_type));
    msg_ptr->message_type = message_type;
  }

  return msg_ptr;
} /* qmi_cat_create_message */


/*===========================================================================
  FUNCTION QMI_CAT_POST_MESSAGE

  DESCRIPTION
    Adds a message to the QMI CAT queue and posts an event to DCC task,
    so that it can be processed later in the right context.

  PARAMETERS
    cat_message_ptr  : QMI CAT message

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_post_message
(
  qmi_cat_message_type * cat_message_ptr
)
{
  ASSERT(qmi_cati_global_ptr);

  if(cat_message_ptr == NULL)
  {
    return;
  }

  /* Add the message to the QMI CAT queue */
  q_put(&qmi_cati_global_ptr->qmi_cat_q, &cat_message_ptr->link_next);

  /* Enable the GSTK_QMI_CAT_SIG to process in GSTK context*/
  gstk_set_qmi_cat_sig();
} /* qmi_cat_post_message */

/*===========================================================================
  FUNCTION QMI_CAT_GET_MESSAGE

  DESCRIPTION
    Retrieve a message from the queue of QMI CAT

  PARAMETERS
    None

  RETURN VALUE
    qmi_cat_message_type : Message extracted from the queue

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_cat_message_type * qmi_cat_get_message
(
  void
)
{
  qmi_cat_message_type * cat_message_ptr = NULL;

  ASSERT(qmi_cati_global_ptr);

  if (q_cnt(&qmi_cati_global_ptr->qmi_cat_q) > 0)
  {
    cat_message_ptr = (qmi_cat_message_type*)q_get(&qmi_cati_global_ptr->qmi_cat_q);
  }

  return cat_message_ptr;
} /* qmi_cat_get_message */

/*===========================================================================
  FUNCTION QMI_CAT_CONNECT_CB

  DESCRIPTION
    Connect callback function registered with the QCSI to be invoked when
    client sends the first request to QMI UIM.

  PARAMETERS
    cli_handle
    service_cookie
    connection_handle

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_cat_connect_cb (
  qmi_client_handle   cli_handle,
  void               *service_cookie,
  void              **connection_handle
)
{
  uint16                      index = 0;
  qmi_cati_client_state_type *cl_sp = NULL;

  /* there is no need to store service specific context information */
  (void) service_cookie;

  UIM_MSG_HIGH_1("qmi_cat_connect_cb Incoming Client allocation for client handle: 0x%x",
                 cli_handle);

  ASSERT(qmi_cat_state_ptr);

  if(connection_handle == NULL)
  {
    UIM_MSG_ERR_0("Invalid connection handle");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* Check if client already registered */
  if(qmi_cat_util_get_cli_index(cli_handle, &index))
  {
    UIM_MSG_ERR_2("qmi_cat_connect_cb: client 0x%x present at index 0x%x ",
                   cli_handle, index);
    return QMI_CSI_CB_CONN_REFUSED;
  }
  
  cl_sp = uimqmi_malloc(sizeof(qmi_cati_client_state_type));

  if(cl_sp == NULL)
  {
    UIM_MSG_ERR_0("Memory exhausted");
    return QMI_CSI_CB_INTERNAL_ERR;
  }
  
  memset(cl_sp, 0, sizeof(qmi_cati_client_state_type));

  for (index = 0; index < UIMQMI_SVC_MAX_CLIDS; index++)
  {
    if (qmi_cat_state_ptr->client[index] == NULL)
    {
      qmi_cat_state_ptr->client[index] = cl_sp;
      qmi_cat_state_ptr->client[index]->client_handle = cli_handle; 
      q_init(&qmi_cat_state_ptr->client[index]->cmd_q);
     *connection_handle = qmi_cat_state_ptr->client[index];
      break;
    }
  }
  
  if(index >= UIMQMI_SVC_MAX_CLIDS)
  {
    uimqmi_free(cl_sp);
    UIM_MSG_ERR_0("qmi_cat_connect_cb: No table entry availbale for new client");
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  UIM_MSG_HIGH_1("qmi_cat_connect_cb: client registered successfully *connection_handle 0x%x ", *connection_handle );
  return QMI_CSI_CB_NO_ERR;
} /* qmi_cat_connect_cb */


/*===========================================================================
  FUNCTION QMI_CAT_DISCONNECT_CB

  DESCRIPTION
    Callback function registered with the QCSI to be invoked when a client
    requests to remove connection with service.

  PARAMETERS
    connection_handle
    service_cookie

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_disconnect_cb (
  void                          * cl_sp_in,
  void                          * service_cookie
)
{
  uint16                      cli_index   = 0;
  qmi_cati_client_state_type *cl_sp       = NULL;
  qmi_cati_client_state_type *cl_sp_input = NULL;

  (void)service_cookie;
  
  ASSERT(cl_sp_in && qmi_cat_state_ptr);

  cl_sp_input = (qmi_cati_client_state_type*) cl_sp_in;

  UIM_MSG_HIGH_1("qmi_cat_disconnect_cb: QMI_CAT deallocation for Client: 0x%x",
                 cl_sp_input->client_handle);

  if (!qmi_cat_util_get_cli_index(cl_sp_input->client_handle, &cli_index))
  {
    UIM_MSG_ERR_1("Client is not registered, client_handle: 0x%x", cl_sp_input->client_handle);
    return;
  }

  cl_sp = qmi_cat_state_ptr->client[cli_index];

  /* Increment Client reg. count when disconnect occurs. This ensures
     that when responses are recevied from MMGSDI for any pending command
     w.r.t. this client, no response is built */
  qmi_cat_state_ptr->client_reg_count[cli_index]++;
  
  qmi_cati_reset_client(cl_sp);
  uimqmi_free(cl_sp);
  qmi_cat_state_ptr->client[cli_index] = NULL;
  cl_sp = NULL;
} /* qmi_cat_disconnect_cb */


/*===========================================================================
  FUNCTION QMI_CATI_INIT_CBACK

  DESCRIPTION
    Callback for the qmi framework registration.

  PARAMETERS
    num_instances : The number of qmi_cat instances

  RETURN VALUE
    None

  DEPENDENCIES
    Must have registered with the qmi framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cati_init_cback
(
  uint16 num_instances
)
{
  /* Nothing to do here. Callback required by qmi_framework_reg_service() */
  UIM_MSG_LOW_1("qmi_cati_init_cback instances:0x%x", num_instances);
} /* qmi_cati_init_cback */


/*===========================================================================
FUNCTION QMI_CAT_UTIL_REMOVE_QMI_CMD

DESCRIPTION
  This function will remove the perticular cmd from the 
  client cmd q and free it, which match with provided
  req_handle and client_handle.
  
PARAMETER
  q_ptr  [IN]        :     ptr to queue
  qmi_req_handle[IN] :     req_handle
  uint32[IN]         :     client_handle

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None

SEE ALSO
  None

===========================================================================*/
void qmi_cat_util_remove_qmi_cmd
(
  qmi_req_handle      req_handle,
  qmi_client_handle   client_handle,
  q_type             *q_ptr
)
{
  qmi_cat_cmd_buf_s *item_ptr  = NULL;
  qmi_cat_compare_s  compare_s = {0,};
  
  if(!q_ptr)
  {
    return;
  }

  compare_s.client_handle = client_handle;
  compare_s.req_handle    = req_handle;

  item_ptr = (qmi_cat_cmd_buf_s*)q_linear_search(q_ptr, qmi_cat_util_compare_qmi_cmd, &compare_s);
  if(item_ptr)
  {
    if(!q_delete_ext(q_ptr, &item_ptr->link_next))
    {
      UIM_MSG_ERR_0("unable to delete from client q");
    }
    uimqmi_free(item_ptr);
  }
} /* qmi_cat_util_remove_qmi_cmd */


/*===========================================================================
  FUNCTION QMI_CAT_HANDLE_REQ_CB

  DESCRIPTION
    Request callback function registered with the QCSI to be invoked when
    client sends request message to service.

  PARAMETERS
    connection_handle : client state pointer
    qmi_req_handle    : qmi request handle
    msg_id            : message id
    req_ptr           : pointer to request IDL structure
    req_len           : structure len
    service_cookie    : Not used

  RETURN VALUE
    qmi_csi_cb_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_cat_handle_req_cb (
  void                                    * cl_ptr,
  qmi_req_handle                            req_handle,
  unsigned int                              msg_id,
  void                                    * req_ptr,
  unsigned int                              req_len,
  void                                    * service_cookie
)
{
  qmi_cat_cmd_hdlr_type       *cmd_hdlr_ptr    = NULL;
  qmi_cat_cmd_buf_s           *cmd_buf_p       = NULL;
  uint16                       cmd             = 0;
  qmi_csi_error                errval          = QMI_CSI_NO_ERR;
  uint16                       tbl_size        = 0;
  uint16                       client_index    = 0;
  qmi_cati_client_state_type  *cl_sp           = NULL;

  (void)service_cookie;

  if(cl_ptr == NULL || req_ptr == NULL || req_len == 0)
  {
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  cl_sp = (qmi_cati_client_state_type *)cl_ptr;

  UIM_MSG_HIGH_0("qmi_cat_handle_req_cb");
    
  if(!qmi_cat_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    UIM_MSG_ERR_1("qmi_cat_handle_req_cb: client not registered cli_handle 0x%x",
                  cl_sp->client_handle);
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  /* Search through the UIM command callback table for
     the command specified in the incoming req */
  cmd_hdlr_ptr = qmi_cati_cmd_callbacks;
  tbl_size = sizeof(qmi_cati_cmd_callbacks) / sizeof(qmi_cati_cmd_callbacks[0]);
  for(cmd = 0; cmd < tbl_size; cmd++, cmd_hdlr_ptr++)
  {
    if (msg_id == cmd_hdlr_ptr->cmd_type)
    {
      break;
    }
  }

  if((cmd == tbl_size) ||
     (cmd_hdlr_ptr->request_hdlr == NULL) ||
     (qmi_cat_is_command_disabled(cmd_hdlr_ptr->cmd_type) != FALSE))
  {
    /* We couldn't find this command in the UIM command callbacks array */
    UIM_MSG_ERR_1("Unrecognized type 0x%x for QMI CMD HDLR", msg_id );
    return QMI_CSI_CB_UNSUPPORTED_ERR;
  }

  cmd_buf_p = uimqmi_malloc(sizeof(qmi_cat_cmd_buf_s));
  if(cmd_buf_p == NULL)
  {
    return QMI_CSI_CB_INTERNAL_ERR;
  }
  cmd_buf_p->req_handle = req_handle;
  cmd_buf_p->client_handle = cl_sp->client_handle;
  cmd_buf_p->cmd_type = (uint16) msg_id;
  /* We found the specified command in the UIM  or CAT command callbacks array */
  q_put(&qmi_cat_state_ptr->client[client_index]->cmd_q, &cmd_buf_p->link_next);
  errval = cmd_hdlr_ptr->request_hdlr(cl_sp, req_handle, req_ptr, req_len);
  if(errval != QMI_CSI_NO_ERR)
  {
    UIM_MSG_HIGH_1("qmi_cat_handle_req_cb errval 0x%x",errval);
    /* removing cmd data from client q in case of error */
    qmi_cat_util_remove_qmi_cmd(req_handle,
                                cl_sp->client_handle,
                                &qmi_cat_state_ptr->client[client_index]->cmd_q);
    
  }
  UIM_MSG_HIGH_2("Handled cmd:0x%x errval = 0x%x",
                    cmd_hdlr_ptr->cmd_type, errval);

  return qmi_uim_util_map_csi_to_csi_cb_err(errval);
} /* qmi_cat_handle_req_cb */

/*===========================================================================
  FUNCTION QMI_CATI_MAP_GSTK_STATUS_TO_ERRVAL

  DESCRIPTION
    Map gstk status to QMI-Error value

  PARAMETERS
    gstk_status_enum_type: gstk_status

  RETURN VALUE
    qmi_error_type_v01: QMI Error type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_cati_map_gstk_status_to_errval
(
  gstk_status_enum_type  gstk_status
)
{
  qmi_error_type_v01  errval = QMI_ERR_NONE_V01;

  UIM_MSG_HIGH_1("Received gstk_status %d", gstk_status);

  switch(gstk_status)
  {
    case GSTK_SUCCESS:
      errval = QMI_ERR_NONE_V01;
      break;

    case GSTK_EXTRA_PARAM_NOT_REQUIRED:
    case GSTK_EXTRA_PARAM_MISSING:
    case GSTK_INVALID_LENGTH:
    case GSTK_BAD_PARAM:
      errval = QMI_ERR_INVALID_ARG_V01;
      break;

    case GSTK_MEMORY_ERROR:
    case GSTK_CLIENT_SPACE_FULL:
      errval = QMI_ERR_NO_MEMORY_V01;
      break;

    case GSTK_UNSUPPORTED_COMMAND:
      errval = QMI_ERR_INVALID_OPERATION_V01;
      break;

    default:
      UIM_MSG_MED_0("Unhandled gstk_status received");
      errval = QMI_ERR_INTERNAL_V01;
  }

  return errval;
} /* qmi_cati_map_gstk_status_to_errval */


/*===========================================================================
  FUNCTION QMI_CATI_CONVERT_QMI_CNF_TO_GSTK_CNF()

  DESCRIPTION
    to get the GSTK cnf from the qmi terminal response cnf

  PARAMETERS
    qmi_tr_cnf : QMI TR CNF
    gstk_tr_cnf : Gstk TR_CNF

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_cati_convert_qmi_cnf_to_gstk_cnf
(
  uint8                qmi_tr_cnf,
  gstk_cmd_enum_type * gstk_tr_cnf
)
{
  ASSERT(gstk_tr_cnf != NULL);

  switch (qmi_tr_cnf)
  {
    case CATI_EVENT_DISPLAY_TEXT_CNF:
      *gstk_tr_cnf = GSTK_DISPLAY_TEXT_CNF;
      break;

    case CATI_EVENT_GET_INKEY_CNF:
      *gstk_tr_cnf = GSTK_GET_INKEY_CNF;
      break;

    case CATI_EVENT_GET_INPUT_CNF:
      *gstk_tr_cnf = GSTK_GET_INPUT_CNF;
      break;

    case CATI_EVENT_LAUNCH_BROWSER_CNF:
      *gstk_tr_cnf = GSTK_LAUNCH_BROWSER_CNF;
      break;

    case CATI_EVENT_PLAY_TONE_CNF:
      *gstk_tr_cnf = GSTK_PLAY_TONE_CNF;
      break;

    case CATI_EVENT_SELECT_ITEM_CNF:
      *gstk_tr_cnf = GSTK_SELECT_ITEM_CNF;
      break;

    case CATI_EVENT_SETUP_MENU_CNF:
      *gstk_tr_cnf = GSTK_SETUP_MENU_CNF;
      break;

    case CATI_EVENT_SETUP_IDLE_TEXT_CNF:
      *gstk_tr_cnf = GSTK_SETUP_IDLE_TEXT_CNF;
      break;

    case CATI_EVENT_PROVIDE_LANGUAGE_INFO_CNF:
      *gstk_tr_cnf= GSTK_PROVIDE_LANG_INFO_CNF;
      break;

    case CATI_EVENT_LANGUAGE_NOTIFICATION_CNF:
      *gstk_tr_cnf = GSTK_LANG_NOTIFICATION_CNF;
      break;

    case CATI_EVENT_USER_ACTIVITY_CNF:
    case CATI_EVENT_IDLE_SCREEN_CNF:
    case CATI_EVENT_LANGUAGE_SEL_CNF:
    case CATI_EVENT_HCI_CONNECTIVITY_CNF:
    case CATI_EVENT_BROWSER_TERM_CNF:
      *gstk_tr_cnf = GSTK_SETUP_EVENT_LIST_CNF;
      break;

    case CATI_EVENT_ACTIVATE_CNF:
      *gstk_tr_cnf = GSTK_ACTIVATE_CNF;
    break;

    case CATI_EVENT_SEND_SMS_CNF:
      *gstk_tr_cnf = GSTK_SEND_SMS_CNF;
      break;

    case CATI_EVENT_SETUP_CALL_CNF:
      *gstk_tr_cnf = GSTK_SETUP_CALL_CNF;
      break;

    case CATI_EVENT_SEND_DTMF_CNF:
      *gstk_tr_cnf = GSTK_SEND_DTMF_CNF;
      break;

    case CATI_EVENT_SEND_SS_CNF:
      *gstk_tr_cnf = GSTK_SEND_SS_CNF;
      break;

    case CATI_EVENT_SEND_USSD_CNF:
      *gstk_tr_cnf = GSTK_SEND_USSD_CNF;
      break;

    default:
      return QMI_ERR_INVALID_ARG_V01;
  }
  return QMI_ERR_NONE_V01;
} /* qmi_cati_convert_qmi_cnf_to_gstk_cnf */


/*===========================================================================
  FUNCTION QMI_CATI_CONVERT_GSTK_SLOT_ID_TO_QMI_SLOT_INDEX

  DESCRIPTION
    to get qmi slot index from gstk slot id

  PARAMETERS
    gstk_slot_id
    qmi_slot_index

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_cati_convert_gstk_slot_id_to_qmi_slot_index
(
  gstk_slot_id_enum_type    gstk_slot_id,
  uint8                   * slot_index_ptr
)
{
  ASSERT (slot_index_ptr != NULL);
  ASSERT(qmi_cati_global_ptr);

  /* Retrieve the slot index */
  if((gstk_slot_id == GSTK_SLOT_1) &&
     (qmi_cati_global_ptr->slot_count >= 1))
  {
    *slot_index_ptr = 0;
  }
  else if((gstk_slot_id == GSTK_SLOT_2) &&
          (qmi_cati_global_ptr->slot_count >= 2))
  {
    *slot_index_ptr = 1;
  }
  else if((gstk_slot_id == GSTK_SLOT_3) &&
          (qmi_cati_global_ptr->slot_count >= 3))
  {
    *slot_index_ptr = 2;
  }
  else
  {
    UIM_MSG_ERR_1("Device doesn't support operation on slot 0x%x",
                  gstk_slot_id);
    return QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
  }
  return QMI_ERR_NONE_V01;
} /* qmi_cati_convert_gstk_slot_id_to_qmi_slot_index*/


/*===========================================================================
  FUNCTION QMI_CATI_CHECK_LAST_GSTK_CMD

  DESCRIPTION
    check last GSTK command with TR / Event confirmation received from client

  PARAMETERS
    slot_index: slot index
    is_tr: TRUE if the request is for Terminal response
           FALSE if the request is for event confirmation.

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_cati_check_last_gstk_cmd
(
  uint8    slot_index,
  boolean  is_tr
)
{
  ASSERT(qmi_cati_global_ptr);

  /* if last cmd is end session or invalid command reference id,
     then GSTK is not expecting a TR or Evt Conf */
  if (qmi_cati_global_ptr->last_gstk_cmd[slot_index] == GSTK_END_PROACTIVE_CMD_REQ ||
      qmi_cati_global_ptr->ref_command_id[slot_index] == 0)
  {
    UIM_MSG_ERR_0("Invalid TR/Event Conf");
    return QMI_ERR_INVALID_OPERATION_V01;
  }

  /* TR is expected for these commands */
  switch(qmi_cati_global_ptr->last_gstk_cmd[slot_index])
  {
    /* TR is expected for these commands */
    case GSTK_DISPLAY_TEXT_REQ:
    case GSTK_GET_INKEY_REQ:
    case GSTK_GET_INPUT_REQ:
    case GSTK_SETUP_MENU_REQ:
    case GSTK_SELECT_ITEM_REQ:
    case GSTK_SETUP_IDLE_TEXT_REQ:
    case GSTK_LANG_NOTIFICATION_REQ:
    case GSTK_PLAY_TONE_REQ:
    case GSTK_LAUNCH_BROWSER_REQ:
    case GSTK_PROVIDE_LOCAL_INFO_REQ:
    case GSTK_PROVIDE_LANG_INFO_REQ:
    case GSTK_ACTIVATE_REQ:
      if(is_tr)
      {
        return QMI_ERR_NONE_V01;
      }
      break;
    case GSTK_SETUP_EVENT_LIST_REQ:
      /* For GSTK_FORMAT, for the same SETUP_EVENT_LIST proactive command, GSTK
         sends each setup event back to back, i.e., more than one TRs could be
         pending for the same proactive command, reject TR only if all the pending
         TRs for the same SETUP_EVENT_LIST proactive commmand are received.
       */
      if(is_tr &&
         ((qmi_cati_global_ptr->config_mode != CATI_CONFIG_MODE_DECODED &&
           qmi_cati_global_ptr->config_mode != CATI_CONFIG_MODE_CUSTOM_DECODED) ||
          qmi_cati_global_ptr->decoded_setup_evt_in_progress[slot_index] > 0))
      {
        return QMI_ERR_NONE_V01;
      }
      break;
    /* Event conf is expected for these commands */
    case GSTK_OPEN_CH_REQ:
    case GSTK_CLOSE_CH_REQ:
    case GSTK_RECEIVE_DATA_REQ:
    case GSTK_SEND_DATA_REQ:
    case GSTK_REFRESH_REQ:
    case GSTK_SCWS_OPEN_CH_REQ:
    case GSTK_SCWS_CLOSE_CH_REQ:
    case GSTK_SCWS_SEND_DATA_REQ:
    case GSTK_SCWS_DATA_AVAIL_RSP_REQ:
      if(!is_tr)
      {
        return QMI_ERR_NONE_V01;
      }
      break;
    /* Event conf/TR is expected for these commands */
    case GSTK_SETUP_CALL_REQ:
    case GSTK_SEND_SMS_REQ:
    case GSTK_SEND_USSD_REQ:
    case GSTK_SEND_DTMF_REQ:
    case GSTK_SEND_SS_REQ:
      /* TR is expected only if the clients registers for full functionality */
      if(is_tr == qmi_cati_global_ptr->full_func_evt[slot_index])
      {
        return QMI_ERR_NONE_V01;
      }
      break;
    default:
      break;
  }

  return QMI_ERR_INVALID_OPERATION_V01;
} /* qmi_cati_check_last_gstk_cmd*/


/*===========================================================================
  FUNCTION QMI_CATI_READ_CONFIGURATION()

  DESCRIPTION
    Read the configuration mode from the EFS

  PARAMETERS
    None

  RETURN VALUE
    Configuration mode

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_cati_config_mode_e_type qmi_cati_read_configuration
(
  void
)
{
  uim_common_efs_status_enum_type status     = UIM_COMMON_EFS_ERROR;
  uint8                           value      = 0;
  uint32                          size       = 0;

  /* Retrieve NV value from EFS */
  status = uim_common_efs_read(
             UIM_COMMON_EFS_UIMQMI_QMI_CAT_MODE,
             UIM_COMMON_EFS_ITEM_FILE_TYPE,
             UIM_COMMON_EFS_DEVICE,
             &value,
             sizeof(uint8));

  UIM_MSG_HIGH_2("QMI CAT configuration: status: 0x%x value: 0x%x",
                 status, value);

  if (status == UIM_COMMON_EFS_SUCCESS)
  {
    switch(value)
    {
      case 0:
      case 1:
        /* GOBI mode is no longer supported by QMI_CAT */ 
        return CATI_CONFIG_MODE_DISABLED;        
      case 2:
        return CATI_CONFIG_MODE_ANDROID;
      case 3:
        return CATI_CONFIG_MODE_DECODED;
      case 4:
        return CATI_CONFIG_MODE_DECODED_PULLONLY;
      case 5:
        return CATI_CONFIG_MODE_CUSTOM_RAW;
      case 6:
        return CATI_CONFIG_MODE_CUSTOM_DECODED;
      default:
        break;
    }
  }
  else
  {
    /* Create directory for cat config file */
    if (uim_common_efs_get_dir_stat(
          UIM_COMMON_EFS_MODEM,
          &size) != UIM_COMMON_EFS_SUCCESS)
    {
      (void)uim_common_efs_create_dir(UIM_COMMON_EFS_MODEM, 0777);
      (void)uim_common_efs_create_dir(UIM_COMMON_EFS_UIMQMI_QMI, 0777);
      (void)uim_common_efs_create_dir(UIM_COMMON_EFS_UIMQMI_QMI_CAT, 0777);
    }
    else if (uim_common_efs_get_dir_stat(
          UIM_COMMON_EFS_UIMQMI_QMI,
          &size) != UIM_COMMON_EFS_SUCCESS)
    {
      (void)uim_common_efs_create_dir(UIM_COMMON_EFS_UIMQMI_QMI, 0777);
      (void)uim_common_efs_create_dir(UIM_COMMON_EFS_UIMQMI_QMI_CAT, 0777);
    }
    else if (uim_common_efs_get_dir_stat(
          UIM_COMMON_EFS_UIMQMI_QMI_CAT,
          &size) != UIM_COMMON_EFS_SUCCESS)
    {
      (void)uim_common_efs_create_dir(UIM_COMMON_EFS_UIMQMI_QMI_CAT, 0777);
    }
  }

  /* NV item is not present or has an invalid value:
     return default value.*/
  return CATI_CONFIG_MODE_DECODED;
} /* qmi_cati_read_configuration */


/*===========================================================================
  FUNCTION QMI_CAT_DISABLE_RAW_UNSUPPORTED_FEATURES()

  DESCRIPTION
    Disable the raw unsupported features from TP, To disable features we are  
    setting value to zero in TP structure

  PARAMETERS
    profile_value_list_ptr    : List of TP structure 
    profile_value_list_length : Number of TP structure 

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_disable_raw_unsupported_features
(
  gstk_profile_dl_support_ext_type *profile_value_list_ptr,
  uint8                             profile_value_list_length
)
{
  uim_common_efs_status_enum_type status     = UIM_COMMON_EFS_ERROR;
  uint8                           i          = 0;

  /* Structure holding unsupported raw feature */
  PACKED struct PACKED_POST
  {
    boolean skip_language_notification;
    boolean skip_user_activity_event;
    boolean rfu[18];
  } raw_unsupported_features;

  if(profile_value_list_ptr == NULL || 
     profile_value_list_length == 0)
  {
    return;
  }

  memset(&raw_unsupported_features, 0 , sizeof(raw_unsupported_features));

  /* Retrieve NV value from EFS */
  status = uim_common_efs_read(
             UIM_COMMON_EFS_UIMQMI_QMI_CAT_RAW_UNSUPPORTED_FEATURES,
             UIM_COMMON_EFS_ITEM_FILE_TYPE,
             UIM_COMMON_EFS_DEVICE,
             (uint8 *)&raw_unsupported_features,
             sizeof(raw_unsupported_features));

  UIM_MSG_LOW_1("QMI CAT RAW UNSUPPORTED FEATURES: status: 0x%x", status);

  for(i = 0; i < profile_value_list_length; i++)
  {
    switch(profile_value_list_ptr[i].profile_dl_bitmask)
    {
      case GSTK_LANGUAGE_NOTIFICATION_SUPPORT:
        if(raw_unsupported_features.skip_language_notification)
        {
          profile_value_list_ptr[i].value = 0;
        }
        break;
      case GSTK_USER_ACT_EVT_SUPPORT:
        if(raw_unsupported_features.skip_user_activity_event)
        {
          profile_value_list_ptr[i].value = 0;
        }
        break;
      default:
        break;
    }
  }
} /* qmi_cat_disable_raw_unsupported_features */

  
/*===========================================================================
  FUNCTION QMI_CATI_READ_DISPLAY_ALPHA_CONF_CONF()

  DESCRIPTION
    Read the configuration mode from the EFS

  PARAMETERS
    None

  RETURN VALUE
    Configuration mode

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_cati_display_alpha_conf_e_type qmi_cati_read_display_alpha_conf
(
  void
)
{
  uim_common_efs_status_enum_type status     = UIM_COMMON_EFS_ERROR;
  uint8                           value      = 0;

  ASSERT(qmi_cati_global_ptr);

  /* Retrieve NV value from EFS */
  status = uim_common_efs_read(
             UIM_COMMON_EFS_UIMQMI_QMI_CAT_DISPLAY_ALPHA_FILE,
             UIM_COMMON_EFS_ITEM_FILE_TYPE,
             UIM_COMMON_EFS_DEVICE,
             &value,
             sizeof(uint8));

  UIM_MSG_HIGH_2("QMI CAT display alpha file status: 0x%x value: 0x%x",
                 status, value);

  if (status == UIM_COMMON_EFS_SUCCESS)
  {
    switch(value)
    {
      case 0:
        return CATI_DISPLAY_ALPHA_NO_AUTOMATIC_CONF;
      case 1:
        return CATI_DISPLAY_ALPHA_AUTOMATIC_FALSE;
      case 2:
        return CATI_DISPLAY_ALPHA_AUTOMATIC_TRUE;
      default:
        break;
    }
  }

  /* NV item is not present or has an invalid value:
     return default value.*/
  switch(qmi_cati_global_ptr->config_mode)
  {
    /* For Custom raw config_mode (WP): WP doesn't support display alpha,
       thus set default value to FALSE.*/
    case CATI_CONFIG_MODE_CUSTOM_RAW:
      return CATI_DISPLAY_ALPHA_AUTOMATIC_FALSE;
    /* For Android config_mode: QMI sends conformation to GSTK for
       Setup call display request without sending indication to UI
       Limitation in Android: UI has no knowledge if icon is being
       displayed, assume icon display successfully */
    case CATI_CONFIG_MODE_ANDROID:
      return CATI_DISPLAY_ALPHA_AUTOMATIC_TRUE;
    default:
      break;
  }
  return CATI_DISPLAY_ALPHA_NO_AUTOMATIC_CONF;
} /* qmi_cati_read_display_alpha_conf */


/*===========================================================================
  FUNCTION QMI_CATI_READ_SMS_PP_ENVELOPE_CONF()

  DESCRIPTION
    Read the SMS PP DL blocking flag from the EFS

  PARAMETERS
    None

  RETURN VALUE
    Configuration mode

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_cati_read_sms_pp_envelope_conf
(
  void
)
{
  uim_common_efs_status_enum_type status     = UIM_COMMON_EFS_ERROR;
  boolean                         value      = 0;
  
  /* Retrieve NV value from EFS */
  status = uim_common_efs_read(
             UIM_COMMON_EFS_UIMQMI_QMI_CAT_BLOCK_SMS_PP_ENV,
             UIM_COMMON_EFS_ITEM_FILE_TYPE,
             UIM_COMMON_EFS_DEVICE,
             &value,
             sizeof(uint8));

  if (status == UIM_COMMON_EFS_SUCCESS && value == 0)
  {
    return FALSE;
  }

  /* NV item is not present or has an invalid value:
     return default value as blocked */
  UIM_MSG_HIGH_0("SMS PP DL envelope is blocked due to security");
  return TRUE;
} /* qmi_cati_read_sms_pp_envelope_conf */

/*===========================================================================
  FUNCTION QMI_CATI_GET_CUSTOM_TP()

  DESCRIPTION
    Reads custom TP from NV and allocate memory to save the parsed TP
    
  PARAMETERS
    profile_value_list_pptr : Input address of TP value list pointer
    tp_count_ptr            : Output TP count pointer

  RETURN VALUE
    none

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_cati_get_custom_tp
(
  gstk_profile_dl_support_ext_type  **profile_value_list_pptr,
  uint8                              *tp_count_ptr
)
{
  int                                file_len                          = 0;
  uint16                             cmd_bitmask                       = 0;
  uint16                             tp_byte                           = 0;
  uint8                              tp_count                          = 0;
  uint8                              index                             = 0;
  uint8                              i                                 = 0;
  uint8                              mask                              = 1;
  uint8                              custom_tp[CATI_TLV_CUSTOM_TP_MAX] = {0};
  gstk_profile_dl_support_ext_type  *custom_profile_ptr                = NULL;
  uim_common_efs_status_enum_type status            = UIM_COMMON_EFS_ERROR;

  ASSERT(profile_value_list_pptr && NULL == *profile_value_list_pptr);
  ASSERT(tp_count_ptr);

  file_len = sizeof(uint8) * CATI_TLV_CUSTOM_TP_MAX;

  /* Retrieve NV value from EFS */
  status = uim_common_efs_read(
             UIM_COMMON_EFS_UIMQMI_QMI_CAT_CONFIG_TP,
             UIM_COMMON_EFS_ITEM_FILE_TYPE,
             UIM_COMMON_EFS_DEVICE,
             custom_tp,
             file_len);

  UIM_MSG_HIGH_0("qmi_cati_get_custom_tp");

  if(status != UIM_COMMON_EFS_SUCCESS)
  {
    return QMI_ERR_INTERNAL_V01;
  }

  for(i = 0; i < file_len; i++ )
  {
    UIM_MSG_LOW_2("QMI CAT custom TP: custom_tp[0x%x]:0x%x ", i, custom_tp[i]);
  }

  /* Find out tp_count */
  for(index = 0; index < file_len; index++)
  {
    tp_byte = (index + 1) << 8;
    cmd_bitmask = custom_tp[index];
    mask = 1;
    for(i = 0; i < 8 && tp_count < QMI_CAT_MODE_CUSTOM_TP_DL_MAX_VALUE; i++)
    {
      if(cmd_bitmask & mask)
      {
        /* add new entry for profile value with same dl bit mask*/
        if((GSTK_USAT_DISPLAY_SUPPORT_BYTE_2_BIT_8   == (uint32)(tp_byte | mask)) ||
           (GSTK_USAT_DISP_TEXT_SUPPORT_BYTE_9_BIT_1 == (uint32)(tp_byte | mask)))
        {
          tp_count++;
        }
        tp_count++;
      }
      mask = mask << 1;
    }
  }

  if (0 == tp_count)
  {
    *tp_count_ptr = 0;
    return QMI_ERR_NONE_V01;
  }

  /* Allocate memory based on tp_count */
  *profile_value_list_pptr = (gstk_profile_dl_support_ext_type *) uimqmi_malloc(
                                tp_count * sizeof(gstk_profile_dl_support_ext_type));
  if (NULL == *profile_value_list_pptr)
  {
    return QMI_ERR_NO_MEMORY_V01;
  }

  /* Parse and set values for each download profile */
  *tp_count_ptr      = tp_count;
  tp_count           = 0;
  custom_profile_ptr = *profile_value_list_pptr;
  for(index = 0; index < file_len; index++)
  {
    tp_byte = (index + 1) << 8;
    cmd_bitmask = custom_tp[index];
    mask = 1;
    for(i = 0; i < 8 && tp_count < *tp_count_ptr; i++)
    {
      if(cmd_bitmask & mask)
      {
        custom_profile_ptr->profile_dl_bitmask = (uint32)(tp_byte | mask);

        /* Soft Key is controlled by 2 keys value, thus set TP bitmask value to 2*/
        if(custom_profile_ptr->profile_dl_bitmask == GSTK_MAX_SK_SUPPORT)
        {
          custom_profile_ptr->value = 2;
        }
        else
        {
          custom_profile_ptr->value = 1;
        }

        if((GSTK_USSD_DO_IN_CC_SUPPORT == custom_profile_ptr->profile_dl_bitmask) ||
           (GSTK_DISPLAY_OF_EXTN_TEXT  == custom_profile_ptr->profile_dl_bitmask) ||
           (GSTK_SUSTAINED_DISP_TEXT_SUPPORT == custom_profile_ptr->profile_dl_bitmask))
        {
          custom_profile_ptr->card_app_type = (uint8)GSTK_APP_SIM;
        }
        else
        {
          custom_profile_ptr->card_app_type = (uint8)GSTK_APP_NONE;
        }

        /* add new entry for profile value with same dl bit mask*/
        if((GSTK_USAT_DISPLAY_SUPPORT_BYTE_2_BIT_8   == custom_profile_ptr->profile_dl_bitmask) ||
           (GSTK_USAT_DISP_TEXT_SUPPORT_BYTE_9_BIT_1 == custom_profile_ptr->profile_dl_bitmask))
        {
          UIM_MSG_HIGH_3("Profile dl bitmask added: 0x%x, tp_byte: 0x%x, mask 0x%x",
                         custom_profile_ptr->profile_dl_bitmask,
                         tp_byte,
                         mask);
          tp_count++;
          custom_profile_ptr++;
          custom_profile_ptr->value              = 1;
          custom_profile_ptr->profile_dl_bitmask = (uint32)(tp_byte | mask);
          custom_profile_ptr->card_app_type      = (uint8)GSTK_APP_USIM;
        }

        UIM_MSG_HIGH_3("Profile dl bitmask added: 0x%x, tp_byte: 0x%x, mask 0x%x",
                       custom_profile_ptr->profile_dl_bitmask,
                       tp_byte,
                       mask);

        tp_count++;
        custom_profile_ptr++;
      }
      mask = mask << 1;
    }
  }

  return QMI_ERR_NONE_V01;
} /* qmi_cati_get_custom_tp */


/*===========================================================================
  FUNCTION QMI_CATI_GET_TP_FROM_NV()

  DESCRIPTION
    sgt the terminal profile list from NV

  PARAMETERS
    None

  RETURN VALUE
    TP length

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint8 qmi_cati_get_tp_from_nv
(
  uint8   * custom_tp_ptr,
  uint8     custom_tp_len
)
{
  UIM_MSG_HIGH_0("qmi_cati_get_tp_from_nv");

  ASSERT (custom_tp_ptr != NULL);
  ASSERT (custom_tp_len > 0);

  memset( custom_tp_ptr, 0, custom_tp_len);

  /* Retrieve custom TP NV value from EFS */
  if(uim_common_efs_read(
       UIM_COMMON_EFS_UIMQMI_QMI_CAT_CONFIG_TP,
       UIM_COMMON_EFS_ITEM_FILE_TYPE,
       UIM_COMMON_EFS_DEVICE,
       custom_tp_ptr,
       custom_tp_len) != UIM_COMMON_EFS_SUCCESS)
  {
    return 0;
  }

  UIM_MSG_HIGH_1("qmi_cati_get_tp_from_nv file_len: 0x%x", custom_tp_len);

  return custom_tp_len;
} /* qmi_cati_get_tp_from_nv */


/*===========================================================================
  FUNCTION QMI_CATI_STORE_TP_TO_NV()

  DESCRIPTION
    store the custom tp bitmaskt to NV

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_cati_store_tp_to_nv
(
  uint8   * custom_tp_bitmask_ptr,
  uint8     custom_tp_bitmask_len
)
{
  ASSERT (custom_tp_bitmask_ptr != NULL);

  /* Store NV value to EFS */
  if (uim_common_efs_write(
        UIM_COMMON_EFS_UIMQMI_QMI_CAT_CONFIG_TP,
        UIM_COMMON_EFS_ITEM_FILE_TYPE,
        UIM_COMMON_EFS_DEVICE,
        (const char *)custom_tp_bitmask_ptr,
        (uint32)custom_tp_bitmask_len)
      != UIM_COMMON_EFS_SUCCESS)
  {
    UIM_MSG_ERR_0("failed to write custom TP file");
    return QMI_ERR_INTERNAL_V01;
  }

  return QMI_ERR_NONE_V01;
} /* qmi_cati_store_tp_to_nv */


/*===========================================================================
  FUNCTION QMI_CATI_STORE_CONFIG_MODE_TO_NV()

  DESCRIPTION
    store the qmi config mode to NV

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_cati_store_config_mode_to_nv
(
  uint8 config_mode
)
{
  /* expected directory and file created during bootup*/
  /* Store NV value to EFS */
  if (uim_common_efs_write(
        UIM_COMMON_EFS_UIMQMI_QMI_CAT_MODE,
        UIM_COMMON_EFS_ITEM_FILE_TYPE,
        UIM_COMMON_EFS_DEVICE,
        (const char *)&config_mode,
        sizeof(config_mode))
      != UIM_COMMON_EFS_SUCCESS)
  {
    UIM_MSG_ERR_0("failed to write config mode file");
    return QMI_ERR_INTERNAL_V01;
  }

  return QMI_ERR_NONE_V01;
} /* qmi_cati_store_config_mode_to_nv */


/*===========================================================================
  FUNCTION  CATI_CONV_RAW_MASK_TO_TLV_ID()

  DESCRIPTION
    Converts the raw mask to prm type.

  PARAMETERS
    event_mask : Event info

  RETURN VALUE
    Converted Prm type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint8 cati_conv_raw_mask_to_tlv_id
(
  uint32                        ev_mask,
  qmi_cati_data_event_e_type    data_evt
)
{
  uint8 ret_val = 0;

  switch (ev_mask)
  {
    case CATI_REPORT_STATUS_EV_DISPLAY_TEXT:
      ret_val = CATI_PRM_TYPE_DISPLAY_TEXT;
      break;

    case CATI_REPORT_STATUS_EV_GET_INKEY:
      ret_val = CATI_PRM_TYPE_GET_INKEY;
      break;

    case CATI_REPORT_STATUS_EV_GET_INPUT:
      ret_val = CATI_PRM_TYPE_GET_INPUT;
      break;

    case CATI_REPORT_STATUS_EV_SETUP_MENU:
      ret_val = CATI_PRM_TYPE_SETUP_MENU;
      break;

    case CATI_REPORT_STATUS_EV_SELECT_ITEM:
      ret_val = CATI_PRM_TYPE_SELECT_ITEM;
      break;

    case CATI_REPORT_STATUS_EV_SETUP_IDL_MODE_TEXT:
      ret_val = CATI_PRM_TYPE_SETUP_IDLE_MODE_TEXT;
      break;

    case CATI_REPORT_STATUS_EV_LANG_NOTIFICATION:
      ret_val = CATI_PRM_TYPE_LANG_NOTIFICATION;
      break;

    case CATI_REPORT_STATUS_EV_PLAY_TONE:
      ret_val = CATI_PRM_TYPE_PLAY_TONE_EVENT;
      break;

    case CATI_REPORT_STATUS_EV_SETUP_CALL:
      ret_val = CATI_PRM_TYPE_SETUP_CALL_EVENT;
      break;

    case CATI_REPORT_STATUS_EV_SEND_DTMF:
      ret_val = CATI_PRM_TYPE_SEND_DTMF_EVENT;
      break;

    case CATI_REPORT_STATUS_EV_LAUNCH_BROWSER:
      ret_val = CATI_PRM_TYPE_LAUNCH_BROWSER_EVENT;
      break;

    case CATI_REPORT_STATUS_EV_SENDSMS_ALPHA_ID:
      ret_val = CATI_PRM_TYPE_SEND_SMS_EVENT;
      break;

    case CATI_REPORT_STATUS_EV_SEND_SS:
      ret_val = CATI_PRM_TYPE_SEND_SS_EVENT;
      break;

    case CATI_REPORT_STATUS_EV_SEND_USSD:
      ret_val = CATI_PRM_TYPE_SEND_USSD_EVENT;
      break;

    case CATI_REPORT_STATUS_EV_PROVIDE_LOCAL_INFO_LANG:
    case CATI_REPORT_STATUS_EV_PROVIDE_LOCAL_INFO_TIME:
    case CATI_PROVIDE_LOCAL_INFO_MASK:
      ret_val = CATI_PRM_TYPE_PROVIDE_LOCAL_INFO;
      break;

    case CATI_SETUP_EVT_LIST_MASK:
    case CATI_REPORT_STATUS_EV_SETUPEVT_USER_ACT:
    case CATI_REPORT_STATUS_EV_SETUPEVT_IDL_SCR_NOTIFY:
    case CATI_REPORT_STATUS_EV_SETUPEVT_LANGSEL_NOTIFY:
    case CATI_REPORT_STATUS_EV_SETUPEVT_BROWSER_TERM:
    case CATI_REPORT_STATUS_EV_SETUPEVT_HCI_CONN:
      ret_val = CATI_PRM_TYPE_SETUP_EVENT_LIST_RAW;
      break;

    /* Data Event ev_mask has more than 1 possible prm type conversion.*/
    case CATI_REPORT_STATUS_EV_DATA_EVT:
      switch(data_evt)
      {
        case CATI_DATA_EVENT_OPEN_CHANNEL:
          ret_val = CATI_PRM_TYPE_OPEN_CHANNEL;
          break;

        case CATI_DATA_EVENT_CLOSE_CHANNEL:
          ret_val = CATI_PRM_TYPE_CLOSE_CHANNEL;
          break;

        case CATI_DATA_EVENT_SEND_DATA:
          ret_val = CATI_PRM_TYPE_SEND_DATA;
          break;

        case CATI_DATA_EVENT_RECEIVE_DATA:
          ret_val = CATI_PRM_TYPE_RECEIVE_DATA;
          break;

        default:
          UIM_MSG_ERR_1("Unhandled Raw DATA_EVT mask 0x%x", data_evt);
          break;
      }
      break;

    case CATI_REPORT_STATUS_EV_ACTIVATE:
      ret_val = CATI_PRM_TYPE_ACTIVATE;
      break;

    case CATI_REPORT_STATUS_EV_REFRESH:
      ret_val = CATI_PRM_TYPE_REFRESH_ALPHA;
      break;

    default:
      UIM_MSG_ERR_1("Unhandled Raw ev_mask to prm type conversion 0x%x",
                    ev_mask);
      break;
  }
  return ret_val;
} /* cati_conv_raw_mask_to_tlv_id */


/*===========================================================================
  FUNCTION QMI_CAT_GET_CB_USERDATA

  DESCRIPTION
    To obtain the userdata used in GSTK callbacks

  PARAMETERS
    request_id             : Type of the request
    cmd_buf_ptr            : QMI command buffer required for response

  RETURN VALUE
    qmi_cati_userdata_type : Poiner to the structure used for userdata

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_cati_userdata_type* qmi_cat_get_cb_userdata
(
  uint16                      request_id,
  qmi_cat_cmd_buf_s *         cmd_buf_ptr,
  qmi_cati_ind_token_info_type* token_ptr
)
{
  qmi_cati_userdata_type  * userdata_ptr = NULL;
  uint16                    cli_index    = 0;

  ASSERT(cmd_buf_ptr);
  ASSERT(qmi_cat_state_ptr);

  if (!qmi_cat_util_get_cli_index(cmd_buf_ptr->client_handle, &cli_index))
  {
    UIM_MSG_ERR_1("Invalid cli_handle 0x%x", cmd_buf_ptr->client_handle);
    return NULL;
  }

  /* Allocate userdata dynamically */
  userdata_ptr = uimqmi_malloc(sizeof(qmi_cati_userdata_type));
  if (userdata_ptr == NULL)
  {
    return NULL;
  }

  /* Update parameters */
  memset(userdata_ptr, 0, sizeof(qmi_cati_userdata_type));
  
  userdata_ptr->request_id       = request_id;
  userdata_ptr->req_handle      = (cmd_buf_ptr) ? cmd_buf_ptr->req_handle : 0;
  userdata_ptr->client_handle    = cmd_buf_ptr->client_handle;
  userdata_ptr->client_reg_count = qmi_cat_state_ptr->client_reg_count[cli_index];
  if (token_ptr)
  {
    userdata_ptr->ind_token.token    = token_ptr->token;
    userdata_ptr->ind_token.is_valid = TRUE;
  }

  return userdata_ptr;
} /* qmi_cat_get_cb_userdata */


/*===========================================================================
  FUNCTION QMI_CAT_FREE_CB_USERDATA

  DESCRIPTION
    To free the structure used for MMGSDI callbacks

  PARAMETERS
    request_id             : Type of the request
    cmd_buf_ptr            : QMI command buffer required for response

  RETURN VALUE
    qmi_cati_userdata_type : Poiner to the structure used for userdata

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_free_cb_userdata
(
  qmi_cati_userdata_type*  cb_userdata_ptr
)
{
  if (cb_userdata_ptr != NULL)
  {
    /* Free the dynamically allocated pointer earlier in the request */
    memset(cb_userdata_ptr, 0, sizeof(qmi_cati_userdata_type));
    uimqmi_free(cb_userdata_ptr);
    cb_userdata_ptr = NULL;
  }
} /* qmi_cat_free_cb_userdata */


/*===========================================================================
  FUNCTION QMI_CAT_SERVICE_INITIALIZE()

  DESCRIPTION
    Initializes the QMI CAT module. This function is executed only once,
    at the very beginning.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_service_initialize
(
  void
)
{
  int                     card_index    = 0;
  gstk_status_enum_type   gstk_status   = GSTK_SUCCESS;
  uim_hw_slot_info_type   slot_info;

  if (qmi_cati_global_ptr == NULL)
  {
    return;
  }

  /* Get count of available slots */
  slot_info = uim_get_hw_slot_info();
  for (card_index = 0 ; card_index < UIM_MAX_NUMBER_INSTANCES; card_index++)
  {
    if((slot_info.slot_status[card_index].slot_available) &&
       (slot_info.slot_status[card_index].slot_enabled))
    {
      qmi_cati_global_ptr->slot_count++;
    }
  }

  /* Read EFS for display alpha confirmation value*/
  qmi_cati_global_ptr->display_alpha_conf = qmi_cati_read_display_alpha_conf();

  /* Read EFS for SMS PP envelope cmd control */
  qmi_cati_global_ptr->sms_pp_envelope_blocked = qmi_cati_read_sms_pp_envelope_conf();

  /* Skip GSTK registration for disabled mode */
  if (qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_DISABLED)
  {
    return;
  }

  /* Register UI type client of GSTK and get a client id */
  if (qmi_cati_global_ptr->config_mode != CATI_CONFIG_MODE_DECODED_PULLONLY)
  {
    gstk_status = gstk_slot_client_init_ext(GSTK_SLOT_1, GSTK_UI_TYPE,
                                            qmi_cati_gstk_init_cb,
                                            GSTK_SLOT_1);
    if (GSTK_SUCCESS != gstk_status)
    {
      /* Part of QMI_CAT init succeeded which maye leave QMI CAT in a weird state
      (half initialized). So we can simply ignore the error, print an ERR trace and continue */
      UIM_MSG_MED_0("Client init failed for SLOT_1");
    }

    if(qmi_cati_global_ptr->slot_count > 1)
    {
      gstk_status = gstk_slot_client_init_ext(GSTK_SLOT_2, GSTK_UI_TYPE,
                                              qmi_cati_gstk_init_cb,
                                              GSTK_SLOT_2);
      if (GSTK_SUCCESS != gstk_status)
      {
        /* Part of QMI_CAT init succeeded which maye leave QMI CAT in a weird state
        (half initialized). So we can simply ignore the error, print an ERR trace and continue */
        UIM_MSG_MED_0("Client init failed for SLOT_2");
      }
    }

    if(qmi_cati_global_ptr->slot_count > 2)
    {
      gstk_status = gstk_slot_client_init_ext(GSTK_SLOT_3, GSTK_UI_TYPE,
                                              qmi_cati_gstk_init_cb,
                                              GSTK_SLOT_3);
      if (GSTK_SUCCESS != gstk_status)
      {
        /* Part of QMI_CAT init succeeded which maye leave QMI CAT in a weird state
        (half initialized). So we can simply ignore the error, print an ERR trace and continue */
        UIM_MSG_MED_0("Client init failed for SLOT_3");
      }
    }
  }

  /* Register PULL type client of GSTK and get a client id for Slot 1*/
  gstk_status = gstk_slot_client_init_ext(GSTK_SLOT_1, GSTK_WAKEUP_TYPE,
                                          qmi_cati_gstk_init_cb_external,
                                          GSTK_SLOT_1);
  if (GSTK_SUCCESS != gstk_status)
  {
    /* Part of QMI_CAT init succeeded which maye leave QMI CAT in a weird state
    (half initialized). So we can simply ignore the error, print an ERR trace and continue */
    UIM_MSG_MED_0("Client init failed for SLOT_1");
  }

  if(qmi_cati_global_ptr->slot_count > 1)
  {
    /* Register PULL type client of GSTK and get a client id for Slot 2 */
    gstk_status = gstk_slot_client_init_ext(GSTK_SLOT_2, GSTK_WAKEUP_TYPE,
                                            qmi_cati_gstk_init_cb_external,
                                            GSTK_SLOT_2);
    if (GSTK_SUCCESS != gstk_status)
    {
      /* Part of QMI_CAT init succeeded which maye leave QMI CAT in a weird state
      (half initialized). So we can simply ignore the error, print an ERR trace and continue */
      UIM_MSG_MED_0("Client init failed for SLOT_2");
    }
  }

  if(qmi_cati_global_ptr->slot_count > 2)
  {
    /* Register PULL type client of GSTK and get a client id for Slot 3 */
    gstk_status = gstk_slot_client_init_ext(GSTK_SLOT_3, GSTK_WAKEUP_TYPE,
                                            qmi_cati_gstk_init_cb_external,
                                            GSTK_SLOT_3);
    if (GSTK_SUCCESS != gstk_status)
    {
      /* Part of QMI_CAT init succeeded which maye leave QMI CAT in a weird state
      (half initialized). So we can simply ignore the error, print an ERR trace and continue */
      UIM_MSG_MED_0("Client init failed for SLOT_3");
    }
  }

  if(MMGSDI_SUCCESS != mmgsdi_client_id_and_evt_reg(qmi_cat_mmgsdi_evt_cb,
                                                    qmi_cat_mmgsdi_response_cb,
                                                    0))
  {
    UIM_MSG_ERR_0("QMI-CAT : mmgsdi_client_evt_reg failed");
  }
} /* qmi_cat_service_initialize */


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_CAT_INIT_PRE_STARTUP()

  DESCRIPTION
    Initializes the QMI CAT global data and message queues and initalize the 
    service and registers with MMGSDI, GSTK.


  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_cat_init_pre_startup
(
  void
)
{
  int  card_index  = 0;
  int  cache_index = 0;
  
  UIM_MSG_MED_0 ("QMI CAT qmi_cat_init_pre_startup");

  if (qmi_cat_state_ptr == NULL)
  {
    qmi_cat_state_ptr = uimqmi_malloc(sizeof(qmi_cati_state_type));
    ASSERT(qmi_cat_state_ptr != NULL);
    memset(qmi_cat_state_ptr, 0, sizeof(qmi_cati_state_type));
  }

  /* Do nothing if qmi_cati_global_ptr is already initialized */
  if (qmi_cati_global_ptr != NULL)
  {
    return;
  }

  qmi_cati_global_ptr = uimqmi_malloc(sizeof(qmi_cati_global_type));
  ASSERT(qmi_cati_global_ptr != NULL)
  memset(qmi_cati_global_ptr, 0, sizeof(qmi_cati_global_type));

  /* Initialize message queue */
  (void) q_init(&qmi_cati_global_ptr->qmi_cat_q);

  /* Init cache (raw and decoded) and event masks */
  for (card_index = 0; card_index < QMI_CAT_MAX_CARD_COUNT; card_index++)
  {
    qmi_cati_global_ptr->evt_reg_mask[card_index]           = CATI_REPORT_STATUS_NONE;
    qmi_cati_global_ptr->full_func_evt_reg_mask[card_index] = CATI_REPORT_STATUS_EV_FULL_FUNC_NONE;
    qmi_cati_global_ptr->last_gstk_cmd[card_index]          = GSTK_END_PROACTIVE_CMD_REQ;
    qmi_cati_global_ptr->ref_command_id[card_index]         = 0;
    qmi_cati_global_ptr->full_func_evt[card_index]          = FALSE;

    for (cache_index = 0; cache_index < CATI_MAX_RAW_CACHED_EVTS; cache_index++)
    {
      qmi_cati_global_ptr->raw_cache[card_index][cache_index] = NULL;
    }
    for (cache_index = 0; cache_index < CATI_MAX_RAW_QMI_CLIENT_CACHED_EVTS; cache_index++)
    {
      qmi_cati_global_ptr->recovery_raw_cache[card_index][cache_index] = NULL;
    }
    for (cache_index = 0; cache_index < QMI_CATI_MAX_BIP_CHANNEL; cache_index++)
    {
      qmi_cati_global_ptr->bip_status_cache[card_index][cache_index] = CATI_BIP_SESSION_END;
    }
    for (cache_index = 0; cache_index < QMI_CATI_MAX_SCWS_CHANNEL; cache_index++)
    {
      qmi_cati_global_ptr->scws_cmd_buf[card_index][cache_index] = NULL;
    }
  }
  
  /* Read configuration mode, this is required, so that GSTK can send the TERMINAL PROFILE correctly,
     before QMI task is fully initialized */
  qmi_cati_global_ptr->config_mode = qmi_cati_read_configuration();
} /* qmi_cat_init_pre_startup */


/*===========================================================================
  FUNCTION QMI_CAT_INIT_POST_STARTUP()

  DESCRIPTION
    Registers the QMI CAT service to the QMI framework

  PARAMETERS
    tcb_ptr :   MMGSDI tcb pointer
    sig     :   QMI Signal that has to be register with
                QCSI.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_cat_init_post_startup
(
  rex_tcb_type  * tcb_ptr,
  rex_sigs_type   sig
)
{
    qmi_csi_error                 err          =  QMI_CSI_NO_ERR;
    qmi_sap_error                 sap_err      =  QMI_SAP_NO_ERR;
    qmi_idl_service_object_type   service_obj  =  cat_get_service_object_v02();
    qmi_csi_os_params             os_params;
    qmi_csi_options               options;
    
    UIM_MSG_HIGH_0("qmi_cat_init_post_startup");
    
    ASSERT(tcb_ptr);
    
    /* Initialize global data
       We must do this now before we register for service because
       at the time of the reg_service_cb we need to have the
       internal qmi_message q initialized */
    qmi_cat_service_initialize();
    memset(&os_params, 0, sizeof(qmi_csi_os_params));
    
    os_params.tcb = tcb_ptr;
    os_params.sig = sig;
    
    if (service_obj == NULL)
    {
      UIM_MSG_ERR_1("Cannot initialize QMI CAT Remote service obj 0x%x",
                    service_obj);
      return;
    }
    
    QMI_CSI_OPTIONS_INIT(options);
    QMI_CSI_OPTIONS_SET_SCOPE(options, IPC_POLICY_SCOPE_OPEN);
    
    err = qmi_csi_register_with_options (service_obj,
                                         qmi_cat_connect_cb,
                                         qmi_cat_disconnect_cb,
                                         qmi_cat_handle_req_cb,
                                         NULL,
                                         &os_params,
                                         &options,
                                         &(qmi_cat_state_ptr->service_handle));
    
    if (err != QMI_CSI_NO_ERR || qmi_cat_state_ptr->service_handle == NULL)
    {
      UIM_MSG_ERR_1("QMI CSI register failed, error = 0x%x", err);
      return;
    }
    
    /* SAP registration necessary for native QMI clients to access the service
       and to test using QMI Test Pro */
    sap_err = qmi_sap_register(service_obj, NULL, &(qmi_cat_state_ptr->sap_handle));
    if (sap_err != QMI_SAP_NO_ERR)
    {
      UIM_MSG_ERR_1("QMI SAP register failed, error=%d", sap_err);
    }

} /* qmi_cat_init_post_startup */


/*===========================================================================
  FUNCTION QMI_CAT_GET_TERMINAL_PROFILE_VALUE_LIST()

  DESCRIPTION
    Get terminal profile value list according to the QMI_CAT config mode.

  PARAMETERS
    none

  RETURN VALUE
    gstk_profile_dl_support_ext_type* : array of terminal profile

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
gstk_profile_dl_support_ext_type* qmi_cat_get_terminal_profile_value_list
(
  uint8                            * tp_count_ptr
)
{
  qmi_error_type_v01                  errval             = QMI_ERR_NONE_V01;
  qmi_cati_config_mode_e_type         config_mode        = CATI_CONFIG_MODE_DISABLED;

  if (tp_count_ptr == NULL)
  {
    return NULL;
  }

  *tp_count_ptr = 0;

  if (qmi_cati_global_ptr != NULL)
  {
    /* Read configuration mode from global */
    config_mode = qmi_cati_global_ptr->config_mode;
  }
  else
  {
    /* QMI CAT is not init yet, read configuration mode from NV */
    config_mode = qmi_cati_read_configuration();
  }

  /* Send Terminal Profile download to GSTK */
  if (config_mode == CATI_CONFIG_MODE_ANDROID)
  {
    * tp_count_ptr = sizeof(qmi_cati_android_profile_value_list) / sizeof(gstk_profile_dl_support_ext_type);
    qmi_cat_disable_raw_unsupported_features(qmi_cati_android_profile_value_list, *tp_count_ptr);
    return qmi_cati_android_profile_value_list;
  }
  else if (config_mode == CATI_CONFIG_MODE_DECODED ||
           config_mode == CATI_CONFIG_MODE_DECODED_PULLONLY)
  {
    * tp_count_ptr = sizeof(qmi_cati_decoded_profile_value_list) / sizeof(gstk_profile_dl_support_ext_type);
    return qmi_cati_decoded_profile_value_list;
  }
  else if (config_mode == CATI_CONFIG_MODE_CUSTOM_DECODED ||
           config_mode == CATI_CONFIG_MODE_CUSTOM_RAW)
  {
    UIM_MSG_HIGH_1("update profile for QMI_CAT custom mode 0x%x", config_mode);

    /* qmi_cat_get_terminal_profile_value_list() is called for each GSTK instance.
       Allocate memory for qmi_cati_custom_profile_value_list_ptr and reads TP
       only if the memory is not already allocated
     */
    if (NULL == qmi_cati_custom_profile.value_list_ptr)
    {
      errval = qmi_cati_get_custom_tp(&qmi_cati_custom_profile.value_list_ptr,
                                      &qmi_cati_custom_profile.tp_count);
    }

    if (errval != QMI_ERR_NONE_V01|| 0 == qmi_cati_custom_profile.tp_count)
    {
      UIM_MSG_HIGH_0("Unable to get custom TP from NV or 0 TP, default registration to GSTK.");

      /* Free allocated memory */
      QMI_CAT_FREE_IF_NOT_NULL(qmi_cati_custom_profile.value_list_ptr);

      /* Clear qmi_cati_custom_tp_count */
      qmi_cati_custom_profile.tp_count = 0;

      /* If there was an error reading custom_tp values from NV, we send an empty
         profile value list. */
      return qmi_cati_empty_profile_value_list;
    }

    /* Set tp_count */
    *tp_count_ptr = qmi_cati_custom_profile.tp_count;
    return qmi_cati_custom_profile.value_list_ptr;
  }

  UIM_MSG_HIGH_0("CATI config mode from the NV is mode_disabled or is undefined.");
  return qmi_cati_empty_profile_value_list;
} /* qmi_cat_get_terminal_profile_value_list */


/*===========================================================================
FUNCTION: qmi_cati_gstk_init_cb

DESCRIPTION:
  Callback function from GSTK providing QMI-CAT with client id if successful.

PARAMETERS:
  gstk_reg_status : gstk client id registration status
  client_id       : client id is registration is successful
  user_data       : user data

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
static void qmi_cati_gstk_init_cb
(
  gstk_status_enum_type             gstk_reg_status,
  gstk_client_id_type               client_id,
  gstk_client_ref_data_type         user_data
)
{
  gstk_status_enum_type  gstk_status = GSTK_SUCCESS;
  uint8                  slot_index  = 0;

  ASSERT(qmi_cati_global_ptr);

  (void)user_data;

  UIM_MSG_HIGH_2("qmi_cati_gstk_init_cb(): gstk_reg_status=0x%x, user_data=0x%x",
                 gstk_reg_status, user_data);

  if (gstk_reg_status != GSTK_SUCCESS)
  {
    return;
  }

  /* Retrieve the slot index */
  if(qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                                    (gstk_slot_id_enum_type)user_data,
                                    &slot_index) != QMI_ERR_NONE_V01)
  {
    return;
  }

  /* Store client id */
  qmi_cati_global_ptr->cat_gstk_client_id[slot_index] = client_id;

  /* Android mode: everything in raw format */
  if (qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_ANDROID ||
      qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_CUSTOM_RAW)
  {
    gstk_status = gstk_client_toolkit_cmd_reg(
                          qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                          0x00,
                          GSTK_RAW_FORMAT,
                          sizeof(qmi_cati_complete_reg_list)/sizeof(gstk_toolkit_cmd_reg_info_type),
                          qmi_cati_complete_reg_list,
                          qmi_cati_gstk_cmd_cb,
                          qmi_cati_gstk_evt_cb);

    if (GSTK_SUCCESS != gstk_status)
    {
      UIM_MSG_LOW_1("QMI-CAT raw reg for proactive command failed 0x%x", gstk_status);
      return;
    }

    gstk_status = gstk_client_toolkit_cmd_reg(
                          qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                          0x00,
                          GSTK_RAW_FORMAT,
                          sizeof(qmi_cati_date_time_reg_list)/sizeof(qmi_cati_date_time_reg_list[0]),
                          qmi_cati_date_time_reg_list,
                          qmi_cati_gstk_cmd_cb,
                          qmi_cati_gstk_evt_cb);

    if (GSTK_SUCCESS != gstk_status)
    {
      UIM_MSG_LOW_1("QMI-CAT raw reg for DATE_TIME failed 0x%x", gstk_status);
      return;
    }

  }

  /* Decoded mode: everything in decoded format */
  if (qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_DECODED ||
      qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_CUSTOM_DECODED)
  {
    gstk_status = gstk_client_toolkit_cmd_reg(
                          qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                          0x00,
                          GSTK_GSTK_FORMAT,
                          sizeof(qmi_cati_complete_reg_list)/sizeof(gstk_toolkit_cmd_reg_info_type),
                          qmi_cati_complete_reg_list,
                          qmi_cati_gstk_cmd_cb,
                          qmi_cati_gstk_evt_cb);

    if (GSTK_SUCCESS != gstk_status)
    {
      UIM_MSG_LOW_1("QMI-CAT gstk reg for proactive command failed 0x%x",
                    gstk_status);
      return;
    }

    gstk_status = gstk_client_toolkit_cmd_reg(
                          qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                          0x00,
                          GSTK_GSTK_FORMAT,
                          sizeof(qmi_cati_scws_reg_list)/sizeof(gstk_toolkit_cmd_reg_info_type),
                          qmi_cati_scws_reg_list,
                          qmi_cati_gstk_cmd_cb,
                          qmi_cati_gstk_evt_cb);

    if (GSTK_SUCCESS != gstk_status)
    {
      UIM_MSG_LOW_1("QMI-CAT gstk reg for scws failed 0x%x", gstk_status);
      return;
    }
  }
  /* Terminal profile is sent only when the PULL type is initialized
     bacause that is done in all cases. So, no need to do it here. */
}/* qmi_cati_gstk_init_cb */


/*===========================================================================
FUNCTION: qmi_cati_gstk_init_cb_external

DESCRIPTION:
  Callback function from GSTK providing QMI-CAT with client id if successful.

PARAMETERS:
  gstk_reg_status : gstk client id registration status
  client_id       : client id is registration is successful
  user_data       : user data

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
static void qmi_cati_gstk_init_cb_external
(
  gstk_status_enum_type             gstk_reg_status,
  gstk_client_id_type               client_id,
  gstk_client_ref_data_type         user_data
)
{
  uint8                             slot_index  = 0;

  ASSERT(qmi_cati_global_ptr);

  if (gstk_reg_status != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("Error retrieving GSTK client id: 0x%x", gstk_reg_status);
    return;
  }

  /* Retrieve the slot index */
  if(qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                                    (gstk_slot_id_enum_type)user_data,
                                    &slot_index) != QMI_ERR_NONE_V01)
  {
    UIM_MSG_ERR_1("Invalid slot id for proactive command: 0x%x",
                  (gstk_slot_id_enum_type)user_data);
    return;
  }

  /* Store client id */
  qmi_cati_global_ptr->cat_gstk_client_id_external[slot_index] = client_id;

  /* For PULL only configuration, UI client has not been registered,
     so we can set Wakeup client type in both global spaces */
  if (qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_DECODED_PULLONLY)
  {
    qmi_cati_global_ptr->cat_gstk_client_id[slot_index] = client_id;
  }
} /* qmi_cati_gstk_init_cb_external */


/*===========================================================================
FUNCTION: qmi_cati_gstk_cmd_cb

DESCRIPTION:
  Immediate callback invoked to reflect status of registration

PARAMETERS:
  gstk_reg_status : gstk client id registration status
  user_data       : user data

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
static void qmi_cati_gstk_cmd_cb
(
  gstk_status_enum_type             gstk_reg_status,
  gstk_client_ref_data_type         user_data
)
{
  (void)user_data;

  /* Nothing to do... just print the result in the log */
  if (GSTK_SUCCESS != gstk_reg_status)
  {
    UIM_MSG_LOW_1("QMI-CAT: Registration with GSTK failed 0x%x",
                  gstk_reg_status);
  }
} /* qmi_cati_gstk_cmd_cb */


/*===========================================================================
FUNCTION: QMI_CATI_COVERT_STK_CMD_ID_TO_GSTK_CMD_ID

DESCRIPTION
  This function converts an STK command to a GSTK command type

PARAMETERS
  stk_cmd_id     : [Input]        STK command type
  gstk_cmd_id_ptr: [Input/Output] GSTK command type

DEPENDENCIES
  None

RETURN VALUE
  qmi_error_type_v01

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static qmi_error_type_v01 qmi_cati_covert_stk_cmd_id_to_gstk_cmd_id
(
  uint8                stk_cmd_id,
  gstk_cmd_enum_type  *gstk_cmd_id_ptr
)
{
  qmi_error_type_v01  ret_val = QMI_ERR_INVALID_ARG_V01;
  uint8             i       = 0;

  if (NULL == gstk_cmd_id_ptr)
  {
    return QMI_ERR_INTERNAL_V01;
  }

  for(i = 0;
      i < sizeof(qmi_cat_stk_cmd_to_gstk_cmd)/sizeof(qmi_cat_stk_cmd_to_gstk_cmd[0]);
      i++)
  {
    if (qmi_cat_stk_cmd_to_gstk_cmd[i].stk_cmd_id == stk_cmd_id)
    {
      *gstk_cmd_id_ptr = qmi_cat_stk_cmd_to_gstk_cmd[i].gstk_cmd_id;
      ret_val = QMI_ERR_NONE_V01;
      break;
    }
  }

  if (QMI_ERR_NONE_V01!= ret_val)
  {
    UIM_MSG_HIGH_1("qmi_cati_covert_stk_cmd_id_to_gstk_cmd_id(): unhandled stk_cmd_id:0x%x",
                   stk_cmd_id);
  }
  return ret_val;
}/* qmi_cati_covert_stk_cmd_id_to_gstk_cmd_id */


/*===========================================================================
FUNCTION: qmi_cati_gstk_evt_cb

DESCRIPTION:
  Callback function from GSTK providing QMI-CAT with the registered
  proactive command

PARAMETERS:
  req_ptr : [Input] proactive command information in the format client
                    requested (GSTK/RAW)

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
static void qmi_cati_gstk_evt_cb
(
  gstk_client_pro_cmd_reg_data_type * req_ptr
)
{
  qmi_cat_message_type  * msg_ptr              = NULL;
  gstk_cmd_enum_type      cati_gstk_command_id = GSTK_CMD_ENUM_NOT_USE;
  uint8                   card_index           = 0;

  ASSERT(qmi_cati_global_ptr);

  /* Check parameters */
  if (req_ptr == NULL)
  {
    return;
  }

  /* Check the configuration mode: events from GSTK should be handled
     only for PUSH mode */
  if (qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_DECODED_PULLONLY)
  {
    UIM_MSG_ERR_0("qmi_cati_gstk_evt_cb: Invalid mode");
    return;
  }

  /* Put the command in the DS cmd queue to be handled in DS context */
  switch (req_ptr->data_format_type)
  {
    /* RAW Format */
    case GSTK_RAW_FORMAT:
      if(!req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr)
      {
        return;
      }

      /* Validate the slot_id passed by GSTK */
      if(qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                     req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->raw_hdr.sim_slot_id,
                     &card_index) != QMI_ERR_NONE_V01)
      {
        UIM_MSG_ERR_1("qmi_cati_gstk_evt_cb: Invalid card_index: 0x%x",
                          req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->raw_hdr.sim_slot_id);
        return;
      }

      msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_RAW_PROACTIVE_COMMAND);
      if(NULL == msg_ptr)
      {
        return;
      }

      msg_ptr->data.raw_proactive_cmd.cmd_ref_id =
        req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->raw_hdr.cmd_ref_id;
      msg_ptr->data.raw_proactive_cmd.slot_id =
        req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->raw_hdr.sim_slot_id;

      /* GSTK sets raw_hdr.tag for setup call cmd, where
         user conf alpha setup call cmd = GSTK_TAG_USER_CONF_ALPHA
         display setup call cmd = GSTK_TAG_DISPLAY_ALPHA
         other commands are set to GSTK_TAG_ORIGINAL_CMD. */
      msg_ptr->data.raw_proactive_cmd.raw_hdr_tag =
        req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->raw_hdr.tag;

      msg_ptr->data.raw_proactive_cmd.raw_data_len =
            req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->payload.data_len;

      /* data len is less expected to be less than 255 */
      ASSERT(req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->payload.data_len <= CAT_RAW_MAX_DATA_LEN )

      (void)memscpy(msg_ptr->data.raw_proactive_cmd.raw_data_buffer,
                    sizeof(msg_ptr->data.raw_proactive_cmd.raw_data_buffer),
                    req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->payload.data_buffer_ptr,
                    (uint32)req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->payload.data_len);

      qmi_cat_post_message(msg_ptr);
      break;

    /* GSTK Format */
    case GSTK_GSTK_FORMAT :
      if (!req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr)
      {
        return;
      }

      /* Validate slot_id passed by GSTK */
      if(qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                     req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.sim_slot_id,
                     &card_index) != QMI_ERR_NONE_V01)
      {
        UIM_MSG_ERR_1("qmi_cati_gstk_evt_cb: Invalid card_index: 0x%x",
                          req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.sim_slot_id);
        return;
      }

      /* Copy the command id */
      cati_gstk_command_id = req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.command_id;
      UIM_MSG_HIGH_1("qmi_cati_gstk_evt_cb : Command Id 0x%x",
                     cati_gstk_command_id);
      switch(cati_gstk_command_id)
      {
        case GSTK_END_PROACTIVE_CMD_REQ:
          /* END Proactive session cmd is always made available in GSTK format.
             The same indication is used in all modes (decoded, raw) */
          msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_END_PROACTIVE_SESSION);
          if(NULL == msg_ptr)
          {
            return;
          }

          /* Store slot id */
          msg_ptr->data.end_proactive_cmd.slot_id =
            req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.sim_slot_id;

          /* Store GSTK tag */
          msg_ptr->data.end_proactive_cmd.tag =
            req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.tag;

          /* Store end session type */
          if (req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->cmd.end_pro_cmd_req.end_from_card)
          {
            msg_ptr->data.end_proactive_cmd.proactive_session_end_type =
                  CATI_PROACTIVE_SESSION_END_DUE_TO_CARD;
          }
          else
          {
            msg_ptr->data.end_proactive_cmd.proactive_session_end_type =
                  CATI_PROACTIVE_SESSION_END_INTERNAL;
          }

          qmi_cat_post_message(msg_ptr);
          break;

        case GSTK_PROFILE_DL_IND_RSP:
          /* TP download indication is always made available in GSTK format.
             The same indication is used in all modes (decoded, raw) */
          msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_TP_DL_IND);
          if(NULL == msg_ptr)
          {
            return;
          }

          /* Store slot id */
          msg_ptr->data.tp_dl_ind.slot_id =
            req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.sim_slot_id;

          qmi_cat_post_message(msg_ptr);
          break;

        case GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY:
        case GSTK_SETUP_EVT_USER_ACT_NOTIFY:
        case GSTK_SETUP_EVT_LANG_SEL_NOTIFY:
        case GSTK_DISPLAY_TEXT_REQ:
        case GSTK_GET_INKEY_REQ:
        case GSTK_GET_INPUT_REQ:
        case GSTK_LAUNCH_BROWSER_REQ:
        case GSTK_PLAY_TONE_REQ:
        case GSTK_SELECT_ITEM_REQ:
        case GSTK_SEND_SMS_REQ:
        case GSTK_SEND_SS_REQ:
        case GSTK_SEND_USSD_REQ:
        case GSTK_SETUP_CALL_REQ:
        case GSTK_SETUP_IDLE_TEXT_REQ:
        case GSTK_SETUP_MENU_REQ:
        case GSTK_SEND_DTMF_REQ:
        case GSTK_LANG_NOTIFICATION_REQ:
        case GSTK_PROVIDE_LANG_INFO_REQ:
        case GSTK_OPEN_CH_REQ:
        case GSTK_CLOSE_CH_REQ:
        case GSTK_SEND_DATA_REQ:
        case GSTK_RECEIVE_DATA_REQ:
        case GSTK_ACTIVATE_REQ:
        case GSTK_SETUP_EVT_HCI_CONNECT_NOTIFY:
        case GSTK_SETUP_EVT_BROWSER_TERM_NOTIFY:
        case GSTK_REFRESH_REQ:
        case GSTK_OPEN_CH_CNF:
        case GSTK_CLOSE_CH_CNF:
        case GSTK_SCWS_OPEN_CH_REQ:
        case GSTK_SCWS_CLOSE_CH_REQ:
        case GSTK_SCWS_SEND_DATA_REQ:
        case GSTK_SCWS_DATA_AVAIL_RSP_REQ:
          break;

        case GSTK_TERM_RSP_SW1_SW2_RPT:
          msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_TERM_RSP_SW1_SW2);
          if(NULL == msg_ptr)
          {
            return;
          }

          /* Clone responses SW1 & SW2 data */
          (void)qmi_cati_clone_decoded_proactive_command(
                         &(msg_ptr->data.terminal_response_sw1_sw2_info.sw1_sw2_data),
                         req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr);

          qmi_cat_post_message(msg_ptr);
          break;

        default :
          UIM_MSG_ERR_1("Unhandled command id in GSTK format 0x%x",
                        cati_gstk_command_id);
          break;
      }
      break;

    default :
      break;
  }
} /* qmi_cati_gstk_evt_cb */


/*===========================================================================
FUNCTION: qmi_cati_gstk_raw_env_cb

DESCRIPTION:
  Callback function from GSTK providing QMI-CAT with the envelope response
  from the card

PARAMETERS:
  env_rsp_ptr : [Input] Envelope rsp ptr (GSTK/RAW)

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
static void qmi_cati_gstk_raw_env_cb
(
  gstk_cmd_from_card_type * env_rsp_ptr
)
{
  qmi_cat_message_type * msg_ptr = NULL;

  UIM_MSG_HIGH_0("qmi_cati_gstk_raw_env_cb");

  if (!env_rsp_ptr)
  {
    return;
  }

  /* Put the command in the DS cmd queue to be handled in DS context */
  switch (env_rsp_ptr->hdr_cmd.command_id)
  {
    case GSTK_RAW_ENVELOPE_RSP :
      /* Queue this for processing in the ds task context */
      msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_ENVELOPE_RESPONSE);
      if(NULL == msg_ptr)
      {
        return;
      }

      /* Clone envelope rsp */
      (void)qmi_cati_clone_envelope_rsp(
        &(msg_ptr->data.envelope_rsp.envelope_rsp), env_rsp_ptr);
      msg_ptr->data.envelope_rsp.user_data       = (void *)env_rsp_ptr->hdr_cmd.user_data;

      qmi_cat_post_message(msg_ptr);
      break;

      default :
        UIM_MSG_ERR_1("qmi_cati_gstk_raw_env_cb:Unhandled data format 0x%x ",
                      env_rsp_ptr->hdr_cmd.command_id);
        break;
   }
}/* qmi_cati_gstk_raw_env_cb */

/*===========================================================================
  FUNCTION QMI_CAT_MMGSDI_RESPONSE_CB()

  DESCRIPTION
    Callback function called by mmgsdi to return status for
    client id and evt regs api's

  PARAMETERS
    status  : return status
    cnf     : type of cnf response
    cnf_ptr : response data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_mmgsdi_response_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  qmi_cat_message_type  *msg_ptr = NULL;

  if (status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_2("qmi_cat_mmgsdi_response_cb(): MMGSDI op failed, status=0x%x, cnf_type=0x%x",
                  status, cnf_type);
    return;
  }

  if (NULL == cnf_ptr)
  {
    return;
  }

  switch(cnf_type)
  {
    case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF:
      if (MMGSDI_SUCCESS != cnf_ptr->client_id_and_evt_reg_cnf.response_header.mmgsdi_status)
      {
        UIM_MSG_ERR_0("qmi_cat_mmgsdi_response_cb(): Client id and evt registration failed");
        break;
      }
      msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_MMGSDI_CNF);
      if (msg_ptr)
      {
        (void)memscpy(&msg_ptr->data.mmgsdi_cnf.cnf_value.client_id_and_evt_reg_cnf,
                      sizeof(mmgsdi_client_id_and_evt_reg_cnf_type),
                      &cnf_ptr->client_id_and_evt_reg_cnf,
                      sizeof(mmgsdi_client_id_and_evt_reg_cnf_type));
        msg_ptr->data.mmgsdi_cnf.cnf_type = cnf_type;
        qmi_cat_post_message(msg_ptr);
      }
      break;

    default:
      break;
  }
} /* qmi_cat_mmgsdi_response_cb */


/*===========================================================================
FUNCTION QMI_CAT_MMGSDI_EVT_CB

DESCRIPTION
  Callback function to return SIM card status.

RETURN VALUE
  void.

DEPENDENCIES

===========================================================================*/
static void qmi_cat_mmgsdi_evt_cb
(
    const mmgsdi_event_data_type   *mmgsdi_event
)
{
  qmi_cat_message_type     *msg_ptr    = NULL;
  mmgsdi_slot_id_enum_type  slot_id    = MMGSDI_MAX_SLOT_ID_ENUM;
  
  ASSERT(qmi_cati_global_ptr);

  if (NULL == mmgsdi_event)
  {
    return;
  }

  UIM_MSG_HIGH_1("qmi_cat_mmgsdi_evt_cb: evt=0x%x", mmgsdi_event->evt);

  /* Clear globals on recovery, UICC_RESET, hot swap, card power cycle */
  switch (mmgsdi_event->evt)
  {
    case MMGSDI_CARD_INSERTED_EVT:
      slot_id = mmgsdi_event->data.card_inserted.slot;
      break;
    case MMGSDI_CARD_ERROR_EVT:
      slot_id = mmgsdi_event->data.card_error.slot;
      break;
    default:
      break;
  }

  msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_MMGSDI_EVT);
  if(msg_ptr)
  {
    msg_ptr->data.mmgsdi_evt.evt  = mmgsdi_event->evt;
    msg_ptr->data.mmgsdi_evt.slot = slot_id;
    qmi_cat_post_message(msg_ptr);
  }
} /* qmi_cat_mmgsdi_evt_cb */


/*===========================================================================
FUNCTION: QMI_CAT_SEND_BUFFERED_RAW_PROACTIVE_CMD_BY_INDEX

DESCRIPTION:
  QMI CAT send buffered commands at the given index to the control point that
  registers for the event

PARAMETERS:
  slot_index  : Index to card slot
  cache_index : Index to raw cache

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
static void qmi_cat_send_buffered_raw_proactive_cmd_by_index
(
  uint8  slot_index,
  uint8  cache_index
)
{
  qmi_cat_message_type     *msg_ptr   = NULL;
  qmi_cati_raw_cache_type  *cache_ptr = NULL;

  ASSERT(qmi_cati_global_ptr);

  if (slot_index  >= QMI_CAT_MAX_CARD_COUNT ||
      slot_index  >= qmi_cati_global_ptr->slot_count ||
      cache_index >= CATI_MAX_RAW_CACHED_EVTS)
  {
    return;
  }

  cache_ptr = qmi_cati_global_ptr->raw_cache[slot_index][cache_index];
  if (NULL == cache_ptr)
  {
    return;
  }

  msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_RAW_PROACTIVE_COMMAND);
  if(NULL == msg_ptr)
  {
    return;
  }

  msg_ptr->data.raw_proactive_cmd.slot_id =
      (gstk_slot_id_enum_type)(GSTK_SLOT_1 + slot_index);
  msg_ptr->data.raw_proactive_cmd.cmd_ref_id =
      cache_ptr->cached_event.cati_evt_rpt.cati_raw_pc_evt.cmd_ref_id;
  msg_ptr->data.raw_proactive_cmd.raw_data_len =
      cache_ptr->cached_event.cati_evt_rpt.cati_raw_pc_evt.raw_data_len;
  (void)memscpy(msg_ptr->data.raw_proactive_cmd.raw_data_buffer,
                sizeof(msg_ptr->data.raw_proactive_cmd.raw_data_buffer),
                cache_ptr->cached_event.cati_evt_rpt.cati_raw_pc_evt.raw_data,
                (uint32)msg_ptr->data.raw_proactive_cmd.raw_data_len);
  msg_ptr->data.raw_proactive_cmd.is_cached_cmd = TRUE;
  
  qmi_cat_post_message(msg_ptr);

  /* Mark cache as invalid */
  QMI_CAT_FREE_IF_NOT_NULL(qmi_cati_global_ptr->raw_cache[slot_index][cache_index]);
}/* qmi_cat_send_buffered_raw_proactive_cmd_by_index */


/*===========================================================================
FUNCTION: QMI_CAT_SEND_RECOVERY_CACHE_PROACTIVE_CMD_BY_INDEX

DESCRIPTION:
  QMI CAT send recovery cache commands at the given index to the control point
  that registers for the event

PARAMETERS:
  slot_index  : Index to card slot
  cache_index : Index to raw cache

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
static void qmi_cat_send_recovery_cache_proactive_cmd_by_index
(
  uint8  slot_index,
  uint8  cache_index
)
{
  qmi_cat_message_type     *msg_ptr   = NULL;
  qmi_cati_raw_cache_type  *cache_ptr = NULL;

  ASSERT(qmi_cati_global_ptr);

  if (slot_index  >= QMI_CAT_MAX_CARD_COUNT ||
      slot_index  >= qmi_cati_global_ptr->slot_count ||
      cache_index >= CATI_MAX_RAW_QMI_CLIENT_CACHED_EVTS)
  {
    return;
  }

  cache_ptr = qmi_cati_global_ptr->recovery_raw_cache[slot_index][cache_index];
  if (NULL == cache_ptr)
  {
    return;
  }

  msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_RAW_PROACTIVE_COMMAND);
  if(NULL == msg_ptr)
  {
    return;
  }

  msg_ptr->data.raw_proactive_cmd.slot_id =
      (gstk_slot_id_enum_type)(GSTK_SLOT_1 + slot_index);
  msg_ptr->data.raw_proactive_cmd.cmd_ref_id =
      cache_ptr->cached_event.cati_evt_rpt.cati_raw_pc_evt.cmd_ref_id;
  msg_ptr->data.raw_proactive_cmd.raw_data_len =
      cache_ptr->cached_event.cati_evt_rpt.cati_raw_pc_evt.raw_data_len;
  (void)memscpy(msg_ptr->data.raw_proactive_cmd.raw_data_buffer,
                sizeof(msg_ptr->data.raw_proactive_cmd.raw_data_buffer),
                cache_ptr->cached_event.cati_evt_rpt.cati_raw_pc_evt.raw_data,
                (uint32)msg_ptr->data.raw_proactive_cmd.raw_data_len);
  msg_ptr->data.raw_proactive_cmd.is_cached_cmd = TRUE;

  qmi_cat_post_message(msg_ptr);
}/* qmi_cat_send_recovery_cache_proactive_cmd_by_index */


/*===========================================================================
FUNCTION: QMI_CAT_SEND_BUFFERED_RAW_PROACTIVE_CMD

DESCRIPTION:
  QMI CAT send buffered commands when the control point registers for these
  later

PARAMETERS:
  mask : [Input] mask for sending the buffered command

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
static void qmi_cat_send_buffered_raw_proactive_cmd
(
  uint32 mask,
  uint8  card_index
)
{
  ASSERT(qmi_cati_global_ptr);

  if (card_index  >= QMI_CAT_MAX_CARD_COUNT ||
      card_index  >= qmi_cati_global_ptr->slot_count)
  {
    return;
  }

  /* If there is a cached Setup menu not sent yet, Queue this proactive command
     else if TR is pending send the recovery cached command */
  if (mask & CATI_REPORT_STATUS_EV_SETUP_MENU)
  {
    if(qmi_cati_global_ptr->raw_cache[card_index][CATI_CACHED_EVT_SETUP_MENU] != NULL)
    {
      qmi_cat_send_buffered_raw_proactive_cmd_by_index(card_index,
                                                       CATI_CACHED_EVT_SETUP_MENU);
    }
    else if(qmi_cati_global_ptr->last_gstk_cmd[card_index] == GSTK_SETUP_MENU_REQ)
    {
      qmi_cat_send_recovery_cache_proactive_cmd_by_index(card_index,
                                                         CATI_QMI_CLIENT_CACHED_EVT_SETUP_MENU);
    }
  }

  /* If there is a cached Display text not sent yet,
     Queue this proactive command for DS task */
  if (mask & CATI_REPORT_STATUS_EV_DISPLAY_TEXT)
  {
    qmi_cat_send_buffered_raw_proactive_cmd_by_index(card_index,
                                                     CATI_CACHED_EVT_DISPLAY_TEXT);
  }

  /* If there is a cached Setup Idle Mode Text not sent yet, Queue this proactive command
     else if TR is pending send the recovery cached command */
  if (mask & CATI_REPORT_STATUS_EV_SETUP_IDL_MODE_TEXT)
  {
    if(qmi_cati_global_ptr->raw_cache[card_index][CATI_CACHED_EVT_SETUP_IDLE_MODE_TEXT] != NULL)
    {
      qmi_cat_send_buffered_raw_proactive_cmd_by_index(card_index,
                                                       CATI_CACHED_EVT_SETUP_IDLE_MODE_TEXT);
    }
    else if(qmi_cati_global_ptr->last_gstk_cmd[card_index] == GSTK_SETUP_IDLE_TEXT_REQ)
    {
      qmi_cat_send_recovery_cache_proactive_cmd_by_index(card_index,
                                                         CATI_QMI_CLIENT_CACHED_EVT_SETUP_IDLE_MODE_TEXT);
    }
  }

  /* If there is a cached Setup Event List not sent yet, Queue this proactive command
     else if TR is pending send the recovery cached command  */
  if (mask & CATI_SETUP_EVT_LIST_MASK)
  {
    if(qmi_cati_global_ptr->raw_cache[card_index][CATI_CACHED_EVT_SETUP_EVT_LIST] != NULL)
    {
      qmi_cat_send_buffered_raw_proactive_cmd_by_index(card_index,
                                                       CATI_CACHED_EVT_SETUP_EVT_LIST);
    }
    else if(qmi_cati_global_ptr->last_gstk_cmd[card_index] == GSTK_SETUP_EVENT_LIST_REQ)
    {
      qmi_cat_send_recovery_cache_proactive_cmd_by_index(card_index,
                                                         CATI_QMI_CLIENT_CACHED_EVT_SETUP_EVT_LIST);
    }
  }

  /* If there is a cached PLI not sent yet,
     Queue this proactive command for DS task */
  if (mask & CATI_PROVIDE_LOCAL_INFO_MASK)
  {
    qmi_cat_send_buffered_raw_proactive_cmd_by_index(card_index,
                                                     CATI_CACHED_EVT_PROVIDE_LOCAL_INFO);
  }

  /* If there is a cached GET INPUT not sent yet,
     Queue this proactive CATI_REPORT_STATUS_EV_GET_INPUT for DS task */
  if (mask & CATI_REPORT_STATUS_EV_GET_INPUT)
  {
    qmi_cat_send_buffered_raw_proactive_cmd_by_index(card_index,
                                                     CATI_CACHED_EVT_GET_INPUT);
  }
} /* qmi_cat_send_buffered_raw_proactive_cmd */


/*===========================================================================
  FUNCTION QMI_CATI_EVENT_REPORT_IND()

  DESCRIPTION
    Is called when any of the conditions set in set_event_report becomes true
    Sends an indication to the client

  PARAMETERS
    cl_sp          : client state pointer
    event_info_ptr : pointer to the event report

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cati_event_report_ind
(
  qmi_client_handle    client_handle,
  const qmi_cati_evt_rpt_info_s_type * event_info_ptr
)
{
  cat_event_report_ind_msg_v02 *event_report_ind_ptr = NULL;
  
  ASSERT(event_info_ptr);
  ASSERT(qmi_cati_global_ptr);

  event_report_ind_ptr = uimqmi_malloc(sizeof(cat_event_report_ind_msg_v02));
  if(event_report_ind_ptr == NULL)
  {
    return;
  }

  memset(event_report_ind_ptr, 0, sizeof(cat_event_report_ind_msg_v02));

  if (event_info_ptr->mask &  CATI_RAW_PROACTIVE_CMD_MASK ||
           event_info_ptr->mask &  CATI_PROVIDE_LOCAL_INFO_MASK)
  {
    uint8  tlv_id = 0;

    /* Populate evt tlv for raw cmd types */
    tlv_id = cati_conv_raw_mask_to_tlv_id(
                          event_info_ptr->mask,
                          event_info_ptr->cati_evt_rpt.cati_raw_pc_evt.data_evt_type);

    if (QMI_ERR_NONE_V01 != qmi_cat_response_raw_proactive_command(
                              tlv_id,
                              event_report_ind_ptr,
                              event_info_ptr->cati_evt_rpt.cati_raw_pc_evt.cmd_ref_id,
                              event_info_ptr->cati_evt_rpt.cati_raw_pc_evt.raw_data_len,
                              event_info_ptr->cati_evt_rpt.cati_raw_pc_evt.raw_data_ptr,
                              event_info_ptr->slot_id,
                              event_info_ptr->response_type,
                              (uint32)event_info_ptr->mask,
                              event_info_ptr->cati_evt_rpt.cati_raw_pc_evt.data_evt_type))
    {
      UIM_MSG_HIGH_0("Unable to generate CATI Event Report indication for raw proactive cmd!");
      return;
    }
  }
  else if(event_info_ptr->mask & CATI_SETUP_EVT_LIST_MASK)
  {
    if (QMI_ERR_NONE_V01!= qmi_cat_response_raw_proactive_command(
                              CATI_PRM_TYPE_SETUP_EVENT_LIST_RAW,
                              event_report_ind_ptr,
                              event_info_ptr->cati_evt_rpt.cati_raw_pc_evt.cmd_ref_id,
                              event_info_ptr->cati_evt_rpt.cati_raw_pc_evt.raw_data_len,
                              event_info_ptr->cati_evt_rpt.cati_raw_pc_evt.raw_data_ptr,
                              event_info_ptr->slot_id,
                              event_info_ptr->response_type,
                              (uint32)event_info_ptr->mask,
                              event_info_ptr->cati_evt_rpt.cati_raw_pc_evt.data_evt_type))
    {
      UIM_MSG_HIGH_0("Unable to generate CATI Event Report indication for Setup Evt list!");
      return;
    }
  }
  else if(event_info_ptr->mask & CATI_REPORT_STATUS_EV_END_PROACTIVE_SESSION)
  {
    memset(event_report_ind_ptr, 0, sizeof(cat_event_report_ind_msg_v02));

    event_report_ind_ptr->proactive_session_end_type.proactive_session_end_type = event_info_ptr->cati_evt_rpt.cati_end_proactive_cmd_evt.proactive_session_end_type;
    event_report_ind_ptr->proactive_session_end_type_valid = TRUE;

    switch(event_info_ptr->slot_id)
    {
      case GSTK_SLOT_1:
        event_report_ind_ptr->slot_valid = TRUE;
        event_report_ind_ptr->slot.slot = 1;
        break;
      case GSTK_SLOT_2:
        event_report_ind_ptr->slot_valid = TRUE;
        event_report_ind_ptr->slot.slot = 2;
        break;
      case GSTK_SLOT_3:
        event_report_ind_ptr->slot_valid = TRUE;
        event_report_ind_ptr->slot.slot = 3;
        break;
      default:
        break;
    }
  }
  else
  {
    UIM_MSG_ERR_0("Should not be here ... unsupported mask in qmi_cati_event_report_ind!");
    return;
  }

  (void)qmi_cat_send_response_ind( 
          NULL,
          event_report_ind_ptr,
          QMI_CAT_EVENT_REPORT_IND_V02,
          sizeof(cat_event_report_ind_msg_v02),
          TRUE,
          client_handle);
  
  uimqmi_free(event_report_ind_ptr);
    
} /* qmi_cati_event_report_ind */


/*===========================================================================
FUNCTION qmi_cati_find_length_of_length_value

DESCRIPTION
  This function determines the size of the length field:
  first byte of length field [0x00, 0x7F] -> length field size == 1
  first byte of length field == 0x81 -> length field size == 2

PARAMETERS
  length_value: [Input] pointer to the length value field

DEPENDENCIES
  None

RETURN VALUE
  byte -> length of the length field in TLV

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static uint8 qmi_cati_find_length_of_length_value
(
  const uint8 * length_value
)
{
  uint8 ret_val = 0;

  if (NULL == length_value)
  {
    return ret_val;
  }

  UIM_MSG_LOW_1(" == IN qmi_cati_find_length_of_length_value, length: 0x%x", length_value[0]);

  /* This length field can either be 0x00-0x7F or
     0x81 if the 2nd byte is used */
  if(length_value[0] <= 0x7F)
  {
    ret_val = 1;
  }
  else if(length_value[0] == 0x81)
  {
    ret_val = 2;
  }
  return ret_val;
} /* qmi_cati_find_length_of_length_value */


/*===========================================================================
FUNCTION qmi_cati_send_raw_term_response

DESCRIPTION
  This function sends the raw Term Response

PARAMETERS
  mask: Input mask specifying the event
  cmd_ref_id :

DEPENDENCIES
  None

RETURN VALUE
  byte -> length of the length field in TLV

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type qmi_cati_send_raw_term_response
(
  gstk_slot_id_enum_type         slot_id,
  uint32                         cmd_ref_id,
  int32                          raw_data_len,
  const uint8*                   raw_data_ptr,
  gstk_general_result_enum_type  result
)
{
  int32                  length_of_TLV_length_field = 0;
  gstk_status_enum_type  gstk_status                = GSTK_SUCCESS;
  gstk_generic_data_type term_resp                  = {0};
  uint8                  slot_index                 = 0;

  /* Struct used for the raw msg type */
  PACKED struct PACKED_POST
  {
    uint8     command_details[CATI_CMD_DETAILS_LEN];
    uint8     device_id_tag;
    uint8     device_tag_length;
    uint8     source_device;
    uint8     target_device;
    uint8     result_tag;
    uint8     result_tag_length;
    uint8     result;
    uint8     addl_result;
  }STK_response;

  ASSERT(qmi_cati_global_ptr);

  memset(&STK_response, 0, sizeof(STK_response));

  if (raw_data_len < 1 || raw_data_ptr == NULL)
  {
    UIM_MSG_HIGH_2("Invalid raw length (0x%x) or pointer (0x%x)",
                   raw_data_len, raw_data_ptr);
    return GSTK_ERROR;
  }

  /* Retrieve the slot index */
  if( qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(slot_id,
                                                      &slot_index) != QMI_ERR_NONE_V01)
  {
    UIM_MSG_ERR_1("qmi_cati_send_term_response: fail to covert slot_id, 0x%x",
                  slot_id);
    return GSTK_ERROR;
  }

  /* Populate the generic successful resp here for the cached cmds */
  length_of_TLV_length_field = qmi_cati_find_length_of_length_value(raw_data_ptr + 1);

  if(length_of_TLV_length_field != 1 &&
     length_of_TLV_length_field != 2)
  {
    /* Invalid BER TLV len , ignore the command */
    UIM_MSG_MED_1(" ERROR:  Length of TLV Invalid: 0x%x",
                  length_of_TLV_length_field);
    return GSTK_ERROR;
  }

  if (raw_data_len < length_of_TLV_length_field + 1 + (int32) sizeof(STK_response.command_details))
  {
    UIM_MSG_HIGH_1("Invalid raw length: 0x%x", raw_data_len);
    return GSTK_ERROR;
  }

  /* 4th location after total length field is the type of command
  ** 0   Proactive SIM Command Tag
  ** 1-2 total length
  ** +1  Command Details tag
  ** +1  Command details len = 3
  ** +1  Command number
  ** +1  Command Type
  ** +1  Command Qualifier */

  /* Pack various TLVs */
  /* Command details */
  (void)memscpy((void *) STK_response.command_details,
                sizeof(STK_response.command_details),
                (raw_data_ptr + length_of_TLV_length_field + 1),
                sizeof(STK_response.command_details));

  /* device ID */
  STK_response.device_id_tag     = 0x02; /* GSTK_DEVICE_IDENTITY_TAG */
  STK_response.device_tag_length = 0x02; /* GSTK_DEVICE_IDENTITY_LEN */
  STK_response.source_device     = 0x82; /* GSTK_ME_DEVICE */
  STK_response.target_device     = 0x81; /* GSTK_UICC_SIM_DEVICE */

  /* result tag */
  STK_response.result_tag        = 0x03; /* GSTK_RESULT_TAG */
  STK_response.result_tag_length = (result == GSTK_COMMAND_PERFORMED_SUCCESSFULLY) ? 1: 2;
  STK_response.result            = (uint8)result;

  /* total data length:  command details data + 2   (2 is for the tag and length field)
                      + device id data + 2
                      + result data + 2
  */

  /* Fill in the term_resp now for the  raw format api */
  term_resp.data_len = (result == GSTK_COMMAND_PERFORMED_SUCCESSFULLY) ?
                         (sizeof(STK_response) - sizeof(STK_response.addl_result)) :
                         sizeof(STK_response);
  term_resp.data_buffer_ptr = (uint8 *)&STK_response;

  gstk_status = gstk_send_raw_terminal_response (qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                                                  0,
                                                  cmd_ref_id,
                                                  term_resp );

  if(GSTK_SUCCESS != gstk_status)
  {
    UIM_MSG_ERR_1("qmi_cati_send_term_response: tr fail, 0x%x", gstk_status);
  }

  return gstk_status;
}/* qmi_cati_send_raw_term_response */


/*===========================================================================
FUNCTION: qmi_cat_cleanup_globals

DESCRIPTION:
  This function is used to free the qmicat globals

PARAMETERS:
  card_index: slot index

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
static void qmi_cat_cleanup_globals
(
  uint8 card_index
)
{
  uint8                  cache_index     = 0;

  qmi_cati_global_ptr->last_gstk_cmd[card_index]                 = GSTK_END_PROACTIVE_CMD_REQ;
  qmi_cati_global_ptr->ref_command_id[card_index]                = 0;
  qmi_cati_global_ptr->full_func_evt[card_index]                 = FALSE;
  qmi_cati_global_ptr->decoded_setup_evt_in_progress[card_index] = 0;
  qmi_cati_global_ptr->card_state[card_index]                    = QMI_CAT_CARD_STATE_ERROR;

  /* Clear QMI cache*/
  for (cache_index = 0; cache_index < QMI_CATI_MAX_BIP_CHANNEL; cache_index++)
  {
    qmi_cati_global_ptr->bip_status_cache[card_index][cache_index] = CATI_BIP_SESSION_END;
  }

  for(cache_index = 0; cache_index < CATI_MAX_RAW_CACHED_EVTS; cache_index++)
  {
    if(qmi_cati_global_ptr->raw_cache[card_index][cache_index] != NULL)
    {
      uimqmi_free(qmi_cati_global_ptr->raw_cache[card_index][cache_index]);
      qmi_cati_global_ptr->raw_cache[card_index][cache_index] = NULL;
    }
  }

  for(cache_index = 0; cache_index < CATI_MAX_RAW_QMI_CLIENT_CACHED_EVTS; cache_index++)
  {
    if(qmi_cati_global_ptr->recovery_raw_cache[card_index][cache_index] != NULL)
    {
      uimqmi_free(qmi_cati_global_ptr->recovery_raw_cache[card_index][cache_index]);
      qmi_cati_global_ptr->recovery_raw_cache[card_index][cache_index] = NULL;
    }
  }

  for(cache_index = 0; cache_index < QMI_CATI_MAX_SCWS_CHANNEL; cache_index++)
  {
    qmi_cati_cmd_buf_cache_type *current_node_ptr = NULL;

    current_node_ptr = qmi_cati_global_ptr->scws_cmd_buf[card_index][cache_index];
    while (current_node_ptr != NULL)
    {
      qmi_cati_cmd_buf_cache_type *next_node_ptr = NULL;

      next_node_ptr = current_node_ptr->next_ptr;
      qmi_cat_free_cb_userdata((qmi_cati_userdata_type *)current_node_ptr->cb_userdata_ptr);
      uimqmi_free(current_node_ptr);
      current_node_ptr = next_node_ptr;
    }
    qmi_cati_global_ptr->scws_cmd_buf[card_index][cache_index] = NULL;
  }
} /* qmi_cat_cleanup_globals */


/*===========================================================================
  FUNCTION  QMI_CAT_CACHE_RAW_PROACTIVE_COMMAND()

  DESCRIPTION
    Store proactive command in QMI CAT regular or recovery raw cache

  PARAMETERS
    cat_msg_ptr       : Pointer to qmi_cat_message_type
    command_id        : Command id, e.g. GSTK_CMD_STK_SET_UP_MENU
    slot_index        : Slot on which the command is for
    is_recovery_cache : is the proactive command to be stored in recovery cache

  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_type_v01 qmi_cat_cache_raw_proactive_command
(
  const qmi_cat_message_type  *cat_msg_ptr,
  uint8                        command_id,
  uint8                        slot_index,
  boolean                      is_recovery_cache
)
{
  uint8                      i              = 0;
  qmi_cati_raw_cache_type  **raw_cache_pptr = NULL;

  UIM_MSG_HIGH_3("qmi_cat_cache_raw_proactive_command: command_id=0x%x, slot_index=0x%x, is_recovery_cache=0x%x",
                 command_id, slot_index, is_recovery_cache);

  if (NULL == cat_msg_ptr || NULL == qmi_cati_global_ptr)
  {
    UIM_MSG_ERR_0("NULL cat_msg_ptr or qmi_cati_global_ptr");
    return QMI_ERR_INTERNAL_V01;
  }

  if (slot_index >= QMI_CAT_MAX_CARD_COUNT ||
      slot_index >= qmi_cati_global_ptr->slot_count)
  {
    UIM_MSG_ERR_1("Invalid slot_index: 0x%x", slot_index);
    return QMI_ERR_INTERNAL_V01;
  }

  if (cat_msg_ptr->message_type != QMI_CAT_MESSAGE_RAW_PROACTIVE_COMMAND)
  {
    UIM_MSG_ERR_1("Incorrect message_type: 0x%x", cat_msg_ptr->message_type);
    return QMI_ERR_INTERNAL_V01;
  }

  if (qmi_cati_global_ptr->card_state[slot_index] == QMI_CAT_CARD_STATE_INVALID ||
      qmi_cati_global_ptr->card_state[slot_index] == QMI_CAT_CARD_STATE_ERROR)
  {
    UIM_MSG_ERR_1("Incorrect card state: 0x%x", qmi_cati_global_ptr->card_state[slot_index]);
    return QMI_ERR_INTERNAL_V01;
  }

  if (FALSE == is_recovery_cache)
  {
    for (i = 0;
         i < (sizeof(qmi_cat_raw_cache_cmd_index)/sizeof(qmi_cat_raw_cache_cmd_index[0]));
         i++)
    {
      if (qmi_cat_raw_cache_cmd_index[i].command_id == command_id)
      {
        raw_cache_pptr =
          &qmi_cati_global_ptr->raw_cache[slot_index][qmi_cat_raw_cache_cmd_index[i].cache_index];
        break;
      }
    }
  }
  else
  {
    for (i = 0;
         i < (sizeof(qmi_recovery_cat_raw_cache_cmd_index)/sizeof(qmi_recovery_cat_raw_cache_cmd_index[0]));
         i++)
    {
      if (qmi_recovery_cat_raw_cache_cmd_index[i].command_id == command_id)
      {
        raw_cache_pptr =
          &qmi_cati_global_ptr->recovery_raw_cache[slot_index][qmi_recovery_cat_raw_cache_cmd_index[i].cache_index];
        break;
      }
    }
  }

  if (NULL == raw_cache_pptr)
  {
    return QMI_ERR_INTERNAL_V01;
  }

  /* Check raw data len */
  if (cat_msg_ptr->data.raw_proactive_cmd.raw_data_len > CAT_RAW_MAX_DATA_LEN)
  {
    /* Free and invalidate the existing cache entry if not NULL */
    QMI_CAT_FREE_IF_NOT_NULL(*raw_cache_pptr);
    return QMI_ERR_ARG_TOO_LONG_V01;
  }

  /* Cache the proactive command */
  if (NULL == *raw_cache_pptr)
  {
    *raw_cache_pptr =
      (qmi_cati_raw_cache_type *)uimqmi_malloc(sizeof(qmi_cati_raw_cache_type));
    if (NULL == *raw_cache_pptr)
    {
      return QMI_ERR_NO_MEMORY_V01;
    }
  }
  else
  {
    memset(*raw_cache_pptr,
           0x00,
           sizeof(**raw_cache_pptr));
  }

  (*raw_cache_pptr)->cached_event.cati_evt_rpt.cati_raw_pc_evt.cmd_ref_id =
      cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id;
  (*raw_cache_pptr)->cached_event.cati_evt_rpt.cati_raw_pc_evt.raw_data_len =
    (uint16)cat_msg_ptr->data.raw_proactive_cmd.raw_data_len;
  (void)memscpy((*raw_cache_pptr)->cached_event.cati_evt_rpt.cati_raw_pc_evt.raw_data,
                sizeof((*raw_cache_pptr)->cached_event.cati_evt_rpt.cati_raw_pc_evt.raw_data),
                cat_msg_ptr->data.raw_proactive_cmd.raw_data_buffer,
                (uint32)cat_msg_ptr->data.raw_proactive_cmd.raw_data_len);

  return QMI_ERR_NONE_V01;
}/* qmi_cat_cache_raw_proactive_command */


/*===========================================================================
  FUNCTION  QMI_CAT_PROCESS_RAW_PROACTIVE_COMMAND()

  DESCRIPTION
    To process the proactive command from GSTK in QMI context

  PARAMETERS
    cmd_ref_id   : Reference ID unique to command
    raw_data_len : Length of raw command data
    raw_data_ptr : Raw command data buffer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_raw_proactive_command
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  qmi_cati_client_state_type * cl_sp                           = NULL;
  uint8                        i                               = 0;
  uint8                        type_of_command                 = 0;
  uint8                        length_of_TLV_length_field      = 1;
  gstk_status_enum_type        gstk_status                     = GSTK_SUCCESS;
  qmi_cati_evt_rpt_info_s_type cati_event                      = {0};
  boolean                      ind_sent                        = FALSE;
  uint8                        slot_index                      = 0;

  ASSERT(cat_msg_ptr != NULL);
  ASSERT(qmi_cati_global_ptr);
  ASSERT(qmi_cat_state_ptr);

  /* Retrieve the slot index */
  if(qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                                    cat_msg_ptr->data.raw_proactive_cmd.slot_id,
                                    &slot_index) != QMI_ERR_NONE_V01)
  {
    UIM_MSG_ERR_1("Invalid slot id for proactive command: 0x%x",
                  cat_msg_ptr->data.raw_proactive_cmd.slot_id);
    return;
  }

  /* Set slot id in the cat event */
  cati_event.slot_id = cat_msg_ptr->data.raw_proactive_cmd.slot_id;

  /* Set response_type */
  cati_event.response_type = cat_msg_ptr->data.raw_proactive_cmd.raw_hdr_tag;

  /* BIP related commands require raw mask and data_evt_type to determine for
     tlv_id*/
  cati_event.cati_evt_rpt.cati_raw_pc_evt.data_evt_type = CATI_DATA_EVENT_NONE;

  /* Find the BER-TLV Length: This length field can either be 0x00-0x7F
     or 0x81 if the 2nd byte is used */
  length_of_TLV_length_field =
            qmi_cati_find_length_of_length_value(cat_msg_ptr->data.raw_proactive_cmd.raw_data_buffer + 1);

  if ((1 != length_of_TLV_length_field) && (2 != length_of_TLV_length_field))
  {
    /* Invalid BER TLV len, ignore the command */
    UIM_MSG_MED_1(" ERROR:  Length of TLV Invalid: 0x%x",
                  length_of_TLV_length_field);
    return;
  }

  /* 4th location after total length field is the type of command:
     0   Proactive SIM Command Tag
     1-2 Total length
     +1  Command Details tag
     +1  Command details len = 3
     +1  Command number
     +1  Command Type
     +1  Command Qualifier */
  type_of_command = *(cat_msg_ptr->data.raw_proactive_cmd.raw_data_buffer + length_of_TLV_length_field + 4);

  /* For newly received proactive commands, set last_gstk_cmd, ref_command_id,
     and full_func_evt so that those info can be used later to correctly track
     whether a TR/Event should be rejected or allowed.
     last_gstk_cmd, ref_command_id, and full_func_evt are cleared when proactive
     session ends
     Note:
       last_gstk_cmd, ref_command_id, and full_func_evt should be in sync with
       the ongoing UICC proactive command, therefore, for proactive commands
       resent from cache, do not change any of the variables to avoid incorrectly
       set the variables when the processing of the related UICC proactive command
       has already ended.
   */
  if (FALSE == cat_msg_ptr->data.raw_proactive_cmd.is_cached_cmd)
  {
    /* Set last_gstk_cmd */
    if (QMI_ERR_NONE_V01 != qmi_cati_covert_stk_cmd_id_to_gstk_cmd_id(
                            type_of_command,
                            &(qmi_cati_global_ptr->last_gstk_cmd[slot_index])))
    {
      return;
    }

    /* Set full_func_evt for proactive commands that support event routing if
       this is a new UICC proactive command */
    if (qmi_cati_global_ptr->ref_command_id[slot_index] !=
        cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id)
    {
      switch (type_of_command)
      {
        case GSTK_CMD_STK_SEND_SHORT_MESSAGE:
          qmi_cati_global_ptr->full_func_evt[slot_index] =
            (qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index] &
             CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SMS)? TRUE:FALSE;
          break;

        case GSTK_CMD_STK_SET_UP_CALL:
          qmi_cati_global_ptr->full_func_evt[slot_index] =
            (qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index] &
             CATI_REPORT_STATUS_EV_FULL_FUNC_SETUP_CALL)? TRUE:FALSE;
          break;

        case GSTK_CMD_STK_SEND_DTMF:
          qmi_cati_global_ptr->full_func_evt[slot_index] =
            (qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index] &
             CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_DTMF)? TRUE:FALSE;
          break;
          
        case GSTK_CMD_STK_SEND_SS:
          qmi_cati_global_ptr->full_func_evt[slot_index] =
            (qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index] &
             CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS)? TRUE:FALSE;
          break;

        case GSTK_CMD_STK_USSD:
          qmi_cati_global_ptr->full_func_evt[slot_index] =
            (qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index] &
             CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS)? TRUE:FALSE;
          break;

        default:
          break;
      }
    }

    /* Set cmd_ref_id */
    qmi_cati_global_ptr->ref_command_id[slot_index] = cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id;
  }

  /* Store supported proactive commands in recovery cache */
  if (CATI_CONFIG_MODE_ANDROID    == qmi_cati_global_ptr->config_mode ||
      CATI_CONFIG_MODE_CUSTOM_RAW == qmi_cati_global_ptr->config_mode)
  {
    (void)qmi_cat_cache_raw_proactive_command(cat_msg_ptr,
                                              type_of_command,
                                              slot_index,
                                              TRUE);
  }

  switch(type_of_command)
  {
    case GSTK_CMD_STK_DISPLAY_TEXT:
      cati_event.mask = CATI_REPORT_STATUS_EV_DISPLAY_TEXT;

      /* Check if any client is registered for handling this event */
      if (!(qmi_cati_global_ptr->evt_reg_mask[slot_index] & CATI_REPORT_STATUS_EV_DISPLAY_TEXT))
      {
        /* If no client reg for this save it for future and send it from the cache
           Fall through if caching failed so that failure TR can be sent back to GSTK
         */
        if (QMI_ERR_NONE_V01 == qmi_cat_cache_raw_proactive_command(cat_msg_ptr,
                                                                type_of_command,
                                                                slot_index,
                                                                FALSE))
        {
          return;
        }
      }
      else
      {
        /* Free and Invalidate the cache for the evt to be sure */
        QMI_CAT_FREE_IF_NOT_NULL(qmi_cati_global_ptr->raw_cache[slot_index][CATI_CACHED_EVT_DISPLAY_TEXT]);
      }
      break;

    case GSTK_CMD_STK_GET_INKEY:
      cati_event.mask = CATI_REPORT_STATUS_EV_GET_INKEY;
      break;

    case GSTK_CMD_STK_GET_INPUT:
      cati_event.mask = CATI_REPORT_STATUS_EV_GET_INPUT;

     /* Check if any client is registered for handling this event */
      if(!(qmi_cati_global_ptr->evt_reg_mask[slot_index] & CATI_REPORT_STATUS_EV_GET_INPUT) &&
         (qmi_cati_global_ptr->config_mode != CAT_CONFIG_MODE_GOBI_V02))
      {
        /* If no client reg for this save it for future and send it from the cache
           Fall through if caching failed so that failure TR can be sent back to GSTK
         */
        if (QMI_ERR_NONE_V01 == qmi_cat_cache_raw_proactive_command(cat_msg_ptr,
                                                                type_of_command,
                                                                slot_index,
                                                                FALSE))
        {
          return;
        }
      }
      else
      {
        /* Free and Invalidate the cache for the evt to be sure */
        QMI_CAT_FREE_IF_NOT_NULL(qmi_cati_global_ptr->raw_cache[slot_index][CATI_CACHED_EVT_GET_INPUT]);
      }
      break;

    case GSTK_CMD_STK_SET_UP_MENU:
      cati_event.mask = CATI_REPORT_STATUS_EV_SETUP_MENU;

      /* Check if any client is registered for handling this event */
      if (!(qmi_cati_global_ptr->evt_reg_mask[slot_index] & CATI_REPORT_STATUS_EV_SETUP_MENU))
      {
        /* If no client reg for this save it for future and send it from the cache
           Fall through if caching failed so that failure TR can be sent back to GSTK
         */
        if (QMI_ERR_NONE_V01 == qmi_cat_cache_raw_proactive_command(cat_msg_ptr,
                                                                type_of_command,
                                                                slot_index,
                                                                FALSE))
        {
          return;
        }
      }
      else
      {
        /* Free and Invalidate the cache for the evt to be sure */
        QMI_CAT_FREE_IF_NOT_NULL(qmi_cati_global_ptr->raw_cache[slot_index][CATI_CACHED_EVT_SETUP_MENU]);
      }
      break;

    case GSTK_CMD_STK_SELECT_ITEM:
      cati_event.mask = CATI_REPORT_STATUS_EV_SELECT_ITEM;
      break;

    case GSTK_CMD_STK_SEND_SHORT_MESSAGE:
      cati_event.mask = CATI_REPORT_STATUS_EV_SENDSMS_ALPHA_ID;
      break;

    case GSTK_CMD_STK_SET_UP_IDLE_MODE_TEXT:
      cati_event.mask = CATI_REPORT_STATUS_EV_SETUP_IDL_MODE_TEXT;

      /* Check if any client is registered for handling this event */
      if (!(qmi_cati_global_ptr->evt_reg_mask[slot_index] & CATI_REPORT_STATUS_EV_SETUP_IDL_MODE_TEXT))
      {
        /* If no client reg for this save it for future and send it from the cache
           Fall through if caching failed so that failure TR can be sent back to GSTK
         */
        if (QMI_ERR_NONE_V01 == qmi_cat_cache_raw_proactive_command(cat_msg_ptr,
                                                                type_of_command,
                                                                slot_index,
                                                                FALSE))
        {
          return;
        }
      }
      else
      {
        /* Free and Invalidate the cache for the evt to be sure */
        QMI_CAT_FREE_IF_NOT_NULL(qmi_cati_global_ptr->raw_cache[slot_index][CATI_CACHED_EVT_SETUP_IDLE_MODE_TEXT]);
      }
      break;

    case GSTK_CMD_STK_LANG_NOTIFICATION:
      cati_event.mask = CATI_REPORT_STATUS_EV_LANG_NOTIFICATION;
      break;

    case GSTK_CMD_STK_PLAY_TONE:
      cati_event.mask = CATI_REPORT_STATUS_EV_PLAY_TONE;
      break;

    case GSTK_CMD_STK_SET_UP_CALL:
      cati_event.mask = CATI_REPORT_STATUS_EV_SETUP_CALL;

      /* display_alpha_conf can be set by NV
         default values (if NV is not set):
           - ANDROID: CATI_DISPLAY_ALPHA_AUTOMATIC_TRUE
                      -- due to limitation in Android: UI has no knowledge if icon
                      is being displayed, assume icon display successfully.
           - CUSTOM_RAW: CATI_DISPLAY_ALPHA_AUTOMATIC_FALSE
                         -- since WP doesn't support icon display.
           - Others: CATI_DISPLAY_ALPHA_NO_AUTOMATIC_CONF */
      if(cat_msg_ptr->data.raw_proactive_cmd.raw_hdr_tag == GSTK_TAG_DISPLAY_ALPHA
         && qmi_cati_global_ptr->display_alpha_conf != CATI_DISPLAY_ALPHA_NO_AUTOMATIC_CONF)
      {
        gstk_status = gstk_send_icon_display_rsp(
                     qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                     cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id,
                     (qmi_cati_global_ptr->display_alpha_conf == CATI_DISPLAY_ALPHA_AUTOMATIC_TRUE) ? TRUE : FALSE
                     );
        if (gstk_status != GSTK_SUCCESS)
        {
          UIM_MSG_ERR_1("failed to send cmd to GSTK: gstk_status = status %d",
                        gstk_status);
        }

        /* Reset last_gstk_cmd, ref_command_id, full_func_evt */
        qmi_cati_global_ptr->last_gstk_cmd[slot_index]  = GSTK_END_PROACTIVE_CMD_REQ;
        qmi_cati_global_ptr->ref_command_id[slot_index] = 0;
        qmi_cati_global_ptr->full_func_evt[slot_index]  = FALSE;
        return;
      }
      break;

    case GSTK_CMD_STK_SEND_DTMF:
      cati_event.mask = CATI_REPORT_STATUS_EV_SEND_DTMF;
      break;

    case GSTK_CMD_STK_LAUNCH_BROWSER:
      cati_event.mask = CATI_REPORT_STATUS_EV_LAUNCH_BROWSER;
      break;

    case GSTK_CMD_STK_SEND_SS:
      cati_event.mask = CATI_REPORT_STATUS_EV_SEND_SS;
      break;

    case GSTK_CMD_STK_USSD:
      cati_event.mask = CATI_REPORT_STATUS_EV_SEND_USSD;
      break;

    case GSTK_CMD_STK_PROVIDE_LOCAL_INFO:
      cati_event.mask = CATI_PROVIDE_LOCAL_INFO_MASK;

      /* Check if any client is registered for handling this event */
      if(!(qmi_cati_global_ptr->evt_reg_mask[slot_index] & CATI_PROVIDE_LOCAL_INFO_MASK))
      {
        /* If no client reg for this save it for future and send it from the cache
           Fall through if caching failed so that failure TR can be sent back to GSTK
         */
        if (QMI_ERR_NONE_V01 == qmi_cat_cache_raw_proactive_command(cat_msg_ptr,
                                                                type_of_command,
                                                                slot_index,
                                                                FALSE))
        {
          return;
        }
      }
      else
      {
        /* Free and Invalidate the cache for the evt to be sure */
        QMI_CAT_FREE_IF_NOT_NULL(qmi_cati_global_ptr->raw_cache[slot_index][CATI_CACHED_EVT_PROVIDE_LOCAL_INFO]);
      }
      break;

    case GSTK_CMD_STK_SET_UP_EVENT_LIST:
      cati_event.mask = CATI_SETUP_EVT_LIST_MASK;

      /* Check if any client is registered for handling this event */
      if (!(qmi_cati_global_ptr->evt_reg_mask[slot_index] & CATI_SETUP_EVT_LIST_MASK))
      {
        /* If no client reg for this save it for future and send it from the cache
           Fall through if caching failed so that failure TR can be sent back to GSTK
         */
        if (QMI_ERR_NONE_V01 == qmi_cat_cache_raw_proactive_command(cat_msg_ptr,
                                                                type_of_command,
                                                                slot_index,
                                                                FALSE))
        {
          return;
        }
      }
      else
      {
        /* Free and Invalidate the cache for the evt to be sure */
        QMI_CAT_FREE_IF_NOT_NULL(qmi_cati_global_ptr->raw_cache[slot_index][CATI_CACHED_EVT_SETUP_EVT_LIST]);
      }
      break;

    case GSTK_CMD_STK_OPEN_CHANNEL:
      cati_event.mask = CATI_REPORT_STATUS_EV_DATA_EVT;
      cati_event.cati_evt_rpt.cati_raw_pc_evt.data_evt_type = CATI_DATA_EVENT_OPEN_CHANNEL;
      break;

    case GSTK_CMD_STK_CLOSE_CHANNEL:
      cati_event.mask = CATI_REPORT_STATUS_EV_DATA_EVT;
      cati_event.cati_evt_rpt.cati_raw_pc_evt.data_evt_type = CATI_DATA_EVENT_CLOSE_CHANNEL;
      break;

    case GSTK_CMD_STK_RECEIVE_DATA:
      cati_event.mask = CATI_REPORT_STATUS_EV_DATA_EVT;
      cati_event.cati_evt_rpt.cati_raw_pc_evt.data_evt_type = CATI_DATA_EVENT_RECEIVE_DATA;
      break;

    case GSTK_CMD_STK_SEND_DATA:
      cati_event.mask = CATI_REPORT_STATUS_EV_DATA_EVT;
      cati_event.cati_evt_rpt.cati_raw_pc_evt.data_evt_type = CATI_DATA_EVENT_SEND_DATA;
      break;

    case GSTK_CMD_STK_ACTIVATE:
      cati_event.mask = CATI_REPORT_STATUS_EV_ACTIVATE;
      break;

    case GSTK_CMD_STK_REFRESH:
      cati_event.mask = CATI_REPORT_STATUS_EV_REFRESH;
      break;

    default:
      UIM_MSG_ERR_0("Received unhandled Raw Proactive cmd");
      return;
  }

  UIM_MSG_MED_2("Proactive Cmd in process - 0x%x, qmi_cat card state - 0x%x",
                type_of_command, qmi_cati_global_ptr->card_state[slot_index]);

  if(qmi_cati_global_ptr->card_state[slot_index] == QMI_CAT_CARD_STATE_PRESENT)
  {
    /* Copy the raw command */
    cati_event.cati_evt_rpt.cati_raw_pc_evt.cmd_ref_id = cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id;
    cati_event.cati_evt_rpt.cati_raw_pc_evt.raw_data_len = (uint16)cat_msg_ptr->data.raw_proactive_cmd.raw_data_len;
    cati_event.cati_evt_rpt.cati_raw_pc_evt.raw_data_ptr = cat_msg_ptr->data.raw_proactive_cmd.raw_data_buffer;

    /* Generate event report ind for clients registered for the proactive cmd */
    for (i = 0; i < UIMQMI_SVC_MAX_CLIDS && !ind_sent; i++)
    {
      cl_sp = (qmi_cati_client_state_type*)qmi_cat_state_ptr->client[i];
      /* check if this service instance is bound to the qmi device */
      if(NULL == cl_sp)
      {
        continue;
      }

      /* Dont send to ALL clients just the one that registered for this */
      if( (qmi_cat_util_get_cli_index(cl_sp->client_handle, NULL)) &&
          (cl_sp->evt_report_mask[slot_index] & cati_event.mask ) )
      {
        qmi_cati_event_report_ind(cl_sp->client_handle,
                                  &cati_event);
        ind_sent = TRUE;
      }
      
    }
  }

  /* Send a TR if command is not sent to QMI client for any reason
     and
     The command being processed is not a cached UICC proactive command
     whose processing has already ended
   */
  if (!ind_sent &&
      qmi_cati_global_ptr->ref_command_id[slot_index] ==
      cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id)
  {
    UIM_MSG_HIGH_0("Indication not sent... sending TR or confirmation");

    /* Reset last last_gstk_cmd,  ref_command_id to filter duplicate TR */
      qmi_cati_global_ptr->last_gstk_cmd[slot_index]  = GSTK_END_PROACTIVE_CMD_REQ;
      qmi_cati_global_ptr->ref_command_id[slot_index] = 0;

    /* if the event is full functionality event send error TR */
    if(qmi_cati_global_ptr->full_func_evt[slot_index])
    {
      qmi_cati_global_ptr->full_func_evt[slot_index] = FALSE;
      gstk_status = qmi_cati_send_raw_term_response(cat_msg_ptr->data.raw_proactive_cmd.slot_id,
                                                    cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id,
                                                    cat_msg_ptr->data.raw_proactive_cmd.raw_data_len,
                                                    cat_msg_ptr->data.raw_proactive_cmd.raw_data_buffer,
                                                    GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND);
      if (gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_ERR_0 ("QMI-CAT : TR not sent!");
      }
    }
    else
    {
      switch (type_of_command)
      {
        case GSTK_CMD_STK_CLOSE_CHANNEL:
        case GSTK_CMD_STK_RECEIVE_DATA:
        case GSTK_CMD_STK_SEND_DATA:
        case GSTK_CMD_STK_REFRESH:
        case GSTK_CMD_STK_SEND_SHORT_MESSAGE:
        case GSTK_CMD_STK_SEND_DTMF:
        case GSTK_CMD_STK_SEND_SS:
        case GSTK_CMD_STK_USSD:
          /* Send icon display confirmation for above commands */
          gstk_status = gstk_send_icon_display_rsp(qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                                                   cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id,
                                                   FALSE );
          if (gstk_status != GSTK_SUCCESS)
          {
            UIM_MSG_ERR_0 ("QMI-CAT : Confirmation not sent!");
          }
          break;
        case GSTK_CMD_STK_OPEN_CHANNEL:
        case GSTK_CMD_STK_SET_UP_CALL:
          /* Send user confirmation for open channel and setup call */
          gstk_status = gstk_send_user_cnf_alpha_and_icon_disp_rsp(qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                                                                   cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id,
                                                                   FALSE,
                                                                   FALSE);
          if (gstk_status != GSTK_SUCCESS)
          {
            UIM_MSG_ERR_0 ("QMI-CAT : Confirmation not sent!");
          }
          break;
        default:
          gstk_status = qmi_cati_send_raw_term_response(cat_msg_ptr->data.raw_proactive_cmd.slot_id,
                                                        cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id,
                                                        cat_msg_ptr->data.raw_proactive_cmd.raw_data_len,
                                                        cat_msg_ptr->data.raw_proactive_cmd.raw_data_buffer,
                                                        GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND);
          if (gstk_status != GSTK_SUCCESS)
          {
            UIM_MSG_ERR_0 ("QMI-CAT : TR not sent!");
          }
          break;
      }
    }
  }
} /* qmi_cat_process_raw_proactive_command */


/*===========================================================================
  FUNCTION  QMI_CAT_PROCESS_ENVELOPE_RESPONSE()

  DESCRIPTION
    Process the raw envelope response from GSTK in QMI context

  PARAMETERS
    user_data  : User data
    env_result : Envelope result

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_envelope_response
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  const gstk_cmd_from_card_type      *env_rsp_ptr      = NULL;
  qmi_error_type_v01                  errval           = QMI_ERR_NONE_V01;
  qmi_csi_error                       csi_error        = QMI_CSI_NO_ERR;
  qmi_cati_userdata_type *            cb_userdata_ptr  = NULL;
  uint32                              ind_resp_len     = 0;
  void*                               ind_resp_ptr     = NULL;
  cat_send_envelope_cmd_resp_msg_v02 *send_envelope_cmd_resp_ptr = NULL;
  cat_send_envelope_cmd_ind_msg_v02  *send_envelope_cmd_ind_ptr = NULL;

  ASSERT(cat_msg_ptr != NULL);

  UIM_MSG_HIGH_0("Sending response for raw env rsp");

  /* First set the response pointer */
  env_rsp_ptr = &(cat_msg_ptr->data.envelope_rsp.envelope_rsp);

  /* Retrieve userdata */
  cb_userdata_ptr = (qmi_cati_userdata_type *)cat_msg_ptr->data.envelope_rsp.user_data;
  if (cb_userdata_ptr == NULL)
  {
    UIM_MSG_HIGH_0("Null userdata ptr in response");
    return;
  }
  
  if(cb_userdata_ptr->ind_token.is_valid == TRUE)
  {
    ind_resp_len   = sizeof(cat_send_envelope_cmd_ind_msg_v02);
    send_envelope_cmd_ind_ptr = (cat_send_envelope_cmd_ind_msg_v02*) uimqmi_malloc(ind_resp_len);
    if(send_envelope_cmd_ind_ptr == NULL)
    {
      return;
    }

    ind_resp_ptr  = send_envelope_cmd_ind_ptr;
  }
  else
  {
    ind_resp_len   = sizeof(cat_send_envelope_cmd_resp_msg_v02);
    send_envelope_cmd_resp_ptr = (cat_send_envelope_cmd_resp_msg_v02*) uimqmi_malloc(ind_resp_len);
    if(send_envelope_cmd_resp_ptr == NULL)
    {
      return;
    }
    ind_resp_ptr = send_envelope_cmd_resp_ptr;
  }

  /* Build the response from back to front */
  if (GSTK_ENVELOPE_CMD_SUCCESS == env_rsp_ptr->cmd.raw_envelope_rsp.general_resp)
  {
    if(cb_userdata_ptr->ind_token.is_valid == TRUE)
    {
      errval = qmi_cat_ind_raw_envelope(env_rsp_ptr,ind_resp_ptr);
    }
    else
    {
      errval = qmi_cat_response_raw_envelope(env_rsp_ptr,ind_resp_ptr);      
    }
    UIM_MSG_HIGH_1("Sending response for raw env rsp: errval 0x%x", errval);
  }

  if (cb_userdata_ptr->ind_token.is_valid)
  {
    csi_error = qmi_csi_send_ind(cb_userdata_ptr->client_handle, cb_userdata_ptr->request_id , ind_resp_ptr, ind_resp_len); 
  }
  else
  {
    qmi_cat_cmd_buf_s*   cmd_buf_ptr = NULL;
    cmd_buf_ptr = qmi_cat_util_get_cmd_buf_ptr(cb_userdata_ptr->req_handle, cb_userdata_ptr->client_handle); 
    if(cmd_buf_ptr != NULL && cmd_buf_ptr->resp_pending)
    {
      csi_error =  qmi_csi_send_resp(cb_userdata_ptr->req_handle, cb_userdata_ptr->request_id, ind_resp_ptr, ind_resp_len);
    }
  }
  if(csi_error != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_1("can not send resp/ind, errval 0x%x", csi_error);
  }

  uimqmi_free(send_envelope_cmd_resp_ptr);
  
  /* Free userdata since it is no longer needed */
  qmi_cat_free_cb_userdata(cb_userdata_ptr);
  cb_userdata_ptr = NULL;
} /* qmi_cat_process_envelope_response */


/*===========================================================================
  FUNCTION  QMI_CAT_PROCESS_END_PROACTIVE_SESSION()

  DESCRIPTION
    Process end of proactive command from gstk in QMI context

  PARAMETERS
    proactive_session_end_type : Proactive session end type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_end_proactive_session
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  qmi_cati_client_state_type      * cl_sp      = NULL;
  uint8                             j          = 0;
  uint8                             slot_index = 0;
  qmi_cati_evt_rpt_info_s_type      cati_event;

  ASSERT(cat_msg_ptr != NULL);
  ASSERT(qmi_cati_global_ptr);
  ASSERT(qmi_cat_state_ptr);

  UIM_MSG_HIGH_1("Generating CAT EVT REPORT IND for --- END Proactive Cmd with tag 0x%x ",
                 cat_msg_ptr->data.end_proactive_cmd.tag);

  /* Retrieve the slot index */
  if(qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                                    cat_msg_ptr->data.end_proactive_cmd.slot_id,
                                    &slot_index) != QMI_ERR_NONE_V01)
  {
    UIM_MSG_ERR_1("Invalid slot id for end proactive session command: 0x%x",
                  cat_msg_ptr->data.end_proactive_cmd.slot_id);
    return;
  }
  UIM_MSG_HIGH_2("Generating CAT EVT REPORT IND for --- END Proactive Cmd with tag 0x%x, slot_index 0x%x ",
                 cat_msg_ptr->data.end_proactive_cmd.tag, slot_index);

  /* Keep track of end session cmd for checking invalid TR / Evt Conf */
  qmi_cati_global_ptr->last_gstk_cmd[slot_index]  = GSTK_END_PROACTIVE_CMD_REQ;
  qmi_cati_global_ptr->ref_command_id[slot_index] = 0;
  qmi_cati_global_ptr->full_func_evt[slot_index]  = FALSE;
  qmi_cati_global_ptr->decoded_setup_evt_in_progress[slot_index] = 0;

  memset(&cati_event, 0, sizeof(qmi_cati_evt_rpt_info_s_type));
  cati_event.mask = CATI_REPORT_STATUS_EV_END_PROACTIVE_SESSION;
  cati_event.cati_evt_rpt.cati_end_proactive_cmd_evt.proactive_session_end_type =
    cat_msg_ptr->data.end_proactive_cmd.proactive_session_end_type;
  cati_event.slot_id = cat_msg_ptr->data.end_proactive_cmd.slot_id;

  /* Generate event report ind for clients registered for the proactive event.
     The same event is used for both raw and decoded format */
  for (j = 0; j < UIMQMI_SVC_MAX_CLIDS; j++)
  {
    cl_sp = qmi_cat_state_ptr->client[j];
    if (NULL == cl_sp)
    {
      continue;
    }

    if (cl_sp->evt_report_mask[slot_index] & cati_event.mask)
    {
      qmi_cati_event_report_ind(cl_sp->client_handle,
                                &cati_event);
    }
  }
} /* qmi_cat_process_end_proactive_session */


/*===========================================================================
  FUNCTION QMI_CAT_CMD_RSP_SW1_SW2()

  DESCRIPTION
    To process exclusively the status words messages for command response as reported by GSTK

  PARAMETERS
    cat_msg_ptr: Pointer to data from GSTK

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_term_rsp_sw1_sw2
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  qmi_error_type_v01        errval           = QMI_ERR_NONE_V01;
  uint32                    client_reg_count = 0;
  qmi_cati_userdata_type *  cb_userdata_ptr  = NULL;
  cat_send_tr_resp_msg_v02* send_tr_resp_ptr = NULL;
  cat_send_tr_ind_msg_v02*  send_tr_ind_ptr  = NULL;
  uint32                    ind_resp_len = 0;
  void*                     ind_resp_ptr = NULL;
  qmi_csi_error             csi_error    = QMI_CSI_NO_ERR;

  ASSERT(cat_msg_ptr != NULL);

  UIM_MSG_HIGH_0("Processing Status words for terminal response");

  /* Retrieve userdata */
  cb_userdata_ptr = (qmi_cati_userdata_type *)
    cat_msg_ptr->data.terminal_response_sw1_sw2_info.sw1_sw2_data.hdr_cmd.user_data;
  if (cb_userdata_ptr == NULL)
  {
    UIM_MSG_HIGH_0("Null userdata ptr in response");
    return;
  }

  /* Retrieve info */
  client_reg_count = cb_userdata_ptr->client_reg_count;

  if(cb_userdata_ptr->ind_token.is_valid == TRUE)
  {
    ind_resp_len   = sizeof(cat_send_tr_ind_msg_v02);
    send_tr_ind_ptr = (cat_send_tr_ind_msg_v02*) uimqmi_malloc(ind_resp_len);
    if(send_tr_ind_ptr == NULL)
    {
      return;
    }

    ind_resp_ptr  = send_tr_ind_ptr;
  }
  else
  {
    ind_resp_len   = sizeof(cat_send_tr_resp_msg_v02);
    send_tr_resp_ptr = (cat_send_tr_resp_msg_v02*) uimqmi_malloc(ind_resp_len);
    if(send_tr_resp_ptr == NULL)
    {
      return;
    }
    ind_resp_ptr = send_tr_resp_ptr;
  }

  if(cb_userdata_ptr->ind_token.is_valid == TRUE)  
  {
    errval = qmi_cat_ind_send_tr(
               ind_resp_ptr,
               &cat_msg_ptr->data.terminal_response_sw1_sw2_info.sw1_sw2_data.cmd.term_rsp_sw1_sw2_rpt);
  }
  else
  {
        errval = qmi_cat_response_send_tr(
               ind_resp_ptr,
               &cat_msg_ptr->data.terminal_response_sw1_sw2_info.sw1_sw2_data.cmd.term_rsp_sw1_sw2_rpt);
  }
  if (errval != QMI_ERR_NONE_V01)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
  }
  
  if (cb_userdata_ptr->ind_token.is_valid)
  {
    csi_error = qmi_csi_send_ind(cb_userdata_ptr->client_handle, cb_userdata_ptr->request_id , ind_resp_ptr, ind_resp_len); 
  }
  else
  {
    qmi_cat_cmd_buf_s*   cmd_buf_ptr=NULL;
    cmd_buf_ptr = qmi_cat_util_get_cmd_buf_ptr(cb_userdata_ptr->req_handle, cb_userdata_ptr->client_handle); 

    if(cmd_buf_ptr != NULL && cmd_buf_ptr->resp_pending)
    {
      csi_error =  qmi_csi_send_resp(cb_userdata_ptr->req_handle, cb_userdata_ptr->request_id, ind_resp_ptr, ind_resp_len);
    }
  }
  if(csi_error != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_1("can not send resp/ind, errval 0x%x", errval);
  }
  if(send_tr_ind_ptr != NULL)
  {
    uimqmi_free(send_tr_ind_ptr);
  }
  else if(send_tr_resp_ptr != NULL)
  {
    uimqmi_free(send_tr_resp_ptr);
  }
} /* qmi_cat_process_term_rsp_sw1_sw2 */

/*===========================================================================
  FUNCTION QMI_CAT_PROCESS_MMGSDI_CNF()

  DESCRIPTION
    To process MMGSDI confirmation

  PARAMETERS
    cat_message_ptr : message to the internal qmi_cat_message
                     containing an external command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_mmgsdi_cnf
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  ASSERT(cat_msg_ptr);
  ASSERT(qmi_cati_global_ptr);

  switch (cat_msg_ptr->data.mmgsdi_cnf.cnf_type)
  {
    case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF:
      qmi_cati_global_ptr->cat_mmgsdi_client_id =
        cat_msg_ptr->data.mmgsdi_cnf.cnf_value.client_id_and_evt_reg_cnf.response_header.client_id;
      break;

    default:
      break;
  }
} /* qmi_cat_process_mmgsdi_cnf() */


/*===========================================================================
  FUNCTION QMI_CAT_PROCESS_MMGSDI_EVT()

  DESCRIPTION
    To process mmgsdi events

  PARAMETERS
    cat_message_ptr : message to the internal qmi_cat_message
                     containing an external command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_mmgsdi_evt
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  uint8 slot_index = 0;

  ASSERT(cat_msg_ptr);
  ASSERT(qmi_cati_global_ptr);

  if (MMGSDI_MAX_SLOT_ID_ENUM == cat_msg_ptr->data.mmgsdi_evt.slot                                ||
      (MMGSDI_SLOT_1 == cat_msg_ptr->data.mmgsdi_evt.slot && qmi_cati_global_ptr->slot_count < 1) ||
      (MMGSDI_SLOT_2 == cat_msg_ptr->data.mmgsdi_evt.slot && qmi_cati_global_ptr->slot_count < 2) ||
      (MMGSDI_SLOT_3 == cat_msg_ptr->data.mmgsdi_evt.slot && qmi_cati_global_ptr->slot_count < 3))
  {
    return;
  }

  if (QMI_ERR_NONE_V01 != uimqmi_mmgsdi_slot_to_slot_index(
                        cat_msg_ptr->data.mmgsdi_evt.slot,
                        &slot_index) ||
      slot_index >= QMI_CAT_MAX_CARD_COUNT )
  {
    return;
  }

  switch (cat_msg_ptr->data.mmgsdi_evt.evt)
  {
    case MMGSDI_CARD_INSERTED_EVT:
      qmi_cati_global_ptr->card_state[slot_index] = QMI_CAT_CARD_STATE_PRESENT;

      /* Re-queue the cached proactive commands received before the card Insert event to process
         and send them to client */
      if(qmi_cati_global_ptr->evt_reg_mask[slot_index] & CATI_CACHED_EVT_MASK)
      {
        qmi_cat_send_buffered_raw_proactive_cmd(qmi_cati_global_ptr->evt_reg_mask[slot_index],
                                                slot_index);
      }
      break;

    case MMGSDI_PROFILE_SWITCHING_EVT:
      qmi_cati_global_ptr->card_state[slot_index] = QMI_CAT_CARD_STATE_INVALID;

      /* Re-queue the cached proactive commands received before the profile switch event to send TR to GSTK */
      qmi_cat_send_buffered_raw_proactive_cmd(qmi_cati_global_ptr->evt_reg_mask[slot_index],
                                              slot_index);
      /* Send end BIP session to the clients */
     // qmi_cat_end_bip_session(slot_index);
      break;

    case MMGSDI_CARD_ERROR_EVT:
        //qmi_cat_end_bip_session(slot_index);
        qmi_cat_cleanup_globals(slot_index);
        qmi_cati_global_ptr->card_state[slot_index] = QMI_CAT_CARD_STATE_ERROR;
      break;

    default:
      /* Never gets here */
      break;
  }
} /* qmi_cat_process_mmgsdi_evt() */


/*===========================================================================
  FUNCTION QMI_CAT_PROCESS_RECOVERY_COMPLETE()

  DESCRIPTION
    To process card recovery events

  PARAMETERS
    cat_msg_ptr : qmi_cat message pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_recovery_complete
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  uint8  slot_index = 0;

  ASSERT(cat_msg_ptr);
  ASSERT(qmi_cati_global_ptr);

  if (QMI_ERR_NONE_V01 == uimqmi_mmgsdi_slot_to_slot_index(
                        cat_msg_ptr->data.recovery_ind.slot,
                        &slot_index))
  {
    qmi_cati_card_state_e_type card_state =
                  qmi_cati_global_ptr->card_state[slot_index];

    qmi_cat_cleanup_globals(slot_index);
    qmi_cati_global_ptr->card_state[slot_index] = card_state;
  }
} /* qmi_cat_process_recovery_complete() */


/*===========================================================================
  FUNCTION QMI_CAT_PROCESS_TP_DL_IND()

  DESCRIPTION
    To process GSTK TP DL indication

  PARAMETERS
    cat_msg_ptr : qmi_cat message pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_tp_dl_ind
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  uint8  slot_index = 0;

  ASSERT(cat_msg_ptr);
  ASSERT(qmi_cati_global_ptr);

  if (QMI_ERR_NONE_V01 == qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                        cat_msg_ptr->data.tp_dl_ind.slot_id,
                        &slot_index))
  {
    /* for 2G card, when TP DL ind comes after mmgsdi card inserted evt,
       we need store the card state and restore the value after globals cleanup */
    qmi_cati_card_state_e_type card_state = qmi_cati_global_ptr->card_state[slot_index];
    
    qmi_cat_cleanup_globals(slot_index);
    qmi_cati_global_ptr->card_state[slot_index] = QMI_CAT_CARD_STATE_INIT_IN_PROGRESS;

    if(card_state == QMI_CAT_CARD_STATE_PRESENT)
    {
      qmi_cati_global_ptr->card_state[slot_index] = QMI_CAT_CARD_STATE_PRESENT;
    }
  }
} /* qmi_cat_process_tp_dl_ind() */


/*===========================================================================
  FUNCTION QMI_CAT_PROCESS_MESSAGE()

  DESCRIPTION
    Process the messages in DCC task

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_message
(
  void
)
{
  qmi_cat_message_type * cat_msg_ptr = NULL;

  cat_msg_ptr = qmi_cat_get_message();
  if (cat_msg_ptr == NULL)
  {
    UIM_MSG_ERR_0("No message in QMI CAT queue!!!");
    return;
  }

  switch(cat_msg_ptr->message_type)
  {
    case QMI_CAT_MESSAGE_RAW_PROACTIVE_COMMAND:
      qmi_cat_process_raw_proactive_command(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_ENVELOPE_RESPONSE:
      qmi_cat_process_envelope_response(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_END_PROACTIVE_SESSION:
      qmi_cat_process_end_proactive_session(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_TERM_RSP_SW1_SW2:
      qmi_cat_process_term_rsp_sw1_sw2(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_MMGSDI_CNF:
      qmi_cat_process_mmgsdi_cnf(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_MMGSDI_EVT:
      qmi_cat_process_mmgsdi_evt(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_RECOVERY_COMPLETE:
      qmi_cat_process_recovery_complete(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_TP_DL_IND:
      qmi_cat_process_tp_dl_ind(cat_msg_ptr);
      break;
    default:
      break;
  }

  qmi_cat_free_message(cat_msg_ptr);
  cat_msg_ptr = NULL;

} /* qmi_cat_process_message */


/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_CATI_RESET()

  DESCRIPTION
    Reset the issuing CAT client's state

  PARAMETERS
    cl_sp            : client state pointer
    req_handle       : req handle
    req_c_struct     : IDL client structure pointer
    req_c_struct_len : structure len

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error  qmi_cati_reset
(
  qmi_cati_client_state_type * cl_sp,
  qmi_req_handle               req_handle,
  void                       * req_c_struct,
  int                          req_c_struct_len
)
{
  cat_reset_resp_msg_v02  *reset_resp_ptr  = NULL;
  qmi_error_type_v01       errval          = QMI_ERR_NONE_V01;
  uint16                   client_index    = 0;
  qmi_cat_cmd_buf_s       *cmd_buf_p       = NULL;
  qmi_csi_error            csi_errval      = QMI_CSI_NO_ERR;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 &&  qmi_cat_state_ptr);
  
  UIM_MSG_HIGH_0("qmi_cati_reset");

  if(!qmi_cat_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return  QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_cat_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);
  ASSERT(cmd_buf_p);

  /* Reset the client's state */
  qmi_cati_reset_client(cl_sp);

  reset_resp_ptr =(cat_reset_resp_msg_v02*) uimqmi_malloc(sizeof(cat_reset_resp_msg_v02));
  if(!reset_resp_ptr)
  {
    return QMI_CSI_NO_MEM;
  }
  
  qmi_uim_util_populate_resp(errval, &reset_resp_ptr->resp);
  
  csi_errval = qmi_csi_send_resp(req_handle, QMI_CAT_RESET_RESP_V02, reset_resp_ptr, sizeof(cat_reset_resp_msg_v02));
  
  uimqmi_free(reset_resp_ptr);

  return csi_errval;
} /* qmi_cati_reset() */


/*===========================================================================
  FUNCTION QMI_CATI_SET_EVENT_REPORT()

  DESCRIPTION
    To set state for event reporting

  PARAMETERS
    cl_sp            : client state pointer
    req_handle       : req handle
    req_c_struct     : IDL client structure pointer
    req_c_struct_len : structure len

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error  qmi_cati_set_event_report
(
  qmi_cati_client_state_type * cl_sp,
  qmi_req_handle               req_handle,
  void                       * req_c_struct,
  int                          req_c_struct_len
)
{
  qmi_error_type_v01                errval                 = QMI_ERR_NONE_V01;
  qmi_cati_client_state_type      * cat_client_sp          = NULL;
  uint32                            error_raw_mask         = 0;
  uint32                            error_decoded_mask     = 0;
  uint32                            error_full_func_mask   = 0;
  uint64                            disable_full_func_mask = 0;
  uint64                            enable_full_func_mask  = 0;
  gstk_toolkit_cmd_reg_info_type    full_func_reg_list     = {0};
  uint8                             slot_index             = 0;
  gstk_status_enum_type             gstk_status            = GSTK_SUCCESS;
  uint16                            client_index           = 0;
  qmi_cat_cmd_buf_s                 *cmd_buf_p             = NULL;
  qmi_csi_error                     csi_errval             = QMI_CSI_NO_ERR;
  cat_set_event_report_req_msg_v02  *set_event_report_req_ptr  
                                                           = NULL;
  cat_set_event_report_resp_msg_v02 *set_event_report_resp_ptr 
                                                           = NULL;
  qmi_cat_in_set_event_report_type  tlv_data               = {0};

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 &&  qmi_cat_state_ptr);

  if(!qmi_cat_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return  QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_cat_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);
  ASSERT(cmd_buf_p);

  set_event_report_req_ptr  = (cat_set_event_report_req_msg_v02*)req_c_struct;
  cmd_buf_p->req_handle     = req_handle; 

  set_event_report_resp_ptr = (cat_set_event_report_resp_msg_v02*)uimqmi_malloc(
                                      sizeof(cat_set_event_report_resp_msg_v02));
  if(set_event_report_resp_ptr == NULL)
  {
    return QMI_CSI_NO_MEM;
  }

  UIM_MSG_HIGH_0("qmi_cati_set_event_report");

  memset(&full_func_reg_list, 0, sizeof(gstk_toolkit_cmd_reg_info_type));

  /* Parse TLV */
  errval = qmi_cat_parse_request_set_event_report(set_event_report_req_ptr, &tlv_data);
  if (errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  /* Retrieve the client */
  cat_client_sp = (qmi_cati_client_state_type *) cl_sp;

  /* Event report mask for raw format */
  if (tlv_data.raw_mask_present)
  {
    for(slot_index = 0; slot_index < qmi_cati_global_ptr->slot_count; slot_index++)
    {
      if (tlv_data.slot_present && ((tlv_data.slot_mask >> slot_index) & 0x01) == 0)
      {
        continue;
      }
      error_raw_mask = ((tlv_data.raw_mask) &
                        (~(cat_client_sp->evt_report_mask[slot_index])) &
                        (qmi_cati_global_ptr->evt_reg_mask[slot_index]));
      if(error_raw_mask)
      {
        break;
      }
    }

    if (error_raw_mask)
    {
      /* If some client already reg for this evt */
      errval = QMI_ERR_INVALID_OPERATION_V01;
    }
    else
    {
      uint32 new_reg_mask    = 0;
      uint32 new_common_mask = 0;

      for(slot_index = 0; slot_index < qmi_cati_global_ptr->slot_count; slot_index++)
      {
        if (tlv_data.slot_present && ((tlv_data.slot_mask >> slot_index) & 0x01) == 0)
        {
          continue;
        }
        new_common_mask = (qmi_cati_global_ptr->evt_reg_mask[slot_index] | tlv_data.raw_mask);
        new_reg_mask = (tlv_data.raw_mask | cat_client_sp->evt_report_mask[slot_index]);
        cat_client_sp->evt_report_mask[slot_index] = new_reg_mask;
        qmi_cati_global_ptr->evt_reg_mask[slot_index] = new_common_mask;
      }

      /* If the new registration includes setup menu or setup idle mode
         text that were buffered before, queue sending the evt to the control point */
      for(slot_index = 0; slot_index < qmi_cati_global_ptr->slot_count; slot_index++)
      {
        if(tlv_data.slot_present && ((tlv_data.slot_mask >> slot_index) & 0x01) == 0)
        {
          continue;
        }
        if (cat_client_sp->evt_report_mask[slot_index] & CATI_CACHED_EVT_MASK)
        {
          qmi_cat_send_buffered_raw_proactive_cmd(cat_client_sp->evt_report_mask[slot_index],
                                                    slot_index);
        }
      }
    }
  }

  /* Event report mask for full function mask*/
  if (tlv_data.full_func_mask_present)
  {
    uint32 new_full_func_common_mask = 0;

    for(slot_index = 0;slot_index < qmi_cati_global_ptr->slot_count; slot_index++)
    {
      if(tlv_data.slot_present && ((tlv_data.slot_mask >> slot_index) & 0x01) == 0)
      {
        continue;
      }

      new_full_func_common_mask = qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index];
      UIM_MSG_HIGH_1("tlv_data.full_func_mask 0x%x", tlv_data.full_func_mask);

      /* Set the enable & disable bitmask for full function registeration to GSTK */
      if ((cat_client_sp->evt_report_mask[slot_index] & CATI_REPORT_STATUS_EV_SENDSMS_ALPHA_ID))
      {
        if((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SMS) == 0)
        {
          disable_full_func_mask = disable_full_func_mask | GSTK_REG_SEND_SMS_EVT;
          new_full_func_common_mask = new_full_func_common_mask & (~CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SMS);
        }
        else
        {
          enable_full_func_mask = enable_full_func_mask | GSTK_REG_SEND_SMS_EVT;
          new_full_func_common_mask = new_full_func_common_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SMS;
        }
      }
      else
      {
        if ((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SMS) ==
            CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SMS)
        {
          error_full_func_mask = error_full_func_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SMS;
        }
      }

      if (cat_client_sp->evt_report_mask[slot_index] & CATI_REPORT_STATUS_EV_SETUP_CALL)
      {
        if((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SETUP_CALL) == 0)
        {
          disable_full_func_mask = disable_full_func_mask | GSTK_REG_SETUP_CALL_EVT;
          new_full_func_common_mask = new_full_func_common_mask & (~CATI_REPORT_STATUS_EV_FULL_FUNC_SETUP_CALL);
        }
        else
        {
          enable_full_func_mask = enable_full_func_mask | GSTK_REG_SETUP_CALL_EVT;
          new_full_func_common_mask = new_full_func_common_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SETUP_CALL;
        }
      }
      else
      {
        if((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SETUP_CALL) ==
           CATI_REPORT_STATUS_EV_FULL_FUNC_SETUP_CALL)
        {
          error_full_func_mask = error_full_func_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SETUP_CALL;
        }
      }

      if (cat_client_sp->evt_report_mask[slot_index] & CATI_REPORT_STATUS_EV_SEND_SS)
      {
        if((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS) == 0)
        {
          disable_full_func_mask = disable_full_func_mask | GSTK_REG_SEND_SS_EVT;
          new_full_func_common_mask = new_full_func_common_mask & (~CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS);
        }
        else
        {
          enable_full_func_mask = enable_full_func_mask | GSTK_REG_SEND_SS_EVT;
          new_full_func_common_mask = new_full_func_common_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS;
        }
      }
      else
      {
        if((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS) ==
           CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS)
        {
          error_full_func_mask = error_full_func_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS;
        }
      }

      if (cat_client_sp->evt_report_mask[slot_index] & CATI_REPORT_STATUS_EV_SEND_DTMF)
      {
        if((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_DTMF) == 0)
        {
          disable_full_func_mask = disable_full_func_mask | GSTK_REG_SEND_DTMF_EVT;
          new_full_func_common_mask = new_full_func_common_mask & (~CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_DTMF);
        }
        else
        {
          enable_full_func_mask = enable_full_func_mask | GSTK_REG_SEND_DTMF_EVT;
          new_full_func_common_mask = new_full_func_common_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_DTMF;
        }
      }
      else
      {
        if((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_DTMF) ==
           CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_DTMF)
        {
          error_full_func_mask = error_full_func_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_DTMF;
        }
      }

      if (cat_client_sp->evt_report_mask[slot_index] & CATI_REPORT_STATUS_EV_SEND_USSD)
      {
        if((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_USSD) == 0)
        {
          disable_full_func_mask = disable_full_func_mask | GSTK_REG_SEND_USSD_EVT;
          new_full_func_common_mask = new_full_func_common_mask & (~CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_USSD);
        }
        else
        {
          enable_full_func_mask = enable_full_func_mask | GSTK_REG_SEND_USSD_EVT;
          new_full_func_common_mask = new_full_func_common_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_USSD;
        }
      }
      else
      {
        if((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_USSD) ==
           CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_USSD)
        {
          error_full_func_mask = error_full_func_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_USSD;
        }
      }

      if (error_full_func_mask != 0)
      {
        errval = QMI_ERR_INVALID_ARG_V01;
        UIM_MSG_ERR_1("QMI-CAT error_full_func_mask 0x%x",
                      error_full_func_mask);
        break;
      }

      if (disable_full_func_mask != 0)
      {
      UIM_MSG_HIGH_1("QMI-CAT disable_full_func_mask 0x%x",
                     disable_full_func_mask);
        gstk_status = gstk_enable_estk_evt_handling((gstk_slot_id_enum_type)tlv_data.slot_mask,
                                       disable_full_func_mask);

        if (GSTK_SUCCESS != gstk_status)
        {
          UIM_MSG_ERR_1("QMI-CAT disable_full_func_mask failed 0x%x",
                        gstk_status);
          errval = QMI_ERR_INTERNAL_V01;
        }
      }
      if (errval != QMI_ERR_NONE_V01)
      {
        break;
      }

      if (enable_full_func_mask != 0)
      {
        UIM_MSG_HIGH_1("QMI-CAT enable_full_func_mask 0x%x",
                       enable_full_func_mask);
        full_func_reg_list.reg_set.class_bit_mask = GSTK_REG_CATEGORY_0_CMD;
        full_func_reg_list.reg_set.cmd_reg_bit_mask = enable_full_func_mask;
        full_func_reg_list.client_func_type = GSTK_HANDLE_ALL_FUNC;

        /* Android mode: everything in raw format */
        if (qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_ANDROID ||
            qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_CUSTOM_RAW)
        {
          gstk_status = gstk_client_toolkit_cmd_reg(
                          qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                          0x00,
                          GSTK_RAW_FORMAT,
                          1,
                          &full_func_reg_list,
                          qmi_cati_gstk_cmd_cb,
                          qmi_cati_gstk_evt_cb);

        }
        else if(qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_DECODED ||
                qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_CUSTOM_DECODED)
        {
          gstk_status = gstk_client_toolkit_cmd_reg(
                          qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                          0x00,
                          GSTK_GSTK_FORMAT,
                          1,
                          &full_func_reg_list,
                          qmi_cati_gstk_cmd_cb,
                          qmi_cati_gstk_evt_cb);
        }
        else
        {
          // full function capability only supported in Android or Decoded mode.
          errval = QMI_ERR_INVALID_OPERATION_V01;
        }

        if (GSTK_SUCCESS != gstk_status)
        {
          UIM_MSG_ERR_1("QMI-CAT enable_full_func_mask failed 0x%x",
                        gstk_status);
          errval = QMI_ERR_INTERNAL_V01;
        }
      }
      if (errval == QMI_ERR_NONE_V01)
      {
        qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index] = new_full_func_common_mask;
      }
      else
      {
        break;
      }
    }
  }

  /* The response TLV is required only in case of error */
  if (errval != QMI_ERR_NONE_V01)
  {
    if (QMI_ERR_NONE_V01 != qmi_cat_response_set_event_report(
                                     set_event_report_resp_ptr,
                                     error_raw_mask,
                                     error_decoded_mask,
                                     error_full_func_mask))
    {
      errval = QMI_ERR_NO_MEMORY_V01;
    }
  }
send_result:
  qmi_uim_util_populate_resp(errval, &set_event_report_resp_ptr->resp);
    
  csi_errval = qmi_cat_send_immediate_resp(cmd_buf_p, 
                                           set_event_report_resp_ptr,
                                           sizeof(cat_set_event_report_resp_msg_v02),
                                           cl_sp->client_handle);
  uimqmi_free(set_event_report_resp_ptr);
    
  return csi_errval;
} /* qmi_cati_set_event_report() */


/*===========================================================================
  FUNCTION QMI_CATI_GET_SERVICE_STATE()

  DESCRIPTION
    Gives the status of the device power including battery strength of the
    device and whether the device is externally powered

  PARAMETERS
    cl_sp            : client state pointer
    req_handle       : req handle
    req_c_struct     : IDL client structure pointer
    req_c_struct_len : structure len

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error  qmi_cati_get_service_state
(
  qmi_cati_client_state_type * cl_sp,
  qmi_req_handle               req_handle,
  void                       * req_c_struct,
  int                          req_c_struct_len
)
{
  qmi_cati_client_state_type *  cat_client_sp       = NULL;
  qmi_error_type_v01            errval              = QMI_ERR_NONE_V01;
  uint32                        raw_client_mask     = 0;
  uint32                        raw_global_mask     = 0;
  uint32                        decoded_client_mask = 0;
  uint32                        decoded_global_mask = 0;
  uint32                        full_func_mask      = 0;
  uint8                         slot_index          = 0;
  qmi_csi_error                 csi_errval          = QMI_CSI_NO_ERR;
  cat_get_service_state_resp_msg_v02 *get_service_state_resp_ptr 
                                                    = NULL;
  uint16                        client_index        = 0;
  qmi_cat_cmd_buf_s             *cmd_buf_p          = NULL;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 &&  qmi_cat_state_ptr);

  if(!qmi_cat_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return  QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_cat_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);
  ASSERT(cmd_buf_p);
  cmd_buf_p->req_handle     = req_handle; 

  get_service_state_resp_ptr = (cat_get_service_state_resp_msg_v02*)uimqmi_malloc(
                                      sizeof(cat_get_service_state_resp_msg_v02));
  if(get_service_state_resp_ptr == NULL)
  {
    UIM_MSG_ERR_0("Memory exhausted");
    return QMI_CSI_NO_MEM;
  }

  /* Retrieve client */
  cat_client_sp = (qmi_cati_client_state_type *) cl_sp;

  /* Create response */
  for(slot_index = 0; slot_index < qmi_cati_global_ptr->slot_count; slot_index++)
  {
    raw_global_mask     = raw_global_mask | qmi_cati_global_ptr->evt_reg_mask[slot_index];
    raw_client_mask     = raw_client_mask | cat_client_sp->evt_report_mask[slot_index];
    full_func_mask      = full_func_mask | qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index];
  }

  /* Add TLV to response */
  errval = qmi_cat_response_get_service_state(
                                     get_service_state_resp_ptr,
                                     raw_client_mask,
                                     raw_global_mask,
                                     decoded_client_mask,
                                     decoded_global_mask,
                                     full_func_mask);
  if (errval != QMI_ERR_NONE_V01)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
  }

  qmi_uim_util_populate_resp(errval, &get_service_state_resp_ptr->resp);
      
  csi_errval = qmi_cat_send_immediate_resp(cmd_buf_p, 
                                           get_service_state_resp_ptr,
                                           sizeof(cat_get_service_state_resp_msg_v02),
                                           cl_sp->client_handle);
  uimqmi_free(get_service_state_resp_ptr);
      
  return csi_errval;
} /* qmi_cati_get_service_state() */


/*===========================================================================
  FUNCTION QMI_CATI_SEND_TR()

  DESCRIPTION
    To send terminal response.

  PARAMETERS
    cl_sp            : client state pointer
    req_handle       : req handle
    req_c_struct     : IDL client structure pointer
    req_c_struct_len : structure len

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error  qmi_cati_send_tr
(
  qmi_cati_client_state_type * cl_sp,
  qmi_req_handle               req_handle,
  void                       * req_c_struct,
  int                          req_c_struct_len
)
{
  qmi_error_type_v01        errval           = QMI_ERR_NONE_V01;
  gstk_status_enum_type     gstk_status      = GSTK_SUCCESS;
  uint8                     slot_index       = 0;
  gstk_generic_data_type    term_resp        = {0};
  qmi_cat_in_raw_tr_type    tlv_data         = {0};  
  qmi_cati_userdata_type   *cb_userdata_ptr  = NULL;
  cat_send_tr_req_msg_v02  *send_tr_req_ptr  = NULL; 
  cat_send_tr_resp_msg_v02 *send_tr_resp_ptr = NULL;
  qmi_csi_error             csi_errval       = QMI_CSI_NO_ERR;
  uint16                    client_index     = 0;
  qmi_cat_cmd_buf_s        *cmd_buf_p        = NULL;
  qmi_cati_ind_token_info_type* token_ptr    = NULL;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 &&  qmi_cat_state_ptr);

  if(!qmi_cat_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return  QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_cat_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);
  ASSERT(cmd_buf_p);
  cmd_buf_p->req_handle     = req_handle; 

  send_tr_resp_ptr = (cat_send_tr_resp_msg_v02*)uimqmi_malloc(
                                      sizeof(cat_send_tr_resp_msg_v02));
  if(send_tr_resp_ptr == NULL)
  {
    UIM_MSG_ERR_0("Memory exhausted");
    return QMI_CSI_NO_MEM;
  }

  memset(&tlv_data, 0, sizeof(qmi_cat_in_raw_tr_type));
  memset(&term_resp, 0, sizeof(gstk_generic_data_type));
  send_tr_req_ptr = (cat_send_tr_req_msg_v02*)req_c_struct;

  /* Parse TLV */
  errval = qmi_cat_parse_request_raw_tr(send_tr_req_ptr, &tlv_data);
  if (errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  /* SEND TR to the card in raw format via GSTK */
  term_resp.data_len = tlv_data.term_resp.tr_len;
  term_resp.data_buffer_ptr = (uint8 *) &(tlv_data.term_resp.terminal_response[0]);

  /* Retrieve the slot index */
  if(tlv_data.slot_present)
  {
    errval = qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                            (gstk_slot_id_enum_type)tlv_data.slot.slot,
                            &slot_index);
    if (errval != QMI_ERR_NONE_V01)
    {
      goto send_result;
    }
  }

  errval = qmi_cati_check_last_gstk_cmd(slot_index, TRUE);
  if (errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  /* Reject TR if:
     1. cmd_ref_id does not match ref_command_id
   */
  if(tlv_data.term_resp.cmd_ref_id != qmi_cati_global_ptr->ref_command_id[slot_index])
  {
    errval = QMI_ERR_INVALID_OPERATION_V01;
    goto send_result;
  }

  token_ptr = (qmi_cati_ind_token_info_type*)uimqmi_malloc(
                                      sizeof(qmi_cati_ind_token_info_type));
  if(token_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  token_ptr->is_valid = (boolean)send_tr_req_ptr->indication_token_valid;
  token_ptr->token = send_tr_req_ptr->indication_token;
  cb_userdata_ptr = qmi_cat_get_cb_userdata(QMI_CAT_SEND_TR_REQ_V02, cmd_buf_p, send_tr_req_ptr->indication_token_valid ? token_ptr : NULL);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  gstk_status = gstk_send_raw_terminal_response(qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                                                (uint32)cb_userdata_ptr,
                                                tlv_data.term_resp.cmd_ref_id,
                                                term_resp);


  if(gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_HIGH_1("qmi_cati_send_tr : gstk_status = status %d", gstk_status);
    errval = qmi_cati_map_gstk_status_to_errval(gstk_status);

    /* Free userdata since it is no longer needed */
    qmi_cat_free_cb_userdata(cb_userdata_ptr);
    cb_userdata_ptr = NULL;
    goto send_result;
  }

  /* Reset last_gstk_cmd, ref_command_id to filter duplicate TR */
  qmi_cati_global_ptr->last_gstk_cmd[slot_index]  = GSTK_END_PROACTIVE_CMD_REQ;
  qmi_cati_global_ptr->ref_command_id[slot_index] = 0;
  qmi_cati_global_ptr->full_func_evt[slot_index]  = FALSE;

  if(gstk_nv_get_me_feature_status(GSTK_CFG_FEATURE_ME_REPORT_SW1_SW2) == TRUE)
  {
    uimqmi_free(send_tr_resp_ptr);
    uimqmi_free(token_ptr);
    cmd_buf_p->resp_pending = TRUE;
    return QMI_CSI_NO_ERR;
  }

  /* Free userdata since it is no longer needed */
  qmi_cat_free_cb_userdata(cb_userdata_ptr);
  cb_userdata_ptr = NULL;

send_result:
  if(errval == QMI_ERR_NONE_V01 && !send_tr_req_ptr->indication_token_valid)
  {
    cmd_buf_p->resp_pending = TRUE;
    return QMI_CSI_NO_ERR;
  }

   /* Update token info */
  send_tr_resp_ptr->indication_token_valid = send_tr_req_ptr->indication_token_valid;
  send_tr_resp_ptr->indication_token = send_tr_req_ptr->indication_token;
  
  qmi_uim_util_populate_resp(errval, &send_tr_resp_ptr->resp);

  csi_errval = qmi_cat_send_immediate_resp(cmd_buf_p, 
                                           send_tr_resp_ptr,
                                           sizeof(cat_send_tr_resp_msg_v02),
                                           cl_sp->client_handle);
  uimqmi_free(send_tr_resp_ptr);
  uimqmi_free(token_ptr);

  return csi_errval;
} /* qmi_cati_send_tr() */


/*===========================================================================
  FUNCTION QMI_CATI_SEND_ENVELOPE_CMD()

  DESCRIPTION
    Performs a manual service provisioning with data provided by the user

  PARAMETERS
    cl_sp            : client state pointer
    req_handle       : req handle
    req_c_struct     : IDL client structure pointer
    req_c_struct_len : structure len

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error  qmi_cati_send_envelope_cmd
(
  qmi_cati_client_state_type * cl_sp,
  qmi_req_handle               req_handle,
  void                       * req_c_struct,
  int                          req_c_struct_len
)
{
  qmi_error_type_v01           errval           = QMI_ERR_NONE_V01;
  gstk_status_enum_type        gstk_status      = GSTK_SUCCESS;
  uint8                        slot_index       = 0;
  qmi_cat_in_raw_envelope_type tlv_data         = {0};
  gstk_generic_data_type       envelope_cmd     = {0};
  qmi_cati_userdata_type      *cb_userdata_ptr  = NULL;
  cat_send_envelope_cmd_req_msg_v02  *send_envelope_cmd_req_ptr = NULL;
  cat_send_envelope_cmd_resp_msg_v02 *send_envelope_cmd_resp_ptr= NULL;
  qmi_csi_error                csi_errval       = QMI_CSI_NO_ERR;
  uint16                       client_index     = 0;
  qmi_cat_cmd_buf_s           *cmd_buf_p        = NULL;
  qmi_cati_ind_token_info_type* token_ptr       = NULL;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 &&  qmi_cat_state_ptr);

  if(!qmi_cat_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return  QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_cat_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);
  ASSERT(cmd_buf_p);
  cmd_buf_p->req_handle     = req_handle; 

  send_envelope_cmd_resp_ptr = (cat_send_envelope_cmd_resp_msg_v02*)uimqmi_malloc(
                                      sizeof(cat_send_envelope_cmd_resp_msg_v02));
  if(send_envelope_cmd_resp_ptr == NULL)
  {
    UIM_MSG_ERR_0("Memory exhausted");
    return QMI_CSI_NO_MEM;
  }

  memset(&tlv_data, 0, sizeof(qmi_cat_in_raw_envelope_type));
  memset(&envelope_cmd, 0, sizeof(gstk_generic_data_type));

  send_envelope_cmd_req_ptr = (cat_send_envelope_cmd_req_msg_v02*)req_c_struct;

  /* Parse TLV */
  errval = qmi_cat_parse_request_raw_envelope(send_envelope_cmd_req_ptr, &tlv_data);
  if (errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  /* Validate envelope command length */
  if (tlv_data.envelope.env_cmd_len < 2)
  {
    UIM_MSG_HIGH_1("Invalid envelope cmd len:0x%x ",
                   tlv_data.envelope.env_cmd_len);
    errval = QMI_ERR_INVALID_ARG_V01;
    goto send_result;
  }

  /* Validate if SMS PP envelope command is blocked */
  if (tlv_data.envelope.envelope_data[0] == CATI_SMS_PP_DOWNLOAD_TAG &&
      qmi_cati_global_ptr->sms_pp_envelope_blocked)
  {
    UIM_MSG_HIGH_0("SMS PP DL is blocked for security.");
    errval = QMI_ERR_INVALID_OPERATION_V01;
    goto send_result;
  }

  /* Validate the envelope command type */
  if (tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_MENU_SEL &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_EVT_DL_USER_ACT &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_EVT_DL_IDL_SCREEN_AVAIL &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_EVT_DL_LANG_SEL &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_UNKNOWN &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_EVT_DL_BROWSER_TERM &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_SEND_CALL_CONTROL &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_EVT_DL_HCI_CONN &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_SMS_PP_DOWNLOAD &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_EVT_DL_MT_CALL &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_EVT_DL_CALL_CONNECTED &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_EVT_DL_CALL_DISCONNECTED &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_EVT_DL_POLL_INTERVAL)
  {
    errval = QMI_ERR_INVALID_ARG_V01;
    goto send_result;
  }
  else if( tlv_data.envelope.env_cmd_type == CATI_ENV_CMD_TYPE_SMS_PP_DOWNLOAD &&
           (qmi_cati_global_ptr->sms_pp_envelope_blocked))
  {
    errval = QMI_ERR_INVALID_OPERATION_V01;
    goto send_result;
  }

  /* SEND Envelope Cmd to the card in raw format via GSTK */
  envelope_cmd.data_len = tlv_data.envelope.env_cmd_len;
  envelope_cmd.data_buffer_ptr = (uint8 *) &(tlv_data.envelope.envelope_data[0]);

  /* Retrieve the slot index */
  if(tlv_data.slot_present)
  {
    errval = qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                            (gstk_slot_id_enum_type)tlv_data.slot.slot,
                            &slot_index);
    if (errval != QMI_ERR_NONE_V01)
    {
      goto send_result;
    }
  }

  token_ptr = (qmi_cati_ind_token_info_type*)uimqmi_malloc(
                                      sizeof(qmi_cati_ind_token_info_type));
  if(token_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }
  
  token_ptr->is_valid = (boolean)send_envelope_cmd_req_ptr->indication_token_valid;
  token_ptr->token = send_envelope_cmd_req_ptr->indication_token;
  
  cb_userdata_ptr = qmi_cat_get_cb_userdata(QMI_CAT_SEND_ENVELOPE_CMD_REQ_V02, cmd_buf_p, send_envelope_cmd_req_ptr->indication_token_valid ? token_ptr : NULL);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  gstk_status = gstk_send_raw_envelope_command ( qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                                                 (uint32)cb_userdata_ptr,
                                                 envelope_cmd,
                                                 qmi_cati_gstk_raw_env_cb);

  if (GSTK_SUCCESS != gstk_status)
  {
    UIM_MSG_HIGH_1("qmi_cati_send_envelope_cmd : gstk_status = status %d",
                   gstk_status);
    errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
    /* Free userdata since it is no longer needed */
    qmi_cat_free_cb_userdata(cb_userdata_ptr);
    cb_userdata_ptr = NULL;
    goto send_result;
  }

  cmd_buf_p->resp_pending = TRUE;
  return QMI_CSI_NO_ERR;

send_result:    
   /* Update token info */
  send_envelope_cmd_resp_ptr->indication_token_valid = send_envelope_cmd_req_ptr->indication_token_valid;
  send_envelope_cmd_resp_ptr->indication_token = send_envelope_cmd_req_ptr->indication_token;
   
  qmi_uim_util_populate_resp(errval, &send_envelope_cmd_resp_ptr->resp);
    
  csi_errval = qmi_cat_send_immediate_resp(cmd_buf_p, 
                                           send_envelope_cmd_resp_ptr,
                                           sizeof(cat_send_envelope_cmd_resp_msg_v02),
                                           cl_sp->client_handle);
  uimqmi_free(send_envelope_cmd_resp_ptr);
   
  return csi_errval;
} /* qmi_cati_send_envelope_cmd() */


/*===========================================================================
  FUNCTION QMI_CATI_RESET_CLIENT()

  DESCRIPTION
    Resets the state for the given client

  PARAMETERS
    cl_sp_in :  client state pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cati_reset_client
(
  qmi_cati_client_state_type * cl_sp
)
{
  uint8                         slot_index      = 0;

  ASSERT(qmi_cati_global_ptr);

  /*-------------------------------------------------------------------------
    Clear the common mask bits specific to this client
  -------------------------------------------------------------------------*/
  for(slot_index = 0; slot_index < QMI_CAT_MAX_CARD_COUNT; slot_index ++)
  {
    qmi_cati_global_ptr->evt_reg_mask[slot_index] =
        ( qmi_cati_global_ptr->evt_reg_mask[slot_index] &
          (~cl_sp->evt_report_mask[slot_index]) );

    /* Clear event reporting */
    cl_sp->evt_report_mask[slot_index]         = CATI_REPORT_STATUS_NONE;
  }
} /* qmi_cati_reset_client */


/*===========================================================================
  FUNCTION QMI_CATI_GET_EVENT_REPORT()

  DESCRIPTION
    Fetches the last proactive command from the modem

  PARAMETERS
    cl_sp            : client state pointer
    req_handle       : req handle
    req_c_struct     : IDL client structure pointer
    req_c_struct_len : structure len

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error  qmi_cati_get_event_report
(
  qmi_cati_client_state_type * cl_sp,
  qmi_req_handle               req_handle,
  void                       * req_c_struct,
  int                          req_c_struct_len
)
{
  qmi_error_type_v01                 errval           = QMI_ERR_NONE_V01;
  gstk_status_enum_type              gstk_status      = GSTK_SUCCESS;
  uint8                              slot_index       = 0;
  gstk_client_pro_cmd_reg_data_type  proactive_cmd    = {0};
  qmi_cat_in_get_event_report_type   tlv_data         = {0};
  cat_get_event_report_req_msg_v02  *get_event_report_req_ptr  = NULL; 
  cat_get_event_report_resp_msg_v02 *get_event_report_resp_ptr = NULL;
  qmi_csi_error             csi_errval                = QMI_CSI_NO_ERR;
  uint16                    client_index              = 0;
  qmi_cat_cmd_buf_s         *cmd_buf_p                = NULL;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 &&  qmi_cat_state_ptr);

  if(!qmi_cat_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return  QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_cat_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);
  ASSERT(cmd_buf_p);
  cmd_buf_p->req_handle     = req_handle; 

  get_event_report_resp_ptr = (cat_get_event_report_resp_msg_v02*)uimqmi_malloc(
                                      sizeof(cat_get_event_report_resp_msg_v02));
  if(get_event_report_resp_ptr == NULL)
  {
    UIM_MSG_ERR_0("Memory exhausted");
    return QMI_CSI_NO_MEM;
  }

  memset(&tlv_data, 0, sizeof(qmi_cat_in_get_event_report_type));
  memset(&proactive_cmd, 0, sizeof(gstk_client_pro_cmd_reg_data_type));
  get_event_report_req_ptr = (cat_get_event_report_req_msg_v02*)req_c_struct;
  /* Parse TLV */
  errval = qmi_cat_parse_request_get_event_report(get_event_report_req_ptr, &tlv_data);
  if (errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  /* cmd_ref_id is no longer the index to GSTK's command_details_ref_table[],
     instead, it is a unique number of uint32, therefore, the check of
     cmd_ref_id against CATI_GSTK_PROACTIVE_COMMANDS_MAX is no longer valid
     and should be removed
   */

  /* Convert the format */
  switch(tlv_data.proactive_command.format)
  {
    case 1:
      proactive_cmd.data_format_type = GSTK_RAW_FORMAT;
      break;
    case 2:
      proactive_cmd.data_format_type = GSTK_GSTK_FORMAT;
      break;
    default:
      errval = QMI_ERR_INVALID_ARG_V01;
      break;
  }
  if (errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  /* Allocate memory for command from GSTK */
  proactive_cmd.pro_cmd_reg_data.gstk_req_rsp_ptr =
        (gstk_cmd_from_card_type*)uimqmi_malloc(sizeof(gstk_cmd_from_card_type));
  if (proactive_cmd.pro_cmd_reg_data.gstk_req_rsp_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY_V01;
    goto send_result;
  }

  /* Get command from GSTK */
  for (slot_index = 0; slot_index < qmi_cati_global_ptr->slot_count; slot_index++)
  {
    gstk_status = gstk_get_card_event_data(qmi_cati_global_ptr->cat_gstk_client_id_external[slot_index],
                                         tlv_data.proactive_command.cmd_ref_id,
                                         &proactive_cmd);

    if ((GSTK_SUCCESS != gstk_status) && (slot_index + 1) < qmi_cati_global_ptr->slot_count)
    {
      continue;
    }

    if (GSTK_SUCCESS != gstk_status)
    {
      UIM_MSG_HIGH_1("qmi_cati_get_event_report: gstk_status = status 0x%x",
                     gstk_status);

      errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
      uimqmi_free(proactive_cmd.pro_cmd_reg_data.gstk_req_rsp_ptr);
      goto send_result;
    }

    /* RAW format */
    if (proactive_cmd.data_format_type == GSTK_RAW_FORMAT)
    {
      /* get_event_report not supported in RAW FORMAT */
      if ((slot_index + 1) < qmi_cati_global_ptr->slot_count)
      {
        continue;
      }

      errval = QMI_ERR_INVALID_OPERATION_V01;

      uimqmi_free(proactive_cmd.pro_cmd_reg_data.gstk_req_rsp_ptr);
      goto send_result;
    }

    /* GSTK format */
    if (proactive_cmd.data_format_type == GSTK_GSTK_FORMAT)
    {
      if (proactive_cmd.pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.command_id == GSTK_END_PROACTIVE_CMD_REQ)
      {
        uint8 end_session_type = 0;
        /* Populate End Proactive Session TLV */
        if (proactive_cmd.pro_cmd_reg_data.gstk_req_rsp_ptr->cmd.end_pro_cmd_req.end_from_card)
        {
          end_session_type = CATI_PROACTIVE_SESSION_END_DUE_TO_CARD;
        }
        else
        {
          end_session_type = CATI_PROACTIVE_SESSION_END_INTERNAL;
        }

        errval = qmi_cat_response_end_proactive_session(
                                  get_event_report_resp_ptr,
                                  end_session_type,
                                  proactive_cmd.pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.sim_slot_id);
        if (errval != QMI_ERR_NONE_V01)
        {
          UIM_MSG_HIGH_0("Unable to generate CATI Event Report indication for end session!");
        }
      }

      uimqmi_free(proactive_cmd.pro_cmd_reg_data.gstk_req_rsp_ptr);
    }
    /* exit from for loop if there is a command for slot 1. If there are 2 commands,
       the function will return only the command in slot 1. User need to invoke again
       later for command in slot 2.*/
    break;
  }

send_result:
  qmi_uim_util_populate_resp(errval, &get_event_report_resp_ptr->resp);
    
  csi_errval = qmi_cat_send_immediate_resp(cmd_buf_p, 
                                           get_event_report_resp_ptr,
                                           sizeof(cat_get_event_report_resp_msg_v02),
                                           cl_sp->client_handle);
  uimqmi_free(get_event_report_resp_ptr);
    
  return csi_errval;
} /* qmi_cati_get_event_report() */


/*===========================================================================
  FUNCTION QMI_CATI_EVENT_CONFIRMATION()

  DESCRIPTION
    Send the confirmation for a network related command

  PARAMETERS
    cl_sp            : client state pointer
    req_handle       : req handle
    req_c_struct     : IDL client structure pointer
    req_c_struct_len : structure len

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error  qmi_cati_event_confirmation
(
  qmi_cati_client_state_type * cl_sp,
  qmi_req_handle               req_handle,
  void                       * req_c_struct,
  int                          req_c_struct_len
)
{
  qmi_error_type_v01                   errval       = QMI_ERR_NONE_V01;
  gstk_status_enum_type                gstk_status  = GSTK_SUCCESS;
  uint8                                slot_index   = 0;
  qmi_cat_in_event_confirmation_type   tlv_data     = {0};
  cat_event_confirmation_req_msg_v02  *event_confirmation_req_ptr  = NULL;
  cat_event_confirmation_resp_msg_v02 *event_confirmation_resp_ptr = NULL;
  qmi_csi_error                        csi_errval   = QMI_CSI_NO_ERR;
  uint16                               client_index = 0;
  qmi_cat_cmd_buf_s                   *cmd_buf_p    = NULL;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 &&  qmi_cat_state_ptr);

  if(!qmi_cat_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return  QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_cat_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);
  ASSERT(cmd_buf_p);
  cmd_buf_p->req_handle     = req_handle; 

  event_confirmation_resp_ptr = (cat_event_confirmation_resp_msg_v02*)uimqmi_malloc(
                                      sizeof(cat_event_confirmation_resp_msg_v02));

  memset(&tlv_data, 0, sizeof(qmi_cat_in_event_confirmation_type));
  event_confirmation_req_ptr = (cat_event_confirmation_req_msg_v02*)req_c_struct;
  /* Parse TLV */
  errval = qmi_cat_parse_request_event_confirmation(event_confirmation_req_ptr, &tlv_data);
  if (errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  /* Retrieve the slot index */
  if(tlv_data.slot_present)
  {
    errval = qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                            (gstk_slot_id_enum_type)tlv_data.slot.slot,
                            &slot_index);
    if (errval != QMI_ERR_NONE_V01)
    {
      goto send_result;
    }
  }

  errval = qmi_cati_check_last_gstk_cmd(slot_index, FALSE);
  if (errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  /* Send command to GSTK */
  if (tlv_data.user_confirmed_present)
  {
    gstk_status = gstk_send_user_cnf_alpha_and_icon_disp_rsp(
                           qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                           qmi_cati_global_ptr->ref_command_id[slot_index],
                           tlv_data.user_confirmed.user_confirmed,
                           tlv_data.icon_display_present ? tlv_data.icon_display.icon_display : FALSE
                           );
  }
  else if(tlv_data.icon_display_present)
  {
    gstk_status = gstk_send_icon_display_rsp(
                           qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                           qmi_cati_global_ptr->ref_command_id[slot_index],
                           tlv_data.icon_display.icon_display
                           );
  }
  else
  {
    UIM_MSG_HIGH_2("qmi_cati_event_confirmation : Got user = 0x%x, Got icon = 0x%x",
                   tlv_data.user_confirmed_present,
                   tlv_data.icon_display_present);
  }

  if (gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_HIGH_1("qmi_cati_event_confirmation : gstk_status = status %d",
                   gstk_status);
    errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
  }
  else
  {
    /* reset last GSTK cmd to filter duplicate Event Confirmation */
    qmi_cati_global_ptr->last_gstk_cmd[slot_index]  = GSTK_END_PROACTIVE_CMD_REQ;
    qmi_cati_global_ptr->ref_command_id[slot_index] = 0;
    qmi_cati_global_ptr->full_func_evt[slot_index]  = FALSE;
  }

send_result:
  qmi_uim_util_populate_resp(errval, &event_confirmation_resp_ptr->resp);
    
  csi_errval = qmi_cat_send_immediate_resp(cmd_buf_p, 
                                           event_confirmation_resp_ptr,
                                           sizeof(cat_event_confirmation_resp_msg_v02),
                                           cl_sp->client_handle);
  uimqmi_free(event_confirmation_resp_ptr);
    
  return csi_errval;
} /* qmi_cati_event_confirmation */

/*===========================================================================
  FUNCTION QMI_CATI_GET_TERMINAL_PROFILE()

  DESCRIPTION
    To retrieve Terminal Profile configuration.

  PARAMETERS
    cl_sp            : client state pointer
    req_handle       : req handle
    req_c_struct     : IDL client structure pointer
    req_c_struct_len : structure len

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error  qmi_cati_get_terminal_profile
(
  qmi_cati_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  qmi_error_type_v01                    errval        = QMI_ERR_NONE_V01;
  gstk_status_enum_type                 gstk_status   = GSTK_SUCCESS;
  uint8                                 terminal_profile_cache[GSTK_TERMINAL_PROFILE_SIZE];
  qmi_cat_in_get_terminal_profile_type  tlv_data      = {0};
  gstk_slot_id_enum_type                slot          = GSTK_SLOT_ID_MAX;
  cat_get_terminal_profile_req_msg_v02 *get_terminal_profile_req_ptr = NULL;
  cat_get_terminal_profile_resp_msg_v02*get_terminal_profile_resp_ptr = NULL;
  qmi_csi_error                         csi_errval    = QMI_CSI_NO_ERR;
  qmi_cat_cmd_buf_s                    *cmd_buf_p     = NULL;
  uint16                                client_index  = 0;
  
  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_cat_state_ptr && qmi_cati_global_ptr);
  
  if(!qmi_cat_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return  QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_cat_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);
  ASSERT(cmd_buf_p);
  cmd_buf_p->req_handle     = req_handle; 

  memset(&tlv_data, 0x00, sizeof(qmi_cat_in_get_terminal_profile_type));

  get_terminal_profile_req_ptr = (cat_get_terminal_profile_req_msg_v02*)req_c_struct;
  get_terminal_profile_resp_ptr = (cat_get_terminal_profile_resp_msg_v02*)uimqmi_malloc(
                                      sizeof(cat_get_terminal_profile_resp_msg_v02));
  if(get_terminal_profile_resp_ptr == NULL)
  {
    return QMI_CSI_NO_MEM;
  }

  /* Parse TLV */
  errval = qmi_cat_parse_request_get_terminal_profile(get_terminal_profile_req_ptr, &tlv_data);
  if (errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  if(tlv_data.slot_present)
  {
    slot = (gstk_slot_id_enum_type)tlv_data.slot.slot;
  }
  else
  {
    slot = GSTK_SLOT_1;
  }

  gstk_status = gstk_io_ctrl(GSTK_IO_GET_TERMINAL_PROFILE_DATA,
                             &slot,
                             terminal_profile_cache);
  if(GSTK_SUCCESS != gstk_status)
  {
    if(GSTK_INSUFFICIENT_INFO == gstk_status)
    {
      errval = QMI_ERR_DEVICE_NOT_READY_V01;
    }
    else
    {
      errval = QMI_ERR_INTERNAL_V01;
    }
    goto send_result;
  }

  /* Add TLV to response */
  errval = qmi_cat_response_get_terminal_profile(get_terminal_profile_resp_ptr,
                                                 (uint32)GSTK_TERMINAL_PROFILE_SIZE,
                                                 &terminal_profile_cache[0]);

send_result:
  qmi_uim_util_populate_resp(errval, &get_terminal_profile_resp_ptr->resp);

  csi_errval = qmi_cat_send_immediate_resp(cmd_buf_p, 
                                           get_terminal_profile_resp_ptr,
                                           sizeof(cat_get_terminal_profile_resp_msg_v02),
                                           cl_sp->client_handle);
  uimqmi_free(get_terminal_profile_resp_ptr);

  return csi_errval;

} /* qmi_cati_get_terminal_profile() */


/*===========================================================================
  FUNCTION QMI_CATI_SET_CONFIGURATION()

  DESCRIPTION
    Set QMI_CAT configuration

  PARAMETERS
    cl_sp            : client state pointer
    req_handle       : req handle
    req_c_struct     : IDL client structure pointer
    req_c_struct_len : structure len

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error  qmi_cati_set_configuration
(
  qmi_cati_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  qmi_error_type_v01                   errval                    = QMI_ERR_NONE_V01;
  qmi_cat_in_set_configuration_type    tlv_data                  = {0};
  cat_set_configuration_req_msg_v02   *set_configuration_req_ptr = NULL;
  cat_set_configuration_resp_msg_v02  *set_configuration_resp_ptr= NULL;
  qmi_csi_error                        csi_errval                = QMI_CSI_NO_ERR;
  uint16                               client_index              = 0;
  qmi_cat_cmd_buf_s                   *cmd_buf_p                 = NULL;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 &&  qmi_cat_state_ptr);

  if(!qmi_cat_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return  QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_cat_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);
  ASSERT(cmd_buf_p);
  cmd_buf_p->req_handle      = req_handle; 

  memset(&tlv_data, 0, sizeof(qmi_cat_in_set_configuration_type));

  set_configuration_req_ptr  = (cat_set_configuration_req_msg_v02*)req_c_struct;
  set_configuration_resp_ptr = (cat_set_configuration_resp_msg_v02*)uimqmi_malloc(
                                      sizeof(cat_set_configuration_resp_msg_v02));
  if(set_configuration_resp_ptr == NULL)
  {
    return QMI_CSI_NO_MEM;
  }

  /* Parse TLV */
  errval = qmi_cat_parse_request_set_configuration(set_configuration_req_ptr, &tlv_data);
  if (errval != QMI_ERR_NONE_V01)
  {
    goto send_result;
  }

  if(tlv_data.config_mode.config_mode == CATI_CONFIG_MODE_CUSTOM_DECODED ||
     tlv_data.config_mode.config_mode == CATI_CONFIG_MODE_CUSTOM_RAW)
  {
    if (!tlv_data.custom_tp_present)
    {
      UIM_MSG_ERR_1("custom tp TLV not present for custom config mode: 0x%x",
                    tlv_data.config_mode.config_mode);
      errval = QMI_ERR_MISSING_ARG_V01;
      goto send_result;
    }

    if (tlv_data.custom_tp.tp_len > CATI_TLV_CUSTOM_TP_MAX)
    {
      UIM_MSG_ERR_0("custom tp len out of range");
      errval = QMI_ERR_INVALID_ARG_V01;
      goto send_result;
    }

    /* Write to NV with the additionally padded zero bytes */
    errval = qmi_cati_store_tp_to_nv(tlv_data.custom_tp.tp_bitmask,
                                     sizeof(tlv_data.custom_tp.tp_bitmask));
    if (errval != QMI_ERR_NONE_V01)
    {
      goto send_result;
    }
  }
  else
  {
    if(tlv_data.custom_tp_present)
    {
      UIM_MSG_HIGH_1("Config mode : 0x%x doesn't support custom tp, ignoring TLV",
                     tlv_data.config_mode.config_mode);
    }
  }

  errval = qmi_cati_store_config_mode_to_nv(tlv_data.config_mode.config_mode);

send_result:
  qmi_uim_util_populate_resp(errval, &set_configuration_resp_ptr->resp);

  csi_errval = qmi_cat_send_immediate_resp(cmd_buf_p, 
                                           set_configuration_resp_ptr,
                                           sizeof(cat_set_configuration_resp_msg_v02),
                                           cl_sp->client_handle);
  uimqmi_free(set_configuration_resp_ptr);

  return csi_errval;
} /* qmi_cati_set_configuration */


/*===========================================================================
  FUNCTION QMI_CATI_GET_CONFIGURATION()

  DESCRIPTION
    Get QMI_CAT configuration

  PARAMETERS
    cl_sp            : client state pointer
    req_handle       : req handle
    req_c_struct     : IDL client structure pointer
    req_c_struct_len : structure len

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error  qmi_cati_get_configuration
(
  qmi_cati_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  qmi_error_type_v01                  errval        = QMI_ERR_NONE_V01;
  uint8                               tp_len        = 0;
  uint8                               tp[CATI_TLV_CUSTOM_TP_MAX];
  qmi_csi_error                       csi_errval    = QMI_CSI_NO_ERR;
  cat_get_configuration_resp_msg_v02 *get_configuration_resp_ptr = NULL;
  qmi_cat_cmd_buf_s                  *cmd_buf_p     = NULL;
  uint16                              client_index  = 0;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_cat_state_ptr && qmi_cati_global_ptr);

  if(!qmi_cat_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return  QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_cat_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);
  ASSERT(cmd_buf_p);
  cmd_buf_p->req_handle     = req_handle; 
  
  get_configuration_resp_ptr = (cat_get_configuration_resp_msg_v02*)uimqmi_malloc(
                                      sizeof(cat_get_configuration_resp_msg_v02));
  if(get_configuration_resp_ptr == NULL)
  {
    return QMI_CSI_NO_MEM;
  }
  
  /* Add TLV to response */
  if(qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_CUSTOM_RAW ||
     qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_CUSTOM_DECODED)
  {
    tp_len = qmi_cati_get_tp_from_nv(tp, sizeof(tp));
    if(tp_len > 0)
    {
      errval = qmi_cat_response_get_configuration(
                                         get_configuration_resp_ptr,
                                         qmi_cati_global_ptr->config_mode,
                                         tp_len,
                                         tp);
    }
    else
    {
      UIM_MSG_ERR_0("custom tp bitmask not set in NV");
      errval = QMI_ERR_INTERNAL_V01;
    }
  }
  else
  {
    errval = qmi_cat_response_get_configuration(
                                       get_configuration_resp_ptr,
                                       qmi_cati_global_ptr->config_mode,
                                       0,
                                       NULL);
  }

  qmi_uim_util_populate_resp(errval, &get_configuration_resp_ptr->resp);

  csi_errval = qmi_cat_send_immediate_resp(cmd_buf_p, 
                                           get_configuration_resp_ptr,
                                           sizeof(cat_get_configuration_resp_msg_v02),
                                           cl_sp->client_handle);
  uimqmi_free(get_configuration_resp_ptr);

  return csi_errval;
} /* qmi_cati_get_configuration() */


/*===========================================================================
  FUNCTION QMI_CATI_GET_CACHED_PROACTIVE_CMD()

  DESCRIPTION
    Fetches the requested proactive command from the recovery cache

  PARAMETERS
    cl_sp            : client state pointer
    req_handle       : req handle
    req_c_struct     : IDL client structure pointer
    req_c_struct_len : structure len

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error  qmi_cati_get_cached_proactive_cmd
(
  qmi_cati_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
)
{
  qmi_error_type_v01                         errval        = QMI_ERR_NONE_V01;
  qmi_cati_raw_cache_type                   *raw_cache_ptr = NULL;
  uint8                                      slot_index    = 0;
  cat_get_cached_proactive_cmd_req_msg_v02  *get_cached_proactive_cmd_req_ptr = NULL;
  cat_get_cached_proactive_cmd_resp_msg_v02 *get_cached_proactive_cmd_rsp_ptr = NULL;  
  qmi_cat_cmd_buf_s                         *cmd_buf_p     = NULL;
  qmi_csi_error                              csi_errval    = QMI_CSI_NO_ERR;
  uint16                                     client_index  = 0;

  ASSERT(cl_sp && req_c_struct && req_c_struct_len > 0 && qmi_cat_state_ptr && qmi_cati_global_ptr);

  UIM_MSG_HIGH_1("qmi_cati_get_cached_proactive_cmd: config_mode=0x%x",
                 qmi_cati_global_ptr->config_mode);

  /* Check configuration mode, currently supports Android mode and Custom Raw mode only */
  if (qmi_cati_global_ptr->config_mode != CATI_CONFIG_MODE_ANDROID &&
      qmi_cati_global_ptr->config_mode != CATI_CONFIG_MODE_CUSTOM_RAW)
  {
    errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    goto send_result;
  }
  
  if(!qmi_cat_util_get_cli_index(cl_sp->client_handle, &client_index))
  {
    return QMI_CSI_INVALID_HANDLE;
  }

  cmd_buf_p = qmi_cat_util_get_cmd_buf_ptr(req_handle, cl_sp->client_handle);// change this to qmi_cat
  
  ASSERT(cmd_buf_p);
  
  get_cached_proactive_cmd_req_ptr  = (cat_get_cached_proactive_cmd_req_msg_v02*)req_c_struct;
  cmd_buf_p->req_handle             = req_handle; 

  get_cached_proactive_cmd_rsp_ptr = (cat_get_cached_proactive_cmd_resp_msg_v02*)uimqmi_malloc(
                                      sizeof(cat_get_cached_proactive_cmd_resp_msg_v02));
  if(get_cached_proactive_cmd_rsp_ptr == NULL)
  {
    return QMI_CSI_NO_MEM;
  }

  /* Validate slot id */
  if (get_cached_proactive_cmd_req_ptr->slot_valid &&
      (get_cached_proactive_cmd_req_ptr->slot.slot > qmi_cati_global_ptr->slot_count ||
       get_cached_proactive_cmd_req_ptr->slot.slot > QMI_CAT_MAX_CARD_COUNT))
  {
    errval = QMI_ERR_INVALID_ARG_V01;
    goto send_result;
  }

  /* Get slot_index */
  if (get_cached_proactive_cmd_req_ptr->slot_valid)
  {
    switch (get_cached_proactive_cmd_req_ptr->slot.slot)
    {
      case CAT_SLOT1_V02:
        slot_index = 0;
        break;
      case CAT_SLOT2_V02:
        slot_index = 1;
        break;
      case CAT_SLOT3_V02:
        slot_index = 2;
        break;
      case CAT_SLOT4_V02:
        slot_index = 3;
        break;
      case CAT_SLOT5_V02:
        slot_index = 4;
        break;
      default:
        /* Won't get here since slot id is already validated */
        break;
    }
  }

  if (slot_index >= QMI_CAT_MAX_CARD_COUNT)
  {
    errval = QMI_ERR_OP_DEVICE_UNSUPPORTED_V01;
    goto send_result;
  }

  /* Get proactive command from recovery cache only if proactive command is sent and
     the TR is not pending from the client, if TR is pending cached command will be
     sent during set event report no need to send again */
  switch (get_cached_proactive_cmd_req_ptr->command_id)
  {
    case CATI_CACHED_CMD_TYPE_SETUP_MENU:
      if(qmi_cati_global_ptr->last_gstk_cmd[slot_index] != GSTK_SETUP_MENU_REQ)
      {
        raw_cache_ptr = qmi_cati_global_ptr->recovery_raw_cache[slot_index][CATI_QMI_CLIENT_CACHED_EVT_SETUP_MENU];
      }
      break;
    case CATI_CACHED_CMD_TYPE_SETUP_EVT_LIST:
      if(qmi_cati_global_ptr->last_gstk_cmd[slot_index] != GSTK_SETUP_EVENT_LIST_REQ)
      {
        raw_cache_ptr = qmi_cati_global_ptr->recovery_raw_cache[slot_index][CATI_QMI_CLIENT_CACHED_EVT_SETUP_EVT_LIST];
      }
      break;
    case CATI_CACHED_CMD_TYPE_SETUP_IDLE_MODE_TEXT:
      if(qmi_cati_global_ptr->last_gstk_cmd[slot_index] != GSTK_SETUP_IDLE_TEXT_REQ)
      {
        raw_cache_ptr = qmi_cati_global_ptr->recovery_raw_cache[slot_index][CATI_QMI_CLIENT_CACHED_EVT_SETUP_IDLE_MODE_TEXT];
      }
      break;
    default:
      errval = QMI_ERR_INVALID_ARG_V01;
      break;
  }

  if (raw_cache_ptr == NULL)
  {
    /* Set QMI_ERR_INFO_UNAVAILABLE only if command_id is valid */
    if (QMI_ERR_NONE_V01== errval)
    {
      errval = QMI_ERR_INFO_UNAVAILABLE_V01;
    }
    goto send_result;
  }

  errval = qmi_cat_response_cached_proactive_cmd(
              get_cached_proactive_cmd_req_ptr->command_id,
              get_cached_proactive_cmd_rsp_ptr,
              raw_cache_ptr->cached_event.cati_evt_rpt.cati_raw_pc_evt.cmd_ref_id,
              raw_cache_ptr->cached_event.cati_evt_rpt.cati_raw_pc_evt.raw_data_len,
              raw_cache_ptr->cached_event.cati_evt_rpt.cati_raw_pc_evt.raw_data);

send_result:
  qmi_uim_util_populate_resp(errval, &get_cached_proactive_cmd_rsp_ptr->resp);

  csi_errval = qmi_cat_send_immediate_resp(cmd_buf_p, 
                                           get_cached_proactive_cmd_rsp_ptr,
                                           sizeof(cat_get_cached_proactive_cmd_resp_msg_v02),
                                           cl_sp->client_handle);
  uimqmi_free(get_cached_proactive_cmd_rsp_ptr);

  return csi_errval;
} /* qmi_cati_get_cached_proactive_cmd() */


/*===========================================================================
  FUNCTION QMI_CAT_RECOVERY_COMPLETE()

  DESCRIPTION
    This function is called by the QMI UIM when a recovery indication
    has completed successfully

  PARAMETERS
    slot: Indicates the slot in which the recovery occured

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_cat_recovery_complete
(
  mmgsdi_slot_id_enum_type  slot
)
{
  qmi_cat_message_type * msg_ptr = NULL;

  UIM_MSG_HIGH_0("Received recovery indication");

  msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_RECOVERY_COMPLETE);
  if (msg_ptr)
  {
    msg_ptr->data.recovery_ind.slot = slot;
    qmi_cat_post_message(msg_ptr);
  }
} /* qmi_cat_recovery_complete*/


/*===========================================================================
FUNCTION: qmi_cat_deinit

DESCRIPTION:
  This function is used to free the qmicat private resources.
  This is called when GSTK is powering down.

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void qmi_cat_deinit
(
  void
)
{
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
}/*qmi_cat_deinit */


/*===========================================================================
FUNCTION: qmi_cat_check_if_supported

DESCRIPTION:
  This function is used to check if QMI CAT is supported

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  boolean: Indicates if QMI CAT is supported or not

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
boolean qmi_cat_check_if_supported
(
  void
)
{
  return TRUE;
}/* qmi_cat_check_if_supported */


/*===========================================================================
  FUNCTION QMI_CAT_SEND_IMMEDIATE_RESP

  DESCRIPTION
    This function validated the client and then send immediate response
    to the client and free the cmd data from clinet queue.

  PARAMETERS
    qmi_cat_cmd_buf_s *      cmd_buf_ptr
    void *                   response_ptr
    uint16                   response_size
    qmi_client_handle        cli_handle    

  DEPENDENCIES
    None

  RETURN VALUE
    qmi_csi_error

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_cat_send_immediate_resp
(
  qmi_cat_cmd_buf_s       *cmd_buf_ptr,
  void                    *response_ptr,
  unsigned int             response_size,
  qmi_client_handle        cli_handle
)
{
  uint16                   client_index     = 0;
  qmi_csi_error            errval           = QMI_CSI_INTERNAL_ERR;

  ASSERT(response_ptr && response_size > 0);
  
  if(!qmi_cat_util_get_cli_index( cli_handle, &client_index))
  {
    UIM_MSG_ERR_1("invalid client 0x%x", cli_handle);
    return QMI_CSI_INVALID_HANDLE;
  }

  if(cmd_buf_ptr != NULL       &&
     /* Getting buffer to check if client is stil active */
    qmi_cat_util_get_cmd_buf_ptr(cmd_buf_ptr->req_handle, cmd_buf_ptr->client_handle))
  {
    errval =  qmi_csi_send_resp(cmd_buf_ptr->req_handle, cmd_buf_ptr->cmd_type, response_ptr, response_size);
    /* removing cmd data from client q  */
    qmi_cat_util_remove_qmi_cmd(cmd_buf_ptr->req_handle,
                                cmd_buf_ptr->client_handle,
                                &qmi_cat_state_ptr->client[client_index]->cmd_q);
  }
  
  if(errval != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_1("can not immidiate send resp errval 0x%x", errval);
  }

  return errval;
}/* qmi_cat_send_immidiate_resp */


/*===========================================================================
  FUNCTION QMI_CAT_SEND_RESPONSE_IND

  DESCRIPTION
    This function validated the client and then send response or indication to
    the client depends upon its type.

  PARAMETERS
    qmi_cat_cmd_buf_s *      cmd_buf_ptr
    void *                   response_ptr
    uint16                   cmd_type
    uint16                   response_size
    boolean                  ind_required
    qmi_client_handle        cli_handle    

  DEPENDENCIES
    None

  RETURN VALUE
    qmi_csi_error

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_error qmi_cat_send_response_ind
(
  qmi_cat_cmd_buf_s       *cmd_buf_ptr,
  void                    *response_ptr,
  uint16                   cmd_type,
  unsigned int             response_size,
  boolean                  ind_required,
  qmi_client_handle        cli_handle
)
{
  uint16                   client_index     = 0;
  qmi_csi_error            errval           = QMI_CSI_INTERNAL_ERR;

  ASSERT(response_ptr && response_size > 0);
  
  if(!qmi_cat_util_get_cli_index( cli_handle, &client_index))
  {
    UIM_MSG_ERR_1("invalid client 0x%x", cli_handle);
    return QMI_CSI_INVALID_HANDLE;
  }

  /* Send response or indication */
  if (ind_required)
  {
    errval = qmi_csi_send_ind(cli_handle, cmd_type , response_ptr, response_size); 
  }
  else
  {
    if(cmd_buf_ptr != NULL       &&
       cmd_buf_ptr->resp_pending &&
       /* Getting buffer to check if client is stil active */
       qmi_cat_util_get_cmd_buf_ptr(cmd_buf_ptr->req_handle, cmd_buf_ptr->client_handle))
    {
      errval =  qmi_csi_send_resp(cmd_buf_ptr->req_handle, cmd_buf_ptr->cmd_type, response_ptr, response_size);
    }
  }
  if(errval != QMI_CSI_NO_ERR)
  {
    UIM_MSG_ERR_2("can not send resp/ind, indcation valid 0x%x, errval 0x%x", ind_required, errval);
  }

  return errval;
}/* qmi_cat_send_response_ind */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
