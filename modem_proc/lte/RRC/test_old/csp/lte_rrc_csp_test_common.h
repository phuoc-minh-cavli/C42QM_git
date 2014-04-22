/*!
  @file lte_rrc_csp_test_common.h

  @brief
  Utilities for CSP unit tests.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/csp/lte_rrc_csp_test_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/09   sureshs Support for lte_rrc_get_release_version stub
10/23/08   sureshs Support for lte_rrc_controller_get_access_class stub
10/09/08   sureshs Added functions to construct and free SIB1
10/08/08 sureshs Initial version
===========================================================================*/

#ifndef LTE_RRC_CSP_TEST_COMMON_H
#define LTE_RRC_CSP_TEST_COMMON_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include <stm2.h>
#include <lte_rrc_utils.h>
#include <__lte_rrc_csp.h>
}
#include "lte_rrc_test_integration_common.h"
#include "lte_rrc_test_sib_cell_db.h"

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

typedef struct lte_rrc_csp_validate_system_scan_req_s
{
  
  sys_sband_lte_e_type  band;
  lte_bandwidth_e		bandwidth;
  lte_earfcn_t			earfcn;
}lte_rrc_csp_validate_system_scan_req_s;

typedef struct lte_rrc_csp_send_band_scan_cnf_s
{
  sys_sband_lte_e_type    band;
  uint16                  num_candidates;
  lte_cphy_scan_result_s  candidates[LTE_CPHY_BANDSCAN_NUM_CANDIDATES];
  lte_cphy_band_scan_req_unusable_systems_s unusable_systems;
}lte_rrc_csp_send_band_scan_cnf_s;

typedef struct lte_rrc_csp_test_cfg_req_param_s
{
  uint32                trans_id;
  uint8			        num_systems;
  sys_sband_lte_e_type  band;
  lte_earfcn_t          earfcn;
  lte_bandwidth_e       bandwidth;
  lte_phy_cell_id_t     phy_cell_id;
  uint8                 num_black_cells;
  int16                 energy;
  uint16                num_candidates;
  errno_enum_type       status;
}lte_rrc_csp_test_cfg_req_param_s;

/* Initializes the stub required for CSP unit test  */
extern void lte_rrc_csp_test_stub_init();

/* Set the desired vallue of Repeated sys scan for RLF recovery EFS */
extern  void lte_rrc_csp_test_efs_rlf_sys_scan_duration_write(uint32 duration);

/* Set the desired output of lte_rrc_controller_get_state */
extern void lte_rrc_csp_test_stub_controller_set_state
(
  stm_state_t state
);

/* Set the desired output of lte_rrc_cre_get_state */
extern void lte_rrc_csp_test_stub_cre_set_state
(
  stm_state_t state
);

/* Set the desired output of lte_rrc_cre_get_state */
extern void lte_rrc_csp_test_stub_cre_get_state
(
  stm_state_t state
);

/* Set the desired output of lte_rrc_controller_get_access_class  */
extern void lte_rrc_csp_test_stub_controller_set_access_class
(
  uint16 access_class
);

/* Set the desired output of lte_rrc_get_3gpp_release_version  */
extern void lte_rrc_csp_test_stub_set_3gpp_release_version
(
    lte_3gpp_release_ver_e rel_ver
);

/* Set the memory for band scan results in lte_cphy_band_scan_cnf_s */
extern void lte_rrc_csp_test_set_band_scan_result_ptr
(
  lte_cphy_band_scan_cnf_s *bs_cnf_ptr
);

/* Set num_tx_antennas in MIB */
extern void lte_rrc_csp_test_set_num_tx_antennas
(
  uint8 num_tx_antennas
);

/* Construct a basic SIB1 message */
extern void lte_rrc_csp_test_construct_basic_sib1(void);

/* Construct a basic SIB1 message */
extern void lte_rrc_csp_test_construct_basic_sib1_bandChgFly
(
   sys_sband_lte_e_type band,
   boolean intrFreqSelectionNotAllowed,
   boolean cellNotBarred
);

/* Construct a basic SIB1 with SIB 8 scheduled message */
extern void lte_rrc_csp_test_construct_sib1_with_sib8(void);

/* Construct a basic SIB1 message */
extern void lte_rrc_csp_test_free_sib1_pdu(void);

/* Return SIB1 PDU pointer */
extern void *lte_rrc_csp_test_get_sib1_pdu_ptr(void);

/* Construct a basic SIB8 message */
extern void lte_rrc_csp_test_construct_basic_sib8(void);

/* Construct a basic SIB8 message */
extern void lte_rrc_csp_test_free_sib8_pdu(void);

/* Return SIB8 PDU pointer */
extern void *lte_rrc_csp_test_get_sib8_pdu_ptr(void);

/* Construct a basic SIB6 message */
extern void lte_rrc_csp_test_construct_basic_sib6(void);

/* Construct a basic SIB6 message */
extern void lte_rrc_csp_test_free_sib6_pdu(void);

/* Return SIB6 PDU pointer */
extern void *lte_rrc_csp_test_get_sib6_pdu_ptr(void);

extern uint16 lte_rrc_csp_test_stub_controller_get_access_class(void);

extern  void lte_rrc_csp_test_handle_rrc_cfg_req
  (
 	lte_rrc_csp_test_cfg_req_param_s        *cfg_req_param,
    lte_rrc_csp_validate_system_scan_req_s  *validate_system_scan,
	lte_rrc_csp_send_band_scan_cnf_s       *send_band_scan_cnf

  );

extern byte	*lte_rrc_csp_test_handle_rrc_cfg_rsp(lte_rrc_cfg_type_e ,uint32 ,int16 ,
	sys_sband_lte_e_type  ,lte_bandwidth_e ,lte_earfcn_t,lte_phy_cell_id_t );
  
#endif /* LTE_RRC_CSP_TEST_COMMON_H */

