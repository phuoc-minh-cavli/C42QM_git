/*!
  @file lte_rrc_paging_test_common.cpp

  @brief
  Common routines for paging unit tests.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/paging/lte_rrc_paging_test_common.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/17/08   ask     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <TestFramework.h>
#include <tf_stub.h>

extern "C"
{
#include <lte.h>
#include <lte_rrc_int_msg.h>
#include <lte_rrc_csp.h>
#include <lte_rrc_sib.h>
#include <lte_rrc_osys_asn1util.h>
#include <lte_rrc_utils.h>
#include "__lte_rrc_controller.h"
#include "__lte_rrc_cre.h"
}

#include "lte_rrc_utf.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

lte_rrc_osys_SystemInformationBlockType2 lte_rrc_paging_test_sib2;
boolean lte_rrc_paging_test_config_ho_in_progress;
stm_state_t lte_rrc_paging_test_controller_state;
boolean lte_rrc_paging_test_cre_rlf_search_in_progress;
lte_rrc_revoke_priority_e lte_rrc_paging_test_revoke_priority;
boolean lte_rrc_paging_test_controller_ue_in_dsds_mode;
boolean lte_rrc_paging_test_srb2_setup;

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern "C"
{
  TF_PROTO_STUB(lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s *);
  TF_PROTO_STUB(void*, lte_rrc_sib_get_cell_sib, lte_rrc_sib_bitmask_t, lte_rrc_sib_db_key_t);
  TF_PROTO_STUB(boolean, lte_rrc_config_ho_in_progress, void);
  TF_PROTO_STUB(stm_state_t, lte_rrc_controller_get_state, void);
  TF_PROTO_STUB(boolean, lte_rrc_cre_rlf_search_in_progress, void);
  TF_PROTO_STUB(lte_rrc_revoke_priority_e, lte_rrc_get_revoke_priority, lte_rrc_revoke_trigger_e);
  TF_PROTO_STUB(boolean, lte_rrc_llc_is_srb2_established, void);
}

lte_errno_e lte_rrc_paging_test_stub__csp_get_serving_cell_params
(
  lte_rrc_cell_info_s *cell_info_ptr
);

void* lte_rrc_paging_test_stub__sib_get_cell_sib
(
  lte_rrc_sib_bitmask_t sib_bitmask, /*!< Sib requested; LTE_RRC_SIB1_BITMASK=SIB1, LTE_RRC_SIB2_BITMASK=SIB2, ...*/
  lte_rrc_sib_db_key_t sib_db_key /*!< Index in sib_db of the cell */
);

boolean lte_rrc_paging_test_stub__config_ho_in_progress
(
  void
);

stm_state_t lte_rrc_paging_test_stub__controller_get_state
(
  void
);

boolean lte_rrc_paging_test_stub__cre_rlf_search_in_progress
(
  void
);

lte_rrc_revoke_priority_e lte_rrc_paging_test_stub__get_revoke_priority
(
  lte_rrc_revoke_trigger_e revoke_trigger
);

boolean lte_rrc_paging_test_stub__controller_ue_in_dsds_mode
(
  void
);

boolean lte_rrc_paging_test_stub__llc_is_srb2_established
(
  void
)
{
  return lte_rrc_paging_test_srb2_setup;
}

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_rrc_paging_test_init

===========================================================================*/
/*!
  @brief
  Initializes the tf proto stubs.

  @return
  None
*/
/*=========================================================================*/
void lte_rrc_paging_test_init
(
  void
)
{

  /*-----------------------------------------------------------------------*/

  /*-----------------------------------------------------------------------*/

  TF_STUB(lte_rrc_csp_get_serving_cell_params) =
    lte_rrc_paging_test_stub__csp_get_serving_cell_params;

  TF_STUB(lte_rrc_sib_get_cell_sib) =
    lte_rrc_paging_test_stub__sib_get_cell_sib;

  TF_STUB(lte_rrc_config_ho_in_progress) =
    lte_rrc_paging_test_stub__config_ho_in_progress;

  TF_STUB(lte_rrc_controller_get_state) =
    lte_rrc_paging_test_stub__controller_get_state;

  TF_STUB(lte_rrc_cre_rlf_search_in_progress) =
    lte_rrc_paging_test_stub__cre_rlf_search_in_progress;

  TF_STUB(lte_rrc_get_revoke_priority) = 
    lte_rrc_paging_test_stub__get_revoke_priority;

  TF_STUB(lte_rrc_llc_is_srb2_established) = 
    lte_rrc_paging_test_stub__llc_is_srb2_established;

  lte_rrc_paging_test_config_ho_in_progress = FALSE;
  lte_rrc_paging_test_controller_state = LTE_RRC_CONTROLLER_SM__IDLE_CAMPED;
  lte_rrc_paging_test_cre_rlf_search_in_progress = FALSE;
  lte_rrc_paging_test_controller_ue_in_dsds_mode = FALSE;
  lte_rrc_paging_test_srb2_setup = TRUE;
} /* lte_rrc_paging_test_init() */



/*===========================================================================

  FUNCTION:  lte_rrc_paging_test_stub__csp_get_serving_cell_params

===========================================================================*/
/*!
  @brief
  Stub func for csp_get_serving_cell_params

  @return
  Status code.
*/
/*=========================================================================*/
lte_errno_e lte_rrc_paging_test_stub__csp_get_serving_cell_params
(
  lte_rrc_cell_info_s *cell_info_ptr
)
{
  /*-----------------------------------------------------------------------*/

  TF_ASSERT(cell_info_ptr != NULL);

  /*-----------------------------------------------------------------------*/

  // Fill in some bogus values
  cell_info_ptr->cell_id.phy_cell_id = 0x5678;
  cell_info_ptr->cell_id.freq = 0x2345;

  return E_SUCCESS;

} /* lte_rrc_paging_test_stub__csp_get_serving_cell_params() */


/*===========================================================================

  FUNCTION:  lte_rrc_paging_test_stub__sib_get_cell_sib

===========================================================================*/
/*!
  @brief
  Stub function for sib_get_cell_sib

  @return
  Ptr to sib requested
*/
/*=========================================================================*/
void* lte_rrc_paging_test_stub__sib_get_cell_sib
(
  lte_rrc_sib_bitmask_t sib_bitmask, /*!< Sib requested; LTE_RRC_SIB1_BITMASK=SIB1, LTE_RRC_SIB2_BITMASK=SIB2, ...*/
  lte_rrc_sib_db_key_t sib_db_key /*!< Index in sib_db of the cell */
)
{

  /*-----------------------------------------------------------------------*/

  TF_ASSERT(sib_bitmask == LTE_RRC_SIB2_BITMASK);

  /*-----------------------------------------------------------------------*/

  // Set the values of nb and default_paging_cycle
  lte_rrc_paging_test_sib2.radioResourceConfigCommon.pcch_Config.nB = 
    lte_rrc_osys_PCCH_Config_nB_halfT;

  lte_rrc_paging_test_sib2.radioResourceConfigCommon.pcch_Config.defaultPagingCycle = 
    lte_rrc_osys_PCCH_Config_defaultPagingCycle_rf128;

  return &lte_rrc_paging_test_sib2;

} /* lte_rrc_paging_test_stub__sib_get_cell_sib() */


/*===========================================================================

  FUNCTION:  lte_rrc_paging_test_stub__config_ho_in_progress

===========================================================================*/
/*!
  @brief
  Stub function for config_ho_in_progress

  @return
  Boolean indicating HO is in progress or not
*/
/*=========================================================================*/
boolean lte_rrc_paging_test_stub__config_ho_in_progress
(
)
{

  return lte_rrc_paging_test_config_ho_in_progress;

} /* lte_rrc_paging_test_stub__config_ho_in_progress() */


stm_state_t lte_rrc_paging_test_stub__controller_get_state
(
)
{
  return lte_rrc_paging_test_controller_state;
}

boolean lte_rrc_paging_test_stub__cre_rlf_search_in_progress
(
)
{
  return lte_rrc_paging_test_cre_rlf_search_in_progress;
}

lte_rrc_revoke_priority_e lte_rrc_paging_test_stub__get_revoke_priority
(
  lte_rrc_revoke_trigger_e revoke_trigger
)
{
  return lte_rrc_paging_test_revoke_priority;
}

boolean lte_rrc_paging_test_stub__controller_ue_in_dsds_mode
(
)
{
  return lte_rrc_paging_test_controller_ue_in_dsds_mode;
}


void lte_rrc_paging_test_send_get_sibs_reqi()
{
  lte_rrc_get_sibs_reqi_s get_sibs_reqi;

  // Prepare get_sibs_reqi to send
  lte_rrc_init_default_hdr(&get_sibs_reqi.msg_hdr, LTE_RRC_GET_SIBS_REQI);

  // Send get_sibs_reqi
  lte_rrc_utf_send_msg((byte*)&get_sibs_reqi, sizeof(get_sibs_reqi));
  lte_rrc_utf_wait_for_done();

}

/*===========================================================================

                                UNIT TEST

===========================================================================*/
