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

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/msg_lib_declarations.h#1 $
$Author: pwbldsvc $
$DateTime: 2023/04/25 14:39:18 $

when         who                          what                              
--------     --------  ------------------------------------------------
05/17/09   hnam    Added the code to support security
==============================================================================*/

#ifndef _MSG_LIB_DECLARATIONS_H_
#define _MSG_LIB_DECLARATIONS_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "lte_nas_emm_message_type.h"
#include "lte_nas_esm_message_type.h"
#include "lte_nas.h"
#include "lte_nas_security_info.h"
#include "mm_v.h"


/*==============================================================================
                      MACRO DECLARATIONS
==============================================================================*/
#define WORD_SIZE 8

#define MIN_EMM_LEN 2*8 /* pd + security header + message type */

#define MIN_ESM_LEN 3*8 /* pd + security header + message type  + Bearer Id */

#define ADJUST_OFFSET_AND_BUF_LEN_EMM(num_bits_read,offset_ptr,buffer_len_ptr)\
            {*offset_ptr = *offset_ptr + num_bits_read;\
             *buffer_len_ptr = *buffer_len_ptr - num_bits_read;}
			 
#define SKIP_DECODING_ESM(offset_ptr,buffer_len_ptr,esm_len)\
           {*offset_ptr = *offset_ptr + esm_len;\
            *buffer_len_ptr = *buffer_len_ptr - esm_len;\
            esm_len = 0;}



#define MAX_BCD_VAL 9

/*==============================================================================
                      DATA DECLARATIONS
==============================================================================*/
extern boolean msg_lib_encode_error;

extern boolean buffer_length_check_failed;

extern byte total_num_messages;

extern byte nas_emm_index;

extern byte nas_esm_index;

extern boolean detach_reason;

/*==============================================================================
                      FUNCTION DECLARATIONS
==============================================================================*/
boolean ADJUST_OFFSET_AND_BUF_LEN_ESM(word num_bits_read,word* offset_ptr,word* buffer_len_ptr,word* esm_len_ptr);


#ifdef FEATURE_LTE_NAS_DEEP_SEC
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
extern word msg_lib_validate_update_incoming_nas_msg_length
( 
  word                           buffer_length,
  lte_nas_protocol_discriminator pd,
  lte_nas_message_id             message_id
);
#endif


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
extern void report_emm_success
(
  lte_nas_incoming_msg_type *nas_incoming_msg_ptr
);


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
extern void report_emm_failure
(
  lte_nas_incoming_msg_type *nas_incoming_msg_ptr
);


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
extern void unknown_emm_msg_received
(
  lte_nas_incoming_msg_type *nas_incoming_msg_ptr
);


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
extern void report_esm_failure
(
  lte_nas_incoming_msg_type *nas_incoming_msg_ptr
);


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
extern void report_esm_success
(
  lte_nas_incoming_msg_type *nas_incoming_msg_ptr
);


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
extern dword calc_gprs_timer
(
  byte gprs_timer_octet
);


/*===========================================================================
FUNCTION      NAS_MSG_INTEGRITY_CHECK

DESCRIPTION
  This function performs integrity check of a received DL NAS message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If integrity check is successful
  FALSE: If inegrity check failed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean nas_msg_integrity_check
( 
  byte                          *src,
  word                          *pos,
  word                          *buffer_length,
  lte_nas_dl_security_info_type *nas_dl_sec_info,
  boolean                       nas_msg_lib,
  boolean                       smc
);


/*===========================================================================
FUNCTION      REPORT_INVALID_GUTI_OR_NAS_SECURITY

DESCRIPTION:  In order for INTER-RAT to happen, NAS_SECURITY & GUTI must be valid.
This function reports if NAS_SECURITY is invalid.
   
DEPENDENCIES
  None

RETURN VALUE
  TRUE: If GUTI and NAS Security valid
  FALSE: If either GUTI or NAS Security or both invalid

SIDE EFFECTS
===========================================================================*/
extern boolean report_invalid_guti_or_nas_security
(
  boolean                   guti_present,
  lte_nas_incoming_msg_type *nas_incoming_msg_ptr,
  emm_ctrl_data_type        *emm_ctrl_data_ptr
);


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
);

#endif /* FEATURE_LTE */
#endif /* _MSG_LIB_DECLARATIONS_H_ */