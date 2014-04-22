/*!
  @file
  lte_rrc_mh_test_common.cpp

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/rrc_mh/lte_rrc_mh_test_common.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "__lte_rrc_mh.h"
#include "__lte_rrc_controller.h"
#include "lte_rrc_mhi.h"
#include "lte_static_rrc_mem_util.h"
#include "lte_pdcp_msg.h"
#include "dsm_item.h"
#include "dsm_init.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_rrc_mh_asn1_test_util.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "tf_stub.h"
#include "lte_rrc_mh_test_common.h"



/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/





/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/




/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/

void lte_rrc_mh_test_send_ul_dcch_msg(boolean ack_needed, int mu_id, unsigned char *msg_ptr, int length)
{
  lte_rrc_send_ul_msg_reqi_s ul_msg_reqi;
  int decode_status;
  
  // ----------------------------------------------------------------------

  lte_rrc_init_default_hdr(&ul_msg_reqi.msg_hdr, LTE_RRC_SEND_UL_MSG_REQI);

  ul_msg_reqi.ack_needed = ack_needed;
  ul_msg_reqi.rb_id = 1;
  ul_msg_reqi.mu_id = (uint8 *)lte_rrc_mem_calloc(sizeof(uint8));
  ul_msg_reqi.encode_status = (uint8 *)lte_rrc_mem_calloc(sizeof(uint8));
  ul_msg_reqi.ul_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(msg_ptr,
                               4,
                               UL_DCCH_Message,
                               &decode_status);

  lte_rrc_utf_send_msg((byte*)&ul_msg_reqi, sizeof(ul_msg_reqi));
  lte_rrc_utf_wait_for_done();

  /* Encoding should be successful */
  TF_ASSERT(*(ul_msg_reqi.encode_status) == 0);
  /* For the first message, mu_id should be 0 */
  if(ack_needed == TRUE)
  {
    TF_ASSERT(*(ul_msg_reqi.mu_id) == mu_id);
  }

  lte_rrc_mem_free(ul_msg_reqi.mu_id);
  lte_rrc_mem_free(ul_msg_reqi.encode_status);
}




/*===========================================================================

                                UNIT TEST

===========================================================================*/

