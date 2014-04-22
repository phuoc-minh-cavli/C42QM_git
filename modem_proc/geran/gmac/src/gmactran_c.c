/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC TRANSFER CONCURRENT MODULE
***
***
*** DESCRIPTION
***
***  This module contains functionality related to concurrent packet           
***  transfer (i.e. when UL and DL exist)                 
***  
*** EXTERNALIZED FUNCTIONS
***
*** 
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***
*** Copyright (c) 2001,2002,2003,2004,2005-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //components/rel/geran.mpss/5.2.0/gmac/src/gmactran_c.c#3 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 05/12/01   sb      Created
***
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h"

#include "grlcx.h"    /* required for test output usage */
#include "csn.h"
#include "gmacdef.h"
#include "gmacrb.h"
#include "gmactransfer.h"
#include "gmacutil.h"
#include "gmactran_dl.h"
#include "gmactran_ul.h"
#include "gmactran_c.h"
#include "gmaclog.h"
#include "gmacproc.h"

#if defined(TEST_FRAMEWORK)
#error code not present
#endif // TEST_FRAMEWORK

/*****************************************************************************
***
***     Public Data Declarations
***
*****************************************************************************/

/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/
static void gmac_concurrent_transfer_handle_tbf_release(void);  

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_concurrent_data_ind 
===
===  DESCRIPTION
===  Handles received logical channel messages during uplink dynamic
===  packet transfer           
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header 
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_tdynamic_concurrent_data_ind(void)
{
#ifdef FEATURE_GSM_EDTM
  gprs_mac_transfer_substates_t tran_state_change = TRANSFER_INACTIVE;
  char                         *new_state_str           = NULL;
#endif /*FEATURE_GSM_EDTM*/
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr();
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr();
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr();

  /* If message not recognised, send MOBILE_TBF_STATUS to network and ignore */
  if (global_data_ptr->csn_dec_hdr_ptr->error_code == UNKNOWN_MESSAGE_TYPE)
  {
    MSG_GERAN_LOW_0_G("MAC info CSN MT unknown");
    gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_PROTOCOL_ERROR_CAUSE, global_data_ptr->csn_dec_hdr_ptr->message_type);
    return;
  }

#ifdef FEATURE_GSM_DTM  
  if ( transfer_data_ptr->t_flags.dtm_realloc_in_progress )
  {
    MSG_GERAN_MED_1_G("GMAC reallocating DTM, PH_DATA_IND inhibited, msg_type:0x%x",global_data_ptr->csn_dec_hdr_ptr->message_type);
    return;
  }
#endif /*FEATURE_GSM_DTM*/

  switch (global_data_ptr->csn_dec_hdr_ptr->message_type)
  {
    case PACKET_UPLINK_ACK_NACK:
      gmac_tdynamic_handle_puan(TRUE);
      break;

    case PACKET_DOWNLINK_ASSIGNMENT:

      /* PDA rxd in UL state so apply current_mac_mode */
      if (gmac_transfer_ul_handle_pda(global_data_ptr->gmac_current_mac_mode))
      {
        grlc_dl_start_t3190();
        gmac_tran_log_state_change(TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN, __LINE__);
        transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DYNAMIC_DL_REASSIGN;
        /*gmac_rcvd_dl_assignment_in_concurrency flag is set to true . This flag is used to 
        go to a appropriate transfer state if gmac gets a dl release cnf before connect
        ind from GL1. */
        transfer_data_ptr->t_flags.gmac_rcvd_dl_assignment_in_concurrency = TRUE;

        if (((packet_downlink_assignment_t*)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf(); 
        }            

        (void)mac_log_state();
      }
      break;

    case PACKET_UPLINK_ASSIGNMENT:
      if(transfer_data_ptr->t_flags.block_dl_msg_during_normal_ul_release == FALSE)
      {
        /* CR 69716 */
        if( 
            ( rex_get_timer(&mac_timers_ptr->timer_t3180) == 0 ) && 
            ( global_data_ptr->current_test_mode != TEST_MODE_SRB ) 
          )
        {
          MSG_GERAN_MED_2_G("PUA rcvd during ul rel. t3180 = %d, Test Mode: %d",
                            rex_get_timer(&mac_timers_ptr->timer_t3180),global_data_ptr->current_test_mode); 
        }
        else
        {
          if (gmac_tdynamic_handle_pua())
          {
            gmac_stop_t3168();
            gmac_tran_log_state_change(TRANSFER_CONCURRENT_DYNAMIC_REALLOC, __LINE__);
            transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DYNAMIC_REALLOC;
  
            if (((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
            {
              gmac_transfer_simulate_freq_cnf(); 
            }            
   
            (void)mac_log_state();
          }       
        }
      }
      else
      {
        MSG_GERAN_HIGH_0_G("Blocking PUA in T_CON, releasing UL normally");
      }
      break;

    case PACKET_TIMESLOT_RECONFIGURE:
        if (gmac_tdynamic_handle_ptr())
        {
          gmac_stop_t3168();
          gmac_tran_log_state_change(TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC, __LINE__);
          transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC;
          /*gmac_rcvd_dl_assignment_in_concurrency flag is set to true . This flag is used to 
          go to a appropriate transfer state if gmac gets a dl release cnf before connect
          ind from GL1 */
          transfer_data_ptr->t_flags.gmac_rcvd_dl_assignment_in_concurrency = TRUE;
  
          if (((packet_timeslot_reconfigure_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
          {
            gmac_transfer_simulate_freq_cnf();
          }
  
          (void)mac_log_state();
        }
      break;

    case PACKET_ACCESS_REJECT:
      gmac_transfer_handle_par();
      break;

    case PACKET_PWR_CTRL_TMNG_ADV:
      gmac_transfer_handle_ppcta(TRUE);
      break;

    case PACKET_MEASUREMENT_ORDER:
      gmac_util_pmo_handler();
      break;
      
    case PACKET_CELL_CHANGE_ORDER:
      gmac_tran_pcco_handler(transfer_data_ptr->mac_transfer_substate);
      break;

    case PACKET_TBF_RELEASE:
      gmac_concurrent_transfer_handle_tbf_release();
      break;
      
    case PACKET_PDCH_RELEASE:
      gmac_tran_handle_pdch_release(((packet_pdch_release_t *)global_data_ptr->csn_dec_data_ptr)->timeslots_available,UL_AND_DL);
      break;

    case PACKET_POLLING_REQUEST:
      MSG_GERAN_LOW_0_G("MAC Rcvd PACKET_POLLING_REQUEST");
      gmac_idle_pkt_polling_request_handler();
      break;
  
    case PACKET_PAGING_REQUEST:
      MSG_GERAN_LOW_0_G("MAC Rcvd PACKET_PAGING_REQUEST");
      gmac_util_pkt_paging_on_pacch();
      break;

#ifdef FEATURE_GSM_EDTM
    case PACKET_CS_COMMAND:
      gmac_transfer_edtm_pkt_cs_cmd_handler();
      break;

    case PACKET_CS_RELEASE:
      tran_state_change = gmac_tran_edtm_cs_rel_ind_handler( transfer_data_ptr->mac_transfer_substate,
                                                             UL_AND_DL);

      if ( tran_state_change != transfer_data_ptr->mac_transfer_substate )
      {
        new_state_str = gmac_convert_transfer_substate_to_string(tran_state_change);

        MSG_SPRINTF_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "GMAC State tran = %s", new_state_str);       
      }

      transfer_data_ptr->mac_transfer_substate = tran_state_change;
      (void)mac_log_state();
    break;
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
    case PACKET_PS_HANDOVER:
      if(gmac_tran_check_psho_addressing() == TRUE)
      {
        MSG_GERAN_MED_0_G("MAC Rcvd PS_HANDOVER_COMMAND");
        gmac_send_data_ind_to_grr();
        rrbp_control_block_handler_2(PCA_DL_MSG);        
      }
    break;
#endif/*FEATURE_GPRS_PS_HANDOVER*/

    default:
      MSG_GERAN_HIGH_2_G("MAC Rcvd MT %d EC %d",global_data_ptr->csn_dec_hdr_ptr->message_type,global_data_ptr->csn_dec_hdr_ptr->error_code);
      break;
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_concurrent_dl_reassign_data_ind 
===
===  DESCRIPTION
===  Handles received logical channel messages during uplink dynamic
===  packet transfer           
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header 
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_tdynamic_concurrent_dl_reassign_data_ind(void)
{
#ifdef FEATURE_GSM_EDTM
  gprs_mac_transfer_substates_t tran_state_change = TRANSFER_INACTIVE;
  char                         *new_state_str           = NULL;
#endif /*FEATURE_GSM_EDTM*/
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr();
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr();

  /* If message not recognised, send MOBILE_TBF_STATUS to network and ignore */
  if (global_data_ptr->csn_dec_hdr_ptr->error_code == UNKNOWN_MESSAGE_TYPE)
  {
    MSG_GERAN_LOW_0_G("MAC info CSN MT unknown");
    gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_PROTOCOL_ERROR_CAUSE, global_data_ptr->csn_dec_hdr_ptr->message_type);
    return;
  }

#ifdef FEATURE_GSM_DTM  
  if ( transfer_data_ptr->t_flags.dtm_realloc_in_progress )
  {
    MSG_GERAN_MED_1_G("GMAC reallocating DTM, PH_DATA_IND inhibited, msg_type:0x%x",global_data_ptr->csn_dec_hdr_ptr->message_type);
    return;
  }
#endif /*FEATURE_GSM_DTM*/

  switch (global_data_ptr->csn_dec_hdr_ptr->message_type)
  {
    case PACKET_UPLINK_ACK_NACK:
      gmac_tdynamic_handle_puan(TRUE);
      break;

    case PACKET_DOWNLINK_ASSIGNMENT:

      /* PDA rxd in UL state so apply current_mac_mode */
      if(gmac_transfer_ul_handle_pda(global_data_ptr->gmac_current_mac_mode))
      {
         grlc_dl_start_t3190();

         if (((packet_downlink_assignment_t*)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
         {
           gmac_transfer_simulate_freq_cnf(); 
         }
      }
      break;

    case PACKET_UPLINK_ASSIGNMENT:
      /* ignore until DL reassignment complete */
      break;

    case PACKET_TIMESLOT_RECONFIGURE:
      if (gmac_tdynamic_handle_ptr())
      {
        gmac_tran_log_state_change(TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC, __LINE__);
        transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC;

        if (((packet_timeslot_reconfigure_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf();
        }

        (void)mac_log_state();
      }
      break;

    case PACKET_ACCESS_REJECT:
      gmac_transfer_handle_par();
      break;

    case PACKET_PWR_CTRL_TMNG_ADV:
      gmac_transfer_handle_ppcta(TRUE);
      break;

    case PACKET_MEASUREMENT_ORDER:
      gmac_util_pmo_handler();
      break;
      
    case PACKET_CELL_CHANGE_ORDER:
      gmac_tran_pcco_handler(transfer_data_ptr->mac_transfer_substate);
      break;

    case PACKET_TBF_RELEASE:
      gmac_concurrent_transfer_handle_tbf_release();
      break;
      
    case PACKET_PDCH_RELEASE:
      gmac_tran_handle_pdch_release(((packet_pdch_release_t *)global_data_ptr->csn_dec_data_ptr)->timeslots_available,UL_AND_DL);
      break;
      
    case PACKET_POLLING_REQUEST:
      MSG_GERAN_LOW_0_G("MAC Rcvd PACKET_POLLING_REQUEST");
      gmac_idle_pkt_polling_request_handler();
      break;
      
    case PACKET_PAGING_REQUEST:
      MSG_GERAN_LOW_0_G("MAC Rcvd PACKET_PAGING_REQUEST");
      gmac_util_pkt_paging_on_pacch();
      break;

#ifdef FEATURE_GSM_EDTM
    case PACKET_CS_COMMAND:
      gmac_transfer_edtm_pkt_cs_cmd_handler();
      break;

    case PACKET_CS_RELEASE:
      tran_state_change = gmac_tran_edtm_cs_rel_ind_handler( transfer_data_ptr->mac_transfer_substate,
                                                             UL_AND_DL);

      if ( tran_state_change != transfer_data_ptr->mac_transfer_substate )
      {
        new_state_str = gmac_convert_transfer_substate_to_string(tran_state_change);

        MSG_SPRINTF_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "GMAC State tran = %s", new_state_str);        
      }

      transfer_data_ptr->mac_transfer_substate = tran_state_change;
      (void)mac_log_state();
    break;
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
    case PACKET_PS_HANDOVER:
      gmac_tran_psho_handler();
      break;
#endif/*FEATURE_GPRS_PS_HANDOVER*/

    default:
      MSG_GERAN_HIGH_2_G("MAC Rcvd MT %d EC %d",global_data_ptr->csn_dec_hdr_ptr->message_type,global_data_ptr->csn_dec_hdr_ptr->error_code);
      break;
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_concurrent_realloc_data_ind 
===
===  DESCRIPTION
===  Handles received logical channel messages during uplink dynamic
===  packet transfer           
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header                       
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_tdynamic_concurrent_realloc_data_ind(void)
{
#ifdef FEATURE_GSM_EDTM
  gprs_mac_transfer_substates_t tran_state_change = TRANSFER_INACTIVE;
  char                         *new_state_str           = NULL;
#endif /*FEATURE_GSM_EDTM*/
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr();
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr();

  /* If message not recognised, send MOBILE_TBF_STATUS to network and ignore */
  if (global_data_ptr->csn_dec_hdr_ptr->error_code == UNKNOWN_MESSAGE_TYPE)
  {
    MSG_GERAN_LOW_0_G("MAC info CSN MT unknown");
    gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_PROTOCOL_ERROR_CAUSE, global_data_ptr->csn_dec_hdr_ptr->message_type);
    return;
  }

#ifdef FEATURE_GSM_DTM  
  if ( transfer_data_ptr->t_flags.dtm_realloc_in_progress )
  {
    MSG_GERAN_MED_1_G("GMAC reallocating DTM, PH_DATA_IND inhibited, msg_type:0x%x",global_data_ptr->csn_dec_hdr_ptr->message_type);
    return;
  }
#endif /*FEATURE_GSM_DTM*/

  switch (global_data_ptr->csn_dec_hdr_ptr->message_type)
  {
    case PACKET_UPLINK_ACK_NACK:
      gmac_tdynamic_handle_puan(TRUE);
      break;

    case PACKET_DOWNLINK_ASSIGNMENT:

      /* cr 697201 PDA rxd while waiting for connect ind for UL in             */
      /*TRANSFER_CONCURRENT_DYNAMIC_REALLOC state. Reception of PDA is         */
      /*going to put MAC into  TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC */ 
      /*therefor  MAC is going to need to know if its expecting to get two     */
      /*connect inds from l1.  1 for the UL and 1 for the DL UL and D so need  */
      /*set waiting_for_ph_con_for_dl and waiting_for_ph_con_for_ul            */

      /* PDA rxd in UL state so apply current_mac_mode */
      if (gmac_transfer_ul_handle_pda(global_data_ptr->gmac_current_mac_mode))
      {
        
        transfer_data_ptr->t_flags.waiting_for_ph_con_for_dl = TRUE;

        /*Must be in this state but checking to confirm*/
        if(transfer_data_ptr->mac_transfer_substate == TRANSFER_CONCURRENT_DYNAMIC_REALLOC)
        {
          transfer_data_ptr->t_flags.waiting_for_ph_con_for_ul = TRUE;
        }
        
        grlc_dl_start_t3190();
        gmac_tran_log_state_change(TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC, __LINE__);
        transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC;
       
        /*gmac_rcvd_dl_assignment_in_concurrency flag is set to true . This flag is used to 
        go to a appropriate transfer state if gmac gets a dl release cnf before connect
        ind from GL1. */
        transfer_data_ptr->t_flags.gmac_rcvd_dl_assignment_in_concurrency = TRUE;

        if (((packet_downlink_assignment_t*)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf();
        }            

        (void)mac_log_state();
      }

      break;

    case PACKET_UPLINK_ASSIGNMENT:
      if (gmac_tdynamic_handle_pua())
      {
        if (((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf(); 
        }
      }
      break;

    case PACKET_TIMESLOT_RECONFIGURE:
      if (gmac_tdynamic_handle_ptr())
      {
        gmac_tran_log_state_change(TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC, __LINE__);
        transfer_data_ptr->mac_transfer_substate = TRANSFER_CONCURRENT_DL_REASSIGN_DYNAMIC_REALLOC;

        if (((packet_timeslot_reconfigure_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf();
        }

        (void)mac_log_state();
      }
      break;

    case PACKET_PWR_CTRL_TMNG_ADV:
      gmac_transfer_handle_ppcta(TRUE);
      break;
      
    case PACKET_MEASUREMENT_ORDER:
      gmac_util_pmo_handler();
      break;
      
    case PACKET_CELL_CHANGE_ORDER:
      gmac_tran_pcco_handler(transfer_data_ptr->mac_transfer_substate);
      break;
      
    case PACKET_TBF_RELEASE:
      gmac_concurrent_transfer_handle_tbf_release();
      break;
      
    case PACKET_PDCH_RELEASE:
      gmac_tran_handle_pdch_release(((packet_pdch_release_t *)global_data_ptr->csn_dec_data_ptr)->timeslots_available,UL_AND_DL);
      break;
      
    case PACKET_POLLING_REQUEST:
      MSG_GERAN_LOW_0_G("MAC Rcvd PACKET_POLLING_REQUEST");
      gmac_idle_pkt_polling_request_handler();
      break;

    case PACKET_PAGING_REQUEST:
      MSG_GERAN_LOW_0_G("MAC Rcvd PACKET_PAGING_REQUEST");
      gmac_util_pkt_paging_on_pacch();
      break;

#ifdef FEATURE_GSM_EDTM
    case PACKET_CS_COMMAND:
      gmac_transfer_edtm_pkt_cs_cmd_handler();
      break;

    case PACKET_CS_RELEASE:
      tran_state_change = gmac_tran_edtm_cs_rel_ind_handler( transfer_data_ptr->mac_transfer_substate,
                                                             UL_AND_DL );

      if ( tran_state_change != transfer_data_ptr->mac_transfer_substate )
      {
        new_state_str = gmac_convert_transfer_substate_to_string(tran_state_change);

        MSG_SPRINTF_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "GMAC State tran = %s", new_state_str);        
      }

      transfer_data_ptr->mac_transfer_substate = tran_state_change;
      (void)mac_log_state();
    break;
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
    case PACKET_PS_HANDOVER:
      gmac_tran_psho_handler();
      break;
#endif/*FEATURE_GPRS_PS_HANDOVER*/

    default:
      MSG_GERAN_HIGH_2_G("MAC Rcvd MT %d EC %d",global_data_ptr->csn_dec_hdr_ptr->message_type,global_data_ptr->csn_dec_hdr_ptr->error_code);
      break;
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_concurrent_realloc_dl_reassign_data_ind 
===
===  DESCRIPTION
===  Handles received logical channel messages during uplink dynamic
===  packet transfer           
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header 
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_tdynamic_concurrent_realloc_dl_reassign_data_ind(void)
{
#ifdef FEATURE_GSM_EDTM
  gprs_mac_transfer_substates_t tran_state_change = TRANSFER_INACTIVE;
  char                         *new_state_str           = NULL;
#endif /*FEATURE_GSM_EDTM*/
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr();
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr();

  /* If message not recognised, send MOBILE_TBF_STATUS to network and ignore */
  if (global_data_ptr->csn_dec_hdr_ptr->error_code == UNKNOWN_MESSAGE_TYPE)
  {
    MSG_GERAN_LOW_0_G("MAC info CSN MT unknown");
    gmac_transfer_send_tbf_status(MOBILE_TBF_STATUS_PROTOCOL_ERROR_CAUSE, global_data_ptr->csn_dec_hdr_ptr->message_type);
    return;
  }

#ifdef FEATURE_GSM_DTM  
  if ( transfer_data_ptr->t_flags.dtm_realloc_in_progress )
  {
    MSG_GERAN_MED_1_G("GMAC reallocating DTM, PH_DATA_IND inhibited, msg_type:0x%x",global_data_ptr->csn_dec_hdr_ptr->message_type);
    return;
  }
#endif /*FEATURE_GSM_DTM*/
  
  switch (global_data_ptr->csn_dec_hdr_ptr->message_type)
  {
    case PACKET_UPLINK_ACK_NACK:
      gmac_tdynamic_handle_puan(TRUE);
      break;
    
    case PACKET_DOWNLINK_ASSIGNMENT:
      /* ignore until PTR complete */
      break;

    case PACKET_UPLINK_ASSIGNMENT:
      /* ignore until PTR complete */
      break;

    case PACKET_TIMESLOT_RECONFIGURE:
      if(gmac_tdynamic_handle_ptr())
      {
        if (((packet_timeslot_reconfigure_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
        {
          gmac_transfer_simulate_freq_cnf();
        }
      }
    break;

    case PACKET_PWR_CTRL_TMNG_ADV:
      gmac_transfer_handle_ppcta(TRUE);
      break;

    case PACKET_MEASUREMENT_ORDER:
      gmac_util_pmo_handler();
      break;
      
    case PACKET_CELL_CHANGE_ORDER:
      gmac_tran_pcco_handler(transfer_data_ptr->mac_transfer_substate);
      break;
      
    case PACKET_TBF_RELEASE:
      gmac_concurrent_transfer_handle_tbf_release();
      break;
      
    case PACKET_PDCH_RELEASE: 
      gmac_tran_handle_pdch_release(((packet_pdch_release_t *)global_data_ptr->csn_dec_data_ptr)->timeslots_available,UL_AND_DL);
      break;
      
    case PACKET_POLLING_REQUEST:
      MSG_GERAN_LOW_0_G("MAC Rcvd PACKET_POLLING_REQUEST");
      gmac_idle_pkt_polling_request_handler();
      break;

    case PACKET_PAGING_REQUEST:
      MSG_GERAN_LOW_0_G("MAC Rcvd PACKET_PAGING_REQUEST");
      gmac_util_pkt_paging_on_pacch();
      break;

#ifdef FEATURE_GSM_EDTM
    case PACKET_CS_COMMAND:
      gmac_transfer_edtm_pkt_cs_cmd_handler();
      break;

    case PACKET_CS_RELEASE:
      tran_state_change = gmac_tran_edtm_cs_rel_ind_handler( transfer_data_ptr->mac_transfer_substate,
                                                             UL_AND_DL );

      if ( tran_state_change != transfer_data_ptr->mac_transfer_substate )
      {
        new_state_str = gmac_convert_transfer_substate_to_string(tran_state_change);

        MSG_SPRINTF_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "GMAC State tran = %s", new_state_str);        
      }

      transfer_data_ptr->mac_transfer_substate = tran_state_change;
      (void)mac_log_state();
    break;
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
    case PACKET_PS_HANDOVER:
      gmac_tran_psho_handler();
      break;
#endif/*FEATURE_GPRS_PS_HANDOVER*/

    default:
      MSG_GERAN_HIGH_2_G("MAC Rcvd MT %d EC %d",global_data_ptr->csn_dec_hdr_ptr->message_type,global_data_ptr->csn_dec_hdr_ptr->error_code);
      break;
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_concurrent_transfer_handle_tbf_release()
===
===  DESCRIPTION
===  Handles PACKET TBF RELEASE for concurrent TBF
===
===  UL TBF
===  ------
===  If the cause is NORMAL the MS shall continue to the next LLC PDU 
===  boundary, starting the count down procedure.
===
===  If the cause is ABNORMAL the MS shall abort the uplink TBF and perform
===  an abnormal release with access retry. If a valid RRBP field is received
===  the MS shall send a PCA in the uplink radio block specified.
===
===  DL TBF
===  ------
===  The network may initiate abnormal release of a downlink TBF.
=== 
===  If a valid RRBP, MS shall transmit a PCA in the uplink radio 
===  block specified.
===
===  If there is no UL TBF, the MS shall return to packet idle. Otherwise
===  remain in UL packet transfer.
===
===  DEPENDENCIES
===  
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_concurrent_transfer_handle_tbf_release(void)
{
  rrbp_msg_t rrbp_msg;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr();
  packet_tbf_release_t *packet_rel_ptr = (packet_tbf_release_t *)global_data_ptr->csn_dec_data_ptr;
  gmac_transfer_data_t *transfer_data_ptr = gmac_get_transfer_data_ptr();
  
  if (gmac_util_global_tfi(packet_rel_ptr->global_tfi))
  {
    MSG_GERAN_MED_4_G("MAC Rcvd PACKET_TBF_RELEASE rel cause %d, UL:%d, DL:%d, test_mode %d",
      packet_rel_ptr->tbf_release_cause,
      packet_rel_ptr->uplink_release,
      packet_rel_ptr->dwnlink_release,
      global_data_ptr->current_test_mode);

    if ( global_data_ptr->current_test_mode == TEST_MODE_SRB )
    {
      /* If the SRB sub-mode is ON then L1 inhibits all control signals (inc. PCA's).
         In this state the NW may send a polled TBF release to terminate SRB mode.  To ensure that
         the PCA is sent the rrbp_msg is set to PCA_SRB to override the inhibit */

      rrbp_msg = PCA_SRB;

      /* The NW should release both UL and DL when in SRB mode.  If it does not
         then force UL+DL release to 1. */
      packet_rel_ptr->uplink_release = 1;
      packet_rel_ptr->dwnlink_release = 1;
    }
    else
    {
      rrbp_msg = PCA_DL_MSG;
    }

    rrbp_control_block_handler_2(rrbp_msg);
    
    if (packet_rel_ptr->tbf_release_cause == GMAC_PACKET_TBF_REL_NORMAL_CAUSE) 
    {
      /* Normal cause value */
      if (packet_rel_ptr->uplink_release)
      {
        gmac_util_send_ul_release_to_rlc(REL_UL_NORMAL); 
        mac_log_ul_tbf_release(MAC_LOG_UL_NORMAL_RELEASE);
        
      }
      /* downlink release is always abnormal!! */
      if (packet_rel_ptr->dwnlink_release)
      {
        gmac_util_send_dl_release_to_rlc();
        gmac_transfer_dl_release_clean();
        gmac_tran_send_mac_l1_release(DL_TBF_ABNORMAL);
        mac_log_dl_tbf_release(MAC_LOG_DL_TBF_RELEASE);        
      }
    }
    else
    {
      /* Abnormal cause value - Release TBF's specified in Packet TBF Release */
      /* Remain in transfer if any currently active TBF's are not specified by the Pkt TBF Rel*/

      if (packet_rel_ptr->uplink_release)
      {
        gmac_transfer_ul_dynamic_release_clean();
        gmac_util_block_rlc(NO_DEL, GMAC_TRAN_UL, __LINE__);
        mac_log_ul_tbf_release(MAC_LOG_UL_TBF_RELEASE);
      }

      if (packet_rel_ptr->dwnlink_release)
      {
        gmac_transfer_dl_release_clean();
        gmac_util_send_dl_release_to_rlc();
        mac_log_dl_tbf_release(MAC_LOG_DL_TBF_RELEASE);
      }

      if ( (packet_rel_ptr->uplink_release) &&
           (packet_rel_ptr->dwnlink_release) )
      {
        gmac_tran_send_mac_l1_release(UL_AND_DL_TBF_ABNORMAL);
        transfer_data_ptr->t_flags.abnormal_ul_and_dl_rel_flg = TRUE;
      }
      else if (packet_rel_ptr->uplink_release)
      {
        gmac_tran_send_mac_l1_release(UL_TBF_ABNORMAL); 
      }
      else if (packet_rel_ptr->dwnlink_release)
      {
        gmac_tran_send_mac_l1_release(DL_TBF_ABNORMAL); 
      }
    }
  }
}

/*** EOF: don't remove! ***/
