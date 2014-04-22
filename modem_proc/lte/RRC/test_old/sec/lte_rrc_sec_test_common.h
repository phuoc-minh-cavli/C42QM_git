/*!
  @file lte_rrc_sec_test_common.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sec/lte_rrc_sec_test_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/05/11   kp      Added changes for W2L PSHO feature.
===========================================================================*/

#ifndef LTE_RRC_SEC_TEST_COMMON_H
#define LTE_RRC_SEC_TEST_COMMON_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include "__lte_rrc_sec.h"
#include "lte_rrc_sec.h"
#include "lte_rrc_mh.h"
#include "stm2.h"
#include "lte_rrc_csp.h"
#include <a2_ul_sec.h>
#include <lte_rrc_osys_asn1.h>
#include <lte_rrc_osys_asn1util.h>
#include "lte.h"
}



#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include <tf_stub.h>



/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief External type 
*/
extern lte_rrc_pdu_buf_s lte_rrc_sec_test_encoded_pdu;

/*! @brief  Internal structure common for all the 
 CEP tests
*/


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

   FUNCTION:  lte_rrc_test_sec_init

===========================================================================*/
/*!
    @brief
    Initializes the SEC tests

    @detail

    @return
    None

*/
/*=========================================================================*/

extern void lte_rrc_test_sec_init();
/*===========================================================================

   FUNCTION:  lte_rrc_test_sec_init2

===========================================================================*/
/*!
    @brief
    Initializes the SEC tests-2

    @detail

    @return
    None

*/
/*=========================================================================*/

extern void lte_rrc_test_sec_init2();

/*===========================================================================

   FUNCTION:  lte_rrc_test_sec_teardown

===========================================================================*/
/*!
    @brief
    Teardown function for the SEC tests

    @detail

    @return
    None

*/
/*=========================================================================*/

extern void lte_rrc_test_sec_teardown();

/*===========================================================================

   FUNCTION:  lte_rrc_test_sec_stub_a2_write_keys

===========================================================================*/
/*!
    @brief
    Stub function for writing keys into A2
    @detail

    @return
    None

*/
/*=========================================================================*/
extern void lte_rrc_test_sec_stub_a2_write_keys(uint8 rrc_int_index,
                                    uint8 *k_rrc_int_ptr,
                                    uint8 rrc_enc_index,
                                    uint8 *k_rrc_enc_ptr,
                                    uint8 rrc_up_enc_index,
                                    uint8 *k_rrc_up_enc_ptr) ;

/*===========================================================================

   FUNCTION:  lte_rrc_test_cep_stub_get_serving_cell_params

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_test_sec_stub_get_serving_cell_params 

    @detail

    @return
    Populates the cell_info_ptr struct(in the SEC test framework)
    returns E_SUCCESS

*/
/*=========================================================================*/
extern lte_errno_e lte_rrc_test_sec_stub_get_serving_cell_params(lte_rrc_cell_info_s *cell_info_ptr);

/*===========================================================================

   FUNCTION:  lte_rrc_test_sec_stub_get_serving_cell_params2

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_sec_stub_get_serving_cell_params2 

    @detail

    @return
    Populates the cell_info_ptr struct(in the SEC test framework)
    returns E_SUCCESS

*/
/*=========================================================================*/
extern lte_errno_e lte_rrc_test_sec_stub_get_serving_cell_params2(lte_rrc_cell_info_s *cell_info_ptr);

/*===========================================================================

   FUNCTION:  lte_rrc_test_sec_stub_get_camped_cell_identity

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_test_sec_stub_get_camped_cell_identity

    @detail

    @return
    Populates the global_cell_id_ptr struct(in the SEC test framework)
    returns E_SUCCESS

*/
/*=========================================================================*/
extern lte_errno_e lte_rrc_test_sec_stub_get_camped_cell_identity(lte_rrc_global_cell_id_t *global_cell_id_ptr);

/*===========================================================================

  FUNCTION:  lte_rrc_sec_test_mh_get_last_encoded_message_rcvd

===========================================================================*/
/*!
    @brief
    This function allows access to the last encoded message recieved on the
    downlink by the MH module

    @return
    void

 */
/*=========================================================================*/
extern void lte_rrc_sec_test_mh_get_last_encoded_message_rcvd
(
  lte_rrc_pdu_buf_s *pdu_buf  /*! PDU buffer ptr */
);

/*===========================================================================

   FUNCTION:  lte_rrc_test_sec_test_stub_controller_get_state

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_controller_get_state 

    @detail

    @return
    Returns the state of RRC controller
    returns  LTE_RRC_CONTROLLER_SM__CONNECTED

*/
/*=========================================================================*/
extern stm_state_t lte_rrc_sec_test_stub_controller_get_state(void);
/*===========================================================================

   FUNCTION:  lte_rrc_test_sec_stub_set_non_L2W_HO_flag

===========================================================================*/
/*!
    @brief
    Stub function set L2W HO flag
    @detail

    @return
    None

*/
/*=========================================================================*/
extern void lte_rrc_test_sec_stub_set_non_L2W_HO_flag
(
  boolean L2W_HO_flag
);

#endif /* LTE_RRC_SEC_TEST_COMMON_H */
