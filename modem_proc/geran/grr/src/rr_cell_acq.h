#ifndef RR_CELL_ACQ_H
#define RR_CELL_ACQ_H

/*============================================================================
  @file rr_cell_acq.h

  Signalling interface for the RR Cell Acquisition module.

                Copyright (c) 2002, 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_cell_acq.h#2 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "rr_events.h"
#include "rr_task.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

/**
  @brief Determine whether RR cell acquisition state machine is suspending MAC.
 
  @param 

  @return TRUE if RR cell acquisition state machine is suspending MAC, FALSE
          otherwise.
*/
extern boolean rr_cell_acq_pending_mac_suspension( void );

/**
  @brief RR cell acquisition state machine.

  This is the RR cell acquisition state machine.

  @param event_in: An input event.
  @param message:  Optional input message.
*/
extern void rr_cell_acq_control(rr_event_T event_in, rr_cmd_bdy_type *message );

#endif /* RR_CELL_ACQ_H */

