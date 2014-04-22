
/*==============================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who       what, where, why
--------   ---       -------------------------------------------------------------

==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include <qtf.h>
#include <qtf_stub.h>
#include <qtf_tmc.h>
#include <qtf_testcase.h>  

extern "C" 
{
  #include "comdef.h"
  #include "stdio.h"
  #include "stdlib.h"
  #include "gs_v.h"
  #include "sys.h"
  #include "sys_plmn_selection_v.h"
  #include "reg_mm.h"
  #include "ms.h"
  #include "lte_nas_common_v.h"
  #include "lte_nas.h"
  #include <msgr.h>
  #include <msgr_lte.h>
  #include "lte_rrc_ext_msg.h"
  #include "esm_emm_msg_if.h"
  #include "esm_cm_msgr_enum_msg.h"
  #include "esm_cm_msg_if.h"
  #include "esm_emm_msgr_enum_msg.h"
  #include "gsdi_exp.h"
  #include "nas_exp.h"
  #include "emm_utility.h"
  #include "emm_esm_if_msg.h"
  #include "emm_update_lib.h"
  #include "uim.h"
  #include "mm_v.h"
  
}

extern "C"
{
  extern boolean tmp_t3402_started;
}

#include <emm_test_lib.h>
#include <emm_test_attach_lib.h>
#include <emm_test_detach_lib.h>

/* Global var used for EMM page ind with IMSI test case */
extern lte_rrc_plmn_s          req_plmn;
extern lte_nas_message_id  last_encoded_emm_message;
extern lte_nas_emm_cause_type emm_test_emm_cause;
extern lte_nas_emm_eps_attach_result_type  attach_result;
extern lte_nas_emm_eps_update_result_type  update_result;


void emm_test_complete_simple_attach(msgr_client_t  *client_ptr)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case NAS_ESM_ACTIVE_EPS_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_ACTIVE_EPS_IND ");
        keep_the_test_running = FALSE ;
        break ;
      case NAS_ESM_DATA_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_DATA_IND ");
        nas_stub_send_emm_data_req();
        break ;
      case LTE_RRC_UL_DATA_REQ:         
        {
          TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
          nas_stub_send_rrc_active_eps_ind();
        }
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        break;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }

    TF_YIELD(); 
  }
} /* emm_test_complete_simple_attach */

void emm_test_begin_attach_access_barred(msgr_client_t  *client_ptr)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  boolean                  est_failure_access_barred = TRUE;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SERVICE_REQ ");
        /* Save the rrc plmn */
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, stub_rx.rrc_service_req.req_plmn); 
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND");
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        if(est_failure_access_barred == TRUE){
          nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED); //access barred
          est_failure_access_barred = FALSE;
          TF_YIELD();
          TF_YIELD();
          TF_SLEEP(1*1000); //expire barring timer
        }
        else
        {
          nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS); // establish new connection after access barring
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
          nas_stub_send_attach_accept_and_def_bearer_ctxt_req(ESM_INVALID_BEARER_ID, ESM_INVALID_PTI);
          keep_the_test_running = FALSE ;
        }
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        break;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
    TF_YIELD();
  }
} /* emm_test_begin_attach_access_barred */

void start_gw_oos_lte_attach_procedure(msgr_client_t  *client_ptr)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND");
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS); // establish new connection after access barring
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
          nas_stub_send_attach_accept_and_def_bearer_ctxt_req(ESM_INVALID_BEARER_ID, ESM_INVALID_PTI);
          keep_the_test_running = FALSE ;
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        break;
      default:
        break ;
    }
    TF_YIELD();
  }
}

void emm_test_attach_mo_detach_required(msgr_client_t  *client_ptr)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SERVICE_REQ ");
        /* Save the rrc plmn */
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, stub_rx.rrc_service_req.req_plmn); 
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND");
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD();
        nas_stub_trigger_mo_detach(MO_ESM_DETACH_CMD); //initiate detach
        TF_YIELD();
        nas_stub_send_attach_accept_and_def_bearer_ctxt_req(ESM_INVALID_BEARER_ID, ESM_INVALID_PTI);
        keep_the_test_running = FALSE ;
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        break;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
    TF_YIELD();
  }
} /* emm_test_attach_mo_detach_required */

void emm_test_attach_detach_collision(msgr_client_t               *client_ptr,
                                      lte_nas_emm_mt_detach_type   detach_type,
                                      lte_nas_emm_cause_type       emm_cause)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SERVICE_REQ ");
        /* Save the rrc plmn */
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, stub_rx.rrc_service_req.req_plmn); 
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND");
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_trigger_mt_detach(detach_type, emm_cause);
        TF_YIELD();
        if(detach_type == REATTACH_REQUIRED)
        {
          last_encoded_emm_message = ATTACH_REQUEST;
          nas_stub_send_attach_accept_and_def_bearer_ctxt_req(ESM_INVALID_BEARER_ID, ESM_INVALID_PTI);          
        }
        keep_the_test_running = FALSE;
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        break;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
    TF_YIELD();
  }
} /* emm_test_attach_detach_collision */

void emm_test_attach_request_T3410_timeout(msgr_client_t  *client_ptr){

  stub_rx_type          stub_rx ;
  errno_enum_type       e_retval;
  dword                 bytes_rcvd;
  boolean               keep_the_test_running = TRUE;
  byte                  t3402_count = 0;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

    while (keep_the_test_running)
  {
    e_retval = msgr_receive( client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        TF_MSG("NAS_TEST> EMM sent LTE_RRC_SERVICE_REQ ");
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id,stub_rx.rrc_service_req.req_plmn);
        TF_YIELD();
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND");
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("NAS_TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD();
        TF_SLEEP(15*1000); //Expire 3410 timer
        break ;
      case LTE_RRC_CONN_ABORT_REQ:
        TF_MSG("NAS_TEST> EMM sent LTE_RRC_CONN_ABORT_REQ ");
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_ABORTED);
        TF_MSG("Attach Attempt counter = %d",emm_ctrl_data_ptr->emm_attach_attempt_counter);
        if(tmp_t3402_started == TRUE)
        {
          TF_SLEEP(720*1000); //Expire 3402 timer
          tmp_t3402_started = FALSE;
          t3402_count++;
        }
        else
        {
          TF_SLEEP(10*1000); //Expire 3411 timer
        }
        /* This will be an infinite loop but the test stop after T3402 expires 5 times */
        if(t3402_count == 5)
        {
          keep_the_test_running = FALSE;
        }
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        break;
      default:
        TF_MSG("NAS_TEST> EMM sent unexpected command");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
        stub_rx.msg_hdr.id,
        msgr_get_umid_name(stub_rx.msg_hdr.id) );
        break ;
  }
    TF_YIELD(); /* to allow any other task that is waiting to run*/
  }
}/* emm_test_attach_request_T3410_timeout*/

void emm_test_begin_attach_reject_by_nw(msgr_client_t *client_ptr){
  stub_rx_type stub_rx ;
  errno_enum_type        e_retval;
  dword                  bytes_rcvd;
  boolean                keep_the_test_running = TRUE ;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);

 while (keep_the_test_running)
 {
    e_retval = msgr_receive( client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        TF_MSG("NAS_TEST> EMM sent LTE_RRC_SERVICE_REQ ");
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id,stub_rx.rrc_service_req.req_plmn);
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND");
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("NAS_TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD(); /* to allow any other task that is waiting to run*/
        if(emm_test_emm_cause == LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG || emm_test_emm_cause == LTE_NON_EPS_AUTH_UNACCEPTABLE){
            keep_the_test_running = FALSE;
        }
        nas_stub_send_attach_reject(emm_test_emm_cause);
        TF_YIELD(); /* to allow any other task that is waiting to run*/
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        break;
      case NAS_ESM_FAILURE_IND:
        TF_MSG("NAS_TEST_STUB> EMM sent NAS_ESM_FAILURE_IND");
        keep_the_test_running = FALSE;
        break;
      default:
        TF_MSG("NAS_TEST> EMM sent unexpected command");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
        stub_rx.msg_hdr.id,
        msgr_get_umid_name(stub_rx.msg_hdr.id) );
        break ;
    }
    TF_YIELD(); /* to allow any other task that is waiting to run*/
  }
}/*emm_test_begin_attach_reject_by_nw*/

void emm_test_begin_attach_reject(msgr_client_t  *client_ptr){
  stub_rx_type stub_rx ;
  errno_enum_type        e_retval;
  dword                 bytes_rcvd;
  boolean                keep_the_test_running = TRUE ;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  
  

    while (keep_the_test_running)
  {
    e_retval = msgr_receive( client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        TF_MSG("NAS_TEST> EMM sent LTE_RRC_SERVICE_REQ ");
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id,stub_rx.rrc_service_req.req_plmn);
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND");
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("NAS_TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD(); /* to allow any other task that is waiting to run*/
        nas_stub_send_attach_reject(emm_test_emm_cause);
        TF_SLEEP(720*1000);//Expire 3402 timer
        TF_YIELD(); /* to allow any other task that is waiting to run*/
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("NAS_TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        nas_stub_send_attach_accept_and_def_bearer_ctxt_req(ESM_INVALID_BEARER_ID, ESM_INVALID_PTI);
        keep_the_test_running = FALSE;
        break;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        break;
      default:
        TF_MSG("NAS_TEST> EMM sent unexpected command");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
        stub_rx.msg_hdr.id,
        msgr_get_umid_name(stub_rx.msg_hdr.id) );
        break ;
    }

    TF_YIELD(); /* to allow any other task that is waiting to run*/
  }
}/* emm_test_simple_attach_reject */

void emm_test_begin_attach_tx_failure(msgr_client_t *client_ptr, lte_rrc_ul_data_cnf_status_e  cause)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SERVICE_REQ ");
        /* Save the rrc plmn */
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, stub_rx.rrc_service_req.req_plmn); 
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND");
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(cause, stub_rx.rrc_conn_est_req.transaction_id);
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        nas_stub_send_attach_accept_and_def_bearer_ctxt_req(ESM_INVALID_BEARER_ID, ESM_INVALID_PTI);
        keep_the_test_running = FALSE ;
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        break;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
    TF_YIELD();
  }
} /* emm_test_attach_transmission_failure */

void emm_test_begin_attach_lower_layer_failure(msgr_client_t  *client_ptr,
                         lte_rrc_conn_est_status_e  cause)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SERVICE_REQ ");
        /* Save the rrc plmn */
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, stub_rx.rrc_service_req.req_plmn); 
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND");
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        if((emm_ctrl_data_ptr->emm_attach_attempt_counter + 1) < 5)
        {
          TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
          nas_stub_send_rrc_conn_est_cnf(cause); //lower layer failure
          TF_MSG("Attach Attempt counter = %d",emm_ctrl_data_ptr->emm_attach_attempt_counter);
          TF_YIELD();
          /* Sleep 10 sec to expire T3411 */
          TF_SLEEP(10 * 1000);
        }
        else 
        {
          nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
          nas_stub_send_attach_accept_and_def_bearer_ctxt_req(ESM_INVALID_BEARER_ID, ESM_INVALID_PTI);
          keep_the_test_running = FALSE ;
        }
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        break;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
    TF_YIELD();
  }
} /* emm_test_begin_attach_lower_layer_failure */

void emm_test_begin_attach_cell_change(msgr_client_t  *client_ptr)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SERVICE_REQ ");
        /* Save the rrc plmn */
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, stub_rx.rrc_service_req.req_plmn); 
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND");
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_attach_accept_and_def_bearer_ctxt_req(ESM_INVALID_BEARER_ID, ESM_INVALID_PTI);
        TF_YIELD();
        keep_the_test_running = FALSE;
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        break;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
    TF_YIELD();
  }
} /* emm_test_begin_attach_cell_change */

void emm_test_complete_attach_cell_change(msgr_client_t  *client_ptr)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  boolean           make_cell_change=TRUE;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND");
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case NAS_ESM_ACTIVE_EPS_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_ACTIVE_EPS_IND ");
        keep_the_test_running = FALSE ;
        break ;
      case NAS_ESM_DATA_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_DATA_IND ");
        if(make_cell_change)
        {
          nas_stub_send_rrc_service_ind_with_param(0xFFFFFFFF,req_plmn,10,123);
        }
        else
        {
          nas_stub_send_emm_data_req();
        }
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        if(make_cell_change)
        {
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
          nas_stub_send_attach_accept_and_def_bearer_ctxt_req(ESM_INVALID_BEARER_ID, ESM_INVALID_PTI);
          make_cell_change=FALSE;
        }
        else
        {
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
          nas_stub_send_rrc_active_eps_ind();
        }
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        break;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
        stub_rx.msg_hdr.id,
        msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }

    TF_YIELD(); 
  }
} /* emm_test_complete_attach_cell_change */

void emm_test_complete_attach_esm_failure(msgr_client_t  *client_ptr)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case NAS_ESM_ACTIVE_EPS_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_ACTIVE_EPS_IND ");
        break ;
      case NAS_ESM_DATA_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_DATA_IND ");
        nas_stub_send_emm_data_req_failure(); //esm failure
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        nas_stub_send_detach_accept();
        break ;
      case NAS_ESM_DETACH_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_DETACH_IND");
        break;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        if(EMM_GET_STATE()==EMM_DEREGISTERED && EMM_GET_SUBSTATE()==EMM_DEREGISTERED_PLMN_SEARCH)
        {
          keep_the_test_running = FALSE;
        }
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        break;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
        stub_rx.msg_hdr.id,
        msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }

    TF_YIELD(); 
  }
} /* emm_test_attach_esm_failure */

void emm_test_attach_procedure_conn_rel(msgr_client_t  *client_ptr)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  boolean                  conn_rel = TRUE ;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        /* Save the rrc plmn */
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, stub_rx.rrc_service_req.req_plmn); 
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
            /* Start attach recovery after connection release */
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
        if(conn_rel == TRUE)
        {
          nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL); //release conn
          TF_YIELD();
          /* Sleep 10 sec to expire T3411 */
          TF_SLEEP(10 * 1000);
          conn_rel = FALSE;
        }
        else
        {
          nas_stub_send_attach_accept_and_def_bearer_ctxt_req(ESM_INVALID_BEARER_ID, 2);
        }
        break ;

        
        break ;
      case NAS_ESM_DATA_IND:
        nas_stub_send_emm_data_req();
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        keep_the_test_running = FALSE;
        break ;
      default:
        break ;
    }

    TF_YIELD(); 
  }
} /* emm_test_attach_procedure_conn_rel */

void emm_test_simple_attach(msgr_client_t  *client_ptr)
{
  emm_test_begin_simple_attach(client_ptr, FALSE);
  emm_test_complete_simple_attach(client_ptr);
  
} /* emm_test_simple_attach */

void emm_test_attach_reject(msgr_client_t  *client_ptr, lte_nas_emm_cause_type rej_cause)
{
  emm_test_emm_cause = rej_cause; //set reject reason for unit test
  emm_test_begin_attach_reject(client_ptr);
  emm_test_complete_simple_attach(client_ptr);
} /* emm_test_attach_reject */


void emm_test_begin_attach_reject_by_nw_3
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  send_active_eps_ind = FALSE;
  boolean                  keep_the_test_running = TRUE ;

   while (keep_the_test_running)
    {
      e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                              sizeof(stub_rx_type), &bytes_rcvd );
      TF_ASSERT( e_retval == E_SUCCESS );
  
      switch (stub_rx.msg_hdr.id)
      {
        case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
          TF_MSG("TEST> EMM sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND");
          nas_stub_send_pdn_connectivity_req_in_esm_data_req();
          break;
        case LTE_RRC_SERVICE_REQ:
          TF_MSG("TEST> EMM sent LTE_RRC_SERVICE_REQ ");
          req_plmn = stub_rx.rrc_service_req.req_plmn;
          nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                        stub_rx.rrc_service_req.req_plmn); 
          break ;
        case LTE_RRC_CONN_EST_REQ:
          TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
          nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);  
              nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                        stub_rx.rrc_ul_data_req.trans_id);

          keep_the_test_running = FALSE;      
              nas_stub_send_attach_reject(emm_test_emm_cause);
          /* Expire T3411 timer */
          TF_SLEEP(10 * 1000);
          break ;
        case LTE_RRC_UL_DATA_REQ: 
          TF_MSG("NAS_TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                        stub_rx.rrc_ul_data_req.trans_id);
          keep_the_test_running = FALSE;
          /* Expire T3411 timer */
          nas_stub_send_attach_reject(emm_test_emm_cause);
              TF_SLEEP(10 * 1000);
          break;
        case NAS_ESM_DATA_IND:
          TF_MSG("TEST> EMM sent NAS_ESM_DATA_IND ");
          nas_stub_send_emm_data_req();
          break ;
        case NAS_ESM_ACTIVE_EPS_IND:
          TF_MSG("TEST> EMM sent NAS_ESM_ACTIVE_EPS_IND ");
          keep_the_test_running = FALSE ;
          break ;
        default:
          break;
      }
      TF_YIELD();
    }
}

/* Attach reject by nw three times */
void emm_test_attach_reject_by_nw_thrice(msgr_client_t  *client_ptr, lte_nas_emm_cause_type rej_cause)
{

  int16 rej_cnt =0;
  stub_rx_type             stub_rx;
  boolean is_int_test = FALSE;
  boolean keep_the_test_running = TRUE;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while(rej_cnt < 3)
  {
    rej_cnt++; 
    emm_test_emm_cause = rej_cause;
    emm_test_begin_attach_reject_by_nw_3(client_ptr);
  }

  /* After 3 Rejects with cause 19 mobility info is invalidated
     and T3402 timer is started */
  TF_SLEEP(720 * 1000);

  /* finish the attach complete, RRC connection has not been terminated */
  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
     
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND");
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;

      case LTE_RRC_SERVICE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SERVICE_REQ ");
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      stub_rx.rrc_service_req.req_plmn); 
        break ;
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);  
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);

      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        nas_stub_send_attach_accept_and_def_bearer_ctxt_req(5, 1);
        break;

      case NAS_ESM_DATA_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_DATA_IND ");
        nas_stub_send_emm_data_req();
        keep_the_test_running = FALSE;
        break ;

      default:
        break;
    }
     TF_YIELD();
  }
} /* emm_test_attach_reject_by_nw_thrice */

void emm_test_attach_reject_by_nw(msgr_client_t  *client_ptr, lte_nas_emm_cause_type rej_cause)
{
  emm_test_emm_cause = rej_cause; //set reject reason for unit test
  emm_test_begin_attach_reject_by_nw(client_ptr);
} /* emm_test_attach_reject_by_nw */

void emm_test_attach_access_barred(msgr_client_t  *client_ptr)
{
  emm_test_begin_attach_access_barred(client_ptr);
  emm_test_complete_simple_attach(client_ptr);

} /* emm_test_attach_access_barred */

void emm_test_attach_tx_failure(msgr_client_t  *client_ptr, lte_rrc_ul_data_cnf_status_e  cause)
{
  emm_test_begin_attach_tx_failure(client_ptr, cause);
  emm_test_complete_simple_attach(client_ptr);
}/* emm_test_attach_transmission_failure */

void emm_test_attach_esm_failure(msgr_client_t  *client_ptr)
{
  emm_test_begin_simple_attach(client_ptr, FALSE);
  emm_test_complete_attach_esm_failure(client_ptr);
}/* emm_test_attach_transmission_failure */

void emm_test_attach_cell_change(msgr_client_t  *client_ptr)
{  
  emm_test_begin_attach_cell_change(client_ptr);
  emm_test_complete_attach_cell_change(client_ptr);
}/* emm_test_attach_transmission_failure */

void emm_test_attach_lower_layer_failure(msgr_client_t  *client_ptr, lte_rrc_conn_est_status_e  cause){
  emm_test_begin_attach_lower_layer_failure(client_ptr, cause);
  emm_test_complete_simple_attach(client_ptr);
}/* emm_test_attach_lower_layer_failure */

void emm_test_attach_limited_ra(msgr_client_t  *client_ptr)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  lte_rrc_plmn_s           plmn = {0x6, 0x2, 0x73};

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req_nw_sel_mode(
                       SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION);

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SERVICE_REQ");
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, plmn); 
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND");
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_attach_accept_and_def_bearer_ctxt_req(5, 1);
        break ;
      case NAS_ESM_DATA_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_DATA_IND ");
        nas_stub_send_emm_data_req();
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        nas_stub_send_rrc_active_eps_ind();
        break;
      case LTE_RRC_NW_SEL_MODE_RESET_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_NW_SEL_MODE_RESET_REQ");
        ASSERT(stub_rx.rrc_nw_sel_mode_reset_req.network_select_mode == 
                                         SYS_NETWORK_SELECTION_MODE_AUTOMATIC);
        keep_the_test_running = FALSE ;
        break;
      default:
        break ;
    }
    TF_YIELD();
  }

} /* emm_test_attach_limited_ra */

void emm_test_attach_reject_abnormal_cause
(
  msgr_client_t          *client_ptr,
  lte_nas_emm_cause_type  emm_cause
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  send_active_eps_ind = FALSE;
  boolean                  keep_the_test_running = TRUE ;

  emm_test_emm_cause = emm_cause;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND");
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_SERVICE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SERVICE_REQ ");
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      stub_rx.rrc_service_req.req_plmn); 
        break ;
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_attach_reject(emm_test_emm_cause);
        TF_YIELD();
        /* Expire T3411 timer */
        TF_SLEEP(10 * 1000);
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("NAS_TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        if(send_active_eps_ind == FALSE)
        {
          nas_stub_send_attach_accept_and_def_bearer_ctxt_req(5, 1);
          send_active_eps_ind = TRUE;
        }
        else
        {
          nas_stub_send_rrc_active_eps_ind();
        }
        break;
      case NAS_ESM_DATA_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_DATA_IND ");
        nas_stub_send_emm_data_req();
        break ;
      case NAS_ESM_ACTIVE_EPS_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_ACTIVE_EPS_IND ");
        keep_the_test_running = FALSE ;
        break ;
      default:
        break;
    }
    TF_YIELD();
  }

} /* emm_test_attach_reject_abnormal_cause */

void emm_test_dereg_limited_state_attach
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type                stub_rx ;
  errno_enum_type             e_retval;
  dword                       bytes_rcvd;
  boolean                     keep_the_test_running = TRUE ;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive( client_ptr, (byte*)&(stub_rx), 
                             sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_attach_accept_and_def_bearer_ctxt_req(ESM_INVALID_BEARER_ID, ESM_INVALID_PTI);
        break ;
      case NAS_ESM_DATA_IND:
        nas_stub_send_emm_data_req();
        break;
      case LTE_RRC_UL_DATA_REQ: 
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        keep_the_test_running = FALSE;
        break;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND");
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      default:
        break ;
    }

    TF_YIELD(); 
  }

} /* emm_test_dereg_limited_state_attach */


void emm_test_attach_reject_forbidden_list
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type                stub_rx ;
  errno_enum_type             e_retval;
  dword                       bytes_rcvd;
  boolean                     keep_the_test_running = TRUE ;
  uint16 tac = 3;
  uint16 attac_rej_counter = 0,i=0;
  uint16 num_rejects_to_send;
  lte_rrc_plmn_s  plmn;
  emm_ctrl_data_type      *emm_ctrl_ptr = NULL;

  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);

  /* Send service ind in different TA */
  memset(&plmn, 0x0, sizeof(lte_rrc_plmn_s));

  emm_ctrl_ptr = emm_db_get_ctrl_data();
 
  num_rejects_to_send = 3;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive( client_ptr, (byte*)&(stub_rx), 
                             sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      stub_rx.rrc_service_req.req_plmn); 
        break;
      case NAS_ESM_DATA_IND:
        nas_stub_send_emm_data_req();
        break;
      case LTE_RRC_UL_DATA_REQ: 
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        TF_YIELD();
        break;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND");
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;

      case LTE_RRC_CONN_EST_REQ:
        emm_test_emm_cause = LTE_NAS_TRACKING_AREA_NOT_ALLOWED; //set reject reason for unit test
        TF_MSG("NAS_TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD(); /* to allow any other task that is waiting to run*/
        nas_stub_send_attach_reject(LTE_NAS_TRACKING_AREA_NOT_ALLOWED);
        attac_rej_counter++;
        TF_YIELD(); /* to allow any other task that is waiting to run*/
        
        if(attac_rej_counter > 1)
        {
          /* Expire T3411 timer */
          TF_SLEEP(10 * 1000);
        }

        if(attac_rej_counter == num_rejects_to_send)
        {
          keep_the_test_running = FALSE;
        }
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        break;
      case NAS_ESM_FAILURE_IND:
        TF_MSG("NAS_TEST_STUB> EMM sent NAS_ESM_FAILURE_IND");
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        /* Send service ind in different TA */
        nas_stub_send_rrc_service_ind(0xFFFFFFFF, plmn);
        nas_stub_send_rrc_service_ind_with_param(0xFFFFFFFF,req_plmn,tac,1);
        tac++;
        break;
      default:
        TF_MSG("NAS_TEST> EMM sent unexpected command");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
        stub_rx.msg_hdr.id,
        msgr_get_umid_name(stub_rx.msg_hdr.id) );
        break ;
    }

    TF_YIELD(); 
  }

  TF_YIELD(); 
  TF_YIELD(); 

  ASSERT(emm_ctrl_ptr->forbidden_for_service_list_ptr->tai_lst_length == attac_rej_counter);
   /* PRINT the tai list */
  for(i=0;i<emm_ctrl_ptr->forbidden_for_service_list_ptr->tai_lst_length;i++)
  {
   TF_MSG("Forbidden list[%d] tac: %d",i,
            emm_ctrl_ptr->forbidden_for_service_list_ptr->diff_plmn[i].tac,0);
   //TF_MSG("plmn[0] %d plmn[1] %d plmn[2] %d",
   //          emm_ctrl_ptr->forbidden_for_service_list_ptr->diff_plmn[i].plmn.identity[0],
   //          emm_ctrl_ptr->forbidden_for_service_list_ptr->diff_plmn[i].plmn.identity[1],
   //          emm_ctrl_ptr->forbidden_for_service_list_ptr->diff_plmn[i].plmn.identity[2]);
  }
  
} /* emm_test_attach_reject_forbidden_list */


void emm_test_stop_mode_pdn_conn_req_collision
(
  msgr_client_t          *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  send_active_eps_ind = FALSE;
  boolean                  keep_the_test_running = TRUE ;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SERVICE_REQ ");
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      stub_rx.rrc_service_req.req_plmn); 
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_stop_mode_req(SYS_STOP_MODE_REASON_IRAT);        
        break;
      case LTE_RRC_DEACTIVATE_REQ:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case NAS_ESM_FAILURE_IND:
        keep_the_test_running = FALSE;
        break;
      default:
        break;
    }
    TF_YIELD();
  }

} /* emm_test_stop_mode_pdn_conn_req_collision */

void emm_test_stop_mode_offline_t3411_collision
(
  msgr_client_t          *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  send_active_eps_ind = FALSE;
  boolean                  keep_the_test_running = TRUE ;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      stub_rx.rrc_service_req.req_plmn); 
        break;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD();
        TF_SLEEP(15*1000); //Expire 3410 timer
        break;
      case LTE_RRC_CONN_ABORT_REQ:
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_ABORTED);
        TF_YIELD();
        nas_stub_send_stop_mode_req(SYS_STOP_MODE_REASON_OFFLINE); 
        break;
      case LTE_RRC_DEACTIVATE_REQ:
        TF_ASSERT(mm_timer_status[TIMER_T3411] == TIMER_STOPPED);
        nas_stub_send_rrc_deactivate_cnf(TRUE);
        break;
      case NAS_ESM_FAILURE_IND:
        keep_the_test_running = FALSE;
        break;
      default:
        break;
    }
    TF_YIELD();
  }

} /* emm_test_stop_mode_offline_t3411_collision */

void emm_test_stop_mode_deepsleep_t3411_collision
(
  msgr_client_t          *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  send_active_eps_ind = FALSE;
  boolean                  keep_the_test_running = TRUE ;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      stub_rx.rrc_service_req.req_plmn); 
        break;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD();
        TF_SLEEP(15*1000); //Expire 3410 timer
        break;
      case LTE_RRC_CONN_ABORT_REQ:
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_ABORTED);
        TF_YIELD();
        nas_stub_send_stop_mode_req(SYS_STOP_MODE_REASON_DEEP_SLEEP);
        break;
      case LTE_RRC_DEACTIVATE_REQ:
        TF_SLEEP(10*1000); //Expire T3411   
        nas_stub_send_rrc_deactivate_cnf(TRUE);
        TF_YIELD();
        keep_the_test_running = FALSE;
        break;
      default:
        break;
    }
    TF_YIELD();
  }

} /* emm_test_stop_mode_deepsleep_t3411_collision */

void emm_test_pdn_conn_req_stop_mode
(
  msgr_client_t          *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  send_active_eps_ind = FALSE;
  boolean                  keep_the_test_running = TRUE ;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SERVICE_REQ ");
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      stub_rx.rrc_service_req.req_plmn); 
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_stop_mode_req(SYS_STOP_MODE_REASON_IRAT);        
        break;
      case LTE_RRC_DEACTIVATE_REQ:
        nas_stub_send_rrc_deactivate_cnf(TRUE);
        TF_YIELD();
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case NAS_ESM_FAILURE_IND:
        keep_the_test_running = FALSE;
        break;
      default:
        break;
    }
    TF_YIELD();
  }

} /* emm_test_pdn_conn_req_stop_mode */

void emm_test_cell_chg_b4_attach_complete_ack
(
  msgr_client_t          *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      stub_rx.rrc_service_req.req_plmn); 
        break;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_attach_accept_and_def_bearer_ctxt_req(ESM_INVALID_BEARER_ID, 
                                                            ESM_INVALID_PTI);
        TF_YIELD();
        break;
      case NAS_ESM_DATA_IND:
        nas_stub_send_emm_data_req();
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        nas_stub_send_rrc_service_ind_with_param(0xFFFFFFFF,req_plmn,2,12345);
        TF_YIELD();
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        TF_YIELD();
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        break ;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        keep_the_test_running = FALSE;
        break;
      default:
        break;
    }
    TF_YIELD();
  }

} /* emm_test_cell_chg_b4_attach_complete_ack */

void emm_test_cell_chg_after_access_barring
(
  msgr_client_t          *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  rej_conn_est_req = TRUE;
  boolean                  keep_the_test_running = TRUE ;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      stub_rx.rrc_service_req.req_plmn); 
        break;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        if(rej_conn_est_req == TRUE)
        {
          nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED);
          rej_conn_est_req = FALSE;
          TF_YIELD();
          TF_YIELD();
          nas_stub_send_rrc_service_ind_with_param(0xFFFFFFFF,req_plmn,2,12345);
        }
        else
        {
          nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                        stub_rx.rrc_conn_est_req.transaction_id);
          nas_stub_send_attach_accept_and_def_bearer_ctxt_req(ESM_INVALID_BEARER_ID, 
                                                              ESM_INVALID_PTI);
        }        
        break;
      case NAS_ESM_DATA_IND:
        nas_stub_send_emm_data_req();
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        TF_YIELD();
        keep_the_test_running = FALSE;
        break;
      default:
        break;
    }
    TF_YIELD();
  }

} /* emm_test_cell_chg_after_access_barring */

void emm_test_access_barring_srv_ind_same_cell
(
  msgr_client_t          *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  rej_conn_est_req = TRUE;
  boolean                  keep_the_test_running = TRUE ;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      stub_rx.rrc_service_req.req_plmn); 
        break;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        if(rej_conn_est_req == TRUE)
        {
          nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED);
          rej_conn_est_req = FALSE;
          TF_YIELD();
          TF_YIELD();
          nas_stub_send_rrc_service_ind(0xFFFFFFFF, req_plmn);
          TF_YIELD();
          TF_YIELD();
          TF_SLEEP(1000);
        }
        else
        {
          nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                        stub_rx.rrc_conn_est_req.transaction_id);
          nas_stub_send_attach_accept_and_def_bearer_ctxt_req(ESM_INVALID_BEARER_ID, 
                                                              ESM_INVALID_PTI);
        }        
        break;
      case NAS_ESM_DATA_IND:
        nas_stub_send_emm_data_req();
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        TF_YIELD();
        keep_the_test_running = FALSE;
        break;
      default:
        break;
    }
    TF_YIELD();
  }

} /* emm_test_access_barring_srv_ind_same_cell */

void emm_test_plmn_chg_after_t3402
(
  msgr_client_t          *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  lte_rrc_plmn_s           plmn_b;
  boolean                  attach_rej = TRUE;
  dword                    bytes_rcvd;
  boolean                  rej_conn_est_req = TRUE;
  boolean                  keep_the_test_running = TRUE ;
  emm_ctrl_data_type      *emm_ctrl_ptr = NULL;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();

  emm_ctrl_ptr = emm_db_get_ctrl_data();

  /* Enable backoff PLMN list feature */
  emm_ctrl_ptr->temp_fplmn_backoff_time = 0xFFFFFFFF;

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                                          SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      stub_rx.rrc_service_req.req_plmn);
        TF_YIELD();
        TF_YIELD();
        if(stub_rx.rrc_service_req.network_select_mode == 
                        SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)
        {
          memset((void *)&plmn_b, 0x1, sizeof(lte_rrc_plmn_s));
          nas_stub_send_rrc_service_ind(0xFFFFFFFF, plmn_b);
        }
        break;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD();

        if(attach_rej == TRUE)
        {
          emm_test_emm_cause = LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED;
          nas_stub_send_attach_reject(LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED);
          TF_YIELD();
          nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);  
          TF_YIELD();
          TF_YIELD();
        }
        else
        {
          nas_stub_send_only_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL, 
                                                    FALSE);
          TF_YIELD();
          nas_stub_send_rrc_service_ind(0xFFFFFFFF, plmn_b);
          TF_YIELD();
          TF_YIELD();
        }        
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        if(attach_rej == TRUE)
        {
          nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                         SYS_RAT_LTE_RADIO_ACCESS,
                         SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION);
          attach_rej = FALSE;
        }
        else
        {
          nas_stub_send_rrc_service_ind(0xFFFFFFFF, req_plmn);
          TF_YIELD();
          TF_YIELD();
          keep_the_test_running = FALSE;
        }
        break;
      default:
        break;
    }
    TF_YIELD();
  }

} /* emm_test_plmn_chg_after_t3402 */

void emm_test_attach_reject_cause_15_manual
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type     stub_rx ;
  errno_enum_type  e_retval;
  dword            bytes_rcvd;
  boolean          no_srv_ind = FALSE;
  lte_rrc_plmn_s   plmn;
  boolean          keep_the_test_running = TRUE;

  memset((void *)&plmn, 0, sizeof(lte_rrc_plmn_s));

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);

  /* Automatic mode */
  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        if(no_srv_ind == FALSE)
        {
          req_plmn = stub_rx.rrc_service_req.req_plmn;
          nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id,
                                        stub_rx.rrc_service_req.req_plmn);
          no_srv_ind = TRUE;
        }
        else
        {
          nas_stub_send_no_svc_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                               plmn,
                                               SYS_NO_SVC_CAUSE_NORMAL);
          TF_YIELD();
          TF_YIELD();
          keep_the_test_running = FALSE;
        }
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD();
        emm_test_emm_cause = LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA;
        nas_stub_send_attach_reject(emm_test_emm_cause);
        TF_YIELD();
        break ;
      case NAS_ESM_FAILURE_IND:
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                  SYS_RAT_LTE_RADIO_ACCESS,
                                  SYS_NETWORK_SELECTION_MODE_AUTOMATIC);
        TF_YIELD();
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        TF_YIELD();
      default:
        break ;
    }
    TF_YIELD(); /* to allow any other task that is waiting to run*/
  }

  /* Manual mode */
  keep_the_test_running = TRUE;
  no_srv_ind = FALSE;

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_MANUAL);

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        if(no_srv_ind == FALSE)
        {
          nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id,
                                        stub_rx.rrc_service_req.req_plmn);
          no_srv_ind = TRUE;
        }
        else
        {
          nas_stub_send_no_svc_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                               plmn,
                                               SYS_NO_SVC_CAUSE_NORMAL);
          TF_YIELD();
          TF_YIELD();
          keep_the_test_running = FALSE;
        }
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD();
        emm_test_emm_cause = LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA;
        nas_stub_send_attach_reject(emm_test_emm_cause);
        TF_YIELD();
        break;
      case NAS_ESM_FAILURE_IND:
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                  SYS_RAT_LTE_RADIO_ACCESS,
                                  SYS_NETWORK_SELECTION_MODE_MANUAL);
        TF_YIELD();
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        TF_YIELD();
      default:
        break ;
    }
    TF_YIELD(); /* to allow any other task that is waiting to run*/
  }

}/* emm_test_attach_reject_cause_15_manual */

void emm_test_attach_reject_cause_11_manual
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type     stub_rx ;
  errno_enum_type  e_retval;
  dword            bytes_rcvd;
  byte             manual_reg_attempt = 0;
  boolean          keep_the_test_running = TRUE;
  
  (void)reg_sim_load_card_mode(); 
  
//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);

  /* Automatic mode */
  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id,
                                      stub_rx.rrc_service_req.req_plmn);
        TF_YIELD();
        TF_YIELD();
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD();
        emm_test_emm_cause = LTE_NAS_PLMN_NOT_ALLOWED;
        nas_stub_send_attach_reject(emm_test_emm_cause);
        TF_YIELD();
        break ;
      case NAS_ESM_FAILURE_IND:
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        keep_the_test_running = FALSE;
      default:
        break ;
    }
    TF_YIELD(); /* to allow any other task that is waiting to run*/
  }

  /* Manual mode */
  keep_the_test_running = TRUE;

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_MANUAL);

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id,
                                      stub_rx.rrc_service_req.req_plmn);
        if(manual_reg_attempt == 1)
        {
          keep_the_test_running = FALSE;
        }
        break;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        manual_reg_attempt ++;
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD();
        emm_test_emm_cause = LTE_NAS_PLMN_NOT_ALLOWED;
        nas_stub_send_attach_reject(emm_test_emm_cause);
        TF_YIELD();
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        TF_YIELD();
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                  SYS_RAT_LTE_RADIO_ACCESS,
                                  SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION);
        break;
      default:
        break ;
    }
    TF_YIELD(); /* to allow any other task that is waiting to run*/
  }

  ASSERT(EMM_GET_STATE() == EMM_DEREGISTERED);
  ASSERT(EMM_GET_SUBSTATE() != EMM_DEGEGISTERED_WAITING_PDN_CONN_REQ);

}/* emm_test_attach_reject_cause_11_manual */

void emm_test_attach_max_attempts_manual
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type     stub_rx ;
  errno_enum_type  e_retval;
  dword            bytes_rcvd;
  byte             manual_reg_attempt = 0;
  boolean          keep_the_test_running = TRUE;
  emm_ctrl_data_type  *emm_ctrl_ptr = NULL;
  
  (void)reg_sim_load_card_mode(); 
  
//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);

  /* Enable backoff PLMN list feature */
  emm_ctrl_ptr = emm_db_get_ctrl_data();
  emm_ctrl_ptr->temp_fplmn_backoff_time = 0xFFFFFFFF;

  /* Automatic mode */
  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id,
                                      stub_rx.rrc_service_req.req_plmn);
        TF_YIELD();
        TF_YIELD();
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD();
        emm_test_emm_cause = LTE_NAS_PLMN_NOT_ALLOWED;
        nas_stub_send_attach_reject(emm_test_emm_cause);
        TF_YIELD();
        break ;
      case NAS_ESM_FAILURE_IND:
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        keep_the_test_running = FALSE;
      default:
        break ;
    }
    TF_YIELD(); /* to allow any other task that is waiting to run*/
  }

  /* Manual mode */
  keep_the_test_running = TRUE;

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_MANUAL);

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id,
                                      stub_rx.rrc_service_req.req_plmn);
        if(manual_reg_attempt == 1)
        {
          keep_the_test_running = FALSE;
        }
        break;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        manual_reg_attempt ++;
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD();
        emm_test_emm_cause = LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED;
        nas_stub_send_attach_reject(emm_test_emm_cause);
        TF_YIELD();
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        TF_YIELD();
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                  SYS_RAT_LTE_RADIO_ACCESS,
                                  SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION);
        break;
      default:
        break ;
    }
    TF_YIELD(); /* to allow any other task that is waiting to run*/
  }

  ASSERT(EMM_GET_STATE() == EMM_DEREGISTERED);
  ASSERT(EMM_GET_SUBSTATE() != EMM_DEGEGISTERED_WAITING_PDN_CONN_REQ);

}/* emm_test_attach_max_attempts_manual */

void emm_test_attach_lmited_manual
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type     stub_rx ;
  errno_enum_type  e_retval;
  dword            bytes_rcvd;
  boolean          keep_the_test_running = TRUE;
  emm_ctrl_data_type  *emm_ctrl_ptr = NULL;
  boolean          no_srv_ind = TRUE;
  lte_rrc_plmn_s   plmn;

  memset((void *)&plmn, 0, sizeof(lte_rrc_plmn_s));
  
  (void)reg_sim_load_card_mode(); 
  
//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);

  /* Enable backoff PLMN list feature */
  emm_ctrl_ptr = emm_db_get_ctrl_data();
  emm_ctrl_ptr->temp_fplmn_backoff_time = 0xFFFFFFFF;

  /* Automatic mode */
  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id,
                                      stub_rx.rrc_service_req.req_plmn);
        TF_YIELD();
        TF_YIELD();
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD();
        emm_test_emm_cause = LTE_NAS_PLMN_NOT_ALLOWED;
        nas_stub_send_attach_reject(emm_test_emm_cause);
        TF_YIELD();
        break ;
      case NAS_ESM_FAILURE_IND:
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        keep_the_test_running = FALSE;
      default:
        break ;
    }
    TF_YIELD(); /* to allow any other task that is waiting to run*/
  }

  /* Manual mode */
  keep_the_test_running = TRUE;

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_MANUAL);

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        if(no_srv_ind == TRUE)
        {
          nas_stub_send_no_svc_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                               plmn,
                                               SYS_NO_SVC_CAUSE_NORMAL);
          no_srv_ind = FALSE;
          TF_YIELD();
          nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                    SYS_RAT_LTE_RADIO_ACCESS,
                                    SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION);
        }
        else
        {
          nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id,
                                        stub_rx.rrc_service_req.req_plmn);
        }
        TF_YIELD();
        break;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        keep_the_test_running = FALSE;
        break;
      default:
        break ;
    }
    TF_YIELD(); /* to allow any other task that is waiting to run*/
  }

  ASSERT(EMM_GET_STATE() == EMM_REGISTERED_INITIATED);
  ASSERT(EMM_GET_SUBSTATE() == EMM_WAITING_FOR_NW_RESPONSE);

}/* emm_test_attach_lmited_manual */

void emm_test_attach_manual_conn_rel
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type     stub_rx ;
  errno_enum_type  e_retval;
  dword            bytes_rcvd;
  boolean          release_conn = TRUE;
  boolean          keep_the_test_running = TRUE;
  
  (void)reg_sim_load_card_mode(); 
  
//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);

  /* Automatic mode */
  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id,
                                      stub_rx.rrc_service_req.req_plmn);
        TF_YIELD();
        TF_YIELD();
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD();
        emm_test_emm_cause = LTE_NAS_PLMN_NOT_ALLOWED;
        nas_stub_send_attach_reject(emm_test_emm_cause);
        TF_YIELD();
        break ;
      case NAS_ESM_FAILURE_IND:
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        keep_the_test_running = FALSE;
      default:
        break ;
    }
    TF_YIELD(); /* to allow any other task that is waiting to run*/
  }

  /* Manual mode */
  keep_the_test_running = TRUE;

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_MANUAL);

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id,
                                      stub_rx.rrc_service_req.req_plmn);
        break;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD();
        if(release_conn == TRUE)
        {
          nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
          release_conn = FALSE;
          TF_YIELD();
          TF_SLEEP(10 * 1000);
        }
        else
        {
          keep_the_test_running = FALSE;   
        }
        TF_YIELD();
        break;
      default:
        break ;
    }
    TF_YIELD(); /* to allow any other task that is waiting to run*/
  }

  ASSERT(EMM_GET_STATE() == EMM_REGISTERED_INITIATED);
  ASSERT(EMM_GET_SUBSTATE() == EMM_WAITING_FOR_NW_RESPONSE);

}/* emm_attach_manual_conn_rel */

void emm_test_attach_double_service_req
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type     stub_rx ;
  errno_enum_type  e_retval;
  dword            bytes_rcvd;
  emm_ctrl_data_type  *emm_ctrl_ptr = NULL;
  uint32           attach_req_trans_id = 0;
  boolean          second_srv_req = FALSE;
  boolean          keep_the_test_running = TRUE;
  
  emm_ctrl_ptr = emm_db_get_ctrl_data();
  
//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id,
                                      stub_rx.rrc_service_req.req_plmn);
        TF_YIELD();
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        attach_req_trans_id = stub_rx.rrc_conn_est_req.transaction_id;
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        TF_YIELD();
        TF_YIELD();
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                  SYS_RAT_LTE_RADIO_ACCESS,
                                  SYS_NETWORK_SELECTION_MODE_AUTOMATIC);
        TF_YIELD();
        TF_YIELD();
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD();
        nas_stub_send_attach_accept_and_def_bearer_ctxt_req(5, 1);
        break;
      case NAS_ESM_DATA_IND:
        nas_stub_send_emm_data_req();
        break ;
      case LTE_RRC_UL_DATA_REQ:
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        break;
      case NAS_EMM_ATTACH_COMPLETE_IND:
        keep_the_test_running = FALSE;
        break;
      default:
        break ;
    }
    TF_YIELD(); /* to allow any other task that is waiting to run*/
  }

  ASSERT(emm_ctrl_ptr->reg_req_pended_indication == FALSE);

}/* emm_test_attach_double_service_req */

void emm_test_attach_plmn_chg_after_t3411
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  lte_rrc_plmn_s           plmn_b;
  boolean                  attach_rej = TRUE;
  boolean                  end_test = FALSE;
  dword                    bytes_rcvd;
  boolean                  rej_conn_est_req = TRUE;
  boolean                  keep_the_test_running = TRUE ;
  emm_ctrl_data_type      *emm_ctrl_ptr = NULL;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();

  emm_ctrl_ptr = emm_db_get_ctrl_data();

  /* Enable backoff PLMN list feature */
  emm_ctrl_ptr->temp_fplmn_backoff_time = 0xFFFFFFFF;

  memset((void *)&plmn_b, 0x1, sizeof(lte_rrc_plmn_s));

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      stub_rx.rrc_service_req.req_plmn);
        TF_YIELD();
        if(end_test == TRUE)
        {
          TF_SLEEP(10 * 1000);
          TF_YIELD();
          keep_the_test_running = FALSE;
        }
        break;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD();
        if(attach_rej == TRUE)
        {
          emm_test_emm_cause = LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED;
          nas_stub_send_attach_reject(LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED);
          TF_YIELD();
          nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);  
          TF_YIELD();
          TF_YIELD();
        }
        else
        {
          nas_stub_send_only_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL, 
                                                    FALSE);
          TF_YIELD();
          nas_stub_send_rrc_service_ind(0xFFFFFFFF, plmn_b);
          TF_YIELD();
          TF_YIELD();
        }        
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        if(attach_rej == TRUE)
        {
          nas_stub_send_rrc_service_ind(0xFFFFFFFF, plmn_b);
          attach_rej = FALSE;
        }
        else
        {
          nas_stub_send_no_svc_rrc_service_ind(0xFFFFFFFF, 
                                               plmn_b,
                                               SYS_NO_SVC_CAUSE_NORMAL);
          TF_YIELD();
          TF_YIELD();
          nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                    SYS_RAT_LTE_RADIO_ACCESS,
                                    SYS_NETWORK_SELECTION_MODE_AUTOMATIC); 
          end_test = TRUE; 
        }
        break;
      default:
        break;
    }
    TF_YIELD();
  }

  ASSERT(mm_timer_status[TIMER_T3402] == TIMER_STOPPED);
  ASSERT(EMM_GET_STATE() == EMM_DEREGISTERED);

}/* emm_test_attach_plmn_chg_after_t3411 */

void emm_test_attach_oos_during_t3411
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  boolean                  conn_rel = TRUE ;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        /* Save the rrc plmn */
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      stub_rx.rrc_service_req.req_plmn);
        TF_YIELD();
        TF_YIELD();
        if(conn_rel == FALSE) 
        {
          /* Sleep 10 sec to expire T3411 */
          TF_SLEEP(10 * 1000);
        }
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        if(conn_rel == TRUE) 
        {
          nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
          conn_rel = FALSE;
        }
        else
        {
          keep_the_test_running = FALSE;
        }
        TF_YIELD();
        break ;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        nas_stub_send_no_svc_rrc_service_ind(0xFFFFFFFF, 
                                             req_plmn,
                                             SYS_NO_SVC_CAUSE_NORMAL);
        TF_YIELD();
        TF_YIELD();
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                  SYS_RAT_LTE_RADIO_ACCESS,
                                  SYS_NETWORK_SELECTION_MODE_AUTOMATIC); 
        break;
      default:
        break ;
    }

    TF_YIELD(); 
  }
} /* emm_test_attach_oos_during_t3411 */

void emm_test_attach_oos_rat_chg_during_t3411
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  boolean                  conn_rel = TRUE ;

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        /* Save the rrc plmn */
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      stub_rx.rrc_service_req.req_plmn);
        TF_YIELD();
        TF_YIELD();
        if(conn_rel == FALSE) 
        {
          /* Sleep 10 sec to expire T3411 */
          TF_SLEEP(10 * 1000);
        }
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        if(conn_rel == TRUE) 
        {
          nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
          conn_rel = FALSE;
        }
        else
        {
          keep_the_test_running = FALSE;
        }
        TF_YIELD();
        break ;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        nas_stub_send_no_svc_rrc_service_ind(0xFFFFFFFF, 
                                             req_plmn,
                                             SYS_NO_SVC_CAUSE_NORMAL);
        TF_YIELD();
        TF_YIELD();
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                  SYS_RAT_UMTS_RADIO_ACCESS,
                                  SYS_NETWORK_SELECTION_MODE_AUTOMATIC); 
        break;
      case LTE_RRC_DEACTIVATE_REQ:
        nas_stub_send_rrc_deactivate_cnf(TRUE);
        TF_YIELD();
        TF_YIELD();
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                  SYS_RAT_LTE_RADIO_ACCESS,
                                  SYS_NETWORK_SELECTION_MODE_AUTOMATIC); 
        break;
      default:
        break ;
    }

    TF_YIELD(); 
  }
} /* emm_test_attach_oos_rat_chg_during_t3411 */

void emm_test_attach_complete_tx_failure_rlf
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  boolean                  rlf = FALSE;
  boolean                  second_conn_est = FALSE;

  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  
  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        if(rlf == FALSE)
        {
          req_plmn = stub_rx.rrc_service_req.req_plmn;
          nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id,
                                        stub_rx.rrc_service_req.req_plmn);
          TF_YIELD();
        }
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        if(second_conn_est == FALSE)
        {
          nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                        stub_rx.rrc_conn_est_req.transaction_id);
          TF_YIELD();
          nas_stub_send_attach_accept_and_def_bearer_ctxt_req(5, 1);
          second_conn_est= TRUE;
        }
        else
        {
          keep_the_test_running = FALSE;
        }
        break;
      case NAS_ESM_DATA_IND:
        nas_stub_send_emm_data_req();
        break ;
      case LTE_RRC_UL_DATA_REQ:
        nas_stub_send_no_svc_rrc_service_ind(0xFFFFFFFF, 
                                             req_plmn,
                                             SYS_NO_SVC_CAUSE_RLF);
        rlf = TRUE;
        TF_YIELD();
        TF_YIELD();
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                  SYS_RAT_LTE_RADIO_ACCESS,
                                  SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  
        TF_YIELD();
        TF_YIELD();
        nas_stub_send_only_rrc_connection_rel_ind(LTE_RRC_CONN_REL_CRE_FAILURE,
                                                  FALSE);
        break;
      case NAS_ESM_DETACH_IND:
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                  SYS_RAT_LTE_RADIO_ACCESS,
                                  SYS_NETWORK_SELECTION_MODE_AUTOMATIC); 
        rlf = FALSE;
        break;
      default:
        break ;
    }
    TF_YIELD(); /* to allow any other task that is waiting to run*/
  }

   ASSERT(EMM_GET_STATE() == EMM_REGISTERED_INITIATED);

} /* emm_test_attach_complete_tx_failure_rlf */

void emm_test_plmn_chg_forbidden
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  lte_rrc_plmn_s           plmn_b;
  boolean                  camp_on_plmnb = FALSE;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  emm_ctrl_data_type      *emm_ctrl_ptr = NULL;

  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();

  emm_ctrl_ptr = emm_db_get_ctrl_data();

  memset((void *)&plmn_b, 0x1, sizeof(lte_rrc_plmn_s));
#ifdef FEATURE_DUAL_SIM
  if (reg_sim_add_plmn_to_gprs_fplmn_list( emm_convert_rrc_plmn_id_to_nas_plmn_id(plmn_b), FALSE, (sys_modem_as_id_e_type)mm_sub_id))
#else
  if (reg_sim_add_plmn_to_gprs_fplmn_list(emm_convert_rrc_plmn_id_to_nas_plmn_id(plmn_b),FALSE))
#endif /*FEATURE_DUAL_SIM|| FEATURE_SGLTE*/ 
  {
#ifdef FEATURE_LTE_TO_1X
    mm_send_mmr_fplmn_list_change_ind(SYS_FORBIDDEN_LIST_TYPE_GPRS_FPLMN);
#endif
  }

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        if(camp_on_plmnb == FALSE)
        {
          req_plmn = stub_rx.rrc_service_req.req_plmn;
          nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                        stub_rx.rrc_service_req.req_plmn);
          TF_YIELD();
          if(mm_timer_status[TIMER_T3402] == TIMER_ACTIVE)
          {
            ASSERT(EMM_GET_STATE() == EMM_DEREGISTERED);
            TF_SLEEP(720 * 1000);
            keep_the_test_running = FALSE;
          }
        }
        else
        {
          nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                        plmn_b);
          TF_YIELD();
          ASSERT(mm_timer_status[TIMER_T3402] == TIMER_ACTIVE);
          ASSERT(EMM_GET_STATE() == EMM_DEREGISTERED);
          camp_on_plmnb = FALSE;
          nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                    SYS_RAT_LTE_RADIO_ACCESS,
                                    SYS_NETWORK_SELECTION_MODE_AUTOMATIC); 
        }
        break;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD();
        emm_test_emm_cause = LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED;
        nas_stub_send_attach_reject(LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED);
        TF_YIELD();
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);  
        TF_YIELD();
        TF_YIELD();
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                  SYS_RAT_LTE_RADIO_ACCESS,
                                  SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION); 
        camp_on_plmnb = TRUE;
        break;
      default:
        break;
    }
    TF_YIELD();
  }

  ASSERT(EMM_GET_STATE() == EMM_REGISTERED_INITIATED);

}/* emm_test_plmn_chg_forbidden */

void emm_test_2nd_srv_req_while_waiting_pdn
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  byte                     srv_req_cnt = 0;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;

  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        srv_req_cnt++;
        if(srv_req_cnt == 2)
        {
          nas_stub_send_pdn_connectivity_req_in_esm_data_req();
          TF_YIELD();
          TF_YIELD();
          ASSERT(EMM_GET_STATE() == EMM_DEREGISTERED);
          ASSERT(EMM_GET_SUBSTATE() == EMM_DEREGISTERED_ATTACH_NEEDED);
          ASSERT(emm_ctrl_data_ptr->pdn_conn_req_trans_id != 
                                            EMM_INVALID_PDN_CONN_REQ_TRANS_ID);
        }
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      stub_rx.rrc_service_req.req_plmn);
        break;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                  SYS_RAT_LTE_RADIO_ACCESS,
                                  SYS_NETWORK_SELECTION_MODE_AUTOMATIC);
        break;
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        ASSERT(EMM_GET_STATE() == EMM_REGISTERED_INITIATED);
        keep_the_test_running = FALSE;
        TF_YIELD();
        break;
      default:
        break;
    }
    TF_YIELD();
  }

  ASSERT(EMM_GET_STATE() == EMM_REGISTERED_INITIATED);

}/* emm_test_2nd_srv_req_while_waiting_pdn */

void emm_test_nonsuitable_cell_chg_while_waiting_pdn
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  lte_rrc_plmn_s           plmn_b;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  emm_ctrl_data_type      *emm_ctrl_ptr = NULL;

  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();

  emm_ctrl_ptr = emm_db_get_ctrl_data();

  memset((void *)&plmn_b, 0x1, sizeof(lte_rrc_plmn_s));

#ifdef FEATURE_DUAL_SIM
  if (reg_sim_add_plmn_to_gprs_fplmn_list( emm_convert_rrc_plmn_id_to_nas_plmn_id(plmn_b), FALSE, (sys_modem_as_id_e_type)mm_sub_id))
#else
  if (reg_sim_add_plmn_to_gprs_fplmn_list(emm_convert_rrc_plmn_id_to_nas_plmn_id(plmn_b), FALSE))
#endif /*FEATURE_DUAL_SIM|| FEATURE_SGLTE*/ 
  {
#ifdef FEATURE_LTE_TO_1X
    mm_send_mmr_fplmn_list_change_ind(SYS_FORBIDDEN_LIST_TYPE_GPRS_FPLMN);
#endif
  }
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      stub_rx.rrc_service_req.req_plmn);

        break;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      plmn_b);
        TF_YIELD();
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case NAS_ESM_FAILURE_IND:
        keep_the_test_running = FALSE;
        break;
      default:
        break;
    }
    TF_YIELD();
  }

  ASSERT(EMM_GET_STATE() == EMM_DEREGISTERED);
  ASSERT(EMM_GET_SUBSTATE() == EMM_DEREGISTERED_PLMN_SEARCH);
  ASSERT(emm_ctrl_data_ptr->pdn_conn_req_trans_id == 
                                            EMM_INVALID_PDN_CONN_REQ_TRANS_ID);

}/* emm_test_nonsuitable_cell_chg_while_waiting_pdn */

void emm_test_srv_req_during_attach_conn_setup
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type     stub_rx ;
  errno_enum_type  e_retval;
  dword            bytes_rcvd;
  boolean          second_srv_req_sent = FALSE;
  boolean          keep_the_test_running = TRUE;
  
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();

  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id,
                                      stub_rx.rrc_service_req.req_plmn);
        TF_YIELD();
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        if(second_srv_req_sent == FALSE)
        {
          nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_CS_PS,
                                    SYS_RAT_LTE_RADIO_ACCESS,
                                    SYS_NETWORK_SELECTION_MODE_AUTOMATIC);
          second_srv_req_sent = TRUE;
        }
        else
        {
          keep_the_test_running = FALSE;
        }
        TF_YIELD();
        break;
      case LTE_RRC_CONN_ABORT_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_FAILURE_ABORTED);
        ASSERT(EMM_GET_STATE() == EMM_DEREGISTERED);
        break;
      default:
        break ;
    }
    TF_YIELD(); /* to allow any other task that is waiting to run*/
  }

  ASSERT(EMM_GET_STATE() == EMM_REGISTERED_INITIATED);

}/* emm_test_srv_req_during_attach_conn_setup */

void emm_test_srv_req_waiting_attach_pdn_connected
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type     stub_rx ;
  errno_enum_type  e_retval;
  dword            bytes_rcvd;
  boolean          second_srv_req_sent = FALSE;
  boolean          keep_the_test_running = TRUE;
  
  /* Trigger service request procedure */
  nas_stub_send_emm_data_req();

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        if(second_srv_req_sent == FALSE)
        {
          nas_stub_send_service_reject(LTE_NAS_IMPLICITLY_DETACHED);
          nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        }
        else
        {
          keep_the_test_running = FALSE;
        }
        break;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                  SYS_RAT_LTE_RADIO_ACCESS,
                                  SYS_NETWORK_SELECTION_MODE_AUTOMATIC);
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        second_srv_req_sent = TRUE;
        break;
       case MM_CM_PDN_CONNECTIVITY_PACKED_REQUEST_IND:
        esm_ut_build_and_send_cm_pkd_pdn_conn_req_cnf(
                                    stub_rx.pkd_pdn_conn_req_ind.esm_hdr,
		                                stub_rx.pkd_pdn_conn_req_ind.pkd_msg_len,
                                    stub_rx.pkd_pdn_conn_req_ind.pti,
		                                stub_rx.pkd_pdn_conn_req_ind.pkd_msg,
		                                ESM_MSG_ACCEPTED);
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_SIG_CONNECTION_RELEASED_IND ");
        break ;
      case LTE_RRC_CONN_ABORT_REQ:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        TF_YIELD();
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_ABORTED);
        break;
      case LTE_RRC_SERVICE_REQ:
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id,
                                      stub_rx.rrc_service_req.req_plmn);
        TF_YIELD();
        break ;
      default:
        break ;
    }
    TF_YIELD(); /* to allow any other task that is waiting to run*/
  }

  ASSERT(EMM_GET_STATE() == EMM_REGISTERED_INITIATED);

}/* emm_test_srv_req_waiting_attach_pdn_connected */

void emm_test_attach_complete_tx_failure_ta_chg
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                            SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  
  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_SERVICE_REQ:
        req_plmn = stub_rx.rrc_service_req.req_plmn;
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id,
                                      stub_rx.rrc_service_req.req_plmn);
        TF_YIELD();
        break ;
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
        break;
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        TF_YIELD();
        nas_stub_send_attach_accept_and_def_bearer_ctxt_req(5, 1);
        break;
      case NAS_ESM_DATA_IND:
        nas_stub_send_emm_data_req();
        break ;
      case LTE_RRC_UL_DATA_REQ:
        nas_stub_send_rrc_service_ind_with_param(0xFFFFFFFF,req_plmn,10,800);
        TF_YIELD();
        TF_YIELD();
        ASSERT(EMM_GET_STATE() != EMM_TRACKING_AREA_UPDATING_INITIATED);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_FAILURE_HO, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        TF_YIELD();
        TF_YIELD();
        keep_the_test_running = FALSE;
        break;
      default:
        break ;
    }
    TF_YIELD(); /* to allow any other task that is waiting to run*/
  }

} /* emm_test_attach_complete_tx_failure_ta_chg */


#endif /*FEATURE_LTE*/
