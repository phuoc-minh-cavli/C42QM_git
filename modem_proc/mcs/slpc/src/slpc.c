/*=========================================================================
  FILE: slpc.c

  OVERVIEW: Sleep controller module for maintaining tech timing during
            sleep.

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

$Header: //components/rel/mcs.mpss/5.1/slpc/src/slpc.c#15 $


============================================================================*/


/*============================================================================

                           INCLUDE FILES

============================================================================*/
#include <hexagon_protos.h>
#include <rcinit.h>
#include <qurt_mutex.h>
#include <qurt_anysignal.h>
#include <amssassert.h>
#include <DALSys.h>
#include <DALStdErr.h>
#include <DALDeviceId.h>
#include <DalDevice.h>
#include <DDIInterruptController.h>
#include <DDIChipInfo.h>
#include <err.h>
#include <msg.h>
#include <timer.h>
#include "tcxomgr.h"
#include "slpc.h"
#include "slpci.h"
#include "rpmclient.h"
#include "pm_xo_core.h"

#include "vstmr.h"
#ifndef FEATURE_BUILD_PAGEONLY_IMAGE
#include "vstmr_1x.h"
#include "vstmr_hdr.h"
#include "vstmr_geran.h"
#include "vstmr_wcdma.h"
#include "vstmr_tds.h"
#endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */
#include "vstmr_lte.h"
#include "vstmr_nb1.h"


/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typedefs,
and other items needed by this module.

============================================================================*/

/*----------------------------------------------------------------------------
  Global constants
----------------------------------------------------------------------------*/

#ifndef FEATURE_BUILD_PAGEONLY_IMAGE
#define SLPC_GSM_SYS_FREQ           ( 1625000uLL * 8uLL )
#define SLPC_1X_SYS_FREQ            ( 1228800uLL * 8uLL )
#define SLPC_HDR_SYS_FREQ           ( 1228800uLL * 8uLL )
#define SLPC_WCDMA_SYS_FREQ         ( 3840000uLL * 8uLL )
#define SLPC_TDSCDMA_SYS_FREQ       ( 1280000uLL * 8uLL )
#define SLPC_GSM_ALT_SYS_FREQ       ( 1625000uLL * 8uLL )
#endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */
#define SLPC_LTE_SYS_FREQ           ( 3840000uLL * 8uLL )
#define SLPC_NB1_ALT_SYS_FREQ       ( 1920000uLL )

#ifndef FEATURE_BUILD_PAGEONLY_IMAGE
#define SLPC_GSM_K                  ( 23 )
#define SLPC_1X_K                   ( 21 )
#define SLPC_HDR_K                  ( 21 )
#define SLPC_WCDMA_K                ( 25 )
#define SLPC_TDSCDMA_K              ( 23 )
#define SLPC_GSM_ALT_K              ( 23 )
#endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */
#define SLPC_LTE_K                  ( 28 )
#define SLPC_NB1_ALT_K              ( 28 )

/* maximum allowed error to accumulate for tracking (3ppm)*/
#define SLPC_MAX_ACCUM_ERR          ( 3 * 1024 )

/* Signal that can be sent to slpc task */
#define SLPC_WAKEUP_TIMER_EXP_SIG   ( 0x01 )
#define SLPC_ALL_SIGS               ( SLPC_WAKEUP_TIMER_EXP_SIG )

/* lower part of timetick captured in ustmr count */
#define SLPC_TIMETICK_LOWER_MASK    ( 0x0000000007FFFFFFuLL )

/* max number of clients that can register for wakeup changes */
#define SLPC_MAX_WAKEUP_CALLBACKS   ( SLPC_NUM_CLIENTS )

/*
  Blast OS can return timer callbacks up to 100 usec early, so that error
  must be accounted for when sanity checking the wakeup timer.
  Double the buffer for safety.
*/
/* 200 usec = 3840 xo ticks */
#define SLPC_WAKEUP_TIMER_ERR       ( 3840 )

#define INIT_TECH_CRIT_SECTION(id) \
  if (DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, \
                     &slpc[id].sync_ptr, \
                     &slpc[id].sync_obj)) \
  { \
    ERR_FATAL("sleepctl_init: DALSYS_SyncCreate failed!", 0, 0, 0); \
  }
#define ENTER_TECH_CRIT_SECTION(id) DALSYS_SyncEnter(slpc[id].sync_ptr)
#define LEAVE_TECH_CRIT_SECTION(id) DALSYS_SyncLeave(slpc[id].sync_ptr)

#define INIT_GLOBAL_CRIT_SECTION()  qurt_mutex_init(&slpc_data.slpc_lock)
#define ENTER_GLOBAL_CRIT_SECTION() qurt_mutex_lock(&slpc_data.slpc_lock)
#define LEAVE_GLOBAL_CRIT_SECTION() qurt_mutex_unlock(&slpc_data.slpc_lock)

/* absolute value macro */
#define  ABS( x ) ( ((x) < 0) ? -(x) : (x) )

/* USTMR theshold for considering a wakeup potentially rude (30ms) */
#define SLPC_RUDE_WAKEUP_THRESH 576000

typedef union
{
  uint64 ud;
  uint32 uw[2];
}
slpc_dword_type;

/*----------------------------------------------------------------------------
  Sleep controller state
----------------------------------------------------------------------------*/

typedef enum
{
  /* Sleep controller in not active */
  SLPC_INACTIVE_STATE,

  /* Sleep controller has captured start point */
  SLPC_START_STATE,

  /* Sleep controller is counting slow clocks until the "wakeup" point */
  SLPC_SLEEP_STATE,

  /* Wakeup point has been reached.  Next: Warmup, or back to sleep? */
  SLPC_WAKEUP_STATE,

}
slpc_state_enum;


/*----------------------------------------------------------------------------
  Sleep start data
----------------------------------------------------------------------------*/

typedef struct
{
  /* USTMR value at read sync */
  ustmr_type                      ustmr_ts;

  /* TSTMR value at read sync */
  slpc_tstmr_type                 tstmr;

  /* timetick value at read sync */
  uint64                          tick;
}
slpc_start_type;


/*----------------------------------------------------------------------------
  Warmup data
----------------------------------------------------------------------------*/

typedef struct
{
  /* Warmup duration, in microseconds (Max = 65.535ms) */
  uint16                          dur_usec;

  /* Warmup duration, in timeticks */
  uint64                          dur_tick;
}
slpc_warmup_type;


/*----------------------------------------------------------------------------
  Wakeup data
----------------------------------------------------------------------------*/

typedef struct
{
  /* Wakeup event start timestamp, in XO units */
  uint64                          cb_start_tick;

  /* Wakeup event end timestamp, in XO units */
  uint64                          cb_end_tick;

  /* timetick value at wakeup */
  uint64                          tick;

  /* timetick value of last wakeup */
  uint64                          last;

  /* Timer group to be defined to non-deferrable */
  timer_group_type                slpc_timer_group;

  /* Timer used to generate wakeup event */
  timer_type                      timer;

  /* Wakeup event notification callback */
  slpc_event_callback_type        cb;

  /* Number of times we recorded invalid wakeup event */
  uint8                          invalid_cnt;

  /* Indicates if wakeup point is an update of an earlier wakeup */
  boolean                         update;

  /* Indicates if wakeup point is an extension of an earlier wakeup */
  boolean                         extension;

  /* Indicates wakeup is a possible rude wakeup */
  boolean                         rude;
}
slpc_wakeup_type;


/*----------------------------------------------------------------------------
  Sleep ending data
----------------------------------------------------------------------------*/

typedef struct
{
  /* ustmr value at ending point */
  ustmr_type                      ustmr_ts;

  /* timetick value at ending point */
  uint64                          tick;

  /* Sleep ending event notification callback */
  slpc_event_callback_type        cb;
}
slpc_ending_type;



/*----------------------------------------------------------------------------
  Last sleep data - used for slpc_error_feedback
----------------------------------------------------------------------------*/

typedef struct
{
  /* Last sleep duration, in technology STMR ticks */
  tstmr_type                      duration_tstmr;

  /* Last sleep duration, in universal STMR ticks */
  ustmr_type                      duration_ustmr;

  /* XO ticks during the last sleep duration */
  uint64                          xo_sleep_cnt;
}
slpc_last_sleep_type;

typedef struct
{
  /* the time value of this tstmr at start time transfer point (in tech stmr
     resolution) */
  uint64                               start_time;

  /* the time value of this tstmr at start time transfer point (in tech rtc
     resolution) */
  uint64                               start_time_in_rtc;

  /* the phase value of this tstmr at start time transfer point */
  uint64                               start_phase;

  /* value to load in alternate tech stmr at online pulse */
  uint64                               tstmr;

  /* flag to enable alternate stmr activation on online pulse */
  boolean                              valid;

  /* flag to enable alternate stmr activation on online pulse */
  boolean                              load_tstmr;

}
slpc_alt_id_type;

typedef struct
{
  /* Maps thread with an index in the pool and also determines total number
     of threads in pool post RCINIT. */
  atomic_word_t  thread_idx;

  /* Mask of active threads at any given time in the pool. */
  atomic_word_t  active_mask;

  /* Thread Id to SLPC client mapping array. */
  slpc_id_type  thread_to_client_map[SLPC_NUM_THREADS];

  /* Signal array each corresponding to one slpc thread. */
  qurt_signal_t  task_sig[SLPC_NUM_THREADS];
} slpc_thread_pool_s;

/* SLPC thread pool structure. */
static  slpc_thread_pool_s  slpc_thread_pool;

/*----------------------------------------------------------------------------
  tstmr slam function
----------------------------------------------------------------------------*/
typedef void (*slpc_vstmr_tstmr_slam_type)( slpc_id_type pri_id, uint64 dur );

/*----------------------------------------------------------------------------
  Forward declarations
----------------------------------------------------------------------------*/
#ifndef FEATURE_BUILD_PAGEONLY_IMAGE
static void slpc_gsm1_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);
static void slpc_gsm2_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);
static void slpc_gsm3_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);
static void slpc_1x_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);
static void slpc_hdr_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);
static void slpc_wcdma_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);
static void slpc_wcdma2_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);
static void slpc_tdscdma_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);
#endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */
static void slpc_lte_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);
static void slpc_nb1_vstmr_tstmr_slam(slpc_id_type pri_id, uint64 dur);

/*----------------------------------------------------------------------------
  Per technology data
----------------------------------------------------------------------------*/

typedef struct
{
  /* Tech system frequency */
  uint64                               system_freq;

  /* Accumulated slew error */
  int64                                slew_err_accum;

  /* Unfiltered accumulated slew error This is what SLPC tracks if
     XO sleep drift was turned OFF */
  int64                                unfiltered_slew_err_accum;

  /* Total sleep duration, in USTMR ticks */
  uint64                               ustmr_dur;

  /* Total sleep duration, in TSTMR ticks */
  uint64                               tstmr_dur;

  /* XO ticks during the current sleep duration */
  uint64                               xo_sleep_cnt;

  /* Start of sleep data */
  slpc_start_type                      start;

  /* RF warmup time constants */
  slpc_warmup_type                     warmup;

  /* Wakeup event data */
  slpc_wakeup_type                     wakeup;

  /* End of sleep data */
  slpc_ending_type                     ending;

  /* Last sleep duration, for error_feedback */
  slpc_last_sleep_type                 last_sleep;

  /* List of other techs where timing must be maintained */
  slpc_alt_id_type                     alt_techs[SLPC_NUM_CLIENTS];

  /* per-tech lock object to allow maximum wakeup parallelism */
  DALSYSSyncObj                        sync_obj;

  /* per-tech lock handle to allow maximum wakeup parallelism */
  DALSYSSyncHandle                     sync_ptr;

  /* Sleep controller identifier */
  const char *                         id;

  /* Slam function for this client */
  slpc_vstmr_tstmr_slam_type           vstmr_tstmr_slam_func;

  /* TCXO Manager id's required for retrieving tech specific RGS data */
  tcxomgr_client_info_struct_type      tm_client;

  /* Software state */
  slpc_state_enum                      state;

  /* Scaling needed in dyn gain err feedback */
  uint8                                k;

  /* Flag to determine whether to use XO shutdown
     duty cycle to estimate next wakeup */
  boolean                              use_xo_sleep_cnt;
}
slpc_client_type;

static slpc_client_type slpc[SLPC_NUM_CLIENTS] =
{
#ifndef FEATURE_BUILD_PAGEONLY_IMAGE
  { .id = "GSM", .state = SLPC_INACTIVE_STATE, .system_freq = SLPC_GSM_SYS_FREQ, .k = SLPC_GSM_K,
    .tm_client = { TCXOMGR_CLIENT_GSM, TCXOMGR_AS_ID_1 }, .vstmr_tstmr_slam_func = slpc_gsm1_vstmr_tstmr_slam},

  { .id = "1X", .state = SLPC_INACTIVE_STATE, .system_freq = SLPC_1X_SYS_FREQ, .k = SLPC_1X_K,
    .tm_client = { TCXOMGR_CLIENT_CDMA_1X, TCXOMGR_AS_ID_1 }, .vstmr_tstmr_slam_func = slpc_1x_vstmr_tstmr_slam},

  { .id = "HDR", .state = SLPC_INACTIVE_STATE, .system_freq = SLPC_HDR_SYS_FREQ, .k = SLPC_HDR_K,
    .tm_client = { TCXOMGR_CLIENT_CDMA_HDR, TCXOMGR_AS_ID_1 }, .vstmr_tstmr_slam_func = slpc_hdr_vstmr_tstmr_slam},

  { .id = "WCDMA", .state = SLPC_INACTIVE_STATE, .system_freq = SLPC_WCDMA_SYS_FREQ, .k = SLPC_WCDMA_K,
    .tm_client = { TCXOMGR_CLIENT_WCDMA, TCXOMGR_AS_ID_1 }, .vstmr_tstmr_slam_func = slpc_wcdma_vstmr_tstmr_slam},

  { .id = "WCDMA2", .state = SLPC_INACTIVE_STATE, .system_freq = SLPC_WCDMA_SYS_FREQ, .k = SLPC_WCDMA_K,
    .tm_client = { TCXOMGR_CLIENT_WCDMA, TCXOMGR_AS_ID_2 }, .vstmr_tstmr_slam_func = slpc_wcdma2_vstmr_tstmr_slam},

  { .id = "LTE", .state = SLPC_INACTIVE_STATE, .system_freq = SLPC_LTE_SYS_FREQ, .k =  SLPC_LTE_K,
    .tm_client = { TCXOMGR_CLIENT_LTE, TCXOMGR_AS_ID_1 }, .vstmr_tstmr_slam_func = slpc_lte_vstmr_tstmr_slam},

  { .id = "TDSCDMA", .state = SLPC_INACTIVE_STATE, .system_freq = SLPC_TDSCDMA_SYS_FREQ, .k =  SLPC_TDSCDMA_K,
    .tm_client = { TCXOMGR_CLIENT_TDSCDMA, TCXOMGR_AS_ID_1 }, .vstmr_tstmr_slam_func = slpc_tdscdma_vstmr_tstmr_slam},

  { .id = "GSM2", .state = SLPC_INACTIVE_STATE, .system_freq = SLPC_GSM_ALT_SYS_FREQ,  .k =  SLPC_GSM_ALT_K,
    .tm_client = { TCXOMGR_CLIENT_GSM, TCXOMGR_AS_ID_2 }, .vstmr_tstmr_slam_func = slpc_gsm2_vstmr_tstmr_slam},

  { .id = "GSM3", .state = SLPC_INACTIVE_STATE, .system_freq = SLPC_GSM_ALT_SYS_FREQ,  .k =  SLPC_GSM_ALT_K,
    .tm_client = { TCXOMGR_CLIENT_GSM, TCXOMGR_AS_ID_3 }, .vstmr_tstmr_slam_func = slpc_gsm3_vstmr_tstmr_slam, },

  { .id = "NB1", .state = SLPC_INACTIVE_STATE, .system_freq = SLPC_NB1_ALT_SYS_FREQ,  .k =  SLPC_NB1_ALT_K,
    .tm_client = { TCXOMGR_CLIENT_NB1, TCXOMGR_AS_ID_1 }, .vstmr_tstmr_slam_func = slpc_nb1_vstmr_tstmr_slam}
#else
  { .id = "LTE", .state = SLPC_INACTIVE_STATE, .system_freq = SLPC_LTE_SYS_FREQ, .k =  SLPC_LTE_K,
    .tm_client = { TCXOMGR_CLIENT_LTE, TCXOMGR_AS_ID_1 }, .vstmr_tstmr_slam_func = slpc_lte_vstmr_tstmr_slam},

  { .id = "NB1", .state = SLPC_INACTIVE_STATE, .system_freq = SLPC_NB1_ALT_SYS_FREQ,  .k =  SLPC_NB1_ALT_K,
    .tm_client = { TCXOMGR_CLIENT_NB1, TCXOMGR_AS_ID_1 }, .vstmr_tstmr_slam_func = slpc_nb1_vstmr_tstmr_slam}
#endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */
};

typedef struct
{
#ifndef FEATURE_BUILD_PAGEONLY_IMAGE
  /* gsm1 technology view */
  vstmr_geran_view_s *gsm_view1;

  /* gsm2 technology view */
  vstmr_geran_view_s *gsm_view2;

  /* gsm3 technology view */
  vstmr_geran_view_s *gsm_view3;

  /* 1x technology view */
  vstmr_1x_view_s *onex_view;

  /* wcdma technology view */
  vstmr_wcdma_view_s *wcdma_view;

  /* wcdma2 technology view */
  vstmr_wcdma_view_s *wcdma_view2;

  /* hdr technology view */
  vstmr_hdr_view_s *hdr_view;

  /* tdscdma technology view */
  vstmr_tds_view_s *tdscdma_view;
#endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */

  /* lte technology view */
  vstmr_lte_view_s *lte_view;

  /* nb1 technology view */
  vstmr_nb1_view_s *nb1_view;
}
slpc_vstmr_data_type;

/*----------------------------------------------------------------------------
  Overall data
----------------------------------------------------------------------------*/

typedef struct
{

  /* for atomic sections within slpc module */
  qurt_mutex_t slpc_lock;

  /* callbacks for anyone wanting to track wakeup point changes */
  slpc_notify_cb_type notify_cb[SLPC_MAX_WAKEUP_CALLBACKS];

  /* callbacks for anyone wanting wakeup event notifications */
  slpc_notify_wakeup_cb_type notify_wakeup_cb[SLPC_MAX_WAKEUP_CALLBACKS];

  /* handle used for timetick services */
  DalDeviceHandle *tt_handle;

  /* handles and other data required in vstmr interface */
  slpc_vstmr_data_type vstmr_data;

}
slpc_type;

static slpc_type slpc_data;

typedef struct
{
  slpc_id_type   id;
  int8           offset;
}slpc_xo_comp_info_type;

static slpc_xo_comp_info_type slpc_xo_comp_data =
{
  SLPC_NUM_CLIENTS,
  0
};

/*----------------------------------------------------------------------------
  Function definitions
----------------------------------------------------------------------------*/

/******************************************************************************
  @brief SLPC_GET_STMR_MODE

  Return whether SLPC client uses HW TSTMR or virtual STMR (VSTMR).

  @param id: identifies the calling client
  @param mode: is client using VSTMR or HW STMR?
  @return slpc_stmr_mode_enum
******************************************************************************/
slpc_stmr_mode_enum slpc_get_stmr_mode( slpc_id_type id )
{
  return SLPC_STMR_VSTMR;
}

/******************************************************************************
  @brief SLPC_IS_ACTIVE

  Is sleep controller active for client (started/sleep/wakeup/warmup)?

  @param id: identifies the calling client
  @return TRUE if slpc active, else FALSE
******************************************************************************/
boolean slpc_is_active( slpc_id_type id )
{
  return ( slpc[id].state != SLPC_INACTIVE_STATE ? TRUE : FALSE );
}

/******************************************************************************

  FUNCTION:  slpc_alloc_thread

******************************************************************************
    @brief
    Allocate a slpc thread index.

    @detail
    Allocate a thread if available and return its index.
    Otherwise, fatal error.

    This function may be called from multiple threads simultaneously, and the
    allocation is reliable even in that case.

    @return
    Thread index. Fatal error if not available.
********************************************************************************/
static uint32 slpc_alloc_thread (void)
{
  uint32 old_used, new_used, idx;

  /*-----------------------------------------------------------------------*/

  /* Repeat until we atomically allocate a thread, or fail. */
  do
  {
    old_used = atomic_read(&(slpc_thread_pool.active_mask));

    /* Find the lowest 0-bit in the "old_used" mask */
    idx = Q6_R_ct1_R(old_used);
    if ((uint32) idx >= SLPC_NUM_THREADS)
    {
      ERR_FATAL("Cannot allocate SLPC thread for tech (0x%08x,0x%08x).",
                old_used, idx, 0);
    }

    new_used = old_used | (1 << idx);

  } while (!atomic_compare_and_set(&(slpc_thread_pool.active_mask),
                                   old_used, new_used));

  /* Index has been reserved, return it. */
  return idx;
}

/******************************************************************************
  @brief SLPC_WAKEUP_TIMER_CB

  Wakeup timer expiration CB.

  @param id: identifies the calling client
  @return none.
******************************************************************************/
static void slpc_wakeup_timer_cb( slpc_id_type id )
{
  uint32  thread_idx;

  TRACE(id, WAKEUP_TIMER_CB, 0, 0, 0);

  /* Get the thread idx for slpc tech task */
  thread_idx = slpc_alloc_thread();

  TRACE(id, THREAD_ALLOCATE, atomic_read(&slpc_thread_pool.active_mask), thread_idx, 0);

  /* Save the client id in the respective thread idx. */
  slpc_thread_pool.thread_to_client_map[thread_idx] = id;

  /* Wakeup waiting thread */
  qurt_signal_set(&(slpc_thread_pool.task_sig[thread_idx]), SLPC_WAKEUP_TIMER_EXP_SIG);
}

/******************************************************************************
  @brief SLPC_GET_TICK

  Get 64-bit absolute timetick

  @param none
  @return timetick
******************************************************************************/
uint64 slpc_get_tick ( void )
{
  uint64 tick = 0;
  uint32 offset = 0;

  if (DAL_SUCCESS != DalTimetick_GetTimetick64(
                     slpc_data.tt_handle, (DalTimetickTime64Type *) &tick ))
  {
    ERR_FATAL("timetick error!", 0, 0, 0);
  }

  DalTimetick_GetOffset( slpc_data.tt_handle, &offset );

  return (tick - offset);
}

/******************************************************************************
  @brief SLPC_GET_START_TSTMR

  Get start TSTMR value for alt tech

  @param id: identifies the calling client
  @return: alt tstmr
******************************************************************************/
slpc_tstmr_type slpc_get_start_tstmr( slpc_id_type id, slpc_id_type alt_id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* Storage for return of alt_id tech stmr value at time transfer */
  slpc_tstmr_type tstmr;

  /* Initialize tstmr */
  tstmr.alt_time  = 0;
  tstmr.alt_phase = 0;

/*--------------------------------------------------------------------------*/

  ASSERT( id < SLPC_NUM_CLIENTS );
  ASSERT( alt_id < SLPC_NUM_CLIENTS );

  tstmr.time = sc->alt_techs[alt_id].start_time;
  tstmr.phase = sc->alt_techs[alt_id].start_phase;

  return tstmr;
}

/******************************************************************************
  @brief SLPC_VSTMR_TSTMR_READ_START

  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code.

  @param id: identifies the calling client
  @see
  @return none.
******************************************************************************/
static slpc_start_type slpc_vstmr_tstmr_read_start( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* return value */
  slpc_start_type start_ts = {0, {0, 0, 0, 0}, 0};

  /* local copy of tech dependent stmr formatted value */
  #ifndef FEATURE_BUILD_PAGEONLY_IMAGE
  vstmr_gstmr_t    vstmr_gstmr;
  vstmr_wstmr_t    vstmr_wstmr;
  vstmr_1xframe_t  vstmr_1xframe;
  vstmr_hstmr_t    vstmr_hstmr;
  vstmr_tdsstmr_t  vstmr_tdsstmr;
  #endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */
  vstmr_ostmr_t    vstmr_ostmr;
  vstmr_nb1_stmr_t vstmr_nb1stmr;

/*--------------------------------------------------------------------------*/

  ASSERT( id < SLPC_NUM_CLIENTS );

  /* read the current XO value */
  start_ts.tick = VSTMR_XO_READ_FULL();
  start_ts.ustmr_ts = start_ts.tick & SLPC_TIMETICK_LOWER_MASK;

  /* find corresponding tstmr value for current xo, for all techs */

  #ifndef FEATURE_BUILD_PAGEONLY_IMAGE
  /* for GERAN, capture the start time in RTC resolution as well. GERAN has a
     special requirement which can allow it to change its frame size. SLPC is
     not aware those frame adjustments. Hence, SLPC captures the raw RTC for
     GERAN and uses that to compute GERAN's slam point. */

  vstmr_gstmr = vstmr_geran_xo_to_gstmr(slpc_data.vstmr_data.gsm_view1,
                                        (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_GSM].start_time = vstmr_gstmr.all;
  sc->alt_techs[SLPC_GSM].start_time_in_rtc =
     vstmr_xo_to_rtc(slpc_data.vstmr_data.gsm_view1->rtc, (uint32) start_ts.ustmr_ts);

  vstmr_gstmr = vstmr_geran_xo_to_gstmr(slpc_data.vstmr_data.gsm_view2,
                                        (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_GSM2].start_time = vstmr_gstmr.all;
  sc->alt_techs[SLPC_GSM2].start_time_in_rtc =
     vstmr_xo_to_rtc(slpc_data.vstmr_data.gsm_view2->rtc, (uint32) start_ts.ustmr_ts);

  vstmr_gstmr = vstmr_geran_xo_to_gstmr(slpc_data.vstmr_data.gsm_view3,
                                        (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_GSM3].start_time = vstmr_gstmr.all;
  sc->alt_techs[SLPC_GSM3].start_time_in_rtc =
     vstmr_xo_to_rtc(slpc_data.vstmr_data.gsm_view3->rtc, (uint32) start_ts.ustmr_ts);

  vstmr_1xframe = vstmr_1x_xo_to_frame(slpc_data.vstmr_data.onex_view,
                                       (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_1X].start_time = vstmr_1xframe.all;
  vstmr_hstmr = vstmr_hdr_xo_to_hstmr(slpc_data.vstmr_data.hdr_view,
                                       (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_HDR].start_time = vstmr_hstmr.all;
  vstmr_wstmr = vstmr_wcdma_xo_to_wstmr(slpc_data.vstmr_data.wcdma_view,
                                       (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_WCDMA].start_time = vstmr_wstmr.all;
  vstmr_wstmr = vstmr_wcdma_xo_to_wstmr(slpc_data.vstmr_data.wcdma_view2,
                                       (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_WCDMA2].start_time = vstmr_wstmr.all;

  vstmr_tdsstmr = vstmr_tds_xo_to_stmr(slpc_data.vstmr_data.tdscdma_view,
                                       (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_TDSCDMA].start_time = vstmr_tdsstmr.all;
  #endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */

  vstmr_ostmr = vstmr_lte_xo_to_ostmr(slpc_data.vstmr_data.lte_view,
                                       (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_LTE].start_time = vstmr_ostmr.all;
  vstmr_nb1stmr = vstmr_nb1_xo_to_stmr(slpc_data.vstmr_data.nb1_view,
                                       (uint32) start_ts.ustmr_ts);
  sc->alt_techs[SLPC_NB1].start_time = vstmr_nb1stmr.all;

  /* to match legacy operation, return alt tech data to G and W */
  start_ts.tstmr.time = sc->alt_techs[id].start_time;
  #ifndef FEATURE_BUILD_PAGEONLY_IMAGE
  if ( (id == SLPC_GSM) || (id == SLPC_GSM2) || (id == SLPC_GSM3) )
  {
    start_ts.tstmr.alt_time = sc->alt_techs[SLPC_WCDMA].start_time;
  }
  if ( id == SLPC_WCDMA )
  {
    start_ts.tstmr.alt_time = sc->alt_techs[SLPC_GSM].start_time;
  }
  #endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */

  return start_ts;
}

/******************************************************************************
  @brief slpc_enable_alt_id

  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code.

  @param id: identifies the calling client
  @see
  @return none.
******************************************************************************/
void slpc_enable_alt_id( slpc_id_type id, slpc_id_type alt_id,
                         slpc_tstmr_type *tstmr )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

/*--------------------------------------------------------------------------*/

  ASSERT( id < SLPC_NUM_CLIENTS );
  ASSERT( alt_id < SLPC_NUM_CLIENTS );

  sc->alt_techs[alt_id].valid = TRUE;

  if ( tstmr == NULL )
  {
    sc->alt_techs[alt_id].load_tstmr = FALSE;
    sc->alt_techs[alt_id].tstmr = 0;
  }
  else
  {
    sc->alt_techs[alt_id].load_tstmr = TRUE;
    sc->alt_techs[alt_id].tstmr = tstmr->time;
  }
}

/******************************************************************************
  @brief slpc_disable_alt_id

  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code.

  @param id: identifies the calling client
  @see
  @return none.
******************************************************************************/
void slpc_disable_alt_id( slpc_id_type id, slpc_id_type alt_id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

/*--------------------------------------------------------------------------*/

  sc->alt_techs[alt_id].valid = FALSE;
  sc->alt_techs[alt_id].load_tstmr = FALSE;
  sc->alt_techs[alt_id].tstmr = 0;
}

/******************************************************************************
  @brief SLPC_CALL_NOTIFY

  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code.

  @param id: identifies the calling client
  @see
  @return none.
******************************************************************************/
static void slpc_call_notify( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* loop counter */
  uint8 i = 0;

/*--------------------------------------------------------------------------*/

  for ( i = 0; i < SLPC_MAX_WAKEUP_CALLBACKS; i++ )
  {
    if ( slpc_data.notify_cb[i] != NULL )
    {
      slpc_data.notify_cb[i]( id, sc->wakeup.tick, sc->wakeup.update,
                              sc->wakeup.extension );
    }
  }
}

/******************************************************************************
  @brief SLPC_CALL_NOTIFY_WAKEUP

  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code.

  @param id: identifies the calling client
  @see
  @return none.
******************************************************************************/
static void slpc_call_notify_wakeup( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* loop counter */
  uint8 i = 0;

/*--------------------------------------------------------------------------*/

  for ( i = 0; i < SLPC_MAX_WAKEUP_CALLBACKS; i++ )
  {
    if ( slpc_data.notify_wakeup_cb[i] != NULL )
    {
      slpc_data.notify_wakeup_cb[i]( id, sc->wakeup.tick );
    }
  }
}

/******************************************************************************
  @brief SLPC_UPDATE_WAKEUP_POINT

  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code.

  @param id: identifies the calling client
  @see
  @return none.
******************************************************************************/
static slpc_err_type slpc_update_wakeup_point( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* Local read of current timetick */
  uint64 tick;

  /* return value */
  slpc_err_type err_code = SLPC_ERR_NO_ERR;

/*--------------------------------------------------------------------------*/

  sc->wakeup.last = sc->wakeup.tick;
  sc->wakeup.tick = sc->ending.tick - sc->warmup.dur_tick;

  ENTER_TECH_CRIT_SECTION(id);

  tick = slpc_get_tick();
  TRACE(id, UPDATE_WAKEUP, sc->state, tick, sc->wakeup.tick);

  if ( sc->wakeup.tick < tick )
  {
    DBG_3(ERROR, "Wakeup point is in the past! n=%u w=%u s=%u",
          (uint32) tick, (uint32) sc->wakeup.tick, (uint32) sc->ending.tick);
    err_code = SLPC_ERR_WAKEUP_IN_PAST;
  }
  else
  {
    timer_set_64( &sc->wakeup.timer, (sc->wakeup.tick - tick), 0, T_TICK );
  }

  /* check if this is an update to a previously set wakeup point */
  if ( sc->state == SLPC_SLEEP_STATE )
  {
    sc->wakeup.update = TRUE;
  }
  else
  {
    sc->wakeup.update = FALSE;
  }

  /* check if new wakeup is later than old, so this is an extension */
  if ( sc->wakeup.tick > sc->wakeup.last )
  {
    sc->wakeup.extension = TRUE;
  }
  else
  {
    sc->wakeup.extension = FALSE;
  }

  /* check if new wakeup is possibly rude; if not, elevate slpc priority */
  if ( sc->wakeup.update && (sc->wakeup.tick - tick) < SLPC_RUDE_WAKEUP_THRESH )
  {
    sc->wakeup.rude = TRUE;
    DBG_0( HIGH, "Rude wakeup" );
  }
  else
  {
    sc->wakeup.rude = FALSE;
  }

  LEAVE_TECH_CRIT_SECTION(id);

  return err_code;
}


/******************************************************************************
  @brief SLPC_TSTMR_TO_USTMR

  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code.

  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @param orig_ustmr_dur : Pointer to the variable to return the unfiltered
                          sleep duration in USTMR ticks
  @see Func2
  @return ustmr_type - value of duration in USTMR units
******************************************************************************/
static uint64 slpc_tstmr_to_ustmr( slpc_id_type id, tstmr_type tstmr_dur, ustmr_type *orig_ustmr_dur )
{
  /* Duration in "uncorrected", or raw, XO's */
  int64 dur_xo = 0;

  /* Duration in "unfiltered", or raw, XO's */
  int64 unfiltered_dur_xo = 0;

  /* Error storage, in ppm/1024 */
  int64 err = 0;

  /* Unfiltered error storage, in ppm/1024 */
  int64 unfiltered_err = 0;

  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* XO manager xo error data */
  tcxomgr_vco_info_type vco = tcxomgr_get_client_rgs(sc->tm_client);

/*--------------------------------------------------------------------------*/

  /* collect all error in 1/1024 ppm, part from slew, part from RGS */
  err = sc->slew_err_accum + vco.rot_value;
  unfiltered_err = sc->unfiltered_slew_err_accum + vco.rot_value;

  DBG_3(HIGH, "err: %d, slew_err_accum: %d, rot_err: %d",
        (int32) err, (int32) sc->slew_err_accum, (int32) vco.rot_value );

  /* convert to intermediate err, accounting for rounding */
  err = ((192 * err) + ((1024 * 10) >> 1)) / (1024 * 10);
  dur_xo = (int64) tstmr_dur * (19200000LL + err);
  dur_xo = (dur_xo + (sc->system_freq >> 1)) / sc->system_freq;

  /* convert to intermediate err, accounting for rounding */
  unfiltered_err = ((192 * unfiltered_err) + ((1024 * 10) >> 1)) / (1024 * 10);
  unfiltered_dur_xo = (int64) tstmr_dur * (19200000LL + unfiltered_err);
  unfiltered_dur_xo = (unfiltered_dur_xo + (sc->system_freq >> 1)) / sc->system_freq;

  TRACE(id, TSTMR_TO_USTMR, tstmr_dur, sc->slew_err_accum, dur_xo);

  DBG_3(HIGH, "(UNFILTERED) err: %d, slew_err_accum: %d dur_xo: %d",
        (int32) unfiltered_err,
        (int32) sc->unfiltered_slew_err_accum,
        (int32) unfiltered_dur_xo);

  if (orig_ustmr_dur != NULL)
  {
    *orig_ustmr_dur = unfiltered_dur_xo;
  }
  else
  {
    DBG_0(ERROR, "Null pointer passed in for unfiltered sleep duration");
  }

  return dur_xo;
}

/******************************************************************************
  @brief SLPC_GET_WAKEUP_TICK

  Get wakeup interrupt point, in Qtimer ticks

  @param id: identifies the calling client

  @return absolute timetick of the wakeup point
******************************************************************************/
uint64 slpc_get_wakeup_tick( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

/*--------------------------------------------------------------------------*/

  TRACE(id, GET_WAKEUP_TICK, sc->wakeup.tick, 0, 0);
  return sc->wakeup.tick;
}


/******************************************************************************
  @brief SLPC_GET_OLS_TICK

  Get the absolute timetick of the ending interrupt (Qtimer)

  @param id: identifies the calling client

  @return absolute timetick of the online start point
******************************************************************************/
uint64 slpc_get_ols_tick( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

/*--------------------------------------------------------------------------*/

  TRACE(id, GET_OLS_TICK, sc->ending.tick, 0, 0);
  return sc->ending.tick;
}

/******************************************************************************
  @brief SLPC_GET_WAKEUP_AND_OLS_TICK

  Get the absolute timeticks of the wakeup and online start points

  @param id: identifies the calling client

  @return absolute timetick of the wakeup point and OLS point
******************************************************************************/
void slpc_get_wakeup_and_ols_tick( slpc_id_type id, uint64 *wakeup_tick,
                                   uint64 *ols_tick )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

/*--------------------------------------------------------------------------*/

  ENTER_TECH_CRIT_SECTION(id);

  TRACE(id, GET_WAKEUP_TICK, sc->wakeup.tick, 0, 0);
  TRACE(id, GET_OLS_TICK, sc->ending.tick, 0, 0);

  if ( wakeup_tick != NULL )
  {
    *wakeup_tick = sc->wakeup.tick;
  }
  if ( ols_tick != NULL )
  {
    *ols_tick = sc->ending.tick;
  }

  LEAVE_TECH_CRIT_SECTION(id);
}

/******************************************************************************
  @brief SLPC_GET_SYSCLK_COUNT

  Returns the number of TSTMR ticks the controller has been active for.

  @param id: identifies the calling client
  @return TSTMR ticks
******************************************************************************/
uint64 slpc_get_sysclk_count( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* Error calculation in ppm */
  int64 err_ppm = 0;

  /* XO manager xo error data */
  tcxomgr_vco_info_type vco = tcxomgr_get_client_rgs(sc->tm_client);

  /* Local read of current timetick, duration in timeticks */
  uint64 tick, dur_xo, dur_tstmr;

/*--------------------------------------------------------------------------*/

  tick = slpc_get_tick();

  dur_xo = tick - sc->start.tick;

  /* collect all error in 1/1024 ppm, part from slew, part from RGS */
  err_ppm = sc->slew_err_accum + vco.rot_value;
  dur_xo = (int64) dur_xo - (int64) ((err_ppm * (int64) dur_xo) / 1024000000LL);
  dur_tstmr = (dur_xo * sc->system_freq) / 19200000LL;

  TRACE(id, GET_SYSCLK_COUNT, tick, err_ppm, dur_tstmr);
  return dur_tstmr;
}

/******************************************************************************
  @brief SLPC_ERROR_FEEDBACK

  Adjust the slow clock frequency estimate, based on the sleep duration
  and the reacquisition slews.

  @param id: identifies the calling client
  @param slew_err: slew error (TSTMR)
  @return none
******************************************************************************/
void slpc_error_feedback( slpc_id_type id, int64 slew_err )
{
  slpc_error_feedback_ext(id, slew_err);
}

/******************************************************************************
  @brief SLPC_ERROR_FEEDBACK_EXT

  Adjust the slow clock frequency estimate, based on the sleep duration
  and the reacquisition slews.

  @param id: identifies the calling client
  @param slew_err: slew error (TSTMR)
  @return XO shutdown duty cycle weighted error in PPM - Q10 format
          for the current sleep cycle.
******************************************************************************/
int32 slpc_error_feedback_ext( slpc_id_type id, int64 slew_err )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* Error calculation in ppm */
  int64 err_ppm = 0;

  /* Unfiltered error calculation in ppm */
  int64 unfiltered_err_ppm = 0;

  /* Unfiltered error reported by techs */
  int64 unfiltered_slew_err = slew_err;

/*--------------------------------------------------------------------------*/

  /* Weigh the reported error (in chips) by the duty cycle
     to prevent precision loss. If last XO sleep count is 0, then
     assume duty cycle of 100% and resort to legacy behavior */
  if (sc->use_xo_sleep_cnt && sc->last_sleep.xo_sleep_cnt > 0)
  {
    slew_err = slew_err * sc->last_sleep.xo_sleep_cnt;
    slew_err /= sc->last_sleep.duration_ustmr;
  }

  DBG_3(HIGH, "dur: %u sysclks, orig err: %d sysclks corr err: %d sysclks",
        (uint32) sc->last_sleep.duration_tstmr, (int32) unfiltered_slew_err, (int32) slew_err);

  /* conv to 1/1024ppm err:  err = (slew_err / dur_sysclk) * 1024e6 */
  /* apply dyn gain:  err = err * (dur_syslk / 2^k)  */
  /* k is tech specific, (gain is 1 for ~1.25sec dur) */
  /* combine:  err = (slew_err * 1e6) / (1 << (k-10)) */
  /* limit dyn gain to 4 when duration gets too large */
  if ( sc->last_sleep.duration_tstmr <= (1 << (sc->k + 2)))
  {
    err_ppm = (slew_err * 1000000LL) / (int64) (1 << (sc->k - 10));
    unfiltered_err_ppm = (unfiltered_slew_err * 1000000LL) / (int64) (1 << (sc->k - 10));
  }
  else
  {
    err_ppm = (slew_err * 4096000000LL) / (int64) sc->last_sleep.duration_tstmr;
    unfiltered_err_ppm = (unfiltered_slew_err * 4096000000LL) / (int64) sc->last_sleep.duration_tstmr;
  }

  /* apply constant loop gain - 1/8 rounded correctly */
  err_ppm = err_ppm >> 2;
  err_ppm += 1;
  err_ppm = err_ppm >> 1;

  unfiltered_err_ppm = unfiltered_err_ppm >> 2;
  unfiltered_err_ppm += 1;
  unfiltered_err_ppm = unfiltered_err_ppm >> 1;

  /* Add into accumulator to be used in future duration calcuations */
  sc->slew_err_accum += err_ppm;
  sc->unfiltered_slew_err_accum += unfiltered_err_ppm;

  TRACE(id, ERR_FEEDBACK, slew_err, err_ppm, sc->slew_err_accum);

  /* Limit the accum slew error to avoid run away conditions */
  if ( sc->slew_err_accum > SLPC_MAX_ACCUM_ERR )
  {
    sc->slew_err_accum = SLPC_MAX_ACCUM_ERR;
    DBG_1(ERROR, "accum limit:  %d/1024 ppm", (int32) sc->slew_err_accum);
  }
  else if ( sc->slew_err_accum < -SLPC_MAX_ACCUM_ERR )
  {
    sc->slew_err_accum = -SLPC_MAX_ACCUM_ERR;
    DBG_1(ERROR, "accum limit:  %d/1024 ppm", (int32) sc->slew_err_accum);
  }

  /* Limit the unfiltered accum slew error to avoid run away conditions */
  if ( sc->unfiltered_slew_err_accum > SLPC_MAX_ACCUM_ERR )
  {
    sc->unfiltered_slew_err_accum = SLPC_MAX_ACCUM_ERR;
    DBG_1(ERROR, "(UNFILTERED) accum limit:  %d/1024 ppm", (int32) sc->unfiltered_slew_err_accum);
  }
  else if ( sc->unfiltered_slew_err_accum < -SLPC_MAX_ACCUM_ERR )
  {
    sc->unfiltered_slew_err_accum = -SLPC_MAX_ACCUM_ERR;
    DBG_1(ERROR, "(UNFILTERED) accum limit:  %d/1024 ppm", (int32) sc->unfiltered_slew_err_accum);
  }

  DBG_4(HIGH, "err: %d, accum: %d. (UNFILTERED) err: %d, accum: %d.",
        (int32) err_ppm, (int32) sc->slew_err_accum,
        (int32) unfiltered_err_ppm, (int32) sc->unfiltered_slew_err_accum);

  return (int32)slew_err;
}

/******************************************************************************
  @brief SLPC_START

  Get snapshot of current timer values for start of sleep cycle

  @param id: identifies the calling client
  @return start tick (TSTMR)
******************************************************************************/
slpc_tstmr_type slpc_start( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

/*--------------------------------------------------------------------------*/

  if ( sc->state == SLPC_INACTIVE_STATE )
  {
    /* Get a synchronous snapshot of current timer values */
      sc->start = slpc_vstmr_tstmr_read_start( id );

    sc->state = SLPC_START_STATE;

    TRACE(id, START,
          sc->start.tstmr.time, sc->start.tick, sc->start.ustmr_ts);
  }
  else
  {
    DBG_2(ERROR, "slpc_start called by %d in state %d", sc->id, sc->state );
  }

  return ( sc->start.tstmr );
}

#ifndef FEATURE_BUILD_PAGEONLY_IMAGE
/******************************************************************************
  @brief slpc_gsm1_vstmr_tstmr_slam

  Performs slam calculations for GSM1 alt ID.

  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none.
******************************************************************************/
static void slpc_gsm1_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* local container used in slam calculations */
  slpc_dword_type tstmr_slam;

  /* for storing the raw start time */
  uint32 start, xo_curr;

  /* for storing the raw start time in rtc resolution (for geran) */
  uint64 start_time_in_rtc;

  /* local copy of tech dependent stmr formatted value */
  vstmr_gstmr_t vstmr_gstmr, gstmr_before_sync, gstmr_after_sync;

/*--------------------------------------------------------------------------*/
  xo_curr = (vstmr_get_ustmr() & VSTMR_XO_MASK);

  ustmr_slam = sc->ending.ustmr_ts;

  vstmr_gstmr = (vstmr_gstmr_t) sc->alt_techs[SLPC_GSM].start_time;
  start_time_in_rtc = sc->alt_techs[SLPC_GSM].start_time_in_rtc;
  /* get the RTC integer */
  start = start_time_in_rtc >> 32;
  dur %= VSTMR_GERAN_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_GERAN_RTC_MODULO;
  tstmr_slam.uw[0] = (uint32)(start_time_in_rtc & 0xFFFFFFFF);
  tstmr_slam.uw[1] = slam;
  /* GERAN has a special requirement which can allow it to change its frame
     size. Since SLPC is not aware those frame adjustments, GERAN time correction
     is done RTC resolution. */
  gstmr_before_sync = vstmr_geran_xo_to_gstmr(
                      slpc_data.vstmr_data.gsm_view1, xo_curr );
  TRACE(SLPC_GSM, VSTMR_SLAM_CALC, start, dur, slam);
  DBG_3(HIGH, "client:%d VSTMR_RTC_SYNC (xo, rtc):(0x%08x, 0x%08x)",
              SLPC_GSM, (uint32)ustmr_slam, (uint32)slam);

  vstmr_rtc_sync(slpc_data.vstmr_data.gsm_view1->rtc, ustmr_slam, tstmr_slam.ud);
  gstmr_after_sync = vstmr_geran_xo_to_gstmr(
                     slpc_data.vstmr_data.gsm_view1, xo_curr );

  DBG_5(HIGH, "client:%d(GSM) VSTMR_READ (frame_num, raw_13M)-> before:(%d, %d) after:(%d, %d)",
              SLPC_GSM, (uint32)gstmr_before_sync.frame_num, (uint32)gstmr_before_sync.raw_13M,
              (uint32)gstmr_after_sync.frame_num, (uint32)gstmr_after_sync.raw_13M);
  TRACE(SLPC_GSM, VSMTR_READ, gstmr_before_sync.all, gstmr_after_sync.all, ustmr_slam);

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_GSM, start, dur, slam);

}
#endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */

#ifndef FEATURE_BUILD_PAGEONLY_IMAGE
/******************************************************************************
  @brief slpc_gsm2_vstmr_tstmr_slam

  Performs slam calculations for GSM2 alt ID.

  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none.
******************************************************************************/
static void slpc_gsm2_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* local container used in slam calculations */
  slpc_dword_type tstmr_slam;

  /* for storing the raw start time */
  uint32 start, xo_curr;

  /* for storing the raw start time in rtc resolution (for geran) */
  uint64 start_time_in_rtc;

  /* local copy of tech dependent stmr formatted value */
  vstmr_gstmr_t vstmr_gstmr, gstmr_before_sync, gstmr_after_sync;

/*--------------------------------------------------------------------------*/

  xo_curr = (vstmr_get_ustmr() & VSTMR_XO_MASK);

  ustmr_slam = sc->ending.ustmr_ts;

  vstmr_gstmr = (vstmr_gstmr_t) sc->alt_techs[SLPC_GSM2].start_time;
  start_time_in_rtc = sc->alt_techs[SLPC_GSM2].start_time_in_rtc;
  /* get the RTC integer */
  start = start_time_in_rtc >> 32;
  dur %= VSTMR_GERAN_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_GERAN_RTC_MODULO;
  tstmr_slam.uw[0] = (uint32)(start_time_in_rtc & 0xFFFFFFFF);
  tstmr_slam.uw[1] = slam;
  /* GERAN has a special requirement which can allow it to change its frame
     size. Since SLPC is not aware those frame adjustments, GERAN time correction
     is done RTC resolution. */
  gstmr_before_sync = vstmr_geran_xo_to_gstmr(
                      slpc_data.vstmr_data.gsm_view2, xo_curr );
  TRACE(SLPC_GSM2, VSTMR_SLAM_CALC, start, dur, slam);
  DBG_3(HIGH, "client:%d VSTMR_RTC_SYNC (xo, rtc):(0x%08x, 0x%08x)",
              SLPC_GSM2, (uint32)ustmr_slam, (uint32)slam);

  vstmr_rtc_sync(slpc_data.vstmr_data.gsm_view2->rtc, ustmr_slam, tstmr_slam.ud);
  gstmr_after_sync = vstmr_geran_xo_to_gstmr(
                     slpc_data.vstmr_data.gsm_view2, xo_curr );
  DBG_5(HIGH, "client:%d(GSM2) VSTMR_READ (frame_num, raw_13M)-> before:(%d, %d) after:(%d, %d)",
              SLPC_GSM2, (uint32)gstmr_before_sync.frame_num, (uint32)gstmr_before_sync.raw_13M,
              (uint32)gstmr_after_sync.frame_num, (uint32)gstmr_after_sync.raw_13M);
  TRACE(SLPC_GSM2, VSMTR_READ, gstmr_before_sync.all, gstmr_after_sync.all, ustmr_slam);

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_GSM2, start, dur, slam);

}
#endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */

#ifndef FEATURE_BUILD_PAGEONLY_IMAGE
/******************************************************************************
  @brief slpc_gsm3_vstmr_tstmr_slam

  Performs slam calculations for GSM3 alt ID.

  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none.
******************************************************************************/
static void slpc_gsm3_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* local container used in slam calculations */
  slpc_dword_type tstmr_slam;

  /* for storing the raw start time */
  uint32 start, xo_curr;

  /* for storing the raw start time in rtc resolution (for geran) */
  uint64 start_time_in_rtc;

  /* local copy of tech dependent stmr formatted value */
  vstmr_gstmr_t vstmr_gstmr, gstmr_before_sync, gstmr_after_sync;

/*--------------------------------------------------------------------------*/

  xo_curr = (vstmr_get_ustmr() & VSTMR_XO_MASK);

  ustmr_slam = sc->ending.ustmr_ts;

  vstmr_gstmr = (vstmr_gstmr_t) sc->alt_techs[SLPC_GSM3].start_time;
  start_time_in_rtc = sc->alt_techs[SLPC_GSM3].start_time_in_rtc;
  /* get the RTC integer */
  start = start_time_in_rtc >> 32;
  dur %= VSTMR_GERAN_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_GERAN_RTC_MODULO;
  tstmr_slam.uw[0] = (uint32)(start_time_in_rtc & 0xFFFFFFFF);
  tstmr_slam.uw[1] = slam;
  /* GERAN has a special requirement which can allow it to change its frame
     size. Since SLPC is not aware those frame adjustments, GERAN time correction
     is done RTC resolution. */
  gstmr_before_sync = vstmr_geran_xo_to_gstmr(
                      slpc_data.vstmr_data.gsm_view3, xo_curr );
  TRACE(SLPC_GSM3, VSTMR_SLAM_CALC, start, dur, slam);
  DBG_3(HIGH, "client:%d VSTMR_RTC_SYNC (xo, rtc):(0x%08x, 0x%08x)",
              SLPC_GSM3, (uint32)ustmr_slam, (uint32)slam);

  vstmr_rtc_sync(slpc_data.vstmr_data.gsm_view3->rtc, ustmr_slam, tstmr_slam.ud);
  gstmr_after_sync = vstmr_geran_xo_to_gstmr(
                     slpc_data.vstmr_data.gsm_view3, xo_curr );
  DBG_5(HIGH, "client:%d(GSM3) VSTMR_READ (frame_num, raw_13M)-> before:(%d, %d) after:(%d, %d)",
              SLPC_GSM3, (uint32)gstmr_before_sync.frame_num, (uint32)gstmr_before_sync.raw_13M,
              (uint32)gstmr_after_sync.frame_num, (uint32)gstmr_after_sync.raw_13M);
  TRACE(SLPC_GSM3, VSMTR_READ, gstmr_before_sync.all, gstmr_after_sync.all, ustmr_slam);

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_GSM3, start, dur, slam);

}
#endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */

#ifndef FEATURE_BUILD_PAGEONLY_IMAGE
/******************************************************************************
  @brief slpc_1x_vstmr_tstmr_slam

  Performs slam calculations for 1X alt ID.

  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none.
******************************************************************************/
static void slpc_1x_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* local container used in slam calculations */
  slpc_dword_type tstmr_slam;

  /* for storing the raw start time */
  uint32 start, xo_curr;

  /* local copy of tech dependent stmr formatted value */
  vstmr_1xframe_t vstmr_1xframe, onexframe_before_sync, onexframe_after_sync;

/*--------------------------------------------------------------------------*/

  xo_curr = (vstmr_get_ustmr() & VSTMR_XO_MASK);

  ustmr_slam = sc->ending.ustmr_ts;

  vstmr_1xframe = (vstmr_1xframe_t) sc->alt_techs[SLPC_1X].start_time;
  start = vstmr_1x_frame_to_cx8( vstmr_1xframe );
  dur %= VSTMR_1X_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_1X_RTC_MODULO;
  tstmr_slam.uw[0] = vstmr_1xframe.frac;
  tstmr_slam.uw[1] = slam;
  vstmr_1xframe = vstmr_1x_frame_segment_cnt( tstmr_slam.ud );
  onexframe_before_sync = vstmr_1x_xo_to_frame(
                          slpc_data.vstmr_data.onex_view, xo_curr );
  TRACE(SLPC_1X, VSTMR_SLAM_CALC, start, dur, slam);
  DBG_3(HIGH, "client:%d VSTMR_RTC_SYNC (xo, rtc):(0x%08x, 0x%08x)",
              SLPC_1X, (uint32)ustmr_slam, (uint32)slam);

  vstmr_1x_frame_sync( slpc_data.vstmr_data.onex_view, vstmr_1xframe,
                       ustmr_slam );
  onexframe_after_sync = vstmr_1x_xo_to_frame(
                         slpc_data.vstmr_data.onex_view, xo_curr );
  TRACE(SLPC_1X, VSMTR_READ, onexframe_before_sync.all, onexframe_after_sync.all, ustmr_slam);

  DBG_5(HIGH, "client:%d(1X) VSTMR_READ (pcg_cx8, pcg_cnt)-> before:(%d, %d) after:(%d, %d)",
              SLPC_1X, (uint32)onexframe_before_sync.pcg_cx8, (uint32)onexframe_before_sync.pcg_cnt,
              (uint32)onexframe_after_sync.pcg_cx8, (uint32)onexframe_after_sync.pcg_cnt);

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_1X, start, dur, slam);

}
#endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */

#ifndef FEATURE_BUILD_PAGEONLY_IMAGE
/******************************************************************************
  @brief slpc_hdr_vstmr_tstmr_slam

  Performs slam calculations for HDR alt ID.

  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none.
******************************************************************************/
static void slpc_hdr_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* for storing the raw start time */
  uint32 start, xo_curr;

  /* local copy of tech dependent stmr formatted value */
  vstmr_hstmr_t vstmr_hstmr, hstmr_before_sync, hstmr_after_sync;

/*--------------------------------------------------------------------------*/

  xo_curr = (vstmr_get_ustmr() & VSTMR_XO_MASK);

  ustmr_slam = sc->ending.ustmr_ts;

  vstmr_hstmr = (vstmr_hstmr_t) sc->alt_techs[SLPC_HDR].start_time;
  start = vstmr_hstmr.cx8;
  dur %= VSTMR_HDR_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_HDR_RTC_MODULO;
  vstmr_hstmr.cx8 = slam;
  hstmr_before_sync = vstmr_hdr_xo_to_hstmr(
                      slpc_data.vstmr_data.hdr_view, xo_curr );

  TRACE(SLPC_HDR, VSTMR_SLAM_CALC, start, dur, slam);
  DBG_3(HIGH, "client:%d VSTMR_RTC_SYNC (xo, rtc):(0x%08x, 0x%08x)",
              SLPC_HDR, (uint32)ustmr_slam, (uint32)slam);

  vstmr_hdr_hstmr_sync( slpc_data.vstmr_data.hdr_view, vstmr_hstmr,
                        ustmr_slam );
  hstmr_after_sync = vstmr_hdr_xo_to_hstmr(
                     slpc_data.vstmr_data.hdr_view, xo_curr );
  TRACE(SLPC_HDR, VSMTR_READ, hstmr_before_sync.all, hstmr_after_sync.all, ustmr_slam);
  DBG_3(HIGH, "client:%d(HDR) VSTMR_READ cx8-> before:%d, after:%d",
              SLPC_HDR, (uint32)hstmr_before_sync.cx8, (uint32)hstmr_after_sync.cx8);

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_HDR, start, dur, slam);

}
#endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */

#ifndef FEATURE_BUILD_PAGEONLY_IMAGE
/******************************************************************************
  @brief slpc_wcdma_vstmr_tstmr_slam

  Performs slam calculations for WCDMA alt ID.

  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none.
******************************************************************************/
static void slpc_wcdma_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* local container used in slam calculations */
  slpc_dword_type tstmr_slam;

  /* for storing the raw start time */
  uint32 start, xo_curr;

  /* local copy of tech dependent stmr formatted value */
  vstmr_wstmr_t vstmr_wstmr, wstmr_before_sync, wstmr_after_sync;

/*--------------------------------------------------------------------------*/

  xo_curr = (vstmr_get_ustmr() & VSTMR_XO_MASK);

  ustmr_slam = sc->ending.ustmr_ts;

  vstmr_wstmr = (vstmr_wstmr_t) sc->alt_techs[SLPC_WCDMA].start_time;
  start = vstmr_wcdma_seg_to_cx8( vstmr_wstmr );
  dur %= VSTMR_WCDMA_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_WCDMA_RTC_MODULO;
  tstmr_slam.uw[0] = vstmr_wstmr.frac;
  tstmr_slam.uw[1] = slam;
  vstmr_wstmr = vstmr_wcdma_segment_cnt( tstmr_slam.ud );

  wstmr_before_sync = vstmr_wcdma_xo_to_wstmr(
                      slpc_data.vstmr_data.wcdma_view, xo_curr );

  TRACE(SLPC_WCDMA, VSTMR_SLAM_CALC, start, dur, slam);
  DBG_3(HIGH, "client:%d VSTMR_RTC_SYNC (xo, rtc):(0x%08x, 0x%08x)",
              SLPC_WCDMA, (uint32)ustmr_slam, (uint32)slam);
  vstmr_wcdma_wstmr_sync( slpc_data.vstmr_data.wcdma_view, vstmr_wstmr,
                          ustmr_slam );
  wstmr_after_sync = vstmr_wcdma_xo_to_wstmr(
                     slpc_data.vstmr_data.wcdma_view, xo_curr );

  TRACE(SLPC_WCDMA, VSMTR_READ, wstmr_before_sync.all, wstmr_after_sync.all, ustmr_slam);
  DBG_5(HIGH, "client:%d(WCDMA) VSTMR_READ (frame_num, subframecx8)-> before:(%d, %d) after:(%d, %d)",
              SLPC_WCDMA, (uint32)wstmr_before_sync.frame_no, (uint32)wstmr_before_sync.subframecx8,
              (uint32)wstmr_after_sync.frame_no, (uint32)wstmr_after_sync.subframecx8);

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_WCDMA, start, dur, slam);
}
#endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */

#ifndef FEATURE_BUILD_PAGEONLY_IMAGE
/******************************************************************************
  @brief slpc_wcdma2_vstmr_tstmr_slam

  Performs slam calculations for WCDMA2 alt ID.

  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none.
******************************************************************************/
static void slpc_wcdma2_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* local container used in slam calculations */
  slpc_dword_type tstmr_slam;

  /* for storing the raw start time */
  uint32 start, xo_curr;

  /* local copy of tech dependent stmr formatted value */
  vstmr_wstmr_t vstmr_wstmr, wstmr_before_sync, wstmr_after_sync;

/*--------------------------------------------------------------------------*/

  xo_curr = (vstmr_get_ustmr() & VSTMR_XO_MASK);

  ustmr_slam = sc->ending.ustmr_ts;

  vstmr_wstmr = (vstmr_wstmr_t) sc->alt_techs[SLPC_WCDMA2].start_time;
  start = vstmr_wcdma_seg_to_cx8( vstmr_wstmr );
  dur %= VSTMR_WCDMA_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_WCDMA_RTC_MODULO;
  tstmr_slam.uw[0] = vstmr_wstmr.frac;
  tstmr_slam.uw[1] = slam;
  vstmr_wstmr = vstmr_wcdma_segment_cnt( tstmr_slam.ud );

  wstmr_before_sync = vstmr_wcdma_xo_to_wstmr(
                      slpc_data.vstmr_data.wcdma_view2, xo_curr );

  TRACE(SLPC_WCDMA2, VSTMR_SLAM_CALC, start, dur, slam);
  DBG_3(HIGH, "client:%d VSTMR_RTC_SYNC (xo, rtc):(0x%08x, 0x%08x)",
              SLPC_WCDMA2, (uint32)ustmr_slam, (uint32)(slam));
  vstmr_wcdma_wstmr_sync( slpc_data.vstmr_data.wcdma_view2, vstmr_wstmr,
                          ustmr_slam );
  wstmr_after_sync = vstmr_wcdma_xo_to_wstmr(
                     slpc_data.vstmr_data.wcdma_view2, xo_curr );
  TRACE(SLPC_WCDMA2, VSMTR_READ, wstmr_before_sync.all, wstmr_after_sync.all, ustmr_slam);
  DBG_5(HIGH, "client:%d(WCDMA2) VSTMR_READ (frame_num, subframecx8)-> before:(%d, %d) after:(%d, %d)",
              SLPC_WCDMA2, (uint32)wstmr_before_sync.frame_no, (uint32)wstmr_before_sync.subframecx8,
              (uint32)wstmr_after_sync.frame_no, (uint32)wstmr_after_sync.subframecx8);

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_WCDMA2, start, dur, slam);

}
#endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */

/******************************************************************************
  @brief slpc_lte_vstmr_tstmr_slam

  Performs slam calculations for LTE alt ID.

  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none.
******************************************************************************/
static void slpc_lte_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* for storing the raw start time */
  uint32 start, xo_curr;

  /* local copy of tech dependent stmr formatted value */
  vstmr_ostmr_t vstmr_ostmr, ostmr_before_sync, ostmr_after_sync;

/*--------------------------------------------------------------------------*/

  xo_curr = (vstmr_get_ustmr() & VSTMR_XO_MASK);

  ustmr_slam = sc->ending.ustmr_ts;

  vstmr_ostmr = (vstmr_ostmr_t) sc->alt_techs[SLPC_LTE].start_time;
  start = vstmr_ostmr.ostmr_cnt;
  dur %= VSTMR_LTE_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_LTE_RTC_MODULO;
  vstmr_ostmr.ostmr_cnt = slam;

  ostmr_before_sync = vstmr_lte_xo_to_ostmr(
                      slpc_data.vstmr_data.lte_view, xo_curr );
  TRACE(SLPC_LTE, VSTMR_SLAM_CALC, start, dur, slam);
  DBG_3(HIGH, "client:%d VSTMR_RTC_SYNC (xo, rtc):(0x%08x, 0x%08x)",
              SLPC_LTE, (uint32)ustmr_slam, (uint32)slam);

  vstmr_lte_ostmr_sync( slpc_data.vstmr_data.lte_view, vstmr_ostmr,
                        ustmr_slam );
  ostmr_after_sync = vstmr_lte_xo_to_ostmr(
                     slpc_data.vstmr_data.lte_view, xo_curr );
  TRACE(SLPC_LTE, VSMTR_READ, ostmr_before_sync.all, ostmr_after_sync.all, ustmr_slam);

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_LTE, start, dur, slam);
  DBG_3(HIGH, "client:%d(LTE) VSTMR_READ ostmr_cnt-> before:%d after:%d",
              SLPC_LTE, (uint32)ostmr_before_sync.ostmr_cnt, (uint32)ostmr_after_sync.ostmr_cnt);
}

#ifndef FEATURE_BUILD_PAGEONLY_IMAGE
/******************************************************************************
  @brief slpc_tdscdma_vstmr_tstmr_slam

  Performs slam calculations for TDSCDMA alt ID.

  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none.
******************************************************************************/
static void slpc_tdscdma_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* local container used in slam calculations */
  slpc_dword_type tstmr_slam;

  /* for storing the raw start time */
  uint32 start, xo_curr;

  /* local copy of tech dependent stmr formatted value */
  vstmr_tdsstmr_t vstmr_tdsstmr, tdsstmr_before_sync, tdsstmr_after_sync;

/*--------------------------------------------------------------------------*/

  xo_curr = (vstmr_get_ustmr() & VSTMR_XO_MASK);

  ustmr_slam = sc->ending.ustmr_ts;

  vstmr_tdsstmr = (vstmr_tdsstmr_t) sc->alt_techs[SLPC_TDSCDMA].start_time;
  start = vstmr_tds_seg_to_cx8( vstmr_tdsstmr );
  dur %= VSTMR_TDS_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_TDS_RTC_MODULO;
  tstmr_slam.uw[0] = vstmr_tdsstmr.frac;
  tstmr_slam.uw[1] = slam;
  vstmr_tdsstmr = vstmr_tds_segment_cnt( tstmr_slam.ud );

  tdsstmr_before_sync = vstmr_tds_xo_to_stmr(
                        slpc_data.vstmr_data.tdscdma_view, xo_curr );

  TRACE(SLPC_TDSCDMA, VSTMR_SLAM_CALC, start, dur, slam);
  DBG_3(HIGH, "client:%d VSTMR_RTC_SYNC (xo, rtc):(0x%08x, 0x%08x)",
              SLPC_TDSCDMA, (uint32)ustmr_slam, (uint32)slam);

  vstmr_tds_stmr_sync( slpc_data.vstmr_data.tdscdma_view, vstmr_tdsstmr,
                       ustmr_slam );
  tdsstmr_after_sync = vstmr_tds_xo_to_stmr(
                       slpc_data.vstmr_data.tdscdma_view, xo_curr );
  TRACE(SLPC_TDSCDMA, VSMTR_READ, tdsstmr_before_sync.all, tdsstmr_after_sync.all, ustmr_slam);
  DBG_5(HIGH, "client:%d(TDSCDMA) VSTMR_READ (subframe_no, subframecx8)-> before:(%d, %d) after:"
              "(%d, %d)",
              SLPC_TDSCDMA, (uint32)tdsstmr_before_sync.subframe_no,
              (uint32)tdsstmr_before_sync.subframecx8, (uint32)tdsstmr_after_sync.subframe_no,
              (uint32)tdsstmr_after_sync.subframecx8);

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_TDSCDMA, start, dur, slam);
}
#endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */

/******************************************************************************
  @brief slpc_nb1_vstmr_tstmr_slam

  Performs slam calculations for NB1 alt ID.

  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks

  @return none.
******************************************************************************/
static void slpc_nb1_vstmr_tstmr_slam( slpc_id_type pri_id, uint64 dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[pri_id];

  /* the corrected ustmr value, local container for slam calculations */
  uint64 ustmr_slam, slam;

  /* local container used in slam calculations */
  slpc_dword_type tstmr_slam;

  /* for storing the raw start time */
  uint32 start, xo_curr;

  /* local copy of tech dependent stmr formatted value */
  vstmr_nb1_stmr_t vstmr_nb1_stmr, nb1_stmr_before_sync, nb1_stmr_after_sync;

/*--------------------------------------------------------------------------*/

  xo_curr = (vstmr_get_ustmr() & VSTMR_XO_MASK);

  ustmr_slam = sc->ending.ustmr_ts;

  vstmr_nb1_stmr = (vstmr_nb1_stmr_t) sc->alt_techs[SLPC_NB1].start_time;
  start = vstmr_nb1_seg_to_full_cnt( vstmr_nb1_stmr );
  dur %= VSTMR_NB1_RTC_MODULO;
  slam = start + dur;
  slam %= VSTMR_NB1_RTC_MODULO;
  tstmr_slam.uw[0] = vstmr_nb1_stmr.frac;
  tstmr_slam.uw[1] = slam;
  vstmr_nb1_stmr = vstmr_nb1_segment_cnt( tstmr_slam.ud );

  nb1_stmr_before_sync = vstmr_nb1_xo_to_stmr(
                         slpc_data.vstmr_data.nb1_view, xo_curr );
  TRACE(SLPC_NB1, VSTMR_SLAM_CALC, start, dur, slam);
  DBG_3(HIGH, "client:%d VSTMR_RTC_SYNC (xo, rtc):(0x%08x, 0x%08x)",
              SLPC_NB1, (uint32)ustmr_slam, (uint32)slam);

  vstmr_nb1_stmr_sync( slpc_data.vstmr_data.nb1_view, vstmr_nb1_stmr,
                       ustmr_slam );
  nb1_stmr_after_sync = vstmr_nb1_xo_to_stmr(
                        slpc_data.vstmr_data.nb1_view, xo_curr );
  TRACE(SLPC_NB1, VSMTR_READ, nb1_stmr_before_sync.all, nb1_stmr_after_sync.all, ustmr_slam);
  DBG_7(HIGH, "client:%d(NB1) VSTMR_READ (fullFrNum, subFrNum, subFrCnt)-> before:(%d, %d, %d)"
              " after:(%d, %d, %d)",
              SLPC_NB1, (uint32)nb1_stmr_before_sync.fullFrNum, (uint32)nb1_stmr_before_sync.subFrNum,
              (uint32)nb1_stmr_before_sync.subFrCnt, (uint32)nb1_stmr_after_sync.fullFrNum,
              (uint32)nb1_stmr_after_sync.subFrNum, (uint32)nb1_stmr_after_sync.subFrCnt);

  DBG_4(HIGH, "vstmr slam %d, start=%d, dur=%d, slam=%d", SLPC_NB1, start, dur, slam);
}

/******************************************************************************
  @brief SLPC_VSTMR_SLAM

  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code.

  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none.
******************************************************************************/
void slpc_vstmr_slam( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* For holding the converted duration for the alternate technologies */
  uint64 alt_dur;

  int i;

/*--------------------------------------------------------------------------*/

  /* slam the primary tech */
  sc->vstmr_tstmr_slam_func( id, sc->tstmr_dur );

  TRACE(id, VSTMR_SLAM, sc->tstmr_dur, 0, 0);

  for ( i = 0; i < SLPC_NUM_CLIENTS; i++ )
  {
    if ( sc->alt_techs[i].valid )
    {
      /* calculate the alternate duration */
      alt_dur = (uint64) (sc->tstmr_dur * slpc[i].system_freq);
      alt_dur = (alt_dur + (sc->system_freq >> 1)) / sc->system_freq;

      TRACE(id, VSTMR_SLAM, i, alt_dur, 0);

      /* slam the alternate tech */
      slpc[i].vstmr_tstmr_slam_func( id, alt_dur );
    }
  }
}

/******************************************************************************
  @brief SLPC_UPDATE_XO_SLEEP_CNT

  This function reads the XO SLEEP COUNT in Core and updates the SLEEPING
  TECHs with that value. This is to reset the counter for the current tech
  going to sleep.

  @param none
  @see
  @return none.
******************************************************************************/
static void slpc_update_xo_sleep_cnt( void )
{
  /* loop counter */
  slpc_id_type id;
  uint64 xo_sleep_cnt, tot_xo_sleep_cnt;

  xo_sleep_cnt = rpm_get_xo_accumulated_duration();

  DBG_2(HIGH, "rpm_xo_accumulated_duration, xo_sleep_cnt=0x%x%08x",
        (uint32)(xo_sleep_cnt>>32),
        (uint32)xo_sleep_cnt);

  for ( id = SLPC_INIT; id < SLPC_NUM_CLIENTS; id++ )
  {
    if (slpc[id].state == SLPC_SLEEP_STATE)
    {
      ENTER_TECH_CRIT_SECTION(id);
      tot_xo_sleep_cnt = slpc[id].xo_sleep_cnt + xo_sleep_cnt;
      slpc[id].xo_sleep_cnt = tot_xo_sleep_cnt;
      LEAVE_TECH_CRIT_SECTION(id);

      DBG_3(HIGH, "update_xo_sleep_cnt for tech=%d total_xo_sleep_cnt=0x%x%08x",
            id,
            (uint32)((tot_xo_sleep_cnt)>>32),
            (uint32)(tot_xo_sleep_cnt));
    }
  }

  /* Now that everyone sleeping has been updated, reset the CORE counter */
  rpm_update_xo_drift_counter(FALSE);
}


/******************************************************************************
  @brief SLPC_SET_DURATION_AND_WARMUP
  Sets the sleep duration (in TSTMR ticks) and programs the wakeup timer

  @detail
  IF HW TSTMR: The warmup period is subtracted from the sleep duration
               (ending/OLS) to determine the wakeup point.
  IF VSTMR: RTC is "slammed" at commit_wakeup

  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in tstmr ticks
  @param dur_usec: warmup length, in usec
  @return slpc_err_type
******************************************************************************/
slpc_err_type slpc_set_duration_and_warmup( slpc_id_type id, uint64 tstmr_dur,
                                            uint16 dur_usec )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* return value */
  slpc_err_type err_code = SLPC_ERR_NO_ERR;

  /* Sleep duration computed by the unfiltered accumulated slew error */
  ustmr_type orig_ustmr_dur;

/*--------------------------------------------------------------------------*/

  /* must first do slpc_start, cannot be past wakeup complete */
  if ( sc->state == SLPC_INACTIVE_STATE )
  {
    /* Get a synchronous snapshot of current timer values */
    DBG_1(ERROR, "slpc_set_duration_and_warmup called by %d ", sc->id );
  }
  else
  {
    ENTER_TECH_CRIT_SECTION(id);

    /* Calculate ustmr duration */
    sc->tstmr_dur = tstmr_dur;
    sc->ustmr_dur = (ustmr_type) slpc_tstmr_to_ustmr( id, tstmr_dur, &orig_ustmr_dur );
    sc->ending.tick = sc->start.tick + sc->ustmr_dur;
    sc->ending.ustmr_ts = sc->ending.tick & SLPC_TIMETICK_LOWER_MASK;

    /* Update warmup and wakeup based on new duration */
    if ( sc->warmup.dur_usec != dur_usec )
    {
      sc->warmup.dur_usec = dur_usec;
      sc->warmup.dur_tick = (19200000ULL * sc->warmup.dur_usec) / 1000000ULL;
    }

    TRACE(id, SET_DURATION, sc->ending.tick, sc->tstmr_dur, sc->ustmr_dur);
    TRACE(id, SET_WARMUP, sc->warmup.dur_usec, sc->warmup.dur_tick, 0xFFFF);

    /* Update the wakeup point based on the new sleep duration/warmup */
    err_code = slpc_update_wakeup_point(id);

    LEAVE_TECH_CRIT_SECTION(id);

    /* notify anyone else about this technology dur/wakeup update */
    slpc_call_notify(id);

    /* Updating running sum for TECHs that are already SLEEPING before resetting
       the XO Sleep Counter in Core. This ensures that the current tech has an
       accurate XO sleep count when it wakes up */
    slpc_update_xo_sleep_cnt();

    DBG_6(MED, "Set duration and warmup: id=%d, wake=%u, ols=%u, (UNFILTERED) ols=%u, ustmr=0x%x%08x",
          sc->id,
          (uint32) sc->wakeup.tick,
          (uint32) sc->ending.tick,
          (uint32) (sc->start.tick + orig_ustmr_dur),
          (uint32) ((sc->ustmr_dur)>>32),
          (uint32) (sc->ustmr_dur));

    /* Enter sleep state */
    sc->state = SLPC_SLEEP_STATE;
  }

  return err_code;
}

/******************************************************************************
  @brief SLPC_SET_DURATION

  Sets the sleep duration (in TSTMR ticks) and programs the wakeup timer

  @detail
  IF HW TSTMR: The warmup period is subtracted from the sleep duration
               to determine the wakeup point.
  IF VSTMR: warmup time does not matter, RTC is "slammed" at wakeup

  @param id: identifies the calling client
  @param tstmr_dur: the duration of sleep requested, in tstmr ticks
  @return slpc_err_type
******************************************************************************/
slpc_err_type slpc_set_duration( slpc_id_type id, uint64 tstmr_dur )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* return value */
  slpc_err_type err_code = SLPC_ERR_NO_ERR;

  /* Sleep duration computed by the unfiltered accumulated slew error */
  ustmr_type orig_ustmr_dur;

/*--------------------------------------------------------------------------*/

  /* must first do slpc_start, cannot be past wakeup complete */
  if ( sc->state == SLPC_INACTIVE_STATE )
  {
    /* Get a synchronous snapshot of current timer values */
    DBG_1(ERROR, "slpc_set_duration called by %d", sc->id );
  }
  else
  {
    ENTER_TECH_CRIT_SECTION(id);

    /* Calculate ustmr duration */
    sc->tstmr_dur = tstmr_dur;
    sc->ustmr_dur = (ustmr_type) slpc_tstmr_to_ustmr( id, tstmr_dur, &orig_ustmr_dur );
    sc->ending.tick = sc->start.tick + sc->ustmr_dur;
    sc->ending.ustmr_ts = sc->ending.tick & SLPC_TIMETICK_LOWER_MASK;

    TRACE(id, SET_DURATION, sc->ending.tick, sc->tstmr_dur, sc->ustmr_dur);

    /* Update the wakeup point based on the new sleep duration */
    err_code = slpc_update_wakeup_point(id);

    LEAVE_TECH_CRIT_SECTION(id);

    /* notify anyone else about this technology dur/wakeup update */
    slpc_call_notify(id);

    /* Updating running sum for TECHs that are already SLEEPING before resetting
       the XO Sleep Counter in Core. This ensures that the current tech has an
       accurate XO sleep count when it wakes up */
    slpc_update_xo_sleep_cnt();

    DBG_6(HIGH, "Set duration: id=%d, wake=%u, ols=%u (UNFILTERED) ols=%u, ustmr=0x%x%08x",
          sc->id,
          (uint32) sc->wakeup.tick,
          (uint32) sc->ending.tick,
          (uint32) (sc->start.tick + orig_ustmr_dur),
          (uint32) ((sc->ustmr_dur)>>32),
          (uint32) (sc->ustmr_dur));

    /* Enter sleep state */
    sc->state = SLPC_SLEEP_STATE;
  }

  return err_code;
}

/******************************************************************************
  @brief SLPC_SET_WARMUP

  Set the warmup period. This determines amount of time prior to the
  ending point that the wakeup callback should occur.

  @param id: identifies the calling client
  @param warmup_usec: warmup period in usec
  @return slpc_err_type
******************************************************************************/
slpc_err_type slpc_set_warmup( slpc_id_type id, uint16 dur_usec )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* return value */
  slpc_err_type err_code = SLPC_ERR_NO_ERR;

/*--------------------------------------------------------------------------*/

  /* Convert microseconds to slow clocks only if warmup has changed */
  if ( sc->warmup.dur_usec != dur_usec )
  {
    ENTER_TECH_CRIT_SECTION(id);

    sc->warmup.dur_usec = dur_usec;
    sc->warmup.dur_tick = (19200000ULL * sc->warmup.dur_usec) / 1000000ULL;

    TRACE(id, SET_WARMUP, sc->warmup.dur_usec, sc->warmup.dur_tick, 0);

    /* Only update the wakeup point if we're SLEEP */
    if ( sc->state == SLPC_SLEEP_STATE )
    {
      /* Update the wakeup point based on the new warmup time */
      err_code = slpc_update_wakeup_point(id);

      LEAVE_TECH_CRIT_SECTION(id);

      /* notify anyone else about this technology dur/wakeup update */
      slpc_call_notify(id);
    }
    else
    {
      LEAVE_TECH_CRIT_SECTION(id);
    }
  }

  return err_code;
}

/******************************************************************************
  @brief SLPC_SET_NOTIFY_CALLBACK
  Set a callbacks for anyone wanting to track wakeup point changes,
  called whenever any tech sets a wakeup point

  @param n_cb: callback
  @return none
******************************************************************************/
void slpc_set_notify_callback( slpc_notify_cb_type n_cb )
{
  uint8 i = 0;

/*--------------------------------------------------------------------------*/

  ENTER_GLOBAL_CRIT_SECTION();
  while ( slpc_data.notify_cb[i++] != NULL );
  i--;

  if ( i < SLPC_MAX_WAKEUP_CALLBACKS )
  {
    /* Save the notify callback */
    slpc_data.notify_cb[i] = n_cb;
  }
  else
  {
    ERR_FATAL("no room for notify callback allocation", 0, 0, 0);
  }
  LEAVE_GLOBAL_CRIT_SECTION();

}

/******************************************************************************
  @brief SLPC_SET_NOTIFY_WAKEUP_CALLBACK

  This callback will be called at any tech's wakeup event. For anyone
  wanting to track wakeups.

  @param id: n_cb: notify callback

  @return none.
******************************************************************************/
void slpc_set_notify_wakeup_callback( slpc_notify_wakeup_cb_type n_cb )
{
  uint8 i = 0;

/*--------------------------------------------------------------------------*/

  ENTER_GLOBAL_CRIT_SECTION();
  while ( slpc_data.notify_wakeup_cb[i++] != NULL );
  i--;

  if ( i < SLPC_MAX_WAKEUP_CALLBACKS )
  {
    /* Save the notify callback */
    slpc_data.notify_wakeup_cb[i] = n_cb;
  }
  else
  {
    ERR_FATAL("no room for wakeup notify callback allocation", 0, 0, 0);
  }
  LEAVE_GLOBAL_CRIT_SECTION();
}

/******************************************************************************
  @brief SLPC_DEREGISTER_NOTIFY_CALLBACK

  Clear a previously set notify callback

  @param n_cb: callback
  @return none
******************************************************************************/
void slpc_deregister_notify_callback( slpc_notify_cb_type n_cb )
{
  uint8 i;

/*--------------------------------------------------------------------------*/

  for ( i = 0; i < SLPC_MAX_WAKEUP_CALLBACKS; i++ )
  {
    if ( slpc_data.notify_cb[i] == n_cb )
    {
      slpc_data.notify_cb[i] = NULL;
    }
  }
}

/******************************************************************************
  @brief SLPC_DEREGISTER_NOTIFY_WAKEUP_CALLBACK

  Deregister the wakeup callback, if previously set

  @param id: n_cb: notify callback

  @return none.
******************************************************************************/
void slpc_deregister_notify_wakeup_callback( slpc_notify_wakeup_cb_type n_cb )
{
  uint8 i;

/*--------------------------------------------------------------------------*/

  for ( i = 0; i < SLPC_MAX_WAKEUP_CALLBACKS; i++ )
  {
    if ( slpc_data.notify_wakeup_cb[i] == n_cb )
    {
      slpc_data.notify_wakeup_cb[i] = NULL;
    }
  }
}

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
void slpc_set_wakeup_callback( slpc_id_type id, slpc_event_callback_type w_cb )
{
  /* Save the wakeup event callback */
  slpc[id].wakeup.cb = w_cb;

}

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
void slpc_set_ols_callback( slpc_id_type id, slpc_event_callback_type e_cb )
{
  DBG_1(ERROR, "API is deprecated, Client ID=%d", id);

  /* Save the ols event callback */
  slpc[id].ending.cb = e_cb;

}

/******************************************************************************
  @brief SLPC_WAKEUP_EVENT

  Wakeup triggered by core timer (ending point - warmup time). Call the
  client wakeup callbacks. Clients can either extend sleep or commit
  to wakeup.

  @param id: identifies the calling client
  @return none
******************************************************************************/
void slpc_wakeup_event( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];
  uint64 xo_sleep_cnt, tot_xo_sleep_cnt;

/*--------------------------------------------------------------------------*/

  ENTER_TECH_CRIT_SECTION(id);

  if ( sc->state != SLPC_SLEEP_STATE )
  {
    LEAVE_TECH_CRIT_SECTION(id);
    sc->wakeup.invalid_cnt++;
    DBG_2(ERROR, "Wakeup in wrong state %d, cnt=%d", sc->state,
          sc->wakeup.invalid_cnt);
    return;
  }

  /* Capture the timestamp when the wakeup ISR starts */
  sc->wakeup.cb_start_tick = slpc_get_tick();

  TRACE(id, WAKEUP_START, sc->state, sc->wakeup.tick, sc->wakeup.cb_start_tick);

  /* Reasonable wakeup? */
  if ( (sc->wakeup.cb_start_tick + SLPC_WAKEUP_TIMER_ERR) >= sc->wakeup.tick )
  {
    /* Update the sleep controller's software state to "wakeup" state */
    sc->state = SLPC_WAKEUP_STATE;
    xo_sleep_cnt = rpm_get_xo_accumulated_duration();

    tot_xo_sleep_cnt = sc->xo_sleep_cnt + xo_sleep_cnt;
    sc->xo_sleep_cnt = tot_xo_sleep_cnt;

    LEAVE_TECH_CRIT_SECTION(id);

    DBG_5(HIGH, "wakeup_event tech=%d, XO sleep count current=0x%x%08x, total=0x%x%08x",
          id,
          (uint32)(xo_sleep_cnt>>32),
          (uint32)xo_sleep_cnt,
          (uint32)((tot_xo_sleep_cnt)>>32),
          (uint32)(tot_xo_sleep_cnt));

    /* Invoke the wakeup callback */
    if (sc->wakeup.cb != NULL )
    {
      sc->wakeup.cb();
    }
  }
  else
  {
    LEAVE_TECH_CRIT_SECTION(id);
    DBG_3(ERROR, "%d Wakeup ignored!  now=%u, wake=%u", sc->id,
          (uint32) sc->wakeup.cb_start_tick, (uint32) sc->wakeup.tick);
  }

  /* Capture the timestamp when the wakeup ISR ends */
  sc->wakeup.cb_end_tick = slpc_get_tick();

  /* notify anyone else about this technology wakeup event */
  slpc_call_notify_wakeup(id);

  DBG_3(MED, "Wakeup event: %d, start=%u, end=%u", sc->id,
        (uint32) sc->wakeup.cb_start_tick, (uint32) sc->wakeup.cb_end_tick);

  TRACE(id, WAKEUP_END, sc->wakeup.tick,
          sc->wakeup.cb_start_tick, sc->wakeup.cb_end_tick);
}

/******************************************************************************
  @brief SLPC_SUFFICIENT_WARMUP_TIME

  May be called by the wakeup_cb to determine whether or not the wakeup
  event is being processed too late.

  @param id: identifies the calling client
  @return boolean
******************************************************************************/
boolean slpc_sufficient_warmup_time ( slpc_id_type id, uint32 min_warmup )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* Current timetick */
  uint64 tick;

  /* convert from usec to xo ticks */
  int64 min_ticks = (min_warmup * 19200000uLL) / 1000000uLL;

/*--------------------------------------------------------------------------*/

  tick = slpc_get_tick();
  if ( ( (int64) sc->ending.tick - (int64) tick) > min_ticks ) return TRUE;

  return FALSE;
}

/******************************************************************************
  @brief SLPC_COMPLETE_WAKEUP

  May be called by the wakeup_cb to commit to the wakeup if there is
  sufficient warmup time remaining.

  @param id: identifies the calling client
  @param min_warmup: min required warmup time (usec)
  @param tstmr: pointer to TSTMR for SLPC to program, or NULL if client
                will handle this
  @return FALSE if not enough time
  @return TRUE if succeeded
******************************************************************************/
boolean slpc_complete_wakeup ( slpc_id_type id, uint32 min_warmup,
                               slpc_tstmr_type *tstmr )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

  /* Current timetick */
  uint64 tick;

  /* Current relative location to ending tick */
  int64 delta;

  /* convert from usec to xo ticks */
  int64 min_ticks = (min_warmup * 19200000uLL) / 1000000uLL;

/*--------------------------------------------------------------------------*/

    /* virtually slam the tech stmr */
    slpc_vstmr_slam( id );
    sc->state = SLPC_INACTIVE_STATE;
    TRACE(id, COMPLETE_WAKEUP, min_warmup, sc->xo_sleep_cnt, 0);
    DBG_0(HIGH, "slpc_complete_wakeup called");
    /* Record the sleep duration, for error_feedback */
    sc->last_sleep.duration_tstmr = sc->tstmr_dur;
    sc->last_sleep.duration_ustmr = sc->ustmr_dur;
    sc->last_sleep.xo_sleep_cnt = sc->xo_sleep_cnt;
    sc->xo_sleep_cnt = 0;

    if (sc->last_sleep.xo_sleep_cnt > sc->last_sleep.duration_ustmr)
    {
      DBG_4(HIGH, "Last sleep: xo_sleep_cnt:0x%x%08x is greater than duration_ustmr:0x%x%08x",
            (uint32)((sc->last_sleep.xo_sleep_cnt) >> 32), (uint32)(sc->last_sleep.xo_sleep_cnt),
            (uint32)((sc->last_sleep.duration_ustmr) >> 32), (uint32)(sc->last_sleep.duration_ustmr));
    }

    return TRUE;

}

/******************************************************************************
  @brief SLPC_ABORT

  Stop the current sleep cycle and reset the sleep controller state

  @param id: identifies the calling client
  @return none.
******************************************************************************/
void slpc_abort ( slpc_id_type id )
{
  /* Sleep controller structure */
  slpc_client_type *sc = &slpc[id];

/*--------------------------------------------------------------------------*/

  TRACE(id, ABORT, sc->state, sc->wakeup.tick, 0);

  ENTER_TECH_CRIT_SECTION(id);

  /* Record the sleep duration, for error_feedback */
  sc->last_sleep.duration_tstmr = sc->tstmr_dur;
  sc->last_sleep.duration_ustmr = sc->ustmr_dur;
  sc->last_sleep.xo_sleep_cnt = sc->xo_sleep_cnt;

  if ( (sc->state == SLPC_START_STATE) ||
       (sc->state == SLPC_SLEEP_STATE) )
  {
    timer_clr( &sc->wakeup.timer, T_NONE );
  }

  /* Update the sleep controller's software state to "inactive" state */
  sc->state = SLPC_INACTIVE_STATE;

  LEAVE_TECH_CRIT_SECTION(id);
}

/******************************************************************************
  @brief SLPC_TASK_COMMON

  SLPC task function -- Initialize and loop forever

  @param arg: Notused
  @return none.
******************************************************************************/
void slpc_task_common( void *arg )
{
  unsigned int  sigs;
  int32         thread_idx;

/*--------------------------------------------------------------------------*/
  NOTUSED(arg);

  rcinit_handshake_startup();

  /* Allocate the thread idx to each thread */
  thread_idx = atomic_inc_return(&slpc_thread_pool.thread_idx);

  ASSERT(thread_idx < SLPC_NUM_THREADS);

  while( TRUE )
  {
    sigs = qurt_signal_wait_any(&(slpc_thread_pool.task_sig[thread_idx]), (SLPC_ALL_SIGS));
    qurt_signal_clear(&(slpc_thread_pool.task_sig[thread_idx]), sigs);

    if (sigs & SLPC_WAKEUP_TIMER_EXP_SIG)
    {
      /* Call wakeup cb events  */
      slpc_wakeup_event(slpc_thread_pool.thread_to_client_map[thread_idx]);

      TRACE(slpc_thread_pool.thread_to_client_map[thread_idx], THREAD_RELEASE,
            atomic_read(&(slpc_thread_pool.active_mask)), thread_idx, 0);

      /* Clear the thread_idx atomically */
      atomic_clear_bit(&(slpc_thread_pool.active_mask), thread_idx);

      /* Reinitialise the client id back to default. */
      slpc_thread_pool.thread_to_client_map[thread_idx] = SLPC_NUM_CLIENTS;
    }
  }
}

/******************************************************************************
  @brief SLPC_INIT

  Startup initialization for SLPC module, called at modem bootup using RCINIT.

  @param none.
  @see
  @return none.
******************************************************************************/
void slpc_init( void )
{
  /* Loop variable */
  uint8 i, id;

  /* make sure initialization is done only once */
  static boolean slpc_initialized = FALSE;

/*--------------------------------------------------------------------------*/

  if ( slpc_initialized )
  {
    return;
  }

  slpc_initialized = TRUE;

  INIT_GLOBAL_CRIT_SECTION();

  /* Initialize slpc_thread_pool structure */
  for ( id = 0; id < SLPC_NUM_THREADS; id++ )
  {
    qurt_signal_init( &slpc_thread_pool.task_sig[id] );
    slpc_thread_pool.thread_to_client_map[id] = SLPC_NUM_CLIENTS;
  }
  atomic_set(&slpc_thread_pool.active_mask, 0);
  atomic_set(&slpc_thread_pool.thread_idx, -1);

  if( ( DalTimetick_Attach("SystemTimer", &slpc_data.tt_handle)
        != DAL_SUCCESS ) || ( slpc_data.tt_handle == NULL ) )
  {
    ERR_FATAL("Cannot attach to timetick driver!", 0, 0, 0);
  }

  slpc_trace_init();

  for ( id = 0; id < SLPC_NUM_CLIENTS; id++ )
  {
    INIT_TECH_CRIT_SECTION(id);

    timer_group_set_deferrable(&slpc[id].wakeup.slpc_timer_group, FALSE);
    timer_def_priority( &slpc[id].wakeup.timer, &slpc[id].wakeup.slpc_timer_group,
               NULL, 0, (timer_t1_cb_type) &slpc_wakeup_timer_cb,
               (timer_cb_data_type) id, TIMER_PRIORITY_0);

    /* Initialize the sleep controller's state */
    slpc[id].state = SLPC_INACTIVE_STATE;
    slpc[id].slew_err_accum = 0;
    slpc[id].unfiltered_slew_err_accum = 0;
    slpc[id].wakeup.invalid_cnt = 0;
    slpc[id].warmup.dur_tick = 0;
    slpc[id].warmup.dur_usec = 0;

    for ( i = 0; i < SLPC_NUM_CLIENTS; i++ )
    {
      slpc[id].alt_techs[i].valid = FALSE;
      slpc[id].alt_techs[i].tstmr = 0;
    }
    slpc[id].xo_sleep_cnt = 0;
    slpc[id].use_xo_sleep_cnt = FALSE;
  }

  #ifndef FEATURE_BUILD_PAGEONLY_IMAGE
  slpc_data.vstmr_data.gsm_view1 =
    vstmr_geran_get_view_handle(VSTMR_RTC_GERAN_SUB0, VSTMR_GERAN_GSTMR_VIEW);
  slpc_data.vstmr_data.gsm_view2 =
    vstmr_geran_get_view_handle(VSTMR_RTC_GERAN_SUB1, VSTMR_GERAN_GSTMR_VIEW);
  slpc_data.vstmr_data.gsm_view3 =
    vstmr_geran_get_view_handle(VSTMR_RTC_GERAN_SUB1, VSTMR_GERAN_GSTMR_VIEW);
  slpc_data.vstmr_data.wcdma_view =
    vstmr_wcdma_get_view_handle(VSTMR_RTC_WCDMA_SUB0, VSTMR_WCDMA_REF_COUNT_VIEW);
  slpc_data.vstmr_data.wcdma_view2 =
    vstmr_wcdma_get_view_handle(VSTMR_RTC_WCDMA_SUB1, VSTMR_WCDMA_REF_COUNT_VIEW);
  slpc_data.vstmr_data.onex_view =
    vstmr_1x_get_view_handle(VSTMR_RTC_1X_SUB0, VSTMR_1X_RTC_VIEW);
  slpc_data.vstmr_data.hdr_view =
    vstmr_hdr_get_view_handle(VSTMR_RTC_HDR_SUB0, VSTMR_HDR_RTC_VIEW);
  slpc_data.vstmr_data.tdscdma_view =
    vstmr_tds_get_view_handle(VSTMR_RTC_TDS_SUB0, VSTMR_TDS_WALL_TIME_VIEW);
  #endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */
  slpc_data.vstmr_data.lte_view =
    vstmr_lte_get_view_handle(VSTMR_RTC_LTE_SUB0, VSTMR_LTE_OSTMR_VIEW);
  slpc_data.vstmr_data.nb1_view =
    vstmr_nb1_get_view_handle(VSTMR_RTC_NB1, VSTMR_NB1_REF_COUNT_VIEW);
}

/******************************************************************************
  @brief SLPC_SET_USE_XO_SLEEP_CNT

  Sets the flag for using XO sleep count to determine next wake up point

  @param id: identifies the calling client
  @param flag: TRUE or FALSE
  @return NONE
******************************************************************************/
boolean slpc_set_use_xo_sleep_cnt( slpc_id_type id, boolean flag )
{
  boolean ret_val = FALSE;
  if (id < SLPC_INIT || id >= SLPC_NUM_CLIENTS)
  {
    DBG_1(ERROR, "Invalid tech id %d for setting xo sleep cnt flag", id );
  }
  else
  {
    slpc[id].use_xo_sleep_cnt = flag;
    ret_val = TRUE;
  }
  return ret_val;
}

/******************************************************************************
  @brief SLPC_GET_ACCUM_SLEW_ERR

  Returns the accumulated slew error for the specified technology

  @param id: identifies the calling client
  @param Pointer to return accumulated slew error in PPM - Q10 format
  @return TRUE  if slew error is successfully retrieved
          FALSE if slew error cannot be retrieved due to invalid slpc id.
******************************************************************************/
boolean slpc_get_accum_slew_err( slpc_id_type id, int32 *accum_err )
{
  boolean ret_val = FALSE;
  if (id < SLPC_INIT || id >= SLPC_NUM_CLIENTS)
  {
    DBG_1(ERROR, "Invalid tech id %d for retrieving accumulated error", id );
  }
  else
  {
    *accum_err = slpc[id].slew_err_accum;
    ret_val = TRUE;
  }
  return ret_val;
}

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
boolean slpc_set_accum_slew_err( slpc_id_type id, int32 err_value )
{
  boolean ret_val = FALSE;
  if (id < SLPC_INIT || id >= SLPC_NUM_CLIENTS)
  {
    DBG_2(ERROR, "Invalid tech id %d for setting accumulated error of %d PPM",
          id, err_value );
  }
  else
  {
    slpc[id].slew_err_accum = err_value;
    ret_val = TRUE;
    DBG_2(HIGH, "Set accumulated slew error for tech id %d to %d PPM",
          id, err_value );
  }
  return ret_val;
}

/******************************************************************************
  @brief SLPC_SET_XO_TRIM_COMP_OFFSET

  Updates the TRIM value by applying the specified offset to compenssate
  for XO capacitance changein PMIC during sleep

  @param id: identifies the calling client
  @param offset: offset to apply to the current XO TRIM
  @return TRUE  if setting the offset is successful
          FALSE if offset cannot be set as specified
******************************************************************************/
boolean slpc_set_xo_trim_comp_offset( slpc_id_type id, int8 offset )
{
  boolean ret_val = FALSE;
  #ifndef FEATURE_BUILD_PAGEONLY_IMAGE
  pm_err_flag_type err_flag;
  if (id < SLPC_INIT || id >= SLPC_NUM_CLIENTS)
  {
    DBG_2(ERROR, "Invalid tech id %d for setting XO trim compensation of %d",
          id, offset );
  }
  else
  {
    err_flag = pm_xo_core_set_xo_comp(0, offset);
    if (err_flag == PM_ERR_FLAG__SUCCESS)
    {
      ret_val = TRUE;
      DBG_2(HIGH, "Tech %d set XO trim compensation of %d", id, offset );
      slpc_xo_comp_data.id = id;
      slpc_xo_comp_data.offset = offset;
    }
    else
    {
      DBG_2(ERROR, "Core API for setting XO trim compensation of %d returned error cause %d",
          offset, (uint32)err_flag);
    }
  }
  #else 
    NOTUSED(id);
    NOTUSED(offset);
    DBG_0( ERROR, "slpc_set_xo_trim_comp_offset called in PAGE ONLY" );
  #endif /* !FEATURE_BUILD_PAGEONLY_IMAGE */
  return ret_val;
}

/******************************************************************************
  @brief SLPC_GET_XO_TRIM_COMP_OFFSET

  Retrieves the current TRIM offset configured in Core to compensate for XO
  capacitance change during sleep

  @return offset: offset being applied to the current XO TRIM

******************************************************************************/
int8 slpc_get_xo_trim_comp_offset( void )
{
  DBG_2(HIGH, "Current XO trim compensation of %d was set by tech %d",
        slpc_xo_comp_data.offset, slpc_xo_comp_data.id);
  return slpc_xo_comp_data.offset;
}


