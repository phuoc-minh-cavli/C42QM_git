/*!
  @file lte_rrc_dt_test_common.h

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
  By acdtting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/dt/lte_rrc_dt_test_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef LTE_RRC_DT_TEST_COMMON_H
#define LTE_RRC_DT_TEST_COMMON_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "__lte_rrc_dt.h"
#include "__lte_rrc_controller.h"
#include "stm2.h"
#include "lte_rrc_controller.h"
#include "lte_rrc_csp.h"
#include "lte.h"
}



#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include <tf_stub.h>

#define NUM_MAX_UMIDS 20

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief External type 
*/


/*! @brief  Internal structure common for all the 
 DT tests
*/

typedef struct
{
  stm_state_t rrc_controller_state;
  lte_rrc_plmn_s plmn_ptr;
  uint32 selected_plmn_index;
  boolean srb2_established;
} lte_rrc_dt_test_type_s ;




/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

   FUNCTION:  lte_rrc_test_dt_init

===========================================================================*/
/*!
    @brief
    Initializes the DT tests

    @detail

    @return
    None

*/
/*=========================================================================*/

extern void lte_rrc_test_dt_init();

/*===========================================================================

   FUNCTION:  lte_rrc_test_dt_teardown

===========================================================================*/
/*!
    @brief
    Close function for the DT tests

    @detail

    @return
    None

*/
/*=========================================================================*/

extern void lte_rrc_test_dt_teardown();

/*===========================================================================

   FUNCTION:  lte_rrc_test_dt_stub_controller_get_state

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_test_dt_stub_controller_get_state 

    @detail

    @return
    Returns the RRC controller state (in the DT test framework)

*/
/*=========================================================================*/

extern void lte_rrc_test_dt_set_controller_state(stm_state_t state);

/*===========================================================================

   FUNCTION:  lte_rrc_test_dt_set_controller_state

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_set_controller_state 

    @detail

    @return
    Returns the RRC controller state (in the DT test framework)

*/
/*=========================================================================*/


extern stm_state_t lte_rrc_test_dt_stub_controller_get_state();


/*===========================================================================

   FUNCTION:  lte_rrc_test_dt_stub_get_selected_plmn

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_test_dt_stub_get_selected_plmn 

    @detail

    @return
    Sets the selected PLMN identity
    returns E_SUCCESS

*/
/*=========================================================================*/
lte_errno_e lte_rrc_test_dt_stub_get_selected_plmn(lte_rrc_plmn_s *plmn_ptr,
                                                    uint32 *plmn_index_ptr);

/*===========================================================================

   FUNCTION:  lte_rrc_test_dt_stub_get_camped_cell_tac

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_test_dt_stub_get_camped_cell_tac 

    @detail

    @return
    the TAC of the camped cell
    returns E_SUCCESS

*/
/*=========================================================================*/
lte_errno_e lte_rrc_test_dt_stub_get_camped_cell_tac(uint16 *tac_ptr);

/*===========================================================================

FUNCTION:  lte_rrc_test_dt_stub_is_srb2_established

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_llc_is_srb2_established 

    @detail

    @return
    the TAC of the camped cell
    returns E_SUCCESS

*/
/*=========================================================================*/
extern boolean lte_rrc_test_dt_stub_is_srb2_established( void );

extern void lte_rrc_test_dt_set_srb2_status( boolean srb2_established );

#endif /* LTE_RRC_DT_TEST_COMMON_H */
