#ifndef RR_DTM_H
#define RR_DTM_H

/** @file */

/*============================================================================
  @file rr_dtm.h

  This is the header file for the RR-DTM module

                Copyright (c) 2005-2015 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_dtm.h#2 $ */
/* $DateTime: 2018/10/10 02:31:27 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_DTM

#include "comdef.h"
#include "rr_conn_release.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief Called when a OTA message arrives from L2 with a GTTP Protocol
         Discriminator. The encapsulated LLC PDU container is passed to GLLC

  @param dl_data_ind 
 */
extern void rr_dtm_gttp_message_received(
  dl_data_ind_T *dl_data_ind
);

/**
  @brief The main RR-DTM module state machine

  @param event_in 
  @param message_ptr 
  
  @return rr_event_T 
 */
extern rr_event_T rr_dtm_control(
  rr_event_T event_in,
  void *message_ptr
);

/**
  @brief DTM initialisation function. Should be called once at RR task-start.
 */
extern void rr_dtm_init(void );

#ifdef FEATURE_WCDMA
/**
  @brief Called when a W2G handover is in progress. This functions sets up the 
         PS data path to potentially go into PS transfer if the new cell
         supports DTM

  @param ps_attached 
 */
extern void rr_dtm_w2g_ho(
  boolean ps_attached
);
#endif // FEATURE_WCDMA

/**
  @brief Notifies rr_dtm_control of a CS establishment in progress
 */
extern void rr_dtm_send_imsg_cs_est_ind( void );

/**
  @brief Notifies rr_dtm_control of a CS connection established
 */
extern void rr_dtm_send_imsg_cs_conn_ind(void );

/**
  @brief Notifies rr_dtm_control of a CS release
 */
extern void rr_dtm_send_imsg_cs_rel_ind(void );

/**
  @brief Notifies rr_dtm_control of a L2 ack for a GTTP transfer
 */
extern void rr_dtm_send_imsg_l2_gttp_ack_ind( void );

/**
  @brief This function decodes the contents of a DTM ASSIGNMENT COMMAND message. 
         See 44.018 9.1.12e DTM Assignment Command 

  @param band_ptr       output pointer to band information from channel
                        description
  @param csps_required  indication of whether CS or PS allocations must be 
                        present
  
  @return RR cause value indicating any error (RR_NORMAL_EVENT if no error)
 */
extern RR_cause_T rr_dtm_process_dtm_assignment_command(
  sys_band_T *band_ptr,
  dtm_validation_t csps_required
);

/**
  @brief Called when L1 confirms a DTM or Packet Assignment

  @param mac_status 
 */
extern void rr_dtm_dual_transfer_confirmed(
  rr_mac_status_t mac_status
   
);

/**
  @brief This function copies the channel configuration information contained in 
         a received DTM ASSIGNMENT COMMAND message to RR's internal structures,
         and configures the vocoder (sends RR_SYNC_IND)
 */
extern void rr_dtm_start_using_new_channel_spec( void );

#ifdef FEATURE_GSM_EDTM
/**
  @brief Validate a DTM Assignment for EDTM establishment and sends it to GMAC 
         (in a GRR_MAC_DTM_CHANNEL_ASSIGN_REQ)
  
  @return T EV_DTM_ASSIGNMENT_ERROR is the CS assignment is invalid; EV_NO_EVENT 
          otherwise
 */
extern rr_event_T rr_dtm_send_edtm_assignment_to_gmac( void);
#endif /* FEATURE_GSM_EDTM */

/**
  @brief Discards all operational data
 */
extern void rr_dtm_discard_data(void );

/**
  @brief Checks is RR-DTM-CONTROL is in a suitable state to handle EV_DTM_NO_PS_ACCESS_REQ
 
  @return TRUE is the message can be handled; FALSE otherwise
 */ 
extern boolean rr_dtm_can_process_no_ps_access_req(void );

/**
  @brief Checks is RR-DTM-CONTROL is in a suitable state to handle EV_DTM_PS_ACCESS_REQ
 
  @return TRUE is the message can be handled; FALSE otherwise
 */ 
extern boolean rr_dtm_can_process_ps_access_req(void );

/**
  @brief Checks is RR-DTM-CONTROL is in a suitable state to handle EV_DTM_MSC_CHANGE_IND
 
  @return TRUE is the message can be handled; FALSE otherwise
 */ 
extern boolean rr_dtm_can_process_msc_change_ind(void );

/**
  @brief Checks is RR-DTM-CONTROL is in a suitable state to handle RR_DTM_IMSG_CS_RELEASE_REQ
 
  @return TRUE is the message can be handled; FALSE otherwise
 */ 
extern boolean rr_dtm_can_process_cs_release_req( void );

/**
  @brief Initialisation function to be called at task-start
 */
extern void rr_dtm_task_start_init(
  void
);

#endif /* FEATURE_GSM_DTM */

#endif /* #ifndef RR_DTM_H */

/* EOF */

