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

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/msg_lib_decode_esm.c#1 $
$Author: pwbldsvc $
$DateTime: 2023/04/25 14:39:18 $

when         who                          what                              
--------     --------  ------------------------------------------------
04/24/15     cogolt   Added Rel-10 ESM backoff timer (T3396 timer) support
05/14/10      RI      Mainlined spec changes upto Dec 09.
01/21/10      zren    Added EMM/ESM STATUS feature
08/26/09      MNK      Fix a bug in decode activate default bearer request for MAR spec. Move report_esm_failure
08/04/09      hnam     Return TRUE whenever optional fields fails
                        (so that full message is not discarded)
07/29/09      hnam     Changed the name of the MACRO (for MARCH/DEC spec compatibiltiy), 
                        used the standard macro commonly used by LTE_MODEM_SW
06/08/09      hnam     (1) If ESM PD fails, set nas_esm_index to zero. Previously, this was not reset 
                           & this caused ESM failure (as ESM presumed to have a valid PD). 
                       (2) Fix for CR# 184007 
05/17/09      hnam     Added the code to support security
04/13/09      hnam     Added valid flags for all optional fields.
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
#include "lte_nas_local_cause.h"
#include "amssassert.h"
#include "msg_lib_decode_esm.h"
#include "msg_lib_decode_common_esm_ie.h"
#include "lte_nas_emm_message_type.h"
#include "lte_nas_emm_message_description.h"
#include "msg_lib_declarations.h"
#include "msg_lib_validation.h"
#include "msg.h"
#include "err.h"
#include "esm_cm_msg_if.h"
#include "event_defs.h"
#include "emm_utility.h"
#ifdef FEATURE_LTE_REL9
#include "emm_database.h"
#endif
#include "reg_sim.h"

/*===========================================================================
                        FUNCTION DEFINITIONS
===========================================================================*/

/*============================================================================================================
FUNCTION      POPULATE_ESM_CAUSE

DESCRIPTION
  This function populates the esm_cause value as per Table 9.9.4.4.1: ESM cause information element
  Any other value received by the UE shall be treated as 0010 0010, "service option temporarily out of order"

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================================================*/
static void populate_esm_cause
(
  esm_cause_enum_T *esm_cause,
  byte             unpackedbytes
)
{
  if((unpackedbytes == (byte)NAS_ESM_OPTR_DETERMINED_BARRING)
     ||
     ((unpackedbytes  >= (byte)NAS_ESM_INSUFFICIENT_RESOURCES)&&
      (unpackedbytes  <= (byte)NAS_ESM_PTI_MISMATCH))
     ||
     ((unpackedbytes >= (byte)NAS_ESM_LAST_PDN_DISCONNECTED_NOT_ALLOWED)&&
      (unpackedbytes <= (byte)NAS_ESM_UNSUPPORTED_QCI_VALUE))
     ||
     (unpackedbytes == (byte)NAS_ESM_INVALID_PTI_VALUE)
     ||
     (unpackedbytes == (byte)NAS_ESM_BEARER_HANDLING_NOT_SUPPORTED)
     ||
     ((unpackedbytes >= (byte)NAS_ESM_SYMANTICALLY_INVALID_MSG) &&
      (unpackedbytes <= (byte)NAS_ESM_MSG_INCOMPATIBLE_WITH_PRO_STATE))
     ||
     (unpackedbytes == (byte)NAS_ESM_PROTOCOL_ERROR)
     ||
     (unpackedbytes == (byte)NAS_ESM_APN_RESTRICT_VAL_INCOMPAT_WITH_ACT_CONTEXT)
     ||
     (unpackedbytes == (byte)NAS_ESM_MAX_PDP_CONTEXT_REACHED)
     ||
     (unpackedbytes == (byte)NAS_ESM_APN_NOT_SUPPORTED_IN_PLMN_RAT_COMBINATION))
  {
    *esm_cause = (esm_cause_enum_T)unpackedbytes;
  }
  else
  {
    *esm_cause = NAS_ESM_SERV_OPT_TEMP_OUT_OF_ORDER;
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Unknown ESM cause value = %d received, defaulting to NAS_ESM_SERV_OPT_TEMP_OUT_OF_ORDER", unpackedbytes);
  }
} /* end populate_esm_cause() */


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
boolean decode_esm_notification
(
  byte                     *src,
  word                     *pos,
  word                     *buffer_length,
  lte_nas_esm_notification *esm_notification,
  word                     *esm_len_in_bits
)
{
  byte num_bits_to_read = 0, unpacked_bytes = 0;
  byte notification_ind_len = 1;

  if(*buffer_length < (NAS_ESM_NOTIFICATION - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "NAS_MSG_LIB: Buffer length = %d is insufficient to decode ESM NOTIFICATION",
                   *buffer_length);
    *buffer_length = 0;
    *esm_len_in_bits = 0;
    return FALSE;
  }

  /***** Notification Indicator IE *****/
  /* IE length*/
  num_bits_to_read = 8;
  unpacked_bytes = b_unpackb(src, *pos, num_bits_to_read);
  if(unpacked_bytes != notification_ind_len)
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Invalid length = %d for Notification indicator",
                   unpacked_bytes);
    return FALSE;
  }
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits);

  unpacked_bytes = b_unpackb(src, *pos, num_bits_to_read);
  if(unpacked_bytes != 0x01)
  {
    /* 9.9.4.7A.1 all other values are ignored */
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Ignore unused Notification indicator = %d",
                  unpacked_bytes);
    return FALSE;
  }

  esm_notification->notification_ind = unpacked_bytes;
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits);

  return TRUE;
} /* end decode_esm_notification() */
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
void decode_pdn_conn_req
(
  byte                       *src,
  word                       buffer_length, 
  emm_pdn_connectivity_req_s_type *pdn_conn_req
)
{
  word pos = 24; /* Start reading from the RPDN_TYPE IE (3rd octet) */
  word buf_len_in_bits = 0;
  byte num_bits_to_read = 0;
  byte unpacked_bytes = 0;
  byte esm_info_iei = 0x0d;
  byte apn_iei = 0x28;
#ifdef FEATURE_LTE_REL10
  byte device_prop_iei = 0xC;
#endif
#ifdef FEATURE_CIOT
  byte hc_config_iei = 0x66;
#endif
  byte index = 0;
  word dummy_buf_len = 0;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  

  buffer_length -= 3; /* Start reading from PDN_TYPE, so skip the first three octets */
  buf_len_in_bits = buffer_length * 8;

  /* PDN type IE */
  num_bits_to_read = 4;
  pdn_conn_req->pdn_data.pdn_type = b_unpackb(src, pos, num_bits_to_read);
  pos += num_bits_to_read;
  buf_len_in_bits -= num_bits_to_read;

  /* Request type IE */
  pdn_conn_req->request_type = b_unpackb(src, pos, num_bits_to_read);
  pos += num_bits_to_read;
  buf_len_in_bits -= num_bits_to_read;

  /* ESM information transfer flag IE */
  unpacked_bytes = b_unpackb(src, pos, num_bits_to_read);
  if(esm_info_iei == unpacked_bytes)
  {
    pos += num_bits_to_read;
    buf_len_in_bits -= num_bits_to_read;

    if(buf_len_in_bits)
    {
      /* Transfer flag value */
      pdn_conn_req->pdn_data.esm_info_transfer_flag = b_unpackb(src, pos, num_bits_to_read);
      pos += num_bits_to_read;
      buf_len_in_bits -= num_bits_to_read;
    }
  }

  if(buf_len_in_bits)
  {
    /* Access point name IE */
    num_bits_to_read = 8;
    unpacked_bytes = b_unpackb(src, pos, num_bits_to_read);

    if(apn_iei == unpacked_bytes)
    {
      pos += num_bits_to_read;
      buf_len_in_bits -= num_bits_to_read;
      pdn_conn_req->pdn_data.apn_name.valid = TRUE;
      /* Length of APN address IE*/
      pdn_conn_req->pdn_data.apn_name.apn_addr_len = b_unpackb(src, pos, num_bits_to_read);
      pos += num_bits_to_read;
      buf_len_in_bits -= num_bits_to_read;

      for(index = 0;index < (pdn_conn_req->pdn_data.apn_name.apn_addr_len) &&
                    index < MAX_APN_ADR_LEN; index++)
      {
        pdn_conn_req->pdn_data.apn_name.address[index] = b_unpackb(src, pos, num_bits_to_read);
        pos += num_bits_to_read;
        buf_len_in_bits -= num_bits_to_read;
      }
    }
  }

  num_bits_to_read = 8;

  /* Protocol configuration options IE */
  if(buf_len_in_bits > num_bits_to_read)
  {
    /* IEI */
    unpacked_bytes = b_unpackb(src, pos, num_bits_to_read);
     
    if(unpacked_bytes == PROT_CONFIG_OPT_IEI)
    {
      pos += num_bits_to_read;
      buf_len_in_bits -= num_bits_to_read;

      dummy_buf_len = buf_len_in_bits;
      pdn_conn_req->pdn_data.protocol_config_options.valid =
      decode_prot_conf_opt(src,
                           &pos,
                           &buf_len_in_bits,
                           &pdn_conn_req->pdn_data.protocol_config_options,&dummy_buf_len);
    }
  }

#ifdef FEATURE_LTE_REL10
  /*Device properties IE*/
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
     (reg_sim_per_subs_chk_low_pri_cfg((sys_modem_as_id_e_type)mm_sub_id) == TRUE))
  {
    num_bits_to_read = 8;
    if(buf_len_in_bits)
    {
      /*Get IEI*/
      unpacked_bytes = b_unpackb(src, pos, num_bits_to_read);
      if(((unpacked_bytes & 0xF0)>>4) == device_prop_iei)
      {
        pos += num_bits_to_read;
        buf_len_in_bits -= num_bits_to_read;
        pdn_conn_req->pdn_data.low_priority_signaling = (boolean)(unpacked_bytes & 0xF);
      }
    }
  }
#endif

#ifdef FEATURE_CIOT
  num_bits_to_read = 8;
  /*Header compression configuration IE*/
  if(buf_len_in_bits)
  {
    /*Get IEI*/
    unpacked_bytes = b_unpackb(src, pos, num_bits_to_read);
    if(unpacked_bytes == hc_config_iei)
    {
      pos += num_bits_to_read;
      buf_len_in_bits -= num_bits_to_read;

      dummy_buf_len = buf_len_in_bits;
      pdn_conn_req->pdn_data.hc_config.valid =
      decode_hc_config_ie(src, 
                          &pos,
                          &buf_len_in_bits,
                          &pdn_conn_req->pdn_data.hc_config,&dummy_buf_len);
    }
  }
#endif
} /* end decode_pdn_conn_req() */


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
boolean decode_pdn_disconnect_rej
(
  byte                           *src,
  word                           *pos,
  word                           *buffer_length,
  lte_nas_esm_pdn_disconnect_rej *pdn_disconnect_rej,
  word                           *esm_len_in_bits
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  boolean valid_ie = TRUE;
  word temp_esm_rem_len = 0;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif

  if(*buffer_length < (NAS_ESM_PDN_DISCONNECT_REJ - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode PDN DISCONNECT REJECT",
                   *buffer_length);
    *buffer_length = 0;
    *esm_len_in_bits = 0;
    return FALSE;
  }

  /***** ESM cause IE *****/
  num_bits_to_read = 8;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);

  populate_esm_cause(&pdn_disconnect_rej->esm_cause.esm_cause, unpackedbytes);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
  pdn_disconnect_rej->esm_cause.valid = TRUE;


                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif
  
  while(*esm_len_in_bits)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      *esm_len_in_bits = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    if (ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits) == FALSE)
    {
      *buffer_length = 0;
      *esm_len_in_bits = 0;
       MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= buffer wraparound");
       return TRUE;
    }

    /* IE length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /* Store offset address & remaining buffer length, in case any octet
    is faulty this info would be required to jump to next IEI */
    iei_pos = *pos + 8;
    iei_buffer_len = *buffer_length - 8;
    temp_esm_rem_len = *esm_len_in_bits - 8;

    switch(iei_val)
    {
      /***** Protocol configuration options IE *****/
      case PROT_CONFIG_OPT_IEI:
        if(opt_iei_index == 1)
        {
          valid_ie = decode_prot_conf_opt(src,
                                          pos,
                                          buffer_length,
                                          &pdn_disconnect_rej->protocol_conf_opt,
                                          esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            pdn_disconnect_rej->protocol_conf_opt.valid = FALSE;
          }
          else
          {
            pdn_disconnect_rej->protocol_conf_opt.valid = TRUE;
          }
          opt_iei_index = 2;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated PCO IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
      /***** Ext Protocol configuration options IE *****/
      case EXT_PROT_CONFIG_OPT_IEI:
        if(opt_iei_index == 2)
        {
          valid_ie = decode_ext_prot_conf_opt(src,
                                              pos,
                                              buffer_length,
                                              &pdn_disconnect_rej->ext_protocol_conf_opt,
                                              esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            pdn_disconnect_rej->ext_protocol_conf_opt.valid = FALSE; 
          }
          else
          {
            pdn_disconnect_rej->ext_protocol_conf_opt.valid = TRUE;
          }
          opt_iei_index = 3;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated EPCO IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
      default:
        /* Skip unknown IEs, 24.301 - 7.6.1 */
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received unknown IEI = %d", iei_val);
        skip_unknown_ie = TRUE;
        valid_ie = FALSE;
        break;
    } /* end switch(iei_val) */

    /* Check to see if the IE is faulty, if so move 
       to next IE by incremement offset address and 
       decrementing available buffer length */
    if(valid_ie == FALSE)
    {
      if(skip_unknown_ie == TRUE)
      {
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, TRUE, esm_len_in_bits);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
        *esm_len_in_bits = 0;
      }
      else
      {
        if(iei_buffer_len > (iei_len*8))
        {
          *buffer_length = iei_buffer_len - iei_len*8;
          *pos = iei_pos + iei_len*8;
          if(temp_esm_rem_len > (iei_len*8))
          {
            *esm_len_in_bits = temp_esm_rem_len - iei_len*8;
          }
          else
          {
            *esm_len_in_bits = 0;
          }
        }
        else
        {
          *buffer_length = 0;
          *esm_len_in_bits = 0;
        }
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*esm_len_in_bits) */

  return TRUE;
} /* end decode_pdn_disconnect_rej() */


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
boolean decode_bearer_modify_context_rej
(
  byte                                   *src,
  word                                   *pos,
  word                                   *buffer_length,
  lte_nas_esm_bearer_resource_modifi_rej *bearer_resource_modifi_rej,
  word                                   *esm_len_in_bits
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  boolean valid_ie = TRUE;
  word temp_esm_rem_len = 0;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif

#ifdef FEATURE_LTE_REL10
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  
#endif


  if(*buffer_length < (NAS_ESM_BEARER_RES_MODIFI_REJ_MIN_LEN - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode BEARER RESOURCE MODIFICATION REJECT",
                   *buffer_length);
    *buffer_length = 0;
    *esm_len_in_bits = 0;
    return FALSE;
  }
  
  /***** ESM cause IE *****/
  num_bits_to_read = 8;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);

  populate_esm_cause(&bearer_resource_modifi_rej->esm_cause.esm_cause, unpackedbytes);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
  bearer_resource_modifi_rej->esm_cause.valid = TRUE;

#ifndef FEATURE_LTE_REL12
  if (bearer_resource_modifi_rej->esm_cause.esm_cause == NAS_ESM_INSUFFICIENT_RESOURCES)
  {
    bearer_resource_modifi_rej->backoff_timer.active = TRUE;
    bearer_resource_modifi_rej->backoff_timer.timer_count = 0;
  }
#endif
                         /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*esm_len_in_bits)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      *esm_len_in_bits = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    if (ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits) == FALSE)
    {
      *buffer_length = 0;
      *esm_len_in_bits = 0;
       MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= buffer wraparound");
       return TRUE;
    }
 

    /* IE length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /* Store offset address & remaining buffer length, in case any octet
    is faulty this info would be required to jump to next IEI */
    iei_pos = *pos + 8;
    iei_buffer_len = *buffer_length - 8;
    temp_esm_rem_len = *esm_len_in_bits - 8;

    switch(iei_val)
    {
      /***** Protocol configuration options IE *****/
      case PROT_CONFIG_OPT_IEI:
        if(opt_iei_index == 1)
        {
          valid_ie = decode_prot_conf_opt(src,
                                          pos,
                                          buffer_length,
                                          &bearer_resource_modifi_rej->protocol_conf_opt,
                                          esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            bearer_resource_modifi_rej->protocol_conf_opt.valid = FALSE;
          }
          else
          {
            bearer_resource_modifi_rej->protocol_conf_opt.valid = TRUE; 
          }
          opt_iei_index = 2;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated PCO IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

#ifdef FEATURE_LTE_REL10
      /***** T3396 value IE *****/
      case T3396_VALUE_IEI:
        if(opt_iei_index <= 2)
        {
          if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
          {
            if (emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL12)
            {
               if(bearer_resource_modifi_rej->esm_cause.esm_cause == NAS_ESM_INSUFFICIENT_RESOURCES)
               {
                 valid_ie = decode_t3396_value(src,
                                               pos,
                                               buffer_length,
                                               &bearer_resource_modifi_rej->backoff_timer,
                                               esm_len_in_bits);
               }
               else
               {
                /* This IE shall not be included when ESM Cause is not
                   NAS_ESM_INSUFFICIENT_RESOURCES(#26). Skip decoding of this IE */
                 valid_ie = FALSE;
               }
            }
            else
            {
               valid_ie = decode_t3396_value(src,
                                             pos,
                                             buffer_length,
                                             &bearer_resource_modifi_rej->backoff_timer,
                                             esm_len_in_bits);
            }
            }
            else
            {
              MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3396 value IE unsupported in UE rel %d",
                         emm_ctrl_data_ptr->emm_3gpp_spec_ver);
              valid_ie = FALSE;
            }
          opt_iei_index = 3;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated T3396 value IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif

#ifdef FEATURE_LTE_REL12
			/***** Re-attempt Indicator IE *****/
	 case REATTEMPT_INDICATOR_IEI:
		 if(opt_iei_index <= 3)
		 {
			if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL12)
			{
			  valid_ie = decode_reattempt_ind_value(src,
														pos,
														buffer_length,
														&bearer_resource_modifi_rej->reattempt_indicator,
														esm_len_in_bits);
				  
			}
          else
          {
			 MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Reattempt Indicator IE unsupported in UE rel %d",
                         emm_ctrl_data_ptr->emm_3gpp_spec_ver);
             valid_ie = FALSE;
          }
          opt_iei_index = 4;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Re-Attempt Indicator value IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif

      /***** EXT Protocol configuration options IE *****/
      case EXT_PROT_CONFIG_OPT_IEI:
        if(opt_iei_index <= 4)
        {
          valid_ie = decode_ext_prot_conf_opt(src,
                                              pos,
                                              buffer_length,
                                              &bearer_resource_modifi_rej->ext_protocol_conf_opt, 
                                              esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            bearer_resource_modifi_rej->ext_protocol_conf_opt.valid = FALSE;
          }
          else
          {
            bearer_resource_modifi_rej->ext_protocol_conf_opt.valid = TRUE; 
          }
          opt_iei_index = 5;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated EPCO IE, opt_iei_idx = %d",
                       opt_iei_index);
          valid_ie = FALSE;
        }
        break;


      default:
        /* Skip unknown IEs, 24.301 - 7.6.1 */
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received unknown IEI = %d", iei_val);
        skip_unknown_ie = TRUE;
        valid_ie = FALSE;
        break;
    } /* end switch(iei_val) */

    /* Check to see if the IE is faulty, if so move 
       to next IE by incremement offset address and 
       decrementing available buffer length */
    if(valid_ie == FALSE)
    {
      if(skip_unknown_ie == TRUE)
      {
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, TRUE, esm_len_in_bits);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
        *esm_len_in_bits = 0;
      }
      else
      {

        if(iei_buffer_len > (iei_len*8))
        {
          *buffer_length = iei_buffer_len - iei_len*8;
          *pos = iei_pos + iei_len*8;
          if(temp_esm_rem_len > (iei_len*8))
          {
            *esm_len_in_bits = temp_esm_rem_len - iei_len*8;
          }
          else
          {
            *esm_len_in_bits = 0;
          }
        }
        else
        {
          *buffer_length = 0;
          *esm_len_in_bits = 0;
        }

        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*esm_len_in_bits) */

  return TRUE;
} /* end decode_bearer_modify_context_rej() */


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
boolean decode_modify_eps_context_req
(
  byte                                   *src,
  word                                   *pos, 
  word                                   *buffer_length,   
  lte_nas_esm_modify_eps_bearer_ctxt_req *modify_bearer_ctxt_req,
  word                                   *esm_len_in_bits
)
{
  byte num_bits_to_read = 0;
  boolean valid_ie = TRUE;
  word temp_esm_rem_len = 0;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;
  boolean buffer_end = FALSE;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif


  /********************************************************************
   MODIFY BEARER CONTEXT REQUEST shall contain 3 octets minimum, 
   reaching this stage implies that 3 octets have already been parsed.
  *********************************************************************/

                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*esm_len_in_bits)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      *esm_len_in_bits = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    if (ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits) == FALSE)
    {
        /* End of the buffer detected. Allow execution for partial IEIs */
        if(RADIO_PRIORITY_IEI == (byte)(0xF0 & iei_val))
        {
            buffer_end = TRUE;
        }
        else
        {
            *buffer_length = 0;
            *esm_len_in_bits = 0;
             MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= buffer wraparound");
             return TRUE;
         }
    }

    if (FALSE == buffer_end)
    {
        /* IE length */
        iei_len = b_unpackb(src, *pos, num_bits_to_read);

        /* Store offset address & remaining buffer length, in case any octet
           is faulty this info would be required to jump to next IEI       */
       iei_pos = *pos + 8;
       iei_buffer_len = *buffer_length - 8;
       temp_esm_rem_len = *esm_len_in_bits - 8;
    }

    switch(iei_val)
    {
      /***** New EPS QoS IE *****/
      case EPS_QOS_IEI:
        if(opt_iei_index == 1)
        {
          valid_ie = decode_eps_qos(src,
                                    pos,
                                    buffer_length,
                                    &modify_bearer_ctxt_req->sdf_qos,
                                    esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            modify_bearer_ctxt_req->sdf_qos.valid = FALSE;
          }
          else
          {
            modify_bearer_ctxt_req->sdf_qos.valid = TRUE;
          }
          opt_iei_index = 2;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated EPS QoS IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      /***** Traffic flow template IE *****/
      case TFT_IEI:
        if(opt_iei_index <= 2)
        {
          valid_ie = decode_tft(src,
                                pos,
                                buffer_length,
                                &modify_bearer_ctxt_req->tft,
                                esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            modify_bearer_ctxt_req->tft.valid = FALSE;
          }
          else
          {
            modify_bearer_ctxt_req->tft.valid = TRUE;
          }
          opt_iei_index = 3;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated TFT IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

#ifdef FEATURE_NAS_GW
      /***** New QoS IE *****/
      case NEGOTIATED_QOS_IEI:
        if(opt_iei_index <= 3)
        {
          valid_ie = decode_qos(src,
                                pos,
                                buffer_length,
                                &modify_bearer_ctxt_req->neg_qos,
                                esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            modify_bearer_ctxt_req->neg_qos.valid = FALSE;
          }
          else
          {
            modify_bearer_ctxt_req->neg_qos.valid = TRUE;
          }
          opt_iei_index = 4;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated QoS IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif

      /***** LLC service access point identifier IE *****/
      case NEGOTIATED_LLC_SAPI_IEI:
#ifdef FEATURE_NAS_GW
        if(opt_iei_index <= 4)
        {
          /***** 4 Spare bits *****/
          num_bits_to_read = 4;
          if (ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits) == FALSE)
          {
              *buffer_length = 0;
              *esm_len_in_bits = 0;
               MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= buffer wraparound");
               return TRUE;
          }
 

          /***** LLC SAPI *****/
          modify_bearer_ctxt_req->neg_llc_sapi.sapi = b_unpackb(src, *pos, num_bits_to_read);
          (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
          modify_bearer_ctxt_req->neg_llc_sapi.valid = TRUE;
          valid_ie = TRUE;
          opt_iei_index = 5;
        }
        else
#endif
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Negotiated LLC SAPI IE, opt_iei_idx = %d",
                         opt_iei_index);

          /* Negotiated LLC SAPI IE is of 2 octets and format TV. It doesn't have Length part
             so it cannot be skipped correctly after the switch block. Hence set valid_ie to
             TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = 8;
          (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
        }
        break;

#ifdef FEATURE_NAS_GW
      /***** Packet flow identifier IE *****/
      case PACKET_FLOW_IDENTIFIER_IEI:
        if(opt_iei_index <= 6)
        {
          valid_ie = decode_packet_flow_identifier(src,
                                                   pos,
                                                   buffer_length,
                                                   &modify_bearer_ctxt_req->pack_flo_id,
                                                   esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            modify_bearer_ctxt_req->pack_flo_id.valid = FALSE;
          }
          else
          {
            modify_bearer_ctxt_req->pack_flo_id.valid = TRUE;
          }
          opt_iei_index = 7;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Packet flow identifier IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif

      /***** APN aggregate maximum bit rate IE *****/
      case APN_AMBR_IEI:
        if(opt_iei_index <= 7)
        {
          valid_ie = decode_apn_ambr(src,
                                     pos,
                                     buffer_length,
                                     &modify_bearer_ctxt_req->apn_ambr,
                                     esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            modify_bearer_ctxt_req->apn_ambr.valid = FALSE;
          }
          else
          {
            modify_bearer_ctxt_req->apn_ambr.valid = TRUE;
          }
          opt_iei_index = 8;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated APN-AMBR IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      /***** Protocol configuration options IE *****/
      case PROT_CONFIG_OPT_IEI:
        if(opt_iei_index <= 8)
        {
          valid_ie = decode_prot_conf_opt(src,
                                          pos,
                                          buffer_length,
                                          &modify_bearer_ctxt_req->protocol_conf_opt,
                                          esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            modify_bearer_ctxt_req->protocol_conf_opt.valid = FALSE;
          }
          else
          {
            modify_bearer_ctxt_req->protocol_conf_opt.valid = TRUE; 
          }
          opt_iei_index = 9;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated PCO IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
      case HC_CONFIG_IEI:
#ifdef FEATURE_CIOT
        if(opt_iei_index <= 11)
        {
          valid_ie = decode_hc_config_ie(src,
                                    pos,
                                    buffer_length,
                                    &modify_bearer_ctxt_req->hc_config,
                                    esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            modify_bearer_ctxt_req->hc_config.valid = FALSE;
          }
          else
          {
            modify_bearer_ctxt_req->hc_config.valid = TRUE;
          }
          opt_iei_index = 12;
        }
        else
#endif
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated HC CONFIG IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      /***** EXT Protocol configuration options IE *****/
      case EXT_PROT_CONFIG_OPT_IEI:
        if(opt_iei_index <= 12)
        {
          valid_ie = decode_ext_prot_conf_opt(src,
                                              pos,
                                              buffer_length,
                                              &modify_bearer_ctxt_req->ext_protocol_conf_opt, 
                                              esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            modify_bearer_ctxt_req->ext_protocol_conf_opt.valid = FALSE;
          }
          else
          {
            modify_bearer_ctxt_req->ext_protocol_conf_opt.valid = TRUE; 
          }
          opt_iei_index = 13;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated EPCO IE, opt_iei_idx = %d",
                       opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      default:
        /***** Radio priority IE *****/
        /* IEI value is present in bits 5-8 of the octet that is already read.
           So just mask the bits to get the value */
        if(RADIO_PRIORITY_IEI == (byte)(0xF0 & iei_val))
        {
#ifdef FEATURE_NAS_GW
          if(opt_iei_index <= 5)
          {
            /* Radio priority level value is present in bits 1-3 of the octet
               that is already read. So just mask the bits to get the value */
            modify_bearer_ctxt_req->radio_priority.radio_priority = (byte)(0x07 & iei_val);
            modify_bearer_ctxt_req->radio_priority.valid = TRUE;
            valid_ie = TRUE;
            opt_iei_index = 6;
          }
          else
#endif
          {
            /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Radio priority IE, opt_iei_idx = %d",
                           opt_iei_index);
            /* Radio Priority IE is of 1 byte and format TV. It doesn't have Length part so
             it cannot be skipped correctly after the switch block. Hence set valid_ie to
             TRUE. iei_val is already read so no need to adjust buffer length and position */
            valid_ie = TRUE;
          }
        }
        else
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received unknown IEI = %d", iei_val);
          skip_unknown_ie = TRUE;
          valid_ie = FALSE;
        }
        break;
      } /* end switch(iei_val) */

      /* Check to see if the IE is faulty, if so move 
         to next IE by incremement offset address and 
         decrementing available buffer length */
      if(valid_ie == FALSE)
      {
        if(skip_unknown_ie == TRUE)
        {
          msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, TRUE, esm_len_in_bits);
          skip_unknown_ie = FALSE;
          valid_ie = TRUE;
        }
        else if(buffer_length_check_failed == TRUE)
        {
          MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                         (*buffer_length/8), iei_len);
          *buffer_length = 0;
          *esm_len_in_bits = 0;
        }
        else
        {
          if(iei_buffer_len > (iei_len*8))
          {
            *buffer_length = iei_buffer_len - iei_len*8;
            *pos = iei_pos + iei_len*8;
            if(temp_esm_rem_len > (iei_len*8))
            {
              *esm_len_in_bits = temp_esm_rem_len - iei_len*8;
            }
            else
            {
              *esm_len_in_bits = 0;
            }
          }
          else
          {
            *buffer_length = 0;
            *esm_len_in_bits = 0;
          }
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
          valid_ie = TRUE;
        }
      } /* end if(valid_ie == FALSE) */
  } /* end while(*esm_len_in_bits) */

  return TRUE;
} /* end decode_modify_eps_context_req() */


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
boolean decode_bearer_resource_alloc_rej
(
  byte                                       *src,
  word                                       *pos,
  word                                       *buffer_length,
  lte_nas_esm_bearer_resource_allocation_rej *bearer_resource_alloc_rej,
  word                                       *esm_len_in_bits
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  boolean valid_ie = TRUE;
  word temp_esm_rem_len = 0;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif

#ifdef FEATURE_LTE_REL10
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  
#endif

  if(*buffer_length < (NAS_ESM_BEARER_RES_ALLOC_REJ_MIN_LEN - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode BEARER RESOURCE ALLOCATION REJECT",
                   *buffer_length);
    *buffer_length = 0;
    *esm_len_in_bits = 0;
    return FALSE;
  }
  
  /***** ESM cause IE *****/
  num_bits_to_read = 8;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);

  populate_esm_cause(&bearer_resource_alloc_rej->esm_cause.esm_cause, unpackedbytes);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
  bearer_resource_alloc_rej->esm_cause.valid = TRUE;

#ifndef FEATURE_LTE_REL12
  if (bearer_resource_alloc_rej->esm_cause.esm_cause == NAS_ESM_INSUFFICIENT_RESOURCES)
  {
    bearer_resource_alloc_rej->backoff_timer.active = TRUE;
    bearer_resource_alloc_rej->backoff_timer.timer_count = 0;
  }
#endif
                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*esm_len_in_bits)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      *esm_len_in_bits = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    if (ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits) == FALSE)
    {
      *buffer_length = 0;
      *esm_len_in_bits = 0;
       MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= buffer wraparound");
       return TRUE;
    }
 

    /* IE length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /* Store offset address & remaining buffer length, in case any octet 
    is faulty this info would be required to jump to next IEI */
    iei_pos = *pos + 8;
    iei_buffer_len = *buffer_length - 8;
    temp_esm_rem_len = *esm_len_in_bits - 8;

    switch(iei_val)
    {
      /***** Protocol configuration options IE *****/
      case PROT_CONFIG_OPT_IEI:
        if(opt_iei_index == 1)
        {
          valid_ie = decode_prot_conf_opt(src,
                                          pos,
                                          buffer_length,
                                          &bearer_resource_alloc_rej->protocol_conf_opt,
                                          esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            bearer_resource_alloc_rej->protocol_conf_opt.valid = FALSE;
          }
          else
          {
            bearer_resource_alloc_rej->protocol_conf_opt.valid = TRUE;
          }
          opt_iei_index = 2;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated PCO IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

#ifdef FEATURE_LTE_REL10
      /***** T3396 value IE *****/
      case T3396_VALUE_IEI:
        if(opt_iei_index <= 2)
        {
          if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
          {
            if (emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL12)
            {
               if(bearer_resource_alloc_rej->esm_cause.esm_cause == NAS_ESM_INSUFFICIENT_RESOURCES)
               {
                 valid_ie = decode_t3396_value(src,
                                               pos,
                                               buffer_length,
                                               &bearer_resource_alloc_rej->backoff_timer,
                                               esm_len_in_bits);
               }
               else
               {
                /* This IE shall not be included when ESM Cause is not
                   NAS_ESM_INSUFFICIENT_RESOURCES(#26). Skip decoding of this IE */
                 valid_ie = FALSE;
               }
            }
            else
            {
              valid_ie = decode_t3396_value(src,
                                            pos,
                                            buffer_length,
                                            &bearer_resource_alloc_rej->backoff_timer,
                                            esm_len_in_bits);
            }
          }
          else
          {
            MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3396 value IE unsupported in UE rel %d",
                         emm_ctrl_data_ptr->emm_3gpp_spec_ver);
            valid_ie = FALSE;
          }
          opt_iei_index = 3;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated T3396 value IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif

#ifdef FEATURE_LTE_REL12
			/***** Re-attempt Indicator IE *****/
	  case REATTEMPT_INDICATOR_IEI:
		if(opt_iei_index <= 3)
		{
		  if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL12)
		  {
				  valid_ie = decode_reattempt_ind_value(src,
														pos,
														buffer_length,
														&bearer_resource_alloc_rej->reattempt_indicator,
														esm_len_in_bits);
				  
		  }
          else
          {
				  MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Reattempt Indicator IE unsupported in UE rel %d",
                         emm_ctrl_data_ptr->emm_3gpp_spec_ver);
            valid_ie = FALSE;
          }
          opt_iei_index = 4;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Re-Attempt Indicator value IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif

      /***** EXT Protocol configuration options IE *****/
      case EXT_PROT_CONFIG_OPT_IEI:
        if(opt_iei_index <= 5)
        {
          valid_ie = decode_ext_prot_conf_opt(src,
                                              pos,
                                              buffer_length,
                                              &bearer_resource_alloc_rej->ext_protocol_conf_opt, 
                                              esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            bearer_resource_alloc_rej->ext_protocol_conf_opt.valid = FALSE;
          }
          else
          {
            bearer_resource_alloc_rej->ext_protocol_conf_opt.valid = TRUE; 
          }
          opt_iei_index = 6;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated EPCO IE, opt_iei_idx = %d",
                       opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      default:
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received unknown IEI = %d", iei_val);
        skip_unknown_ie = TRUE;
        valid_ie = FALSE;
        break;
    } /* end switch(iei_val) */

    /* Check to see if the IE is faulty, if so move
       to next IE by incremement offset address and
       decrementing available buffer length */
    if(valid_ie == FALSE)
    {
      if(skip_unknown_ie == TRUE)
      {
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, TRUE, esm_len_in_bits);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
        *esm_len_in_bits = 0;
      }
      else
      {
        if(iei_buffer_len > (iei_len*8))
        {
          *buffer_length = iei_buffer_len - iei_len*8;
          *pos = iei_pos + iei_len*8;
          if(temp_esm_rem_len > (iei_len*8))
          {
            *esm_len_in_bits = temp_esm_rem_len - iei_len*8;
          }
          else
          {
            *esm_len_in_bits = 0;
          }
        }
        else
        {
          *buffer_length = 0;
          *esm_len_in_bits = 0;
        }
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*esm_len_in_bits) */

  return TRUE;
} /* end decode_bearer_resource_alloc_rej() */


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
boolean decode_deactivate_eps_bearer_req
(
   byte                                     *src,
   word                                     *pos, 
   word                                     *buffer_length,
   lte_nas_esm_deact_eps_bearer_context_req *deactivate_bearer_context_req,
   word                                     *esm_len_in_bits
 )
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  boolean valid_ie = TRUE;
  word temp_esm_rem_len = 0;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif

#ifdef FEATURE_LTE_REL11
    emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    
#endif

  if(*buffer_length < (NAS_ESM_DEACT_BEARER_REQ_MIN_LEN - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode DEACTIVATE BEARER CONTEXT REQUEST",
                   *buffer_length);
    *buffer_length = 0;
    *esm_len_in_bits = 0;
    return FALSE;
  }
  
  /***** ESM cause IE *****/
  num_bits_to_read = 8;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);

  populate_esm_cause(&deactivate_bearer_context_req->esm_cause.esm_cause, unpackedbytes);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
  deactivate_bearer_context_req->esm_cause.valid = TRUE;

#ifndef FEATURE_LTE_REL12
  if (deactivate_bearer_context_req->esm_cause.esm_cause == NAS_ESM_INSUFFICIENT_RESOURCES)
  {
#ifdef FEATURE_LTE_REL11
    deactivate_bearer_context_req->backoff_timer.active = TRUE;
#else
    deactivate_bearer_context_req->backoff_timer.active = FALSE;
#endif
    deactivate_bearer_context_req->backoff_timer.timer_count = 0;
  }
#endif
                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*esm_len_in_bits)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      *esm_len_in_bits = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);    
    if (ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits) == FALSE)
    {
      *buffer_length = 0;
      *esm_len_in_bits = 0;
       MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= buffer wraparound");
       return TRUE;
    }
 

    /* IE length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /* Store offset address & remaining buffer length, in case any octet 
    is faulty this info would be required to jump to next IEI */
    iei_pos = *pos + 8;
    iei_buffer_len = *buffer_length - 8;
    temp_esm_rem_len = *esm_len_in_bits - 8;

    switch(iei_val)
    {
      /***** Protocol configuration options *****/
      case PROT_CONFIG_OPT_IEI:
        if(opt_iei_index == 1)
        {
          valid_ie = decode_prot_conf_opt(src,
                                          pos,
                                          buffer_length,
                                          &deactivate_bearer_context_req->protocol_conf_opt,
                                          esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            deactivate_bearer_context_req->protocol_conf_opt.valid = FALSE;
          }
          else
          {
            deactivate_bearer_context_req->protocol_conf_opt.valid = TRUE;
          }
          opt_iei_index = 2;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated PCO IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

#ifdef FEATURE_LTE_REL11
      /***** T3396 value IE *****/
      case T3396_VALUE_IEI:
        if(opt_iei_index <= 2)
        {
          if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)
          {
            if (emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL12)
            {
               if(deactivate_bearer_context_req->esm_cause.esm_cause == NAS_ESM_INSUFFICIENT_RESOURCES)
               {
                 valid_ie = decode_t3396_value(src,
                                               pos,
                                               buffer_length,
                                               &deactivate_bearer_context_req->backoff_timer,
                                               esm_len_in_bits);
               }
               else
               {
                /* This IE shall not be included when ESM Cause is not
                   NAS_ESM_INSUFFICIENT_RESOURCES(#26). Skip decoding of this IE */
                 valid_ie = FALSE;
               }
            }
            else
            {
               valid_ie = decode_t3396_value(src,
                                             pos,
                                             buffer_length,
                                             &deactivate_bearer_context_req->backoff_timer,
                                             esm_len_in_bits);
            }
          }
          else
          {
            MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3396 value IE unsupported in UE rel %d",
                          emm_ctrl_data_ptr->emm_3gpp_spec_ver);
            valid_ie = FALSE;
          }
          opt_iei_index = 3;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated T3396 value IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }

        deactivate_bearer_context_req->backoff_timer.valid = valid_ie;
        break;
#endif

      /***** Ext Protocol configuration options *****/
      case EXT_PROT_CONFIG_OPT_IEI:
        if(opt_iei_index == 5)
        {
          valid_ie = decode_ext_prot_conf_opt(src,
                                              pos,
                                              buffer_length,
                                              &deactivate_bearer_context_req->ext_protocol_conf_opt, 
                                              esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            deactivate_bearer_context_req->ext_protocol_conf_opt.valid = FALSE; 
          }
          else
          {
            deactivate_bearer_context_req->ext_protocol_conf_opt.valid = TRUE;
          }
          opt_iei_index = 6;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated EPCO IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
      default:
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received unknown IEI = %d", iei_val);
        skip_unknown_ie = TRUE;
        valid_ie = FALSE;
        break;
    } /* end switch(iei_val) */

    /* Check to see if the IE is faulty, if so move
       to next IE by incremement offset address and
       decrementing available buffer length */
    if(valid_ie == FALSE)
    {
      if(skip_unknown_ie == TRUE)
      {
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, TRUE, esm_len_in_bits);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
        *esm_len_in_bits = 0;
      }
      else
      {
        if(iei_buffer_len > (iei_len*8))
        {
          *buffer_length = iei_buffer_len - iei_len*8;
          *pos = iei_pos + iei_len*8;
          if(temp_esm_rem_len > (iei_len*8))
          {
            *esm_len_in_bits = temp_esm_rem_len - iei_len*8;
          }
          else
          {
            *esm_len_in_bits = 0;
          }
        }
        else
        {
          *buffer_length = 0;
          *esm_len_in_bits = 0;
        }
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*esm_len_in_bits) */

  return TRUE;
} /* end decode_deactivate_eps_bearer_req() */


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
boolean decode_dedicated_eps_bearer_req
(
  byte                                         *src,
  word                                         *pos,
  word                                         *buffer_length,
  lte_nas_esm_act_dedicated_bearer_context_req *dedicated_bearer_context_req,
  word                                         *esm_len_in_bits
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  boolean valid_ie = TRUE;
  word temp_esm_rem_len = 0;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;
  boolean buffer_end = FALSE;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif

  ASSERT(src != NULL);
  ASSERT(pos != NULL);
  ASSERT(buffer_length != NULL);
  ASSERT(dedicated_bearer_context_req != NULL);
  ASSERT(esm_len_in_bits != NULL);

  if(*buffer_length < (NAS_ESM_ACT_DED_REQ_MIN_LEN - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode ACTIVATE DEDICATED BEARER CONTEXT REQUEST",
                   *buffer_length);
    *buffer_length = 0;
    *esm_len_in_bits = 0;
    return FALSE;
  }
  

  
  /****** 4 Spare bits + 4 bits Linked EPS bearer identity IE *****/
  num_bits_to_read = 4;
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
  if(unpackedbytes <= LINKED_EPS_BEARER_RESERVED_VALUE)
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Linked EBI = %d is reserved", unpackedbytes);
    return FALSE;
  }
  dedicated_bearer_context_req->linked_eps_bearer_identity = unpackedbytes;
  if (ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits) == FALSE)
    {
      *buffer_length = 0;
      *esm_len_in_bits = 0;
     MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= buffer wraparound");
     return TRUE;
    }
 

  /***** EPS quality of service IE *****/
  valid_ie = decode_eps_qos(src,
                            pos,
                            buffer_length,
                            &dedicated_bearer_context_req->sdf_qos,
                            esm_len_in_bits);
  if(valid_ie == FALSE)
  {
    dedicated_bearer_context_req->sdf_qos.valid = FALSE;
    return FALSE;
  }
  dedicated_bearer_context_req->sdf_qos.valid = TRUE;


  /***** Traffic flow template IE *****/
  valid_ie = decode_tft(src,
                        pos,
                        buffer_length,
                        &dedicated_bearer_context_req->tft,
                        esm_len_in_bits);
  if(valid_ie == FALSE)
  {
    dedicated_bearer_context_req->tft.valid = FALSE;
    return FALSE;
  }
  dedicated_bearer_context_req->tft.valid = TRUE;


                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*esm_len_in_bits)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      *esm_len_in_bits = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    if (ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits) == FALSE)
    {
         /* End of the buffer detected allow execution for half-octet IEIs*/
         if(RADIO_PRIORITY_IEI == (byte)(0xF0 & iei_val))
         {
             buffer_end = TRUE;
         }
         else
         {
            *buffer_length = 0;
            *esm_len_in_bits = 0;
             MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= buffer wraparound");
             return TRUE;
         }
    }

    if (FALSE == buffer_length)
    {
       /* IE length */
       iei_len = b_unpackb(src, *pos, num_bits_to_read);

       /*Store offset address & remaining buffer length, in case any octet 
         is faulty this info would be required to jump to next IEI*/
       iei_pos = *pos + 8;
       iei_buffer_len = *buffer_length - 8;
       temp_esm_rem_len = *esm_len_in_bits - 8;
    }

    switch(iei_val)
    {
#ifdef FEATURE_NAS_GW
      /***** Transaction Identifier IE *****/
      case TRANSACTION_IDENTIFIER_IEI:
        if(opt_iei_index == 1)
        {
          valid_ie = decode_transaction_id(src,
                                           pos,
                                           buffer_length,
                                           &dedicated_bearer_context_req->ti,
                                           esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            dedicated_bearer_context_req->ti.valid = FALSE;
          }
          else
          {
            dedicated_bearer_context_req->ti.valid = TRUE;
          }
          opt_iei_index = 2;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Transaction identifier IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      /***** Negotiated QoS IE *****/
      case NEGOTIATED_QOS_IEI:
        if(opt_iei_index <= 2)
        {
          valid_ie = decode_qos(src,
                                pos,
                                buffer_length,
                                &dedicated_bearer_context_req->neg_qos,
                                esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            dedicated_bearer_context_req->neg_qos.valid = FALSE;
          }
          else
          {
            dedicated_bearer_context_req->neg_qos.valid = TRUE;
          }
          opt_iei_index = 3;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated QoS IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif

      /***** Negotiated LLC SAPI IE *****/
      case NEGOTIATED_LLC_SAPI_IEI:
#ifdef FEATURE_NAS_GW
        if(opt_iei_index <= 3)
        {
          /***** 4 Spare bits *****/
          num_bits_to_read = 4;
          if (ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits) == FALSE)
          {
            *buffer_length = 0;
            *esm_len_in_bits = 0;
             MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= buffer wraparound");
             return TRUE;
          }
 

          /***** LLC SAPI value *****/
          dedicated_bearer_context_req->neg_llc_sapi.sapi = b_unpackb(src, *pos, num_bits_to_read);
          ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits);

          dedicated_bearer_context_req->neg_llc_sapi.valid = TRUE;
          valid_ie = TRUE;
          opt_iei_index = 4;
        }
        else
#endif
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Negotiated LLC SAPI IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* Negotiated LLC SAPI IE is of 2 octets and format Type TV. It doesn't have Length
             part so it cannot be skipped correctly after the switch block. Hence set valid_ie
             to TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = 8;
          (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
        }
        break;

#ifdef FEATURE_NAS_GW
      /***** Packet flow Identifier IE *****/
      case PACKET_FLOW_IDENTIFIER_IEI:
        if(opt_iei_index <= 5)
        {
          valid_ie = decode_packet_flow_identifier(src,
                                                   pos,
                                                   buffer_length,
                                                   &dedicated_bearer_context_req->pack_flo_id,
                                                   esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            dedicated_bearer_context_req->pack_flo_id.valid = FALSE;
          }
          else
          {
            dedicated_bearer_context_req->pack_flo_id.valid = TRUE;
          }
          opt_iei_index = 6;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Packet flow identifier IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif

      /***** Protocol configuration options IE *****/
      case PROT_CONFIG_OPT_IEI:
        if(opt_iei_index <= 6)
        {
          valid_ie = decode_prot_conf_opt(src,
                                          pos,
                                          buffer_length,
                                          &dedicated_bearer_context_req->protocol_conf_opt,
                                          esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            dedicated_bearer_context_req->protocol_conf_opt.valid = FALSE;
          }
          else
          {
            dedicated_bearer_context_req->protocol_conf_opt.valid = TRUE;
          }
          opt_iei_index = 7;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated PCO IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      /***** EXT Protocol configuration options IE *****/
      case EXT_PROT_CONFIG_OPT_IEI:
        if(opt_iei_index <= 9)
        {
          valid_ie = decode_ext_prot_conf_opt(src,
                                              pos,
                                              buffer_length,
                                              &dedicated_bearer_context_req->ext_protocol_conf_opt, 
                                              esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            dedicated_bearer_context_req->ext_protocol_conf_opt.valid = FALSE;
          }
          else
          {
            dedicated_bearer_context_req->ext_protocol_conf_opt.valid = TRUE; 
          }
          opt_iei_index = 10;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated EPCO IE, opt_iei_idx = %d",
                       opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      default:
        /***** Radio priority IE *****/
        /* IEI value is present in bits 5-8 of the octet that is already read.
           So just mask the bits to get the value */
        if(RADIO_PRIORITY_IEI == (byte)(0xF0 & iei_val))
        {
#ifdef FEATURE_NAS_GW
          if(opt_iei_index <= 4)
          {
            /* Radio priority level value is present in bits 1-3 of the octet
               that is already read. So just mask the bits to get the value */
            dedicated_bearer_context_req->radio_priority.radio_priority = (byte)(0x07 & iei_val);
            dedicated_bearer_context_req->radio_priority.valid = TRUE;
            valid_ie = TRUE;
            opt_iei_index = 5;
          }
          else
#endif
          {
            /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Radio Priority IE, opt_iei_idx = %d",
                           opt_iei_index);
            /* Radio Priority IE is of 1 byte and format TV. It doesn't have Length part so
             it cannot be skipped correctly after the switch block. Hence set valid_ie to
             TRUE. iei_val is already read so no need to adjust buffer length and position */
            valid_ie = TRUE;
          }
        }
        else
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received unknown IEI = %d", iei_val);
          skip_unknown_ie = TRUE;
          valid_ie = FALSE;
        }
        break;
    } /* end switch(iei_val) */

    /* Check to see if the IE is faulty, if so move
       to next IE by incremement offset address and
       decrementing available buffer length */
    if(valid_ie == FALSE)
    {
      if(skip_unknown_ie == TRUE)
      {
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, TRUE, esm_len_in_bits);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
        *esm_len_in_bits = 0;
      }
      else
      {
        if(iei_buffer_len > (iei_len*8))
        {
          *buffer_length = iei_buffer_len - iei_len*8;
          *pos = iei_pos + iei_len*8;
          if(temp_esm_rem_len > (iei_len*8))
          {
            *esm_len_in_bits = temp_esm_rem_len - iei_len*8;
          }
          else
          {
            *esm_len_in_bits = 0;
          }
        }
        else
        {
          *buffer_length = 0;
          *esm_len_in_bits = 0;
        }
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*esm_len_in_bits) */

  return TRUE;
} /* end decode_dedicated_eps_bearer_req() */


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
boolean decode_act_def_context_req
(
  byte                                       *src,
  word                                       *pos,
  word                                       *buffer_length,
  lte_nas_esm_act_default_bearer_context_req *def_bearer_context_req,
  word                                       *esm_len_in_bits
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  boolean valid_ie = TRUE;
  word temp_esm_rem_len = 0;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;
  boolean buffer_end = FALSE;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif

  if(*buffer_length < (NAS_ESM_ACT_DEF_REQ_MIN_LEN - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "Buffer length = %d is insufficient to decode ACTIVATE DEFAULT BEARER CONTEXT REQUEST",
                   *buffer_length);
    *buffer_length = 0;
    *esm_len_in_bits = 0;
    return FALSE;
  }
  
  /***** EPS quality of service IE *****/
  valid_ie = decode_eps_qos(src,
                            pos,
                            buffer_length,
                            &def_bearer_context_req->sdf_qos,
                            esm_len_in_bits);

  if(valid_ie == FALSE)
  {
    if(buffer_length_check_failed == TRUE)
    {
      *buffer_length = 0;
      *esm_len_in_bits = 0;
    }
    def_bearer_context_req->sdf_qos.valid = FALSE;
    return FALSE;
  }
  def_bearer_context_req->sdf_qos.valid = TRUE;
  
  /***** Access point name IE *****/
  valid_ie = decode_apn(src,
                        pos,
                        buffer_length,
                        &def_bearer_context_req->access_point_name,
                        esm_len_in_bits);
  if(valid_ie == FALSE)
  {
    if(buffer_length_check_failed == TRUE)
    {
      *buffer_length = 0;
      *esm_len_in_bits = 0;
    }
    def_bearer_context_req->access_point_name.valid = FALSE;
    return FALSE;
  }
  def_bearer_context_req->access_point_name.valid = TRUE;

  /***** PDN Address IE *****/
  valid_ie = decode_pdn_address(src,
                                pos,
                                buffer_length,
                                &def_bearer_context_req->pdn_address,
                                esm_len_in_bits);
  if(valid_ie == FALSE)
  {
    if(buffer_length_check_failed == TRUE)
    {
      *buffer_length = 0;
      *esm_len_in_bits = 0;
    }
    def_bearer_context_req->pdn_address.valid = FALSE;
    return FALSE;
  }
  def_bearer_context_req->pdn_address.valid = TRUE;


                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*esm_len_in_bits)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      *esm_len_in_bits = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    if (ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits) == FALSE)
    {
        /* End of the buffer detected, allow processing of half-octet IEIs */
        if(RADIO_PRIORITY_IEI == (byte)(0xF0 & iei_val))
        {
            buffer_end = TRUE;
        }
		else if(CP_ONLY_IND_IEI == (byte)(0xF0 & iei_val))
		{
		  buffer_end = TRUE;
		}
        else
        {
            *buffer_length = 0;
            *esm_len_in_bits = 0;
            MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= buffer wraparound");
            return TRUE;
        }    
    }    

    if (FALSE == buffer_end)
    {
       /* IE length */
       iei_len = b_unpackb(src, *pos, num_bits_to_read);

       /* Store offset address & remaining buffer length, in case any octet
        is faulty this info would be required to jump to next IEI     */
       iei_pos = *pos + 8; 
       iei_buffer_len = *buffer_length - 8;
       temp_esm_rem_len = *esm_len_in_bits - 8;
    }

    switch(iei_val)
    {
#ifdef FEATURE_NAS_GW
      /***** Transaction Identifier IE *****/
      case TRANSACTION_IDENTIFIER_IEI:
        if(opt_iei_index == 1)
        {
          valid_ie = decode_transaction_id(src,
                                           pos,
                                           buffer_length,
                                           &def_bearer_context_req->ti,
                                           esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            def_bearer_context_req->ti.valid = FALSE;
          }
          else
          {
            def_bearer_context_req->ti.valid = TRUE;
          }
          opt_iei_index = 2;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Transaction identifier IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      /***** Negotiated QOS IE *****/
      case NEGOTIATED_QOS_IEI:
        if(opt_iei_index <= 2)
        {
          valid_ie = decode_qos(src,
                                pos,
                                buffer_length, 
                                &def_bearer_context_req->neg_qos,
                                esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            def_bearer_context_req->neg_qos.valid = FALSE;
          }
          else
          {
            def_bearer_context_req->neg_qos.valid = TRUE; 
          }
          opt_iei_index = 3;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated QoS IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif

      /***** Negotiated LLC SAPI IE *****/
      case NEGOTIATED_LLC_SAPI_IEI:
#ifdef FEATURE_NAS_GW
        if(opt_iei_index <= 3)
        {
          /***** 4 Spare bits *****/
          num_bits_to_read = 4;
          if (ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits) == FALSE)
          {
            *buffer_length = 0;
            *esm_len_in_bits = 0;
             MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= buffer wraparound");
             return TRUE;
          }
 

          /***** LLC SAPI *****/
          def_bearer_context_req->neg_llc_sapi.sapi = b_unpackb(src, *pos, num_bits_to_read);
          def_bearer_context_req->neg_llc_sapi.valid = TRUE;
          (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
          valid_ie = TRUE;
          opt_iei_index = 4;
        }
        else
#endif
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Negotiated LLC SAPI IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* Negotiated LLC SAPI IE is of 2 octets and format TV. It doesn't have Length part
             so it cannot be skipped correctly after the switch block. Hence set valid_ie to
             TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = 8;
          (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
        }
        break;

#ifdef FEATURE_NAS_GW
      /***** Packet flow Identifier IE *****/
      case PACKET_FLOW_IDENTIFIER_IEI:
        if(opt_iei_index <= 5)
        {
          valid_ie = decode_packet_flow_identifier(src,
                                                   pos,
                                                   buffer_length,
                                                   &def_bearer_context_req->pack_flo_id,
                                                   esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            def_bearer_context_req->pack_flo_id.valid = FALSE;
          }
          else
          {
            def_bearer_context_req->pack_flo_id.valid = TRUE;
          }
          opt_iei_index = 6;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Packet flow identifier IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif

      /***** APN aggregate maximum bit rate IE *****/
      case APN_AMBR_IEI:
        if(opt_iei_index <= 6)
        {
          valid_ie = decode_apn_ambr(src,
                                     pos,
                                     buffer_length,
                                     &def_bearer_context_req->apn_ambr,
                                     esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            def_bearer_context_req->apn_ambr.valid = FALSE;
          }
          else
          {
            def_bearer_context_req->apn_ambr.valid = TRUE;
          }
          opt_iei_index = 7;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated APN-AMBR IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      /***** ESM cause IE *****/
      case ESM_CAUSE_IEI:
        if(opt_iei_index <= 7)
        {
          num_bits_to_read = 8;
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);

          populate_esm_cause(&def_bearer_context_req->esm_cause.esm_cause, unpackedbytes);
          def_bearer_context_req->esm_cause.valid = TRUE;
          valid_ie = TRUE;
          (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
          opt_iei_index = 8;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated ESM cause IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* ESM Cause IE is of Type TV. It doesn't have Length part so it cannot
           be skipped correctly after the switch block. Hence set valid_ie to
           TRUE and skip the IE right here */
          valid_ie = TRUE;
          (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
        }
        break;

      /***** Protocol configuration options IE *****/
      case PROT_CONFIG_OPT_IEI:
        if(opt_iei_index <= 8)
        {
          valid_ie = decode_prot_conf_opt(src,
                                          pos,
                                          buffer_length,
                                          &def_bearer_context_req->protocol_conf_opt,
                                          esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            def_bearer_context_req->protocol_conf_opt.valid = FALSE;
          }
          else
          {
            def_bearer_context_req->protocol_conf_opt.valid = TRUE;
          }
          opt_iei_index = 9;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated PCO IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
      case HC_CONFIG_IEI:
#ifdef FEATURE_CIOT
        if(opt_iei_index <= 12)
        {
          valid_ie = decode_hc_config_ie(src,
                                    pos,
                                    buffer_length,
                                    &def_bearer_context_req->hc_config,
                                    esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            def_bearer_context_req->hc_config.valid = FALSE;
          }
          else
          {
            def_bearer_context_req->hc_config.valid = TRUE;
          }
          opt_iei_index = 13;
        }
        else
#endif
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated HC CONFIG IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
      /***** Ext Protocol configuration options IE *****/
      case EXT_PROT_CONFIG_OPT_IEI:
        if(opt_iei_index <= 14)
        {
          valid_ie = decode_ext_prot_conf_opt(src,
                                              pos,
                                              buffer_length,
                                              &def_bearer_context_req->ext_protocol_conf_opt, 
                                              esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            def_bearer_context_req->ext_protocol_conf_opt.valid = FALSE;
          }
          else
          {
            def_bearer_context_req->ext_protocol_conf_opt.valid = TRUE;
          }
          opt_iei_index = 15;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated EPCO IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
      /***** Serving PLMN rate control IE *****/
      case SRV_PLMN_RATE_CTRL_IEI:
#ifdef FEATURE_CIOT
        if(opt_iei_index <= 15)
        {
          valid_ie = decode_serving_plmn_rate_control(src,
                                                      pos,
                                                      buffer_length,
                                                      &def_bearer_context_req->splmn_rate_ctrl,
                                                      esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            def_bearer_context_req->splmn_rate_ctrl.valid = FALSE;
          }
          else
          {
            def_bearer_context_req->splmn_rate_ctrl.valid = TRUE;
          }
          opt_iei_index = 16;
        }
        else
#endif
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Serving PLMN rate control IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
      default:
        /***** Radio priority IE *****/
        /* IEI value is present in bits 5-8 of the octet that is already read.
           So just mask the bits to get the value */
        if(RADIO_PRIORITY_IEI == (byte)(0xF0 & iei_val))
        {
#ifdef FEATURE_NAS_GW
          if(opt_iei_index <= 4)
          {
            /* Radio priority level value is present in bits 1-3 of the octet
               that is already read. So just mask the bits to get the value */
            def_bearer_context_req->radio_priority.radio_priority = (byte)(0x07 & iei_val);
            def_bearer_context_req->radio_priority.valid = TRUE;
            valid_ie = TRUE;
            opt_iei_index = 5;
          }
          else
#endif
          {
            /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Radio Priority IE, opt_iei_idx = %d",
                           opt_iei_index);
            /* Radio Priority IE is of 1 byte and format TV. It doesn't have Length part so
             it cannot be skipped correctly after the switch block. Hence set valid_ie to
             TRUE. iei_val is already read so no need to adjust buffer length and position */
            valid_ie = TRUE;
          }
        }
        else if(CP_ONLY_IND_IEI == (byte)(0xF0 & iei_val))
        {
#ifdef FEATURE_CIOT
          if(opt_iei_index <= 13)
          {
            num_bits_to_read = 8;
            if ((byte)(0x01 & iei_val) == 0) /*0 is reserved */
            {
              def_bearer_context_req->cp_only_opt.cp_only_indication = FALSE;
            }
            else
            {
              def_bearer_context_req->cp_only_opt.cp_only_indication = TRUE;
            }
            MSG_MED_DS_1(MM_SUB, "=NAS_MSG_LIB= control plane only ind = %d",
                         def_bearer_context_req->cp_only_opt.cp_only_indication);
            valid_ie = TRUE;
            opt_iei_index = 14;
          }
          else
#endif
          {
            /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Control Plane Only IE, opt_iei_idx = %d",
                           opt_iei_index);
            /* Control plane only indication is of 1 byte and format TV. It doesn't have Length part so
             it cannot be skipped correctly after the switch block. Hence set valid_ie to
             TRUE. iei_val is already read so no need to adjust buffer length and position */
            valid_ie = TRUE;
          }
        }
        else
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received unknown IEI = %d", iei_val);
          skip_unknown_ie = TRUE;
          valid_ie = FALSE;
        }

        break;
    } /* end switch(iei_val) */

    /* Check to see if the IE is faulty, if so move
       to next IE by incremement offset address and
       decrementing available buffer length */
    if(valid_ie == FALSE)
    {
      if(skip_unknown_ie == TRUE)
      {
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, TRUE, esm_len_in_bits);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
        *esm_len_in_bits = 0;
      }
      else
      {
        if(iei_buffer_len > (iei_len*8))
        {
          *buffer_length = iei_buffer_len - iei_len*8;
          *pos = iei_pos + iei_len*8;
          if(temp_esm_rem_len > (iei_len*8))
          {
            *esm_len_in_bits = temp_esm_rem_len - iei_len*8;
          }
          else
          {
            *esm_len_in_bits = 0;
          }
        }
        else
        {
          *buffer_length = 0;
          *esm_len_in_bits = 0;
        }
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*esm_len_in_bits) */

  return TRUE;
} /* end decode_def_bearer_context_request() */


#ifdef FEATURE_CIOT
/*===========================================================================
FUNCTION      DECODE ESM DATA TRANSPORT

DESCRIPTION
  This function decodes ESM DATA TRANSPORT message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
boolean decode_esm_data_transport
(
  byte                         *src,
  word                         *pos,
  word                         *buffer_length,
  lte_nas_esm_data_transport   *esm_data_transport_ptr,
  word                         *esm_len_in_bits
)
{
  byte num_bits_to_read = 0, unpacked_bytes = 0;
  uint16 unpacked_word = 0;
  dsm_item_type *dsm_ptr = NULL;
  uint16 dsm_packet_length = 0;
  uint16 len=0;

  if(*buffer_length < (NAS_ESM_ESM_DATA_TRANSPORT_MIN_LEN - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Buffer length is insufficient to decode ESM DATA TRANSPORT");
    *buffer_length = 0;
    return FALSE;
  }

  /***** user data container IE *****/
  /* Length */
  num_bits_to_read = 16;
  unpacked_word = b_unpackw(src, *pos, num_bits_to_read);
  if (ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits) == FALSE)
  {
    *buffer_length = 0;
    *esm_len_in_bits = 0;
     MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= buffer wraparound");
     return FALSE;
  }

  if(unpacked_word != 0)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Packing user data container with length = %d into a DSM item", unpacked_word);
    if(unpacked_word > ((*esm_len_in_bits)/8))
    {
      *buffer_length = 0;
      *esm_len_in_bits = 0;
       return FALSE;
    }
    /*Push down into appropriate pool*/
	/*if we do not have enough small items use large item pool*/
    if((unpacked_word <= DSM_DS_SMALL_ITEM_SIZ) && 
     (unpacked_word <= NAS_REM_SMALL_ITEM_POOL_SIZE))
    {
      MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d small item pool", unpacked_word);
      /* Pushdown DSM */
      dsm_packet_length = dsm_pushdown(&dsm_ptr, 
                                     src + (*pos/8),
                                     unpacked_word, 
                                     DSM_DS_SMALL_ITEM_POOL);

      if(unpacked_word != dsm_packet_length)
      {
        /*free any partial memory allocated */
        dsm_free_packet(&dsm_ptr);

        MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d large item pool", dsm_packet_length);
        /* If Small item pool is running out always try Large item pool */
        len = dsm_pushdown(&dsm_ptr,src + (*pos/8),unpacked_word, DSM_DS_LARGE_ITEM_POOL);
        if(len != unpacked_word) 
        {
          MSG_FATAL_DS(MM_SUB,"=EMM= Failed to push len %d large item pool",dsm_packet_length,0,0);
        }
      }
    }
    else
    {
      MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d large item pool", dsm_packet_length);

      /* If Small item pool is running out always use Large item pool */
      dsm_packet_length = dsm_pushdown(&dsm_ptr, 
                                     src + (*pos/8),
                                     unpacked_word, 
                                     DSM_DS_LARGE_ITEM_POOL);

      if(unpacked_word != dsm_packet_length)
      {
        /*free any partial memory allocated */
        dsm_free_packet(&dsm_ptr);

        MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d small item pool", unpacked_word);

        /* If Small item pool is running out always try Large item pool */
        len = dsm_pushdown(&dsm_ptr,src + (*pos/8),unpacked_word, DSM_DS_SMALL_ITEM_POOL);
        if(len != unpacked_word) 
        {
          MSG_FATAL_DS(MM_SUB,"=EMM= Failed to push len %d small item pool",unpacked_word,0,0);
        }
      }
    }
    esm_data_transport_ptr->dsm_ptr = dsm_ptr;

    mm_check_for_null_ptr((void *)dsm_ptr);

    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(unpacked_word*8, pos, buffer_length, esm_len_in_bits);

    return TRUE;
  }
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM DATA TRANSPORT received with invalid data container length");
    return FALSE;
  }
} /* end decode_esm_data_transport() */
#endif
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
boolean decode_pdn_conn_rej
(
  byte                         *src,
  word                         *pos,
  word                         *buffer_length,
  lte_nas_pdn_connectivity_rej *pdn_conn_rej,
  word                         *esm_len_in_bits
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  boolean valid_ie = TRUE;
  word temp_esm_rem_len = 0;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif

#ifdef FEATURE_LTE_REL10
  byte security_hdr_type = 0, security_hdr_offset = 0;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  lte_nas_message_id emm_msg_id;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  
#endif

 
  if(*buffer_length < (NAS_ESM_PDN_CON_REJ_MIN_LEN - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "Buffer length = %d is insufficient to decode PDN CONNECTIVITY REJECT",
                   *buffer_length);
    *buffer_length = 0;
    *esm_len_in_bits = 0;
    return FALSE;
  }

  /***** ESM cause IE *****/
  num_bits_to_read = 8;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);

  populate_esm_cause(&pdn_conn_rej->esm_cause.esm_cause, unpackedbytes);
  pdn_conn_rej->esm_cause.valid = TRUE;
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits);


                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*esm_len_in_bits)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      *esm_len_in_bits = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;
 
    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    if (ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits) == FALSE)
    {
      *buffer_length = 0;
      *esm_len_in_bits = 0;
      MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= buffer wraparound");
      return TRUE;
    }

    /* IE length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /* Store offset address & remaining buffer length, in case any octet 
    is faulty this info would be required to jump to next IE */
    iei_pos = *pos + 8;
    iei_buffer_len = *buffer_length - 8;
    temp_esm_rem_len = *esm_len_in_bits - 8;

    switch(iei_val)
    {
      /***** Protocol configuration options IE *****/
      case PROT_CONFIG_OPT_IEI:
        if(opt_iei_index == 1)
        {
          valid_ie = decode_prot_conf_opt(src,
                                          pos,
                                          buffer_length,
                                          &pdn_conn_rej->protocol_conf_opt,
                                          esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            pdn_conn_rej->protocol_conf_opt.valid = FALSE;
          }
          else
          {
            pdn_conn_rej->protocol_conf_opt.valid = TRUE; 
          }
          opt_iei_index = 2;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated PCO IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

#ifdef FEATURE_LTE_REL10
      /***** T3396 value IE *****/
      case T3396_VALUE_IEI:
        if(opt_iei_index <= 2)
        {
          if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
          {
            if (emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL12)
            {
               if((pdn_conn_rej->esm_cause.esm_cause == NAS_ESM_INSUFFICIENT_RESOURCES) ||
                  (pdn_conn_rej->esm_cause.esm_cause == NAS_ESM_UNKNOWN_MISSING_APN))
               {
               valid_ie = decode_t3396_value(src,
                                               pos,
                                               buffer_length,
                                               &pdn_conn_rej->backoff_timer,
                                               esm_len_in_bits);
               }
               else /* This IE shall not be included when ESM Cause is not NAS_ESM_INSUFFICIENT_RESOURCES(#26) or
                       NAS_ESM_UNKNOWN_MISSING_APN(#27). Skip decoding of this IE */
               {
                 valid_ie = FALSE;
               }
            }
            else
            {
              valid_ie = decode_t3396_value(src,
                                               pos,
                                               buffer_length,
                                               &pdn_conn_rej->backoff_timer,
                                               esm_len_in_bits);
            }


              if(valid_ie == TRUE)
              {
                num_bits_to_read = 4;
                security_hdr_type = (byte)b_unpackb(emm_ctrl_data_ptr->emm_ota_message_ptr,
                                                     security_hdr_offset,
                                                     num_bits_to_read);

                emm_msg_id = emm_ctrl_data_ptr->nas_incoming_msg_ptr->emm_incoming_msg.hdr.msg_id;
                MSG_HIGH_DS_2(MM_SUB, "MSG_LIB: emm_msg_id=%d, sec_hdr=%d",
                              emm_msg_id,
                              security_hdr_type);
   
                if((emm_msg_id == ATTACH_REJECT) &&
                   (security_hdr_type != INTEGRITY_PROTECT) &&
                   (security_hdr_type != INTEGRITY_PROTECT_CIPHER) &&
                   (pdn_conn_rej->esm_cause.esm_cause != NAS_ESM_APN_NOT_SUPPORTED_IN_PLMN_RAT_COMBINATION))
                {
                  /* Set timer to active and assign a random timer value between 15min to 30min */
                  pdn_conn_rej->backoff_timer.active = TRUE;
                  pdn_conn_rej->backoff_timer.timer_count = ((15 + mm_get_random_value(15)) * 60 * 1000);
                  MSG_HIGH_DS_3(MM_SUB, "MSG_LIB: msg_id=%d: sec_hdr=%d: random backoff_timer value = %d milliseconds",
                                emm_msg_id,
                                security_hdr_type,
                                pdn_conn_rej->backoff_timer.timer_count);
                }
              }
            opt_iei_index = 3;
          }
          else
          {
             MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3396 value IE unsupported in UE rel %d",
                         emm_ctrl_data_ptr->emm_3gpp_spec_ver);
            valid_ie = FALSE;
          }
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated T3396 value IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif
#ifdef FEATURE_LTE_REL12
			/***** Re-attempt Indicator IE *****/
			case REATTEMPT_INDICATOR_IEI:
			  if(opt_iei_index <= 3)
			  {
				if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL12)
				{
				  valid_ie = decode_reattempt_ind_value(src,
														pos,
														buffer_length,
														&pdn_conn_rej->reattempt_indicator,
														esm_len_in_bits);
          }
          else
          {
				  MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Reattempt Indicator IE unsupported in UE rel %d",
                         emm_ctrl_data_ptr->emm_3gpp_spec_ver);
            valid_ie = FALSE;
          }
				opt_iei_index = 4;
			  }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Re-Attempt Indicator value IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif
      /***** EXT Protocol configuration options IE *****/
      case EXT_PROT_CONFIG_OPT_IEI:
        if(opt_iei_index <= 4)
        {
          valid_ie = decode_ext_prot_conf_opt(src,
                                        pos,
                                        buffer_length,
                                        &pdn_conn_rej->ext_protocol_conf_opt, 
                                        esm_len_in_bits);
          if(valid_ie == FALSE)
          {
            pdn_conn_rej->ext_protocol_conf_opt.valid = FALSE;
          }
          else
          {
            pdn_conn_rej->ext_protocol_conf_opt.valid = TRUE; 
          }
          opt_iei_index = 5;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated EPCO IE, opt_iei_idx = %d",
                       opt_iei_index);
          valid_ie = FALSE;
        }
        break;
      default:
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received unknown IEI = %d", iei_val);
        skip_unknown_ie = TRUE;
        valid_ie = FALSE;
        break;
    } /* end switch(iei_val) */

    /* Check to see if the IE is faulty, if so move
       to next IE by incremement offset address and
       decrementing available buffer length */
    if(valid_ie == FALSE)
    {
      if(skip_unknown_ie == TRUE)
      {
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, TRUE, esm_len_in_bits);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
        *esm_len_in_bits = 0;
      }
      else
      {
        if(iei_buffer_len > (iei_len*8))
        {
          *buffer_length = iei_buffer_len - iei_len*8;
          *pos = iei_pos + iei_len*8;
          if(temp_esm_rem_len > (iei_len*8))
          {
            *esm_len_in_bits = temp_esm_rem_len - iei_len*8;
          }
          else
          {
            *esm_len_in_bits = 0;
          }
        }
        else
        {
          *buffer_length = 0;
          *esm_len_in_bits = 0;
        }

        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*esm_len_in_bits) */

  return TRUE; 
} /* end decode_pdn_conn_rej() */


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
boolean decode_esm_status
(
  byte               *src,
  word               *pos, 
  word               *buffer_length,
  lte_nas_esm_status *esm_status,
  word               *esm_len_in_bits
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;


  if(*buffer_length < (NAS_ESM_ESM_STATUS - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode ESM STATUS",
                   *buffer_length);
    *buffer_length = 0;
    *esm_len_in_bits = 0;
    return FALSE;
  }
  
  /****** ESM cause IE ******/
  num_bits_to_read = 8;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
 
  populate_esm_cause(&esm_status->esm_cause, unpackedbytes);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_len_in_bits);

  return TRUE;
} /* end decode_esm_status() */


#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
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
boolean decode_esm_msg
(
  byte                      *src,
  word                      *pos,
  word                      *buffer_length,
  word                      esm_len,
  lte_nas_incoming_msg_type *nas_incoming_msg_ptr
)
{ 
  byte num_bits_to_read = 0, pd = 0, bearer_id = 0, pt_id = 0, message_id = 0;
  log_lte_nas_ota_in_msg_event_type event;
  boolean valid_ie = FALSE;
  word esm_len_in_bits = 0;
  lte_nas_esm_incoming_msg_type *msg_type;
  boolean esm_msg_failed = FALSE;
  byte first_three_esm_bytes = 3; /*PD + Bearer ID (1) + PTI (1) + MSG_TYPE (1)*/
  emm_ctrl_data_type *emm_ctrl_data_ptr;


  buffer_length_check_failed = FALSE;
  esm_len_in_bits = (esm_len * 8);
  
  if(*buffer_length < MIN_ESM_LEN || esm_len_in_bits < MIN_ESM_LEN )
  {
    MSG_ERROR_DS_3(MM_SUB, "=NAS_MSG_LIB= NAS_MSG_LIB: Either Buffer length = %d or ESM length = %d less than minimum ESM length = %d",
                   *buffer_length, esm_len_in_bits, MIN_ESM_LEN);
    return FALSE;
  }

  MSG_MED_DS_0(MM_SUB, "=NAS_MSG_LIB= Starting to decode ESM message");
  msg_type = &nas_incoming_msg_ptr->esm_incoming_msg;

  /***** Bearer ID (reading the MSB 4 bits) *****/
  num_bits_to_read = 4;
  bearer_id = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, &esm_len_in_bits);

  /***** ESM Protocol Discriminator (reading the LSB 4 bits) *****/
  num_bits_to_read = 4;
  pd = b_unpackb(src, *pos, num_bits_to_read);


  MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= Decoding ESM message EPS Bearer ID = %d Protocol Discriminator = %d",bearer_id,pd);

  if(pd!= (byte)EPS_SESSION_MANAGEMENT_MESSAGES)
  {
    MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Received Protocol Discriminator other than ESM");
    SKIP_DECODING_ESM(pos, buffer_length, esm_len_in_bits);
    return FALSE;
  }
  msg_type->nas_hdr.pd = (lte_nas_protocol_discriminator)pd;
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, &esm_len_in_bits);

  /***** Protocol Transaction ID *****/
  num_bits_to_read = 8;
  pt_id = b_unpackb(src, *pos, num_bits_to_read);
  MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Protocol Transaction ID = %d", pt_id);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, &esm_len_in_bits);

  /***** ESM Message ID *****/
  num_bits_to_read = 8;
  message_id = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, &esm_len_in_bits);

  /* Write these parameters into the corresponding structures */
  msg_type->nas_hdr.msg_id = (lte_nas_message_id)message_id;

  /* Populate OTA message info */
  msg_type->nas_hdr.ota_msg_ptr = src;
  msg_type->nas_hdr.ota_msg_len = (*buffer_length + (first_three_esm_bytes)*8)/8;
  event.in_msg = message_id;
  event_report_payload(EVENT_LTE_ESM_OTA_INCOMING_MSG, sizeof(event), (void*)&event);

  switch(message_id)
  {
    case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST:
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM Message ID --> ACTIVATE DEFAULT BEARER CONTEXT REQUEST");
      msg_type->default_bearer_context_req.lte_nas_hdr.pd = (lte_nas_protocol_discriminator)pd;
      msg_type->default_bearer_context_req.lte_nas_hdr.msg_id = (lte_nas_message_id)message_id;
      msg_type->default_bearer_context_req.lte_nas_esm_hdr.bearer_id = bearer_id;
      msg_type->default_bearer_context_req.lte_nas_esm_hdr.pti = pt_id;
      valid_ie = decode_act_def_context_req(src,
                                            pos,
                                            buffer_length,
                                            &msg_type->default_bearer_context_req,
                                            &esm_len_in_bits);
      MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= ACTIVATE DEFAULT BEARER CONTEXT REQUEST decoding PASSED(1)/FAILED(0) = %d", valid_ie);
      if(valid_ie == FALSE)
      {
        SKIP_DECODING_ESM(pos,buffer_length,esm_len_in_bits);
        MSG_HIGH_DS_0(MM_SUB, "=LTE_NAS_MSG_LIB= ACTIVATE DEFAULT BEARER CONTEXT REQUEST decoding failed");
        return FALSE;
      }
      total_num_messages++;
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ACTIVATE DEFAULT BEARER CONTEXT REQUEST decoding passed");
      break;

    case PDN_CONNECTIVITY_REJECT:
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM Message ID --> PDN CONNECTIVITY REJECT");
      msg_type->pdn_connectivity_rej.lte_nas_hdr.pd = (lte_nas_protocol_discriminator)pd;
      msg_type->pdn_connectivity_rej.lte_nas_hdr.msg_id = (lte_nas_message_id)message_id;
      msg_type->pdn_connectivity_rej.lte_nas_esm_hdr.bearer_id = bearer_id;
      msg_type->pdn_connectivity_rej.lte_nas_esm_hdr.pti = pt_id;

      /* If PDN CONNECTIVITY REJECT has different PTI than the one allocated in PDN
         CONNECTIVITY REQUEST then stop decoding PDN_CON_REJ message */
      emm_ctrl_data_ptr = emm_db_get_ctrl_data();
      
      if((emm_ctrl_data_ptr->emm_state == EMM_REGISTERED_INITIATED) &&
         (emm_ctrl_data_ptr->emm_substate == EMM_WAITING_FOR_NW_RESPONSE) &&
         (emm_ctrl_data_ptr->pdn_conn_req_ptr != NULL) &&
         (pt_id != emm_ctrl_data_ptr->pdn_conn_req_ptr->lte_nas_esm_hdr.pti))
      {
        SKIP_DECODING_ESM(pos, buffer_length, esm_len_in_bits);
        MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= PTI mismatch, expected PTI = %d. Stop decoding",
                      emm_ctrl_data_ptr->pdn_conn_req_ptr->lte_nas_esm_hdr.pti);
        return FALSE;
      }
      valid_ie = decode_pdn_conn_rej(src,
                                     pos,
                                     buffer_length,
                                     &msg_type->pdn_connectivity_rej,
                                     &esm_len_in_bits);
      if(valid_ie == FALSE)
      {
        SKIP_DECODING_ESM(pos, buffer_length, esm_len_in_bits);
        MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= PDN CONNECTIVITY REJECT decoding failed");
        return FALSE;
      }
      total_num_messages++;
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= PDN CONNECTIVITY REJECT decoding passed");
      break;

    case ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST:
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM Message ID --> ACTIVATE DEDICATED EPS BEARER CONTEXT REQUEST");
      msg_type->dedicated_bearer_context_req.lte_nas_hdr.pd = (lte_nas_protocol_discriminator)pd;
      msg_type->dedicated_bearer_context_req.lte_nas_hdr.msg_id = (lte_nas_message_id)message_id;
      msg_type->dedicated_bearer_context_req.lte_nas_esm_hdr.bearer_id = bearer_id;
      msg_type->dedicated_bearer_context_req.lte_nas_esm_hdr.pti = pt_id;
      valid_ie = decode_dedicated_eps_bearer_req(src,
                                                 pos,
                                                 buffer_length,
                                                 &msg_type->dedicated_bearer_context_req,
                                                 &esm_len_in_bits);
      if(valid_ie == FALSE)
      {
        SKIP_DECODING_ESM(pos,buffer_length,esm_len_in_bits);
        MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ACTIVATE DEDICATED EPS BEARER CONTEXT REQUEST decoding failed");
        return FALSE;
      }
      total_num_messages++;
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ACTIVATE DEDICATED EPS BEARER CONTEXT REQUEST decoding passed");
      break;

    case DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST:
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM Message ID --> DEACTIVATE EPS BEARER CONTEXT REQUEST");
      msg_type->deactivate_eps_bearer_context_req.lte_nas_hdr.pd =
                                                        (lte_nas_protocol_discriminator)pd;
      msg_type->deactivate_eps_bearer_context_req.lte_nas_hdr.msg_id =
                                                         (lte_nas_message_id)message_id;
      msg_type->deactivate_eps_bearer_context_req.lte_nas_esm_hdr.bearer_id = bearer_id;
      msg_type->deactivate_eps_bearer_context_req.lte_nas_esm_hdr.pti = pt_id;
      valid_ie = decode_deactivate_eps_bearer_req(src,
                                                  pos,
                                                  buffer_length,
                                                  &msg_type->deactivate_eps_bearer_context_req,
                                                  &esm_len_in_bits);
      if(valid_ie == FALSE)
      {
        SKIP_DECODING_ESM(pos, buffer_length, esm_len_in_bits);
        MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= DEACTIVATE EPS BEARER CONTEXT REQUEST decoding failed");
        return FALSE;
      }
      total_num_messages++;
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= DEACTIVATE EPS BEARER CONTEXT REQUEST decoding passed");
      break;

    case BEARER_RESOURCE_ALLOCATION_REJECT:
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM Message ID --> BEARER RESOURCE ALLOCATION REJECT");
      msg_type->bearer_resource_alloc_rej.lte_nas_hdr.pd = (lte_nas_protocol_discriminator)pd;
      msg_type->bearer_resource_alloc_rej.lte_nas_hdr.msg_id = (lte_nas_message_id)message_id;
      msg_type->bearer_resource_alloc_rej.lte_nas_esm_hdr.bearer_id = bearer_id;
      msg_type->bearer_resource_alloc_rej.lte_nas_esm_hdr.pti = pt_id;
      valid_ie = decode_bearer_resource_alloc_rej(src,
                                                  pos,
                                                  buffer_length,
                                                  &msg_type->bearer_resource_alloc_rej,
                                                  &esm_len_in_bits);
      if(valid_ie == FALSE)
      {
        SKIP_DECODING_ESM(pos, buffer_length, esm_len_in_bits);
        MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= BEARER RESOURCE ALLOCATION REJECT decoding failed");
        return FALSE;
      }
      total_num_messages++;
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= BEARER RESOURCE ALLOCATION REJECT decoding passed");
      break;

    case BEARER_RESOURCE_MODIFICATION_REJECT:
      MSG_HIGH_DS_0(MM_SUB, "=LTE_NAS_MSG_LIB= ESM Message ID --> BEARER RESOURCE MODIFICATION REJECT");
      msg_type->bearer_resource_modifi_rej.lte_nas_hdr.pd = (lte_nas_protocol_discriminator)pd;
      msg_type->bearer_resource_modifi_rej.lte_nas_hdr.msg_id = (lte_nas_message_id)message_id;
      msg_type->bearer_resource_modifi_rej.lte_nas_esm_hdr.bearer_id = bearer_id;
      msg_type->bearer_resource_modifi_rej.lte_nas_esm_hdr.pti = pt_id;
      valid_ie = decode_bearer_modify_context_rej(src,
                                                  pos,
                                                  buffer_length,
                                                  &msg_type->bearer_resource_modifi_rej,
                                                  &esm_len_in_bits);
      if(valid_ie == FALSE)
      {
        SKIP_DECODING_ESM(pos, buffer_length, esm_len_in_bits);
        MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= BEARER RESOURCE MODIFICATION REJECT decoding failed");
        return FALSE;
      }
      total_num_messages++;
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= BEARER RESOURCE MODIFICATION REJECT decoding passed");
      break;

    case MODIFY_EPS_BEARER_CONTEXT_REQUEST:
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM Message ID --> MODIFY EPS BEARER CONTEXT REQUEST");
        msg_type->modify_eps_bearer_ctxt_req.lte_nas_hdr.pd = (lte_nas_protocol_discriminator)pd;
        msg_type->modify_eps_bearer_ctxt_req.lte_nas_hdr.msg_id = (lte_nas_message_id)message_id;
        msg_type->modify_eps_bearer_ctxt_req.lte_nas_esm_hdr.bearer_id = bearer_id;
        msg_type->modify_eps_bearer_ctxt_req.lte_nas_esm_hdr.pti = pt_id;
        valid_ie = decode_modify_eps_context_req(src,
                                                 pos,
                                                 buffer_length,
                                                 &msg_type->modify_eps_bearer_ctxt_req,
                                                 &esm_len_in_bits);
        if(valid_ie == FALSE)
        {
          SKIP_DECODING_ESM(pos, buffer_length, esm_len_in_bits);
          MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= MODIFY EPS BEARER CONTEXT REQUEST decoding failed");
          return FALSE;
        }
        total_num_messages++;
        MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= MODIFY EPS BEARER CONTEXT REQUEST decoding passed");
        break;

    case PDN_DISCONNECT_REJECT:
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM Message ID --> PDN DISCONNECT REJECT");
      msg_type->pdn_disconnect_rej.lte_nas_hdr.pd = (lte_nas_protocol_discriminator)pd;
      msg_type->pdn_disconnect_rej.lte_nas_hdr.msg_id = (lte_nas_message_id)message_id;
      msg_type->pdn_disconnect_rej.lte_nas_esm_hdr.bearer_id = bearer_id;
      msg_type->pdn_disconnect_rej.lte_nas_esm_hdr.pti = pt_id;
      valid_ie = decode_pdn_disconnect_rej(src,
                                           pos,
                                           buffer_length,
                                           &msg_type->pdn_disconnect_rej,
                                           &esm_len_in_bits);
      if(valid_ie == FALSE)
      {
        SKIP_DECODING_ESM(pos, buffer_length, esm_len_in_bits);
        MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= PDN DISCONNECT REJECT decoding failed");
        return FALSE;
      }
      total_num_messages++;
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= PDN DISCONNECT REJECT decoding passed");
      break;

    case ESM_INFORMATION_REQUEST:
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM Message ID --> ESM INFORMATION REQUEST");
      msg_type->esm_info_req.lte_nas_hdr.pd = (lte_nas_protocol_discriminator)pd;
      msg_type->esm_info_req.lte_nas_hdr.msg_id = (lte_nas_message_id)message_id;
      msg_type->esm_info_req.lte_nas_esm_hdr.bearer_id = bearer_id;
      msg_type->esm_info_req.lte_nas_esm_hdr.pti = pt_id;
      total_num_messages++;
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM INFORMATION REQUEST decoding passed");
      break;

#ifdef FEATURE_LTE_REL9
    case ESM_NOTIFICATION:
      emm_ctrl_data_ptr = emm_db_get_ctrl_data();
      
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
      {
        MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM Message ID --> ESM NOTIFICATION");
        msg_type->esm_notification.lte_nas_hdr.pd = (lte_nas_protocol_discriminator)pd;
        msg_type->esm_notification.lte_nas_hdr.msg_id = (lte_nas_message_id)message_id;
        msg_type->esm_notification.lte_nas_esm_hdr.bearer_id = bearer_id;
        msg_type->esm_notification.lte_nas_esm_hdr.pti = pt_id;
        valid_ie = decode_esm_notification(src,
                                           pos,
                                           buffer_length,
                                           &msg_type->esm_notification,
                                           &esm_len_in_bits);
        if(valid_ie == FALSE)
        {
          SKIP_DECODING_ESM(pos, buffer_length, esm_len_in_bits);
          MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM NOTIFICATION decoding failed");
          return FALSE;
        }
        total_num_messages++;
        MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM NOTIFICATION decoding passed");
      }
      break;
#endif
    case ESM_STATUS:
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM Message ID --> ESM STATUS");
      msg_type->mt_esm_status.lte_nas_hdr.pd =
                                          (lte_nas_protocol_discriminator)pd;
      msg_type->mt_esm_status.lte_nas_hdr.msg_id = (lte_nas_message_id)message_id;
      msg_type->mt_esm_status.lte_nas_esm_hdr.bearer_id = bearer_id;
      msg_type->mt_esm_status.lte_nas_esm_hdr.pti = pt_id;
      valid_ie = decode_esm_status(src,
                                   pos,
                                   buffer_length,
                                   &msg_type->mt_esm_status,
                                   &esm_len_in_bits);
      if(valid_ie == FALSE)
      {
        SKIP_DECODING_ESM(pos, buffer_length, esm_len_in_bits);
        MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM STATUS decoding failed");
        return FALSE;
      }
      total_num_messages++;
      break;
#ifdef FEATURE_CIOT
    case ESM_DATA_TRANSPORT:
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM Message ID --> ESM_DATA_TRANSPORT");
      msg_type->esm_data_transport.lte_nas_hdr.pd =
                                                        (lte_nas_protocol_discriminator)pd;
      msg_type->esm_data_transport.lte_nas_hdr.msg_id =
                                                         (lte_nas_message_id)message_id;
      msg_type->esm_data_transport.lte_nas_esm_hdr.bearer_id = bearer_id;
      msg_type->esm_data_transport.lte_nas_esm_hdr.pti = pt_id;
      valid_ie = decode_esm_data_transport(src,
                                           pos,
                                           buffer_length,
                                           &msg_type->esm_data_transport,
                                           &esm_len_in_bits);
      if(valid_ie == FALSE)
      {
        SKIP_DECODING_ESM(pos, buffer_length, esm_len_in_bits);
        MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM DATA TRANSPORT decoding failed");
        return FALSE;
      }
      total_num_messages++;
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= NAS_MSG_LIB= ESM DATA TRANSPORT decoding passed");
      break;
    case ESM_DUMMY_MESSAGE:
       MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM Message ID --> ESM_DUMMY_MESSAGE");
       msg_type->esm_dummy_message.lte_nas_hdr.pd =
                                                        (lte_nas_protocol_discriminator)pd;
       msg_type->esm_dummy_message.lte_nas_hdr.msg_id =
                                                         (lte_nas_message_id)message_id;
       msg_type->esm_dummy_message.lte_nas_esm_hdr.bearer_id = bearer_id;
       msg_type->esm_dummy_message.lte_nas_esm_hdr.pti = pt_id;
       valid_ie = TRUE;
       total_num_messages++;
       MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= NAS_MSG_LIB= ESM_DUMMY_MESSAGE decoding passed");
      break;
#endif
    default:
      MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Unknown ESM messsage");
      msg_type->nas_hdr.pd = (lte_nas_protocol_discriminator)pd;
      msg_type->nas_hdr.msg_id = (lte_nas_message_id)message_id;
      msg_type->nas_hdr.valid = FALSE;
      msg_type->nas_hdr.esm_decode_fail_reason =
                               (esm_cause_enum_T)LTE_NAS_NON_EXISTENT_MSG_TYPE;
      msg_type->esm_unknown_msg.lte_nas_esm_hdr.bearer_id = bearer_id;
      msg_type->esm_unknown_msg.lte_nas_esm_hdr.pti = pt_id;
      SKIP_DECODING_ESM(pos, buffer_length, esm_len_in_bits);
      esm_msg_failed = TRUE;
      total_num_messages++;
      break;
  } /* end switch(message_id) */

  esm_len_in_bits = 0;
#ifdef TEST_FRAMEWORK
  #error code not present
#endif /* TEST_FRAMEWORK */
  if(esm_msg_failed)
  {
    return FALSE;
  }
  return TRUE;
} /* end decode_esm_msg() */

#endif /* FEATURE_LTE */