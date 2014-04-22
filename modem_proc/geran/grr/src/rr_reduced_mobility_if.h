#ifndef RR_REDUCED_MOBILITY_IF_H
#define RR_REDUCED_MOBILITY_IF_H

/*============================================================================
  @file rr_resel_if.h

  Signalling interface for the RR reselection module.

                Copyright (c) 2002, 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_reduced_mobility_if.h#2 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#if defined (FEATURE_GERAN_EXTENDED_DRX) || defined(FEATURE_GERAN_POWER_EFFICIENT_OPERATION) 
#include "rr_message_header.h"
#include "rr_defs.h"
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Input Messages
 *--------------------------------------------------------------------------*/

typedef enum
{
  RR_MM_PSM_EDRX_PARAMS_UPDATE,
  RR_MM_READY_TIMER_START,
  RR_MM_READY_TIMER_STOP,
  RR_DSC_INCREMENT,
  RR_DSC_DECREMENT
} rr_l1_update_reason_e;

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION

typedef enum
{
  RR_REDUCED_MOBILITY_CELL_SELECTION,
  RR_REDUCED_MOBILITY_MAX
} rr_reduced_mobility_start_reason_e;

typedef enum
{
  RR_C1_DELTA_AFTER_PLMN_SELECT,
  RR_C1_DELTA_AFTER_CELL_CHANGE,
  RR_C1_DELTA_AFTER_SCELL_NCELL_MEAS
} rr_c1_delta_update_reason_e;

typedef enum
{
  /*This signal is sent to the RR reduced mbility control  to indicate that
       reduced mobility ha started. */
  RR_RESEL_RMC_IMSG_START_IND,

  /*This signal is sent when  CCCH message is received */
  RR_RESEL_RMC_IMSG_CCCH_MSG_RCVD_IND,

  /*This signal is sent when  CS  req is made */
  RR_RESEL_RMC_IMSG_CS_DATA_REQ,  

  /*This signal is sent when  Paket  req is made */
  RR_RESEL_RMC_IMSG_PS_DATA_REQ,

  /*This signal is sent when  BSI acquition is complete */
  RR_RESEL_RMC_IMSG_BSI_ACQ_COMPLETE_IND,

  RR_RESEL_RMC_IMSG_MAX
} rr_resel_rmc_imsg_e;


typedef enum
{
  /* This signal is sent out the RR reduced mbility control  to indicate that
        a better cell needs to be found. */
  RR_RESEL_RMC_OMSG_FIND_CELL_IND,

  /*This signal is sent to inform that PS data can continue or not */
  RR_RESEL_RMC_OMSG_CS_DATA_CNF,  
  
  /*This signal is sent to inform that PS data can continue or not */
  RR_RESEL_RMC_OMSG_PS_DATA_CNF,

  RR_RESEL_RMC_MMSG_MAX
} rr_resel_rmc_omsg_e;


typedef struct
{
  rr_message_header_t          header;
  rr_reduced_mobility_start_reason_e reason;
} rr_resel_rmc_imsg_start_ind_t;

typedef struct
{
  rr_message_header_t          header;
  boolean                      rcc_changed;
  boolean                      peo_bcch_changed;
} rr_resel_rmc_imsg_ccch_msg_rcvd_ind_t;


typedef struct
{
  rr_message_header_t          header;
} rr_resel_rmc_imsg_ps_data_req_t;

typedef struct
{
  rr_message_header_t          header;
} rr_resel_rmc_imsg_bsi_acq_complete_ind_t;


typedef union
{
  rr_message_header_t          header;
} rr_resel_rmc_omsg_u;

typedef union
{
  rr_message_header_t           header;
  rr_resel_rmc_imsg_start_ind_t rmc_start_ind;
  rr_resel_rmc_imsg_ps_data_req_t ps_data_req;
  rr_resel_rmc_imsg_ccch_msg_rcvd_ind_t ccch_msg_ind;
  rr_resel_rmc_imsg_bsi_acq_complete_ind_t bsi_acq_cmplt_ind;
} rr_resel_rmc_imsg_u;


/* Union of all messages. */
typedef union
{
  rr_resel_rmc_imsg_u imsg;
  rr_resel_rmc_omsg_u omsg;
} rr_resel_rmc_msg_u;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

extern const char * rr_rmc_imsg_name(const rr_resel_rmc_imsg_e imsg);

extern const char * rr_rmc_omsg_name(const rr_resel_rmc_omsg_e omsg);

extern void rr_resel_rmc_send_imsg_start_ind(rr_reduced_mobility_start_reason_e reason  );

extern void rr_resel_rmc_send_imsg_ps_data_req( );

extern void rr_resel_rmc_send_imsg_cs_data_req( );


extern void rr_resel_rmc_send_imsg_ccch_msg_rcvd_ind(boolean rcc_changed, boolean peo_bcch_changed  );

extern void rr_resel_rmc_send_imsg_bsi_acquition_complete_ind( );

/*OMSG prototype */

extern void rr_resel_rmc_send_omsg_find_cell_ind( );

extern void rr_resel_rmc_send_omsg_ps_data_cnf( );

extern void rr_resel_rmc_send_omsg_cs_data_cnf( );

#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

#endif /* FEATURE_GERAN_EXTENDED_DRX || FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/
#endif /* #ifndef RR_REDUCED_MOBILITY_IF_H */

/* EOF */

