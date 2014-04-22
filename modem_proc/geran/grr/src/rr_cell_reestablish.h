#ifndef RR_CELL_REESTABLISH_H
#define RR_CELL_REESTABLISH_H

/*! \file rr_cell_reestablish.h
 
  This module implements an optimised cell selection process, for the purpose of
  call re-establishment following Radio Link Failure
 
                Copyright (c) 2007-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_cell_reestablish.h#2 $ */
/* $DateTime: 2018/10/10 02:31:27 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_GERAN_CS_SUPPORT
#include "rr_task.h"
#include "rr_gprs_defs.h"
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

/*!
 * \brief Returns the dedicated channel information in use before the Radio Link Failure.
 * 
 * \param channel_type_1 
 * \param channel_type_2 
 * 
 * \return uint8 - number of dedicated channels
 */
extern uint8 rr_cell_reest_get_channel_info(
  channel_type_T *channel_type_1,
  channel_type_T *channel_type_2
);

/*!
 * \brief When called, makes a copy of the current dedicated channel information.
 */
extern void rr_cell_reest_rlf_occurred( );

/*!
 * \brief The RR-CELL-REESTABLISH state machine.
 * 
 * \param reest_event 
 * \param new_message 
 * 
 * \return rr_event_T 
 */
extern rr_event_T rr_cell_reest_control(
  rr_event_T reest_event,
  rr_cmd_bdy_type *new_message
);

/*!
 * \brief Initialisation function. Called once at task start.
 */
extern void rr_cell_reest_task_start_init(
  void
);
#endif /* FEATURE_GERAN_CS_SUPPORT */

#endif /* #ifndef RR_CELL_REESTABLISH_H */

/* EOF */

