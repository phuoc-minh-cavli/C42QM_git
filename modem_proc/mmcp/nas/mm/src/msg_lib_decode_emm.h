
/*==============================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*Author: hnam
  Reference Document: 3GPP TS 24.301 V1.1.1 (2008-10)
  START DATE: 2/5/2009*/
/*==============================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/msg_lib_decode_emm.h#1 $
$Author: pwbldsvc $
$DateTime: 2023/04/25 14:39:18 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/21/10   zren    Added EMM/ESM STATUS feature
06/16/09   hnam    Added "lte_nas_dl_security_info_type" in decipher_nas_msg
05/17/09   hnam    Added the code to support security
04/10/09   hnam    Deleted authentication reject function extern definition.
04/10/09   hnam    Added function definitions for existing emm messages
==============================================================================*/

#ifndef _MSG_LIB_DECODE_EMM_H_
#define _MSG_LIB_DECODE_EMM_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "lte_nas.h"
#include "lte_nas_common_v.h"
#include "comdef.h"
#include "lte_nas_security_info.h"
#include "mm_v.h"

/*===========================================================================
                             MACRO DECLARATIONS
=============================================================================*/

/* EMM IEIs */
#define LOC_AREA_ID_IEI               0x13
#define T3402_VAL_ATTACH_REJECT_IEI   0x16
#define T3402_VAL_IEI                 0x17
#define MS_IDENTITY_IEI               0x23
#define EMERGENCY_NUM_LIST_IEI        0x34
#define NW_FULL_NAME_IEI              0x43
#define NW_PARTIAL_NAME_IEI           0x45
#define LOCAL_TIME_ZONE_IEI           0x46
#define UNIV_LOCAL_TIME_ZONE_IEI      0x47
#define NW_DAYLIGHT_SAVINGS_TIME_IEI  0x49
#define EQUIVALENT_PLMN_IEI           0x4A
#ifdef FEATURE_LTE_REL14
#define HASH_MME_IEI                  0x4F
#endif
#define GUTI_IEI                      0x50
#define EMM_CAUSE_IEI                 0x53
#define TAI_LIST_IEI                  0x54
#define NONCE_UE_IEI                  0x55
#define NONCE_MME_IEI                 0x56
#define EPS_BEARER_CONTEXT_STATUS_IEI 0x57
#define T3423_VAL_IEI                 0x59
#define T3412_TIMER_IEI               0x5A
#define T3442_VAL_IEI                 0x5B
#define HC_CONFIG_STATUS_IEI          0x68
#ifdef FEATURE_LTE_REL10
#define T3412_EXT_VAL_IEI             0x5E
#define T3346_VAL_IEI                 0x5F
#endif

/* 24.301 5.5.1.2.5   */
#ifdef FEATURE_LTE_REL12
#define EXT_EMM_CAUSE_IEI             0xA0
#endif

#define CALLING_LINE_INFO_IEI         0x60
#define SUPPLIMENTARY_SERV_CODE_IEI   0x61
#define LCS_IND_IEI                   0x62
#define EPS_NW_FEATURE_SUPPORT_IEI    0x64
#define ESM_IEI                       0x78
#define NAS_KSI_IEI                   0x80
#define ADDITIONAL_UPDATE_RESULT_IEI  0xF0
#define IMEISV_IEI                    0xC0
#define T3324_VAL_IEI                 0x6A
#define EDRX_PARAMS_IEI               0x6E
#define SMS_SERVICE_STATUS_IEI        0xE0
#define DCN_ID_IEI                    0x65
#define T3448_VAL_IEI                 0x6B

/* EMM DL OTA messages Length */
#define NAS_EMM_ATTACH_REJECT_MIN_LEN        3*8
#define NAS_EMM_ATTACH_ACCEPT_MIN_LEN        13*8
#define NAS_EMM_MT_DETACH_REQ_MIN_LEN        3*8
#define NAS_EMM_SECURITY_MODE_CMD_MIN_LEN    7*8
#define NAS_EMM_SERV_REJ_MIN_LEN             3*8
#define NAS_EMM_SERV_ACCEPT_MIN_LEN          2*8
#define NAS_EMM_TAU_ACCEPT_MIN_LEN           3*8
#define NAS_MSG_CONTAINER_MIN_LEN            3*8
#define NAS_MSG_CONTAINER_MAX_LEN            252*8
#define MIN_ESM_CONTAINER_LEN                3*8
#define NAS_EMM_TAU_REJECT_MIN_LEN           3*8
#define AUTH_REQ_LEN                         34*8
#define NAS_EMM_IDENTITY_REQUEST_LEN         3*8
#define NAS_EMM_GUTI_REALLOC_CMD_LEN         14*8
#define NAS_EMM_STATUS_LEN                   3*8
#define NAS_EMM_INFO_LEN                     2*8
#define NAS_EMM_CS_SERV_NOTIFICATION_LEN     3*8

#ifdef FEATURE_LTE_REL9
#define NAS_EMM_DL_GENERIC_NAS_TRANSPORT_LEN 6*8
#endif

/*===========================================================================
                      DATA DECLARATIONS
===========================================================================*/
extern lte_nas_sec_info_params_type nas_sec_params;

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

#if defined (FEATURE_LTE_REL9) 
#ifdef FEATURE_NAS_GPS_ENABLE
/*===========================================================================
FUNCTION      DECODE_DL_GENERIC_NAS_TRANSPORT

DESCRIPTION
  This function decodes Downlink Generic NAS Transport message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_dl_generic_nas_transport
(
  byte                                 *src,
  word                                 *pos,
  word                                 *buffer_length,
  lte_nas_emm_dl_generic_nas_transport *dl_generic_nas_transport
);
#endif
#endif


/*===========================================================================
FUNCTION      DECODE_EMM_INFO

DESCRIPTION
  This function decodes EMM INFORMATION message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_emm_info
(
  byte             *src,
  word             *pos, 
  word             *buffer_length, 
  lte_nas_emm_info *emm_info
);


/*===========================================================================
FUNCTION      DECODE_CS_SERV_NOTIFICATION

DESCRIPTION
  This function decodes CS Servicce Notification message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
#if defined (FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE)
extern boolean decode_cs_serv_notification
(
  byte                             *src,
  word                             *pos,
  word                             *buffer_length,
  lte_nas_emm_cs_serv_notification *cs_serv_notification
);
#endif

/*===========================================================================
FUNCTION  DECODE_GUTI_REALLOC_CMD

DESCRIPTION
  This function decodes GUTI REALLOCATION COMMAND message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_guti_realloc_cmd
(
  byte                         *src,
  word                         *pos,
  word                         *buffer_length,
  lte_nas_emm_guti_realloc_cmd *guti_realloc_cmd
);


/*===========================================================================
FUNCTION      DECODE_IDENTITY_REQ

DESCRIPTION
  This function decodes IDENTITY REQUEST message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_identity_req
(
  byte                      *src,
  word                      *pos,
  word                      *buffer_length,
  lte_nas_emm_identity_req *identity_req
);


/*===========================================================================
FUNCTION      DECODE_TAU_REJ

DESCRIPTION
  This function decodes TRACKING AREA UPDATE REJECT message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_tau_reject
(
  byte                *src,
  word                *pos,
  word                *buffer_length,
  lte_nas_emm_tau_rej *tau_rej
);


/*===========================================================================
FUNCTION      DECODE_TAU_ACCEPT

DESCRIPTION
  This function decodes TRACKING AREA UPDATE ACCEPT message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_tau_accept
(
  byte                        *src,
  word                        *pos,
  word                        *buffer_length,
  lte_nas_emm_tau_accept_type *tau_accept
);


/*===========================================================================
FUNCTION      DECODE_DETACH_REQUEST

DESCRIPTION
  This function decodes DETACH REQUEST message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_detach_request
(
  byte                           *src,
  word                           *pos,
  word                           *buffer_length,
  lte_nas_emm_mt_detach_req_type *mt_detach
);


/*===========================================================================
FUNCTION      DECODE_ATTACH_REJECT

DESCRIPTION
  This function decodes ATTACH REJECT message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_attach_reject
(
  byte                        *src,
  word                        *pos,
  word                        *buffer_length,
  lte_nas_emm_attach_rej_type *attach_reject,
  lte_nas_incoming_msg_type   *nas_incoming_msg_ptr
);


/*===========================================================================
FUNCTION      DECODE_ATTACH_ACCEPT

DESCRIPTION
  This function decodes ATTACH ACCEPT message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_attach_accept
(
  byte                           *src,
  word                           *pos,
  word                           *buffer_length,
  lte_nas_emm_attach_accept_type *attach_accept,
  struct emm_ctrl_data_tag       *emm_ctrl_data_ptr,
  lte_nas_incoming_msg_type      *nas_incoming_msg_ptr
);


/*===========================================================================
FUNCTION      DECODE_SECURITY_MODE_CMD

DESCRIPTION
  This function decodes SECURITY MODE COMMAND message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_security_mode_cmd
(
  byte                              *src,
  word                              *pos,
  word                              *buffer_length,
  lte_nas_emm_security_mode_command *security_mode_cmd
);


/*===========================================================================
FUNCTION      DECODE_SERVICE_REJ

DESCRIPTION
  This function decodes SERVICE REJECT message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_service_rej
(
  byte                         *src,
  word                         *pos,
  word                         *buffer_length,
  lte_nas_emm_service_rej_type *serv_rej
);


/*===========================================================================
FUNCTION      DECODE_AUTHENTICATION_REQ

DESCRIPTION
  This function decodes AUTHENTICATION REQUEST message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_authentication_req
(
  byte                 *src,
  word                 *pos,
  word                 *buffer_length,
  lte_nas_emm_auth_req *auth_req
);


/*===========================================================================
FUNCTION      DECODE_EMM_STATUS

DESCRIPTION
  This function decodes EMM STATUS message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_emm_status
(
  byte               *src,
  word               *pos,
  word               *buffer_length,
  lte_nas_emm_status *emm_status
);


/*===========================================================================
FUNCTION      DECODE_DOWNLINK_NAS_TRANSPORT

DESCRIPTION
  This function decodes Downlink NAS Transport message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_dl_nas_transport
(
  byte                         *src_ptr,
  word                         *pos_ptr,
  word                         *buffer_length_ptr,
  lte_nas_emm_dl_nas_transport *dl_nas_transport_ptr
);


/*===========================================================================
FUNCTION      DECIPHER_NAS_MESSAGE

DESCRIPTION
  This function deciphers Downlink NAS Transport message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decipher_nas_msg
(
  byte                          *src,
  word                          *buffer_length,
  lte_nas_dl_security_info_type *nas_dl_sec_info
);


#ifdef FEATURE_CIOT
/*===========================================================================
FUNCTION      DECODE_SERVICE_ACCEPT

DESCRIPTION
  This function decodes SERVICE ACCEPT message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
boolean decode_service_accept
(
  byte                             *src,
  word                             *pos,
  word                             *buffer_length,
  lte_nas_emm_service_accept_type  *serv_accept
);
#endif
#endif /* FEATURE_LTE */
#endif /* _MSG_LIB_DECODE_EMM_H_ */
