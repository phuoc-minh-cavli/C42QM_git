/*!
  @file lte_rrc_capabilities_test_common.h

  @brief
  Utilities for Capability unit tests.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/capabilities/lte_rrc_capabilities_test_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/11/09 sureshs Initial version
===========================================================================*/

#ifndef LTE_RRC_CAPABILITIES_TEST_COMMON_H
#define LTE_RRC_CAPABILITIES_TEST_COMMON_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include <sys.h>
#include <lte_rrc_int_msg.h>
#include <lte_rrc_osys_asn1.h>
#include <lte_rrc_osys_asn1util.h>
}

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/* Initializes the stub required for Capabilities unit test  */
extern void lte_rrc_capabilities_test_stub_init(void);

/* Set the desired output of lte_rrc_csp_get_lte_band_preference */
extern void lte_rrc_capabilities_test_stub_csp_set_lte_band_preference
(
  sys_lte_band_mask_e_type lte_band_preference /*!< Desired LTE band preference */
);

/* Set the desired output of lte_rrc_csp_set_cdma_band_preference */
extern void lte_rrc_capabilities_test_stub_csp_set_do_band_preference
(
  sys_band_mask_type do_band_preference /*!< Desired CDMA band preference */
);

/* Set the desired output of lte_rrc_csp_set_cdma_band_preference */
extern void lte_rrc_capabilities_test_stub_csp_set_1x_band_preference
(
  sys_band_mask_type band_preference /*!< Desired CDMA band preference */
);

/* Set the desired output of lte_rrc_csp_set_wcdma_band_preference */
extern void lte_rrc_capabilities_test_stub_csp_set_wcdma_band_preference
(
  sys_band_mask_type wcdma_band_preference /*!< Desired WCDMA band preference */
);

/* Set the desired output of lte_rrc_csp_set_tdscdma_band_preference */
extern void lte_rrc_capabilities_test_stub_csp_set_tdscdma_band_preference
(
  sys_band_mask_type tdscdma_band_preference /*!< Desired TDSCDMA band preference */
);

/* Set the desired output of lte_rrc_csp_set_gsm_band_preference */
extern void lte_rrc_capabilities_test_stub_csp_set_gsm_band_preference
(
  sys_band_mask_type gsm_band_preference /*!< Desired GSM band preference */
);


/*Set cdma_do_supported flag*/
extern void lte_rrc_capabilities_test_set_cdma_do_is_supported(boolean my_flag);

extern lte_errno_e lte_rrc_cap_test_stub_csp_get_selected_plmn
(
  lte_rrc_plmn_s *selected_plmn_ptr,
  uint32 *selected_plmn_index_ptr
);

extern boolean lte_rrc_cap_test_stub_tds_rrc_rat_is_supported
(
  sys_plmn_id_s_type camped_plmn_id
);

/*Set w_supported flag*/
extern void lte_rrc_capabilities_test_set_w_is_supported(boolean my_flag);

/*Set tds_supported flag*/
extern void lte_rrc_capabilities_test_set_tds_is_supported(boolean my_flag);

/*Set gsm_supported flag*/
extern void lte_rrc_capabilities_test_set_gsm_is_supported(boolean my_flag);

/*Set cdma_1x_supported flag*/
extern void lte_rrc_capabilities_test_set_cdma_1x_is_supported(boolean my_flag);

/*Set controller state*/
extern void lte_rrc_capabilities_test_set_controller_state(stm_state_t controller_state);

/* Constructs a Capability Enquiry DLM by dynamic allocation */
extern void lte_rrc_capabilities_test_construct_cap_enq_msg
(
  lte_rrc_mh_dlm_s *msg_ptr /*!< Pointer to message */
);

#if 0
static uint8 cap_enquiry_1x_eutra_1[] = 
{
  0x3C, 0x04, 0x10 
};
#endif

#endif /* LTE_RRC_CAPABILITIES_TEST_COMMON_H */

