/*!
  @file lte_rrc_crp_test_common.h

  @brief
  Utilities for CRP unit tests.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/crp/lte_rrc_crp_test_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/13/08   sureshs Renamed some functions
09/12/2008 sureshs Initial version
===========================================================================*/

#ifndef LTE_RRC_CRP_TEST_COMMON_H
#define LTE_RRC_CRP_TEST_COMMON_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include <stm2.h>
}

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/* Initializes the stub required for CRP unit test  */
extern void lte_rrc_crp_test_stub_init();

/* Set the desired output of lte_rrc_controller_get_state */
extern void lte_rrc_crp_test_stub_controller_set_state
(
  stm_state_t state /*!< Desired state */
);

/* Set the desired output of lte_rrc_cep_get_state */
extern void lte_rrc_crp_test_stub_cep_set_state
(
  stm_state_t state /*!< Desired state */
);

/* Set the desired output of lte_rrc_csp_cdma_do_is_supported */
extern void lte_rrc_crp_test_stub_csp_set_do_is_supported
(
  boolean do_is_supported /*!< Supported or not */
);

/* Set the desired output of lte_rrc_csp_cdma_1x_is_supported */
extern void lte_rrc_crp_test_stub_csp_set_1x_is_supported
(
  boolean cdma_1x_is_supported /*!< Supported or not */
);

/* Set the desired output of lte_rrc_csp_gsm_is_supported */
extern void lte_rrc_crp_test_stub_csp_set_gsm_is_supported
(
  boolean gsm_is_supported /*!< Supported or not */
);

/* Set the desired output of lte_rrc_csp_wcdma_is_supported */
extern void lte_rrc_crp_test_stub_csp_set_wcdma_is_supported
(
  boolean wcdma_is_supported /*!< Supported or not */
);

extern lte_errno_e lte_rrc_crp_test_stub_csp_get_selected_plmn
(
  lte_rrc_plmn_s *selected_plmn_ptr, /*!< Pointer to selected PLMN to be filled
                                          in */
  uint32 *selected_plmn_index_ptr /*!< Pointer to selected PLMN index to be 
                                       filled in */
);

extern boolean lte_rrc_crp_test_stub_sib_b12_to_b17_conversion_is_needed
(
  void
);

extern boolean lte_rrc_crp_test_stub_sib_b12_to_b17_conversion_is_not_needed
(
  void
);

extern lte_3gpp_release_ver_e lte_rrc_crp_test_stub__get_3gpp_release_version
(
  void
);

#endif /* LTE_RRC_CRP_TEST_COMMON_H */

