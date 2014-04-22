
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

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/msg_lib_decode_esm.h#1 $
$Author: pwbldsvc $
$DateTime: 2023/04/25 14:39:18 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
==============================================================================*/

#ifndef _MSG_LIB_DECODE_ESM_H_
#define _MSG_LIB_DECODE_ESM_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "lte_nas.h"
#include "lte_nas_common_v.h"
#include "esm_cm_msg_if.h"
#include "comdef.h"

/*===========================================================================
                             MACRO DECLARATIONS
=============================================================================*/

/* ESM IEIs */
#define PROT_CONFIG_OPT_IEI        0x27
#define EXT_PROT_CONFIG_OPT_IEI    0x7B
#define NEGOTIATED_QOS_IEI         0x30
#define NEGOTIATED_LLC_SAPI_IEI    0x32
#define PACKET_FLOW_IDENTIFIER_IEI 0x34
#define TFT_IEI                    0x36

#ifdef FEATURE_LTE_REL10
#define T3396_VALUE_IEI            0x37
#endif

#define ESM_CAUSE_IEI              0x58
#define PDN_ADDRESS_IEI            0x59
#define EPS_QOS_IEI                0x5B
#define TRANSACTION_IDENTIFIER_IEI 0x5D
#define APN_AMBR_IEI               0x5E
#ifdef FEATURE_LTE_REL12
#define REATTEMPT_INDICATOR_IEI    0x6B
#endif
#define RADIO_PRIORITY_IEI         0x80
#ifdef FEATURE_CIOT
#define CP_ONLY_IND_IEI            0x90
#define HC_CONFIG_IEI              0x66
#define SRV_PLMN_RATE_CTRL_IEI     0x6E
#endif


/* ESM DL OTA messages Length */
#define NAS_ESM_ACT_DED_REQ_MIN_LEN           8*8
#define NAS_ESM_ACT_DEF_REQ_MIN_LEN           7*8
#define NAS_ESM_PDN_CON_REJ_MIN_LEN           4*8
#define NAS_ESM_DEACT_BEARER_REQ_MIN_LEN      4*8
#define NAS_ESM_BEARER_RES_ALLOC_REJ_MIN_LEN  4*8
#define NAS_ESM_BEARER_RES_MODIFI_REJ_MIN_LEN 4*8
#define NAS_ESM_PDN_DISCONNECT_REJ            4*8
#define NAS_ESM_ESM_STATUS                    4*8
#define NAS_ESM_ESM_DATA_TRANSPORT_MIN_LEN    5*8

#ifdef FEATURE_LTE_REL9 
#define NAS_ESM_NOTIFICATION                  5*8
#endif

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

#ifdef FEATURE_LTE_REL9
/*===========================================================================
FUNCTION      DECODE_ESM_NOTIFICATION

DESCRIPTION
  This function decodes ESM NOTIFICATION message.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_esm_notification
(
  byte                     *src,
  word                     *pos,
  word                     *buffer_length,
  lte_nas_esm_notification *esm_notification,
  word                     *esm_len_in_bits
);
#endif


/*===========================================================================
FUNCTION  DECODE_PDN_CONN_REQUEST

DESCRIPTION
  This function decodes PDN CONNECTIVITY REQUEST message.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None
==========================================================================*/
extern void decode_pdn_conn_req
(
  byte                       *src,
  word                       buffer_length,
  emm_pdn_connectivity_req_s_type *pdn_conn_req
);


/*===========================================================================
FUNCTION      DECODE_PDN_DISCONNECT_REJ

DESCRIPTION
  This function decodes PDN DISCONNECT REJECT message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_pdn_disconnect_rej
(
  byte                           *src,
  word                           *pos,
  word                           *buffer_length,
  lte_nas_esm_pdn_disconnect_rej *pdn_disconnect_rej,
  word                           *esm_len_in_bits
);


/*===========================================================================
FUNCTION      DECODE_BEARER_MODIFY_CONTEXT_REJ

DESCRIPTION
  This function decodes BEARER MODIFICATION CONTEXT REJECT message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_bearer_modify_context_rej
(
  byte                                   *src,
  word                                   *pos,
  word                                   *buffer_length,
  lte_nas_esm_bearer_resource_modifi_rej *bearer_resource_modifi_rej,
  word                                   *esm_len_in_bits
);


/*===========================================================================
FUNCTION      DECODE_MODIFY_EPS_CONTEXT_REQ

DESCRIPTION
  This function decodes MODIFY EPS BEARER CONTEXT REQUEST message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_modify_eps_context_req
(
  byte                                   *src,
  word                                   *pos,
  word                                   *buffer_length,
  lte_nas_esm_modify_eps_bearer_ctxt_req *modify_bearer_ctxt_req,
  word                                   *esm_len_in_bits
);


/*===========================================================================
FUNCTION      DECODE_BEARER_RESOURCE_ALLOC_REJ

DESCRIPTION
  This function decodes BEARER RESOURCE ALLOCATION REJECT message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_bearer_resource_alloc_rej
(
  byte                                       *src,
  word                                       *pos,
  word                                       *buffer_length,
  lte_nas_esm_bearer_resource_allocation_rej *bearer_resource_alloc_rej,
  word                                       *esm_len_in_bits
);


/*===========================================================================
FUNCTION      DECODE_DEACTIVATE_EPS_BEARER_REQ

DESCRIPTION
  This function decodes DEACTIVATE EPS BEARER CONTEXT REQUEST message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_deactivate_eps_bearer_req
(
   byte                                     *src,
   word                                     *pos,
   word                                     *buffer_length,
   lte_nas_esm_deact_eps_bearer_context_req *deactivate_bearer_context_req,
   word                                     *esm_len_in_bits
 );


/*===========================================================================
FUNCTION      DECODE_DEDICATED_EPS_BEARER_REQ

DESCRIPTION
  This function decodes DEDICATED EPS BEARER CONTEXT REQUEST message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_dedicated_eps_bearer_req
(
  byte                                         *src,
  word                                         *pos,
  word                                         *buffer_length,
  lte_nas_esm_act_dedicated_bearer_context_req *dedicated_bearer_context_req,
  word                                         *esm_len_in_bits
);


/*===========================================================================
FUNCTION      DECODE_ACT_DEF_CONTEXT_REQ

DESCRIPTION
  This function decodes ACTIVATE DEFAULT EPS BEARER CONTEXT REQUEST message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_act_def_context_req
(
  byte                                       *src,
  word                                       *pos,
  word                                       *buffer_length,
  lte_nas_esm_act_default_bearer_context_req *def_bearer_context_req,
  word                                       *esm_len_in_bits
);


/*===========================================================================
FUNCTION      DECODE_PDN_CONN_REJ

DESCRIPTION
  This function decodes PDN CONNECTIVITY REJECT message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_pdn_conn_rej
(
  byte                         *src,
  word                         *pos,
  word                         *buffer_length,
  lte_nas_pdn_connectivity_rej *pdn_conn_rej,
  word                         *esm_len_in_bits
);


/*===========================================================================
FUNCTION      DECODE_ESM_STATUS

DESCRIPTION
  This function decodes ESM STATUS message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_esm_status
(
  byte               *src,
  word               *pos,
  word               *buffer_length,
  lte_nas_esm_status *esm_status,
  word               *esm_len_in_bits
);


/*===========================================================================
FUNCTION      DECODE_ESM_MSG

DESCRIPTION
  This function decodes ESM messages.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_esm_msg
(
  byte                      *src,
  word                      *pos,
  word                      *buffer_length,
  word                      esm_len,
  lte_nas_incoming_msg_type *nas_incoming_msg_ptr
);

#endif /* FEATURE_LTE */
#endif /* _MSG_LIB_DECODE_ESM_H_ */
