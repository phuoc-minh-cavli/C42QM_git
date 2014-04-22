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

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/msg_lib_decode_common_emm_ie.c#1 $
$Author: pwbldsvc $
$DateTime: 2023/04/25 14:39:18 $

when         who                          what                              
--------    --------  ------------------------------------------------
10/16/14     RI        Mainlined spec changes upto Dec 09.
10/26/09     hnam      GUTI parsing is wrongly interpreted in teh spec,
                       corrected the working (based on June spec)                       
10/2/09      hnam      Fixed the GUTI validation check for MNC DIGIT 3
07/31/09     hnam      Added validation check for MNC/MCC in GUTI 
06/22/09     hnam      Fixed the "decode_tai_list" to parse tai list 0.
                       (Ref: CR 186713)
06/04/09     zren      Added code to save the type of tai list in Attach Accept
                       message
05/17/09     hnam      Added the code to support security
04/25/09     hnam      Removed Length parameter in EPS_BEARER_CONTEXT_STATUS
04/10/09     hnam      Added decode_ms_identity_ie function to encode ms identity
04/07/09     hnam      Fix m_tmsi and guti's mme group id encoding and decoding
==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "comdef.h"
#include "bit.h"
#include "lte_nas.h"
#include "lte_nas_common_v.h"
#include "msg_lib_decode_common_emm_ie.h" 
#include "lte_nas_esm_message_type.h"
#include "lte_nas_esm_message_description.h"
#include "lte_nas_emm_message_description.h"
#include "msg_lib_declarations.h"
#include "msg.h"
#include "err.h"
#include "emm_utility.h"
#include "gmm_int_v.h"

/*===========================================================================
                        FUNCTION DEFINITIONS
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
boolean is_plmn_bcd_coded
(
  sys_plmn_id_s_type plmn
)
{
  /*If the operator decides to use 2 digit MNC, then mnc digit 3 will have value '1111' (binary)*/
  byte two_digit_mnc_val = 15; /*15 = 1111 (binary)*/

  if( LOWER_BCD(plmn.identity[0]) > MAX_BCD_VAL )
  {
    MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: MCC digit1 = %X is not BCD coded.",
                   LOWER_BCD(plmn.identity[0]));
    return FALSE;
  }
  else if( UPPER_BCD(plmn.identity[0]) > MAX_BCD_VAL )
  {
    MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: MCC digit2 = %X is not BCD coded.",
                   UPPER_BCD(plmn.identity[0]));
    return FALSE;
  }
  else if( LOWER_BCD(plmn.identity[1]) > MAX_BCD_VAL )
  {
    MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: MCC digit3 = %X is not BCD coded.",
                   LOWER_BCD(plmn.identity[1]));
    return FALSE;
  }
  else if( (UPPER_BCD(plmn.identity[1]) > MAX_BCD_VAL) &&
    (UPPER_BCD(plmn.identity[1]) != two_digit_mnc_val) )
  {
    MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: MNC digit3 = %X is not BCD coded.",
                   UPPER_BCD(plmn.identity[1]));
    return FALSE;
  }
  else if( LOWER_BCD(plmn.identity[2]) > MAX_BCD_VAL )
  {
    MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: MNC digit1 = %X is not BCD coded.",
                   LOWER_BCD(plmn.identity[2]));
    return FALSE;
  }
  else if( UPPER_BCD(plmn.identity[2]) > MAX_BCD_VAL )
  {
    MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: MNC digit2 = %X is not BCD coded.",
                   UPPER_BCD(plmn.identity[2]));
    return FALSE;
  }
  else
  {
    return TRUE;
  }
} /* end is_plmn_bcd_coded() */


/*===========================================================================
FUNCTION      DECODE_ADDITIONAL_INFO_IE

DESCRIPTION
  This function decodes Additional Information Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
boolean decode_additional_info_ie
(
  byte                                 *src,
  word                                 *pos,
  word                                 *buffer_length,
  lte_nas_generic_additional_info_type *add_info
)
{
  byte num_bits_to_read = 0;

  /***** Length of Additional information IE *****/
  num_bits_to_read = 8;
  add_info->additional_info_length = b_unpackb(src, *pos, num_bits_to_read);

  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  if(add_info->additional_info_length > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), add_info->additional_info_length);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(add_info->additional_info_length < MIN_ADDITIONAL_INFO_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received Additional information IE len %d < %d Min allowed",
                   add_info->additional_info_length, MIN_ADDITIONAL_INFO_LENGTH);
    return FALSE;
  }

  /* Additional information value is packed into a DSM item */
  add_info->additional_info_value_ptr = src + (*pos/8);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(add_info->additional_info_length*8, pos, buffer_length);

  return TRUE;
} /* end decode_additional_info_ie() */

#if !defined(FEATURE_NAS_IMS_VOICE_DISABLED) || !defined(FEATURE_NAS_DISABLE_VOICE)
/*===========================================================================
FUNCTION      DECODE_EMERGENCY_NUM_LIST

DESCRIPTION
  This function decodes Emergency Number List Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
boolean decode_emergency_num_list
(
  byte                          *src,
  word                          *pos, 
  word                          *buffer_length,
  byte                          ie_length, 
  mmr_emergency_num_list_s_type *emergency_num_list
)
{

  /* Based on the length of Emergency number list IE decode the other fields */
  if(ie_length > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), ie_length);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(ie_length < MIN_EMERGENCY_NUM_LIST_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received Emergency number list IE len %d < %d Min allowed",
                   ie_length, MIN_EMERGENCY_NUM_LIST_LENGTH);
    return FALSE;
  }

  mm_parse_emergency_number(emergency_num_list, src + *pos/8);

  if(emergency_num_list->emergency_number_count == 0)
  {
    return FALSE;
  }

  /* Adjust buffer length. +1 is for the Length indicator byte which was not adjusted before */
  *pos = *pos + (ie_length + 1)*8;
  *buffer_length = *buffer_length - (ie_length + 1)*8;

  return TRUE;
} /* end decode_emergency_num_list() */
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
boolean decode_gprs_timer2_ie
(
  byte  *src,
  word  *pos,
  word  *buffer_length,
  dword *timer_value
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  byte ie_len = 0;

  /***** Length of GPRS timer 2 IE *****/
  num_bits_to_read = 8;
  ie_len = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /* Based on the length of GPRS timer 2 IE decode the other fields */
  if(ie_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), ie_len);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(ie_len < MIN_GPRS_TIMER2_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received GPRS timer 2 IE len %d < %d Min allowed",
                   ie_len, MIN_GPRS_TIMER2_LENGTH);
    return FALSE;
  }

  /* Read timer octet to decode GPRS timer 2 unit and value */
  num_bits_to_read = 8;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  *timer_value = calc_gprs_timer(unpackedbytes);

   /* Discard additional octets in GPRS timer 2 IE */
   if(ie_len > MAX_GPRS_TIMER2_LENGTH)
   {
     MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in GPRS timer 2 IE",
                    (ie_len - MAX_GPRS_TIMER2_LENGTH));
     ADJUST_OFFSET_AND_BUF_LEN_EMM((ie_len - MAX_GPRS_TIMER2_LENGTH)*8, pos, buffer_length);
   }

  return TRUE;
} /* end decode_gprs_timer2_ie() */


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
boolean decode_gprs_timer3_ie
(
  byte  *src,
  word  *pos,
  word  *buffer_length,
  uint64 *timer_value
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  byte ie_len = 0;

  /***** Length of GPRS timer 3 IE *****/
  num_bits_to_read = 8;
  ie_len = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /* Based on the length of GPRS timer 3 IE decode the other fields */
  if(ie_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), ie_len);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(ie_len < MIN_GPRS_TIMER3_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received GPRS timer 3 IE len %d < %d Min allowed",
                   ie_len, MIN_GPRS_TIMER3_LENGTH);
    return FALSE;
  }

  /* Read timer octet to decode GPRS timer 3 unit and value */
  num_bits_to_read = 8;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  *timer_value = gmm_convert_timer3_value_ie_to_msecs(unpackedbytes);

   /* Discard additional octets in GPRS timer 3 IE */
   if(ie_len > MAX_GPRS_TIMER3_LENGTH)
   {
     MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in GPRS timer 3 IE",
                    (ie_len - MAX_GPRS_TIMER3_LENGTH));
     ADJUST_OFFSET_AND_BUF_LEN_EMM((ie_len - MAX_GPRS_TIMER3_LENGTH)*8, pos, buffer_length);
   }

  return TRUE;
} /* end decode_gprs_timer3_ie() */


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
boolean decode_eps_nw_feat_spt_ie
(
  byte                          *src,
  word                          *pos,
  word                          *buffer_length,
  sys_nw_feature_support_s_type *eps_nw_ft_spt
)
{
  byte num_bits_to_read = 0, ie_len = 0;
  byte extra_length = 0;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  /***** Length of EPS network feature support IE *****/
  num_bits_to_read = 8;
  ie_len = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /* Based on the length of EPS network feature support IE decode the other fields */
  if(ie_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), ie_len);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(ie_len < MIN_EPS_NW_FEAT_SPT_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received EPS network feature support IE len %d < %d Min allowed",
                   ie_len, MIN_EPS_NW_FEAT_SPT_LENGTH);
    return FALSE;
  }

  if(ie_len > MAX_EPS_NW_FEAT_SPT_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received EPS network feature support IE len %d > %d Max allowed. Decoding upto Max",
                   ie_len, MAX_EPS_NW_FEAT_SPT_LENGTH);
    extra_length = ie_len - MAX_EPS_NW_FEAT_SPT_LENGTH;
  }

#ifdef FEATURE_CIOT
    num_bits_to_read = 1;
    eps_nw_ft_spt->cp_ciot_supported = b_unpackb(src,(word)(*pos), num_bits_to_read);
    eps_nw_ft_spt->er_without_pdn_supported = b_unpackb(src,(word)(*pos+1), num_bits_to_read);
#endif
#ifdef FEATURE_LTE_REL9
#ifdef FEATURE_LTE_REL10
  if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
  {
    num_bits_to_read = 1;
    /* read ESR PS bit directly */
    eps_nw_ft_spt->esr_ps_supported = b_unpackb(src,(word)(*pos+2), num_bits_to_read);
  }
#endif
  /* Skip 3 bits and read CS-LCS bits directly */
  num_bits_to_read = 2;
  eps_nw_ft_spt->cs_lcs_val = b_unpackb(src,(word)(*pos+3), num_bits_to_read);
  if(eps_nw_ft_spt->cs_lcs_val > SYS_CS_LCS_SUPPORTED)
  {
    MSG_ERROR_DS_1(MM_SUB, "CL-LCS value = %d is reserved", eps_nw_ft_spt->cs_lcs_val);
    return FALSE;
  }

  /* Read EPC-LCS bit */
  num_bits_to_read = 1;
  eps_nw_ft_spt->lcs_supported = b_unpackb(src,(word)(*pos+5), num_bits_to_read);

  /* Read EMC BS bit */
  num_bits_to_read = 1; 
  eps_nw_ft_spt->emc_bs_supported = b_unpackb(src,(word)(*pos+6), num_bits_to_read);
#endif /* FEATURE_LTE_REL9 */

  /* Read IMS VoPS bit */
  num_bits_to_read = 1;
  eps_nw_ft_spt->ims_vops_supported = b_unpackb(src, (word)(*pos+7), num_bits_to_read);

  num_bits_to_read = 8;
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

#ifdef FEATURE_CIOT
  if(ie_len >= MAX_EPS_NW_FEAT_SPT_LENGTH) 
  {
    num_bits_to_read = 1;

	
    /* Read CE bit bit */
    eps_nw_ft_spt->ce_restriction = b_unpackb(src, (word)(*pos+3), num_bits_to_read);
    MSG_MED_DS_1(MM_SUB, "CE_supported value = %d", eps_nw_ft_spt->ce_restriction);

    /* Read epco bit */
    eps_nw_ft_spt->epco_supported = b_unpackb(src, (word)(*pos+4), num_bits_to_read);
    MSG_MED_DS_1(MM_SUB, "epco_supported value = %d", eps_nw_ft_spt->epco_supported);

    /* Read hc_cp_ciot bit */
    eps_nw_ft_spt->hc_cp_ciot_supported = b_unpackb(src, (word)(*pos+5), num_bits_to_read);
    MSG_MED_DS_1(MM_SUB, "hc_cp_ciot_supported value = %d", eps_nw_ft_spt->hc_cp_ciot_supported);

    /* Read s1_u_data bit */
    eps_nw_ft_spt->s1_u_data_supported = b_unpackb(src, (word)(*pos+6), num_bits_to_read);
    MSG_MED_DS_1(MM_SUB, "s1_u_data_supported value = %d", eps_nw_ft_spt->s1_u_data_supported);

    /* Read up_ciot bit */
    eps_nw_ft_spt->up_ciot_supported = b_unpackb(src, (word)(*pos+7), num_bits_to_read);
    MSG_MED_DS_1(MM_SUB, "up_ciot_supported value = %d", eps_nw_ft_spt->up_ciot_supported);
  
    /* Adjust buffer length */
    num_bits_to_read = 8;
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
  }
#endif

  /* Discard additional octets in EPS network feature support IE */
  if(extra_length > 0)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in EPS network feature support IE",
                 extra_length);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(extra_length*8, pos, buffer_length);
  }

  return TRUE;
} /* end decode_eps_nw_feat_spt_ie() */


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
boolean decode_eplmn_list_ie
(
  byte                      *src,
  word                      *pos,
  word                      *buffer_length,
  lte_nas_emm_plmn_lst_type *eplmn_list
)
{
  byte num_bits_to_read = 0, ie_len = 0;
  byte extra_length = 0, plmn_index = 0;
  byte plmn_len = 3;

  /***** Length of Equivalent PLMNs IE *****/
  num_bits_to_read = 8;
  ie_len = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /* Based on the length of Equivalent PLMNs IE decode the other fields */
  if(ie_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), ie_len);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(ie_len < MIN_EPLMN_LIST_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received Equivalent PLMNs IE len %d < %d Min allowed",
                   ie_len, MIN_EPLMN_LIST_LENGTH);
    return FALSE;
  }

  if(ie_len > MAX_EPLMN_LIST_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received Equivalent PLMNs IE len %d > %d Max allowed. Decoding upto Max",
                   ie_len, MAX_EPLMN_LIST_LENGTH);
    extra_length = ie_len - MAX_EPLMN_LIST_LENGTH;
    ie_len = MAX_EPLMN_LIST_LENGTH;
  }

  /* Total number of EPLMNs */
  eplmn_list->length_of_plmn_lst_contents = ie_len/plmn_len;

  /***** Decode EPLMNs *****/
  num_bits_to_read = 8;
  for(plmn_index = 0; (plmn_index < eplmn_list->length_of_plmn_lst_contents) &&
                                                   (plmn_index < MAX_NO_OF_PLMNs); plmn_index++)
  {
    eplmn_list->plmn[plmn_index].identity[0] = b_unpackb(src, *pos, num_bits_to_read);
    eplmn_list->plmn[plmn_index].identity[1] = b_unpackb(src, (word)(*pos+8), num_bits_to_read);
    eplmn_list->plmn[plmn_index].identity[2] = b_unpackb(src, (word)(*pos+16), num_bits_to_read);

    /* Adjust buffer length for PLMN octets read */
    ADJUST_OFFSET_AND_BUF_LEN_EMM((num_bits_to_read * plmn_len), pos, buffer_length);

    if(is_plmn_bcd_coded(eplmn_list->plmn[plmn_index]) == FALSE)
    {
      MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= PLMN %d not BCD coded", (plmn_index+1));
      return FALSE;
    }
  }

  /* Discard additional octets in Equivalent PLMNs IE. In case ie_len is less than
     MAX_EPLMN_LIST_LENGTH and also is not an integral multiple of plmn_len, discard
     the redundant octets that cannot be decoded as well. This will not happen but is
     is done to ensure that all additional ocets in Equivalent PLMNs IE are discarded */
  if((extra_length + (ie_len % plmn_len)) > 0)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in Equivalent PLMNs IE",
                 (extra_length + (ie_len % plmn_len)));
    ADJUST_OFFSET_AND_BUF_LEN_EMM((extra_length + (ie_len % plmn_len))*8, pos, buffer_length);
  }

  return TRUE;
} /* end decode_eplmn_list_ie() */


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
boolean decode_nw_name
(
  byte                 *src,
  word                 *pos,
  word                 *buffer_length,
  lte_nas_nw_name_type *nw_name
)
{
  byte num_bits_to_read = 0, ie_len = 0;
  byte extra_length = 0, num_spare_bits = 0;
  byte octet = 0;


  /***** Length of Network name IE *****/
  num_bits_to_read = 8;
  ie_len = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /* Based on the length of Network name IE decode the other fields */
  if(ie_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), ie_len);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(ie_len < MIN_NETWORK_NAME_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received Network Name IE len %d < %d Min allowed",
                   ie_len, MIN_NETWORK_NAME_LENGTH);
    return FALSE;
  }

  if(ie_len > MAX_NW_NAME_LEN)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received Network name IE len %d > %d Max allowed. Decoding upto Max",
                   ie_len, MAX_NW_NAME_LEN);
    extra_length = ie_len - MAX_NW_NAME_LEN;
  }

  /***** Skip ext bit *****/
  num_bits_to_read = 1;
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /***** Coding scheme *****/
  num_bits_to_read = 3;
  nw_name->coding_scheme = b_unpackb(src, *pos, num_bits_to_read);
  if((nw_name->coding_scheme != GSM_DEFAULT_ALPHABET) &&
     (nw_name->coding_scheme != UCS_2))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Invalid coding scheme = %d", nw_name->coding_scheme);
    return FALSE;
  }
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /***** Add CI *****/
  num_bits_to_read = 1;
  nw_name->country_initial_req = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /***** Number of spare bits in last octet *****/
  num_bits_to_read = 3;
  num_spare_bits = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /* Initialize network name text string length to 0 */
  nw_name->length = 0;

  /* One octet is already read out of ie_len octets. Last octet may have spare bits,
     so start reading from 2nd octet till (ie_len - 1) octet and read the last octet
     separately. Also make sure that nw_name->length doesn't exceed 247 octets as the last
     octet needs to be read separately. There can be max 248 (MAX_NW_NAME_LEN - 1)
     octets in nw_name->val array and the last octet needs to be read outside this for loop. */
  num_bits_to_read = 8;
  for(octet = 2; (octet < ie_len) && (nw_name->length < (MAX_NW_NAME_LEN - 1)); octet++)
  {
    nw_name->val[nw_name->length] = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
    nw_name->length++;
  }

  /* Handle last octet */
  if((ie_len > 1) && (nw_name->length < MAX_NW_NAME_LEN))
  {
    num_bits_to_read = 8 - num_spare_bits;
    nw_name->val[nw_name->length] =
                           b_unpackb(src,(word)(*pos + num_spare_bits), num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(8, pos, buffer_length);
    nw_name->length++;
  }

  /* Discard additional octets in Network name IE */
  if(extra_length > 0)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in Network name IE",
                  extra_length);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(extra_length*8, pos, buffer_length);
  }

  return TRUE;
} /* end decode_nw_name() */


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
boolean decode_daylight_saving_time_ie
(
  byte                           *src,
  word                           *pos,
  word                           *buffer_length,
  lte_nas_day_light_savings_type *daylight_saving_time
)
{
  byte num_bits_to_read = 0, ie_len = 0;
  byte extra_length = 0;

  /***** Length of Daylight saving time IE *****/
  num_bits_to_read = 8;
  ie_len = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /* Based on the length of Daylight saving time IE decode the other fields */
  if(ie_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), ie_len);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(ie_len < MIN_DAYLIGHT_SAVING_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received Daylight saving time IE len %d < %d Min allowed",
                   ie_len, MIN_DAYLIGHT_SAVING_LENGTH);
    return FALSE;
  }

  if(ie_len > MAX_DAYLIGHT_SAVING_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received Daylight saving time IE len %d > %d Max allowed. Decoding upto Max",
                   ie_len, MAX_DAYLIGHT_SAVING_LENGTH);
    extra_length = ie_len - MAX_DAYLIGHT_SAVING_LENGTH;
  }

  /* Skip 6 Spare bits and read 2 bits Daylight saving time value directly */
  num_bits_to_read = 2;

  *daylight_saving_time = b_unpackb(src, (word)(*pos+6), num_bits_to_read);
  if((*daylight_saving_time != NO_ADJUSTMENTS) &&
     (*daylight_saving_time != PLUS_ONE_HOUR) &&
     (*daylight_saving_time != PLUS_TWO_HOUR))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Invalid Daylight saving time value = %d received",
                   *daylight_saving_time);
    return FALSE;
  }

  /* Adjust buffer length */
  num_bits_to_read = 8;
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /* Discard additional octets in Daylight saving time IE */
  if(extra_length > 0)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in Daylight saving time IE",
                 extra_length);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(extra_length*8, pos, buffer_length);
  }

  return TRUE;
} /* end decode_daylight_saving_time_ie() */


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
boolean decode_eps_bearer_context_status
(
  byte                                   *src,
  word                                   *pos,
  word                                   *buffer_length,
  lte_nas_eps_bearer_context_status_type *bearer_context_status
)
{
  byte num_bits_to_read = 0, ie_len = 0;
  byte extra_length = 0;


  /***** Length of EPS bearer context status IE *****/
  num_bits_to_read = 8;
  ie_len = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /* Based on the length of EPS bearer context status IE decode the other fields */
  if(ie_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), ie_len);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(ie_len < MIN_EPS_BEARER_CONTEXT_STATUS_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received EPS bearer context status IE len %d < %d Min allowed",
                   ie_len, MIN_EPS_BEARER_CONTEXT_STATUS_LENGTH);
    return FALSE;
  }

  if(ie_len > MAX_EPS_BEARER_CONTEXT_STATUS_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received EPS bearer context status IE len %d > %d Max allowed. Decoding upto Max",
                   ie_len, MAX_EPS_BEARER_CONTEXT_STATUS_LENGTH);
    extra_length = ie_len - MAX_EPS_BEARER_CONTEXT_STATUS_LENGTH;
  }

  /* EBI(0) - EBI(4): Bits 0 to 4 of octet 3 are spare and shall be coded as zero 
     Hence read EBI(5) - EBI(7) only in octet 3 */
  num_bits_to_read = 1;
  bearer_context_status->eps_bearer_context_7_exists = b_unpackb(src, *pos, num_bits_to_read);  
  bearer_context_status->eps_bearer_context_6_exists = b_unpackb(src,(word)(*pos+1), num_bits_to_read);
  bearer_context_status->eps_bearer_context_5_exists = b_unpackb(src,(word)(*pos+2), num_bits_to_read);

  /* Octet 3 is read. Adjust buffer length */
  ADJUST_OFFSET_AND_BUF_LEN_EMM(8, pos, buffer_length);
  
  /* Read octet 4: EBI(8) - EBI(15) */
  bearer_context_status->eps_bearer_context_15_exists = b_unpackb(src, *pos, num_bits_to_read);  
  bearer_context_status->eps_bearer_context_14_exists = b_unpackb(src, (word)(*pos+1), num_bits_to_read);
  bearer_context_status->eps_bearer_context_13_exists = b_unpackb(src, (word)(*pos+2), num_bits_to_read);
  bearer_context_status->eps_bearer_context_12_exists = b_unpackb(src, (word)(*pos+3), num_bits_to_read);  
  bearer_context_status->eps_bearer_context_11_exists = b_unpackb(src, (word)(*pos+4), num_bits_to_read);
  bearer_context_status->eps_bearer_context_10_exists = b_unpackb(src, (word)(*pos+5), num_bits_to_read);
  bearer_context_status->eps_bearer_context_9_exists = b_unpackb(src, (word)(*pos+6), num_bits_to_read);  
  bearer_context_status->eps_bearer_context_8_exists = b_unpackb(src, (word)(*pos+7), num_bits_to_read);

  /* Octet 4 is read. Adjust buffer length */
  ADJUST_OFFSET_AND_BUF_LEN_EMM(8, pos, buffer_length);

  /* Discard additional octets in EPS bearer context status IE */
  if(extra_length > 0)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in EPS bearer context status IE",
                  extra_length);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(extra_length*8, pos, buffer_length);
  }

  return TRUE;
} /* end decode_eps_bearer_context_status() */

#ifdef FEATURE_CIOT

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
)
{
  byte num_bits_to_read = 0, ie_len = 0;
  byte extra_length = 0;

  /***** Length of DCN ID IE *****/
  num_bits_to_read = 8;
  ie_len = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /* Based on the length of  DCN IE decode the other fields */
  if(ie_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d",(*buffer_length/8));
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(ie_len < MIN_DCN_ID_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received DCN IE len %d < %d Min allowed",
                   ie_len, MIN_DCN_ID_LENGTH);
    return FALSE;
  }

  if(ie_len > MAX_DCN_ID_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received DCN IE len %d > %d Max allowed. Decoding upto Max",
                   ie_len, MAX_DCN_ID_LENGTH);
    extra_length = ie_len - MAX_DCN_ID_LENGTH;
  }

  num_bits_to_read = 16;
  *dcn_id = b_unpackw(src, *pos, num_bits_to_read);  

  /* Octet 3&4 is read. Adjust buffer length */
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
  
  /* Discard additional octets in EPS bearer context status IE */
  if(extra_length > 0)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in DCN IE",
                  extra_length);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(extra_length*8, pos, buffer_length);
  }

  return TRUE;
} /* end decode_dcn_id_ie() */


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
boolean decode_hc_config_status_ie
(
  byte                                     *src,
  word                                     *pos,
  word                                     *buffer_length,
  uint16                                   *hc_config_status
)
{
  byte num_bits_to_read = 0, ie_len = 0;
  byte extra_length = 0;

  /***** Length of hc configuration status IE *****/
  num_bits_to_read = 8;
  ie_len = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /* Based on the length of EPS bearer context status IE decode the other fields */
  if(ie_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d",(*buffer_length/8));
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(ie_len < MIN_HC_CONFIG_STATUS_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received HC config status IE len %d < %d Min allowed",
                   ie_len, MIN_HC_CONFIG_STATUS_LENGTH);
    return FALSE;
  }

  if(ie_len > MAX_HC_CONFIG_STATUS_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received HC config status IE len %d > %d Max allowed. Decoding upto Max",
                   ie_len, MAX_HC_CONFIG_STATUS_LENGTH);
    extra_length = ie_len - MAX_HC_CONFIG_STATUS_LENGTH;
  }

  num_bits_to_read = 16;
  *hc_config_status = b_unpackw(src, *pos, num_bits_to_read);  

  /* Octet 3&4 is read. Adjust buffer length */
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
  
  /* Discard additional octets in EPS bearer context status IE */
  if(extra_length > 0)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in HC configuration status IE",
                  extra_length);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(extra_length*8, pos, buffer_length);
  }

  return TRUE;
} /* end decode_hc_config_status_ie() */
#endif

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
boolean decode_tai_list_ie
(
  byte                          *src,
  word                          *pos,
  word                          *buffer_length,
  lte_nas_emm_tai_lst_info_type *msg_type
)
{
  byte tai_list_len = 0, extra_length = 0, max_tai_elements = 0;
  word max_tai_bits = 0, tai_lst_index = 0, loop = 0 ;
  byte num_bits_to_read = 0, plmn_len = 3, octet = 0;
  byte tac_len = 2, tai_count = 0;
  byte partial_tai_list_count = 0;
  byte tai_list_type = 0;
  boolean plmn_bcd_coded = FALSE;
  boolean exit_for_loop = FALSE;
  boolean exit_while_loop = FALSE;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word orig_max_tai_bits = 0;
#endif
  /***** Length of TAI List IE *****/
  /***** Length of TAI List *****/
  num_bits_to_read = 8;
  tai_list_len = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /* Based on the length of TAI List IE decode the other fields */
  if(tai_list_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), tai_list_len);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(tai_list_len < MIN_TAI_LIST_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received TAI List IE len %d < %d Min allowed",
                   tai_list_len, MIN_TAI_LIST_LENGTH);
    return FALSE;
  }

  if(tai_list_len > MAX_TAI_LIST_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received TAI List IE len %d > %d Max allowed. Restricting to Max",
                   tai_list_len, MAX_TAI_LIST_LENGTH);
    extra_length = tai_list_len - MAX_TAI_LIST_LENGTH;
    tai_list_len = MAX_TAI_LIST_LENGTH;
  }

  /* TAI list length in bits */
  max_tai_bits = tai_list_len*8;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  orig_max_tai_bits = max_tai_bits;
#endif
    
  while(max_tai_bits)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if((max_tai_bits > orig_max_tai_bits) || (*buffer_length < max_tai_bits ))
    {
      MSG_ERROR_DS_3(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len = %d, max_tai_bits = %d, orig_max_tai_bits %d. Stop parsing",
                     *buffer_length, max_tai_bits, orig_max_tai_bits);
      buffer_length_check_failed = TRUE;
      return FALSE;
    }
#endif

    if(tai_count == MAX_NO_OF_TAI)
    {
      MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Max permissible TAIs = %d already decoded", MAX_NO_OF_TAI);
      break;
    }

    /***** Spare bit *****/
    num_bits_to_read = 1;
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
    max_tai_bits = max_tai_bits - num_bits_to_read;

    /***** Type of list *****/
    num_bits_to_read = 2;
    tai_list_type = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
    max_tai_bits = max_tai_bits - num_bits_to_read;
    
    /* TAI List type 0 & 1 are similar, so we use NAS_EMM_TAI_LST0 for type 0 & 1 */
    if((tai_list_type == 0) || (tai_list_type == 1))
    {
      msg_type->tai_lst_elem[tai_lst_index].tai_lst = NAS_EMM_TAI_LST0;
    }
    else if(tai_list_type == 2)
    {
      msg_type->tai_lst_elem[tai_lst_index].tai_lst = NAS_EMM_TAI_LST2;
    }
    else
    {
      MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Invalid partial TAI list type = %d received", tai_list_type);
      break;
    }


    switch(tai_list_type)
    {
      /***** TAI List type 0 *****/
      case NAS_EMM_TAI_LST0:
        /***** Number of elements *****/
        num_bits_to_read = 5;
        max_tai_elements = b_unpackb(src, *pos, num_bits_to_read) + 1;
        ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        max_tai_bits = max_tai_bits - num_bits_to_read;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(max_tai_elements > (MAX_NO_OF_TAI - tai_count))
        {
          MSG_HIGH_DS_2(MM_SUB,"=NAS_MSG_LIB= TAIs received = %d exceed max available = %d. Restricting to max",
                        max_tai_elements, (MAX_NO_OF_TAI - tai_count));
          max_tai_elements = (MAX_NO_OF_TAI - tai_count);
        }
#endif
        if(max_tai_bits < ((plmn_len + (max_tai_elements*tac_len))*8))
        {
          MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Available TAI List length is insufficient to completely decode typelist 0");
          exit_while_loop = TRUE;
          break;
        }

        msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.tac_lst_length = max_tai_elements;
 
        /***** Decode PLMN *****/
        num_bits_to_read = 8;
        for(octet = 0; octet < plmn_len; octet++)
        {
          msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.plmn.identity[octet] =
                                                      b_unpackb(src, *pos, num_bits_to_read);
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
          max_tai_bits = max_tai_bits - num_bits_to_read;
        }

        plmn_bcd_coded = is_plmn_bcd_coded(msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.plmn);
        if(plmn_bcd_coded == FALSE)
        {
          MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= PLMN not BCD coded");
          exit_while_loop = TRUE;
          break;
        }

        /* Decode the bytes representing Tracking Area Code */
        for(octet = 0; octet < max_tai_elements; octet++)
        {
          /* Each TAC takes 2 bytes, so decode 16 bits of data */
          num_bits_to_read = 16;
          msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.tac_lst[octet] =
                                                        b_unpackw(src, *pos, num_bits_to_read);
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
          max_tai_bits = max_tai_bits - num_bits_to_read;
          tai_count++;
        }
        partial_tai_list_count++;
        break;

      /***** TAI List type 1 *****/
      case NAS_EMM_TAI_LST1:
        /***** Number of elements *****/
        num_bits_to_read = 5;
        max_tai_elements = b_unpackb(src, *pos, num_bits_to_read) + 1;
        ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        max_tai_bits = max_tai_bits - num_bits_to_read;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(max_tai_elements > (MAX_NO_OF_TAI - tai_count))
        {
          MSG_HIGH_DS_2(MM_SUB,"=NAS_MSG_LIB= TAIs received = %d exceed max available = %d. Restricting to max",
                        max_tai_elements, (MAX_NO_OF_TAI - tai_count));
          max_tai_elements = (MAX_NO_OF_TAI - tai_count);
        }
#endif
        if(max_tai_bits < ((plmn_len + tac_len)*8))
        {
          MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Available TAI List length is insufficient to completely decode typelist 1");
          exit_while_loop = TRUE;
          break;
        }

        /***** Decode PLMN *****/
        num_bits_to_read = 8;
        for(octet = 0; octet < plmn_len; octet++)
        {
          msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.plmn.identity[octet] =
                                                      b_unpackb(src, *pos, num_bits_to_read);
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
          max_tai_bits = max_tai_bits - num_bits_to_read;
        }

        plmn_bcd_coded = is_plmn_bcd_coded(msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.plmn);
        if(plmn_bcd_coded == FALSE)
        {
          MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= PLMN not BCD coded");
          exit_while_loop = TRUE;
          break;
        }

        /* Decode TACs. Each TAC takes 2 bytes, so decode 16 bits of data */
        num_bits_to_read = 16;
        msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.tac_lst_length = max_tai_elements;
        msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.tac_lst[0] =
                                                     b_unpackw(src, *pos, num_bits_to_read);

        ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        max_tai_bits = max_tai_bits - num_bits_to_read;
        tai_count++;

        /* 1 TAC decoded. Compute remaining TACs */
        for(octet = 1; octet < max_tai_elements; octet++)
        {
          msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.tac_lst[octet] =
                                              msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.tac_lst[octet - 1] + 1;
          tai_count++;
        }
        partial_tai_list_count++;
        break;

      /***** TAI List type 2 *****/
      case NAS_EMM_TAI_LST2:
        /***** Number of elements *****/
        num_bits_to_read = 5;
        max_tai_elements = b_unpackb(src, *pos, num_bits_to_read) + 1;
        ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        max_tai_bits = max_tai_bits - num_bits_to_read;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(max_tai_elements > (MAX_NO_OF_TAI - tai_count))
        {
          MSG_MED_DS_2(MM_SUB,"=NAS_MSG_LIB= TAIs received = %d exceed max available = %d. Restricting to max",
                        max_tai_elements, (MAX_NO_OF_TAI - tai_count));
          max_tai_elements = (MAX_NO_OF_TAI - tai_count);
        }
#endif
        if(max_tai_bits < ((plmn_len + tac_len) * max_tai_elements * 8))
        {
          MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Available TAI List length is insufficient to completely decode typelist 2");
          exit_while_loop = TRUE;
          break;
        }

        msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst2.tai_lst_length = max_tai_elements;

        for(loop = 0; loop < max_tai_elements; loop++)
        {
          /***** Decode PLMN *****/
          num_bits_to_read = 8;
          for(octet = 0; octet < plmn_len; octet++)
          {
            msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst2.diff_plmn[loop].plmn.identity[octet] =
                                      b_unpackb(src, *pos, num_bits_to_read);

            ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
            max_tai_bits = max_tai_bits - num_bits_to_read;
          }

          plmn_bcd_coded = is_plmn_bcd_coded(msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst2.diff_plmn[loop].plmn);
          if(plmn_bcd_coded == FALSE)
          {
            MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= PLMN not BCD coded");
            exit_for_loop = TRUE;
            exit_while_loop = TRUE;
            break;
          }

          /* Decode TACs. Each TAC takes 2 bytes, so decode 16 bits of data */
          num_bits_to_read = 16;
          msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst2.diff_plmn[loop].tac =
                                                 b_unpackw(src, *pos, num_bits_to_read);

          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
          max_tai_bits = max_tai_bits - num_bits_to_read;
          tai_count++;
        }
        if(exit_for_loop == TRUE)
        {
          break;
        }
        partial_tai_list_count++;
        break;
    
      default:
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Unknown TAI Typelist %d received", tai_list_type);
        exit_while_loop = TRUE;
        break;
    } /* end switch(tai_list_type) */

    if(exit_while_loop == TRUE)
    {
      break;
    }

    /* Increment TAI List index */
    tai_lst_index++; 
  } /* end while(max_tai_bits) */

  /* Discard additional bits in TAI list IE */
  if((max_tai_bits + extra_length*8) > 0)
  {
    MSG_MED_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d bits in TAI list IE",
                 (max_tai_bits + extra_length*8));
    ADJUST_OFFSET_AND_BUF_LEN_EMM((max_tai_bits + extra_length*8), pos, buffer_length);
  }

  msg_type->tai_lst_length = partial_tai_list_count;

  if(msg_type->tai_lst_length == 0)
  {
    MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Tracking area identity list IE decoding failed");
    return FALSE;
  }

  return TRUE;
} /* end decode_tai_list_ie() */


/*===========================================================================
FUNCTION      DECODE_MS_IDENTITY_IE

DESCRIPTION
  This function decodes MS Identity Information Element.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
boolean decode_ms_identity_ie
(
  byte                  *src,
  word                  *pos,
  word                  *buffer_length,
  gmm_ie_LV_mobile_id_T *ms_id
)
{
  byte ms_identity_len = 0, extra_length = 0;
  byte loop = 0, num_bits_to_read = 0;
  byte ms_id_type = 0;

  /***** Length of MS Identity IE *****/
  num_bits_to_read = 8;
  ms_identity_len = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /* Based on the length of MS Identity IE decode the other fields */
  if(ms_identity_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), ms_identity_len);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(ms_identity_len < MIN_MS_IDENTITY_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received MS Identity IE len %d < %d Min allowed",
                   ms_identity_len, MIN_MS_IDENTITY_LENGTH);
    return FALSE;
  }

  if(ms_identity_len > MAX_MS_IDENTITY_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received MS Identity IE len %d > %d Max allowed. Decoding upto Max",
                   ms_identity_len, MAX_MS_IDENTITY_LENGTH);
    extra_length = ms_identity_len - MAX_MS_IDENTITY_LENGTH;
    ms_identity_len = MAX_MS_IDENTITY_LENGTH;
  }

  /* Skip identity digit 1 and odd/even indicator bits and read Type of Identity
  This IE is sent from network to UE in Attach accept or TAU accept message 3GPP TS 24.301 V8.4.0 (2009-12)
  3GPP TS 24.301 V8.4.0 (2009-12) 8.2.26 Tracking area update accept, 8.2.1.4 MS identity
  This IE may be included to assign or unassign a new TMSI to a UE during a combined attach
  3GPP TS 24.301 V8.4.0 (2009-12) 8.2.26 Tracking area update accept, 8.2.26.7 MS identity
  This IE may be included to assign or unassign a new TMSI to a UE during a combined TA/LA update
  Check is restricted to MS Identity type IMSI(for unassigning) & TMSI(for assigning) for above reasons 
  Also refer to subsections 5.5.1.3.4.2 Combined attach successful & 5.5.3.3.4.2 Combined tracking area
  updating successful in 3GPP TS 24.301 V8.4.0 (2009-12)*/
  num_bits_to_read = 3;
  ms_id_type = b_unpackb(src, (word)(*pos+5), num_bits_to_read);

  /* Adjust buffer length */
  num_bits_to_read = 8;
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
  ms_identity_len--;

  if(ms_id_type == LTE_NAS_EMM_IMSI)
  {
    ms_id->id_type = ms_id_type;
  }
  else if(ms_id_type == LTE_NAS_EMM_TMSI)
  {
    ms_id->id_type = ms_id_type;
  
    /* 3GPP TS 24.008 V8.9.0 (2010-03) 10.5.1.4 Mobile Identity the TMSI/P-TMSI/M-TMSI
    is 4 octets long. Populate TMSI from next 4 bytes (TMSI_SIZE) */
    num_bits_to_read = 8;
    for(loop = 0; loop < TMSI_SIZE; loop++)
    {
      if(loop < sizeof(ms_id->value))
      {
        ms_id->value[loop] = b_unpackb(src, *pos, num_bits_to_read);
        ms_id->length++;
        ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        ms_identity_len--;
      }
    }
  }
  else
  {
    MSG_ERROR_DS_1(MM_SUB,"=NAS_MSG_LIB Invalid/unexpected MS identity type = %d",
                   ms_id_type);
    return FALSE;
  }

  /* Discard remaining/extra octets in MS identity IE */
  if((ms_identity_len + extra_length) > 0)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in MS identity IE",
                 (ms_identity_len + extra_length));
    ADJUST_OFFSET_AND_BUF_LEN_EMM((ms_identity_len + extra_length)*8, pos, buffer_length);
  }

  return TRUE;
} /* end decode_ms_identity_ie */


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
boolean decode_guti_ie
(
  byte                  *src,
  word                  *pos,
  word                  *buffer_length,
  lte_nas_emm_guti_type *guti
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  byte guti_len = 0, extra_length = 0, plmn_len = 3;
  byte octet = 0;

  /***** Length of GUTI IE *****/
  num_bits_to_read = 8;
  guti_len = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /* Based on the length of GUTI IE decode the other fields */
  if(guti_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), guti_len);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(guti_len < MIN_GUTI_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received GUTI IE len %d < %d Min allowed",
                   guti_len, MIN_GUTI_LENGTH);
    return FALSE;
  }

  if(guti_len > MAX_GUTI_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received GUTI IE len %d > %d Max allowed. Decoding upto Max",
                   guti_len, MAX_GUTI_LENGTH);
    extra_length = guti_len - MAX_GUTI_LENGTH;
  }

  /* Skip first 4 bits and read odd/even indication bit */
  num_bits_to_read = 1;   
  unpackedbytes = b_unpackb(src, (word)(*pos+4), num_bits_to_read);
  if(unpackedbytes != 0)
  {
    MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Even indication not received in GUTI");
    return FALSE;
  }

  /***** Type of identity *****/
  num_bits_to_read = 3;
  unpackedbytes = b_unpackb(src, (word)(*pos+5), num_bits_to_read);

  if(unpackedbytes != (byte)GUTI_ID)
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Invalid Type of Identity = %d", unpackedbytes);
    return FALSE;
  }
  guti->ue_id = (ue_id_type)unpackedbytes;

  /* Adjust buffer length */
  num_bits_to_read = 8;
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /***** GUTI PLMN *****/
  num_bits_to_read = 8;
  for(octet = 0; octet < plmn_len; octet++)
  {
    guti->plmn.identity[octet] = b_unpackb(src, *pos, num_bits_to_read);

    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
  }

  if(is_plmn_bcd_coded(guti->plmn) == FALSE)
  {
    MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= GUTI PLMN not BCD coded");
    return FALSE;
  }

  /***** MME Group ID *****/
  num_bits_to_read = 8;
  guti->mme_group_id[0] = b_unpackb(src, (word)(*pos+8), num_bits_to_read);
  guti->mme_group_id[1] = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read*2, pos, buffer_length);

  /***** MME Code *****/
  num_bits_to_read =  8;
  guti->mme_code = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /***** M-TMSI *****/
  num_bits_to_read = 8;
  guti->m_tmsi[0] = b_unpackb(src, (word)(*pos+24), num_bits_to_read);
  guti->m_tmsi[1] = b_unpackb(src, (word)(*pos+16), num_bits_to_read);
  guti->m_tmsi[2] = b_unpackb(src, (word)(*pos+8), num_bits_to_read);
  guti->m_tmsi[3] = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read*4, pos, buffer_length);

  /* Skip additional octets in GUTI IE */
  if(extra_length > 0)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in GUTI IE", extra_length);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(extra_length*8, pos, buffer_length);
  }

  return TRUE;
} /* end decode_guti_ie() */

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
boolean decode_hash_mme_ie
(
  byte    *src,
  word    *pos,
  word    *buffer_length,
  uint64  *hash_mme
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  byte ie_len = 0;

  ASSERT(src != NULL);
  ASSERT(pos != NULL);
  ASSERT(buffer_length != NULL);
  ASSERT(hash_mme != NULL);

  /***** Length of HashMME IE *****/
  num_bits_to_read = 8;
  ie_len = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /* Based on the length of HashMME IE decode the other fields */
  if(ie_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), ie_len);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(ie_len < HASH_MME_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received HashMME IE len %d < %d Min allowed",
                   ie_len, HASH_MME_LENGTH);
    return FALSE;
  }

  /* Read HashMME */
   num_bits_to_read = 32;
   *hash_mme += ((uint64)(b_unpackd(src, *pos, num_bits_to_read)) << 32) +
                (uint64)(b_unpackd(src, (word)(*pos+32), num_bits_to_read));

  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read*2, pos, buffer_length);

  /* Discard additional octets in HashMME IE */
  if(ie_len > HASH_MME_LENGTH)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in HashMME IE",
                  (ie_len - HASH_MME_LENGTH));
    ADJUST_OFFSET_AND_BUF_LEN_EMM((ie_len - HASH_MME_LENGTH)*8, pos, buffer_length);
   }

  return TRUE;
} /* end decode_hash_mme_ie() */
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
boolean decode_edrx_params_ie
(
  byte  *src,
  word  *pos,
  word  *buffer_length,
  byte  *edrx_params
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  byte ie_len = 0;

  MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Decoding Extended DRX parameters IE");

  /***** Length of GPRS timer 3 IE *****/
  num_bits_to_read = 8;
  ie_len = b_unpackb(src, *pos, num_bits_to_read);
  MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Decoded length of Extended DRX parameters IE = %d", ie_len);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /* Based on the length of Extended DRX parameters IE decode the other fields */
  if(ie_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d",(*buffer_length/8));
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(ie_len < MIN_EDRX_PARAMS_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received Extended DRX parameters IE len %d < %d Min allowed",
                   ie_len, MIN_EDRX_PARAMS_LENGTH);
    return FALSE;
  }

  /* Read eDRX parameters octet to decode Paging timer window and eDRX value */
  num_bits_to_read = 8;
  *edrx_params = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

   /* Discard additional octets in Extended DRX parameters  IE */
   if(ie_len > MAX_EDRX_PARAMS_LENGTH)
   {
     MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in Extended DRX parameters IE",
                    (ie_len - MAX_EDRX_PARAMS_LENGTH));
     ADJUST_OFFSET_AND_BUF_LEN_EMM((ie_len - MAX_EDRX_PARAMS_LENGTH)*8, pos, buffer_length);
   }

  return TRUE;
}

#endif /* FEATURE_LTE */