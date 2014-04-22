#ifndef RR_L1_IDLE_MODE_H
#define RR_L1_IDLE_MODE_H

/*============================================================================
  rr_l1_idle_mode.h

  This is the header file for the RR-L1-IDLE module.

                Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_l1_idle_mode.h#2 $ */
/* $DateTime: 2018/10/10 02:31:27 $ */

/** 
  @file rr_l1_idle.h
  @brief This is the header file for the RR-L1-IDLE module
 */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "rr_task.h"
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
  @brief Returns the L1 idle mode (GSM or CCCH)
*/ 
extern grr_l1_idle_mode_t grr_determine_l1_idle_mode(
  gprs_scell_info_t *cell_info
);

/**
  @brief Returns TRUE if RR-L1-IDLE-CONTROL is ready to process RR_L1_IDLE_IMSG_START_IDLE_REQ
*/ 
extern boolean rr_l1_idle_can_process_start_idle_req( );


/**
  @brief Returns TRUE if RR-L1-IDLE-CONTROL is in state RR_L1_STATE_START_IDLE
*/ 
extern boolean rr_is_l1_idle_state_start_idle( );


/**
  @brief Main RR-L1-IDLE-CONTROL state machine
*/ 
extern void rr_l1_idle_control(
  const rr_cmd_bdy_type *msg_ptr
   
);

/**
  @brief Initialisation function. Resets internal variables and the state machine
*/ 
extern void rr_l1_idle_init( );

/**
  @brief Initialisation function. Should be called once at RR task start 
   
  Note: The primary purpose of this function is to initialise pointers to control/data structures.
*/ 
extern void rr_l1_idle_task_start_init(void);


#endif /* #ifndef RR_L1_IDLE_MODE_H */

