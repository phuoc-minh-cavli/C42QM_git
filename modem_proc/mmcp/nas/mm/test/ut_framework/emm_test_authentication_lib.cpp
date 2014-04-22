
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
$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/test/ut_framework/emm_test_authentication_lib.cpp#1 $
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
  #include "bit.h"
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
  #include "lte_nas_common_v.h"
  #include "qurt_signal.h" 
  
}
extern void FuzzOTAFile();
extern void esm_ut_lib_send_act_def_ctxt_accept(byte bid, byte con_id);
extern void stop_and_attach(boolean keep_running, msgr_client_t *client_ptr, qurt_signal_t* signal);
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

/* Global var used for EMM page ind with IMSI test case */
extern lte_rrc_plmn_s            req_plmn;

#include <emm_test_lib.h>
#include <emm_test_attach_lib.h>
#include "mmtask_v.h"

void emm_test_send_attach_req(msgr_client_t  *client_ptr)
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
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_authentication_req();
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
  }
  TF_YIELD(); 

} 

void emm_test_auth_with_identity_req(msgr_client_t  *client_ptr)
{
 
  boolean                  keep_the_test_running = TRUE ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  stub_rx_type     stub_rx ;
  stub_rx_type*    stub_rx_ptr = NULL;
  uint8  num_attach = 0;
  uint32 dsm_packet_length;
  dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
  msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */
  byte msg_payload[EMM_MAX_MESSAGE_LENGTH];

  stub_rx_ptr = &stub_rx;

// nas_stub_send_mmr_act_req();
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
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_authentication_req();
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        break;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);

        memset( msg_payload, 0, EMM_MAX_MESSAGE_LENGTH);
  
        /* See if NAS has sent the DT message as a DSM item by checking for attachments */
        num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req);
      
        if ( num_attach == 1 )
        {
          att_ptr = msgr_get_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req, 0);
          msgr_get_dsm_attach(att_ptr, &dsm_ptr);
          TF_ASSERT(dsm_ptr != NULL);
      
          dsm_packet_length               = dsm_length_packet(dsm_ptr);
          TF_MSG("TEST>DSM UL Data found! item len %d", dsm_packet_length,0,0);
          
          TF_ASSERT(dsm_packet_length == dsm_pullup(&dsm_ptr,msg_payload, dsm_packet_length));
         
          /* Received dsm should have been freed */
          TF_ASSERT(dsm_ptr == NULL);
        }
        else
        {
          ERR_FATAL("TEST> DSM UL not found. EMM using old interface",0,0,0);
        }

        if( msg_payload[1] == EMM_AUTHENTICATION_FAILURE)
        {
              nas_stub_send_identity_req();
        } else if (msg_payload[1] == EMM_IDENTITY_RESPONSE)
        {
              nas_stub_send_authentication_rej();
        }

        break ;

      case LTE_RRC_CONN_ABORT_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_ABORT_REQ ");
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        keep_the_test_running = FALSE;
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

} 

void emm_test_authentication_req(msgr_client_t  *client_ptr)
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
    case LTE_RRC_UL_DATA_REQ: 
      TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
      nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
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
} 


void emm_test_authentication_rej(msgr_client_t  *client_ptr)
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
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        nas_stub_send_authentication_req();
        nas_stub_send_authentication_rej();
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        keep_the_test_running = FALSE;
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
} 


void emm_test_authentication_timer_expiry(msgr_client_t  *client_ptr)
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
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        TF_YIELD();
        TF_SLEEP(15*1000);
        TF_YIELD();
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        keep_the_test_running = FALSE;
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
} 


void emm_test_authentication_t3420_expiry(msgr_client_t  *client_ptr)
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
        nas_test_conn_est_req_dsm_pullup(&stub_rx);
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
       nas_stub_send_authentication_req();
        break ;

      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("NAS_TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_test_ul_dsm_pullup(&stub_rx);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        TF_YIELD();
        mm_timer_expiry(TIMER_T3420<< 16);
        TF_YIELD();
        TF_YIELD();
        break;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        break;

      case LTE_RRC_CONN_ABORT_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_ABORT_REQ");
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);

        keep_the_test_running = FALSE ;

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

} 

void emm_test_authentication_t3418_expiry(msgr_client_t  *client_ptr)
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
        nas_test_conn_est_req_dsm_pullup(&stub_rx);
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_authentication_req();
        TF_YIELD();
        break ;

      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("NAS_TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_test_ul_dsm_pullup(&stub_rx);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);

        TF_YIELD();
        mm_timer_expiry(TIMER_T3418<< 16);
        TF_YIELD();
        TF_YIELD();
        break;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        break;

      case LTE_RRC_CONN_ABORT_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_ABORT_REQ");
        nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
        keep_the_test_running = FALSE ;
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

} 
void emm_test_auth_and_smc(msgr_client_t  *client_ptr)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  uint8  num_attach = 0;
  uint32 dsm_packet_length;
  dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
  msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */
  byte msg_payload[EMM_MAX_MESSAGE_LENGTH];

  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);    

  TF_STUB(get_nas_sec_info) = my_get_nas_sec_info;
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
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_authentication_req();
        break ;

      case LTE_RRC_UL_DATA_REQ:
        memset( msg_payload, 0, EMM_MAX_MESSAGE_LENGTH);
  
        /* See if NAS has sent the DT message as a DSM item by checking for attachments */
        num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req);
      
        if ( num_attach == 1 )
        {
          att_ptr = msgr_get_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req, 0);
          msgr_get_dsm_attach(att_ptr, &dsm_ptr);
          TF_ASSERT(dsm_ptr != NULL);
      
          dsm_packet_length               = dsm_length_packet(dsm_ptr);
          TF_MSG("TEST>DSM UL Data found! item len %d", dsm_packet_length,0,0);
          
          TF_ASSERT(dsm_packet_length == dsm_pullup(&dsm_ptr,msg_payload, dsm_packet_length));
         
          /* Received dsm should have been freed */
          TF_ASSERT(dsm_ptr == NULL);
        }
        else
        {
          ERR_FATAL("TEST> DSM UL not found. EMM using old interface",0,0,0);
        }
         
      if((msg_payload[1] ==SECURITY_MODE_COMPLETE) ||
         (msg_payload[1] ==SECURITY_MODE_REJECT ))
        keep_the_test_running = FALSE;
      else
      {
        nas_stub_send_security_mode_cmd();
        TF_YIELD();
        TF_YIELD();
        TF_YIELD();
        TF_YIELD();
      }
      break;
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
  }
  TF_YIELD(); 

} 
void nas_int_emm_test_dload_transport_fuzz(msgr_client_t  *client_ptr)
{   
    stub_rx_type             stub_rx ;
    errno_enum_type          e_retval;
    dword                    bytes_rcvd;
    boolean                  keep_the_test_running = TRUE ;
    boolean                  reset_in_progress = FALSE;
    uint8  num_attach = 0;
    uint32 dsm_packet_length;
    dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
    msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */
    byte msg_payload[EMM_MAX_MESSAGE_LENGTH];
    qurt_signal_init(&signal_nas);

#ifdef FEATURE_NAS_AFL 
    while(__AFL_LOOP(1000))
#endif
    {        
        stop_and_attach(keep_the_test_running, client_ptr, &signal_nas);
        qurt_signal_wait(&signal_nas, 2, QURT_SIGNAL_ATTR_WAIT_ANY);
        qurt_signal_clear(&signal_nas, 2);

        processing_done = FALSE;
        ota_message_discarded_nft = FALSE;
        nas_msg_decoding_status_nft = FALSE;
        tlb_message_discarded_nft = FALSE;
        qurt_signal_clear(&signal_nas, 2);
        FuzzOTAFile();
        qurt_signal_wait(&signal_nas, 2, QURT_SIGNAL_ATTR_WAIT_ANY);                  
        nas_msg_decoding_status_nft = FALSE; /* Reset OTA message decoding status */
        keep_the_test_running = TRUE ;
        if (( no_of_ota_messages_nft == 0 ) || (ota_message_discarded_nft == TRUE)|| (tlb_message_discarded_nft == TRUE))
        {
            keep_the_test_running = FALSE;
        }

        while(keep_the_test_running)
        {
            e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
            TF_ASSERT( e_retval == E_SUCCESS );

            switch (stub_rx.msg_hdr.id)
            {
            case LTE_RRC_UL_DATA_REQ: 
                nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
                memset( msg_payload, 0, EMM_MAX_MESSAGE_LENGTH);
                num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req);
                att_ptr = msgr_get_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req, 0);
                msgr_get_dsm_attach(att_ptr, &dsm_ptr);                
                dsm_packet_length               = dsm_length_packet(dsm_ptr);
                TF_ASSERT(dsm_packet_length == dsm_pullup(&dsm_ptr,msg_payload, dsm_packet_length));                
                if (msg_payload[1] == EMM_STATUS)
                {
                    keep_the_test_running = FALSE;                    
                }       
                break;
            case NAS_EMM_DL_GENERIC_NAS_TRANSPORT_IND:
                keep_the_test_running = FALSE;                    
                break ;
            default:
                break;
            }
        } 
    }
    qurt_signal_destroy(&signal_nas);
}

void emm_test_auth_fuzz(msgr_client_t  *client_ptr)
{   
    stub_rx_type             stub_rx ;
    errno_enum_type          e_retval;
    dword                    bytes_rcvd;
    boolean                  keep_the_test_running = TRUE ;    
    boolean                  reset_in_progress = FALSE;
    uint8  num_attach = 0;
    uint32 dsm_packet_length;
    dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
    msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */
    byte msg_payload[EMM_MAX_MESSAGE_LENGTH];
    qurt_signal_init(&signal_nas);

#ifdef FEATURE_NAS_AFL 
    while(__AFL_LOOP(1000))
#endif
    {
        if (keep_the_test_running == FALSE)
        {               
            nas_stub_send_stop_mode_req(SYS_STOP_MODE_REASON_LPM);
            reset_in_progress = TRUE;
            while(reset_in_progress)
            {
                e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
                switch (stub_rx.msg_hdr.id)
                {
                case LTE_RRC_UL_DATA_REQ: 
                    nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
                    break;
                case LTE_RRC_CONN_ABORT_REQ:
                    nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
                    break;
                case LTE_RRC_DEACTIVATE_REQ:
                    nas_stub_send_rrc_deactivate_cnf(TRUE);
                    reset_in_progress = FALSE;
                    break;
                default:
                    break;
                }
            }
            memset(&stub_rx, 0, sizeof(stub_rx_type));
            esm_fuzzing_reset();
        }
        nas_stub_send_mmr_sim_available_req();
        qurt_signal_wait(&signal_nas, 1, QURT_SIGNAL_ATTR_WAIT_ANY);              
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);           
        keep_the_test_running = TRUE;                
        while (keep_the_test_running)
        {
            e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );            
            switch (stub_rx.msg_hdr.id)
            {
            case LTE_RRC_SERVICE_REQ:                
                req_plmn = stub_rx.rrc_service_req.req_plmn;
                nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, stub_rx.rrc_service_req.req_plmn); 
                break ;
            case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:        
                nas_stub_send_pdn_connectivity_req_in_esm_data_req();
                break;
            case LTE_RRC_CONN_EST_REQ:        
                nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
                nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                    stub_rx.rrc_conn_est_req.transaction_id);

                //nas_stub_send_authentication_req();

                processing_done = FALSE;
                ota_message_discarded_nft = FALSE;
                nas_msg_decoding_status_nft = FALSE;
                tlb_message_discarded_nft = FALSE;
                qurt_signal_clear(&signal_nas, 2);                            
                FuzzOTAFile();                
                qurt_signal_wait(&signal_nas, 2, QURT_SIGNAL_ATTR_WAIT_ANY);   
                qurt_signal_clear(&signal_nas, 2);               
                nas_msg_decoding_status_nft = FALSE; /* Reset OTA message decoding status */       
                if (( no_of_ota_messages_nft == 0 ) || (ota_message_discarded_nft == TRUE)|| (tlb_message_discarded_nft == TRUE))
                {               
                    keep_the_test_running = FALSE;
                }
                break ;

            case LTE_RRC_UL_DATA_REQ:
                memset( msg_payload, 0, EMM_MAX_MESSAGE_LENGTH);                
                num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req);

                    att_ptr = msgr_get_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req, 0);
                    msgr_get_dsm_attach(att_ptr, &dsm_ptr);
                    TF_ASSERT(dsm_ptr != NULL);      
                    dsm_packet_length               = dsm_length_packet(dsm_ptr);                    
                    TF_ASSERT(dsm_packet_length == dsm_pullup(&dsm_ptr,msg_payload, dsm_packet_length));

                if((msg_payload[1] == EMM_AUTHENTICATION_RESPONSE)||(msg_payload[1] == EMM_STATUS))      
                {
                    keep_the_test_running = FALSE;
                }                
                break;
            default:     
                break ;
            }
        }
    }
    qurt_signal_destroy(&signal_nas);   
} 

void emm_test_auth_rej_fuzz(msgr_client_t  *client_ptr)
{    
    stub_rx_type             stub_rx ;
    errno_enum_type          e_retval;
    dword                    bytes_rcvd;
    boolean                  keep_the_test_running = TRUE ;    
    boolean                  reset_in_progress = FALSE;
    uint8  num_attach = 0;
    uint32 dsm_packet_length;
    dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
    msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */
    byte msg_payload[EMM_MAX_MESSAGE_LENGTH];
    qurt_signal_init(&signal_nas);

#ifdef FEATURE_NAS_AFL 
    while(__AFL_LOOP(1000))
#endif
    {
        if (keep_the_test_running == FALSE)
        {               
            nas_stub_send_stop_mode_req(SYS_STOP_MODE_REASON_LPM);
            reset_in_progress = TRUE;
            while(reset_in_progress)
            {
                e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
                switch (stub_rx.msg_hdr.id)
                {
                case LTE_RRC_UL_DATA_REQ: 
                    nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
                    break;
                case LTE_RRC_CONN_ABORT_REQ:
                    nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
                    break;
                case LTE_RRC_DEACTIVATE_REQ:
                    nas_stub_send_rrc_deactivate_cnf(TRUE);
                    reset_in_progress = FALSE;
                    break;
                default:
                    break;
                }
            }
            memset(&stub_rx, 0, sizeof(stub_rx_type));
            esm_fuzzing_reset();
        }
        nas_stub_send_mmr_sim_available_req();
        qurt_signal_wait(&signal_nas, 1, QURT_SIGNAL_ATTR_WAIT_ANY);              
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);           
        keep_the_test_running = TRUE;
        while (keep_the_test_running)
        {
            e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );            
            switch (stub_rx.msg_hdr.id)
            {
            case LTE_RRC_CONN_ABORT_REQ:                
                nas_stub_send_only_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL, FALSE);
                break;
            case NAS_EMM_DETACH_IND: // andre IOT
            case NAS_ESM_DETACH_IND:
                keep_the_test_running = FALSE;
                break;
            case LTE_RRC_SERVICE_REQ:                
                req_plmn = stub_rx.rrc_service_req.req_plmn;
                nas_stub_send_rrc_service_ind(stub_rx.rrc_service_req.trans_id, stub_rx.rrc_service_req.req_plmn); 
                break ;
            case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:        
                nas_stub_send_pdn_connectivity_req_in_esm_data_req();
                break;
            case LTE_RRC_CONN_EST_REQ:        
                nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
                nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                    stub_rx.rrc_conn_est_req.transaction_id);
                nas_stub_send_authentication_req();
                break ;

            case LTE_RRC_UL_DATA_REQ:
                memset( msg_payload, 0, EMM_MAX_MESSAGE_LENGTH);                
                num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req);

                att_ptr = msgr_get_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req, 0);
                msgr_get_dsm_attach(att_ptr, &dsm_ptr);                
                    dsm_packet_length               = dsm_length_packet(dsm_ptr);                    
                TF_ASSERT(dsm_packet_length == dsm_pullup(&dsm_ptr,msg_payload, dsm_packet_length));                   
                if (msg_payload[1] == EMM_AUTHENTICATION_RESPONSE)
                {
                    processing_done = FALSE;
                    ota_message_discarded_nft = FALSE;
                    nas_msg_decoding_status_nft = FALSE;
                    tlb_message_discarded_nft = FALSE;
                    qurt_signal_clear(&signal_nas, 2);                            
                    FuzzOTAFile();   
                    qurt_signal_wait(&signal_nas, 2, QURT_SIGNAL_ATTR_WAIT_ANY);   
                    qurt_signal_clear(&signal_nas, 2);                            
                    nas_msg_decoding_status_nft = FALSE; /* Reset OTA message decoding status */                
                    if (( no_of_ota_messages_nft == 0 ) || (ota_message_discarded_nft == TRUE)|| (tlb_message_discarded_nft == TRUE))
                    {               
                        keep_the_test_running = FALSE;
                    }
                }
                break;
            default:     
                break ;
            }
        }
    }
    qurt_signal_destroy(&signal_nas);    
} 



void emm_test_smc_fuzz(msgr_client_t  *client_ptr)
{   
    dword                    bytes_rcvd;
    boolean reset_in_progress = TRUE;     
    boolean                  keep_the_test_running = TRUE ;
    byte cnt = 1;
    uint8  num_attach = 0;
    uint32 dsm_packet_length;
    dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
    msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */
    stub_rx_type             stub_rx ;
    errno_enum_type          e_retval;
    byte msg_payload[EMM_MAX_MESSAGE_LENGTH];    

    qurt_signal_init(&signal_nas);    
    
#ifdef FEATURE_NAS_AFL 
    while(__AFL_LOOP(1000))
#endif
    {
        if (keep_the_test_running == FALSE)
        {               
            nas_stub_send_stop_mode_req(SYS_STOP_MODE_REASON_LPM);
            reset_in_progress = TRUE;
            while(reset_in_progress)
            {
                e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
                switch (stub_rx.msg_hdr.id)
                {
                case LTE_RRC_UL_DATA_REQ: 
                    nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
                    break;
                case LTE_RRC_CONN_ABORT_REQ:
                    nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
                    break;
                case LTE_RRC_DEACTIVATE_REQ:
                    nas_stub_send_rrc_deactivate_cnf(TRUE);
                    reset_in_progress = FALSE;
                    break;
                default:
                    break;
                }
            }
            memset(&stub_rx, 0, sizeof(stub_rx_type));
            esm_fuzzing_reset();
        }

        nas_stub_send_mmr_sim_available_req();
        qurt_signal_wait(&signal_nas, 1, QURT_SIGNAL_ATTR_WAIT_ANY);          
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);
        cnt = 1;        
        keep_the_test_running = TRUE;        
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
                nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
                nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                    stub_rx.rrc_conn_est_req.transaction_id);

                //nas_stub_send_security_mode_cmd();
                processing_done = FALSE;
                ota_message_discarded_nft = FALSE;
                nas_msg_decoding_status_nft = FALSE;
                tlb_message_discarded_nft = FALSE;
                qurt_signal_clear(&signal_nas, 2);
                FuzzOTAFile();
                qurt_signal_wait(&signal_nas, 2, QURT_SIGNAL_ATTR_WAIT_ANY);                
                nas_msg_decoding_status_nft = FALSE; /* Reset OTA message decoding status */               
                if (( no_of_ota_messages_nft == 0 ) || (ota_message_discarded_nft == TRUE)|| (tlb_message_discarded_nft == TRUE))
                {               
                    keep_the_test_running = FALSE;
                }
                cnt++;
                break ;
            case LTE_RRC_UL_DATA_REQ: 
                nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
                if( cnt == 2 )
                {
                    keep_the_test_running = FALSE;
                }        
                break;            
            default:        
                break ;
            }
        }
    }
    qurt_signal_destroy(&signal_nas);
} 


void emm_test_identity_req_fuzz(msgr_client_t  *client_ptr)
{   
    boolean                  keep_the_test_running = TRUE ;
    boolean                  reset_in_progress = FALSE;
    int cnt = 1;    
    uint8  num_attach = 0;
    uint32 dsm_packet_length;
    stub_rx_type             stub_rx ;
    errno_enum_type          e_retval;
    dword                    bytes_rcvd;
    dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
    msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */
    byte msg_payload[EMM_MAX_MESSAGE_LENGTH];
    qurt_signal_init(&signal_nas);

#ifdef FEATURE_NAS_AFL 
    while(__AFL_LOOP(1000))
#endif
    {
        if (keep_the_test_running == FALSE)
        {               
            nas_stub_send_stop_mode_req(SYS_STOP_MODE_REASON_LPM);
            reset_in_progress = TRUE;
            while(reset_in_progress)
            {
                e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
                switch (stub_rx.msg_hdr.id)
                {
                case LTE_RRC_UL_DATA_REQ: 
                    nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
                    break;
                case LTE_RRC_CONN_ABORT_REQ:
                    nas_stub_send_rrc_connection_rel_ind(LTE_RRC_CONN_REL_NORMAL);
                    break;
                case LTE_RRC_DEACTIVATE_REQ:
                    nas_stub_send_rrc_deactivate_cnf(TRUE);
                    reset_in_progress = FALSE;
                    break;
                default:
                    break;
                }
            }
            memset(&stub_rx, 0, sizeof(stub_rx_type));
            esm_fuzzing_reset();
        }
        nas_stub_send_mmr_sim_available_req();
        qurt_signal_wait(&signal_nas, 1, QURT_SIGNAL_ATTR_WAIT_ANY);        
        nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);           
        keep_the_test_running = TRUE;        
        cnt = 1;

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
                nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
                nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                    stub_rx.rrc_conn_est_req.transaction_id);

                //nas_stub_send_identity_req();

                processing_done = FALSE;
                ota_message_discarded_nft = FALSE;
                nas_msg_decoding_status_nft = FALSE;
                tlb_message_discarded_nft = FALSE;
                qurt_signal_clear(&signal_nas, 2);
                FuzzOTAFile();
                qurt_signal_wait(&signal_nas, 2, QURT_SIGNAL_ATTR_WAIT_ANY);                
                nas_msg_decoding_status_nft = FALSE; /* Reset OTA message decoding status */        
                if (( no_of_ota_messages_nft == 0 ) || (ota_message_discarded_nft == TRUE)|| (tlb_message_discarded_nft == TRUE))
                {               
                    keep_the_test_running = FALSE;
                }
                cnt++;
                break ;

            case LTE_RRC_UL_DATA_REQ: 
                // Identity Response
                nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
                if( cnt == 2 )
                {
                    keep_the_test_running = FALSE;
                }        
                break;            
            default:                
                break ;
            }
        }
    }
    qurt_signal_destroy(&signal_nas);
} 


void emm_test_auth_and_smc_mme_change(msgr_client_t  *client_ptr)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;

  byte cnt = 1;
  uint8  num_attach = 0;
  uint32 dsm_packet_length;
  dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
  msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */
  byte msg_payload[EMM_MAX_MESSAGE_LENGTH];

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC);    

  TF_STUB(get_nas_sec_info) = my_get_nas_sec_info;  
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
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_authentication_req();
        break ;

      case LTE_RRC_UL_DATA_REQ:
 
      if( cnt == 2 )
        keep_the_test_running = FALSE;

        memset( msg_payload, 0, EMM_MAX_MESSAGE_LENGTH);
  
        /* See if NAS has sent the DT message as a DSM item by checking for attachments */
        num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req);
      
        if ( num_attach == 1 )
        {
          att_ptr = msgr_get_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req, 0);
          msgr_get_dsm_attach(att_ptr, &dsm_ptr);
          TF_ASSERT(dsm_ptr != NULL);
      
          dsm_packet_length               = dsm_length_packet(dsm_ptr);
          TF_MSG("TEST>DSM UL Data found! item len %d", dsm_packet_length,0,0);
          
          TF_ASSERT(dsm_packet_length == dsm_pullup(&dsm_ptr,msg_payload, dsm_packet_length));
         
          /* Received dsm should have been freed */
          TF_ASSERT(dsm_ptr == NULL);
        }
        else
        {
          ERR_FATAL("TEST> DSM UL not found. EMM using old interface",0,0,0);
        }

      if((msg_payload[1] ==SECURITY_MODE_COMPLETE ) ||
         (msg_payload[1] ==SECURITY_MODE_REJECT ) )
      {
        nas_stub_send_security_mode_cmd();
        cnt++;
      }
      else
      {
        nas_stub_send_security_mode_cmd();
        TF_YIELD();
        TF_YIELD();
        TF_YIELD();
        TF_YIELD();
      }
      break;
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
  }
  TF_YIELD(); 

} 

void emm_test_attach_with_auth(msgr_client_t  *client_ptr)
{
  stub_rx_type     stub_rx ;
  stub_rx_type*    stub_rx_ptr = NULL;
  boolean keep_going = TRUE;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  stub_rx_ptr = &stub_rx;
  uint8  num_attach = 0;
  uint32 dsm_packet_length;
  dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
  msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */
  byte msg_payload[EMM_MAX_MESSAGE_LENGTH];

//  nas_stub_send_mmr_act_req();
  nas_stub_send_mmr_sim_not_available_req();
  nas_stub_send_mmr_sim_available_req();
  nas_stub_send_mmr_reg_req(SYS_SRV_DOMAIN_PS_ONLY,SYS_RAT_LTE_RADIO_ACCESS,
                            SYS_NETWORK_SELECTION_MODE_AUTOMATIC); 

  while(keep_going)
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
        (void)nas_test_conn_est_req_dsm_pullup(&stub_rx);
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        nas_stub_send_authentication_req();
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_SIM_UPDATE_REQ");
        break;
      case LTE_RRC_UL_DATA_REQ:
        TF_MSG("NAS_TEST_STUB> EMM sent  LTE_RRC_UL_DATA_REQ");
        memset( msg_payload, 0, EMM_MAX_MESSAGE_LENGTH);
  
        /* See if NAS has sent the DT message as a DSM item by checking for attachments */
        num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req);
      
        if ( num_attach == 1 )
        {
          att_ptr = msgr_get_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req, 0);
          msgr_get_dsm_attach(att_ptr, &dsm_ptr);
          TF_ASSERT(dsm_ptr != NULL);
      
          dsm_packet_length               = dsm_length_packet(dsm_ptr);
          TF_MSG("TEST>DSM UL Data found! item len %d", dsm_packet_length,0,0);
          
          TF_ASSERT(dsm_packet_length == dsm_pullup(&dsm_ptr,msg_payload, dsm_packet_length));
         
          /* Received dsm should have been freed */
          TF_ASSERT(dsm_ptr == NULL);
        }
        else
        {
          ERR_FATAL("TEST> DSM UL not found. EMM using old interface",0,0,0);
        }

        if(msg_payload[1] == EMM_AUTHENTICATION_RESPONSE)
        {
         // authentication is done, send an attach accept 
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
          nas_stub_send_attach_accept_and_def_bearer_ctxt_req(ESM_INVALID_BEARER_ID, ESM_INVALID_PTI);
        }
        else if(msg_payload[1] == ATTACH_COMPLETE )
        {
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
          nas_stub_send_rrc_active_eps_ind();
        }
        break;
    case NAS_ESM_DATA_IND:
        nas_stub_send_emm_data_req();
    break;
    case NAS_ESM_ACTIVE_EPS_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_ACTIVE_EPS_IND ");
        keep_going = FALSE;
        break ;

    case LTE_RRC_ACTIVE_EPS_BEARER_UPDATE_IND:
     TF_MSG("TEST> EMM sent LTE_RRC_ACTIVE_EPS_BEARER_UPDATE_IND ");
    break;
     }
  }
}


void emm_test_auth_res_tx_fail(msgr_client_t  *client_ptr)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  uint8  num_attach = 0;
  uint32 dsm_packet_length;
  dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
  msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */
  byte msg_payload[EMM_MAX_MESSAGE_LENGTH];

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
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        nas_stub_send_authentication_req();
        break ;

      case LTE_RRC_UL_DATA_REQ:
        memset( msg_payload, 0, EMM_MAX_MESSAGE_LENGTH);
  
        /* See if NAS has sent the DT message as a DSM item by checking for attachments */
        num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req);
      
        if ( num_attach == 1 )
        {
          att_ptr = msgr_get_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req, 0);
          msgr_get_dsm_attach(att_ptr, &dsm_ptr);
          TF_ASSERT(dsm_ptr != NULL);
      
          dsm_packet_length               = dsm_length_packet(dsm_ptr);
          TF_MSG("TEST>DSM UL Data found! item len %d", dsm_packet_length,0,0);
          
          TF_ASSERT(dsm_packet_length == dsm_pullup(&dsm_ptr,msg_payload, dsm_packet_length));
         
          /* Received dsm should have been freed */
          TF_ASSERT(dsm_ptr == NULL);
        }
        else
        {
          ERR_FATAL("TEST> DSM UL not found. EMM using old interface",0,0,0);
        }

        if(msg_payload[1] == EMM_AUTHENTICATION_RESPONSE)
        {
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_FAILURE_TXN, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        }
        if(msg_payload[1] == ATTACH_REQUEST)
        {
          keep_the_test_running = FALSE;
        }
      break;
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
  }
  TF_YIELD(); 

} 


void emm_test_tau_with_auth_res_tx_fail(msgr_client_t       *client_ptr)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  lte_rrc_plmn_s           plmn;
  byte                     count = 0;
  uint8  num_attach = 0;
  uint32 dsm_packet_length;
  dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
  msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */
  byte msg_payload[EMM_MAX_MESSAGE_LENGTH];

  /* Send RRC service indication with updated TAI to trigger TAU */
  memset(&plmn, 0x00, sizeof(lte_rrc_plmn_s));
  
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
        //nas_stub_send_rrc_service_ind(0xFFFFFFFF, plmn);
        nas_stub_send_attach_accept_and_def_bearer_ctxt_req(ESM_INVALID_BEARER_ID, ESM_INVALID_PTI);
        break ;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
         
        TF_YIELD();
        memset( msg_payload, 0, EMM_MAX_MESSAGE_LENGTH);
  
        /* See if NAS has sent the DT message as a DSM item by checking for attachments */
        num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req);
      
        if ( num_attach == 1 )
        {
          att_ptr = msgr_get_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req, 0);
          msgr_get_dsm_attach(att_ptr, &dsm_ptr);
          TF_ASSERT(dsm_ptr != NULL);
      
          dsm_packet_length               = dsm_length_packet(dsm_ptr);
          TF_MSG("TEST>DSM UL Data found! item len %d", dsm_packet_length,0,0);
          
          TF_ASSERT(dsm_packet_length == dsm_pullup(&dsm_ptr,msg_payload, dsm_packet_length));
         
          /* Received dsm should have been freed */
          TF_ASSERT(dsm_ptr == NULL);
        }
        else
        {
          ERR_FATAL("TEST> DSM UL not found. EMM using old interface",0,0,0);
        }

        if(msg_payload[1] == ATTACH_COMPLETE)
        {
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
          nas_stub_send_rrc_service_ind(0xFFFFFFFF, plmn);
        }
        else if(msg_payload[1] == TRACKING_AREA_UPADTE_REQUEST)
        {
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
          count++;
          if( count == 2 )
          {
            keep_the_test_running = FALSE;
          }
          else 
           nas_stub_send_authentication_req();
            
        } else if(msg_payload[1] == EMM_AUTHENTICATION_RESPONSE)
         {
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_FAILURE_TXN, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        }
        
        break ;
      case NAS_ESM_ACTIVE_EPS_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_ACTIVE_EPS_IND ");
        break ;
      case NAS_ESM_DATA_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_DATA_IND ");
        nas_stub_send_emm_data_req();
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

} /* emm_test_tau_with_auth_res_tx_fail */


void emm_test_service_req_with_auth_res_tx_fail(msgr_client_t  *client_ptr)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  uint8  num_attach = 0;
  uint32 dsm_packet_length;
  dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
  msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */
  byte msg_payload[EMM_MAX_MESSAGE_LENGTH];

  nas_stub_send_service_request(LTE_RRC_EST_CAUSE_MO_DATA);

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
        nas_stub_send_authentication_req();
        break;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
                memset( msg_payload, 0, EMM_MAX_MESSAGE_LENGTH);
  
        /* See if NAS has sent the DT message as a DSM item by checking for attachments */
        num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req);
      
        if ( num_attach == 1 )
        {
          att_ptr = msgr_get_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req, 0);
          msgr_get_dsm_attach(att_ptr, &dsm_ptr);
          TF_ASSERT(dsm_ptr != NULL);
      
          dsm_packet_length               = dsm_length_packet(dsm_ptr);
          TF_MSG("TEST>DSM UL Data found! item len %d", dsm_packet_length,0,0);
          
          TF_ASSERT(dsm_packet_length == dsm_pullup(&dsm_ptr,msg_payload, dsm_packet_length));
         
          /* Received dsm should have been freed */
          TF_ASSERT(dsm_ptr == NULL);
        }
        else
        {
          ERR_FATAL("TEST> DSM UL not found. EMM using old interface",0,0,0);
        }

        if(msg_payload[1] == EMM_AUTHENTICATION_RESPONSE)
        {
          nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_FAILURE_HO, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
        }
        
        if(msg_payload[1] == TRACKING_AREA_UPADTE_REQUEST)
          keep_the_test_running = FALSE;
        break ;
      case NAS_ESM_ACTIVE_EPS_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_ACTIVE_EPS_IND ");
        break ;
      case NAS_ESM_DATA_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_DATA_IND ");
        nas_stub_send_emm_data_req();
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SIM_UPDATE_REQ");
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_SIG_CONNECTION_RELEASED_IND");
        break;
      case NAS_ESM_FAILURE_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_FAILURE_IND");
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

} 



void emm_test_service_req_with_auth_fail_tx_fail(msgr_client_t  *client_ptr)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  uint8  num_attach = 0;
  uint32 dsm_packet_length;
  dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
  msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */
  byte msg_payload[EMM_MAX_MESSAGE_LENGTH];

  while (keep_the_test_running)
  {
    e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
    TF_ASSERT( e_retval == E_SUCCESS );

    switch (stub_rx.msg_hdr.id)
    {
      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_authentication_req();
        break;
      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        memset( msg_payload, 0, EMM_MAX_MESSAGE_LENGTH);
  
        /* See if NAS has sent the DT message as a DSM item by checking for attachments */
        num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req);
      
        if ( num_attach == 1 )
        {
          att_ptr = msgr_get_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req, 0);
          msgr_get_dsm_attach(att_ptr, &dsm_ptr);
          TF_ASSERT(dsm_ptr != NULL);
      
          dsm_packet_length               = dsm_length_packet(dsm_ptr);
          TF_MSG("TEST>DSM UL Data found! item len %d", dsm_packet_length,0,0);
          
          TF_ASSERT(dsm_packet_length == dsm_pullup(&dsm_ptr,msg_payload, dsm_packet_length));
         
          /* Received dsm should have been freed */
          TF_ASSERT(dsm_ptr == NULL);
        }
        else
        {
          ERR_FATAL("TEST> DSM UL not found. EMM using old interface",0,0,0);
        }
        
        if(msg_payload[1] == EMM_AUTHENTICATION_RESPONSE)
        {
          nas_stub_send_rrc_ul_data_cnf_no_tai_chg(LTE_RRC_UL_DATA_CNF_FAILURE_HO, 
                                      stub_rx.rrc_conn_est_req.transaction_id);
          TF_YIELD();
        } else 
        {
          nas_stub_send_service_reject(LTE_NAS_PLMN_NOT_ALLOWED);
          TF_YIELD();
        }
        break ;
      case NAS_ESM_ACTIVE_EPS_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_ACTIVE_EPS_IND ");
        break ;
      case NAS_ESM_DATA_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_DATA_IND ");
        nas_stub_send_emm_data_req();
        break ;
      case LTE_RRC_SYSTEM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SYSTEM_UPDATE_REQ ");
        break;
      case LTE_RRC_SIM_UPDATE_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_SIM_UPDATE_REQ");
        keep_the_test_running = FALSE;
        break;
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_SIG_CONNECTION_RELEASED_IND");
        nas_stub_send_service_request(LTE_RRC_EST_CAUSE_MO_DATA);
        break;
      case NAS_ESM_FAILURE_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_FAILURE_IND");
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

} 


void emm_test_guti_realloc_cmd(msgr_client_t  *client_ptr)
{

  stub_rx_type             stub_rx ;
  errno_enum_type          e_retval;
  dword                    bytes_rcvd;
  boolean                  keep_the_test_running = TRUE ;
  uint8  num_attach = 0;
  uint32 dsm_packet_length;
  dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
  msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */
  byte msg_payload[EMM_MAX_MESSAGE_LENGTH];
  
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

      case LTE_RRC_CONN_EST_REQ:
        TF_MSG("TEST> EMM sent LTE_RRC_CONN_EST_REQ ");
        nas_stub_send_rrc_conn_est_cnf(LTE_RRC_CONN_EST_SUCCESS);
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_conn_est_req.transaction_id);
        nas_test_conn_est_req_dsm_pullup(&stub_rx);
        nas_stub_send_authentication_req();
        break ;

      case NAS_ESM_ACTIVE_EPS_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_ACTIVE_EPS_IND ");
        break ;

      case NAS_ESM_DATA_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_DATA_IND ");
        nas_stub_send_emm_data_req();
        break ;

      case LTE_RRC_UL_DATA_REQ: 
        TF_MSG("TEST> EMM sent LTE_RRC_UL_DATA_REQ ");
        nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
        memset( msg_payload, 0, EMM_MAX_MESSAGE_LENGTH);
  
        /* See if NAS has sent the DT message as a DSM item by checking for attachments */
        num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req);
      
        if ( num_attach == 1 )
        {
          att_ptr = msgr_get_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req, 0);
          msgr_get_dsm_attach(att_ptr, &dsm_ptr);
          TF_ASSERT(dsm_ptr != NULL);
      
          dsm_packet_length               = dsm_length_packet(dsm_ptr);
          TF_MSG("TEST>DSM UL Data found! item len %d", dsm_packet_length,0,0);
          
          TF_ASSERT(dsm_packet_length == dsm_pullup(&dsm_ptr,msg_payload, dsm_packet_length));
         
          /* Received dsm should have been freed */
          TF_ASSERT(dsm_ptr == NULL);
        }
        else
        {
          ERR_FATAL("TEST> DSM UL not found. EMM using old interface",0,0,0);
        }

        if (msg_payload[1] == EMM_AUTHENTICATION_RESPONSE)
        {
          nas_stub_send_security_mode_cmd();
          TF_YIELD();
        }
        else if (msg_payload[1] == SECURITY_MODE_COMPLETE)
        {
          nas_stub_send_attach_accept_and_def_bearer_ctxt_req(ESM_INVALID_BEARER_ID, ESM_INVALID_PTI);
          TF_YIELD();
        }
        else if (msg_payload[1] == ATTACH_COMPLETE)
        {
          nas_stub_send_guti_realloc_cmd();
          TF_YIELD();
        } else if (msg_payload[1] == GUTI_REALLOCATION_COMPLETE)
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
      case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
        TF_MSG("TEST> EMM sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND");
        nas_stub_send_pdn_connectivity_req_in_esm_data_req();
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
} 

void emm_test_guti_realloc_fuzz(msgr_client_t  *client_ptr)
{  
    stub_rx_type             stub_rx ;
    errno_enum_type          e_retval;
    dword                    bytes_rcvd;
    boolean                  keep_the_test_running = TRUE ;
    boolean                  reset_in_progress = FALSE;
    uint8  num_attach = 0;
    uint32 dsm_packet_length;
    dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
    msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */
    byte msg_payload[EMM_MAX_MESSAGE_LENGTH];
    qurt_signal_init(&signal_nas);

#ifdef FEATURE_NAS_AFL 
    while(__AFL_LOOP(1000))
#endif
    {        
        stop_and_attach(keep_the_test_running, client_ptr, &signal_nas);
        qurt_signal_wait(&signal_nas, 2, QURT_SIGNAL_ATTR_WAIT_ANY);
        qurt_signal_clear(&signal_nas, 2);

        processing_done = FALSE;
        ota_message_discarded_nft = FALSE;
        nas_msg_decoding_status_nft = FALSE;
        tlb_message_discarded_nft = FALSE;
        qurt_signal_clear(&signal_nas, 2);
        FuzzOTAFile();
        qurt_signal_wait(&signal_nas, 2, QURT_SIGNAL_ATTR_WAIT_ANY);                  
        nas_msg_decoding_status_nft = FALSE; /* Reset OTA message decoding status */
        keep_the_test_running = TRUE ;
        if (( no_of_ota_messages_nft == 0 ) || (ota_message_discarded_nft == TRUE)|| (tlb_message_discarded_nft == TRUE))
        {
            keep_the_test_running = FALSE;
        }

        while(keep_the_test_running)
        {
            e_retval = msgr_receive(client_ptr, (byte*)&(stub_rx), sizeof(stub_rx_type), &bytes_rcvd );
            TF_ASSERT( e_retval == E_SUCCESS );

            switch (stub_rx.msg_hdr.id)
            {
            case LTE_RRC_UL_DATA_REQ: 
                nas_stub_send_rrc_ul_data_cnf(LTE_RRC_UL_DATA_CNF_SUCCESS, stub_rx.rrc_ul_data_req.trans_id);
                memset( msg_payload, 0, EMM_MAX_MESSAGE_LENGTH);
                num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req);
                att_ptr = msgr_get_attach((msgr_hdr_struct_type *)&stub_rx.rrc_ul_data_req, 0);
                msgr_get_dsm_attach(att_ptr, &dsm_ptr);                
                dsm_packet_length               = dsm_length_packet(dsm_ptr);
                TF_ASSERT(dsm_packet_length == dsm_pullup(&dsm_ptr,msg_payload, dsm_packet_length));                
                if ((msg_payload[1] == GUTI_REALLOCATION_COMPLETE)||(msg_payload[1] == EMM_STATUS))
                {
                    keep_the_test_running = FALSE;                    
                }          
                break ;

            default:
                break;
            }
        } 
    }
    qurt_signal_destroy(&signal_nas);
}


#endif
