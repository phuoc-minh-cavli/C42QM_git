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
#include <customer.h>

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
  #include "emm.h"
  #include "gmm_int_v.h"  
  #include "qurt_signal.h" 
}
extern "C"
{ 
  extern boolean nas_msg_decoding_status_nft;
  extern byte no_of_ota_messages_nft;
  extern boolean ota_message_discarded_nft;
  extern boolean processing_done;
  extern boolean tlb_message_discarded_nft;
  void esm_fuzzing_reset(void);
  extern qurt_signal_t  signal_nas;
}
extern void FuzzOTAFile();

#include <emm_test_lib.h>
#include <emm_test_tau_lib.h>
extern void nas_stub_send_new_tac_rrc_service_ind(uint32 trans_id, lte_rrc_plmn_s plmn);
extern lte_rrc_plmn_s          req_plmn;
extern lte_nas_emm_cause_type emm_test_emm_cause;
extern lte_nas_emm_eps_update_result_type  update_result;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  extern boolean gmm_is_suspended_sim[MAX_NAS_STACKS];
  #define gmm_is_suspended gmm_is_suspended_sim[mm_as_id]
#else
  extern boolean  gmm_is_suspended;
#endif

extern void nas_int_emm_test_combined_attach_no_loop_fuzz
(
  stub_rx_type      *stub_ptr,
  msgr_client_t     *clt_ptr,
  boolean skip_start,
  boolean fuzz
);
extern  void stop_and_attach(boolean keep_running, msgr_client_t *client_ptr, qurt_signal_t* signal);
void emm_test_normal_periodic_tau(msgr_client_t       *client_ptr)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();


  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        switch(emm_ctrl_data_ptr->tau_cause)
        {
          case TAU_CAUSE_TA_CHANGE:
            TF_ASSERT(stub_rx.rrc_conn_est_req.include_s_tmsi == FALSE); 
            TF_ASSERT(stub_rx.rrc_conn_est_req.registered_mme_is_present == TRUE); 
            break;
          case TAU_CAUSE_LOAD_BALANCING:
            TF_ASSERT(stub_rx.rrc_conn_est_req.include_s_tmsi == FALSE); 
            TF_ASSERT(stub_rx.rrc_conn_est_req.registered_mme_is_present == FALSE); 
            break;
          default:
            TF_ASSERT(stub_rx.rrc_conn_est_req.include_s_tmsi == TRUE); 
            TF_ASSERT(stub_rx.rrc_conn_est_req.registered_mme_is_present == FALSE); 
            break;
        }
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_tau_accept();
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        TF_YIELD();
        keep_the_test_running = FALSE ;
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SIM_UPDATE_REQ");
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        break ;
      case NAS_EMM_EPS_BEARER_STATUS_CMD:
        TF_MSG("TEST> ESM sent NAS_EMM_EPS_BEARER_STATUS_CMD");
        break ;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_normal_periodic_tau */

void emm_test_normal_periodic_tau_fuzz(msgr_client_t       *client_ptr)
{

    stub_rx_type             stub_rx ;
    errno_enum_type          e_retval;
    dword                    bytes_rcvd;
    boolean                  keep_the_test_running = TRUE ;
    boolean                  tau_started = FALSE;
    boolean                  reset_in_progress = FALSE;
    emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
    lte_rrc_plmn_s plmn;
    plmn.mcc[0] = 0x03;
    plmn.mcc[1] = 0x00;
    plmn.mcc[2] = 0x02;
    plmn.mnc[0] = 0x03;
    plmn.mnc[1] = 0x08;
    plmn.mnc[2] = 0x00;
    plmn.num_mnc_digits = 2;     
    qurt_signal_init(&signal_nas);
#ifdef FEATURE_NAS_AFL 
    while(__AFL_LOOP(1000))
#endif
    {
        /* The test case will wait for the signal informing that the SIM is ready*/
        stop_and_attach(keep_the_test_running, client_ptr, &signal_nas);
        /* UE is attached now */
        qurt_signal_wait(&signal_nas, 2, QURT_SIGNAL_ATTR_WAIT_ANY);
        qurt_signal_clear(&signal_nas, 2);
        tau_started = FALSE;
        /* Force TAU */
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);  
        nas_stub_send_rrc_service_ind(1,plmn);

        emm_ctrl_data_ptr = emm_db_get_ctrl_data();
        keep_the_test_running = TRUE;
        while (keep_the_test_running)
        {
            e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
            switch (stub_rx.msg_hdr.id)
            {
            case LTE_RRC_CONN_EST_REQ:  
                nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
                nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
                processing_done = FALSE;
                ota_message_discarded_nft = FALSE;
                nas_msg_decoding_status_nft = FALSE;
                tlb_message_discarded_nft = FALSE;
                FuzzOTAFile();
                qurt_signal_wait(&signal_nas, 2, QURT_SIGNAL_ATTR_WAIT_ANY);                
                nas_msg_decoding_status_nft = FALSE; /* Reset OTA message decoding status */
                if (( no_of_ota_messages_nft == 0 ) || (ota_message_discarded_nft == TRUE)|| (tlb_message_discarded_nft == TRUE))
                {               
                    keep_the_test_running = FALSE;
                }
                tau_started = TRUE;
                break ;
            case LTE_RRC_UL_DATA_REQ:         
                nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);        
                break ;
            case LTE_RRC_SYSTEM_UPDATE_REQ:        
                break;
            case NAS_EMM_TAU_COMPLETE_IND: // andre IOT
                keep_the_test_running = FALSE ;
                break;
            case LTE_RRC_SIM_UPDATE_REQ:            
                if (tau_started)
                    keep_the_test_running = FALSE ;
                break;
            case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
                break ;
            case NAS_EMM_EPS_BEARER_STATUS_CMD:        
                break ;
            default:        
                break ;
            }
        }  // while (keep_the_test_running)
    }
    qurt_signal_destroy(&signal_nas);
} /* emm_test_normal_periodic_tau */

void emm_test_normal_periodic_tau_reject(msgr_client_t            *client_ptr, 
                                         lte_nas_emm_cause_type    emm_cause)
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
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_tau_reject(emm_cause);
        if((emm_cause==LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED) ||
           (emm_cause == LTE_NAS_IMPLICITLY_DETACHED) ||
           (emm_cause ==  LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW) )
        {
          nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
          keep_the_test_running = FALSE;
        }
        else if((emm_cause != LTE_NAS_PLMN_NOT_ALLOWED) &&
           (emm_cause != LTE_NAS_TRACKING_AREA_NOT_ALLOWED) &&
           (emm_cause != LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA) &&
           (emm_cause != LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN) &&
           (emm_cause != LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA))
        {
          keep_the_test_running = FALSE;
        }
        else
        {
          nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                    SYS_RAT_LTE_RADIO_ACCESS,
                                    SYS_NETWORK_SELECTION_MODE_AUTOMATIC); 
          TF_YIELD();
          /* Expire T3440 */
          TF_SLEEP(10 * 1000);
        }
        break;
      case LTE_RRC_CONN_ABORT_REQ:
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        break;
      case LTE_RRC_SERVICE_REQ:
        keep_the_test_running = FALSE;
        break;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_normal_periodic_tau_reject */


void emm_test_normal_periodic_tau_reject_fuzz(msgr_client_t            *client_ptr, 
                                         lte_nas_emm_cause_type    emm_cause)
{

    stub_rx_type             stub_rx ;
    errno_enum_type          e_retval;
    dword                    bytes_rcvd;
    boolean                  keep_the_test_running = TRUE ;
    boolean                  tau_started = FALSE;
    boolean                  reset_in_progress = FALSE;
    emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
    lte_rrc_plmn_s plmn;
    plmn.mcc[0] = 0x03;
    plmn.mcc[1] = 0x00;
    plmn.mcc[2] = 0x02;
    plmn.mnc[0] = 0x03;
    plmn.mnc[1] = 0x08;
    plmn.mnc[2] = 0x00;
    plmn.num_mnc_digits = 2;     
    qurt_signal_init(&signal_nas);

#ifdef FEATURE_NAS_AFL 
    while(__AFL_LOOP(1000))
#endif
    {
        /* The test case will wait for the signal informing that the SIM is ready*/
        stop_and_attach(keep_the_test_running, client_ptr, &signal_nas);
        /* UE is attached now */
        qurt_signal_wait(&signal_nas, 2, QURT_SIGNAL_ATTR_WAIT_ANY);
        qurt_signal_clear(&signal_nas, 2);
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);  
        nas_stub_send_rrc_service_ind(1,plmn);
        keep_the_test_running = TRUE;
        while (keep_the_test_running)
        {
            e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
            TF_ASSERT( e_retval == E_SUCCESS );

            switch (stub_rx.msg_hdr.id)
            {
            case NAS_ESM_DETACH_IND:
                keep_the_test_running = FALSE;
                break;
            case LTE_RRC_CONN_EST_REQ:              
                nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
                nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);

                //nas_stub_send_tau_reject(emm_cause);

                processing_done = FALSE;
                ota_message_discarded_nft = FALSE;
                nas_msg_decoding_status_nft = FALSE;
                tlb_message_discarded_nft = FALSE;
                FuzzOTAFile();
                qurt_signal_wait(&signal_nas, 2, QURT_SIGNAL_ATTR_WAIT_ANY);                
                nas_msg_decoding_status_nft = FALSE; /* Reset OTA message decoding status */
                if (( no_of_ota_messages_nft == 0 ) || (ota_message_discarded_nft == TRUE)|| (tlb_message_discarded_nft == TRUE))
                {               
                    keep_the_test_running = FALSE;
                }
                tau_started = TRUE;

                break ;
            case LTE_RRC_UL_DATA_REQ:         
                nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);        
                break ;          
            case LTE_RRC_CONN_ABORT_REQ:
                nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
                break;
            case LTE_RRC_SERVICE_REQ:
                if (tau_started)
                {
                    keep_the_test_running = FALSE ;
                }
                break;
            default:        
                break ;
            }
        }  // while (keep_the_test_running)
    }
    qurt_signal_destroy(&signal_nas);
} /* emm_test_normal_periodic_tau_reject */

void emm_test_normal_periodic_tau_reject_cause_95_thru_111(msgr_client_t *client_ptr, lte_nas_emm_cause_type  emm_cause)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();


  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
	    nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_tau_reject(emm_cause);
	      TF_SLEEP(720*1000);//Expire 3402 timer
        TF_YIELD(); /* to allow any other task that is waiting to run*/
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
	  nas_stub_send_tau_accept();
        TF_YIELD();
        keep_the_test_running = FALSE ;
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SIM_UPDATE_REQ");
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        break ;
      case NAS_EMM_EPS_BEARER_STATUS_CMD:
        TF_MSG("TEST> ESM sent NAS_EMM_EPS_BEARER_STATUS_CMD");
        break ;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_normal_periodic_tau_reject_cause_95_thru_111*/

void emm_test_normal_periodic_tau_t3430_expire(msgr_client_t  *client_ptr,
                                               lte_rrc_plmn_s  plmn)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  send_tau_accept = FALSE;
  boolean                  keep_the_test_running = TRUE ;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
        if(send_tau_accept == FALSE)
        {
          TF_YIELD();
          /* Sleep 15 sec to expire T3430 */
          TF_SLEEP(15 * 1000);
          send_tau_accept = TRUE;
        }
        else
        {
          nas_stub_send_tau_accept();
        }
        break ;
      case LTE_RRC_CONN_ABORT_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_ABORT_REQ");
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        TF_YIELD();
        /* Sleep 10 sec to expire T3411 */
        TF_SLEEP(10 * 1000);
        break;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        keep_the_test_running = FALSE ;
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SIM_UPDATE_REQ");
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        break ;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_normal_periodic_tau_t3430_expire */

void emm_test_normal_periodic_tau_release_ind(msgr_client_t  *client_ptr,
                                              lte_rrc_plmn_s  plmn)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  send_tau_accept = FALSE;
  boolean                  keep_the_test_running = TRUE ;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
        if(send_tau_accept == FALSE)
        {
          TF_YIELD();
          /* Send RRC Release Ind */
          nas_stub_send_only_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL, FALSE);
          nas_stub_send_rrc_service_ind(0xFFFFFFFF, plmn);
          send_tau_accept = TRUE;
          TF_YIELD();
          /* Sleep 10 sec to expire T3411 */
          TF_SLEEP(10 * 1000);
        }
        else
        {
          nas_stub_send_tau_accept();
        }
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        keep_the_test_running = FALSE ;
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SIM_UPDATE_REQ");
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        break ;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_normal_periodic_tau_release_ind */

void emm_test_normal_periodic_tau_req_rrc_conn_est_failure(msgr_client_t  *client_ptr)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  send_tau_accept = TRUE;
  boolean                  keep_the_test_running = TRUE ;
  int                      data_req_count = 0;
  int                      tau_attempt = 0;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        tau_attempt++;
        if(tau_attempt < 5)
        {
          nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_FAILURE);
          TF_YIELD();
          /* Sleep 10 sec to expire T3411 */
          TF_SLEEP(10 * 1000);
        }
        else
        {
          nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
          nas_stub_send_tau_accept();
        }
        break;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        TF_YIELD();
        keep_the_test_running = FALSE ;
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SIM_UPDATE_REQ");
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        break ;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_normal_periodic_tau_req_rrc_conn_est_failure */

void emm_test_normal_periodic_tau_req_tx_failure_data_suspended(msgr_client_t  *client_ptr)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  send_tau_accept = TRUE;
  boolean                  keep_the_test_running = TRUE ;
  int                      data_req_count = 0;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_FAILURE_TXN, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
		data_req_count++;
		switch(data_req_count){
			case 1:
				nas_stub_send_only_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL, FALSE);
				nas_stub_send_rrc_service_ind(0xFFFFFFFF, req_plmn);
				TF_YIELD();
				/* Sleep 10 sec to expire T3411 */
				TF_SLEEP(10 * 1000);
				break;
			case 2:
				nas_stub_send_tau_accept();
				break;
			case 3:
				keep_the_test_running = FALSE ;
				break;
		}
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SIM_UPDATE_REQ");
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        break ;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_normal_periodic_tau_req_tx_failure_data_suspended */

void emm_test_normal_periodic_tau_req_tx_failure(msgr_client_t  *client_ptr, lte_rrc_ul_data_cnf_status_e  status)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  send_tau_accept = TRUE;
  boolean                  keep_the_test_running = TRUE ;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(status, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
		if (status == LTE_RRC_UL_DATA_CNF_FAILURE_CONN_REL || status == LTE_RRC_UL_DATA_CNF_FAILURE_RLF ){
			/* start tau recovery */
			TF_YIELD();
			/* Sleep 10 sec to expire T3411 */
			TF_SLEEP(10 * 1000);
		}
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        if(send_tau_accept == TRUE)
        {
          nas_stub_send_tau_accept();
          send_tau_accept = FALSE;
        }
        else
        {
          keep_the_test_running = FALSE ;
        }
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SIM_UPDATE_REQ");
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        break ;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_normal_periodic_tau_req_tx_failure */

void emm_test_normal_periodic_tau_complete_tx_failure(msgr_client_t *client_ptr,
                                    lte_rrc_ul_data_cnf_status_e     cause)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  int                      data_req_count = 0;
  boolean                  keep_the_test_running = TRUE ;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_tau_accept();
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        data_req_count ++;
        if(cause == LTE_RRC_UL_DATA_CNF_FAILURE_HO)
        {
          switch(data_req_count)
          {
            case 1:
              nas_stub_send_rrc_ul_data_cnf(cause, stub_rx.rrc_ul_data_req.trans_id);
              break;
            case 2:
              nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                            stub_rx.rrc_ul_data_req.trans_id);
              nas_stub_send_tau_accept();
              break;
            case 3:
              nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                            stub_rx.rrc_ul_data_req.trans_id);
              keep_the_test_running = FALSE;
              break;
            default:
              break;
          }
        }
        else
        {
          nas_stub_send_rrc_ul_data_cnf(cause, stub_rx.rrc_ul_data_req.trans_id);
          keep_the_test_running = FALSE ;
        }        
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SIM_UPDATE_REQ");
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        break ;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
  }
  TF_YIELD(); 
} /* emm_test_normal_periodic_tau_complete_tx_failure */

void emm_test_normal_periodic_tau_complete_conn_est_failure(msgr_client_t *client_ptr,
															lte_rrc_conn_rel_reason_e  rel_reason)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  int                      data_req_count = 0;
  boolean				   conn_est_cnf_failure = TRUE;
  boolean                  keep_the_test_running = TRUE ;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
		nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
		nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
		nas_stub_send_tau_accept();
		nas_stub_send_rrc_connection_rel_ind(rel_reason); // release conn
		/* tau complete message delivery failed */
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        keep_the_test_running = FALSE ;     
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SIM_UPDATE_REQ");
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        break ;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
  }
  TF_YIELD(); 
} /* emm_test_normal_periodic_tau_complete_conn_est_failure */

void emm_test_normal_periodic_tau_cell_change(msgr_client_t  *client_ptr,
                                              boolean         send_release_ind)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  send_tau_accept = TRUE;
  boolean                  keep_the_test_running = TRUE ;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
        //nas_stub_send_cell_change_indication(req_plmn); // cell change
        nas_stub_send_rrc_service_ind(0xFFFFFFFF, req_plmn);
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        if(send_release_ind == TRUE)
        {    
          /* For release ind test with cell in TAI list */
          nas_stub_send_only_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL, FALSE);
          nas_stub_send_rrc_service_ind(0xFFFFFFFF, req_plmn);
          TF_YIELD();
          /* Sleep 10 sec to expire T3411 */
          TF_SLEEP(10 * 1000);
          keep_the_test_running = FALSE;
        }
        else
        {
          /* For cell change test. Test case stops at second UL Data Req (TAU complete)*/
          if(send_tau_accept == TRUE)
          {
            nas_stub_send_tau_accept();
            send_tau_accept = FALSE;
          }
          else
          {
            keep_the_test_running = FALSE;
          }
        }
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SIM_UPDATE_REQ");
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        break ;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_normal_periodic_tau_cell_change */

void emm_test_normal_periodic_tau_req_lower_layer_failure(msgr_client_t  *client_ptr, 
														  lte_rrc_conn_est_status_e  cause)
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
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(cause);
        TF_YIELD();
        /* Sleep 10 sec to expire T3411 */
        TF_SLEEP(10 * 1000);
        keep_the_test_running = FALSE ;
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        break ;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_normal_periodic_tau_req_lower_layer_failure */

void emm_test_normal_periodic_tau_t3402(msgr_client_t  *client_ptr)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  send_tau_accept = TRUE;
  lte_rrc_plmn_s           plmn;
  boolean                  keep_the_test_running = TRUE ;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_tau_reject(LTE_NAS_NON_EXISTANT_INFO);
        nas_stub_send_only_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL, FALSE);
        memset(&plmn, 0x00, sizeof(lte_rrc_plmn_s));
        nas_stub_send_rrc_service_ind(0xFFFFFFFF, plmn);
        TF_YIELD();
        /* Sleep 12 minutes to expire T3402*/
        TF_SLEEP(12 * 60 * 1000);
        keep_the_test_running = FALSE;
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        break ;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_normal_periodic_tau_t3402 */

void emm_test_normal_periodic_tau_access_barring(msgr_client_t       *client_ptr)
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
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED);
        TF_YIELD();
        TF_SLEEP(1*1000);
        keep_the_test_running = FALSE;
        break ;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        break;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_normal_periodic_tau_access_barring */

void emm_test_tau_detach_collision(msgr_client_t       *client_ptr)
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
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
		TF_YIELD();
		nas_stub_trigger_mo_detach(MO_ESM_DETACH_CMD); //initiate detach
		TF_YIELD();
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
		nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        nas_stub_send_detach_accept();
        TF_YIELD();
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
		if(EMM_GET_STATE()==EMM_DEREGISTERED && EMM_GET_SUBSTATE()==EMM_DEREGISTERED_PLMN_SEARCH){
			keep_the_test_running = FALSE;
		}
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SIM_UPDATE_REQ");
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        break ;
      case NAS_EMM_EPS_BEARER_STATUS_CMD:
        TF_MSG("TEST> ESM sent NAS_EMM_EPS_BEARER_STATUS_CMD");
        break ;
      case NAS_EMM_DETACH_CMD:
        TF_MSG("TEST> ESM sent NAS_EMM_DETACH_CMD");
        break;
      case NAS_ESM_DETACH_IND:
        TF_MSG("TEST> ESM sent NAS_ESM_DETACH_IND");
        break;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_tau_detach_collision */

void emm_test_tau_limited_ra(msgr_client_t       *client_ptr)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  lte_rrc_plmn_s           plmn = {0x6, 0x2, 0x73};

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
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_tau_accept();
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        break ;
      case LTE_RRC_NW_SEL_MODE_RESET_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_NW_SEL_MODE_RESET_REQ ");
        ASSERT(stub_rx.rrc_nw_sel_mode_reset_req.network_select_mode == 
                                         SYS_NETWORK_SELECTION_MODE_AUTOMATIC);
        keep_the_test_running = FALSE ;
        break;
      default:
        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_tau_limited_ra */

void emm_test_combined_tau_ps_only
(
  msgr_client_t          *client_ptr,
  lte_nas_emm_cause_type  emm_cause
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  send_tau_accept = TRUE;
  boolean                  keep_the_test_running = TRUE ;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        emm_test_emm_cause = emm_cause;
        nas_stub_send_tau_accept();
        TF_YIELD();
        TF_YIELD();
        /* To stop the T3440 timer TAU REQ was sent in IDLE */
        nas_stub_send_rrc_active_eps_ind();
        TF_YIELD();
        TF_YIELD();
        if((emm_cause == LTE_NAS_IMSI_UNKNOWN_IN_HSS) ||
           (emm_cause == LTE_NAS_CS_DOMAIN_NOT_AVAILABLE))
        {
          ASSERT(EMM_GET_STATE() == EMM_REGISTERED);
          ASSERT(EMM_GET_SUBSTATE() == EMM_REGISTERED_NORMAL_SERVICE);
          keep_the_test_running = FALSE ;
        }
        else
        {
          ASSERT(EMM_GET_STATE() == EMM_REGISTERED);
          ASSERT(EMM_GET_SUBSTATE() == EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM);
          /* Expires T3411 */
          TF_SLEEP(10 * 1000);
        }
        break;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        if(send_tau_accept == TRUE)
        {
          update_result = TA_LA_UPDATE;
          nas_stub_send_tau_accept();
          send_tau_accept = FALSE;
        }
        else
        {
          ASSERT(EMM_GET_STATE() == EMM_REGISTERED);
          ASSERT(EMM_GET_SUBSTATE() == EMM_REGISTERED_NORMAL_SERVICE);
          keep_the_test_running = FALSE ;
        }
        TF_YIELD();
        break ;
      default:
        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_combined_tau_ps_only */

void emm_test_normal_tau_guti_realloc_collision(msgr_client_t       *client_ptr)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();


  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        switch(emm_ctrl_data_ptr->tau_cause)
        {
          case TAU_CAUSE_TA_CHANGE:
            TF_ASSERT(stub_rx.rrc_conn_est_req.include_s_tmsi == FALSE); 
            TF_ASSERT(stub_rx.rrc_conn_est_req.registered_mme_is_present == TRUE); 
            break;
          case TAU_CAUSE_LOAD_BALANCING:
            TF_ASSERT(stub_rx.rrc_conn_est_req.include_s_tmsi == FALSE); 
            TF_ASSERT(stub_rx.rrc_conn_est_req.registered_mme_is_present == FALSE); 
            break;
          default:
            TF_ASSERT(stub_rx.rrc_conn_est_req.include_s_tmsi == TRUE); 
            TF_ASSERT(stub_rx.rrc_conn_est_req.registered_mme_is_present == FALSE); 
            break;
        }
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
	  nas_stub_send_guti_realloc_cmd(); //send guti realloc when TA update is in progress
        nas_stub_send_tau_accept();
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        TF_YIELD();
        keep_the_test_running = FALSE ;
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SIM_UPDATE_REQ");
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        break ;
      case NAS_EMM_EPS_BEARER_STATUS_CMD:
        TF_MSG("TEST> ESM sent NAS_EMM_EPS_BEARER_STATUS_CMD");
        break ;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_normal_tau_guti_realloc_collision */


/*
	in progress tests that are yet to be completed and working
*/

void emm_test_tau_collision(msgr_client_t       *client_ptr)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();


  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        /*switch(emm_ctrl_data_ptr->tau_cause)
        {
          case TAU_CAUSE_TA_CHANGE:
            TF_ASSERT(stub_rx.rrc_conn_est_req.include_s_tmsi == FALSE); 
            TF_ASSERT(stub_rx.rrc_conn_est_req.registered_mme_is_present == TRUE); 
            break;
          case TAU_CAUSE_LOAD_BALANCING:
            TF_ASSERT(stub_rx.rrc_conn_est_req.include_s_tmsi == FALSE); 
            TF_ASSERT(stub_rx.rrc_conn_est_req.registered_mme_is_present == FALSE); 
            break;
          default:
            TF_ASSERT(stub_rx.rrc_conn_est_req.include_s_tmsi == TRUE); 
            TF_ASSERT(stub_rx.rrc_conn_est_req.registered_mme_is_present == FALSE); 
            break;
        }*/
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_tau_accept();
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        TF_YIELD();
        keep_the_test_running = FALSE ;
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SIM_UPDATE_REQ");
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        break ;
      case NAS_EMM_EPS_BEARER_STATUS_CMD:
        TF_MSG("TEST> ESM sent NAS_EMM_EPS_BEARER_STATUS_CMD");
        break ;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_normal_periodic_tau */

void emm_test_normal_periodic_tau_t3402_oos(msgr_client_t  *client_ptr)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  lte_rrc_plmn_s           plmn;
  boolean                  send_tau_accept = TRUE;
  boolean                  keep_the_test_running = TRUE ;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_tau_reject(LTE_NAS_NON_EXISTANT_INFO);
        nas_stub_send_only_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL, FALSE);
        memset(&plmn, 0x00, sizeof(lte_rrc_plmn_s));
        nas_stub_send_rrc_service_ind(0xFFFFFFFF, plmn);
        TF_YIELD();
        TF_SLEEP(3 * 60 * 1000);
        /* Move EMM to EMM_REGISTERED_NO_CELL_AVAILABLE state */
        nas_stub_send_no_svc_rrc_service_ind(0xFFFFFFFF, 
                                             plmn,
                                             SYS_NO_SVC_CAUSE_NORMAL);
        TF_YIELD();
        TF_YIELD();
        TF_YIELD();
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                  SYS_RAT_LTE_RADIO_ACCESS,
						                      SYS_NETWORK_SELECTION_MODE_AUTOMATIC); 
        TF_YIELD();
        TF_YIELD();
        nas_stub_send_rrc_service_ind(0x1030002, plmn);
        TF_YIELD();
        TF_YIELD();
        TF_SLEEP(9 * 60 * 1000);
        keep_the_test_running = FALSE;
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        break ;
      default:
        TF_MSG("TEST> EMM sent unexpected command ");
        TF_MSG( "ID = 0x%x RECEIVE... (%s)",
                stub_rx.msg_hdr.id,
                msgr_get_umid_name(stub_rx.msg_hdr.id) );

        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_normal_periodic_tau_t3402_oos */

void emm_test_normal_periodic_tau_access_barring_paging
(
  msgr_client_t  *client_ptr
)
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
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED);
        TF_YIELD();
        keep_the_test_running = FALSE;
        break ;
      default:
        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_normal_periodic_tau_access_barring_paging */

void emm_test_tau_connected(msgr_client_t  *client_ptr)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();


  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        nas_stub_send_tau_accept();
        TF_YIELD();
        keep_the_test_running = FALSE ;
        break ;
      default:
        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_tau_connected */

void emm_test_rrc_srv_req_srv_ind_race_condition
(
  msgr_client_t  *client_ptr
)
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
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_tau_reject(LTE_NAS_SEMANTICALLY_INCORRECT_MSG);
        TF_YIELD();
        TF_YIELD();
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY, 
                                  SYS_RAT_LTE_RADIO_ACCESS,
							                    SYS_NETWORK_SELECTION_MODE_AUTOMATIC);  
        TF_YIELD();
        break;
      case LTE_RRC_CONN_ABORT_REQ:
        TF_YIELD();
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        break ;
      case LTE_RRC_SERVICE_REQ:
        nas_stub_send_no_svc_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                             stub_rx.rrc_service_req.req_plmn,
                                             SYS_NO_SVC_CAUSE_NORMAL); 
        TF_YIELD();
        TF_YIELD();
        keep_the_test_running = FALSE;
        break ;
      default:
        break ; 
    }
  }

} /* emm_test_rrc_srv_req_srv_ind_race_condition*/

void emm_test_tau_manual_req_t3402_running
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  send_tau_accept = FALSE;
  boolean                  keep_the_test_running = TRUE ;
  emm_ctrl_data_type      *emm_ctrl_ptr = NULL;

  /* Enable backoff PLMN list feature */
  emm_ctrl_ptr = emm_db_get_ctrl_data();
  //emm_ctrl_ptr->temp_fplmn_backoff_time = 0xFFFFFFFF;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        if(send_tau_accept == FALSE)
        {
          nas_stub_send_tau_reject(LTE_NAS_SEMANTICALLY_INCORRECT_MSG);
          TF_YIELD();
          nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
          send_tau_accept = TRUE;
        }
        else
        {
          nas_stub_send_tau_accept();
        }
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                  SYS_RAT_LTE_RADIO_ACCESS,
							                    SYS_NETWORK_SELECTION_MODE_MANUAL); 
        break;
      case LTE_RRC_SERVICE_REQ:
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      stub_rx.rrc_service_req.req_plmn); 
        break;
      case LTE_RRC_UL_DATA_REQ: 
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        keep_the_test_running = FALSE;
        break;
      default:
        break;
    }
  }
  TF_YIELD(); 

} /* emm_test_tau_manual_req_t3402_running */

void emm_test_tau_complete_failure_conn_rel
(
  msgr_client_t  *client_ptr
)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  byte                     rel_conn = TRUE;
  boolean                  keep_the_test_running = TRUE ;

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
        nas_stub_send_tau_accept();
        break ;
      case LTE_RRC_UL_DATA_REQ:
        if(rel_conn == TRUE)
        {
          nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL); 
          rel_conn = FALSE;
        }
        else
        {
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                        stub_rx.rrc_ul_data_req.trans_id);
          keep_the_test_running = FALSE;
        }     
        break ;
      default:
        break ;
    }
    TF_YIELD();
  }
} /* emm_test_tau_complete_failure_conn_rel */

void emm_test_tau_gw_ps_suspended(msgr_client_t  *client_ptr)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;

  /* Simulate the state of UE leaving LTE and GW PS is suspended */
  nas_stub_send_no_svc_rrc_service_ind(0xFFFFFFFF, 
                                       req_plmn,
                                       SYS_NO_SVC_CAUSE_NORMAL);
  //gmm_is_suspended = TRUE; 

  TF_YIELD();

  nas_stub_send_new_tac_rrc_service_ind(0xFFFFFFFF, req_plmn); 
  
  while(keep_the_test_running)
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
        TF_YIELD();
        nas_stub_send_tau_accept();
        break ;
      case LTE_RRC_UL_DATA_REQ:
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        TF_YIELD();
        TF_YIELD();
        keep_the_test_running = FALSE;
        break;
      default:
        break;
    }
  }
  TF_YIELD(); 

  //TF_ASSERT(gmm_is_suspended == FALSE);

} /* emm_test_tau_gw_ps_suspended */

void emm_test_tau_manual_req_conn_rel
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  release_conn = TRUE;
  boolean                  send_tau_accept = FALSE;
  boolean                  keep_the_test_running = TRUE ;
  emm_ctrl_data_type      *emm_ctrl_ptr = NULL;

  /* Enable backoff PLMN list feature */
  emm_ctrl_ptr = emm_db_get_ctrl_data();
  //emm_ctrl_ptr->temp_fplmn_backoff_time = 0xFFFFFFFF;

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
        if(send_tau_accept == FALSE)
        {
          nas_stub_send_tau_reject(LTE_NAS_SEMANTICALLY_INCORRECT_MSG);
          TF_YIELD();
          nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
          send_tau_accept = TRUE;
        }
        else
        {
          if(release_conn == TRUE)
          {
            nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
            release_conn = FALSE;
            TF_YIELD();
            TF_YIELD();
            TF_SLEEP(10 * 1000);
          }
          else
          {
            nas_stub_send_tau_accept();
          }
        }
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                  SYS_RAT_LTE_RADIO_ACCESS,
                                  SYS_NETWORK_SELECTION_MODE_MANUAL); 
        break;
      case LTE_RRC_SERVICE_REQ:
        nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                      stub_rx.rrc_service_req.req_plmn); 
        break;
      case LTE_RRC_UL_DATA_REQ: 
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        keep_the_test_running = FALSE;
        break;
      default:
        break;
    }
  }
  TF_YIELD(); 

} /* emm_test_tau_manual_req_conn_rel */

void emm_test_ta_chg_during_tau
(
  msgr_client_t  *client_ptr
)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  lte_rrc_plmn_s           plmn;
  boolean                  keep_the_test_running = TRUE ;

  /* Send RRC service indication with updated TAI to trigger TAU */
  memset(&plmn, 0x00, sizeof(lte_rrc_plmn_s));
  nas_stub_send_rrc_service_ind(0xFFFFFFFF, plmn);

  while(keep_the_test_running)
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
        TF_YIELD();
        nas_stub_send_rrc_service_ind(0xFFFFFFFF, req_plmn);
        break;
      case LTE_RRC_UL_DATA_REQ:
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        TF_YIELD();
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL); 
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        if(mm_timer_status[TIMER_T3411] == TIMER_ACTIVE)
        {
          nas_stub_send_emm_service_req();
        }
        break;
      case NAS_ESM_FAILURE_IND:
        keep_the_test_running = FALSE;
        break;
      default:
        break ;
    }
    TF_YIELD();
  }
} /* emm_ta_chg_during_tau */

void emm_test_tau_sr_collision
(
  msgr_client_t  *client_ptr
)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  uint32                   trans_id = 0;
  boolean                  conn_est = FALSE;
  boolean                  conn_released = FALSE;
  boolean                  keep_the_test_running = TRUE ;

  
  /* Expire timer 3412 to trigger TAU */
  TF_SLEEP(T3412_VALUE);

  while(keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        if(conn_est == FALSE)
        {
          nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_FAILURE);
          TF_YIELD();
          conn_est = TRUE;
          /* Sleep 9s and start service request */
          TF_SLEEP(9 * 1000);
          nas_stub_send_emm_service_req();
          TF_YIELD();
          /* Sleep 1 more sec to expire T3411 */
          TF_SLEEP(1 * 1000);
        }
        else
        {
          if(conn_released == TRUE)
          {
            nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
            nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                          stub_rx.rrc_conn_est_req.transaction_id);
            nas_stub_send_tau_accept();
          }
          else
          {
            trans_id = stub_rx.rrc_conn_est_req.transaction_id;
          }
        }
        break;
      case LTE_RRC_CONN_ABORT_REQ:
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        TF_YIELD();
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_FAILURE_TXN, 
                                      trans_id);
        TF_YIELD();
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_ABORTED); 
        conn_released = TRUE;
        TF_YIELD();
        break;
      case LTE_RRC_UL_DATA_REQ:
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        keep_the_test_running = FALSE;
        TF_YIELD();
      default:
        break ;
    }
    TF_YIELD();
  }

  ASSERT(EMM_GET_STATE() == EMM_REGISTERED);

} /* emm_test_tau_sr_collision */

void emm_test_tau_t3411_expiry_on_ftai
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  lte_rrc_plmn_s           fplmn;
  byte                     conn_rel_counter = 0;
  boolean                  keep_the_test_running = TRUE ;
  emm_ctrl_data_type      *emm_ctrl_ptr = NULL;

  /* Enable backoff PLMN list feature */
  emm_ctrl_ptr = emm_db_get_ctrl_data();
  emm_ctrl_ptr->temp_fplmn_backoff_time = 0xFFFFFFFF;

  fplmn.mcc[0] = 3;
  fplmn.mcc[1] = 1;
  fplmn.mcc[2] = 1;
  fplmn.num_mnc_digits = 3;
  fplmn.mnc[0] = 4;
  fplmn.mnc[1] = 8;
  fplmn.mnc[2] = 0;

  emm_ctrl_ptr->forbidden_for_service_list_ptr->tai_lst_length = 1;
  emm_ctrl_ptr->forbidden_for_service_list_ptr->diff_plmn[0].plmn.identity[0] = 0x13;
  emm_ctrl_ptr->forbidden_for_service_list_ptr->diff_plmn[0].plmn.identity[1] = 0x1;
  emm_ctrl_ptr->forbidden_for_service_list_ptr->diff_plmn[0].plmn.identity[2] = 0x84;
  emm_ctrl_ptr->forbidden_for_service_list_ptr->diff_plmn[0].tac = 1;

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
        TF_YIELD();
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        TF_YIELD();
        nas_stub_send_no_svc_rrc_service_ind(0xFFFFFFFF, 
                                             req_plmn,
                                             SYS_NO_SVC_CAUSE_NORMAL);
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        conn_rel_counter ++;
        if(conn_rel_counter == 2)
        {
          nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                    SYS_RAT_LTE_RADIO_ACCESS,
                                    SYS_NETWORK_SELECTION_MODE_AUTOMATIC); 
        }
        break;
      case LTE_RRC_SERVICE_REQ:
        nas_stub_send_rrc_service_ind_with_param(stub_rx.rrc_service_req.trans_id,
                                                 fplmn,1,1);
        TF_YIELD();
        TF_SLEEP(10 *1000);
        keep_the_test_running = FALSE;
        break;
      default:
        break;
    }
  }
  TF_YIELD(); 

  ASSERT(EMM_GET_STATE() == EMM_REGISTERED);
  ASSERT(EMM_GET_SUBSTATE() == EMM_REGISTERED_PLMN_SEARCH);

} /* emm_test_tau_t3402_expiry_on_ftai */

void emm_test_tau_t3411_temp_oos
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  lte_rrc_plmn_s           fplmn;
  byte                     conn_rel_counter = 0;
  boolean                  camp_on_ftai = TRUE;
  boolean                  keep_the_test_running = TRUE ;
  emm_ctrl_data_type      *emm_ctrl_ptr = NULL;

  /* Enable backoff PLMN list feature */
  emm_ctrl_ptr = emm_db_get_ctrl_data();
  emm_ctrl_ptr->temp_fplmn_backoff_time = 0xFFFFFFFF;

  fplmn.mcc[0] = 3;
  fplmn.mcc[1] = 1;
  fplmn.mcc[2] = 1;
  fplmn.num_mnc_digits = 3;
  fplmn.mnc[0] = 4;
  fplmn.mnc[1] = 8;
  fplmn.mnc[2] = 0;

  emm_ctrl_ptr->forbidden_for_service_list_ptr->tai_lst_length = 1;
  emm_ctrl_ptr->forbidden_for_service_list_ptr->diff_plmn[0].plmn.identity[0] = 0x13;
  emm_ctrl_ptr->forbidden_for_service_list_ptr->diff_plmn[0].plmn.identity[1] = 0x1;
  emm_ctrl_ptr->forbidden_for_service_list_ptr->diff_plmn[0].plmn.identity[2] = 0x84;
  emm_ctrl_ptr->forbidden_for_service_list_ptr->diff_plmn[0].tac = 1;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        if(camp_on_ftai == TRUE)
        {
          nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                        stub_rx.rrc_conn_est_req.transaction_id);
          TF_YIELD();
          nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
          TF_YIELD();
          nas_stub_send_no_svc_rrc_service_ind(0xFFFFFFFF, 
                                               req_plmn,
                                               SYS_NO_SVC_CAUSE_NORMAL);
        }
        else
        {
          keep_the_test_running = FALSE;
        }
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        conn_rel_counter ++;
        if(conn_rel_counter == 2)
        {
          nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                    SYS_RAT_LTE_RADIO_ACCESS,
                                    SYS_NETWORK_SELECTION_MODE_AUTOMATIC); 
        }
        break;
      case LTE_RRC_SERVICE_REQ:
        if(camp_on_ftai == TRUE)
        {
          nas_stub_send_rrc_service_ind_with_param(stub_rx.rrc_service_req.trans_id,
                                                   fplmn,1,1);
          TF_YIELD();
          nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                    SYS_RAT_LTE_RADIO_ACCESS,
                                    SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION); 
          camp_on_ftai = FALSE;
        }
        else
        {
          nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, 
                                        req_plmn);
          TF_YIELD();
          TF_SLEEP(10 *1000);
        }
        break;
      default:
        break;
    }
  }
  TF_YIELD(); 

  ASSERT(EMM_GET_STATE() == EMM_TRACKING_AREA_UPDATING_INITIATED);

} /* emm_test_tau_t3411_temp_oos */



void emm_test_tau_req_without_active_flag
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  lte_rrc_plmn_s           plmn;
  boolean                  keep_the_test_running = TRUE ;

  /* Send RRC service indication with updated TAI to trigger TAU */
  memset(&plmn, 0x00, sizeof(lte_rrc_plmn_s));
  nas_stub_send_rrc_service_ind(0xFFFFFFFF, plmn);

  while(keep_the_test_running)
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
        TF_YIELD();
        nas_stub_send_tau_accept();
        TF_YIELD();
        TF_YIELD();
        nas_stub_send_rrc_active_eps_ind();
        TF_YIELD();
        keep_the_test_running = FALSE;
        break;
      default:
        break ;
    }
    TF_YIELD();
  }
} /* emm_test_tau_req_without_active_flag */


void emm_test_tau_req_without_active_flag_t3440_expiry
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  lte_rrc_plmn_s           plmn;
  boolean                  keep_the_test_running = TRUE ;

  /* Send RRC service indication with updated TAI to trigger TAU */
  memset(&plmn, 0x00, sizeof(lte_rrc_plmn_s));
  nas_stub_send_rrc_service_ind(0xFFFFFFFF, plmn);

  while(keep_the_test_running)
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
        TF_YIELD();
        nas_stub_send_tau_accept();
        TF_YIELD();
        TF_YIELD();       
        TF_SLEEP(10 * 1000); /* T3440 expiry*/
        break;
      case LTE_RRC_CONN_ABORT_REQ:
        TF_MSG("TEST > EMM sent LTE_RRC_CONN_ABORT_REQ");
        keep_the_test_running = FALSE;
        break;
      default:
        break ;
    }
    TF_YIELD();
  }
} /* emm_test_tau_req_without_active_flag */


void emm_test_combined_tau_ps_only_optional_accept
(
  msgr_client_t          *client_ptr,
  lte_nas_emm_cause_type  emm_cause,
  boolean                 send_tau_accept
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  boolean                  tau_accept_sent = FALSE;

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), 
                            sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        emm_test_emm_cause = emm_cause;
        nas_stub_send_tau_accept();
        TF_YIELD();
        TF_YIELD();
        /* To stop the T3440 timer TAU REQ was sent in IDLE */
        nas_stub_send_rrc_active_eps_ind();
        TF_YIELD();
        TF_YIELD();
        if((emm_cause == LTE_NAS_IMSI_UNKNOWN_IN_HSS) ||
           (emm_cause == LTE_NAS_CS_DOMAIN_NOT_AVAILABLE))
        {
          ASSERT(EMM_GET_STATE() == EMM_REGISTERED);
          ASSERT(EMM_GET_SUBSTATE() == EMM_REGISTERED_NORMAL_SERVICE);
          keep_the_test_running = FALSE ;
        }
        else if(send_tau_accept != TRUE)
        {
          keep_the_test_running = FALSE ;
        }
        else
        {
          ASSERT(EMM_GET_STATE() == EMM_REGISTERED);
          ASSERT(EMM_GET_SUBSTATE() == EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM);
          /* Expires T3411 */
          TF_SLEEP(10 * 1000);
        }
        break;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_ul_data_req.trans_id);
        if(send_tau_accept == TRUE)
        {
          update_result = TA_LA_UPDATE;
          nas_stub_send_tau_accept();
          send_tau_accept = FALSE;
          tau_accept_sent = TRUE;
        }
        else if(tau_accept_sent == TRUE)
        {
          ASSERT(EMM_GET_STATE() == EMM_REGISTERED);
          ASSERT(EMM_GET_SUBSTATE() == EMM_REGISTERED_NORMAL_SERVICE);
          keep_the_test_running = FALSE ;
        }
        else
        {
          emm_test_emm_cause = emm_cause;
          nas_stub_send_tau_accept();
          TF_YIELD();
          TF_YIELD();
          /* To stop the T3440 timer TAU REQ was sent in IDLE */
          nas_stub_send_rrc_active_eps_ind();
          keep_the_test_running = FALSE ;
        }
        TF_YIELD();
        break ;
      default:
        break ;
    }
  }
  TF_YIELD(); 

} /* emm_test_combined_tau_ps_only */

void emm_test_stop_mode_t3440_active
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;

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
        nas_stub_send_tau_reject(LTE_NAS_PLMN_NOT_ALLOWED);
        TF_YIELD();
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                    SYS_RAT_LTE_RADIO_ACCESS,
                                    SYS_NETWORK_SELECTION_MODE_AUTOMATIC); 
        TF_YIELD();
        nas_stub_send_stop_mode_req(SYS_STOP_MODE_REASON_OFFLINE);
        break;
      case LTE_RRC_DEACTIVATE_REQ:
        nas_stub_send_rrc_deactivate_cnf(TRUE);
        keep_the_test_running = FALSE;
        break;
      case LTE_RRC_CONN_ABORT_REQ:
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        break;
      default:
        break ;
    }
  }
  TF_YIELD(); 

  TF_ASSERT(emm_ctrl_data_ptr->reg_req_pended_indication == FALSE);
  TF_ASSERT(emm_reg_container == NULL);
  TF_ASSERT(reg_service_report == emm_send_reg_ind);

} /* emm_test_stop_mode_t3440_active */

void emm_test_tau_rej_15_conn_rel
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;

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
        nas_stub_send_tau_reject(LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA);
        TF_YIELD();
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,
                                    SYS_RAT_LTE_RADIO_ACCESS,
                                    SYS_NETWORK_SELECTION_MODE_AUTOMATIC); 
        TF_YIELD();
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        break;
      case LTE_RRC_SERVICE_REQ:
        keep_the_test_running = FALSE;
        break;
      default:
        break ;
    }
  }
  TF_YIELD(); 

  TF_ASSERT(emm_ctrl_data_ptr->reg_req_pended_indication == TRUE);
  TF_ASSERT(emm_reg_container == NULL);
  TF_ASSERT(reg_service_report == emm_send_reg_cnf);
  TF_ASSERT(mm_timer_status[TIMER_T3440] == TIMER_STOPPED);

} /* emm_test_tau_rej_15_conn_rel */

void emm_test_gcf_9_2_3_1_25
(
  msgr_client_t  *client_ptr
)
{
  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  send_tau_rej = TRUE;
  boolean                  keep_the_test_running = TRUE ;

  TF_SLEEP(emm_ctrl_data_ptr->t3412_value);

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
        if(send_tau_rej == TRUE)
        {
          TF_ASSERT(emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_PERIODIC);
          TF_ASSERT(emm_ctrl_data_ptr->emm_ongoing_eps_ta_update == PERIODIC_UPDATING);          
          nas_stub_send_tau_reject(LTE_NAS_SEMANTICALLY_INCORRECT_MSG);
          TF_YIELD();
          nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
          TF_YIELD();
          TF_SLEEP(emm_ctrl_data_ptr->t3402_value); 
          send_tau_rej = FALSE;
        }
        else
        {
          keep_the_test_running = FALSE;
        }
        break;
      default:
        break ;
    }
  }
  TF_YIELD(); 

  TF_ASSERT(emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_OTHERS);
  TF_ASSERT(emm_ctrl_data_ptr->emm_ongoing_eps_ta_update == TA_UPDATING);

} /* emm_test_gcf_9_2_3_1_25 */

#endif /*FEATURE_LTE*/
