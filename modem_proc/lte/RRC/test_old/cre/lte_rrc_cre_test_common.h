/*!
  @file lte_rrc_cre_test_common.h

  @brief
  Utilities for cre unit tests.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/cre/lte_rrc_cre_test_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/08   mm     Initial Revision
===========================================================================*/

#ifndef LTE_RRC_CRE_TEST_COMMON_H
#define LTE_RRC_CRE_TEST_COMMON_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include <stm2.h>
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "lte_rrc_osys_asn1util.h"
}

/*===========================================================================

                           EXTERNAL TYPE DEFINITIONS

===========================================================================*/
extern byte cre_reestablishment_msg[17];

extern lte_rrc_pdu_buf_s lte_rrc_cre_test_pdu_buf;

/*===========================================================================

                           EXTERNAL FUNCTIONS PROTOTYPES

===========================================================================*/
extern stm_state_t lte_rrc_cre_test_get_controller_state
(
  void
);

extern lte_errno_e lte_rrc_cre_test_csp_get_serving_cell_params
(
  lte_rrc_cell_info_s *cell_info_ptr /*!< Pointer to cell info */
);

extern boolean lte_rrc_test_stub_get_camped_cell_info
(
  lte_rrc_plmn_s *selected_plmn_ptr,
   /*!< Ptr to selected PLMN to be filled*/
  lte_earfcn_t *dl_earfcn_ptr, /*!< Ptr to DL Center frequency of cell */
  lte_phy_cell_id_t *phy_cell_id_ptr,/*!< Ptr to Physical cell id of cell */
  lte_rrc_global_cell_id_t *cell_identity_ptr, 
  /*!< Ptr to Global cell id (from SIB1) of cell */
  uint16 *tac_ptr /*!< Ptr to Tracking area code of cell */
);

extern boolean lte_rrc_test_stub_get_camped_cell_info_2
(
  lte_rrc_plmn_s *selected_plmn_ptr,
   /*!< Ptr to selected PLMN to be filled*/
  lte_earfcn_t *dl_earfcn_ptr, /*!< Ptr to DL Center frequency of cell */
  lte_phy_cell_id_t *phy_cell_id_ptr,/*!< Ptr to Physical cell id of cell */
  lte_rrc_global_cell_id_t *cell_identity_ptr, 
  /*!< Ptr to Global cell id (from SIB1) of cell */
  uint16 *tac_ptr /*!< Ptr to Tracking area code of cell */
);

extern boolean lte_rrc_test_stub_get_camped_cell_info_3
(
  lte_rrc_plmn_s *selected_plmn_ptr,
   /*!< Ptr to selected PLMN to be filled*/
  lte_earfcn_t *dl_earfcn_ptr, /*!< Ptr to DL Center frequency of cell */
  lte_phy_cell_id_t *phy_cell_id_ptr,/*!< Ptr to Physical cell id of cell */
  lte_rrc_global_cell_id_t *cell_identity_ptr, 
  /*!< Ptr to Global cell id (from SIB1) of cell */
  uint16 *tac_ptr /*!< Ptr to Tracking area code of cell */
);

extern boolean lte_rrc_test_stub_get_camped_cell_info_4
(
  lte_rrc_plmn_s *selected_plmn_ptr,
   /*!< Ptr to selected PLMN to be filled*/
  lte_earfcn_t *dl_earfcn_ptr, /*!< Ptr to DL Center frequency of cell */
  lte_phy_cell_id_t *phy_cell_id_ptr,/*!< Ptr to Physical cell id of cell */
  lte_rrc_global_cell_id_t *cell_identity_ptr, 
  /*!< Ptr to Global cell id (from SIB1) of cell */
  uint16 *tac_ptr /*!< Ptr to Tracking area code of cell */
);

extern boolean lte_rrc_test_stub_get_camped_cell_info_5
(
  lte_rrc_plmn_s *selected_plmn_ptr,
   /*!< Ptr to selected PLMN to be filled*/
  lte_earfcn_t *dl_earfcn_ptr, /*!< Ptr to DL Center frequency of cell */
  lte_phy_cell_id_t *phy_cell_id_ptr,/*!< Ptr to Physical cell id of cell */
  lte_rrc_global_cell_id_t *cell_identity_ptr, 
  /*!< Ptr to Global cell id (from SIB1) of cell */
  uint16 *tac_ptr /*!< Ptr to Tracking area code of cell */
);

extern uint16 lte_rrc_test_stub_get_rlf_window_size
(
   void
);

extern uint32 lte_test_stub_get_current_time_since_power_up_secs
(
   void
);
extern uint32 lte_test_stub_get_current_time_since_power_up_secs_2
(
   void
);
extern uint32 lte_test_stub_get_current_time_since_power_up_secs_3
(
   void
);
extern uint32 lte_test_stub_get_current_time_since_power_up_secs_4
(
   void
);
extern uint32 lte_test_stub_get_current_time_since_power_up_secs_5
(
   void
);
extern uint32 lte_test_stub_get_current_time_since_power_up_secs_6
(
   void
);

extern void lte_rrc_cre_test_llc_get_source_cell_info
(
  lte_rrc_cell_info_s *cell_info_ptr /*!< Pointer to cell info */
);

extern void *lte_rrc_cre_basic_sib2_update_test_get_serv_cell_sib
(
  lte_rrc_sib_bitmask_t sib, /*!< Sib requested; 1=SIB1, 2=SIB2, ...*/
  lte_rrc_sib_db_key_t key /*!< sib db index */
);

extern lte_rnti_val_t lte_rrc_cre_test_get_crnti
(
  void
);

extern lte_errno_e lte_rrc_cre_test_update_crnti_in_cfgdb
(
  lte_rnti_val_t crnti_val /*!< CRNTI value */
);

extern uint16 lte_rrc_cre_test_get_shortmaci
(
 lte_phy_cell_id_t phy_cell_id,  /*!< Physical cell id of the previous cell*/ 
 lte_rnti_val_t c_rnti  /*C-RNTI the UE had on the previous cell*/
);

extern boolean lte_rrc_cre_test_is_sec_activated
(
  void
);

extern lte_rrc_release_e lte_rrc_cre_test_get_release_version
(
  void
);

extern lte_rrc_release_e lte_rrc_cre_test_get_release_version_for_2_0
(
  void
);

extern void lte_rrc_cre_test_free_sib_ptr
(
  void
);

extern boolean lte_rrc_cre_test_ho_in_progress
(
  void
);

extern stm_state_t lte_rrc_cre_config_get_state_for_meas_reapply
(
  void
);

extern stm_state_t lte_rrc_cre_config_get_state_for_meas_revert
(
  void
);

extern boolean lte_rrc_cre_test_config_rb_reconfig_in_progress
(
  void
);

extern boolean lte_rrc_cre_test_config_rb_reconfig_not_in_progress
(
  void
);

extern boolean lte_rrc_cre_test_config_meas_reconfig_in_progress
(
  void
);

extern boolean lte_rrc_cre_test_config_meas_reconfig_not_in_progress
(
  void
);

extern boolean lte_rrc_cre_test_llc_dedicated_rlf_timers_configured
(
  lte_rrc_rlf_tmr_s *rlf_tmr_ptr
);

extern boolean lte_rrc_cre_test_llc_dedicated_rlf_timers_not_configured
(
  lte_rrc_rlf_tmr_s *rlf_tmr_ptr
);

extern void lte_rrc_cre_test_stub_init
(
  void
);

extern boolean lte_rrc_config_test_ho_to_eutra_in_progress
(
   void
);

extern void lte_rrc_cre_test_reestablishment
(
  void
);


#endif /* LTE_RRC_CRE_TEST_COMMON_H */



