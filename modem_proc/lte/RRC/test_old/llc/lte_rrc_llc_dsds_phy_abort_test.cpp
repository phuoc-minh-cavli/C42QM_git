/*!
  @file lte_rrc_llc_dsds_phy_abort_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of CPHY ABORT for DSDS

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
9/5/13   MD     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "lte_rrc_llc.h"
#include "lte_rrc_sib.h"
#include "lte_static_rrc_mem_util.h"
#include "lte_static_rrc_pendq.h"


}

#include "lte_rrc_llc_test_common.h"
#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include <tf_stub.h>  

extern "C"
{
  TF_PROTO_STUB(void *, lte_rrc_sib_get_cell_sib, lte_rrc_sib_bitmask_t, lte_rrc_sib_db_key_t);
  //TF_PROTO_STUB(void, lte_rrc_controller_get_cphy_rf_status_info, lte_rrc_rf_status_info_s *);
  TF_PROTO_STUB(uint32, lte_rrc_csp_get_last_cfg_req_id, void);
}

/*! @brief Typedef of variables internal to module lte_rrc_llc_basic_sib1_update_test.cpp
*/

typedef struct
{
  uint32 trans_id; 
  lte_l1_mib_info_param_s llc_basic_mib;
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib1_ptr;
} lte_rrc_llc_basic_sib1_update_s;

typedef struct
{
  uint32 callback_count;
  boolean callback_return;

} lte_rrc_llc_test_s;


typedef struct
{
  msgr_hdr_s        hdr;
  int               dont_care;

} lte_rrc_llc_test_dummy_msg_s;


/*! @brief Variables internal to module lte_rrc_llc_basic_sib1_update_test.cpp
*/
static lte_rrc_llc_basic_sib1_update_s lte_rrc_llc_basic_sib1_update_data_phy_abort;

STATIC lte_rrc_llc_test_s lte_rrc_llc_test;


/*===========================================================================

                                FUNCTIONS

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_cell_sib1_for_cphy_abort

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API that returns the serving cell SIB1.

    @return
    Pointer to SIB2

 */
/*=========================================================================*/
void *lte_rrc_llc_test_get_cell_sib1_for_cphy_abort
(
  lte_rrc_sib_bitmask_t sib, /*!< Sib requested; 1=SIB1, 2=SIB2, ...*/
  lte_rrc_sib_db_key_t key /*!< sib db index */
)
{
  lte_rrc_pdu_buf_s encoded_pdu;
  int decode_status;
  /* Initializing(using) the parameters to handle compiler warnings */
  key = 0;

  if (sib == LTE_RRC_SIB1_BITMASK) 
  {
    if (lte_rrc_llc_basic_sib1_update_data_phy_abort.bcch_dl_sch_sib1_ptr == NULL) 
    {    
      
      encoded_pdu.length = sizeof(llc_basic_sib1_1);
      encoded_pdu.value = llc_basic_sib1_1;
      lte_rrc_llc_basic_sib1_update_data_phy_abort.bcch_dl_sch_sib1_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                                   BCCH_DL_SCH_Message, &decode_status);
      TF_ASSERT(decode_status == 0);
      // Mask of p_max present bitmask
      lte_rrc_llc_basic_sib1_update_data_phy_abort.bcch_dl_sch_sib1_ptr->message.u.c1->u.
      systemInformationBlockType1->m.p_MaxPresent = 0; 
    }
    /* return sib 1 ptr */
    return lte_rrc_llc_basic_sib1_update_data_phy_abort.bcch_dl_sch_sib1_ptr->message.u.c1->u.
      systemInformationBlockType1; 
  }
  else if (sib == LTE_RRC_MIB_BITMASK) 
  {
    lte_rrc_llc_basic_sib1_update_data_phy_abort.llc_basic_mib.num_tx_antennas = 1;
    return &lte_rrc_llc_basic_sib1_update_data_phy_abort.llc_basic_mib;
  }
  else
  { 
    TF_ASSERT(0);
  }

} /* lte_rrc_llc_test_get_cell_sib1_for_cphy_abort() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_csp_trans_id

===========================================================================*/
/*!
 
    @return
    Uint32

 */
/*=========================================================================*/
uint32 lte_rrc_llc_test_get_csp_trans_id
(
  void
)
{
  return 2;

} /* lte_rrc_llc_test_get_csp_trans_id() */


boolean LteRrcLlcPendingQueue__CallbackForDsds(byte *msg_ptr, uint32 msg_len)
{
  lte_rrc_llc_test_s *priv = &lte_rrc_llc_test;

  TF_ASSERT(msg_ptr != NULL);

  // Increment the callback count
  priv->callback_count++;

  return priv->callback_return;
}


/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcPhyRFHandling);

void LteRrcLlcPhyRFHandling::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM,lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_get_cell_sib1_for_cphy_abort;
}

void LteRrcLlcPhyRFHandling::Test()
{
  uint32 buf_len; 
  lte_rrc_cfg_reqi_s cfg_reqi;
  lte_cphy_rf_available_ind_s available_req;
  lte_cphy_rf_unavailable_ind_s unavailable_req;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_cphy_common_cfg_cnf_s cphy_common_cfg_cnf;
  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;
  lte_rrc_cfg_cnfi_s *cfg_cnfi_ptr = NULL;
  TF_MSG("\nLLC: CPHY RF handling (DSDS) test begins \n");
  // LLC should be in initial state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  // build CFG_REQI for COMMON CFG and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_SRV_COMMON;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = TRUE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSent CFG REQI for SIB1 Update\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  // LLC should be in WAIT_FOR_CPHY_CFG_CNF state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);

  TF_MSG("\nLLC waiting for CPHY_CFG_CNF");

  // Send RF unavailable IND
  lte_rrc_init_default_hdr((msgr_hdr_s *)&unavailable_req.msgr_hdr,LTE_CPHY_RF_UNAVAILABLE_IND);
  TF_MSG("\nSending RF UNAVAILABLE IND\n");
  lte_rrc_utf_send_msg((byte*)&unavailable_req, sizeof(unavailable_req));
  lte_rrc_utf_wait_for_done();

  // Send RF available IND
  lte_rrc_init_default_hdr((msgr_hdr_s *)&available_req.msgr_hdr, LTE_CPHY_RF_AVAILABLE_IND);
  TF_MSG("\nSending RF AVAILABLE IND\n");
  lte_rrc_utf_send_msg((byte*)&available_req, sizeof(available_req));
  lte_rrc_utf_wait_for_done();

  TF_MSG("\nLLC: Send CPHY_COMMON_CFG_CNF with sucess \n");
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req_ptr != NULL);
  TF_ASSERT(mac_cfg_req_ptr->hdr.id == LTE_MAC_CFG_REQ);

  /* send MAC_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send MAC_CFG_CNF with sucess \n");
  lte_rrc_llc_test_mac_cfg_cnf(&mac_cfg_cnf, TRUE);
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf, sizeof(mac_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status == E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_NONE);
  TF_MSG("\nReceived CNFI for CSP with Failure type None");

  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_basic_sib1_update_data_phy_abort.bcch_dl_sch_sib1_ptr, 
                                           lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  // Make sure RRC does not throw any more messages
  lte_rrc_utf_check_no_more_msgs();
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  lte_rrc_utf_reset(); 

}

void LteRrcLlcPhyRFHandling::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcRfUnavailableWhileSendingReqToPhy);

void LteRrcLlcRfUnavailableWhileSendingReqToPhy::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM,lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_get_cell_sib1_for_cphy_abort;
  //TF_STUB(lte_rrc_controller_get_cphy_rf_status_info) = lte_rrc_llc_test_cphy_rf_status_un_available;
}

void LteRrcLlcRfUnavailableWhileSendingReqToPhy::Test()
{
  uint32 buf_len; 
  lte_rrc_cfg_reqi_s cfg_reqi;
  lte_cphy_rf_available_ind_s available_req;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_cphy_common_cfg_cnf_s cphy_common_cfg_cnf;
  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;
  lte_rrc_cfg_cnfi_s *cfg_cnfi_ptr = NULL;
  TF_MSG("\nLLC: CPHY RF handling (DSDS) test begins \n");
  // LLC should be in initial state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  TF_MSG("\nLLC: RF Status is unavailable at the start \n");
  
  // build CFG_REQI for COMMON CFG and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_SRV_COMMON;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = TRUE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSent CFG REQI for SIB1 Update\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  // LLC should be in WAIT_FOR_CPHY_CFG_CNF state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);

  TF_MSG("\nLLC waiting for CPHY_CFG_CNF");

  // Send RF available IND
  lte_rrc_init_default_hdr((msgr_hdr_s *)&available_req.msgr_hdr, LTE_CPHY_RF_AVAILABLE_IND);
  TF_MSG("\nSending RF AVAILABLE IND\n");
  lte_rrc_utf_send_msg((byte*)&available_req, sizeof(available_req));
  lte_rrc_utf_wait_for_done();

  //TF_STUB(lte_rrc_controller_get_cphy_rf_status_info) = lte_rrc_llc_test_cphy_rf_status_available;

  TF_MSG("\nLLC: Send CPHY_COMMON_CFG_CNF with sucess \n");
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req_ptr != NULL);
  TF_ASSERT(mac_cfg_req_ptr->hdr.id == LTE_MAC_CFG_REQ);

  /* send MAC_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send MAC_CFG_CNF with sucess \n");
  lte_rrc_llc_test_mac_cfg_cnf(&mac_cfg_cnf, TRUE);
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf, sizeof(mac_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status == E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_NONE);
  TF_MSG("\nReceived CNFI for CSP with Failure type None");

  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_basic_sib1_update_data_phy_abort.bcch_dl_sch_sib1_ptr, 
                                           lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  // Make sure RRC does not throw any more messages
  lte_rrc_utf_check_no_more_msgs();
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  lte_rrc_utf_reset(); 

}

void LteRrcLlcRfUnavailableWhileSendingReqToPhy::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcPhyAbortDuringSIBUpdateWaitingForL1Cnf);

void LteRrcLlcPhyAbortDuringSIBUpdateWaitingForL1Cnf::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM,lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_get_cell_sib1_for_cphy_abort;
}

void LteRrcLlcPhyAbortDuringSIBUpdateWaitingForL1Cnf::Test()
{
  uint32 buf_len; 
  lte_rrc_cfg_reqi_s cfg_reqi;
  lte_rrc_cfg_cnfi_s *cfg_cnfi_ptr = NULL;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_cphy_abort_req_s *cphy_abort_req_ptr = NULL;
  lte_cphy_abort_cnf_s cphy_abort_cnf;
  TF_MSG("\nLLC: CPHY ABORT (DSDS) During SIB UPDATE begins \n");
  // LLC should be in initial state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  // build CFG_REQI for COMMON CFG and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_SRV_COMMON;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = TRUE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSent CFG REQI for SIB1 Update\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  // LLC should be in WAIT_FOR_CPHY_CFG_CNF state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);

  TF_MSG("\nLLC waiting for CPHY_CFG_CNF");
  TF_MSG("\nSending CFG REQI (CSP) that will be pended \n");
  // Send Another CFG REQI which will be pended
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_SRV_COMMON;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  // Send CPHY ABORT
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_ABORT_ML1;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSending High Priority CPHY ABORT \n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  // Catch all CNFI's
  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status != E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_PHY_ABORT);
  TF_MSG("\nReceived CNFI for CSP with Failure type Abort\n");

  TF_MSG("\nCPHY ABORT sent\n");
  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req_ptr, &buf_len);
 
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  TF_ASSERT(cphy_abort_req_ptr != NULL);
  TF_ASSERT(cphy_abort_req_ptr->msgr_hdr.id == LTE_CPHY_ABORT_REQ);
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;
  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status != E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_PHY_ABORT);
  TF_MSG("\nReceived CNFI for CSP with Failure type Abort\n");

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status == E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_NONE);
  TF_MSG("\nReceived CNFI for CSP with Failure type None\n");

  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_basic_sib1_update_data_phy_abort.bcch_dl_sch_sib1_ptr, 
                                           lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  // Make sure RRC does not throw any more messages
  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_utf_reset(); 

}

void LteRrcLlcPhyAbortDuringSIBUpdateWaitingForL1Cnf::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcB2BAbortHandlingWithCspReqInPendq);

void LteRrcLlcB2BAbortHandlingWithCspReqInPendq::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM,lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_get_cell_sib1_for_cphy_abort;
}

void LteRrcLlcB2BAbortHandlingWithCspReqInPendq::Test()
{
  uint32 buf_len; 
  lte_rrc_cfg_reqi_s cfg_reqi;
  lte_rrc_cfg_cnfi_s *cfg_cnfi_ptr = NULL;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_cphy_abort_req_s *cphy_abort_req_ptr = NULL;
  lte_cphy_abort_cnf_s cphy_abort_cnf;
  stm_state_t state;
  int i;

for(i=0;i<11;i++)
{
  // LLC should be in initial state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  // Send CPHY ABORT
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = FALSE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_ABORT_ML1;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSending High Priority CPHY ABORT \n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  TF_MSG("\nCPHY ABORT sent\n");
  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req_ptr, &buf_len);
 
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  TF_ASSERT(cphy_abort_req_ptr != NULL);
  TF_ASSERT(cphy_abort_req_ptr->msgr_hdr.id == LTE_CPHY_ABORT_REQ);
  
  // build CFG_REQI for COMMON CFG and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = FALSE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_ACQ;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSent CFG REQI for ACQ\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  // build CFG_REQI for COMMON CFG and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = FALSE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_SYSTEM_SCAN;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSent CFG REQI for System Scan\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  // build CFG_REQI for COMMON CFG and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = FALSE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_BAND_SCAN;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSent CFG REQI for System Scan\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  


  // Send CPHY ABORT
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = FALSE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_ABORT_ML1;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSending High Priority CPHY ABORT \n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req_ptr, &buf_len);
 
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  TF_ASSERT(cphy_abort_req_ptr != NULL);
  TF_ASSERT(cphy_abort_req_ptr->msgr_hdr.id == LTE_CPHY_ABORT_REQ);

  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;
  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));
  lte_rrc_utf_wait_for_done();
  }
  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_basic_sib1_update_data_phy_abort.bcch_dl_sch_sib1_ptr, 
                                           lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
   state = lte_rrc_llc_get_state();
  TF_ASSERT(state == LTE_RRC_LLC_SM__INITIAL);
   // Make sure RRC does not throw any more messages
  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_utf_reset(); 

}

void LteRrcLlcB2BAbortHandlingWithCspReqInPendq::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcDeactivateReqWhileWaitingForAbortCnf);

void LteRrcLlcDeactivateReqWhileWaitingForAbortCnf::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM,lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_get_cell_sib1_for_cphy_abort;
}

void LteRrcLlcDeactivateReqWhileWaitingForAbortCnf::Test()
{
  uint32 buf_len; 
  lte_rrc_cfg_reqi_s cfg_reqi;
  lte_rrc_cfg_cnfi_s *cfg_cnfi_ptr = NULL;
  lte_rrc_pended_cfg_reqi_s *pended_cfg_reqi_ptr = NULL;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_cphy_abort_req_s *cphy_abort_req_ptr = NULL;
  lte_cphy_abort_cnf_s cphy_abort_cnf;
  TF_MSG("\nLLC: CPHY ABORT (DSDS) During SIB UPDATE begins \n");
  // LLC should be in initial state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  // build CFG_REQI for COMMON CFG and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_SRV_COMMON;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = TRUE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSent CFG REQI for SIB1 Update\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  // LLC should be in WAIT_FOR_CPHY_CFG_CNF state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);

  TF_MSG("\nLLC waiting for CPHY_CFG_CNF");
  TF_MSG("\nSending CFG REQI (CSP) that will be pended \n");
  // Send Another CFG REQI which will be pended
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_SRV_COMMON;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  // Send CPHY ABORT
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_ABORT_ML1;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSending High Priority CPHY ABORT \n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  // Catch all CNFI's
  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status != E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_PHY_ABORT);
  TF_MSG("\nReceived CNFI for CSP with Failure type Abort\n");

  TF_MSG("\nCPHY ABORT sent\n");
  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req_ptr, &buf_len);
 
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  TF_ASSERT(cphy_abort_req_ptr != NULL);
  TF_ASSERT(cphy_abort_req_ptr->msgr_hdr.id == LTE_CPHY_ABORT_REQ);

  TF_MSG("\nSending CFG REQI (CSP) that will be pended \n");
  // Send Another CFG REQI which will be pended
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_ACQ;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  // FS_DEACTIVATE
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CONTROLLER;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_DEACTIVATE_FS;
  TF_MSG("\nSending Deactivate Req\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;
  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status != E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_CPHY_STOP);
  TF_MSG("\nReceived CNFI for CSP with Failure type STOP\n");

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status != E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_CPHY_STOP);
  TF_MSG("\nReceived CNFI for CSP with Failure type STOP\n");

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status == E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_NONE);
  TF_MSG("\nReceived CNFI for CSP with Failure type None\n");

  /* Verify pended cfg reqi*/
  lte_rrc_utf_get_next_int_msg((byte **) &pended_cfg_reqi_ptr,&buf_len);
  TF_ASSERT(pended_cfg_reqi_ptr->msg_hdr.id == LTE_RRC_PENDED_CFG_REQI);

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CONTROLLER);
  TF_ASSERT(cfg_cnfi_ptr->status == E_SUCCESS);

  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_basic_sib1_update_data_phy_abort.bcch_dl_sch_sib1_ptr, 
                                           lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  // Make sure RRC does not throw any more messages
  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_utf_reset(); 

}

void LteRrcLlcDeactivateReqWhileWaitingForAbortCnf::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcFailingIncomingReqWhileFsDeactivateinPendq);

void LteRrcLlcFailingIncomingReqWhileFsDeactivateinPendq::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM,lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_get_cell_sib1_for_cphy_abort;
}

void LteRrcLlcFailingIncomingReqWhileFsDeactivateinPendq::Test()
{
  uint32 buf_len; 
  lte_rrc_cfg_reqi_s cfg_reqi;
  lte_rrc_cfg_cnfi_s *cfg_cnfi_ptr = NULL;
  lte_rrc_pended_cfg_reqi_s *pended_cfg_reqi_ptr = NULL;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_cphy_abort_req_s *cphy_abort_req_ptr = NULL;
  lte_cphy_abort_cnf_s cphy_abort_cnf;
  TF_MSG("\nLLC: CPHY ABORT (DSDS) During SIB UPDATE begins \n");
  // LLC should be in initial state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  // build CFG_REQI for COMMON CFG and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_SRV_COMMON;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = TRUE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSent CFG REQI for SIB1 Update\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  // LLC should be in WAIT_FOR_CPHY_CFG_CNF state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);

  TF_MSG("\nLLC waiting for CPHY_CFG_CNF");
  TF_MSG("\nSending CFG REQI (CSP) that will be pended \n");
  // Send Another CFG REQI which will be pended
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_SRV_COMMON;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  // Send CPHY ABORT
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_ABORT_ML1;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSending High Priority CPHY ABORT \n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  // Catch all CNFI's
  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status != E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_PHY_ABORT);
  TF_MSG("\nReceived CNFI for CSP with Failure type Abort\n");

  TF_MSG("\nCPHY ABORT sent\n");
  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req_ptr, &buf_len);
 
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  TF_ASSERT(cphy_abort_req_ptr != NULL);
  TF_ASSERT(cphy_abort_req_ptr->msgr_hdr.id == LTE_CPHY_ABORT_REQ);

  // FS_DEACTIVATE
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CONTROLLER;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_DEACTIVATE_FS;
  TF_MSG("\nSending Deactivate Req\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  // Send Another CFG REQI which will be pended
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_ACQ;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status != E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_CPHY_STOP);
  TF_MSG("\nReceived CNFI for CSP with Failure type STOP\n");
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;
  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status != E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_CPHY_STOP);
  TF_MSG("\nReceived CNFI for CSP with Failure type STOP\n");

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status == E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_NONE);
  TF_MSG("\nReceived CNFI for CSP with Failure type None\n");

  /* Verify pended cfg reqi*/
  lte_rrc_utf_get_next_int_msg((byte **) &pended_cfg_reqi_ptr,&buf_len);
  TF_ASSERT(pended_cfg_reqi_ptr->msg_hdr.id == LTE_RRC_PENDED_CFG_REQI);

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CONTROLLER);
  TF_ASSERT(cfg_cnfi_ptr->status == E_SUCCESS);

  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_basic_sib1_update_data_phy_abort.bcch_dl_sch_sib1_ptr, 
                                           lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  // Make sure RRC does not throw any more messages
  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_utf_reset(); 

}

void LteRrcLlcFailingIncomingReqWhileFsDeactivateinPendq::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcRLFDuringReconfig);

void LteRrcLlcRLFDuringReconfig::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM,lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_get_cell_sib1_for_cphy_abort;
}

void LteRrcLlcRLFDuringReconfig::Test()
{
  uint32 buf_len; 
  lte_rrc_cfg_reqi_s cfg_reqi;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  int decode_status = 0;
  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;
  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;
  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf_msg;
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf_msg;
  lte_rrc_cfg_cnfi_s *cfg_cnfi_ptr = NULL;
  lte_cphy_abort_req_s *cphy_abort_req;
  lte_cphy_abort_cnf_s cphy_abort_cnf;

  lte_pdcpul_recfg_prep_req_msg_s *pdcpul_cfg_prep_req_ptr;
  lte_pdcpul_recfg_prep_cnf_msg_s pdcpul_cfg_prep_cnf_msg;

  lte_mac_cfg_req_msg_s *mac_cfg_req;

  lte_mac_access_abort_req_msg_s *mac_access_abort_req_ptr = NULL;
  lte_mac_access_abort_cnf_msg_s mac_access_abort_cnf_msg;

  lte_rlcdl_cfg_req_s *rlcdl_cfg_req;
  lte_rlc_cfg_cnf_s rlcdl_cfg_cnf;

  lte_rlcul_cfg_req_s *rlcul_cfg_req;
  lte_rlc_cfg_cnf_s rlcul_cfg_cnf;

  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req;
  lte_pdcpul_cfg_cnf_msg_s pdcpul_cfg_cnf;
  
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req;
  lte_pdcpdl_cfg_cnf_msg_s pdcpdl_cfg_cnf;
  lte_rrc_pended_cfg_reqi_s *pended_cfg_reqi_ptr = NULL;
  lte_cphy_drx_opt_req_s *drx_opt_req_ptr = NULL;

  TF_MSG("\nLLC: CPHY ABORT (DSDS) During Reconfig begins \n");
  // LLC should be in initial state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  encoded_pdu.length = sizeof(llc_reconfig_drb3_setup_msg);
  encoded_pdu.value = llc_reconfig_drb3_setup_msg;

  /* Construct asn1 pdu to be sent to the Config module after decoding the encoded pdu*/
  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                               DL_DCCH_Message, &decode_status);
  TF_ASSERT(decode_status == 0);
  TF_ASSERT(dl_dcch_msg_ptr != NULL);

  /* Set the DRB id based on the passed parameter drb_id */
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
    rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->drb_Identity = 1;

  /* Set the EPS bearer id based on the passed parameter drb_id */
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
    rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
    drb_ToAddModList.elem->eps_BearerIdentity = 1;

  /* Set the LC id based on the passed parameter lc_id */
  dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->criticalExtensions.u.c1->u.
	rrcConnectionReconfiguration_r8->radioResourceConfigDedicated.
	drb_ToAddModList.elem->logicalChannelIdentity = 3;

  // build CFG_REQI for DED CONFIG and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CONFIG;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_DED;
  cfg_reqi.cfg_info[0].cfg_u.ded_cfg.msg_ptr = dl_dcch_msg_ptr;
  TF_MSG("\nSent CFG REQI for CONFIG with Reconfiguration\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req_ptr, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req_ptr != NULL);
  TF_ASSERT(mac_cfg_req_ptr->hdr.id == LTE_MAC_CFG_REQ);

  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_MAC_CFG_CNF);
  TF_MSG("\nLLC waiting for MAC_CFG_CNF");

  // Send RLF that will be pended
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CRE;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_RLF;
  TF_MSG("\nSending RLF, that will be pended \n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  /* send MAC_CFG_CNF back to LLC with SUCCESS */
  lte_rrc_llc_test_mac_cfg_cnf(&mac_cfg_cnf, TRUE);
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf, sizeof(mac_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);
  
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  lte_rrc_utf_wait_for_done();

  /* Catch the RLCUL Config request sent by LLC to RLCUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcul_cfg_req_ptr,&buf_len);
  /* Make sure the RLCUL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcul_cfg_req_s) == buf_len);
  
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCUL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCUL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCUL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  lte_rrc_utf_wait_for_done();

  /* Catch the PDCPDL Config request sent by LLC to PDCPDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpdl_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPDL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpdl_cfg_req_msg_s) == buf_len);

  /* Construct and send the lte_pdcpdl_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf_msg.hdr,LTE_PDCPDL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPDL_CFG_CNF sent by PDCPDL */
  pdcpdl_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPDL_CFG_CNF to LLC there by mimicing the action of PDCPDL */
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf_msg, sizeof(pdcpdl_cfg_cnf_msg));
  lte_rrc_utf_wait_for_done();

  /* Catch the PDCPUL Config request sent by LLC to PDCPUL */
  lte_rrc_utf_get_next_ext_msg((byte **)&pdcpul_cfg_req_ptr,&buf_len);
  /* Make sure the PDCPUL Config request is valid */
  TF_ASSERT(sizeof(lte_pdcpul_cfg_req_msg_s) == buf_len);

  /* Construct and send the lte_pdcpul_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf_msg.hdr,LTE_PDCPUL_CFG_CNF);
  /* Setting the fields in the LTE_PDCPUL_CFG_CNF sent by PDCPUL */
  pdcpul_cfg_cnf_msg.status = E_SUCCESS;
  /* Send the LTE_PDCPUL_CFG_CNF to LLC there by mimicing the action of PDCPUL */
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf_msg, sizeof(pdcpul_cfg_cnf_msg));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CONFIG);
  TF_ASSERT(cfg_cnfi_ptr->status == E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_NONE);
  TF_MSG("\nReceived CNFI for CONFIG with Failure type None\n");

  lte_rrc_utf_get_next_ext_msg((byte **) &drx_opt_req_ptr,&buf_len);
  TF_ASSERT(drx_opt_req_ptr->msgr_hdr.id == LTE_CPHY_DRX_OPT_REQ);

  // We expect a pdcpul_recfg_prep_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_prep_req_ptr, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_recfg_prep_req_msg_s));
  TF_ASSERT(pdcpul_cfg_prep_req_ptr != NULL);
  TF_ASSERT(pdcpul_cfg_prep_req_ptr->hdr.id == LTE_PDCPUL_RECFG_PREP_REQ);

  // Prepare pdcpul_recfg_prep_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_prep_cnf_msg.hdr, LTE_PDCPUL_RECFG_PREP_CNF);

  pdcpul_cfg_prep_cnf_msg.status = E_SUCCESS;

  // Send pdcpul_prep_req_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_prep_cnf_msg.hdr, sizeof(pdcpul_cfg_prep_cnf_msg));

  // ---------------------------------------------
  // Catch the ACCESS_ABORT_REQ to MAC 
  lte_rrc_utf_get_next_ext_msg((byte **)&mac_access_abort_req_ptr,&buf_len);
  // Make sure the ACCESS_ABORT_REQ is valid 
  TF_ASSERT(sizeof(lte_mac_access_abort_req_msg_s) == buf_len);

  // Send ACCESS_ABORT_CNF to Config 
  // Setting the header for internal RRC messages 
  lte_rrc_init_default_hdr(&mac_access_abort_cnf_msg.hdr,LTE_MAC_ACCESS_ABORT_CNF);
  mac_access_abort_cnf_msg.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&mac_access_abort_cnf_msg, sizeof(mac_access_abort_cnf_msg));


  // ---------------------------------------------

  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;

  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  // ---------------------------------------------

  // We expect a mac_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req != NULL);
  TF_ASSERT(mac_cfg_req->hdr.id == LTE_MAC_CFG_REQ);

  // Prepare mac_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&mac_cfg_cnf.hdr, LTE_MAC_CFG_CNF);

  mac_cfg_cnf.status = E_SUCCESS;

  // Send mac_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf.hdr, sizeof(mac_cfg_cnf));

  // ---------------------------------------------

  // We expect a rlcdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcdl_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcdl_cfg_req_s));
  TF_ASSERT(rlcdl_cfg_req != NULL);
  TF_ASSERT(rlcdl_cfg_req->msg_hdr.id == LTE_RLCDL_CFG_REQ);

  // Prepare rlcdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlcdl_cfg_cnf.msg_hdr, LTE_RLCDL_CFG_CNF);

  rlcdl_cfg_cnf.cfg_status = E_SUCCESS;

  // Send rlcdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlcdl_cfg_cnf.msg_hdr, sizeof(rlcdl_cfg_cnf));

  // ---------------------------------------------

  // We expect a rlcul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&rlcul_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_rlcul_cfg_req_s));
  TF_ASSERT(rlcul_cfg_req != NULL);
  TF_ASSERT(rlcul_cfg_req->msg_hdr.id == LTE_RLCUL_CFG_REQ);

  // Prepare rlcul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&rlcul_cfg_cnf.msg_hdr, LTE_RLCUL_CFG_CNF);

  rlcul_cfg_cnf.cfg_status = E_SUCCESS;

  // Send rlcul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&rlcul_cfg_cnf.msg_hdr, sizeof(rlcul_cfg_cnf));

  // ---------------------------------------------

  // We expect a pdcpdl_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpdl_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpdl_cfg_req_msg_s));
  TF_ASSERT(pdcpdl_cfg_req != NULL);
  TF_ASSERT(pdcpdl_cfg_req->hdr.id == LTE_PDCPDL_CFG_REQ);

  // Prepare pdcpdl_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpdl_cfg_cnf.hdr, LTE_PDCPDL_CFG_CNF);

  pdcpdl_cfg_cnf.status = E_SUCCESS;

  // Send pdcpdl_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpdl_cfg_cnf.hdr, sizeof(pdcpdl_cfg_cnf));

  // ---------------------------------------------

  // We expect a pdcpul_cfg_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&pdcpul_cfg_req, &buf_len);
  
  TF_ASSERT(buf_len == sizeof(lte_pdcpul_cfg_req_msg_s));
  TF_ASSERT(pdcpul_cfg_req != NULL);
  TF_ASSERT(pdcpul_cfg_req->hdr.id == LTE_PDCPUL_CFG_REQ);

  // Prepare pdcpul_cfg_cnf to send to RRC
  lte_rrc_init_default_hdr(&pdcpul_cfg_cnf.hdr, LTE_PDCPUL_CFG_CNF);
  pdcpul_cfg_cnf.status = E_SUCCESS;

  // Send pdcpul_cfg_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&pdcpul_cfg_cnf.hdr, sizeof(pdcpul_cfg_cnf));

  /* Verify pended cfg reqi*/
  lte_rrc_utf_get_next_int_msg((byte **) &pended_cfg_reqi_ptr,&buf_len);
  TF_ASSERT(pended_cfg_reqi_ptr->msg_hdr.id == LTE_RRC_PENDED_CFG_REQI);

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CRE);
  TF_ASSERT(cfg_cnfi_ptr->status == E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_NONE);
  TF_MSG("\nReceived CNFI for CRE with Failure type None\n");
  
  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_basic_sib1_update_data_phy_abort.bcch_dl_sch_sib1_ptr, 
                                           lte_rrc_osys_BCCH_DL_SCH_Message_PDU);

  // Make sure RRC does not throw any more messages
  lte_rrc_utf_check_no_more_msgs();
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  lte_rrc_utf_reset(); 

}

void LteRrcLlcRLFDuringReconfig::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcPhyAbortDuringEmpMtchSetupWaitingForL2Cnf);

void LteRrcLlcPhyAbortDuringEmpMtchSetupWaitingForL2Cnf::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM,lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_get_cell_sib1_for_cphy_abort;
}

void LteRrcLlcPhyAbortDuringEmpMtchSetupWaitingForL2Cnf::Test()
{
  uint32 buf_len; 
  lte_rrc_cfg_reqi_s cfg_reqi;
  lte_rrc_cfg_cnfi_s *cfg_cnfi_ptr = NULL;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr = NULL;
  lte_cphy_common_cfg_cnf_s cphy_common_cfg_cnf;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;
  lte_cphy_abort_req_s *cphy_abort_req_ptr = NULL;
  lte_cphy_abort_cnf_s cphy_abort_cnf;
  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;
  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_rrc_pended_cfg_reqi_s *pended_cfg_reqi_ptr = NULL;
  TF_MSG("\nLLC: CPHY ABORT (DSDS) During EmpMtchSetupWaitingForL2Cnf begins \n");
  // LLC should be in initial state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
#ifdef FEATURE_LTE_EMBMS
  // build CFG_REQI for MTCH SETUP and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_EMP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_EMBMS;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mcch = 0;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mtch = 1;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.release_all_mrb = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_id = LTE_EMBMS_MCCH_RB_ID;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.pmch_id = 1;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.lc_id = 3;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.session_id_present = TRUE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.session_id = 5;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x12;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x34;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x56;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0x01;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0x02;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0x03;

  TF_MSG("\nSent CFG REQI for EmpMtchSetup\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();
#endif

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  // LLC should be in WAIT_FOR_CPHY_CFG_CNF state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  lte_rrc_utf_wait_for_done();
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req_ptr != NULL);
  TF_ASSERT(mac_cfg_req_ptr->hdr.id == LTE_MAC_CFG_REQ);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_MAC_CFG_CNF);
  TF_MSG("\nWaiting for L2 CFG CNF \n");

  TF_MSG("\nSending CFG REQI for CSP PROC that will be pended \n");
  // Send Another CFG REQI which will be pended
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_SRV_COMMON;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  // Send CPHY ABORT
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = FALSE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_ABORT_ML1;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSending High Priority CPHY ABORT \n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  /* send MAC_CFG_CNF back to LLC with SUCCESS */
  lte_rrc_llc_test_mac_cfg_cnf(&mac_cfg_cnf, TRUE);
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf, sizeof(mac_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);
  
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_EMP);
  TF_ASSERT(cfg_cnfi_ptr->status == E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_NONE);
  TF_MSG("\nReceived CNFI for EMP with Failure type None");

  /* Verify pended cfg reqi*/
  lte_rrc_utf_get_next_int_msg((byte **) &pended_cfg_reqi_ptr,&buf_len);
  TF_ASSERT(pended_cfg_reqi_ptr->msg_hdr.id == LTE_RRC_CFG_REQI);  

  TF_MSG("\nCPHY ABORT sent\n");
  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req_ptr, &buf_len);
 
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  TF_ASSERT(cphy_abort_req_ptr != NULL);
  TF_ASSERT(cphy_abort_req_ptr->msgr_hdr.id == LTE_CPHY_ABORT_REQ);
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;
  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  /* Verify pended cfg reqi*/
  lte_rrc_utf_get_next_int_msg((byte **) &pended_cfg_reqi_ptr,&buf_len);
  TF_ASSERT(pended_cfg_reqi_ptr->msg_hdr.id == LTE_RRC_PENDED_CFG_REQI);  

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status != E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_PHY_ABORT);
  TF_MSG("\nReceived CNFI for CSP with Failure type Abort");

  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_basic_sib1_update_data_phy_abort.bcch_dl_sch_sib1_ptr, 
                                           lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  // Make sure RRC does not throw any more messages
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  lte_rrc_utf_check_no_more_msgs();
  lte_rrc_utf_reset(); 

}

void LteRrcLlcPhyAbortDuringEmpMtchSetupWaitingForL2Cnf::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcCspCmdsIgnoredDueToTransIdMismatch);

void LteRrcLlcCspCmdsIgnoredDueToTransIdMismatch::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM,lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_srs_get_serv_cell_sib;
}

void LteRrcLlcCspCmdsIgnoredDueToTransIdMismatch::Test()
{
  uint32 buf_len; 
  lte_rrc_cfg_reqi_s cfg_reqi;
  lte_rrc_cfg_cnfi_s *cfg_cnfi_ptr = NULL;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_mac_cfg_req_msg_s *mac_cfg_req_ptr = NULL;
  lte_cphy_common_cfg_cnf_s cphy_common_cfg_cnf;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;
  lte_cphy_abort_req_s *cphy_abort_req_ptr = NULL;
  lte_cphy_abort_cnf_s cphy_abort_cnf;
  lte_rlcdl_cfg_req_s *rlcdl_cfg_req_ptr = NULL;
  lte_rlc_cfg_cnf_s rlc_cfg_cnf_msg;
  lte_rlcul_cfg_req_s *rlcul_cfg_req_ptr = NULL;
  lte_pdcpul_cfg_req_msg_s *pdcpul_cfg_req_ptr = NULL;
  lte_pdcpdl_cfg_req_msg_s *pdcpdl_cfg_req_ptr = NULL;
  lte_rrc_pended_cfg_reqi_s *pended_cfg_reqi_ptr = NULL;
  TF_MSG("\nLLC: CPHY ABORT (DSDS) During EmpMtchSetupWaitingForL2Cnf begins \n");
  // LLC should be in initial state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
#ifdef FEATURE_LTE_EMBMS
  // build CFG_REQI for MTCH SETUP and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_EMP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_EMBMS;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mcch = 0;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mtch = 1;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.release_all_mrb = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_id = LTE_EMBMS_MCCH_RB_ID;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.pmch_id = 1;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.lc_id = 3;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.session_id_present = TRUE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.session_id = 5;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x12;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x34;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x56;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0x01;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0x02;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0x03;

  TF_MSG("\nSent CFG REQI for EmpMtchSetup\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();
#endif

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  // LLC should be in WAIT_FOR_CPHY_CFG_CNF state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  lte_rrc_utf_wait_for_done();
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req_ptr != NULL);
  TF_ASSERT(mac_cfg_req_ptr->hdr.id == LTE_MAC_CFG_REQ);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_MAC_CFG_CNF);
  TF_MSG("\nWaiting for L2 CFG CNF \n");

  // Send CPHY ABORT
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = FALSE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_ABORT_ML1;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSending High Priority CPHY ABORT \n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  TF_MSG("\nSending CFG REQI for CSP PROC that will be pended \n");
  // Send Another CFG REQI which will be pended
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_ACQ;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  /* send MAC_CFG_CNF back to LLC with SUCCESS */
  lte_rrc_llc_test_mac_cfg_cnf(&mac_cfg_cnf, TRUE);
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf, sizeof(mac_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Catch the RLCDL Config request sent by LLC to RLCDL */
  lte_rrc_utf_get_next_ext_msg((byte **)&rlcdl_cfg_req_ptr,&buf_len);
  /* Make sure the RLCDL Config request is valid */
  TF_ASSERT(sizeof(lte_rlcdl_cfg_req_s) == buf_len);
  
  /* Construct and send the lte_rlc_cfg_cnf_msg message back to LLC */
  lte_rrc_init_default_hdr(&rlc_cfg_cnf_msg.msg_hdr,LTE_RLCDL_CFG_CNF);
  /* Setting the fields in the LTE_RLC_CFG_CNF sent by RLCDL */
  rlc_cfg_cnf_msg.cfg_status = E_SUCCESS;
  /* Send the LTE_RLC_CFG_CNF to LLC there by mimicing the action of RLCDL */
  lte_rrc_utf_send_msg((byte*)&rlc_cfg_cnf_msg, sizeof(rlc_cfg_cnf_msg));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_EMP);
  TF_ASSERT(cfg_cnfi_ptr->status == E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_NONE);
  TF_MSG("\nReceived CNFI for EMP with Failure type None");

  TF_MSG("\nCPHY ABORT sent\n");
  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req_ptr, &buf_len);
 
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  TF_ASSERT(cphy_abort_req_ptr != NULL);
  TF_ASSERT(cphy_abort_req_ptr->msgr_hdr.id == LTE_CPHY_ABORT_REQ);
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;
  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  /* Verify pended cfg reqi*/
  lte_rrc_utf_get_next_int_msg((byte **) &pended_cfg_reqi_ptr,&buf_len);
  TF_ASSERT(pended_cfg_reqi_ptr->msg_hdr.id == LTE_RRC_PENDED_CFG_REQI);  

  /* Verify pended cfg reqi*/
  lte_rrc_utf_get_next_int_msg((byte **) &pended_cfg_reqi_ptr,&buf_len);
  TF_ASSERT(pended_cfg_reqi_ptr->msg_hdr.id == LTE_RRC_PENDED_CFG_REQI);  

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status != E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_NORMAL);
  TF_MSG("\nReceived CNFI for CSP with Failure type Nor");

  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_basic_sib1_update_data_phy_abort.bcch_dl_sch_sib1_ptr, 
                                           lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  // Make sure RRC does not throw any more messages
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  lte_rrc_utf_check_no_more_msgs();

  TF_STUB(lte_rrc_csp_get_last_cfg_req_id) = lte_rrc_llc_test_get_csp_trans_id;

  // LLC should be in initial state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
#ifdef FEATURE_LTE_EMBMS
  // build CFG_REQI for MTCH SETUP and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_EMP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_EMBMS;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mcch = 0;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mtch = 1;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.release_all_mrb = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_id = LTE_EMBMS_MCCH_RB_ID;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.pmch_id = 1;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.lc_id = 3;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.session_id_present = TRUE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.session_id = 5;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x12;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x34;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x56;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0x01;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0x02;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0x03;

  TF_MSG("\nSent CFG REQI for EmpMtchSetup\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();
#endif

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  // LLC should be in WAIT_FOR_CPHY_CFG_CNF state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  lte_rrc_utf_wait_for_done();
  lte_rrc_utf_get_next_ext_msg((byte**)&mac_cfg_req_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_mac_cfg_req_msg_s));
  TF_ASSERT(mac_cfg_req_ptr != NULL);
  TF_ASSERT(mac_cfg_req_ptr->hdr.id == LTE_MAC_CFG_REQ);
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_MAC_CFG_CNF);
  TF_MSG("\nWaiting for L2 CFG CNF \n");

  // Send CPHY ABORT
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = FALSE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_ABORT_ML1;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSending High Priority CPHY ABORT \n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  lte_cphy_acq_req_s  cphy_acq_req_ptr;
  lte_rrc_init_default_hdr(&cphy_acq_req_ptr.msgr_hdr,LTE_CPHY_ACQ_REQ);

  TF_MSG("\nSending CFG REQI for CSP PROC that will be pended \n");
  // Send Another CFG REQI which will be pended
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_ACQ;
  LTE_RRC_MEMCPY (&cfg_reqi.cfg_info[0].cfg_u.acq_req_cfg, sizeof(lte_cphy_acq_req_s), &cphy_acq_req_ptr, sizeof(lte_cphy_acq_req_s));
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  /* send MAC_CFG_CNF back to LLC with SUCCESS */
  lte_rrc_llc_test_mac_cfg_cnf(&mac_cfg_cnf, TRUE);
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf, sizeof(mac_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_EMP);
  TF_ASSERT(cfg_cnfi_ptr->status == E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_NONE);
  TF_MSG("\nReceived CNFI for EMP with Failure type None");

  TF_MSG("\nCPHY ABORT sent\n");
  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req_ptr, &buf_len);
 
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  TF_ASSERT(cphy_abort_req_ptr != NULL);
  TF_ASSERT(cphy_abort_req_ptr->msgr_hdr.id == LTE_CPHY_ABORT_REQ);
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;
  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  /* Verify pended cfg reqi*/
  lte_rrc_utf_get_next_int_msg((byte **) &pended_cfg_reqi_ptr,&buf_len);
  TF_ASSERT(pended_cfg_reqi_ptr->msg_hdr.id == LTE_RRC_PENDED_CFG_REQI);  

  /* Verify pended cfg reqi*/
  lte_rrc_utf_get_next_int_msg((byte **) &pended_cfg_reqi_ptr,&buf_len);
  TF_ASSERT(pended_cfg_reqi_ptr->msg_hdr.id == LTE_RRC_PENDED_CFG_REQI);  

  /* Verify pended cfg reqi*/
  lte_rrc_utf_get_next_int_msg((byte **) &pended_cfg_reqi_ptr,&buf_len);
  TF_ASSERT(pended_cfg_reqi_ptr->msg_hdr.id == LTE_RRC_PENDED_CFG_REQI);  

  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_acq_req_ptr, &buf_len);
  
  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_basic_sib1_update_data_phy_abort.bcch_dl_sch_sib1_ptr, 
                                           lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  // Make sure RRC does not throw any more messages
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  lte_rrc_utf_reset(); 

}

void LteRrcLlcCspCmdsIgnoredDueToTransIdMismatch::Teardown()
{
  lte_rrc_utf_teardown();
}




TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcPhyAbortDuringEmpMcchRelease);

void LteRrcLlcPhyAbortDuringEmpMcchRelease::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM,lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_srs_get_serv_cell_sib;
}

void LteRrcLlcPhyAbortDuringEmpMcchRelease::Test()
{
  uint32 buf_len; 
  lte_rrc_cfg_reqi_s cfg_reqi;
  lte_cphy_common_cfg_cnf_s cphy_common_cfg_cnf;
  lte_rrc_cfg_cnfi_s *cfg_cnfi_ptr = NULL;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_cphy_abort_req_s *cphy_abort_req_ptr = NULL;
  lte_cphy_abort_cnf_s cphy_abort_cnf;
  lte_rrc_pended_cfg_reqi_s *pended_cfg_reqi_ptr = NULL;
  TF_MSG("\nLLC: CPHY ABORT (DSDS) During EmpMcchRelease begins \n");
  // LLC should be in initial state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  lte_rrc_llc_test_sib2_srs_discontinue_and_mbms_cfg();
  
#ifdef FEATURE_LTE_EMBMS
    // build CFG_REQI for MCCH SETUP and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_EMP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_EMBMS;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mcch = 1;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mtch = 0;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.release_all_mrb = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_id = LTE_EMBMS_MCCH_RB_ID;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.cfg_type = LTE_RRC_EMBMS_MCCH_CFG_ALL;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mbsfn_area_id = 100;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.monitor_pattern = LTE_CPHY_MBSFN_AREA_MONITOR_ALL_OCCASIONS;

  TF_MSG("\nSent CFG REQI for EmpMcchSetup\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));

  lte_rrc_utf_wait_for_done();
#endif

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  // LLC should be in WAIT_FOR_CPHY_CFG_CNF state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  lte_rrc_utf_wait_for_done();


#ifdef FEATURE_LTE_EMBMS
  // build CFG_REQI for EMP_MCCH_RELEASE and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_EMP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_EMBMS;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mcch = 1;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mtch = 0;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.release_all_mrb = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_id = LTE_EMBMS_MCCH_RB_ID;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_RELEASE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.cfg_type = LTE_RRC_EMBMS_MCCH_CFG_ALL;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mbsfn_area_id = 100;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.monitor_pattern = LTE_CPHY_MBSFN_AREA_MONITOR_ALL_OCCASIONS;

  TF_MSG("\nSent CFG REQI for EmpMcchRelease\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));

  lte_rrc_utf_wait_for_done();
#endif

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  // LLC should be in WAIT_FOR_CPHY_CFG_CNF state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
  TF_MSG("\nLLC waiting for CPHY_CFG_CNF");

  TF_MSG("\nSending CFG REQI for CSP PROC that will be pended \n");
  // Send Another CFG REQI which will be pended
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_SRV_COMMON;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  // Send CPHY ABORT
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = FALSE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_ABORT_ML1;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSending High Priority CPHY ABORT \n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  TF_MSG("\nLLC: Send CPHY_COMMON_CFG_CNF with sucess \n");
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_EMP);
  TF_ASSERT(cfg_cnfi_ptr->status == E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_NONE);
  TF_MSG("\nReceived CNFI for EMP with Failure type None\n");

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_EMP);
  TF_ASSERT(cfg_cnfi_ptr->status == E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_NONE);
  TF_MSG("\nReceived CNFI for EMP with Failure type None\n");

  TF_MSG("\nCPHY ABORT sent\n");
  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req_ptr, &buf_len);
 
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  TF_ASSERT(cphy_abort_req_ptr != NULL);
  TF_ASSERT(cphy_abort_req_ptr->msgr_hdr.id == LTE_CPHY_ABORT_REQ);
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;
  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  /* Verify pended cfg reqi*/
  lte_rrc_utf_get_next_int_msg((byte **) &pended_cfg_reqi_ptr,&buf_len);
  TF_ASSERT(pended_cfg_reqi_ptr->msg_hdr.id == LTE_RRC_CFG_REQI);

  /* Verify pended cfg reqi*/
  lte_rrc_utf_get_next_int_msg((byte **) &pended_cfg_reqi_ptr,&buf_len);
  TF_ASSERT(pended_cfg_reqi_ptr->msg_hdr.id == LTE_RRC_PENDED_CFG_REQI);

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status != E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_PHY_ABORT);
  TF_MSG("\nReceived CNFI for CSP with Failure type Abort\n");

  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_basic_sib1_update_data_phy_abort.bcch_dl_sch_sib1_ptr, 
                                           lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  // Make sure RRC does not throw any more messages
  lte_rrc_utf_check_no_more_msgs();
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  lte_rrc_utf_reset(); 

}

void LteRrcLlcPhyAbortDuringEmpMcchRelease::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcEmpHandlingWhenCphyAbortInPendq);

void LteRrcLlcEmpHandlingWhenCphyAbortInPendq::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM,lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_srs_get_serv_cell_sib;
}

void LteRrcLlcEmpHandlingWhenCphyAbortInPendq::Test()
{
  uint32 buf_len; 
  lte_rrc_cfg_reqi_s cfg_reqi;
  lte_cphy_common_cfg_cnf_s cphy_common_cfg_cnf;
  lte_rrc_cfg_cnfi_s *cfg_cnfi_ptr = NULL;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_cphy_abort_req_s *cphy_abort_req_ptr = NULL;
  lte_cphy_abort_cnf_s cphy_abort_cnf;
  lte_rrc_pended_cfg_reqi_s *pended_cfg_reqi_ptr = NULL;
  TF_MSG("\nLLC: CPHY ABORT (DSDS) During EmpMcchRelease begins \n");
  // LLC should be in initial state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  lte_rrc_llc_test_sib2_srs_discontinue_and_mbms_cfg();
  
#ifdef FEATURE_LTE_EMBMS  
    // build CFG_REQI for MCCH SETUP and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_EMP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_EMBMS;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mcch = 1;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mtch = 0;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.release_all_mrb = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_id = LTE_EMBMS_MCCH_RB_ID;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.cfg_type = LTE_RRC_EMBMS_MCCH_CFG_ALL;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mbsfn_area_id = 100;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.monitor_pattern = LTE_CPHY_MBSFN_AREA_MONITOR_ALL_OCCASIONS;

  TF_MSG("\nSent CFG REQI for EmpMcchSetup\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));

  lte_rrc_utf_wait_for_done();
#endif

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  // LLC should be in WAIT_FOR_CPHY_CFG_CNF state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  lte_rrc_utf_wait_for_done();


#ifdef FEATURE_LTE_EMBMS
  // build CFG_REQI for EMP_MCCH_RELEASE and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_EMP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_EMBMS;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mcch = 1;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mtch = 0;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.release_all_mrb = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_id = LTE_EMBMS_MCCH_RB_ID;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_RELEASE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.cfg_type = LTE_RRC_EMBMS_MCCH_CFG_ALL;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mbsfn_area_id = 100;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.monitor_pattern = LTE_CPHY_MBSFN_AREA_MONITOR_ALL_OCCASIONS;

  TF_MSG("\nSent CFG REQI for EmpMcchRelease\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));

  lte_rrc_utf_wait_for_done();
#endif

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  // LLC should be in WAIT_FOR_CPHY_CFG_CNF state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
  TF_MSG("\nLLC waiting for CPHY_CFG_CNF");

  TF_MSG("\nSending CFG REQI for CSP PROC that will be pended \n");
  // Send Another CFG REQI which will be pended
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_SRV_COMMON;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  // Send CPHY ABORT
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = FALSE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_ABORT_ML1;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSending High Priority CPHY ABORT \n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

#ifdef FEATURE_LTE_EMBMS
  // build CFG_REQI for MTCH SETUP and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_EMP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_EMBMS;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mcch = 0;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mtch = 1;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.release_all_mrb = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_id = LTE_EMBMS_MCCH_RB_ID;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.pmch_id = 1;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.lc_id = 3;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.session_id_present = TRUE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.session_id = 5;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x12;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x34;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x56;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0x01;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0x02;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0x03;

  TF_MSG("\nSent CFG REQI for EmpMtchSetup that will be pended when CPHY ABORT is in the queue\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();
#endif

  TF_MSG("\nLLC: Send CPHY_COMMON_CFG_CNF with sucess \n");
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_EMP);
  TF_ASSERT(cfg_cnfi_ptr->status == E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_NONE);
  TF_MSG("\nReceived CNFI for EMP with Failure type None\n");

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_EMP);
  TF_ASSERT(cfg_cnfi_ptr->status == E_SUCCESS);

  TF_MSG("\nCPHY ABORT sent\n");
  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req_ptr, &buf_len);
 
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  TF_ASSERT(cphy_abort_req_ptr != NULL);
  TF_ASSERT(cphy_abort_req_ptr->msgr_hdr.id == LTE_CPHY_ABORT_REQ);
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;
  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));

  /* Verify pended cfg reqi*/
  lte_rrc_utf_get_next_int_msg((byte **) &pended_cfg_reqi_ptr,&buf_len);
  TF_ASSERT(pended_cfg_reqi_ptr->msg_hdr.id == LTE_RRC_CFG_REQI);  

  /* Verify pended cfg reqi*/
  lte_rrc_utf_get_next_int_msg((byte **) &pended_cfg_reqi_ptr,&buf_len);
  TF_ASSERT(pended_cfg_reqi_ptr->msg_hdr.id == LTE_RRC_PENDED_CFG_REQI);

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_EMP);
  TF_ASSERT(cfg_cnfi_ptr->status != E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_PHY_ABORT);
  TF_MSG("\nReceived CNFI for CSP with Failure type Abort\n");

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status != E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_PHY_ABORT);
  TF_MSG("\nReceived CNFI for EMP with Failure type Abort\n");

  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_basic_sib1_update_data_phy_abort.bcch_dl_sch_sib1_ptr, 
                                           lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  // Make sure RRC does not throw any more messages
  lte_rrc_utf_check_no_more_msgs();
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  lte_rrc_utf_reset(); 

}

void LteRrcLlcEmpHandlingWhenCphyAbortInPendq::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcCphyAbortWhilePendqIsFull);

void LteRrcLlcCphyAbortWhilePendqIsFull::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM,lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_get_cell_sib1_for_cphy_abort;
}

void LteRrcLlcCphyAbortWhilePendqIsFull::Test()
{
  uint32 buf_len; 
  uint8 i;
  lte_rrc_cfg_reqi_s cfg_reqi;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_rrc_llc_test_dummy_msg_s lte_rrc_llc_test_dummy_msg;
  lte_rrc_cfg_cnfi_s *cfg_cnfi_ptr = NULL;
  lte_cphy_abort_req_s *cphy_abort_req_ptr = NULL;
  lte_cphy_abort_cnf_s cphy_abort_cnf;
  lte_rrc_pended_cfg_reqi_s *pended_cfg_reqi_ptr = NULL;
  
  TF_MSG("\nLLC: CPHY ABORT (DSDS) While Pending Q is full begins \n");
  // LLC should be in initial state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  // build CFG_REQI for COMMON CFG and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_SRV_COMMON;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = TRUE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSent CFG REQI for SIB1 Update\n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  // LLC should be in WAIT_FOR_CPHY_CFG_CNF state
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
  TF_MSG("\nLLC waiting for CPHY_CFG_CNF");

  TF_MSG("\nSending 5 dummy messages that will be pended \n");
  // Now fill up the pending queue
  for ( i= 0; i < 5; i++)
  {
    lte_static_rrc_pendq_put((msgr_hdr_s*) &lte_rrc_llc_test_dummy_msg, 
                      sizeof(lte_rrc_llc_test_dummy_msg),
                      LteRrcLlcPendingQueue__CallbackForDsds);  
  }
 
  TF_MSG("\nSending 1st CFG REQI (CSP) that will be pended \n");
  // Send Another CFG REQI which will be pended
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_SRV_COMMON;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

#ifdef FEATURE_LTE_EMBMS
  // build CFG_REQI for MCCH SETUP and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_EMP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_EMBMS;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mcch = 1;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mtch = 0;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.release_all_mrb = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_id = LTE_EMBMS_MCCH_RB_ID;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.cfg_type = LTE_RRC_EMBMS_MCCH_CFG_ALL;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mbsfn_area_id = 100;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.monitor_pattern = LTE_CPHY_MBSFN_AREA_MONITOR_ALL_OCCASIONS;

  TF_MSG("\nSending 2nd CFG REQI (EMP MCCH setup) that will be pended \n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  // build CFG_REQI for EMP_MTCH_SETUP and send
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_EMP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_EMBMS;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mcch = 0;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mtch = 1;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.release_all_mrb = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_id = LTE_EMBMS_MCCH_RB_ID;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_CFG;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.pmch_id = 1;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.lc_id = 3;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.mbsfn_area_id = 100;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.session_id_present = TRUE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.session_id = 5;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[0] = 0x12;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[1] = 0x34;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.plmn_id.identity[2] = 0x56;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[0] = 0x01;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[1] = 0x02;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mtch_cfg[0].mrb_cfg_u.mtch_cfg.session_info.tmgi.service_id[2] = 0x03;

  TF_MSG("\nSending 3rd CFG REQI for EMP MTCH Setup that will be pended \n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  TF_MSG("\nSending 4th CFG REQI (EMP MCCH RELEASE) that will be pended\n");
  // Send Another CFG REQI which will be pended
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_EMP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[1] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[1];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_EMBMS;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.release_all_mrb = TRUE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mcch = 1;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.num_mtch = 0;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_id = LTE_EMBMS_MCCH_RB_ID;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mrb_action = LTE_RRC_EMBMS_RB_ACTION_RELEASE;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.cfg_type = LTE_RRC_EMBMS_MCCH_CFG_ALL;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.mbsfn_area_id = 100;
  cfg_reqi.cfg_info[0].cfg_u.embms_cfg.mcch_cfg[0].mrb_cfg_u.mcch_cfg.monitor_pattern = LTE_CPHY_MBSFN_AREA_MONITOR_ALL_OCCASIONS;
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();
#endif

  TF_MSG("\nSending 5th CFG REQI for CSP PROC that will be pended \n");
  // Send Another CFG REQI which will be pended
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_SRV_COMMON;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  // Send CPHY ABORT
  lte_rrc_init_default_hdr(&cfg_reqi.msg_hdr,LTE_RRC_CFG_REQI);
  cfg_reqi.num_cfg = 1;
  cfg_reqi.proc = LTE_RRC_PROC_CSP;
  cfg_reqi.cnfi_required = TRUE;
  lte_rrc_llc_test_common_data.trans_id[0] = LTE_RRC_INVALID_TRANS_ID;
  cfg_reqi.trans_id_ptr = &lte_rrc_llc_test_common_data.trans_id[0];
  cfg_reqi.num_cfg = 1;
  cfg_reqi.cfg_info[0].cfg_type = LTE_RRC_CFG_ABORT_ML1;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib2_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.sib1_update = FALSE;
  cfg_reqi.cfg_info[0].cfg_u.common_cfg.default_cfg = FALSE;
  TF_MSG("\nSending High Priority CPHY ABORT \n");
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status != E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_PHY_ABORT);
  TF_MSG("\nReceived CNFI for CSP with Failure type Abort\n");

  TF_MSG("\nCPHY ABORT sent\n");
  // We expect a cphy_abort_req message from RRC
  lte_rrc_utf_get_next_ext_msg((byte**)&cphy_abort_req_ptr, &buf_len);
 
  TF_ASSERT(buf_len == sizeof(lte_cphy_abort_req_s));
  TF_ASSERT(cphy_abort_req_ptr != NULL);
  TF_ASSERT(cphy_abort_req_ptr->msgr_hdr.id == LTE_CPHY_ABORT_REQ);
  
  // Prepare cphy_abort_cnf to send to RRC
  lte_rrc_init_default_hdr(&cphy_abort_cnf.msgr_hdr, LTE_CPHY_ABORT_CNF);
  cphy_abort_cnf.status = E_SUCCESS;
  // Send cphy_abort_cnf to RRC
  lte_rrc_utf_send_msg((byte*)&cphy_abort_cnf.msgr_hdr, sizeof(cphy_abort_cnf));
  
  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status != E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_PHY_ABORT);
  TF_MSG("\nReceived CNFI for CSP with Failure type Abort\n");

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_EMP);
  TF_ASSERT(cfg_cnfi_ptr->status != E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_PHY_ABORT);
  TF_MSG("\nReceived CNFI for EMP with Failure type Abort\n");

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_EMP);
  TF_ASSERT(cfg_cnfi_ptr->status != E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_PHY_ABORT);
  TF_MSG("\nReceived CNFI for EMP with Failure type Abort\n");

   lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status != E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_PHY_ABORT);
  TF_MSG("\nReceived CNFI for CSP with Failure type Abort\n");

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_CSP);
  TF_ASSERT(cfg_cnfi_ptr->status == E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_NONE);
  TF_MSG("\nReceived CNFI for CSP with Failure type None\n");

  /* Verify pended cfg reqi*/
  lte_rrc_utf_get_next_int_msg((byte **) &pended_cfg_reqi_ptr,&buf_len);
  TF_ASSERT(pended_cfg_reqi_ptr->msg_hdr.id == LTE_RRC_PENDED_CFG_REQI);

  lte_rrc_utf_get_next_int_msg((byte **) &cfg_cnfi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_cnfi_s) == buf_len);
  TF_ASSERT(cfg_cnfi_ptr->msg_hdr.id == LTE_RRC_CFG_CNFI);
  TF_ASSERT(cfg_cnfi_ptr->proc == LTE_RRC_PROC_EMP);
  TF_ASSERT(cfg_cnfi_ptr->status == E_SUCCESS);
  TF_ASSERT(cfg_cnfi_ptr->failure_type == LTE_RRC_CFG_FAILURE_NONE);
  TF_MSG("\nReceived CNFI for EMP with Failure type None\n");

  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_basic_sib1_update_data_phy_abort.bcch_dl_sch_sib1_ptr, 
                                           lte_rrc_osys_BCCH_DL_SCH_Message_PDU);
  // Make sure RRC does not throw any more messages
  lte_rrc_utf_check_no_more_msgs();
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  lte_rrc_utf_reset(); 

}

void LteRrcLlcCphyAbortWhilePendqIsFull::Teardown()
{
  lte_rrc_utf_teardown();
}



