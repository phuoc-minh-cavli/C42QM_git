/*!
  @file lte_rrc_paging_test_sim_update.cpp

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/paging/lte_rrc_paging_test_sim_update.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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
#include "__lte_rrc_cre.h"
#include "__lte_rrc_controller.h"
#include "lte_rrc_paging.h"
#include "lte_rrc_pagingi.h"
#include "lte_static_rrc_pendq.h"
#include "lte_rrc_osys_asn1util.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_paging_test_msg.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module lte_rrc_paging_test_sim_update.cpp
*/
typedef struct
{
  int   internal_var;  /*!< My internal variable */
  void *internal_ptr;  /*!< My internal pointer */

} lte_rrc_paging_test_sim_update_s;

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_paging_test_sim_update.cpp
*/
extern "C"
{
  extern lte_rrc_paging_s lte_rrc_paging;
  //extern lte_static_rrc_pendq_s lte_rrc_pendq;
}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern void lte_rrc_paging_test_init(void);
extern boolean lte_rrc_paging_test_config_ho_in_progress;
extern boolean lte_rrc_paging_test_cre_rlf_search_in_progress;
extern stm_state_t lte_rrc_paging_test_controller_state;
extern lte_rrc_revoke_priority_e lte_rrc_paging_test_revoke_priority;

extern "C"
{
uint16 lte_rrc_paging_calc_drx_ue_id
(
  lte_rrc_imsi_s *imsi  /*!< IMSI */
);

}

/*===========================================================================

                                FUNCTIONS

===========================================================================*/



/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingSimUpdateStmsi);

void LteRrcPagingSimUpdateStmsi::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingSimUpdateStmsi::Test()
{
  lte_rrc_sim_update_req_s sim_update_req;
  lte_rrc_camped_indi_s camped_indi;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // ----------------------------------------------------------------------
  // Send a camped indication, Paging should move to the idle_camped state
  // We test only in idle_camped state, assuming that the handling for connected state
  // is exactly the same.

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

  // Send the sim_update request in the idle_camped state
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
  
}

void LteRrcPagingSimUpdateStmsi::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingSimUpdateImsi);

void LteRrcPagingSimUpdateImsi::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingSimUpdateImsi::Test()
{
  lte_rrc_sim_update_req_s sim_update_req;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  lte_rrc_camped_indi_s camped_indi;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  uint8 imsi_digits[21] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0, 0x1, 0x2, 
                           0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0, 0x1};
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // ----------------------------------------------------------------------
  // Send a camped indication, Paging should move to the idle_camped state
  // We test only in idle_camped state, assuming that the handling for connected state
  // is exactly the same.

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

  // Send the sim_update request in the idle_camped state
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);

  // Access private data of paging, and init the two paging cycles to a high value
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;
  lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_T2;

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

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);

  TF_ASSERT(cfg_req->idle_drx_param.paging_cycle == 1280);
  TF_ASSERT(cfg_req->idle_drx_param.ue_id == 
            lte_rrc_paging_calc_drx_ue_id(&sim_update_req.ue_id_list.ue_id[0].value.imsi));
}

void LteRrcPagingSimUpdateImsi::Teardown()
{
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingSimUpdateImsiStmsi);

void LteRrcPagingSimUpdateImsiStmsi::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingSimUpdateImsiStmsi::Test()
{
  lte_rrc_sim_update_req_s sim_update_req;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  lte_rrc_camped_indi_s camped_indi;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  uint8 imsi_digits[21] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0, 0x1, 0x2, 
                           0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0, 0x1};
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // ----------------------------------------------------------------------
  // Send a camped indication, Paging should move to the idle_camped state
  // We test only in idle_camped state, assuming that the handling for connected state
  // is exactly the same.

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

  // Send the sim_update request in the idle_camped state
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);

  // Access private data of paging, and init the two paging cycles to a high value
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;
  lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_T2;

  // Just include one S-TMSI and one IMSI id
  sim_update_req.security_info_is_valid = FALSE;
  sim_update_req.ue_id_list_is_valid = TRUE;
  sim_update_req.ue_id_list.num_ue_identities = 2;
  sim_update_req.ue_id_list.ue_id[0].type = LTE_RRC_UE_ID_IMSI;
  sim_update_req.ue_id_list.ue_id[0].value.imsi.num_digits = sizeof(imsi_digits);
  memcpy(sim_update_req.ue_id_list.ue_id[0].value.imsi.imsi_digits,
         imsi_digits,
         sizeof(sim_update_req.ue_id_list.ue_id[0].value.imsi.imsi_digits));
  sim_update_req.ue_id_list.ue_id[1].type = LTE_RRC_UE_ID_STMSI;
  sim_update_req.ue_id_list.ue_id[1].value.s_tmsi.m_tmsi = 0x1234;
  sim_update_req.ue_id_list.ue_id[1].value.s_tmsi.mmec = 0x56;
  sim_update_req.access_class = 1 << 3;

  lte_rrc_utf_send_msg((byte*)&sim_update_req, sizeof(sim_update_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);

  TF_ASSERT(cfg_req->idle_drx_param.paging_cycle == 1280);
  TF_ASSERT(cfg_req->idle_drx_param.ue_id == 
            lte_rrc_paging_calc_drx_ue_id(&sim_update_req.ue_id_list.ue_id[0].value.imsi));

  // Compare paging private data to see if s-tmsi was stored correctly
  TF_ASSERT(memcmp(&lte_rrc_paging.sd_ptr->ue_id[LTE_RRC_UE_ID_STMSI].value.s_tmsi, 
                   &sim_update_req.ue_id_list.ue_id[1].value.s_tmsi,
                   sizeof(lte_rrc_s_tmsi_s)) == 0);
}

void LteRrcPagingSimUpdateImsiStmsi::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingSimUpdateNoId);

void LteRrcPagingSimUpdateNoId::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingSimUpdateNoId::Test()
{
  lte_rrc_sim_update_req_s sim_update_req;
  lte_rrc_camped_indi_s camped_indi;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  uint8 imsi_digits[21] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0, 0x1, 0x2, 
                           0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0, 0x1};
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // ----------------------------------------------------------------------
  // Send a camped indication, Paging should move to the idle_camped state
  // We test only in idle_camped state, assuming that the handling for connected state
  // is exactly the same.

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

  // Send the sim_update request in the idle_camped state
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);

  // Send no Ids
  sim_update_req.security_info_is_valid = FALSE;
  sim_update_req.ue_id_list_is_valid = TRUE;
  sim_update_req.ue_id_list.num_ue_identities = 0;
  sim_update_req.ue_id_list.ue_id[0].type = LTE_RRC_UE_ID_IMSI;
  sim_update_req.ue_id_list.ue_id[0].value.imsi.num_digits = sizeof(imsi_digits);
  memcpy(sim_update_req.ue_id_list.ue_id[0].value.imsi.imsi_digits,
         imsi_digits,
         sizeof(sim_update_req.ue_id_list.ue_id[0].value.imsi.imsi_digits));
  sim_update_req.access_class = 1 << 3;

  lte_rrc_utf_send_msg((byte*)&sim_update_req, sizeof(sim_update_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We don't expect Paging sm to have thrown a idle drx cfg_req
  // since IMSI was not updated
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
}

void LteRrcPagingSimUpdateNoId::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingSimUpdateImsiHoInProgress);

void LteRrcPagingSimUpdateImsiHoInProgress::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingSimUpdateImsiHoInProgress::Test()
{
  lte_rrc_sim_update_req_s sim_update_req;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_connected_indi_s connected_indi;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  uint8 imsi_digits[21] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0, 0x1, 0x2, 
                           0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0, 0x1};
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // ----------------------------------------------------------------------
  // Send a camped indication, Paging should move to the idle_camped state
  // We test only in idle_camped state, assuming that the handling for connected state
  // is exactly the same.

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

    // On receiving a connected indication, we should move to the connected state
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();
  
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  // HO is in progress
  lte_rrc_paging_test_config_ho_in_progress = TRUE;

  // Send the sim_update request in the idle_camped state
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);

  // Access private data of paging, and init the two paging cycles to a high value
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;
  lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_T2;

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

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  // cfg_req should not be thrown as HO is in progress, but private data should be updated
  TF_ASSERT(memcmp(&sim_update_req.ue_id_list.ue_id[0], 
                   &lte_rrc_paging.sd_ptr->ue_id[LTE_RRC_UE_ID_IMSI],
                   sizeof(lte_rrc_paging.sd_ptr->ue_id[0])) == 0);
  lte_rrc_utf_check_no_more_msgs();
}

void LteRrcPagingSimUpdateImsiHoInProgress::Teardown()
{
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingSimUpdateImsiCreInProgress);

void LteRrcPagingSimUpdateImsiCreInProgress::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingSimUpdateImsiCreInProgress::Test()
{
  lte_rrc_sim_update_req_s sim_update_req;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_connected_indi_s connected_indi;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  uint8 imsi_digits[21] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0, 0x1, 0x2, 
                           0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0, 0x1};
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // ----------------------------------------------------------------------
  // Send a camped indication, Paging should move to the idle_camped state
  // We test only in idle_camped state, assuming that the handling for connected state
  // is exactly the same.

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

    // On receiving a connected indication, we should move to the connected state
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();
  
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  // RLF search is in progress
  lte_rrc_paging_test_cre_rlf_search_in_progress = TRUE;

  // Send the sim_update request in the idle_camped state
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);

  // Access private data of paging, and init the two paging cycles to a high value
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;
  lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_T2;

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

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  // cfg_req should not be thrown as RLF search is in progress, but private data should be updated
  TF_ASSERT(memcmp(&sim_update_req.ue_id_list.ue_id[0], 
                   &lte_rrc_paging.sd_ptr->ue_id[LTE_RRC_UE_ID_IMSI],
                   sizeof(lte_rrc_paging.sd_ptr->ue_id[0])) == 0);
  lte_rrc_utf_check_no_more_msgs();
}

void LteRrcPagingSimUpdateImsiCreInProgress::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingSimUpdatePendingQueue);

void LteRrcPagingSimUpdatePendingQueue::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingSimUpdatePendingQueue::Test()
{
  lte_rrc_sim_update_req_s sim_update_req;
  lte_rrc_camped_indi_s camped_indi;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  uint8 imsi_digits[21] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0, 0x1, 0x2, 
                           0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0, 0x1};
  
  // Initialize the proto stubs
  lte_rrc_paging_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the initial state
  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__INITIAL);

  // ----------------------------------------------------------------------
  // Send a camped indication, Paging should move to the idle_camped state
  // We test only in idle_camped state, assuming that the handling for connected state
  // is exactly the same.

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

  // Send the sim_update request in the idle_camped state
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);

  // Access private data of paging, and init the two paging cycles to a high value
  // lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;
  // lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_T2;

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

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We don't expect Paging sm to have thrown a idle drx cfg_req
  // since IMSI was not updated
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // Put controller in suspended state
  // Controller suspended, priority lower
  lte_rrc_paging_test_controller_state = LTE_RRC_CONTROLLER_SM__SUSPENDED;
  lte_rrc_paging_test_revoke_priority = LTE_RRC_REVOKE_PRIORITY_NOT_REQD;

  // verify pending queue count is 0
  //TF_ASSERT(q_cnt(&lte_rrc_pendq) == 0);
  //TF_ASSERT(lte_rrc_pendq.free_queue.cnt == LTE_RRC_PENDQ_MAX_QUEUE_LEN);


  // Send 10 SIM update requests and verify pending queue count is always 1
  
  for(int i = 0; i < 10; i++)
  {
    // Send the sim_update request in the idle_camped state
    lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);

    // Access private data of paging, and init the two paging cycles to a high value
    // lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;
    // lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_T2;

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

    // verify pending queue count is 1
    //TF_ASSERT(q_cnt(&lte_rrc_pendq) == 1);
    //TF_ASSERT(lte_rrc_pendq.free_queue.cnt == LTE_RRC_PENDQ_MAX_QUEUE_LEN - 1);
  }
 
  lte_rrc_paging_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;

  // Send the sim_update request in the idle_camped state
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);
  sim_update_req.security_info_is_valid = FALSE;
  sim_update_req.ue_id_list_is_valid = TRUE;
  sim_update_req.ue_id_list.num_ue_identities = 0;
  sim_update_req.ue_id_list.ue_id[0].type = LTE_RRC_UE_ID_IMSI;
  sim_update_req.ue_id_list.ue_id[0].value.imsi.num_digits = sizeof(imsi_digits);
  memcpy(sim_update_req.ue_id_list.ue_id[0].value.imsi.imsi_digits,
         imsi_digits,
         sizeof(sim_update_req.ue_id_list.ue_id[0].value.imsi.imsi_digits));
  sim_update_req.access_class = 1 << 3;

  lte_rrc_utf_send_msg((byte*)&sim_update_req, sizeof(sim_update_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // verify pending queue count is 0
  //TF_ASSERT(q_cnt(&lte_rrc_pendq) == 0);
  //TF_ASSERT(lte_rrc_pendq.free_queue.cnt == LTE_RRC_PENDQ_MAX_QUEUE_LEN);
}

void LteRrcPagingSimUpdatePendingQueue::Teardown()
{
  lte_rrc_utf_teardown();
}