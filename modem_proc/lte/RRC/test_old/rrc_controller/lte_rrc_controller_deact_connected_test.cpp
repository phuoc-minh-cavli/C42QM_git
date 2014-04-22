/*!
  @file lte_rrc_controller_deact_connected_test.cpp

  @brief
  Unit test code for basic RRC controller state transitions. Take it from
  Inactive -> Connected -> Inactive

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

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/09   fhu     replaced lte_cphy_sap.h by lte_cphy_msg.h
09/12/08   sureshs Streamlined header file includes
09/04/08   sureshs Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include <lte_rrc_int_msg.h>
#include <lte_rrc_controller.h>
#include <lte_rrc_utils.h>
#include <lte_cphy_msg.h>
#include <lte_mac_msg.h>
#include <lte_mac_rrc.h>
#include <lte_pdcp_msg.h>
#include <lte_rlc_msg.h>
#include <lte_rlc_rrc.h>
}

#include <TestFramework.h>
#include <lte_rrc_utf.h>
#include <tf_stub.h>

extern "C"
{
  //TF_PROTO_STUB(boolean, lte_rrc_misc_get_csfb_call_status, void);
}

boolean lte_rrc_controller_test_stub_misc_get_csfb_call_status(void)
{
  return TRUE;
}


void                lte_rrc_controller_csfb_init
(
  void
)
{
 // TF_STUB(lte_rrc_misc_get_csfb_call_status) = lte_rrc_controller_test_stub_misc_get_csfb_call_status;
}


//external messages generated by code under test
static uint32 ext_umid_list[] = 
{ 
  LTE_CPHY_START_REQ,
  LTE_CPHY_STOP_REQ,
  LTE_MAC_START_REQ,
  LTE_MAC_STOP_REQ,
  LTE_RLCDL_START_REQ,
  LTE_RLCDL_STOP_REQ,
  LTE_RLCUL_START_REQ,
  LTE_RLCUL_STOP_REQ,
  LTE_PDCPDL_START_REQ,
  LTE_PDCPDL_STOP_REQ,
  LTE_PDCPUL_START_REQ,
  LTE_PDCPUL_STOP_REQ,
  LTE_RRC_COMMON_MSIM_CRAT_FEATURE_REQ,
};

//message structures
static lte_rrc_mode_change_reqi_s mc_req;
static lte_rrc_cfg_cnfi_s cfg_cnfi;
static lte_cphy_start_cnf_s start_cnf;
static lte_cphy_stop_cnf_s stop_cnf;
static lte_rrc_camped_indi_s camped_ind;
static lte_rrc_conn_establishment_started_indi_s conn_est_started_ind;
static lte_rrc_connected_indi_s connected_ind;
static lte_mac_start_cnf_msg_s mac_start_cnf;
static lte_mac_stop_cnf_msg_s mac_stop_cnf;
static lte_rlc_start_cnf_s rlc_start_cnf;
static lte_rlc_stop_cnf_s rlc_stop_cnf;
static lte_pdcp_start_cnf_msg_s pdcp_start_cnf;
static lte_pdcp_stop_cnf_msg_s pdcp_stop_cnf;

/*===========================================================================

                                UNIT TEST

===========================================================================*/

void LteRrcControllerDeactConnected_Test(boolean ue_mode)
{
  byte *buf_ptr;
  uint32 buf_len;
  lte_cphy_start_req_s *start_req_ptr;
  lte_rrc_mode_change_cnfi_s *mc_cnf_ptr;
  lte_rrc_cfg_reqi_s *cfg_req_ptr;
  lte_rrc_stopped_indi_s *stopped_ind_ptr;
  lte_mac_start_req_msg_s *mac_start_req_ptr;
  lte_mac_stop_req_msg_s *mac_stop_req_ptr;
  lte_rlc_start_req_s *rlc_start_req_ptr;
  lte_rlc_stop_req_s *rlc_stop_req_ptr;
  lte_pdcp_start_req_msg_s *pdcp_start_req_ptr;
  lte_pdcp_stop_req_msg_s *pdcp_stop_req_ptr;
  lte_common_msim_crat_feature_req_s *mode_ptr;

  TF_MSG("Start of test");

  TF_MSG("Checking for INACTIVE state");
  TF_ASSERT(lte_rrc_controller_get_state() == LTE_RRC_CONTROLLER_SM__INACTIVE);

  if (ue_mode)
  {
    lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
    TF_ASSERT(sizeof(lte_common_msim_crat_feature_req_s) == buf_len);
    mode_ptr = (lte_common_msim_crat_feature_req_s *)(void*) buf_ptr;
    TF_ASSERT(LTE_RRC_COMMON_MSIM_CRAT_FEATURE_REQ == mode_ptr->msg_hdr.id); 
  }

  TF_MSG("Sending Mode Change Req to Activate with dest of Idle Not Camped");
  lte_rrc_init_default_hdr(&mc_req.msg_hdr,LTE_RRC_MODE_CHANGE_REQI);
  mc_req.dest_scenario = LTE_RRC_ACTIVATED_DUE_TO_NAS_REQUEST;
  mc_req.mode = LTE_RRC_MODE_CHANGE_ACTIVATE;
  lte_rrc_utf_send_msg((byte*)&mc_req, sizeof(mc_req));

  TF_MSG("Checking for LTE_CPHY_START_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_start_req_s) == buf_len);
  start_req_ptr = (lte_cphy_start_req_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_CPHY_START_REQ == start_req_ptr->msgr_hdr.id); 

  TF_MSG("Checking for LTE_MAC_START_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_mac_start_req_msg_s) == buf_len);
  mac_start_req_ptr = (lte_mac_start_req_msg_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_MAC_START_REQ == mac_start_req_ptr->hdr.id); 

  TF_MSG("Checking for LTE_RLCDL_START_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rlc_start_req_s) == buf_len);
  rlc_start_req_ptr = (lte_rlc_start_req_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RLCDL_START_REQ == rlc_start_req_ptr->msg_hdr.id); 

  TF_MSG("Checking for LTE_RLCUL_START_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rlc_start_req_s) == buf_len);
  rlc_start_req_ptr = (lte_rlc_start_req_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RLCUL_START_REQ == rlc_start_req_ptr->msg_hdr.id); 

  TF_MSG("Checking for LTE_PDCPDL_START_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_pdcp_start_req_msg_s) == buf_len);
  pdcp_start_req_ptr = (lte_pdcp_start_req_msg_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_PDCPDL_START_REQ == pdcp_start_req_ptr->msg_hdr.id); 

  TF_MSG("Checking for LTE_PDCPUL_START_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_pdcp_start_req_msg_s) == buf_len);
  pdcp_start_req_ptr = (lte_pdcp_start_req_msg_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_PDCPUL_START_REQ == pdcp_start_req_ptr->msg_hdr.id); 

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for INACTIVE state");
  TF_ASSERT(lte_rrc_controller_get_state() == LTE_RRC_CONTROLLER_SM__INACTIVE);

  TF_MSG("Sending Start Cnf");
  msgr_init_hdr(&start_cnf.msgr_hdr,MSGR_LTE_CPHY,LTE_CPHY_START_CNF);
  start_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&start_cnf, sizeof(start_cnf));

  TF_MSG("Sending MAC Start Cnf");
  msgr_init_hdr(&mac_start_cnf.hdr,MSGR_LTE_MAC,LTE_MAC_START_CNF);
  mac_start_cnf.status = E_SUCCESS;  
  lte_rrc_utf_send_msg((byte*)&mac_start_cnf, sizeof(mac_start_cnf));

  TF_MSG("Sending RLCDL Start Cnf");
  msgr_init_hdr(&rlc_start_cnf.msg_hdr,MSGR_LTE_RLCDL,LTE_RLCDL_START_CNF);
  rlc_start_cnf.cnf_status = E_SUCCESS;  
  lte_rrc_utf_send_msg((byte*)&rlc_start_cnf, sizeof(rlc_start_cnf));

  TF_MSG("Sending RLCUL Start Cnf");
  msgr_init_hdr(&rlc_start_cnf.msg_hdr,MSGR_LTE_RLCUL,LTE_RLCUL_START_CNF);
  rlc_start_cnf.cnf_status = E_SUCCESS;  
  lte_rrc_utf_send_msg((byte*)&rlc_start_cnf, sizeof(rlc_start_cnf));

  TF_MSG("Sending PDCPDL Start Cnf");
  msgr_init_hdr(&pdcp_start_cnf.msg_hdr,MSGR_LTE_PDCPDL,LTE_PDCPDL_START_CNF);
  pdcp_start_cnf.cnf_status = E_SUCCESS;  
  lte_rrc_utf_send_msg((byte*)&pdcp_start_cnf, sizeof(pdcp_start_cnf));

  TF_MSG("Sending PDCPUL Start Cnf");
  msgr_init_hdr(&pdcp_start_cnf.msg_hdr,MSGR_LTE_PDCPUL,LTE_PDCPUL_START_CNF);
  pdcp_start_cnf.cnf_status = E_SUCCESS;  
  lte_rrc_utf_send_msg((byte*)&pdcp_start_cnf, sizeof(pdcp_start_cnf));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for LTE_RRC_MODE_CHANGE_CNFI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_mode_change_cnfi_s) == buf_len);
  mc_cnf_ptr = (lte_rrc_mode_change_cnfi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_MODE_CHANGE_CNFI == mc_cnf_ptr->msg_hdr.id);
  TF_ASSERT(TRUE == mc_cnf_ptr->status);

  TF_MSG("Checking for IDLE_NOT_CAMPED state");
  TF_ASSERT(lte_rrc_controller_get_state() == 
            LTE_RRC_CONTROLLER_SM__IDLE_NOT_CAMPED);

  TF_MSG("Sending Idle Camped indication");
  lte_rrc_init_default_hdr(&camped_ind.msg_hdr,LTE_RRC_CAMPED_INDI);
  memset(&camped_ind.camped_cell,0,sizeof(camped_ind.camped_cell));
  camped_ind.reason = LTE_RRC_CAMPED_CELL_SELECTION;
  lte_rrc_utf_send_msg((byte*)&camped_ind, sizeof(camped_ind));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for IDLE_CAMPED state");
  TF_ASSERT(lte_rrc_controller_get_state() == 
            LTE_RRC_CONTROLLER_SM__IDLE_CAMPED);

  TF_MSG("Sending Conn Est Started indication");
  lte_rrc_init_default_hdr(&conn_est_started_ind.msg_hdr,
                           LTE_RRC_CONN_ESTABLISHMENT_STARTED_INDI);
  lte_rrc_utf_send_msg((byte*)&conn_est_started_ind, 
                       sizeof(conn_est_started_ind));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for CONNECTING state");
  TF_ASSERT(lte_rrc_controller_get_state() == 
            LTE_RRC_CONTROLLER_SM__CONNECTING);

  TF_MSG("Sending Connected indication");
  lte_rrc_init_default_hdr(&connected_ind.msg_hdr,LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_ind, 
                       sizeof(connected_ind));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for CONNECTED state");
  TF_ASSERT(lte_rrc_controller_get_state() == 
            LTE_RRC_CONTROLLER_SM__CONNECTED);

  TF_MSG("Sending Mode Change Req to Dectivate");
  lte_rrc_init_default_hdr(&mc_req.msg_hdr,LTE_RRC_MODE_CHANGE_REQI);
  mc_req.dest_scenario = LTE_RRC_ACTIVATED_DUE_TO_NAS_REQUEST; /* don't care */
  mc_req.mode = LTE_RRC_MODE_CHANGE_DEACTIVATE;
  lte_rrc_utf_send_msg((byte*)&mc_req, sizeof(mc_req));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for LTE_RRC_CFG_REQI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_reqi_s) == buf_len);
  cfg_req_ptr = (lte_rrc_cfg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_CFG_REQI == cfg_req_ptr->msg_hdr.id);

  TF_MSG("Checking for LTE_RRC_STOPPED_INDI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_stopped_indi_s) == buf_len);
  stopped_ind_ptr = (lte_rrc_stopped_indi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_STOPPED_INDI == stopped_ind_ptr->msg_hdr.id);

  TF_MSG("Sending cfg_cnfi");
  lte_rrc_init_default_hdr(&cfg_cnfi.msg_hdr,LTE_RRC_CFG_CNFI);
  cfg_cnfi.proc = LTE_RRC_PROC_CONTROLLER;
  cfg_cnfi.trans_id = 0;
  lte_rrc_utf_send_msg((byte*)&cfg_cnfi, sizeof(cfg_cnfi));

  TF_MSG("Checking for LTE_PDCPUL_STOP_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_pdcp_stop_req_msg_s) == buf_len);
  pdcp_stop_req_ptr = (lte_pdcp_stop_req_msg_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_PDCPUL_STOP_REQ == pdcp_stop_req_ptr->msg_hdr.id); 

  TF_MSG("Checking for LTE_PDCPDL_STOP_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_pdcp_stop_req_msg_s) == buf_len);
  pdcp_stop_req_ptr = (lte_pdcp_stop_req_msg_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_PDCPDL_STOP_REQ == pdcp_stop_req_ptr->msg_hdr.id); 

  TF_MSG("Checking for LTE_RLCUL_STOP_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rlc_stop_req_s) == buf_len);
  rlc_stop_req_ptr = (lte_rlc_stop_req_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RLCUL_STOP_REQ == rlc_stop_req_ptr->msg_hdr.id); 

  TF_MSG("Checking for LTE_RLCDL_STOP_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rlc_stop_req_s) == buf_len);
  rlc_stop_req_ptr = (lte_rlc_stop_req_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RLCDL_STOP_REQ == rlc_stop_req_ptr->msg_hdr.id); 

  TF_MSG("Checking for LTE_MAC_STOP_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_mac_stop_req_msg_s) == buf_len);
  mac_stop_req_ptr = (lte_mac_stop_req_msg_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_MAC_STOP_REQ == mac_stop_req_ptr->hdr.id); 

}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcControllerDeactConnectedTest);

void LteRrcControllerDeactConnectedTest::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CONTROLLER_SM, ext_umid_list, 12);
}

void LteRrcControllerDeactConnectedTest::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  lte_rrc_mode_change_cnfi_s *mc_cnf_ptr;
  lte_cphy_stop_req_s *stop_req_ptr;

  LteRrcControllerDeactConnected_Test(FALSE);

  TF_MSG("Checking for LTE_CPHY_STOP_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_stop_req_s) == buf_len);
  stop_req_ptr = (lte_cphy_stop_req_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_CPHY_STOP_REQ == stop_req_ptr->msgr_hdr.id); 

  TF_MSG("Checking for INACTIVE state");
  TF_ASSERT(lte_rrc_controller_get_state() == LTE_RRC_CONTROLLER_SM__INACTIVE);

  TF_MSG("Sending PDCPUL Stop Cnf");
  msgr_init_hdr(&pdcp_stop_cnf.msg_hdr,MSGR_LTE_PDCPUL,LTE_PDCPUL_STOP_CNF);
  pdcp_stop_cnf.cnf_status = E_SUCCESS; 
  lte_rrc_utf_send_msg((byte*)&pdcp_stop_cnf, sizeof(pdcp_stop_cnf));

  TF_MSG("Sending PDCPDL Stop Cnf");
  msgr_init_hdr(&pdcp_stop_cnf.msg_hdr,MSGR_LTE_PDCPDL,LTE_PDCPDL_STOP_CNF);
  pdcp_stop_cnf.cnf_status = E_SUCCESS; 
  lte_rrc_utf_send_msg((byte*)&pdcp_stop_cnf, sizeof(pdcp_stop_cnf));

  TF_MSG("Sending RLCUL Stop Cnf");
  msgr_init_hdr(&rlc_stop_cnf.msg_hdr,MSGR_LTE_RLCUL,LTE_RLCUL_STOP_CNF);
  rlc_stop_cnf.cnf_status = E_SUCCESS; 
  lte_rrc_utf_send_msg((byte*)&rlc_stop_cnf, sizeof(rlc_stop_cnf));

  TF_MSG("Sending RLCDL Stop Cnf");
  msgr_init_hdr(&rlc_stop_cnf.msg_hdr,MSGR_LTE_RLCDL,LTE_RLCDL_STOP_CNF);
  rlc_stop_cnf.cnf_status = E_SUCCESS; 
  lte_rrc_utf_send_msg((byte*)&rlc_stop_cnf, sizeof(rlc_stop_cnf));

  TF_MSG("Sending MAC Stop Cnf");
  msgr_init_hdr(&mac_stop_cnf.hdr,MSGR_LTE_MAC,LTE_MAC_STOP_CNF);
  mac_stop_cnf.status = E_SUCCESS; 
  lte_rrc_utf_send_msg((byte*)&mac_stop_cnf, sizeof(mac_stop_cnf));

  TF_MSG("Sending Stop Cnf");
  msgr_init_hdr(&stop_cnf.msgr_hdr,MSGR_LTE_CPHY,LTE_CPHY_STOP_CNF);
  stop_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&stop_cnf, sizeof(stop_cnf));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for LTE_RRC_MODE_CHANGE_CNFI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_mode_change_cnfi_s) == buf_len);
  mc_cnf_ptr = (lte_rrc_mode_change_cnfi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_MODE_CHANGE_CNFI == mc_cnf_ptr->msg_hdr.id);
  TF_ASSERT(TRUE == mc_cnf_ptr->status);

  TF_MSG("Checking for INACTIVE state");
  TF_ASSERT(lte_rrc_controller_get_state() == LTE_RRC_CONTROLLER_SM__INACTIVE);

  TF_MSG("End of test");
}

void LteRrcControllerDeactConnectedTest::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcControllerDeactConnectedTest_DSDSDeactCSFBCall);

void LteRrcControllerDeactConnectedTest_DSDSDeactCSFBCall::Setup()
{
  lte_rrc_utf_setup(LTE_RRC_CONTROLLER_SM, ext_umid_list, 13);
  lte_rrc_controller_csfb_init();
}

void LteRrcControllerDeactConnectedTest_DSDSDeactCSFBCall::Test()
{
  byte *buf_ptr;
  uint32 buf_len;
  lte_rrc_mode_change_cnfi_s *mc_cnf_ptr;
  lte_cphy_stop_req_s *stop_req_ptr;
  lte_rrc_ds_status_change_req_s ds_status_change;

  msgr_init_hdr(&ds_status_change.msg_hdr,MSGR_LTE_EMM,LTE_RRC_DS_STATUS_CHANGE_REQ);
  ds_status_change.dual_standby_mode = TRUE;
  ds_status_change.standby_mode = SYS_MODEM_DS_PREF_SINGLE_STANDBY;

  // Send DSDS Status Change
  lte_rrc_utf_send_msg((byte*)&ds_status_change.msg_hdr, sizeof(ds_status_change));

  TF_MSG("Sent DS Status Change Ind to RRC");

  LteRrcControllerDeactConnected_Test(TRUE);

  TF_MSG("Checking for LTE_CPHY_STOP_REQ");
  lte_rrc_utf_get_next_ext_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_cphy_stop_req_s) == buf_len);
  stop_req_ptr = (lte_cphy_stop_req_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_CPHY_STOP_REQ == stop_req_ptr->msgr_hdr.id); 
  TF_ASSERT(stop_req_ptr->stop_cause == LTE_CPHY_STOP_REQ_DEFAULT);

  TF_MSG("Checking for INACTIVE state");
  TF_ASSERT(lte_rrc_controller_get_state() == LTE_RRC_CONTROLLER_SM__INACTIVE);

  TF_MSG("Sending PDCPUL Stop Cnf");
  msgr_init_hdr(&pdcp_stop_cnf.msg_hdr,MSGR_LTE_PDCPUL,LTE_PDCPUL_STOP_CNF);
  pdcp_stop_cnf.cnf_status = E_SUCCESS; 
  lte_rrc_utf_send_msg((byte*)&pdcp_stop_cnf, sizeof(pdcp_stop_cnf));

  TF_MSG("Sending PDCPDL Stop Cnf");
  msgr_init_hdr(&pdcp_stop_cnf.msg_hdr,MSGR_LTE_PDCPDL,LTE_PDCPDL_STOP_CNF);
  pdcp_stop_cnf.cnf_status = E_SUCCESS; 
  lte_rrc_utf_send_msg((byte*)&pdcp_stop_cnf, sizeof(pdcp_stop_cnf));

  TF_MSG("Sending RLCUL Stop Cnf");
  msgr_init_hdr(&rlc_stop_cnf.msg_hdr,MSGR_LTE_RLCUL,LTE_RLCUL_STOP_CNF);
  rlc_stop_cnf.cnf_status = E_SUCCESS; 
  lte_rrc_utf_send_msg((byte*)&rlc_stop_cnf, sizeof(rlc_stop_cnf));

  TF_MSG("Sending RLCDL Stop Cnf");
  msgr_init_hdr(&rlc_stop_cnf.msg_hdr,MSGR_LTE_RLCDL,LTE_RLCDL_STOP_CNF);
  rlc_stop_cnf.cnf_status = E_SUCCESS; 
  lte_rrc_utf_send_msg((byte*)&rlc_stop_cnf, sizeof(rlc_stop_cnf));

  TF_MSG("Sending MAC Stop Cnf");
  msgr_init_hdr(&mac_stop_cnf.hdr,MSGR_LTE_MAC,LTE_MAC_STOP_CNF);
  mac_stop_cnf.status = E_SUCCESS; 
  lte_rrc_utf_send_msg((byte*)&mac_stop_cnf, sizeof(mac_stop_cnf));

  TF_MSG("Sending Stop Cnf");
  msgr_init_hdr(&stop_cnf.msgr_hdr,MSGR_LTE_CPHY,LTE_CPHY_STOP_CNF);
  stop_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&stop_cnf, sizeof(stop_cnf));

  lte_rrc_utf_wait_for_done();

  TF_MSG("Checking for LTE_RRC_MODE_CHANGE_CNFI");
  lte_rrc_utf_get_next_int_msg(&buf_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_mode_change_cnfi_s) == buf_len);
  mc_cnf_ptr = (lte_rrc_mode_change_cnfi_s *)(void*) buf_ptr;
  TF_ASSERT(LTE_RRC_MODE_CHANGE_CNFI == mc_cnf_ptr->msg_hdr.id);
  TF_ASSERT(TRUE == mc_cnf_ptr->status);

  TF_MSG("Checking for INACTIVE state");
  TF_ASSERT(lte_rrc_controller_get_state() == LTE_RRC_CONTROLLER_SM__INACTIVE);

  TF_MSG("End of test");
}

void LteRrcControllerDeactConnectedTest_DSDSDeactCSFBCall::Teardown()
{
  lte_rrc_utf_teardown();
}
