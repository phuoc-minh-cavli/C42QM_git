#ifndef SLPC_H
#define SLPC_H

/*=========================================================================
  FILE: slpc.h

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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
=========================================================================*/

/*==============================================================================

$Header: //components/rel/mcs.mpss/5.1/api/slpc.h#4 $

============================================================================*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include "comdef.h"
#include "DDITimetick.h"


/*============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typedefs,
and other items needed by this module.

============================================================================*/

/*----------------------------------------------------------------------------
  Sleep controller identifiers
----------------------------------------------------------------------------*/

typedef enum
{
  SLPC_INIT,
  #ifdef FEATURE_BUILD_PAGEONLY_IMAGE
  SLPC_LTE = SLPC_INIT,
  SLPC_NB1,
  #else
  SLPC_GSM = SLPC_INIT,
  SLPC_1X,
  SLPC_HDR,
  SLPC_WCDMA,
  SLPC_WCDMA2,
  SLPC_LTE,
  SLPC_TDSCDMA,
  SLPC_GSM_ALT,
  SLPC_GSM2 = SLPC_GSM_ALT,
  SLPC_GSM3,
  SLPC_NB1,
  #endif /* FEATURE_BUILD_PAGEONLY_IMAGE */
  SLPC_NUM_CLIENTS

} slpc_id_type;

typedef uint64 ustmr_type;
typedef uint64 tstmr_type;
typedef void (*slpc_event_callback_type) (void);
typedef void (*slpc_notify_cb_type)
  (slpc_id_type id, uint64 wakeup_tick, boolean update, boolean extension);
typedef void (*slpc_notify_wakeup_cb_type)
  (slpc_id_type id, uint64 wakeup_tick);

typedef struct
{
  uint64 time;
  uint64 phase;
  uint64 alt_time;
  uint64 alt_phase;
} slpc_tstmr_type;

typedef enum
{
  SLPC_ERR_NO_ERR,
  SLPC_ERR_WAKEUP_IN_PAST
} slpc_err_type;

typedef enum
{
  SLPC_STMR_VSTMR,
} slpc_stmr_mode_enum;

/******************************************************************************
  @brief SLPC_GET_STMR_MODE

  Return whether SLPC client uses HW TSTMR or virtual STMR (VSTMR).

  @param id: identifies the calling client
  @param mode: is client using VSTMR or HW STMR?
  @return slpc_stmr_mode_enum
******************************************************************************/
slpc_stmr_mode_enum slpc_get_stmr_mode( slpc_id_type id );

/******************************************************************************
  @brief SLPC_IS_ACTIVE

  Is sleep controller active for client (started/sleep/wakeup/warmup)?

  @param id: identifies the calling client
  @return TRUE if slpc active, else FALSE
******************************************************************************/
boolean slpc_is_active( slpc_id_type id );

/******************************************************************************
  @brief SLPC_START

  Get snapshot of current timer values for start of sleep cycle

  @param id: identifies the calling client
  @return start tick (TSTMR)
******************************************************************************/
slpc_tstmr_type slpc_start( slpc_id_type id );

/******************************************************************************
  @brief SLPC_GET_START_TSTMR

  Get start TSTMR value for alt tech

  @param id: identifies the calling client
  @return: alt tstmr
******************************************************************************/
slpc_tstmr_type slpc_get_start_tstmr( slpc_id_type id, slpc_id_type alt_id );

/******************************************************************************
  @brief SLPC_SET_DURATION
  Sets the sleep duration (in TSTMR ticks) and programs the wakeup timer

  @detail
  The warmup period is subtracted from the sleep duration (ending/OLS)
  to determine the wakeup point.

  @param id: identifies the calling client
  @param tstmr_dur: the duration of sleep requested, in tstmr ticks
  @return slpc_err_type
******************************************************************************/
slpc_err_type slpc_set_duration( slpc_id_type id, uint64 tstmr_dur_ts );

/******************************************************************************
  @brief SLPC_SET_NOTIFY_CALLBACK
  Set a callbacks for anyone wanting to track wakeup point changes,
  called whenever any tech sets a wakeup point

  @param n_cb: callback
  @return none
******************************************************************************/
void slpc_set_notify_callback( slpc_notify_cb_type n_cb );

/******************************************************************************
  @brief SLPC_SET_NOTIFY_WAKEUP_CALLBACK

  This callback will be called at any tech's wakeup event. For anyone
  wanting to track wakeups.

  @param id: n_cb: notify callback

  @return none.
******************************************************************************/
void slpc_set_notify_wakeup_callback( slpc_notify_wakeup_cb_type n_cb );

/******************************************************************************
  @brief SLPC_DEREGISTER_NOTIFY_CALLBACK

  Clear a previously set notify callback

  @param n_cb: callback
  @return none
******************************************************************************/
void slpc_deregister_notify_callback( slpc_notify_cb_type n_cb );

/******************************************************************************
  @brief SLPC_DEREGISTER_NOTIFY_WAKEUP_CALLBACK

  Deregister the wakeup callback, if previously set

  @param id: n_cb: notify callback

  @return none.
******************************************************************************/
void slpc_deregister_notify_wakeup_callback( slpc_notify_wakeup_cb_type n_cb );

/******************************************************************************
  @brief SLPC_SET_OLS_CALLBACK

  Set the callback to call when the slpc ending point (online start) is
  reached.

  Note this callback is not used with VSTMR, because there is no hardware
  to "slam", so everything is ready after complete_wakeup.

  @param id: identifies the calling client
  @param e_cb: event callback
  @return none
******************************************************************************/
void slpc_set_ols_callback( slpc_id_type id, slpc_event_callback_type e_cb );

/******************************************************************************
  @brief SLPC_SET_WAKEUP_CALLBACK

  Sets the function to call at the wakeup event. Wakeup event will happen
  at (ending point - warmup time). In this callback, wakeup can either be
  extended (set_duration), or commited (commit_wakeup, at which point it
  cannot be changed).

  @param id: identifies the calling client
  @param e_cb: event callback
  @return none
******************************************************************************/
void slpc_set_wakeup_callback( slpc_id_type id, slpc_event_callback_type w_cb );

/******************************************************************************
  @brief SLPC_SET_DURATION_AND_WARMUP
  Sets the sleep duration (in TSTMR ticks), the required warmup length,
  and programs the wakeup timer

  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in tstmr ticks
  @param dur_usec: warmup length, in usec
  @return slpc_err_type
******************************************************************************/
slpc_err_type slpc_set_duration_and_warmup( slpc_id_type id, uint64 tstmr_dur,
                                            uint16 dur_usec );

/******************************************************************************
  @brief SLPC_SET_WARMUP

  Set the warmup period. This determines amount of time prior to the
  ending point that the wakeup callback should occur.

  @param id: identifies the calling client
  @param warmup_usec: warmup period in usec
  @return slpc_err_type
******************************************************************************/
slpc_err_type slpc_set_warmup( slpc_id_type id, uint16 warmup_usec );

/******************************************************************************
  @brief SLPC_SUFFICIENT_WARMUP_TIME

  May be called by the wakeup_cb to determine whether or not the wakeup
  event is being processed too late.

  @param id: identifies the calling client
  @param min_warmup: min required warmup time (usec)
  @return boolean
******************************************************************************/
boolean slpc_sufficient_warmup_time ( slpc_id_type id, uint32 min_warmup );

/******************************************************************************
  @brief SLPC_COMPLETE_WAKEUP

  May be called by the wakeup_cb to commit to the wakeup if there is
  sufficient warmup time remaining.

  @param id: identifies the calling client
  @param min_warmup: min required warmup time (usec)
  @param tstmr: pointer to TSTMR for SLPC to program, or NULL if client
                will handle this
  @return FALSE if not enough time or no HW resources available
  @return TRUE if succeeded
******************************************************************************/
boolean slpc_complete_wakeup ( slpc_id_type id, uint32 min_warmup,
                               slpc_tstmr_type *tstmr );

/******************************************************************************
  @brief SLPC_ERROR_FEEDBACK

  Adjust the slow clock frequency estimate, based on the sleep duration
  and the reacquisition slews.

  @param id: identifies the calling client
  @param slew_err: slew error (TSTMR)
  @return none.
******************************************************************************/
void slpc_error_feedback( slpc_id_type id, int64 slew_err );

/******************************************************************************
  @brief SLPC_ERROR_FEEDBACK_EXT

  Adjust the slow clock frequency estimate, based on the sleep duration
  and the reacquisition slews.

  @param id: identifies the calling client
  @param slew_err: slew error (TSTMR)
  @return slew_err weighted by the XO shutdown duty cycle
******************************************************************************/
int32 slpc_error_feedback_ext( slpc_id_type id, int64 slew_err );

/******************************************************************************
  @brief SLPC_GET_WAKEUP_TICK

  Get wakeup interrupt point, in Qtimer ticks

  @param id: identifies the calling client

  @return absolute timetick of the wakeup point
******************************************************************************/
uint64 slpc_get_wakeup_tick( slpc_id_type id );

/******************************************************************************
  @brief SLPC_GET_OLS_TICK

  Get the absolute timetick of the ending interrupt (Qtimer)

  @param id: identifies the calling client

  @return absolute timetick of the online start point
******************************************************************************/
uint64 slpc_get_ols_tick( slpc_id_type id );

/******************************************************************************
  @brief SLPC_GET_WAKEUP_AND_OLS_TICK

  Get the absolute timeticks of the wakeup and online start points

  @param id: identifies the calling client

  @return absolute timetick of the wakeup point and OLS point
******************************************************************************/
void slpc_get_wakeup_and_ols_tick( slpc_id_type id, uint64 *wakeup_tick,
                                   uint64 *ols_tick );

/******************************************************************************
  @brief SLPC_GET_SYSCLK_COUNT

  Returns the number of TSTMR ticks the controller has been active for.

  @param id: identifies the calling client
  @return TSTMR ticks
******************************************************************************/
uint64 slpc_get_sysclk_count( slpc_id_type id );

/******************************************************************************
  @brief SLPC_ABORT

  Stop the current sleep cycle and reset the sleep controller state

  @param id: identifies the calling client
  @return none.
******************************************************************************/
void slpc_abort ( slpc_id_type id );

/******************************************************************************
  @brief SLPC_GET_TICK

  Get 64-bit absolute timetick

  @param none
  @return timetick
******************************************************************************/
uint64 slpc_get_tick ( void );

/******************************************************************************
  @brief slpc_enable_alt_id

  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code.

  @param id: identifies the calling client
  @return none.
******************************************************************************/
void slpc_enable_alt_id( slpc_id_type id, slpc_id_type alt_id,
                         slpc_tstmr_type *tstmr );

/******************************************************************************
  @brief slpc_disable_alt_id

  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code.

  @param id: identifies the calling client
         alt_id:  id's the additional tech that should no longer be slammed
  @see
  @return none.
******************************************************************************/
void slpc_disable_alt_id( slpc_id_type id, slpc_id_type alt_id );

/******************************************************************************
  @brief SLPC_SET_USE_XO_SLEEP_CNT

  Sets the flag for using XO sleep count to determine next wake up point

  @param id: identifies the calling client
  @param flag: TRUE or FALSE
  @return TRUE or FALSE
******************************************************************************/
boolean slpc_set_use_xo_sleep_cnt( slpc_id_type id, boolean flag );

/******************************************************************************
  @brief SLPC_GET_ACCUM_SLEW_ERR

  Returns the accumulated slew error for the specified technology

  @param id: identifies the calling client
  @param Pointer to return accumulated slew error in PPM - Q10 format
  @return TRUE  if slew error is successfully retrieved
          FALSE if slew error cannot be retrieved due to invalid slpc id.
******************************************************************************/
boolean slpc_get_accum_slew_err( slpc_id_type id, int32 *accum_err );

/******************************************************************************
  @brief SLPC_SET_ACCUM_SLEW_ERR

  Sets the accumulated slew error for the specified technology.
  Most common use err_value is expected to be 0 to reset the loop after
  long sleep.

  @param id: identifies the calling client
  @param err_value: error value in PPM - Q10 format
  @return TRUE  if setting the slew error is successful
          FALSE if slew error cannot be set due to invalid slpc id.
******************************************************************************/
boolean slpc_set_accum_slew_err( slpc_id_type id, int32 err_value );

/******************************************************************************
  @brief SLPC_SET_XO_TRIM_COMP_OFFSET

  Signals the TRIM offset to Core to compenssate for XO capacitance change
  in PMIC during sleep

  @param id: identifies the calling client
  @param offset: offset to apply to the current XO TRIM
  @return TRUE  if setting the offset is successful
          FALSE if offset cannot be set as specified
******************************************************************************/
boolean slpc_set_xo_trim_comp_offset( slpc_id_type id, int8 offset );

/******************************************************************************
  @brief SLPC_GET_XO_TRIM_COMP_OFFSET

  Retrieves the current TRIM offset configured in Core to compensate for XO
  capacitance change during sleep

  @return offset: offset being applied to the current XO TRIM

******************************************************************************/
int8 slpc_get_xo_trim_comp_offset( void );

#endif
