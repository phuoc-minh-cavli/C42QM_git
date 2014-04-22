

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*==============================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/inc/msg_lib_decode_common_emm_ie.h#1 $
$Author: pwbldsvc $
$DateTime: 2023/04/25 14:39:18 $

when         who                          what                              
--------     --------  ------------------------------------------------
05/14/10   RI      Mainlined spec changes upto Dec 09.
02/03/09     hnam      Added extern def for decode_guti_ie
==============================================================================*/
#ifndef _MSG_LIB_DECODE_COMMON_EMM_IE_H_
#define _MSG_LIB_DECODE_COMMON_EMM_IE_H_

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "lte_nas.h"
#include "lte_nas_common_v.h"
#include "comdef.h"
#include "lte_nas_emm_message_type.h"

/*==============================================================================
                      MACRO DECLARATIONS
==============================================================================*/
/* MIN/MAX Length value for EMM IEs after Length Indicator */

/* Emergency Number List */
#define MIN_EMERGENCY_NUM_LIST_LENGTH        3

/* Network Name */
#define MIN_NETWORK_NAME_LENGTH              1

/* Daylight Saving Time */
#define MIN_DAYLIGHT_SAVING_LENGTH           1
#define MAX_DAYLIGHT_SAVING_LENGTH           1

/* EPS Bearer Context Status */
#define MIN_EPS_BEARER_CONTEXT_STATUS_LENGTH 2
#define MAX_EPS_BEARER_CONTEXT_STATUS_LENGTH 2

/* Header compression configuration status  */
#define MIN_HC_CONFIG_STATUS_LENGTH          2
#define MAX_HC_CONFIG_STATUS_LENGTH          2
#define MIN_DCN_ID_LENGTH                    2
#define MAX_DCN_ID_LENGTH                    2

/* Tracking Area Identity List */
#define MIN_TAI_LIST_LENGTH                  6
#define MAX_TAI_LIST_LENGTH                  96

/* MS Identity */
#define MIN_MS_IDENTITY_LENGTH               5
#define MAX_MS_IDENTITY_LENGTH               8

/* GUTI */
#define MIN_GUTI_LENGTH                      11
#define MAX_GUTI_LENGTH                      11

/* Generic Message Container */
#define MIN_GENERIC_MSG_CONTAINER_LENGTH     1

/* Additional Information */
#define MIN_ADDITIONAL_INFO_LENGTH           1

/* Equivalent PLMNs */
#define MIN_EPLMN_LIST_LENGTH                3
#define MAX_EPLMN_LIST_LENGTH                45

/* EPS Network Feature Support */
#define MIN_EPS_NW_FEAT_SPT_LENGTH           1
/* EPS Network Feature Support */
#ifdef FEATURE_CIOT
#define MAX_EPS_NW_FEAT_SPT_LENGTH           2
#else
#define MAX_EPS_NW_FEAT_SPT_LENGTH           1
#endif

/* GPRS Timer 2 */
#define MIN_GPRS_TIMER2_LENGTH               1
#define MAX_GPRS_TIMER2_LENGTH               1

/* GPRS Timer 3 */
#define MIN_GPRS_TIMER3_LENGTH               1
#define MAX_GPRS_TIMER3_LENGTH               1

/* Replayed UE Security Capabilities */
#define MIN_REP_SEC_CAP_LENGTH               2

/* CLI */
#define MIN_CLI_LENGTH                       1
#define MAX_CLI_LENGTH                       12


/* Extended DRX Parameters */
#define MIN_EDRX_PARAMS_LENGTH               1
#define MAX_EDRX_PARAMS_LENGTH               1

/* Other MACROS */
#define MIN_T3346_VALUE_LENGTH               1
#define T3442_LEN                            8
#define LOCATION_AREA_IDENTITY_LEN           5
#define MAX_BCD_VAL                          9
#define MAX_TAC_VAL                          15
#ifdef FEATURE_LTE_REL14
#define HASH_MME_LENGTH                      8
#endif

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION      IS_PLMN_BCD_CODED

DESCRIPTION
  This function checks BCD coding of a PLMN.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: PLMN BCD coded
  FALSE: PLMN not BCD coded

SIDE EFFECTS
  None
===========================================================================*/
extern boolean is_plmn_bcd_coded
(
  sys_plmn_id_s_type plmn
);


/*===========================================================================
FUNCTION      DECODE_ADDITIONAL_INFO_IE

DESCRIPTION
  This function decodes Additional information Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_additional_info_ie
(
  byte                                 *src,
  word                                 *pos,
  word                                 *buffer_length,
  lte_nas_generic_additional_info_type *add_info
);

#if !defined(FEATURE_NAS_IMS_VOICE_DISABLED) || !defined(FEATURE_NAS_DISABLE_VOICE)
/*===========================================================================
FUNCTION  DECODE_EMERGENCY_NUM_LIST

DESCRIPTION
  This function decodes Emergency Number List Information Element

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_emergency_num_list
(
  byte                          *src,
  word                          *pos,
  word                          *buffer_length,
  byte                          ie_length,
  mmr_emergency_num_list_s_type *emergency_num_list
);
#endif 

/*===========================================================================
FUNCTION      DECODE_GPRS_TIMER2_IE

DESCRIPTION
  This function decodes GPRS timer 2 Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_gprs_timer2_ie
(
  byte  *src,
  word  *pos,
  word  *buffer_length,
  dword *timer_value
);


/*===========================================================================
FUNCTION      DECODE_GPRS_TIMER3_IE

DESCRIPTION
  This function decodes GPRS timer 3 Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_gprs_timer3_ie
(
  byte  *src,
  word  *pos,
  word  *buffer_length,
  uint64 *timer_value
);


/*===========================================================================
FUNCTION      DECODE_EPS_NW_FEAT_SPT_IE

DESCRIPTION
  This function decodes EPS network feature support Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_eps_nw_feat_spt_ie
(
  byte                          *src,
  word                          *pos,
  word                          *buffer_length,
  sys_nw_feature_support_s_type *eps_nw_ft_spt
);


/*===========================================================================
FUNCTION      DECODE_EPLMN_LIST_IE

DESCRIPTION
  This function decodes Equivalent PLMNs Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_eplmn_list_ie
(
  byte                      *src,
  word                      *pos,
  word                      *buffer_length,
  lte_nas_emm_plmn_lst_type *eplmn_list
);


/*===========================================================================
FUNCTION      DECODE_NW_NAME

DESCRIPTION
  This function decodes Network name Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_nw_name
(
  byte                 *src,
  word                 *pos,
  word                 *buffer_length,
  lte_nas_nw_name_type *nw_name
);


/*===========================================================================
FUNCTION      DECODE_DAYLIGHT_SAVING_TIME_IE

DESCRIPTION
  This function decodes Daylight saving time Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_daylight_saving_time_ie
(
  byte                           *src,
  word                           *pos,
  word                           *buffer_length,
  lte_nas_day_light_savings_type *daylight_saving_time
);


/*===========================================================================
FUNCTION      DECODE_EPS_BEARER_CONTEXT_STATUS

DESCRIPTION
  This function decodes EPS bearer context status Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_eps_bearer_context_status
(
  byte                                   *src,
  word                                   *pos,
  word                                   *buffer_length,
  lte_nas_eps_bearer_context_status_type *bearer_context_status
);


/*===========================================================================
FUNCTION      decode_dcn_id_ie

DESCRIPTION
  This function decode dcn id.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
boolean decode_dcn_id_ie
(
  byte                                     *src,
  word                                     *pos,
  word                                     *buffer_length,
  uint16                                   *dcn_id
);

/*===========================================================================
FUNCTION      decode_hc_config_status_ie

DESCRIPTION
  This function decode header compression configuration status Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_hc_config_status_ie
(
  byte                                     *src,
  word                                     *pos,
  word                                     *buffer_length,
  uint16                                   *hc_config_status
);

/*===========================================================================
FUNCTION      DECODE_TAI_LIST_IE

DESCRIPTION
  This function decodes TAI list Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_tai_list_ie
(
  byte                          *src,
  word                          *pos,
  word                          *buffer_length,
  lte_nas_emm_tai_lst_info_type *msg_type
);


/*===========================================================================
FUNCTION      DECODE_MS_IDENTITY_IE

DESCRIPTION
  This function decodes MS identity Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_ms_identity_ie
(
  byte                  *src,
  word                  *pos,
  word                  *buffer_length,
  gmm_ie_LV_mobile_id_T *ms_id
);


/*===========================================================================
FUNCTION      DECODE_GUTI_IE

DESCRIPTION
  This function decodes GUTI Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_guti_ie
(
  byte                  *src,
  word                  *pos,
  word                  *buffer_length,
  lte_nas_emm_guti_type *guti
);

#ifdef FEATURE_LTE_REL14
/*===========================================================================
FUNCTION      DECODE_HASH_MME_IE

DESCRIPTION
  This function decodes HashMME Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_hash_mme_ie
(
  byte    *src,
  word    *pos,
  word    *buffer_length,
  uint64  *hash_mme
);
#endif


/*===========================================================================
FUNCTION      DECODE_EDRX_PARAMS_IE

DESCRIPTION
  This function decodes eDRX parameters Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean decode_edrx_params_ie
(
  byte  *src,
  word  *pos,
  word  *buffer_length,
  byte  *edrx_params
);

#endif /* FEATURE_LTE */
#endif /* _MSG_LIB_DECODE_COMMON_EMM_IE_H_ */
