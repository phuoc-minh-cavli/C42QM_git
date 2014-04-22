
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

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/msg_lib_decode_common_esm_ie.h#1 $
$Author: pwbldsvc $
$DateTime: 2023/04/25 14:39:18 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/17/12   cogolt  Rel-10, Added declaration for decode_t3396_value
==============================================================================*/

#ifndef _MSG_LIB_DECODE_COMMON_ESM_IE_H_
#define _MSG_LIB_DECODE_COMMON_ESM_IE_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "lte_nas.h"
#include "lte_nas_common_v.h"
#include "comdef.h"
#include "lte_nas_emm_message_type.h"
#include "lte_nas_esm_message_type.h"

/*==============================================================================
                      MACRO DECLARATIONS
==============================================================================*/
/* MIN/MAX Length value for ESM IEs after Length Indicator */

/* EPS QoS IE */
#define MIN_EPS_QOS_LENGTH               1

/* QoS IE */
#define MIN_QOS_LENGTH                   12

/* TFT IE */
#define MIN_TFT_LENGTH                   1
#define MAX_TFT_LENGTH                   255

#define RESERVED_TFT_OPN_CODE            7
#define CREATE_NEW_TFT                   1
#define DELETE_PKT_FLTR_IN_EXISTING_TFT  5
#define ADD_PKT_FLTR_IN_EXISTING_TFT     3
#define REPLACE_PKT_FLTR_IN_EXISTING_TFT 4

/* Transaction ID IE */
#define MIN_TRANSACTION_ID_LENGTH        1
#define MAX_TRANSACTION_ID_LENGTH        2

/* Packet flow identifier IE */
#define MIN_PKT_FLOW_ID_LENGTH           1
#define MAX_PKT_FLOW_ID_LENGTH           1

/* Protocol configuration options IE */
#define MIN_PROT_CONFIG_OPT_LENGTH       1
#define MAX_PROT_CONFIG_OPT_LENGTH       251

/* Header compression configuration IE */
#define MIN_HC_CONFIG_LENGTH             3
#define MAX_HC_CONFIG_LENGTH             255

#ifdef FEATURE_LTE_REL10
/* T3396 value IE */
#define MIN_T3396_VALUE_LENGTH           1
#define MAX_T3396_VALUE_LENGTH           1
#endif

#ifdef FEATURE_LTE_REL12
/* Reattempt Indicator IE */
#define MIN_RATC_VALUE_LENGTH           1
#define MAX_RATC_VALUE_LENGTH           1
#endif

/* PDN address IE */
#define MIN_PDN_ADDR_LENGTH              5
#define MAX_PDN_ADDR_LENGTH              13

/* Access point name IE */
#define MIN_APN_LENGTH                   1
#define MAX_APN_LENGTH                   100

/* APN-AMBR IE */
#define MIN_APN_AMBR_LENGTH              2
#define MAX_APN_AMBR_LENGTH              6

#ifdef FEATURE_CIOT
/* Serving PLMN rate control IE */
#define MIN_SPLMN_RATE_CTRL_LENGTH       2
#define MAX_SPLMN_RATE_CTRL_LENGTH       2
#endif

/* Other MACROS */
#define LINKED_EPS_BEARER_RESERVED_VALUE 4

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DECODE_TFT

DESCRIPTION
  This function decodes TFT Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_tft
(
  byte       *src,
  word       *pos,
  word       *buffer_length,
  tft_type_T *tft,
  word       *esm_msg_len
);


/*===========================================================================
FUNCTION      DECODE_EPS_QoS

DESCRIPTION
  This function decodes EPS QoS Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_eps_qos
(
  byte      *src,
  word      *pos,
  word      *buffer_length,
  sdf_qos_T *eps_qos,
  word      *esm_msg_len
);


/*===========================================================================
FUNCTION      DECODE_PACKET_FLOW_IDENTIFIER

DESCRIPTION
  This function decodes Packet Flow Identifier Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_packet_flow_identifier
(
  byte  *src,
  word  *pos,
  word  *buffer_length,
  pfi_T *pkt_flow_id,
  word  *esm_msg_len
);


/*===========================================================================
FUNCTION      DECODE_APN

DESCRIPTION
  This function decodes APN Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_apn
(
  byte  *src,
  word  *pos,
  word  *buffer_length,
  apn_T *apn,
  word  *esm_msg_len
);


/*===========================================================================
FUNCTION      DECODE_TRANSACTION_ID

DESCRIPTION
  This function decodes TRANSACTION ID Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_transaction_id
(
  byte                      *src,
  word                      *pos,
  word                      *buffer_length,
  gsm_umts_transaction_id_T *ti,
  word                      *esm_msg_len
);


/*===========================================================================
FUNCTION      DECODE_PROT_CONF_OPT

DESCRIPTION
  This function decodes Protocol Configuration Options Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_prot_conf_opt
(
  byte                   *src,
  word                   *pos,
  word                   *buffer_length,
  protocol_cfg_options_T *prot_conf_opt,
  word                   *esm_msg_len
);

/*===========================================================================
FUNCTION      decode_ext_prot_conf_opt

DESCRIPTION
  This function decodes extended protocol configuration options element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_ext_prot_conf_opt
(
  byte                   *src,
  word                   *pos,
  word                   *buffer_length,
  ext_protocol_cfg_options_T *ext_prot_conf_opt,
  word                   *esm_msg_len
);

/*===========================================================================
FUNCTION      DECODE_APN_AMBR

DESCRIPTION
  This function decodes APN-AMBR Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/        
extern boolean decode_apn_ambr
(
  byte       *src,
  word       *pos,
  word       *buffer_length,
  apn_ambr_T *apn_ambr,
  word       *esm_msg_len
);


/*===========================================================================
FUNCTION      DECODE_QoS

DESCRIPTION
  This function decodes QoS Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_qos
(
  byte  *src,
  word  *pos,
  word  *buffer_length,
  qos_T *neg_qos,
  word  *esm_msg_len
);


/*===========================================================================
FUNCTION      DECODE_PDN_ADDRESS

DESCRIPTION
  This function decodes PDN ADDRESS Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_pdn_address
(
  byte          *src,
  word          *pos,
  word          *buffer_length,
  pdn_address_T *pdn_address,
  word          *esm_msg_len
);


#ifdef FEATURE_LTE_REL10
/*===========================================================================
FUNCTION      DECODE_T3396_VALUE

DESCRIPTION
  This function decodes T3396 value Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_t3396_value
(
  byte             *src,
  word             *pos,
  word             *buffer_length,
  timer_3gpp_type3 *backoff_timer,
  word             *esm_msg_len
);
#endif

#ifdef FEATURE_LTE_REL12
/*===========================================================================
FUNCTION      DECODE_REATTEMPT_IND_VALUE

DESCRIPTION
  This function decodes Reattempt Indicator value Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_reattempt_ind_value
(
  byte             *src,
  word             *pos,
  word             *buffer_length,
  reattempt_ind_T  *reattempt_ind,
  word             *esm_msg_len
);
#endif

#ifdef FEATURE_CIOT
/*===========================================================================
FUNCTION      decode_hc_config_ie

DESCRIPTION
  This function decodes Header compression configuration Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_hc_config_ie
(
  byte                   *src,
  word                   *pos,
  word                   *buffer_length,
  hc_config_T            *hc_config,
  word                   *esm_msg_len
);
#endif

#ifdef FEATURE_CIOT
/*===========================================================================
FUNCTION      DECODE_SERVING_PLMN_RATE_CONTROL

DESCRIPTION
  This function decodes Serving PLMN Rate Control Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_serving_plmn_rate_control
(
  byte              *src,
  word              *pos,
  word              *buffer_length,
  splmn_rate_ctrl_T *splmn_rate_ctrl,
  word              *esm_msg_len
);
#endif

#endif /* FEATURE_LTE */
#endif /* _MSG_LIB_DECODE_COMMON_ESM_IE_H_ */