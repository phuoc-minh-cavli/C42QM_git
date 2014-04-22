/*!
  @file lte_rrc_sec_test_conn_setup.cpp


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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sec/lte_rrc_sec_test_smc_no_sim_update_fail.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/20/09   dd     Initial Revision
           
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
#include "lte_rrc_sec_test_ota_msg.h"
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
DECLARE_LTE_RRC_SEC_SMC_EEA1_EIA1;

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*! @brief Variables internal to module lte_rrc_sec_test_smc1.cpp
*/


extern "C"
{

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

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSecSmcNoSimUpdateFailTest1);

static uint32 ext_umid_list[20];


void LteRrcSecSmcNoSimUpdateFailTest1::Setup()
{
  uint8 ext_count = 1;
  
  ext_umid_list[0] = LTE_PDCPDL_SEC_RESET_REQ;
 
  lte_rrc_utf_setup(LTE_RRC_SEC_SM, 
                    &(ext_umid_list[0]), 
                    ext_count);
  lte_rrc_test_sec_init();
  lte_rrc_test_sec_init2();
}

void LteRrcSecSmcNoSimUpdateFailTest1::Test()
{
  lte_rrc_send_ul_msg_reqi_s *ulm_ptr = NULL;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  //lte_rrc_osys_SecurityModeComplete *sec_mode_complete_ptr = NULL;
  lte_rrc_osys_SecurityModeFailure *sec_mode_failure_ptr = NULL;

  byte *buf_ptr;
  uint32 buf_len; 
  //lte_rrc_cfg_reqi_s *cfg_req_ptr = NULL;
  //lte_pdcp_security_cfg_s pdcp_sec_cfg;
  //lte_rrc_cfg_cnfi_s cfg_cnf;
  lte_rrc_sim_update_req_s sim_update_req;
  lte_rrc_sec_s *sec_data_ptr = NULL;
  lte_rrc_tmri_s *tmri_ptr = NULL;
  lte_rrc_conn_mode_failure_indi_s *conn_mode_fail_indi_ptr = NULL;
 
  lte_rrc_dlm_processed_indi_s *dlm_processed_indi_ptr = NULL;

  lte_pdcpdl_sec_reset_req_msg_s *pdcpdl_reset_req_ptr = NULL;

  int decode_status = 0;



  uint8 k_asme_1[] = 
    {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f};

  /*
  uint8 x_k_enb_1[] =
    {0x41, 0xf8, 0x71, 0x4e, 0x31, 0x50, 0x07, 0xc0, 0xfa, 0x96, 0x80, 0x2f, 0x2c, 0x50, 0x55, 0x1b,
     0x57, 0xc1, 0x00, 0xf2, 0xca, 0xcd, 0x05, 0x28, 0xa9, 0x47, 0xca, 0xe3, 0xc1, 0x32, 0x75, 0xf6};

  
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
  */
  TF_MSG("Start of test");

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__INITIAL);
  TF_MSG("Sending the SimUpdateReq");
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);


  sim_update_req.security_info_is_valid = TRUE;
  sim_update_req.security_info.eksi = 1;
  /*NAS count is set to 0*/
  sim_update_req.security_info.nas_count = 0;

  /*Populate the K_ASME in SIM_UPDATE_REQ*/
  memcpy(sim_update_req.security_info.k_asme, k_asme_1, LTE_GENERATOR_KEY_LEN);

  /*Inject SIM_UPDATE_REQ to SEC SM*/
#if 0
  lte_rrc_utf_send_msg((byte*)&sim_update_req, sizeof(sim_update_req));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__INITIAL);

  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  TF_ASSERT(NULL != sec_data_ptr);
  TF_ASSERT(NULL != sec_data_ptr->dd_ptr->dd_ptr);

  /*Validate the NAS security context*/
  TF_ASSERT(0 == sec_data_ptr->dd_ptr->new_nas_security_context_index);
  
  TF_ASSERT(0 == 
            memcmp(sec_data_ptr->dd_ptr->nas_security_context[0].sec_info_from_nas.k_asme, k_asme_1, LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(1 == sec_data_ptr->dd_ptr->nas_security_context[0].sec_info_from_nas.eksi);

  TF_ASSERT(0 == sec_data_ptr->dd_ptr->nas_security_context[0].sec_info_from_nas.nas_count);

  TF_ASSERT(0 == 
            memcmp(sec_data_ptr->dd_ptr->nas_security_context[0].k_enb_initial, x_k_enb_1, LTE_GENERATOR_KEY_LEN));

#endif

  /* Initialise the currently used encoded message*/
  lte_rrc_sec_test_encoded_pdu.value = sec_smc1_eea1_eia1;
  lte_rrc_sec_test_encoded_pdu.length = sizeof(sec_smc1_eea1_eia1);

  smc_dlm.dl_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(sec_smc1_eea1_eia1, sizeof(sec_smc1_eea1_eia1),
                               DL_DCCH_Message,
                               &decode_status);
      //lte_rrc_asn1_decode_pdu(sec_smc_1, lte_rrc_osys_DL_DCCH_Message_PDU, sizeof(sec_smc_1));  
      //lte_rrc_asn1_decode_pdu(sec_smc_1, lte_rrc_osys_DL_DCCH_Message_PDU, 4);  
  smc_dlm.pdu_num =  lte_rrc_osys_DL_DCCH_Message_PDU;
  smc_dlm.pdcp_hdr = 0x87;
  smc_dlm.count = 0x789;
  //Set the MAC-I erroneously 
  smc_dlm.mac_i[0] = 0x9f;
  smc_dlm.mac_i[1] = 0x1e;
  smc_dlm.mac_i[2] = 0xbc;
  smc_dlm.mac_i[3] = 0xb2;

  lte_rrc_init_default_hdr(&(smc_dlm.msg_hdr),
                           LTE_RRC_SECURITY_MODE_COMMAND_DLM);

  /*Inject SMC to SEC SM*/
  lte_rrc_utf_send_msg((byte*)&smc_dlm, sizeof(smc_dlm));
  lte_rrc_utf_wait_for_done();

  /*SEC SM transitions to SMC_FAIL state*/
  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SMC_FAIL);


#if 0
/*Expect a Config Req from CEP*/
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_cfg_reqi_s) == buf_len);
  cfg_req_ptr = 
    (lte_rrc_cfg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(cfg_req_ptr !=NULL);
  TF_ASSERT((cfg_req_ptr->msg_hdr.id) ==  
            LTE_RRC_CFG_REQI);

 /*Assign a transaction ID to the CFG req*/
  /*lte_rrc_cep_test.cfg_transaction_id = 3;*/
  TF_ASSERT(cfg_req_ptr->trans_id_ptr !=NULL);
  //*(cfg_req_ptr->trans_id_ptr)= lte_rrc_cep_test.cfg_transaction_id;
  *(cfg_req_ptr->trans_id_ptr)= 3;
  TF_ASSERT(cfg_req_ptr->cnfi_required == TRUE);
  TF_ASSERT(cfg_req_ptr->num_cfg == 1);
  TF_ASSERT(LTE_RRC_PROC_SEC == cfg_req_ptr->proc);
  TF_ASSERT(LTE_RRC_CFG_SEC == cfg_req_ptr->cfg_info[0].cfg_type);


  /*Verify that the state of the CEP SM is WT_CFG_CNF*/
  TF_MSG("Verifying that the SEC SM is in WT_CFG_CNF state");

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__WT_CFG_CNF);


  
  /*Call the API to prepare Security config*/
  lte_rrc_sec_param_prepare_config(0, FALSE, &pdcp_sec_cfg);

  /*!@todo: Verify the security cfg*/
  TF_ASSERT(0 == pdcp_sec_cfg.srb_integrity_key_index);

  TF_ASSERT(1 == pdcp_sec_cfg.srb_cipher_key_index);
  TF_ASSERT(2 == pdcp_sec_cfg.drb_cipher_key_index);

  TF_ASSERT(LTE_CIPHER_ALGO_SNOW3G == pdcp_sec_cfg.srb_cipher_algo);
  TF_ASSERT(LTE_CIPHER_ALGO_SNOW3G == pdcp_sec_cfg.drb_cipher_algo);
  TF_ASSERT(LTE_INTEGRITY_ALGO_SNOW3G == pdcp_sec_cfg.srb_integrity_algo);
  TF_ASSERT( NULL != pdcp_sec_cfg.srb_integrity_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_rrc_int_1,  
                   pdcp_sec_cfg.srb_integrity_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT( NULL != pdcp_sec_cfg.srb_cipher_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_rrc_enc_1,  
                   pdcp_sec_cfg.srb_cipher_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT( NULL != pdcp_sec_cfg.drb_cipher_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_up_enc_1,  
                   pdcp_sec_cfg.drb_cipher_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));


  /*Make sure that the Encode status for SecModeComplete is set to 0*/
  lte_rrc_utf_set_sync_ul_msg_params(0,0);
  TF_MSG("Sending the ConfigCnf");
  lte_rrc_init_default_hdr(&cfg_cnf.msg_hdr, LTE_RRC_CFG_CNFI);
  cfg_cnf.status = E_SUCCESS ; 
  cfg_cnf.proc= LTE_RRC_PROC_SEC;
  cfg_cnf.trans_id = 3;
  lte_rrc_utf_send_msg((byte*)&cfg_cnf, sizeof(lte_rrc_cfg_cnfi_s));

  lte_rrc_utf_wait_for_done();

#endif
  /*Expect the SecurityModeFailure*/

  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_send_ul_msg_reqi_s) == buf_len);
  ulm_ptr = 
    (lte_rrc_send_ul_msg_reqi_s *)(void*) buf_ptr;
  TF_ASSERT(ulm_ptr !=NULL);
  TF_ASSERT((ulm_ptr->msg_hdr.id) ==  
            LTE_RRC_SEND_UL_MSG_REQI);

  /*Send on SRB1 */
 
  TF_ASSERT(ulm_ptr->rb_id == 1);
  *((uint8 *)(ulm_ptr->encode_status)) = 0; 
  ul_dcch_msg_ptr = (lte_rrc_osys_UL_DCCH_Message *)ulm_ptr->ul_msg_ptr;

  TF_ASSERT(ul_dcch_msg_ptr !=NULL);

  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1 ==
            ul_dcch_msg_ptr->message.t);

 
  TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1_securityModeFailure  ==
            ul_dcch_msg_ptr->message.u.c1->t);
  sec_mode_failure_ptr  = (ul_dcch_msg_ptr->message).u.c1->u.securityModeFailure;

  TF_ASSERT(2 == sec_mode_failure_ptr->rrc_TransactionIdentifier);
  //TF_ASSERT(1 == sec_mode_complete_ptr->rrc_TransactionIdentifier);

  TF_ASSERT(sec_mode_failure_ptr->criticalExtensions.t ==
            T_lte_rrc_osys_SecurityModeFailure_criticalExtensions_securityModeFailure_r8);

  TF_ASSERT(sec_mode_failure_ptr->criticalExtensions.u.securityModeFailure_r8->m.nonCriticalExtensionPresent == 0);



  /*Expect a LTE_RRC_DLM_PROCESSED_INDI from CEP*/
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_dlm_processed_indi_s) == buf_len);
  dlm_processed_indi_ptr = 
    (lte_rrc_dlm_processed_indi_s  *)(void*) buf_ptr;
  TF_ASSERT(dlm_processed_indi_ptr != NULL);
  TF_ASSERT((dlm_processed_indi_ptr->msg_hdr.id) ==  
            LTE_RRC_DLM_PROCESSED_INDI);

  lte_rrc_utf_wait_for_done();

  /*SEC SM transitions to SMC_FAIL state*/
  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SMC_FAIL);

  
  TF_ASSERT(FALSE == lte_rrc_sec_security_is_configured());


  /*Expect PDCPDL Sec Reset Req*/
  lte_rrc_utf_get_next_ext_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_pdcpdl_sec_reset_req_msg_s) == buf_len);
 
  pdcpdl_reset_req_ptr = (lte_pdcpdl_sec_reset_req_msg_s *)(void *)(buf_ptr);
 
  TF_ASSERT(pdcpdl_reset_req_ptr !=NULL);
  TF_ASSERT((pdcpdl_reset_req_ptr->hdr.id) ==  
             LTE_PDCPDL_SEC_RESET_REQ);



 
  /*Expire the SMC FAIL timer*/
  TF_MSG("Expiring the SMC FAIL timer");
  TF_SLEEP(LTE_RRC_SEC_SMC_FAIL_TIME);

  /*Expect SMC Failure tmri*/
  TF_MSG("Checking for SMC Fail tmr expiry");
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  tmri_ptr = 
    (lte_rrc_tmri_s *)(void*) buf_ptr;
  TF_ASSERT(tmri_ptr !=NULL);
  TF_ASSERT((tmri_ptr->msg_hdr.id) ==  
             LTE_RRC_SMC_FAIL_TMRI);


  TF_MSG("Expecting for SMC");
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_conn_mode_failure_indi_s) == buf_len);
  conn_mode_fail_indi_ptr =  
    (lte_rrc_conn_mode_failure_indi_s *)(void *)buf_ptr;
  TF_ASSERT(NULL != conn_mode_fail_indi_ptr);
  TF_ASSERT((conn_mode_fail_indi_ptr->msg_hdr.id) ==  
            LTE_RRC_CONN_MODE_FAILURE_INDI);
  TF_ASSERT(LTE_RRC_CONN_MODE_FAILURE_CAUSE_SMC_FAILURE == 
              conn_mode_fail_indi_ptr->conn_mode_failure_cause);

  TF_ASSERT(LTE_RRC_HO_FAILURE_CAUSE_NONE == 
            conn_mode_fail_indi_ptr->ho_failure_cause);

  #if 0
  /*Expect SMC Failure tmri*/
  TF_MSG("Checking for SMC Fail tmr expiry");
  lte_rrc_utf_get_next_int_msg (&buf_ptr, &buf_len);
  TF_ASSERT(sizeof(lte_rrc_tmri_s) == buf_len);
  tmri_ptr = 
    (lte_rrc_tmri_s *)(void*) buf_ptr;
  TF_ASSERT(tmri_ptr !=NULL);
  TF_ASSERT((tmri_ptr->msg_hdr.id) ==  
             LTE_RRC_SMC_FAIL_TMRI);
  #endif
  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);
  TF_ASSERT(NULL != sec_data_ptr);
  TF_ASSERT(LTE_RRC_SEC_INVALID_NAS_CONTEXT_INDEX == 
            sec_data_ptr->dd_ptr->current_nas_security_context_index);
  /*SEC SM transitions to INACTIVE state*/
  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__INITIAL);


 lte_rrc_utf_reset();
 }

void LteRrcSecSmcNoSimUpdateFailTest1::Teardown()
{
  lte_rrc_test_sec_teardown();
  lte_rrc_utf_teardown();
}
