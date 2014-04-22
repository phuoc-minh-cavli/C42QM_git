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

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/msg_lib_decode_emm.c#1 $
$Author: pwbldsvc $
$DateTime: 2023/04/25 14:39:18 $

when         who                          what                              
--------     --------  ------------------------------------------------
08/01/11      Rajesh  T3442 timer related code changes
12/13/10       abhi      Deleted lte_nas_emm_add_update_result_type; used sys_lte_cs_capability_e_type
05/14/10      RI      Mainlined spec changes upto Dec 09.
01/22/10      hnam    Updated the KSI handling.
01/21/10      zren    Added EMM/ESM STATUS feature
10/05/09      hnam     spare bit parsed for security context
09/02/09      hnam     Featurized the TAU accept (based on MAR <->DEC spec)
07/30/09      hnam     Corrected EMM cause validation in MT detach
07/23/09      hnam     Called send_nas_ota_msg_log_packet() after decoding 
                       the length of ESM container
07/14/09      hnam     Moved calc_gprs_timer() to "msg_lib_declarations.c"
07/07/09      hnam     Fixed the decoding of TAI in TAU ACCEPT
06/22/09      hnam     Corrected security mode command parsing & EMM CAUSE parsing
06/16/09      hnam     Corrected the parsing in "decode_security_mode_cmd"
06/04/09      zren     Added code to parse GPRS timer
05/17/09      hnam     Added the code to support security
04/13/09      hnam     Added authentication request code.
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
#include "msg_lib_decode_emm.h"
#include "msg_lib_decode_esm.h"
#include "msg_lib_decode_common_emm_ie.h"
#include "lte_nas_emm_message_type.h"
#include "lte_nas_emm_message_description.h"
#include "msg_lib_declarations.h"
#include "emm_gps_if_msg.h"
#include "msg.h"
#include "err.h"
#include "lte_nas_log.h"
#include "naslog_v.h"
#include "lte_log_codes.h"
#include "amssassert.h"
#include "mm_v.h"
#include "emm_cnm_handler.h"
#ifdef FEATURE_LTE_REL10
#include "gmm_int_v.h"
#endif
#include "stringl.h"
#include "emm_utility.h"
/*===========================================================================
                        FUNCTION DEFINITIONS
===========================================================================*/

#if defined (FEATURE_LTE_REL9) 
#ifdef FEATURE_NAS_GPS_ENABLE
/*===========================================================================
FUNCTION      DECODE_DL_GENERIC_NAS_TRANSPORT

DESCRIPTION
  This function decodes DOWNLINK GENERIC NAS TRANSPORT message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
boolean decode_dl_generic_nas_transport
(
  byte                                 *src,
  word                                 *pos,
  word                                 *buffer_length,
  lte_nas_emm_dl_generic_nas_transport *dl_generic_nas_transport
)
{
  byte num_bits_to_read = 0, unpacked_bytes = 0;
  word unpacked_word = 0;
  boolean valid_ie = TRUE;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif

  if(*buffer_length < (NAS_EMM_DL_GENERIC_NAS_TRANSPORT_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode DOWNLINK GENERIC NAS TRANSPORT",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /***** Generic message container type IE *****/
  num_bits_to_read = 8;
  unpacked_bytes = b_unpackb(src, *pos, num_bits_to_read);

  if((unpacked_bytes < LPP_MSG_CONTAINER) ||
     (unpacked_bytes > LOCATION_SERVICE_MSG_CONTAINER))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Invalid Generic message container type = %d received. Valid range 1-2",unpacked_bytes);
    *buffer_length = 0;
    return FALSE;
  }
  dl_generic_nas_transport->generic_msg_container_type = unpacked_bytes;
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /***** Generic message container IE *****/
  /* Length */
  num_bits_to_read = 16;
  unpacked_word = b_unpackw(src, *pos, num_bits_to_read);

  if ((*buffer_length/8) <= unpacked_word)
  {
      /* length in OTA is corrupted */
      *buffer_length = 0;
	  MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Insufficient length of the buffer");
      return FALSE;
  }

  if(unpacked_word < MIN_GENERIC_MSG_CONTAINER_LENGTH)
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Invalid Generic message container length = %d received",
                   unpacked_word);
    *buffer_length = 0;
    return FALSE;
  }
  dl_generic_nas_transport->generic_msg_container.container_length = unpacked_word;
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
 
  /* Generic message container value is packed into a DSM item */
  dl_generic_nas_transport->generic_msg_container.container_value_ptr = src + (*pos/8);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(dl_generic_nas_transport->generic_msg_container.container_length*8,
                                pos, buffer_length);

                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*buffer_length)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

    /* IE length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /* Store offset address & remaining buffer length, in case any octet
    is faulty this info would be required to jump to next IEI */
    iei_pos = *pos + 8;
    iei_buffer_len = *buffer_length - 8;

    switch(iei_val)
    {
      /***** Additional information IE *****/
      case EMM_ADDITIONAL_INFORMATION_IEI:
        if(opt_iei_index == 1)
        {
          valid_ie = decode_additional_info_ie(src,
                                               pos,
                                               buffer_length,
                                               &dl_generic_nas_transport->additional_info);

          if(valid_ie == FALSE)
          {
            dl_generic_nas_transport->additional_info_exists = FALSE;
          }
          else
          {
            dl_generic_nas_transport->additional_info_exists = TRUE;
          }
          opt_iei_index = 2;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Additional Information IE, opt_iei_idx = %d",
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
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, FALSE, 0);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
      }
      else
      {
        if(iei_buffer_len > (iei_len*8))
        {
          *buffer_length = iei_buffer_len - iei_len*8;
          *pos = iei_pos + iei_len*8;
        }
        else
        {
          *buffer_length = 0;
        }
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*buffer_length) */

  return TRUE;
} /* end decode_dl_generic_nas_transport() */
#endif
#endif
#if defined (FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE)
/*===========================================================================
FUNCTION      DECODE_CS_SERV_NOTIFICATION

DESCRIPTION
  This function decodes CS SERVICE NOTIFICATION message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
boolean decode_cs_serv_notification
(
  byte                             *src,
  word                             *pos,
  word                             *buffer_length, 
  lte_nas_emm_cs_serv_notification *cs_serv_notification
)
{
  byte num_bits_to_read = 0;
  byte index = 0;
  byte unpackedbytes = 0;
  byte extra_length = 0;
  boolean valid_ie = TRUE;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif


  if(*buffer_length < (NAS_EMM_CS_SERV_NOTIFICATION_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode CS SERVICE NOTIFICATION",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /***** 7 Spare bits + Paging identity IE *****/
  num_bits_to_read = 1;
  cs_serv_notification->paging_id = b_unpackb(src, (word)(*pos+7), num_bits_to_read);

  /* Adjust buffer length */
  num_bits_to_read = 8;
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*buffer_length)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

    /* IE length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /* Store offset address & remaining buffer length, in case any octet
    is faulty this info would be required to jump to next IEI */
    iei_pos = *pos + 8;
    iei_buffer_len = *buffer_length - 8;

    switch(iei_val)
    {
      /***** CLI IE *****/
      case CALLING_LINE_INFO_IEI:
        if(opt_iei_index == 1)
        {
          /* Ajust buffer length for the IE length octet */
          num_bits_to_read = 8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

          if(iei_len > (*buffer_length/8))
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d",(*buffer_length/8));
            buffer_length_check_failed = TRUE;
            cs_serv_notification->calling_line_info_exists = FALSE;
            valid_ie = FALSE;
            break;
          }

          if(iei_len < MIN_CLI_LENGTH)
          {
            MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received CLI len %d < %d Min allowed",
                           iei_len, MIN_CLI_LENGTH);
            valid_ie = FALSE;
            cs_serv_notification->calling_line_info_exists = FALSE;
            break;
          }
  
          if(iei_len > MAX_CLI_LENGTH )
          {
            MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received CLI IE len %d > Max allowed. Decoding upto Max",
                           iei_len, MAX_CLI_LENGTH);
            extra_length = iei_len - MAX_CLI_LENGTH;
            iei_len = MAX_CLI_LENGTH;
          }


          /* Decode remaining octets in CLI IE as per TS 24.008, section 10.5.4.9 Calling party BCD number */

          /* Skip 1 ext bit and read 3 bits Type of number directly */
          num_bits_to_read = 3;

          unpackedbytes = b_unpackb(src, (word)(*pos+1), num_bits_to_read);
          if(unpackedbytes > DEDICATED_ACCESS_SHORT_CODE)
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Invalid Type of number = %d received. Valid range is 0-4",
                           unpackedbytes);
            valid_ie = FALSE;
            cs_serv_notification->calling_line_info_exists = FALSE;
            break;
          }

          cs_serv_notification->calling_line_info.number_type = unpackedbytes;

          /* Read 4 bits Numbering plan identification directly */
          num_bits_to_read = 4;
          cs_serv_notification->calling_line_info.numbering_plan_identification =
                                                       b_unpackb(src, (word)(*pos+4), num_bits_to_read);

          /* Ajust buffer length for the last octet read */
          num_bits_to_read = 8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

          iei_len--;

          if(iei_len > 0)
          {
            /* Skip 1 ext bit and read 2 bits Presentation indicator directly */
            num_bits_to_read = 2;

            unpackedbytes = b_unpackb(src, (word)(*pos+1), num_bits_to_read);
            if(unpackedbytes > NUMBER_UNAVAILABLE_DUE_TO_INTERWORKING)
            {
              MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Presentation indicator = %d is reserved",
                             unpackedbytes);
	          valid_ie = FALSE;
              cs_serv_notification->calling_line_info_exists = FALSE;
              break;
            }
            cs_serv_notification->calling_line_info.presentation_ind = unpackedbytes;

            /* Skip 3 spare bits and read 2 bits Screening indicator directly */
            num_bits_to_read = 2;

            cs_serv_notification->calling_line_info.screen_ind =
                                                      b_unpackb(src, *pos, num_bits_to_read);

            /* Ajust buffer length for the last octet read */
            num_bits_to_read = 8;
            ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
		  
            iei_len--;

            /***** Number digits *****/
            index = 0;
            cs_serv_notification->calling_line_info.number_of_digits = 0;
            num_bits_to_read = 4;
            while((iei_len > 0) && (index + 1 < MAX_CLI_DIGITS))
            {
              cs_serv_notification->calling_line_info.digits[index] =
                                                b_unpackb(src, (word)(*pos+4), num_bits_to_read);
              cs_serv_notification->calling_line_info.digits[index+1] =
                                                b_unpackb(src, *pos, num_bits_to_read);
              index += 2;
              iei_len--;
              cs_serv_notification->calling_line_info.number_of_digits += 2;
              /* Ajust buffer length for the last octet read */
              ADJUST_OFFSET_AND_BUF_LEN_EMM(8, pos, buffer_length);
            }
          }
          else
          {
            /* If octet 3a is omitted the value "00 - Presentation allowed" is assumed */
            cs_serv_notification->calling_line_info.presentation_ind = PRESENTATION_ALLOWED;

            /* If octet 3a is omitted the value "0 0 - User provided, not screened" is assumed */
            cs_serv_notification->calling_line_info.screen_ind = USER_PROVIDED_UNSCREENED;
            break;
          }

          /* Discard additional octets in CLI IE. */
          if(extra_length > 0)
          {
            MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in CLI IE", extra_length);
            ADJUST_OFFSET_AND_BUF_LEN_EMM(extra_length*8, pos, buffer_length);
          }

          cs_serv_notification->calling_line_info_exists = TRUE;
          valid_ie = TRUE;
          opt_iei_index = 2;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated CLI IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      /***** SS Code IE *****/
      case SUPPLIMENTARY_SERV_CODE_IEI:
        if(opt_iei_index <= 2)
        {
          num_bits_to_read = 8;
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          if((unpackedbytes != CALLING_LINE_IDENTIFICATION_PRESENTATION) &&
             (unpackedbytes != CALLING_LINE_IDENTIFICATION_RESTRICTION) &&
             (unpackedbytes != CONNECTED_LINE_IDENTIFICATION_PRESENTATION) &&
             (unpackedbytes != CONNECTED_LINE_IDENTIFICATION_RESTRICTION) &&
             (unpackedbytes != ALL_NAME_IDENTIFICATION) &&
             (unpackedbytes != CALLING_NAME_PRESENTATION) &&
             (unpackedbytes != ALL_FORWARDING) &&
             (unpackedbytes != CALL_FORWARDING_UNCONDITIONAL) &&
             (unpackedbytes != ALL_CONDITIONAL_FORWARDING) &&
             (unpackedbytes != CALL_FWD_MOBILE_SUBSCRIBER_BUSY) &&
             (unpackedbytes != CALL_FWD_NO_REPLY) &&
             (unpackedbytes != CALL_FWD_MOBILE_SUBSCRIBER_UNREACHABLE) &&
             (unpackedbytes != CALL_DEFLECTION) &&
             (unpackedbytes != EXPLICIT_CALL_TRANSFER) &&
             (unpackedbytes != SS_CALL_WAITING) &&
             (unpackedbytes != CALL_HOLD) &&
             (unpackedbytes != CALL_COMPLETION_TO_BUSY_SUBS_ORIG_SIDE) &&
             (unpackedbytes != CALL_COMPLETION_TO_BUSY_SUBS_DEST_SIDE) &&
             (unpackedbytes != MULTI_CALL) &&
             (unpackedbytes != MULTI_PARTY) &&
             (unpackedbytes != CLOSED_USER_GROUP) &&
             (unpackedbytes != ADVICE_OF_CHARGE_INFORNMATION) &&
             (unpackedbytes != ADVICE_OF_CHARGE_CHARGING) &&
             (unpackedbytes != USER_TO_USER_SIGNALLING_1) &&
             (unpackedbytes != USER_TO_USER_SIGNALLING_2) &&
             (unpackedbytes != USER_TO_USER_SIGNALLING_3) &&
             (unpackedbytes != ALL_BARRING) &&
             (unpackedbytes != BARRING_OF_OUTGOING_CALLS) &&
             (unpackedbytes != BARRING_OF_ALL_OUTGOING_CALLS) &&
             (unpackedbytes != BARRING_OF_ALL_OUTGOING_INTERNATIONAL_CALLS) &&
             (unpackedbytes != BARRING_OF_ALL_OUTGOING_INT_CALLS_EXCEPT_DIRECTED_TO_HPLMN_COUNTRY) &&
             (unpackedbytes != BARRING_OF_INCOMING_CALLS) &&
             (unpackedbytes != BARRING_OF_ALL_INCOMING_CALLS) &&
             (unpackedbytes != BARRING_OF_ALL_INCOMING_CALL_EXCEPT_ROAMING_OUTSIDE_HPLMN_COUNTRY) &&
             (unpackedbytes != ENHANCED_MULTILEVEL_PRESEDENCE_PREEMPTION_SERVICE) &&
             (unpackedbytes != ALL_LCS_PRIVACY_EXCEMPTION) &&
             (unpackedbytes != UNIVERSAL) &&
             (unpackedbytes != CALL_SESSION_RELATED) &&
             (unpackedbytes != CALL_SESSION_UNRELATED) &&
             (unpackedbytes != PLMN_OPERATOR) &&
             (unpackedbytes != SERVICE_TYPE) &&
             (unpackedbytes != ALL_MO_LOCATION_REQUEST_CLASS) &&
             (unpackedbytes != BASIC_SELF_LOCATION) &&
             (unpackedbytes != AUTONOMOUS_SELF_LOCATION) &&
             (unpackedbytes != TRANSFER_TO_THIRD_PARTY) &&
             (unpackedbytes != ALL_PLMN_SPECIFIC_SS) &&
             (unpackedbytes != PLMN_SPECIFIC_SS_1) &&
             (unpackedbytes != PLMN_SPECIFIC_SS_2) &&
             (unpackedbytes != PLMN_SPECIFIC_SS_3) &&
             (unpackedbytes != PLMN_SPECIFIC_SS_4) &&
             (unpackedbytes != PLMN_SPECIFIC_SS_5) &&
             (unpackedbytes != PLMN_SPECIFIC_SS_6) &&
             (unpackedbytes != PLMN_SPECIFIC_SS_7) &&
             (unpackedbytes != PLMN_SPECIFIC_SS_8) &&
             (unpackedbytes != PLMN_SPECIFIC_SS_9) &&
             (unpackedbytes != PLMN_SPECIFIC_SS_A) &&
             (unpackedbytes != PLMN_SPECIFIC_SS_B) &&
             (unpackedbytes != PLMN_SPECIFIC_SS_C) &&
             (unpackedbytes != PLMN_SPECIFIC_SS_D) &&
             (unpackedbytes != PLMN_SPECIFIC_SS_E) &&
             (unpackedbytes != PLMN_SPECIFIC_SS_F))
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Invalid SS Code value = %d. Decoding of SS Code IE failed",
                           unpackedbytes);
            cs_serv_notification->supl_serv_code_exists = FALSE;
            /* SS code IE is of 2 octets and format Type TV. It doesn't have Length
               part so it cannot be skipped correctly after the switch block. Hence set valid_ie
               to TRUE and skip the IE right here */
            valid_ie = TRUE;
          }
          else
          {
            cs_serv_notification->supl_serv_code = unpackedbytes;
            cs_serv_notification->supl_serv_code_exists = TRUE;
            valid_ie = TRUE;
          }
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
          opt_iei_index = 3;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated SS Code IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* SS code IE is of 2 octets and format Type TV. It doesn't have Length
             part so it cannot be skipped correctly after the switch block. Hence set valid_ie
             to TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = 8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        }
        break;

      /***** LCS indicator IE *****/
      case LCS_IND_IEI:
        if(opt_iei_index <= 3)
        {
          MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Decoding LCS Indicator IE");
          num_bits_to_read = 8;
          cs_serv_notification->lcs_ind =  (lte_nas_lcs_ind_type)b_unpackb(src, *pos, num_bits_to_read);
          cs_serv_notification->lcs_ind_exists = TRUE;
          valid_ie = TRUE;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
          opt_iei_index = 4;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated SS Code IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* LCS indicator IE is of 2 octets and format Type TV. It doesn't have Length
             part so it cannot be skipped correctly after the switch block. Hence set valid_ie
             to TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = 8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
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
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, FALSE, 0);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
      }
      else
      {
        if(iei_buffer_len > (iei_len*8))
        {
          *buffer_length = iei_buffer_len - iei_len*8;
          *pos = iei_pos + iei_len*8;
        }
        else
        {
          *buffer_length = 0;
        }
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*buffer_length) */

  return TRUE;
} /* end decode_cs_serv_notification() */
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
boolean decode_emm_info
(
  byte             *src,
  word             *pos,
  word             *buffer_length,
  lte_nas_emm_info *emm_info
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  boolean valid_ie = TRUE;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif

                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*buffer_length)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

    /* IE length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /* Store offset address & remaining buffer length, in case any octet
    is faulty this info would be required to jump to next IEI */
    iei_pos = *pos + 8;
    iei_buffer_len = *buffer_length - 8;

    switch(iei_val)
    {
      /***** Full name for network IE *****/
      case NW_FULL_NAME_IEI:
        if(opt_iei_index == 1)
        {
          valid_ie = decode_nw_name(src, pos, buffer_length, &emm_info->nw_full_name);
          if(valid_ie == FALSE)
          {
            emm_info->nw_full_name_exists = FALSE;
          }
          else
          {
            emm_info->nw_full_name_exists = TRUE;
          }
          opt_iei_index = 2;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Additional Information IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      /***** Short name for network IE *****/
      case NW_PARTIAL_NAME_IEI:
        if(opt_iei_index <= 2)
        {
          valid_ie = decode_nw_name(src, pos, buffer_length, &emm_info->nw_partial_name);
          if(valid_ie == FALSE)
          {
            emm_info->nw_partial_name_exists = FALSE;
          }
          else
          {
            emm_info->nw_partial_name_exists = TRUE;
          }
          opt_iei_index = 3;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Additional Information IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      /***** Local time zone IE *****/
      case LOCAL_TIME_ZONE_IEI:
        if(opt_iei_index <= 3)
        {
          num_bits_to_read = 8;
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          emm_info->local_time_zone = mm_convert_time_zone(unpackedbytes);
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
          emm_info->local_time_zone_exists = TRUE;
          valid_ie = TRUE;
          opt_iei_index = 4;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Additional Information IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* Local time zone IE is of 2 octets and format Type TV. It doesn't have Length
             part so it cannot be skipped correctly after the switch block. Hence set valid_ie
             to TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = 8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        }
        break;

      /***** Universal time and local time zone IE *****/
      case UNIV_LOCAL_TIME_ZONE_IEI:
        if(opt_iei_index <= 4)
        {
          num_bits_to_read = 8;

          emm_info->tp_serv_center_time_stamp.year =
                       mm_convert_semi_octets(b_unpackb(src, *pos, num_bits_to_read));

          emm_info->tp_serv_center_time_stamp.month =
                       mm_convert_semi_octets(b_unpackb(src, (word)(*pos+8), num_bits_to_read));

          emm_info->tp_serv_center_time_stamp.day =
                       mm_convert_semi_octets(b_unpackb(src, (word)(*pos+16), num_bits_to_read));

          emm_info->tp_serv_center_time_stamp.hour =
                       mm_convert_semi_octets(b_unpackb(src, (word)(*pos+24), num_bits_to_read));

          emm_info->tp_serv_center_time_stamp.minute =
                       mm_convert_semi_octets(b_unpackb(src, (word)(*pos+32), num_bits_to_read));

          emm_info->tp_serv_center_time_stamp.second =
                       mm_convert_semi_octets(b_unpackb(src, (word)(*pos+40), num_bits_to_read));
    
          emm_info->tp_serv_center_time_stamp.time_zone =
                       mm_convert_time_zone(b_unpackb(src, (word)(*pos+48), num_bits_to_read));
      
          emm_info->tp_serv_center_time_stamp_exists = TRUE;
          valid_ie = TRUE;
          num_bits_to_read = 7*8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
          opt_iei_index = 5;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Additional Information IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* Universal time and local time zone IE is of 8 octets and format Type TV. It doesn't
             have Length part so it cannot be skipped correctly after the switch block. Hence set
             valid_ie to TRUE and skip the IE right here */
          valid_ie = TRUE;
          /* There are 7 octets in value part of the IE. IEI part is already read, hence skip 7*8 bits */
          num_bits_to_read = 7*8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        }
        break;

      /***** Network daylight saving time IE *****/
      case NW_DAYLIGHT_SAVINGS_TIME_IEI:
        if(opt_iei_index <= 5)
        {
          valid_ie = decode_daylight_saving_time_ie(src,
                                                    pos,
                                                    buffer_length,
                                                    &emm_info->day_light_savings);
          if(valid_ie == FALSE)
          {
            emm_info->day_light_savings_exists = FALSE;
          }
          else
          {
            emm_info->day_light_savings_exists = TRUE;
          }
          opt_iei_index = 6;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Network daylight saving time IE, opt_iei_idx = %d",
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
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, FALSE, 0);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
      }
      else
      {
        if(iei_buffer_len > (iei_len*8))
        {
          *buffer_length = iei_buffer_len - iei_len*8;
          *pos = iei_pos + iei_len*8;
        }
        else
        {
          *buffer_length = 0;
        }
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*buffer_length) */

  return TRUE;
} /* end decode_emm_info() */


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
boolean decode_guti_realloc_cmd
(
  byte                         *src,
  word                         *pos, 
  word                         *buffer_length,
  lte_nas_emm_guti_realloc_cmd *guti_realloc_cmd
)
{
  byte num_bits_to_read = 0;
  boolean valid_ie = TRUE;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif


  if(*buffer_length < (NAS_EMM_GUTI_REALLOC_CMD_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode GUTI REALLOCATION COMMAND",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /***** GUTI IE *****/
  valid_ie = decode_guti_ie(src, pos, buffer_length, &guti_realloc_cmd->guti);
  if(valid_ie == FALSE)
  {
    MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Invalid GUTI received");
    *buffer_length = 0;
    return FALSE;
  }

                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*buffer_length)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

    /* IE length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /* Store offset address & remaining buffer length, in case any octet
    is faulty this info would be required to jump to next IEI */
    iei_pos = *pos + 8;
    iei_buffer_len = *buffer_length - 8;

    switch(iei_val)
    {
      /***** TAI list IE *****/
      case TAI_LIST_IEI:
        if(opt_iei_index == 1)
        {
          valid_ie = decode_tai_list_ie(src, pos, buffer_length, &guti_realloc_cmd->tai_lst);
          if(valid_ie == FALSE)
          {
            guti_realloc_cmd->tai_list_exists = FALSE;
          }
          else
          {
            guti_realloc_cmd->tai_list_exists = TRUE;
          }
          opt_iei_index = 2;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated TAI list IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
      case DCN_ID_IEI:
	  	if(opt_iei_index <= 2)
        {
          valid_ie = decode_dcn_id_ie(src, pos, buffer_length, &guti_realloc_cmd->dcn_id);
          if(valid_ie == FALSE)
          {
            guti_realloc_cmd->dcn_id_valid= FALSE;
          }
          else
          {
            guti_realloc_cmd->dcn_id_valid = TRUE;
          }
          opt_iei_index = 3;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated DCN IE, opt_iei_idx = %d",
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
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, FALSE, 0);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
      }
      else
      {
        if(iei_buffer_len > (iei_len*8))
        {
          *buffer_length = iei_buffer_len - iei_len*8;
          *pos = iei_pos + iei_len*8;
        }
        else
        {
          *buffer_length = 0;
        }
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*buffer_length) */

  return TRUE;
} /* end decode_guti_realloc_cmd() */


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
boolean decode_identity_req
(
  byte                     *src,
  word                     *pos,
  word                     *buffer_length,
  lte_nas_emm_identity_req *identity_req
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;


  if(*buffer_length < (NAS_EMM_IDENTITY_REQUEST_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode IDENTITY REQUEST",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /***** Spare half octet + Identity type IE *****/
  num_bits_to_read = 4;

  unpackedbytes = b_unpackb(src, (word)(*pos+4), num_bits_to_read);
  if(unpackedbytes > (byte)LTE_NAS_EMM_TMSI)
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Invalid Identity type value = %d received. Valid range 0-4",
                   unpackedbytes);
    *buffer_length = 0;
    return FALSE;
  }
  identity_req->mobile_identity = (mobile_id_type) unpackedbytes;
  num_bits_to_read = 8;
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  return TRUE;
} /* end decode_identity_req() */


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
boolean decode_tau_reject
(
  byte                *src,
  word                *pos,
  word                *buffer_length,
  lte_nas_emm_tau_rej *tau_rej
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  boolean valid_ie = TRUE;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  if(*buffer_length < (NAS_EMM_TAU_REJECT_MIN_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode TRACKING AREA UPDATE REJECT",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /***** EMM Cause IE *****/
  num_bits_to_read = 8;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
  if((unpackedbytes <(byte)LTE_NAS_IMSI_UNKNOWN_IN_HSS)||
     (unpackedbytes > (byte)LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Unknown EMM cause value = %d received", unpackedbytes);
    *buffer_length = 0;
    return FALSE;
  }

  tau_rej->emm_cause = (lte_nas_emm_cause_type) unpackedbytes;
  MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= EMM cause value = %d", tau_rej->emm_cause);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
 
                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*buffer_length)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

    /* IE length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /* Store offset address & remaining buffer length, in case any octet
    is faulty this info would be required to jump to next IEI */
    iei_pos = *pos + 8;
    iei_buffer_len = *buffer_length - 8;
 
    switch(iei_val)
    {
#ifdef FEATURE_LTE_REL10
      /***** T3346 value IE *****/
      case T3346_VAL_IEI:
        if(opt_iei_index == 1)
        {
          if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
          {
            valid_ie = decode_gprs_timer2_ie(src, pos, buffer_length, &tau_rej->t3346_val);

            if(valid_ie == TRUE)
            {
              /* If t3346 value is not zero/deactivated and received message is not
                 integrity protected use a random value b/w 15-30 mins */
              if((tau_rej->t3346_val != 0) &&
                 (tau_rej->security_hdr != INTEGRITY_PROTECT_CIPHER) &&
                 (tau_rej->security_hdr != INTEGRITY_PROTECT))
              {
                tau_rej->t3346_val =((15 + mm_get_random_value(15)) * 60 * 1000);
                MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= Random T3346 value = %d ms, sec_hdr = %d",
                              tau_rej->t3346_val,
                              tau_rej->security_hdr);
              }
              else
              {
                MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= T3346 value = %d ms, sec_hdr = %d",
                              tau_rej->t3346_val,
                              tau_rej->security_hdr);
              }
            }
            opt_iei_index = 2;
          }
          else
          {
            MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3346 value IE unsupported in UE rel %d",
                         emm_ctrl_data_ptr->emm_3gpp_spec_ver);
            valid_ie = FALSE;
          }
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated T3346 value IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif /* end FEATURE_LTE_REL10 */

      default:
#ifdef FEATURE_LTE_REL12 /* CR 141624 */
        if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL12)&&
                (EXT_EMM_CAUSE_IEI == (byte)(0xF0 & iei_val)))
        {        
           if(opt_iei_index <= 2)
            {
                 tau_rej->eutran_not_allowed = iei_val & 0x1;
#ifdef FEATURE_CIOT
                 tau_rej->ciot_opt_info_present = iei_val & 0x2;
                 MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= EXT_EMM_CAUSE_IEI value is %d ", tau_rej->ciot_opt_info_present);
#endif
                 valid_ie = TRUE;  
                 MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= EXT_EMM_CAUSE_IEI value is %d ", tau_rej->eutran_not_allowed);
                 opt_iei_index = 3;
                break;
            }
           else
           {
               MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= EXT_EMM_CAUSE_IEI value IE unsupported in UE rel %d",
                          emm_ctrl_data_ptr->emm_3gpp_spec_ver);
               valid_ie = FALSE;
           }
        }
        else
#endif
        {
            /* Skip unknown IEs, 24.301 - 7.6.1 */
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
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, FALSE, 0);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
      }
      else
      {
        if(iei_buffer_len > (iei_len*8))
        {
          *buffer_length = iei_buffer_len - iei_len*8;
          *pos = iei_pos + iei_len*8;
        }
        else
        {
          *buffer_length = 0;
        }
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*buffer_length) */

  return TRUE;
} /* end decode_tau_reject() */


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
boolean decode_tau_accept
(
  byte                        *src,
  word                        *pos,
  word                        *buffer_length,
  lte_nas_emm_tau_accept_type *tau_accept
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  boolean valid_ie = TRUE, plmn_len = 3;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif

  if(*buffer_length < (NAS_EMM_TAU_ACCEPT_MIN_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode TRACKING AREA UPDATE ACCEPT",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /***** Spare half octet + EPS update result IE *****/
  /* Skip all 5 spare bits and read 3 bits for EPS update result value directly */
  num_bits_to_read = 3;
  unpackedbytes = b_unpackb(src, (word)(*pos+5), num_bits_to_read);
  if((unpackedbytes == (byte)TA_UPDATE)||(unpackedbytes <= (byte)TA_LA_UPDATE)||
     (unpackedbytes == (byte)TA_UPDATE_ISR_ATIVATE)||
     (unpackedbytes == (byte)TA_LA_UPDATE_ISR_ACTIVATE))
  {
    tau_accept->eps_update_result = (lte_nas_emm_eps_update_result_type) unpackedbytes;
  }
  else
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received reserved EPS update result value = %d",
                   unpackedbytes);
    *buffer_length = 0;
    return FALSE;
  }

  /* Adjust buffer length */
  num_bits_to_read = 8;
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*buffer_length)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

    /* IE length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /* Store offset address & remaining buffer length, in case any octet
    is faulty this info would be required to jump to next IEI */
    iei_pos = *pos + 8;
    iei_buffer_len = *buffer_length - 8;
 
    switch(iei_val)
    {
      /***** T3412 value IE *****/
      case T3412_TIMER_IEI:
        if(opt_iei_index == 1)
        {
          num_bits_to_read = 8;

          if(*buffer_length < num_bits_to_read)
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d bits",
                          *buffer_length);
            buffer_length_check_failed = TRUE;
            tau_accept->t3412_val_exists = FALSE;
            valid_ie = FALSE;
            break;
          }

          /* Read timer octet to decode T3412 timer unit and value */
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          tau_accept->t3412_val = calc_gprs_timer(unpackedbytes);
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
          tau_accept->t3412_val_exists = TRUE;
          valid_ie = TRUE;
          MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3412 value IE decoded successfully, T3412 value = %d",tau_accept->t3412_val);
          opt_iei_index = 2;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated T3412 value IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* T3412 value IE is of 2 octets and format Type TV. It doesn't have Length
             part so it cannot be skipped correctly after the switch block. Hence set valid_ie
             to TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = 8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        }
        break;

      /***** GUTI IE *****/
      case GUTI_IEI:
        if(opt_iei_index <= 2)
        {
          valid_ie = decode_guti_ie(src, pos, buffer_length, &tau_accept->guti);
          if(valid_ie == FALSE)
          {
            tau_accept->guti_type_exists = FALSE;
          }
          else
          {
            tau_accept->guti_type_exists = TRUE;
          }
          opt_iei_index = 3;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated GUTI IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      /***** TAI list IE *****/
      case TAI_LIST_IEI:
        if(opt_iei_index <= 3)
        {
          valid_ie = decode_tai_list_ie(src, pos, buffer_length, &tau_accept->tai_lst);
          if(valid_ie == FALSE)
          {
            tau_accept->tai_lst_exists = FALSE;
          }
          else
          {
            tau_accept->tai_lst_exists = TRUE;
          }
          opt_iei_index = 4;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated TAI list IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      /***** EPS bearer context status IE *****/
      case EPS_BEARER_CONTEXT_STATUS_IEI:
        if(opt_iei_index <= 4)
        {
          valid_ie = decode_eps_bearer_context_status(src,
                                                      pos,
                                                      buffer_length,
                                                      &tau_accept->eps_bearer_context_status);
          if(valid_ie == FALSE)
          {
            tau_accept->eps_bearer_context_status_exists = FALSE;
          }
          else
          {
            tau_accept->eps_bearer_context_status_exists = TRUE;
          }
          opt_iei_index = 5;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated EPS bearer context status list IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      /***** Location area identification IE *****/
      case LOC_AREA_ID_IEI:
        if(opt_iei_index <= 5)
        {
          if((*buffer_length/8) < LOCATION_AREA_IDENTITY_LEN)
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d bytes",
                           (*buffer_length/8));
            buffer_length_check_failed = TRUE;
            tau_accept->loc_area_id_exists = FALSE;
            valid_ie = FALSE;
            break;
          }

          /***** Decode PLMN *****/
          num_bits_to_read = 8;
          tau_accept->loc_area_id.PLMN_id.identity[0] =
                                      b_unpackb(src, *pos, num_bits_to_read);
          tau_accept->loc_area_id.PLMN_id.identity[1] =
                                      b_unpackb(src, (word)(*pos+8), num_bits_to_read);
          tau_accept->loc_area_id.PLMN_id.identity[2] =
                                      b_unpackb(src, (word)(*pos+16), num_bits_to_read);

          /* Adjust buffer length for PLMN octets read */
          ADJUST_OFFSET_AND_BUF_LEN_EMM((num_bits_to_read * plmn_len), pos, buffer_length);

          if(is_plmn_bcd_coded(tau_accept->loc_area_id.PLMN_id) == FALSE)
          {
            MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= PLMN not BCD coded");
            /* Location area identification IE is of 6 octets and format Type TV. It doesn't have
             Length part so it cannot be skipped correctly after the switch block. Hence set
             valid_ie to TRUE and skip the IE right here. 3 octets for PLMN are already read, so
             skip remaining 2 octets */
            tau_accept->loc_area_id_exists = FALSE;
            valid_ie = TRUE;
            num_bits_to_read = (LOCATION_AREA_IDENTITY_LEN - plmn_len)*8;
            ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
            break;
          }

          /***** Decode Location area code *****/
          num_bits_to_read = 8;
          tau_accept->loc_area_id.location_area_code[0] = b_unpackb(src, *pos, num_bits_to_read);
          tau_accept->loc_area_id.location_area_code[1] =
                                                b_unpackb(src, (word)(*pos+8), num_bits_to_read);
          tau_accept->loc_area_id_exists = TRUE;
          /* Adjust buffer length for 2 octets */
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read*2, pos, buffer_length);
          valid_ie = TRUE;
          opt_iei_index = 6;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Location area identification IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* Location area identification IE is of 6 octets and format Type TV. It doesn't have
             Length part so it cannot be skipped correctly after the switch block. Hence set
             valid_ie to TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = LOCATION_AREA_IDENTITY_LEN*8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        }
        break;

      /***** MS identity IE *****/
      case MS_IDENTITY_IEI:
        if(opt_iei_index <= 6)
        {
          valid_ie = decode_ms_identity_ie(src, pos, buffer_length, &tau_accept->ms_id);
          if(valid_ie == FALSE)
          {
            tau_accept->ms_id_exists = FALSE;
          }
          else
          {
            tau_accept->ms_id_exists = TRUE;
          }
          opt_iei_index = 7;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated MS identity IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      /***** EMM cause IE *****/
      case EMM_CAUSE_IEI:
        if(opt_iei_index <= 7)
        {
          num_bits_to_read = 8;
          if(*buffer_length < num_bits_to_read)
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d bits",
                          *buffer_length);
            buffer_length_check_failed = TRUE;
            tau_accept->emm_cause_exists = FALSE;
            valid_ie = FALSE;
            break;
          }

          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          if((unpackedbytes < (byte)LTE_NAS_IMSI_UNKNOWN_IN_HSS)||
            (unpackedbytes > (byte)LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED))
          { 
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Unknown EMM cause value = %d received",
                           unpackedbytes);
            tau_accept->emm_cause_exists = FALSE;
            /* EMM cause IE is of 2 octets and format Type TV. It doesn't have Length
               part so it cannot be skipped correctly after the switch block. Hence set valid_ie
               to TRUE and skip the IE right here */
            valid_ie = TRUE;
            num_bits_to_read = 8;
            ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
          }
          else
          {
            tau_accept->emm_cause = (lte_nas_emm_cause_type) unpackedbytes;
            MSG_MED_DS_1(MM_SUB, "=NAS_MSG_LIB= EMM cause value = %d", tau_accept->emm_cause);
            num_bits_to_read = 8;
            ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
            tau_accept->emm_cause_exists = TRUE;
            valid_ie = TRUE;
            MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= EMM cause IE decoded successfully");
          }
          opt_iei_index = 8;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated EMM cause IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* EMM cause IE is of 2 octets and format Type TV. It doesn't have Length
             part so it cannot be skipped correctly after the switch block. Hence set valid_ie
             to TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = 8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        }
        break;

      /***** T3402 value IE *****/
      case T3402_VAL_IEI:
        if(opt_iei_index <= 8)
        {
          num_bits_to_read = 8;

          if(*buffer_length < num_bits_to_read)
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d bits",
                          *buffer_length);
            buffer_length_check_failed = TRUE;
            tau_accept->t3402_val_exists = FALSE;
            valid_ie = FALSE;
            break;
          }

          /* Read timer octet to decode T3402 timer unit and value */
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          tau_accept->t3402_val = calc_gprs_timer(unpackedbytes);
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
          tau_accept->t3402_val_exists = TRUE;
          valid_ie = TRUE;
          MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3402 value = %d ",tau_accept->t3402_val);
          opt_iei_index = 9;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated T3402 value IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* T3402 value IE is of 2 octets and format Type TV. It doesn't have Length
             part so it cannot be skipped correctly after the switch block. Hence set valid_ie
             to TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = 8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        }
        break;

      /***** T3423 value IE *****/
      case T3423_VAL_IEI:
        if(opt_iei_index <= 9)
        {
          num_bits_to_read = 8;

          if(*buffer_length < num_bits_to_read)
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d bits",
                          *buffer_length);
            buffer_length_check_failed = TRUE;
            tau_accept->t3423_val_exists = FALSE;
            valid_ie = FALSE;
            break;
          }

          /* Read timer octet to decode T3423 timer unit and value */
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          tau_accept->t3423_val = calc_gprs_timer(unpackedbytes);
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
          tau_accept->t3423_val_exists = TRUE;
          valid_ie = TRUE;
          MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3423 value = %d",tau_accept->t3423_val);
          opt_iei_index = 10;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated T3423 value IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* T3423 value IE is of 2 octets and format Type TV. It doesn't have Length
             part so it cannot be skipped correctly after the switch block. Hence set valid_ie
             to TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = 8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        }
        break;

      /***** Equivalent PLMNs IE *****/
      case EQUIVALENT_PLMN_IEI:
        if(opt_iei_index <= 10)
        {
          valid_ie = decode_eplmn_list_ie(src, pos, buffer_length, &tau_accept->plmn_lst);
          if(valid_ie == FALSE)
          {
            tau_accept->plmn_exists = FALSE;
          }
          else
          {
            tau_accept->plmn_exists = TRUE;
          }
          opt_iei_index = 11;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Equivalent PLMNs IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#if !defined(FEATURE_NAS_IMS_VOICE_DISABLED) || !defined(FEATURE_NAS_DISABLE_VOICE)
      /***** Emergency number list IE *****/
      case EMERGENCY_NUM_LIST_IEI:
        if(opt_iei_index <= 11)
        {
          valid_ie = decode_emergency_num_list(src,
                                               pos,
                                               buffer_length,
                                              (byte)iei_len,
                                               &tau_accept->emergency_num_list);
          if(valid_ie == FALSE)
          {
            tau_accept->emergency_num_list_exists = FALSE;
          }
          else
          {
            tau_accept->emergency_num_list_exists = TRUE;
          }
          opt_iei_index = 12;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Emergency number list IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif

      /***** EPS network feature support IE *****/
      case EPS_NW_FEATURE_SUPPORT_IEI:
        if(opt_iei_index <= 12)
        {
          valid_ie = decode_eps_nw_feat_spt_ie(src,
                                               pos,
                                               buffer_length,
                                               &tau_accept->eps_nw_feature_support);
          if(valid_ie == FALSE)
          {
            tau_accept->eps_nw_feature_support_exists = FALSE;
          }
          else
          {
            tau_accept->eps_nw_feature_support_exists = TRUE;
          }
          opt_iei_index = 13;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated EPS network feature support IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

#ifdef FEATURE_LTE_REL10
      /***** T3412 extended value IE *****/
      case T3412_EXT_VAL_IEI:
        if(opt_iei_index <= 14)
        {
          /* Read octet containing timer unit and value in advance */
          num_bits_to_read = 8;
          unpackedbytes = b_unpackb(src, (word)(*pos+8), num_bits_to_read);
          valid_ie = decode_gprs_timer3_ie(src, pos, buffer_length, &tau_accept->t3412_ext_val);

          if(valid_ie == TRUE)
          {
            tau_accept->t3412_ext_val_exists = TRUE;
            /* From 24.008 10.5.7.4a - If T3412 extended value IE is received in an integrity
               protected message, value shall be interpreted as multiples of 320 hours. 
               Otherwise value shall be interpreted as multiples of 1 hour */
            if((tau_accept->security_hdr != INTEGRITY_PROTECT_CIPHER) &&
               (tau_accept->security_hdr != INTEGRITY_PROTECT))
            {
              tau_accept->t3412_ext_val = 3600000 * (unpackedbytes & 0x1F) ;
            }
            else if((unpackedbytes >> 5) == 7) /* timer is deactivated */
            {
              tau_accept->t3412_ext_val_exists = FALSE;
            }

            if(tau_accept->t3412_ext_val_exists == TRUE)
            {
              MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3412 extended value = %d",tau_accept->t3412_ext_val);
            }
          }
          else
          {
            tau_accept->t3412_ext_val_exists = FALSE;
          }
          opt_iei_index = 15;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated T3412 extended value IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif
      /***** T3324 value IE *****/
      case T3324_VAL_IEI:
        if(opt_iei_index <= 15)
        {
          /* Read octet containing timer unit and value in advance */
          num_bits_to_read = 8;
          unpackedbytes = b_unpackb(src, (word)(*pos+8), num_bits_to_read);
          valid_ie = decode_gprs_timer2_ie(src, pos, buffer_length, &tau_accept->t3324_val);

          if(valid_ie == TRUE)
          {
            if((unpackedbytes >> 5) == 7) /* Timer is deactivated */
            {
              tau_accept->t3324_val_exists = FALSE;
              MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Timer T3324 is deactivated");
            }
            else
            {
              MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3324 value = %d ms", tau_accept->t3324_val);
              tau_accept->t3324_val_exists = TRUE;
            }
          }
          opt_iei_index = 16;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated T3324 value IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
      /***** Extended DRX parameters IE *****/
      case EDRX_PARAMS_IEI:
        if(opt_iei_index <= 16)
        {
          valid_ie = decode_edrx_params_ie(src,
                                           pos,
                                           buffer_length,
                                           &tau_accept->edrx_params);
          if(valid_ie == FALSE)
          {
            tau_accept->edrx_params_exists = FALSE;
          }
          else
          {
            tau_accept->edrx_params_exists = TRUE;
          }
          opt_iei_index = 17;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Extended DRX parameters IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
      /***** Header compression configuration status IE *****/
      case HC_CONFIG_STATUS_IEI:
#ifdef FEATURE_CIOT
        if(opt_iei_index <= 17)
        {
          valid_ie = decode_hc_config_status_ie(src,
                                                pos,
                                                buffer_length,
                                                &tau_accept->hc_config_status); 
          if(valid_ie == FALSE)
          {
              tau_accept->hc_config_status_exists = FALSE; 
          }
          else
          {
            tau_accept->hc_config_status_exists = TRUE;
          }
          opt_iei_index = 18;
        }
        else
#endif
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated header compression configuration status IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
       case DCN_ID_IEI:
	     if(opt_iei_index <= 18)  
	     {
	       valid_ie = decode_dcn_id_ie(src,
                                      pos,
                                      buffer_length,
                                      &tau_accept->dcn_id);
	      if(valid_ie == FALSE)
          {
              tau_accept->dcn_id_exist= FALSE; 
          }
          else
          {
            tau_accept->dcn_id_exist = TRUE;
          }
          opt_iei_index = 19;
        }
	    else
	    {
		  /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated header compression configuration status IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
	    }
	    break;
	   case T3448_VAL_IEI:
#ifdef FEATURE_CIOT
	  	if(opt_iei_index <= 21)
	  	{
          /* Read octet containing timer unit and value in advance */
          num_bits_to_read = 8;
          unpackedbytes = b_unpackb(src, (word)(*pos+8), num_bits_to_read);
          valid_ie = decode_gprs_timer2_ie(src, pos, buffer_length, &tau_accept->t3448_val);
          if(valid_ie == TRUE)
          {
            if((unpackedbytes >> 5) == 7 || tau_accept->t3448_val == 0 ) /* Timer is deactivated */
            {
              tau_accept->t3448_val_exists = FALSE;
              MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Timer T3448 is deactivated");
            }
            else
            {
              MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3448 value = %d ms", tau_accept->t3448_val);
              tau_accept->t3448_val_exists = TRUE;
            }
          }
          opt_iei_index = 22;
	  	}
		else
#endif
	    {
		  /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated header compression configuration status IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
	    }
	   break;
      default:
        /***** Additional update result IE *****/
        /* IEI value is present in bits 5-8 of the octet that is already read.
           So just mask the bits to get the value */
        if(ADDITIONAL_UPDATE_RESULT_IEI == (byte)(0xF0 & iei_val))
        {
          if(opt_iei_index <= 13)
          {
            MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Decoding Additional update result IE");
            /* Additional update result value is present in bits 1-2 of the octet
               that is already read. So just mask the bits to get the value */
            if(0x03 != (sys_lte_cs_capability_e_type)(0x03 & iei_val))
            {
              tau_accept->additional_update_result =
                                (sys_lte_cs_capability_e_type)(0x03 & iei_val);

              tau_accept->additional_update_result_exists = TRUE;
              MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Additional update result value = %d",
                                    tau_accept->additional_update_result);

            }
            else
            {
              MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Additional update result value = %d is reserved",
                             (0x03 & iei_val));
              tau_accept->additional_update_result_exists = FALSE;
            }
            /* Additional update result IE is of 1 byte and format TV. It doesn't have Length
               part so it cannot be skipped correctly after the switch block if decoding failed.
               Hence set valid_ie to TRUE. iei_val is already read so no need to adjust buffer
               length and position */
            valid_ie = TRUE;
            opt_iei_index = 14;
          }
          else
          {
            /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Additional update result IE, opt_iei_idx = %d",
                           opt_iei_index);
            /* Additional update result IE is of 1 byte and format TV. It doesn't have Length
                         part so it cannot be skipped correctly after the switch block. Hence set valid_ie
                        to TRUE. iei_val is already read so no need to adjust buffer length and position */
            valid_ie = TRUE;
           }  
      }
      else if(SMS_SERVICE_STATUS_IEI== (byte)(0xF0 & iei_val))
	  {
#ifdef FEATURE_CIOT
              if(opt_iei_index <= 19)
              {
                 MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Decoding SMS STATUS IE %d",iei_val);
				/* SMS services status value (octet 1, bit 1 to 3)  */
                if((sys_cp_sms_service_status_e_type)(0x0F & iei_val) <= 0x03 )
                {
                  tau_accept->sms_service_status = (sys_cp_sms_service_status_e_type)(0x0F & iei_val);
                  tau_accept->sms_service_exists= TRUE;
                  MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= SMS STATUS value = %d",
											   (0x0F & iei_val));
                }
                else
                {
                  MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= SMS STATUS value = %d is reserved",
											   (0x0F & iei_val));
                  tau_accept->sms_service_exists = FALSE;
                }
                valid_ie = TRUE;
	            opt_iei_index = 20;
               }
               else
               {
	         /* SMS service status IE is of 1 byte and format TV. It doesn't have Length
                    part so it cannot be skipped correctly after the switch block. Hence set valid_ie
                    to TRUE. iei_val is already read so no need to adjust buffer length and position */
                 MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated SMS STATUS IEI, opt_iei_idx = %d",
										   opt_iei_index);
                 valid_ie = TRUE;
               }
#endif
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
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, FALSE, 0);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
      }
      else
      {
        if(iei_buffer_len > (iei_len*8))
        {
          *buffer_length = iei_buffer_len - iei_len*8;
          *pos = iei_pos + iei_len*8;
        }
        else
        {
          *buffer_length = 0;
        }
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*buffer_length) */

  return TRUE;
} /* end decode_tau_accept() */


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
boolean decode_detach_request
(
  byte                           *src,
  word                           *pos,
  word                           *buffer_length,
  lte_nas_emm_mt_detach_req_type *mt_detach
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  boolean valid_ie = TRUE;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif

  if(*buffer_length < (NAS_EMM_MT_DETACH_REQ_MIN_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode DETACH REQUEST", 
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /***** Spare half octet + Detach type IE *****/
  num_bits_to_read = 4;
  mt_detach->detach_req =
          (lte_nas_emm_mt_detach_type) b_unpackb(src, (word)(*pos+4), num_bits_to_read);

  switch(mt_detach->detach_req)
  {
    case REATTACH_REQUIRED:
    case REATTACH_NOT_REQUIURED:
    case IMSI_DETACH_MT_DETACH:
      break;

    case RESERVED_MT_DETACH_TYPE_ONE:
    case RESERVED_MT_DETACH_TYPE_TWO:
      MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Detach type = %d is reserved", mt_detach->detach_req);
      *buffer_length = 0;
      return FALSE;

    default:
      MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Interpreting Detach type = %d as re-attach not required",
                    mt_detach->detach_req);
      mt_detach->detach_req = REATTACH_NOT_REQUIURED;
      break;
  } /* end switch(mt_detach->detach_req) */

  /* Adjust buffer length */
  num_bits_to_read = 8;
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*buffer_length)
  { 
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

    /* IE length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /* Store offset address & remaining buffer length, in case any octet 
    is faulty this info would be required to jump to next IEI */
    iei_pos = *pos + 8;
    iei_buffer_len = *buffer_length - 8;
 
    switch(iei_val)
    {
      /***** EMM cause IE *****/
      case EMM_CAUSE_IEI:
        if(opt_iei_index == 1)
        {
          num_bits_to_read = 8;

          if(*buffer_length < num_bits_to_read)
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d bits",
                          *buffer_length);
            buffer_length_check_failed = TRUE;
            mt_detach->emm_cause_exists = FALSE;
            valid_ie = FALSE;
            break;
          }

          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          if(!(((unpackedbytes == (byte)LTE_NAS_IMSI_UNKNOWN_IN_HSS) ||
                (unpackedbytes == (byte)LTE_NAS_ILLEGAL_UE)) ||
               ((unpackedbytes >= (byte)LTE_NAS_ILLEGAL_ME) &&
                (unpackedbytes <= (byte)LTE_NON_EPS_AUTH_UNACCEPTABLE)) ||
               ((unpackedbytes >= (byte)LTE_NAS_CS_FALLBACK_CALL_EST_NOT_ALLOWED) &&
                (unpackedbytes <= (byte)LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED)) ||
               ((unpackedbytes >= (byte)LTE_NAS_SEMANTICALLY_INCORRECT_MSG) &&
                (unpackedbytes <= (byte)LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED))))
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Unknown EMM cause value = %d received",
                           unpackedbytes);
            mt_detach->emm_cause_exists = FALSE;
            /* EMM cause IE is of 2 octets and format Type TV. It doesn't have Length
               part so it cannot be skipped correctly after the switch block. Hence set valid_ie
               to TRUE and skip the IE right here */
            valid_ie = TRUE;
            num_bits_to_read = 8;
            ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
            break;
          }

          mt_detach->emm_cause = (lte_nas_emm_cause_type) unpackedbytes;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
          mt_detach->emm_cause_exists = TRUE;
          opt_iei_index = 2;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated EMM cause IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* EMM cause IE is of 2 octets and format Type TV. It doesn't have Length
             part so it cannot be skipped correctly after the switch block. Hence set valid_ie
             to TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = 8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
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
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, FALSE, 0);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
      }
      else
      {
        /* This block will not be reached currently since there are no optional IEs
           of TLV format present in MT Detach Request. In future versions of the spec
           if any optional IEs of format TLV are added then this block is needed to skip
           faulty TLV IEs */
        if(iei_buffer_len > (iei_len*8))
        {
          *buffer_length = iei_buffer_len - iei_len*8;
          *pos = iei_pos + iei_len*8;
        }
        else
        {
          *buffer_length = 0;
        }
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*buffer_length) */

  return TRUE;
} /* end decode_detach_request() */


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
boolean decode_attach_reject
(
  byte                        *src,
  word                        *pos,
  word                        *buffer_length,
  lte_nas_emm_attach_rej_type *attach_reject,
  lte_nas_incoming_msg_type   *nas_incoming_msg_ptr
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  boolean valid_ie = TRUE;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;
  boolean valid_esm_ie = FALSE;
  word esm_len = 0;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  if(*buffer_length < (NAS_EMM_ATTACH_REJECT_MIN_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode ATTACH REJECT",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /***** EMM Cause IE *****/
  num_bits_to_read = 8;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
  if((unpackedbytes <(byte)LTE_NAS_IMSI_UNKNOWN_IN_HSS)||
     (unpackedbytes > (byte)LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Unknown EMM cause value = %d received", unpackedbytes);
    *buffer_length = 0;
    return FALSE;
  }

  attach_reject->emm_cause = (lte_nas_emm_cause_type) unpackedbytes;
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*buffer_length)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

    /* IE length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /* Store offset address & remaining buffer length, in case any octet
    is faulty this info would be required to jump to next IEI */
    iei_pos = *pos + 8;
    iei_buffer_len = *buffer_length - 8;
 
    switch(iei_val)
    {
      /***** ESM message container IE *****/
      case ESM_IEI:
        if(opt_iei_index == 1)
        {
          if(*buffer_length >= MIN_ESM_CONTAINER_LEN)
          {
            /***** ESM message container IE length *****/
            num_bits_to_read = 16;
            esm_len = b_unpackw(src, *pos, num_bits_to_read);
            if (esm_len > (*buffer_length/8))
            {
              MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d",(*buffer_length/8));
              buffer_length_check_failed = TRUE;
              valid_ie = FALSE;
              break;
            }
            ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
            send_nas_ota_msg_log_packet(LOG_LTE_NAS_ESM_OTA_IN_MSG_LOG_C,*buffer_length,src + (*pos/8));
            nas_esm_index++;
            valid_esm_ie = decode_esm_msg(src,
                                          pos,
                                          buffer_length,
                                          esm_len,
                                          &nas_incoming_msg_ptr[nas_esm_index]);
            /* Attach reject can only come with ESM container with valid PDN connectivity reject */
            if((attach_reject->emm_cause == LTE_NAS_ESM_FAILURE) &&
               ((valid_esm_ie == FALSE) ||
                (nas_incoming_msg_ptr[nas_esm_index].esm_incoming_msg.nas_hdr.msg_id != PDN_CONNECTIVITY_REJECT)))
            {
              MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= EMM reject cause #19 came without PDN conn reject, dropping ATTACH REJECT message");
              report_esm_failure(nas_incoming_msg_ptr);
              return FALSE;
            }
            else if(valid_esm_ie == FALSE)
            {       
              valid_ie = FALSE;
              report_esm_failure(nas_incoming_msg_ptr);
              MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM decoding failed");
            }
            else
            {
              valid_ie = TRUE;
              report_esm_success(nas_incoming_msg_ptr);
            }
            opt_iei_index = 2;
          }
          else
          {
            valid_ie = FALSE;
            buffer_length_check_failed = TRUE;
          }
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated ESM message container IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

#ifdef FEATURE_LTE_REL10
      /***** T3346 value IE *****/
      case T3346_VAL_IEI:
        if(opt_iei_index <= 2)
        {
          if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
          {
            valid_ie = decode_gprs_timer2_ie(src, pos, buffer_length, &attach_reject->t3346_val);

            if(valid_ie == TRUE)
            {
              /* If t3346 value is not zero/deactivated and received message is not
                 integrity protected use a random value b/w 15-30 mins */
              if((attach_reject->t3346_val != 0) &&
                 (attach_reject->security_hdr != INTEGRITY_PROTECT_CIPHER) &&
                 (attach_reject->security_hdr != INTEGRITY_PROTECT))
              {
                attach_reject->t3346_val =((15 + mm_get_random_value(15)) * 60 * 1000);
                MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= Random T3346 value = %d ms, sec_hdr = %d",
                             attach_reject->t3346_val,
                             attach_reject->security_hdr);
              }
              else
              {
                MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= T3346 value = %d ms, sec_hdr = %d",
                             attach_reject->t3346_val,
                             attach_reject->security_hdr);
              }
            }
            opt_iei_index = 3;
          }
          else
          {
            MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3346 value IE unsupported in UE rel %d",
                         emm_ctrl_data_ptr->emm_3gpp_spec_ver);
            valid_ie = FALSE;
          }
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated T3346 value IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      /***** T3402 value IE *****/
      case T3402_VAL_ATTACH_REJECT_IEI:
        if(opt_iei_index <= 3)
        {
          if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
          {
            valid_ie = decode_gprs_timer2_ie(src, pos, buffer_length, &attach_reject->t3402_val);

            if(valid_ie == TRUE)
            {
              /* if message is integrity protected use the value provided */
              if((attach_reject->security_hdr == INTEGRITY_PROTECT_CIPHER) || 
                 (attach_reject->security_hdr == INTEGRITY_PROTECT))
              {
                MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3402 value is %d ms", attach_reject->t3402_val);
                attach_reject->t3402_val_exists = TRUE;
              }
              else
              {
                attach_reject->t3402_val_exists = FALSE;
              }
            }
            else
            {
              attach_reject->t3402_val_exists = FALSE;
            }
            opt_iei_index = 4;
          }
          else
          {
            MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3402 value IE unsupported in UE rel %d",
                         emm_ctrl_data_ptr->emm_3gpp_spec_ver);
            valid_ie = FALSE;
          }
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated T3402 value IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif /* end FEATURE_LTE_REL10 */

      default:
     /* CR 141624 */
#ifdef FEATURE_LTE_REL12
        if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL12)&&
                (EXT_EMM_CAUSE_IEI == (byte)(0xF0 & iei_val)))
        {
            if(opt_iei_index <= 4)
            {
                 attach_reject->eutran_not_allowed = iei_val & 0x1;
#ifdef FEATURE_CIOT
                 attach_reject->ciot_opt_info_present = iei_val & 0x2;
                 MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= EXT_EMM_CAUSE_IEI value is %d ", attach_reject->ciot_opt_info_present);
#endif
				 
                 valid_ie = TRUE;  
                 MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= EXT_EMM_CAUSE_IEI value is %d ", attach_reject->eutran_not_allowed);
                 opt_iei_index = 5;
                break;
            }        
            else
            {
                MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= EXT_EMM_CAUSE_IEI value IE unsupported in UE rel %d",
                          emm_ctrl_data_ptr->emm_3gpp_spec_ver);
                valid_ie = FALSE;
            }
        }
        else
#endif
        {
            /* Skip unknown IEs, 24.301 - 7.6.1 */
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
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, FALSE, 0);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
      }
      else
      {
        if(iei_val == ESM_IEI)
        {
          num_bits_to_read = 16;
          esm_len = b_unpackw(src, (word)(iei_pos - 8), num_bits_to_read);
          if((iei_buffer_len + 8) > (esm_len*8 + num_bits_to_read))
          {
            *buffer_length = (iei_buffer_len + 8) - num_bits_to_read - esm_len*8;
            *pos = (iei_pos - 8) + num_bits_to_read + esm_len*8;  
          }
          else
          {
            *buffer_length = 0;
          }
        }
        else
        {
          if(iei_buffer_len > (iei_len*8))
          {
            *buffer_length = iei_buffer_len - iei_len*8;
            *pos = iei_pos + iei_len*8;
          }
          else
          {
            *buffer_length = 0;
          }
        }
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*buffer_length) */

  /* If EMM reject cause indicates ESM failure then network must include ESM IE
     with valid PDN connectivity reject */
  if((attach_reject->emm_cause == LTE_NAS_ESM_FAILURE) &&
     (valid_esm_ie == FALSE))
  {
    MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= EMM reject cause #19 came without PDN conn reject, dropping attach reject message");
    return FALSE;
  }

  return TRUE;
} /* end decode_attach_reject() */


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
boolean decode_attach_accept
(
  byte                           *src,
  word                           *pos,
  word                           *buffer_length,
  lte_nas_emm_attach_accept_type *attach_accept,
  struct emm_ctrl_data_tag       *emm_ctrl_data_ptr,
  lte_nas_incoming_msg_type      *nas_incoming_msg_ptr
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  boolean valid_ie = TRUE, plmn_len = 3;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;
  word esm_len = 0;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif


  if(*buffer_length < (NAS_EMM_ATTACH_ACCEPT_MIN_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode ATTACH ACCEPT",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /***** Spare half octet + EPS attach result IE *****/
  /* Skip all 5 spare bits and read 3 bits for EPS attach result value directly */
  num_bits_to_read = 3;
  unpackedbytes = b_unpackb(src, (word)(*pos+5), num_bits_to_read);

  if((unpackedbytes == (byte)EPS_ONLY_ATTACH_RESULT)||
     (unpackedbytes == (byte)COMBINED_EPS_IMSI_ATTACH_RESULT))
  {
    attach_accept->eps_attach_result = (lte_nas_emm_eps_attach_result_type) unpackedbytes;
  }
  else
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received reserved EPS attach result value = %d",
                   unpackedbytes);
    *buffer_length = 0;
    return FALSE;
  }

  /* Adjust buffer length */
  num_bits_to_read = 8;
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /***** T3412 value IE *****/
  num_bits_to_read = 8;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
  attach_accept->t3412_val = calc_gprs_timer(unpackedbytes);
  MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3412 value = %d ms", attach_accept->t3412_val);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /***** TAI list IE *****/
  valid_ie = decode_tai_list_ie(src, pos, buffer_length, &attach_accept->tai_lst);
  if(valid_ie == FALSE)
  {
    *buffer_length = 0;
    return FALSE;
  }
  if (0 == *buffer_length)
  {
      return FALSE;
  }
  
  /***** ESM message container IE length *****/
  num_bits_to_read = 16;
  esm_len = b_unpackw(src, *pos, num_bits_to_read);
  if (esm_len*8 >= *buffer_length)
  {
      *buffer_length = 0;
      MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Insufficient length of the buffer");
      return FALSE;
  }
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
  nas_esm_index++;
  send_nas_ota_msg_log_packet(LOG_LTE_NAS_ESM_OTA_IN_MSG_LOG_C,
                              (word)(*buffer_length/8),src + (*pos/8));
  valid_ie = decode_esm_msg(src,pos,buffer_length,esm_len,&nas_incoming_msg_ptr[nas_esm_index]);
  if(valid_ie == FALSE)
  {
    MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM decoding failed");
    report_esm_failure(nas_incoming_msg_ptr);
    *buffer_length = 0;
    return FALSE;
  }
  else
  {
    report_esm_success(nas_incoming_msg_ptr);
  }

                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*buffer_length)
  { 
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

    /* IE length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /* Store offset address & remaining buffer length, in case any octet
    is faulty this info would be required to jump to next IEI */
    iei_pos = *pos + 8;
    iei_buffer_len = *buffer_length - 8;
 
    switch(iei_val)
    {
      /***** GUTI IE *****/
      case GUTI_IEI:
        if(opt_iei_index == 1)
        {
          valid_ie = decode_guti_ie(src, pos, buffer_length, &attach_accept->guti);
          if(valid_ie == FALSE)
          {
            attach_accept->guti_type_exists = FALSE;
          }
          else
          {
            attach_accept->guti_type_exists = TRUE;
          }
          opt_iei_index = 2;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated GUTI IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      /***** Location area identification IE *****/
      case LOC_AREA_ID_IEI:
        if(opt_iei_index <= 2)
        {
          if((*buffer_length/8) < LOCATION_AREA_IDENTITY_LEN)
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d bytes",
                           (*buffer_length/8));
            buffer_length_check_failed = TRUE;
            attach_accept->loc_area_id_exists = FALSE;
            valid_ie = FALSE;
            break;
          }

          /***** Decode PLMN *****/
          num_bits_to_read = 8;
          attach_accept->loc_area_id.PLMN_id.identity[0] =
                                      b_unpackb(src, *pos, num_bits_to_read);
          attach_accept->loc_area_id.PLMN_id.identity[1] =
                                      b_unpackb(src, (word)(*pos+8), num_bits_to_read);
          attach_accept->loc_area_id.PLMN_id.identity[2] =
                                      b_unpackb(src, (word)(*pos+16), num_bits_to_read);

          /* Adjust buffer length for PLMN octets read */
          ADJUST_OFFSET_AND_BUF_LEN_EMM((num_bits_to_read * plmn_len), pos, buffer_length);

          if(is_plmn_bcd_coded(attach_accept->loc_area_id.PLMN_id) == FALSE)
          {
            MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= PLMN not BCD coded");
            /* Location area identification IE is of 6 octets and format Type TV. It doesn't have
             Length part so it cannot be skipped correctly after the switch block. Hence set
             valid_ie to TRUE and skip the IE right here. 3 octets for PLMN are already read, so
             skip remaining 2 octets */
            attach_accept->loc_area_id_exists = FALSE;
            valid_ie = TRUE;
            num_bits_to_read = (LOCATION_AREA_IDENTITY_LEN - plmn_len)*8;
            ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
            break;
          }

          /***** Decode Location area code *****/
          num_bits_to_read = 8;
          attach_accept->loc_area_id.location_area_code[0] = b_unpackb(src, *pos, num_bits_to_read);
          attach_accept->loc_area_id.location_area_code[1] =
                                                   b_unpackb(src, (word)(*pos+8), num_bits_to_read);
          MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= Location area identification IE decoded successfully, location_area_code[0] = %d, location_area_code[1] = %d",
                       attach_accept->loc_area_id.location_area_code[0],
                       attach_accept->loc_area_id.location_area_code[1]);
          attach_accept->loc_area_id_exists = TRUE;
          MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Location area identification IE decoded successfully");
          /* Adjust buffer length for 2 octets */
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read*2, pos, buffer_length);
          valid_ie = TRUE;
          opt_iei_index = 3;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Location area identification IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* Location area identification IE is of 6 octets and format Type TV. It doesn't have
             Length part so it cannot be skipped correctly after the switch block. Hence set
             valid_ie to TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = LOCATION_AREA_IDENTITY_LEN*8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        }
        break;

      /***** MS identity IE *****/
      case MS_IDENTITY_IEI:
        if(opt_iei_index <= 3)
        {
          valid_ie = decode_ms_identity_ie(src, pos, buffer_length, &attach_accept->ms_id);
          if(valid_ie == FALSE)
          {
            attach_accept->ms_id_exists = FALSE;
          }
          else
          {
            attach_accept->ms_id_exists = TRUE;
          }
          opt_iei_index = 4;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated MS identity IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      /***** EMM cause IE *****/
      case EMM_CAUSE_IEI:
        if(opt_iei_index <= 4)
        {
          num_bits_to_read = 8;
          if(*buffer_length < num_bits_to_read)
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d bits",
                          *buffer_length);
            buffer_length_check_failed = TRUE;
            attach_accept->emm_cause_exists = FALSE;
            valid_ie = FALSE;
            break;
          }

          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          if((unpackedbytes < (byte)LTE_NAS_IMSI_UNKNOWN_IN_HSS) ||
            (unpackedbytes > (byte)LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED))
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Unknown EMM cause value = %d received",
                           unpackedbytes);
            attach_accept->emm_cause_exists = FALSE;
            /* EMM cause IE is of 2 octets and format Type TV. It doesn't have Length
               part so it cannot be skipped correctly after the switch block. Hence set valid_ie
               to TRUE and skip the IE right here */
            valid_ie = TRUE;
            num_bits_to_read = 8;
            ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
          }
          else
          {
            attach_accept->emm_cause = (lte_nas_emm_cause_type) unpackedbytes;
            num_bits_to_read = 8;
            ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
            attach_accept->emm_cause_exists = TRUE;
            valid_ie = TRUE;
          }
          opt_iei_index = 5;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated EMM cause IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* EMM cause IE is of 2 octets and format Type TV. It doesn't have Length
             part so it cannot be skipped correctly after the switch block. Hence set valid_ie
             to TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = 8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        }
        break;

      /***** T3402 value IE *****/
      case T3402_VAL_IEI:
        if(opt_iei_index <= 5)
        {
          num_bits_to_read = 8;

          if(*buffer_length < num_bits_to_read)
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d bits",
                          *buffer_length);
            buffer_length_check_failed = TRUE;
            attach_accept->t3402_val_exists = FALSE;
            valid_ie = FALSE;
            break;
          }

          /* Read timer octet to decode T3402 timer unit and value */
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          attach_accept->t3402_val = calc_gprs_timer(unpackedbytes);
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
          attach_accept->t3402_val_exists = TRUE;
          valid_ie = TRUE;
          MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3402 value = %d",attach_accept->t3402_val);
          opt_iei_index = 6;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated T3402 value IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* T3402 value IE is of 2 octets and format Type TV. It doesn't have Length
             part so it cannot be skipped correctly after the switch block. Hence set valid_ie
             to TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = 8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        }
        break;

      /***** T3423 value IE *****/
      case T3423_VAL_IEI:
        if(opt_iei_index <= 6)
        {
          num_bits_to_read = 8;

          if(*buffer_length < num_bits_to_read)
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d bits",
                           *buffer_length);
            buffer_length_check_failed = TRUE;
            attach_accept->t3423_val_exists = FALSE;
            valid_ie = FALSE;
            break;
          }

          /* Read timer octet to decode T3423 timer unit and value */
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          attach_accept->t3423_val = calc_gprs_timer(unpackedbytes);
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
          attach_accept->t3423_val_exists = TRUE;
          valid_ie = TRUE;
          MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3423 value = %d",attach_accept->t3423_val);
          opt_iei_index = 7;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated T3423 value IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* T3423 value IE is of 2 octets and format Type TV. It doesn't have Length
             part so it cannot be skipped correctly after the switch block. Hence set valid_ie
             to TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = 8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        }
        break;

      /***** Equivalent PLMNs IE *****/
      case EQUIVALENT_PLMN_IEI:
        if(opt_iei_index <= 7)
        {
          valid_ie = decode_eplmn_list_ie(src, pos, buffer_length, &attach_accept->plmn_lst);
          if(valid_ie == FALSE)
          {
            attach_accept->plmn_exists = FALSE;
          }
          else
          {
            attach_accept->plmn_exists = TRUE;
          }
          opt_iei_index = 8;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Equivalent PLMNs IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

#if !defined(FEATURE_NAS_IMS_VOICE_DISABLED) || !defined(FEATURE_NAS_DISABLE_VOICE)
      /***** Emergency number list IE *****/
      case EMERGENCY_NUM_LIST_IEI:
        if(opt_iei_index <= 8)
        {
          valid_ie = decode_emergency_num_list(src,
                                               pos,
                                               buffer_length,
                                              (byte)iei_len,
                                               &attach_accept->emergency_num_list);
          if(valid_ie == FALSE)
          {
            attach_accept->emergency_num_list_exists = FALSE;
          }
          else
          {
            attach_accept->emergency_num_list_exists = TRUE;
          }
          opt_iei_index = 9;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Emergency number list IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif 
      /***** EPS network feature support IE *****/
      case EPS_NW_FEATURE_SUPPORT_IEI:
        if(opt_iei_index <= 9)
        {
          valid_ie = decode_eps_nw_feat_spt_ie(src,
                                               pos,
                                               buffer_length,
                                               &attach_accept->eps_nw_feature_support);
          if(valid_ie == FALSE)
          {
            attach_accept->eps_nw_feature_support_exists = FALSE;
          }
          else
          {
            attach_accept->eps_nw_feature_support_exists = TRUE;
          }
          opt_iei_index = 10;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated EPS network feature support IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

#ifdef FEATURE_LTE_REL10
      /***** T3412 extended value IE *****/
      case T3412_EXT_VAL_IEI:
        if(opt_iei_index <= 11)
        {
          /* Read octet containing timer unit and value in advance */
          num_bits_to_read = 8;
          unpackedbytes = b_unpackb(src, (word)(*pos+8), num_bits_to_read);
          valid_ie = decode_gprs_timer3_ie(src, pos, buffer_length, &attach_accept->t3412_ext_val);

          if(valid_ie == TRUE)
          {

            MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3412 extended value = %d", attach_accept->t3412_ext_val);
            attach_accept->t3412_ext_val_exists = TRUE;
            /* From 24.008 10.5.7.4a - If T3412 extended value IE is received in an integrity
               protected message, value shall be interpreted as multiples of 320 hours. 
               Otherwise value shall be interpreted as multiples of 1 hour */
            if((attach_accept->security_hdr != INTEGRITY_PROTECT_CIPHER) &&
               (attach_accept->security_hdr != INTEGRITY_PROTECT))
            {
              attach_accept->t3412_ext_val = 3600000 * (unpackedbytes & 0x1F) ;
            }
            else if((unpackedbytes >> 5) == 7) /* timer is deactivated */
            {
              attach_accept->t3412_ext_val_exists = FALSE;
            }

            if(attach_accept->t3412_ext_val_exists == TRUE)
            {
              MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3412 extended value = %d",
                            attach_accept->t3412_ext_val);
            }
          }
          else
          {
            attach_accept->t3412_ext_val_exists = FALSE;
          }
          opt_iei_index = 12;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated T3412 extended value IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif
      /***** T3324 value IE *****/
      case T3324_VAL_IEI:
        if(opt_iei_index <= 12)
        {
          /* Read octet containing timer unit and value in advance */
          num_bits_to_read = 8;
          unpackedbytes = b_unpackb(src, (word)(*pos+8), num_bits_to_read);
          valid_ie = decode_gprs_timer2_ie(src, pos, buffer_length, &attach_accept->t3324_val);

          if(valid_ie == TRUE)
          {
            if((unpackedbytes >> 5) == 7) /* Timer is deactivated */
            {
              attach_accept->t3324_val_exists = FALSE;
              MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Timer T3324 is deactivated");
            }
            else
            {
              MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3324 value = %d ms", attach_accept->t3324_val);
              attach_accept->t3324_val_exists = TRUE;
            }
          }
          opt_iei_index = 13;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated T3324 value IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;

      /***** Extended DRX parameters IE *****/
      case EDRX_PARAMS_IEI:
        if(opt_iei_index <= 13)
        {
          valid_ie = decode_edrx_params_ie(src,
                                           pos,
                                           buffer_length,
                                           &attach_accept->edrx_params);
          if(valid_ie == FALSE)
          {
            attach_accept->edrx_params_exists = FALSE;
          }
          else
          {
            attach_accept->edrx_params_exists = TRUE;
          }
          opt_iei_index = 14;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Extended DRX parameters IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
       case DCN_ID_IEI:
        if(opt_iei_index <= 14)
	    {
		  valid_ie = decode_dcn_id_ie(src,
                                      pos,
                                      buffer_length,
                                      &attach_accept->dcn_id);
		  if(valid_ie == FALSE)
          {
            attach_accept->dcn_id_exist= FALSE; 
          }
          else
          {
            attach_accept->dcn_id_exist = TRUE;
          }
          opt_iei_index = 15;
	    }
	    else
	    {
		  /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated header compression configuration status IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
	    }
       break;
	  case T3448_VAL_IEI:
#ifdef FEATURE_CIOT
	  	if(opt_iei_index <= 17)
	  	{
          /* Read octet containing timer unit and value in advance */
          num_bits_to_read = 8;
          unpackedbytes = b_unpackb(src, (word)(*pos+8), num_bits_to_read);
          valid_ie = decode_gprs_timer2_ie(src, pos, buffer_length, &attach_accept->t3448_val);

          if(valid_ie == TRUE)
          {
            if((unpackedbytes >> 5) == 7 || attach_accept->t3448_val == 0 ) /* Timer is deactivated */
            {
              attach_accept->t3448_val_exists = FALSE;
              MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Timer T3448 is deactivated");
            }
            else
            {
              MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3448 value = %d ms", attach_accept->t3448_val);
              attach_accept->t3448_val_exists = TRUE;
            }
          }
          opt_iei_index = 18;
	  	}
		else
#endif
	    {
		  /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated header compression configuration status IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
	    }
	   break;
      default:
        /***** Additional update result IE *****/
        /* IEI value is present in bits 5-8 of the octet that is already read.
           So just mask the bits to get the value */
        if(ADDITIONAL_UPDATE_RESULT_IEI == (byte)(0xF0 & iei_val))
        {
          if(opt_iei_index <= 10)
          {
            MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Decoding Additional update result IE");
            /* Additional update result value is present in bits 1-2 of the octet
               that is already read. So just mask the bits to get the value */
            if(0x03 != (sys_lte_cs_capability_e_type)(0x03 & iei_val))
            {
              attach_accept->additional_update_result =
                                (sys_lte_cs_capability_e_type)(0x03 & iei_val);
              attach_accept->additional_update_result_exists = TRUE;
            }
            else
            {
              MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Additional update result value = %d is reserved",
                             (0x03 & iei_val));
              attach_accept->additional_update_result_exists = FALSE;
            }
            /* Additional update result IE is of 1 byte and format TV. It doesn't have Length
               part so it cannot be skipped correctly after the switch block if decoding failed.
               Hence set valid_ie to TRUE. iei_val is already read so no need to adjust buffer
               length and position */
            valid_ie = TRUE;
            opt_iei_index = 11;
          }
          else
          {
            /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Additional update result IE, opt_iei_idx = %d",
                           opt_iei_index);
            /* Additional update result IE is of 1 byte and format TV. It doesn't have Length
               part so it cannot be skipped correctly after the switch block. Hence set valid_ie
               to TRUE. iei_val is already read so no need to adjust buffer length and position */
            valid_ie = TRUE;
          }
        }
	else if(SMS_SERVICE_STATUS_IEI == (byte)(0xF0 & iei_val))
	{
		
#ifdef FEATURE_CIOT
          if(opt_iei_index <= 15)
          {
              MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Decoding SMS STATUS IE %d ", iei_val);
		      /* SMS services status value (octet 1, bit 1 to 3) */
              if((sys_cp_sms_service_status_e_type)(0x0F & iei_val) <= 0x03)
              {
                  attach_accept->sms_service_status = (sys_cp_sms_service_status_e_type)(0x0F & iei_val);
                  attach_accept->sms_service_exists= TRUE;
                  MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= SMS STATUS value = %d",(0x0F & iei_val));
              }
              else
              {
                 MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= SMS STATUS value = %d is reserved",
							 (0x0F & iei_val));
                 attach_accept->sms_service_exists = FALSE;
              }
              valid_ie = TRUE;
              opt_iei_index = 16;
	      }
	      else
              {
	          /* SMS service status IE is of 1 byte and format TV. It doesn't have Length
		     part so it cannot be skipped correctly after the switch block if decoding failed.
		     Hence set valid_ie to TRUE. iei_val is already read so no need to adjust buffer
		   length and position */
               MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated SMS STATUS IEI, opt_iei_idx = %d",
								 opt_iei_index);
               valid_ie = TRUE;
          }
#endif
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
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, FALSE, 0);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
      }
      else
      {
        if(iei_buffer_len > (iei_len*8))
        {
          *buffer_length = iei_buffer_len - iei_len*8;
          *pos = iei_pos + iei_len*8;
        }
        else
        {
          *buffer_length = 0;
        }
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*buffer_length) */

  if((emm_ctrl_data_ptr->emm_guti_valid == FALSE) &&
     (attach_accept->guti_type_exists == FALSE))
  {
    MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= No GUTI in ATTACH ACCEPT while no valid GUTI on UE");
    attach_accept->lte_nas_hdr.valid = FALSE;
    attach_accept->lte_nas_hdr.emm_decode_fail_reason =
                                 LTE_NAS_MSG_INCOMPATIBLE_WITH_PROTOCOL_STATE;
    nas_esm_index = 0;
    return FALSE;
  }
  else
  {
    return TRUE;
  }
} /* end decode_attach_accept() */


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
boolean decode_security_mode_cmd
(
  byte                              *src,
  word                              *pos,
  word                              *buffer_length,
  lte_nas_emm_security_mode_command *security_mode_cmd
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  byte tsc_bit = 0, ue_security_len = 0;
  byte extra_length = 0, octet = 1;
  boolean valid_ie = TRUE, skip_unknown_ie = FALSE;
  boolean exit_while_loop = FALSE;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  emm_ctrl_data_type  *emm_ctrl_data_ptr = NULL;
  
#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  if(*buffer_length < (NAS_EMM_SECURITY_MODE_CMD_MIN_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode SECURITY MODE COMMAND",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /***** Selected NAS security algorithms IE *****/
  /* Skip 1 spare bit and read 3 bits of Type of ciphering alogorithm value directly */
  num_bits_to_read = 3;
  security_mode_cmd->nas_security_alg.ciphering_alg =
           (lte_nas_emm_ciphering_alg_type) b_unpackb(src, (word)(*pos+1), num_bits_to_read);

  /*Skip 1 spare bit and read 3 bits of Type of integrity alogorithm value directly */
  num_bits_to_read = 3;
  security_mode_cmd->nas_security_alg.integrity_protect_alg =
           (lte_nas_emm_integrity_protect_alg_type) b_unpackb(src, (word)(*pos+5), num_bits_to_read);

  /* Adjust buffer length for the entire previous octet read */
  num_bits_to_read = 8;
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /***** NAS key set identifier IE *****/

  /* Skip 4 spare bits and read TSC bit directly */
  num_bits_to_read = 1;
  tsc_bit = b_unpackb(src, (word)(*pos+4), num_bits_to_read);

  /* Read 3 bits of NAS key set identifier value directly */
  num_bits_to_read = 3;
  unpackedbytes = b_unpackb(src, (word)(*pos+5), num_bits_to_read);

  if(unpackedbytes == NO_KEY_AVAIALABLE)
  {
    MSG_ERROR_DS_2(MM_SUB, "NAS_MSG_LIB: Received invalid KSI = %d, TSC = %d", unpackedbytes, tsc_bit);
    *buffer_length = 0;
    return FALSE;
  }

  if(tsc_bit == 1)
  {
    security_mode_cmd->nas_key_set_identifier_sgsn.security_context = 1;
    security_mode_cmd->nas_key_set_identifier_sgsn.keyset_id = unpackedbytes;
  }
  else
  {
    security_mode_cmd->nas_key_set_identifier_sgsn.security_context = 0;
    security_mode_cmd->nas_key_set_identifier_asme.keyset_id = unpackedbytes;
  }

  /* Adjust buffer length for the entire previous octet read */
  num_bits_to_read = 8;
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /***** Replayed UE security capabilities IE *****/

  /* Length of Replayed UE security capabilities IE */
  num_bits_to_read = 8;
  ue_security_len = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  if((ue_security_len < MIN_REP_SEC_CAP_LENGTH) ||
     (ue_security_len > (*buffer_length/8)))
  {
    MSG_ERROR_DS_3(MM_SUB, "=NAS_MSG_LIB= Replayed UE security capabilities IE len %d, Min allowed %d, buffer_len %d",
                   ue_security_len, MIN_REP_SEC_CAP_LENGTH, (*buffer_length/8));
    *buffer_length = 0;
    return FALSE;
  }

  octet = 1;
  while(octet <= ue_security_len)
  {
    num_bits_to_read = 1;
    switch(octet)
    {
      case 1:
        /* EEA0 */
        security_mode_cmd->ue_security_capability.eps_encrypt_alg_128_eea0_supported =
                                               b_unpackb(src, *pos, num_bits_to_read);
        /* 128-EEA1 */
        security_mode_cmd->ue_security_capability.eps_encrypt_alg_128_eea1_supported =
                                               b_unpackb(src, (word)(*pos+1), num_bits_to_read);
        /* 128-EEA2 */
        security_mode_cmd->ue_security_capability.eps_encrypt_alg_128_eea2_supported =
                                               b_unpackb(src, (word)(*pos+2), num_bits_to_read);
        /* 128-EEA3 */
        security_mode_cmd->ue_security_capability.eps_encrypt_alg_eea3_supported =
                                               b_unpackb(src, (word)(*pos+3), num_bits_to_read);
        /* EEA4 */
        security_mode_cmd->ue_security_capability.eps_encrypt_alg_eea4_supported =
                                               b_unpackb(src, (word)(*pos+4), num_bits_to_read);
        /* EEA5 */
        security_mode_cmd->ue_security_capability.eps_encrypt_alg_eea5_supported =
                                               b_unpackb(src, (word)(*pos+5), num_bits_to_read);
        /* EEA6 */
        security_mode_cmd->ue_security_capability.eps_encrypt_alg_eea6_supported =
                                               b_unpackb(src, (word)(*pos+6), num_bits_to_read);
        /* EEA7 */
        security_mode_cmd->ue_security_capability.eps_encrypt_alg_eea7_supported =
                                               b_unpackb(src, (word)(*pos+7), num_bits_to_read);

        security_mode_cmd->ue_security_capability.eps_enc_exists =
                                      emm_ctrl_data_ptr->ue_security_capability.eps_enc_exists;
        break;

      case 2:
        /* EIA0 */
        security_mode_cmd->ue_security_capability.spare_bit =
                                               b_unpackb(src, *pos, num_bits_to_read);
        /* 128-EIA1 */
        security_mode_cmd->ue_security_capability.eps_integrity_alg_128_eia1_supported =
                                               b_unpackb(src, (word)(*pos+1), num_bits_to_read);
        /* 128-EIA2 */
        security_mode_cmd->ue_security_capability.eps_integrity_alg_128_eia2_supported =
                                               b_unpackb(src, (word)(*pos+2), num_bits_to_read);
        /* 128-EIA3 */
        security_mode_cmd->ue_security_capability.eps_integrity_alg_128_eia3_supported =
                                               b_unpackb(src, (word)(*pos+3), num_bits_to_read);
        /* EIA4 */
        security_mode_cmd->ue_security_capability.eps_integrity_alg_128_eia4_supported =
                                               b_unpackb(src, (word)(*pos+4), num_bits_to_read);
        /* EIA5 */
        security_mode_cmd->ue_security_capability.eps_integrity_alg_128_eia5_supported =
                                               b_unpackb(src, (word)(*pos+5), num_bits_to_read);
        /* EIA6 */
        security_mode_cmd->ue_security_capability.eps_integrity_alg_128_eia6_supported =
                                               b_unpackb(src, (word)(*pos+6), num_bits_to_read);
        /* EIA7 */
        security_mode_cmd->ue_security_capability.eps_integrity_alg_128_eia7_supported =
                                               b_unpackb(src, (word)(*pos+7), num_bits_to_read);

        security_mode_cmd->ue_security_capability.eps_int_exists =
                                      emm_ctrl_data_ptr->ue_security_capability.eps_int_exists;
        break;

      case 3:
        /* UEA0 */
        security_mode_cmd->ue_security_capability.umts_encrypt_alg_uea0_supported =
                                               b_unpackb(src, *pos, num_bits_to_read);
        /* UEA1 */
        security_mode_cmd->ue_security_capability.umts_encrypt_alg_uea1_supported =
                                               b_unpackb(src, (word)(*pos+1), num_bits_to_read);
        /* UEA2 */
        security_mode_cmd->ue_security_capability.umts_encrypt_alg_uea2_supported =
                                               b_unpackb(src, (word)(*pos+2), num_bits_to_read);
        /* UEA3 */
        security_mode_cmd->ue_security_capability.umts_encrypt_alg_uea3_supported =
                                               b_unpackb(src, (word)(*pos+3), num_bits_to_read);
        /* UEA4 */
        security_mode_cmd->ue_security_capability.umts_encrypt_alg_uea4_supported =
                                               b_unpackb(src, (word)(*pos+4), num_bits_to_read);
        /* UEA5 */
        security_mode_cmd->ue_security_capability.umts_encrypt_alg_uea5_supported =
                                               b_unpackb(src, (word)(*pos+5), num_bits_to_read);
        /* UEA6 */
        security_mode_cmd->ue_security_capability.umts_encrypt_alg_uea6_supported =
                                               b_unpackb(src, (word)(*pos+6), num_bits_to_read);
        /* UEA7 */
        security_mode_cmd->ue_security_capability.umts_encrypt_alg_uea7_supported =
                                               b_unpackb(src, (word)(*pos+7), num_bits_to_read);

        security_mode_cmd->ue_security_capability.umts_enc_exists =
                                      emm_ctrl_data_ptr->ue_security_capability.umts_enc_exists;
        break;

      case 4:
        /* Skip 1 Spare bit */

        /* UIA1 */
        security_mode_cmd->ue_security_capability.umts_integrity_alg_uia1_supported =
                                                b_unpackb(src, (word)(*pos+1), num_bits_to_read);
        /* UIA2 */
        security_mode_cmd->ue_security_capability.umts_integrity_alg_uia2_supported =
                                                b_unpackb(src, (word)(*pos+2), num_bits_to_read);
        /* UIA3 */
        security_mode_cmd->ue_security_capability.umts_integrity_alg_uia3_supported =
                                                b_unpackb(src, (word)(*pos+3), num_bits_to_read);
        /* UIA4 */
        security_mode_cmd->ue_security_capability.umts_integrity_alg_uia4_supported =
                                                b_unpackb(src, (word)(*pos+4), num_bits_to_read);
        /* UIA5 */
        security_mode_cmd->ue_security_capability.umts_integrity_alg_uia5_supported =
                                                b_unpackb(src, (word)(*pos+5), num_bits_to_read);
        /* UIA6 */
        security_mode_cmd->ue_security_capability.umts_integrity_alg_uia6_supported =
                                                b_unpackb(src, (word)(*pos+6), num_bits_to_read);
        /* UIA7 */
        security_mode_cmd->ue_security_capability.umts_integrity_alg_uia7_supported =
                                                b_unpackb(src, (word)(*pos+7), num_bits_to_read);

        security_mode_cmd->ue_security_capability.umts_int_exists =
                                       emm_ctrl_data_ptr->ue_security_capability.umts_int_exists;
        break;

      case 5:
        /* Skip 1 Spare bit */
 
        /* GEA1 */
        security_mode_cmd->ue_security_capability.gprs_encrypt_alg_1_supported =
                                                b_unpackb(src, (word)(*pos+1), num_bits_to_read);
        /* GEA2 */
        security_mode_cmd->ue_security_capability.gprs_encrypt_alg_2_supported =
                                                b_unpackb(src, (word)(*pos+2), num_bits_to_read);
        /* GEA3 */
        security_mode_cmd->ue_security_capability.gprs_encrypt_alg_3_supported =
                                                b_unpackb(src, (word)(*pos+3), num_bits_to_read);
        /* GEA4 */
        security_mode_cmd->ue_security_capability.gprs_encrypt_alg_4_supported =
                                                b_unpackb(src, (word)(*pos+4), num_bits_to_read);
        /* GEA5 */
        security_mode_cmd->ue_security_capability.gprs_encrypt_alg_5_supported =
                                                b_unpackb(src, (word)(*pos+5), num_bits_to_read);
        /* GEA6 */
        security_mode_cmd->ue_security_capability.gprs_encrypt_alg_6_supported =
                                                b_unpackb(src, (word)(*pos+6), num_bits_to_read);
        /* GEA7 */
        security_mode_cmd->ue_security_capability.gprs_encrypt_alg_7_supported =
                                                b_unpackb(src, (word)(*pos+7), num_bits_to_read);

        security_mode_cmd->ue_security_capability.gprs_enc_exists =
                                       emm_ctrl_data_ptr->ue_security_capability.gprs_enc_exists;
        break;

      default:
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Unsupported octet #%d after length octet", octet);
        extra_length = ue_security_len - (octet - 1);
        exit_while_loop = TRUE;
        break;

    } /* end switch(octet) */
    
    if(exit_while_loop == TRUE)
    {
      break;
    }
    octet++;
    /* Adjust buffer length for the entire previous octet read */
    num_bits_to_read = 8;
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
  } /* end while(octet <= ue_security_len) */

  if(extra_length > 0)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in Replayed UE security capabilities IE",
                  extra_length);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(extra_length*8, pos, buffer_length);
  }

  if((security_mode_cmd->ue_security_capability.umts_enc_exists == TRUE) &&
     (security_mode_cmd->ue_security_capability.umts_int_exists == FALSE))
  {
    MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= Replayed UE security includes Octet 5 but not Octet 6");
    *buffer_length = 0;
    return FALSE;
  }

                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*buffer_length)
  { 
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

    /* IE length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /* Store offset address & remaining buffer length, in case any octet
    is faulty this info would be required to jump to next IEI */
    iei_pos = *pos + 8;
    iei_buffer_len = *buffer_length - 8;
 
    switch(iei_val)
    {
      /***** Replayed nonceUE IE *****/
      case NONCE_UE_IEI:
        if(opt_iei_index <= 2)
        {
          num_bits_to_read = 8;

          if((*buffer_length/8) < sizeof(security_mode_cmd->nonce_ue))
          {
            for(octet = 0; octet < sizeof(security_mode_cmd->nonce_ue); octet++)
            {
              security_mode_cmd->nonce_ue[octet] = b_unpackb(src, *pos, num_bits_to_read);
              ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
            }

            security_mode_cmd->nonce_ue_exists = TRUE;
            valid_ie = TRUE;
          }
          else
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d",(*buffer_length/8));
            security_mode_cmd->nonce_ue_exists = FALSE;
            buffer_length_check_failed = TRUE;
            valid_ie = FALSE;
          }
          opt_iei_index = 3;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated Replayed nonceUE IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* Replayed nonceUE IE is of 5 octets and format Type TV. It doesn't have Length
             part so it cannot be skipped correctly after the switch block. Hence set valid_ie
             to TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = 5*8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        }
      break;

      /***** NonceMME IE *****/
      case NONCE_MME_IEI:
        if(opt_iei_index <= 3)
        {
          num_bits_to_read = 8;

          if((*buffer_length/8) < sizeof(security_mode_cmd->nonce_ue))
          {
            for(octet = 0; octet < sizeof(security_mode_cmd->nonce_mme); octet++)
            {
              security_mode_cmd->nonce_mme[octet] = b_unpackb(src, *pos, num_bits_to_read);
              ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
            }

            MSG_HIGH_DS_4(MM_SUB, "=NAS_MSG_LIB= NonceMME[0] = %d, NonceMME[1] = %d, NonceMME[2] = %d, NonceMME[3] = %d",
                         security_mode_cmd->nonce_mme[0], security_mode_cmd->nonce_mme[1],
                         security_mode_cmd->nonce_mme[2], security_mode_cmd->nonce_mme[3]);
            security_mode_cmd->nonce_mme_exists = TRUE;
            valid_ie = TRUE;
          }
          else
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d",(*buffer_length/8));
            security_mode_cmd->nonce_mme_exists = FALSE;
            buffer_length_check_failed = TRUE;
            valid_ie = FALSE;
          }
          opt_iei_index = 4;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated NonceMME IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* NonceMME IE is of 5 octets and format Type TV. It doesn't have Length
             part so it cannot be skipped correctly after the switch block. Hence set valid_ie
             to TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = 5*8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        }

#ifdef FEATURE_LTE_REL14
      /***** HashMME IE *****/
      case HASH_MME_IEI:
        if(opt_iei_index <= 4)
        {
          if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL14)
          {
            valid_ie = decode_hash_mme_ie(src, pos, buffer_length, &security_mode_cmd->hash_mme);

            security_mode_cmd->hash_mme_exists = valid_ie;
          }
          else
          {
            security_mode_cmd->hash_mme_exists = FALSE;
            MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= HashMME IE unsupported in UE rel %d",
                          emm_ctrl_data_ptr->emm_3gpp_spec_ver);
            valid_ie = FALSE;
          }
          opt_iei_index = 5;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated HashMME IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif

      default:
        /***** IMEISV request IE *****/
        /* IEI value is present in bits 5-8 of the octet that is already read.
           So just mask the bits to get the value */
        if(IMEISV_IEI == (byte)(0xF0 & iei_val))
        {
          if(opt_iei_index == 1)
          {
            /* IMEISV request value is present in bits 1-3 of the octet
               that is already read. So just mask the bits to get the value */
            if(0x01 == (lte_nas_emm_imiesv_req_type)(0x07 & iei_val))
            {
              security_mode_cmd->imiesv_req = IMIESV_REQUESTED;
            }
            else
            {
              security_mode_cmd->imiesv_req = IMIESV_NOT_REQUESTED;
            }
            security_mode_cmd->imiesv_req_exists = TRUE;
            MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= IMEISV request IE decoded successfully IMEISV requested = %d",security_mode_cmd->imiesv_req);
            /* IMEISV request IE is of 1 byte and format TV. It doesn't have Length
               part so it cannot be skipped correctly after the switch block if decoding failed.
               Hence set valid_ie to TRUE. iei_val is already read so no need to adjust buffer
               length and position */
            valid_ie = TRUE;
            opt_iei_index = 2;
          }
          else
          {
            /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated IMEISV request IE, opt_iei_idx = %d",
                           opt_iei_index);
            /* IMEISV request IE is of 1 byte and format TV. It doesn't have Length
               part so it cannot be skipped correctly after the switch block. Hence set valid_ie
               to TRUE. iei_val is already read so no need to adjust buffer length and position */
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
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, FALSE, 0);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
      }
      else
      {
        /* This block will not be reached currently since there are no optional IEs
           of TLV format present in Security Mode Command. In future versions of the spec
           if any optional IEs of format TLV are added then this block is needed to skip
           faulty TLV IEs */
        if(iei_buffer_len > (iei_len*8))
        {
          *buffer_length = iei_buffer_len - iei_len*8;
          *pos = iei_pos + iei_len*8;
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        }
        else
        {
          *buffer_length = 0;
        }
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*buffer_length) */

  return TRUE;
} /* end decode_security_mode_cmd() */


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
boolean decode_service_rej
(
  byte                         *src,
  word                         *pos,
  word                         *buffer_length,
  lte_nas_emm_service_rej_type *serv_rej
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  boolean valid_ie = TRUE;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;
  emm_ctrl_data_type  *emm_ctrl_data_ptr = NULL;
  
#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  if(*buffer_length < (NAS_EMM_SERV_REJ_MIN_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode SERVICE REJECT",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /***** EMM Cause IE *****/
  num_bits_to_read = 8;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
  if(((unpackedbytes == (byte)LTE_NAS_IMSI_UNKNOWN_IN_HSS) || 
      (unpackedbytes == (byte)LTE_NAS_ILLEGAL_UE)) ||
     ((unpackedbytes >= (byte)LTE_NAS_ILLEGAL_ME) &&
      (unpackedbytes <= (byte)LTE_NON_EPS_AUTH_UNACCEPTABLE)) ||
     ((unpackedbytes >= (byte)LTE_NAS_CS_FALLBACK_CALL_EST_NOT_ALLOWED) &&
      (unpackedbytes <= (byte)LTE_NAS_SEVERE_NW_FAILURE)) ||
     ((unpackedbytes >= (byte)LTE_NAS_SEMANTICALLY_INCORRECT_MSG) &&
      (unpackedbytes <= (byte)LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED)))
  {
    serv_rej->emm_cause = (lte_nas_emm_cause_type) unpackedbytes;
  } 
  else
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Unknown EMM cause value = %d received", unpackedbytes);
    *buffer_length = 0;
    return FALSE;
  }

  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*buffer_length)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

    /* IE length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /* Store offset address & remaining buffer length, in case any octet 
    is faulty this info would be required to jump to next IEI */
    iei_pos = *pos + 8;
    iei_buffer_len = *buffer_length - 8;
 
    switch(iei_val)
    {
      /***** T3442 value IE *****/
      case T3442_VAL_IEI:
        if(opt_iei_index == 1)
        {
          num_bits_to_read = 8;

          if(*buffer_length < num_bits_to_read)
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d bits",
                          *buffer_length);
            buffer_length_check_failed = TRUE;
            serv_rej->t3442_val_exists = FALSE;
            valid_ie = FALSE;
            break;
          }

          /* Read timer octet to decode T3442 timer unit and value */
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          serv_rej->t3442_val = calc_gprs_timer(unpackedbytes);
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
          serv_rej->t3442_val_exists = TRUE;
          valid_ie = TRUE;
          MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3442 value = %d ms",serv_rej->t3442_val);
          opt_iei_index = 2;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated T3442 value IE, opt_iei_idx = %d",
                         opt_iei_index);
          /* T3442 value IE is of 2 octets and format Type TV. It doesn't have Length
             part so it cannot be skipped correctly after the switch block. Hence set valid_ie
             to TRUE and skip the IE right here */
          valid_ie = TRUE;
          num_bits_to_read = 8;
          ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
        }
        break;

#ifdef FEATURE_LTE_REL10
      case T3346_VAL_IEI:
        if(opt_iei_index <= 2)
        {
          if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
          {
            valid_ie = decode_gprs_timer2_ie(src, pos, buffer_length, &serv_rej->t3346_val);

            if(valid_ie == TRUE)
            {
              /* If t3346 value is not zero/deactivated and received message is not
                 integrity protected use a random value b/w 15-30 mins */
              if((serv_rej->t3346_val != 0) &&
                 (serv_rej->security_hdr != INTEGRITY_PROTECT_CIPHER) &&
                 (serv_rej->security_hdr != INTEGRITY_PROTECT))
              {
                serv_rej->t3346_val = ((15 + mm_get_random_value(15)) * 60 * 1000);
                MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= Random T3346 value = %d ms, sec_hdr = %d",
                              serv_rej->t3346_val,
                              serv_rej->security_hdr);
              }
              else
              {
                MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= T3346 value = %d ms, sec_hdr = %d",
                              serv_rej->t3346_val,
                              serv_rej->security_hdr);
              }
            }
            opt_iei_index = 3;
          }
          else
          {
            MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3346 value IE unsupported in UE rel %d",
                         emm_ctrl_data_ptr->emm_3gpp_spec_ver);
            valid_ie = FALSE;
          }
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated T3346 value IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
#endif /* FEATURE_LTE_REL10 */

	  case T3448_VAL_IEI:
#ifdef FEATURE_CIOT
	  if(opt_iei_index <= 3)
	  {
		/* Read octet containing timer unit and value in advance */
		num_bits_to_read = 8;
		unpackedbytes = b_unpackb(src, (word)(*pos+8), num_bits_to_read);
		valid_ie = decode_gprs_timer2_ie(src, pos, buffer_length, &serv_rej->t3448_val);
		if(valid_ie == TRUE)
        {
              /* If t3448 value is not zero/deactivated and received message is not
                 integrity protected use a random value b/w 15-30 mins */
              if( (serv_rej->emm_cause ==LTE_NAS_CONGESTION) &&
			  	  (serv_rej->t3448_val != 0) &&
                  (serv_rej->security_hdr != INTEGRITY_PROTECT_CIPHER) &&
                  (serv_rej->security_hdr != INTEGRITY_PROTECT))
              {
                serv_rej->t3448_val = ((15 + mm_get_random_value(15)) * 60 * 1000);
                MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= Random T3448 value = %d ms, sec_hdr = %d",
                              serv_rej->t3448_val,
                              serv_rej->security_hdr);
              }
              else
              {
                MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= T3448 value = %d ms, sec_hdr = %d",
                              serv_rej->t3448_val,
                              serv_rej->security_hdr);
              }
         }
		if(valid_ie == TRUE)
		{
		  if((unpackedbytes >> 5) == 7 || serv_rej->t3448_val == 0 ) /* Timer is deactivated */
		  {
			serv_rej->t3448_val_exists = FALSE;
			MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Timer T3448 is deactivated");
		  }
		  else
		  {
			MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3448 value = %d ms", serv_rej->t3448_val);
			serv_rej->t3448_val_exists = TRUE;
		  }
		}
		opt_iei_index = 4;
	  }
	  else
#endif
	  {
		/* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
		MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated header compression configuration status IE, opt_iei_idx = %d",
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
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, FALSE, 0);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
      }
      else
      {
        if(iei_buffer_len > (iei_len*8))
        {
          *buffer_length = iei_buffer_len - iei_len*8;
          *pos = iei_pos + iei_len*8;
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        }
        else
        {
          *buffer_length = 0;
        }
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*buffer_length) */

  return TRUE;
} /* end decode_service_rej() */

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
  lte_nas_emm_service_accept_type  *service_accept
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  boolean valid_ie = TRUE;
  byte iei_val = 0, iei_len = 0;
  word iei_pos = 0, iei_buffer_len = 0;
  byte opt_iei_index = 1;
  boolean skip_unknown_ie = FALSE;
  emm_ctrl_data_type  *emm_ctrl_data_ptr = NULL;
  
#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
#endif

         /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*buffer_length)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;

    /* IEI */
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Optional IEI = %d", iei_val);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

    /* IE length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /* Store offset address & remaining buffer length, in case any octet 
    is faulty this info would be required to jump to next IEI */
    iei_pos = *pos + 8;
    iei_buffer_len = *buffer_length - 8;
 
     switch(iei_val)
    {

      /***** EPS bearer context status IE *****/
      case EPS_BEARER_CONTEXT_STATUS_IEI:
        if(opt_iei_index == 1)
        {
          valid_ie = decode_eps_bearer_context_status(src,
                                                      pos,
                                                      buffer_length,
                                                      &service_accept->eps_bearer_context_status);
          if(valid_ie == FALSE)
          {
            service_accept->eps_bearer_context_status_exists = FALSE;
          }
          else
          {
            service_accept->eps_bearer_context_status_exists = TRUE;
          }
          opt_iei_index = 2;
        }
        else
        {
          /* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated EPS bearer context status list IE, opt_iei_idx = %d",
                         opt_iei_index);
          valid_ie = FALSE;
        }
        break;
	  case T3448_VAL_IEI:
#ifdef FEATURE_CIOT
	  if(opt_iei_index <= 2)
	  {
		/* Read octet containing timer unit and value in advance */
		num_bits_to_read = 8;
		unpackedbytes = b_unpackb(src, (word)(*pos+8), num_bits_to_read);
		valid_ie = decode_gprs_timer2_ie(src, pos, buffer_length, &service_accept->t3448_val);
		if(valid_ie == TRUE)
		{
		  if((unpackedbytes >> 5) == 7 || service_accept->t3448_val == 0) /* Timer is deactivated */
		  {
			service_accept->t3448_val_exists = FALSE;
			MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Timer T3448 is deactivated");
		  }
		  else
		  {
			MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= T3448 value = %d ms", service_accept->t3448_val);
			service_accept->t3448_val_exists = TRUE;
		  }
		}
		opt_iei_index = 3;
	  }
	  else
#endif
	  {
		/* Skip out of sequence or repeated IEs, 24.301 - 7.6.2, 7.6.3 */
		MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip out of sequence or repeated header compression configuration status IE, opt_iei_idx = %d",
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
        msg_lib_skip_unknown_iei(iei_val, src, pos, buffer_length, FALSE, 0);
        skip_unknown_ie = FALSE;
        valid_ie = TRUE;
      }
      else if(buffer_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= buff_len = %d < iei_len = %d. Stop parsing",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
      }
      else
      {
        if(iei_buffer_len > (iei_len*8))
        {
          *buffer_length = iei_buffer_len - iei_len*8;
          *pos = iei_pos + iei_len*8;
        }
        else
        {
          *buffer_length = 0;
        }
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Faulty Optional IEI = %d, Skipping IEI", iei_val);
        valid_ie = TRUE;
      }
    } /* end if(valid_ie == FALSE) */
  } /* end while(*buffer_length) */

  return TRUE;
}/* end decode_service_accept() */
#endif
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
boolean decode_authentication_req
(
  byte                 *src,
  word                 *pos,
  word                 *buffer_length,
  lte_nas_emm_auth_req *auth_req
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  byte tsc_bit = 0, index = 0, autn_len = 0;

  if(*buffer_length < AUTH_REQ_LEN)
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode AUTHENTICATION REQUEST",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /***** Spare half octet + NAS key set identifier(ASME) IE *****/
  /* Skip 4 spare bits and read TSC bit directly */
  num_bits_to_read = 1;
  tsc_bit = b_unpackb(src, (word)(*pos+4), num_bits_to_read);

  /* Read 3 bits of NAS key set identifier value directly */
  num_bits_to_read = 3;
  unpackedbytes = b_unpackb(src, (word)(*pos+5), num_bits_to_read);

  if((tsc_bit == 1) || (unpackedbytes == NO_KEY_AVAIALABLE))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received invalid TSC = %d or invalid KSI = %d",
                   tsc_bit, unpackedbytes);
    *buffer_length = 0;
    return FALSE;
  }
  auth_req->nas_key_set_identifier_asme.security_context = tsc_bit;
  auth_req->nas_key_set_identifier_asme.keyset_id = unpackedbytes;

  /* Adjust buffer length for the last octet read */
  num_bits_to_read = 8;
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  /***** Authentication parameter RAND (EPS challenge) IE *****/
  num_bits_to_read = 8;
  for(index = 0; index < SIZE_OF_UMTS_AUTH_CHALLENGE_PARAM; index++)
  {
    auth_req->nas_auth_rand_param[index] = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
  }

  /***** Authentication parameter AUTN (EPS challenge) IE *****/
  /* AUTN length */
  num_bits_to_read = 8;
  autn_len = b_unpackb(src, *pos, num_bits_to_read);
  if(autn_len < SIZE_OF_UMTS_AUTH_CHALLENGE_PARAM)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= AUTN length = %d < Minimum length = %d",
                   autn_len, SIZE_OF_UMTS_AUTH_CHALLENGE_PARAM);
    *buffer_length = 0;
    return FALSE;
  }

  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  auth_req->nas_umts_auth_challenge.length = autn_len;
  for(index = 0; (index < autn_len) && (index < SIZE_OF_UMTS_AUTH_CHALLENGE_PARAM); index++)
  {
    auth_req->nas_umts_auth_challenge.val[index] = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);
  }

  /* Discard additional octets in Authentication parameter AUTN (EPS challenge) IE */
  if(autn_len > SIZE_OF_UMTS_AUTH_CHALLENGE_PARAM)
  {
    auth_req->nas_umts_auth_challenge.length = SIZE_OF_UMTS_AUTH_CHALLENGE_PARAM;
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in Authentication parameter AUTN (EPS challenge) IE",
                  (autn_len - SIZE_OF_UMTS_AUTH_CHALLENGE_PARAM));
    ADJUST_OFFSET_AND_BUF_LEN_EMM((autn_len - SIZE_OF_UMTS_AUTH_CHALLENGE_PARAM)*8,
                                   pos, buffer_length);
  }

  return TRUE;
} /* end decode_authentication_req() */


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
boolean decode_emm_status
(
  byte               *src,
  word               *pos,
  word               *buffer_length,
  lte_nas_emm_status *emm_status
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;

  if(*buffer_length != (NAS_EMM_STATUS_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode EMM STATUS",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /***** EMM Cause IE *****/
  num_bits_to_read = 8;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
  if(((unpackedbytes == (byte)LTE_NAS_IMSI_UNKNOWN_IN_HSS) ||
      (unpackedbytes == (byte)LTE_NAS_ILLEGAL_UE)) ||
     ((unpackedbytes >= (byte)LTE_NAS_ILLEGAL_ME) &&
      (unpackedbytes <= (byte)LTE_NON_EPS_AUTH_UNACCEPTABLE)) ||
     ((unpackedbytes >= (byte)LTE_NAS_CS_FALLBACK_CALL_EST_NOT_ALLOWED) &&
      (unpackedbytes <= (byte)LTE_NAS_SEVERE_NW_FAILURE)) ||
     ((unpackedbytes >= (byte)LTE_NAS_SEMANTICALLY_INCORRECT_MSG) &&
      (unpackedbytes <= (byte)LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED)))
  {
    emm_status->emm_cause = (lte_nas_emm_cause_type) unpackedbytes;
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= EMM cause value = %d", emm_status->emm_cause);
  }
  else
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Unknown EMM cause value = %d received", unpackedbytes);
    *buffer_length = 0;
    return FALSE;
  }

  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos, buffer_length);

  return TRUE;  
} /* end decode_emm_status() */


/*===========================================================================
FUNCTION      DECODE_DOWNLINK_NAS_TRANSPORT

DESCRIPTION
  This function decodes DOWNLINK NAS TRANSPORT message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
boolean decode_dl_nas_transport
(
  byte                         *src_ptr,
  word                         *pos_ptr,
  word                         *buffer_length_ptr,
  lte_nas_emm_dl_nas_transport *dl_nas_transport_ptr
)
{
  word msg_len = 0;
  byte extra_length = 0;
  byte num_bits_to_read = 0;

  /***** NAS message container IE *****/
  if(*buffer_length_ptr < NAS_MSG_CONTAINER_MIN_LEN)
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode DOWNLINK NAS TRANSPORT",
                   *buffer_length_ptr);
    *buffer_length_ptr = 0;
    return FALSE;
  }

  /* Length of NAS message container contents */
  num_bits_to_read = 8;
  msg_len = b_unpackb(src_ptr, *pos_ptr, num_bits_to_read);

  if(msg_len < EMM_SMS_DATA_OFFSET)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received NAS message container IE len %d < %d Min allowed",
                   msg_len, EMM_SMS_DATA_OFFSET);
    *buffer_length_ptr = 0;
    return FALSE;
  }

  if(msg_len > MAX_OCTETS_L3_MESSAGE)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received NAS message container IE len %d > Max allowed %d. Truncating to max",
                   msg_len, MAX_OCTETS_L3_MESSAGE);
    extra_length = (msg_len - MAX_OCTETS_L3_MESSAGE);
    msg_len = MAX_OCTETS_L3_MESSAGE;
  }

  dl_nas_transport_ptr->nas_msg_container.msg_len = msg_len;
  num_bits_to_read = 8;
  ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_to_read, pos_ptr, buffer_length_ptr);

  if(msg_len > (*buffer_length_ptr/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length_ptr/8), msg_len);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  memscpy((void*)&dl_nas_transport_ptr->nas_msg_container.msg_payload[0],
          sizeof(dl_nas_transport_ptr->nas_msg_container.msg_payload),
          (void *)(src_ptr + *pos_ptr/8),
          dl_nas_transport_ptr->nas_msg_container.msg_len);

  /* Discard additional octets in NAS message container IE */
  if(extra_length > 0)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in NAS message container IE",
                  extra_length);
    ADJUST_OFFSET_AND_BUF_LEN_EMM(extra_length*8, pos_ptr, buffer_length_ptr);
  }

  return TRUE;
} /* end decode_dl_nas_transport() */

#endif /* FEATURE_LTE */