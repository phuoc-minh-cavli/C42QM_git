/*!
  @file lte_rrc_llc_invalid_sib2_update_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of 
  a LTE_RRC_CFG_REQI with invalid SIB2 update.

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

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/09/08   da     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "__lte_rrc_llc_int.h"
#include "lte_rrc_llc.h"
#include "lte_rrc_sib.h"
#include "lte_static_rrc_mem_util.h"
}

#include "lte_rrc_llc_test_common.h"
#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include <tf_stub.h>  


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module lte_rrc_llc_invalid_sib2_update_test.cpp
*/
typedef struct
{
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_msg_ptr;
} lte_rrc_llc_invalid_sib2_update_s;


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_llc_invalid_sib2_update_test.cpp
*/
static lte_rrc_llc_invalid_sib2_update_s invalid_sib2_update_data;

extern "C"
{
  TF_PROTO_STUB(void *, lte_rrc_sib_get_cell_sib, lte_rrc_sib_bitmask_t, lte_rrc_sib_db_key_t);
}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/
void *lte_rrc_invalid_sib2_update_test_get_serv_cell_sib
(
  lte_rrc_sib_bitmask_t sib, /*!< Sib requested; 1=SIB1, 2=SIB2, ...*/
  lte_rrc_sib_db_key_t key /*!< sib db index */
)
{

  lte_rrc_pdu_buf_s encoded_pdu;
  int decode_status = 0;
  /* Initializing(using) the parameters to handle compiler warnings */
  sib = 0;
  key = 0;

  encoded_pdu.length = 29;
  encoded_pdu.value = llc_invalid_sib2_spare_delta_field;

  invalid_sib2_update_data.bcch_dl_sch_msg_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value,
                               encoded_pdu.length,
                               BCCH_DL_SCH_Message,
                               &decode_status);
  TF_ASSERT(decode_status == 0);
  /* return sib 2 ptr */
  return invalid_sib2_update_data.bcch_dl_sch_msg_ptr->message.u.c1->u.
	systemInformation->criticalExtensions.u.systemInformation_r8->sib_TypeAndInfo.elem->u.sib2;
} /* lte_rrc_invalid_sib2_update_test_get_serv_cell_sib() */

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(NO_RUN, LteRrcLlcInvalidSib2Update);

void LteRrcLlcInvalidSib2Update::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  TF_STUB(lte_rrc_sib_get_cell_sib) = 
	lte_rrc_invalid_sib2_update_test_get_serv_cell_sib;
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcInvalidSib2Update::Test()
{

  lte_rrc_cfg_reqi_s cfg_reqi; 
  uint32 trans_id;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: invalid SIB2 Update Test Begins \n");
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  TF_MSG("\nLLC: Send CFG_REQI for SIB2 Update \n");
  /* build CFG_REQI for COMMON CFG and send */
  lte_rrc_llc_test_construct_common_cfg_reqi(&cfg_reqi, FALSE, TRUE, FALSE, &trans_id);
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CSP, cfg_reqi.trans_id_ptr);
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  /* free memory allocated for transaction identifier */
  lte_rrc_mem_free(cfg_reqi.trans_id_ptr);
  lte_rrc_osys_asn1_free_pdu(invalid_sib2_update_data.bcch_dl_sch_msg_ptr, 
                             lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  TF_MSG("\nLLC: invalid SIB2 Update Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcInvalidSib2Update::Teardown()
{
  lte_rrc_utf_teardown();
}







