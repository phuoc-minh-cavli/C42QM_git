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

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/msg_lib_declarations.c#1 $
$Author: pwbldsvc $
$DateTime: 2023/04/25 14:39:18 $

when           who                          what                              
--------     --------  ------------------------------------------------
05/17/09      hnam    Added the code to support security
==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#ifdef FEATURE_LTE
/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "lte_nas_emm_message_type.h"
#include "lte_nas_esm_message_type.h"
#include "msg_lib_declarations.h"
#include "lte_nas.h"
#include "err.h"
#include "lte_nas_security_info.h"
#include "emm_security.h"
#include "bit.h"

/*==============================================================================
                      DATA DECLARATIONS
==============================================================================*/
byte total_num_messages;
byte nas_emm_index;
byte nas_esm_index;
boolean buffer_length_check_failed;
#define MAX_BCD_VALUE 9

#ifdef FEATURE_LTE_NAS_DEEP_SEC

#define MAX_ACT_DEFAULT_BEARER_CTXT_REQ_LEN       417
#define MAX_ATTACH_ACCEPT_LEN                     MAX_ACT_DEFAULT_BEARER_CTXT_REQ_LEN + 237 /* ESM + EMM */
#define MAX_TRACKING_AREA_UPDATE_ACCEPT_LEN       243
#define MAX_ATTACH_REJECT_LEN                     3 + 257 /* PDN_CONN_REJ = 257 */
#define MAX_SECURITY_MODE_COMMAND_LEN             21
#define MAX_EMM_AUTHENTICATION_REQUEST_LEN        36
#define MAX_GUTI_REALLOCATION_COMMAND_LEN         112
#define MAX_EMM_INFORMATION_LEN                   15 +  251 /* NW_NAME */ + 251 /* SHORT_NAME */
#define MAX_DOWNLINK_NAS_TRANSPORT_LEN            254
#define MAX_CS_SERV_NOTIFICATION_LEN              278

#define MAX_MODIFY_EPS_BEARER_CONTEXT_REQUEST_LEN 538

/*==============================================================================
                      FUNCTION DEFINITIONS
==============================================================================*/

/*===========================================================================
FUNCTION  MSG_LIB_VALIDATE_UPDATE_INCOMING_NAS_MSG_LENGTH

DESCRIPTION
  This function checks the incoming NAS message length, if the length 
  is greater then it is truncated to the MAX length for that OTA message

DEPENDENCIES
  None

RETURN VALUE
  Updated length (in bits) of the incoming NAS message

SIDE EFFECTS
  None
===========================================================================*/
word msg_lib_validate_update_incoming_nas_msg_length
( 
  word                           buffer_length,
  lte_nas_protocol_discriminator pd,
  lte_nas_message_id             message_id
)
{
  switch(pd)
  {
    case EPS_MOBILITY_MANAGEMENT_MESSAGES:
      switch (message_id)
      {
        case ATTACH_ACCEPT:
          if(buffer_length > MAX_ATTACH_ACCEPT_LEN)
          {
            MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= ATTACH_ACCEPT length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                           buffer_length,
                           MAX_ATTACH_ACCEPT_LEN);
            buffer_length = MAX_ATTACH_ACCEPT_LEN;
          }
          break;

        case TRACKING_AREA_UPDATE_ACCEPT:
          if(buffer_length > MAX_TRACKING_AREA_UPDATE_ACCEPT_LEN)
          {
            MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= TRACKING_AREA_UPDATE_ACCEPT length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                           buffer_length,
                           MAX_TRACKING_AREA_UPDATE_ACCEPT_LEN);
            buffer_length = MAX_TRACKING_AREA_UPDATE_ACCEPT_LEN;
          }
          break;

        case ATTACH_REJECT:  
          if(buffer_length > MAX_ATTACH_REJECT_LEN)
          {
            MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= ATTACH_REJECT length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                           buffer_length,
                           MAX_ATTACH_REJECT_LEN);
            buffer_length = MAX_ATTACH_REJECT_LEN;
          }
          break;

        case SECURITY_MODE_COMMAND:
          if(buffer_length > MAX_SECURITY_MODE_COMMAND_LEN)
          {
            MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= SECURITY_MODE_COMMAND length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                           buffer_length,
                           MAX_SECURITY_MODE_COMMAND_LEN);
            buffer_length = MAX_SECURITY_MODE_COMMAND_LEN;
          }
          break;

        case EMM_AUTHENTICATION_REQUEST:
          if(buffer_length > MAX_EMM_AUTHENTICATION_REQUEST_LEN)
          {
            MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB ACT_DEFAULT_BEARER_CTXT_REQ length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                           buffer_length,
                           MAX_ACT_DEFAULT_BEARER_CTXT_REQ_LEN);
            buffer_length = MAX_ACT_DEFAULT_BEARER_CTXT_REQ_LEN;
          }
          break;

        case GUTI_REALLOCATION_COMMAND:
          if(buffer_length > MAX_GUTI_REALLOCATION_COMMAND_LEN)
          {
            MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= GUTI_REALLOCATION_COMMAND length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                           buffer_length,
                           MAX_GUTI_REALLOCATION_COMMAND_LEN);
            buffer_length = MAX_GUTI_REALLOCATION_COMMAND_LEN;
          }
          break;

        case EMM_INFORMATION:
          if(buffer_length > MAX_EMM_INFORMATION_LEN)
          {
            MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= EMM_INFORMATION length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                           buffer_length,
                           MAX_EMM_INFORMATION_LEN);
            buffer_length = MAX_EMM_INFORMATION_LEN;
          }
          break;

        case DOWNLINK_NAS_TRANSPORT:
          if(buffer_length > MAX_DOWNLINK_NAS_TRANSPORT_LEN)
          {
            MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= DOWNLINK_NAS_TRANSPORT length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                           buffer_length,
                           MAX_ACT_DEFAULT_BEARER_CTXT_REQ_LEN);
            buffer_length = MAX_DOWNLINK_NAS_TRANSPORT_LEN;
          }
          break;

        case CS_SERV_NOTIFICATION:
          if(buffer_length > MAX_CS_SERV_NOTIFICATION_LEN)
          {
            MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= CS_SERV_NOTIFICATION length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                           buffer_length,
                           MAX_CS_SERV_NOTIFICATION_LEN);
            buffer_length = MAX_CS_SERV_NOTIFICATION_LEN;
          }
          break;

        default:
          break;
      } /* end switch (message_id) */
      break;
   
    case EPS_SESSION_MANAGEMENT_MESSAGES:  
      switch(message_id)
      {
        case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST:
          if(buffer_length > MAX_ACT_DEFAULT_BEARER_CTXT_REQ_LEN)
          {
            MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= ACT_DEFAULT_BEARER_CTXT_REQ length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                           buffer_length,
                           MAX_ACT_DEFAULT_BEARER_CTXT_REQ_LEN);
            buffer_length = MAX_ACT_DEFAULT_BEARER_CTXT_REQ_LEN;
          }
          break;

        case MODIFY_EPS_BEARER_CONTEXT_REQUEST:
          if(buffer_length > MAX_MODIFY_EPS_BEARER_CONTEXT_REQUEST_LEN)
          {
            MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= MODIFY_EPS_BEARER_CONTEXT_REQUEST length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                           buffer_length,
                           MAX_MODIFY_EPS_BEARER_CONTEXT_REQUEST_LEN);
            buffer_length = MAX_MODIFY_EPS_BEARER_CONTEXT_REQUEST_LEN;
          }
          break;

        default:
          break;
      } /* end switch(message_id) */
      break;

    default:
      break;
  } /* end switch(pd) */

  MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Message_length = %d", buffer_length);
  /* Length was received in bytes, it needs to be returned in bits, so convert here.
     Length was incremented and given to this function, so subtract that length
     and return the original length to the calling function */
  return (buffer_length - 1)*8;
} /* end msg_lib_validate_update_incoming_nas_msg_length() */
#endif /* FEATURE_LTE_NAS_DEEP_SEC */


/*===========================================================================
FUNCTION      REPORT_EMM_SUCCESS

DESCRIPTION
  This function reports successful decoding of DL EMM message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void report_emm_success
(
  lte_nas_incoming_msg_type *nas_incoming_msg_ptr
)
{
  nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.valid = TRUE;
  nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.emm_decode_fail_reason = LTE_NAS_NO_FAILURE;
}


/*===========================================================================
FUNCTION      REPORT_EMM_FAILURE

DESCRIPTION
  This function reports decoding failure of DL EMM message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void report_emm_failure
(
  lte_nas_incoming_msg_type *nas_incoming_msg_ptr
)
{
  nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.valid = FALSE;
  nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.emm_decode_fail_reason =
                                                          LTE_NAS_INVALID_MANDATORY_INFO;
}


/*===========================================================================
FUNCTION      UNKNOWN_EMM_MSG_RECEIVED

DESCRIPTION
  This function reports an unknown DL EMM message received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void unknown_emm_msg_received
(
  lte_nas_incoming_msg_type      *nas_incoming_msg_ptr
)
{
  nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.valid = FALSE;
  nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.emm_decode_fail_reason =
                                                          LTE_NAS_NON_EXISTENT_MSG_TYPE;
}


/*===========================================================================
FUNCTION      REPORT_ESM_FAILURE

DESCRIPTION
  This function reports decoding failure of DL ESM message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void report_esm_failure
(
  lte_nas_incoming_msg_type *nas_incoming_msg_ptr
)
{
  nas_incoming_msg_ptr[nas_esm_index].esm_incoming_msg.nas_hdr.valid = FALSE;
  nas_incoming_msg_ptr[nas_esm_index].esm_incoming_msg.nas_hdr.esm_decode_fail_reason =
                                                          NAS_ESM_INVALID_MANDATORY_INFORMATION;
}


/*===========================================================================
FUNCTION      REPORT_ESM_SUCCESS

DESCRIPTION
  This function reports successful decoding of DL ESM message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void report_esm_success
(
  lte_nas_incoming_msg_type *nas_incoming_msg_ptr
)
{
  nas_incoming_msg_ptr[nas_esm_index].esm_incoming_msg.nas_hdr.valid = TRUE; 
  nas_incoming_msg_ptr[nas_esm_index].esm_incoming_msg.nas_hdr.esm_decode_fail_reason =
                                                                          NAS_ESM_NO_FAILURE;
} /* end report_esm_success() */


/*===========================================================================
FUNCTION      CALC_GPRS_TIMER

DESCRIPTION
  This function decodes GPRS timer IE

DEPENDENCIES
  None

RETURN VALUE
  Timer value in milliseconds

SIDE EFFECTS
  None
===========================================================================*/
dword calc_gprs_timer
(
  byte gprs_timer_octet
)
{
  byte    timer_value_unit = 0;
  dword   timer_value = 0;

  /* Bits 6 to 8 defines the timer value unit for the GPRS timer as follows:
     Bits
     8 7 6
     0 0 0 value is incremented in multiples of 2 seconds
     0 0 1 value is incremented in multiples of 1 minute
     0 1 0 value is incremented in multiples of decihours
     1 1 1 value indicates that the timer is deactivated.   */
  timer_value_unit = gprs_timer_octet >> 5;

  /* Bits 5 to 1 represent the binary coded timer value. */
  timer_value = (dword)(gprs_timer_octet & 0x1F);

  /* Convert timer value to ms */
  switch(timer_value_unit)
  {
    case 0:
      timer_value = (timer_value * 2) * 1000;
      break;

    case 1:
      timer_value = (timer_value * 60) * 1000;
      break;

    case 2:
      timer_value = (timer_value * 6 * 60) * 1000;
      break;

    case 7:
      timer_value = 0;
      break;

    default:
      timer_value = (timer_value * 60) * 1000;
      break;
  } /* end switch(timer_value_unit) */

  return timer_value;
} /* end calc_gprs_timer() */


/*===========================================================================
FUNCTION      REPORT_INVALID_GUTI_OR_NAS_SECURITY

DESCRIPTION:  In order for INTER-RAT to happen, NAS_SECURITY & GUTI must be valid.
              This function reports if NAS_SECURITY OR GUTI are not in sync
              (meaning valid GUTI & invalid SECURITY or vice-versa)
   
DEPENDENCIES
  None

RETURN VALUE
  TRUE: If GUTI and NAS Security valid
  FALSE: If either GUTI or NAS Security or both invalid

SIDE EFFECTS
===========================================================================*/
boolean report_invalid_guti_or_nas_security
(
  boolean                    guti_present,
  lte_nas_incoming_msg_type *nas_incoming_msg_ptr,
  emm_ctrl_data_type        *emm_ctrl_data_ptr
)
{
  boolean valid_guti_sec_ctxt = TRUE;

  if(emm_ctrl_data_ptr->avoid_guti_nas_security_check == TRUE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= EFS flag is present to avoid GUTI & NAS SECURITY validation check, shall skip this step");
    return valid_guti_sec_ctxt;
  }

  if(guti_present == TRUE)
  {
    if(NULL == emm_get_context_by_state(emm_ctrl_data_ptr, CONTEXT_STATE_CURRENT))
    {
      MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Received GUTI is valid but CURRENT_SECURITY context is invalid, shall send EMM status message");
      valid_guti_sec_ctxt = FALSE;
    }
  }
  else
  {
    if(emm_ctrl_data_ptr->emm_guti_valid == FALSE)
    {
      MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Neither stored nor received GUTI are valid, shall send EMM status message");
      valid_guti_sec_ctxt = FALSE;
    }
    else
    {
      if(NULL == emm_get_context_by_state(emm_ctrl_data_ptr, CONTEXT_STATE_CURRENT))
      {
        MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Stored GUTI is valid but CURRENT_SECURITY context is invalid, shall send EMM status message");
        valid_guti_sec_ctxt = FALSE;
      }
    }
  }
  if(valid_guti_sec_ctxt == FALSE)
  {
    nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.valid = FALSE;
    nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.emm_decode_fail_reason =
                                                   LTE_NAS_MSG_INCOMPATIBLE_WITH_PROTOCOL_STATE;
  }

  return valid_guti_sec_ctxt;
} /* end report_invalid_guti_or_nas_security() */


/*===========================================================================
FUNCTION      MSG_LIB_SKIP_UNKNOWN_IEI

DESCRIPTION:  This function skips decoding an unknown IEI during an EMM or
              ESM message decoding as per 24.007 section 11.2.4
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void msg_lib_skip_unknown_iei
(
  byte    iei,
  byte    *src,
  word    *pos,
  word    *buffer_length,
  boolean skip_esm,
  word    *esm_len
)
{
  byte num_bits_to_read = 0;
  word iei_len = 0;

  if(iei & 0x80)
  {
  /* Bit 8 of the IEI octet is set to "1" indicates a TV formatted type 1 standard IE
     or a T formatted type 2 IEs. Hence, a 1 valued bit 8 indicates that the whole IE
     is one octet long.*/
     num_bits_to_read = 0;
  }
  else if((iei & 0x78) == 0x78)
  {
   /* Bit 8 of the IEI octet set to "0" and bits 7 to 4 set to "1" indicates a TLV-E
      formatted type 6 IE, i.e. the following two octets are length octets. */
    num_bits_to_read = 16;
  }
  else
  {
   /* Bit 8 of the IEI octet set to "0" and bit 7 to 4 set to any other bit combination
      indicates a TLV formatted type 4 IE, i.e. the following octet is a length octet */
    num_bits_to_read = 8;
  }

  if (num_bits_to_read <= *buffer_length )
  {
   if(num_bits_to_read == 8)
   {
     iei_len = b_unpackb(src, *pos, num_bits_to_read);
   }
   else if(num_bits_to_read == 16)
   {
     iei_len = b_unpackw(src, *pos, num_bits_to_read);
   }
  }
  
  if (*buffer_length <= num_bits_to_read + iei_len*8)
  {
      *buffer_length = 0;
      if(esm_len != NULL)
      {
         *esm_len = 0;
      }
      MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Unable to skip the unknown IEI, discarding the rest");
      return;
  }
  /* IEI is already read. Skip bits for number of length octets + IE value part */
  *pos = *pos + num_bits_to_read + iei_len*8;
  *buffer_length = *buffer_length - num_bits_to_read - iei_len*8;

  if(skip_esm == TRUE)
  {
    if(*esm_len >= num_bits_to_read + iei_len*8)
    {
      *esm_len = *esm_len - num_bits_to_read - iei_len*8;
    }
    else
    {
      *esm_len = 0;
      *buffer_length = 0;
    }
  }
} /* end msg_lib_skip_unknown_iei() */


/*===========================================================================
FUNCTION      ADJUST_OFFSET_AND_BUF_LEN_ESM

DESCRIPTION:  This function adjusts the poistion to be read from the outer buffer
              the lenghth of the remaining buffer and the lenght of the esm buffer
   
DEPENDENCIES
  None

RETURN VALUE
  boolean     TRUE if there are still bytes to be read in the buffer. 
              FALSE otherwise

SIDE EFFECTS
===========================================================================*/
boolean ADJUST_OFFSET_AND_BUF_LEN_ESM(word num_bits_read,word* offset_ptr,word* buffer_len_ptr,word* esm_len_ptr)
{    
    word max_val = (word)-1;

    if(*buffer_len_ptr <= num_bits_read)
    {       
       /* We are at the end of the outer buffer or we will be pointing outside the buffer
       ** The offset will not be advanced
       */
        *buffer_len_ptr = 0;
        /* leave as is *offset_ptr = 0; */
        *esm_len_ptr = 0;
        return FALSE;
    }
    else
    {
       /* We are still in the buffer, guaranteed not to wrap around */
       *buffer_len_ptr = *buffer_len_ptr - num_bits_read;   
       /* Check for arithmentic overflow only and advance the offset */
       if (*offset_ptr < max_val - num_bits_read)
       {
          *offset_ptr = *offset_ptr + num_bits_read;
       }
    }
    /* We are still in the buffer , ensure no wraparound */
    if (*esm_len_ptr <= num_bits_read)
    {
       /* We are at the end of the ESM buffer or we will be pointing outside the buffer
       ** The offset will not be advanced. The while() condition will take care of it
       */       
        *esm_len_ptr = 0;
    }
    else
    {
       *esm_len_ptr = *esm_len_ptr - num_bits_read;
    }
    return TRUE;
}

#endif /* FEATURE_LTE */