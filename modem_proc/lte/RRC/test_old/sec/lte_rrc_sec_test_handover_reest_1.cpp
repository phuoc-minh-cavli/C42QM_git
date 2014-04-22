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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sec/lte_rrc_sec_test_handover_reest_1.cpp#1 $

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

DECLARE_LTE_RRC_SEC_SMC_EEA1_EIA1;
DECLARE_LTE_RRC_SEC_RECFG_NCC0_KCI0;
DECLARE_LTE_RRC_SEC_RECFG_NCC5_KCI0;
DECLARE_LTE_RRC_SEC_RECFG_NCC4_KCI0;
DECLARE_LTE_RRC_SEC_RECFG_NCC0_KCI1;
DECLARE_LTE_RRC_SEC_RECFG_NCC2_KCI1_EEA2_EIA2;
DECLARE_LTE_RRC_SEC_RECFG_NCC2_KCI0;

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

/*
K_ASME:
202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f

NH (Vertical Key) table:
  (Key-NCC)
41f8714e315007c0fa96802f2c50551b57c100f2cacd0528a947cae3c13275f6-0
1000c1321970677d303da64ee05fe4262de8ec98ad8ecd80558e5db135c66d65 -1
18a9c894d036145962eaf973eca9960b5949796e937a1c3f6d55243a35d3c2a3-2
2406633349f6a681b0d0326e69b300fde729a4c465caba911602a8b483f0aa72-3
26e563cc2ce57a993ea45ccd6bf8dd4bdadede5bd71316a99229d34543d94326-4
da291ecfb23ea059e07f4317c65fdd501e48f28e8a7f1142b8f23bb42aa5a4c3-5
d6ebb818b0a98ffa12f57009f147eaba29d4bffb9fcecdcbf80d11230fa48fdf-6
96caab1308d583d85922d55959aba251a5f9d65737c8732590f4f3425982035f-7
1e95ff774b79e0a0653bb7e20783a89a1e7a11bba4c60ec84def536ddadd8e88-0
3974d60dca96e38effdc047a5722e15144dc308b7d55bb37b87c03b39aab0e66-1
8ebe189543e915628b60560801968fc55fb7b0b03cda775fb277d61b2aedccc5-2
bfb89db7425158a05db0fb22e722ec892ab6e551cb44961d4d7734c17e356a10-3
472374883562ad25004163fc6bbd51c4db60fd7321bebc72e46e8861533c928b-4

K_ASME:
afaeadacabaaa9a8a7a6a5a4a3a2a1a0d3d5d7d9dbddded1d0d2d4d6d8dadcde
NAS count = 3
NH (Vertical Key) table:
(Key-NCC)
263e8415b78d471629146dac053e9e8c7ac2556922250e0458431e5f3c7b25c5-0
b8863e0192594239d1b2f5f676c2d041bea6907ae1e1a808a66a5f8b66e75ea5-1
3f5cbb8f71a57c2f898c531e938a394352a6a40861a3bab8d6ee0df07a51ddcf-2
d1506e5a0ac0755d9963b31166be3670a1f66d922f17b331752e8f98eebcef32-3
d03e1e34222c31fd0a3f8a1524a8e32ec6407dc6893008694a7a65b86ceef535-4
327cd51f7df417d19f1d83b2af6ea6cbb7aef414570c50e846c8569d81c2d51f-5
0590f317750de076c5083bad5f8e6ece877ae4e6ff85ff58be9ea39d3d8e8dd7-6
9ea041f05837a2d8021d4eefd14bad23c9a4dedd339f3c8e9f24aeae0a4f726d-7



K_ASME:
afaeadacabaaa9a8a7a6a5a4a3a2a1a0d3d5d7d9dbddded1d0d2d4d6d8dadcde
NAS count = 4
NH (Vertical Key) table:
(Key-NCC)
215629e447129d2bed3586868a52ae9a05b7d544ad8ac9c611978a07aec11aa7-0
29c8d0ac3eb161355d35f0ef183f3f13baab10c349f2bcca27c716df33e2e26f-1
67046c82aa646c13b3312fdb43887817fce80e1779e1df55e8c9a2f4ddd57c8a-2
dffba522d7e56a6acfd5b58d45877d2b255ddf5bb1c63da0caccf64e4ce0d341-3
e294071370711f9bd0f2263d29e49bbc1e40a1ccf968adffac84328c80c4716c-4
e05990a5bc10528e2fd144989dc3a0a92c6618f51f5cf9dcbfe0befe9e7d989b-5
9f9b187c19ea193c0d9a9d3b6a689289365975431650186900b5008d811bf9ec-6
c3c55f1ded6264f14295c02c47819c3ee762031a0ed92a67bb6219783fd33739-7
d28c874a626181f083893e45eed183b03a3304c094376da292277f2fa5dd0c23-0
bfedbdcdf8b1fe4a8e81e8c3d36341e0bd5a3b606bec5600f0dc74aa37c71f77-1

*/


static uint32 ext_umid_list[20];

static int decode_status = 0;

void LteRrcSecHandoverNCC0_Test1__Test()
{
  /*In this test, a Reconfig (Handover Command is beings ent with KCI=0, NCC=0*/
  /*This is a horizontal handover*/
  lte_rrc_send_ul_msg_reqi_s *ulm_ptr = NULL;
  lte_rrc_osys_UL_DCCH_Message *ul_dcch_msg_ptr = NULL;
  lte_rrc_osys_SecurityModeComplete *sec_mode_complete_ptr = NULL;

  byte *buf_ptr;
  uint32 buf_len; 
  lte_rrc_cfg_reqi_s *cfg_req_ptr = NULL;
  lte_pdcp_security_cfg_s pdcp_sec_cfg;
  lte_rrc_cfg_cnfi_s cfg_cnf;
  lte_rrc_sim_update_req_s sim_update_req;
  lte_rrc_sec_s *sec_data_ptr = NULL;

  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_RRCConnectionReconfiguration *reconfig_ptr = NULL;

  lte_rrc_handover_completed_ind_s handover_completed_ind;

  lte_errno_e status; 

  uint8 k_asme_1[] = 
    {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f};

  uint8 x_k_enb_1[] =
    {0x41, 0xf8, 0x71, 0x4e, 0x31, 0x50, 0x07, 0xc0, 0xfa, 0x96, 0x80, 0x2f, 0x2c, 0x50, 0x55, 0x1b,
     0x57, 0xc1, 0x00, 0xf2, 0xca, 0xcd, 0x05, 0x28, 0xa9, 0x47, 0xca, 0xe3, 0xc1, 0x32, 0x75, 0xf6};

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
    0x7b, 0xd0, 0x78, 0x68, 0x91, 0x2a, 0x71, 0xdf, 0xf7, 0x0a, 0x2d, 0x25, 0x1c, 0x70, 0x91, 0x44
  };

  uint8 x_rrc_enc_2[] = 
  {
   0x05, 0x8c, 0xe6, 0xf2, 0xa7, 0xef, 0x00, 0x68, 0x0d, 0x6b, 0x1b, 0x24, 0xcf, 0x6b, 0x92, 0x31
  };

  uint8 x_up_enc_2[] =
  {
    0xd1, 0xc5, 0x1b, 0x23, 0x91, 0xea, 0xe1, 0x3b, 0xcd, 0xa4, 0x78, 0xa7, 0x77, 0xe1, 0x44, 0xd1
  };

  TF_MSG("Start of test");

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__INITIAL);
  TF_MSG("Sending the SimUpdateReq");
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);


  sim_update_req.security_info_is_valid = TRUE;
  sim_update_req.security_info.eksi = 1;
  /*NAS count is set to 0*/
  sim_update_req.security_info.nas_count = 0;
  sim_update_req.security_info.flush_nas_security_context = FALSE;

  /*Populate the K_ASME in SIM_UPDATE_REQ*/
  memcpy(sim_update_req.security_info.k_asme, k_asme_1, LTE_GENERATOR_KEY_LEN);

  /*Inject SIM_UPDATE_REQ to SEC SM*/

  lte_rrc_utf_send_msg((byte*)&sim_update_req, sizeof(sim_update_req));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__INITIAL);

  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  TF_ASSERT(NULL != sec_data_ptr);
  TF_ASSERT(NULL != sec_data_ptr->dd_ptr);

  /*Validate the NAS security context*/
  TF_ASSERT(0 == sec_data_ptr->dd_ptr->new_nas_security_context_index);
  
  TF_ASSERT(0 == 
            memcmp(sec_data_ptr->dd_ptr->nas_security_context[0].sec_info_from_nas.k_asme, k_asme_1, LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(1 == sec_data_ptr->dd_ptr->nas_security_context[0].sec_info_from_nas.eksi);

  TF_ASSERT(0 == sec_data_ptr->dd_ptr->nas_security_context[0].sec_info_from_nas.nas_count);

  TF_ASSERT(0 == 
            memcmp(sec_data_ptr->dd_ptr->nas_security_context[0].k_enb_initial, x_k_enb_1, LTE_GENERATOR_KEY_LEN));

  /* Initialise the currently used encoded message*/
  lte_rrc_sec_test_encoded_pdu.value = sec_smc1_eea1_eia1;
  lte_rrc_sec_test_encoded_pdu.length = sizeof(sec_smc1_eea1_eia1);

  smc_dlm.dl_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(sec_smc1_eea1_eia1, sizeof(sec_smc1_eea1_eia1),
                               DL_DCCH_Message,
                               &decode_status);
  smc_dlm.pdu_num =  lte_rrc_osys_DL_DCCH_Message_PDU;
  smc_dlm.pdcp_hdr = 0x87;
  smc_dlm.count = 0x789;
  //Set the MAC-I approproately
  /*
  This is valid if BEARER == 1
  smc_dlm.mac_i[0] = 0x9f;
  smc_dlm.mac_i[1] = 0x1e;
  smc_dlm.mac_i[2] = 0xbc;
  smc_dlm.mac_i[3] = 0xb2;
  */

  /*If BEARER == 0*/
  smc_dlm.mac_i[0] = 0xb2;
  smc_dlm.mac_i[1] = 0xda;
  smc_dlm.mac_i[2] = 0x5e;
  smc_dlm.mac_i[3] = 0xa4;

  lte_rrc_init_default_hdr(&(smc_dlm.msg_hdr),
                           LTE_RRC_SECURITY_MODE_COMMAND_DLM);

  /*Inject SMC to SEC SM*/
  lte_rrc_utf_send_msg((byte*)&smc_dlm, sizeof(smc_dlm));
  lte_rrc_utf_wait_for_done();

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


 /*Expect the SecurityModeComplete*/

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

 
 TF_ASSERT(T_lte_rrc_osys_UL_DCCH_MessageType_c1_securityModeComplete ==
           ul_dcch_msg_ptr->message.u.c1->t);
 sec_mode_complete_ptr = (ul_dcch_msg_ptr->message).u.c1->u.securityModeComplete;

 TF_ASSERT(2 == sec_mode_complete_ptr->rrc_TransactionIdentifier);
 //TF_ASSERT(1 == sec_mode_complete_ptr->rrc_TransactionIdentifier);

 TF_ASSERT(sec_mode_complete_ptr->criticalExtensions.t ==
    T_lte_rrc_osys_SecurityModeComplete_criticalExtensions_securityModeComplete_r8);

 TF_ASSERT(sec_mode_complete_ptr->criticalExtensions.u.securityModeComplete_r8->m.nonCriticalExtensionPresent == 0);
  
 TF_ASSERT(TRUE == lte_rrc_sec_security_is_configured());
 TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);

 dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(sec_recfg_ncc0_kci0, sizeof(sec_recfg_ncc0_kci0),
                               DL_DCCH_Message,
                               &decode_status);

 reconfig_ptr = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration;

 /*Validate Reconfig msg*/
 status = lte_rrc_sec_validate_config(reconfig_ptr);
 TF_ASSERT(E_SUCCESS == status);

 /*Prepare the PDCP cfg*/
 status = lte_rrc_sec_dcch_prepare_config(dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                                          rrc_TransactionIdentifier, 
                                          &pdcp_sec_cfg, 
                                          LTE_RRC_SEC_TEST_PHY_CELL_ID_1,
                                          LTE_RRC_SEC_TEST_DL_EARFCN_1);



 TF_ASSERT(E_SUCCESS == status);    

 sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

 TF_ASSERT(1 == sec_data_ptr->dd_ptr->new_as_security_context_index);

 TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[1].k_enb[0]),
                       x_k_enb_star_1,
                       LTE_GENERATOR_KEY_LEN));
 TF_ASSERT(0 == sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].ncc);

 TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].nh[0]),
                       x_k_enb_1,
                       LTE_GENERATOR_KEY_LEN));

 TF_ASSERT(3 == pdcp_sec_cfg.srb_integrity_key_index);

 TF_ASSERT(4 == pdcp_sec_cfg.srb_cipher_key_index);
 TF_ASSERT(5 == pdcp_sec_cfg.drb_cipher_key_index);

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

 TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);

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
 TF_ASSERT(1 == sec_data_ptr->dd_ptr->new_as_security_context_index);
 TF_ASSERT(1 == sec_data_ptr->dd_ptr->current_as_security_context_index);

 TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);
 /*Free the un-encoded VarShort-MACI*/
 lte_rrc_osys_asn1_free_pdu(reconfig_ptr, lte_rrc_osys_DL_DCCH_Message_PDU);

}
void LteRrcSecHandoverNCC5_Test1__Test()
{
  /*In this test, a Reconfig (Handover Command is beings ent with KCI=0, NCC=5*/
  /*This is a veritcal handover*/

  lte_rrc_sec_s *sec_data_ptr = NULL;
  lte_errno_e status;
  lte_pdcp_security_cfg_s pdcp_sec_cfg;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_RRCConnectionReconfiguration *reconfig_ptr = NULL;
  lte_rrc_handover_completed_ind_s handover_completed_ind;

  uint8 x_nh[] =
  {
    0xda, 0x29, 0x1e, 0xcf, 0xb2, 0x3e, 0xa0, 0x59, 0xe0, 0x7f, 0x43, 0x17, 0xc6, 0x5f, 0xdd, 0x50,
    0x1e, 0x48, 0xf2, 0x8e, 0x8a, 0x7f, 0x11, 0x42, 0xb8, 0xf2, 0x3b, 0xb4, 0x2a, 0xa5, 0xa4, 0xc3
  } ;
  uint8 x_kenb_star_2[] =
  {
    0x8e, 0x4f, 0xac, 0xd7, 0xdf, 0xc6, 0xba, 0x6e, 0x9b, 0x14, 0x32, 0xdb, 0xc8, 0xa1, 0xba, 0x71,
    0x2f, 0x3a, 0xcd, 0xc1, 0x33, 0x6d, 0x28, 0x98, 0x23, 0xbe, 0xd9, 0xb8, 0x20, 0xff, 0xff, 0x79
  };
  uint8 x_krrc_enc_2[] =
  {
    0x64, 0xf1, 0x88, 0x4a, 0x9f, 0x8c, 0xb1, 0x83, 0x76, 0x7f, 0xa0, 0x30, 0x69, 0xa0, 0x25, 0xb1
  };

  uint8 x_krrc_int_2[] =
  {
    0x24, 0x6d, 0xbb, 0xf6, 0xa7, 0xb9, 0x1e, 0x6b, 0x30, 0xac, 0xa7, 0x1c, 0xf7, 0x92, 0x10, 0x8d
  };

  uint8 x_kup_enc_2[] =
  {
    0x53, 0x16, 0xd1, 0xe4, 0x46, 0x02, 0x81, 0x5c, 0x91, 0x30, 0xed, 0x91, 0x5b, 0x2b, 0x6f, 0x7f
  };
  /*Change the PCI and DL freq to new values*/
  TF_STUB(lte_rrc_csp_get_serving_cell_params) =
    lte_rrc_test_sec_stub_get_serving_cell_params2;

  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(sec_recfg_ncc5_kci0, sizeof(sec_recfg_ncc5_kci0),
                               DL_DCCH_Message,
                               &decode_status);

  reconfig_ptr = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration;

  /*Validate Reconfig msg*/
  status = lte_rrc_sec_validate_config(reconfig_ptr);
  TF_ASSERT(E_SUCCESS == status);

  /*Prepare the PDCP cfg*/
  status = lte_rrc_sec_dcch_prepare_config(dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                                           rrc_TransactionIdentifier, 
                                           &pdcp_sec_cfg,
                                           LTE_RRC_SEC_TEST_PHY_CELL_ID_2,
                                           LTE_RRC_SEC_TEST_DL_EARFCN_2);


  TF_ASSERT(E_SUCCESS == status);    

  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  TF_ASSERT(0 == sec_data_ptr->dd_ptr->new_as_security_context_index);

  TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[0].nh[0]),
                        x_nh,
                        LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[0].k_enb[0]),
                        x_kenb_star_2,
                        LTE_GENERATOR_KEY_LEN));
  TF_ASSERT(5== sec_data_ptr->dd_ptr->as_security_context[0].ncc);

  TF_ASSERT(6 == pdcp_sec_cfg.srb_integrity_key_index);

  TF_ASSERT(7 == pdcp_sec_cfg.srb_cipher_key_index);
  TF_ASSERT(8 == pdcp_sec_cfg.drb_cipher_key_index);

  TF_ASSERT(LTE_CIPHER_ALGO_SNOW3G == pdcp_sec_cfg.srb_cipher_algo);
  TF_ASSERT(LTE_CIPHER_ALGO_SNOW3G == pdcp_sec_cfg.drb_cipher_algo);
  TF_ASSERT(LTE_INTEGRITY_ALGO_SNOW3G == pdcp_sec_cfg.srb_integrity_algo);

  TF_ASSERT( NULL != pdcp_sec_cfg.srb_integrity_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_krrc_int_2,  
                   pdcp_sec_cfg.srb_integrity_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT( NULL != pdcp_sec_cfg.srb_cipher_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_krrc_enc_2,  
                   pdcp_sec_cfg.srb_cipher_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT( NULL != pdcp_sec_cfg.drb_cipher_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_kup_enc_2,  
                   pdcp_sec_cfg.drb_cipher_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);

  TF_MSG("Sending Handover Completed Indication");
  lte_rrc_init_default_hdr(&handover_completed_ind.msg_hdr, 
                           LTE_RRC_HANDOVER_COMPLETED_IND);

  /*Set the DL freq and the PCI appropriately.*/
  handover_completed_ind.dl_freq = 0xabcd;
  handover_completed_ind.phy_cell_id =  0x0123;
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
void LteRrcSecHandoverNCC4_Test1__Test()
{
  /*In this test, a Reconfig (Handover Command is beings ent with KCI=0, NCC=4)*/
  /*This is a veritcal handover*/

  lte_rrc_sec_s *sec_data_ptr = NULL;
  lte_errno_e status;
  lte_pdcp_security_cfg_s pdcp_sec_cfg;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_RRCConnectionReconfiguration *reconfig_ptr = NULL;
  lte_rrc_handover_completed_ind_s handover_completed_ind;



  uint8 x_nh[] =
  {
    0x47, 0x23, 0x74, 0x88, 0x35, 0x62, 0xad, 0x25, 0x00, 0x41, 0x63, 0xfc, 0x6b, 0xbd, 0x51, 0xc4,
    0xdb, 0x60, 0xfd, 0x73, 0x21, 0xbe, 0xbc, 0x72, 0xe4, 0x6e, 0x88, 0x61, 0x53, 0x3c, 0x92, 0x8b
  };
  uint8 x_kenb_star_2[] =
  {
    0xde, 0xb4, 0x39, 0x57, 0x27, 0xd7, 0x92, 0x0b, 0xb3, 0xa0, 0x48, 0x23, 0xbd, 0xe3, 0x71, 0x94,
    0x85, 0xa9, 0x7e, 0x12, 0xb8, 0x1c, 0xf4, 0x37, 0x53, 0x23, 0x23, 0xd4, 0x29, 0x3f, 0x8f, 0x10
  };
  uint8 x_krrc_enc_2[] =
  { 
    0xad, 0x91, 0x5d, 0xc6, 0xf9, 0x6a, 0x2b, 0x3b, 0x32, 0x1b, 0x66, 0x15, 0x98, 0x67, 0x3b, 0x4c
  };

  uint8 x_krrc_int_2[] =
  {
    0x5c, 0x16, 0x52, 0x5a, 0x32, 0xf1, 0x1b, 0x8f, 0xd4, 0x82, 0xaf, 0x44, 0x00, 0x32, 0x1c, 0x99
  };

  uint8 x_kup_enc_2[] =
  {
    0x4c, 0xce, 0x6b, 0x29, 0xc9, 0x5e, 0xb7, 0x90, 0xcc, 0x7c, 0x0a, 0xd9, 0x2b, 0xd5, 0xd3, 0xf7
  };
  /*Change the PCI and DL freq to new values*/
  TF_STUB(lte_rrc_csp_get_serving_cell_params) =
    lte_rrc_test_sec_stub_get_serving_cell_params2;

  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(sec_recfg_ncc4_kci0, sizeof(sec_recfg_ncc4_kci0),
                               DL_DCCH_Message,
                               &decode_status);

  reconfig_ptr = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration;

  /*Validate Reconfig msg*/
  status = lte_rrc_sec_validate_config(reconfig_ptr);
  TF_ASSERT(E_SUCCESS == status);

  /*Prepare the PDCP cfg*/
  status = lte_rrc_sec_dcch_prepare_config(dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                                           rrc_TransactionIdentifier, 
                                           &pdcp_sec_cfg,
                                           LTE_RRC_SEC_TEST_PHY_CELL_ID_2,
                                           LTE_RRC_SEC_TEST_DL_EARFCN_2);



  TF_ASSERT(E_SUCCESS == status);    

  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  TF_ASSERT(1 == sec_data_ptr->dd_ptr->new_as_security_context_index);

  TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[1].nh[0]),
                        x_nh,
                        LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[1].k_enb[0]),
                        x_kenb_star_2,
                        LTE_GENERATOR_KEY_LEN));  

  TF_ASSERT(4== sec_data_ptr->dd_ptr->as_security_context[1].ncc);


  TF_ASSERT(9 == pdcp_sec_cfg.srb_integrity_key_index);

  TF_ASSERT(10 == pdcp_sec_cfg.srb_cipher_key_index);
  TF_ASSERT(11 == pdcp_sec_cfg.drb_cipher_key_index);

  TF_ASSERT(LTE_CIPHER_ALGO_SNOW3G == pdcp_sec_cfg.srb_cipher_algo);
  TF_ASSERT(LTE_CIPHER_ALGO_SNOW3G == pdcp_sec_cfg.drb_cipher_algo);
  TF_ASSERT(LTE_INTEGRITY_ALGO_SNOW3G == pdcp_sec_cfg.srb_integrity_algo);

  TF_ASSERT( NULL != pdcp_sec_cfg.srb_integrity_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_krrc_int_2,  
                   pdcp_sec_cfg.srb_integrity_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT( NULL != pdcp_sec_cfg.srb_cipher_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_krrc_enc_2,  
                   pdcp_sec_cfg.srb_cipher_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT( NULL != pdcp_sec_cfg.drb_cipher_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_kup_enc_2,  
                   pdcp_sec_cfg.drb_cipher_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);

  TF_MSG("Sending Handover Completed Indication");
  lte_rrc_init_default_hdr(&handover_completed_ind.msg_hdr, 
                           LTE_RRC_HANDOVER_COMPLETED_IND);

  /*Set the DL freq and the PCI appropriately.*/
  handover_completed_ind.dl_freq = 0xabcd;
  handover_completed_ind.phy_cell_id =  0x0123;
  /*Inject SIM_UPDATE_REQ to SEC SM*/
  lte_rrc_utf_send_msg((byte*)&handover_completed_ind, 
                       sizeof(handover_completed_ind));

  lte_rrc_utf_wait_for_done();

  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  /*Make sure that the Current and New AS contexts both point to the "Set 1"*/
  TF_ASSERT(1 == sec_data_ptr->dd_ptr->new_as_security_context_index);
  TF_ASSERT(1 == sec_data_ptr->dd_ptr->current_as_security_context_index);

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);
  /*Free the un-encoded Reconfig*/
  lte_rrc_osys_asn1_free_pdu(reconfig_ptr, lte_rrc_osys_DL_DCCH_Message_PDU);


}

void LteRrcSecHandoverNCC0KCI1_Test1__Test()
{
  /*In this test, a Reconfig (Handover Command is beings ent with KCI=1, NCC=0)*/
  /*This is a veritcal handover*/

  lte_rrc_sec_s *sec_data_ptr = NULL;
  lte_errno_e status;
  lte_pdcp_security_cfg_s pdcp_sec_cfg;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_RRCConnectionReconfiguration *reconfig_ptr = NULL;
  lte_rrc_handover_completed_ind_s handover_completed_ind;

  lte_rrc_sim_update_req_s sim_update_req;

  uint8 k_asme_2[] = 
  {
    0xaf, 0xae, 0xad, 0xac, 0xab, 0xaa, 0xa9, 0xa8, 0xa7, 0xa6, 0xa5, 0xa4, 0xa3, 0xa2, 0xa1, 0xa0,
    0xd3, 0xd5, 0xd7, 0xd9, 0xdb, 0xdd, 0xde, 0xd1, 0xd0, 0xd2, 0xd4, 0xd6, 0xd8, 0xda, 0xdc, 0xde
  };
  uint8 x_k_enb_2[] = 
  {
    0x26, 0x3e, 0x84, 0x15, 0xb7, 0x8d, 0x47, 0x16, 0x29, 0x14, 0x6d, 0xac, 0x05, 0x3e, 0x9e, 0x8c,
    0x7a, 0xc2, 0x55, 0x69, 0x22, 0x25, 0x0e, 0x04, 0x58, 0x43, 0x1e, 0x5f, 0x3c, 0x7b, 0x25, 0xc5
  };

  uint8 x_k_enb_2_star[] = 
  {
    0x2d, 0x0e, 0x63, 0x0f, 0xb8, 0xc0, 0xa4, 0x3f, 0xc1, 0xa4, 0xeb, 0xf2, 0xbf, 0xe3, 0x86, 0x75,
    0x21, 0xd2, 0xeb, 0x5c, 0xcf, 0x91, 0xeb, 0xf2, 0x72, 0x58, 0x51, 0xd5, 0x93, 0x1d, 0xee, 0xaf
  };


  uint8 x_krrc_int_3[] =
  {
    //0x12, 0x37, 0x32, 0x12, 0x95, 0xfe, 0xbe, 0x40, 0x2e, 0x06, 0x17, 0x15, 0xaa, 0x6b, 0xe8, 0x52
    0x6f, 0xaf, 0x6e, 0x62, 0x08, 0xa8, 0xf4, 0x12, 0xa8, 0x86, 0x89, 0x6b, 0x65, 0xa3, 0x14, 0xa5
  };
  uint8 x_krrc_enc_3[] = 
  {
    //0x17, 0x9b, 0xa9, 0x43, 0x15, 0x7f, 0x61, 0xde, 0x16, 0xc3, 0x7d, 0x0f, 0x22, 0x28, 0x66, 0x09 
    0xa8, 0x6a, 0x49, 0xa7, 0x75, 0x9e, 0x67, 0xd0, 0xf6, 0xff, 0x2f, 0x61, 0x63, 0xf1, 0xa8, 0xfb


  };
  uint8 x_kup_enc_3[] =
  {
    //0xef, 0xcb, 0x8f, 0x44, 0x2a, 0x02, 0x46, 0x44, 0x9a, 0xb3, 0xef, 0xe7, 0x60, 0xc7, 0x0e, 0x22
    0x9f, 0xa7, 0x5f, 0xda, 0x09, 0x3d, 0x0b, 0x54, 0x66, 0x26, 0x0f, 0xe4, 0x5e, 0x58, 0xdc, 0x05
  };
  TF_MSG("Sending the SimUpdateReq");
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);


  sim_update_req.security_info_is_valid = TRUE;
  sim_update_req.security_info.eksi = 2;
  /*NAS count is set to 0*/
  sim_update_req.security_info.nas_count = 3;

  sim_update_req.security_info.flush_nas_security_context = FALSE;

  /*Populate the K_ASME in SIM_UPDATE_REQ*/
  memcpy(sim_update_req.security_info.k_asme, k_asme_2, LTE_GENERATOR_KEY_LEN);

  /*Inject SIM_UPDATE_REQ to SEC SM*/

  lte_rrc_utf_send_msg((byte*)&sim_update_req, sizeof(sim_update_req));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);

  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  TF_ASSERT(NULL != sec_data_ptr);

  /*Validate the NAS security context*/
  TF_ASSERT(1 == sec_data_ptr->dd_ptr->new_nas_security_context_index);
  
  TF_ASSERT(0 == 
            memcmp(sec_data_ptr->dd_ptr->nas_security_context[1].sec_info_from_nas.k_asme, k_asme_2, LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(2 == sec_data_ptr->dd_ptr->nas_security_context[1].sec_info_from_nas.eksi);

  TF_ASSERT(3 == sec_data_ptr->dd_ptr->nas_security_context[1].sec_info_from_nas.nas_count);

  TF_ASSERT(0 == 
            memcmp(sec_data_ptr->dd_ptr->nas_security_context[1].k_enb_initial, 
                   x_k_enb_2, 
                   LTE_GENERATOR_KEY_LEN));

  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(sec_recfg_ncc0_kci1, sizeof(sec_recfg_ncc0_kci1),
                               DL_DCCH_Message,
                               &decode_status);

  reconfig_ptr = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration;
    
  /*Validate Reconfig msg*/
  status = lte_rrc_sec_validate_config(reconfig_ptr);
  TF_ASSERT(E_SUCCESS == status);

  /*Prepare the PDCP cfg*/
  status = lte_rrc_sec_dcch_prepare_config(dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                                           rrc_TransactionIdentifier, 
                                           &pdcp_sec_cfg,
                                           LTE_RRC_SEC_TEST_PHY_CELL_ID_2,
                                           LTE_RRC_SEC_TEST_DL_EARFCN_2);
  TF_ASSERT(E_SUCCESS == status);    

  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  TF_ASSERT(0 == sec_data_ptr->dd_ptr->new_as_security_context_index);

  TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[0].nh[0]),
                        x_k_enb_2,
                        LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[0].k_enb[0]),
                        //x_k_enb_2,
                        x_k_enb_2_star,
                        LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(0== sec_data_ptr->dd_ptr->as_security_context[0].ncc);

  TF_ASSERT(TRUE == sec_data_ptr->dd_ptr->as_security_context[0].is_initial_k_enb);


  TF_ASSERT(12 == pdcp_sec_cfg.srb_integrity_key_index);

  TF_ASSERT(13 == pdcp_sec_cfg.srb_cipher_key_index);
  TF_ASSERT(14 == pdcp_sec_cfg.drb_cipher_key_index);

  TF_ASSERT(LTE_CIPHER_ALGO_SNOW3G == pdcp_sec_cfg.srb_cipher_algo);
  TF_ASSERT(LTE_CIPHER_ALGO_SNOW3G == pdcp_sec_cfg.drb_cipher_algo);
  TF_ASSERT(LTE_INTEGRITY_ALGO_SNOW3G == pdcp_sec_cfg.srb_integrity_algo);

  TF_ASSERT( NULL != pdcp_sec_cfg.srb_integrity_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_krrc_int_3,  
                   pdcp_sec_cfg.srb_integrity_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT( NULL != pdcp_sec_cfg.srb_cipher_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_krrc_enc_3,  
                   pdcp_sec_cfg.srb_cipher_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT( NULL != pdcp_sec_cfg.drb_cipher_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_kup_enc_3,  
                   pdcp_sec_cfg.drb_cipher_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));
  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);


  TF_MSG("Sending Handover Completed Indication");
  lte_rrc_init_default_hdr(&handover_completed_ind.msg_hdr, 
                           LTE_RRC_HANDOVER_COMPLETED_IND);

  /*Set the DL freq and the PCI appropriately.*/
  handover_completed_ind.dl_freq = 0xabcd;
  handover_completed_ind.phy_cell_id =  0x0123;
  /*Inject SIM_UPDATE_REQ to SEC SM*/
  lte_rrc_utf_send_msg((byte*)&handover_completed_ind, 
                       sizeof(handover_completed_ind));

  lte_rrc_utf_wait_for_done();

  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  /*Make sure that the Current and New AS contexts both point to the "Set 0"*/
  TF_ASSERT(0 == sec_data_ptr->dd_ptr->new_as_security_context_index);
  TF_ASSERT(0 == sec_data_ptr->dd_ptr->current_as_security_context_index);

  /*Make sure that the Current and New NAS contexts both point to the "Set 1"*/
  TF_ASSERT(1 == sec_data_ptr->dd_ptr->new_nas_security_context_index);
  TF_ASSERT(1 == sec_data_ptr->dd_ptr->current_nas_security_context_index);

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);

  /*Free the un-encoded Reconfig*/
  lte_rrc_osys_asn1_free_pdu(reconfig_ptr, lte_rrc_osys_DL_DCCH_Message_PDU);
}
void LteRrcSecHandoverNCC2KCI1_EEA2EIA2_Test1__Test(uint8 a2_index)
{
  /*In this test, a Reconfig (Handover Command is beings ent with KCI=1, NCC=2)*/
  /*This is a veritcal handover*/

  lte_rrc_sec_s *sec_data_ptr = NULL;
  lte_errno_e status;
  lte_pdcp_security_cfg_s pdcp_sec_cfg;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_RRCConnectionReconfiguration *reconfig_ptr = NULL;

  lte_rrc_sim_update_req_s sim_update_req;

  uint8 k_asme_2[] = 
  {
    0xaf, 0xae, 0xad, 0xac, 0xab, 0xaa, 0xa9, 0xa8, 0xa7, 0xa6, 0xa5, 0xa4, 0xa3, 0xa2, 0xa1, 0xa0,
    0xd3, 0xd5, 0xd7, 0xd9, 0xdb, 0xdd, 0xde, 0xd1, 0xd0, 0xd2, 0xd4, 0xd6, 0xd8, 0xda, 0xdc, 0xde
  };

  uint8 x_kenb_3[] = 
  {
    0x21, 0x56, 0x29, 0xe4, 0x47, 0x12, 0x9d, 0x2b, 0xed, 0x35, 0x86, 0x86, 0x8a, 0x52, 0xae, 0x9a,
    0x05, 0xb7, 0xd5, 0x44, 0xad, 0x8a, 0xc9, 0xc6, 0x11, 0x97, 0x8a, 0x07, 0xae, 0xc1, 0x1a, 0xa7
  } ;

  /*NH for NCC = 2*/
  uint8 x_nh_2[] = 
  {
    0x67, 0x04, 0x6c, 0x82, 0xaa, 0x64, 0x6c, 0x13, 0xb3, 0x31, 0x2f, 0xdb, 0x43, 0x88, 0x78, 0x17,
    0xfc, 0xe8, 0x0e, 0x17, 0x79, 0xe1, 0xdf, 0x55, 0xe8, 0xc9, 0xa2, 0xf4, 0xdd, 0xd5, 0x7c, 0x8a
  };
  uint8 x_kenb_4[] = 
  {

    0x31, 0xe9, 0x3f, 0x27, 0xa7, 0x42, 0x8d, 0x1c, 0xf8, 0x4b, 0x05, 0x9d, 0x64, 0x52, 0x70, 0xd1,
    0x63, 0x3b, 0x19, 0x27, 0xe4, 0x28, 0x70, 0x5b, 0xd5, 0x1a, 0xd4, 0xb1, 0x8e, 0xcb, 0x0c, 0xd4 
  };
  uint8 x_krrc_int_4[] =
  {
    /*0xe8, 0xeb, 0x72, 0x7f, 0xc7, 0x9c, 0xc1, 0x8c, 0x89, 0x80, 0x71, 0x04, 0x3c, 0x7e, 0x8e, 0x55*/
    0x78, 0x83, 0x68, 0x2a, 0xc9, 0xf6, 0x12, 0x16, 0xce, 0x98, 0xad, 0xb5, 0xe2, 0x07, 0x74, 0xb9
  };
  uint8 x_krrc_enc_4[] = 
  {
    /*0x39, 0xf5, 0x1e, 0x63, 0xa3, 0x0c, 0x15, 0x81, 0x96, 0x13, 0x24, 0xf3, 0xaf, 0x7a, 0x06, 0x7e*/
    0x7d, 0xe8, 0x2b, 0xa0, 0xb4, 0x7c, 0x25, 0x69, 0x80, 0x9c, 0x19, 0x22, 0x8a, 0x8d, 0x98, 0xef
  };
  uint8 x_kup_enc_4[] =
  {
    /*0x64, 0x3f, 0x01, 0x25, 0xcb, 0x7a, 0xbc, 0x23, 0x79, 0xa4, 0x1e, 0xab, 0xc1, 0x70, 0xa2, 0x3b*/
    0x46, 0x93, 0x91, 0x5a, 0x77, 0x41, 0xa7, 0x2c, 0x40, 0x6d, 0x12, 0x75, 0xe4, 0xb4, 0x13, 0xb8
  };

 

  TF_MSG("Sending the SimUpdateReq");
  lte_rrc_init_default_hdr(&sim_update_req.msg_hdr, LTE_RRC_SIM_UPDATE_REQ);
 
  sim_update_req.security_info_is_valid = TRUE;
  sim_update_req.security_info.eksi = 2;
  /*NAS count is set to 4*/
  sim_update_req.security_info.nas_count = 4;

  /*Set the flush flag to FALSE*/
  sim_update_req.security_info.flush_nas_security_context = FALSE;

  /*Populate the K_ASME in SIM_UPDATE_REQ*/
  memcpy(sim_update_req.security_info.k_asme, k_asme_2, LTE_GENERATOR_KEY_LEN);

  /*Inject SIM_UPDATE_REQ to SEC SM*/

  lte_rrc_utf_send_msg((byte*)&sim_update_req, sizeof(sim_update_req));
  lte_rrc_utf_wait_for_done();

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);

  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  TF_ASSERT(NULL != sec_data_ptr);

  /*Validate the NAS security context*/
  TF_ASSERT(0 == sec_data_ptr->dd_ptr->new_nas_security_context_index);
  
  TF_ASSERT(0 == 
            memcmp(sec_data_ptr->dd_ptr->nas_security_context[sec_data_ptr->dd_ptr->new_nas_security_context_index].sec_info_from_nas.k_asme, k_asme_2, LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(2 == 
            sec_data_ptr->dd_ptr->nas_security_context[sec_data_ptr->dd_ptr->new_nas_security_context_index].sec_info_from_nas.eksi);

  TF_ASSERT(4 == 
            sec_data_ptr->dd_ptr->nas_security_context[sec_data_ptr->dd_ptr->new_nas_security_context_index].sec_info_from_nas.nas_count);

  
  TF_ASSERT(0 == 
            memcmp(sec_data_ptr->dd_ptr->nas_security_context[sec_data_ptr->dd_ptr->new_nas_security_context_index].k_enb_initial, 
                   x_kenb_3, 
                   LTE_GENERATOR_KEY_LEN));

  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(sec_recfg_ncc2_kci1_eea2_eia2, 
                               sizeof(sec_recfg_ncc2_kci1_eea2_eia2),
                               DL_DCCH_Message,
                               &decode_status);

  reconfig_ptr = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration;
  
  /*Validate Reconfig msg*/
  status = lte_rrc_sec_validate_config(reconfig_ptr);
  TF_ASSERT(E_SUCCESS == status);

  /*Prepare the PDCP cfg*/
  status = lte_rrc_sec_dcch_prepare_config(dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                                           rrc_TransactionIdentifier, 
                                           &pdcp_sec_cfg,
                                           LTE_RRC_SEC_TEST_PHY_CELL_ID_2,
                                           LTE_RRC_SEC_TEST_DL_EARFCN_2);
  TF_ASSERT(E_SUCCESS == status);    

  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  TF_ASSERT(1 == sec_data_ptr->dd_ptr->new_as_security_context_index);


  /*NH should match*/
  TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].nh[0]),
                        x_nh_2,
                        LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].k_enb[0]),
                        x_kenb_4,
                        LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(2== sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].ncc);

  TF_ASSERT(TRUE == 
            sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].is_initial_k_enb);

  //TF_ASSERT(0 == pdcp_sec_cfg.srb_integrity_key_index);
  TF_ASSERT(a2_index*3 == pdcp_sec_cfg.srb_integrity_key_index);

  //TF_ASSERT(1 == pdcp_sec_cfg.srb_cipher_key_index);
  TF_ASSERT(a2_index*3+1 == pdcp_sec_cfg.srb_cipher_key_index);
  //TF_ASSERT(2 == pdcp_sec_cfg.drb_cipher_key_index);
  TF_ASSERT(a2_index*3+2 == pdcp_sec_cfg.drb_cipher_key_index);

  TF_ASSERT(LTE_CIPHER_ALGO_AES == pdcp_sec_cfg.srb_cipher_algo);
  TF_ASSERT(LTE_CIPHER_ALGO_AES == pdcp_sec_cfg.drb_cipher_algo);
  TF_ASSERT(LTE_INTEGRITY_ALGO_AES == pdcp_sec_cfg.srb_integrity_algo);

  TF_ASSERT( NULL != pdcp_sec_cfg.srb_integrity_key_ptr);

  TF_ASSERT(0 == 
            memcmp(x_krrc_int_4,  
                   pdcp_sec_cfg.srb_integrity_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT( NULL != pdcp_sec_cfg.srb_cipher_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_krrc_enc_4,  
                   pdcp_sec_cfg.srb_cipher_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT( NULL != pdcp_sec_cfg.drb_cipher_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_kup_enc_4,  
                   pdcp_sec_cfg.drb_cipher_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));
  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);


  /*Free the un-encoded Reconfig*/
  lte_rrc_osys_asn1_free_pdu(reconfig_ptr, lte_rrc_osys_DL_DCCH_Message_PDU);
  
}
void LteRrcSecHandoverSuccessful2__Test()
{
  lte_rrc_handover_completed_ind_s handover_completed_ind;
  lte_rrc_sec_s *sec_data_ptr = NULL;

  TF_MSG("Sending Handover Completed Indication");
  lte_rrc_init_default_hdr(&handover_completed_ind.msg_hdr, 
                           LTE_RRC_HANDOVER_COMPLETED_IND);

  /*Set the DL freq and the PCI appropriately.*/
  handover_completed_ind.dl_freq = 0xabcd;
  handover_completed_ind.phy_cell_id =  0x0123;
  /*Inject SIM_UPDATE_REQ to SEC SM*/
  lte_rrc_utf_send_msg((byte*)&handover_completed_ind, 
                       sizeof(handover_completed_ind));

  lte_rrc_utf_wait_for_done();

  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  /*Make sure that the Current and New AS contexts both point to the "Set 0"*/
  TF_ASSERT(1 == sec_data_ptr->dd_ptr->new_as_security_context_index);
  TF_ASSERT(1 == sec_data_ptr->dd_ptr->current_as_security_context_index);

  /*Make sure that the Current and New NAS contexts both point to the "Set 1"*/
  TF_ASSERT(0 == sec_data_ptr->dd_ptr->new_nas_security_context_index);
  TF_ASSERT(0 == sec_data_ptr->dd_ptr->current_nas_security_context_index);

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);

  
}
void LteRrcSecReestablishmentNCC7__Test()
{
  lte_pdcp_security_cfg_s pdcp_sec_cfg;
  uint8 ncc=7;
  lte_rrc_sec_s *sec_data_ptr = NULL;
  lte_errno_e status;

  lte_rrc_cre_completed_indi_s cre_completed_indi;
  
  uint8 x_nh_7[] =
  {
    /*
    0xbf, 0xed, 0xbd, 0xcd, 0xf8, 0xb1, 0xfe, 0x4a, 0x8e, 0x81, 0xe8, 0xc3, 0xd3, 0x63, 0x41, 0xe0,
    0xbd, 0x5a, 0x3b, 0x60, 0x6b, 0xec, 0x56, 0x00, 0xf0, 0xdc, 0x74, 0xaa, 0x37, 0xc7, 0x1f, 0x77
    */
    0x9e, 0xa0, 0x41, 0xf0, 0x58, 0x37, 0xa2, 0xd8, 0x02, 0x1d, 0x4e, 0xef, 0xd1, 0x4b, 0xad, 0x23,
    0xc9, 0xa4, 0xde, 0xdd, 0x33, 0x9f, 0x3c, 0x8e, 0x9f, 0x24, 0xae, 0xae, 0x0a, 0x4f, 0x72, 0x6d

  };
  uint8 x_kenb_star_5[] =
  {
    /*
    0xbd, 0xa5, 0xd4, 0x88, 0x13, 0xa6, 0x19, 0x25, 0xe7, 0x54, 0xd7, 0xdc, 0x11, 0x22, 0xb4, 0x97,
    0xfb, 0x28, 0x06, 0xe4, 0x34, 0xf6, 0xc3, 0x4f, 0xd9, 0x28, 0xaa, 0x8d, 0x0c, 0xc9, 0xf8, 0x88
    */
    0x10, 0x0b, 0x03, 0x31, 0x97, 0xb1, 0xf2, 0x37, 0x19, 0xed, 0xab, 0xcb, 0xb0, 0x78, 0x41, 0x55,
    0xdc, 0x5b, 0xf4, 0x20, 0xc8, 0x8d, 0xa9, 0x81, 0x63, 0x06, 0xe0, 0x74, 0xc6, 0xfe, 0x42, 0xf6
  };

  uint8 x_krrc_int_5[] = 
  {  
    0x5a, 0xec, 0x81, 0xd2, 0x20, 0x63, 0x47, 0xf9, 0xec, 0x78, 0x67, 0x46, 0x20, 0xf2, 0xf8, 0xc1
  };
  uint8 x_krrc_enc_5[] = 
  {
    0x4e, 0x7a, 0x57, 0x58, 0xb3, 0x56, 0x8c, 0x20, 0xae, 0x5a, 0xc9, 0x25, 0x32, 0xa3, 0x8c, 0xba
  };
  uint8 x_kup_enc_5[] = 
  { 
    0x8f, 0x6d, 0x3d, 0x8b, 0xe5, 0x71, 0xdd, 0xc2, 0x2f, 0x2e, 0x9a, 0xfa, 0x67, 0xac, 0x76, 0x99
  };
  status = lte_rrc_sec_param_prepare_config(ncc, TRUE, &pdcp_sec_cfg);
  TF_ASSERT(E_SUCCESS == status); 

  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  TF_ASSERT(0 == sec_data_ptr->dd_ptr->current_as_security_context_index);

  TF_ASSERT(1 == sec_data_ptr->dd_ptr->new_as_security_context_index);

  TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].nh[0]),
                        x_nh_7,
                        LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].k_enb[0]),
                        x_kenb_star_5,
                        LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(ncc == 
            sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].ncc);

  TF_ASSERT(FALSE == 
            sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].is_initial_k_enb);

  TF_ASSERT(3 == pdcp_sec_cfg.srb_integrity_key_index);

  TF_ASSERT(4 == pdcp_sec_cfg.srb_cipher_key_index);
  TF_ASSERT(5 == pdcp_sec_cfg.drb_cipher_key_index);

  TF_ASSERT(LTE_CIPHER_ALGO_SNOW3G == pdcp_sec_cfg.srb_cipher_algo);
  TF_ASSERT(LTE_CIPHER_ALGO_SNOW3G == pdcp_sec_cfg.drb_cipher_algo);
  TF_ASSERT(LTE_INTEGRITY_ALGO_SNOW3G == pdcp_sec_cfg.srb_integrity_algo);

  TF_ASSERT( NULL != pdcp_sec_cfg.srb_integrity_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_krrc_int_5,  
                   pdcp_sec_cfg.srb_integrity_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT( NULL != pdcp_sec_cfg.srb_cipher_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_krrc_enc_5,  
                   pdcp_sec_cfg.srb_cipher_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT( NULL != pdcp_sec_cfg.drb_cipher_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_kup_enc_5,  
                   pdcp_sec_cfg.drb_cipher_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);

  TF_MSG("Sending CRE Completed Indication");
  lte_rrc_init_default_hdr(&cre_completed_indi.msg_hdr,
                           LTE_RRC_CRE_COMPLETED_INDI);

  /*Inject CRE_COMPLETED_INDI to SEC SM*/
  lte_rrc_utf_send_msg((byte*)&cre_completed_indi, 
                       sizeof(cre_completed_indi));

 
  lte_rrc_utf_wait_for_done();

  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  /*Make sure that the Current and New AS contexts both point to the "Set 1"*/
  TF_ASSERT(1 == sec_data_ptr->dd_ptr->new_as_security_context_index);
  TF_ASSERT(1 == sec_data_ptr->dd_ptr->current_as_security_context_index);

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);

 // sec_data_ptr->dd_ptr->new_as_security_context_index
}

void LteRrcSecReestablishmentNCC7__Test2()
{
  lte_pdcp_security_cfg_s pdcp_sec_cfg;
  uint8 ncc=7;
  lte_rrc_sec_s *sec_data_ptr = NULL;
  lte_errno_e status;

  lte_rrc_cre_completed_indi_s cre_completed_indi;
  
  uint8 x_nh_7[] =
  {
    /*
    0xbf, 0xed, 0xbd, 0xcd, 0xf8, 0xb1, 0xfe, 0x4a, 0x8e, 0x81, 0xe8, 0xc3, 0xd3, 0x63, 0x41, 0xe0,
    0xbd, 0x5a, 0x3b, 0x60, 0x6b, 0xec, 0x56, 0x00, 0xf0, 0xdc, 0x74, 0xaa, 0x37, 0xc7, 0x1f, 0x77
    */
    0x9e, 0xa0, 0x41, 0xf0, 0x58, 0x37, 0xa2, 0xd8, 0x02, 0x1d, 0x4e, 0xef, 0xd1, 0x4b, 0xad, 0x23,
    0xc9, 0xa4, 0xde, 0xdd, 0x33, 0x9f, 0x3c, 0x8e, 0x9f, 0x24, 0xae, 0xae, 0x0a, 0x4f, 0x72, 0x6d

  };
  uint8 x_kenb_star_6[] =
  {
    /*
    0xbd, 0xa5, 0xd4, 0x88, 0x13, 0xa6, 0x19, 0x25, 0xe7, 0x54, 0xd7, 0xdc, 0x11, 0x22, 0xb4, 0x97,
    0xfb, 0x28, 0x06, 0xe4, 0x34, 0xf6, 0xc3, 0x4f, 0xd9, 0x28, 0xaa, 0x8d, 0x0c, 0xc9, 0xf8, 0x88
    */
    /*
    0x10, 0x0b, 0x03, 0x31, 0x97, 0xb1, 0xf2, 0x37, 0x19, 0xed, 0xab, 0xcb, 0xb0, 0x78, 0x41, 0x55,
    0xdc, 0x5b, 0xf4, 0x20, 0xc8, 0x8d, 0xa9, 0x81, 0x63, 0x06, 0xe0, 0x74, 0xc6, 0xfe, 0x42, 0xf6
    */
    
    0x81, 0x36, 0xd9, 0x08, 0x2a, 0x07, 0x76, 0x5c, 0xf4, 0x41, 0x93, 0x2b, 0x41, 0x9d, 0x78, 0xf1,
    0x32, 0xcf, 0x86, 0x15, 0x7d, 0xfd, 0xe1, 0xf8, 0x7d, 0x37, 0xda, 0x86, 0xae, 0x9c, 0x2f, 0xf7
    
  };

  uint8 x_krrc_int_6[] = 
  { 
    /*
    0x5a, 0xec, 0x81, 0xd2, 0x20, 0x63, 0x47, 0xf9, 0xec, 0x78, 0x67, 0x46, 0x20, 0xf2, 0xf8, 0xc1
    */  
    0x16, 0xda, 0xe4, 0xdd, 0x0c, 0xc4, 0x24, 0x01, 0x62, 0x93, 0xd0, 0x4e, 0x90, 0x84, 0x9b, 0xef

  };
  uint8 x_krrc_enc_6[] = 
  {
    /*
    0x4e, 0x7a, 0x57, 0x58, 0xb3, 0x56, 0x8c, 0x20, 0xae, 0x5a, 0xc9, 0x25, 0x32, 0xa3, 0x8c, 0xba
    */
    0xcf, 0x48, 0xb3, 0x4e, 0x52, 0xa4, 0x8b, 0x63, 0xa7, 0x4b, 0x2f, 0x0d, 0xe5, 0x5d, 0xf6, 0xed
  };
  uint8 x_kup_enc_6[] = 
  { 
    /*
    0x8f, 0x6d, 0x3d, 0x8b, 0xe5, 0x71, 0xdd, 0xc2, 0x2f, 0x2e, 0x9a, 0xfa, 0x67, 0xac, 0x76, 0x99
    */  
    0x09, 0xfc, 0xa9, 0x3c, 0xe0, 0xb5, 0x5a, 0x2f, 0x8e, 0x0f, 0xa5, 0x8e, 0x66, 0x74, 0xab, 0x77

  };
  status = lte_rrc_sec_param_prepare_config(ncc, TRUE, &pdcp_sec_cfg);
  TF_ASSERT(E_SUCCESS == status); 

  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  TF_ASSERT(1 == sec_data_ptr->dd_ptr->current_as_security_context_index);

  TF_ASSERT(0 == sec_data_ptr->dd_ptr->new_as_security_context_index);

  TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].nh[0]),
                        x_nh_7,
                        LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].k_enb[0]),
                        x_kenb_star_6,
                        LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(ncc == 
            sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].ncc);

  TF_ASSERT(FALSE == 
            sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].is_initial_k_enb);

  TF_ASSERT(6 == pdcp_sec_cfg.srb_integrity_key_index);

  TF_ASSERT(7 == pdcp_sec_cfg.srb_cipher_key_index);
  TF_ASSERT(8 == pdcp_sec_cfg.drb_cipher_key_index);

  TF_ASSERT(LTE_CIPHER_ALGO_SNOW3G == pdcp_sec_cfg.srb_cipher_algo);
  TF_ASSERT(LTE_CIPHER_ALGO_SNOW3G == pdcp_sec_cfg.drb_cipher_algo);
  TF_ASSERT(LTE_INTEGRITY_ALGO_SNOW3G == pdcp_sec_cfg.srb_integrity_algo);

  TF_ASSERT( NULL != pdcp_sec_cfg.srb_integrity_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_krrc_int_6,  
                   pdcp_sec_cfg.srb_integrity_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT( NULL != pdcp_sec_cfg.srb_cipher_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_krrc_enc_6,  
                   pdcp_sec_cfg.srb_cipher_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT( NULL != pdcp_sec_cfg.drb_cipher_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_kup_enc_6,  
                   pdcp_sec_cfg.drb_cipher_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);

  TF_MSG("Sending CRE Completed Indication");
  lte_rrc_init_default_hdr(&cre_completed_indi.msg_hdr,
                           LTE_RRC_CRE_COMPLETED_INDI);

  /*Inject CRE_COMPLETED_INDI to SEC SM*/
  lte_rrc_utf_send_msg((byte*)&cre_completed_indi, 
                       sizeof(cre_completed_indi));

 
  lte_rrc_utf_wait_for_done();

  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  /*Make sure that the Current and New AS contexts both point to the "Set 1"*/
  TF_ASSERT(0 == sec_data_ptr->dd_ptr->new_as_security_context_index);
  TF_ASSERT(0 == sec_data_ptr->dd_ptr->current_as_security_context_index);

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);

}
void LteRrcSecHandoverNCC2_Horizontal_Test()
{
  
  lte_rrc_sec_s *sec_data_ptr = NULL;
  lte_errno_e status;
  lte_pdcp_security_cfg_s pdcp_sec_cfg;
  lte_rrc_osys_DL_DCCH_Message *dl_dcch_msg_ptr = NULL;
  lte_rrc_osys_RRCConnectionReconfiguration *reconfig_ptr = NULL;
  lte_rrc_handover_completed_ind_s handover_completed_ind;


  uint8 x_nh_2[] =
  {
    /*
    0x47, 0x23, 0x74, 0x88, 0x35, 0x62, 0xad, 0x25, 0x00, 0x41, 0x63, 0xfc, 0x6b, 0xbd, 0x51, 0xc4,
    0xdb, 0x60, 0xfd, 0x73, 0x21, 0xbe, 0xbc, 0x72, 0xe4, 0x6e, 0x88, 0x61, 0x53, 0x3c, 0x92, 0x8b
    */
    0x67, 0x04, 0x6c, 0x82, 0xaa, 0x64, 0x6c, 0x13, 0xb3, 0x31, 0x2f, 0xdb, 0x43, 0x88, 0x78, 0x17,
    0xfc, 0xe8, 0x0e, 0x17, 0x79, 0xe1, 0xdf, 0x55, 0xe8, 0xc9, 0xa2, 0xf4, 0xdd, 0xd5, 0x7c, 0x8a
  };

  uint8 x_kenb_star[] =
  {
    /*
    0xde, 0xb4, 0x39, 0x57, 0x27, 0xd7, 0x92, 0x0b, 0xb3, 0xa0, 0x48, 0x23, 0xbd, 0xe3, 0x71, 0x94,
    0x85, 0xa9, 0x7e, 0x12, 0xb8, 0x1c, 0xf4, 0x37, 0x53, 0x23, 0x23, 0xd4, 0x29, 0x3f, 0x8f, 0x10*/

    /*
    0x31, 0xe9, 0x3f, 0x27, 0xa7, 0x42, 0x8d, 0x1c, 0xf8, 0x4b, 0x05, 0x9d, 0x64, 0x52, 0x70, 0xd1,
    0x63, 0x3b, 0x19, 0x27, 0xe4, 0x28, 0x70, 0x5b, 0xd5, 0x1a, 0xd4, 0xb1, 0x8e, 0xcb, 0x0c, 0xd4*/

    0x25, 0x28, 0x50, 0x00, 0xd6, 0xdb, 0xcb, 0x5a, 0x14, 0xe5, 0xbf, 0x0b, 0x18, 0xc7, 0x92, 0x86,
    0xf3, 0xeb, 0x8d, 0x56, 0x4a, 0x9c, 0xae, 0x6f, 0xd8, 0x56, 0x29, 0x7a, 0x63, 0x1d, 0xf3, 0xc6

  };
  uint8 x_krrc_enc_2[] =
  { 

    /*0xad, 0x91, 0x5d, 0xc6, 0xf9, 0x6a, 0x2b, 0x3b, 0x32, 0x1b, 0x66, 0x15, 0x98, 0x67, 0x3b, 0x4c*/
    /*0x10, 0x75, 0xa6, 0x8f, 0x03, 0x2c, 0x94, 0x72, 0x62, 0x10, 0x6d, 0xe3, 0xa9, 0x0f, 0xdc, 0xd5*/
    0x3f, 0x6b, 0x79, 0x60, 0x74, 0x4f, 0xfb, 0xf2, 0xc8, 0x0c, 0xd8, 0xce, 0x95, 0xa1, 0xb9, 0xf3
  };

  uint8 x_krrc_int_2[] =
  {
    /*0x5c, 0x16, 0x52, 0x5a, 0x32, 0xf1, 0x1b, 0x8f, 0xd4, 0x82, 0xaf, 0x44, 0x00, 0x32, 0x1c, 0x99*/
    /*0xed, 0xb7, 0x25, 0x54, 0xc8, 0xb0, 0x31, 0x63, 0x85, 0xb8, 0x96, 0x10, 0x16, 0xc0, 0x4a, 0xe2*/
    0x3c, 0x60, 0x5c, 0xc7, 0xfa, 0x84, 0x2a, 0x74, 0x7b, 0xec, 0x98, 0x07, 0x8b, 0x07, 0x7b, 0x25
  };

  uint8 x_kup_enc_2[] =
  {
    /*0x4c, 0xce, 0x6b, 0x29, 0xc9, 0x5e, 0xb7, 0x90, 0xcc, 0x7c, 0x0a, 0xd9, 0x2b, 0xd5, 0xd3, 0xf7*/
    0xd8, 0x3e, 0xed, 0x7c, 0x73, 0x5f, 0x1a, 0xab, 0xe9, 0xc5, 0x2e, 0x7b, 0xe1, 0xb5, 0xcf, 0x9b
  };

  dl_dcch_msg_ptr = (lte_rrc_osys_DL_DCCH_Message *)lte_rrc_osys_asn1_decode_pdu(sec_recfg_ncc2_kci0, 
                               sizeof(sec_recfg_ncc2_kci0),
                               DL_DCCH_Message,
                               &decode_status); 

  reconfig_ptr = dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration; 
  
  /*Validate Reconfig msg*/
  status = lte_rrc_sec_validate_config(reconfig_ptr);
  TF_ASSERT(E_SUCCESS == status);

  /*Prepare the PDCP cfg*/
  status = lte_rrc_sec_dcch_prepare_config(dl_dcch_msg_ptr->message.u.c1->u.rrcConnectionReconfiguration->
                                           rrc_TransactionIdentifier, 
                                           &pdcp_sec_cfg,
                                           LTE_RRC_SEC_TEST_PHY_CELL_ID_2,
                                           LTE_RRC_SEC_TEST_DL_EARFCN_2);



  TF_ASSERT(E_SUCCESS == status);    

  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  TF_ASSERT(0 == sec_data_ptr->dd_ptr->new_as_security_context_index);
  TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].nh[0]),
                        x_nh_2,
                        LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].k_enb[0]),
                        x_kenb_star,
                        LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(2 == sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].ncc);

  TF_ASSERT(12 == pdcp_sec_cfg.srb_integrity_key_index);

  TF_ASSERT(13 == pdcp_sec_cfg.srb_cipher_key_index);
  TF_ASSERT(14 == pdcp_sec_cfg.drb_cipher_key_index);

  TF_ASSERT(LTE_CIPHER_ALGO_AES == pdcp_sec_cfg.srb_cipher_algo);
  TF_ASSERT(LTE_CIPHER_ALGO_AES == pdcp_sec_cfg.drb_cipher_algo);
  TF_ASSERT(LTE_INTEGRITY_ALGO_AES == pdcp_sec_cfg.srb_integrity_algo);

  TF_ASSERT( NULL != pdcp_sec_cfg.srb_integrity_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_krrc_int_2,  
                   pdcp_sec_cfg.srb_integrity_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT( NULL != pdcp_sec_cfg.srb_cipher_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_krrc_enc_2,  
                   pdcp_sec_cfg.srb_cipher_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT( NULL != pdcp_sec_cfg.drb_cipher_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_kup_enc_2,  
                   pdcp_sec_cfg.drb_cipher_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);

  TF_MSG("Sending Handover Completed Indication");
  lte_rrc_init_default_hdr(&handover_completed_ind.msg_hdr, 
                           LTE_RRC_HANDOVER_COMPLETED_IND);

  /*Set the DL freq and the PCI appropriately.*/
  handover_completed_ind.dl_freq = 0xabcd;
  handover_completed_ind.phy_cell_id =  0x0123;
  /*Inject SIM_UPDATE_REQ to SEC SM*/
  lte_rrc_utf_send_msg((byte*)&handover_completed_ind, 
                       sizeof(handover_completed_ind));

  lte_rrc_utf_wait_for_done();
  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  /*Make sure that the Current and New AS contexts both point to the "Set 1"*/
  TF_ASSERT(0 == sec_data_ptr->dd_ptr->new_as_security_context_index);
  TF_ASSERT(0 == sec_data_ptr->dd_ptr->current_as_security_context_index);

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);
  /*Free the un-encoded Reconfig*/
  lte_rrc_osys_asn1_free_pdu(reconfig_ptr, lte_rrc_osys_DL_DCCH_Message_PDU);

}
void LteRrcSecReestablishmentNCC1__Test()
{
  lte_pdcp_security_cfg_s pdcp_sec_cfg;
  uint8 ncc=1;
  lte_rrc_sec_s *sec_data_ptr = NULL;
  lte_errno_e status;

  lte_rrc_cre_completed_indi_s cre_completed_indi;
  
  uint8 x_nh_1[] =
  {
    
    0xbf, 0xed, 0xbd, 0xcd, 0xf8, 0xb1, 0xfe, 0x4a, 0x8e, 0x81, 0xe8, 0xc3, 0xd3, 0x63, 0x41, 0xe0,
    0xbd, 0x5a, 0x3b, 0x60, 0x6b, 0xec, 0x56, 0x00, 0xf0, 0xdc, 0x74, 0xaa, 0x37, 0xc7, 0x1f, 0x77

    /*
    0x9e, 0xa0, 0x41, 0xf0, 0x58, 0x37, 0xa2, 0xd8, 0x02, 0x1d, 0x4e, 0xef, 0xd1, 0x4b, 0xad, 0x23,
    0xc9, 0xa4, 0xde, 0xdd, 0x33, 0x9f, 0x3c, 0x8e, 0x9f, 0x24, 0xae, 0xae, 0x0a, 0x4f, 0x72, 0x6d
    */
  };
  uint8 x_kenb_star_5[] =
  {
    
    0xbd, 0xa5, 0xd4, 0x88, 0x13, 0xa6, 0x19, 0x25, 0xe7, 0x54, 0xd7, 0xdc, 0x11, 0x22, 0xb4, 0x97,
    0xfb, 0x28, 0x06, 0xe4, 0x34, 0xf6, 0xc3, 0x4f, 0xd9, 0x28, 0xaa, 0x8d, 0x0c, 0xc9, 0xf8, 0x88

    /*
    0x10, 0x0b, 0x03, 0x31, 0x97, 0xb1, 0xf2, 0x37, 0x19, 0xed, 0xab, 0xcb, 0xb0, 0x78, 0x41, 0x55,
    0xdc, 0x5b, 0xf4, 0x20, 0xc8, 0x8d, 0xa9, 0x81, 0x63, 0x06, 0xe0, 0x74, 0xc6, 0xfe, 0x42, 0xf6*/
  };

  uint8 x_krrc_int_5[] = 
  {  
    /*0x5a, 0xec, 0x81, 0xd2, 0x20, 0x63, 0x47, 0xf9, 0xec, 0x78, 0x67, 0x46, 0x20, 0xf2, 0xf8, 0xc1*/
    0x8c, 0xac, 0xbb, 0x4d, 0xbe, 0xa9, 0x98, 0xa2, 0xda, 0x52, 0x24, 0xd0, 0x0e, 0x8c, 0x40, 0x97
  };
  uint8 x_krrc_enc_5[] = 
  {
    /*0x4e, 0x7a, 0x57, 0x58, 0xb3, 0x56, 0x8c, 0x20, 0xae, 0x5a, 0xc9, 0x25, 0x32, 0xa3, 0x8c, 0xba*/
    0xef, 0x1e, 0xf4, 0x8e, 0xad, 0x53, 0xde, 0x7f, 0x3e, 0xf1, 0x76, 0xbc, 0x99, 0xd5, 0x83, 0x7c
  };
  uint8 x_kup_enc_5[] = 
  { 
    /*0x8f, 0x6d, 0x3d, 0x8b, 0xe5, 0x71, 0xdd, 0xc2, 0x2f, 0x2e, 0x9a, 0xfa, 0x67, 0xac, 0x76, 0x99*/
    0x32, 0xdc, 0xbf, 0x2f, 0xfb, 0x32, 0x70, 0x3e, 0xeb, 0x77, 0xa3, 0x28, 0xbe, 0xd4, 0x07, 0xbd
  };
  status = lte_rrc_sec_param_prepare_config(ncc, TRUE, &pdcp_sec_cfg);
  TF_ASSERT(E_SUCCESS == status); 

  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  TF_ASSERT(0 == sec_data_ptr->dd_ptr->current_as_security_context_index);

  TF_ASSERT(1 == sec_data_ptr->dd_ptr->new_as_security_context_index);

  TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].nh[0]),
                        x_nh_1,
                        LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(0 == memcmp(&(sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].k_enb[0]),
                        x_kenb_star_5,
                        LTE_GENERATOR_KEY_LEN));

  TF_ASSERT(ncc == 
            sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].ncc);

  TF_ASSERT(FALSE == 
            sec_data_ptr->dd_ptr->as_security_context[sec_data_ptr->dd_ptr->new_as_security_context_index].is_initial_k_enb);

  TF_ASSERT(0 == pdcp_sec_cfg.srb_integrity_key_index);

  TF_ASSERT(1 == pdcp_sec_cfg.srb_cipher_key_index);
  TF_ASSERT(2 == pdcp_sec_cfg.drb_cipher_key_index);

  TF_ASSERT(LTE_CIPHER_ALGO_AES == pdcp_sec_cfg.srb_cipher_algo);
  TF_ASSERT(LTE_CIPHER_ALGO_AES == pdcp_sec_cfg.drb_cipher_algo);
  TF_ASSERT(LTE_INTEGRITY_ALGO_AES == pdcp_sec_cfg.srb_integrity_algo);

  TF_ASSERT( NULL != pdcp_sec_cfg.srb_integrity_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_krrc_int_5,  
                   pdcp_sec_cfg.srb_integrity_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT( NULL != pdcp_sec_cfg.srb_cipher_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_krrc_enc_5,  
                   pdcp_sec_cfg.srb_cipher_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT( NULL != pdcp_sec_cfg.drb_cipher_key_ptr);
  TF_ASSERT(0 == 
            memcmp(x_kup_enc_5,  
                   pdcp_sec_cfg.drb_cipher_key_ptr, 
                   LTE_CIPHER_INTEGRITY_KEY_LEN));

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);

  TF_MSG("Sending CRE Completed Indication");
  lte_rrc_init_default_hdr(&cre_completed_indi.msg_hdr,
                           LTE_RRC_CRE_COMPLETED_INDI);

  /*Inject CRE_COMPLETED_INDI to SEC SM*/
  lte_rrc_utf_send_msg((byte*)&cre_completed_indi, 
                       sizeof(cre_completed_indi));

 
  lte_rrc_utf_wait_for_done();

  sec_data_ptr = (lte_rrc_sec_s *) stm_get_user_data(LTE_RRC_SEC_SM);

  /*Make sure that the Current and New AS contexts both point to the "Set 1"*/
  TF_ASSERT(1 == sec_data_ptr->dd_ptr->new_as_security_context_index);
  TF_ASSERT(1 == sec_data_ptr->dd_ptr->current_as_security_context_index);

  TF_ASSERT(stm_get_state(LTE_RRC_SEC_SM) == LTE_RRC_SEC_SM__SECURE);

}

/*----------------------------------------------------------------------*/
//NCC 0 (no change) KCI 0 test.... One Horizontal Handover

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSecHandoverNCCTest1);
void LteRrcSecHandoverNCCTest1::Setup()
{
  uint8 ext_count = 0;
  
  //ext_umid_list[0] = LTE_MAC_ACCESS_REQ;
  
  lte_rrc_utf_setup(LTE_RRC_SEC_SM, 
                    &(ext_umid_list[0]), 
                    ext_count);
  lte_rrc_test_sec_init();
  lte_rrc_test_sec_init2();
}

void LteRrcSecHandoverNCCTest1::Test()
{
  LteRrcSecHandoverNCC0_Test1__Test();

}

void LteRrcSecHandoverNCCTest1::Teardown()
{
  lte_rrc_utf_reset();
  lte_rrc_test_sec_teardown();
  lte_rrc_utf_teardown();
}
/*----------------------------------------------------------------------*/
//NCC 5 (change) KCI 0 test.... One Horizontal Handover followed by a Vertical Handover
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSecHandoverNCC5ChangeTest2);
void LteRrcSecHandoverNCC5ChangeTest2::Setup()
{
  uint8 ext_count = 0;
  
  //ext_umid_list[0] = LTE_MAC_ACCESS_REQ;
  
  lte_rrc_utf_setup(LTE_RRC_SEC_SM, 
                    &(ext_umid_list[0]), 
                    ext_count);
  lte_rrc_test_sec_init();
  lte_rrc_test_sec_init2();
}

void LteRrcSecHandoverNCC5ChangeTest2::Test()
{
  LteRrcSecHandoverNCC0_Test1__Test();
  LteRrcSecHandoverNCC5_Test1__Test();

}

void LteRrcSecHandoverNCC5ChangeTest2::Teardown()
{
  lte_rrc_utf_reset();
  lte_rrc_test_sec_teardown();
  lte_rrc_utf_teardown();
}

/*----------------------------------------------------------------------*/
//NCC 4 (change) KCI 0 test ... Ome horizontal handover followed by two Vertical Handovers
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSecHandoverNCC4ChangeTest2);
void LteRrcSecHandoverNCC4ChangeTest2::Setup()
{
  uint8 ext_count = 0;
  
    
  lte_rrc_utf_setup(LTE_RRC_SEC_SM, 
                    &(ext_umid_list[0]), 
                    ext_count);
  lte_rrc_test_sec_init();
  lte_rrc_test_sec_init2();
}

void LteRrcSecHandoverNCC4ChangeTest2::Test()
{
  LteRrcSecHandoverNCC0_Test1__Test();
  LteRrcSecHandoverNCC5_Test1__Test();
  LteRrcSecHandoverNCC4_Test1__Test();

}

void LteRrcSecHandoverNCC4ChangeTest2::Teardown()
{
  lte_rrc_utf_reset();
  lte_rrc_test_sec_teardown();
  lte_rrc_utf_teardown();
}
/*----------------------------------------------------------------------*/
//NCC 0  KCI 1 test ... One horizontal handover followed by two 
// Vertical Handovers and a Key Change on the fly
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSecHandoverNCC0KCI1Test1);
void LteRrcSecHandoverNCC0KCI1Test1::Setup()
{
  uint8 ext_count = 0;
  
    
  lte_rrc_utf_setup(LTE_RRC_SEC_SM, 
                    &(ext_umid_list[0]), 
                    ext_count);
  lte_rrc_test_sec_init();
  lte_rrc_test_sec_init2();
}

void LteRrcSecHandoverNCC0KCI1Test1::Test()
{
  LteRrcSecHandoverNCC0_Test1__Test();
  LteRrcSecHandoverNCC5_Test1__Test();
  LteRrcSecHandoverNCC4_Test1__Test();
  LteRrcSecHandoverNCC0KCI1_Test1__Test();
 

}

void LteRrcSecHandoverNCC0KCI1Test1::Teardown()
{
  lte_rrc_utf_reset();
  lte_rrc_test_sec_teardown();
  lte_rrc_utf_teardown();
}
/*----------------------------------------------------------------------*/
//NCC 2  KCI 1 test ... One horizontal handover followed by two 
// Vertical Handovers and a Key Change on the fly
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSecHandoverNCC2KCI1Test1);
void LteRrcSecHandoverNCC2KCI1Test1::Setup()
{
  uint8 ext_count = 0;
  
    
  lte_rrc_utf_setup(LTE_RRC_SEC_SM, 
                    &(ext_umid_list[0]), 
                    ext_count);
  lte_rrc_test_sec_init();
  lte_rrc_test_sec_init2();
}

void LteRrcSecHandoverNCC2KCI1Test1::Test()
{
  LteRrcSecHandoverNCC0_Test1__Test();
  LteRrcSecHandoverNCC5_Test1__Test();
  LteRrcSecHandoverNCC4_Test1__Test();
  LteRrcSecHandoverNCC0KCI1_Test1__Test();
  LteRrcSecHandoverNCC2KCI1_EEA2EIA2_Test1__Test(0);
  LteRrcSecHandoverSuccessful2__Test();

}

void LteRrcSecHandoverNCC2KCI1Test1::Teardown()
{
  lte_rrc_utf_reset();
  lte_rrc_test_sec_teardown();
  lte_rrc_utf_teardown();
}

/*----------------------------------------------------------------------*/
//Re-establishment test 
// Vertical Handovers and; 2 Key Change on the fly; Re-establishment
TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSecReestablishmentTest1);
void LteRrcSecReestablishmentTest1::Setup()
{
  uint8 ext_count = 0;
  
    
  lte_rrc_utf_setup(LTE_RRC_SEC_SM, 
                    &(ext_umid_list[0]), 
                    ext_count);
  lte_rrc_test_sec_init();
  lte_rrc_test_sec_init2();
}

void LteRrcSecReestablishmentTest1::Test()
{
  LteRrcSecHandoverNCC0_Test1__Test();
  LteRrcSecHandoverNCC5_Test1__Test();
  LteRrcSecHandoverNCC4_Test1__Test();
  LteRrcSecHandoverNCC0KCI1_Test1__Test();
  LteRrcSecHandoverNCC2KCI1_EEA2EIA2_Test1__Test(0);
  LteRrcSecReestablishmentNCC7__Test() ;

}
void LteRrcSecReestablishmentTest1::Teardown()
{
  lte_rrc_utf_reset();
  lte_rrc_test_sec_teardown();
  lte_rrc_utf_teardown();
}

/*----------------------------------------------------------------------*/
//Re-establishment test2 
// Vertical Handovers and; 2 Key Change on the fly; Re-establishment with NCC change; 
// Followed by another re-establishment without NCC change

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSecReestablishmentTest2);
void LteRrcSecReestablishmentTest2::Setup()
{
  uint8 ext_count = 0;
  
    
  lte_rrc_utf_setup(LTE_RRC_SEC_SM, 
                    &(ext_umid_list[0]), 
                    ext_count);
  lte_rrc_test_sec_init();
  lte_rrc_test_sec_init2();
}

void LteRrcSecReestablishmentTest2::Test()
{
  LteRrcSecHandoverNCC0_Test1__Test();
  LteRrcSecHandoverNCC5_Test1__Test();
  LteRrcSecHandoverNCC4_Test1__Test();
  LteRrcSecHandoverNCC0KCI1_Test1__Test();
  LteRrcSecHandoverNCC2KCI1_EEA2EIA2_Test1__Test(0);
  LteRrcSecReestablishmentNCC7__Test() ;
  LteRrcSecReestablishmentNCC7__Test2();

}

void LteRrcSecReestablishmentTest2::Teardown()
{
  lte_rrc_utf_reset();
  lte_rrc_test_sec_teardown();
  lte_rrc_utf_teardown();
}

/*----------------------------------------------------------------------*/
//Re-establishment test3 
// Vertical Handovers and; 2 Key Change on the fly; Re-establishment with NCC change; 
// Followed by another re-establishment without NCC change

TF_DEFINE_TEST_CASE(COMPONENT, LteRrcSecComboTest1);
void LteRrcSecComboTest1::Setup()
{
  uint8 ext_count = 0;
  
    
  lte_rrc_utf_setup(LTE_RRC_SEC_SM, 
                    &(ext_umid_list[0]), 
                    ext_count);
  lte_rrc_test_sec_init();
  lte_rrc_test_sec_init2();
}

void LteRrcSecComboTest1::Test()
{
  LteRrcSecHandoverNCC0_Test1__Test();
  LteRrcSecHandoverNCC5_Test1__Test();
  LteRrcSecHandoverNCC4_Test1__Test();
  LteRrcSecHandoverNCC0KCI1_Test1__Test();
  LteRrcSecHandoverNCC2KCI1_EEA2EIA2_Test1__Test(0);
  LteRrcSecReestablishmentNCC7__Test() ;
  LteRrcSecReestablishmentNCC7__Test2();
  LteRrcSecHandoverNCC2KCI1_EEA2EIA2_Test1__Test(3);
  LteRrcSecHandoverSuccessful2__Test();
  LteRrcSecHandoverNCC2_Horizontal_Test();
  LteRrcSecReestablishmentNCC1__Test();


}

void LteRrcSecComboTest1::Teardown()
{
  lte_rrc_utf_reset();
  lte_rrc_test_sec_teardown();
  lte_rrc_utf_teardown();
}




