
/*!
  @file
  lte_rrc_dt_test_common.cpp
  

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/dt/lte_rrc_dt_test_common.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

/*===========================================================================

INCLUDE FILES

===========================================================================*/

#include "lte_rrc_dt_test_common.h"

#include "TestFramework.h"

extern "C"
{

#include <lte.h>

}

/*===========================================================================

INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

static lte_rrc_dt_test_type_s lte_rrc_dt_test;



/*===========================================================================

LOCAL VARIABLES

===========================================================================*/


/*===========================================================================

INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

FUNCTIONS

===========================================================================*/

extern "C"
{
    TF_PROTO_STUB( stm_state_t, lte_rrc_controller_get_state, void);
    TF_PROTO_STUB( lte_errno_e, lte_rrc_csp_get_selected_plmn,
                   lte_rrc_plmn_s *,
                   uint32 *);
    TF_PROTO_STUB( lte_errno_e, lte_rrc_csp_get_camped_cell_tac, uint16 *);
    TF_PROTO_STUB( boolean, lte_rrc_llc_is_srb2_established, void);
}
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

void lte_rrc_test_dt_init(void)
{
    TF_STUB(lte_rrc_controller_get_state) = 
        lte_rrc_test_dt_stub_controller_get_state; 

    TF_STUB(lte_rrc_csp_get_selected_plmn) = 
        lte_rrc_test_dt_stub_get_selected_plmn;

    TF_STUB(lte_rrc_csp_get_camped_cell_tac) = 
        lte_rrc_test_dt_stub_get_camped_cell_tac;

    TF_STUB(lte_rrc_llc_is_srb2_established) = 
        lte_rrc_test_dt_stub_is_srb2_established;

}/* lte_rrc_test_dt_init*/

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

extern void lte_rrc_test_dt_teardown(void)
{
}/* lte_rrc_test_dt_teardown*/


/*===========================================================================*/
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
                                                   uint32 *plmn_index_ptr)
{   
    *plmn_index_ptr = 0;
    lte_rrc_dt_test.selected_plmn_index = *plmn_index_ptr;
    lte_rrc_dt_test.plmn_ptr = *plmn_ptr;
    return E_SUCCESS;
}/*lte_rrc_test_dt_stub_get_selected_plmn*/



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
lte_errno_e lte_rrc_test_dt_stub_get_camped_cell_tac(uint16 *tac_ptr)
{
    *tac_ptr = 0;
   return E_SUCCESS;
}


/*===========================================================================

FUNCTION:  lte_rrc_test_dt_stub_controller_get_state

===========================================================================*/
/*!
  @brief
  Stub function for lte_rrc_controller_get_state 

  @detail

  @return
  Returns the RRC controller state (in the DT test framework)

*/
/*=========================================================================*/
stm_state_t lte_rrc_test_dt_stub_controller_get_state()
{
    return ((stm_state_t)(lte_rrc_dt_test.rrc_controller_state));
}/*lte_rrc_test_dt_stub_controller_get_state*/


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

void lte_rrc_test_dt_set_controller_state(stm_state_t state)
{
    lte_rrc_dt_test.rrc_controller_state = state;
}

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
boolean lte_rrc_test_dt_stub_is_srb2_established( void )
{
   return lte_rrc_dt_test.srb2_established;
}

void lte_rrc_test_dt_set_srb2_status( boolean srb2_established )
{
  lte_rrc_dt_test.srb2_established = srb2_established;
}

