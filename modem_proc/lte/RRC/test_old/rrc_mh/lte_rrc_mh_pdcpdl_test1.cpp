/*!
  @file
  lte_rrc_mh_test.cpp

  @brief
  Unit tests for MH
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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/rrc_mh/lte_rrc_mh_pdcpdl_test1.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/05/08   amit    Initial Version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "lte_rrc_mh_asn1_test_util.h"
#include "__lte_rrc_mh.h"
#include "lte_rrc_mhi.h"
#include "lte_static_rrc_mem_util.h"
#include "lte_pdcp_msg.h"
#include "dsm_item.h"
#include "dsm_init.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_rrc_llc.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "tf_stub.h"

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

extern "C"
{
  TF_PROTO_STUB(stm_state_t, lte_rrc_controller_get_state, void);
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s *);
  extern lte_rrc_mh_s lte_rrc_mh;
}

extern stm_state_t lte_rrc_mh_test_stub_controller_get_state();

extern lte_errno_e lte_rrc_mh_test_stub_csp_get_serving_cell_params(lte_rrc_cell_info_s *);




/*===========================================================================

                                UNIT TEST

===========================================================================*/
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMHPdcpdlTest1);

void LteRrcMHPdcpdlTest1::Setup()
{
  
  lte_rrc_utf_setup(LTE_RRC_MH_SM, NULL, 0);

  TF_STUB(lte_rrc_controller_get_state) = 
    lte_rrc_mh_test_stub_controller_get_state;
  TF_STUB(lte_rrc_csp_get_serving_cell_params) = 
    lte_rrc_mh_test_stub_csp_get_serving_cell_params;
}

void LteRrcMHPdcpdlTest1::Test()
{
  
  lte_rrc_dlm_processed_indi_s ind;

  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

  // ----------------------------------------------------------------------

  for(int i = 0; i < LTE_RRC_MH_MAX_NUM_DL_MSG_SAVED + 4; i++)
  {
    lte_pdcpdl_sdu_ind_msg_s dl_msg_ind;
    dsm_item_type *dsm_ptr=NULL;                /* DSM pointer */
    msgr_attach_struct_type *att_ptr;           /* Message attachment pointer */
    unsigned char *encoded_msg;
    long length;

    TF_ASSERT(lte_rrc_mh.dd_ptr->dlm_processing_active == FALSE);

    msgr_init_hdr_attach(&dl_msg_ind.hdr, MSGR_LTE_PDCPDL, LTE_PDCPDL_SDU_IND, 0, 1);
    length = lte_rrc_mh_test_get_CDMA2000_CSFBParametersResponse(&encoded_msg);
    TF_ASSERT(dsm_pushdown(&dsm_ptr, encoded_msg, (uint16)length, DSM_DS_SMALL_ITEM_POOL) == (uint16)length);
    TF_ASSERT(dsm_ptr != NULL);
    att_ptr = msgr_get_attach(&dl_msg_ind.hdr, 0);
    msgr_set_dsm_attach(att_ptr, dsm_ptr);
    dl_msg_ind.rb_cfg_idx = 33;
    lte_rrc_utf_send_msg((byte*)&dl_msg_ind, sizeof(dl_msg_ind));
    lte_rrc_utf_wait_for_done();

    TF_ASSERT(lte_rrc_mh.dd_ptr->dlm_processing_active == TRUE);

    (void)lte_rrc_osys_asn1_free_buf(encoded_msg);
    lte_rrc_mh_dlm_s *dlm_ptr;
    uint32 buf_len;
    lte_rrc_utf_get_next_int_msg((byte **)&dlm_ptr, &buf_len);
    TF_ASSERT(dlm_ptr->msg_hdr.id == LTE_RRC_CSFB_PARAMETERS_RESPONSE_CDMA2000_DLM);
    TF_ASSERT(dlm_ptr->dl_msg_ptr != NULL);
    TF_ASSERT(dlm_ptr->pdu_num == lte_rrc_osys_DL_DCCH_Message_PDU);


    TF_ASSERT(stm_get_state(LTE_RRC_MH_SM) == LTE_RRC_MH_SM__INITIAL);

    /* send DLM_PROCESSED_INDI */
    lte_rrc_init_default_hdr(&ind.msg_hdr,LTE_RRC_DLM_PROCESSED_INDI);
    lte_rrc_utf_send_msg((byte*)&ind, sizeof(ind));
    lte_rrc_utf_wait_for_done();
  }

  lte_rrc_utf_reset();
}


void LteRrcMHPdcpdlTest1::Teardown()
{
  lte_rrc_utf_teardown();
}
