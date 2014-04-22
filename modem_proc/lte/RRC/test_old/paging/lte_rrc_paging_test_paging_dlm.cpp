/*!
  @file lte_rrc_paging_test_paging_dlm.cpp

  @brief
  Unit test code for the paging procedure.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/paging/lte_rrc_paging_test_paging_dlm.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/04/08   amit    Removed sibs_needed bitmask from get_sibs_req
09/23/08   ask     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "__lte_rrc_paging.h"
#include "lte_rrc_paging.h"
#include "lte_rrc_pagingi.h"
#include "lte_rrc_osys_asn1util.h"
#include <fs_public.h>
#include "__lte_rrc_controller.h"
#include "lte_misc.h"

}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_paging_test_msg.h"
#include <qtf_efs.h>
#include <fs_lib.h>

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module lte_rrc_paging_test_paging_dlm.cpp
*/
typedef struct
{
  int   internal_var;  /*!< My internal variable */
  void *internal_ptr;  /*!< My internal pointer */

} lte_rrc_paging_test_paging_dlm_s;


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_paging_test_paging_dlm.cpp
*/
DECLARE_LTE_RRC_PAGING_TEST_MSG2;
DECLARE_LTE_RRC_PAGING_TEST_MSG3;
DECLARE_LTE_RRC_PAGING_TEST_MSG4;
DECLARE_LTE_RRC_PAGING_TEST_MSG5;
DECLARE_LTE_RRC_PAGING_TEST_MSG6;
DECLARE_LTE_RRC_PAGING_TEST_MSG7;
DECLARE_LTE_RRC_PAGING_TEST_CMAS;
DECLARE_LTE_RRC_PAGING_TEST_CS;


extern "C"
{
  extern lte_rrc_paging_s lte_rrc_paging;
}

extern stm_state_t lte_rrc_paging_test_controller_state;
extern boolean lte_rrc_paging_test_controller_ue_in_dsds_mode;
extern boolean lte_rrc_paging_test_srb2_setup;

static uint64 time_since_power_up_ms = 0;

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern void lte_rrc_paging_test_init(void);


/*===========================================================================

                                FUNCTIONS

===========================================================================*/



/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDlmImsiNomatch);

void LteRrcPagingDlmImsiNomatch::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingDlmImsiNomatch::Test()
{
  lte_rrc_mh_dlm_s mh_dlm;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr;
  int decode_status;
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // Send a camped indication, Paging should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Send paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x0012;
  mh_dlm.pdu_num = lte_rrc_osys_PCCH_Message_PDU;

  // Decode Paging test message 2 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg2,
                               sizeof(lte_rrc_paging_test_msg2),
                               PCCH_Message,
                               &decode_status);

  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));

  lte_rrc_utf_wait_for_done();

  // dlm_processed_indi should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We don't expect Paging to send a page ind to NAS since UE Id was bogus
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

}

void LteRrcPagingDlmImsiNomatch::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDlmImsiStmsiNomatch);

void LteRrcPagingDlmImsiStmsiNomatch::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingDlmImsiStmsiNomatch::Test()
{
  lte_rrc_mh_dlm_s mh_dlm;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr;
  int decode_status;
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // Send a camped indication, Paging should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Send paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x0012;
  mh_dlm.pdu_num = lte_rrc_osys_PCCH_Message_PDU;

  // Decode Paging test message 3 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg3,
                               sizeof(lte_rrc_paging_test_msg3),
                               PCCH_Message,
                               &decode_status);
  
  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));

  lte_rrc_utf_wait_for_done();

  // dlm_processed_indi should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We don't expect Paging to send a page ind to NAS since UE Id was bogus
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
}

void LteRrcPagingDlmImsiStmsiNomatch::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDlmImsiMatch);

void LteRrcPagingDlmImsiMatch::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_RRC_PAGE_IND,
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingDlmImsiMatch::Test()
{
  lte_rrc_mh_dlm_s mh_dlm;
  lte_rrc_sim_update_req_s sim_update_req;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  uint8 imsi_digits[10] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0};
  uint32 buf_len;
  lte_rrc_page_ind_s *page_ind;
  lte_rrc_camped_indi_s camped_indi;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr;
  int decode_status;
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // Before we send the paging dlm, we send a sim_update to Paging and
  // configure the ue id
  // Send the sim_update request
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);

  // Just include one IMSI id
  sim_update_req.security_info_is_valid = FALSE;
  sim_update_req.ue_id_list_is_valid = TRUE;
  sim_update_req.ue_id_list.num_ue_identities = 1;
  sim_update_req.ue_id_list.ue_id[0].type = LTE_RRC_UE_ID_IMSI;
  sim_update_req.ue_id_list.ue_id[0].value.imsi.num_digits = sizeof(imsi_digits);
  memcpy(sim_update_req.ue_id_list.ue_id[0].value.imsi.imsi_digits,
         imsi_digits,
         sizeof(sim_update_req.ue_id_list.ue_id[0].value.imsi.imsi_digits));
  sim_update_req.access_class = 1 << 3;

  lte_rrc_utf_send_msg((byte*)&sim_update_req, sizeof(sim_update_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // We don't expect Paging to send a config req, since it is still in initial state
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // Send a camped indication, Paging should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Send paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x0012;
  mh_dlm.pdu_num = lte_rrc_osys_PCCH_Message_PDU;

  // Decode Paging test message 3 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg3,
                               sizeof(lte_rrc_paging_test_msg3),
                               PCCH_Message,
                               &decode_status);
  
  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We expect Paging to send a page ind to NAS since UE Id should have matched
  lte_rrc_utf_get_next_ext_msg((byte**)&page_ind, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_page_ind_s));
  TF_ASSERT(page_ind->msg_hdr.id == LTE_RRC_PAGE_IND);

  TF_ASSERT(page_ind->cn_domain == LTE_RRC_PAGING_CN_DOMAIN_PS);
  TF_ASSERT(page_ind->cause == LTE_RRC_PAGING_CAUSE_FFS);
  TF_ASSERT(page_ind->ue_id.type == LTE_RRC_UE_ID_IMSI);
  TF_ASSERT(page_ind->ue_id.value.imsi.num_digits == sizeof(imsi_digits));
  TF_ASSERT(memcmp(page_ind->ue_id.value.imsi.imsi_digits,
                   imsi_digits, sizeof(imsi_digits)) == 0);

  // dlm_processed_indi should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

}

void LteRrcPagingDlmImsiMatch::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDlmStmsiMatch);

void LteRrcPagingDlmStmsiMatch::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_RRC_PAGE_IND,
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingDlmStmsiMatch::Test()
{
  lte_rrc_mh_dlm_s mh_dlm;
  lte_rrc_sim_update_req_s sim_update_req;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  uint32 buf_len;
  lte_rrc_page_ind_s *page_ind;
  lte_rrc_camped_indi_s camped_indi;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr;
  int decode_status;
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // Before we send the paging dlm, we send a sim_update to Paging and
  // configure the ue id
  // Send the sim_update request
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);

  // Just include one S-TMSI id
  sim_update_req.security_info_is_valid = FALSE;
  sim_update_req.ue_id_list_is_valid = TRUE;
  sim_update_req.ue_id_list.num_ue_identities = 1;
  sim_update_req.ue_id_list.ue_id[0].type = LTE_RRC_UE_ID_STMSI;
  sim_update_req.ue_id_list.ue_id[0].value.s_tmsi.m_tmsi = 0x34567890;
  sim_update_req.ue_id_list.ue_id[0].value.s_tmsi.mmec = 0x12;
  sim_update_req.access_class = 1 << 3;

  lte_rrc_utf_send_msg((byte*)&sim_update_req, sizeof(sim_update_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // We don't expect Paging to send a config req, since it is still in initial state
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // Send a camped indication, Paging should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Send paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x0012;
  mh_dlm.pdu_num = lte_rrc_osys_PCCH_Message_PDU;

  // Decode Paging test message 3 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg3,
                               sizeof(lte_rrc_paging_test_msg3),
                               PCCH_Message,
                               &decode_status);
  
  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We expect Paging to send a page ind to NAS since UE Id should have matched
  lte_rrc_utf_get_next_ext_msg((byte**)&page_ind, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_page_ind_s));
  TF_ASSERT(page_ind->msg_hdr.id == LTE_RRC_PAGE_IND);

  TF_ASSERT(page_ind->cn_domain == LTE_RRC_PAGING_CN_DOMAIN_PS);
  TF_ASSERT(page_ind->cause == LTE_RRC_PAGING_CAUSE_FFS);
  TF_ASSERT(page_ind->ue_id.type == LTE_RRC_UE_ID_STMSI);
  TF_ASSERT(page_ind->ue_id.value.s_tmsi.mmec == 0x12);
  TF_ASSERT(page_ind->ue_id.value.s_tmsi.m_tmsi == 0x34567890);

  // dlm_processed_indi should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

}

void LteRrcPagingDlmStmsiMatch::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDlmSiUpdate);

void LteRrcPagingDlmSiUpdate::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_RRC_PAGE_IND,
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}


void LteRrcPagingDlmSiUpdate::Test()
{
  lte_rrc_mh_dlm_s mh_dlm;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  uint32 buf_len;
  lte_rrc_get_sibs_reqi_s *get_sibs;
  lte_rrc_camped_indi_s camped_indi;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr;
  int decode_status;
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // Send a camped indication, Paging should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Send paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x0012;
  mh_dlm.pdu_num = lte_rrc_osys_PCCH_Message_PDU;

  // Decode Paging test message 4 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg4,
                               sizeof(lte_rrc_paging_test_msg4),
                               PCCH_Message,
                               &decode_status);
  
  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Make sure no page inds are thrown
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(ext_queue_count == 0);

  // We expect Paging to send a get_sibs req to SIB
  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_reqi_s));
  TF_ASSERT(get_sibs->msg_hdr.id == LTE_RRC_GET_SIBS_REQI);

  // Check if SFN and CELL ID have been fwded correctly
  TF_ASSERT(get_sibs->cell_id.phy_cell_id == 0x1234);
  TF_ASSERT(get_sibs->cell_id.freq == 0x5678);
  TF_ASSERT(get_sibs->sfn.sys_fn == 0x0012);

  // Check requesting procedure and cause fields
  TF_ASSERT(get_sibs->req_proc == LTE_RRC_PROC_PG);
  TF_ASSERT(get_sibs->cause == LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE);
  TF_ASSERT(get_sibs->cnf_needed == FALSE);
  
  // dlm_processed_indi should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

}

void LteRrcPagingDlmSiUpdate::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDlmEtws);

void LteRrcPagingDlmEtws::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_RRC_PAGE_IND,
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));

}


void LteRrcPagingDlmEtws::Test()
{
  lte_rrc_mh_dlm_s mh_dlm;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  uint32 buf_len;
  lte_rrc_get_sibs_reqi_s *get_sibs;
  lte_rrc_camped_indi_s camped_indi;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr;
  int decode_status;
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // Send a camped indication, Paging should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Send paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x0012;
  mh_dlm.pdu_num = lte_rrc_osys_PCCH_Message_PDU;

  // Decode Paging test message 5 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg5,
                               sizeof(lte_rrc_paging_test_msg5),
                               PCCH_Message,
                               &decode_status);

  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Make sure no page inds are thrown
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(ext_queue_count == 0);

  // We expect Paging to send a get_sibs req to SIB
  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_reqi_s));
  TF_ASSERT(get_sibs->msg_hdr.id == LTE_RRC_GET_SIBS_REQI);

  // Check if SFN and CELL ID have been fwded correctly
  TF_ASSERT(get_sibs->cell_id.phy_cell_id == 0x1234);
  TF_ASSERT(get_sibs->cell_id.freq == 0x5678);
  TF_ASSERT(get_sibs->sfn.sys_fn == 0x0012);

  // Check requesting procedure and cause fields
  TF_ASSERT(get_sibs->req_proc == LTE_RRC_PROC_PG);
  TF_ASSERT(get_sibs->cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
  TF_ASSERT(get_sibs->cnf_needed == FALSE);

  // dlm_processed_indi should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

}

void LteRrcPagingDlmEtws::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDlmSiUpdateEtws);

void LteRrcPagingDlmSiUpdateEtws::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_RRC_PAGE_IND,
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));

}


void LteRrcPagingDlmSiUpdateEtws::Test()
{
  lte_rrc_mh_dlm_s mh_dlm;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  uint32 buf_len;
  lte_rrc_get_sibs_reqi_s *get_sibs;
  lte_rrc_camped_indi_s camped_indi;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr;
  int decode_status;
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // Send a camped indication, Paging should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Send paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x0012;
  mh_dlm.pdu_num = lte_rrc_osys_PCCH_Message_PDU;

  // Decode Paging test message 6 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg6,
                               sizeof(lte_rrc_paging_test_msg6),
                               PCCH_Message,
                               &decode_status);
  
  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Make sure no page inds are thrown
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(ext_queue_count == 0);

  // We expect Paging to send a get_sibs req to SIB for SiUpdate
  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_reqi_s));
  TF_ASSERT(get_sibs->msg_hdr.id == LTE_RRC_GET_SIBS_REQI);

  // Check if SFN and CELL ID have been fwded correctly
  TF_ASSERT(get_sibs->cell_id.phy_cell_id == 0x1234);
  TF_ASSERT(get_sibs->cell_id.freq == 0x5678);
  TF_ASSERT(get_sibs->sfn.sys_fn == 0x0012);

  // Check requesting procedure and cause fields
  TF_ASSERT(get_sibs->req_proc == LTE_RRC_PROC_PG);
  TF_ASSERT(get_sibs->cause == LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE);
  TF_ASSERT(get_sibs->cnf_needed == FALSE);

  // We expect Paging to send another get_sibs req to SIB for Etws
  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_reqi_s));
  TF_ASSERT(get_sibs->msg_hdr.id == LTE_RRC_GET_SIBS_REQI);

  // Check if SFN and CELL ID have been fwded correctly
  TF_ASSERT(get_sibs->cell_id.phy_cell_id == 0x1234);
  TF_ASSERT(get_sibs->cell_id.freq == 0x5678);
  TF_ASSERT(get_sibs->sfn.sys_fn == 0x0012);

  // Check requesting procedure and cause fields
  TF_ASSERT(get_sibs->req_proc == LTE_RRC_PROC_PG);
  TF_ASSERT(get_sibs->cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
  TF_ASSERT(get_sibs->cnf_needed == FALSE);

  // dlm_processed_indi should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

}

void LteRrcPagingDlmSiUpdateEtws::Teardown()
{
  qtf_efs_deltree();
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDlmImsiStmsiNomatchSiUpdateEtws);

void LteRrcPagingDlmImsiStmsiNomatchSiUpdateEtws::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
  //lte_rrc_paging.sd_ptr->etws_supported = TRUE;
}

void LteRrcPagingDlmImsiStmsiNomatchSiUpdateEtws::Test()
{
  lte_rrc_mh_dlm_s mh_dlm;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_get_sibs_reqi_s *get_sibs;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr;
  int decode_status;
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // Send a camped indication, Paging should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Send paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x0012;
  mh_dlm.pdu_num = lte_rrc_osys_PCCH_Message_PDU;

  // Decode Paging test message 7 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg7,
                               sizeof(lte_rrc_paging_test_msg7),
                               PCCH_Message,
                               &decode_status);
  
  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We don't expect Paging to send a page ind to NAS since UE Id was bogus
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(ext_queue_count == 0);

  // We expect Paging to send a get_sibs req to SIB for SiUpdate
  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_reqi_s));
  TF_ASSERT(get_sibs->msg_hdr.id == LTE_RRC_GET_SIBS_REQI);

  // Check if SFN and CELL ID have been fwded correctly
  TF_ASSERT(get_sibs->cell_id.phy_cell_id == 0x1234);
  TF_ASSERT(get_sibs->cell_id.freq == 0x5678);
  TF_ASSERT(get_sibs->sfn.sys_fn == 0x0012);

  // Check requesting procedure and cause fields
  TF_ASSERT(get_sibs->req_proc == LTE_RRC_PROC_PG);
  TF_ASSERT(get_sibs->cause == LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE);
  TF_ASSERT(get_sibs->cnf_needed == FALSE);

  // We expect Paging to send another get_sibs req to SIB for Etws
  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_reqi_s));
  TF_ASSERT(get_sibs->msg_hdr.id == LTE_RRC_GET_SIBS_REQI);

  // Check if SFN and CELL ID have been fwded correctly
  TF_ASSERT(get_sibs->cell_id.phy_cell_id == 0x1234);
  TF_ASSERT(get_sibs->cell_id.freq == 0x5678);
  TF_ASSERT(get_sibs->sfn.sys_fn == 0x0012);

  // Check requesting procedure and cause fields
  TF_ASSERT(get_sibs->req_proc == LTE_RRC_PROC_PG);
  TF_ASSERT(get_sibs->cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
  TF_ASSERT(get_sibs->cnf_needed == FALSE);

  // dlm_processed_indi should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

}

void LteRrcPagingDlmImsiStmsiNomatchSiUpdateEtws::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDlmImsiStmsiMatchSiUpdateEtws);

void LteRrcPagingDlmImsiStmsiMatchSiUpdateEtws::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_RRC_PAGE_IND,
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
  //lte_rrc_paging.sd_ptr->etws_supported = TRUE;
}

void LteRrcPagingDlmImsiStmsiMatchSiUpdateEtws::Test()
{
  lte_rrc_mh_dlm_s mh_dlm;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sim_update_req_s sim_update_req;
  lte_rrc_get_sibs_reqi_s *get_sibs;
  lte_rrc_page_ind_s *page_ind;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr;
  int decode_status;
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // Before we send the paging dlm, we send a sim_update to Paging and
  // configure the ue id
  // Send the sim_update request
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);

  // Just include one S-TMSI id
  sim_update_req.security_info_is_valid = FALSE;
  sim_update_req.ue_id_list_is_valid = TRUE;
  sim_update_req.ue_id_list.num_ue_identities = 1;
  sim_update_req.ue_id_list.ue_id[0].type = LTE_RRC_UE_ID_STMSI;
  sim_update_req.ue_id_list.ue_id[0].value.s_tmsi.m_tmsi = 0x34567890;
  sim_update_req.ue_id_list.ue_id[0].value.s_tmsi.mmec = 0x12;
  sim_update_req.access_class = 1 << 3;

  lte_rrc_utf_send_msg((byte*)&sim_update_req, sizeof(sim_update_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // We don't expect Paging to send a config req, since it is still in initial state
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // Send a camped indication, Paging should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Send paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x0012;
  mh_dlm.pdu_num = lte_rrc_osys_PCCH_Message_PDU;

  // Decode Paging test message 7 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg7,
                               sizeof(lte_rrc_paging_test_msg7),
                               PCCH_Message,
                               &decode_status);
  
  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We expect Paging to send a page ind to NAS since UE Id should have matched
  lte_rrc_utf_get_next_ext_msg((byte**)&page_ind, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_page_ind_s));
  TF_ASSERT(page_ind->msg_hdr.id == LTE_RRC_PAGE_IND);

  TF_ASSERT(page_ind->cn_domain == LTE_RRC_PAGING_CN_DOMAIN_PS);
  TF_ASSERT(page_ind->cause == LTE_RRC_PAGING_CAUSE_FFS);
  TF_ASSERT(page_ind->ue_id.type == LTE_RRC_UE_ID_STMSI);
  TF_ASSERT(page_ind->ue_id.value.s_tmsi.mmec == 0x12);
  TF_ASSERT(page_ind->ue_id.value.s_tmsi.m_tmsi == 0x34567890);

  // We expect Paging to send a get_sibs req to SIB for SiUpdate
  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_reqi_s));
  TF_ASSERT(get_sibs->msg_hdr.id == LTE_RRC_GET_SIBS_REQI);

  // Check if SFN and CELL ID have been fwded correctly
  TF_ASSERT(get_sibs->cell_id.phy_cell_id == 0x1234);
  TF_ASSERT(get_sibs->cell_id.freq == 0x5678);
  TF_ASSERT(get_sibs->sfn.sys_fn == 0x0012);

  // Check requesting procedure and cause fields
  TF_ASSERT(get_sibs->req_proc == LTE_RRC_PROC_PG);
  TF_ASSERT(get_sibs->cause == LTE_RRC_GET_SIBS_CAUSE_SIB_UPDATE);
  TF_ASSERT(get_sibs->cnf_needed == FALSE);

  // We expect Paging to send another get_sibs req to SIB for Etws
  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_reqi_s));
  TF_ASSERT(get_sibs->msg_hdr.id == LTE_RRC_GET_SIBS_REQI);

  // Check if SFN and CELL ID have been fwded correctly
  TF_ASSERT(get_sibs->cell_id.phy_cell_id == 0x1234);
  TF_ASSERT(get_sibs->cell_id.freq == 0x5678);
  TF_ASSERT(get_sibs->sfn.sys_fn == 0x0012);

  // Check requesting procedure and cause fields
  TF_ASSERT(get_sibs->req_proc == LTE_RRC_PROC_PG);
  TF_ASSERT(get_sibs->cause == LTE_RRC_GET_SIBS_CAUSE_ETWS);
  TF_ASSERT(get_sibs->cnf_needed == FALSE);

  // dlm_processed_indi should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

}

void LteRrcPagingDlmImsiStmsiMatchSiUpdateEtws::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDlmEtwsNotSupported);

void LteRrcPagingDlmEtwsNotSupported::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_RRC_PAGE_IND,
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  // Code to set etws_supported = FALSE by EFS
  int oflag, mode, efs_ret;
  boolean etws_supported = FALSE;

  /* Init EFS */
  qtf_efs_initialize("LteRrcPagingDlmSiUpdateEtws");

  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/sib/etws", 
                    &etws_supported, sizeof(etws_supported), oflag, mode) >= 0);

  etws_supported = TRUE;

  /* Read the EFS to get the etws_supported flag */
  TF_ASSERT((efs_ret = efs_get("/nv/item_files/modem/lte/rrc/sib/etws",
                               &etws_supported, 
                               sizeof(etws_supported))) != -1);

  TF_ASSERT(etws_supported == FALSE);

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));

}


void LteRrcPagingDlmEtwsNotSupported::Test()
{
  lte_rrc_mh_dlm_s mh_dlm;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  uint32 buf_len;
  lte_rrc_get_sibs_reqi_s *get_sibs;
  lte_rrc_camped_indi_s camped_indi;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr;
  int decode_status;
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // Send a camped indication, Paging should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Send paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x0012;
  mh_dlm.pdu_num = lte_rrc_osys_PCCH_Message_PDU;

  // Decode Paging test message 5 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg5,
                               sizeof(lte_rrc_paging_test_msg5),
                               PCCH_Message,
                               &decode_status);

  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));

  lte_rrc_utf_wait_for_done();

  // dlm_processed_indi should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Make sure no page inds are thrown
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(ext_queue_count == 0);

  lte_rrc_utf_check_no_more_msgs();
}

void LteRrcPagingDlmEtwsNotSupported::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDlmCmas);

void LteRrcPagingDlmCmas::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_RRC_PAGE_IND,
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));

}


void LteRrcPagingDlmCmas::Test()
{
  lte_rrc_mh_dlm_s mh_dlm;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  uint32 buf_len;
  lte_rrc_get_sibs_reqi_s *get_sibs;
  lte_rrc_camped_indi_s camped_indi;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr;
  int decode_status;
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // Send a camped indication, Paging should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Send paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x0012;
  mh_dlm.pdu_num = lte_rrc_osys_PCCH_Message_PDU;

  // Decode Paging test message 5 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_cmas,
                               sizeof(lte_rrc_paging_test_cmas),
                               PCCH_Message,
                               &decode_status);

  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Make sure no page inds are thrown
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(ext_queue_count == 0);

  // We expect Paging to send a get_sibs req to SIB
  lte_rrc_utf_get_next_int_msg((byte**)&get_sibs, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_get_sibs_reqi_s));
  TF_ASSERT(get_sibs->msg_hdr.id == LTE_RRC_GET_SIBS_REQI);

  // Check if SFN and CELL ID have been fwded correctly
  TF_ASSERT(get_sibs->cell_id.phy_cell_id == 0x1234);
  TF_ASSERT(get_sibs->cell_id.freq == 0x5678);
  TF_ASSERT(get_sibs->sfn.sys_fn == 0x0012);

  // Check requesting procedure and cause fields
  TF_ASSERT(get_sibs->req_proc == LTE_RRC_PROC_PG);
  TF_ASSERT(get_sibs->cause == LTE_RRC_GET_SIBS_CAUSE_CMAS);
  TF_ASSERT(get_sibs->cnf_needed == FALSE);

  // dlm_processed_indi should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

}

void LteRrcPagingDlmCmas::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingPageForUEinConnectedAndDSDS);

void LteRrcPagingPageForUEinConnectedAndDSDS::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_RRC_PAGE_IND,
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingPageForUEinConnectedAndDSDS::Test()
{
  lte_rrc_mh_dlm_s mh_dlm;
  lte_rrc_sim_update_req_s sim_update_req;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  uint8 imsi_digits[10] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0};
  uint32 buf_len;
  lte_rrc_page_ind_s *page_ind;
  lte_rrc_camped_indi_s camped_indi;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr;
  int decode_status;
  lte_rrc_connected_indi_s connected_indi;
  lte_rrc_initiate_conn_rel_indi_s *initiate_conn_rel_indi_ptr = NULL;
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // Before we send the paging dlm, we send a sim_update to Paging and
  // configure the ue id
  // Send the sim_update request
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);

  // Just include one IMSI id
  sim_update_req.security_info_is_valid = FALSE;
  sim_update_req.ue_id_list_is_valid = TRUE;
  sim_update_req.ue_id_list.num_ue_identities = 1;
  sim_update_req.ue_id_list.ue_id[0].type = LTE_RRC_UE_ID_IMSI;
  sim_update_req.ue_id_list.ue_id[0].value.imsi.num_digits = sizeof(imsi_digits);
  memcpy(sim_update_req.ue_id_list.ue_id[0].value.imsi.imsi_digits,
         imsi_digits,
         sizeof(sim_update_req.ue_id_list.ue_id[0].value.imsi.imsi_digits));
  sim_update_req.access_class = 1 << 3;

  lte_rrc_utf_send_msg((byte*)&sim_update_req, sizeof(sim_update_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // We don't expect Paging to send a config req, since it is still in initial state
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // Send a camped indication, Paging should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Send paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x0012;
  mh_dlm.pdu_num = lte_rrc_osys_PCCH_Message_PDU;

  // Decode Paging test message 3 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg3,
                               sizeof(lte_rrc_paging_test_msg3),
                               PCCH_Message,
                               &decode_status);
  
  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We expect Paging to send a page ind to NAS since UE Id should have matched
  lte_rrc_utf_get_next_ext_msg((byte**)&page_ind, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_page_ind_s));
  TF_ASSERT(page_ind->msg_hdr.id == LTE_RRC_PAGE_IND);

  TF_ASSERT(page_ind->cn_domain == LTE_RRC_PAGING_CN_DOMAIN_PS);
  TF_ASSERT(page_ind->cause == LTE_RRC_PAGING_CAUSE_FFS);
  TF_ASSERT(page_ind->ue_id.type == LTE_RRC_UE_ID_IMSI);
  TF_ASSERT(page_ind->ue_id.value.imsi.num_digits == sizeof(imsi_digits));
  TF_ASSERT(memcmp(page_ind->ue_id.value.imsi.imsi_digits,
                   imsi_digits, sizeof(imsi_digits)) == 0);

  // dlm_processed_indi should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

  lte_rrc_paging_test_controller_state = LTE_RRC_CONTROLLER_SM__CONNECTED;
  lte_rrc_paging_test_controller_ue_in_dsds_mode = TRUE;

  lte_set_current_time(time_since_power_up_ms += 50);

  // On receiving a connected indication, we should move to the connected state
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  lte_rrc_paging_test_srb2_setup = TRUE;
  lte_set_current_time(time_since_power_up_ms += 10100);

  // Send paging dlm to the paging sm
   lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

   // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
   mh_dlm.cell_id.phy_cell_id = 0x1234;
   mh_dlm.cell_id.freq = 0x5678;
   mh_dlm.sfn.sys_fn = 0x0012;
   mh_dlm.pdu_num = lte_rrc_osys_PCCH_Message_PDU;

   // Decode Paging test message 3 and attach it to the mh_dlm
   mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg3,
                                sizeof(lte_rrc_paging_test_msg3),
                                PCCH_Message,
                                &decode_status);

   lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));

   lte_rrc_utf_wait_for_done();

   /* catch the initiate connection release indication thrown by CRE */
   lte_rrc_utf_get_next_int_msg((byte **)&initiate_conn_rel_indi_ptr,&buf_len);
   TF_ASSERT(sizeof(lte_rrc_initiate_conn_rel_indi_s) == buf_len);
   TF_ASSERT(initiate_conn_rel_indi_ptr != NULL);
   TF_ASSERT(initiate_conn_rel_indi_ptr->rel_reason == LTE_RRC_CONN_REL_RLF);

  // dlm_processed_indi should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

}

void LteRrcPagingPageForUEinConnectedAndDSDS::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingPageForUEinConnectedLessThan10Sec);

void LteRrcPagingPageForUEinConnectedLessThan10Sec::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_RRC_PAGE_IND,
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingPageForUEinConnectedLessThan10Sec::Test()
{
  lte_rrc_mh_dlm_s mh_dlm;
  lte_rrc_sim_update_req_s sim_update_req;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  uint8 imsi_digits[10] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0};
  uint32 buf_len;
  lte_rrc_page_ind_s *page_ind;
  lte_rrc_camped_indi_s camped_indi;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr;
  int decode_status;
  lte_rrc_connected_indi_s connected_indi;
  lte_rrc_initiate_conn_rel_indi_s *initiate_conn_rel_indi_ptr = NULL;
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // Before we send the paging dlm, we send a sim_update to Paging and
  // configure the ue id
  // Send the sim_update request
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);

  // Just include one IMSI id
  sim_update_req.security_info_is_valid = FALSE;
  sim_update_req.ue_id_list_is_valid = TRUE;
  sim_update_req.ue_id_list.num_ue_identities = 1;
  sim_update_req.ue_id_list.ue_id[0].type = LTE_RRC_UE_ID_IMSI;
  sim_update_req.ue_id_list.ue_id[0].value.imsi.num_digits = sizeof(imsi_digits);
  memcpy(sim_update_req.ue_id_list.ue_id[0].value.imsi.imsi_digits,
         imsi_digits,
         sizeof(sim_update_req.ue_id_list.ue_id[0].value.imsi.imsi_digits));
  sim_update_req.access_class = 1 << 3;

  lte_rrc_utf_send_msg((byte*)&sim_update_req, sizeof(sim_update_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // We don't expect Paging to send a config req, since it is still in initial state
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // Send a camped indication, Paging should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Send paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x0012;
  mh_dlm.pdu_num = lte_rrc_osys_PCCH_Message_PDU;

  // Decode Paging test message 3 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg_cs,
                               sizeof(lte_rrc_paging_test_msg_cs),
                               PCCH_Message,
                               &decode_status);
  
  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We expect Paging to send a page ind to NAS since UE Id should have matched
  lte_rrc_utf_get_next_ext_msg((byte**)&page_ind, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_page_ind_s));
  TF_ASSERT(page_ind->msg_hdr.id == LTE_RRC_PAGE_IND);

  TF_ASSERT(page_ind->cn_domain == LTE_RRC_PAGING_CN_DOMAIN_CS);
  TF_ASSERT(page_ind->cause == LTE_RRC_PAGING_CAUSE_FFS);
  TF_ASSERT(page_ind->ue_id.type == LTE_RRC_UE_ID_IMSI);
  TF_ASSERT(page_ind->ue_id.value.imsi.num_digits == sizeof(imsi_digits));
  TF_ASSERT(memcmp(page_ind->ue_id.value.imsi.imsi_digits,
                   imsi_digits, sizeof(imsi_digits)) == 0);

  // dlm_processed_indi should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

  lte_rrc_paging_test_controller_state = LTE_RRC_CONTROLLER_SM__CONNECTED;

  lte_set_current_time(time_since_power_up_ms += 50);

  // On receiving a connected indication, we should move to the connected state
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  lte_set_current_time(time_since_power_up_ms += 5000);

  // Send paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x0012;
  mh_dlm.pdu_num = lte_rrc_osys_PCCH_Message_PDU;

  // Decode Paging test message 3 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg_cs,
                               sizeof(lte_rrc_paging_test_msg_cs),
                               PCCH_Message,
                               &decode_status);

  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));
  lte_rrc_utf_wait_for_done();

  // We expect Paging to send a page ind to NAS since UE Id should have matched
  lte_rrc_utf_get_next_ext_msg((byte**)&page_ind, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_rrc_page_ind_s));
  TF_ASSERT(page_ind->msg_hdr.id == LTE_RRC_PAGE_IND);

  TF_ASSERT(page_ind->cn_domain == LTE_RRC_PAGING_CN_DOMAIN_CS);
  TF_ASSERT(page_ind->cause == LTE_RRC_PAGING_CAUSE_FFS);
  TF_ASSERT(page_ind->ue_id.type == LTE_RRC_UE_ID_IMSI);

  // dlm_processed_indi should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);


  lte_rrc_paging_test_srb2_setup = FALSE;

  lte_set_current_time(time_since_power_up_ms += 5500);

  // Send paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x0012;
  mh_dlm.pdu_num = lte_rrc_osys_PCCH_Message_PDU;

  // Decode Paging test message 3 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg3,
                               sizeof(lte_rrc_paging_test_msg3),
                               PCCH_Message,
                               &decode_status);

  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));
  lte_rrc_utf_wait_for_done();

  // dlm_processed_indi should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

  lte_rrc_paging_test_srb2_setup = TRUE;

  lte_set_current_time(time_since_power_up_ms += 100);

  // Send paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x0012;
  mh_dlm.pdu_num = lte_rrc_osys_PCCH_Message_PDU;

  // Decode Paging test message 3 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg_cs,
                               sizeof(lte_rrc_paging_test_msg_cs),
                               PCCH_Message,
                               &decode_status);

  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));
  lte_rrc_utf_wait_for_done();

  /* catch the initiate connection release indication thrown by CRE */
  lte_rrc_utf_get_next_int_msg((byte **)&initiate_conn_rel_indi_ptr,&buf_len);
  TF_ASSERT(sizeof(lte_rrc_initiate_conn_rel_indi_s) == buf_len);
  TF_ASSERT(initiate_conn_rel_indi_ptr != NULL);
  TF_ASSERT(initiate_conn_rel_indi_ptr->rel_reason == LTE_RRC_CONN_REL_RLF);

  // dlm_processed_indi should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

}

void LteRrcPagingPageForUEinConnectedLessThan10Sec::Teardown()
{
  lte_rrc_utf_teardown();
}



