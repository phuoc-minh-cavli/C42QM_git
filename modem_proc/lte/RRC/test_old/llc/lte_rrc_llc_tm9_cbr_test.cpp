/*!
  @file lte_rrc_llc_tm9_cbr_test.cpp

  @brief
  Unit test code for the LLC procedure that tests TM9 CBR

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
03/04/14   md     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "__lte_rrc_llc.h"
#include "__lte_rrc_llc_int.h"
#include "lte_rrc_llci.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_rrc_sib.h"
#include "lte_static_rrc_mem_util.h"
}

#include "lte_rrc_llc_test_common.h"
#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include <tf_stub.h>  

/*===========================================================================

                                INTERNAL DEFINITIONS

===========================================================================*/
/*===========================================================================
	
							 INTERNAL VARIABLES
	
===========================================================================*/
	
extern "C"
{
  extern lte_rrc_llc_s lte_rrc_llc_data;
  }


extern "C"
{
  TF_PROTO_STUB(lte_rrc_div_duplex_e, lte_rrc_csp_get_last_camped_mode, void);
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
}

/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcTM9CBRTest);

void LteRrcLlcTM9CBRTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_fdd;
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel10;
}

void LteRrcLlcTM9CBRTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
   lte_cphy_drx_opt_req_s *drx_opt_req_ptr = NULL;
   uint32 buf_len = 0;
  lte_rrc_llc_test_basic_sib2_update(); 

  // TM9 with 6 bits
  encoded_pdu.length = sizeof(llc_conn_setup_with_tm9_ies);
  encoded_pdu.value = llc_conn_setup_with_tm9_ies;
  lte_rrc_llc_test_rrc_connection_setup_for_tm9(encoded_pdu);

  // With 64 bits
  encoded_pdu.length = sizeof(llc_reconfig_tm9_cbr_64_msg);
  encoded_pdu.value = llc_reconfig_tm9_cbr_64_msg;

  lte_rrc_llc_test_tm9_wo_csi_rs_cfg(encoded_pdu);
  
  // with 109 bits
  encoded_pdu.length = sizeof(llc_reconfig_tm9_cbr_109_msg);
  encoded_pdu.value = llc_reconfig_tm9_cbr_109_msg;
  
  lte_rrc_utf_get_next_ext_msg((byte **) &drx_opt_req_ptr,&buf_len);
  TF_ASSERT(drx_opt_req_ptr->msgr_hdr.id == LTE_CPHY_DRX_OPT_REQ);
  
  lte_rrc_llc_test_tm9_wo_csi_rs_cfg(encoded_pdu);

  // With 6 bits
  encoded_pdu.length = sizeof(llc_reconfig_tm9_cbr_6_msg);
  encoded_pdu.value = llc_reconfig_tm9_cbr_6_msg;

  lte_rrc_llc_test_tm9_wo_csi_rs_cfg(encoded_pdu);
  
  lte_rrc_utf_reset(); 
}

void LteRrcLlcTM9CBRTest::Teardown()
{
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcTM9WOCsiTest);

void LteRrcLlcTM9WOCsiTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_fdd;
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel10;
}

void LteRrcLlcTM9WOCsiTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_llc_test_basic_sib2_update(); 
  
  encoded_pdu.length = sizeof(llc_conn_setup_with_csi_ies);
  encoded_pdu.value = llc_conn_setup_with_csi_ies;
  lte_rrc_llc_test_rrc_connection_setup_for_tm9(encoded_pdu);

  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();

  encoded_pdu.length = sizeof(llc_reconfig_tm9_wo_csi_msg);
  encoded_pdu.value = llc_reconfig_tm9_wo_csi_msg;

  lte_rrc_llc_test_tm9_wo_csi_rs_cfg(encoded_pdu);

  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->cqi_cfg.cqi_rel9_param.pmi_ri_report_configured == FALSE);
  lte_rrc_utf_reset();
}

void LteRrcLlcTM9WOCsiTest::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcConnSetupWithPmiReconfigWithTM9WOCsiTest);

void LteRrcLlcConnSetupWithPmiReconfigWithTM9WOCsiTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
  TF_STUB(lte_rrc_csp_get_last_camped_mode) = lte_rrc_llc_test_get_last_camped_mode_fdd;
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel10;
}

void LteRrcLlcConnSetupWithPmiReconfigWithTM9WOCsiTest::Test()
{
  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_llc_test_basic_sib2_update(); 
  
  encoded_pdu.length = sizeof(llc_conn_setup_with_csi_pmi_ies);
  encoded_pdu.value = llc_conn_setup_with_csi_pmi_ies;
  lte_rrc_llc_test_rrc_connection_setup_for_tm9(encoded_pdu);

  encoded_pdu.length = sizeof(llc_reconfig_tm9_wo_csi_pmi_msg);
  encoded_pdu.value = llc_reconfig_tm9_wo_csi_pmi_msg;

  lte_rrc_llc_test_tm9_wo_csi_rs_cfg(encoded_pdu);

  TF_ASSERT(lte_rrc_llc_data.dd_ptr->cc_ptr->cqi_cfg.cqi_rel9_param.pmi_ri_report_configured == FALSE);
  lte_rrc_utf_reset();
}

void LteRrcLlcConnSetupWithPmiReconfigWithTM9WOCsiTest::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcLlcScellTm7Invalidation);

void LteRrcLlcScellTm7Invalidation::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_LLC_SM, lte_rrc_llc_test_umid_list, 
                    sizeof(lte_rrc_llc_test_umid_list)/sizeof(lte_rrc_llc_test_umid_list[0]));
  lte_rrc_llc_test_stub_init();
}

void LteRrcLlcScellTm7Invalidation::Test()
{

  lte_rrc_pdu_buf_s encoded_pdu;
  lte_rrc_llc_test_basic_sib2_update(); 
  
  encoded_pdu.length = sizeof(llc_conn_setup_with_csi_ies);
  encoded_pdu.value = llc_conn_setup_with_csi_ies;
  lte_rrc_llc_test_rrc_connection_setup_for_tm9(encoded_pdu);

  lte_rrc_llc_test_reconfig_srb2_setup_drb3_um_bi();

  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_llc_test_get_3gpp_release_version_for_rel10;

  encoded_pdu.length = sizeof(llc_scell_tm7_test);
  encoded_pdu.value = llc_scell_tm7_test;

  lte_rrc_llc_scell_cqi_bw_test(encoded_pdu);

  lte_rrc_utf_reset();

}

void LteRrcLlcScellTm7Invalidation::Teardown()
{
  lte_rrc_utf_teardown();
}



