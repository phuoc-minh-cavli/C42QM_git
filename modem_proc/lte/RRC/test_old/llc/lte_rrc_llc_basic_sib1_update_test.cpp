/*!
  @file lte_rrc_llc_basic_sib1_update_test.cpp

  @brief
  Unit test code for the LLC procedure that tests the processing of 
  a LTE_RRC_CFG_REQI with basic sib1 update.

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
#include "lte_rrc_llc.h"
#include "lte_rrc_sib.h"
#include "lte_static_rrc_mem_util.h"
}

#include "lte_rrc_llc_test_common.h"
#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include <tf_stub.h>  

extern "C"
{
  TF_PROTO_STUB(void *, lte_rrc_sib_get_cell_sib, lte_rrc_sib_bitmask_t, lte_rrc_sib_db_key_t);
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
  TF_PROTO_STUB(lte_rrc_div_duplex_e, lte_rrc_csp_get_last_camped_mode, void);
}

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module lte_rrc_llc_basic_sib1_update_test.cpp
*/

typedef struct
{
  uint32 trans_id; 
  lte_l1_mib_info_param_s llc_basic_mib;
  lte_rrc_osys_BCCH_DL_SCH_Message *bcch_dl_sch_sib1_ptr;
} lte_rrc_llc_basic_sib1_update_s;

/*! @brief Variables internal to module lte_rrc_llc_basic_sib1_update_test.cpp
*/
static lte_rrc_llc_basic_sib1_update_s lte_rrc_llc_basic_sib1_update_data;

/*===========================================================================

                                FUNCTIONS

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_cell_sib1

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API that returns the serving cell SIB1.

    @return
    Pointer to SIB2

 */
/*=========================================================================*/
void *lte_rrc_llc_test_get_cell_sib1
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
    if (lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr == NULL) 
    {    
      
      encoded_pdu.length = sizeof(llc_basic_sib1_1);
      encoded_pdu.value = llc_basic_sib1_1;
      lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                                   BCCH_DL_SCH_Message, &decode_status);
      TF_ASSERT(decode_status == 0);
      // Mask of p_max present bitmask
      lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.
      systemInformationBlockType1->m.p_MaxPresent = 0; 
    }
    /* return sib 1 ptr */
    return lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.
      systemInformationBlockType1; 
  }
  else if (sib == LTE_RRC_MIB_BITMASK) 
  {
    lte_rrc_llc_basic_sib1_update_data.llc_basic_mib.num_tx_antennas = 1;
    return &lte_rrc_llc_basic_sib1_update_data.llc_basic_mib;
  }
  else
  { 
    TF_ASSERT(0);
  }

} /* lte_rrc_llc_test_get_cell_sib1() */


/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_cell_sib1_ssf9

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API that returns the serving cell SIB1.

    @return
    Pointer to SIB2

 */
/*=========================================================================*/
void *lte_rrc_llc_test_get_cell_sib1_ssf9
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
    if (lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr == NULL) 
    {    
      
      encoded_pdu.length = sizeof(llc_basic_sib1_ssf9);
      encoded_pdu.value = llc_basic_sib1_ssf9;
      lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                                   BCCH_DL_SCH_Message, &decode_status);
      TF_ASSERT(decode_status == 0);
      // Mask of p_max present bitmask
      lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.
      systemInformationBlockType1->m.p_MaxPresent = 0; 
    }
    /* return sib 1 ptr */
    return lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.
      systemInformationBlockType1; 
  }
  else if (sib == LTE_RRC_MIB_BITMASK) 
  {
    lte_rrc_llc_basic_sib1_update_data.llc_basic_mib.num_tx_antennas = 1;
    return &lte_rrc_llc_basic_sib1_update_data.llc_basic_mib;
  }
  else
  { 
    TF_ASSERT(0);
  }

} /* lte_rrc_llc_test_get_cell_sib1_ssf9() */

/*===========================================================================

  FUNCTION:  lte_rrc_llc_test_get_cell_sib1_ssf9_1

===========================================================================*/
/*!
    @brief
    This function simulates the SIB API that returns the serving cell SIB1.

    @return
    Pointer to SIB2

 */
/*=========================================================================*/
void *lte_rrc_llc_test_get_cell_sib1_ssf9_1
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
    if (lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr == NULL) 
    {    
      
      encoded_pdu.length = sizeof(llc_basic_sib1_ssf9_1);
      encoded_pdu.value = llc_basic_sib1_ssf9_1;
      lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr = (lte_rrc_osys_BCCH_DL_SCH_Message *)lte_rrc_osys_asn1_decode_pdu(encoded_pdu.value, encoded_pdu.length, 
                                   BCCH_DL_SCH_Message, &decode_status);
      TF_ASSERT(decode_status == 0);
      // Mask of p_max present bitmask
      lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.
      systemInformationBlockType1->m.p_MaxPresent = 0; 
    }
    /* return sib 1 ptr */
    return lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.
      systemInformationBlockType1; 
  }
  else if (sib == LTE_RRC_MIB_BITMASK) 
  {
    lte_rrc_llc_basic_sib1_update_data.llc_basic_mib.num_tx_antennas = 1;
    return &lte_rrc_llc_basic_sib1_update_data.llc_basic_mib;
  }
  else
  { 
    TF_ASSERT(0);
  }

} /* lte_rrc_llc_test_get_cell_sib1_ssf9_2() */
/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcBasicSib1Update);

void LteRrcLlcBasicSib1Update::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM,lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_get_cell_sib1;
}

void LteRrcLlcBasicSib1Update::Test()
{
  uint32 buf_len, trans_id; 
  lte_rrc_cfg_reqi_s cfg_reqi;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_cphy_common_cfg_cnf_s cphy_common_cfg_cnf;
  lte_mac_cfg_req_msg_s *mac_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;
  lte_rrc_osys_SystemInformationBlockType1 *sib1_ptr = NULL;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: Basic sib1 Update Test Begins \n");
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  
  /* build CFG_REQI for COMMON CFG and send */
  lte_rrc_llc_test_construct_common_cfg_reqi(&cfg_reqi, TRUE, FALSE, FALSE, &trans_id);
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  sib1_ptr = lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.
    systemInformationBlockType1;

  TF_MSG("\nLLC: Rcvd CPHY_COMMON_CFG_REQ \n");
  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  /* LLC should be in WAIT_FOR_CPHY_CFG_CNF state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
  TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.cfg_mask == 
			(LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG));

  /* Cell selection information */
  TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.cell_selection_cfg.
            q_rxlevmin == (2 * sib1_ptr->cellSelectionInfo.q_RxLevMin));

  TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.cell_selection_cfg.
              q_rxlevmin_offset == LTE_RRC_LLC_DEFAULT_Q_RXLEVMIN_OFFSET);

  if (sib1_ptr->m.p_MaxPresent)
  {
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.cell_selection_cfg.
              p_max == sib1_ptr->p_Max);
  }
  else
  {
    TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.cell_selection_cfg.p_max == LTE_DEFAULT_PMAX);
  }

  /* send CPHY_COMMON_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send CPHY_COMMON_CFG_CNF with sucess \n");
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  TF_MSG("\nLLC: Rcvd MAC_CFG_REQ \n");
  lte_rrc_utf_get_next_ext_msg((byte **) &mac_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);

  /* LLC should be in WAIT_FOR_MAC_CFG_CNF state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_MAC_CFG_CNF);

  /* Validate all fields of mac_cfg_req */

  TF_ASSERT(mac_req_ptr->cfg_data.cfg_bitmask == (LTE_MAC_RACH_CFG_BM));

  /* RACH configuration : this would include both MAC and ML1 configuration */
  TF_ASSERT(mac_req_ptr->cfg_data.rach_cfg->p_max == LTE_DEFAULT_PMAX);

  /* send MAC_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send MAC_CFG_CNF with sucess \n");
  lte_rrc_llc_test_mac_cfg_cnf(&mac_cfg_cnf, TRUE);
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf, sizeof(mac_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CSP, cfg_reqi.trans_id_ptr);

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr, 
                             lte_rrc_osys_BCCH_DL_SCH_Message_PDU);

  TF_MSG("\nLLC: Basic sib1 Update Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcBasicSib1Update::Teardown()
{
  lte_rrc_utf_teardown();
}





TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcBasicSib1TddCfgR11Update);

void LteRrcLlcBasicSib1TddCfgR11Update::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM,lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel11;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_tdd;
}

void LteRrcLlcBasicSib1TddCfgR11Update::Test()
{
  uint32 buf_len, trans_id; 
  lte_rrc_cfg_reqi_s cfg_reqi;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_cphy_common_cfg_cnf_s cphy_common_cfg_cnf;
  lte_mac_cfg_req_msg_s *mac_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;
  lte_rrc_osys_SystemInformationBlockType1 *sib1_ptr = NULL;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: Basic sib1 Update Test Begins \n");
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_get_cell_sib1_ssf9;
  /* build CFG_REQI for COMMON CFG and send */
  lte_rrc_llc_test_construct_common_cfg_reqi(&cfg_reqi, TRUE, FALSE, FALSE, &trans_id);
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  sib1_ptr = lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.
    systemInformationBlockType1;

  TF_MSG("\nLLC: Rcvd CPHY_COMMON_CFG_REQ \n");
  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  /* LLC should be in WAIT_FOR_CPHY_CFG_CNF state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
  TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.cfg_mask == 
			(LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG));


  /* send CPHY_COMMON_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send CPHY_COMMON_CFG_CNF with sucess \n");
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  TF_MSG("\nLLC: Rcvd MAC_CFG_REQ \n");
  lte_rrc_utf_get_next_ext_msg((byte **) &mac_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);

  /* LLC should be in WAIT_FOR_MAC_CFG_CNF state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_MAC_CFG_CNF);

  /* Validate all fields of mac_cfg_req */

  TF_ASSERT(mac_req_ptr->cfg_data.cfg_bitmask == (LTE_MAC_RACH_CFG_BM |
                                                  LTE_MAC_UL_CFG_BM));

  /* RACH configuration : this would include both MAC and ML1 configuration */
  TF_ASSERT(mac_req_ptr->cfg_data.rach_cfg->p_max == LTE_DEFAULT_PMAX);

  /* send MAC_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send MAC_CFG_CNF with sucess \n");
  lte_rrc_llc_test_mac_cfg_cnf(&mac_cfg_cnf, TRUE);
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf, sizeof(mac_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CSP, cfg_reqi.trans_id_ptr);

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr, 
                             lte_rrc_osys_BCCH_DL_SCH_Message_PDU);

  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_get_cell_sib1_ssf9_1;
  /* build CFG_REQI for COMMON CFG and send */
  lte_rrc_llc_test_construct_common_cfg_reqi(&cfg_reqi, TRUE, FALSE, FALSE, &trans_id);
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  sib1_ptr = lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.
    systemInformationBlockType1;

 
  TF_MSG("\nLLC: Rcvd MAC_CFG_REQ \n");
  lte_rrc_utf_get_next_ext_msg((byte **) &mac_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);

  /* LLC should be in WAIT_FOR_MAC_CFG_CNF state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_MAC_CFG_CNF);

  /* Validate all fields of mac_cfg_req */

  TF_ASSERT(mac_req_ptr->cfg_data.cfg_bitmask == (LTE_MAC_RACH_CFG_BM |
                                                  LTE_MAC_UL_CFG_BM));

  /* RACH configuration : this would include both MAC and ML1 configuration */
  TF_ASSERT(mac_req_ptr->cfg_data.rach_cfg->p_max == LTE_DEFAULT_PMAX);

  /* send MAC_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send MAC_CFG_CNF with sucess \n");
  lte_rrc_llc_test_mac_cfg_cnf(&mac_cfg_cnf, TRUE);
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf, sizeof(mac_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CSP, cfg_reqi.trans_id_ptr);

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr, 
                             lte_rrc_osys_BCCH_DL_SCH_Message_PDU);

  TF_MSG("\nLLC: Basic sib1 Update Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcBasicSib1TddCfgR11Update::Teardown()
{
  lte_rrc_utf_teardown();
}





TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcBasicSib1TddCfgR11toR8Update);

void LteRrcLlcBasicSib1TddCfgR11toR8Update::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM,lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel11;
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_tdd;
}

void LteRrcLlcBasicSib1TddCfgR11toR8Update::Test()
{
  uint32 buf_len, trans_id; 
  lte_rrc_cfg_reqi_s cfg_reqi;
  lte_cphy_common_cfg_req_s *cphy_common_cfg_req_ptr = NULL;
  lte_cphy_common_cfg_cnf_s cphy_common_cfg_cnf;
  lte_mac_cfg_req_msg_s *mac_req_ptr = NULL;
  lte_mac_cfg_cnf_msg_s mac_cfg_cnf;
  lte_rrc_osys_SystemInformationBlockType1 *sib1_ptr = NULL;
  /* ---------------------------------------------------------------------- */
  TF_MSG("\nLLC: Basic sib1 Update Test Begins \n");
  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);
  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_get_cell_sib1_ssf9;
  /* build CFG_REQI for COMMON CFG and send */
  lte_rrc_llc_test_construct_common_cfg_reqi(&cfg_reqi, TRUE, FALSE, FALSE, &trans_id);
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();

  sib1_ptr = lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr->message.u.c1->u.
    systemInformationBlockType1;

  TF_MSG("\nLLC: Rcvd CPHY_COMMON_CFG_REQ \n");
  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  /* LLC should be in WAIT_FOR_CPHY_CFG_CNF state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
  TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.cfg_mask == 
			(LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG));


  /* send CPHY_COMMON_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send CPHY_COMMON_CFG_CNF with sucess \n");
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  TF_MSG("\nLLC: Rcvd MAC_CFG_REQ \n");
  lte_rrc_utf_get_next_ext_msg((byte **) &mac_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);

  /* LLC should be in WAIT_FOR_MAC_CFG_CNF state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_MAC_CFG_CNF);

  /* Validate all fields of mac_cfg_req */

  TF_ASSERT(mac_req_ptr->cfg_data.cfg_bitmask == (LTE_MAC_RACH_CFG_BM |
                                                  LTE_MAC_UL_CFG_BM));

  /* RACH configuration : this would include both MAC and ML1 configuration */
  TF_ASSERT(mac_req_ptr->cfg_data.rach_cfg->p_max == LTE_DEFAULT_PMAX);

  /* send MAC_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send MAC_CFG_CNF with sucess \n");
  lte_rrc_llc_test_mac_cfg_cnf(&mac_cfg_cnf, TRUE);
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf, sizeof(mac_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CSP, cfg_reqi.trans_id_ptr);

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr, 
                             lte_rrc_osys_BCCH_DL_SCH_Message_PDU);

  TF_STUB(lte_rrc_sib_get_cell_sib) = lte_rrc_llc_test_tdd_get_cell_sib;
  /* build CFG_REQI for COMMON CFG and send */
  lte_rrc_llc_test_construct_common_cfg_reqi(&cfg_reqi, TRUE, FALSE, FALSE, &trans_id);
  lte_rrc_utf_send_msg((byte*)&cfg_reqi, sizeof(cfg_reqi));
  lte_rrc_utf_wait_for_done();



  TF_MSG("\nLLC: Rcvd CPHY_COMMON_CFG_REQ \n");
  lte_rrc_utf_get_next_ext_msg((byte **) &cphy_common_cfg_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_common_cfg_req_s) == buf_len);
  /* LLC should be in WAIT_FOR_CPHY_CFG_CNF state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_CPHY_CFG_CNF);
  TF_ASSERT(cphy_common_cfg_req_ptr->common_cfg_param.cfg_mask == 
                        (LTE_CPHY_COMMON_CFG_MASK_BIT_CELL_SELECT_CFG));


  /* send CPHY_COMMON_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send CPHY_COMMON_CFG_CNF with sucess \n");
  lte_rrc_llc_test_cphy_common_cfg_cnf(&cphy_common_cfg_cnf, TRUE, FALSE);
  lte_rrc_utf_send_msg((byte*)&cphy_common_cfg_cnf, sizeof(cphy_common_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  TF_MSG("\nLLC: Rcvd MAC_CFG_REQ \n");
  lte_rrc_utf_get_next_ext_msg((byte **) &mac_req_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_mac_cfg_req_msg_s) == buf_len);

  /* LLC should be in WAIT_FOR_MAC_CFG_CNF state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__WAIT_FOR_MAC_CFG_CNF);

  /* Validate all fields of mac_cfg_req */

  TF_ASSERT(mac_req_ptr->cfg_data.cfg_bitmask == (LTE_MAC_RACH_CFG_BM |
                                                  LTE_MAC_UL_CFG_BM));

  

  /* send MAC_CFG_CNF back to LLC with SUCCESS */
  TF_MSG("\nLLC: Send MAC_CFG_CNF with sucess \n");
  lte_rrc_llc_test_mac_cfg_cnf(&mac_cfg_cnf, TRUE);
  lte_rrc_utf_send_msg((byte*)&mac_cfg_cnf, sizeof(mac_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  /* Verify CFG_CNFI from LLC */
  lte_rrc_llc_test_verify_cfg_cnfi(E_SUCCESS, LTE_RRC_PROC_CSP, cfg_reqi.trans_id_ptr);

  /* LLC should be in initial state  */
  TF_ASSERT(lte_rrc_llc_get_state() == LTE_RRC_LLC_SM__INITIAL);

  lte_rrc_osys_asn1_free_pdu(lte_rrc_llc_basic_sib1_update_data.bcch_dl_sch_sib1_ptr, 
                             lte_rrc_osys_BCCH_DL_SCH_Message_PDU);

  TF_MSG("\nLLC: Basic sib1 Update Test Ends \n");
  lte_rrc_utf_reset(); 

  /* ---------------------------------------------------------------------- */
}

void LteRrcLlcBasicSib1TddCfgR11toR8Update::Teardown()
{
  lte_rrc_utf_teardown();
}







