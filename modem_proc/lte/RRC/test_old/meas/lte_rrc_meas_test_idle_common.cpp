/*!
  @file lte_rrc_meas_test_idle_common.cpp

  @brief
  Unit test code for the meas procedure.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/meas/lte_rrc_meas_test_idle_common.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/10   svk     Modified the name from UTRA to WCDMA
08/12/09   sk      March 09 spec migration for SIB4,SIB4,SIB5 and conn meas
04/03/09   sk      Fixed warnings
03/31/09   sk      Added support for Generic Error handling as per spec
01/23/09   ask     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "lte_static_rrc_mem_util.h"
#include "__lte_rrc_meas.h"
//#include "lte_rrc_meas.h"
#include "lte_rrc_measi.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_rrc_csp.h"
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_meas_test_msg.h"
#include <TestFramework.h>
#include <tf_stub.h>

extern "C"
{
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s *);
  TF_PROTO_STUB(lte_3gpp_release_ver_e, lte_rrc_get_3gpp_release_version, void);
}

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module lte_rrc_meas_test_idle_common.cpp
*/
DECLARE_LTE_RRC_MEAS_TEST_SIB3_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_SIB4_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_SIB5_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_SIB6_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_SIB7_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_SIB8_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_SIB8_MSG2;
DECLARE_LTE_RRC_MEAS_TEST_REL9_RSRQ_FIELDS_PRESENT_SIB3_MSG;
DECLARE_LTE_RRC_MEAS_TEST_REL9_RSRQ_FIELDS_PRESENT_SIB5_MSG;
DECLARE_LTE_RRC_MEAS_TEST_REL9_RSRQ_FIELDS_PRESENT_SIB6_MSG;

typedef struct
{
  int   internal_var;  /*!< My internal variable */
  void *internal_ptr;  /*!< My internal pointer */

} lte_rrc_meas_test_idle_common_s;


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_meas_test_idle_common.cpp
*/
extern "C"
{
  extern lte_rrc_meas_s lte_rrc_meas_data;
}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                                FUNCTIONS

===========================================================================*/



/*===========================================================================

                                UNIT TEST

===========================================================================*/

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselCommonQHyst);

void LteRrcMeasReselCommonQHyst::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselCommonQHyst::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst test_values[] = 
  {
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB0,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB1,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB2,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB3,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB4,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB5,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB6,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB8,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB10,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB12,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB14,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB16,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB18,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB20,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB22,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB24,
  };
  int i;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // We iterate over q_hyst values, leaving everything else the same
  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    lte_rrc_meas_test_sib3->cellReselectionInfoCommon.q_Hyst = test_values[i];

    // Send sib updated indi to the meas sm
    lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

    // Just indicate that SIB3 and SIB4 were updated
    sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
    sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK |
		                              LTE_RRC_SIB5_BITMASK;
    sib_updated_indi.cell_id.phy_cell_id = 0x10;
    sib_updated_indi.cell_id.freq = 0x100;

    lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

    lte_rrc_utf_wait_for_done();

    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

    // We expect meas sm to have thrown the idle meas cfg req
    lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
    TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

    // Verify cell info
    TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
    TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

    // Verify reselection common info
    TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));

    TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

    switch (test_values[i])
    {
      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB0:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 0);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB1:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB2:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 2);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB3:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 3);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB4:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 4);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB5:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 5);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB6:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 6);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB8:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 8);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB10:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 10);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB12:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 12);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB14:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 14);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB16:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 16);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB18:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 18);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB20:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 20);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB22:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 22);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst_dB24:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 24);
        break;

    }

    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);


    // Verify serving freq cfg
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

    // Verify intra freq cfg
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);


    // Send the cnf msg back to meas
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the cfg cnf
    lte_rrc_utf_wait_for_done();

    lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
    TF_ASSERT(int_queue_count == 0);
    TF_ASSERT(ext_queue_count == 0);

    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  }
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasReselCommonQHyst::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselCommonQHystInvalid);

void LteRrcMeasReselCommonQHystInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselCommonQHystInvalid::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // We set q_hyst to an invalid value
  lte_rrc_meas_test_sib3->cellReselectionInfoCommon.q_Hyst = (lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_q_Hyst) 16;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK |
		                            LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  // We expect meas to reject the SIBs and not send any config to ML1
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();
  
}

void LteRrcMeasReselCommonQHystInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselCommonTEval);

void LteRrcMeasReselCommonTEval::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselCommonTEval::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  lte_rrc_osys_MobilityStateParameters_t_Evaluation test_values[] = 
  {
    lte_rrc_osys_MobilityStateParameters_t_Evaluation_s30,
    lte_rrc_osys_MobilityStateParameters_t_Evaluation_s60,
    lte_rrc_osys_MobilityStateParameters_t_Evaluation_s120,
    lte_rrc_osys_MobilityStateParameters_t_Evaluation_s180,
    lte_rrc_osys_MobilityStateParameters_t_Evaluation_s240,
  };
  int i;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // We iterate over t_eval values, leaving everything else the same
  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    lte_rrc_meas_test_sib3->cellReselectionInfoCommon.speedStateReselectionPars.mobilityStateParameters.t_Evaluation 
      = test_values[i];

    // Send sib updated indi to the meas sm
    lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

    // Just indicate that SIB3 and SIB4 were updated
    sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
    sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK |
		                              LTE_RRC_SIB5_BITMASK;
    sib_updated_indi.cell_id.phy_cell_id = 0x10;
    sib_updated_indi.cell_id.freq = 0x100;

    lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

    lte_rrc_utf_wait_for_done();

    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

    // We expect meas sm to have thrown the idle meas cfg req
    lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
    TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

    // Verify cell info
    TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
    TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

    // Verify reselection common info
    TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
    TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
    
    switch (lte_rrc_meas_test_sib3->cellReselectionInfoCommon.speedStateReselectionPars.mobilityStateParameters.t_Evaluation)
    {
      case lte_rrc_osys_MobilityStateParameters_t_Evaluation_s30:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 30);
        break;

      case lte_rrc_osys_MobilityStateParameters_t_Evaluation_s60:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
        break;

      case lte_rrc_osys_MobilityStateParameters_t_Evaluation_s120:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 120);
        break;

      case lte_rrc_osys_MobilityStateParameters_t_Evaluation_s180:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 180);
        break;

      case lte_rrc_osys_MobilityStateParameters_t_Evaluation_s240:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 240);
        break;

      default:
        TF_ASSERT(0);
        break;
    }

    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

    // Verify serving freq cfg
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

    // Verify intra freq cfg
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);

    // Send the cnf msg back to meas
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the cfg cnf
    lte_rrc_utf_wait_for_done();

    lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
    TF_ASSERT(int_queue_count == 0);
    TF_ASSERT(ext_queue_count == 0);

    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  }
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasReselCommonTEval::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselCommonTEvalInvalid);

void LteRrcMeasReselCommonTEvalInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselCommonTEvalInvalid::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // We set t_eval to an invalid value
  lte_rrc_meas_test_sib3->cellReselectionInfoCommon.speedStateReselectionPars.mobilityStateParameters.t_Evaluation 
    = (lte_rrc_osys_MobilityStateParameters_t_Evaluation) 8;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  
  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK |
		                            LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  
  lte_rrc_utf_wait_for_done();
  
  // We expect meas sm to detect the error and not send any config to ML1
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();
  
}

void LteRrcMeasReselCommonTEvalInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselCommonTEvalSpare);

void LteRrcMeasReselCommonTEvalSpare::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselCommonTEvalSpare::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  uint32 buf_len;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // We set t_eval to an invalid value
  lte_rrc_meas_test_sib3->cellReselectionInfoCommon.speedStateReselectionPars.mobilityStateParameters.t_Evaluation 
    = lte_rrc_osys_MobilityStateParameters_t_Evaluation_spare1;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  
  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK |
		                            LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  
  lte_rrc_utf_wait_for_done();
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify reselection common info
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == FALSE);

  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify intra freq cfg
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);


  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();
  
}

void LteRrcMeasReselCommonTEvalSpare::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselCommonTHystNormal);

void LteRrcMeasReselCommonTHystNormal::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselCommonTHystNormal::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  lte_rrc_osys_MobilityStateParameters_t_HystNormal test_values[] = 
  {
    lte_rrc_osys_MobilityStateParameters_t_HystNormal_s30,
    lte_rrc_osys_MobilityStateParameters_t_HystNormal_s60,
    lte_rrc_osys_MobilityStateParameters_t_HystNormal_s120,
    lte_rrc_osys_MobilityStateParameters_t_HystNormal_s180,
    lte_rrc_osys_MobilityStateParameters_t_HystNormal_s240,
  };
  int i;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // We iterate over t_eval values, leaving everything else the same
  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    lte_rrc_meas_test_sib3->cellReselectionInfoCommon.speedStateReselectionPars.mobilityStateParameters.t_HystNormal
      = test_values[i];

    // Send sib updated indi to the meas sm
    lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

    // Just indicate that SIB3 and SIB4 were updated
    sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
    sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK |
		                              LTE_RRC_SIB5_BITMASK;
    sib_updated_indi.cell_id.phy_cell_id = 0x10;
    sib_updated_indi.cell_id.freq = 0x100;

    lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

    lte_rrc_utf_wait_for_done();

    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

    // We expect meas sm to have thrown the idle meas cfg req
    lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
    TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

    // Verify cell info
    TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
    TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

    // Verify reselection common info
    TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
    TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);

    switch (lte_rrc_meas_test_sib3->cellReselectionInfoCommon.speedStateReselectionPars.mobilityStateParameters.t_HystNormal)
    {
      case lte_rrc_osys_MobilityStateParameters_t_HystNormal_s30:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 30);
        break;

      case lte_rrc_osys_MobilityStateParameters_t_HystNormal_s60:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
        break;

      case lte_rrc_osys_MobilityStateParameters_t_HystNormal_s120:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 120);
        break;

      case lte_rrc_osys_MobilityStateParameters_t_HystNormal_s180:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 180);
        break;

      case lte_rrc_osys_MobilityStateParameters_t_HystNormal_s240:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 240);
        break;

      default:
        TF_ASSERT(0);
        break;
    }

    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

    // Verify serving freq cfg
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

    // Verify intra freq cfg
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);

    // Send the cnf msg back to meas
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the cfg cnf
    lte_rrc_utf_wait_for_done();

    lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
    TF_ASSERT(int_queue_count == 0);
    TF_ASSERT(ext_queue_count == 0);

    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  }
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasReselCommonTHystNormal::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselCommonTHystNormalInvalid);

void LteRrcMeasReselCommonTHystNormalInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselCommonTHystNormalInvalid::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // We set t_hyst_normal to an invalid value
  lte_rrc_meas_test_sib3->cellReselectionInfoCommon.speedStateReselectionPars.mobilityStateParameters.t_HystNormal 
    = (lte_rrc_osys_MobilityStateParameters_t_HystNormal) 8;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  
  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK |
		                            LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  
  lte_rrc_utf_wait_for_done();
  
  // We expect meas sm to detect the error and not send any config to ML1
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasReselCommonTHystNormalInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselCommonTHystNormalSpare);

void LteRrcMeasReselCommonTHystNormalSpare::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselCommonTHystNormalSpare::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  uint32 buf_len;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // We set t_hyst_normal to an invalid value
  lte_rrc_meas_test_sib3->cellReselectionInfoCommon.speedStateReselectionPars.mobilityStateParameters.t_HystNormal 
    = lte_rrc_osys_MobilityStateParameters_t_HystNormal_spare1;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  
  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK |
		                            LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  
  lte_rrc_utf_wait_for_done();
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify reselection common info
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));

  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == FALSE);

  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify intra freq cfg
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);


  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasReselCommonTHystNormalSpare::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselCommonNCellChangeMediumInvalid);

void LteRrcMeasReselCommonNCellChangeMediumInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselCommonNCellChangeMediumInvalid::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // We set n_cell_change_medium to an invalid value
  lte_rrc_meas_test_sib3->cellReselectionInfoCommon.speedStateReselectionPars.mobilityStateParameters.n_CellChangeMedium
    = 17;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  
  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK |
		                            LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  
  lte_rrc_utf_wait_for_done();
  
  // We expect meas sm to detect the error and not send any config to ML1
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasReselCommonNCellChangeMediumInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselCommonNCellChangeHighInvalid);

void LteRrcMeasReselCommonNCellChangeHighInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselCommonNCellChangeHighInvalid::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // We set n_cell_change_high to an invalid value
  lte_rrc_meas_test_sib3->cellReselectionInfoCommon.speedStateReselectionPars.mobilityStateParameters.n_CellChangeHigh
    = 17;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  
  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK |
		                            LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  
  lte_rrc_utf_wait_for_done();
  
  // We expect meas sm to detect the error and not send any config to ML1
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasReselCommonNCellChangeHighInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}



TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselCommonQHystMedium);

void LteRrcMeasReselCommonQHystMedium::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselCommonQHystMedium::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_Medium test_values[] = 
  {
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_Medium_dB_6,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_Medium_dB_4,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_Medium_dB_2,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_Medium_dB0
  };
  int i;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // We iterate over q_hyst_medium values, leaving everything else the same
  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    lte_rrc_meas_test_sib3->cellReselectionInfoCommon.speedStateReselectionPars.q_HystSF.sf_Medium
      = test_values[i];

    // Send sib updated indi to the meas sm
    lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

    // Just indicate that SIB3 and SIB4 were updated
    sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
    sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK |
		                              LTE_RRC_SIB5_BITMASK;
    sib_updated_indi.cell_id.phy_cell_id = 0x10;
    sib_updated_indi.cell_id.freq = 0x100;

    lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

    lte_rrc_utf_wait_for_done();

    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

    // We expect meas sm to have thrown the idle meas cfg req
    lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
    TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

    // Verify cell info
    TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
    TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

    // Verify reselection common info
    TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
    TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);

    switch (lte_rrc_meas_test_sib3->cellReselectionInfoCommon.speedStateReselectionPars.q_HystSF.sf_Medium)
    {
      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_Medium_dB_6:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_Medium_dB_4:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -4);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_Medium_dB_2:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -2);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_Medium_dB0:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == 0);
        break;
    }

    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

    // Verify serving freq cfg
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

    // Verify intra freq cfg
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);

    // Send the cnf msg back to meas
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the cfg cnf
    lte_rrc_utf_wait_for_done();

    lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
    TF_ASSERT(int_queue_count == 0);
    TF_ASSERT(ext_queue_count == 0);

    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  }
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasReselCommonQHystMedium::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselCommonQHystMediumInvalid);

void LteRrcMeasReselCommonQHystMediumInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselCommonQHystMediumInvalid::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // We set q_hyst_medium to an invalid value
  lte_rrc_meas_test_sib3->cellReselectionInfoCommon.speedStateReselectionPars.q_HystSF.sf_Medium
      = (lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_Medium) 4;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  
  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK |
		                            LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  
  lte_rrc_utf_wait_for_done();
  
  // We expect meas sm to detect the error and not send any config to ML1
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasReselCommonQHystMediumInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselCommonQHystHigh);

void LteRrcMeasReselCommonQHystHigh::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselCommonQHystHigh::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_High test_values[] = 
  {
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_High_dB_6,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_High_dB_4,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_High_dB_2,
    lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_High_dB0
  };
  int i;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // We iterate over q_hyst_high values, leaving everything else the same
  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    lte_rrc_meas_test_sib3->cellReselectionInfoCommon.speedStateReselectionPars.q_HystSF.sf_High
      = test_values[i];

    // Send sib updated indi to the meas sm
    lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

    // Just indicate that SIB3 and SIB4 were updated
    sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
    sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK |
		                              LTE_RRC_SIB5_BITMASK;
    sib_updated_indi.cell_id.phy_cell_id = 0x10;
    sib_updated_indi.cell_id.freq = 0x100;

    lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

    lte_rrc_utf_wait_for_done();

    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

    // We expect meas sm to have thrown the idle meas cfg req
    lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
    TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

    // Verify cell info
    TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
    TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

    // Verify reselection common info
    TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
    TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);

    switch (lte_rrc_meas_test_sib3->cellReselectionInfoCommon.speedStateReselectionPars.q_HystSF.sf_High)
    {
      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_High_dB_6:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -6);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_High_dB_4:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -4);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_High_dB_2:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
        break;

      case lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_High_dB0:
        TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == 0);
        break;
    }

    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

    // Verify serving freq cfg
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

    // Verify intra freq cfg
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);

    // Send the cnf msg back to meas
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the cfg cnf
    lte_rrc_utf_wait_for_done();

    lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
    TF_ASSERT(int_queue_count == 0);
    TF_ASSERT(ext_queue_count == 0);

    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  }
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasReselCommonQHystHigh::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselCommonQHystHighInvalid);

void LteRrcMeasReselCommonQHystHighInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselCommonQHystHighInvalid::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // We set q_hyst_high to an invalid value
    lte_rrc_meas_test_sib3->cellReselectionInfoCommon.speedStateReselectionPars.q_HystSF.sf_High
      = (lte_rrc_osys_SystemInformationBlockType3_cellReselectionInfoCommon_speedStateReselectionPars_q_HystSF_sf_High) 4;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  
  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK |
		                            LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  
  lte_rrc_utf_wait_for_done();
  
  // We expect meas sm to detect the error and not send any config to ML1
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasReselCommonQHystHighInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselCommonTReselEutranMedium);

void LteRrcMeasReselCommonTReselEutranMedium::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselCommonTReselEutranMedium::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  lte_rrc_osys_SpeedStateScaleFactors_sf_Medium test_values[] = 
  {
    lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot25,
    lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5,
    lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot75,
    lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_lDot0
  };
  int i;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // We iterate over t_resel_eutran_medium values, leaving everything else the same
  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    lte_rrc_meas_test_sib3->intraFreqCellReselectionInfo.t_ReselectionEUTRA_SF.sf_Medium
      = test_values[i];

    // Send sib updated indi to the meas sm
    lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

    // Just indicate that SIB3 and SIB4 were updated
    sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
    sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK |
		                              LTE_RRC_SIB5_BITMASK;
    sib_updated_indi.cell_id.phy_cell_id = 0x10;
    sib_updated_indi.cell_id.freq = 0x100;

    lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

    lte_rrc_utf_wait_for_done();

    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

    // We expect meas sm to have thrown the idle meas cfg req
    lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
    TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

    // Verify cell info
    TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
    TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

    // Verify reselection common info
    TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
    TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);

    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

    // Verify serving freq cfg
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

    // Verify intra freq cfg
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);

    switch (lte_rrc_meas_test_sib3->intraFreqCellReselectionInfo.t_ReselectionEUTRA_SF.sf_Medium)
    {
      case lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot25:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT25);
        break;

      case lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot5:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
        break;

      case lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_oDot75:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT75);
        break;

      case lte_rrc_osys_SpeedStateScaleFactors_sf_Medium_lDot0:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_1DOT0);
        break;
    }

    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);

    // Send the cnf msg back to meas
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the cfg cnf
    lte_rrc_utf_wait_for_done();

    lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
    TF_ASSERT(int_queue_count == 0);
    TF_ASSERT(ext_queue_count == 0);

    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  }
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasReselCommonTReselEutranMedium::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselCommonTReselEutranHigh);

void LteRrcMeasReselCommonTReselEutranHigh::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselCommonTReselEutranHigh::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  lte_rrc_osys_SpeedStateScaleFactors_sf_High test_values[] = 
  {
    lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot25,
    lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5,
    lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot75,
    lte_rrc_osys_SpeedStateScaleFactors_sf_High_lDot0
  };
  int i;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // We iterate over t_resel_eutran_high values, leaving everything else the same
  for (i = 0; i < (int)(sizeof(test_values)/sizeof(test_values[0])); i++)
  {
    lte_rrc_meas_test_sib3->intraFreqCellReselectionInfo.t_ReselectionEUTRA_SF.sf_High
      = test_values[i];

    // Send sib updated indi to the meas sm
    lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

    // Just indicate that SIB3 and SIB4 were updated
    sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
    sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK |
		                              LTE_RRC_SIB5_BITMASK;
    sib_updated_indi.cell_id.phy_cell_id = 0x10;
    sib_updated_indi.cell_id.freq = 0x100;

    lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

    lte_rrc_utf_wait_for_done();

    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

    // We expect meas sm to have thrown the idle meas cfg req
    lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

    TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
    TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

    // Verify cell info
    TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
    TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

    // Verify reselection common info
    TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));
    TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                         LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);

    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 60);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);
    TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

    // Verify serving freq cfg
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
    TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

    // Verify intra freq cfg
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);

    switch (lte_rrc_meas_test_sib3->intraFreqCellReselectionInfo.t_ReselectionEUTRA_SF.sf_High)
    {
      case lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot25:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT25);
        break;

      case lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);
        break;

      case lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot75:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT75);
        break;

      case lte_rrc_osys_SpeedStateScaleFactors_sf_High_lDot0:
        TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_1DOT0);
        break;
    }

    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);

    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
    TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);

    // Send the cnf msg back to meas
    lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
    cfg_cnf.status = E_SUCCESS;
    lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

    // We wait for Meas to process the cfg cnf
    lte_rrc_utf_wait_for_done();

    lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
    TF_ASSERT(int_queue_count == 0);
    TF_ASSERT(ext_queue_count == 0);

    TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  }
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasReselCommonTReselEutranHigh::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselCommonTReselEutranHighInvalid);

void LteRrcMeasReselCommonTReselEutranHighInvalid::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselCommonTReselEutranHighInvalid::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // We set t_resel_eutran_high to an invalid value
  lte_rrc_meas_test_sib3->intraFreqCellReselectionInfo.t_ReselectionEUTRA_SF.sf_High
    = (lte_rrc_osys_SpeedStateScaleFactors_sf_High) 4;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  
  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK |
		                            LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  
  lte_rrc_utf_wait_for_done();
  
  // We expect meas sm to detect the error and not send any config to ML1
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
  
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasReselCommonTReselEutranHighInvalid::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasSpeedReselDisabled);

void LteRrcMeasSpeedReselDisabled::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasSpeedReselDisabled::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Set the bitmask to disable speed scaling
  lte_rrc_meas_test_sib3->cellReselectionInfoCommon.m.speedStateReselectionParsPresent = 0;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK |
		                            LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  TF_ASSERT(cfg_req->serving_cell_info.phy_cell_id == 0x10);
  TF_ASSERT(cfg_req->serving_cell_info.dl_cell_frequency == 0x100);

  // Verify reselection common info
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));

  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == FALSE);

  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 10);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 12);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 7);

  // Verify intra freq cfg
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[0].q_offset == -22);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist[1].q_offset == -20);
  
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);


  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasSpeedReselDisabled::Teardown()
{
  lte_rrc_utf_teardown();
}

#ifdef RRC_MOB_INCLUDE
TF_DEFINE_TEST_CASE(NO_RUN, LteRrcMeasReselSIB3NotScheduled);

void LteRrcMeasReselSIB3NotScheduled::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

struct bar_s { 
   int a;
   int b;
};

void LteRrcMeasReselSIB3NotScheduled::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas SM should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  
  // Wait for Meas SM to process the camped indi
  lte_rrc_utf_wait_for_done();

  // No messages should be sent out from meas SM either externally or internally
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB5
  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  lte_rrc_meas_test_decode_sib6((void*) lte_rrc_meas_test_sib6_msg1,
                                sizeof(lte_rrc_meas_test_sib6_msg1));

  lte_rrc_meas_test_decode_sib7((void*) lte_rrc_meas_test_sib7_msg1,
                                sizeof(lte_rrc_meas_test_sib7_msg1));

  lte_rrc_meas_test_decode_sib8((void*) lte_rrc_meas_test_sib8_msg1,
                                sizeof(lte_rrc_meas_test_sib8_msg1));


  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // SIB3 is not scheduled. So Meas should disable reselections with ML1
  sib_updated_indi.sibs_scheduled_bitmask = (LTE_RRC_SIB5_BITMASK |
                                             LTE_RRC_SIB6_BITMASK |
                                             LTE_RRC_SIB7_BITMASK |
                                             LTE_RRC_SIB8_BITMASK);
  sib_updated_indi.sibs_changed_bitmask = (LTE_RRC_SIB5_BITMASK |
                                           LTE_RRC_SIB6_BITMASK |
                                           LTE_RRC_SIB7_BITMASK |
                                           LTE_RRC_SIB8_BITMASK);
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);
  
  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req != NULL);
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS)); 
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                      LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA); 

  // Verify default SIB3 params
  lte_rrc_verify_default_sib3_param(cfg_req);

  // Verify SIB5 params
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max== -30);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);

  // Verify SIB6 params
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->t_resel == 3);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->num_ngbr_freq == 3);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].freq == 100);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].cell_resel_priority == 5);

  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[1].freq == 200);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[1].thresh_Xhigh == 50);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[1].thresh_Xlow == 20);

  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[2].freq == 300);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[2].q_rx_lev_min_actual == -79);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[2].q_qual_min == -10);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[2].p_max == 5);

  // Verify SIB7 params
  // Verify GSM CarrierFreq group parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_frequency_groups == 2); 
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->freq_groups[0].thresh_gsm_high == 60);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->freq_groups[0].thresh_gsm_low == 30);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->freq_groups[0].q_rxlev_min == -45);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->freq_groups[0].p_max == 20);

  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->freq_groups[1].gsm_band == LTE_L1_GSM_BAND_PCS1900);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->freq_groups[1].ncc_permitted == 3);

  // Verify ARFCNs in CarrierFreqsGERAN (in the freq groups)
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->num_freqs == 14);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->freqs[0].freq == 50);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->freqs[0].freq_group_index == 0);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->freqs[1].freq == 55);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->freqs[1].freq_group_index == 0);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->freqs[2].freq == 60);
  TF_ASSERT(cfg_req->meas_cfg.irat_gsm_info_ptr->freqs[2].freq_group_index == 0);

  // Verify SIB8 params
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_search_window_size_enabled == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_ehrpd_resel_enabled == FALSE);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_enabled == TRUE);

  // Verify CDMA bandclass info
  lte_rrc_verify_cdma_bandclass_info(cfg_req->meas_cfg.irat_cdma_info_ptr);

  // Verify CDMA neighbor cell list
  // Validate neighbor cell list size
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list_size == 8);
  // Validate first neighbor cell in the list
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[0].
            band_class == SYS_BAND_BC4);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[0].
            channel == 100);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[0].
            cell_id == 100);

  // Validate next neighbor cell in the list
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[1].
            band_class == SYS_BAND_BC4);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[1].
            channel == 100);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[1].
            cell_id == 102);

  // Validate next neighbor cell in the list
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[2].
            band_class == SYS_BAND_BC4);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[2].
            channel == 100);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[2].
            cell_id == 104);

  // Validate next neighbor cell in the list
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[3].
            band_class == SYS_BAND_BC4);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[3].
            channel == 100);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[3].
            cell_id == 106);

  // Validate next neighbor cell in the list
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[4].
            band_class == SYS_BAND_BC4);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[4].
            channel == 100);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[4].
            cell_id == 108);

  // Validate next neighbor cell in the list
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[5].
            band_class == SYS_BAND_BC4);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[5].
            channel == 110);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[5].
            cell_id == 110);

  // Validate next neighbor cell in the list
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[6].
            band_class == SYS_BAND_BC4);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[6].
            channel == 110);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[6].
            cell_id == 112);

  // Validate next neighbor cell in the list
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[7].
            band_class == SYS_BAND_BC4);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[7].
            channel == 110);
  TF_ASSERT(cfg_req->meas_cfg.irat_cdma_info_ptr->irat_cdma_1x_resel_param.neigh_cell_info_list[7].
            cell_id == 115);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  lte_rrc_utf_wait_for_done();

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasReselSIB3NotScheduled */

void LteRrcMeasReselSIB3NotScheduled::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselSIB4ScheduledAndSIB3NotScheduled);

void LteRrcMeasReselSIB4ScheduledAndSIB3NotScheduled::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselSIB4ScheduledAndSIB3NotScheduled::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; 
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas SM should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  
  // Wait for Meas SM to process the camped indi
  lte_rrc_utf_wait_for_done();

  // No messages should be sent out from meas SM either externally or internally
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB4 and SIB5
  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // SIB3 and SIB4 are not scheduled. So Meas should disable reselections with ML1
  sib_updated_indi.sibs_scheduled_bitmask = (LTE_RRC_SIB4_BITMASK |
                                             LTE_RRC_SIB5_BITMASK);

  sib_updated_indi.sibs_changed_bitmask = (LTE_RRC_SIB4_BITMASK |
                                           LTE_RRC_SIB5_BITMASK);
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);
  
  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req != NULL);
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS)); 
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM)); 

  /* No intra freq neighbor and black cell list info should be present */
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 0);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 0);

  // Verify default SIB3 params
  lte_rrc_verify_default_sib3_param(cfg_req);

  // Verify SIB5 params
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max== -30);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));
  lte_rrc_utf_wait_for_done();

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasReselSIB4ScheduledAndSIB3NotScheduled */

void LteRrcMeasReselSIB4ScheduledAndSIB3NotScheduled::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselSIB4NotScheduled);

void LteRrcMeasReselSIB4NotScheduled::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselSIB4NotScheduled::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas SM should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  
  // Wait for Meas SM to process the camped indi
  lte_rrc_utf_wait_for_done();

  // No messages should be sent out from meas SM either externally or internally
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // SIB4 is not scheduled.
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK |
	                                  LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req != NULL);
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS)); 
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  /* No intra freq neighbor and black cell list info should be present */
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 0);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 0);

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max== -30);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
			LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
			LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);
  
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.start_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.end_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.start_pci == 8);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.end_pci == 8);


  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasReselSIB4NotScheduled */

void LteRrcMeasReselSIB4NotScheduled::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselSIB4AndSIB5NotScheduled);

void LteRrcMeasReselSIB4AndSIB5NotScheduled::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselSIB4AndSIB5NotScheduled::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ; 
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas SM should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  
  // Wait for Meas SM to process the camped indi
  lte_rrc_utf_wait_for_done();

  // No messages should be sent out from meas SM either externally or internally
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // SIB4 is not scheduled.
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK;
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req != NULL);
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS)); 
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  /* No intra freq neighbor and black cell list info should be present */
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 0);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 0);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasReselSIB4AndSIB5NotScheduled */

void LteRrcMeasReselSIB4AndSIB5NotScheduled::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselSIB5NotScheduled);

void LteRrcMeasReselSIB5NotScheduled::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselSIB5NotScheduled::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas SM should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  
  // Wait for Meas SM to process the camped indi
  lte_rrc_utf_wait_for_done();

  // No messages should be sent out from meas SM either externally or internally
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // SIB5 is not scheduled. So Meas should disable inter freq reselections with ML1.
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK |
	                                    LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK |
	                                  LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req != NULL);
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);

  /* Bitmask should indicate that SIB5 has not been scheduled */
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                              LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS)); 
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));
  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  lte_rrc_verify_intra_freq_info(cfg_req);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasReselSIB5NotScheduled */

void LteRrcMeasReselSIB5NotScheduled::Teardown()
{
  lte_rrc_utf_teardown();
}


TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselSIBsScheduledAndUnchanged);

void LteRrcMeasReselSIBsScheduledAndUnchanged::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselSIBsScheduledAndUnchanged::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ; 
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  //lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ; STD_ZEROAT(&cfg_cnf);
  //lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  //uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas SM should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  
  // Wait for Meas SM to process the camped indi
  lte_rrc_utf_wait_for_done();

  // No messages should be sent out from meas SM either externally or internally
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // SIB3 is not scheduled. So Meas should disable reselections with ML1 and 
  // remain in the same state.
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB2_BITMASK |
                                            LTE_RRC_SIB3_BITMASK |
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB2_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();
  
}/* LteRrcMeasReselSIBsScheduledAndUnchanged */

void LteRrcMeasReselSIBsScheduledAndUnchanged::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselSIBsScheduledAndSIB3andSIB4Changed);

void LteRrcMeasReselSIBsScheduledAndSIB3andSIB4Changed::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselSIBsScheduledAndSIB3andSIB4Changed::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas SM should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  
  // Wait for Meas SM to process the camped indi
  lte_rrc_utf_wait_for_done();

  // No messages should be sent out from meas SM either externally or internally
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  /* Change t_eval in SIB3 */
  lte_rrc_meas_test_sib3->cellReselectionInfoCommon.speedStateReselectionPars.mobilityStateParameters.t_Evaluation 
      = lte_rrc_osys_MobilityStateParameters_t_Evaluation_s120;

  // Disable neighbour cell list
  lte_rrc_meas_test_sib4->m.intraFreqNeighCellListPresent = 0;
  lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib4->intraFreqNeighCellList.elem);
  lte_rrc_meas_test_sib4->intraFreqNeighCellList.n = 0;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK |
	                                    LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
	                                  LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req != NULL);
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS)); 
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));
  // Verify reselection common info
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->q_hyst == 1);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.is_valid == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_medium == -6);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.q_hyst_high == -2);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_eval == 120);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.t_hyst_normal == 60);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_medium == 5);
  TF_ASSERT(cfg_req->meas_cfg.resel_common_info_ptr->mobility_state_params.config_params.n_cell_change_high == 5);

  // Verify serving freq cfg
  lte_rrc_verify_serving_freq_info(cfg_req);

  // Verify intra frequency info
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->p_max == 26);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 16);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->meas_bandwidth == LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_medium == LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->t_resel_eutran_high == LTE_CPHY_MEAS_TIME_SF_0DOT5);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->whitelist_num_cells == 0);

  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist_num_cells == 2);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.end_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[0].pci_range.start_pci == 10);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.end_pci == 11);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->blacklist[1].pci_range.start_pci == 11);

  // There is no need to verify inter freq cfg

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasReselSIBsScheduledAndSIB3andSIB4Changed */

void LteRrcMeasReselSIBsScheduledAndSIB3andSIB4Changed::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasReselSIBsScheduledAndSIB5Changed);

void LteRrcMeasReselSIBsScheduledAndSIB5Changed::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasReselSIBsScheduledAndSIB5Changed::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas SM should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  
  // Wait for Meas SM to process the camped indi
  lte_rrc_utf_wait_for_done();

  // No messages should be sent out from meas SM either externally or internally
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_sib5_msg1,
                                sizeof(lte_rrc_meas_test_sib5_msg1));

  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].t_ReselectionEUTRA_SF.sf_High=
      lte_rrc_osys_SpeedStateScaleFactors_sf_High_oDot5;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK |
	                                    LTE_RRC_SIB4_BITMASK |
	                                    LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req != NULL);
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS)); 
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 5180);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -46);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].p_max== -30);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 7);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_medium == 
			LTE_CPHY_MEAS_TIME_SF_0DOT75);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran_high == 
			LTE_CPHY_MEAS_TIME_SF_0DOT5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_high == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].threshX_low == 20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].meas_bandwidth == 
			LTE_CPHY_MEAS_BANDWIDTH_15);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -6);
  
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].pci == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[0].q_offset == -20);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].pci == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[1].q_offset == -18);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].pci = 6);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].whitelist[2].q_offset == -14);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist_num_cells == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.start_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[0].pci_range.end_pci == 1);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.start_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[1].pci_range.end_pci == 5);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.start_pci == 8);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].blacklist[2].pci_range.end_pci == 8);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasReselSIBsScheduledAndSIB5Changed */

void LteRrcMeasReselSIBsScheduledAndSIB5Changed::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasSibUpdatedIndInRrcIdleNotCampedState);

void LteRrcMeasSibUpdatedIndInRrcIdleNotCampedState::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasSibUpdatedIndInRrcIdleNotCampedState::Test()
{
  uint8 int_queue_count;
  uint8 ext_queue_count;
  lte_rrc_camped_indi_s camped_indi;
  lte_rrc_sib_updated_indi_s sib_updated_indi;
  lte_cphy_idle_meas_cfg_req_s *cfg_req;
  lte_cphy_idle_meas_cfg_cnf_s cfg_cnf;
  uint32 buf_len;


  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Make sure that the state machine has been activated
  // We start in the inactive state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();
  
  // We wait for Meas to process the camped indi
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Set csp_get_serving_cell_params fn to return E_FAILURE
  TF_STUB(lte_rrc_csp_get_serving_cell_params) =
    lte_rrc_meas_test_stub__csp_get_serving_cell_params_failure;

  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Just indicate that SIB3 and SIB4 were updated
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}

void LteRrcMeasSibUpdatedIndInRrcIdleNotCampedState::Teardown()
{
  lte_rrc_utf_teardown();
}

#ifdef RRC_MOB_INCLUDE
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasRel9RSRQFieldsPresentInSIB3SIB5SIB6);

void LteRrcMeasRel9RSRQFieldsPresentInSIB3SIB5SIB6::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasRel9RSRQFieldsPresentInSIB3SIB5SIB6::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();

  // Set the release to Rel 9
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel9;
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas SM should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  
  // Wait for Meas SM to process the camped indi
  lte_rrc_utf_wait_for_done();

  // No messages should be sent out from meas SM either externally or internally
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_rel9_rsrq_fields_present_sib3_msg,
                                sizeof(lte_rrc_meas_test_rel9_rsrq_fields_present_sib3_msg));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_rel9_rsrq_fields_present_sib5_msg,
                                sizeof(lte_rrc_meas_test_rel9_rsrq_fields_present_sib5_msg));

  lte_rrc_meas_test_decode_sib6((void*) lte_rrc_meas_test_rel9_rsrq_fields_present_sib6_msg,
                                sizeof(lte_rrc_meas_test_rel9_rsrq_fields_present_sib6_msg));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  sib_updated_indi.sibs_scheduled_bitmask = (LTE_RRC_SIB3_BITMASK |
                                             LTE_RRC_SIB4_BITMASK |
                                             LTE_RRC_SIB5_BITMASK |
                                             LTE_RRC_SIB6_BITMASK);
  sib_updated_indi.sibs_changed_bitmask = (LTE_RRC_SIB3_BITMASK |
                                           LTE_RRC_SIB4_BITMASK |
                                           LTE_RRC_SIB5_BITMASK |
                                           LTE_RRC_SIB6_BITMASK);
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req != NULL);
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS)); 
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA));

  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 62);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 0);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 0);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->serving_freq_resel_info_r9_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->serving_freq_resel_info_r9.
            s_non_intra_searchQ_r9 == 4);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->serving_freq_resel_info_r9.
            thresh_serving_lowQ_r9_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->serving_freq_resel_info_r9.
            thresh_serving_lowQ_r9 == 7);

  // Verify intra freq cfg
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 60);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -50);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param.q_qual_min_r9 == -3);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param.s_intra_searchQ_r9 == 25);

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 2);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 100);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].inter_freq_resel_info_rel9_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].inter_freq_resel_param_rel9.
            q_qual_min_r9 == -22);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].inter_freq_resel_param_rel9.
            thresh_rel9_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].inter_freq_resel_param_rel9.
            thresh_rel9.threshX_highQ == 29);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].inter_freq_resel_param_rel9.
            thresh_rel9.threshX_lowQ == 8);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -50);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 4);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -18);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].dl_carrier_freq == 300);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].inter_freq_resel_info_rel9_present == TRUE);
  // Since q_Qual_min is not signaled, default value should be passed
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].inter_freq_resel_param_rel9.
            q_qual_min_r9 == LTE_CPHY_INT8_NEG_INFINITY);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].inter_freq_resel_param_rel9.
            thresh_rel9_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].inter_freq_resel_param_rel9.
            thresh_rel9.threshX_highQ == 31);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].inter_freq_resel_param_rel9.
            thresh_rel9.threshX_lowQ == 12);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].q_rxlevmin == -56);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].t_resel_eutran == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].cell_resel_priority == 5);

  // Verify WCDMA parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->num_ngbr_freq == 1);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].freq == 100);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].thresh_rel9_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].thresh_rel9.threshX_highQ == 31);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].thresh_rel9.threshX_lowQ == 14);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].thresh_Xhigh == 62);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].thresh_Xlow == 42);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].q_rx_lev_min_actual == -45);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].q_qual_min == -10);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].p_max == 10);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasRel9RSRQFieldsPresentInSIB3SIB5SIB6 */

void LteRrcMeasRel9RSRQFieldsPresentInSIB3SIB5SIB6::Teardown()
{
  lte_rrc_utf_teardown();
}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasRel9RSRQFieldsAbsentInSIB3SIB5SIB6);

void LteRrcMeasRel9RSRQFieldsAbsentInSIB3SIB5SIB6::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}

void LteRrcMeasRel9RSRQFieldsAbsentInSIB3SIB5SIB6::Test()
{
  lte_rrc_camped_indi_s         camped_indi        ;
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&camped_indi, 0, sizeof(lte_rrc_camped_indi_s));
  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));
  
  // Initialize the proto stubs
  lte_rrc_meas_test_init();

  // Set the release to Rel 9
  TF_STUB(lte_rrc_get_3gpp_release_version) = lte_rrc_meas_test_stub__get_3gpp_release_version_for_rel9;
  
  // Meas SM starts in INACTIVE state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Send a camped indication, Meas SM should move to the idle_camped state
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  
  // Wait for Meas SM to process the camped indi
  lte_rrc_utf_wait_for_done();

  // No messages should be sent out from meas SM either externally or internally
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_rel9_rsrq_fields_present_sib3_msg,
                                sizeof(lte_rrc_meas_test_rel9_rsrq_fields_present_sib3_msg));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  lte_rrc_meas_test_decode_sib5((void*) lte_rrc_meas_test_rel9_rsrq_fields_present_sib5_msg,
                                sizeof(lte_rrc_meas_test_rel9_rsrq_fields_present_sib5_msg));

  lte_rrc_meas_test_decode_sib6((void*) lte_rrc_meas_test_rel9_rsrq_fields_present_sib6_msg,
                                sizeof(lte_rrc_meas_test_rel9_rsrq_fields_present_sib6_msg));

  // Reset the Rel 9 present masks in SIB3
  lte_rrc_meas_test_sib3->m.s_NonIntraSearch_v920Present = 0;
  lte_rrc_meas_test_sib3->m.s_IntraSearch_v920Present = 0;
  lte_rrc_meas_test_sib3->m.threshServingLowQ_r9Present = 0;
  lte_rrc_meas_test_sib3->m.q_QualMin_r9Present = 0;

  // Reset all Rel 9 present masks in SIB5
  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].m.q_QualMin_r9Present = 0;
  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[0].m.threshX_Q_r9Present = 0;
  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[1].m.q_QualMin_r9Present = 0;
  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[2].m.q_QualMin_r9Present = 0;
  lte_rrc_meas_test_sib5->interFreqCarrierFreqList.elem[2].m.threshX_Q_r9Present = 0;

  // Reset all Rel 9 present masks in SIB6
  lte_rrc_meas_test_sib6->carrierFreqListUTRA_FDD.elem[0].m.threshX_Q_r9Present = 0;

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  sib_updated_indi.sibs_scheduled_bitmask = (LTE_RRC_SIB3_BITMASK |
                                             LTE_RRC_SIB4_BITMASK |
                                             LTE_RRC_SIB5_BITMASK |
                                             LTE_RRC_SIB6_BITMASK);
  sib_updated_indi.sibs_changed_bitmask = (LTE_RRC_SIB3_BITMASK |
                                           LTE_RRC_SIB4_BITMASK |
                                           LTE_RRC_SIB5_BITMASK |
                                           LTE_RRC_SIB6_BITMASK);
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req != NULL);
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS)); 
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA));

  // Verify serving freq cfg
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->s_non_intra_search == 44);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->thresh_serving_low == 0);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->cell_resel_priority == 0);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->serving_freq_resel_info_r9_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->serving_freq_resel_info_r9.
            s_non_intra_searchQ_r9 == 0);
  TF_ASSERT(cfg_req->meas_cfg.serving_freq_info_ptr->serving_freq_resel_info_r9.
            thresh_serving_lowQ_r9_present == FALSE);

  // Verify intra freq cfg
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->s_intra_search == 40);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->q_rxlevmin == -50);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param.q_qual_min_r9 == LTE_CPHY_INT8_NEG_INFINITY);
  TF_ASSERT(cfg_req->meas_cfg.intra_freq_info_ptr->intra_freq_resel_rel9_param.s_intra_searchQ_r9 == 0);

  // Verify inter freq cfg
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->num_freq == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].dl_carrier_freq == 100);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].inter_freq_resel_info_rel9_present == TRUE);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].inter_freq_resel_param_rel9.
            q_qual_min_r9 == LTE_CPHY_INT8_NEG_INFINITY);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].inter_freq_resel_param_rel9.
            thresh_rel9_present == FALSE);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_rxlevmin == -50);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].t_resel_eutran == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].cell_resel_priority == 4);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[0].q_offset_freq == -18);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].dl_carrier_freq == 200);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].inter_freq_resel_info_rel9_present == TRUE);
  // Since q_Qual_min is not signaled, default value should be passed
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].inter_freq_resel_param_rel9.
            q_qual_min_r9 == LTE_CPHY_INT8_NEG_INFINITY);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].inter_freq_resel_param_rel9.
            thresh_rel9_present == FALSE);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].q_rxlevmin == -48);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].t_resel_eutran == 4);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[1].cell_resel_priority == 2);

  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[2].dl_carrier_freq == 300);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[2].inter_freq_resel_info_rel9_present == TRUE);
  // Since q_Qual_min is not signaled, default value should be passed
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[2].inter_freq_resel_param_rel9.
            q_qual_min_r9 == LTE_CPHY_INT8_NEG_INFINITY);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[2].inter_freq_resel_param_rel9.
            thresh_rel9_present == FALSE);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[2].q_rxlevmin == -56);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[2].t_resel_eutran == 3);
  TF_ASSERT(cfg_req->meas_cfg.inter_freq_info_ptr->freq_list[2].cell_resel_priority == 5);

  // Verify WCDMA parameters
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->num_ngbr_freq == 2);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].freq == 100);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].thresh_rel9_present == FALSE);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].cell_resel_priority == 6);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].thresh_Xhigh == 62);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].thresh_Xlow == 42);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].q_rx_lev_min_actual == -45);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].q_qual_min == -10);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[0].p_max == 10);

  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[1].freq == 200);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[1].thresh_rel9_present == FALSE);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[1].cell_resel_priority == 3);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[1].thresh_Xhigh == 52);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[1].thresh_Xlow == 26);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[1].q_rx_lev_min_actual == -39);
  TF_ASSERT(cfg_req->meas_cfg.irat_wcdma_info_ptr->fdd_freq_list[1].q_qual_min == -5);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasRel9RSRQFieldsAbsentInSIB3SIB5SIB6 */

void LteRrcMeasRel9RSRQFieldsAbsentInSIB3SIB5SIB6::Teardown()
{
  lte_rrc_utf_teardown();
}

/* The test case verifies the handling of max GSM freqs (32) after pruning */
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasMaxGSMFreqWithPruning);

void LteRrcMeasMaxGSMFreqWithPruning::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));
}/* LteRrcMeasMaxGSMFreqWithPruning::Setup() */

void LteRrcMeasMaxGSMFreqWithPruning::Test()
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ;
  lte_cphy_idle_meas_cfg_cnf_s  cfg_cnf            ;
  lte_cphy_idle_meas_cfg_req_s *cfg_req            = NULL;
  uint32                        buf_len            = 0;
  uint8                         int_queue_count    = 0;
  uint8                         ext_queue_count    = 0;

  memset(&sib_updated_indi, 0, sizeof(lte_rrc_sib_updated_indi_s));
  memset(&cfg_cnf, 0, sizeof(lte_cphy_idle_meas_cfg_cnf_s));

  // Get Meas module to idle camped state
  lte_rrc_meas_test_get_meas_sm_to_idle_camped();

  // Point the SIBs to be returned to the typical SIB3 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  lte_rrc_meas_test_decode_sib6((void*) lte_rrc_meas_test_sib6_msg1,
                                sizeof(lte_rrc_meas_test_sib6_msg1));

  lte_rrc_meas_test_decode_sib7((void*) lte_rrc_meas_test_sib7_msg1,
                                sizeof(lte_rrc_meas_test_sib7_msg1));

  lte_rrc_meas_test_decode_sib8((void*) lte_rrc_meas_test_sib8_msg2,
                                sizeof(lte_rrc_meas_test_sib8_msg2));

    // Free unused memory in SIB7
  lte_rrc_asn1_osys_IE_free(lte_rrc_meas_test_sib7->carrierFreqsInfoList.elem[0].carrierFreqs.
                   followingARFCNs.u.explicitListOfARFCNs->elem);
  lte_rrc_meas_test_sib7->carrierFreqsInfoList.elem[0].carrierFreqs.
                   followingARFCNs.u.explicitListOfARFCNs->n = 0;

  // Populate maximum allowed number of neighbor freqs in SIB7
  lte_rrc_meas_test_populate_max_gsm_neigh_freqs(&lte_rrc_meas_test_sib7->carrierFreqsInfoList);

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                          LTE_RRC_SIB6_BITMASK |
                                          LTE_RRC_SIB7_BITMASK |
                                          LTE_RRC_SIB8_BITMASK ;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
	                                          LTE_RRC_SIB6_BITMASK |
                                            LTE_RRC_SIB7_BITMASK |
                                            LTE_RRC_SIB8_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect meas sm to have thrown the idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  // Verify cell info
  lte_rrc_verify_serving_cell_info(cfg_req);


  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS));   
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                      LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA);

  // Verify reselection common info
  lte_rrc_verify_common_resel_info(cfg_req);

  // Verify CarrierFreqsInfoList
  lte_rrc_meas_test_verify_max_gsm_neigh_freqs(cfg_req->meas_cfg.irat_gsm_info_ptr);

  // Send the cnf msg back to meas
  lte_rrc_init_default_hdr(&cfg_cnf.msgr_hdr, LTE_CPHY_IDLE_MEAS_CFG_CNF);
  cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(cfg_cnf));

  // We wait for Meas to process the cfg cnf
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();

}/* LteRrcMeasMaxGSMFreqWithPruning::Test() */

void LteRrcMeasMaxGSMFreqWithPruning::Teardown()
{
  lte_rrc_utf_teardown();
}
#endif
