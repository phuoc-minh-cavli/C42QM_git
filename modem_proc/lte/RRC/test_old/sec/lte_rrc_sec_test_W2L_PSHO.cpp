/*!
  @file lte_rrc_sec_test_W2L_PSHO.cpp


  @brief
  Unit test code for SMC 

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sec/lte_rrc_sec_test_W2L_PSHO.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/12/11   kp      Added code for W2L PSHO
12/05/11   kp     Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_utils.h"
#include "__lte_rrc_sec.h"
#include "lte_rrc_sec.h"
#include "lte_rrc_seci.h"
#include "__lte_rrc_controller.h"
#include "stm2.h"
#include "lte_rrc_controller.h"
#include "lte_pdcp_msg.h"
#include "lte_rrc_sec_test_ota_msg.h"
#include "lte_as.h"
}

#include "lte_rrc_sec_test_common.h"
#include "TestFramework.h"
#include "lte_rrc_utf.h"

#include <tf_stub.h>

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Typedef of variables internal to module 
lte_rrc_sec_test_smc1.cpp
*/
typedef struct
{
  int   internal_var;  /*!< My internal variable */
  void *internal_ptr;  /*!< My internal pointer */
 
} lte_rrc_smc_test_type_s;

static lte_rrc_mh_dlm_s smc_dlm;

DECLARE_LTE_RRC_SEC_W2L_PSHO_RECFG_EEA1_EIA1;


#define LTE_RRC_SEC_TEST_PHY_CELL_ID_1 0x1457
#define LTE_RRC_SEC_TEST_DL_EARFCN_1 0xf3ac

#define LTE_RRC_SEC_TEST_PHY_CELL_ID_2 0x0123
#define LTE_RRC_SEC_TEST_DL_EARFCN_2 0xabcd

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_sec_test_smc1.cpp
*/


extern "C"
{
 TF_PROTO_STUB( lte_errno_e, lte_rrc_csp_get_serving_cell_params, lte_rrc_cell_info_s *);
}

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

                                FUNCTIONS

===========================================================================*/



/*===========================================================================

                                UNIT TEST

=======================================================================*/


static uint32 ext_umid_list[20];

static int decode_status = 0;


void LteRrcSecL2WPSHO_Test1__Test()
{
  /*In this test, a Reconfig (Handover Command is beings ent with KCI=0, NCC=0*/
  /*This is a horizontal handover*/
  lte_rrc_send_ul_msg_reqi_s *ulm_ptr = NULL;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_SecurityModeComplete *sec_mode_complete_ptr = NULL;

  byte *buf_ptr;
  uint32 buf_len; 
  lte_rrc_irat_to_lte_psho_key_gen_indi_s *cfg_req_ptr = NULL;
  lte_pdcp_security_cfg_s pdcp_sec_cfg;
  lte_rrc_cfg_cnfi_s cfg_cnf;
  lte_rrc_sim_update_req_s sim_update_req;
  lte_rrc_nas_lte_key_rsp_s nas_lte_key_rsp;
  lte_rrc_sec_s *sec_data_ptr = NULL;
  lte_rrc_irat_to_lte_psho_success_indi_s lte_psho_success_indi;

  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_RRCConnectionReconfiguration *reconfig_ptr = NULL;

  lte_rrc_handover_completed_ind_s handover_completed_ind;

  lte_errno_e status; 

  uint8 k_asme_1[] = 
    {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f};

  uint8 x_k_enb_1[] =
    {0x32, 0xae, 0xc3, 0xa0, 0xa2, 0xce, 0x1d, 0x32, 0x6, 0xc0, 0xcf, 0xac, 0x12, 0xaa, 0x11, 
    0xbe, 0xe3, 0x2b, 0xc7, 0xfc, 0x11, 0x68, 0xa5, 0x2f, 0x0e, 0xe7, 0xed, 0x74, 0x1, 0x97, 
    0x3, 0x5e, };

  
  uint8 x_k_enb_star_1[] =
  {
    0xb4, 0x28, 0xd6, 0x61, 0xb5, 0x17, 0x1c, 0x3e, 0x13, 0x17, 0xd4, 0x28, 0x13, 0x29, 0xa4, 0xbb,
    0x3a, 0xfa, 0xe2, 0x21, 0xbe, 0xab, 0xca, 0x69, 0x31, 0xfb, 0x53, 0xfd, 0x47, 0x97, 0xe0, 0xc1
  } ;

  uint8 x_rrc_int_1[] =
  {0x9f, 0x86, 0x7e, 0x46, 0xc6, 0x71, 0xdb, 0xb8, 0x86, 0xcb, 0xad, 0x53, 0x74, 0x1e, 0xee, 0x6b};


  uint8 x_rrc_enc_1[] = 
  {
    0xf9, 0x86, 0x79, 0x70, 0xb6, 0xf2, 0x1b, 0x7d, 0xd9, 0x93, 0xd2, 0xf2, 0xe3, 0x7e, 0xf4, 0x66
  };

  uint8 x_up_enc_1[] = 
  {
    0x35, 0x9a, 0x30, 0x8f, 0x85, 0x07, 0xe1, 0xd7, 0x6f, 0x8c, 0x9e, 0x31, 0x60, 0xf7, 0x88, 0xaf
  };

  uint8 x_rrc_int_2[] = 
  {
    0x30, 0xfa, 0x53, 0xe4, 0xbe, 0xfa, 0xad, 0x9d, 0x19, 0x37, 0x29, 0x93, 0x13, 0x7, 0xaa, 0x0a
  };

  uint8 x_rrc_enc_2[] = 
  {
   0x63, 0x5a, 0x3f, 0x7b, 0xbf, 0xf6, 0xe9, 0xa0, 0x5b, 0x9b, 0x64, 0xe9, 0xd3, 0x8a, 0x2d, 0xb3 
  };

  uint8 x_up_enc_2[] =
  {
   0x88, 0xca, 0x6b, 0x96, 0x8f, 0xb1, 0x10, 0xf9, 0x7b, 0x3a, 0x31, 0xb5, 0xd3, 0xae, 0x40, 0x0a 
  };

  TF_MSG("Start of test");

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__INITIAL);
  TF_MSG("Sending the LTE_RRC_NAS_LTE_KEY_RSP");




  lte_rrc_init_default_hdr(&nas_lte_key_rsp.msg_hdr, LTE_RRC_NAS_LTE_KEY_RSP);


  nas_lte_key_rsp.security_info.eksi = 1;
  /*NAS count is set to 0*/
  nas_lte_key_rsp.security_info.nas_count = 0xffffffff;
  nas_lte_key_rsp.security_info.flush_nas_security_context = FALSE;

  /*Populate the K_ASME in LTE_RRC_NAS_LTE_KEY_RSP*/
  memcpy(nas_lte_key_rsp.security_info.k_asme, k_asme_1, LTE_GENERATOR_KEY_LEN);

  /*Inject LTE_RRC_NAS_LTE_KEY_RSP to SEC SM*/

  lte_rrc_test_sec_stub_set_non_L2W_HO_flag(FALSE);


  lte_rrc_utf_send_msg((byte*)&nas_lte_key_rsp, sizeof(nas_lte_key_rsp));
  lte_rrc_utf_wait_for_done();


/*Expect a LTE_RRC_IRAT_KEY_GEN_INDI from SEC*/
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_irat_to_lte_psho_key_gen_indi_s) == buf_len);
  cfg_req_ptr = 
    (lte_rrc_irat_to_lte_psho_key_gen_indi_s *)(void*) buf_ptr;
  TF_ASSERT(cfg_req_ptr !=NULL);
  TF_ASSERT((cfg_req_ptr->msg_hdr.id) ==  
            LTE_RRC_IRAT_TO_LTE_PSHO_KEY_GEN_INDI);


  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  TF_ASSERT(NULL != sec_data_ptr);
  TF_ASSERT(NULL != sec_data_ptr->dd_ptr);

  /*Validate the NAS security context*/
  TF_ASSERT(0 == sec_data_ptr->dd_ptr->new_nas_security_context_index);
  
  TF_ASSERT(0 == 
            memcmp(sec_data_ptr->dd_ptr->nas_security_context[0].sec_info_from_nas.k_asme, k_asme_1, LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(1 == sec_data_ptr->dd_ptr->nas_security_context[0].sec_info_from_nas.eksi);

  TF_ASSERT(0xffffffff == sec_data_ptr->dd_ptr->nas_security_context[0].sec_info_from_nas.nas_count);

  TF_ASSERT(0 == 
            memcmp(sec_data_ptr->dd_ptr->nas_security_context[0].k_enb_initial, x_k_enb_1, LTE_GENERATOR_KEY_LEN));

 dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(sec_recfg_w2l_PSHO, sizeof(sec_recfg_w2l_PSHO),
                               DL_DCCH_Message,
                               &decode_status);

 reconfig_ptr = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration;

 /*Validate Reconfig msg*/
 status = lte_rrc_sec_validate_config(reconfig_ptr);
 TF_ASSERT(E_SUCCESS == status);

 /*Prepare the PDCP cfg*/
 /*status = lte_rrc_sec_process_irat_to_lte_ho_config(dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                                                    rrc_TransactionIdentifier, 
                                                    &pdcp_sec_cfg, 
                                                    LTE_RRC_SEC_TEST_PHY_CELL_ID_1,
                                                    LTE_RRC_SEC_TEST_DL_EARFCN_1);

 //TF_ASSERT(E_SUCCESS == status); 
 */

 sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

 TF_ASSERT(0 == sec_data_ptr->dd_ptr->new_as_security_context_index);

 TF_ASSERT(0 == sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].ncc);

 TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].nh[0]),
                       x_k_enb_1,
                       LTE_GENERATOR_KEY_LEN));

 TF_ASSERT(0 == pdcp_sec_cfg.srb_integrity_key_index);

 TF_ASSERT(1 == pdcp_sec_cfg.srb_cipher_key_index);
 TF_ASSERT(2 == pdcp_sec_cfg.drb_cipher_key_index);

 TF_ASSERT(LTE_CIPHER_ALGO_SNOW3G == pdcp_sec_cfg.srb_cipher_algo);
 TF_ASSERT(LTE_CIPHER_ALGO_SNOW3G == pdcp_sec_cfg.drb_cipher_algo);
 TF_ASSERT(LTE_INTEGRITY_ALGO_SNOW3G == pdcp_sec_cfg.srb_integrity_algo);

 TF_ASSERT( NULL != pdcp_sec_cfg.srb_integrity_key_ptr);
 TF_ASSERT(0 == 
           memcmp(x_rrc_int_2,  
                  pdcp_sec_cfg.srb_integrity_key_ptr, 
                  LTE_CIPHER_INTEGRITY_KEY_LEN));

 TF_ASSERT( NULL != pdcp_sec_cfg.srb_cipher_key_ptr);
 TF_ASSERT(0 == 
           memcmp(x_rrc_enc_2,  
                  pdcp_sec_cfg.srb_cipher_key_ptr, 
                  LTE_CIPHER_INTEGRITY_KEY_LEN));

 TF_ASSERT( NULL != pdcp_sec_cfg.drb_cipher_key_ptr);
 TF_ASSERT(0 == 
           memcmp(x_up_enc_2,  
                  pdcp_sec_cfg.drb_cipher_key_ptr, 
                  LTE_CIPHER_INTEGRITY_KEY_LEN));

 lte_rrc_test_sec_stub_set_non_L2W_HO_flag(TRUE);

 lte_rrc_init_default_hdr(&lte_psho_success_indi.msg_hdr, LTE_RRC_IRAT_TO_LTE_PSHO_SUCCESS_INDI);
 

 lte_rrc_utf_send_msg((byte*)&lte_psho_success_indi, sizeof(lte_psho_success_indi));
 lte_rrc_utf_wait_for_done();


 TF_MSG("Sending Handover Completed Indication");
 lte_rrc_init_default_hdr(&handover_completed_ind.msg_hdr, 
                          LTE_RRC_HANDOVER_COMPLETED_IND);

 /*Set the DL freq and the PCI appropriately.*/
 handover_completed_ind.dl_freq = 0xf3ac;
 handover_completed_ind.phy_cell_id =  0x1457;
 /*Inject SIM_UPDATE_REQ to SEC SM*/
 lte_rrc_utf_send_msg((byte*)&handover_completed_ind, 
                      sizeof(handover_completed_ind));

 lte_rrc_utf_wait_for_done();

 sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

 /*Make sure that the Current and New AS contexts both point to the "Set 1"*/
 TF_ASSERT(0 == sec_data_ptr->dd_ptr->new_as_security_context_index);
 TF_ASSERT(0 == sec_data_ptr->dd_ptr->current_as_security_context_index);

 TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);
 /*Free the un-encoded VarShort-MACI*/
 lte_rrc_osys_asn1_free_pdu(reconfig_ptr, lte_rrc_osys_DL_DCCH_Message_PDU);

}

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSecL2WPSHOTest1);
void LteRrcSecL2WPSHOTest1::Setup()
{
  uint8 ext_count = 0;
  
  //ext_umid_list[0] = LTE_MAC_ACCESS_REQ;
  
  lte_rrc_utf_setup(LTE_RRC_SEC_SM, 
                    &(ext_umid_list[0]), 
                    ext_count);
  lte_rrc_test_sec_init();
  lte_rrc_test_sec_init2();
}

void LteRrcSecL2WPSHOTest1::Test()
{
  LteRrcSecL2WPSHO_Test1__Test();

}

void LteRrcSecL2WPSHOTest1::Teardown()
{
  lte_rrc_utf_reset();
  lte_rrc_test_sec_teardown();
  lte_rrc_utf_teardown();
}

