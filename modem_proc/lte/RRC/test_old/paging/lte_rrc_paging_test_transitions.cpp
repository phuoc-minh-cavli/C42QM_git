/*!
  @file lte_rrc_paging_test_transitions.cpp

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/paging/lte_rrc_paging_test_transitions.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/26/08   ask     Initial Revision
           
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
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_paging_test_msg.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module lte_rrc_paging_test_transitions.cpp
*/
typedef struct
{
  int   internal_var;  /*!< My internal variable */
  void *internal_ptr;  /*!< My internal pointer */

} lte_rrc_paging_test_transitions_s;


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_paging_test_transitions.cpp
*/
DECLARE_LTE_RRC_PAGING_TEST_MSG1;

extern "C"
{
  extern lte_rrc_paging_s lte_rrc_paging;
}

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

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingBasicTrans);

void LteRrcPagingBasicTrans::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingBasicTrans::Test()
{
  lte_rrc_camped_indi_s         camped_indi;
  lte_rrc_stopped_indi_s        stopped_indi;
  lte_rrc_connected_indi_s      connected_indi;
  lte_rrc_not_camped_indi_s     not_camped_indi;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;

  uint8 int_queue_count;
  uint8 ext_queue_count;

  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // ----------------------------------------------------------------------

  // On receiving a camped indication, we should move to the idle_camped state
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

  lte_rrc_utf_reset();

  // ----------------------------------------------------------------------

  // On receiving a not camped indication, we should move to the initial state
  lte_rrc_init_default_hdr(&not_camped_indi.msg_hdr, LTE_RRC_NOT_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&not_camped_indi, sizeof(not_camped_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  lte_rrc_utf_reset();

  // ----------------------------------------------------------------------

  // On receiving a camped indication, we should move to the idle_camped state
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

  lte_rrc_utf_reset();

  // ----------------------------------------------------------------------

  // On receiving a connected indication, we should move to the connected state
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  lte_rrc_utf_reset();

  // ----------------------------------------------------------------------

  // On receiving a camped indication, we should be back to idle_camped state
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

  lte_rrc_utf_reset();

  // ----------------------------------------------------------------------

  // On receiving a connected indication, we should move to the connected state
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  lte_rrc_utf_reset();

  // ----------------------------------------------------------------------

  // On receiving a not camped indication, we should move to the initial state
  lte_rrc_init_default_hdr(&not_camped_indi.msg_hdr, LTE_RRC_NOT_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&not_camped_indi, sizeof(not_camped_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  lte_rrc_utf_reset();

  // ----------------------------------------------------------------------

  // On receiving a not camped indication, we should remain in the initial state
  lte_rrc_init_default_hdr(&not_camped_indi.msg_hdr, LTE_RRC_NOT_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&not_camped_indi, sizeof(not_camped_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  lte_rrc_utf_reset();

  // ----------------------------------------------------------------------

  // On receiving a stopped indication, we should remain in the initial state
  lte_rrc_init_default_hdr(&stopped_indi.msg_hdr, LTE_RRC_STOPPED_INDI);
  lte_rrc_utf_send_msg((byte*)&stopped_indi, sizeof(stopped_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  lte_rrc_utf_reset();

  // ----------------------------------------------------------------------

  // On receiving a camped indication, we should move to the idle_camped state
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

  lte_rrc_utf_reset();

  // ----------------------------------------------------------------------

  // On receiving a stopped indication, we should move to the initial state
  lte_rrc_init_default_hdr(&stopped_indi.msg_hdr, LTE_RRC_STOPPED_INDI);
  lte_rrc_utf_send_msg((byte*)&stopped_indi, sizeof(stopped_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  lte_rrc_utf_reset();

  // ----------------------------------------------------------------------

  // On receiving a camped indication, we should move to the idle_camped state
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

  lte_rrc_utf_reset();

  // ----------------------------------------------------------------------

  // On receiving a connected indication, we should move to the connected state
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  lte_rrc_utf_reset();

  // ----------------------------------------------------------------------

  // On receiving a stopped indication, we should move to the initial state
  lte_rrc_init_default_hdr(&stopped_indi.msg_hdr, LTE_RRC_STOPPED_INDI);
  lte_rrc_utf_send_msg((byte*)&stopped_indi, sizeof(stopped_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  lte_rrc_utf_reset();

}


void LteRrcPagingBasicTrans::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingSimUpdateTrans);

void LteRrcPagingSimUpdateTrans::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingSimUpdateTrans::Test()
{
  lte_rrc_sim_update_req_s sim_update_req;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_connected_indi_s connected_indi;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // Send sim update request to the paging sm
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);

  // Just include one S-TMSI id
  sim_update_req.security_info_is_valid = FALSE;
  sim_update_req.ue_id_list_is_valid = TRUE;
  sim_update_req.ue_id_list.num_ue_identities = 1;
  sim_update_req.ue_id_list.ue_id[0].type = LTE_RRC_UE_ID_STMSI;
  sim_update_req.ue_id_list.ue_id[0].value.s_tmsi.m_tmsi = 0x1234;
  sim_update_req.ue_id_list.ue_id[0].value.s_tmsi.mmec = 0x56;
  sim_update_req.access_class = 1 << 3;

  lte_rrc_utf_send_msg((byte*)&sim_update_req, sizeof(sim_update_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // We don't expect Paging to send a cfg req since UE is not camped yet
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // ----------------------------------------------------------------------

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

  // Repeat sending the sim_update request in the idle_camped state
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);

  // Just include one S-TMSI id
  sim_update_req.security_info_is_valid = FALSE;
  sim_update_req.ue_id_list_is_valid = TRUE;
  sim_update_req.ue_id_list.num_ue_identities = 1;
  sim_update_req.ue_id_list.ue_id[0].type = LTE_RRC_UE_ID_STMSI;
  sim_update_req.ue_id_list.ue_id[0].value.s_tmsi.m_tmsi = 0x1234;
  sim_update_req.ue_id_list.ue_id[0].value.s_tmsi.mmec = 0x56;
  sim_update_req.access_class = 1 << 3;

  lte_rrc_utf_send_msg((byte*)&sim_update_req, sizeof(sim_update_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We don't expect Paging sm to have thrown a idle drx cfg_req
  // since IMSI was not updated
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // Compare paging private data to see if s-tmsi was stored correctly
  TF_ASSERT(memcmp(&lte_rrc_paging.sd_ptr->ue_id[LTE_RRC_UE_ID_STMSI].value.s_tmsi, 
                   &sim_update_req.ue_id_list.ue_id[0].value.s_tmsi,
                   sizeof(lte_rrc_s_tmsi_s)) == 0);

  // ----------------------------------------------------------------------

  // On receiving a connected indication, we should move to the connected state
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  // Repeat sending the sim_update request in the idle_camped state
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);

  // Just include one S-TMSI id
  sim_update_req.security_info_is_valid = FALSE;
  sim_update_req.ue_id_list_is_valid = TRUE;
  sim_update_req.ue_id_list.num_ue_identities = 1;
  sim_update_req.ue_id_list.ue_id[0].type = LTE_RRC_UE_ID_STMSI;
  sim_update_req.ue_id_list.ue_id[0].value.s_tmsi.m_tmsi = 0x1234;
  sim_update_req.ue_id_list.ue_id[0].value.s_tmsi.mmec = 0x56;
  sim_update_req.access_class = 1 << 3;

  lte_rrc_utf_send_msg((byte*)&sim_update_req, sizeof(sim_update_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  // We don't expect Paging sm to have thrown a idle drx cfg_req
  // since IMSI was not updated
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // Compare paging private data to see if s-tmsi was stored correctly
  TF_ASSERT(memcmp(&lte_rrc_paging.sd_ptr->ue_id[LTE_RRC_UE_ID_STMSI].value.s_tmsi, 
                   &sim_update_req.ue_id_list.ue_id[0].value.s_tmsi,
                   sizeof(lte_rrc_s_tmsi_s)) == 0);
}

void LteRrcPagingSimUpdateTrans::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDrxInfoTrans);

void LteRrcPagingDrxInfoTrans::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingDrxInfoTrans::Test()
{
  lte_rrc_drx_info_req_s drx_info_req;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_connected_indi_s connected_indi;
  uint8 int_queue_count;
  uint8 ext_queue_count;

  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // Send drx info request to the paging sm
  lte_rrc_init_default_hdr(&drx_info_req.msg_hdr, LTE_RRC_DRX_INFO_REQ);

  // Set the ue-specific paging cycle
  drx_info_req.paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T32;

  lte_rrc_utf_send_msg((byte*)&drx_info_req, sizeof(drx_info_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // We don't expect Paging to send a cfg req since UE is not camped yet
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // ----------------------------------------------------------------------

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

  // Repeat sending the drx request
  lte_rrc_init_default_hdr(&drx_info_req.msg_hdr, LTE_RRC_DRX_INFO_REQ);

  // Access private data of paging, and init the two paging cycles to a high value
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;
  lte_rrc_paging.sd_ptr->specific_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;
  lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_T2;

  // Set the ue-specific paging cycle
  drx_info_req.paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T32;

  lte_rrc_utf_send_msg((byte*)&drx_info_req, sizeof(drx_info_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);

  TF_ASSERT(cfg_req->idle_drx_param.paging_cycle == 320);

  // ----------------------------------------------------------------------

  // On receiving a connected indication, we should move to the connected state
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  // Repeat sending the drx request
  lte_rrc_init_default_hdr(&drx_info_req.msg_hdr, LTE_RRC_DRX_INFO_REQ);

  // Access private data of paging, and init the two paging cycles to a high value
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;
  lte_rrc_paging.sd_ptr->specific_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;

  // Set the ue-specific paging cycle
  drx_info_req.paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T32;

  lte_rrc_utf_send_msg((byte*)&drx_info_req, sizeof(drx_info_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);

  TF_ASSERT(cfg_req->idle_drx_param.paging_cycle == 320);
}


void LteRrcPagingDrxInfoTrans::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingPagingDlmTrans);

void LteRrcPagingPagingDlmTrans::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingPagingDlmTrans::Test()
{
  lte_rrc_mh_dlm_s mh_dlm;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_connected_indi_s connected_indi;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr;
  int decode_status;
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // Send paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x9012;
  mh_dlm.pdu_num = lte_rrc_osys_PCCH_Message_PDU;

  // Decode Paging test message 1 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg1,
                               sizeof(lte_rrc_paging_test_msg1),
                               PCCH_Message,
                               &decode_status);
  


  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // We don't expect Paging to send a page ind to NAS
  // since Paging dlm is not handled in the initial state
  // dlm_processed_indi should be sent from dispatcher
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // ----------------------------------------------------------------------

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

  // Repeat sending paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x9012;

  // Decode Paging test message 1 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg1,
                               sizeof(lte_rrc_paging_test_msg1),
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

  // ----------------------------------------------------------------------

  // On receiving a connected indication, we should move to the connected state
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  // Repeat sending paging dlm to the paging sm
  lte_rrc_init_default_hdr(&mh_dlm.msg_hdr, LTE_RRC_PAGING_DLM);

  // Enter a bogus cell id and sfn, just need to verify that this gets fwded correctly to CSP
  mh_dlm.cell_id.phy_cell_id = 0x1234;
  mh_dlm.cell_id.freq = 0x5678;
  mh_dlm.sfn.sys_fn = 0x9012;

  // Decode Paging test message 1 and attach it to the mh_dlm
  mh_dlm.dl_msg_ptr = (lte_rrc_osys_PCCH_Message *)lte_rrc_osys_asn1_decode_pdu((void*)lte_rrc_paging_test_msg1,
                               sizeof(lte_rrc_paging_test_msg1),
                               PCCH_Message,
                               &decode_status);
  
  lte_rrc_utf_send_msg((byte*)&mh_dlm, sizeof(mh_dlm));

  lte_rrc_utf_wait_for_done();

  // dlm_processed_indi should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&dlm_processed_indi_ptr, &buf_len);
  TF_ASSERT(dlm_processed_indi_ptr->msg_hdr.id == LTE_RRC_DLM_PROCESSED_INDI);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  // We don't expect Paging to send a page ind to NAS since UE Id was bogus
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

}

void LteRrcPagingPagingDlmTrans::Teardown()
{
  lte_rrc_utf_teardown();
}




TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingSibUpdatedTrans);

void LteRrcPagingSibUpdatedTrans::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingSibUpdatedTrans::Test()
{
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_connected_indi_s connected_indi;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // Send sib updated indi to the paging sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB2 was updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB2_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x5678;
  sib_updated_indi.cell_id.freq = 0x2345;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // We don't expect Paging to send anything since UE is not camped yet
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // Actually Paging should not even have processed the sib updated indi in the initial
  // state. Make sure that that is the case
  TF_ASSERT(lte_rrc_paging.dd_ptr->nb == LTE_RRC_NB_MAX);
  TF_ASSERT(lte_rrc_paging.sd_ptr->default_paging_cycle == LTE_RRC_DRX_CN_NOT_SPECIFIED);
  
  // ----------------------------------------------------------------------

  // Reset the sib2 info in paging private data
  lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_MAX;
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_NOT_SPECIFIED;

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

  TF_ASSERT(cfg_req->idle_drx_param.paging_cycle == 1280);
  TF_ASSERT(cfg_req->idle_drx_param.nb == LTE_L1_NB_HALFT);

  // Reset the sib2 info in paging private data
  lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_MAX;
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_NOT_SPECIFIED;

  // Repeat sending the sib updated indi in the idle_camped state
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB2 was updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB2_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x5678;
  sib_updated_indi.cell_id.freq = 0x2345;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);

  TF_ASSERT(cfg_req->idle_drx_param.paging_cycle == 1280);
  TF_ASSERT(cfg_req->idle_drx_param.nb == LTE_L1_NB_HALFT);

  // ----------------------------------------------------------------------

  // Reset the sib2 info in paging private data
  lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_MAX;
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_NOT_SPECIFIED;

  // On receiving a connected indication, we should move to the connected state
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  // Repeat sending the sib updated indi in the connected state
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB2 was updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB2_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x5678;
  sib_updated_indi.cell_id.freq = 0x2345;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);

  TF_ASSERT(cfg_req->idle_drx_param.paging_cycle == 1280);
  TF_ASSERT(cfg_req->idle_drx_param.nb == LTE_L1_NB_HALFT);
}

void LteRrcPagingSibUpdatedTrans::Teardown()
{
  lte_rrc_utf_teardown();
}

