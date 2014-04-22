/*****************************************************************************
***
*** TITLE
***
***  GPRS LAYER 2 SERVICES CONTROL TASK FUNCTIONS
***
***
*** DESCRIPTION
***
***  REX task based scheduling framework for GPRS LLC functionality.
***
***
*** EXTERNALIZED FUNCTIONS
***
***  gllc_task()
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  REX initialisation of the task control blocks and stacks is the
***  function of the TMC Task Mode Controller.
***
***
*** Copyright (c) 2001-2017 Qualcomm Technologies, Inc.
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
*** $Header: //components/rel/geran.mpss/5.2.0/gllc/static/gllc_static_main.c#9 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "rcinit.h"
#include "dog_hb_rex.h"
#include "gllc_static_main.h"
#include "gprs_mem.h"

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
#include "geran_dmm.h"
#endif

/*****************************************************************************
***
***     EXTERNAL DEFINITIONS AND TYPES
***
*****************************************************************************/

/* LLC critical section to protect accesses to LLC messages and internal vars */
rex_crit_sect_type gllc_crit_sec;
rex_crit_sect_type gllc_ul_msg_crit_sec;

rex_sigs_type      gllc_task_sigs = 0;

/* Up Link Event Bitmap Record Declaration.
   For the recording of active NON LLE specific events.
   Use appropriate bitmap definitions in gllci.h. */
uint8              gllc_ul_event_map = GLLC_UL_NO_EVT;

boolean            gllc_initialised = FALSE;
boolean            llc_log_initialized = FALSE;

/* SAPI address to LLE translator. */
const gllc_ll_sapi_t gllc_lle_lookup[ 16 ] =
{
  GLLC_LL_SAPI_RESERVED,    /*  0 */
  GLLC_LL_SAPI_1_GMM,       /*  1 */
  GLLC_LL_SAPI_RESERVED,    /*  2 */
  GLLC_LL_SAPI_3_LL3,       /*  3 */
  GLLC_LL_SAPI_RESERVED,    /*  4 */
  GLLC_LL_SAPI_5_LL5,       /*  5 */
  GLLC_LL_SAPI_RESERVED,    /*  6 */
  GLLC_LL_SAPI_7_SMS,       /*  7 */
  GLLC_LL_SAPI_RESERVED,    /*  8 */
  GLLC_LL_SAPI_9_LL9,       /*  9 */
  GLLC_LL_SAPI_RESERVED,    /* 10 */
  GLLC_LL_SAPI_11_LL11,     /* 11 */
  GLLC_LL_SAPI_RESERVED,    /* 12 */
  GLLC_LL_SAPI_RESERVED,    /* 13 */
  GLLC_LL_SAPI_RESERVED,    /* 14 */
  GLLC_LL_SAPI_RESERVED     /* 15 */
};

/* ....................................................................... */

const uint16 gllc_max_N201_U_octets[ GLLC_LL_NUM_SAPIS ] =
{
  1520,                   /* GLLC_LL_SAPI_1_GMM   */
  1520,                   /* GLLC_LL_SAPI_3_LL3   */
  1520,                   /* GLLC_LL_SAPI_5_LL5   */
  1520,                   /* GLLC_LL_SAPI_7_SMS   */
  1520,                   /* GLLC_LL_SAPI_9_LL9   */
  1520                    /* GLLC_LL_SAPI_11_LL11 */
};

/* ===========================================================================
   LLC Up Link Timer Storage

   REX extended timers for each LLE.
   ======================================================================== */
rex_timer_type       gllc_timer_t200[GLLC_LL_NUM_SAPIS];
rex_timer_type       gllc_timer_t201[GLLC_LL_NUM_SAPIS];
rex_timer_type       gllc_gmm_ready_timer;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
gllc_static_data_t gllc_static_data;
static q_type      llc_dyn_pendq;
#endif /*FEATURE_EXTERNAL_PAGING_GERAN*/

/* ===========================================================================
   Up Link Event Queue Definitions
   ======================================================================== */

/* LLME GMM SAP Messaging Queue Declaration.
   For receipt of LLGMM service primitives from the GMM entity via the
   LLGMM SAP. */
q_type                gllc_gmm_ul_q;

/* ------------------------------------------------------------------------ */

/* LLE C/R Messaging Queue Array Declaration

   For receipt of down link originated commands and responses from the peer
   LLE. */

q_type                gllc_cr_ul_q[GLLC_LL_NUM_SAPIS];

/* ------------------------------------------------------------------------ */

/* LLE LL SAP Signalling Messaging Queue Array Declaration

   For receipt of LL service primitives (other than LL-UNITDATA-REQ &
   LL-DATA-REQ) from layer 3 network entities via the LL SAP interfaces. */

q_type                gllc_ll_ul_sig_q[GLLC_LL_NUM_SAPIS];

/* ------------------------------------------------------------------------ */

/* LLE LL SAP Acknowledged Data Messaging Queue Array Declaration

   For receipt of LL-DATA-REQ service primitives from layer 3 network
   entities via the LL SAP interfaces. */

dsm_watermark_type   gllc_ll_ul_ack_wm[GLLC_LL_NUM_SAPIS];
q_type               gllc_ll_ul_ack_q[GLLC_LL_NUM_SAPIS];

/* ------------------------------------------------------------------------ */

/* LLE LL SAP Unacknowledged Data Messaging Queue Array Declaration

   For receipt of LL-UNITDATA-REQ service primitives from layer 3 network
   entities via the LL SAP interfaces. */

dsm_watermark_type   gllc_ll_ul_unack_wm[GLLC_LL_NUM_SAPIS];
q_type               gllc_ll_ul_unack_q[GLLC_LL_NUM_SAPIS];

/* ===========================================================================
   Down Link Event Queue Declaration
   ======================================================================== */

/* LLE GRR SAP Messaging Queue Declaration

   For receipt of GRR service primitives from the RLC/MAC layer via the GRR
   SAP interface. */

q_type                gllc_grr_dl_q;

/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/
static void gllc_initialise(void);

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
gllc_static_data_t* gllc_static_data_get_ptr(void)
{
  return &gllc_static_data;
}
#endif /*FEATURE_EXTERNAL_PAGING_GERAN*/

void gllc_static_timer_callback(uint32 lle_and_timer_evt)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN  
  if (is_geran_loaded())
  {
    gllc_timer_callback_1(lle_and_timer_evt);
  }
  else
  {
    gllc_static_data.gllc_expired_timers[gllc_static_data.gllc_expired_timers_cnt++] = lle_and_timer_evt;
  }
#else /* FEATURE_EXTERNAL_PAGING_GERAN */
  gllc_timer_callback_1(lle_and_timer_evt);
#endif /* !FEATURE_EXTERNAL_PAGING_GERAN */
}

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
void gllc_send_to_dyn_pendq(void* msg_ptr, gllc_ext_msg_type type)
{
  llc_ext_msg_t* pend_data_ptr = GPRS_MEM_MALLOC(sizeof(llc_ext_msg_t)/sizeof(uint8));

  if (pend_data_ptr != NULL && msg_ptr != NULL)
  {
    pend_data_ptr->msg = msg_ptr;
    pend_data_ptr->msg_type = type;

    /* Put recently received message in Dynamic pending Queue */
    (void)q_link(pend_data_ptr, &pend_data_ptr->link);
    q_put(&llc_dyn_pendq, &pend_data_ptr->link);
  }
  else
  {
    ERR_GERAN_FATAL_0_G( "GLC UL Pendq Put: Heap Exhaustion");
  }
}

void gllc_clear_dyn_pendq()
{
  llc_ext_msg_t* pend_data_ptr = (llc_ext_msg_t*)q_get(&llc_dyn_pendq);

  while(pend_data_ptr != NULL)
  {
    GPRS_MEM_FREE(pend_data_ptr->msg);
    GPRS_MEM_FREE(pend_data_ptr);
    pend_data_ptr = (llc_ext_msg_t*)q_get(&llc_dyn_pendq);
  }
}

#endif
/*===========================================================================
===
===  FUNCTION      GSM_LLC_TASK_X_FN_INIT()
===
===  DESCRIPTION
===
===  Only use this mechanism to perform memory init, hardware init, or other
===  startup init to satisfy dependency that follow. Avoid all HLOS mechanisms
===  that will block.
===
===  Code in this function will add to startup latency. Do not use large loops
===  that can be avoided.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/
void gsm_llc_task_x_fn_init(void)
{
  /* Initialise LLC critical section */
  rex_init_crit_sect(&gllc_crit_sec);
  rex_init_crit_sect(&gllc_ul_msg_crit_sec);
}

/*===========================================================================
===
===  FUNCTION      GLLC_INITIALISE()
===
===  DESCRIPTION
===
===    For use by LLC control task to initialise the LLC Process.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

static void gllc_initialise(void)
{
  uint32  lle;
  uint32  llme_ul_scheme;
  uint32  lle_ul_scheme;

  /* --------------------------------------------------------------------- */

  geran_tcb_set(GERAN_TASK_ID_GLLC_1, rex_self());
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  (void)q_init(&llc_dyn_pendq);
#else
  geran_gllc_init();
#endif
}

/*===========================================================================
===
===  FUNCTION      GLLC_TASK_X()
===
===  DESCRIPTION
===
===    REX task based scheduling framework for GPRS LLC functionality for
===    subscriptions.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gllc_task_x(void)
{
  dog_report_type     gllc_dog_rpt;

  /* Perform startup init here. Satisfy dependency of other tasks
     provided by this task. Do not linger here, startup latency is
     impacted. Higher priority tasks in the same group will preempt
     this task at this point. Do not rely upon priority alone as the
     sole mechanism to arbitrate resource contention with other tasks. */

  rcinit_handshake_startup(); /* required rcinit handshake */

  gsm_llc_task_x_fn_init();
  gllc_initialise();
  gllc_dog_rpt = dog_hb_register_rex((rex_sigs_type)GLLC_DOG_RPT_TMR_SIG);

  /* Begin event based processing via REX scheduling. */
  for (;;)
  {
    /* Recind processor context until a relevent signal(s) becomes active. */

    gllc_task_sigs =
                      rex_wait
                      (
                        (
                          TASK_START_SIG       |
                          TASK_STOP_SIG        |
                          TASK_OFFLINE_SIG     |
                          GLLC_DOG_RPT_TMR_SIG |
                          GLLC_UL_MASTER_SIG   |
                          GLLC_DL_MASTER_SIG   |
                          GLLC_GMM_RDY_TMR_SIG |
                          GLLC_TEST_MODE_SIG   |
                          GLLC_DYN_RESUME_SIG
                        )
                      );

    /* Process any active signals with the following priority. */

    if ( gllc_task_sigs & TASK_START_SIG )
    {
      (void)rex_clr_sigs( rex_self(), TASK_START_SIG );
    }

    if ( gllc_task_sigs & TASK_STOP_SIG )
    {
      /* Used in QTF. Clear down the signal and stop task to fix KW error. */
      (void)rex_clr_sigs( rex_self(), TASK_STOP_SIG );
      rcinit_handshake_term();
      break;
    }

    if ( gllc_task_sigs & TASK_OFFLINE_SIG )
    {
      /* Not used at present so just clear down the signal. */
      (void)rex_clr_sigs( rex_self(), TASK_OFFLINE_SIG );
    }

    if ( gllc_task_sigs & GLLC_DOG_RPT_TMR_SIG )
    {
      (void)rex_clr_sigs( rex_self(), GLLC_DOG_RPT_TMR_SIG );
      dog_hb_report( gllc_dog_rpt );
    }

#ifdef FEATURE_EXTERNAL_PAGING_GERAN  
    if (is_geran_loaded() == TRUE)
#endif
    {
      if ( gllc_task_sigs & GLLC_GMM_RDY_TMR_SIG )
      {
        gllc_gmm_ready_timer_handler();
      }

#ifdef FEATURE_EXTERNAL_PAGING_GERAN  
      if ( gllc_task_sigs & GLLC_DYN_RESUME_SIG )
      {
        uint8 cnt = 0;

        for (cnt = 0; cnt < gllc_static_data.gllc_expired_timers_cnt; cnt++)
        {
          gllc_timer_callback_1(gllc_static_data.gllc_expired_timers[cnt]);
        }
        gllc_static_data.gllc_expired_timers_cnt = 0;

        if (gllc_static_data.gllc_rdy_tmr_expired)
        {
          gllc_gmm_ready_timer_handler();
          gllc_static_data.gllc_rdy_tmr_expired = FALSE;
        }
        
        gllc_process_dyn_pendq(&llc_dyn_pendq);
        (void)rex_clr_sigs( rex_self(), GLLC_DYN_RESUME_SIG );
      }
#endif

      if ( gllc_task_sigs & GLLC_TEST_MODE_SIG )
      {
        gllc_test_mode_handler();
      }
  
      if ( gllc_task_sigs & (GLLC_UL_MASTER_SIG | GLLC_DL_MASTER_SIG) )
      {
        gllc_event_handler();
      }
    }
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
    else
    {
      /* GERAN is not loaded in memory */
      if ( gllc_task_sigs & GLLC_GMM_RDY_TMR_SIG )
      {
        /* Clear the signal,otherwise task will always remain active */
        (void)rex_clr_sigs( rex_self(), GLLC_GMM_RDY_TMR_SIG );

        /* Set the flag so that expiry handling will take place once we are loaded */
        gllc_static_data.gllc_rdy_tmr_expired = TRUE;
      }
    }
#endif /* FEATURE_EXTERNAL_PAGING_GERAN */
 }

  dog_hb_deregister( gllc_dog_rpt );
  /*UNREACHED*/
}

/*===========================================================================
===
===  FUNCTION      GLLC_TASK_1()
===
===  DESCRIPTION
===
===    REX task based scheduling framework for GPRS LLC functionality for 1st
===    subscription.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/
void gllc_task_1
(
  /* Required for REX, ignore. */

  /*lint -esym(715,unused) Prevent lint from complaining about the unused
    parameter 'dummy' which is required to conform to the rex_def_task()
    definition. */

  dword  unused
)
{
  NOTUSED(unused); /* silence compiler warnings of unused parameter */
  gllc_task_x();
}

/*** EOF: don't remove! ***/
