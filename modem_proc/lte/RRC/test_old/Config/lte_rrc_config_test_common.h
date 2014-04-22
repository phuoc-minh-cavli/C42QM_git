/*!
  @file lte_rrc_config_test_common.h

  @brief
  Utilities for Config unit tests.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/Config/lte_rrc_config_test_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/09/09   mm     Initial Revision
===========================================================================*/

#ifndef LTE_RRC_CONFIG_TEST_COMMON_H
#define LTE_RRC_CONFIG_TEST_COMMON_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include <stm2.h>
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "__lte_rrc_config.h"
#include "lte_rrc_utils.h"
#include "lte_rrc_configi.h"
#include "lte_rrc_cep.h"
#include "lte_rrc_osys_asn1util.h"
}

/*===========================================================================

                          EXTERNAL VARIABLES

===========================================================================*/

extern lte_rrc_pdu_buf_s lte_rrc_config_test_encoded_pdu;

extern byte invalid_crit_ext_msg[1];

extern byte invalid_release_msg[2];

extern byte invalid_msg_class_ext[1];

extern byte non_reconfig_msg[3];

extern byte invalid_reconfig_sec_no_mobility_msg[5];

extern byte invalid_reconfig_nas_wo_drb_msg[9];

extern byte reconfig_rrc_msg[6];

extern byte reconfig_meas_nas_info_msg[34];

extern byte reconfig_with_drb_rel_msg[6];

extern byte reconfig_with_ho_only_msg[42];

extern byte reconfig_with_ho_full_config_msg[88];

extern byte reconfig_with_ho_with_meas_msg[45];

extern byte invalid_mobility_info_with_nas_ded_msg[41];

extern byte reconfig_with_ho_delta_config_msg[18];

extern byte reconfig_msg_ho_without_security[37];

extern byte reconfig_with_empty_ies[3];

extern byte reconfig_msg_invalid_t304[39];

extern byte reconfig_meas_info_msg[8];

extern byte reconfig_prox_indi_only_msg[4];

extern byte reconfig_meas_info_with_prox_cfg_msg[8];

extern byte reconfig_meas_info_with_prox_cfg_msg1[8];

extern byte reconfig_meas_info_with_prox_cfg_msg2[8];

extern byte mobility_from_eutra_cco[5];

extern byte mobility_from_eutra_to_w[6];

extern byte mobility_from_eutra_to_1x[5];

extern byte mobility_from_eutra_for_ecsfb[7];

extern byte mobility_from_eutra_cco_rel9[5];

extern byte mobility_from_eutra_to_1x_rel9[5];

extern byte mobility_from_eutra_to_w_rel9[6];

extern byte mobility_from_eutra_to_spare4[5];

extern byte reconfig_meas_nas_info_msg_zero_length[24];

extern byte ho_to_eutra_msg[59];

extern byte ho_invalid_msg[25];

extern byte reconfig_ca_scell_add[187];

extern byte reconfig_ca_scell_modify[175];

extern byte reconfig_ca_scell_release[4];

extern byte reconfig_with_ho_pcell_to_pcell_scell[108];

extern byte reconfig_ca_scell_change[188];

extern byte reconfig_ca_scell_add_configure_a1[194];

extern void lte_rrc_config_test_stub_init
( 
  void 
);

extern stm_state_t lte_rrc_config_test_get_controller_state
(
  void
);

extern lte_errno_e lte_rrc_config_test_csp_get_serving_cell_params
(
  lte_rrc_cell_info_s *cell_info_ptr 
);

extern lte_errno_e lte_rrc_config_test_csp_get_camped_cell_bw
(
  lte_bandwidth_e *bw_ptr 
);

extern boolean lte_rrc_config_test_is_sec_activated
( 
  void 
);

extern lte_errno_e lte_rrc_config_test_sec_validate_config
(
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_ptr  /*! Pointer to DL DCCH msg */
);

extern void lte_rrc_config_test_mh_get_last_encoded_message_rcvd
(
  lte_rrc_pdu_buf_s *pdu_buf  /*! PDU buffer ptr */
);

extern lte_errno_e lte_rrc_config_meas_cfg_test_validate_meas_param
(
  /*! Pointer to decoded ASN.1 DL DCCH Message */
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr,
  /*! TRUE if Reconfig DLM also includes MobilityControlInformation IE */
  boolean is_part_of_handover
);

extern stm_state_t lte_rrc_config_test_get_cre_state
( 
  void
);

extern boolean lte_rrc_config_meas_test_is_sec_activated
(
  void
);

extern boolean lte_rrc_config_prox_cfg_allowed
(
  void
);

extern uint8 lte_rrc_config_test_cap_get_fgi_bit_0
(
  uint8 bit_pos
);

extern lte_3gpp_release_ver_e lte_rrc_config_test_get_3gpp_rel_version_for_rel9
(
  void
);

extern lte_rrc_est_cause_e lte_rrc_config_test_get_est_cause_mos
(
  void
);

#endif /* LTE_RRC_CONFIG_TEST_COMMON_H */


