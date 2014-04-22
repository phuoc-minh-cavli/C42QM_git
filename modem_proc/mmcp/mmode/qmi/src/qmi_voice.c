/*===========================================================================

                         D S _ Q M I _ VOICE . C

DESCRIPTION

 The Data Services Qualcomm MSM Interface Device Management Services source
 file.

EXTERNALIZED FUNCTIONS

  qmi_voice_init()
    Register the VOICE service with QMUX for all applicable QMI links
    

Copyright (c) 2009-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/qmi/src/qmi_voice.c#1 $ $DateTime: 2010/04/22 22:49:27 $ $voiceor: nlocanin $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/25/09    vs     Initial Revision.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_QMI_SERVICE_SPLIT

#include "qmi_voice.h"

#include "dsm.h"
#include "nv.h"
#include "err.h"
#include "amssassert.h"
#include "qmi_voice_cm_if.h"
#include "qmi_voice_cmd_list.h"
#include "modem_mem.h"
#include "qmi_mmode_svc.h"
#include "qmi_mmode_task_cmd.h"

#include "qmi_si.h"
#include "qmi_idl_lib.h"
#include "voice_service_v02.h"
#include "voice_service_impl_v02.h"
#include "qm_timer.h"
#include "qm_util.h"
#ifdef CM_DEBUG
#error code not present
#endif

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

#ifndef FEATURE_NO_QMI_VOICE
/*---------------------------------------------------------------------------
  Service management
---------------------------------------------------------------------------*/
#define VOICEI_MAX_CLIDS    (10)

/*---------------------------------------------------------------------------
  Major and Minor Version Nos for VOICE
---------------------------------------------------------------------------*/
#define VOICEI_BASE_VER_MAJOR    (2)
#define VOICEI_BASE_VER_MINOR    (1)

#define VOICEI_ADDENDUM_VER_MAJOR  (0)
#define VOICEI_ADDENDUM_VER_MINOR  (0)

/*---------------------------------------------------------------------------
  Optional TLVs
---------------------------------------------------------------------------*/
#define VOICEI_IND_REG_DTMF             (0x10)
#define VOICEI_IND_REG_VOICE_PRIV       (0x11)
#define VOICEI_IND_REG_SUPS_NOTIFY      (0x12)
#define VOICEI_IND_REG_CALL_EVENTS_NOTIFY      (0x13)
#define VOICEI_IND_REG_HANDOVER_NOTIFY  (0x14)
#define VOICEI_IND_REG_SPEECH_NOTIFY        (0x15)
#define VOICEI_IND_REG_USSD_NOTIFY          (0x16)
#define VOICEI_IND_REG_SUPS_OTHER_NOTIFY    (0x17)
#define VOICEI_IND_REG_MODIFICATION_NOTIFY  (0x18)
#define VOICEI_IND_REG_UUS_NOTIFY           (0x19)
#define VOICEI_IND_REG_AOC_NOTIFY                         (0x1A)
#define VOICEI_IND_REG_CONF_NOTIFY           (0x1B)
#define VOICEI_IND_REG_EXT_BRST_INTL_NOTIFY     (0x1C)
#define VOICEI_IND_REG_MT_PAGE_MISS_NOTIFY  (0x1D)
#define VOICEI_IND_REG_CC_RESULT_NOTIFY     (0x1E)
#define VOICEI_IND_REG_CONF_PARTS_NOTIFY     (0x1F)
#define VOICEI_IND_REG_TTY_INFO_NOTIFY     (0x20)
#define VOICEI_IND_REG_E911_ORIG_FAIL_NOTIFY   (0x21)
#define VOICEI_IND_REG_AUDIO_RAT_CHANGE_NOTIFY (0x23)
#define VOICEI_IND_REG_ADD_CALL_INFO_NOTIFY     (0x24)
#define VOICEI_IND_REG_VICE_DIALOG_INFO_NOTIFY (0x25)
#define VOICEI_IND_REG_MT_CALL_END_AUTO_REJECT_NOTIFY (0x29)

#define VOICEI_DIAL_CALL_TYPE   (0x10)
#define VOICEI_DIAL_CLIR_TYPE       (0x11)
#ifdef FEATURE_UUS
#define VOICEI_DIAL_UUS_INFO        (0x12)
#endif
#define VOICEI_DIAL_CUG             (0x13)
#define VOICEI_DIAL_CALL_ID         (0x10)
#define VOICEI_DIAL_ALPHA_ID        (0x11)
#define VOICEI_DIAL_EMER_CAT        (0x14)
#define VOICEI_DIAL_CALLED_PARTY_SUB_ADDRRESS      (0x15)
#define VOICEI_DIAL_SRV_TYPE        (0x16)
#define VOICEI_DIAL_SIP_URI_OVERFLOW    (0x17)
#define VOICEI_DIAL_AUDIO_ATTRIB        (0x18)
#define VOICEI_DIAL_VIDEO_ATTRIB        (0x19)
#define VOICEI_DIAL_IP_CALL_PI          (0x1A)
#define VOICEI_DIAL_VS_CALL_ATTRIB      (0x1B)
#define VOICEI_DIAL_ECALL_VARIANT       (0x1C)
#define VOICEI_DIAL_CONFERENCE_INIT_URI      (0x1D)
#define VOICEI_DIAL_DISPLAY_TEXT        (0x1E)


#define VOICEI_DIAL_CC_RESULT_TYPE   (0x12)
#define VOICEI_DIAL_CC_RESULT_SUPS   (0x13)
#define VOICEI_DIAL_RESP_END_REASON  (0x14)
#define VOICEI_DIAL_RESP_MEDIA_ID    (0x15)

#define VOICEI_BURST_DTMF_LEN       (0x10)
#define VOICEI_BURST_DTMF_CALL_ID   (0x10)

#define VOICEI_END_CALL_ID          (0x10)

#define VOICEI_ANSWER_CALL_ID       (0x10)

#define VOICEI_ANSWER_CALL_TYPE        (0x10)
#define VOICEI_ANSWER_AUDIO_ATTRIB     (0x11)
#define VOICEI_ANSWER_VIDEO_ATTRIB     (0x12)
#define VOICEI_ANSWER_IP_CALL_PI       (0x13)
#define VOICEI_ANSWER_VS_FILE_ATTRIB   (0x14)
#define VOICEI_ANSWER_REJECT_CALL      (0x15)
#define VOICEI_ANSWER_REJECT_CAUSE     (0x16)
#define VOICEI_ANSWER_REJECT_SIP_CODE  (0x17)

#define VOICEI_SETUP_ANSWER_RESP_CALL_ID     (0x10)

#define VOICEI_SETUP_ANSWER_REJECT_CALL      (0x10)
#define VOICEI_SETUP_ANSWER_REJECT_CAUSE     (0x11)

#define VOICEI_GET_CALL_INFO        (0x10)
#define VOICEI_GET_CALL_RP_NUM      (0x11)
#define VOICEI_GET_CALL_SO          (0x12)
#define VOICEI_GET_CALL_VP          (0x13)
#define VOICEI_GET_CALL_OTSP        (0x14)
#define VOICEI_GET_CALL_RP_NAME     (0x15)
#ifdef FEATURE_UUS
#define VOICEI_GET_CALL_UUS_INFO    (0x16)
#endif
#define VOICEI_GET_CALL_ALERTING    (0x17)
#define VOICEI_GET_CALL_ALPHA       (0x18)
#define VOICEI_GET_CALL_CONN_NUM_INFO       (0x19)
#define VOICEI_GET_CALL_DIAGNOSTICS       (0x1A)
#define VOICEI_GET_CALL_ALERTING_PATTERN  (0x1B)
#define VOICEI_GET_CALL_AUDIO_ATTRIB       (0x1C)
#define VOICEI_GET_CALL_VIDEO_ATTRIB       (0x1D)
#define VOICEI_GET_CALL_VS_VARIANT         (0x1E)
#define VOICEI_GET_CALL_SIP_URI            (0x1F)
#define VOICEI_GET_CALL_IS_SRVCC_CALL      (0x20)
#define VOICEI_GET_CALL_RP_NUM_EXT         (0x21)
#define VOICEI_GET_CALL_SECOND_ALPHA       (0x22)
#define VOICEI_GET_CALL_END_REASON_TEXT    (0x24)

#define VOICEI_SEND_FLASH_CALL_ID   (0x10)

#define VOICEI_START_CONT_DTMF_CALL_ID   (0x10)

#define VOICEI_STOP_CONT_DTMF_CALL_ID    (0x10)

#define VOICEI_DTMF_IND_PW              (0x10)
#define VOICEI_DTMF_IND_DIG_INT         (0x11)
#define VOICEI_DTMF_IND_IP_VOLUME       (0x12)

#define VOICEI_ALL_CALL_IND_RP_NUM                          (0x10)
#define VOICEI_ALL_CALL_IND_RP_NAME                         (0x11)
#define VOICEI_ALL_CALL_IND_ALERTING                        (0x12)
#define VOICEI_ALL_CALL_IND_SO                              (0x13)
#define VOICEI_ALL_CALL_IND_END_REASON                      (0x14)
#define VOICEI_ALL_CALL_IND_ALPHA                           (0x15)
#define VOICEI_ALL_CALL_IND_CONNECTED_NUM                   (0x16)
#define VOICEI_ALL_CALL_IND_DIAGNOSTICS                     (0x17)
#define VOICEI_ALL_CALL_IND_CALLED_PARTY_NUM                (0x18)
#define VOICEI_ALL_CALL_IND_REDIRECTING_PARTY_NUM           (0x19)
#define VOICEI_ALL_CALL_IND_ALERTING_PATTERN                (0x1A)
#define VOICEI_ALL_CALL_IND_AUDIO_ATTRIB                    (0x1B)
#define VOICEI_ALL_CALL_IND_VIDEO_ATTRIB                    (0x1C)
#define VOICEI_ALL_CALL_IND_VS_VARIANT                      (0x1D)
#define VOICEI_ALL_CALL_IND_SIP_URI                         (0x1E)
#define VOICEI_ALL_CALL_IND_IS_SRVCC_CALL                   (0x1F)
#define VOICEI_ALL_CALL_IND_PARENT_CALL_INFO                (0x20)
#define VOICEI_ALL_CALL_IND_LOCAL_CAP                       (0x21)
#define VOICEI_ALL_CALL_IND_PEER_CAP                        (0x22)
#define VOICEI_ALL_CALL_IND_CHILD_NUM                       (0x23)
#define VOICEI_ALL_CALL_IND_DISPLAY_TEXT                    (0x24)
#define VOICEI_ALL_CALL_IND_RP_NUM_EXT                      (0x25)
#define VOICEI_ALL_CALL_IND_CONNECTED_NUM_EXT               (0x26)
#define VOICEI_ALL_CALL_IND_MEDIA_ID                        (0x27)
#define VOICEI_ALL_CALL_IND_IS_ADDITIONAL_CALL_INFO_PRESENT (0x28)
#define VOICEI_ALL_CALL_IND_CALL_ATTRIB_STATUS              (0x29)
#define VOICEI_ALL_CALL_IND_ORIG_FAIL_REASON                (0x2A)
#define VOICEI_ALL_CALL_IND_RP_NUM_EXT2                     (0x2B)
#define VOICEI_ALL_CALL_IND_SECOND_ALPHA                    (0x2C)
#define VOICEI_ALL_CALL_IND_END_REASON_TEXT                 (0x2E)
#define VOICEI_ALL_CALL_IND_IP_CALLER_NAME                  (0x2D)
#define VOICEI_ALL_CALL_IND_SIP_ERROR_CODE                  (0x33)
#define VOICEI_ALL_CALL_IND_IS_CALLED_PARTY_RINGING         (0x34)
#define VOICEI_ALL_CALL_IND_IP_HIST_INFO                    (0x36)
#define VOICEI_ALL_CALL_IND_ALTERNATE_SIP_URIS               (0x3A)
#define VOICEI_ALL_CALL_IND_VERSTAT_INFO                    (0x3E)

#define VOICEI_GET_ALL_CALL_INFO                  (0x10)
#define VOICEI_GET_ALL_CALL_RP_NUM                (0x11)
#define VOICEI_GET_ALL_CALL_RP_NAME               (0x12)
#define VOICEI_GET_ALL_CALL_ALERTING              (0x13)
#ifdef FEATURE_UUS
#define VOICEI_GET_ALL_CALL_UUS_INFO              (0x14)
#endif
#define VOICEI_GET_ALL_CALL_SO                    (0x15)
#define VOICEI_GET_ALL_CALL_OTASP                 (0x16)
#define VOICEI_GET_ALL_CALL_VP                    (0x17)
#define VOICEI_GET_ALL_CALL_END_REASON            (0x18)
#define VOICEI_GET_ALL_CALL_ALPHA                 (0x19)
#define VOICEI_GET_ALL_CALL_CONNECTED_NUM         (0x1A)
#define VOICEI_GET_ALL_CALL_DIAGNOSTICS           (0x1B)
#define VOICEI_GET_ALL_CALL_CALLED_PARTY_NUM      (0x1C)
#define VOICEI_GET_ALL_CALL_REDIRECTING_PARTY_NUM (0x1D)
#define VOICEI_GET_ALL_CALL_ALERTING_PATTERN      (0x1E)
#define VOICEI_GET_ALL_CALL_AUDIO_ATTRIBUTE       (0x1F)
#define VOICEI_GET_ALL_CALL_VIDEO_ATTRIBUTE       (0x20)
#define VOICEI_GET_ALL_CALL_VS_VARIANT            (0x21)
#define VOICEI_GET_ALL_CALL_SIP_URI               (0x22)
#define VOICEI_GET_ALL_CALL_IS_SRVCC_CALL         (0x23)
#define VOICEI_GET_ALL_CALL_ATTRIB_STATUS         (0x24)
#define VOICEI_GET_ALL_CALL_RP_NUM_EXT            (0x25)
#define VOICEI_GET_ALL_CALL_SECOND_ALPHA          (0x26)
#define VOICEI_GET_ALL_CALL_END_REASON_TEXT       (0x28)
#define VOICEI_GET_ALL_CALL_SIP_ERROR_CODE        (0x2A)
#define VOICEI_GET_ALL_CALL_IP_HIST_INFO          (0x2C)
#define VOICEI_GET_ALL_CALL_ALTERNATE_SIP_URIS       (0x30)

#define VOICEI_MNG_CALLS_REQ_CALL_ID        (0x10)
#define VOICEI_MNG_CALLS_REQ_REJ_CAUSE        (0x11)

#define VOICEI_MNG_CALLS_FAILURE_CAUSE  (0x10)

#define VOICEI_SUPS_NOTIFY_IND_CUG                   (0x10)
#define VOICEI_SUPS_NOTIFY_IND_ECT                   (0x11)
#define VOICEI_SUPS_NOTIFY_IND_REASON                (0x12)
#define VOICEI_SUPS_NOTIFY_IND_IP_FORWARD_HIST_INFO  (0x13)

#ifdef FEATURE_MMODE_SS_SUPS
#define VOICEI_SET_SUPS_SC                   (0x10)
#define VOICEI_SET_SUPS_BARR_PWD             (0x11)
#define VOICEI_SET_SUPS_FWD_NUM              (0x12)
#define VOICEI_SET_SUPS_NR_TIMER             (0x13)
#define VOICEI_SET_SUPS_NUM_TYPE_PLAN        (0x14)
#define VOICEI_SET_SUPS_SC_EXT               (0x15)
#define VOICEI_SET_SUPS_BARR_NUM_LIST        (0x16)
#define VOICEI_SET_SUPS_COLR_PI              (0x17)
#define VOICEI_SET_SUPS_CALL_FWD_START_TIME  (0x18)
#define VOICEI_SET_SUPS_CALL_FWD_END_TIME    (0x19)
#endif

#define VOICEI_SET_SUPS_FAILURE_CAUSE     (0X10)
#define VOICEI_SET_SUPS_ALPHA_ID          (0X11)
#define VOICEI_SET_SUPS_CC_RESULT_TYPE    (0x12)
#define VOICEI_SET_SUPS_CC_RESULT_CALL_ID (0X13)
#define VOICEI_SET_SUPS_CC_RESULT_SUPS    (0X14)
#define VOICEI_SET_SUPS_SERVICE_STATUS    (0X15)
#define VOICEI_SET_SUPS_FAILURE_DESC      (0X16)
#define VOICEI_SET_SUPS_RETRY_DURATION    (0x17)

#define VOICEI_GET_CW_SC                (0x10)
#define VOICEI_GET_CW_REQ_SC_EXT        (0x11)

#define VOICEI_GET_CW_FAILURE_CAUSE     (0X11)
#define VOICEI_GET_CW_ALPHA_ID          (0X12)
#define VOICEI_GET_CW_CC_RESULT_TYPE    (0x13)
#define VOICEI_GET_CW_CC_RESULT_CALL_ID (0X14)
#define VOICEI_GET_CW_CC_RESULT_SUPS    (0X15)
#define VOICEI_GET_CW_RESP_SC_EXT       (0x16)
#define VOICEI_GET_CW_RETRY_DURATION    (0x17)

#define VOICEI_GET_CB_SC                (0x10)
#define VOICEI_GET_CB_REQ_SC_EXT             (0X11)

#define VOICEI_GET_CB_FAILURE_CAUSE     (0X11)
#define VOICEI_GET_CB_ALPHA_ID          (0X12)
#define VOICEI_GET_CB_CC_RESULT_TYPE    (0x13)
#define VOICEI_GET_CB_CC_RESULT_CALL_ID (0X14)
#define VOICEI_GET_CB_CC_RESULT_SUPS    (0X15)
#define VOICEI_GET_CB_RESP_SC_EXT       (0X16)
#define VOICEI_GET_CB_RESP_BARR_LIST    (0X17)
#define VOICEI_GET_CB_RETRY_DURATION    (0x18)
#define VOICEI_GET_CB_FAILURE_CAUSE_DESC  (0X19)
#define VOICEI_GET_CB_SIP_ERR_CODE        (0X1A)

#define VOICEI_GET_CLIP_RESP              (0x10)
#define VOICEI_GET_CLIP_FAILURE_CAUSE     (0X11)
#define VOICEI_GET_CLIP_ALPHA_ID          (0X12)
#define VOICEI_GET_CLIP_CC_RESULT_TYPE    (0x13)
#define VOICEI_GET_CLIP_CC_RESULT_CALL_ID (0X14)
#define VOICEI_GET_CLIP_CC_RESULT_SUPS    (0X15)
#define VOICEI_GET_CLIP_RETRY_DURATION    (0x16)

#define VOICEI_GET_COLP_RESP              (0x10)
#define VOICEI_GET_COLP_FAILURE_CAUSE     (0X11)
#define VOICEI_GET_COLP_ALPHA_ID          (0X12)
#define VOICEI_GET_COLP_CC_RESULT_TYPE    (0x13)
#define VOICEI_GET_COLP_CC_RESULT_CALL_ID (0X14)
#define VOICEI_GET_COLP_CC_RESULT_SUPS    (0X15)
#define VOICEI_GET_COLP_RETRY_DURATION    (0X16)

#define VOICEI_GET_CLIR_RESP              (0x10)
#define VOICEI_GET_CLIR_FAILURE_CAUSE     (0X11)
#define VOICEI_GET_CLIR_ALPHA_ID          (0X12)
#define VOICEI_GET_CLIR_CC_RESULT_TYPE    (0x13)
#define VOICEI_GET_CLIR_CC_RESULT_CALL_ID (0X14)
#define VOICEI_GET_CLIR_CC_RESULT_SUPS    (0X15)
#define VOICEI_GET_CLIR_RETRY_DURATION    (0x16)

#define VOICEI_GET_CF_SC                   (0x10) //Optional Request TLV
#define VOICEI_GET_CF_REQ_SC_EXT               (0x11) 

#define VOICEI_GET_CF_INFO                (0x10)
#define VOICEI_GET_CF_FAILURE_CAUSE       (0X11)
#define VOICEI_GET_CF_ALPHA_ID            (0X12)
#define VOICEI_GET_CF_CC_RESULT_TYPE      (0x13)
#define VOICEI_GET_CF_CC_RESULT_CALL_ID   (0X14)
#define VOICEI_GET_CF_CC_RESULT_SUPS      (0X15)
#define VOICEI_GET_CF_EXTEN_INFO          (0X16)
#define VOICEI_GET_CF_EXTEN2_INFO         (0X17)
#define VOICEI_GET_CF_RETRY_DURATION      (0X18)
#define VOICEI_GET_CF_PROVISION_STATUS    (0X19)
#define VOICEI_GET_CF_CALL_FWD_START_TIME (0x1A)
#define VOICEI_GET_CF_CALL_FWD_END_TIME   (0x1B)

#define VOICEI_GET_ALS_LINE_SWITCH_STATUS   (0x10)

#define VOICEI_GET_ALS_SELECTED_LINE        (0x10)

#define VOICEI_SET_CB_PWD_FAILURE_CAUSE     (0X10)
#define VOICEI_SET_CB_PWD_ALPHA_ID          (0X11)
#define VOICEI_SET_CB_PWD_CC_RESULT_TYPE    (0x12)
#define VOICEI_SET_CB_PWD_CC_RESULT_CALL_ID (0X13)
#define VOICEI_SET_CB_PWD_CC_RESULT_SUPS    (0X14)
#define VOICEI_SET_CB_PWD_RETRY_DURATION    (0x15)

#if defined(FEATURE_MMODE_USS_SUPS)
#define VOICEI_ORIG_USSD_FAILURE_CAUSE     (0X10)
#define VOICEI_ORIG_USSD_ALPHA_ID          (0X11)
#define VOICEI_ORIG_USSD_DATA              (0X12)
#define VOICEI_ORIG_USSD_CC_RESULT_TYPE    (0x13)
#define VOICEI_ORIG_USSD_CC_RESULT_CALL_ID  (0X14)
#define VOICEI_ORIG_USSD_CC_RESULT_SUPS     (0X15)
#define VOICEI_ORIG_USSD_DATA_UTF16         (0x16) 

#define VOICEI_USSD_IND_DATA               (0x10)
#define VOICEI_USSD_IND_DATA_UTF16         (0x11)
#endif
#define VOICEI_FLASH_PAYLOAD    (0x10)
#define VOICEI_FLASH_TYPE       (0x11)

#if defined(FEATURE_MMODE_USS_SUPS)

#define VOICEI_ORIG_USSD_NO_WAIT_ERROR          (0x10)
#define VOICEI_ORIG_USSD_NO_WAIT_FAILURE_CAUSE  (0x11)
#define VOICEI_ORIG_USSD_NO_WAIT_DATA           (0x12)
#define VOICEI_ORIG_USSD_NO_WAIT_ALPHA_ID       (0x13)
#define VOICEI_ORIG_USSD_NO_WAIT_DATA_UTF16     (0x14)
#endif
#define VOICEI_MODEM_SET_CONFIG_AUTO_ANSWER          (0x10)
#define VOICEI_MODEM_SET_CONFIG_AIR_TIMER            (0x11)
#define VOICEI_MODEM_SET_CONFIG_ROAM_TIMER           (0x12)
#define VOICEI_MODEM_SET_CONFIG_TTY_MODE             (0x13)
#define VOICEI_MODEM_SET_CONFIG_PREF_VOICE_SO        (0x14)
#define VOICEI_MODEM_SET_CONFIG_PREF_VOICE_DOMAIN    (0x15)
#define VOICEI_MODEM_SET_CONFIG_UI_TTY_SETTING       (0x16)

#define VOICEI_MODEM_GET_CONFIG_AUTO_ANSWER          (0x10)
#define VOICEI_MODEM_GET_CONFIG_AIR_TIMER            (0x11)
#define VOICEI_MODEM_GET_CONFIG_ROAM_TIMER           (0x12)
#define VOICEI_MODEM_GET_CONFIG_TTY_MODE             (0x13)
#define VOICEI_MODEM_GET_CONFIG_PREF_VOICE_SO        (0x14)
#define VOICEI_MODEM_GET_CONFIG_AMR_STATUS           (0x15)
#define VOICEI_MODEM_GET_CONFIG_VOICE_PRIVACY        (0x16)
#define VOICEI_MODEM_GET_CONFIG_NAM_ID               (0x17)
#define VOICEI_MODEM_GET_CONFIG_PREF_VOICE_DOMAIN    (0x18)
#define VOICEI_MODEM_GET_CONFIG_UI_TTY_SETTING       (0x19)

#define VOICEI_MODEM_GET_CONFIG_RESP_PREF_VOICE_DOMAIN    (0x17)
#define VOICEI_MODEM_GET_CONFIG_RESP_UI_TTY_SETTING       (0x18)

#define VOICEI_BIND_SUBSCRIPTION             (0x10)

#define VOICEI_MNG_IP_CALLS_CALL_ID        (0x10)
#define VOICEI_MNG_IP_CALLS_CALL_TYPE      (0x11)
#define VOICEI_MNG_IP_CALLS_AUDIO_ATTRIB   (0x12)
#define VOICEI_MNG_IP_CALLS_VIDEO_ATTRIB   (0x13)
#define VOICEI_MNG_IP_CALLS_SIP_URI        (0x14)
#define VOICEI_MNG_IP_CALLS_SIP_REJECT_CAUSE       (0x15)
#define VOICEI_MNG_IP_CALLS_SIP_REJECT_CODE    (0x16)
#define VOICEI_MNG_IP_CALLS_SPEECH_CODEC      (0x17)

#define VOICEI_MNG_IP_CALLS_RESP_FAILURE_CAUSE        (0x11)

#define VOICEI_GET_COLR_RESP              (0x10)
#define VOICEI_GET_COLR_FAILURE_CAUSE     (0X11)
#define VOICEI_GET_COLR_ALPHA_ID          (0X12)
#define VOICEI_GET_COLR_CC_RESULT_TYPE    (0x13)
#define VOICEI_GET_COLR_CC_RESULT_CALL_ID (0X14)
#define VOICEI_GET_COLR_CC_RESULT_SUPS    (0X15)
#define VOICEI_GET_COLR_PI                (0x16)
#define VOICEI_GET_COLR_RETRY_DURATION    (0x17)

#define VOICEI_MODIFIED_CALL_ID            (0x01)
#define VOICEI_MODIFIED_CALL_TYPE          (0x10)
#define VOICEI_MODIFIED_CALL_AUDIO_ATTRIB  (0x11)
#define VOICEI_MODIFIED_CALL_VIDEO_ATTRIB  (0x12)
#define VOICEI_MODIFIED_CALL_FAILURE_CAUSE (0x13)
#define VOICEI_MODIFIED_CALL_MEDIA_ID      (0x14)
#define VOICEI_MODIFIED_CALL_ATTRIB_STATUS (0x15)

#define VOICEI_MODIFY_ACCEPT_CALL_ID   (0x01)
#define VOICEI_MODIFY_ACCEPT_CALL_TYPE   (0x10)
#define VOICEI_MODIFY_ACCEPT_AUDIO_ATTRIB   (0x11)
#define VOICEI_MODIFY_ACCEPT_VIDEO_ATTRIB   (0x12)

#define VOICEI_GET_CNAP_RESP              (0x10)
#define VOICEI_GET_CNAP_FAILURE_CAUSE     (0X11)
#define VOICEI_GET_CNAP_ALPHA_ID          (0X12)
#define VOICEI_GET_CNAP_CC_RESULT_TYPE    (0x13)
#define VOICEI_GET_CNAP_CC_RESULT_CALL_ID (0X14)
#define VOICEI_GET_CNAP_CC_RESULT_SUPS    (0X15)
#define VOICEI_GET_CNAP_RETRY_DURATION    (0x16)

#define VOICEI_AOC_GET_CALL_METER_INFO_ACM                (0x10)
#define VOICEI_AOC_GET_CALL_METER_INFO_ACMMAX             (0x11)
#define VOICEI_AOC_GET_CALL_METER_INFO_CCM                (0x12)

#define VOICEI_SRVCC_CALL_CONFIG_REQ_ALERTING_TYPE    (0x10)


/*---------------------------------------------------------------------------
  Response TLVs
---------------------------------------------------------------------------*/

#define VOICEI_INFO_REC_CALL_ID  (0x01)
#define VOICEI_CALL_STATUS       (0x01)
#define VOICEI_DTMF_INFORMATION  (0x01)
#define VOICEI_SIGNAL_INFO       (0x10)
#define VOICEI_CALLER_ID         (0x11)
#define VOICEI_DISPLAY_INFO      (0x12)
#define VOICEI_EXT_DISPLAY_INFO  (0x13)
#define VOICEI_CALLER_NAME_INFO  (0x14)
#define VOICEI_CALL_WAITING_IND  (0x15)
#define VOICEI_CONN_NUM_INFO     (0x16)
#define VOICEI_CALLING_PARTY_NUM_INFO (0x17)
#define VOICEI_CALLED_PARTY_NUM_INFO  (0x18)
#define VOICEI_REDIRECT_NUM_INFO  (0x19)
#define VOICEI_NSS_CLIR_INFO  (0x1A)
#define VOICEI_NSS_AUD_CTRL_INFO  (0x1B)
#define VOICEI_NSS_RELEASE_INFO  (0x1C)
#define VOICEI_LINE_CTRL_INFO  (0x1D)
#define VOICEI_EXT_DISPLAY_INFO_RECORD  (0x1E)


#define VOICEI_OTASP_STATUS      (0x01)

#define VOICEI_PRIVACY_INFO                 (0x01)
#define VOICEI_CALL_END_REASON              (0x10)
#define VOICEI_CALL_TYPE                    (0x11)
#define VOICEI_ALL_CALL_STATUS              (0x01)
#define VOICEI_SUPS_NOTIFY_INFO             (0x01)
#define VOICEI_USSD_NOTIFY_TYPE          (0x01)
#define VOICEI_UUS_IND_DATA               (0x01)

#define VOICEI_SUPS_IND_SERVICE_INFO                 (0x01)
#define VOICEI_SUPS_IND_SERVICE_CLASS               (0x10)
#define VOICEI_SUPS_IND_REASON                            (0x11)
#define VOICEI_SUPS_IND_CFW_NUM                        (0x12)
#define VOICEI_SUPS_IND_CFW_NR_TIMER               (0x13)
#define VOICEI_SUPS_IND_USS_INFO                        (0x14)
#define VOICEI_SUPS_IND_CALL_ID                           (0x15)
#define VOICEI_SUPS_IND_ALPHA                              (0x16)
#define VOICEI_SUPS_IND_PWD                                 (0x17)
#define VOICEI_SUPS_IND_NEW_PWD                        (0x18)
#define VOICEI_SUPS_IND_SUPS_DATA_SOURCE                (0x19)
#define VOICEI_SUPS_IND_FAILURE_CAUSE                   (0x1A)
#define VOICEI_SUPS_IND_CF_NTWK_DATA                    (0x1B)
#define VOICEI_SUPS_IND_CLIR_STATUS                     (0x1C)
#define VOICEI_SUPS_IND_CLIP_STATUS                     (0x1D)
#define VOICEI_SUPS_IND_COLP_STATUS                     (0x1E)
#define VOICEI_SUPS_IND_COLR_STATUS                     (0x1F)
#define VOICEI_SUPS_IND_CNAP_STATUS                     (0x20)
#define VOICEI_SUPS_IND_USS_INFO_UTF16                  (0x21)
#define VOICEI_SUPS_IND_SERVICE_CLASS_EXT               (0x22)
#define VOICEI_SUPS_IND_BARR_LIST                       (0x23)

#define VOICEI_SPEECH_CODEC_INFO_NW_MODE               (0x010)
#define VOICEI_SPEECH_CODEC_INFO_CODEC_TYPE            (0x011)
#define VOICEI_SPEECH_CODEC_INFO_SAMPLE_RATE           (0x012)
#define VOICEI_SPEECH_CODEC_INFO_CALL_ID               (0x013)

#define VOICEI_HANDOVER_STATE                         (0x01)
#define VOICEI_HANDOVER_TYPE                          (0x10)

#define VOICEI_EXT_BRST_INTL_DATA                         (0x01)

#define VOICEI_CONFERENCE_CONF_INFO_XML_DATA          (0x01)
#define VOICEI_CONFERENCE_CONF_INFO_SEQUENCE_NUM          (0x02)
#define VOICEI_CONFERENCE_CONF_INFO_TOTAL_XML_FILE_LENGTH          (0x10)
#define VOICEI_CONFERENCE_CONF_INFO_CALL_ID          (0x11)

#define VOICEI_CONFERENCE_JOIN_CALL_ID                    (0x01)
#define VOICEI_CONFERENCE_JOIN_PARTICIPANT_INFO           (0x02)

#define VOICEI_CONFERENCE_PARTICIPANT_UPDATE_INFO       (0x01)

#define VOICEI_CONFERENCE_PARTICIPANTS_INFO             (0x01)

#define VOICEI_PAGE_MISS_REASON                         (0x01)

#define VOICEI_CC_RESULT_INFO_RESULT                    (0x01)
#define VOICEI_CC_RESULT_INFO_ALPHA_PRESENCE            (0x02)
#define VOICEI_CC_RESULT_INFO_ALPHA_GSM8                (0x10)
#define VOICEI_CC_RESULT_INFO_ALPHA_UTF16               (0x11)

#define VOICEI_TTY_MODE             (0x01)

#define VOICEI_AUDIO_RAT_CHANGE_AUDIO_SESSION_INFO   (0x10)
#define VOICEI_AUDIO_RAT_CHANGE_RAT_INFO             (0x11)

#define VOICEI_CONF_PART_STATUS_INFO_CALL_ID             (0x01)
#define VOICEI_CONF_PART_STATUS_INFO_PART_ID             (0x02)
#define VOICEI_CONF_PART_STATUS_INFO_PART_OP_INFO             (0x10)


#define VOICEI_E911_ORIG_FAIL       (0x01)

#define VOICEI_ADDITIONAL_CALL_INFO_CALL_ID       (0x01)
#define VOICEI_ADDITIONAL_CALL_INFO_EXT_INFO_DATA          (0x10)

#define VOICEI_SPEECH_CODEC_INFO_REQ_CALL_ID               (0x010)
#define VOICEI_SPEECH_CODEC_INFO_REQ_NW_MODE               (0x011)
#define VOICEI_SPEECH_CODEC_INFO_REQ_CODEC_TYPE            (0x012)
#define VOICEI_SPEECH_CODEC_INFO_REQ_SAMPLE_RATE           (0x013)




/*---------------------------------------------------------------------------
  Macro used in command handlers (common)
---------------------------------------------------------------------------*/
#define CHECK_RETVAL()  if (FALSE == retval) { dsm_free_packet(&response); \
                                               return NULL; }

#define VOICEI_TLV_TAG_SIZE     1  /* in bytes */
#define VOICEI_TLV_LENGTH_SIZE  2  /* in bytes */

#define QMI_VOICEI_PULL(sdu_in, value, siz) (siz == dsm_pullup( sdu_in, value, siz ))

/*===========================================================================

                                DATA TYPES

===========================================================================*/


/*---------------------------------------------------------------------------
  VOICE Command enum type - not equal to the actual command values!
  mapping is in qmi_voice_cmd_callbacks table

  DO NOT REORDER THIS ENUM!
---------------------------------------------------------------------------*/
typedef enum
{
  VOICEI_CMD_MIN                      = 0,
  VOICEI_CMD_INDICATION_REGISTER      = VOICEI_CMD_MIN,
  VOICEI_CMD_DIAL_CALL                ,
  VOICEI_CMD_END_CALL                 ,
  VOICEI_CMD_ANSWER_CALL              ,
  VOICEI_CMD_SETUP_ANSWER             ,
  VOICEI_CMD_GET_CALL_INFO            ,
  VOICEI_CMD_SEND_FLASH               ,
  VOICEI_CMD_BURST_DTMF               ,
  VOICEI_CMD_START_CONT_DTMF          ,
  VOICEI_CMD_STOP_CONT_DTMF           ,
  VOICEI_CMD_SET_PREFERRED_PRIVACY    ,
  VOICEI_CMD_SUPS_MNG_CALLS,
  #ifndef FEATURE_MMODE_SS_SUPS
  VOICEI_CMD_SUPS_GET_CLIP,
  VOICEI_CMD_SUPS_GET_COLP,
  VOICEI_CMD_SUPS_GET_CALL_WAITING,
  VOICEI_CMD_SUPS_GET_CALL_BARRING,
  VOICEI_CMD_SUPS_SET_CALL_BARRING,
  VOICEI_CMD_GET_CLIR,
  VOICEI_CMD_GET_CALL_FORWARDING,
  #endif

  VOICEI_CMD_GET_ALL_CALL_INFO ,

  #ifndef FEATURE_MMODE_SS_SUPS
  VOICEI_CMD_SUPS_SET_SUPS_SERVICE,
  #endif
#if defined (FEATURE_MMODE_USS_SUPS)

  VOICEI_CMD_SUPS_ORIG_USSD,
  VOICEI_CMD_SUPS_ANSWER_USSD,
  VOICEI_CMD_SUPS_CANCEL_USSD,
  #endif
  VOICEI_CMD_SET_CONFIG_ITEMS,
  VOICEI_CMD_GET_CONFIG_ITEMS,
  #ifdef FEATURE_MMODE_SS_SUPS
  VOICEI_CMD_SUPS_GET_COLR,
  #endif
  VOICEI_CMD_BIND_SUBSCRIPTION,
#if defined(FEATURE_MMODE_USS_SUPS)

  VOICEI_CMD_SUPS_ORIG_USSD_NO_WAIT,
  #endif
  VOICEI_CMD_ALS_SET_LINE_SWITCHING,
  VOICEI_CMD_ALS_SELECT_LINE,
  VOICEI_CMD_AOC_RESET_ACM                   ,
  VOICEI_CMD_AOC_SET_ACMMAX                  ,
  VOICEI_CMD_AOC_GET_CALL_METER_INFO         ,
  VOICEI_CMD_SUPS_MANAGE_IP_CALLS,
  VOICEI_CMD_ALS_GET_LINE_SWITCHING_STATUS,
  VOICEI_CMD_ALS_GET_SELECTED_LINE,
  VOICEI_CMD_SUPS_GET_CNAP,
  VOICEI_CMD_SRVCC_CALL_CONFIG_REQ,
  VOICEI_CMD_GET_SPEECH_CODEC_INFO,
  VOICEI_CMD_MAX, 
  VOICEI_CMD_WIDTH                    = 0xFFFF          
} qmi_voicei_cmd_e_type;

typedef enum
{
  VOICEI_CMD_VAL_INDICATION_REGISTER    = 0x0003,
  VOICEI_CMD_VAL_DIAL_CALL              = 0x0020,
  VOICEI_CMD_VAL_END_CALL               = 0x0021,
  VOICEI_CMD_VAL_ANSWER_CALL            = 0x0022,
  VOICEI_CMD_VAL_GET_CALL_INFO          = 0x0024,
  VOICEI_CMD_VAL_OTASP_STATUS           = 0x0025,
  VOICEI_CMD_VAL_INFO_REC_IND           = 0x0026,
  VOICEI_CMD_VAL_SEND_FLASH             = 0x0027,
  VOICEI_CMD_VAL_BURST_DTMF             = 0x0028,
  VOICEI_CMD_VAL_START_CONT_DTMF        = 0x0029,
  VOICEI_CMD_VAL_STOP_CONT_DTMF         = 0x002A,
  VOICEI_CMD_VAL_DTMF_IND               = 0x002B,
  VOICEI_CMD_VAL_SET_PREFERRED_PRIVACY  = 0x002C,
  VOICEI_CMD_VAL_PRIVACY_IND            = 0x002D,
  VOICEI_CMD_VAL_ALL_CALL_STATUS_IND    = 0x002E,
  VOICEI_CMD_VAL_GET_ALL_CALL_INFO      = 0x002F,
  /*Supplementary services API's*/
  VOICEI_CMD_VAL_SUPS_MANAGE_CALLS      = 0x0031,
 
 #if defined (FEATURE_MMODE_SS_SUPS) 
  VOICEI_CMD_VAL_SUPS_NOTIFICATION_IND  = 0x0032,
  VOICEI_CMD_VAL_SUPS_SET_SUPS_SERVICE  = 0x0033,
  VOICEI_CMD_VAL_SUPS_GET_CALL_WAITING  = 0x0034,
  VOICEI_CMD_VAL_SUPS_GET_CALL_BARRING  = 0x0035,
  VOICEI_CMD_VAL_SUPS_GET_CLIP          = 0x0036,
  VOICEI_CMD_VAL_SUPS_GET_CLIR             = 0x0037,
  VOICEI_CMD_VAL_SUPS_GET_CALL_FORWARDING  = 0x0038,
  VOICEI_CMD_VAL_SUPS_SET_CALL_BARRING  = 0x0039,
  #endif
 #if defined FEATURE_MMODE_USS_SUPS 
  VOICEI_CMD_VAL_SUPS_ORIG_USSD            = 0x003A,
  VOICEI_CMD_VAL_SUPS_ANSWER_USSD          = 0x003B,
  VOICEI_CMD_VAL_SUPS_CANCEL_USSD          = 0x003C,
  VOICEI_CMD_VAL_SUPS_USSD_RELEASE_IND     = 0x003D,
  VOICEI_CMD_VAL_SUPS_USSD_IND             = 0x003E,
  #endif
  VOICEI_CMD_VAL_SUPS_UUS_IND             = 0x003F,
  /*VOICE NV services API's*/
  VOICEI_CMD_VAL_SET_CONFIG_ITEMS             = 0x0040,
  VOICEI_CMD_VAL_GET_CONFIG_ITEMS             = 0x0041,
  VOICEI_CMD_VAL_SUPS_IND             = 0x0042,
  #if defined(FEATURE_MMODE_USS_SUPS)
  VOICEI_CMD_VAL_SUPS_ORIG_USSD_NO_WAIT       = 0x0043,
  VOICEI_CMD_VAL_SUPS_ORIG_USSD_NO_WAIT_IND   = 0x0043,
  #endif
  /*Voice DSDS API's*/
  VOICEI_CMD_VAL_BIND_SUBSCRIPTION             = 0x0044,
  VOICEI_CMD_VAL_ALS_SET_LINE_SWITCHING    = 0x0045,
  VOICEI_CMD_VAL_ALS_SELECT_LINE           = 0x0046,
  VOICEI_CMD_VAL_AOC_RESET_ACM                        = 0x0047,
  VOICEI_CMD_VAL_AOC_SET_ACMMAX                       = 0x0048,
  VOICEI_CMD_VAL_AOC_GET_CALL_METER_INFO              = 0x0049,
  VOICEI_CMD_VAL_AOC_LOW_FUNDS_IND                    = 0x004A,
  #ifdef FEATURE_MMODE_SS_SUPS
  VOICEI_CMD_VAL_SUPS_GET_COLP          = 0x004B,
  VOICEI_CMD_VAL_SUPS_GET_COLR             = 0x004C,
  #endif
  #ifdef FEATURE_MMODE_CALL_SUPS
  VOICEI_CMD_VAL_SUPS_MANAGE_IP_CALLS      = 0x004E,
  #endif
  VOICEI_CMD_VAL_ALS_GET_LINE_SWITCHING_STATUS    = 0x004F,
  VOICEI_CMD_VAL_ALS_GET_SELECTED_LINE           = 0x0050,
  VOICEI_CMD_VAL_CALL_MODIFIED_IND             = 0x0051,
  VOICEI_CMD_VAL_CALL_MODIFY_ACCEPT_IND             = 0x0052,
  VOICEI_CMD_VAL_SPEECH_CODEC_INFO_IND      = 0x0053,
  VOICEI_CMD_VAL_SUPS_GET_CNAP           = 0x004D,
  VOICEI_CMD_VAL_HANDOVER_IND           = 0x0054,
  VOICEI_CMD_VAL_CONF_INFO_IND          = 0x0055,
  VOICEI_CMD_VAL_CONF_JOIN_IND          = 0x0056,
  VOICEI_CMD_VAL_CONF_PARTICIPANT_UPDATE_IND          = 0x0057,
   VOICEI_CMD_VAL_EXT_BRST_INTL_IND           = 0x0058,
  VOICEI_CMD_VAL_MT_PAGE_MISS_IND       = 0x0059,
  VOICEI_CMD_VAL_CC_RESULT_INFO_IND      = 0x005A,
  VOICEI_CMD_VAL_CONFERENCE_PARTICIPANTS_INFO_IND  = 0x005B,
  VOICEI_CMD_VAL_SETUP_ANSWER           = 0x005C,
  VOICEI_CMD_VAL_TTY_IND  = 0x005D,
  VOICEI_CMD_VAL_ADDITIONAL_CALL_INFO_IND  = 0x0062,
    VOICEI_CMD_VAL_AUDIO_RAT_CHANGE_INFO_IND  = 0x0063,
  VOICEI_CMD_VAL_SET_SRVCC_CALL_CONFIG      = 0x0064,
  VOICEI_CMD_VAL_E911_ORIG_FAIL_IND  = 0x0065,
  VOICEI_CMD_VAL_CONF_PART_STATUS_IND  = 0x0066,  
  VOICEI_CMD_VAL_GET_SPEECH_CODEC_INFO = 0x006E,
/* QC EXTERNAL QMI COMMAND RANGE IS 0x0000 - 0x5555. 
   Add the next external QMI Command here */

/* VENDOR SPECIFIC QMI COMMAND RANGE IS 0x5556 - 0xAAAA.
   IMPORTANT!
   Add the vendor specific QMI Commands within this range only to avoid 
   conflicts with QC QMI commands that would get released in future */

/* RESERVED QC QMI COMMAND RANGE IS 0xAAAB - 0xFFFE 
   Internal QMI Commands must be added in DECREASING ORDER from Below */
  VOICEI_CMD_VAL_WIDTH                  = 0xFFFF
} qmi_voicei_cmd_val_e_type;


/*---------------------------------------------------------------------------
  QMI VOICE client state definition
---------------------------------------------------------------------------*/
typedef struct qmi_voicei_client_state_s
{

  q_link_type       link; /*< Data Structure required by the queue 
                              library to add this as part of a queue */
                                
  qmi_client_handle            clnt;   /* QCSI Client Handle */
  
  struct
  {
    boolean                      reg_dtmf_events;
    boolean                      reg_voice_privacy_events;
    boolean                      reg_sups_events;
    boolean                      reg_call_events;
    boolean                      reg_handover_events;
    boolean                      reg_speech_events;
    boolean                      reg_ussd_notification_events;    
    boolean                      reg_modification_events;
    boolean                      reg_uus_events;   
    boolean                      reg_aoc_events;
    boolean                      reg_conference_events;
    boolean                      reg_brst_intl_events;
    boolean                      reg_mt_page_miss_events;
    boolean                      reg_cc_result_events;
    boolean                      reg_conf_participants_events;
    boolean                      reg_tty_info_events;
    boolean                      reg_audio_rat_change_events;
    boolean                      reg_e911_orig_fail_events;
    boolean                      reg_add_call_info_events;
    boolean                      reg_vice_dialog_info_events;
    boolean                      reg_mt_call_end_auto_reject_events;
  }voice_info;
  struct
  {
    boolean is_bind_set_by_client;
    uint8 as_id;
  }asubs_info;
} qmi_voicei_client_state_type;


/* QMI Voice state */
static qmi_mmode_svc_info_type  qmi_voice_state;

static qmi_mmode_svc_config_type  qmi_voicei_cfg;

struct qmi_voice_e911_orig_fail_ind_s_type
{
  PACK(struct) qmi_voice_e911_orig_fail_ind_s 
  {
    uint8     call_id;
    uint16              end_reason;
  }e911_orig_fail_ind;
};

/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

qmi_idl_service_object_type voice_svc_obj;

/*---------------------------------------------------------------------------
  QMI service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/

#ifdef _WIN32
#define VOICE_HDLR_PROTOTYPE(x)   dsm_item_type* x( qmi_mmode_svc_info_type*, qmi_mmode_qcsi_transaction_type*, void*, dsm_item_type** )
#else
#define VOICE_HDLR_PROTOTYPE(x)   dsm_item_type* x( qmi_mmode_svc_info_type*, qmi_mmode_qcsi_transaction_type*, void*, dsm_item_type** )__attribute__((unused))
#endif

static VOICE_HDLR_PROTOTYPE(qmi_voicei_dial_call);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_end_call);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_answer_call);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_setup_answer);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_get_call_info);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_get_all_call_info);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_indication_register);

#ifndef FEATURE_MMODE_DISABLE_INBAND_SUPP
static VOICE_HDLR_PROTOTYPE(qmi_voicei_start_cont_dtmf);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_stop_cont_dtmf);
#endif

/*Supplementary services*/
#ifdef FEATURE_MMODE_SS_SUPS
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_get_clir);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_get_call_forwarding);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_get_clip);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_get_colp);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_get_call_waiting);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_get_call_barring);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_set_call_barring);
#endif

#if defined (FEATURE_MMODE_CALL_SUPS)
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_manage_calls);
#endif
#ifdef FEATURE_MMODE_SS_SUPS
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_set_sups_service);
#endif

#if defined FEATURE_MMODE_USS_SUPS
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_orig_ussd);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_answer_ussd);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_cancel_ussd);
#endif

#if defined (FEATURE_MMODE_SS_SUPS)
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_get_colr);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_get_cnap);
#endif

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
static VOICE_HDLR_PROTOTYPE(qmi_voicei_srvcc_call_config_req);
#endif

/*Voice NV services*/
static VOICE_HDLR_PROTOTYPE(qmi_voicei_set_modem_config);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_get_modem_config);

#if defined(FEATURE_MMODE_USS_SUPS)
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_orig_ussd_no_wait);
#endif
#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
/*Voice DSDS**/
static VOICE_HDLR_PROTOTYPE(qmi_voicei_bind_subscription);
#endif
#if (defined FEATURE_ALS) && (defined FEATURE_GSM)

/* ALS services*/
static VOICE_HDLR_PROTOTYPE(qmi_voicei_als_set_line_switching);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_als_select_line);
#endif

/*AoC services*/
static VOICE_HDLR_PROTOTYPE(qmi_voicei_aoc_reset_acm);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_aoc_set_acmmax);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_aoc_get_call_meter_info);

#if (defined FEATURE_ALS) && (defined FEATURE_GSM)
static VOICE_HDLR_PROTOTYPE(qmi_voicei_als_get_line_switching_status);
static VOICE_HDLR_PROTOTYPE(qmi_voicei_als_get_selected_line);
#endif

#ifdef FEATURE_MMODE_CALL_SUPS
/* VoIP calls */
static VOICE_HDLR_PROTOTYPE(qmi_voicei_sups_manage_ip_calls);
#endif
static VOICE_HDLR_PROTOTYPE(qmi_voicei_get_speech_codec_info);
#define VOICE_HDLR(a,b)  QMI_MMODE_SVC_RAW_MSG_HDLR( a, (qmi_mmode_svc_raw_msg_hdlr_ftype)b )

static qmi_mmode_svc_cmd_hdlr_type  qmi_voicei_cmd_callbacks[VOICEI_CMD_MAX] =
{   
  #ifndef REMOVE_QMI_VOICE_INDICATION_REGISTER_V02
  VOICE_HDLR( VOICEI_CMD_VAL_INDICATION_REGISTER, qmi_voicei_indication_register ),
  #endif
  #ifndef REMOVE_QMI_VOICE_DIAL_CALL_V02
  VOICE_HDLR( VOICEI_CMD_VAL_DIAL_CALL, qmi_voicei_dial_call ),
  #endif
  #ifndef REMOVE_QMI_VOICE_END_CALL_V02
  VOICE_HDLR( VOICEI_CMD_VAL_END_CALL, qmi_voicei_end_call ),
  #endif
  #ifndef REMOVE_QMI_VOICE_ANSWER_CALL_V02
  VOICE_HDLR( VOICEI_CMD_VAL_ANSWER_CALL, qmi_voicei_answer_call ),
  #endif
  #ifndef REMOVE_QMI_VOICE_SETUP_ANSWER_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SETUP_ANSWER, qmi_voicei_setup_answer ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_CALL_INFO_V02
  VOICE_HDLR( VOICEI_CMD_VAL_GET_CALL_INFO, qmi_voicei_get_call_info ),
  #endif
  #ifndef REMOVE_QMI_VOICE_START_CONT_DTMF_V02
  VOICE_HDLR( VOICEI_CMD_VAL_START_CONT_DTMF, qmi_voicei_start_cont_dtmf ),
  #endif
  #ifndef REMOVE_QMI_VOICE_STOP_CONT_DTMF_V02
  VOICE_HDLR( VOICEI_CMD_VAL_STOP_CONT_DTMF, qmi_voicei_stop_cont_dtmf ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_ALL_CALL_INFO_V02
  VOICE_HDLR( VOICEI_CMD_VAL_GET_ALL_CALL_INFO, qmi_voicei_get_all_call_info ),
  #endif
  #ifdef FEATURE_MMODE_CALL_SUPS
  /* Sups command handlers */
  #ifndef REMOVE_QMI_VOICE_MANAGE_CALLS_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_MANAGE_CALLS, qmi_voicei_sups_manage_calls ),
  #endif
  #endif
  #ifdef FEATURE_MMODE_SS_SUPS
  #ifndef REMOVE_QMI_VOICE_GET_CLIP_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_GET_CLIP, qmi_voicei_sups_get_clip ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_CALL_WAITING_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_GET_CALL_WAITING, qmi_voicei_sups_get_call_waiting ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_CALL_BARRING_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_GET_CALL_BARRING, qmi_voicei_sups_get_call_barring ),
  #endif
  #ifndef REMOVE_QMI_VOICE_SET_CALL_BARRING_PASSWORD_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_SET_CALL_BARRING, qmi_voicei_sups_set_call_barring ),
  #endif
  #ifndef REMOVE_QMI_VOICE_SET_SUPS_SERVICE_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_SET_SUPS_SERVICE, qmi_voicei_sups_set_sups_service ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_CLIR_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_GET_CLIR, qmi_voicei_sups_get_clir ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_CALL_FORWARDING_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_GET_CALL_FORWARDING, qmi_voicei_sups_get_call_forwarding ),
  #endif
  #endif
  #ifdef FEATURE_MMODE_USS_SUPS
  #ifndef REMOVE_QMI_VOICE_ORIG_USSD_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_ORIG_USSD, qmi_voicei_sups_orig_ussd ),
  #endif
  #ifndef REMOVE_QMI_VOICE_ANSWER_USSD_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_ANSWER_USSD, qmi_voicei_sups_answer_ussd ),
  #endif
  #ifndef REMOVE_QMI_VOICE_CANCEL_USSD_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_CANCEL_USSD, qmi_voicei_sups_cancel_ussd ),
  #endif
  #endif
  /* NV command handlers */
  #ifndef REMOVE_QMI_VOICE_SET_CONFIG_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SET_CONFIG_ITEMS, qmi_voicei_set_modem_config ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_CONFIG_V02
  VOICE_HDLR( VOICEI_CMD_VAL_GET_CONFIG_ITEMS, qmi_voicei_get_modem_config ),
  #endif
  #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
  #ifndef REMOVE_QMI_VOICE_BIND_SUBSCRIPTION_V02
  VOICE_HDLR( VOICEI_CMD_VAL_BIND_SUBSCRIPTION, qmi_voicei_bind_subscription ),
  #endif
  #endif
  #ifndef REMOVE_QMI_VOICE_ORIG_USSD_NO_WAIT_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_ORIG_USSD_NO_WAIT, qmi_voicei_sups_orig_ussd_no_wait ),
  #endif
  #ifndef REMOVE_QMI_VOICE_ALS_SET_LINE_SWITCHING_V02
  VOICE_HDLR( VOICEI_CMD_VAL_ALS_SET_LINE_SWITCHING, qmi_voicei_als_set_line_switching ),
  #endif
  #ifndef REMOVE_QMI_VOICE_ALS_SELECT_LINE_V02
  VOICE_HDLR( VOICEI_CMD_VAL_ALS_SELECT_LINE, qmi_voicei_als_select_line ),
  #endif
  #ifndef REMOVE_QMI_VOICE_AOC_RESET_ACM_V02
  VOICE_HDLR( VOICEI_CMD_VAL_AOC_RESET_ACM, qmi_voicei_aoc_reset_acm ),
  #endif
  #ifndef REMOVE_QMI_VOICE_AOC_SET_ACMMAX_V02
  VOICE_HDLR( VOICEI_CMD_VAL_AOC_SET_ACMMAX, qmi_voicei_aoc_set_acmmax ),
  #endif
  #ifndef REMOVE_QMI_VOICE_AOC_GET_CALL_METER_INFO_V02
  VOICE_HDLR( VOICEI_CMD_VAL_AOC_GET_CALL_METER_INFO, qmi_voicei_aoc_get_call_meter_info ),
  #endif
  #ifdef FEATURE_MMODE_CALL_SUPS
  #ifndef REMOVE_QMI_VOICE_MANAGE_IP_CALLS_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_MANAGE_IP_CALLS, qmi_voicei_sups_manage_ip_calls ),
  #endif
  #endif
  #ifdef FEATURE_MMODE_SS_SUPS
  #ifndef REMOVE_QMI_VOICE_GET_COLP_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_GET_COLP, qmi_voicei_sups_get_colp ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_COLR_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_GET_COLR, qmi_voicei_sups_get_colr ),
  #endif
  #endif
  #ifndef REMOVE_QMI_VOICE_ALS_GET_LINE_SWITCHING_STATUS_V02
  VOICE_HDLR( VOICEI_CMD_VAL_ALS_GET_LINE_SWITCHING_STATUS, qmi_voicei_als_get_line_switching_status ),
  #endif
  #ifndef REMOVE_QMI_VOICE_ALS_GET_SELECTED_LINE_V02
  VOICE_HDLR( VOICEI_CMD_VAL_ALS_GET_SELECTED_LINE, qmi_voicei_als_get_selected_line ),
  #endif
  #ifdef FEATURE_MMODE_SS_SUPS
  #ifndef REMOVE_QMI_VOICE_GET_CNAP_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SUPS_GET_CNAP, qmi_voicei_sups_get_cnap ),
  #endif
  #endif
  #ifndef REMOVE_QMI_VOICE_SRVCC_CALL_CONFIG_V02
  VOICE_HDLR( VOICEI_CMD_VAL_SET_SRVCC_CALL_CONFIG, qmi_voicei_srvcc_call_config_req ),
  #endif
  #ifndef REMOVE_QMI_VOICE_GET_SPEECH_CODEC_INFO_V02
  VOICE_HDLR( VOICEI_CMD_VAL_GET_SPEECH_CODEC_INFO, qmi_voicei_get_speech_codec_info),
  #endif

};




/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS 

===========================================================================*/

void qmi_voicei_fw_init_cback_hdlr( void );

static qmi_csi_cb_error qmi_voice_qcsi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
);

static void qmi_voice_qcsi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
);

static void qmi_voicei_dial_call_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_end_call_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_answer_call_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_setup_answer_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_get_call_info_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);


#ifndef FEATURE_MMODE_DISABLE_INBAND_SUPP
static void qmi_voicei_burst_dtmf_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_start_cont_dtmf_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_stop_cont_dtmf_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif

static void qmi_voicei_get_all_call_info_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

#ifndef FEATURE_MMODE_DISABLE_INBAND_SUPP
static void qmi_voicei_dtmf_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif

static void qmi_voicei_info_rec_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_notification_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_all_call_status_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

#if defined(FEATURE_MMODE_USS_SUPS)
static void qmi_voicei_sups_ussd_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_ussd_release_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif
static void qmi_voicei_uus_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#if defined(FEATURE_MMODE_SS_SUPS)
static void qmi_voicei_sups_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif
#if defined(FEATURE_MMODE_USS_SUPS)
static void qmi_voicei_sups_orig_ussd_no_wait_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif
static void qmi_voicei_speech_codec_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
static void qmi_voicei_conf_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif

static void qmi_voicei_additional_call_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
static void qmi_voicei_conf_join_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_conf_participant_update_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif

static void qmi_voicei_handover_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_aoc_low_funds_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);


#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
static void qmi_voicei_conference_participants_info_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif


static void qmi_voicei_call_control_result_info_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
static void qmi_voicei_tty_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_audio_rat_change_info_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_conference_participant_status_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif

#ifndef FEATURE_MMODE_DISABLE_IMS_EXT
static void qmi_voicei_e911_orig_fail_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif

static void qmi_voicei_get_speech_codec_info_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);


#if defined (FEATURE_MMODE_SS_SUPS)

/* Sups command responses */
static void qmi_voicei_sups_get_clir_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
/*Supplementary services resp*/

static void qmi_voicei_sups_get_call_forwarding_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
static void qmi_voicei_sups_get_clip_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_get_colp_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);


static void qmi_voicei_sups_get_call_waiting_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_get_call_barring_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_set_call_barring_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif
#ifdef FEATURE_MMODE_CALL_SUPS
static void qmi_voicei_sups_manage_calls_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif
#ifdef FEATURE_MMODE_SS_SUPS
static void qmi_voicei_sups_set_sups_service_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif
#if defined FEATURE_MMODE_USS_SUPS
static void qmi_voicei_sups_orig_ussd_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_answer_ussd_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_sups_cancel_ussd_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif

static void qmi_voicei_set_modem_config_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_get_modem_config_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

#if (defined FEATURE_ALS) && (defined FEATURE_GSM)
static void qmi_voicei_als_set_line_switching_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_als_select_line_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif
static void qmi_voicei_aoc_reset_acm_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_aoc_set_acmmax_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_aoc_get_call_meter_info_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

#ifdef FEATURE_MMODE_CALL_SUPS
static void qmi_voicei_sups_manage_ip_calls_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif
#ifdef FEATURE_MMODE_SS_SUPS
static void qmi_voicei_sups_get_colr_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif

#if (defined FEATURE_ALS) && (defined FEATURE_GSM)
static void qmi_voicei_als_get_line_switching_status_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_als_get_selected_line_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES

static void qmi_voicei_call_modified_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_call_modify_accept_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif

#ifdef FEATURE_MMODE_SS_SUPS
static void qmi_voicei_sups_get_cnap_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES

static void qmi_voicei_srvcc_call_config_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
);
#endif

static void qmi_voicei_vice_dialog_info_ind
( const qmi_voice_cm_if_info_item_s *const data_ptr
);

static void qmi_voicei_auto_reject_incoming_call_end_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
);


/* FW callbacks */
void qmi_voice_handle_fw_cmd
(
  qmi_mmode_cmd_enum_type cmd,
  void *            user_data_ptr
);

void qmi_voice_process_cmd
(
  qmi_mmode_cmd_enum_type        cmd,
  void                   * user_data_ptr
);

/*===========================================================================
  FUNCTION QMI_VOICE_INIT()

  DESCRIPTION
    Register the VOICE service with QMI FW

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_voice_init
(
  void
)
{
  qmi_csi_error                   err;
  qmi_sap_error                   serr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //QM_MSG_MED("QMI VOICE qmi_voice_init");

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* memset the service configuration */
  memset(&qmi_voicei_cfg, 0, sizeof(qmi_mmode_svc_config_type) );
    
  /*-----------------------------------------------------------------------
  Step1: Fill configuration 
  -----------------------------------------------------------------------*/
  
  qmi_voicei_cfg.cmd_hdlr_array    = qmi_voicei_cmd_callbacks;
  qmi_voicei_cfg.cmd_num_entries   = VOICEI_CMD_MAX;
  
  /*-----------------------------------------------------------------------
  step 2:  calling QMI Framework API to register the service.
  ----------------------------------------------------------------------*/

  qmi_voicei_fw_init_cback_hdlr();
  qmi_voice_state.svc_config        = (void *)&qmi_voicei_cfg;
  qmi_voice_state.service_type      = QMI_MMODE_SERVICE_VOICE;
  qmi_voice_state.uses_raw_msgs     = TRUE;
  
  err = qmi_mmode_svc_qcsi_register(QMI_MMODE_SERVICE_VOICE,
                                  qmi_voice_qcsi_svc_connect_cb,
                                  qmi_voice_qcsi_svc_disconnect_cb,
                                  qmi_mmode_qcsi_svc_handle_req_cb,
                                  &qmi_voice_state);
  
  //register with SAP
  serr = qmi_sap_register(voice_get_service_object_v02(), 
                         NULL, 
                         &qmi_voice_state.sap_client_handle);
  
  if ( serr != QMI_SAP_NO_ERR || err != QMI_CSI_NO_ERR )
  {
    QM_MSG_ERROR_2("qmi_voice_init() qmi_csi_register/sap_register failed %d", err, serr);
    return;
  }

} /* qmi_voice_init() */


void qmi_voice_process_cmd
(
  qmi_mmode_cmd_enum_type        cmd,
  void                   * user_data_ptr
)
{
  #ifndef FEATURE_NO_QMI_VOICE
  qmi_mmode_cmd_data_buf_type *  data_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT (user_data_ptr);

  data_ptr = (qmi_mmode_cmd_data_buf_type *) user_data_ptr;
  ASSERT (data_ptr->cmd.cm_if_data_ptr->header.cmd < QMI_VOICE_CM_IF_CMD_MAX);

  QM_MSG_MED_1("QMI VOICE qmi_voice_process_cmd %d",
               data_ptr->cmd.cm_if_data_ptr->header.cmd);

  switch (data_ptr->cmd.cm_if_data_ptr->header.cmd)
  {
    case QMI_VOICE_CM_IF_CMD_DIAL_CALL_RESP:
      qmi_voicei_dial_call_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_END_CALL_RESP:
      qmi_voicei_end_call_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_ANSWER_CALL_RESP:
      qmi_voicei_answer_call_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SETUP_ANSWER_RESP:
      qmi_voicei_setup_answer_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_GET_ALL_CALL_INFO_RESP:
      qmi_voicei_get_all_call_info_resp( data_ptr->cmd.cm_if_data_ptr );
      break;

    case QMI_VOICE_CM_IF_CMD_INFO_REC_IND:
      qmi_voicei_info_rec_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    
    case QMI_VOICE_CM_IF_CMD_GET_CALL_INFO_RESP:
      qmi_voicei_get_call_info_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
      
    #ifndef FEATURE_MMODE_DISABLE_INBAND_SUPP
    case QMI_VOICE_CM_IF_CMD_START_CONT_DTMF_RESP:
      qmi_voicei_start_cont_dtmf_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_STOP_CONT_DTMF_RESP:
      qmi_voicei_stop_cont_dtmf_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    #endif

    #ifndef FEATURE_MMODE_DISABLE_INBAND_SUPP
    case QMI_VOICE_CM_IF_CMD_DTMF_IND:
      qmi_voicei_dtmf_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    #endif
    
    case QMI_VOICE_CM_IF_CMD_SUPS_NOTIFICATION_IND:
      qmi_voicei_sups_notification_ind( data_ptr->cmd.cm_if_data_ptr );
      break;

    case QMI_VOICE_CM_IF_CMD_ALL_CALL_STATUS_IND:
      qmi_voicei_all_call_status_ind( data_ptr->cmd.cm_if_data_ptr );
      break;


#if defined(FEATURE_MMODE_CALL_SUPS)
/* Sups responses */
    case QMI_VOICE_CM_IF_CMD_SUPS_MANAGE_CALLS_RESP:
      qmi_voicei_sups_manage_calls_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
 #endif   
 #if defined(FEATURE_MMODE_SS_SUPS)    
    case QMI_VOICE_CM_IF_CMD_SUPS_GET_CLIR_RESP:
      qmi_voicei_sups_get_clir_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SUPS_GET_CALL_FORWARDING_RESP:
      qmi_voicei_sups_get_call_forwarding_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SUPS_GET_CLIP_RESP:
       qmi_voicei_sups_get_clip_resp( data_ptr->cmd.cm_if_data_ptr );
       break;
    case QMI_VOICE_CM_IF_CMD_SUPS_GET_CALL_WAITING_RESP:
       qmi_voicei_sups_get_call_waiting_resp( data_ptr->cmd.cm_if_data_ptr );
       break;
    case QMI_VOICE_CM_IF_CMD_SUPS_GET_CALL_BARRING_RESP:
       qmi_voicei_sups_get_call_barring_resp( data_ptr->cmd.cm_if_data_ptr );
       break;
    case QMI_VOICE_CM_IF_CMD_SUPS_SET_CALL_BARRING_RESP:
       qmi_voicei_sups_set_call_barring_resp( data_ptr->cmd.cm_if_data_ptr );
       break;
    case QMI_VOICE_CM_IF_CMD_SUPS_SET_SUPS_SERVICE_RESP:
       qmi_voicei_sups_set_sups_service_resp( data_ptr->cmd.cm_if_data_ptr );
       break;
#endif
#if defined(FEATURE_MMODE_USS_SUPS)

    case QMI_VOICE_CM_IF_CMD_SUPS_ORIG_USSD_RESP:
      qmi_voicei_sups_orig_ussd_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SUPS_ANSWER_USSD_RESP:
      qmi_voicei_sups_answer_ussd_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SUPS_CANCEL_USSD_RESP:
      qmi_voicei_sups_cancel_ussd_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SUPS_USSD_IND:
      qmi_voicei_sups_ussd_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_SUPS_USSD_RELEASE_IND:
      qmi_voicei_sups_ussd_release_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
#endif

    case QMI_VOICE_CM_IF_CMD_UUS_IND:
      qmi_voicei_uus_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
#if defined (FEATURE_MMODE_SS_SUPS)    
    case QMI_VOICE_CM_IF_CMD_SUPS_IND:
      qmi_voicei_sups_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
#endif

    case QMI_VOICE_CM_IF_CMD_HANDOVER_IND:
      qmi_voicei_handover_ind( data_ptr->cmd.cm_if_data_ptr );
      break;	  
    case QMI_VOICE_CM_IF_CMD_AOC_LOW_FUNDS_IND:
      qmi_voicei_aoc_low_funds_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    
    case QMI_VOICE_CM_IF_CMD_SPEECH_CODEC_INFO_IND:
      qmi_voicei_speech_codec_info_ind( data_ptr->cmd.cm_if_data_ptr );
      break;      
    /*Modem Configuration items response*/
    case QMI_VOICE_CM_IF_CMD_SET_MODEM_CONFIG_RESP:
      qmi_voicei_set_modem_config_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_GET_MODEM_CONFIG_RESP:
      qmi_voicei_get_modem_config_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
#if defined(FEATURE_MMODE_USS_SUPS)
    case QMI_VOICE_CM_IF_CMD_SUPS_ORIG_USSD_NO_WAIT_IND:
      qmi_voicei_sups_orig_ussd_no_wait_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
#endif

    #if (defined FEATURE_ALS) && (defined FEATURE_GSM)
    case QMI_VOICE_CM_IF_CMD_ALS_SET_LINE_SWITCHING_RESP:
      qmi_voicei_als_set_line_switching_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_ALS_SELECT_LINE_RESP:
      qmi_voicei_als_select_line_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    #endif
    
    case QMI_VOICE_CM_IF_CMD_AOC_RESET_ACM_RESP:
      qmi_voicei_aoc_reset_acm_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_AOC_SET_ACMMAX_RESP:
      qmi_voicei_aoc_set_acmmax_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_AOC_GET_CALL_METER_INFO_RESP:
      qmi_voicei_aoc_get_call_meter_info_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
      
    #ifdef FEATURE_MMODE_CALL_SUPS
    case QMI_VOICE_CM_IF_CMD_SUPS_MANAGE_IP_CALLS_RESP:
      qmi_voicei_sups_manage_ip_calls_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    #endif
    #ifdef  FEATURE_MMODE_SS_SUPS
    case QMI_VOICE_CM_IF_CMD_SUPS_GET_COLP_RESP:
       qmi_voicei_sups_get_colp_resp( data_ptr->cmd.cm_if_data_ptr );
       break;
   case QMI_VOICE_CM_IF_CMD_SUPS_GET_COLR_RESP:
      qmi_voicei_sups_get_colr_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    #endif

    #if (defined FEATURE_ALS) && (defined FEATURE_GSM)
    case QMI_VOICE_CM_IF_CMD_ALS_GET_LINE_SWITCHING_STATUS_RESP:
      qmi_voicei_als_get_line_switching_status_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_ALS_GET_SELECTED_LINE_RESP:
      qmi_voicei_als_get_selected_line_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
    #endif

     #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
    case QMI_VOICE_CM_IF_CMD_CALL_MODIFIED_IND:
      qmi_voicei_call_modified_ind( data_ptr->cmd.cm_if_data_ptr );
      break;      
    case QMI_VOICE_CM_IF_CMD_CALL_MODIFY_ACCEPT_IND:
      qmi_voicei_call_modify_accept_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    #endif
    
#ifdef FEATURE_MMODE_SS_SUPS
    case QMI_VOICE_CM_IF_CMD_SUPS_GET_CNAP_RESP:
      qmi_voicei_sups_get_cnap_resp( data_ptr->cmd.cm_if_data_ptr );
      break;
#endif

    #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
    case QMI_VOICE_CM_IF_CMD_CONF_INFO_IND:
      qmi_voicei_conf_info_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    #endif
    
    case QMI_VOICE_CM_IF_CMD_ADDITIONAL_CALL_INFO_IND:
      qmi_voicei_additional_call_info_ind( data_ptr->cmd.cm_if_data_ptr );
      break;

    #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
    case QMI_VOICE_CM_IF_CMD_CONF_PARTICIPANT_STATUS_IND:
      qmi_voicei_conference_participant_status_ind( data_ptr->cmd.cm_if_data_ptr );
      break; 	  
    case QMI_VOICE_CM_IF_CMD_CONF_JOIN_IND:
      qmi_voicei_conf_join_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_CONF_PARTICIPANT_UPDATE_IND:
      qmi_voicei_conf_participant_update_ind( data_ptr->cmd.cm_if_data_ptr );
      break;
    case QMI_VOICE_CM_IF_CMD_CONF_PARTICIPANTS_INFO_IND:
      qmi_voicei_conference_participants_info_ind(data_ptr->cmd.cm_if_data_ptr);
      break;
    #endif
    
    case QMI_VOICE_CM_IF_CMD_CC_RESULT_INFO_IND:
      qmi_voicei_call_control_result_info_ind(data_ptr->cmd.cm_if_data_ptr);
      break;

    #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
    case QMI_VOICE_CM_IF_CMD_SRVCC_CALL_CONFIG_RESP:
      qmi_voicei_srvcc_call_config_resp(data_ptr->cmd.cm_if_data_ptr);
      break;
    case QMI_VOICE_CM_IF_CMD_TTY_INFO_IND:
      qmi_voicei_tty_ind(data_ptr->cmd.cm_if_data_ptr);
      break;
    case QMI_VOICE_CM_IF_CMD_AUDIO_RAT_CHANGE_INFO_IND:
      qmi_voicei_audio_rat_change_info_ind(data_ptr->cmd.cm_if_data_ptr);
      break;
    #endif
    
    #ifndef FEATURE_MMODE_DISABLE_IMS_EXT
    case QMI_VOICE_CM_IF_CMD_E911_ORIG_FAIL_IND:
      qmi_voicei_e911_orig_fail_ind(data_ptr->cmd.cm_if_data_ptr);
      break;
    #endif

    #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
    case QMI_VOICE_CM_IF_CMD_VICE_DIALOG_IND:
      qmi_voicei_vice_dialog_info_ind(data_ptr->cmd.cm_if_data_ptr);
      break;
    #endif
    case QMI_VOICE_CM_IF_CMD_SPEECH_CODEC_INFO_RESP:
      qmi_voicei_get_speech_codec_info_resp(data_ptr->cmd.cm_if_data_ptr);
      break;

    case QMI_VOICE_CM_IF_CMD_MT_CALL_END_AUTO_REJECT_IND:
      qmi_voicei_auto_reject_incoming_call_end_ind( data_ptr->cmd.cm_if_data_ptr );
	  break;
    
    default:
      break;
  }
  QMI_VOICE_CM_IF_MEM_FREE(data_ptr->cmd.cm_if_data_ptr);
  #else
  (void) cmd;
  (void) user_data_ptr;
  #endif /* !FEATURE_NO_QMI_VOICE */
  return;
}

/*===========================================================================
  FUNCTION QMI_VOICE_HAS_VALID_CLIENT()

  DESCRIPTION
    Checks if there is a client using this service
    
  PARAMETERS
    None
    
  RETURN VALUE
    True if QMI VOICE has a valid client.
    False otherwise.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_voice_has_valid_client
( 
  void
)
{
  qmi_voicei_client_state_type * cl_sp;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  if( cl_sp != NULL )
  {
    return TRUE;
  }

  return FALSE;
} /* qmi_voice_has_valid_client */


/*===========================================================================
  FUNCTION qmi_voicei_validate_cmd_buf_p()

  DESCRIPTION
    Function to validate returned cmd_buf_p is still allocated in client.

  PARAMETERS
    cmd_buf_p: pointer to queue element for comparison

RETURN VALUE
  Pointer to the client state if cmdbuf valid. otherwise NULL
===========================================================================*/
boolean qmi_voicei_validate_cmd_buf_p( qmi_mmode_qcsi_transaction_type *cmd_buf_p )
{
  qmi_mmode_svc_info_type *svc_state_ptr = &qmi_voice_state;

  if(cmd_buf_p == NULL)
  {
    return NULL;
  }

  if ( NULL != q_linear_search( &svc_state_ptr->client_q,
                                 qmi_mmode_qcsi_compare_client_handle,
                                 cmd_buf_p->clnt_info_ptr) )
  {
    if ( NULL != q_linear_search( &svc_state_ptr->pending_req,
                                 qmi_mmode_qcsi_compare_pending_trans,
                                 cmd_buf_p->req_handle) )
    {
      return TRUE;
    }
  }

  QM_MSG_HIGH("Buffer does not match existing client transaction");
  return FALSE;
}

/*===========================================================================
  FUNCTION QMI_VOICEI_SEND_RESPONSE()

  DESCRIPTION
    Send the response to framework

  PARAMETERS
    clid: uint8

  RETURN VALUE
    TRUE if sent successfully, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_voicei_send_response
(
  qmi_error_e_type                 errval,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  dsm_item_type                   *msg_ptr
)
{
  qmi_csi_error                  err    = QMI_CSI_NO_ERR;
  qmi_result_e_type       result;

  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS 
                                   : QMI_RESULT_FAILURE);

  if(!qmi_svc_put_result_tlv(&msg_ptr, result, errval))
  {
    QM_MSG_ERROR("qmi_svc_put_result_tlv failed. Cannot send response");
    qmi_mmode_svc_free_pending_transaction( &qmi_voice_state, &cmd_buf_p );
    dsm_free_packet(&msg_ptr);
    return FALSE;
  }

  /* Send the response */
  err = qmi_mmode_qcsi_send_resp_raw(&qmi_voice_state, &cmd_buf_p, msg_ptr);

  return ((err == QMI_CSI_NO_ERR)? 1: 0);
}/* qmi_voicei_send_response */


/*===========================================================================
  FUNCTION QMI_VOICEI_SEND_RESPONSE_ENC()

  DESCRIPTION
    Send the encoded response to framework

  PARAMETERS
    clid: uint8

  RETURN VALUE
    TRUE if sent successfully, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_voicei_send_response_enc
(
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  dsm_item_type *         msg_ptr
)
{
  qmi_csi_error                  err    = QMI_CSI_NO_ERR;

  if ( cmd_buf_p == NULL )
  {
    QM_MSG_ERROR("cmd_buf_p is NULL");
    return FALSE;
  }

  /* Send the response */
  err = qmi_mmode_qcsi_send_resp_raw(&qmi_voice_state, &cmd_buf_p, msg_ptr);

  return ((err == QMI_CSI_NO_ERR)? 1: 0);
}/* qmi_voicei_send_response_enc */


/*===========================================================================
  FUNCTION QMI_VOICEI_SEND_INDICATION()

  DESCRIPTION
    This function is calls the QMI Framework API to send out the VOICE service
    indication to client.

  PARAMETERS
    msg_hdr_p : QMI Framework message header
    cmd_type : type of indication
    ind : dsm item
 
  RETURN VALUE
    TRUE if ind sent to Framework sucessfully
    FALSE otherwise
 
  DEPENDENCIES
    QMI Voice service must be initialized and registered with Framework
 
  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_voicei_send_indication
(
  qmi_voicei_client_state_type * cl_sp,
  uint16                         msg_id,
  dsm_item_type *                ind
)
{
  qmi_csi_error                       err    = QMI_CSI_NO_ERR;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(cl_sp);  

  err = qmi_mmode_qcsi_send_ind_raw(cl_sp->clnt, (unsigned int)msg_id, ind);

  return ((err == QMI_CSI_NO_ERR)? 1: 0);
} /* qmi_voicei_send_indication */


/*===========================================================================
  FUNCTION QMI_VOICEI_ALLOC_CL_SP()

  DESCRIPTION
    Allocate the client data buffer

  PARAMETERS
    clid: uint8

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_voicei_client_state_type * qmi_voicei_alloc_cl_sp
(
  qmi_client_handle          client_handle
)
{
  qmi_voicei_client_state_type *  cl_sp = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QM_MSG_MED_1("qmi_voicei_alloc_cl_sp client_handle :%p", client_handle);

  cl_sp = QMI_VOICE_CM_IF_MEM_ALLOC(sizeof(qmi_voicei_client_state_type));
  if ( cl_sp == NULL ) 
  {
    qmi_voice_mem_error();
    ASSERT(cl_sp);
  }
  else
  {
    memset( cl_sp, 0x00, sizeof(qmi_voicei_client_state_type) );
    (void) q_link ( cl_sp, &cl_sp->link );
    cl_sp->clnt = client_handle;
  }
  return cl_sp;
} /* qmi_voicei_alloc_cl_sp */

/*===========================================================================
  FUNCTION QMI_VOICEI_FW_INIT_CBACK_HDLR()

  DESCRIPTION
    Process the Service initialization request from Framework

  PARAMETERS
    num_instances: uint16

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_voicei_fw_init_cback_hdlr( void )
{
  qmi_mmode_svc_info_type *voice_sp;

  voice_sp = &qmi_voice_state;
  memset(voice_sp, 0, sizeof(qmi_mmode_svc_info_type));

  qmi_mmode_set_cmd_handler(QMI_MMODE_CMD_VOICE_CB, qmi_voice_process_cmd);

  /* Initialize the Voice -> CM  interface */
  qmi_voice_cm_if_init();
  
  voice_svc_obj = voice_get_service_object_v02();
}/* voicei_init_cback */

int qmi_voice_qcsi_compare_client_handle
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmi_voicei_client_state_type *clnt_info = (qmi_voicei_client_state_type*)item_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ( (clnt_info->clnt == 
                          (qmi_client_handle)compare_val) ? 1 : 0 );
} /* qmi_voice_qcsi_compare_client_handle */

static int qmi_voice_qcsi_compare_pending_req_for_client
( 
  void *item_ptr,
  void *compare_val 
)
{
  qmi_voicei_client_state_type *clnt_info_ptr;
  qmi_mmode_qcsi_transaction_type *req_node;
  qmi_client_handle clnt;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((item_ptr == NULL) || (compare_val == NULL))
  {
    return FALSE;
  }
  req_node = (qmi_mmode_qcsi_transaction_type *)item_ptr;
  clnt_info_ptr = (qmi_voicei_client_state_type *)req_node->clnt_info_ptr;
  clnt = (qmi_client_handle)compare_val;

  return ( (clnt_info_ptr->clnt == (clnt))? 1 : 0 );
} /* qmi_voice_qcsi_compare_pending_req_for_client */

/*===========================================================================
  FUNCTION qmi_voice_qcsi_connect_cb()

  DESCRIPTION
    CallBack function to be registered with QCSI for connecting a client

  PARAMETERS
    Client Handle
    Service Handle
    Connection Handle

  RETURN VALUE
    QMI_ERR_NONE - Success
    QMI_CSI_CB_CONN_REFUSED - Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_csi_cb_error qmi_voice_qcsi_svc_connect_cb
(
  qmi_client_handle          client_handle,
  void                      *service_handle,
  void                     **connection_handle
)
{

  qmi_voicei_client_state_type      *cl_sp = NULL;
  qmi_csi_cb_error                   err = QMI_CSI_CB_NO_ERR;
  qmi_mmode_svc_info_type           *svc_ptr;

  ASSERT(connection_handle && service_handle);

  svc_ptr = (qmi_mmode_svc_info_type *)service_handle;

  do
  {
    if( svc_ptr == NULL || svc_ptr->service_type != QMI_MMODE_SERVICE_VOICE ||
        svc_ptr->service_handle != qmi_voice_state.service_handle )
    {
      err = QMI_CSI_CB_UNSUPPORTED_ERR;
      QM_MSG_HIGH("Invalid service received");
      break;
    }
    if ( NULL != q_linear_search( &svc_ptr->client_q,
                                    qmi_voice_qcsi_compare_client_handle,
                                    client_handle ) )
    {
      QM_MSG_ERROR_1("voice_qcsi_connect_cb: client 0x%p present",
                      client_handle);
      err = QMI_CSI_CB_CONN_REFUSED;
      break;
    }

    if( q_cnt(&svc_ptr->client_q) == VOICEI_MAX_CLIDS )
    {

      err = QMI_CSI_CB_CONN_REFUSED;
      break;
    }
    
    cl_sp = qmi_voicei_alloc_cl_sp( client_handle );
  
    if(cl_sp)
    {
  
      /* Add client node to the queue of clients */
      q_put( &(svc_ptr->client_q), &(cl_sp->link) );
          
      cl_sp->voice_info.reg_dtmf_events = FALSE;
      cl_sp->voice_info.reg_voice_privacy_events = FALSE;
      cl_sp->voice_info.reg_sups_events = TRUE;
      cl_sp->voice_info.reg_call_events = TRUE;
      cl_sp->voice_info.reg_handover_events = FALSE;
      cl_sp->voice_info.reg_speech_events = FALSE;
      cl_sp->voice_info.reg_ussd_notification_events = TRUE;
      cl_sp->voice_info.reg_modification_events = TRUE;
      cl_sp->voice_info.reg_uus_events = TRUE;
      cl_sp->voice_info.reg_aoc_events               = FALSE;
      cl_sp->voice_info.reg_conference_events = FALSE;
      cl_sp->voice_info.reg_brst_intl_events = FALSE;
      cl_sp->voice_info.reg_conf_participants_events = FALSE;
      cl_sp->voice_info.reg_mt_page_miss_events = FALSE;
      cl_sp->voice_info.reg_cc_result_events = FALSE;
      cl_sp->voice_info.reg_tty_info_events = FALSE;
      cl_sp->voice_info.reg_audio_rat_change_events = FALSE;
      cl_sp->voice_info.reg_e911_orig_fail_events = FALSE;
      cl_sp->voice_info.reg_add_call_info_events = FALSE;
      #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
      cl_sp->voice_info.reg_vice_dialog_info_events = FALSE;
      #endif
      cl_sp->voice_info.reg_mt_call_end_auto_reject_events = FALSE;
      cl_sp->asubs_info.is_bind_set_by_client = FALSE;
      cl_sp->asubs_info.as_id = QMI_VOICE_CM_IF_AS_ID_PRIMARY;
  
      *connection_handle = cl_sp;
      QM_MSG_HIGH_2("voice_qcsi_connect_cb: Alloc client 0x%p client_handle %p", 
                      cl_sp, client_handle);
    }
    else
    {
      QM_MSG_ERROR("voice_qcsi_connect_cb: alloc_cl_sp failed");
      err = QMI_CSI_CB_NO_MEM;
    }
  } while(0);

  if (err != QMI_CSI_CB_NO_ERR)
  {  
    QM_MSG_ERROR_3("voice_qcsi_connect_cb: Error processing err = %d client_handle %p svc handle %p", 
                    err, client_handle, service_handle);
  }
  else
  {
    QM_MSG_HIGH_2("voice_qcsi_connect_cb: client connect client_handle %p connection_handle %p",
                    client_handle, *connection_handle);
  }

  return err;
}/* qmi_voice_qcsi_svc_connect_cb */

/*===========================================================================
  FUNCTION qmi_voice_qcsi_svc_disconnect_cb()

  DESCRIPTION
    CallBack function to be registered with QCSI for disconnecting a client

  PARAMETERS
    connection_handle : QCSI client connection handle
    service_handle : QCSI service handle

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voice_qcsi_svc_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_handle
)
{
  qmi_voicei_client_state_type       *clnt_info_ptr;
  qmi_mmode_svc_info_type            *svc_ptr;
  qmi_csi_error                       err = QMI_CSI_NO_ERR;
  qmi_mmode_qcsi_transaction_type    *pending_req = NULL;
  void                               *temp_ptr = NULL;
  int                                 num_deleted_pending_txs = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(connection_handle && service_handle);
  svc_ptr = (qmi_mmode_svc_info_type *)service_handle;
  clnt_info_ptr = (qmi_voicei_client_state_type *)connection_handle;

  do
  {
    if( svc_ptr == NULL || svc_ptr->service_type != QMI_MMODE_SERVICE_VOICE ||
        svc_ptr->service_handle != qmi_voice_state.service_handle )
    {
      err = QMI_CSI_INVALID_HANDLE;
      QM_MSG_HIGH("Invalid service received");
      break;
    }
    
    if ( TRUE != q_delete_ext( &svc_ptr->client_q,
                               &clnt_info_ptr->link ) )
    {
      err = QMI_CSI_CONN_REFUSED;
      break;
    }

    /* Delete any pending transactions for this client */
    if(q_cnt(&svc_ptr->pending_req) > 0)
    {
      do
      {
        pending_req = q_linear_delete_new(&svc_ptr->pending_req,
                                          qmi_voice_qcsi_compare_pending_req_for_client,
                                          (void*)&clnt_info_ptr->clnt,
                                           NULL,
                                           NULL );
        if (pending_req != NULL)
        {
          temp_ptr = (void *)pending_req;
          modem_mem_free( temp_ptr, MODEM_MEM_CLIENT_QMI );
          num_deleted_pending_txs++;
        }
      }while(pending_req != NULL);

      if(num_deleted_pending_txs > 0)
      {
        QM_MSG_HIGH_2("voice_qcsi_disconnect_cb: Deleted %d pending txs of clnt 0x%p",
                       num_deleted_pending_txs, 
                       clnt_info_ptr->clnt);
      }
    }
    
  } while(0);

  if( err == QMI_CSI_NO_ERR )
  {
    modem_mem_free( clnt_info_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
  }
  
  QM_MSG_HIGH_2("voice_qcsi_disconnect_cb: Releasing client 0x%p err %d",
                  connection_handle, err);
} /* qmi_voice_qcsi_svc_disconnect_cb() */

/*===========================================================================
  FUNCTION QMI_VOICEI_CMD_VALIDATE()

  DESCRIPTION
    
    
  PARAMETERS
    cl_sp     : client state pointer

  RETURN VALUE
    qmi_error_e_type
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_voicei_cmd_validate
(
  qmi_mmode_cmd_data_buf_type     *cmd_ptr,
  void                            *cl_sp,
  qmi_voicei_client_state_type   **client_sp_p
)
{

  if(cmd_ptr == NULL)
  {
    QM_MSG_ERROR("Out of QMI_MMODE cmd buf");
    return QMI_ERR_INTERNAL;
  }

  *client_sp_p = (qmi_voicei_client_state_type *)cl_sp;
  if(*client_sp_p == NULL)
  {
    return QMI_ERR_INVALID_CLIENT_ID;
  } 

  cmd_ptr->cmd.cm_if_data_ptr = QMI_VOICE_CM_IF_MEM_ALLOC(sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));
  if(cmd_ptr->cmd.cm_if_data_ptr == NULL)
  {
    qmi_voice_mem_fatal();
    qmi_mmode_free_cmd_data_buf(cmd_ptr);
    return QMI_ERR_INTERNAL;
  }
 
  return QMI_ERR_NONE;
} /* qmi_voicei_cmd_validate */


/*===========================================================================
  FUNCTION QMI_VOICEI_SEND_RESULT()

  DESCRIPTION
    

  PARAMETERS
    cl_sp: client state pointer

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_send_result
(
  qmi_error_e_type               errval,
  qmi_mmode_cmd_data_buf_type   *cmd_ptr
)
{
  boolean             retval;
  qmi_result_e_type   result   = (errval? QMI_RESULT_FAILURE : QMI_RESULT_SUCCESS);
  dsm_item_type      *response = NULL;

  if(cmd_ptr != NULL)
  {
    if(cmd_ptr->cmd.cm_if_data_ptr != NULL)
    {
      QMI_VOICE_CM_IF_MEM_FREE(cmd_ptr->cmd.cm_if_data_ptr);
    }
    qmi_mmode_free_cmd_data_buf(cmd_ptr);
  }
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_voicei_send_result */

/*=====================QMI-VOICE CMD DECODE & ENCODE APIS==================*/

/*===========================================================================
  FUNCTION QMI_VOICEI_DIAL_CALL()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_dial_call
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_DIAL_CALL_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.dial_call),
                                         (uint32_t) sizeof(voice_dial_call_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_DIAL_CALL;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_dial_call() */

/*===========================================================================
  FUNCTION QMI_VOICEI_DIAL_CALL_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_dial_call_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.dial_call_resp),
                                    (uint32_t) sizeof(voice_dial_call_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
} /* qmi_voicei_dial_call_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_END_CALL()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_end_call
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  /* Deactivate the retransmission timer if started */
  qm_timer_stop(QM_TIMER_ID_INCOM_EVT_RESEND);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_END_CALL_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.end_call),
                                         (uint32_t) sizeof(voice_end_call_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_END_CALL;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
}/* qmi_voicei_end_call() */


/*===========================================================================
  FUNCTION QMI_VOICEI_END_CALL_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_end_call_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.end_call_resp),
                                    (uint32_t) sizeof(voice_end_call_resp_msg_v02),
                                    &response )
    )
  {
   qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
} /* qmi_voicei_end_call_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_ANSWER_CALL()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_answer_call
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  /* Deactivate the retransmission timer if started */
  qm_timer_stop(QM_TIMER_ID_INCOM_EVT_RESEND);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_ANSWER_CALL_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.answer_call),
                                         (uint32_t) sizeof(voice_answer_call_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_ANSWER_CALL;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_answer_call */

/*===========================================================================
  FUNCTION QMI_VOICEI_SETUP_ANSWER()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_setup_answer
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_SETUP_ANSWER_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.setup_answer),
                                         (uint32_t) sizeof(voice_setup_answer_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SETUP_ANSWER;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_setup_answer() */

/*===========================================================================
  FUNCTION QMI_VOICEI_ANSWER_CALL_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_answer_call_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
  {
    qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
    dsm_item_type *         response;
  
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
    cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;
  
    if(cmd_buf_p == NULL)
    {
      QM_MSG_HIGH("cmd_buf_p is NULL");
      return;
    }
  
    response = NULL;
    if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                      QMI_IDL_RESPONSE,
                                      (uint16_t)cmd_buf_p->msg_id,
                                      (void *) &(data_ptr->data.answer_call_resp),
                                      (uint32_t) sizeof(voice_answer_call_resp_msg_v02),
                                      &response )
      )
    {
      qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
    }
  }


/*===========================================================================
  FUNCTION QMI_VOICEI_SETUP_ANSWER_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_setup_answer_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
  {
    qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
    dsm_item_type *         response;
  
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
  
    cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;
  
    if(cmd_buf_p == NULL)
    {
      QM_MSG_HIGH("cmd_buf_p is NULL");
      return;
    }
  
    response = NULL;
    if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                      QMI_IDL_RESPONSE,
                                      (uint16_t)cmd_buf_p->msg_id,
                                      (void *) &(data_ptr->data.setup_answer_resp),
                                      (uint32_t) sizeof(voice_setup_answer_resp_msg_v02),
                                      &response )
      )
    {
      qmi_voicei_send_response_enc((void*)cmd_buf_p, response);
    }
  }

/*===========================================================================
  FUNCTION QMI_VOICEI_GET_CALL_INFO()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_get_call_info
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_CALL_INFO_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_call_info),
                                         (uint32_t) sizeof(voice_get_call_info_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_GET_CALL_INFO;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_get_call_info() */

/*===========================================================================
  FUNCTION QMI_VOICEI_GET_CALL_INFO_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_get_call_info_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;

  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.get_call_info_resp),
                                    (uint32_t) sizeof(voice_get_call_info_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc((void*)cmd_buf_p, response);
  }
} /* qmi_voicei_get_call_info_resp() */

/*===========================================================================
  FUNCTION QMI_VOICEI_INDICATION_REGISTER()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_indication_register
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_voicei_client_state_type * client_sp;

  dsm_item_type *    response;
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type   *cmd_ptr = NULL;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //QM_MSG_MED("QMI VOICE qmi_voicei_indication_register");

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  response          = NULL;
  errval            = QMI_ERR_NONE;
    
  cmd_ptr = qmi_mmode_get_cmd_data_buf();
    
  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_INDICATION_REGISTER_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.register_req),
                                         (uint32_t) sizeof(voice_indication_register_req_msg_v02));
  if(errval) goto send_result;


    
  if(client_sp == NULL)
  {
    QM_MSG_ERROR("Invalid client with id");
    errval = QMI_ERR_INVALID_CLIENT_ID;
    goto send_result;
  }

  if (cmd_ptr->cmd.cm_if_data_ptr->data.register_req.reg_dtmf_events_valid ||
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.sups_events_valid  ||
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.supps_notification_events_valid ||
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.call_events_valid ||
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.handover_events_valid||
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.speech_events_valid||
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.ussd_notification_events_valid||
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.modification_events_valid||
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.uus_events_valid ||
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.aoc_events_valid||
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.conference_events_valid||  
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.conf_participants_events_valid|| 
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.cc_result_events_valid || 
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.orig_fail_events_valid || 
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.vs_status_events_valid || 
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.audio_rat_change_events_valid ||
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.additional_call_info_events_valid || 
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.ecall_status_events_valid || 
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.vice_dialog_event_valid ||
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.tty_info_events_valid ||
      cmd_ptr->cmd.cm_if_data_ptr->data.register_req.auto_rejected_incoming_call_end_event_valid
    )
  {
    QM_MSG_MED("Received registration for some events");
  }
  else
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if (cmd_ptr->cmd.cm_if_data_ptr->data.register_req.reg_dtmf_events_valid)
  {
    client_sp->voice_info.reg_dtmf_events          = cmd_ptr->cmd.cm_if_data_ptr->data.register_req.reg_dtmf_events;
  }

  if (cmd_ptr->cmd.cm_if_data_ptr->data.register_req.call_events_valid)
  {
    client_sp->voice_info.reg_call_events          = cmd_ptr->cmd.cm_if_data_ptr->data.register_req.call_events;
  }

  if (cmd_ptr->cmd.cm_if_data_ptr->data.register_req.handover_events_valid)
  {
    client_sp->voice_info.reg_handover_events      = cmd_ptr->cmd.cm_if_data_ptr->data.register_req.handover_events;
  }
     
  if(cmd_ptr->cmd.cm_if_data_ptr->data.register_req.speech_events_valid)
  {
    client_sp->voice_info.reg_speech_events        = cmd_ptr->cmd.cm_if_data_ptr->data.register_req.speech_events;
  }

  if(cmd_ptr->cmd.cm_if_data_ptr->data.register_req.aoc_events_valid)
  {
    client_sp->voice_info.reg_aoc_events           = cmd_ptr->cmd.cm_if_data_ptr->data.register_req.aoc_events;
  }

  if(cmd_ptr->cmd.cm_if_data_ptr->data.register_req.cc_result_events_valid)
  {
    client_sp->voice_info.reg_cc_result_events     = cmd_ptr->cmd.cm_if_data_ptr->data.register_req.cc_result_events;
  }
     
  if(cmd_ptr->cmd.cm_if_data_ptr->data.register_req.additional_call_info_events_valid)
  {
    client_sp->voice_info.reg_add_call_info_events = cmd_ptr->cmd.cm_if_data_ptr->data.register_req.additional_call_info_events;
  }
       
  #if defined(FEATURE_MMODE_SS_SUPS)
  if (cmd_ptr->cmd.cm_if_data_ptr->data.register_req.sups_events_valid)
  {
    client_sp->voice_info.reg_sups_events               = cmd_ptr->cmd.cm_if_data_ptr->data.register_req.sups_events;
  }
  #endif
  #if defined FEATURE_MMODE_USS_SUPS
  if(cmd_ptr->cmd.cm_if_data_ptr->data.register_req.ussd_notification_events_valid)
  {
    client_sp->voice_info.reg_ussd_notification_events  = cmd_ptr->cmd.cm_if_data_ptr->data.register_req.ussd_notification_events;
  }
  #endif
     
  #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
  if(cmd_ptr->cmd.cm_if_data_ptr->data.register_req.modification_events_valid)
  {
    client_sp->voice_info.reg_modification_events      = cmd_ptr->cmd.cm_if_data_ptr->data.register_req.modification_events;
  }

  if(cmd_ptr->cmd.cm_if_data_ptr->data.register_req.conference_events_valid)
  {
    client_sp->voice_info.reg_conference_events        = cmd_ptr->cmd.cm_if_data_ptr->data.register_req.conference_events;
  }

  if(cmd_ptr->cmd.cm_if_data_ptr->data.register_req.conf_participants_events_valid)
  {                                                                    
    client_sp->voice_info.reg_conf_participants_events = cmd_ptr->cmd.cm_if_data_ptr->data.register_req.conf_participants_events;
  }                     

  if(cmd_ptr->cmd.cm_if_data_ptr->data.register_req.tty_info_events_valid)
  {
    client_sp->voice_info.reg_tty_info_events          = cmd_ptr->cmd.cm_if_data_ptr->data.register_req.tty_info_events;
  }

  if(cmd_ptr->cmd.cm_if_data_ptr->data.register_req.audio_rat_change_events_valid)
  {
    client_sp->voice_info.reg_audio_rat_change_events  = cmd_ptr->cmd.cm_if_data_ptr->data.register_req.audio_rat_change_events;
  }
  #endif

  #ifdef FEATURE_UUS
  if (cmd_ptr->cmd.cm_if_data_ptr->data.register_req.uus_events_valid)
  {
    client_sp->voice_info.reg_uus_events               = cmd_ptr->cmd.cm_if_data_ptr->data.register_req.uus_events;
  }
  #endif
  
  #ifndef FEATURE_MMODE_DISABLE_IMS_EXT
  client_sp->voice_info.reg_e911_orig_fail_events       = e911_orig_fail_event;
  #endif
  
  #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
  if(cmd_ptr->cmd.cm_if_data_ptr->data.register_req.vice_dialog_event_valid)
  {
    client_sp->voice_info.reg_vice_dialog_info_events  = cmd_ptr->cmd.cm_if_data_ptr->data.register_req.vice_dialog_event;
  }
  #endif

  if(cmd_ptr->cmd.cm_if_data_ptr->data.register_req.auto_rejected_incoming_call_end_event_valid)
  {
    client_sp->voice_info.reg_mt_call_end_auto_reject_events = cmd_ptr->cmd.cm_if_data_ptr->data.register_req.auto_rejected_incoming_call_end_event;
  }
  
  QM_MSG_HIGH_8("voice ind_reg with dtmf_events %d call_events %d handover_events %d speech_events %d aoc_events %d cc_result_events %d additional_call_info_events %d mt_call_end_auto_reject_events %d",
                 client_sp->voice_info.reg_dtmf_events, client_sp->voice_info.reg_call_events,
                 client_sp->voice_info.reg_handover_events, client_sp->voice_info.reg_speech_events,
                 client_sp->voice_info.reg_aoc_events, client_sp->voice_info.reg_cc_result_events,
                 client_sp->voice_info.reg_add_call_info_events,client_sp->voice_info.reg_mt_call_end_auto_reject_events);

#if defined(FEATURE_MMODE_USS_SUPS)

  QM_MSG_HIGH_2("voice ind_reg with sups_events %d ussd_notification_events %d",
                 client_sp->voice_info.reg_sups_events,
                 client_sp->voice_info.reg_ussd_notification_events);
#endif

  #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
  QM_MSG_HIGH_5("voice ind_reg with modification_events %d conference_events %d conf_participants_events %d tty_info_events %d audio_rat_change_events %d",
                 client_sp->voice_info.reg_modification_events , client_sp->voice_info.reg_conference_events,
                 client_sp->voice_info.reg_conf_participants_events, client_sp->voice_info.reg_tty_info_events,
                 client_sp->voice_info.reg_audio_rat_change_events);
  #endif
  
send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);

}

#ifndef FEATURE_MMODE_DISABLE_INBAND_SUPP

/*===========================================================================
  FUNCTION QMI_VOICEI_START_CONT_DTMF_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_start_cont_dtmf_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.start_cont_dtmf_resp),
                                    (uint32_t) sizeof(voice_start_cont_dtmf_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
} /* qmi_voicei_start_cont_dtmf_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_STOP_CONT_DTMF_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_stop_cont_dtmf_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.stop_cont_dtmf_resp),
                                    (uint32_t) sizeof(voice_stop_cont_dtmf_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
} /* qmi_voicei_stop_cont_dtmf_resp() */
#endif

/*===========================================================================
  FUNCTION QMI_VOICEI_GET_ALL_CALL_INFO()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_get_all_call_info
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_ALL_CALL_INFO_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_all_call_info),
                                         (uint32_t) sizeof(voice_get_all_call_info_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_GET_ALL_CALL_INFO;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_get_all_call_info() */



/*===========================================================================
  FUNCTION QMI_VOICEI_GET_ALL_CALL_INFO_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_get_all_call_info_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  qmi_voice_cm_if_all_call_info_s       *all_call_info;
  uint8     i, num_of_instances=0,alpha_len=0;
  int       tot_len=0, diags_len=0, con_len=0;
  int       redirect_len=0,called_len=0;
  uint8     len_SIP_URI=0;
  voice_get_all_call_info_resp_msg_v02 *res;
  uint32 no_of_calls = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("Null cmd_buf ptr in qmi_voicei_get_all_call_info_resp");
    return;
  }

  res = QMI_VOICE_CM_IF_MEM_ALLOC(sizeof(voice_get_all_call_info_resp_msg_v02));
  if( NULL == res )
  {
    qmi_voice_mem_fatal();
    return;
  }

  memset( res, 0, sizeof( voice_get_all_call_info_resp_msg_v02 ) );

  all_call_info = (qmi_voice_cm_if_all_call_info_s*)&data_ptr->data.get_all_call_info_resp.info;

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  errval = data_ptr->data.get_all_call_info_resp.error;

  res->resp.result = (qmi_result_type_v01)(errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS 
                                             : QMI_RESULT_FAILURE);
  res->resp.error = (qmi_error_type_v01)errval;

  if( QMI_ERR_NONE == errval )
  {
    no_of_calls = (uint32_t)all_call_info->num_of_calls;

    for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
    {
      const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];

      /* Sip Error Code TLV */
      if(info->is_sip_error_code_available)
      {
        res->sip_error_code_valid = TRUE;
        res->sip_error_code_len = no_of_calls;
        
        res->sip_error_code[i].call_id = info->call_id;
        res->sip_error_code[i].sip_error_code = (uint16_t)info->sip_error_code;
        QM_MSG_MED_2("sip_error_code = %d for call id %d", info->sip_error_code, info->call_id);
      }

      /* IP Hist Info Optional TLV */  
      if( (info->is_ip_hist_info_available) && 
          (info->ip_hist_info.ip_hist_info_length < QMI_VOICE_CM_IF_MAX_IP_HIST_INFO_LEN) && 
          (info->ip_hist_info.ip_hist_info_length != 0) )
      {
        /* ip_hist_info_length is the number of UTF16 characters */
        res->ip_hist_info_valid = TRUE;
        res->ip_hist_info_len = no_of_calls;
        res->ip_hist_info[i].call_id = info->call_id;
        res->ip_hist_info[i].ip_hist_info_len = (uint32_t)info->ip_hist_info.ip_hist_info_length;
        memscpy(res->ip_hist_info[i].ip_hist_info, 2*QMI_VOICE_IP_HIST_INFO_MAX_LEN_V02, info->ip_hist_info.ip_hist_info_buf, 2*info->ip_hist_info.ip_hist_info_length);
        QM_MSG_MED_1("hist_info added for call id %d", info->call_id);
      }

    /* Second Alpha Optional TLV */
      if( (info->is_second_alpha_available == TRUE) && (info->call_state == QMI_VOICE_CM_IF_CALL_STATE_ORIG) )
      {
        res->second_alpha_ident_valid = TRUE;
        res->second_alpha_ident_len = no_of_calls;
        
        res->second_alpha_ident[i].call_id = info->call_id;
        res->second_alpha_ident[i].alpha_dcs = (alpha_dcs_enum_v02)info->second_alpha_info.alpha_dcs;
        res->second_alpha_ident[i].alpha_text_len = (uint32_t)info->second_alpha_info.alpha_len;
        memscpy(res->second_alpha_ident[i].alpha_text, QMI_VOICE_ALPHA_TEXT_MAX_V02, info->second_alpha_info.alpha_text, info->second_alpha_info.alpha_len);			  

        QM_MSG_MED_3("Second Alpha coding scheme = %d name length =%d for call id %d", info->second_alpha_info.alpha_dcs, info->second_alpha_info.alpha_len, info->call_id);
      }

    /* Remote Party Number Ext Optional TLV */
      if( info->is_num_available )
      {
         res->remote_party_number_ext_valid = TRUE;
         res->remote_party_number_ext_len = no_of_calls;
         
         res->remote_party_number_ext[i].call_id = info->call_id;
         res->remote_party_number_ext[i].rp_num_info.num_pi = (pi_num_enum_v02)info->num.pi;
         res->remote_party_number_ext[i].rp_num_info.num_si = (voice_si_enum_v02)info->num.si;
         res->remote_party_number_ext[i].rp_num_info.num_type = (voice_num_type_enum_v02)info->num.num_type;
         res->remote_party_number_ext[i].rp_num_info.num_plan = (voice_num_plan_enum_v02)info->num.num_plan;
         memscpy(res->remote_party_number_ext[i].rp_num_info.num, QMI_VOICE_SIP_URI_MAX_V02, info->num.buf, info->num.len);
        QM_MSG_MED_2("RP Number Ext = %s for call id %d",(const char *)info->num.buf, info->call_id );
      }
    
    /* End Reason Text Optional TLV */
      if( (info->is_end_reason_text_available) && 
          (info->end_reason_text.end_reason_text_length < QMI_VOICE_CM_IF_MAX_END_REASON_TEXT_LEN) && 
          (info->end_reason_text.end_reason_text_length != 0) )
      {
        /* end_reason_text_length is the number of UTF16 characters */
        res->end_reason_text_valid = TRUE;
        res->end_reason_text_len = no_of_calls;
        
        res->end_reason_text[i].call_id = info->call_id;
        res->end_reason_text[i].end_reason_text_len = (uint32_t)info->end_reason_text.end_reason_text_length;
        memscpy(res->end_reason_text[i].end_reason_text, 2*QMI_VOICE_END_REASON_TEXT_MAX_LEN_V02, info->end_reason_text.end_reason_text_buf, 2*info->end_reason_text.end_reason_text_length);			
        QM_MSG_MED_1("end_reason_text added for call id %d", info->call_id);
      }

    /* Call Attribute Status Optional TLV */
      if(info->is_call_attrib_status_available)
      {
        res->call_attrib_status_valid = TRUE;
        res->call_attrib_status_len = no_of_calls;
        
        res->call_attrib_status[i].call_id = info->call_id;
        res->call_attrib_status[i].call_attrib_status = (voice_call_attrib_status_enum_v02)info->call_attrib_status;

        QM_MSG_MED_2("call_attrib_status = %d for call id %d", info->call_attrib_status, info->call_id);
      }

    #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
    /* Is SRVCC  Optional TLV */
      if(info->is_srvcc_call_available)
      {
      res->is_srvcc_valid = TRUE;
      res->is_srvcc_len = no_of_calls;
      
      res->is_srvcc[i].call_id = info->call_id;
      res->is_srvcc[i].is_srvcc_call = (uint8_t)info->is_srvcc_call;

      QM_MSG_MED_2("Is SRVCC CALL = %d for call id %d", info->is_srvcc_call, info->call_id);
    }  
    #endif

    /* Alerting Pattern Optional TLV */
      if(info->is_alerting_pattern_available)
      {
        res->alerting_pattern_valid = TRUE;
        res->alerting_pattern_len = no_of_calls;
        
        res->alerting_pattern[i].call_id = info->call_id;
        res->alerting_pattern[i].alerting_pattern = (alerting_pattern_enum_v02)info->alerting_pattern;

        QM_MSG_MED_2("Alerting Pattern = %d for call id %d", info->alerting_pattern, info->call_id);
      }

    /* Connected number Info Optional TLV */
      if(info->is_conn_num_available)
      {
        res->conn_party_num_valid = TRUE;
        res->conn_party_num_len = no_of_calls;;
        
        res->conn_party_num[i].call_id = info->call_id;
        res->conn_party_num[i].conn_num_pi = (pi_num_enum_v02)info->conn_num_data.pi;
        res->conn_party_num[i].conn_num_si = (voice_si_enum_v02)info->conn_num_data.si;
        res->conn_party_num[i].conn_num_type = (voice_num_type_enum_v02)info->conn_num_data.num_type;
        res->conn_party_num[i].conn_num_plan = (voice_num_plan_enum_v02)info->conn_num_data.num_plan;
        res->conn_party_num[i].conn_num_len = (uint32_t)info->conn_num_data.len;
        memscpy(res->conn_party_num[i].conn_num, QMI_VOICE_CALLER_ID_MAX_V02, info->conn_num_data.num, info->conn_num_data.len);

        QM_MSG_MED_4("Connected num info pi= %d,si=%d,num_len=%d, call id = %d",info->conn_num_data.pi,info->conn_num_data.si,
                      info->conn_num_data.len,info->call_id );
      }
   
    /* Called party number Info Optional TLV */
      if(info->is_called_party_num_available)
      {
        res->called_party_num_valid = TRUE;
        res->called_party_num_len = no_of_calls;
        
        res->called_party_num[i].call_id = info->call_id;
        res->called_party_num[i].num_pi = (pi_num_enum_v02)info->called_party_num_data.pi;
        res->called_party_num[i].num_si = (voice_si_enum_v02)info->called_party_num_data.si;
        res->called_party_num[i].num_type = (voice_num_type_enum_v02)info->called_party_num_data.num_type;
        res->called_party_num[i].num_plan = (voice_num_plan_enum_v02)info->called_party_num_data.num_plan;
        res->called_party_num[i].num_len = (uint32_t)info->called_party_num_data.len;
        memscpy(res->called_party_num[i].num, QMI_VOICE_NUMBER_MAX_V02, info->called_party_num_data.num, info->called_party_num_data.len);

        QM_MSG_MED_4("Called party num info pi= %d,si=%d,num_len=%d, call id = %d",info->called_party_num_data.pi,
                     info->called_party_num_data.si,info->called_party_num_data.len, info->call_id);
      }
        
    /* Redirecting party number Info Optional TLV */
      if(info->is_redirecting_party_num_available)
      {
        res->redirecting_party_num_valid = TRUE;
        res->redirecting_party_num_len = no_of_calls;
        
        res->redirecting_party_num[i].call_id = info->call_id;
        res->redirecting_party_num[i].num_pi = (pi_num_enum_v02)info->redirecting_party_num_data.pi;
        res->redirecting_party_num[i].num_si = (voice_si_enum_v02)info->redirecting_party_num_data.si;
        res->redirecting_party_num[i].num_type = (voice_num_type_enum_v02)info->redirecting_party_num_data.num_type;
        res->redirecting_party_num[i].num_plan = (voice_num_plan_enum_v02)info->redirecting_party_num_data.num_plan;
        res->redirecting_party_num[i].num_len = (uint32_t)info->redirecting_party_num_data.len;
        memscpy(res->called_party_num[i].num, QMI_VOICE_NUMBER_MAX_V02, info->redirecting_party_num_data.num, info->redirecting_party_num_data.len);
        
        QM_MSG_MED_4("Redirect party num info pi= %d,si=%d,num_len=%d call id = %d",info->redirecting_party_num_data.pi,
                     info->redirecting_party_num_data.si,info->redirecting_party_num_data.len,info->call_id);
      }

    /* Call End Diagnostics Info Optional TLV */
      if(info->is_diagnostics_info_available)
      {
        res->diagnostic_info_valid= TRUE;
        res->diagnostic_info_len = no_of_calls;
        
        res->diagnostic_info[i].call_id = info->call_id;
        res->diagnostic_info[i].diagnostic_info_len = (uint32_t)info->diagnostic_data.diagnostic_length;
        memscpy(res->diagnostic_info[i].diagnostic_info, QMI_VOICE_DIAGNOSTIC_INFO_MAX_V02, info->diagnostic_data.diagnostics, info->diagnostic_data.diagnostic_length);

        QM_MSG_MED_2("Diagnostic info len=%d for call id %d", info->diagnostic_data.diagnostic_length, info->call_id);
      }

    #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
      if(info->is_vs_call_variant_valid)
      {
        res->vs_variant_valid = TRUE;
        res->vs_variant_len = no_of_calls;
        
        res->vs_variant[i].call_id = info->call_id;
        res->vs_variant[i].vs_variant = (vs_variant_type_enum_v02)info->vs_call_variant;

        QM_MSG_MED_2("VS variant Attributes = %d for call id %d", info->vs_call_variant, info->call_id);
      }
    #endif

    /* Audio Attributes Optional TLV */
      if(info->is_audio_attrib_available)
      {
        res->audio_attrib_valid = TRUE;
        res->audio_attrib_len = no_of_calls;
        
        res->audio_attrib[i].call_id = info->call_id;
        res->audio_attrib[i].call_attributes = (voice_call_attribute_type_mask_v02)info->audio_attrib;

        QM_MSG_MED_2("Audio Attributes = %d for call id %d", info->audio_attrib, info->call_id);
      }

    #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
    /* Video Attributes Optional TLV */
      if(info->is_video_attrib_available)
      {
        res->video_attrib_valid = TRUE;
        res->video_attrib_len = no_of_calls;
        
        res->video_attrib[i].call_id = info->call_id;
        res->video_attrib[i].call_attributes = (voice_call_attribute_type_mask_v02)info->video_attrib;

        QM_MSG_MED_2("Video Attributes = %d for call id %d", info->video_attrib, info->call_id);
      } 
    #endif

    /* Alpha Optional TLV */
      if(info->is_alpha_available)
      {
        res->alpha_id_valid  = TRUE;
        res->alpha_id_len = no_of_calls;
        
        res->alpha_id[i].call_id = info->call_id;
        res->alpha_id[i].alpha_dcs = (alpha_dcs_enum_v02)info->alpha_info.alpha_dcs;
        res->alpha_id[i].alpha_text_len = (uint32_t)info->alpha_info.alpha_len;
        memscpy(res->alpha_id[i].alpha_text, QMI_VOICE_ALPHA_TEXT_MAX_V02, info->alpha_info.alpha_text, info->alpha_info.alpha_len);		  

        QM_MSG_MED_3("Alpha coding scheme = %d name length =%d for call id %d", info->alpha_info.alpha_dcs, info->alpha_info.alpha_len, info->call_id);
      }

    /* Call End Reason Optional TLV */
      if(info->is_end_reason_available)
      {
        res->call_end_reason_valid = TRUE;
        res->call_end_reason_len = no_of_calls;
        
        res->call_end_reason[i].call_id = info->call_id;
        res->call_end_reason[i].call_end_reason = (call_end_reason_enum_v02)info->end_reason;

        QM_MSG_MED_2("End reason = %d for call id %d", info->end_reason, info->call_id);
      }

     #ifdef FEATURE_UUS
    /* UUS Info Optional TLV */
      if(info->is_uus_info_available)
      {
        res->uus_info_valid = TRUE;
        res->uus_info_len= no_of_calls;

        res->uus_info.call_id = info->call_id;
        res->uus_info.uus_type = (uus_type_enum_v02)info->uus_info.uus_type;
        res->uus_info.uus_dcs = (uus_dcs_enum_v02)info->uus_info.uus_dcs;
        res->uus_info.uus_data_len = (uint32_t)info->uus_info.uus_data_len;
         memscpy(res->uus_info.uus_data, QMI_VOICE_UUS_DATA_MAX_V02, info->uus_info.uus_data, info->uus_info.uus_data_len );

        QM_MSG_MED_3("UUS Type = %d Dcs =%d for call id %d", info->uus_info.uus_type, info->uus_info.uus_dcs, info->call_id);
      }
    #endif

    /* Alerting Type Optional TLV */
      if(info->is_alerting_type_available)
      {
        res->alerting_type_valid = TRUE;
        res->alerting_type_len = no_of_calls;
        
        res->alerting_type[i].call_id = info->call_id;
        res->alerting_type[i].alerting_type = (alerting_type_enum_v02)info->alerting_type;

        QM_MSG_MED_2("Alerting type = %d for call id %d", info->alerting_type, info->call_id);
      }
    
      if(info->is_cnap_info_available)
      {
        res->remote_party_name_valid = TRUE;
        res->remote_party_name_len = no_of_calls;
        res->remote_party_name[i].call_id = info->call_id;
        res->remote_party_name[i].name_pi = (pi_name_enum_v02)info->cnap_info.name_presentation;
        res->remote_party_name[i].coding_scheme = info->cnap_info.coding_scheme;
        res->remote_party_name[i].name_len = (uint32_t)info->cnap_info.name_len;
        memscpy(res->remote_party_name[i].name, QMI_VOICE_CALLER_NAME_MAX_V02, info->cnap_info.name, info->cnap_info.name_len );

        QM_MSG_MED_3("Cnap coding scheme = %d name length =%d for call id %d", info->cnap_info.coding_scheme, info->cnap_info.name_len, info->call_id);
      }

    /* Remote Party Number Optional TLV */
      if(info->is_num_available && (info->num.len<=CM_MAX_CALLED_INFO_CHARS))
      {
        res->remote_party_number_valid = TRUE;
        res->remote_party_number_len = no_of_calls;
        
        res->remote_party_number[i].call_id = info->call_id;
        res->remote_party_number[i].number_pi = (pi_num_enum_v02)info->num.pi;
        res->remote_party_number[i].number_len = (uint32_t)info->num.len;
        memscpy(res->remote_party_number[i].number, QMI_VOICE_NUMBER_MAX_V02, info->num.buf, info->num.len);

        QM_MSG_MED_2("Rp Number = %s for call id %d",(const char *)info->num.buf, info->call_id );
      }
    
    /* SIP URI Optional TLV */
    if(info->is_num_available && (info->num.len>CM_MAX_CALLED_INFO_CHARS))
      {
        res->sip_uri_valid = TRUE;
        res->sip_uri_len = no_of_calls;
        
        res->sip_uri[i].call_id = info->call_id;
        memscpy(res->sip_uri[i].sip_uri, QMI_VOICE_SIP_URI_MAX_V02, info->num.buf, info->num.len);

        QM_MSG_MED_2("SIP URI = %s for call id %d",(const char *)info->num.buf, info->call_id );
      }

    /*Alternate SIP URI Optional TLV */
      if( (info->is_alternate_uri_available) &&
          (info->alternate_uri.alternate_uri_length < QMI_VOICE_SIP_URI_MAX_V02) &&
          (info->alternate_uri.alternate_uri_length > 0) )
      {
        res->alternate_sip_uris_valid = TRUE;
        res->alternate_sip_uris_len = no_of_calls;
        
        res->alternate_sip_uris[i].call_id = info->call_id;
        memscpy(res->alternate_sip_uris[i].sip_uri, QMI_VOICE_SIP_URI_MAX_V02, info->alternate_uri.alternate_uri_buf, info->alternate_uri.alternate_uri_length);
    
        QM_MSG_MED_2("Alternate SIP URI = %s for call id %d",(const char *)info->alternate_uri.alternate_uri_buf, info->call_id );
      }
    
      /* Fill Mandatory basic call information */
      res->call_info_valid = TRUE;
      res->call_info_len = no_of_calls;
      res->call_info[i].call_id = info->call_id;
      res->call_info[i].call_state = (call_state_enum_v02)info->call_state;
      res->call_info[i].call_type = (call_type_enum_v02)info->call_type;
      res->call_info[i].direction = (call_direction_enum_v02)info->direction;
      res->call_info[i].mode = (call_mode_enum_v02)info->mode;
      res->call_info[i].is_mpty = info->is_mpty;
      res->call_info[i].als = (als_enum_v02)info->als;	  
   
      QM_MSG_MED_7("call id %d, Call State =%d, call type =%d, direction %d, Call Mode =%d, is mpty =%d, als =%d", info->call_id,info->call_state,
      info->call_type, info->direction, info->mode, info->is_mpty, info->als);
    }
  }

  response = NULL;

  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) res,
                                    (uint32_t) sizeof(voice_get_all_call_info_resp_msg_v02),
                                    &response )
    )
  {
    if(!qmi_voicei_send_response_enc((void*)cmd_buf_p, response))
    {
      QM_MSG_MED("Unable to send response for QMI VOICE GET ALL CALL INFO operation");
    }
  }

  if(res != NULL) QMI_VOICE_CM_IF_MEM_FREE(res);

} /* qmi_voicei_get_all_call_info_resp() */

#ifdef FEATURE_MMODE_CALL_SUPS
/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_MANAGE_CALLS_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    None 
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_manage_calls_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.manage_calls_resp),
                                    (uint32_t) sizeof(voice_manage_calls_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/* qmi_voicei_sups_manage_calls_resp */
#endif
#ifdef FEATURE_MMODE_SS_SUPS

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CLIP_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_get_clip_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.get_clip_resp),
                                    (uint32_t) sizeof(voice_get_clip_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/*qmi_voicei_sups_get_clip_resp*/


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_COLP_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_get_colp_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.get_colp_resp),
                                    (uint32_t) sizeof(voice_get_colp_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/* qmi_voicei_sups_get_colp_resp */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CALL_WAITING_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_get_call_waiting_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.get_call_waiting_resp),
                                    (uint32_t) sizeof(voice_get_call_waiting_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/* qmi_voicei_sups_get_call_waiting_resp */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CALL_BARRING_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    service class for the specified reason/facility for which call barring is active 
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_get_call_barring_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;
  uint16                   total_alpha_len = 0;
  uint16                  total_barr_list_tlv_size = 0;
  uint8                   barr_sc_count=0, barr_num_count=0;
  uint32 fail_cause_len=0;
  uint8 tag=0;
  uint8 i=0, j=0;
  struct
  {
    uint8 service_class;
  }call_barring_resp;

  struct
  {
    uint32 service_class_ext;
  }call_barring_ext_resp;

  struct
  {
    uint16 failure_cause;
  }get_call_barring_err_resp;
  struct
  {
    uint8 srv_type;
    uint8 reason;
  }sups_cc_data;
  qmi_voice_cm_if_sups_cc_result_s cc_res;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  response = NULL;
  errval = data_ptr->data.get_call_barring_resp.error;
  
  /* SIP Error Code*/
  if( data_ptr->data.get_call_barring_resp.sip_error_code_valid== TRUE )
  {
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      VOICEI_GET_CB_SIP_ERR_CODE,
                                      sizeof(data_ptr->data.get_call_barring_resp.sip_error_code),
                                      (void *) &data_ptr->data.get_call_barring_resp.sip_error_code))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }
  
   //Fill TLV 0x19: Failure Cause desc
  if(data_ptr->data.get_call_barring_resp.failure_cause_description_valid && 
   data_ptr->data.get_call_barring_resp.failure_cause_description_len<QMI_VOICE_CM_IF_MAX_FAILURE_DESC_LEN &&
   data_ptr->data.get_call_barring_resp.failure_cause_description_len!=0)
  {
    tag = VOICEI_GET_CB_FAILURE_CAUSE_DESC;
    fail_cause_len = 2*data_ptr->data.get_call_barring_resp.failure_cause_description_len + sizeof(data_ptr->data.get_call_barring_resp.failure_cause_description_len); 
    if( !(QMI_SVC_PKT_PUSH(&response, (void*)data_ptr->data.get_call_barring_resp.failure_cause_description, 2*data_ptr->data.get_call_barring_resp.failure_cause_description_len) &&
            QMI_SVC_PKT_PUSH(&response, (void*)&data_ptr->data.get_call_barring_resp.failure_cause_description_len, sizeof(data_ptr->data.get_call_barring_resp.failure_cause_description_len)) &&
            QMI_SVC_PKT_PUSH(&response, (void *)&fail_cause_len, VOICEI_TLV_LENGTH_SIZE) &&
            QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
     {
         errval = QMI_ERR_NO_MEMORY;
         dsm_free_packet(&response);
     }

      
  }

  /* Fill Retry Duration info if no error is present */
  if( data_ptr->data.get_call_barring_resp.is_retry_duration_available == TRUE )
  {
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      VOICEI_GET_CB_RETRY_DURATION,
                                      sizeof(data_ptr->data.get_call_barring_resp.retry_duration),
                                      (void *) &data_ptr->data.get_call_barring_resp.retry_duration))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }

  if( QMI_ERR_NONE == errval )
  {
    call_barring_resp.service_class = data_ptr->data.get_call_barring_resp.service_class;
    QM_MSG_MED_1("Sending QMI GET Call barring with service_class = %d", call_barring_resp.service_class);
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      VOICEI_GET_CB_SC,
                                      sizeof(call_barring_resp),
                                      (void *) &call_barring_resp))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }

  if( QMI_ERR_NONE == errval )
  {
    call_barring_ext_resp.service_class_ext = data_ptr->data.get_call_barring_resp.service_class_ext;
    QM_MSG_MED_1("Sending QMI GET Call barring with service_class_ext = %d", call_barring_ext_resp.service_class_ext);
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      VOICEI_GET_CB_RESP_SC_EXT,
                                      sizeof(call_barring_ext_resp),
                                      (void *) &call_barring_ext_resp))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }

  if( QMI_ERR_SUPS_FAILURE_CAUSE == errval )
  {
    get_call_barring_err_resp.failure_cause = data_ptr->data.get_call_barring_resp.sups_failure_cause;
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      VOICEI_GET_CB_FAILURE_CAUSE,
                                      sizeof(get_call_barring_err_resp),
                                      (void *) &get_call_barring_err_resp))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }
  else if( data_ptr->data.get_call_barring_resp.is_cc_modified )
  {
    cc_res = data_ptr->data.get_call_barring_resp.cc_result;
    if( cc_res.is_mod_to_sups )
    {
      sups_cc_data.srv_type = cc_res.service;
      sups_cc_data.reason = cc_res.reason;
      if(FALSE == qmi_svc_put_param_tlv(&response,
                                        VOICEI_GET_CB_CC_RESULT_SUPS,
                                        sizeof(sups_cc_data),
                                        (void *) &sups_cc_data))
      {
        errval = QMI_ERR_NO_MEMORY;
        dsm_free_packet(&response);
      }
    }
    else if( cc_res.is_mod_to_voice )
    {
      if(FALSE == qmi_svc_put_param_tlv(&response,
                                        VOICEI_GET_CB_CC_RESULT_CALL_ID,
                                        sizeof(cc_res.call_id),
                                        (void *) &cc_res.call_id))
      {
        errval = QMI_ERR_NO_MEMORY;
        dsm_free_packet(&response);
      }
    }
    else
    {
      QM_MSG_ERROR("Invalid call control data in response");
    }

    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      VOICEI_GET_CB_CC_RESULT_TYPE,
                                      sizeof(data_ptr->data.get_call_barring_resp.cc_mod_type),
                                      (void *) &(data_ptr->data.get_call_barring_resp.cc_mod_type)))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }

  }

  /* Fill alpha which resulted from SIM call control */
  if(data_ptr->data.get_call_barring_resp.is_alpha_available)
  {
    total_alpha_len = sizeof(data_ptr->data.get_call_barring_resp.alpha_id.alpha_dcs) + \
                      sizeof(data_ptr->data.get_call_barring_resp.alpha_id.alpha_len) + \
                      data_ptr->data.get_call_barring_resp.alpha_id.alpha_len;
    if(FALSE == qmi_svc_put_param_tlv(&response,
                                      VOICEI_GET_CB_ALPHA_ID,
                                      total_alpha_len,
                                      (void *) &(data_ptr->data.get_call_barring_resp.alpha_id)))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }
  
  //Fill TLV 0x17: Barred Number List
  if(data_ptr->data.get_call_barring_resp.sc_barr_num_list_valid)
  {
    tag = VOICEI_GET_CB_RESP_BARR_LIST;

    barr_sc_count = data_ptr->data.get_call_barring_resp.sc_barr_num_list_len;

    for(i=0;i<barr_sc_count;i++)
    {
      const qmi_voice_cm_if_icb_resp_s *barred_num = &data_ptr->data.get_call_barring_resp.sc_barr_num_list_info[i];
      barr_num_count = barred_num->barr_num_list_len;

      for(j=0;j<barr_num_count && j<QMI_VOICE_CM_IF_MAX_BARR_LIST_LEN;j++) 
      {
        const qmi_voice_cm_if_icb_sc_s *barred_num_sc = &barred_num->barr_num_list_info[j];
        if( !(QMI_SVC_PKT_PUSH(&response, (void *)barred_num_sc->barr_num, barred_num_sc->barr_num_len) &&
              QMI_SVC_PKT_PUSH(&response, (void *)&barred_num_sc->barr_num_len, sizeof(barred_num_sc->barr_num_len)) &&
              QMI_SVC_PKT_PUSH(&response, (void *)&barred_num_sc->barr_num_status, sizeof(barred_num_sc->barr_num_status))) )
        {
          errval = QMI_ERR_NO_MEMORY;
          dsm_free_packet(&response);
        }  
        total_barr_list_tlv_size = total_barr_list_tlv_size+\
                                   sizeof(barred_num_sc->barr_num_status)+\
                                   sizeof(barred_num_sc->barr_num_len)+\
                                   barred_num_sc->barr_num_len;
      }
      if(barred_num->barr_num_list_info)
      {
        QMI_VOICE_CM_IF_MEM_FREE((void *)barred_num->barr_num_list_info);
      }
    
      if( !(QMI_SVC_PKT_PUSH(&response, (void*)&barr_num_count, sizeof(barr_num_count)) &&
            QMI_SVC_PKT_PUSH(&response, (void *)&barred_num->barr_sc_ext, sizeof(barred_num->barr_sc_ext))) )
      {
        errval = QMI_ERR_NO_MEMORY;
        dsm_free_packet(&response);
      }
      total_barr_list_tlv_size = total_barr_list_tlv_size+\
                                 sizeof(barred_num->barr_sc_ext)+\
                                 sizeof(barr_num_count);
    }

    total_barr_list_tlv_size += sizeof(barr_sc_count);

    if( !(QMI_SVC_PKT_PUSH(&response, (void *)&barr_sc_count, sizeof(barr_sc_count)) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&total_barr_list_tlv_size, VOICEI_TLV_LENGTH_SIZE) &&
           QMI_SVC_PKT_PUSH(&response, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }    
  }
  
  if (!qmi_voicei_send_response(errval, (void*)cmd_buf_p, response))
  {
    QM_MSG_MED("Unable to send response for QMI VOICE GET CALL BARRING operation");
  }
}/* qmi_voicei_sups_get_call_barring_resp */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_SET_CALL_BARRING_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    None 
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_set_call_barring_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.set_call_barring_resp),
                                    (uint32_t) sizeof(voice_set_call_barring_password_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/* qmi_voicei_sups_set_call_barring_resp */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_SET_SUPS_SERVICE_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    None 
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_set_sups_service_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.set_sups_resp),
                                    (uint32_t) sizeof(voice_set_sups_service_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/* qmi_voicei_sups_set_sups_service_resp */
#endif

#ifndef FEATURE_MMODE_DISABLE_INBAND_SUPP

/*===========================================================================
  FUNCTION QMI_VOICEI_DTMF_IND()

  DESCRIPTION
    
    
  PARAMETERS
    sp      : service provided state pointer (user data)
    clid    : client ID
    sdu_in  : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_dtmf_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_dtmf_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {

      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_DTMF_IND_V02,
                                        (void *) &(data_ptr->data.dtmf_ind),
                                        (uint32_t) sizeof(voice_dtmf_ind_msg_v02),
                                        &ind )
        )
      {
        qmi_voicei_send_indication( cl_sp, QMI_VOICE_DTMF_IND_V02, ind );
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
} /* qmi_voicei_dtmf_ind() */
#endif

/*===========================================================================
  FUNCTION QMI_VOICEI_HANDOVER_IND()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE

    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_handover_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_handover_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_HANDOVER_IND_V02,
                                        (void *) &(data_ptr->data.handover_ind),
                                        (uint32_t) sizeof(voice_handover_ind_msg_v02),
                                        &ind )
        )
      {
        qmi_voicei_send_indication( cl_sp, QMI_VOICE_HANDOVER_IND_V02, ind );
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
} /* qmi_voicei_handover_ind() */

/*===========================================================================
  FUNCTION QMI_VOICEI_AOC_LOW_FUNDS_IND()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE

    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_aoc_low_funds_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_aoc_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_AOC_LOW_FUNDS_IND_V02,
                                        (void *) &(data_ptr->data.low_funds_ind),
                                        (uint32_t) sizeof(voice_aoc_low_funds_ind_msg_v02),
                                        &ind )
        )
      {
        qmi_voicei_send_indication( cl_sp, QMI_VOICE_AOC_LOW_FUNDS_IND_V02, ind );
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
} /* qmi_voicei_aoc_low_funds_ind() */

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
/*===========================================================================
  FUNCTION QMI_VOICEI_CONFERENCE_PARTICIPANTS_INFO_IND()

  DESCRIPTION

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
None

  SIDE EFFECTS
None
===========================================================================*/
static void qmi_voicei_conference_participants_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_conf_participants_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_CONFERENCE_PARTICIPANTS_INFO_IND_V02,
                                        (void *) &(data_ptr->data.parsed_conf_info),
                                        (uint32_t) sizeof(voice_conf_participants_info_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_CONFERENCE_PARTICIPANTS_INFO_IND_V02,ind );
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
} /* qmi_voicei_conference_participants_info_ind() */
#endif

/*===========================================================================
  FUNCTION QMI_VOICEI_CALL_CONTROL_RESULT_INFO_IND()

  DESCRIPTION

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
None

  SIDE EFFECTS
None
===========================================================================*/
static void qmi_voicei_call_control_result_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_cc_result_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_CALL_CONTROL_RESULT_INFO_IND_V02,
                                        (void *) &(data_ptr->data.cc_result_info_ind),
                                        (uint32_t) sizeof(voice_call_control_result_info_ind_msg_v02),
                                        &ind )
        )
      {
        qmi_voicei_send_indication( cl_sp,QMI_VOICE_CALL_CONTROL_RESULT_INFO_IND_V02,ind );
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
} /* qmi_voicei_call_control_result_info_ind() */

/*===========================================================================
  FUNCTION QMI_VOICEI_ALL_CALL_STATUS_IND()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_all_call_status_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type          * cl_sp;
  dsm_item_type *                       indication;
  qmi_voice_cm_if_all_call_info_s       *all_call_info;
  uint8     i, num_of_instances=0;
  int       tot_len=0,alpha_len=0,con_len=0,diags_len=0;
  int       called_len=0,redirect_len=0,verstat_len=0;
  boolean   is_ind_sent = FALSE;
  enum qm_subs_e subs = QM_SUBS_PRI;
  uint8     is_incoming_call_present = FALSE;
  uint8     conn_num_pi=0;
  uint8     is_adnl_info_present= FALSE;
  voice_all_call_status_ind_msg_v02  *ind;
  uint32 no_of_calls = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ind = QMI_VOICE_CM_IF_MEM_ALLOC(sizeof(voice_all_call_status_ind_msg_v02));
  if( NULL == ind )
  {
    qmi_voice_mem_fatal();
    return;
  }

  memset( ind, 0, sizeof( voice_all_call_status_ind_msg_v02 ) );
  all_call_info = (qmi_voice_cm_if_all_call_info_s*)&data_ptr->data.all_call_status_ind.info;

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_call_events) &&
         (!cl_sp->asubs_info.is_bind_set_by_client ||
          (cl_sp->asubs_info.as_id == data_ptr->header.as_id)))
      {

        indication = NULL;
	
        no_of_calls = (uint32_t)all_call_info->num_of_calls;

        for( i=0; (i<all_call_info->num_of_calls) && (i<QMI_VOICE_CM_IF_CALL_ID_MAX_PER_SUBS); i++ )
        {
          const qmi_voice_cm_if_call_info_s *const info = &all_call_info->call_info[i];

          /* Sip Error Code TLV */
          if(info->is_sip_error_code_available)
          {
            ind->sip_error_code_valid = TRUE;
            ind->sip_error_code_len = no_of_calls;
        
            ind->sip_error_code[i].call_id = info->call_id;
            ind->sip_error_code[i].sip_error_code = (uint16_t)info->sip_error_code;
            QM_MSG_MED_2("sip_error_code = %d for call id %d", info->sip_error_code, info->call_id);
          }

          /* IP Hist Info Optional TLV */
          if( (info->is_ip_hist_info_available) && 
              (info->ip_hist_info.ip_hist_info_length < QMI_VOICE_CM_IF_MAX_IP_HIST_INFO_LEN) && 
              (info->ip_hist_info.ip_hist_info_length != 0) )
          {
            /* ip_hist_info_length is the number of UTF16 characters */
            ind->ip_hist_info_valid = TRUE;
            ind->ip_hist_info_len = no_of_calls;
            ind->ip_hist_info[i].call_id = info->call_id;
            ind->ip_hist_info[i].ip_hist_info_len = (uint32_t)info->ip_hist_info.ip_hist_info_length;
            memscpy(ind->ip_hist_info[i].ip_hist_info, 2*QMI_VOICE_IP_HIST_INFO_MAX_LEN_V02, info->ip_hist_info.ip_hist_info_buf, 2*info->ip_hist_info.ip_hist_info_length);
            QM_MSG_MED_1("hist_info added for call id %d", info->call_id);
          }

          if( (info->is_end_reason_text_available) && 
              (info->end_reason_text.end_reason_text_length < QMI_VOICE_CM_IF_MAX_END_REASON_TEXT_LEN) && 
              (info->end_reason_text.end_reason_text_length != 0) )
          {            
            ind->end_reason_text_valid = TRUE;
            ind->end_reason_text_len = no_of_calls;
            
            ind->end_reason_text[i].call_id = info->call_id;
            ind->end_reason_text[i].end_reason_text_len = (uint32_t)info->end_reason_text.end_reason_text_length;
            memscpy(ind->end_reason_text[i].end_reason_text, 2*QMI_VOICE_END_REASON_TEXT_MAX_LEN_V02, info->end_reason_text.end_reason_text_buf, 2*info->end_reason_text.end_reason_text_length);			
            QM_MSG_MED_1("end_reason_text added for call id %d", info->call_id);
          }
        
          /* Second Alpha Optional TLV */
          if( (info->is_second_alpha_available == TRUE) && (info->call_state == QMI_VOICE_CM_IF_CALL_STATE_ORIG) )
          {
            ind->second_alpha_ident_valid = TRUE;
            ind->second_alpha_ident_len = no_of_calls;
            
            ind->second_alpha_ident[i].call_id = info->call_id;
            ind->second_alpha_ident[i].alpha_dcs = (alpha_dcs_enum_v02)info->second_alpha_info.alpha_dcs;
            ind->second_alpha_ident[i].alpha_text_len = (uint32_t)info->second_alpha_info.alpha_len;
            memscpy(ind->second_alpha_ident[i].alpha_text, QMI_VOICE_ALPHA_TEXT_MAX_V02, info->second_alpha_info.alpha_text, info->second_alpha_info.alpha_len);			
            QM_MSG_MED_3("Second Alpha coding scheme = %d name length =%d for call id %d", info->second_alpha_info.alpha_dcs, info->second_alpha_info.alpha_len, info->call_id);
          }
                
        /* Remote Party number Ext2 Optional TLV */
          if(info->is_num_available)
          {
            ind->remote_party_number_ext2_valid = TRUE;
            ind->remote_party_number_ext2_len = no_of_calls;
            
            ind->remote_party_number_ext2[i].call_id = info->call_id;
            ind->remote_party_number_ext2[i].rp_num_info.num_pi = (pi_num_enum_v02)info->num.pi;
            ind->remote_party_number_ext2[i].rp_num_info.num_si = (voice_si_enum_v02)info->num.si;
            ind->remote_party_number_ext2[i].rp_num_info.num_type = (voice_num_type_enum_v02)info->num.num_type;
            ind->remote_party_number_ext2[i].rp_num_info.num_plan = (voice_num_plan_enum_v02)info->num.num_plan;
            memscpy(ind->remote_party_number_ext2[i].rp_num_info.num, QMI_VOICE_SIP_URI_MAX_V02, info->num.buf, info->num.len);
            QM_MSG_MED_2("Remote Pty Number Ext2 = %s for call id %d",(const char *)info->num.buf, info->call_id );
          }

        /* Call attribute status info Optional TLV */
          if(info->is_call_attrib_status_available)
          {
            ind->call_attrib_status_valid = TRUE;
            ind->call_attrib_status_len = no_of_calls;
            
            ind->call_attrib_status[i].call_id = info->call_id;
            ind->call_attrib_status[i].call_attrib_status = (voice_call_attrib_status_enum_v02)info->call_attrib_status;
            QM_MSG_MED_1("Call attribute status added for call id %d", info->call_id);
          }

        /* Is Additional Call info Optional TLV */
          if(info->is_additional_call_info_available)
          {
            ind->is_add_info_present_valid = TRUE;
            ind->is_add_info_present_len = no_of_calls;
            
            ind->is_add_info_present[i].call_id = info->call_id;
            ind->is_add_info_present[i].is_add_info_present = TRUE;
            ind->is_add_info_present[i].num_indications = info->num_indications;
            QM_MSG_MED_1("Additional Info Flag added for call id %d", info->call_id);
          }

        /* Media ID Optional TLV */
          if(info->is_media_id_available)
          {
            ind->media_id_valid = TRUE;
            ind->media_id_len = no_of_calls;
            
            ind->media_id[i].call_id = info->call_id;
            ind->media_id[i].media_id = info->media_id;
            QM_MSG_MED_2("Media ID = %d for call id %d", info->media_id, info->call_id);
          }

         /* Orig fail reason Optional TLV */
          if(info->is_orig_fail_reason_available)
          {
            ind->orig_fail_reason_valid = TRUE;
            ind->orig_fail_reason_len = no_of_calls;
            ind->orig_fail_reason[i].call_id = info->call_id;
            ind->orig_fail_reason[i].orig_fail_reason = (call_end_reason_enum_v02)info->orig_fail_reason;

            QM_MSG_MED_1("Orig fail reason populated for call id %d", info->call_id);
          }

        /* Local call capabilities info Optional TLV */
          if(info->is_local_cap_available)
          {
            ind->local_call_capabilities_info_valid = TRUE;
            ind->local_call_capabilities_info_len = no_of_calls;
            
            ind->local_call_capabilities_info[i].call_id = info->call_id;
            ind->local_call_capabilities_info[i].audio_attrib = (voice_call_attribute_type_mask_v02)info->local_cap.audio_attrib;
            ind->local_call_capabilities_info[i].audio_cause = (voice_capability_restrict_cause_enum_v02)info->local_cap.audio_restrict_cause;
            #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
            ind->local_call_capabilities_info[i].video_attrib = (voice_call_attribute_type_mask_v02)info->local_cap.video_attrib;
            ind->local_call_capabilities_info[i].video_cause = (voice_capability_restrict_cause_enum_v02)info->local_cap.video_restrict_cause;
            #endif

            QM_MSG_MED_1("Local call capabilities added for call id %d", info->call_id);
          }

        /* Peer call capabilities info Optional TLV */
          if(info->is_peer_cap_available)
          {
            ind->peer_call_capabilities_info_valid = TRUE;
            ind->peer_call_capabilities_info_len = no_of_calls;
            
            ind->peer_call_capabilities_info[i].call_id = info->call_id;
            ind->peer_call_capabilities_info[i].audio_attrib = (voice_call_attribute_type_mask_v02)info->peer_cap.audio_attrib;
            ind->peer_call_capabilities_info[i].audio_cause = (voice_capability_restrict_cause_enum_v02)info->peer_cap.audio_restrict_cause;
            #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
            ind->peer_call_capabilities_info[i].video_attrib = (voice_call_attribute_type_mask_v02)info->peer_cap.video_attrib;
            ind->peer_call_capabilities_info[i].video_cause = (voice_capability_restrict_cause_enum_v02)info->peer_cap.video_restrict_cause;
            #endif

            QM_MSG_MED_1("Peer call capabilities added for call id %d", info->call_id);
          }
  
        /* Child number info Optional TLV */
          if(info->is_child_num_available)
          {
            ind->child_number_valid = TRUE;
            ind->child_number_len = no_of_calls;
            
            ind->child_number[i].call_id = info->call_id;
            ind->child_number[i].number_len = (uint32_t)info->child_num.len;
            memscpy(ind->child_number[i].number, QMI_VOICE_SIP_URI_MAX_V02, info->child_num.buf, info->child_num.len);

            QM_MSG_MED_1("Child number added for call id %d", info->call_id);
          }

        /* IP caller name Optional TLV */
          if(info->is_ip_caller_name_available)
          {
            ind->ip_caller_name_valid = TRUE;
            ind->ip_caller_name_len = no_of_calls;
            
            ind->ip_caller_name[i].call_id = info->call_id;
            ind->ip_caller_name[i].ip_caller_name_len = (uint32_t)info->ip_caller_name.name_len;
			/*IP CALLER NAME is in UTF16 format*/
            memscpy(ind->ip_caller_name[i].ip_caller_name, 2*QMI_VOICE_IP_CALLER_NAME_MAX_LEN_V02, info->ip_caller_name.name, 2*(info->ip_caller_name.name_len));
   
            QM_MSG_HIGH_2("IP caller name added for call id %d len %d", info->call_id,ind->ip_caller_name[i].ip_caller_name_len);

          }

       #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
        /* Display Text info Optional TLV */
          if(info->is_display_text_available)
          {
            ind->display_text_valid = TRUE;
            ind->display_text_len = no_of_calls;
            
            ind->display_text[i].call_id = info->call_id;
            ind->display_text[i].display_text_len = (uint32_t)info->display_text.text_len;
            memscpy(ind->display_text[i].display_text, 2*QMI_VOICE_DISPLAY_TEXT_MAX_LEN_V02, info->display_text.text, 2*info->display_text.text_len);
			
            QM_MSG_MED_1("Display text added for call id %d", info->call_id);
          }

        /* Parent ID info  Optional TLV */
          if(info->is_parent_id_available)
          {
            ind->srvcc_parent_call_info_valid = TRUE;
            ind->srvcc_parent_call_info_len = no_of_calls;
            
            ind->srvcc_parent_call_info[i].call_id = info->call_id;
            ind->srvcc_parent_call_info[i].parent_call_id = info->parent_call_id;
            ind->srvcc_parent_call_info[i].is_parent_id_cleared = info->is_parent_call_id_cleared;
           
            QM_MSG_MED_3("Parent_ID = %d, Cleared=%d for call id %d", info->parent_call_id, info->is_parent_call_id_cleared, info->call_id);
          }

        /* Is SRVCC  Optional TLV */
          if(info->is_srvcc_call_available)
          {
            ind->is_srvcc_valid = TRUE;
            ind->is_srvcc_len = no_of_calls;
            
            ind->is_srvcc[i].call_id = info->call_id;
            ind->is_srvcc[i].is_srvcc_call = (uint8_t)info->is_srvcc_call;

            QM_MSG_MED_2("Is SRVCC Call = %d for call id %d", info->is_srvcc_call, info->call_id);
          }        
        #endif
        
       /* Alerting Pattern Optional TLV */
          if(info->is_alerting_pattern_available)
          {
            ind->alerting_pattern_valid = TRUE;
            ind->alerting_pattern_len = no_of_calls;
            
            ind->alerting_pattern[i].call_id = info->call_id;
            ind->alerting_pattern[i].alerting_pattern = (alerting_pattern_enum_v02)info->alerting_pattern;
       
            QM_MSG_MED_2("Alerting pattern = %d for call id %d", info->alerting_pattern, info->call_id);
          }

        /* Connected number Info Optional TLV */
          if(info->is_conn_num_available)
          {
            ind->conn_party_num_valid = TRUE;
            ind->conn_party_num_len = no_of_calls;;
            
            ind->conn_party_num[i].call_id = info->call_id;
            ind->conn_party_num[i].conn_num_pi = (pi_num_enum_v02)info->conn_num_data.pi;
            ind->conn_party_num[i].conn_num_si = (voice_si_enum_v02)info->conn_num_data.si;
            ind->conn_party_num[i].conn_num_type = (voice_num_type_enum_v02)info->conn_num_data.num_type;
            ind->conn_party_num[i].conn_num_plan = (voice_num_plan_enum_v02)info->conn_num_data.num_plan;
            ind->conn_party_num[i].conn_num_len = (uint32_t)info->conn_num_data.len;
            memscpy(ind->conn_party_num[i].conn_num, QMI_VOICE_CALLER_ID_MAX_V02, info->conn_num_data.num, info->conn_num_data.len);

            QM_MSG_MED_4("Connected num info pi= %d,si=%d,num_len=%d,call id = %d",info->conn_num_data.pi,info->conn_num_data.si,info->conn_num_data.len,info->call_id );
            //QM_MSG_MED_3("Connected num info pi= %d,si=%d,num_len=%d",info->conn_num_data.pi,info->conn_num_data.si,info->conn_num_data.len );
            //QM_MSG_MED_1("Connected num info call id = %d",info->call_id );
          }

        if(info->is_called_party_ringing_status_avail)
        {
          ind->is_called_party_ringing_valid = TRUE;
          ind->is_called_party_ringing_len = no_of_calls;
          ind->is_called_party_ringing[i].call_id = info->call_id;
          ind->is_called_party_ringing[i].is_called_party_ringing = (uint8_t)info->is_called_party_ringing;

          QM_MSG_MED_2("Is Called party ringing value = %d call id = %d",info->is_called_party_ringing, info->call_id);
        }

        /* Called party number Info Optional TLV */
          if(info->is_called_party_num_available)
          {
            ind->called_party_num_valid = TRUE;
            ind->called_party_num_len = no_of_calls;

            ind->called_party_num[i].call_id = info->call_id;
            ind->called_party_num[i].num_pi = (pi_num_enum_v02)info->called_party_num_data.pi;
            ind->called_party_num[i].num_si = (voice_si_enum_v02)info->called_party_num_data.si;
            ind->called_party_num[i].num_type = (voice_num_type_enum_v02)info->called_party_num_data.num_type;
            ind->called_party_num[i].num_plan = (voice_num_plan_enum_v02)info->called_party_num_data.num_plan;
            ind->called_party_num[i].num_len = (uint32_t)info->called_party_num_data.len;
            memscpy(ind->called_party_num[i].num, QMI_VOICE_NUMBER_MAX_V02, info->called_party_num_data.num, info->called_party_num_data.len);

            QM_MSG_MED_4("Called party num info pi= %d,si=%d,num_len=%d,call id = %d",info->called_party_num_data.pi,info->called_party_num_data.si,info->called_party_num_data.len,info->call_id );
          }
        
        /* Redirecting party number Info Optional TLV */
          if(info->is_redirecting_party_num_available)
          {
            ind->redirecting_party_num_valid = TRUE;
            ind->redirecting_party_num_len = no_of_calls;
            
            ind->redirecting_party_num[i].call_id = info->call_id;
            ind->redirecting_party_num[i].num_pi = (pi_num_enum_v02)info->redirecting_party_num_data.pi;
            ind->redirecting_party_num[i].num_si = (voice_si_enum_v02)info->redirecting_party_num_data.si;
            ind->redirecting_party_num[i].num_type = (voice_num_type_enum_v02)info->redirecting_party_num_data.num_type;
            ind->redirecting_party_num[i].num_plan = (voice_num_plan_enum_v02)info->redirecting_party_num_data.num_plan;
            ind->redirecting_party_num[i].num_len = (uint32_t)info->redirecting_party_num_data.len;
            memscpy(ind->called_party_num[i].num, QMI_VOICE_NUMBER_MAX_V02, info->redirecting_party_num_data.num, info->redirecting_party_num_data.len);
            
            QM_MSG_MED_4("Redirect party num info pi= %d,si=%d,num_len=%d,call id = %d",info->redirecting_party_num_data.pi,
                         info->redirecting_party_num_data.si,info->redirecting_party_num_data.len,info->call_id  );
          }

        /* Call End Diagnostics Info Optional TLV */
          if(info->is_diagnostics_info_available)
          {
            ind->diagnostic_info_valid= TRUE;
            ind->diagnostic_info_len = no_of_calls;
            
            ind->diagnostic_info[i].call_id = info->call_id;
            ind->diagnostic_info[i].diagnostic_info_len = (uint32_t)info->diagnostic_data.diagnostic_length;
            memscpy(ind->diagnostic_info[i].diagnostic_info, QMI_VOICE_DIAGNOSTIC_INFO_MAX_V02, info->diagnostic_data.diagnostics, info->diagnostic_data.diagnostic_length);
           
            QM_MSG_MED_2("Diagnostic info len=%d for call id %d", info->diagnostic_data.diagnostic_length, info->call_id);
          }

        #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
        /* VS Variant info Optional TLV */
          if(info->is_vs_call_variant_valid)
          {
            ind->vs_variant_valid = TRUE;
            ind->vs_variant_len = no_of_calls;
            
            ind->vs_variant[i].call_id = info->call_id;
            ind->vs_variant[i].vs_variant = (vs_variant_type_enum_v02)info->vs_call_variant;
       
            QM_MSG_MED_2("Vs variant = %d for call id %d", info->vs_call_variant, info->call_id);
          }
       
        /* Video Call Attibutes Optional TLV */
          if(info->is_video_attrib_available)
          {
            ind->video_attrib_valid = TRUE;
            ind->video_attrib_len = no_of_calls;
            
            ind->video_attrib[i].call_id = info->call_id;
            ind->video_attrib[i].call_attributes = (voice_call_attribute_type_mask_v02)info->video_attrib;

            QM_MSG_MED_2("Video Attrib = %d for call id %d", info->video_attrib, info->call_id);
          }    
        #endif

        /* Audio Call Attibutes Optional TLV */
          if(info->is_audio_attrib_available)
          {
            ind->audio_attrib_valid = TRUE;
            ind->audio_attrib_len = no_of_calls;
            
            ind->audio_attrib[i].call_id = info->call_id;
            ind->audio_attrib[i].call_attributes = (voice_call_attribute_type_mask_v02)info->audio_attrib;

            QM_MSG_MED_2("Audio Attrib = %d for call id %d", info->audio_attrib, info->call_id);
          }

         /* Alpha Optional TLV */
          if(info->is_alpha_available)
          {
            ind->alpha_id_valid  = TRUE;
            ind->alpha_id_len = no_of_calls;
            
            ind->alpha_id[i].call_id = info->call_id;
            ind->alpha_id[i].alpha_dcs = (alpha_dcs_enum_v02)info->alpha_info.alpha_dcs;
            ind->alpha_id[i].alpha_text_len = (uint32_t)info->alpha_info.alpha_len;
            memscpy(ind->alpha_id[i].alpha_text, QMI_VOICE_ALPHA_TEXT_MAX_V02, info->alpha_info.alpha_text, info->alpha_info.alpha_len);
           
            QM_MSG_MED_3("Alpha coding scheme = %d name length =%d for call id %d", info->alpha_info.alpha_dcs, info->alpha_info.alpha_len, info->call_id);
          }
     
        /* Call End Reason Optional TLV */
          if(info->is_end_reason_available)
          {
            ind->call_end_reason_valid = TRUE;
            ind->call_end_reason_len = no_of_calls;
            
            ind->call_end_reason[i].call_id = info->call_id;
            ind->call_end_reason[i].call_end_reason = (call_end_reason_enum_v02)info->end_reason;

            QM_MSG_MED_2("End reason = %d for call id %d", info->end_reason, info->call_id);
          }

        /* Alerting Type Optional TLV */
          if(info->is_alerting_type_available)
          {            
            ind->alerting_type_valid = TRUE;
            ind->alerting_type_len = no_of_calls;
            
            ind->alerting_type[i].call_id = info->call_id;
            ind->alerting_type[i].alerting_type = (alerting_type_enum_v02)info->alerting_type;

            QM_MSG_MED_2("Alerting type = %d for call id %d", info->alerting_type, info->call_id);
          }

        /* Cnap Info Optional TLV */
          if(info->is_cnap_info_available)
          {
            ind->remote_party_name_valid = TRUE;
            ind->remote_party_name_len = no_of_calls;
            ind->remote_party_name[i].call_id = info->call_id;
            ind->remote_party_name[i].name_pi = (pi_name_enum_v02)info->cnap_info.name_presentation;
            ind->remote_party_name[i].coding_scheme = info->cnap_info.coding_scheme;
            ind->remote_party_name[i].name_len = (uint32_t)info->cnap_info.name_len;
            memscpy(ind->remote_party_name[i].name, QMI_VOICE_CALLER_NAME_MAX_V02, info->cnap_info.name, info->cnap_info.name_len );

            QM_MSG_MED_3("Cnap coding scheme = %d name length =%d for call id %d", info->cnap_info.coding_scheme, info->cnap_info.name_len, info->call_id);
          }
            
        /* Remote Party Number Optional TLV */
          if(info->is_num_available && (info->num.len<=CM_MAX_CALLED_INFO_CHARS))
          {
             ind->remote_party_number_valid = TRUE;
             ind->remote_party_number_len = no_of_calls;
             
             ind->remote_party_number[i].call_id = info->call_id;
             ind->remote_party_number[i].number_pi = (pi_num_enum_v02)info->num.pi;
             ind->remote_party_number[i].number_len = (uint32_t)info->num.len;
             memscpy(ind->remote_party_number[i].number, QMI_VOICE_NUMBER_MAX_V02, info->num.buf, info->num.len);

            QM_MSG_MED_2("Rp Number = %s for call id %d",(const char *)info->num.buf, info->call_id );
          }
              	
        /* SIP URI Optional TLV */
          if(info->is_num_available && (info->num.len>CM_MAX_CALLED_INFO_CHARS))
          {
            ind->sip_uri_valid = TRUE;
            ind->sip_uri_len = no_of_calls;
            
            ind->sip_uri[i].call_id = info->call_id;
            memscpy(ind->sip_uri[i].sip_uri, QMI_VOICE_SIP_URI_MAX_V02, info->num.buf, info->num.len);

            QM_MSG_MED_2("SIP URI = %s for call id %d",(const char *)info->num.buf, info->call_id );
          }

        /* Ext Remote Party number  info (IP) Optional TLV */
          if(info->is_num_available && (info->num.len>CM_MAX_CALLED_INFO_CHARS))
          {            
            ind->ip_num_info_valid = TRUE;
            ind->ip_num_info_len = no_of_calls;
            
            ind->ip_num_info[i].call_id = info->call_id;
            ind->ip_num_info[i].ip_num_pi = (pi_num_enum_v02)info->num.pi;
            memscpy(ind->ip_num_info[i].ip_num, QMI_VOICE_SIP_URI_MAX_V02, info->num.buf, info->num.len);

            QM_MSG_MED_2("Ext Remote Pty number = %s for call id %d",(const char *)info->num.buf, info->call_id );
          }

        /* Ext Connected Party number  info (IP) Optional TLV */
        
          if(info->is_conn_num_available && (info->conn_num_data.len>CM_MAX_CALLED_INFO_CHARS))
          {
            ind->conn_ip_num_info_valid = TRUE;
            ind->conn_ip_num_info_len = no_of_calls;
            
            ind->conn_ip_num_info[i].call_id = info->call_id;
            ind->conn_ip_num_info[i].conn_ip_num_pi = (pi_num_enum_v02)info->conn_num_data.pi;
            memscpy(ind->conn_ip_num_info[i].conn_ip_num, QMI_VOICE_SIP_URI_MAX_V02, info->conn_num_data.num, info->conn_num_data.len);

            QM_MSG_MED_2("Ext connected number = %s for call id %d",(const char *)info->conn_num_data.num, info->call_id );
          }

          /* Robocall Verstat Info Optional TLV */
          QM_MSG_HIGH_2("is_unwanted_call_flag: %d for i: %d",info->is_unwanted_call_feature_available, i);          
		  
          if(info->is_unwanted_call_feature_available)
          {
            ind->mt_call_verstat_valid = TRUE;
            ind->mt_call_verstat_len = no_of_calls;
          
            ind->mt_call_verstat[i].call_id = info->call_id;
            ind->mt_call_verstat[i].verstat_info.mt_call_verstat_info = (mt_ims_verstat_enum_v02) info->is_unwanted_call.mt_call_verstat;
            ind->mt_call_verstat[i].verstat_info.network_supports_unwanted_call = info->is_unwanted_call.is_nw_support_unwanted_call;
          
            QM_MSG_MED_3("nw_support_unwanted_call = %d, mt_call_verstat = %d, call id = %d",
              info->is_unwanted_call.is_nw_support_unwanted_call, info->is_unwanted_call.mt_call_verstat, info->call_id);
          }

          /* Alternate SIP URI Optional TLV */
          if( (info->is_alternate_uri_available) &&
              (info->alternate_uri.alternate_uri_length < QMI_VOICE_SIP_URI_MAX_V02) &&
              (info->alternate_uri.alternate_uri_length > 0) )
          {
            ind->alternate_sip_uris_valid = TRUE;
            ind->alternate_sip_uris_len = no_of_calls;
            
            ind->alternate_sip_uris[i].call_id = info->call_id;
            memscpy(ind->alternate_sip_uris[i].sip_uri, QMI_VOICE_SIP_URI_MAX_V02, info->alternate_uri.alternate_uri_buf, info->alternate_uri.alternate_uri_length);
        
            QM_MSG_MED_2("Alternate SIP URI = %s for call id %d",(const char *)info->alternate_uri.alternate_uri_buf, info->call_id );
          }

          /* Fill Mandatory basic call information */
          ind->call_info_len = no_of_calls;
          ind->call_info[i].call_id = info->call_id;
          ind->call_info[i].call_state = (call_state_enum_v02)info->call_state;
          ind->call_info[i].call_type = (call_type_enum_v02)info->call_type;
          ind->call_info[i].direction = (call_direction_enum_v02)info->direction;
          ind->call_info[i].mode = (call_mode_enum_v02)info->mode;
          ind->call_info[i].is_mpty = info->is_mpty;
          ind->call_info[i].als = (als_enum_v02)info->als;
          
 
	      QM_MSG_MED_7("Call Info for Call id %d :Call State =%d, call type =%d, dir %d, Call Mode =%d, is mpty =%d, als =%d",info->call_id, info->call_state, 
          info->call_type, info->direction,info->mode, info->is_mpty, info->als);

          if(info->call_state == QMI_VOICE_CM_IF_CALL_STATE_INCOMING)
          {
            is_incoming_call_present = TRUE;
          }
        }
     
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_ALL_CALL_STATUS_IND_V02,
                                        (void *) ind,
                                        (uint32_t) sizeof(voice_all_call_status_ind_msg_v02),
                                        &indication )
        )
      {
         /* This is a broadcast indication. Send this to all the clients one at
         a time just to avoid dealing with instances.*/
        if( !qmi_voicei_send_indication(cl_sp,
                              QMI_VOICE_ALL_CALL_STATUS_IND_V02,
                                 indication))
        {
            QM_MSG_MED("VOICE ALL CALL STATUS Indication could not be sent");
        }
        else
        {
          is_ind_sent = TRUE;
          if(cl_sp->asubs_info.is_bind_set_by_client)
          {
            subs = (enum qm_subs_e)cl_sp->asubs_info.as_id;
          }
        }
      }

    }    

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }

  if(is_ind_sent && is_incoming_call_present)
  {
    /* Check if indication retransmission is required and start the timer for retransmit */
    qm_timer_start(QM_TIMER_ID_INCOM_EVT_RESEND, subs);
  }

  QMI_VOICE_CM_IF_MEM_FREE(ind);

  return;
} /* qmi_voicei_all_call_status_ind() */

#if defined(FEATURE_MMODE_USS_SUPS)

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_USSD_IND()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_ussd_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_ussd_notification_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_USSD_IND_V02,
                                        (void *) &(data_ptr->data.ussd_ind),
                                        (uint32_t) sizeof(voice_ussd_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_USSD_IND_V02, ind );
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
} /* qmi_voicei_sups_ussd_ind() */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_USSD_RELEASE_IND()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_ussd_release_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_ussd_notification_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
        if( !qmi_voicei_send_indication( cl_sp,
                              VOICEI_CMD_VAL_SUPS_USSD_RELEASE_IND,
                                     NULL))
        {
          QM_MSG_MED("VOICE SUPS USSD RELEASE Indication could not be sent");
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
      return;
} /* qmi_voicei_sups_ussd_release_ind() */
#endif

/*===========================================================================
  FUNCTION QMI_VOICEI_UUS_IND()
  
  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_uus_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_uus_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_UUS_IND_V02,
                                        (void *) &(data_ptr->data.uus_ind),
                                        (uint32_t) sizeof(voice_uus_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp, QMI_VOICE_UUS_IND_V02, ind );
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
} /* qmi_voicei_uus_ind() */
#if defined (FEATURE_MMODE_SS_SUPS) 

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_IND()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type * cl_sp;
  dsm_item_type *                ind;
  uint16 tot_len=0;
  uint8 tag=VOICEI_UUS_IND_DATA;
  uint16 total_len = 0;
  uint16                  total_barr_list_tlv_size = 0;
  uint8                   barr_num_count = 0;
  uint8  i = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_sups_events) &&
         (!cl_sp->asubs_info.is_bind_set_by_client ||
         (cl_sp->asubs_info.as_id == data_ptr->header.as_id)))
      {

        ind = NULL;

        if(data_ptr->data.sups_ind.is_ntwk_resp)
        {
          if(FALSE == qmi_svc_put_param_tlv(&ind,
                                            VOICEI_SUPS_IND_SUPS_DATA_SOURCE,
                                            sizeof(data_ptr->data.sups_ind.is_ntwk_resp),
                                            (void *) &data_ptr->data.sups_ind.is_ntwk_resp))
          {
            dsm_free_packet(&ind);
            continue;
          }

          if(data_ptr->data.sups_ind.is_failure_cause_valid && 
            (FALSE == qmi_svc_put_param_tlv(&ind,
                                            VOICEI_SUPS_IND_FAILURE_CAUSE,
                                            sizeof(data_ptr->data.sups_ind.sups_failure_cause),
                                            (void *) &data_ptr->data.sups_ind.sups_failure_cause)))
          {
            dsm_free_packet(&ind);
            continue;
          }

          if(data_ptr->data.sups_ind.is_clir_status_valid && 
            (FALSE == qmi_svc_put_param_tlv(&ind,
                                            VOICEI_SUPS_IND_CLIR_STATUS,
                                            sizeof(data_ptr->data.sups_ind.cli_status),
                                            (void *) &data_ptr->data.sups_ind.cli_status)))
          {
            dsm_free_packet(&ind);
            continue;
          }

          if(data_ptr->data.sups_ind.is_clip_status_valid && 
            (FALSE == qmi_svc_put_param_tlv(&ind,
                                            VOICEI_SUPS_IND_CLIP_STATUS,
                                            sizeof(data_ptr->data.sups_ind.cli_status),
                                            (void *) &data_ptr->data.sups_ind.cli_status)))
          {
            dsm_free_packet(&ind);
            continue;
          }

          if(data_ptr->data.sups_ind.is_colp_status_valid && 
            (FALSE == qmi_svc_put_param_tlv(&ind,
                                            VOICEI_SUPS_IND_COLP_STATUS,
                                            sizeof(data_ptr->data.sups_ind.cli_status),
                                            (void *) &data_ptr->data.sups_ind.cli_status)))
          {
            dsm_free_packet(&ind);
            continue;
          }

	  if(data_ptr->data.sups_ind.is_colr_status_valid && 
            (FALSE == qmi_svc_put_param_tlv(&ind,
                                            VOICEI_SUPS_IND_COLR_STATUS,
                                            sizeof(data_ptr->data.sups_ind.cli_status),
                                            (void *) &data_ptr->data.sups_ind.cli_status)))
          {
            dsm_free_packet(&ind);
            continue;
          }

	  if(data_ptr->data.sups_ind.is_cnap_status_valid && 
            (FALSE == qmi_svc_put_param_tlv(&ind,
                                            VOICEI_SUPS_IND_CNAP_STATUS,
                                            sizeof(data_ptr->data.sups_ind.cli_status),
                                            (void *) &data_ptr->data.sups_ind.cli_status)))
          {
            dsm_free_packet(&ind);
            continue;
          }

		  
          if(data_ptr->data.sups_ind.is_call_fwd_info_from_ntwk_valid)
          {
            const qmi_voice_cm_if_call_fwd_data_ntwk_s *fwd_rsp = &data_ptr->data.sups_ind.cf_data;
            int idx = 0;
            uint8 call_fwd_rsp_tag = VOICEI_SUPS_IND_CF_NTWK_DATA;
            boolean success = TRUE;

            /* Get the highest index */
            idx = fwd_rsp->num_cfw_instances - 1;
            total_len = 0;
            QM_MSG_HIGH_2("CFW: Instances :%d,idx=%d  ", fwd_rsp->num_cfw_instances,idx);
            /* Loop through the service class instances and fill the data */
            while( (idx >= 0) && (idx<QMI_VOICE_CM_IF_MAX_SERVICE_CLASS_INSTANCES))
            {
              /* Put No reply timer */
              if(!QMI_SVC_PKT_PUSH(&ind, (void*)&fwd_rsp->sc_instances[idx].no_reply_timer, 
                                                               sizeof(fwd_rsp->sc_instances[idx].no_reply_timer)))
              {
                dsm_free_packet(&ind);
                success = FALSE;
                break;
              }
              total_len += sizeof(uint8);
              /* Put fwd number if available */
              if(fwd_rsp->sc_instances[idx].num_len > 0)
              {
                if(!QMI_SVC_PKT_PUSH(&ind, (void*)fwd_rsp->sc_instances[idx].num_buf, fwd_rsp->sc_instances[idx].num_len))
                {
                  dsm_free_packet(&ind);\
                  success = FALSE;
                  break;
                }
                total_len += fwd_rsp->sc_instances[idx].num_len;
              }
              /* Put num_len, service_class, service_status */
              if(!(QMI_SVC_PKT_PUSH(&ind, (void*)&fwd_rsp->sc_instances[idx].num_len, 
                                                   sizeof(fwd_rsp->sc_instances[idx].num_len)) &&
                   QMI_SVC_PKT_PUSH(&ind, (void*)&fwd_rsp->sc_instances[idx].service_class, 
                                                   sizeof(fwd_rsp->sc_instances[idx].service_class))&&
                   QMI_SVC_PKT_PUSH(&ind, (void*)&fwd_rsp->sc_instances[idx].service_status,
                                                   sizeof(fwd_rsp->sc_instances[idx].service_status))
                   ) )
              {
                dsm_free_packet(&ind);
                success = FALSE;
                break;
              }
              total_len += 3 * sizeof(uint8);
              /* Decrement the index */
              idx--;
            } /* while */
            
            if( success )
            {
              total_len += sizeof(fwd_rsp->num_cfw_instances);
              QM_MSG_HIGH_2("CFW: Instances :%d,total_len after=%d  ", fwd_rsp->num_cfw_instances,total_len);
              /* Put the num_instances, total length, Tag id */
              if(!(QMI_SVC_PKT_PUSH(&ind, (void*)&fwd_rsp->num_cfw_instances, sizeof(fwd_rsp->num_cfw_instances)) &&
                   QMI_SVC_PKT_PUSH(&ind, (void*)&total_len, VOICEI_TLV_LENGTH_SIZE)&&
                   QMI_SVC_PKT_PUSH(&ind, (void*)&call_fwd_rsp_tag, VOICEI_TLV_TAG_SIZE)
                   ) )
              {
                dsm_free_packet(&ind);
                continue;
              }
            }
            else
            {
              continue;
            }
          }/* Call fwd data filled */
        }

        if(data_ptr->data.sups_ind.is_new_pwd_available)
        {
          tot_len = sizeof(data_ptr->data.sups_ind.new_pwd_again) + \
                        sizeof(data_ptr->data.sups_ind.new_pwd);
          tag = VOICEI_SUPS_IND_NEW_PWD;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)data_ptr->data.sups_ind.new_pwd_again, sizeof(data_ptr->data.sups_ind.new_pwd_again)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void*)data_ptr->data.sups_ind.new_pwd, sizeof(data_ptr->data.sups_ind.new_pwd)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
    
        if(data_ptr->data.sups_ind.is_cb_pwd_available)
        {
          tot_len = sizeof(data_ptr->data.sups_ind.pwd) ;
          tag = VOICEI_SUPS_IND_PWD;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)data_ptr->data.sups_ind.pwd, sizeof(data_ptr->data.sups_ind.pwd)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
    
        if(data_ptr->data.sups_ind.is_alpha_available)
        {
          tot_len = sizeof(data_ptr->data.sups_ind.alpha_info.alpha_dcs) + \
                        sizeof(data_ptr->data.sups_ind.alpha_info.alpha_len) + \
                        data_ptr->data.sups_ind.alpha_info.alpha_len;
          tag = VOICEI_SUPS_IND_ALPHA;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.alpha_info, tot_len) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
    
        if(data_ptr->data.sups_ind.is_call_id_available)
        {
          tot_len = sizeof(data_ptr->data.sups_ind.call_id) ;
          tag = VOICEI_SUPS_IND_CALL_ID;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.call_id, sizeof(data_ptr->data.sups_ind.call_id)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
    
        if(data_ptr->data.sups_ind.is_uss_available)
        {
          tot_len = sizeof(data_ptr->data.sups_ind.uss_info.uss_dcs) + \
                        sizeof(data_ptr->data.sups_ind.uss_info.uss_len) + \
                        data_ptr->data.sups_ind.uss_info.uss_len;
          tag = VOICEI_SUPS_IND_USS_INFO;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.uss_info, tot_len) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
    
        if(data_ptr->data.sups_ind.is_ussd_info_utf16_available)
        {
          QM_MSG_MED("sending UCS2 data");
          tot_len = sizeof(data_ptr->data.sups_ind.ussd_info_utf16.ussd_data_utf16_len) + \
                        (data_ptr->data.sups_ind.ussd_info_utf16.ussd_data_utf16_len)*2;

          tag = VOICEI_SUPS_IND_USS_INFO_UTF16;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)data_ptr->data.sups_ind.ussd_info_utf16.ussd_data_utf16, (data_ptr->data.sups_ind.ussd_info_utf16.ussd_data_utf16_len)*2) &&
                QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.ussd_info_utf16.ussd_data_utf16_len, sizeof(data_ptr->data.sups_ind.ussd_info_utf16.ussd_data_utf16_len)) &&
                QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
        if(data_ptr->data.sups_ind.is_cfw_nrtimer_available)
        {
          tot_len = sizeof(data_ptr->data.sups_ind.no_reply_timer) ;
          tag = VOICEI_SUPS_IND_CFW_NR_TIMER;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.no_reply_timer, sizeof(data_ptr->data.sups_ind.no_reply_timer)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
    
        if(data_ptr->data.sups_ind.is_cfw_number_available)
        {
          tot_len = data_ptr->data.sups_ind.num_len;
          tag = VOICEI_SUPS_IND_CFW_NUM;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)data_ptr->data.sups_ind.num_buf, tot_len) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
    
        if(data_ptr->data.sups_ind.is_reason_available)
        {
          tot_len = sizeof(data_ptr->data.sups_ind.reason) ;
          tag = VOICEI_SUPS_IND_REASON;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.reason, sizeof(data_ptr->data.sups_ind.reason)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
    
        if(data_ptr->data.sups_ind.is_service_class_available)
        {
          tot_len = sizeof(data_ptr->data.sups_ind.service_class) ;
          tag = VOICEI_SUPS_IND_SERVICE_CLASS;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.service_class, sizeof(data_ptr->data.sups_ind.service_class)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
    
	if(data_ptr->data.sups_ind.is_service_class_ext_available)
        {
          tot_len = sizeof(data_ptr->data.sups_ind.service_class_ext) ;
          tag = VOICEI_SUPS_IND_SERVICE_CLASS_EXT;
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.service_class_ext, sizeof(data_ptr->data.sups_ind.service_class_ext)) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }
        }
        if(data_ptr->data.sups_ind.barr_num_list_valid)
        {
          tag = VOICEI_SUPS_IND_BARR_LIST;
          barr_num_count = data_ptr->data.sups_ind.barr_num_list_len;
          total_barr_list_tlv_size = 0;
          for(i=0;i<barr_num_count;i++) 
          {
            const qmi_voice_cm_if_icb_ind_s *barred_num = &data_ptr->data.sups_ind.barr_num_list[i];
            if( !(QMI_SVC_PKT_PUSH(&ind, (void*)barred_num->barr_num, barred_num->barr_num_len) &&
                  QMI_SVC_PKT_PUSH(&ind, (void *)&barred_num->barr_num_len, sizeof(barred_num->barr_num_len))) )
            {
              dsm_free_packet(&ind);
              continue;
            }  
            total_barr_list_tlv_size = total_barr_list_tlv_size+ \
                                       sizeof(barred_num->barr_num_len)+ \
                                       barred_num->barr_num_len;
          }
          
          total_barr_list_tlv_size += sizeof(barr_num_count);
          if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&barr_num_count, sizeof(barr_num_count)) &&
                 QMI_SVC_PKT_PUSH(&ind, (void *)&total_barr_list_tlv_size, VOICEI_TLV_LENGTH_SIZE) &&
                 QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
          {
            dsm_free_packet(&ind);
            continue;
          }    
        }
    
    
    /*Mandatory TLV */
        tot_len = sizeof(data_ptr->data.sups_ind.service_type) + \
                      sizeof(data_ptr->data.sups_ind.is_modified_by_call_control);
        tag = VOICEI_SUPS_IND_SERVICE_INFO;
        if( !(QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.is_modified_by_call_control, sizeof(data_ptr->data.sups_ind.is_modified_by_call_control)) &&
                 QMI_SVC_PKT_PUSH(&ind, (void*)&data_ptr->data.sups_ind.service_type, sizeof(data_ptr->data.sups_ind.service_type)) &&
                 QMI_SVC_PKT_PUSH(&ind, (void *)&tot_len, VOICEI_TLV_LENGTH_SIZE) &&
                 QMI_SVC_PKT_PUSH(&ind, (void *)&tag, VOICEI_TLV_TAG_SIZE)) )
        {
          dsm_free_packet(&ind);
          continue;
        }

      if( !qmi_voicei_send_indication(cl_sp,
                          VOICEI_CMD_VAL_SUPS_IND,
                                      ind))
        {
        QM_MSG_MED("VOICE SUPS Indication could not be sent");
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }

  return;
} /* qmi_voicei_sups_ussd_ind() */
#endif

/*===========================================================================
  FUNCTION qmi_voicei_speech_codec_info_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_speech_codec_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_speech_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_SPEECH_CODEC_INFO_IND_V02,
                                        (void *) &(data_ptr->data.speech_codec_info_ind),
                                        (uint32_t) sizeof(voice_speech_codec_info_ind_msg_v02),
                                        &ind )
        )
      {
        qmi_voicei_send_indication( cl_sp, QMI_VOICE_SPEECH_CODEC_INFO_IND_V02, ind );
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
} /* qmi_voicei_speech_codec_info_ind() */

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
/*===========================================================================
  FUNCTION qmi_voicei_tty_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_tty_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL ))
  {
    if( (cl_sp->voice_info.reg_tty_info_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_TTY_IND_V02,
                                        (void *) &(data_ptr->data.tty_info),
                                        (uint32_t) sizeof(voice_tty_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_TTY_IND_V02,ind );
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
} /* qmi_voicei_tty_ind() */


/*===========================================================================
  FUNCTION qmi_voicei_audio_rat_change_info_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_audio_rat_change_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_audio_rat_change_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_AUDIO_RAT_CHANGE_INFO_IND_V02,
                                        (void *) &(data_ptr->data.audio_rat_change_info),
                                        (uint32_t) sizeof(voice_audio_rat_change_info_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_AUDIO_RAT_CHANGE_INFO_IND_V02,ind);
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
} /* qmi_voicei_audio_rat_change_info_ind() */

/*===========================================================================
  FUNCTION qmi_voicei_conference_participant_status_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_conference_participant_status_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type * cl_sp;
  dsm_item_type *                ind;
  voice_conf_participant_status_info_ind_msg_v02 *conf_part_status_info_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  conf_part_status_info_ptr = (voice_conf_participant_status_info_ind_msg_v02*)&data_ptr->data;

  QM_MSG_MED_4("Participant status IND: call_id=%d, op=%d,uri=%s, status_sip_code =%d", 
               conf_part_status_info_ptr->call_id, conf_part_status_info_ptr->op_status.operation,
  	conf_part_status_info_ptr->participant_uri, conf_part_status_info_ptr->op_status.sip_status);

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_conf_participants_events) &&
       (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)))
    {  
      ind = NULL;

      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_CONF_PARTICIPANT_STATUS_INFO_IND_V02,
                                        (void *) &(data_ptr->data.participant_status_info),
                                        (uint32_t) sizeof(voice_conf_participant_status_info_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_CONF_PARTICIPANT_STATUS_INFO_IND_V02, ind );
      }

    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
  return;
} /* qmi_voicei_conference_participant_status_ind() */


/*===========================================================================
  FUNCTION qmi_voicei_conf_info_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_conf_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_conference_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_CONFERENCE_INFO_IND_V02,
                                        (void *) &(data_ptr->data.conference_info_ind),
                                        (uint32_t) sizeof(voice_conference_info_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_CONFERENCE_INFO_IND_V02,ind );
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
} /* qmi_voicei_conf_info_ind() */
#endif

/*===========================================================================
  FUNCTION qmi_voicei_additional_call_info_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_additional_call_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_add_call_info_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_ADDITIONAL_CALL_INFO_IND_V02,
                                        (void *) &(data_ptr->data.additional_call_info),
                                        (uint32_t) sizeof(voice_additional_call_info_ind_msg_v02),
                                        &ind )
        )
      {
        qmi_voicei_send_indication( cl_sp,QMI_VOICE_ADDITIONAL_CALL_INFO_IND_V02,ind );
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
} /* qmi_voicei_additional_call_info_ind() */

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
/*===========================================================================
  FUNCTION qmi_voicei_conf_join_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_conf_join_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_conference_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_CONFERENCE_JOIN_IND_V02,
                                        (void *) &(data_ptr->data.conference_join_ind),
                                        (uint32_t) sizeof(voice_conference_join_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_CONFERENCE_JOIN_IND_V02,ind );
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
} /* qmi_voicei_conf_join_ind() */


/*===========================================================================
  FUNCTION qmi_voicei_conf_participant_update_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_conf_participant_update_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_conference_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_CONFERENCE_PARTICIPANT_UPDATE_IND_V02,
                                        (void *) &(data_ptr->data.conf_participant_info_ind),
                                        (uint32_t) sizeof(voice_conference_participant_update_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_CONFERENCE_PARTICIPANT_UPDATE_IND_V02,ind );
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
}/* qmi_voicei_conf_participant_update_ind() */
#endif

#ifndef FEATURE_MMODE_DISABLE_IMS_EXT
/*===========================================================================
  FUNCTION qmi_voicei_e911_orig_fail_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void qmi_voicei_e911_orig_fail_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type * cl_sp;
  dsm_item_type *                ind;
  qmi_voice_cm_if_e911_orig_fail_ind_s *e911_orig_info_ptr;
  struct qmi_voice_e911_orig_fail_ind_s_type orig_ind;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  e911_orig_info_ptr = (qmi_voice_cm_if_e911_orig_fail_ind_s*)&data_ptr->data;

  QM_MSG_MED_2 ("E911 orig info IND:call_id=%d,end_reason=%d",e911_orig_info_ptr->call_id,e911_orig_info_ptr->end_reason);

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if((cl_sp->voice_info.reg_e911_orig_fail_events) &&     
       (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)))
    {
  
      ind = NULL;
  
      orig_ind.e911_orig_fail_ind.call_id = e911_orig_info_ptr->call_id;
      orig_ind.e911_orig_fail_ind.end_reason = e911_orig_info_ptr->end_reason;

      if(FALSE == qmi_svc_put_param_tlv(&ind,
                                     VOICEI_E911_ORIG_FAIL,
                                     sizeof(orig_ind.e911_orig_fail_ind),
                                     (void*)&orig_ind.e911_orig_fail_ind))
      {
        dsm_free_packet(&ind);
        continue;
      }
  
      if( !qmi_voicei_send_indication(cl_sp, VOICEI_CMD_VAL_E911_ORIG_FAIL_IND, ind))
      {
        QM_MSG_MED("VOICE TTY Indication could not be sent" );
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
  return;
} /* qmi_voicei_e911_orig_fail_ind() */
#endif

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
/*===========================================================================
  FUNCTION qmi_voicei_vice_dialog_info_ind()

  DESCRIPTION 
   VICE indication from IMS 
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_vice_dialog_info_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type * cl_sp = NULL;
  dsm_item_type *                ind = NULL;
  vice_dialog_info_ind_msg_v02  *vice_dialog_info_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  vice_dialog_info_ptr = (vice_dialog_info_ind_msg_v02*)&data_ptr->data;

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if((cl_sp->voice_info.reg_vice_dialog_info_events) &&
       (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)))
    {  
      ind = NULL;

      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_VICE_DIALOG_INFO_IND_V02,
                                        (void *) &(data_ptr->data.vice_dialog_info_ind),
                                        (uint32_t) sizeof(vice_dialog_info_ind_msg_v02),
                                        &ind )
        )
      {
        if( !qmi_voicei_send_indication( cl_sp,
                                         QMI_VOICE_VICE_DIALOG_INFO_IND_V02,
                                         ind )
          )
        {
          QM_MSG_MED_1("QMI_VOICE_VICE_DIALOG_INFO_IND_V02 could not be sent to client with clnt=%p.", cl_sp->clnt);
        }
      }

    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
  return;
} /* qmi_voicei_vice_dialog_info_ind() */
#endif

#ifndef FEATURE_MMODE_DISABLE_INBAND_SUPP

/*===========================================================================
  FUNCTION QMI_VOICEI_START_CONT_DTMF()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_start_cont_dtmf
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_START_CONT_DTMF_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.start_cont_dtmf),
                                         (uint32_t) sizeof(voice_start_cont_dtmf_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_START_CONT_DTMF;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_start_cont_dtmf() */

/*===========================================================================
  FUNCTION QMI_VOICEI_STOP_CONT_DTMF()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_stop_cont_dtmf
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_STOP_CONT_DTMF_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.stop_cont_dtmf),
                                         (uint32_t) sizeof(voice_stop_cont_dtmf_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_STOP_CONT_DTMF;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_stop_cont_dtmf() */
#endif

/*===========================================================================
  FUNCTION QMI_VOICEI_INFO_REC_IND()

  DESCRIPTION
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_info_rec_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_call_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_INFO_REC_IND_V02,
                                        (void *) &(data_ptr->data.info_rec_ind),
                                        (uint32_t) sizeof(voice_info_rec_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_INFO_REC_IND_V02,ind );
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
} /* qmi_voicei_info_rec_ind() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_NOTIFICATION_IND()

  DESCRIPTION
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_notification_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_sups_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_SUPS_NOTIFICATION_IND_V02,
                                        (void *) &(data_ptr->data.sups_notify_ind),
                                        (uint32_t) sizeof(voice_sups_notification_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_SUPS_NOTIFICATION_IND_V02,ind );
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
} /* qmi_voicei_sups_notification_ind() */

/* Supplementary Services API's*/
#if defined (FEATURE_MMODE_SS_SUPS)

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CLIR()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_get_clir
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_CLIR_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_clir),
                                         (uint32_t) sizeof(voice_get_clir_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_GET_CLIR;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_get_clir() */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CLIR_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_get_clir_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.get_clir_resp),
                                    (uint32_t) sizeof(voice_get_clir_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
} /* qmi_voicei_sups_get_clir_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CALL_FORWARDING()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_get_call_forwarding
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_CALL_FORWARDING_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_call_fwd),
                                         (uint32_t) sizeof(voice_get_call_forwarding_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_GET_CALL_FORWARDING;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_get_call_forwarding() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CALL_FORWARDING_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_get_call_forwarding_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.get_call_fwd_resp),
                                    (uint32_t) sizeof(voice_get_call_forwarding_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
} /* qmi_voicei_sups_get_call_forwarding_resp() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CLIP()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_get_clip
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_CLIP_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_clip),
                                         (uint32_t) sizeof(voice_get_clip_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_GET_CLIP;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_get_clip() */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_COLP()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_get_colp
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_COLP_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_colp),
                                         (uint32_t) sizeof(voice_get_colp_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_GET_COLP;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_get_colp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CALL_WAITING()

  DESCRIPTION
    Get the CallWaiting status of a service class if not specified 
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_sups_get_call_waiting
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_CALL_WAITING_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_call_waiting),
                                         (uint32_t) sizeof(voice_get_call_waiting_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_GET_CALL_WAITING;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
}


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CALL_BARRING()

  DESCRIPTION
    Get the Callbarring status for a specified reason with service class.
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_sups_get_call_barring
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_CALL_BARRING_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_call_barring),
                                         (uint32_t) sizeof(voice_get_call_barring_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_GET_CALL_BARRING;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_get_call_barring() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_SET_CALL_BARRING()

  DESCRIPTION
    Set the Callbarring Password for a specified reason with service class.
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_sups_set_call_barring
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_SET_CALL_BARRING_PASSWORD_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.set_call_barring),
                                         (uint32_t) sizeof(voice_set_call_barring_password_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_SET_CALL_BARRING;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_set_call_barring() */
#endif
#ifdef FEATURE_MMODE_CALL_SUPS
/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_MANAGE_CALLS()

  DESCRIPTION
    Set the Callbarring Password for a specified reason with service class.
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_sups_manage_calls
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  uint8              type;
  uint16             len;
  uint16             expected_len;
  void *             value;
  boolean            got_v_in_required;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  value             = NULL;
  got_v_in_required = FALSE;

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sups_type);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sups_type;
        got_v_in_required = TRUE;
        break;

      case VOICEI_MNG_CALLS_REQ_CALL_ID:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.call_id);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.call_id;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.call_id_valid = TRUE;
        break;

      case VOICEI_MNG_CALLS_REQ_REJ_CAUSE:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.reject_cause);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.reject_cause;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.reject_cause_valid = TRUE;
        break;

      default:
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if ( (expected_len != 0) && (expected_len != len) )
    {
      QM_MSG_MED_2("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    if ( len != dsm_pullup( sdu_in, value, len ) )
    {                                                                    
      QM_MSG_MED("Invalid length in TLV");
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd = QMI_VOICE_CM_IF_CMD_SUPS_MANAGE_CALLS;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_get_call_barring() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_SET_SUPS_SERVICE()

  DESCRIPTION
    Set the Call Independant supplementary services with specified reason and service.
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_sups_set_sups_service
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  uint8              type;
  uint16             len;
  uint16             expected_len;
  void *             value;
  boolean            got_v_in_required;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  uint8 i=0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  value             = NULL;
  got_v_in_required = FALSE;

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));


  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.sups_service_info);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.sups_service_info;
        got_v_in_required = TRUE;
        break;
      case VOICEI_SET_SUPS_SC:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class;
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class_valid = TRUE;
        break;
      case VOICEI_SET_SUPS_SC_EXT:
	expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class_ext);
	value = &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class_ext;
	cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class_ext_valid = TRUE;
	break;
      case VOICEI_SET_SUPS_BARR_PWD:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.password);
        value = cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.password;
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.password_valid = TRUE;
        break;
      case VOICEI_SET_SUPS_FWD_NUM:
        value = cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_number;
        if(len > sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_number))
        {
          QM_MSG_MED("Invalid calling number length");
          errval = QMI_ERR_ARG_TOO_LONG;
          
          goto send_result;
        }
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_number_len = (uint8)len;
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_number_valid = TRUE;
        break;
      case VOICEI_SET_SUPS_NR_TIMER:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.no_reply_timer);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.no_reply_timer;
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.no_reply_timer_valid = TRUE;
        break;
      case VOICEI_SET_SUPS_NUM_TYPE_PLAN:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.num_type_plan);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.num_type_plan;
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.num_type_plan_valid = TRUE;
        break;
      case VOICEI_SET_SUPS_BARR_NUM_LIST:
        if ( !(QMI_VOICEI_PULL(sdu_in, &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.barr_num_list_len, sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.barr_num_list_len)) ) )
        {
          QM_MSG_MED("Invalid length in barr list TLV");
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        else
        {
          if (cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.barr_num_list_len > QMI_VOICE_CM_IF_MAX_BARR_LIST_LEN)
          {
            QM_MSG_MED("Invalid barr_num_list_len TLV");
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }

          for(i=0; i<cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.barr_num_list_len; i++)
          {
            if(!(QMI_VOICEI_PULL(sdu_in, &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.barr_num_list[i].barr_num_len, sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.barr_num_list[i].barr_num_len)) &&
                 QMI_VOICEI_PULL(sdu_in, cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.barr_num_list[i].barr_num, cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.barr_num_list[i].barr_num_len))
                 )
             {
               QM_MSG_MED("Invalid data in barr list TLV");
               errval = QMI_ERR_MALFORMED_MSG;
               goto send_result;
             }
          }
        }
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.barr_num_list_valid = TRUE;
        continue; //By design; do not change.
      case VOICEI_SET_SUPS_COLR_PI:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.colr_pi);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.colr_pi;
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.colr_pi_valid = TRUE;
        break;
      case VOICEI_SET_SUPS_CALL_FWD_START_TIME:
        if(len > sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_start_time))
        {
          QM_MSG_MED("Invalid Start time length");
          errval = QMI_ERR_ARG_TOO_LONG;
          goto send_result;
        }
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_start_time;
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_start_time_valid = TRUE;
        break;
      case VOICEI_SET_SUPS_CALL_FWD_END_TIME:
        if(len > sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_end_time))
        {
          QM_MSG_MED("Invalid End time length");
          errval = QMI_ERR_ARG_TOO_LONG;
          goto send_result;
        }
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_end_time;
        cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.fwd_end_time_valid = TRUE;
        break;
      default:
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if ( (expected_len != 0) && (expected_len != len) )
    {
      QM_MSG_MED_2("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    if ( len != dsm_pullup( sdu_in, value, len ) )
    {                                                                    
      QM_MSG_MED("Invalid length in TLV");
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if( (cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class_valid &&
  	   cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class_ext_valid)
  	   &&
  	  (cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class != 
  	   cmd_ptr->cmd.cm_if_data_ptr->data.set_sups_info.service_class_ext)
    )
    {
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd = QMI_VOICE_CM_IF_CMD_SUPS_SET_SUPS_SERVICE;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_set_sups_service() */

#endif
#if defined FEATURE_MMODE_USS_SUPS
/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_ANSWER_USSD()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_answer_ussd
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_ANSWER_USSD_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.answer_ussd),
                                         (uint32_t) sizeof(voice_answer_ussd_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_ANSWER_USSD;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_answer_ussd */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_ANSWER_USSD_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_answer_ussd_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.answer_ussd_resp),
                                    (uint32_t) sizeof(voice_answer_ussd_resp_msg_v02),
                                    &response )
    )
  {
   qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
} /* qmi_voicei_sups_answer_ussd_resp */


/*===========================================================================
  FUNCTION QMI_VOICEI_ORIG_USSD()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_orig_ussd
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_ORIG_USSD_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.ussd_orig_info),
                                         (uint32_t) sizeof(voice_orig_ussd_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_ORIG_USSD;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_orig_ussd() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_ORIG_USSD_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_orig_ussd_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.ussd_orig_resp),
                                    (uint32_t) sizeof(voice_orig_ussd_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/*qmi_voicei_sups_orig_ussd_resp*/


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_CANCEL_USSD()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_cancel_ussd
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_CANCEL_USSD_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.cancel_ussd),
                                         (uint32_t) sizeof(voice_cancel_ussd_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_CANCEL_USSD;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_cancel_ussd() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_ORIG_USSD_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_cancel_ussd_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.cancel_ussd_resp),
                                    (uint32_t) sizeof(voice_cancel_ussd_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/*qmi_voicei_sups_cancel_ussd_resp*/
#endif

/*===========================================================================
  FUNCTION QMI_VOICEI_SET_CONFIG_ITEMS()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_set_modem_config
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_SET_CONFIG_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.set_config),
                                         (uint32_t) sizeof(voice_set_config_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SET_MODEM_CONFIG;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_set_modem_config() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SET_MODEM_CONFIG_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_set_modem_config_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.set_config_resp),
                                    (uint32_t) sizeof(voice_set_config_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/*qmi_voicei_set_modem_config_resp*/

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
/*===========================================================================
  FUNCTION QMI_VOICEI_SRVCC_CALL_CONFIG_REQ()

  DESCRIPTION
    This Command will fill the call information in CM for enabling SRVCC of calls
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_srvcc_call_config_req
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  uint8              type,i=0,j=0;
  uint16             len;
  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voice_cm_if_set_srvcc_call_context_s  *srvcc_calls_info;
  qmi_voicei_client_state_type * client_sp;

  uint8              instance_id=0;
  boolean            instance_found = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  srvcc_calls_info = &cmd_ptr->cmd.cm_if_data_ptr->data.srvcc_calls_info;

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        if ( !(QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->num_calls, sizeof(srvcc_calls_info->num_calls)) ) )
        {
          QM_MSG_MED("Invalid length in call info TLV");
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        else
        {
          if (srvcc_calls_info->num_calls > QMI_VOICE_CM_IF_MAX_SRVCC_CALLS)
          {
            QM_MSG_HIGH_1("Invalid num_calls TLV %d",
                                   srvcc_calls_info->num_calls);
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }

          for(i=0; i<srvcc_calls_info->num_calls; i++)
          {
            if(!(QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].instance_id, sizeof(srvcc_calls_info->srvcc_calls[i].instance_id)) &&
                 QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].call_type, sizeof(srvcc_calls_info->srvcc_calls[i].call_type)) &&
                 QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].call_state, sizeof(srvcc_calls_info->srvcc_calls[i].call_state)) &&
                 QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].call_substate, sizeof(srvcc_calls_info->srvcc_calls[i].call_substate)) &&
                 QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].is_mpty_call, sizeof(srvcc_calls_info->srvcc_calls[i].is_mpty_call)) &&
                 QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].direction, sizeof(srvcc_calls_info->srvcc_calls[i].direction)) &&
                 QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].num_len, sizeof(srvcc_calls_info->srvcc_calls[i].num_len)) &&
                 QMI_VOICEI_PULL(sdu_in, srvcc_calls_info->srvcc_calls[i].num, srvcc_calls_info->srvcc_calls[i].num_len))
                 )
             {
               QM_MSG_MED("Invalid data in call info TLV");
               errval = QMI_ERR_MALFORMED_MSG;
               goto send_result;
             }
          }
        }
        break;
      case VOICEI_SRVCC_CALL_CONFIG_REQ_ALERTING_TYPE:
        if ( !(QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->alerting_type_list_len, sizeof(srvcc_calls_info->alerting_type_list_len)) ) )
        {
          QM_MSG_MED("Invalid length in call info TLV");
          errval = QMI_ERR_MALFORMED_MSG;
          goto send_result;
        }
        else
        {
          if (srvcc_calls_info->alerting_type_list_len > QMI_VOICE_CM_IF_MAX_SRVCC_CALLS)
          {
            QM_MSG_HIGH_1("Invalid alerting_type_list_len TLV %d",
                                 srvcc_calls_info->alerting_type_list_len);
            errval = QMI_ERR_MALFORMED_MSG;
            goto send_result;
          }
          
          for(i=0; i<srvcc_calls_info->alerting_type_list_len; i++)
          {
            if(QMI_VOICEI_PULL(sdu_in, &instance_id, sizeof(instance_id)))
            {
              instance_found = FALSE;
              for(j=0;j<srvcc_calls_info->num_calls;j++)
              {
                if(srvcc_calls_info->srvcc_calls[j].instance_id == instance_id)
                {
                  instance_found = TRUE;
                  srvcc_calls_info->srvcc_calls[j].is_alerting_type_valid = TRUE;
                  if(!QMI_VOICEI_PULL(sdu_in, &srvcc_calls_info->srvcc_calls[i].alerting_type, sizeof(srvcc_calls_info->srvcc_calls[i].alerting_type)))
                  {
                    QM_MSG_MED("Invalid data in alerting_type TLV");
                    errval = QMI_ERR_MALFORMED_MSG;
                    goto send_result;
                  }
                }
              }
              if(instance_found == FALSE)
              {
                QM_MSG_MED_1("instance ID %d given in the alerting_type not found in call list", instance_id);
                errval = QMI_ERR_MALFORMED_MSG;
                goto send_result;
              }
            }
            else
            {
               QM_MSG_MED("Invalid instance id in alerting_type TLV");
               errval = QMI_ERR_MALFORMED_MSG;
               goto send_result;
            }
          }
        }
        break;
      default:
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break;
    }
  }

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd = QMI_VOICE_CM_IF_CMD_SRVCC_CALL_CONFIG;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }


  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_srvcc_call_config_req() */


/*===========================================================================
  FUNCTION qmi_voicei_srvcc_call_config_resp()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_srvcc_call_config_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;
  qmi_error_e_type        errval;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
  //QM_MSG_MED("QMI VOICE qmi_voicei_srvcc_call_config_resp");

  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Build the response from back to front
  -------------------------------------------------------------------------*/
  errval = data_ptr->data.set_srvcc_resp.error;
  response = NULL;

  if (!qmi_voicei_send_response(errval, (void*)cmd_buf_p, response))
  {
    QM_MSG_MED("Unable to send response for qmi_voicei_srvcc_call_config operation");
  }
}
#endif

/*===========================================================================
  FUNCTION QMI_VOICEI_GET_CONFIG_ITEMS()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_get_modem_config
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_CONFIG_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_config),
                                         (uint32_t) sizeof(voice_get_config_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_GET_MODEM_CONFIG;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_get_modem_config() */

/*===========================================================================
  FUNCTION QMI_VOICEI_GET_MODEM_CONFIG_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_get_modem_config_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  

  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.get_config_resp),
                                    (uint32_t) sizeof(voice_get_config_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc((void*)cmd_buf_p, response);
  }
}/*qmi_voicei_get_modem_config_resp*/

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
/*===========================================================================
  FUNCTION QMI_VOICEI_BIND_SUBSCRIPTION()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_bind_subscription
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  dsm_item_type     *response = NULL;
  qmi_error_e_type   errval;
  qmi_result_e_type  result;
  boolean            retval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_BIND_SUBSCRIPTION_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.bind_subscription),
                                         (uint32_t) sizeof(voice_bind_subscription_req_msg_v02));
  if(errval) goto send_result;

  if((uint8)cmd_ptr->cmd.cm_if_data_ptr->data.bind_subscription.subs_type >= QMI_VOICE_CM_IF_AS_ID_MAX)
  {
    QM_MSG_MED_1("Unsupported TLV value (%d) for subs_type.", cmd_ptr->cmd.cm_if_data_ptr->data.bind_subscription.subs_type);
    errval = QMI_ERR_NO_SUBSCRIPTION;
    goto send_result;
  }

  if(client_sp != NULL)
  {
    client_sp->asubs_info.as_id = (uint8)cmd_ptr->cmd.cm_if_data_ptr->data.bind_subscription.subs_type;
    client_sp->asubs_info.is_bind_set_by_client = TRUE;
    QM_MSG_MED_2("BIND REQUESTED BY: clnt=%p Bounded to as_id=%d ",client_sp->clnt,client_sp->asubs_info.as_id);
  }
  else
  {
    /*Freeing cm_if_data_ptr and cmd_ptr*/
    QMI_VOICE_CM_IF_MEM_FREE(cmd_ptr->cmd.cm_if_data_ptr);
    qmi_mmode_free_cmd_data_buf(cmd_ptr);
    return QMI_SVC_RESPONSE_NONE;
  }

send_result:
  /*Freeing cm_if_data_ptr and cmd_ptr*/
  if(cmd_ptr != NULL)
  {
    if(cmd_ptr->cmd.cm_if_data_ptr != NULL)
    {
      QMI_VOICE_CM_IF_MEM_FREE(cmd_ptr->cmd.cm_if_data_ptr);
    }
    qmi_mmode_free_cmd_data_buf(cmd_ptr);
  }

  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS 
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();
  return response;
}/*qmi_voicei_bind_subscription*/
#endif

#if defined(FEATURE_MMODE_USS_SUPS)

/*===========================================================================
  FUNCTION QMI_VOICEI_ORIG_USSD_NO_WAIT()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_orig_ussd_no_wait
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *                          cl_sp,
  dsm_item_type **                sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  voice_uss_info_type_v02         *uusd_data;
  qmi_voice_cmd_list_public_type   info_ptr;
  qmi_result_e_type   result;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  memset(&info_ptr, 0, sizeof(info_ptr));

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_ORIG_USSD_NO_WAIT_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.ussd_orig_no_wait_info),
                                         (uint32_t) sizeof(voice_orig_ussd_no_wait_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_ORIG_USSD_NO_WAIT;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)client_sp;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  uusd_data = &(cmd_ptr->cmd.cm_if_data_ptr->data.ussd_orig_no_wait_info.uss_info);

  /* Check the incoming UUSD parameters. */
  if ((uusd_data->uss_data_len == 0) ||
      (uusd_data->uss_data_len >= QMI_VOICE_USS_DATA_MAX_V02) ||
      (uusd_data->uss_dcs < USS_DCS_ASCII_V02) ||
      (uusd_data->uss_dcs > USS_DCS_UCS2_V02))
  {
    QM_MSG_HIGH("Received ORIG_USSD_NO_WAIT with Invalid Data");
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  /* Check to see if a USSD request is in progress. */
  if (qmi_voice_cmd_list_query_by_event(QMI_VOICE_CM_IF_EVT_SUPS_USS_IND,
                                        &info_ptr, cmd_ptr->cmd.cm_if_data_ptr->header.as_id) == E_SUCCESS)
  {
    errval = QMI_ERR_INCOMPATIBLE_STATE;
    goto send_result;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

send_result:
  if (errval == QMI_ERR_NONE)
  {
    result = QMI_RESULT_SUCCESS;
  }
  else
  {
    result = QMI_RESULT_FAILURE;
    if(cmd_ptr != NULL)
    {
      if(cmd_ptr->cmd.cm_if_data_ptr != NULL)
      {
        QMI_VOICE_CM_IF_MEM_FREE(cmd_ptr->cmd.cm_if_data_ptr);
      }
      qmi_mmode_free_cmd_data_buf(cmd_ptr);
    }
  }
  
  return qmi_voicei_send_result(errval, NULL);
} /* qmi_voicei_sups_orig_ussd_no_wait() */

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_ORIG_USSD_NO_WAIT_IND()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_orig_ussd_no_wait_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Pull the client state pointer from the handle. For no_wait_ind, the
     handle is the client state and not the cmd_buf. */
  cl_sp = (qmi_voicei_client_state_type *)data_ptr->header.handle;

  /* Verify that the client state has not been freed. */
  if ( NULL == cl_sp )
  {
    return;
  }
  else if(!cl_sp->voice_info.reg_ussd_notification_events)
  {
    QM_MSG_MED("QMI VOICE, client not registed for orig_ussd_no_wait_ind");
    return;
  }

  ind = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_INDICATION,
                                    (uint16_t) QMI_VOICE_ORIG_USSD_NO_WAIT_IND_V02,
                                    (void *) &(data_ptr->data.ussd_orig_no_wait_ind),
                                    (uint32_t) sizeof(voice_orig_ussd_no_wait_ind_msg_v02),
                                    &ind )
    )
  {
   qmi_voicei_send_indication( cl_sp,QMI_VOICE_ORIG_USSD_NO_WAIT_IND_V02,ind );
  }
}/*qmi_voicei_sups_orig_ussd_no_wait_ind*/
#endif

#ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
/*===========================================================================
  FUNCTION qmi_voicei_call_modified_ind()
  
  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_call_modified_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_modification_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_MODIFIED_IND_V02,
                                        (void *) &(data_ptr->data.call_modified_ind),
                                        (uint32_t) sizeof(voice_modified_ind_msg_v02),
                                        &ind )
        )
      {
       qmi_voicei_send_indication( cl_sp,QMI_VOICE_MODIFIED_IND_V02,ind );
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
} /* qmi_voicei_call_modified_ind() */

/*===========================================================================
  FUNCTION qmi_voicei_call_modify_accept_ind()
  
  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_call_modify_accept_ind
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_voicei_client_state_type   *cl_sp;
  dsm_item_type                  *ind;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if( (cl_sp->voice_info.reg_modification_events) &&
        (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)) )
    {
      ind = NULL;
      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_VOICE_MODIFY_ACCEPT_IND_V02,
                                        (void *) &(data_ptr->data.call_modify_accept_ind),
                                        (uint32_t) sizeof(voice_modify_accept_ind_msg_v02),
                                        &ind )
        )
      {
        qmi_voicei_send_indication( cl_sp,QMI_VOICE_MODIFY_ACCEPT_IND_V02,ind );
      }
    }

    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
} /* qmi_voicei_call_modify_accept_ind() */
#endif

#if (defined FEATURE_ALS) && (defined FEATURE_GSM)
/*===========================================================================
  FUNCTION QMI_VOICEI_ALS_SET_LINE_SWITCHING()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_als_set_line_switching
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_ALS_SET_LINE_SWITCHING_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.set_line_switch),
                                         (uint32_t) sizeof(voice_als_set_line_switching_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_ALS_SET_LINE_SWITCHING;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_als_set_line_switching */

/*===========================================================================
  FUNCTION QMI_VOICEI_ALS_SET_LINE_SWITCHING_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_als_set_line_switching_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.set_line_switch_resp),
                                    (uint32_t) sizeof(voice_als_set_line_switching_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
} /* qmi_voicei_als_set_line_switching_resp */


/*===========================================================================
  FUNCTION QMI_VOICEI_ALS_SELECT_LINE()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_als_select_line
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_ALS_SELECT_LINE_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.select_line),
                                         (uint32_t) sizeof(voice_als_select_line_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_ALS_SELECT_LINE;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_als_select_line */

/*===========================================================================
  FUNCTION QMI_VOICEI_ALS_SELECT_LINE_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_als_select_line_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.select_line_resp),
                                    (uint32_t) sizeof(voice_als_select_line_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/* qmi_voicei_als_select_line_resp */
#endif

#ifdef FEATURE_MMODE_SS_SUPS
/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_COLR()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_get_colr
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_COLR_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_colr),
                                         (uint32_t) sizeof(voice_get_colr_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_GET_COLR;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_get_colr() */



/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_COLR_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_get_colr_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.get_colr_resp),
                                    (uint32_t) sizeof(voice_get_colr_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/*qmi_voicei_sups_get_colr_resp*/

/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CNAP()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_sups_get_cnap
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_CNAP_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_cnap),
                                         (uint32_t) sizeof(voice_get_cnap_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SUPS_GET_CNAP;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_get_cnap() */



/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_GET_CNAP_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_get_cnap_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.get_cnap_resp),
                                    (uint32_t) sizeof(voice_get_cnap_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/*qmi_voicei_sups_get_cnap_resp*/
#endif
#ifdef FEATURE_MMODE_CALL_SUPS
/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_MANAGE_IP_CALLS()

  DESCRIPTION
    Manage the call dependant sups for VoIP Calls.
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_sups_manage_ip_calls
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  uint8              type;
  uint16             len;
  uint16             expected_len;
  void *             value;
  boolean            got_v_in_required;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  value             = NULL;
  got_v_in_required = FALSE;

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  while (*sdu_in)
  {
    if( !qmi_svc_get_tl(sdu_in, &type, &len) )
    {
      continue;
    }

    // special value 0 = variable length or don't care (unrecognzied TLV)
    expected_len = 0;
    value = NULL;
    QM_MSG_MED_1("sups_manage_ip_calls(): TLV type (%d)", type);
    switch (type)
    {
      case QMI_TYPE_REQUIRED_PARAMETERS:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sups_type);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sups_type;
        got_v_in_required = TRUE;
        break;

      case VOICEI_MNG_IP_CALLS_CALL_ID:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.call_id);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.call_id;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.call_id_valid = TRUE;
        break;

      case VOICEI_MNG_IP_CALLS_CALL_TYPE:
        expected_len = sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.call_type);
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.call_type;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.call_type_valid = TRUE;
        break;

      case VOICEI_MNG_IP_CALLS_AUDIO_ATTRIB:
        if(len > sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.audio_attrib))
        {
          errval = QMI_ERR_ARG_TOO_LONG;
          goto send_result;
        }
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.audio_attrib;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.is_audio_attrib_valid= TRUE;
        break;

      #ifndef FEATURE_MMODE_DISABLE_MISC_FEATURES
      case VOICEI_MNG_IP_CALLS_VIDEO_ATTRIB:
        if(len > sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.video_attrib))
        {
          errval = QMI_ERR_ARG_TOO_LONG;
          goto send_result;
        }
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.video_attrib;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.is_video_attrib_valid= TRUE;
        break;         
      #endif
      
      case VOICEI_MNG_IP_CALLS_SIP_URI:        
        value = cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sip_uri;
        if(len > sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sip_uri))
        {
          QM_MSG_MED("Invalid SIP URI length");
          errval = QMI_ERR_ARG_TOO_LONG;
          
          goto send_result;
        }
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sip_uri_len =(uint8)len;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.is_sip_uri_valid = TRUE;
        break;
      case VOICEI_MNG_IP_CALLS_SIP_REJECT_CAUSE:                
        if(len > sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.reject_cause))
        {
          QM_MSG_MED("Invalid Reject cause length");
          errval = QMI_ERR_ARG_TOO_LONG;
          
          goto send_result;
        }        
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.reject_cause;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.reject_cause_valid = TRUE;
        break;
      case VOICEI_MNG_IP_CALLS_SIP_REJECT_CODE:                
        if(len > sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sip_reject_code))
        {
          QM_MSG_MED("Invalid SIP Reject Code length");
          errval = QMI_ERR_ARG_TOO_LONG;
          
          goto send_result;
        }        
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sip_reject_code;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.sip_reject_code_valid = TRUE;
        break;
      case VOICEI_MNG_IP_CALLS_SPEECH_CODEC:
        if(len > sizeof(cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.speech_codec))
        {
          QM_MSG_MED("Invalid Speech Codec length");
          errval = QMI_ERR_ARG_TOO_LONG;

          goto send_result;
        }
        value = &cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.speech_codec;
        cmd_ptr->cmd.cm_if_data_ptr->data.manage_calls_info.is_speech_codec_valid = TRUE;
        break;

      default:
        QM_MSG_ERROR_1("Unrecognized TLV type (%d)", type);
        break;
    }

    if ( (expected_len != 0) && (expected_len != len) )
    {
      QM_MSG_MED_2("Invalid TLV len (%d) for type (%d)", len, type);
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }

    if ( len != dsm_pullup( sdu_in, value, len ) )
    {                                                                    
      QM_MSG_MED("Invalid length in TLV");
      errval = QMI_ERR_MALFORMED_MSG;
      goto send_result;
    }
  }

  if ( !got_v_in_required )
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd = QMI_VOICE_CM_IF_CMD_SUPS_MANAGE_IP_CALLS;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;

  cmd_ptr->cmd.cm_if_data_ptr->header.as_id = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_sups_manage_ip_calls() */


/*===========================================================================
  FUNCTION QMI_VOICEI_SUPS_MANAGE_IP_CALLS_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_sups_manage_ip_calls_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  

  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.mng_ip_calls_resp),
                                    (uint32_t) sizeof(voice_manage_ip_calls_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc((void*)cmd_buf_p, response);
  }
} /* qmi_voicei_sups_manage_ip_calls_resp() */
#endif

#if (defined FEATURE_ALS) && (defined FEATURE_GSM)
/*===========================================================================
  FUNCTION QMI_VOICEI_GET_LINE_SWITCHING_STATUS()

  DESCRIPTION
  
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_als_get_line_switching_status
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_ALS_GET_LINE_SWITCHING_STATUS_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_line_switch_status),
                                         (uint32_t) sizeof(voice_als_get_line_switching_status_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_ALS_GET_LINE_SWITCHING_STATUS;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_get_line_switching_status() */


/*===========================================================================
  FUNCTION QMI_VOICEI_GET_LINE_SWITCHING_STATUS_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_als_get_line_switching_status_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  

  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.get_line_switch_status_resp),
                                    (uint32_t) sizeof(voice_als_get_line_switching_status_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc((void*)cmd_buf_p, response);
  }
} /* qmi_voicei_get_line_switching_status_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_GET_SELECTED_LINE()

  DESCRIPTION
  
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type* qmi_voicei_als_get_selected_line
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_ALS_GET_SELECTED_LINE_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_selected_line),
                                         (uint32_t) sizeof(voice_als_get_selected_line_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_ALS_GET_SELECTED_LINE;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_get_line_switching_status() */


/*===========================================================================
  FUNCTION QMI_VOICEI_GET_SELECTED_LINE_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_als_get_selected_line_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  

  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.get_selected_line_resp),
                                    (uint32_t) sizeof(voice_als_get_selected_line_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc((void*)cmd_buf_p, response);
  }
} /* qmi_voicei_get_selected_line() */
#endif

/*===========================================================================
  FUNCTION QMI_VOICEI_AOC_RESET_ACM()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_aoc_reset_acm
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_AOC_RESET_ACM_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.reset_acm),
                                         (uint32_t) sizeof(voice_aoc_reset_acm_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_AOC_RESET_ACM;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
}/* qmi_voicei_aoc_reset_acm() */

/*===========================================================================
  FUNCTION QMI_VOICEI_AOC_RESET_ACM_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_aoc_reset_acm_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  

  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.reset_acm_resp),
                                    (uint32_t) sizeof(voice_aoc_reset_acm_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc((void*)cmd_buf_p, response);
  }
}/* qmi_voicei_aoc_reset_acm_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_AOC_SET_ACMMAX()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_aoc_set_acmmax
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type    *cmd_ptr;
  qmi_voicei_client_state_type   *client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_AOC_SET_ACMMAX_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.set_acmmax),
                                         (uint32_t) sizeof(voice_aoc_set_acmmax_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_AOC_SET_ACMMAX;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
}/* qmi_voicei_aoc_set_acmmax() */

/*===========================================================================
  FUNCTION QMI_VOICEI_AOC_SET_ACMMAX_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_aoc_set_acmmax_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.set_acmmax_resp),
                                    (uint32_t) sizeof(voice_aoc_set_acmmax_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc((void*)cmd_buf_p, response);
  }
}/* qmi_voicei_aoc_set_acmmax_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_AOC_GET_CALL_MATER_INFO()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_aoc_get_call_meter_info
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr;
  qmi_voicei_client_state_type * client_sp;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();

  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_AOC_GET_CALL_METER_INFO_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_call_meter_info),
                                         (uint32_t) sizeof(voice_aoc_get_call_meter_info_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_AOC_GET_CALL_METER_INFO;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
}/* qmi_voicei_aoc_get_call_meter_info() */

/*===========================================================================
  FUNCTION QMI_VOICEI_AOC_GET_CALL_MATER_INFO_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_aoc_get_call_meter_info_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p;
  dsm_item_type *         response;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    return;
  }

  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t)cmd_buf_p->msg_id,
                                    (void *) &(data_ptr->data.get_call_meter_info_resp),
                                    (uint32_t) sizeof(voice_aoc_get_call_meter_info_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc( (void*)cmd_buf_p, response);
  }
}/* qmi_voicei_aoc_get_call_meter_info_resp() */


/*===========================================================================
  FUNCTION QMI_VOICEI_GET_SPEECH_CODEC_INFO()

  DESCRIPTION
    
    
  PARAMETERS
    sp        : service provided state pointer
    cmd_buf_p : ptr to cmd buffer
    cl_sp     : client state pointer
    sdu_in    : incoming request

  RETURN VALUE
    ptr to response or NULL if none.
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_voicei_get_speech_codec_info
(
  qmi_mmode_svc_info_type *sp,
  qmi_mmode_qcsi_transaction_type *cmd_buf_p,
  void *            cl_sp,
  dsm_item_type **  sdu_in
)
{
  qmi_error_e_type   errval;

  qmi_mmode_cmd_data_buf_type *  cmd_ptr = NULL;
  qmi_voicei_client_state_type * client_sp = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  cmd_ptr = qmi_mmode_get_cmd_data_buf();
  QM_MSG_HIGH("In qmi_voicei_get_speech_codec_info()");// To be removed later to reduce F3's
  errval = qmi_voicei_cmd_validate(cmd_ptr, cl_sp, &client_sp);
  if(errval) goto send_result;

  memset(cmd_ptr->cmd.cm_if_data_ptr, 0, sizeof(*(cmd_ptr->cmd.cm_if_data_ptr)));

  errval = qmi_mmode_idl_message_decode( voice_svc_obj,
                                         (uint16_t)QMI_VOICE_GET_SPEECH_CODEC_INFO_REQ_V02,
                                         sdu_in,
                                         (void *) &(cmd_ptr->cmd.cm_if_data_ptr->data.get_speech_codec_info),
                                         (uint32_t) sizeof(voice_get_speech_codec_info_req_msg_v02));
  if(errval) goto send_result;

  cmd_ptr->cmd.cm_if_data_ptr->header.cmd    = QMI_VOICE_CM_IF_CMD_SPEECH_CODEC_INFO;
  cmd_ptr->cmd.cm_if_data_ptr->header.handle = (uint32)cmd_buf_p;
  cmd_ptr->cmd.cm_if_data_ptr->header.as_id  = QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  if(client_sp->asubs_info.is_bind_set_by_client)
  {
    cmd_ptr->cmd.cm_if_data_ptr->header.as_id = client_sp->asubs_info.as_id;
  }
  else
  {
    QM_MSG_HIGH("SUB is not binded by the client");
    errval = QMI_ERR_INVALID_ID;
    goto send_result;
  }

  qmi_mmode_send_cmd(QMI_MMODE_CMD_VOICE_CM_IF_CB, cmd_ptr);

  return QMI_SVC_RESPONSE_PENDING;

  send_result:
  return qmi_voicei_send_result(errval, cmd_ptr);
} /* qmi_voicei_get_speech_codec_info() */

/*===========================================================================
  FUNCTION QMI_VOICEI_GET_SPEECH_CODEC_INFO_RESP()

  DESCRIPTION
    
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_get_speech_codec_info_resp
( 
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
  qmi_mmode_qcsi_transaction_type *      cmd_buf_p = NULL;
  dsm_item_type *         response;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  QM_MSG_HIGH("In qmi_voicei_get_speech_codec_info_resp()");// To be removed later to reduce F3's
  cmd_buf_p = (qmi_mmode_qcsi_transaction_type *) data_ptr->header.handle;

  if(cmd_buf_p == NULL)
  {
    QM_MSG_HIGH("cmd_buf_p is NULL");
    return;
  }
  /* Remove the entry from cmd list here */
  qmi_voice_cmd_list_free( data_ptr->header.handle );
  response = NULL;
  if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                    QMI_IDL_RESPONSE,
                                    (uint16_t) ( (qmi_cmd_buf_type*) cmd_buf_p )->cmd_type,
                                    (void *) &(data_ptr->data.get_speech_codec_info_resp),
                                    (uint32_t) sizeof(voice_get_speech_codec_info_resp_msg_v02),
                                    &response )
    )
  {
    qmi_voicei_send_response_enc((void*)cmd_buf_p, response);
  }
}



/*===========================================================================
  FUNCTION qmi_voicei_auto_reject_incoming_call_end_ind()

  DESCRIPTION 
    
  PARAMETERS

  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_voicei_auto_reject_incoming_call_end_ind
(
  const qmi_voice_cm_if_info_item_s *const data_ptr
)
{
//  qmi_voicei_state_type *        voice_sp = NULL;
  qmi_voicei_client_state_type * cl_sp = NULL;
//  int                            j;
  dsm_item_type *                ind = NULL;
  auto_rejected_incoming_call_end_ind_msg_v02 *mt_call_end_auto_reject = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mt_call_end_auto_reject = (auto_rejected_incoming_call_end_ind_msg_v02*)&data_ptr->data;


  QM_MSG_HIGH_1("Incoming call end auto reject IND. call_end reason : %d",mt_call_end_auto_reject->call_end_reason);

//  voice_sp = &qmi_voice_state;
  cl_sp = (qmi_voicei_client_state_type *)q_check(&(qmi_voice_state.client_q));
  while ( cl_sp != NULL )
  {
    if((cl_sp != NULL)&&
       (cl_sp->voice_info.reg_mt_call_end_auto_reject_events) &&
       (!cl_sp->asubs_info.is_bind_set_by_client ||
        (cl_sp->asubs_info.as_id == data_ptr->header.as_id)))
    {  
      ind = NULL;

      if( qmi_mmode_idl_message_encode( voice_svc_obj,
                                        QMI_IDL_INDICATION,
                                        (uint16_t) QMI_AUTO_REJECTED_INCOMING_CALL_END_IND_V02,
                                        (void *) &(data_ptr->data.auto_rejected_incoming_call_end_ind),
                                        (uint32_t) sizeof(auto_rejected_incoming_call_end_ind_msg_v02),
                                        &ind )
        )
      {
       QM_MSG_HIGH("Send Incoming call auto rejected call end indication  to clients");
       qmi_voicei_send_indication( cl_sp,QMI_AUTO_REJECTED_INCOMING_CALL_END_IND_V02,ind );
      }
    }
    cl_sp = q_next(&(qmi_voice_state.client_q), &(cl_sp->link));
  }
  return;

} /* qmi_voicei_ecall_status_ind() */


#endif/* !FEATURE_NO_QMI_VOICE*/

#endif // FEATURE_QMI_SERVICE_SPLIT


