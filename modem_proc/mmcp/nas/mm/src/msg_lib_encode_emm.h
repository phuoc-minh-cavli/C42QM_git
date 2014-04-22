
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

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/msg_lib_encode_emm.h#1 $
$Author: pwbldsvc $
$DateTime: 2023/04/25 14:39:18 $

when          who     what
--------      ---     -------------------------------------------------------------
04/27/09      hnam     Added authentication response code
04/13/09      hnam     Added code for authentication failure.
==============================================================================*/

#ifndef _MSG_LIB_ENCODE_EMM_H_
#define _MSG_LIB_ENCODE_EMM_H_
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
#include "lte_nas_security_info.h"

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

extern void encode_emm_msg
(
  lte_nas_outgoing_msg_type *out_buf, 
  byte                      *ota_buf, 
  size_t                    ota_buf_len, 
  word                      *pos,
  lte_nas_ul_security_info_type *security_info
);

extern void encode_attach_req
(
  byte                      *ota_buf, 
  size_t                    ota_buf_len, 
  lte_nas_outgoing_msg_type *out_buf,
  word                      *pos
);

extern void encode_attach_complete
(
  byte                      *ota_buf,
  size_t                    ota_buf_len,
  lte_nas_outgoing_msg_type *out_buf, 
  word                      *pos
);

extern void encode_service_request
(
  byte                      *ota_buf, 
  lte_nas_outgoing_msg_type *out_buf, 
  word *pos
);

extern void encode_security_mode_complete
(
  byte                      *ota_buf,
  size_t                    ota_buf_len,
  lte_nas_outgoing_msg_type *out_buf, 
  word                      *pos
);

extern void encode_mo_detach_request
(
  byte                      *ota_buf, 
  lte_nas_outgoing_msg_type *out_buf, 
  word                      *pos
);

extern void encode_authentication_failure
(
  byte                      *ota_buf, 
  lte_nas_outgoing_msg_type *out_buf, 
  word                      *pos
);

extern void encode_security_protected_nas_hdr
(
  lte_nas_outgoing_msg_type     *out_msg, 
  byte                          *ota_buf,
  lte_nas_ul_security_info_type *security_info, 
  word                          *msg_len
);

extern void lte_nas_cipher_user_data
(
  byte                          *ota_buf,
  lte_nas_ul_security_info_type *security_info,
  uint16                        *pay_load_length
);

extern void encode_authentication_response
(
  byte                      *ota_buf, 
  lte_nas_outgoing_msg_type *out_buf, 
  word                      *pos
);

extern boolean service_request;

extern void encode_tau_request
(
  byte                      *ota_buf, 
  lte_nas_outgoing_msg_type *out_buf, 
  word                      *pos
);

extern void encode_identity_resp
(
  byte                      *ota_buf, 
  lte_nas_outgoing_msg_type *out_buf, 
  word                      *pos
);

void encode_security_reject
(
  byte                      *ota_buf, 
  lte_nas_outgoing_msg_type *out_buf, 
  word                      *pos
);

#endif

#endif /*FEATURE_LTE*/
