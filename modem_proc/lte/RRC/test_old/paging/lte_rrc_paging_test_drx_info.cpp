/*!
  @file lte_rrc_paging_test_drx_info.cpp

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/paging/lte_rrc_paging_test_drx_info.cpp#1 $

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
#include "__lte_rrc_controller.h"
#include "__lte_rrc_cre.h"
#include "lte_rrc_paging.h"
#include "lte_rrc_pagingi.h"
#include "lte_rrc_osys_asn1util.h"
#include <fs_public.h>
}

#include <tf_stub.h>
#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_paging_test_msg.h"
#include <qtf_efs.h>
#include <fs_lib.h>

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module lte_rrc_paging_test_drx_info.cpp
*/
typedef struct
{
  int   internal_var;  /*!< My internal variable */
  void *internal_ptr;  /*!< My internal pointer */

} lte_rrc_paging_test_drx_info_s;


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_paging_test_drx_info.cpp
*/
extern "C"
{
  extern lte_rrc_paging_s lte_rrc_paging;
}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern void lte_rrc_paging_test_init(void);
extern boolean lte_rrc_paging_test_config_ho_in_progress;
extern stm_state_t lte_rrc_paging_test_controller_state;
extern lte_rrc_revoke_priority_e lte_rrc_paging_test_revoke_priority;
extern boolean lte_rrc_paging_test_cre_rlf_search_in_progress;
extern void lte_rrc_paging_test_send_get_sibs_reqi();

/*===========================================================================

                                FUNCTIONS

===========================================================================*/


/*===========================================================================

  FUNCTION:  lte_rrc_paging_test_efs_reduce_page_cycle

===========================================================================*/
/*!
    @brief
    This function sets reduce_page_cycle to true
    
    @return
    None

*/
/*=========================================================================*/
void lte_rrc_paging_test_efs_reduce_page_cycle
(
  uint8 half_drx_cfg_valid
)
{
  int oflag, mode, efs_ret;
  uint8 test;
  
  /* Init EFS */
  qtf_efs_initialize("half_drx_cfg_valid");
	
  oflag = O_RDWR | O_AUTODIR | O_TRUNC;
  mode = ACCESSPERMS;

  TF_ASSERT(efs_put("/nv/item_files/modem/lte/rrc/page/half_drx_cfg_valid", 
					&half_drx_cfg_valid, sizeof(half_drx_cfg_valid), oflag, mode) >= 0);

  /* Read the first byte to get the size of sib 1*/
  TF_ASSERT((efs_ret = efs_open("/nv/item_files/modem/lte/rrc/page/reduce_page_cycle", O_RDONLY)) != -1);
  TF_ASSERT(efs_read(efs_ret, &test, sizeof(test))!= -1);
  TF_ASSERT(efs_close(efs_ret) != -1);
}


/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDrxValidCycle);

void LteRrcPagingDrxValidCycle::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingDrxValidCycle::Test()
{
  lte_rrc_drx_info_req_s drx_info_req;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  lte_rrc_camped_indi_s camped_indi;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  
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

  // Send the drx request
  lte_rrc_init_default_hdr(&drx_info_req.msg_hdr, LTE_RRC_DRX_INFO_REQ);

  // Set the ue-specific paging cycle
  drx_info_req.paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T32;

  // Access the private data of paging, and set default paging cycle to something higher
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T64;
	lte_rrc_paging.sd_ptr->specific_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;
  lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_T2;
  
  lte_rrc_utf_send_msg((byte*)&drx_info_req, sizeof(drx_info_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);

  TF_ASSERT(cfg_req->idle_drx_param.paging_cycle == 320);
}


void LteRrcPagingDrxValidCycle::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDrxValidCycle2);

void LteRrcPagingDrxValidCycle2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingDrxValidCycle2::Test()
{
  lte_rrc_drx_info_req_s drx_info_req;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  lte_rrc_camped_indi_s camped_indi;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  
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

  // Send the drx request
  lte_rrc_init_default_hdr(&drx_info_req.msg_hdr, LTE_RRC_DRX_INFO_REQ);

  // Set the ue-specific paging cycle
  drx_info_req.paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T32;

  // Access the private data of paging, and set default paging cycle to something higher
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;
	lte_rrc_paging.sd_ptr->specific_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T64;
  lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_T2;
  
  lte_rrc_utf_send_msg((byte*)&drx_info_req, sizeof(drx_info_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);

  TF_ASSERT(cfg_req->idle_drx_param.paging_cycle == 320);
}


void LteRrcPagingDrxValidCycle2::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDrxValidCycle3);

void LteRrcPagingDrxValidCycle3::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingDrxValidCycle3::Test()
{
  lte_rrc_drx_info_req_s drx_info_req;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  lte_rrc_camped_indi_s camped_indi;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  
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

  // Send the drx request
  lte_rrc_init_default_hdr(&drx_info_req.msg_hdr, LTE_RRC_DRX_INFO_REQ);

  // Set the ue-specific paging cycle
  drx_info_req.paging_cycle = LTE_RRC_DRX_CN_NOT_SPECIFIED;

  // Access the private data of paging, and set default paging cycle to something higher
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;
	lte_rrc_paging.sd_ptr->specific_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T64;
  lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_T2;
  
  lte_rrc_utf_send_msg((byte*)&drx_info_req, sizeof(drx_info_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);

  TF_ASSERT(cfg_req->idle_drx_param.paging_cycle == 1280);
}


void LteRrcPagingDrxValidCycle3::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDrxValidCycle4);

void LteRrcPagingDrxValidCycle4::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingDrxValidCycle4::Test()
{
  lte_rrc_drx_info_req_s drx_info_req;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  lte_rrc_camped_indi_s camped_indi;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  
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

  // Send the drx request
  lte_rrc_init_default_hdr(&drx_info_req.msg_hdr, LTE_RRC_DRX_INFO_REQ);

  // Set the ue-specific paging cycle
  drx_info_req.paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T64;

  // Access the private data of paging, and set default paging cycle to something higher
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;
	lte_rrc_paging.sd_ptr->specific_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T32;
  lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_T2;
  
  lte_rrc_utf_send_msg((byte*)&drx_info_req, sizeof(drx_info_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);

  TF_ASSERT(cfg_req->idle_drx_param.paging_cycle == 640);
}


void LteRrcPagingDrxValidCycle4::Teardown()
{
  lte_rrc_utf_teardown();
}

/***

Commenting out because TF does not have the ability currently to catch exceptions
throw from another thread

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDrxInvalidCycle);

void LteRrcPagingDrxInvalidCycle::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingDrxInvalidCycle::Test()
{
  lte_rrc_drx_info_req_s drx_info_req;
  lte_rrc_cfg_reqi_s *cfg_req;
  uint32 buf_len;
  lte_rrc_camped_indi_s camped_indi;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  
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

  // Send the drx request
  lte_rrc_init_default_hdr(&drx_info_req.msg_hdr, LTE_RRC_DRX_INFO_REQ);

  // Set the ue-specific paging cycle to something invalid
  drx_info_req.paging_cycle = (lte_rrc_paging_cycle_e) 21;

  // Send the message expecting to catch an assert
  TF_CATCH_FATAL
  (
    lte_rrc_utf_send_msg((byte*)&drx_info_req, sizeof(drx_info_req))
  );

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We expect Paging sm to have not throw anything
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
}


void LteRrcPagingDrxInvalidCycle::Teardown()
{
  lte_rrc_utf_teardown();
}

****/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDrxHigherCycle);

void LteRrcPagingDrxHigherCycle::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingDrxHigherCycle::Test()
{
  lte_rrc_drx_info_req_s drx_info_req;
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

  // Send the drx request
  lte_rrc_init_default_hdr(&drx_info_req.msg_hdr, LTE_RRC_DRX_INFO_REQ);

  // Set the ue-specific paging cycle
  drx_info_req.paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;

  // Access the private data of paging, and set default paging cycle to something lower
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T64;
	lte_rrc_paging.sd_ptr->specific_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;
  
  lte_rrc_utf_send_msg((byte*)&drx_info_req, sizeof(drx_info_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We don't expect Paging sm to have thrown a cfg_reqi
  // because the new ue-specific cycle is higher than the default paging cycle
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
}


void LteRrcPagingDrxHigherCycle::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDrxHigherCycle2);

void LteRrcPagingDrxHigherCycle2::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingDrxHigherCycle2::Test()
{
  lte_rrc_drx_info_req_s drx_info_req;
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

  // Send the drx request
  lte_rrc_init_default_hdr(&drx_info_req.msg_hdr, LTE_RRC_DRX_INFO_REQ);

  // Set the ue-specific paging cycle
  drx_info_req.paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;

  // Access the private data of paging, and set default paging cycle to something lower
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T64;
	lte_rrc_paging.sd_ptr->specific_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T32;
  
  lte_rrc_utf_send_msg((byte*)&drx_info_req, sizeof(drx_info_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We don't expect Paging sm to have thrown a cfg_reqi
  // because the new ue-specific cycle is higher than the default paging cycle
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
}


void LteRrcPagingDrxHigherCycle2::Teardown()
{
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDrxEqualCycle);

void LteRrcPagingDrxEqualCycle::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingDrxEqualCycle::Test()
{
  lte_rrc_drx_info_req_s drx_info_req;
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

  // Send the drx request
  lte_rrc_init_default_hdr(&drx_info_req.msg_hdr, LTE_RRC_DRX_INFO_REQ);

  // Set the ue-specific paging cycle
  drx_info_req.paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;

  // Access the private data of paging, and set default paging cycle to exactly the same
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;
	lte_rrc_paging.sd_ptr->specific_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;
  
  lte_rrc_utf_send_msg((byte*)&drx_info_req, sizeof(drx_info_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We don't expect Paging sm to have thrown a cfg_reqi
  // because the new ue-specific cycle is higher than the default paging cycle
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
}


void LteRrcPagingDrxEqualCycle::Teardown()
{
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDrxHoInProgress);

void LteRrcPagingDrxHoInProgress::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingDrxHoInProgress::Test()
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

  // ----------------------------------------------------------------------
  // drx_info_req is received while a HO is in progress; cfg_req should not be 
  // sent.

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

  // Send the drx request
  lte_rrc_init_default_hdr(&drx_info_req.msg_hdr, LTE_RRC_DRX_INFO_REQ);

  // Set the ue-specific paging cycle
  drx_info_req.paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T32;

  // Access the private data of paging, and set default paging cycle to something higher
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T64;
  lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_T2;
  
  lte_rrc_utf_send_msg((byte*)&drx_info_req, sizeof(drx_info_req));

  lte_rrc_utf_wait_for_done();

  // The private data of paging should have been updated but cfg_req should not be sent
  TF_ASSERT(lte_rrc_paging.sd_ptr->specific_paging_cycle == LTE_RRC_DRX_CN_COEFFICIENT_S1_T32);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  lte_rrc_utf_check_no_more_msgs();
}


void LteRrcPagingDrxHoInProgress::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDrxValidCycleSuspended);

void LteRrcPagingDrxValidCycleSuspended::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingDrxValidCycleSuspended::Test()
{
  lte_rrc_drx_info_req_s drx_info_req, *drx_info_req_ptr;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  lte_rrc_camped_indi_s camped_indi;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  
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

  // Send the drx request
  lte_rrc_init_default_hdr(&drx_info_req.msg_hdr, LTE_RRC_DRX_INFO_REQ);

  // Set the ue-specific paging cycle
  drx_info_req.paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T32;

  // Controller suspended, priority lower
  lte_rrc_paging_test_controller_state = LTE_RRC_CONTROLLER_SM__SUSPENDED;
  lte_rrc_paging_test_revoke_priority = LTE_RRC_REVOKE_PRIORITY_NOT_REQD;

  // Access the private data of paging, and set default paging cycle to something higher
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T64;
  lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_T2;
  
  lte_rrc_utf_send_msg((byte*)&drx_info_req, sizeof(drx_info_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Message should have been added to pending queue - no msg should be sent
  lte_rrc_utf_check_no_more_msgs();

  // ue-specific paging cycle shouldn't have been changed
  TF_ASSERT(lte_rrc_paging.sd_ptr->specific_paging_cycle == LTE_RRC_DRX_CN_NOT_SPECIFIED);

  lte_rrc_paging_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;

  // Send a message that Paging does not process
  lte_rrc_paging_test_send_get_sibs_reqi();

  // Pended msg should be dequeued
  lte_rrc_utf_get_next_int_msg((byte**)&drx_info_req_ptr, &buf_len);
  TF_ASSERT(drx_info_req_ptr->msg_hdr.id == LTE_RRC_DRX_INFO_REQ);

  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);

  TF_ASSERT(cfg_req->idle_drx_param.paging_cycle == 320);
}


void LteRrcPagingDrxValidCycleSuspended::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDrxValidCycleSuspendedRevoke);

void LteRrcPagingDrxValidCycleSuspendedRevoke::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingDrxValidCycleSuspendedRevoke::Test()
{
  lte_rrc_drx_info_req_s drx_info_req, *drx_info_req_ptr;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  lte_rrc_camped_indi_s camped_indi;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_ml1_revoke_reqi_s *revoke_reqi_ptr;
  
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

  // Send the drx request
  lte_rrc_init_default_hdr(&drx_info_req.msg_hdr, LTE_RRC_DRX_INFO_REQ);

  // Set the ue-specific paging cycle
  drx_info_req.paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T32;

  // Controller suspended, priority lower
  lte_rrc_paging_test_controller_state = LTE_RRC_CONTROLLER_SM__SUSPENDED;
  lte_rrc_paging_test_revoke_priority = LTE_RRC_REVOKE_PRIORITY_NOW;

  // Access the private data of paging, and set default paging cycle to something higher
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T64;
  lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_T2;
  
  lte_rrc_utf_send_msg((byte*)&drx_info_req, sizeof(drx_info_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Message should have been added to pending queue and revoke_req should be sent
  lte_rrc_utf_get_next_int_msg((byte**)&revoke_reqi_ptr, &buf_len);
  TF_ASSERT(revoke_reqi_ptr->msg_hdr.id == LTE_RRC_ML1_REVOKE_REQI);

  lte_rrc_utf_check_no_more_msgs();

  // ue-specific paging cycle shouldn't have been changed
  TF_ASSERT(lte_rrc_paging.sd_ptr->specific_paging_cycle == LTE_RRC_DRX_CN_NOT_SPECIFIED);

  lte_rrc_paging_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;

  // Send a message that Paging does not process
  lte_rrc_paging_test_send_get_sibs_reqi();

  // Pended msg should be dequeued
  lte_rrc_utf_get_next_int_msg((byte**)&drx_info_req_ptr, &buf_len);
  TF_ASSERT(drx_info_req_ptr->msg_hdr.id == LTE_RRC_DRX_INFO_REQ);

  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);

  TF_ASSERT(cfg_req->idle_drx_param.paging_cycle == 320);
}


void LteRrcPagingDrxValidCycleSuspendedRevoke::Teardown()
{
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingDrxCreInProgress);

void LteRrcPagingDrxCreInProgress::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingDrxCreInProgress::Test()
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

  // ----------------------------------------------------------------------
  // drx_info_req is received while a HO is in progress; cfg_req should not be 
  // sent.

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

  // Send the drx request
  lte_rrc_init_default_hdr(&drx_info_req.msg_hdr, LTE_RRC_DRX_INFO_REQ);

  // Set the ue-specific paging cycle
  drx_info_req.paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T32;

  // Access the private data of paging, and set default paging cycle to something higher
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T64;
  lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_T2;
  
  lte_rrc_utf_send_msg((byte*)&drx_info_req, sizeof(drx_info_req));

  lte_rrc_utf_wait_for_done();

  // The private data of paging should have been updated but cfg_req should not be sent
  TF_ASSERT(lte_rrc_paging.sd_ptr->specific_paging_cycle == LTE_RRC_DRX_CN_COEFFICIENT_S1_T32);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__CONNECTED);

  lte_rrc_utf_check_no_more_msgs();
}

void LteRrcPagingDrxCreInProgress::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcPagingReduceDrx);

void LteRrcPagingReduceDrx::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_DRX_CFG_REQ
    };
  lte_rrc_paging_test_efs_reduce_page_cycle(1);
  lte_rrc_utf_setup(LTE_RRC_PAGING_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcPagingReduceDrx::Test()
{
  lte_rrc_drx_info_req_s drx_info_req;
  lte_cphy_idle_drx_cfg_req_s *cfg_req;
  uint32 buf_len;
  lte_rrc_camped_indi_s camped_indi;
  uint8 int_queue_count;
  uint8 ext_queue_count;
  
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
  TF_ASSERT(cfg_req->idle_drx_param.paging_cycle == 640);
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // Send the drx request
  lte_rrc_init_default_hdr(&drx_info_req.msg_hdr, LTE_RRC_DRX_INFO_REQ);

  // Set the ue-specific paging cycle
  drx_info_req.paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T32;

  // Access the private data of paging, and set default paging cycle to something higher
  lte_rrc_paging.sd_ptr->default_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T64;
	lte_rrc_paging.sd_ptr->specific_paging_cycle = LTE_RRC_DRX_CN_COEFFICIENT_S1_T128;
  lte_rrc_paging.dd_ptr->nb = LTE_RRC_NB_T2;
  
  lte_rrc_utf_send_msg((byte*)&drx_info_req, sizeof(drx_info_req));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_PAGING_SM) == LTE_RRC_PAGING_SM__IDLE_CAMPED);

  // We expect Paging sm to have thrown a idle drx cfg_reqi
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_drx_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_DRX_CFG_REQ);

  TF_ASSERT(cfg_req->idle_drx_param.paging_cycle == 320);
}


void LteRrcPagingReduceDrx::Teardown()
{
  lte_rrc_utf_teardown();
}

