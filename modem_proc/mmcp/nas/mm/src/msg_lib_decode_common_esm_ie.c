

/*==============================================================================

==============================================================================*/

/*Author: hnam
  Reference Document: 3GPP TS 24.301 V1.1.1 (2008-10)
  START DATE: 2/15/2008*/
/*==============================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/msg_lib_decode_common_esm_ie.c#1 $
$Author: pwbldsvc $
$DateTime: 2023/04/25 14:39:18 $

when         who                          what                              
--------     --------  ------------------------------------------------
09/17/12       cogolt  Rel-10, Added function decode_t3396_value
06/22/09       hnam    Changed the MAX_QOS_LEN from 14 to 16
05/18/09       hnam    Fixed the code to decode sdf qos
05/17/09       hnam    Added the code to support security
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
#include "msg_lib_encode_esm.h"
#include "msg_lib_decode_common_esm_ie.h"
#include "lte_nas_esm_message_type.h"
#include "lte_nas_esm_message_description.h"
#include "msg_lib_declarations.h"
#include "msg_lib_validation.h"
#include "msg.h"
#include "err.h"
#include "sm_v.h"
#include "gmm_int_v.h"
#include "emm_database.h"
#include "emm_utility.h"

/*===========================================================================
                        FUNCTION DEFINITIONS
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
boolean decode_tft
(
  byte       *src,
  word       *pos,
  word       *buffer_length,
  tft_type_T *tft,
  word       *esm_msg_len
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0, id = 0;
  word tft_len = 0, extra_length = 0;
  byte pkt_content_index = 0, param_content_index = 0;


  /***** Length of TFT IE *****/
  num_bits_to_read = 8;
  tft->tft_total_length = b_unpackw(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /* Based on the length of TFT IE decode the other fields */
  if(tft->tft_total_length > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), tft->tft_total_length);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(tft->tft_total_length < MIN_TFT_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received TFT len %d < %d Min allowed",
                   tft->tft_total_length, MIN_TFT_LENGTH);
    return FALSE;
  }

  /* Truncate the received length if greater than max supported */
  if(tft->tft_total_length > MAX_TFT_LENGTH)
  {
    extra_length = tft->tft_total_length - MAX_TFT_LENGTH;
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received %d extra octets in TFT IE", extra_length);
    tft->tft_total_length = MAX_TFT_LENGTH;
  }

  tft_len = tft->tft_total_length;

  /***** TFT operation code *****/
  num_bits_to_read = 3;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
  if(unpackedbytes == RESERVED_TFT_OPN_CODE)
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= TFT operation code = %d is reserved. Skip parsing",
                   unpackedbytes);
    return FALSE;
  }

  tft->operation_code = unpackedbytes;
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /***** E bit *****/
  num_bits_to_read = 1;
  tft->e_bit = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /***** Packet filters *****/
  num_bits_to_read = 4;
  tft->num_filters = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  tft_len--;

  if((tft->num_filters == 0) || (tft->num_filters > MAX_PACKET_FILTERS))
  {
    /* Stop further decoding and skip the remaining bytes in the IE */
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM((tft_len + extra_length)*8, pos, buffer_length, esm_msg_len);
    return TRUE;
  }

  num_bits_to_read = 8;

  if(tft->operation_code == DELETE_PKT_FLTR_IN_EXISTING_TFT)
  {
    if(tft_len < tft->num_filters)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient TFT length = %d to decode %d packet filters",
                     tft_len, tft->num_filters);
      return FALSE;
    }

    for(id = 0; id < tft->num_filters; id++)
    {
      tft->filter_list[id].identifier = b_unpackb(src, *pos, num_bits_to_read);
      (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
      tft_len--;
    }        
  }
  else if((tft->operation_code == CREATE_NEW_TFT)||
          (tft->operation_code == ADD_PKT_FLTR_IN_EXISTING_TFT)||
          (tft->operation_code == REPLACE_PKT_FLTR_IN_EXISTING_TFT))
  {
    for(id = 0; (id < tft->num_filters) && (id < MAX_PACKET_FILTERS); id++)
    {
      if(tft_len < 3)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient TFT length = %d to decode TFT packet filterlist [%d]",
                       tft_len, id);
        return FALSE;
      }

      tft->filter_list[id].identifier = b_unpackb(src, *pos, num_bits_to_read);
      (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
      tft_len--;
      tft->filter_list[id].eval_precedence = b_unpackb(src, *pos, num_bits_to_read);
      (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
      tft_len--;

      /***** Content length *****/
      tft->filter_list[id].content_length = b_unpackb(src, *pos, num_bits_to_read);
      (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
      tft_len--;

      if(tft_len < tft->filter_list[id].content_length)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient TFT length = %d to decode TFT packet filterlist [%d] contents",
                       tft_len, id);
        return FALSE;
      }

      /***** Packet filter contents *****/
      for(pkt_content_index = 0;(pkt_content_index < tft->filter_list[id].content_length) &&
                                (pkt_content_index < MAX_FILTER_CONTENT_LENGTH); pkt_content_index++)
      {
        tft->filter_list[id].filter_contents[pkt_content_index] = b_unpackb(src, *pos, num_bits_to_read);
        (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
        tft_len--;
      }
    } 
  }

  /***** Parameter filters *****/
  if(tft_len > 0)
  {
    for(id = 0; id < MAX_PACKET_FILTERS; id++)
    {
      if(tft_len < 2)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient TFT length = %d to decode TFT paramlist [%d]",
                       tft_len, id);
        return FALSE;
      }

      tft->param_list[id].identifier = b_unpackb(src, *pos, num_bits_to_read);
      (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
      tft_len--;
      tft->param_list[id].content_length = b_unpackb(src, *pos, num_bits_to_read);
      (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
      tft_len--;

      if(tft_len < tft->param_list[id].content_length)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient TFT length = %d to decode TFT paramlist[%d] filter contents",
                       tft_len, id);
        return FALSE;
      }

      /***** Parameter filter contents *****/
      for(param_content_index = 0;(param_content_index < tft->param_list[id].content_length) &&
                                  (param_content_index < MAX_FILTER_CONTENT_LENGTH); param_content_index++)
      {
        tft->param_list[id].filter_contents[param_content_index] = b_unpackb(src, *pos, num_bits_to_read);
        (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
        tft_len--;
      }
    }
  }

  /* Skip remaining TFT length if any plus any extra octets in TFT IE */
  if((tft_len + extra_length) > 0)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in TFT IE",
                  (tft_len + extra_length));
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM((tft_len + extra_length)*8, pos, buffer_length, esm_msg_len);
  }

  return TRUE;
} /* end decode_tft() */


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
boolean decode_eps_qos
(
  byte      *src,
  word      *pos,
  word      *buffer_length,
  sdf_qos_T *eps_qos,
  word      *esm_msg_len
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0, octet = 0;
  byte ie_length = 0;
  boolean exit_for_loop = FALSE;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  word length_to_skip = 0;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  /***** Length of EPS QoS IE *****/
  num_bits_to_read = 8;
  ie_length = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /* Based on the length of EPS QoS IE decode the other fields */
  if(ie_length > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), ie_length);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(ie_length < MIN_EPS_QOS_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received EPS QoS len %d < %d Min allowed",
                   ie_length, MIN_EPS_QOS_LENGTH);
    return FALSE;
  }

  /* Initialize EPS QoS length to 0 and increment the length upon decoding each octet */
  eps_qos->length = 0;
  num_bits_to_read = 8;

  for(octet = 1; octet <= ie_length; octet++)
  {
    unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);

    switch(octet)
    {
      case 1: /***** QoS Class ID *****/
        eps_qos->qci = unpackedbytes;
        break;

      case 2: /***** Max bit rate of uplink *****/
        if(unpackedbytes >= (byte)ONE_KBPS)
        {
          eps_qos->max_bit_rate_ul = unpackedbytes;
        }
        else
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Maximum bit rate for UL = %d. Valid range = 1-255",
                         unpackedbytes);
          return FALSE;
        }
        break;

      case 3: /***** Max bit rate of downlink *****/
        if(unpackedbytes >= (byte)ONE_KBPS)
        {
          eps_qos->max_bit_rate_dl = unpackedbytes;
        } 
        else
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Maximum bit rate for DL = %d. Valid range = 1-255",
                         unpackedbytes);
          return FALSE;
        }
        break;

      case 4: /***** Guaranteed bit rate for uplink *****/
        if(unpackedbytes >= (byte)ONE_KBPS)
        {
          eps_qos->gbr_ul = unpackedbytes;
        }
        else
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Guaranteed bit rate for UL = %d. Valid range = 1-255",
                         unpackedbytes);
          return FALSE;
        }
        break;

      case 5: /***** Guaranteed bit rate for downlink *****/
        if(unpackedbytes >= (byte)ONE_KBPS)
        {
          eps_qos->gbr_dl = unpackedbytes;
        }
        else
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Guaranteed bit rate for DL = %d. valid range = 1-255",
                         unpackedbytes);
          return FALSE;
        }
        break;

      case 6: /***** Maximum bit rate for uplink (extended) *****/
        eps_qos->max_bit_rate_ul_ex = unpackedbytes;
        break; 

      case 7: /***** Maximum bit rate for downlink (extended) *****/
        eps_qos->max_bit_rate_dl_ex = unpackedbytes;
        break;

      case 8: /***** Guaranteed bit rate for uplink (extended) *****/
        eps_qos->gbr_ul_ex = unpackedbytes;
        break;

      case 9: /***** Guaranteed bit rate for downlink (extended) *****/
        eps_qos->gbr_dl_ex = unpackedbytes;
        break;

#ifdef FEATURE_LTE_REL11
      case 10: /***** Maximum bit rate for uplink (extended-2) *****/
        if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)
        {
          eps_qos->max_bit_rate_ul_ex2 = unpackedbytes;
        }
        else
        {
          /* Octets from Max bit rate for uplink (extended-2) onwards are only supported in spec
             versions from rel-11 onwards. Therefore stop decoding further and skip the remaining
             octets in EPS QoS IE */
          length_to_skip = ie_length - eps_qos->length;
          exit_for_loop = TRUE;
        }
        break;

      case 11: /***** Maximum bit rate for downlink (extended-2) *****/
        eps_qos->max_bit_rate_dl_ex2 = unpackedbytes;
        break;

      case 12: /***** Guaranteed bit rate for uplink (extended-2) *****/
        eps_qos->gbr_ul_ex2 = unpackedbytes;
        break;

      case 13: /***** Guaranteed bit rate for downlink (extended-2) *****/
        eps_qos->gbr_dl_ex2 = unpackedbytes;
        break;
#endif /* FEATURE_LTE_REL11 */

      default:
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Unsupported Octet #%d (after length field)", octet);
        length_to_skip = ie_length - eps_qos->length;
        exit_for_loop = TRUE;
        break;
    } /* end switch(octet) */

    if(exit_for_loop == TRUE)
    {
      break;
    }

    eps_qos->length++;
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
  } /* end for loop */
  
  /* Discard additional octets in EPS QoS IE */
  if(length_to_skip > 0)
  {
    MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in EPS QoS IE. 3gpp_spec_ver = %d",
                  length_to_skip, emm_ctrl_data_ptr->emm_3gpp_spec_ver);
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(length_to_skip*8, pos, buffer_length, esm_msg_len);
  }
  
  return TRUE;
} /* end decode_eps_qos() */


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
boolean decode_packet_flow_identifier
(
  byte   *src,
  word   *pos,
  word   *buffer_length,
  pfi_T  *pkt_flow_id,
  word   *esm_msg_len
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0, pkt_flow_id_len = 0;

  /************ Length of Packet flow identifier IE ************/
  num_bits_to_read = 8;
  pkt_flow_id_len = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /* Based on the length of Packet flow identifier IE decode the other fields */
  if(pkt_flow_id_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), pkt_flow_id_len);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(pkt_flow_id_len < MIN_PKT_FLOW_ID_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received Packet flow identifier IE len %d < %d Min allowed",
                   pkt_flow_id_len, MIN_PKT_FLOW_ID_LENGTH);
    return FALSE;
  }

  /* Skip 1 Spare bit and read Packet flow identifier value directly */
  /***** Packet flow identifier value *****/
  num_bits_to_read = 7;
  unpackedbytes = b_unpackb(src, (word)(*pos+1), num_bits_to_read);

  if(unpackedbytes > MAX_PACKET_FLOW_IDENTIFIER)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Packet flow id value = %d > %d",
                   unpackedbytes, MAX_PACKET_FLOW_IDENTIFIER);
    return FALSE;
  }

  pkt_flow_id->packet_flow_id = unpackedbytes;

  /* Adjust buffer length */
  num_bits_to_read = 8;
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /* Discard additional octets in Packet flow identifier IE */
  if(pkt_flow_id_len > MAX_PKT_FLOW_ID_LENGTH)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in Packet flow identifier IE",
                  (pkt_flow_id_len - MAX_PKT_FLOW_ID_LENGTH));
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM((pkt_flow_id_len - MAX_PKT_FLOW_ID_LENGTH)*8,
                              pos, buffer_length, esm_msg_len);
  }

  return TRUE;
} /* end decode_packet_flow_identifier() */


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
boolean decode_apn
(
  byte  *src,
  word  *pos,
  word  *buffer_length,
  apn_T *apn,
  word  *esm_msg_len
)
{
  byte num_bits_to_read = 0, octet = 0, extra_length = 0;
  /************ Length of Access point name IE ************/
  num_bits_to_read = 8;
  apn->apn_addr_len = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);


  /* Based on the length of Access point name IE decode the other fields */
  if(apn->apn_addr_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), apn->apn_addr_len);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(apn->apn_addr_len < MIN_APN_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received APN IE len %d < %d Min allowed",
                   apn->apn_addr_len, MIN_APN_LENGTH);
    return FALSE;
  }

  if(apn->apn_addr_len > MAX_APN_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received APN IE len %d > %d Max allowed. Decoding upto Max",
                   apn->apn_addr_len, MAX_APN_LENGTH);
    extra_length = apn->apn_addr_len - MAX_APN_LENGTH;
    apn->apn_addr_len = MAX_APN_LENGTH;
  }

  /***** APN value *****/
  for(octet = 0; (octet < apn->apn_addr_len) && (apn->apn_addr_len < MAX_APN_LENGTH); octet++)
  {
    apn->address[octet] = b_unpackb(src, *pos, num_bits_to_read);
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
  }

  /* Discard additional octets in APN IE */
  if(extra_length > MAX_APN_LENGTH)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in APN IE",
                  extra_length);
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(extra_length*8, pos, buffer_length, esm_msg_len);
  }

  return TRUE;
} /* end decode_apn() */


#ifdef FEATURE_NAS_GW
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
boolean decode_transaction_id
(
  byte                      *src,
  word                      *pos,
  word                      *buffer_length,
  gsm_umts_transaction_id_T *ti,
  word                      *esm_msg_len
)
{ 
  byte num_bits_to_read = 0, extra_length = 0;

  /***** Length of Transaction ID IE *****/
  num_bits_to_read = 8;
  ti->length_of_transaction_id = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /* Based on the length of Transaction ID IE decode the other fields */
  if(ti->length_of_transaction_id > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), ti->length_of_transaction_id);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(ti->length_of_transaction_id < MIN_TRANSACTION_ID_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received Transaction ID IE len %d < %d Min allowed",
                   ti->length_of_transaction_id, MIN_TRANSACTION_ID_LENGTH);
    return FALSE;
  }

  if(ti->length_of_transaction_id > MAX_TRANSACTION_ID_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received PDN address IE len %d > %d Max allowed. Decoding upto Max",
                   ti->length_of_transaction_id, MAX_TRANSACTION_ID_LENGTH);
    extra_length = ti->length_of_transaction_id - MAX_TRANSACTION_ID_LENGTH;
    ti->length_of_transaction_id = MAX_TRANSACTION_ID_LENGTH;
  }

  /***** Transaction ID flag *****/
  num_bits_to_read = 1;
  ti->ti_flag = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /***** Transaction ID Value 0 *****/
  num_bits_to_read = 3; 
  ti->val_of_transaction_id[0] = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /***** 4 Spare bits *****/
  num_bits_to_read = 4;
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  if(ti->length_of_transaction_id > 1)
  {
    /***** 1 EXT bit *****/
    num_bits_to_read = 1;
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
    
    /***** Transaction ID Value 1 *****/
    num_bits_to_read = 7;
    ti->val_of_transaction_id[1] = b_unpackb(src, *pos, num_bits_to_read);
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
  }

  /* Discard additional octets in Transaction ID IE */
  if(extra_length > 0)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in Transaction ID IE",
                  extra_length);
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(extra_length*8, pos, buffer_length, esm_msg_len);
  }
  
  return TRUE;
} /* end decode_transaction_id() */
#endif

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
boolean decode_prot_conf_opt
(
  byte                   *src,
  word                   *pos,
  word                   *buffer_length,
  protocol_cfg_options_T *prot_conf_opt,
  word                   *esm_msg_len
)
{
  byte num_bits_to_read = 0, prot_conf_loop = 0;
  word prot_conf_opt_len = 0, index = 0;
  word extra_length = 0, original_buffer_length = 0;


  MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Decoding Protocol configuration options IE");

  /************ Length of Protocol configuration options IE ************/
  num_bits_to_read = 8;
  prot_conf_opt_len = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /* Based on the length of PCO IE decode the other fields */
  if(prot_conf_opt_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), prot_conf_opt_len);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(prot_conf_opt_len < MIN_PROT_CONFIG_OPT_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received PCO IE len %d < %d Min allowed",
                   prot_conf_opt_len, MIN_PROT_CONFIG_OPT_LENGTH);
    return FALSE;
  }

  /* Truncate the received length if greater than max supported */
  if(prot_conf_opt_len > MAX_PROT_CONFIG_OPT_LENGTH)
  {
    extra_length = prot_conf_opt_len - MAX_PROT_CONFIG_OPT_LENGTH;
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received %d extra octets in PCO IE", extra_length);
    prot_conf_opt_len = MAX_PROT_CONFIG_OPT_LENGTH;
  }

  /****** 1 ext bit + 4 Spare bits *****/
  num_bits_to_read = 5;
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /***** Configuration protocol *****/
  num_bits_to_read = 3;
  prot_conf_opt->config_protocol = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  prot_conf_opt_len--;
  original_buffer_length = *buffer_length;
  
  while(prot_conf_opt_len && (index < NUM_PROTOCOL_ID))
  {
	 if(*buffer_length > original_buffer_length)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow:  original_buffer_length = %d, buffer_length = %d",
                             original_buffer_length/8, (*buffer_length/8));
      buffer_length_check_failed = TRUE;
      return FALSE;
    }
    /***** Protocol ID *****/
    num_bits_to_read = 8;
    prot_conf_opt->procol_info[index].protocol_id[0] = b_unpackb(src, *pos, num_bits_to_read);
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
    prot_conf_opt_len--;
    
    prot_conf_opt->procol_info[index].protocol_id[1] = b_unpackb(src, *pos, num_bits_to_read);
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
    prot_conf_opt_len--;
    

      /***** Protocol ID length *****/
      num_bits_to_read = 8;
      prot_conf_opt->procol_info[index].protocol_len = b_unpackb(src, *pos, num_bits_to_read);
      (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
      prot_conf_opt_len--;

      /***** Protocol ID contents *****/
      num_bits_to_read = 8;
      for(prot_conf_loop = 0;prot_conf_loop < (prot_conf_opt->procol_info[index].protocol_len) &&
                           (prot_conf_loop < MAX_PROTOCOL_ID_CONTENT); prot_conf_loop++)
      {
        if((prot_conf_opt_len > 0) && (prot_conf_opt_len <= (*buffer_length/8)))
        {
          prot_conf_opt->procol_info[index].protocol_contents[prot_conf_loop] =
                                              b_unpackb(src, *pos, num_bits_to_read);
          (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
          prot_conf_opt_len--;
        }
        else
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient PCO Length to decode Protocol ID Contents for Protocol ID %d",
                         index+1);
          buffer_length_check_failed = TRUE;
          return FALSE;
        }
      } /* end protocol ID contents for loop */
    index++;
    prot_conf_opt->num_protocol_ids++;
  } /* end while(prot_conf_opt_len) */

  /* Skip remaining PCO length if any plus any extra octets in PCO IE */
  if((prot_conf_opt_len + extra_length) > 0)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in PCO IE",
                  (prot_conf_opt_len + extra_length));
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM((prot_conf_opt_len + extra_length)*8,
                                   pos, buffer_length, esm_msg_len);
  }

  return TRUE;  
} /* end decode_prot_conf_opt() */


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
boolean decode_ext_prot_conf_opt
(
  byte                   *src,
  word                   *pos,
  word                   *buffer_length,
  ext_protocol_cfg_options_T *ext_prot_conf_opt,
  word                   *esm_msg_len
)
{
  byte num_bits_to_read = 0,index = 0;
  dsm_item_type *dsm_ptr = NULL;
  word epco_len = 0;
  uint16 dsm_packet_length = 0;
  uint16 len=0;
  /*Length of extended protocol configuration options IE */
  num_bits_to_read = 16;
  epco_len = b_unpackw(src, *pos, num_bits_to_read);
  if (ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len) == FALSE)
  {
    *buffer_length = 0;
    *esm_msg_len = 0;
     MSG_ERROR_DS_0(MM_SUB, "=NAS_MSG_LIB= buffer wraparound");
     return FALSE;
  }

  /* Based on the length of PCO IE decode the other fields */
  if(epco_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d",(*buffer_length/8));
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(epco_len < MIN_EPCO_CONTENT_LEN)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received EPCO CONFIG IE len %d < %d Min allowed",
                   epco_len, MIN_EPCO_CONTENT_LEN);
    return FALSE;
  }

  /***** LENGTH Of EPCO IE *****/
  ext_prot_conf_opt->len = epco_len;

  /*Push down into appropriate pool*/
  /*if we do not have enough small items use large item pool*/
  if(((uint16)epco_len <= DSM_DS_SMALL_ITEM_SIZ) && 
     ((uint16)epco_len <= NAS_REM_SMALL_ITEM_POOL_SIZE))
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d small item pool", (uint16)epco_len);
    /* Pushdown DSM */
    dsm_packet_length = dsm_pushdown(&dsm_ptr, 
                                     src + (*pos/8),
                                     (uint16)epco_len, 
                                     DSM_DS_SMALL_ITEM_POOL);

    if((uint16)epco_len != dsm_packet_length)
    {
      /*free any partial memory allocated */
      dsm_free_packet(&dsm_ptr);

      MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d large item pool", dsm_packet_length);
      /* If Small item pool is running out always try Large item pool */
      len = dsm_pushdown(&dsm_ptr,src + (*pos/8),(uint16)epco_len, DSM_DS_LARGE_ITEM_POOL);
      if(len != (uint16)epco_len) 
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
                                     (uint16)epco_len, 
                                     DSM_DS_LARGE_ITEM_POOL);

    if((uint16)epco_len != dsm_packet_length)
    {
      /*free any partial memory allocated */
      dsm_free_packet(&dsm_ptr);

      MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d small item pool", (uint16)epco_len);

      /* If Small item pool is running out always try Large item pool */
      len = dsm_pushdown(&dsm_ptr,src + (*pos/8),(uint16)epco_len, DSM_DS_SMALL_ITEM_POOL);
      if(len != (uint16)epco_len) 
      {
        MSG_FATAL_DS(MM_SUB,"=EMM= Failed to push len %d small item pool",(uint16)epco_len,0,0);
      }
    }
  }
  
  ext_prot_conf_opt->dsm_ptr = dsm_ptr;

  mm_check_for_null_ptr((void *)dsm_ptr);

  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(epco_len*8, pos, buffer_length, esm_msg_len);

  return TRUE;  
} /* end decode_ext_prot_conf_opt() */

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
boolean decode_hc_config_ie
(
  byte                   *src,
  word                   *pos,
  word                   *buffer_length,
  hc_config_T            *hc_config,
  word                   *esm_msg_len
)
{
  byte num_bits_to_read = 0,index = 0;
  byte hc_config_len = 0;

  /*Length of Header compression configuration IE */
  num_bits_to_read = 8;
  hc_config_len = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /* Based on the length of PCO IE decode the other fields */
  if(hc_config_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d",(*buffer_length/8));
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(hc_config_len < MIN_HC_CONFIG_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received HC CONFIG IE len %d < %d Min allowed",
                   hc_config_len, MIN_HC_CONFIG_LENGTH);
    return FALSE;
  }

  /***** LENGTH Of HC CONFIG IE *****/
  hc_config->len = hc_config_len;

  /***** ROHC profile *****/
  num_bits_to_read = 8;
  hc_config->rohc_profile = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
  hc_config_len--;

  /***** MAX CID profile *****/
  num_bits_to_read = 8;
  hc_config->max_cid[0] = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
  hc_config_len--;
  hc_config->max_cid[1] = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
  hc_config_len--;

  if((hc_config_len > 0) && (hc_config_len <= (*buffer_length/8)))
  {
    /*****Additional header compression context setup parameters type *****/
    num_bits_to_read = 8;
    hc_config->hc_additional_parameters[0] = b_unpackb(src, *pos, num_bits_to_read);
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
    hc_config_len--;
    
    /***** Additional header compression context setup parameters container *****/
    for(index= 0;(index < hc_config_len) && (index < MAX_CP_SETUP_LEN); index++)
    {
      num_bits_to_read = 8;
      hc_config->hc_ctx_setup_container[index] = b_unpackb(src, *pos, num_bits_to_read);
      (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);   
    } /* end for loop */
  }
  return TRUE;  
} /* end decode_hc_config_ie() */
#endif

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
boolean decode_apn_ambr
(
  byte       *src,
  word       *pos, 
  word       *buffer_length, 
  apn_ambr_T *apn_ambr,
  word       *esm_msg_len
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  byte apn_ambr_len = 0, octet = 0;

  /************ Length of APN Aggregate maximum bit rate IE ************/
  num_bits_to_read = 8;
  apn_ambr_len = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /* Based on the length of APN-AMBR IE decode the other fields */
  if(apn_ambr_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), apn_ambr_len);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(apn_ambr_len < MIN_APN_AMBR_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received APN-AMBR IE len %d < %d Min allowed",
                   apn_ambr_len, MIN_APN_AMBR_LENGTH);
    return FALSE;
  }

  num_bits_to_read = 8;
  for(octet = 1; (octet <= apn_ambr_len) && (octet <= MAX_APN_AMBR_LENGTH); octet++)
  {
    unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);

    switch(octet)
    {
      case 1: /***** APN-AMBR for downlink *****/
        apn_ambr->apn_ambr_dl = unpackedbytes;
        break;

      case 2: /***** APN-AMBR for uplink *****/
        apn_ambr->apn_ambr_ul = unpackedbytes;
        break;

      case 3: /***** APN-AMBR for downlink (extended) *****/
        /* 24.301, Section 9.9.4.2.1 - APN-AMBR for downlink (extended)
           All other values shall be interpreted as '1 1 1 1 1 0 1 0' */
        if(unpackedbytes > MAX_APN_AMBR_EXT_UL_DL)
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received APN-AMBR DL (ext) = %d, valid range 0-250. Setting the value to 250",
                         unpackedbytes);
          unpackedbytes = MAX_APN_AMBR_EXT_UL_DL;
        }
        apn_ambr->apn_ambr_dl_ext = unpackedbytes;
        break;

      case 4: /***** APN-AMBR for uplink (extended) *****/
        /* 24.301, Section 9.9.4.2.1 - APN-AMBR for uplink (extended)
           The coding is identical to that of the APN-AMBR for downlink (extended) */
        if(unpackedbytes > MAX_APN_AMBR_EXT_UL_DL)
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received APN-AMBR UL (ext) = %d, valid range 0-250. Setting the value to 250",
                         unpackedbytes);
          unpackedbytes = MAX_APN_AMBR_EXT_UL_DL;
        }
        apn_ambr->apn_ambr_ul_ext = unpackedbytes;
        break;

      case 5: /***** APN-AMBR for downlink (extended-2) *****/
        /* 24.301, Section 9.9.4.2.1 - APN-AMBR for downlink (extended-2)
           '1 1 1 1 1 1 1 1' shall be interpreted as '0 0 0 0 0 0 0 0' */
        if(unpackedbytes > MAX_APN_AMBR_EXT2_UL_DL)
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received APN-AMBR DL (ext2) = %d, valid range 0-254. Setting the value to 0",
                         unpackedbytes);
          unpackedbytes = 0;
        }
        apn_ambr->apn_ambr_dl_ext2 = unpackedbytes;
        break;

      case 6: /***** APN-AMBR for uplink (extended-2) *****/
        /* 24.301, Section 9.9.4.2.1 - APN-AMBR for uplink (extended-2)
           The coding is identical to that of the APN-AMBR for downlink (extended-2) */
        if(unpackedbytes > MAX_APN_AMBR_EXT2_UL_DL)
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received APN-AMBR UL (ext2) = %d, valid range 0-254. Setting the value to 0",
                         unpackedbytes);
          unpackedbytes = 0;
        }
        apn_ambr->apn_ambr_ul_ext2 = unpackedbytes;
        break;
    } /* end switch(octet) */

    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
  } /* end for loop */

  /* Discard additional octets in APN-AMBR IE */
  if(apn_ambr_len > MAX_APN_AMBR_LENGTH)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in APN-AMBR IE",
                  (apn_ambr_len - MAX_APN_AMBR_LENGTH));
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM((apn_ambr_len - MAX_APN_AMBR_LENGTH)*8,
                                  pos, buffer_length, esm_msg_len);
  }

  return TRUE;
 } /* end decode_apn_ambr() */


#ifdef FEATURE_NAS_GW
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
boolean decode_qos
(
  byte  *src,
  word  *pos,
  word  *buffer_length,
  qos_T *neg_qos,
  word  *esm_msg_len
)
{
  byte ie_length = 0,num_bits_to_read = 0, unpackedbytes = 0;
  byte octet = 0, neg_qos_length = 0, value = 0;
  boolean exit_for_loop = FALSE;
  word length_to_skip = 0;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
 
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  
  /************ Length of QoS IE ************/
  num_bits_to_read = 8;
  ie_length = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /* Based on the length of QoS IE decode the other fields */
  if(ie_length > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), ie_length);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(ie_length < MIN_QOS_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received QoS IE len %d < %d Min allowed",
                   ie_length, MIN_QOS_LENGTH);
    return FALSE;
  }

  /* Increment Negotiated QoS length upon decoding each octet */
  for(octet = 1; octet <= ie_length; octet++)
  {
    unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);

    switch(octet)
    {
      case 1: /***** 2 Spare bits + 3 Delay class bits + 3 Reliability class bits *****/
        /* bits:|    8    |    7    |    6    |    5    |    4    |    3    |    2    |   1    |
                |    0         0    |<------- Delay class ------->|<--- Reliability class ---->|
                |<----- spare ----->|                             |                            | */

        /* Extract Delay class value */
        value = ((unpackedbytes & 0x38) >> 3);
        if((value == 0) || (value == 7))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Delay class = %d is reserved. Valid range = 1-6", value);
          return FALSE;
        }
        neg_qos->delay_class = value;

      /* Extract Reliability class value */
        value = (unpackedbytes & 0x7);
        if((value == 0) || (value == 7))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Reliability class = %d is reserved. Valid range = 1-6", value);
          return FALSE;
        }
        neg_qos->reliability_class = value;
        break;

      case 2: /***** 4 Peak throughput bits + 1 Spare bit + 3 Precedence class bits *****/
        /* bits:|    8    |    7    |    6    |    5    |    4    |    3    |    2    |   1    |
                |<---------- Peak throughput ---------->|    0    |<---- Precedence class ---->|
                |                                       |<-spare->|                            | */
  
        /* Extract Peak throughput value */
        value = ((unpackedbytes & 0xF0) >> 4);
        if((value == 0) || (value == 15))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Peak throughput = %d is reserved. Valid range = 1-14",
                         value);
          return FALSE;
        }
        neg_qos->peak_throughput = value;
  
        /* Extract Precedence class value */
        value = (unpackedbytes & 0x7);
        if((value == 0) || (value == 7))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Precedence class = %d is reserved. Valid range = 1-6", value);
          return FALSE;
        }
        neg_qos->precedence_class = value;
        break;

      case 3: /***** 3 Spare bits + 5 Mean throughput bits *****/
        /* bits:|    8    |    7    |    6    |    5    |    4    |    3    |    2    |   1    |
                |    0    |    0    |    0    |<--------------  Mean throughput -------------->|
                |<---------- spare ---------->|                                                | */
  
        /* Extract Mean throughput value */
        value = (unpackedbytes & 0x1F);
        if((value == 0) || (value == 30))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Mean throughput = %d is reserved. Valid range = 1-29 or 31",
                         value);
          return FALSE;
        }
        neg_qos->mean_throughput = value;
        break;
  
      case 4: /***** 3 Traffic class bits + 2 Delivery order bits + 3 Delivery of erroneous SDU bits *****/
        /* bits:|    8    |    7    |    6    |    5    |    4    |    3    |    2    |   1    |
                |<------ Traffic class ------>|<- Delivery order->|<-Delivery of erroneous SDU->| */
  
        /* Extract Traffic class value */
        value = ((unpackedbytes & 0xE0) >> 5);
        if((value == 0) || ((value >= 5) && (value <= 7)))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Traffic class = %d is reserved. Valid range = 1-4",
                         value);
          return FALSE;
        }
        neg_qos->traffic_class = value;

        /* Extract Delivery order value */
        value = ((unpackedbytes & 0x18) >> 3);
        if((value == 0) || (value == 3))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Delivery order = %d is reserved. Valid range = 1-2",
                         value);
          return FALSE;
        }
        neg_qos->deliver_order = value;

        /* Extract Delivery of erroneous SDU value */
        value = (unpackedbytes & 0x7);
        if((value == 0) || ((value >= 4) && (value <= 7)))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Delivery of erroneous SDU = %d is reserved. Valid range = 1-3",
                         value);
          return FALSE;
        }
        neg_qos->deliver_err_sdu = value;
        break;
  
      case 5: /***** Maximum SDU size *****/
        if((unpackedbytes == 0) || (unpackedbytes > 153))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Maximum SDU size = %d is reserved. Valid range = 1-153",
                         unpackedbytes);
          return FALSE;
        }
        neg_qos->max_sdu_size = unpackedbytes;
        break;

      case 6: /***** Maximum bit rate for uplink *****/
        if(unpackedbytes == 0)
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Maximum bit rate for uplink = %d is reserved. Valid range = 1-255",
                         unpackedbytes);
          return FALSE;
        }
        neg_qos->max_bit_rare_uplink = unpackedbytes;
        break;
  
      case 7: /***** Maximum bit rate for downlink *****/
        if(unpackedbytes == 0)
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Maximum bit rate for downlink = %d is reserved. Valid range = 1-255",
                         unpackedbytes);
          return FALSE;
        }
        neg_qos->max_bit_rare_downlink = unpackedbytes;
        break;

      case 8: /***** 4 Residual BER bits + 4 SDU error ratio bits *****/
        /* bits:|    8    |    7    |    6    |    5    |    4    |    3    |    2    |   1    |
                |<----------- Residual BER ------------>|<---------- SDU error ratio --------->| */
  
        /* Extract Residual BER value */
        value = ((unpackedbytes & 0xF0) >> 4);
        if((value == 0) || ((value >= 10) && (value <= 15)))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Residual BER = %d is reserved. Valid range = 1-9",
                         value);
          return FALSE;
        }
        neg_qos->residual_ber = value;

        /* Extract SDU error ratio value */
        value = (unpackedbytes & 0xF);
        if((value == 0) || ((value >= 8) && (value <= 15)))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= SDU error ratio = %d is reserved. Valid range = 1-7",
                         value);
          return FALSE;
        }
        neg_qos->sdu_err_ratio = value;
        break;

      case 9: /***** 6 Transfer delay bits + 2 Traffic handling priority bits *****/
        /* bits:|    8    |    7    |    6    |    5    |    4    |    3    |    2    |   1    |
                |<-------------------- Transfer delay --------------------->|<---- Traffic --->
                |                                                            |      handling   | 
                |                                                            |      priority   | */
  
        /* Extract Transfer delay value.
           The Transfer delay value is ignored if the Traffic Class is Interactive class
           or Background class. */
        if((neg_qos->traffic_class != INTERACTIVE_CLASS) &&
           (neg_qos->traffic_class != BACKGROUND_CLASS))
        {
          value = ((unpackedbytes & 0xFC) >> 2);
          if((value == 0) || (value == 63))
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Transfer delay = %d is reserved. Valid range = 1-62",
                           value);
            return FALSE;
          }
          neg_qos->tranfer_delay = value;
        }
        else
        {
          MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Transfer delay is ignored because traffic class is %d",
                       neg_qos->traffic_class);
        }

        /* Extract Traffic handling priority value.
           The Traffic handling priority value is ignored if the Traffic Class is
           Conversational class, Streaming class or Background class. */
        if((neg_qos->traffic_class != CONVERSATION_CLASS) &&
           (neg_qos->traffic_class != STREAMING_CLASS) &&
           (neg_qos->traffic_class != BACKGROUND_CLASS))
        {
          value = (unpackedbytes & 0x3);
          if(value == 0)
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Traffic handling priority = %d is reserved. Valid range = 1-3",
                           value);
            return FALSE;
          }
          neg_qos->traffic_priority = value;
        }
        else
        {
          MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Traffic handling priority is ignored because traffic class is %d",
                       neg_qos->traffic_class);
        }
        break;

      case 10: /***** Guaranteed bit rate for uplink *****/
        /* The Guaranteed bit rate for uplink value is ignored if the Traffic Class
           is Interactive class or Background class */
        if((neg_qos->traffic_class != INTERACTIVE_CLASS) &&
           (neg_qos->traffic_class != BACKGROUND_CLASS))
        {
          if(unpackedbytes == 0)
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Guaranteed bit rate for uplink = %d is reserved. Valid range = 1-255",
                           unpackedbytes);
            return FALSE;
          }
          neg_qos->guarantee_ber_uplink = unpackedbytes;
        }
        else
        {
          MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Guaranteed bit rate for uplink is ignored because traffic class is %d",
                       neg_qos->traffic_class);
        }
        break;

      case 11: /***** Guaranteed bit rate for downlink *****/
        /* The Guaranteed bit rate for downlink value is ignored if the Traffic Class
           is Interactive class or Background class */
        if((neg_qos->traffic_class != INTERACTIVE_CLASS) &&
           (neg_qos->traffic_class != BACKGROUND_CLASS))
        {
          if(unpackedbytes == 0)
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Guaranteed bit rate for downlink = %d is reserved. Valid range = 1-255",
                           unpackedbytes);
            return FALSE;
          }
          neg_qos->guarantee_ber_dwnlink = unpackedbytes;
        }
        else
        {
          MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Guaranteed bit rate for downlink is ignored because traffic class is %d",
                       neg_qos->traffic_class);
        }
        break;

      case 12: /***** 3 Spare bits + 1 Signalling indication bit + 4 Source statistics descriptor bits *****/
        /* bits:|    8    |    7    |    6    |    5    |    4    |    3    |    2    |   1    |
                |    0    |    0    |    0    |<-Signa->|<--- Source statistics descriptor --->|
                |<---------- spare ---------->|lling in |                                      |
                |                             |dication |                                      | */

        /* Extract Signalling indication value.
           The Signalling Indication value is ignored if the Traffic Class is Conversational class,
           Streaming class or Background class */
        if((neg_qos->traffic_class != CONVERSATION_CLASS) &&
           (neg_qos->traffic_class != STREAMING_CLASS) &&
           (neg_qos->traffic_class != BACKGROUND_CLASS))
        {
          neg_qos->sig_ind = ((unpackedbytes & 0x10) >> 4);
        }
        else
        {
          MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Signalling indication is ignored because traffic class is %d",
                       neg_qos->traffic_class);
        }
  
        /***** Source statistics descriptor *****/
        /* In network to MS direction Source statistics descriptor bits are spare so skip it */
        break;

      case 13: /***** Maximum bit rate for downlink(extended) *****/
        neg_qos->ext_max_bit_rate_downlink = unpackedbytes;
        break;

      case 14: /* Guaranteed bit rate for downlink(extended) */
        neg_qos->ext_gtd_bit_rate_downlink = unpackedbytes;
        break;

      case 15: /***** Maximum bit rate for uplink(extended) *****/
        neg_qos->ext_max_bit_rate_uplink = unpackedbytes;
        break;

      case 16: /***** Guaranteed bit rate for uplink(extended) *****/
        neg_qos->ext_gtd_bit_rate_uplink = unpackedbytes;
        break;

#ifdef FEATURE_LTE_REL11
      case 17: /***** Maximum bit rate for downlink(extended-2) *****/
        if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)
        {
          neg_qos->ext2_max_bit_rate_downlink = b_unpackb(src, *pos, num_bits_to_read);
        }
        else
        {
          /* Octets from Max bit rate for downlink(extended-2) onwards are only supported in spec
             versions from rel-11 onwards. Therefore stop decoding further and skip the remaining
             octets in QoS IE */
          length_to_skip = ie_length - neg_qos_length;
          exit_for_loop = TRUE;
        }
        break;

      case 18: /***** Guaranteed bit rate for downlink(extended-2) *****/
        neg_qos->ext2_gtd_bit_rate_downlink = b_unpackb(src, *pos, num_bits_to_read);
        break;

      case 19: /***** Maximum bit rate for uplink(extended-2) *****/
        neg_qos->ext2_max_bit_rate_uplink = b_unpackb(src, *pos, num_bits_to_read);
        break;

      case 20: /***** Guaranteed bit rate for uplink(extended-2) *****/
        neg_qos->ext2_gtd_bit_rate_uplink = b_unpackb(src, *pos, num_bits_to_read);
        break;
#endif /* FEATURE_LTE_REL11 */
      default:
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Unsupported Octet #%d (after length field)", octet);
        length_to_skip = ie_length - neg_qos_length;
        exit_for_loop = TRUE;
        break;
    } /* end switch(octet) */

    if(exit_for_loop == TRUE)
    {
      break;
    }

    neg_qos_length++;
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
  } /* end for loop */

  /* Discard additional octets in QoS IE */
  if(length_to_skip > 0)
  {
    MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in QoS IE. 3gpp_spec_ver = %d",
                  length_to_skip, emm_ctrl_data_ptr->emm_3gpp_spec_ver);
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(length_to_skip*8, pos, buffer_length, esm_msg_len);
  }

  return TRUE;
} /* end decode_qos() */
#endif

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
boolean decode_pdn_address
(
  byte          *src,
  word          *pos,
  word          *buffer_length,
  pdn_address_T *pdn_address, 
  word          *esm_msg_len
)
{
  byte num_bits_to_read = 0, loop = 0, extra_length = 0;

  MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= Decoding PDN address IE");

  /************ Length of PDN address IE ************/
  num_bits_to_read = 8;
  pdn_address->pdn_addr_len = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /* Based on the length of PDN address IE decode the other fields */
  if(pdn_address->pdn_addr_len > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), pdn_address->pdn_addr_len);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(pdn_address->pdn_addr_len < MIN_PDN_ADDR_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received PDN address IE len %d < %d Min allowed",
                   pdn_address->pdn_addr_len, MIN_PDN_ADDR_LENGTH);
    return FALSE;
  }

  if(pdn_address->pdn_addr_len > MAX_PDN_ADDR_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received PDN address IE len %d > %d Max allowed. Decoding upto Max",
                   pdn_address->pdn_addr_len, MAX_PDN_ADDR_LENGTH);
    extra_length = pdn_address->pdn_addr_len - MAX_PDN_ADDR_LENGTH;
    pdn_address->pdn_addr_len = MAX_PDN_ADDR_LENGTH;
  }

  /* Skip 5 Spare bits and read PDN type value directly */
    
  /***** PDN type value *****/
  num_bits_to_read = 3;
  pdn_address->pdn_type_val = (pdn_type_T)b_unpackb(src, (word)(*pos+5), num_bits_to_read);

  /* Adjust buffer length */
  num_bits_to_read = 8;
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
    
  /***** PDN address information ******/
  num_bits_to_read = 8;
  for(loop = 0; (loop < (pdn_address->pdn_addr_len - 1)) && (loop < MAX_PDN_ADR_LEN); loop++)
  {
    pdn_address->address[loop] = b_unpackb(src, *pos, num_bits_to_read);
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);
  }

  /* Discard additional octets in PDN address IE */
  if(extra_length > 0)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in PDN address IE",
                  extra_length);
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(extra_length*8, pos, buffer_length, esm_msg_len);
  }
  return TRUE;
} /* end decode_pdn_address() */


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
boolean decode_t3396_value
(
  byte             *src,
  word             *pos,
  word             *buffer_length,
  timer_3gpp_type3 *backoff_timer,
  word             *esm_msg_len
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  byte t3396_length = 0;

  /***** Length of T3396 value IE *****/
  num_bits_to_read = 8;
  t3396_length = b_unpackb(src, *pos, num_bits_to_read);

  MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Decoded length of T3396 value IE = %d", t3396_length);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);


  backoff_timer->valid = FALSE;

  /* Based on the length of T3396 value IE decode the other fields */
  if(t3396_length > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), t3396_length);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(t3396_length < MIN_T3396_VALUE_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received T3396 value IE len %d < %d Min allowed",
                   t3396_length, MIN_T3396_VALUE_LENGTH);
    return FALSE;
  }

  /* Read timer octet to decode T3396 timer unit and value */
  num_bits_to_read = 8;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  if((unpackedbytes >> 5) == 7)
  {
    /* Timer is deactivated */
    backoff_timer->active = FALSE;
  }
  else
  {
    /* Timer is active */
    backoff_timer->active = TRUE;
  }

  backoff_timer->timer_count = (dword)gmm_convert_timer3_value_ie_to_msecs(unpackedbytes);

  /* Discard additional octets in T3396 value IE */
  if(t3396_length > MAX_T3396_VALUE_LENGTH)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in T3396 value IE",
                   (t3396_length - MAX_T3396_VALUE_LENGTH));
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM((t3396_length - MAX_T3396_VALUE_LENGTH)*8, pos, buffer_length,
                                   esm_msg_len);
  }

  backoff_timer->valid = TRUE;
  return TRUE;
} /* end decode_t3396_value() */
#endif /* FEATURE_LTE_REL10 */

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
boolean decode_reattempt_ind_value
(
  byte             *src,
  word             *pos,
  word             *buffer_length,
  reattempt_ind_T  *reattempt_ind,
  word             *esm_msg_len
)
{
  byte num_bits_to_read = 0;
  byte ie_length = 0;
  
  ASSERT(src != NULL);
  ASSERT(pos != NULL);
  ASSERT(buffer_length != NULL);
  ASSERT(reattempt_ind != NULL);
  ASSERT(esm_msg_len != NULL);

  reattempt_ind->valid = FALSE;

  /***** Length of IE *****/
  num_bits_to_read = 8;
  ie_length = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /* Based on the length of IE decode the other fields */
  if(ie_length > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), ie_length);
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(ie_length < MIN_RATC_VALUE_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received Reattempt Indicator IE len %d < %d Min allowed",
                   ie_length, MIN_RATC_VALUE_LENGTH);
    return FALSE;
  }

  /* Read reattempt indicator content octet */
  num_bits_to_read = 8;
  reattempt_ind->reattempt_ind_content = b_unpackb(src, *pos, num_bits_to_read);

  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /* Discard additional octets in IE */
  if(ie_length > MAX_RATC_VALUE_LENGTH)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in Reattempt Indicator IE",
                  (ie_length - MAX_RATC_VALUE_LENGTH));
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM((ie_length - MAX_RATC_VALUE_LENGTH)*8, pos, buffer_length,
                                   esm_msg_len);
  }

  reattempt_ind->valid = TRUE;
  return TRUE;
} 
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
boolean decode_serving_plmn_rate_control
(
  byte              *src,
  word              *pos,
  word              *buffer_length,
  splmn_rate_ctrl_T *splmn_rate_ctrl,
  word              *esm_msg_len
)
{
  byte num_bits_to_read = 0, ie_length = 0;

  /***** Length of Serving PLMN rate control IE *****/
  num_bits_to_read = 8;
  ie_length = b_unpackb(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /* Based on the length of Serving PLMN rate control IE decode the other fields */
  if(ie_length > (*buffer_length/8))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d",(*buffer_length/8));
    buffer_length_check_failed = TRUE;
    return FALSE;
  }

  if(ie_length < MIN_SPLMN_RATE_CTRL_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received Serving PLMN rate control IE len %d < %d Min allowed",
                   ie_length, MIN_SPLMN_RATE_CTRL_LENGTH);
    return FALSE;
  }

  /* Read Serving PLMN rate control value */
  num_bits_to_read = 16;
  splmn_rate_ctrl->splmn_rate_ctrl_val = b_unpackw(src, *pos, num_bits_to_read);
  (void)ADJUST_OFFSET_AND_BUF_LEN_ESM(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /* Discard additional octets in Serving PLMN rate control IE */
  if(ie_length > MAX_SPLMN_RATE_CTRL_LENGTH)
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in Serving PLMN rate control IE",
                   (ie_length - MAX_SPLMN_RATE_CTRL_LENGTH));
    (void)ADJUST_OFFSET_AND_BUF_LEN_ESM((ie_length - MAX_SPLMN_RATE_CTRL_LENGTH)*8, pos, buffer_length,
                                   esm_msg_len);
  }

  return TRUE;
} /* end decode_serving_plmn_rate_control() */
#endif
#endif /* FEATURE_LTE */