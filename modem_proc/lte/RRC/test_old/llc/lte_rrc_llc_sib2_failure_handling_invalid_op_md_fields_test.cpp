/*!
  @file lte_rrc_llc_sib2_failure_handling_invalid_op_md_fields_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of 
  a LTE_RRC_CFG_REQI with basic SIB2 update and failure handling for optional
  and manditory default fields.

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //source/qcom/qct/modem/lte/RRC/main/latest/test/llc/
lte_rrc_llc_sib2_failure_handling_invalid_op_md_fields_test.cpp#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/09   da     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_rrc_llc.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_rrc_sib.h"
#include "lte_static_rrc_mem_util.h"
}

#include "lte_rrc_llc_test_common.h"
#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include <tf_stub.h>  

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

extern "C"
{
  TF_PROTO_STUB(void *, lte_rrc_sib_get_cell_sib, lte_rrc_sib_bitmask_t,lte_rrc_sib_db_key_t);
}

/*! @brief Typedef of variables internal to module lte_rrc_llc_sib2_failure_handling_invalid_op_md_fields_test.cpp
*/
typedef struct
{
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib2_ptr;
} lte_rrc_llc_sib2_failure_handling_invalid_op_md_fields_test_s;

static lte_rrc_llc_sib2_failure_handling_invalid_op_md_fields_test_s 
  lte_rrc_llc_sib2_failure_handling_invalid_op_md_fields_test_data;

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

                              FUNCTIONS

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_rrc_sib2_with_invalid_op_md_fields

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API that returns the serving cell SIBs.

    @return
    Pointer to SIB2

 */
/*=========================================================================*/
void *lte_rrc_sib2_with_invalid_op_md_fields
(
  lte_rrc_sib_bitmask_t sib, /*!< Sib requested; 1=SIB1, 2=SIB2, ...*/
  lte_rrc_sib_db_key_t key /*!< sib db index */
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_SystemInformationBlockType2 *sib2_ptr = NULL;
  int decode_status = 0;
  /* Initializing(using) the parameters to handle compiler warnings */
  sib = 0;
  key = 0;


  if (lte_rrc_llc_sib2_failure_handling_invalid_op_md_fields_test_data.bcch_dl_sch_sib2_ptr  == NULL) 
  {
    encoded_pdu.length = sizeof(llc_basic_sib2_2);
    encoded_pdu.value = llc_basic_sib2_2;
    
    lte_rrc_llc_sib2_failure_handling_invalid_op_md_fields_test_data.bcch_dl_sch_sib2_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, 
                                 encoded_pdu.length,
                                 BCCH_DL_SCH_Message,
                                 &decode_status);
     
    TF_ASSERT(lte_rrc_llc_sib2_failure_handling_invalid_op_md_fields_test_data.bcch_dl_sch_sib2_ptr != NULL);

  }
    
  sib2_ptr = lte_rrc_llc_sib2_failure_handling_invalid_op_md_fields_test_data.
    bcch_dl_sch_sib2_ptr->message.u.c1->u.systemInformation->criticalExtensions.u.
    systemInformation_r8->sib_TypeAndInfo.elem->u.sib2;
  /* Set prach_FreqOffset to invalid value */
  sib2_ptr->radioResourceConfigCommon.prach_Config.prach_ConfigInfo.
    prach_FreqOffset = 95;
  
  /* return sib 2 ptr */
  return sib2_ptr;
} /* lte_rrc_sib2_with_invalid_op_md_fields() */

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcSib2FailureHandlingInvalidOpMdFieldsTest);

void LteRrcLlcSib2FailureHandlingInvalidOpMdFieldsTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = 
    lte_rrc_sib2_with_invalid_op_md_fields;
}

void LteRrcLlcSib2FailureHandlingInvalidOpMdFieldsTest::Test()
{
  lte_rrc_cfg_reqi_s cfg_reqi; 
  uint32 trans_id;
  /* ---------------------------------------------------------------------- */

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  TF_MSG("\nLLC: Send CFG_REQI for SIB2 Update \n");
  /* build CFG_REQI for COMMON CFG and send */
  lte_rrc_llc_test_construct_common_cfg_reqi(&cfg_reqi, FALSE, TRUE, TRUE, &trans_id);
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  
  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_DATA_INVALID, LTE_RRC_PROC_CSP, cfg_reqi.trans_id_ptr);

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_sib2_failure_handling_invalid_op_md_fields_test_data.
                             bcch_dl_sch_sib2_ptr, 
                             lte_rrc_osys_BCCH_DL_SCH_Message_PDU);

  TF_MSG("\nLLC: Basic SIB2 Update Test Ends \n");

  /* ---------------------------------------------------------------------- */  
  lte_rrc_utf_reset(); 
}

void LteRrcLlcSib2FailureHandlingInvalidOpMdFieldsTest::Teardown()
{
  lte_rrc_utf_teardown();
}







