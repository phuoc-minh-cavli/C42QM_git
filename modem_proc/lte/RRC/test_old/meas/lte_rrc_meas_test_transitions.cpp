/*!
  @file lte_rrc_meas_test_transitions.cpp

  @brief
  Unit test code for the meas procedure.

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/meas/lte_rrc_meas_test_transitions.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/12/09   sk      March 09 spec migration for SIB4,SIB4,SIB5 and conn meas
06/30/09   ask     Meas changes for RLF and HO failure handling
05/11/09   sk      Added support for handling sib updated indi in wait_for_
                   config_cnf and connected state
04/17/09   sk      Updated meas state names
02/23/09   sk      Populated status field for LTE_CPHY_CONN_MEAS_CFG_CNF msg
01/02/09   sk      Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "__lte_rrc_meas.h"
#include "lte_rrc_meas.h"
#include "lte_rrc_measi.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_rrc_ext_api.h"
}
#include <tf_stub.h>
#include "lte_rrc_meas_test_msg.h"
#include "TestFramework.h"
#include "lte_rrc_utf.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module lte_rrc_meas_test_transitions.cpp
*/
typedef struct
{
  int   internal_var;  /*!< My internal variable */
  void *internal_ptr;  /*!< My internal pointer */

} lte_rrc_meas_test_transitions_s;


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_meas_test_transitions.cpp
*/
DECLARE_LTE_RRC_MEAS_TEST_SIB3_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_SIB4_MSG1;
DECLARE_LTE_RRC_MEAS_TEST_MEAS_CONFIG_MSG1;

extern "C"
{
  extern lte_rrc_meas_s lte_rrc_meas;
}

extern "C"
{
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s *);
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

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasBasicTrans);

void LteRrcMeasBasicTrans::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));

}/* LteRrcMeasBasicTrans::Setup() */

void LteRrcMeasBasicTrans::Test()
{
  lte_rrc_camped_indi_s         camped_indi;
  lte_rrc_stopped_indi_s        stopped_indi;
  lte_rrc_connected_indi_s      connected_indi;
  lte_rrc_not_camped_indi_s     not_camped_indi;
  lte_rrc_sib_updated_indi_s    sib_updated_indi;
  lte_rrc_conn_released_indi_s  conn_rel_indi;
  lte_rrc_meas_cfg_reqi_s       meas_cfg_reqi;
  lte_cphy_idle_meas_cfg_cnf_s  idle_meas_cfg_cnf;
  lte_cphy_conn_meas_cfg_cnf_s  conn_meas_cfg_cnf;
  lte_cphy_idle_meas_cfg_req_s *cfg_req = NULL;
  uint32                        buf_len = 0;
  lte_rrc_sib_updated_indi_s  *sib_updated_indi_ptr = NULL;

  lte_rrc_osys_MeasConfig *meas_cfg;

  uint8 int_queue_count;
  uint8 ext_queue_count;

  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Test the dedicated priority owner API
  lte_rrc_dedicated_pri_set_owner(LTE_RRC_DEDICATED_PRI_OWNER_LTE);
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

  lte_rrc_dedicated_pri_set_owner(LTE_RRC_DEDICATED_PRI_OWNER_WCDMA);
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_WCDMA);

  lte_rrc_dedicated_pri_set_owner(LTE_RRC_DEDICATED_PRI_OWNER_GERAN);
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_GERAN);

  lte_rrc_dedicated_pri_set_owner(LTE_RRC_DEDICATED_PRI_OWNER_NONE);
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_NONE);


  // Point the SIBs to be returned to the typical SIB3 and SIB4
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Make sure that the state machine has been activated and that the 
  // initial state is inactive
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // ----------------------------------------------------------------------

  // INACTIVE -> IDLE_CAMPED
  // Trigger event: LTE_RRC_CAMPED_INDI

  TF_MSG("Sending Camped indication");
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  lte_rrc_utf_reset();

  // ----------------------------------------------------------------------

  // IDLE_CAMPED -> INACTIVE
  // Trigger event: LTE_RRC_NOT_CAMPED_INDI

  TF_MSG("Sending Not-Camped indication");
  lte_rrc_init_default_hdr(&not_camped_indi.msg_hdr, LTE_RRC_NOT_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&not_camped_indi, sizeof(not_camped_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  lte_rrc_utf_reset();

  // ----------------------------------------------------------------------

  // INACTIVE -> IDLE_CAMPED -> INACTIVE
  // Trigger event: LTE_RRC_CAMPED_INDI, LTE_RRC_STOPPED_INDI
  
  TF_MSG("Sending Camped indication");
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  TF_MSG("Sending LTE stopped indication");
  lte_rrc_init_default_hdr(&stopped_indi.msg_hdr, LTE_RRC_STOPPED_INDI);
  lte_rrc_utf_send_msg((byte*)&stopped_indi, sizeof(stopped_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // ----------------------------------------------------------------------

  // INACTIVE -> IDLE_CAMPED -> IDLE_CONFIG
  // Trigger event: LTE_RRC_CAMPED_INDI, LTE_RRC_SIB_UPDATED_INDI

  TF_MSG("Sending Camped indication");
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  TF_MSG("Sending SIB updated indication");
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  sib_updated_indi.sibs_changed_bitmask = 0;
  sib_updated_indi.sibs_changed_bitmask |= LTE_RRC_SIB3_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

    // ----------------------------------------------------------------------

  // IDLE_CONFIG -> INACTIVE
  // Trigger event: LTE_RRC_NOT_CAMPED_INDI


  TF_MSG("Sending not-camped indication");
  lte_rrc_init_default_hdr(&not_camped_indi.msg_hdr, LTE_RRC_NOT_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&not_camped_indi, sizeof(not_camped_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // ----------------------------------------------------------------------

  // INACTIVE -> IDLE_CAMPED -> IDLE_CONFIG --> IDLE_CAMPED
  // Trigger event: LTE_RRC_CAMPED_INDI, LTE_RRC_SIB_UPDATED_INDI,
  //                LTE_CPHY_IDLE_MEAS_CFG_CNF


  TF_MSG("Sending Camped indication");
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  TF_MSG("Sending SIB updated indication");
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  sib_updated_indi.sibs_changed_bitmask = 0;
  sib_updated_indi.sibs_changed_bitmask |= LTE_RRC_SIB3_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  TF_MSG("Sending LTE_CPHY_IDLE_MEAS_CFG_CNF");
  msgr_init_hdr(&idle_meas_cfg_cnf.msgr_hdr,MSGR_LTE_CPHY,LTE_CPHY_IDLE_MEAS_CFG_CNF);
  idle_meas_cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&idle_meas_cfg_cnf, sizeof(idle_meas_cfg_cnf));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // ----------------------------------------------------------------------

  // IDLE_CAMPED -> IDLE_CONFIG -> INACTIVE
  // Trigger event: LTE_RRC_CAMPED_INDI, LTE_RRC_SIB_UPDATED_INDI,
  //                LTE_RRC_STOPPED_INDI

  TF_MSG("Sending SIB updated indication");
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  sib_updated_indi.sibs_changed_bitmask = 0;
  sib_updated_indi.sibs_changed_bitmask |= LTE_RRC_SIB3_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  TF_MSG("Sending Stopped indication");
  lte_rrc_init_default_hdr(&stopped_indi.msg_hdr, LTE_RRC_STOPPED_INDI);
  lte_rrc_utf_send_msg((byte*)&stopped_indi, sizeof(stopped_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // ----------------------------------------------------------------------

  // INACTIVE -> IDLE_CAMPED -> CONNECTED
  // Trigger event: LTE_RRC_CAMPED_INDI, LTE_RRC_CONNECTED_INDI

  TF_MSG("Sending Camped indication");
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  TF_MSG("Sending Connected indication");
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // ----------------------------------------------------------------------

  // CONNECTED -> INACTIVE
  // Trigger event: LTE_RRC_STOPPED_INDI

  TF_MSG("Sending Stopped indication");
  lte_rrc_init_default_hdr(&stopped_indi.msg_hdr, LTE_RRC_STOPPED_INDI);
  lte_rrc_utf_send_msg((byte*)&stopped_indi, sizeof(stopped_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // ----------------------------------------------------------------------

  // INACTIVE -> IDLE_CAMPED -> CONNECTED -> IDLE_CAMPED
  // Trigger event: LTE_RRC_NOT_CAMPED_INDI, LTE_RRC_CONN_RELEASED_INDI

  TF_MSG("Sending Camped indication");
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  TF_MSG("Sending Connected indication");
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  TF_MSG("Sending the ConnReleased indication");
  lte_rrc_init_default_hdr(&conn_rel_indi.msg_hdr, LTE_RRC_CONN_RELEASED_INDI);
  lte_rrc_utf_send_msg((byte*)&conn_rel_indi, sizeof(conn_rel_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // ----------------------------------------------------------------------

  // IDLE_CAMPED -> CONNECTED -> CONNECTED_CONFIG
  // Trigger event: LTE_RRC_CONNECTED_INDI,LTE_RRC_MEAS_CFG_REQI

  TF_MSG("Sending Connected indication");
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Point the meas_config to be returned to the typical meas_config
  lte_rrc_meas_test_decode_meas_config((void*) lte_rrc_meas_test_meas_config_msg1,
                                       sizeof(lte_rrc_meas_test_meas_config_msg1),
                                       &meas_cfg);

  // Add an item to the meas obj to add/mod list
  meas_cfg->m.measObjectToAddModListPresent = 1;
  
  meas_cfg->measObjectToAddModList.elem = 
    (lte_rrc_osys_MeasObjectToAddMod*)
    lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectToAddMod));
  meas_cfg->measObjectToAddModList.n = 1;

  meas_cfg->measObjectToAddModList.elem[0].measObjectId = 1;
  meas_cfg->measObjectToAddModList.elem[0].measObject.t = T_lte_rrc_osys_MeasObjectToAddMod_measObject_measObjectEUTRA;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA = 
    (lte_rrc_osys_MeasObjectEUTRA *)lte_rrc_asn1_osys_IE_malloc(sizeof(lte_rrc_osys_MeasObjectEUTRA));
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.blackCellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellForWhichToReportCGIPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToAddModListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->m.cellsToRemoveListPresent = 0;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->carrierFreq = 100;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->allowedMeasBandwidth = lte_rrc_osys_AllowedMeasBandwidth_mbw15;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->presenceAntennaPort1 = TRUE;
  meas_cfg->measObjectToAddModList.elem[0].measObject.u.measObjectEUTRA->offsetFreq = lte_rrc_osys_Q_OffsetRange_dB0;

  // Validate meas cfg
  TF_MSG("Validating Meas Config");
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  TF_MSG("Sending Meas Config request");
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  meas_cfg_reqi.cause = LTE_RRC_MEAS_CFG_REQI_CAUSE_NORMAL;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  // ----------------------------------------------------------------------

  // CONNECTED_CONFIG -> INACTIVE
  // Trigger event: LTE_RRC_STOPPED_INDI

  TF_MSG("Sending Stopped indication");
  lte_rrc_init_default_hdr(&stopped_indi.msg_hdr, LTE_RRC_STOPPED_INDI);
  lte_rrc_utf_send_msg((byte*)&stopped_indi, sizeof(stopped_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // ----------------------------------------------------------------------

  // INACTIVE -> IDLE_CAMPED -> CONNECTED -> -> CONNECTED_CONFIG -> IDLE_CAMPED
  // Trigger event: LTE_RRC_CAMPED_INDI, LTE_RRC_CONNECTED_INDI,
  //                LTE_RRC_MEAS_CFG_REQI, LTE_RRC_CONN_RELEASED_INDI

  TF_MSG("Sending Camped indication");
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  TF_MSG("Sending Connected indication");
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Validate meas cfg
  TF_MSG("Validating Meas Config");
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  TF_MSG("Sending Meas Config request");
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  TF_MSG("Sending the ConnReleased indication");
  lte_rrc_init_default_hdr(&conn_rel_indi.msg_hdr, LTE_RRC_CONN_RELEASED_INDI);
  lte_rrc_utf_send_msg((byte*)&conn_rel_indi, sizeof(conn_rel_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // ----------------------------------------------------------------------
  
  // IDLE_CAMPED -> CONNECTED -> -> CONNECTED_CONFIG -> CONNECTED
  // Trigger event: LTE_RRC_CAMPED_INDI, LTE_RRC_CONNECTED_INDI,
  //                LTE_RRC_MEAS_CFG_REQI, LTE_CPHY_CONN_MEAS_CFG_CNF

  TF_MSG("Sending Connected indication");
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // Validate meas cfg
  TF_MSG("Validating Meas Config");
  TF_ASSERT(lte_rrc_meas_validate_connected_config(lte_rrc_meas_test_meas_config,
                                                    FALSE) == E_SUCCESS);

  TF_MSG("Sending Meas Config request");
  lte_rrc_init_default_hdr(&meas_cfg_reqi.msg_hdr, LTE_RRC_MEAS_CFG_REQI);
  meas_cfg_reqi.dl_dcch_ptr = lte_rrc_meas_test_meas_config;
  lte_rrc_utf_send_msg((byte*)&meas_cfg_reqi, sizeof(meas_cfg_reqi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_CONNECTED_CONFIG_CNF);

  TF_MSG("Sending LTE_CPHY_CONN_MEAS_CFG_CNF");
  msgr_init_hdr(&conn_meas_cfg_cnf.msgr_hdr,MSGR_LTE_CPHY,LTE_CPHY_CONN_MEAS_CFG_CNF);
  conn_meas_cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&conn_meas_cfg_cnf, sizeof(conn_meas_cfg_cnf));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  // ----------------------------------------------------------------------
  
  // CONNECTED -> INACTIVE
  // Trigger event: LTE_RRC_STOPPED_INDI

  TF_MSG("Sending LTE stopped indication");
  lte_rrc_init_default_hdr(&stopped_indi.msg_hdr, LTE_RRC_STOPPED_INDI);
  lte_rrc_utf_send_msg((byte*)&stopped_indi, sizeof(stopped_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();
  // Free allocated memory for reconfig message
  lte_rrc_meas_test_free_reconfig_msg_ptr();

  lte_rrc_utf_reset();
  // ----------------------------------------------------------------------

  // INACTIVE -> IDLE_CAMPED -> WAIT_FOR_IDLE_CFG_CNF 
  // Trigger Event: SIB_UPDATED_INDI
  // WAIT_FOR_IDLE_CFG_CNF -> WAIT_FOR_IDLE_CFG_CNF
  // Trigger Event: While in  WAIT_FOR_IDLE_CFG_CNF ,a second SIB_UPDATED_INDI  is received.
  // The second SIB_UPDATED_INDI should be put in the pending queue.
  // WAIT_FOR_IDLE_CFG_CNF -> IDLE_CAMPED
  // Trigger Event: LTE_CPHY_IDLE_MEAS_CFG_CNF
  // IDLE_CAMPED -> WAIT_FOR_IDLE_CFG_CNF
  // Trigger Event: Pending queue SIB_UPDATED_INDI

  TF_MSG("Sending Camped indication");
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  lte_rrc_utf_wait_for_done();

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

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  lte_rrc_utf_wait_for_done();

  // We expect Meas sm to have thrown a idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req != NULL);
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS)); 
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA));

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  TF_MSG("Sending second SIB updated indication ");
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0); 

  // Verify theat there is no change in state.
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // Sending confirmation for the first idle meas cfg request
  TF_MSG("Sending LTE_CPHY_IDLE_MEAS_CFG_CNF");
  msgr_init_hdr(&idle_meas_cfg_cnf.msgr_hdr,MSGR_LTE_CPHY,LTE_CPHY_IDLE_MEAS_CFG_CNF);
  idle_meas_cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&idle_meas_cfg_cnf, sizeof(idle_meas_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  // We expect Meas sm to have thrown a idle meas cfg req for the second idle meas cfg request 
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS)); 
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA));
  lte_rrc_utf_wait_for_done();

  // Sending confirmation for the second idle meas cfg request
  TF_MSG("Sending LTE_CPHY_IDLE_MEAS_CFG_CNF");
  msgr_init_hdr(&idle_meas_cfg_cnf.msgr_hdr,MSGR_LTE_CPHY,LTE_CPHY_IDLE_MEAS_CFG_CNF);
  idle_meas_cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&idle_meas_cfg_cnf, sizeof(idle_meas_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 1);
  TF_ASSERT(ext_queue_count == 0);

  lte_rrc_utf_get_next_int_msg((byte**)&sib_updated_indi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_sib_updated_indi_s));
  TF_ASSERT(sib_updated_indi_ptr != NULL);
  TF_ASSERT(sib_updated_indi_ptr->msg_hdr.id == LTE_RRC_MEAS_PENDING_SIB_UPDATED_INDI);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();
  
  lte_rrc_utf_reset();

  // ----------------------------------------------------------------------

  // IDLE_CAMPED -> WAIT_FOR_IDLE_CFG_CNF 
  // Trigger Event: SIB_UPDATED_INDI
  // WAIT_FOR_IDLE_CFG_CNF -> WAIT_FOR_IDLE_CFG_CNF
  // Trigger Event: While in  WAIT_FOR_IDLE_CFG_CNF ,a second SIB_UPDATED_INDI  is received.
  // The second SIB_UPDATED_INDI should be put in the pending queue.
  // WAIT_FOR_IDLE_CFG_CNF -> CONNECTED
  // Trigger Event: LTE_RRC_CONNECTED_INDI
  // LTE_RRC_CONNECTED_INDI -> IDLE_CAMPED
  // Trigger Event: LTE_RRC_CONN_RELEASED_INDI

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Point the SIBs to be returned to the typical SIB3 SIB4 and SIB5
  lte_rrc_meas_test_decode_sib3((void*) lte_rrc_meas_test_sib3_msg1,
                                sizeof(lte_rrc_meas_test_sib3_msg1));

  lte_rrc_meas_test_decode_sib4((void*) lte_rrc_meas_test_sib4_msg1,
                                sizeof(lte_rrc_meas_test_sib4_msg1));

  // Send sib updated indi to the meas sm
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  // We expect Meas sm to have thrown a idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);

  TF_MSG("Sending second SIB updated indication");
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                      LTE_RRC_SIB5_BITMASK;
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0); 

  // There should be no change in Meas state
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  TF_MSG("Sending Connected indication");
  lte_rrc_init_default_hdr(&connected_indi.msg_hdr, LTE_RRC_CONNECTED_INDI);
  lte_rrc_utf_send_msg((byte*)&connected_indi, sizeof(connected_indi));

  lte_rrc_utf_wait_for_done();

  // Verify that no idle meas cfg request is thrown out by MEAS SM 
  /*lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 1);
  TF_ASSERT(ext_queue_count == 0);*/

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__CONNECTED);

  TF_MSG("Sending the ConnReleased indication");
  lte_rrc_init_default_hdr(&conn_rel_indi.msg_hdr, LTE_RRC_CONN_RELEASED_INDI);
  lte_rrc_utf_send_msg((byte*)&conn_rel_indi, sizeof(conn_rel_indi));

  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);
  

  // Sending confirmation for the first idle meas cfg request
  TF_MSG("Sending LTE_CPHY_IDLE_MEAS_CFG_CNF");
  msgr_init_hdr(&idle_meas_cfg_cnf.msgr_hdr,MSGR_LTE_CPHY,LTE_CPHY_IDLE_MEAS_CFG_CNF);
  idle_meas_cfg_cnf.status = E_SUCCESS;
  lte_rrc_utf_send_msg((byte*)&idle_meas_cfg_cnf, sizeof(idle_meas_cfg_cnf));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 1);
  TF_ASSERT(ext_queue_count == 0);

  lte_rrc_utf_get_next_int_msg((byte**)&sib_updated_indi_ptr, &buf_len);
  TF_ASSERT(buf_len == sizeof(lte_rrc_sib_updated_indi_s));

  TF_ASSERT(sib_updated_indi_ptr != NULL);


  TF_ASSERT(sib_updated_indi_ptr->msg_hdr.id == LTE_RRC_MEAS_PENDING_SIB_UPDATED_INDI); 

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();
  
  lte_rrc_utf_reset();

}/* LteRrcMeasBasicTrans::Test() */

void LteRrcMeasBasicTrans::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasBasicTrans::Teardown() */

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcMeasIgnorePendedSibIndiWhileTransistioningToInactiveState);

void LteRrcMeasIgnorePendedSibIndiWhileTransistioningToInactiveState::Setup()
{
  uint32 umid_list[] = 
    { 
      LTE_CPHY_IDLE_MEAS_CFG_REQ,
      LTE_CPHY_CONN_MEAS_CFG_REQ,
    };

  lte_rrc_utf_setup(LTE_RRC_MEAS_SM, umid_list, 
                    sizeof(umid_list)/sizeof(umid_list[0]));

}/* LteRrcMeasBasicTrans::Setup() */

void LteRrcMeasIgnorePendedSibIndiWhileTransistioningToInactiveState::Test()
{
  lte_rrc_camped_indi_s         camped_indi;
  lte_rrc_stopped_indi_s        stopped_indi;
  lte_rrc_connected_indi_s      connected_indi;
  lte_rrc_not_camped_indi_s     not_camped_indi;
  lte_rrc_sib_updated_indi_s    sib_updated_indi;
  lte_rrc_conn_released_indi_s  conn_rel_indi;
  lte_rrc_meas_cfg_reqi_s       meas_cfg_reqi;
  lte_cphy_idle_meas_cfg_cnf_s  idle_meas_cfg_cnf;
  lte_cphy_conn_meas_cfg_cnf_s  conn_meas_cfg_cnf;
  lte_cphy_idle_meas_cfg_req_s *cfg_req = NULL;
  uint32                        buf_len = 0;
  lte_rrc_sib_updated_indi_s  *sib_updated_indi_ptr = NULL;

  lte_rrc_osys_MeasConfig *meas_cfg;

  uint8 int_queue_count;
  uint8 ext_queue_count;

  // Initialize the proto stubs
  lte_rrc_meas_test_init();
  
  // Test the dedicated priority owner API
  lte_rrc_dedicated_pri_set_owner(LTE_RRC_DEDICATED_PRI_OWNER_LTE);
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_LTE);

  lte_rrc_dedicated_pri_set_owner(LTE_RRC_DEDICATED_PRI_OWNER_WCDMA);
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_WCDMA);

  lte_rrc_dedicated_pri_set_owner(LTE_RRC_DEDICATED_PRI_OWNER_GERAN);
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_GERAN);

  lte_rrc_dedicated_pri_set_owner(LTE_RRC_DEDICATED_PRI_OWNER_NONE);
  TF_ASSERT(lte_rrc_dedicated_pri_get_owner() == LTE_RRC_DEDICATED_PRI_OWNER_NONE);

  TF_MSG("Sending Camped indication");
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  camped_indi.camped_cell.phy_cell_id = 0x10;
  camped_indi.camped_cell.freq = 0x100;
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  lte_rrc_utf_wait_for_done();

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

  // Send SIB3, SIB4 and SIB5 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  lte_rrc_utf_wait_for_done();

  // We expect Meas sm to have thrown a idle meas cfg req
  lte_rrc_utf_get_next_ext_msg((byte**)&cfg_req, &buf_len);

  TF_ASSERT(buf_len == sizeof(lte_cphy_idle_meas_cfg_req_s));
  TF_ASSERT(cfg_req != NULL);
  TF_ASSERT(cfg_req->msgr_hdr.id == LTE_CPHY_IDLE_MEAS_CFG_REQ);
  TF_ASSERT(cfg_req->meas_cfg.requested_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_RESEL_COMMON |
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_SERVING_FREQ | 
                                                        LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ |
                                                            LTE_CPHY_IDLE_MEAS_CFG_MASK_SKIP_SRCH_MEAS)); 
  TF_ASSERT(cfg_req->meas_cfg.disabled_cfg_bitmask == (LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA |
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA|
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_TDSCDMA|
                                                       LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM));

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  TF_MSG("Sending second SIB updated indication ");
  lte_rrc_init_default_hdr(&sib_updated_indi.msg_hdr, LTE_RRC_SIB_UPDATED_INDI);
  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK;
  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq = 0x100;
  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0); 

  // Verify theat there is no change in state.
  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__WAIT_FOR_IDLE_CONFIG_CNF);

  lte_rrc_init_default_hdr(&not_camped_indi.msg_hdr, LTE_RRC_NOT_CAMPED_INDI);
  lte_rrc_utf_send_msg((byte*)&not_camped_indi, sizeof(not_camped_indi));

  lte_rrc_utf_wait_for_done();
  
  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__INACTIVE);

  // Set csp_get_serving_cell_params fn to return E_FAILURE
  TF_STUB(lte_rrc_csp_get_serving_cell_params) =
    lte_rrc_meas_test_stub__csp_get_serving_cell_params1;

  TF_MSG("Sending Camped indication on a different cell");
  lte_rrc_init_default_hdr(&camped_indi.msg_hdr, LTE_RRC_CAMPED_INDI);
  camped_indi.camped_cell.phy_cell_id = 0x11;
  camped_indi.camped_cell.freq = 0x100;
  lte_rrc_utf_send_msg((byte*)&camped_indi, sizeof(camped_indi));
  lte_rrc_utf_wait_for_done();

  lte_rrc_utf_get_queue_counts(&int_queue_count, &ext_queue_count);
  TF_ASSERT(int_queue_count == 0);
  TF_ASSERT(ext_queue_count == 0);

  TF_ASSERT(stm_get_state(LTE_RRC_MEAS_SM) == LTE_RRC_MEAS_SM__IDLE_CAMPED);

  // Free the allocated sib heap memory
  lte_rrc_meas_test_free_sib_ptr();
  
  lte_rrc_utf_reset();
}/* LteRrcMeasBasicTrans::Test() */

void LteRrcMeasIgnorePendedSibIndiWhileTransistioningToInactiveState::Teardown()
{
  lte_rrc_utf_teardown();
}/* LteRrcMeasBasicTrans::Teardown() */

