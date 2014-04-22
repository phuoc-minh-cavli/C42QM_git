/*!
  @file
  

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/conn_establishment/lte_rrc_cep_test_asn1_common.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "lte_rrc_cep_test_asn1_common.h"
#include "lte_assert.h"


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/





/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/


/*! @brief Typedef variables internal to module 
*/

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/

void lte_rrc_test_cep_decode_conn_req
(
  void *encoded_msg,
  void *my_msg,
  uint16 length
)
{
  int decode_status = 0;
  lte_rrc_osys_UL_CCCH_Message *ul_ccch_ptr = NULL;
  lte_rrc_cep_test_conn_req_s *decoded_conn_req = 
    (lte_rrc_cep_test_conn_req_s *)my_msg;

  ul_ccch_ptr = (lte_rrc_osys_UL_CCCH_Message *)
  lte_rrc_osys_asn1_decode_pdu(encoded_msg, length,  
                               UL_CCCH_Message, &decode_status);
  LTE_ASSERT(ul_ccch_ptr !=NULL);
  LTE_ASSERT(decoded_conn_req !=NULL);
  decoded_conn_req->msg_c1_choice = 
    ul_ccch_ptr->message.t;
  decoded_conn_req->msg_choice =
    ul_ccch_ptr->message.u.c1->t;
  decoded_conn_req->conn_req_r8_choice = 
    ul_ccch_ptr->message.u.c1->u.rrcConnectionRequest->criticalExtensions.t;
  decoded_conn_req->est_cause = 
    ul_ccch_ptr->message.u.c1->u.rrcConnectionRequest->criticalExtensions.u.rrcConnectionRequest_r8->establishmentCause;

  lte_rrc_osys_asn1_free_pdu(ul_ccch_ptr, lte_rrc_osys_UL_CCCH_Message_PDU);   
}




/*===========================================================================

                                UNIT TEST

===========================================================================*/

