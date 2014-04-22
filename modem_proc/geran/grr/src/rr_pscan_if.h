#ifndef RR_POWER_SCAN_IF_H
#define RR_POWER_SCAN_IF_H

/*! \file rr_power_scan_if.h 
 
  Signalling interface for the RR-PSCAN module.
 
                Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_pscan_if.h#2 $ */
/* $DateTime: 2018/10/10 02:31:27 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_message_header.h"
#include "rr_defs.h"
#include "rr_gprs_defs.h"
#include "geran_dual_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/* Input messages. */
typedef enum
{
  RR_PSCAN_IMSG_START_IND,
  RR_PSCAN_IMSG_ABORT_REQ,
  RR_PSCAN_IMSG_COUNT
} rr_pscan_imsg_e;

typedef struct
{
  rr_message_header_t header;
  rr_search_mode_T search_mode;
  sys_band_mask_type band_pref;  
} rr_pscan_start_ind_t;

typedef struct
{
  rr_message_header_t header;
} rr_pscan_abort_req_t;

typedef union
{
  rr_message_header_t header;
  rr_pscan_start_ind_t pscan_start_ind;
  rr_pscan_abort_req_t pscan_abort_req;
} rr_pscan_imsg_u;

/* Output messages. */
typedef enum
{
  RR_PSCAN_OMSG_FINISHED_IND,
  RR_PSCAN_OMSG_ABORT_CNF,
  RR_PSCAN_OMSG_COUNT
} rr_pscan_omsg_e;

typedef struct
{
  rr_message_header_t header;
  boolean l1_suspended;
} rr_pscan_finished_ind_t;

typedef struct
{
  rr_message_header_t header;
} rr_pscan_abort_cnf_t;

typedef union
{
  rr_message_header_t header;
  rr_pscan_finished_ind_t pscan_finished_ind;
  rr_pscan_abort_cnf_t pscan_abort_cnf;  
} rr_pscan_omsg_u;

/* Union of all messages. */
typedef union
{
  rr_pscan_imsg_u imsg;
  rr_pscan_omsg_u omsg;
} rr_pscan_msg_u;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/* Input messages. */

/*!
 * \brief Sends RR_PSCAN_IMSG_START_IND.
 * 
 * \param search_mode 
 * \param band_pref 
 */
extern void rr_pscan_send_start_ind(
  rr_search_mode_T search_mode,
  sys_band_mask_type band_pref
   
);

/*!
 * \brief Sends RR_PSCAN_IMSG_ABORT_REQ.
 * 
 */
extern void rr_pscan_send_abort_req(
   
);

/* Output messages. */

/*!
 * \brief Sends RR_PSCAN_OMSG_FINISHED_IND.
 * 
 * \param l1_suspended (in) - TRUE if L1 suspended following completion of the pscan, FALSE otherwise
 */
extern void rr_pscan_send_finish_ind(
  boolean l1_suspended
   
);

/*!
 * \brief Sends RR_PSCAN_OMSG_ABORT_CNF.
 * 
 */
extern void rr_pscan_send_abort_cnf(
   
);

#endif /* #ifndef RR_POWER_SCAN_IF_H */

/* EOF */

