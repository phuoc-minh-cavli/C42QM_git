
/*!
  @file
  lte_rrc_sec_test_common.cpp
  

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sec/lte_rrc_sec_test_common.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/05/11   kp      Added changes for W2L PSHO feature.
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "lte_rrc_sec_test_common.h"
#include "lte_rrc_mh.h"
#include "qtf.h"
#include "qtf_tmc.h"
#include "qtf_rcinit.h"
#include "TestFramework.h"
#ifdef T_WINNT
#error code not present
#else
#define GetTickCount()  0
#endif // T_WINNT

extern "C"
{

#include "lte_static_rrc_mem_util.h"
#include <lte.h>
#include <lte_rrc_csp.h>
#include <lte_rrc_controller.h>

#include "task.h"

}

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================
                     Global security test encoded message
===========================================================================*/

/* This variable will be initialised by individual test cases so as to return 
   the current encoded message by stubbed out function 
   lte_rrc_sec_test_mh_get_last_encoded_message_rcvd
*/
lte_rrc_pdu_buf_s lte_rrc_sec_test_encoded_pdu;

static boolean is_non_L2W_HO = TRUE;


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
TF_PROTO_STUB( void, a2_ul_sec_write_keys, uint8, uint8 *, uint8, uint8 *, uint8, uint8 *);

TF_PROTO_STUB( lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s *);

TF_PROTO_STUB( lte_errno_e, lte_rrc_csp_get_camped_cell_identity, lte_rrc_global_cell_id_t *);

TF_PROTO_STUB(void, lte_rrc_mh_get_last_encoded_message_rcvd, lte_rrc_pdu_buf_s *);

TF_PROTO_STUB( stm_state_t, lte_rrc_controller_get_state, void);
extern void seccryptarm_task
( 
  dword dummy                            /* unused dummy var               */
);

}

// Must be static to not conflict with securemsm version
#if 0
static void yields(uint32 times)
{
  for (uint32 i=0; i<times; i++)
  {
    TF_YIELD();
  }
}
#endif

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
void lte_rrc_test_sec_stub_a2_write_keys(uint8 rrc_int_index,
                                    uint8 *k_rrc_int_ptr,
                                    uint8 rrc_enc_index,
                                    uint8 *k_rrc_enc_ptr,
                                    uint8 up_enc_index,
                                    uint8 *k_up_enc_ptr)
{
  /*Do nothing*/
  TF_ASSERT(rrc_enc_index < 16);
  TF_ASSERT(up_enc_index < 16);
  TF_ASSERT(rrc_int_index < 16);

  TF_ASSERT(NULL != k_rrc_int_ptr);
  TF_ASSERT(NULL != k_rrc_enc_ptr);
  TF_ASSERT(NULL != k_up_enc_ptr);


}
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
void lte_rrc_test_sec_stub_set_non_L2W_HO_flag
(
  boolean non_L2W_HO_flag
)
{
  TF_MSG("W2L PSHO, set is_non_L2W_HO = %d",non_L2W_HO_flag);
  is_non_L2W_HO = non_L2W_HO_flag;
}

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

void lte_rrc_test_sec_init(void)
{
#ifdef FEATURE_QTF_CRM_TODO
  rex_tcb_type	sec_tcb;
  rex_stack_word_type sec_stack [SEC_STACK_SIZ];
  rex_tcb_type         seccryptarm_tcb;
  rex_stack_word_type  seccryptarm_stack[ SECCRYPTARM_STACK_SIZ ];
#endif
#ifdef FEATURE_SEC
  #ifdef RRC_TEST_FRAMEWORK
  //TF_STUB(lte_rrc_csp_get_serving_cell_params) =
  //  lte_rrc_test_cep_stub_get_serving_cell_params;
    
   
#ifdef FEATURE_QTF_CRM_TODO
  qtf_tmc_spawn_rex_task("sec task", &sec_tcb,
                   (unsigned char *)sec_stack,
                   SEC_STACK_SIZ * 4,
                   (rex_priority_type) SEC_PRI,
                   sec_task,
                   0L);
#endif
  qtf_rcinit_add_task("sec_task");
  TF_YIELD();

#ifdef FEATURE_QTF_CRM_TODO
  qtf_tmc_spawn_rex_task("seccryptarm task", &seccryptarm_tcb,
                   (unsigned char *)seccryptarm_stack,
                   SECCRYPTARM_STACK_SIZ * 4,
                   (rex_priority_type) SECCRYPTARM_PRI,
                   seccryptarm_task,
                   0L);
#endif
  qtf_rcinit_add_task("seccryptarm");
  qtf_rcinit_start();
  TF_YIELD();

  //qtf_tmc_start_rex_task(&sec_tcb);
  //yields(5);
  //TF_YIELD();

 
  //qtf_tmc_start_rex_task(&seccryptarm_tcb);
  //yields(5);
  //TF_YIELD();

  TF_STUB(a2_ul_sec_write_keys) = lte_rrc_test_sec_stub_a2_write_keys;

  /*
  TF_STUB(lte_rrc_csp_get_serving_cell_params) =
    lte_rrc_test_sec_stub_get_serving_cell_params;

  TF_STUB(lte_rrc_csp_get_camped_cell_identity) =
    lte_rrc_test_sec_stub_get_camped_cell_identity;
    */
 #endif//RRC_TEST_FRAMEWORK
#endif//FEATURE_SEC
}/* lte_rrc_test_sec_init*/
/*===========================================================================

   FUNCTION:  lte_rrc_test_sec_init2

===========================================================================*/
/*!
    @brief
    Initializes the SEC tests

    @detail

    @return
    None

*/
/*=========================================================================*/

void lte_rrc_test_sec_init2(void)
{
  TF_STUB(lte_rrc_csp_get_serving_cell_params) =
    lte_rrc_test_sec_stub_get_serving_cell_params;

  TF_STUB(lte_rrc_csp_get_camped_cell_identity) =
    lte_rrc_test_sec_stub_get_camped_cell_identity;

  TF_STUB(lte_rrc_mh_get_last_encoded_message_rcvd) = 
	lte_rrc_sec_test_mh_get_last_encoded_message_rcvd;

  TF_STUB(lte_rrc_controller_get_state) = 
	  lte_rrc_sec_test_stub_controller_get_state;


}/* lte_rrc_test_sec_init2*/

/*===========================================================================

   FUNCTION:  lte_rrc_test_sec_teardown

===========================================================================*/
/*!
    @brief
    Close function for the SEC tests

    @detail

    @return
    None

*/
/*=========================================================================*/

extern void lte_rrc_test_sec_teardown(void)
{
  #ifdef RRC_TEST_FRAMEWORK
  //yields(5);
  TF_YIELD();
  qtf_rcinit_teardown();
  #endif

}/* lte_rrc_test_sec_teardown*/


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
extern stm_state_t lte_rrc_sec_test_stub_controller_get_state()
{
   if(is_non_L2W_HO == TRUE)
   {
	return LTE_RRC_CONTROLLER_SM__CONNECTED;
   }
   else
   {
    TF_MSG("W2L PSHO, return CONTROLLER state IRAT_TO_LTE_STARTED");
    return LTE_RRC_CONTROLLER_SM__IRAT_TO_LTE_STARTED;   
   }
}/*lte_rrc_sec_test_stub_controller_get_state*/


/*===========================================================================

   FUNCTION:  lte_rrc_test_sec_stub_get_serving_cell_params

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_sec_stub_get_serving_cell_params 

    @detail

    @return
    Populates the cell_info_ptr struct(in the SEC test framework)
    returns E_SUCCESS

*/
/*=========================================================================*/
extern lte_errno_e lte_rrc_test_sec_stub_get_serving_cell_params(lte_rrc_cell_info_s *cell_info_ptr)
{
  cell_info_ptr->sib_db_key = 0;
  cell_info_ptr->cell_id.freq = 0xf3ac;
  cell_info_ptr->cell_id.phy_cell_id = 0x1457;
  return E_SUCCESS;
}/*lte_rrc_cep_stub_get_serving_cell_params*/
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
extern lte_errno_e lte_rrc_test_sec_stub_get_serving_cell_params2(lte_rrc_cell_info_s *cell_info_ptr)
{
  cell_info_ptr->sib_db_key = 0;
  cell_info_ptr->cell_id.freq = 0xabcd;
  cell_info_ptr->cell_id.phy_cell_id = 0x0123;
  return E_SUCCESS;
}/*lte_rrc_cep_stub_get_serving_cell_params2*/

/*===========================================================================

   FUNCTION:  lte_rrc_test_sec_stub_get_camped_cell_identity

===========================================================================*/
/*!
    @brief
    Stub function for lte_rrc_sec_stub_get_camped_cell_identity 

    @detail

    @return
    Populates the global_cell_id_ptr struct(in the SEC test framework)
    returns E_SUCCESS

*/
/*=========================================================================*/
extern lte_errno_e lte_rrc_test_sec_stub_get_camped_cell_identity(lte_rrc_global_cell_id_t *global_cell_id_ptr)
{
  *global_cell_id_ptr = 0x0102; 
  return E_SUCCESS;
}/*lte_rrc_sec_stub_get_cell_identity*/

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
void lte_rrc_sec_test_mh_get_last_encoded_message_rcvd
(
  lte_rrc_pdu_buf_s *pdu_buf  /*! PDU buffer */
)
{
  *pdu_buf = lte_rrc_sec_test_encoded_pdu;
}
