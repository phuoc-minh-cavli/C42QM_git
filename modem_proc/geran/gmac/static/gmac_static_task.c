/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC task
***
*** DESCRIPTION
***
***  Handles the initialisation of the MAC task and the signal reception
***  handler.
***
*** EXTERNALIZED FUNCTIONS
***
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
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
*** $Header: //components/rel/geran.mpss/5.2.0/gmac/static/gmac_static_task.c#7 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 01/05/18   asr     Created for Dynamic module loading
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h"

#include "geran_tasks.h"
#include "gprsdef.h"
#include "gprsdef_g.h"

/* rex and system type operations */
#include "rex.h"
#include "task.h"
#include "rcinit.h"

/* for messaging */
#include "queue.h"
#include "dsm.h"
#include "memory.h"
#include "gprs_mem.h"
#include "string.h"

/* for debuging */
#include "msg.h"
#include "err.h"
#include "amssassert.h"

#include "gmac.h"
#include "gmacdef.h"
#include "gmacsig.h"
#include "gmacnull.h"
#include "gmacidle_if.h"
#include "gmactransfer_if.h"
#include "grlcmaccmn.h"
#include "gmacrb.h"
#include "gmactask.h"
#include "gmacutil.h"
#include "gmacl1.h"
#include "dog_hb_rex.h"
#include "gmutex.h"
#include "gmacds.h"
#include "stringl/stringl.h"
#include "gmac_static_task.h"
#include "geran_dmm.h"
#include "gtmrs.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* Critical section to access control GMAC messages */
rex_crit_sect_type gmac_msg_crit_sec;
gmac_timers_t gmac_timers;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
typedef struct
{
  boolean t3172_expired;
} gmac_static_data_t;

static gmac_static_data_t gmac_static_data;
#endif /*FEATURE_EXTERNAL_PAGING_GERAN*/

void gmac_static_timer_callback(const uint32 timer_id)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN  
  if (is_geran_loaded())
  {
    mac_timer_callback(timer_id);
  }
  else if ((timer_id & 0x000000ffUL) == MAC_TIMER_T3172)
  {
    gmac_static_data.t3172_expired = TRUE;
  }
  else
  {
    MSG_GERAN_ERROR_1("GMAC unexpected timer=%d expiry", timer_id);
  }
#else /* FEATURE_EXTERNAL_PAGING_GERAN */
  mac_timer_callback(timer_id);
#endif /* !FEATURE_EXTERNAL_PAGING_GERAN */
}

static void gprs_mac_signal_handler(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
    DM_VOIDFUNC_CALL(GERAN_DMM_GMAC_INTF_ID, geran_static_gmac_ext_api_s, gprs_mac_signal_handler) 
#else
    gprs_mac_signal_handler_dyn();
#endif
}

void gmac_store_trace_send_msg_buf(uint8 message_set, void* message_ptr)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
    DM_VOIDFUNC_CALL(GERAN_DMM_GMAC_INTF_ID, geran_static_gmac_ext_api_s, gmac_store_trace_send_msg_buf, message_set, message_ptr) 
#else
    gmac_store_trace_send_msg_buf_dyn(message_set, message_ptr);
#endif
}

void gprs_l1_mac_acc_func(mac_l1_sig_t *common_msg_ptr)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
    DM_VOIDFUNC_CALL(GERAN_DMM_GMAC_INTF_ID, geran_static_gmac_ext_api_s, gprs_l1_mac_acc_fnc, common_msg_ptr) 
#else
    gprs_l1_mac_acc_func_dyn(common_msg_ptr);
#endif
}

void gmac_start_t3166(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
    DM_VOIDFUNC_CALL(GERAN_DMM_GMAC_INTF_ID, geran_static_gmac_ext_api_s, start_t3166) 
#else
    gmac_start_t3166_dyn();
#endif
}

void new_gmac_trans_calc_e_pdan_size
(
egprs_chn_qual_rpt_t *egprs_qual_rpt,  /* */
uint16 *size_with_qaul,                /* */
uint16 *size_without_qual,             /* */
boolean  *chan_req_present             /* */
)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
    DM_VOIDFUNC_CALL(GERAN_DMM_GMAC_INTF_ID, geran_static_gmac_ext_api_s, gmac_trans_calc_e_pdan_size, egprs_qual_rpt, size_with_qaul, size_without_qual, chan_req_present)
#else
    new_gmac_trans_calc_e_pdan_size_dyn(egprs_qual_rpt, size_with_qaul, size_without_qual, chan_req_present);
#endif
}

#ifndef FEATURE_GERAN_REDUCED_DEBUG
log_on_demand_status_enum_type mac_log_on_demand(log_code_type log_code)
{
  log_on_demand_status_enum_type result = LOG_ON_DEMAND_FAILED_ATTEMPT_S;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GMAC_INTF_ID, geran_static_gmac_ext_api_s, result, mac_log_on_demand, log_code) 
#else
  mac_log_on_demand_dyn(log_code);
#endif
  return result;
}
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

boolean gprs_odm_info_is_assigned_tbf_egprs(void)
{
  boolean result = FALSE;

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  DM_FUNC_CALL(GERAN_DMM_GMAC_INTF_ID, geran_static_gmac_ext_api_s, result, gprs_odm_info_is_assigned_tbf_egprs);
#else
  result = gprs_odm_info_is_assigned_tbf_egprs_dyn();
#endif

  return result;
}
/*===========================================================================
===
===  FUNCTION      gmac_process_task_sigs()
===
===  DESCRIPTION
===    Process the task sigs.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_process_task_sigs(rex_sigs_type gmac_task_sigs, dog_report_type dogRpt)
{

  /* Process any active signals with the following priority. */
  if ( gmac_task_sigs & TASK_START_SIG )
  {
    (void)rex_clr_sigs( rex_self(), TASK_START_SIG );
  }

  if ( gmac_task_sigs & TASK_OFFLINE_SIG )
  {
    (void)rex_clr_sigs( rex_self(), TASK_OFFLINE_SIG );
  }

  if ( gmac_task_sigs & GMAC_DOG_RPT_TMR_SIG )
  {
    (void)rex_clr_sigs( rex_self(), GMAC_DOG_RPT_TMR_SIG );

    dog_hb_report(dogRpt);
  }

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  if ( gmac_task_sigs & GMAC_DYN_RESUME_SIG )
  {
    if (gmac_static_data.t3172_expired)
    {
      mac_timer_callback(MAC_TIMER_T3172);
      gmac_static_data.t3172_expired = FALSE;
    }
    (void)rex_clr_sigs( rex_self(), GMAC_DYN_RESUME_SIG );
  }
#endif /*FEATURE_EXTERNAL_PAGING_GERAN*/

  if ( gmac_task_sigs & GPRS_MAC_MASTER_SIG )
  {
    /* Launch MAC task signal handler */
    gprs_mac_signal_handler();
  }
}

static void gmac_static_init()
{
  rex_def_timer_ex(&gmac_timers.timer_t3172, gmac_static_timer_callback, SET_REX_TIMER(MAC_TIMER_T3172));
}

/*===========================================================================
===
===  FUNCTION      GPRS_MAC_TASK()
===
===  DESCRIPTION
===    Launches the MAC task and MAC signal handler
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gprs_mac_task
(
  /* Required for REX, ignore. */

  dword unused
)
{
  rex_sigs_type  gmac_task_sigs = 0;
  dog_report_type dogRpt;

  /* Put checking here for consistency between Task mapping to gas_id and
     gas_id passed within a signal*/

  NOTUSED(unused); /* silence compiler warnings of unused parameter */

  geran_tcb_set(GERAN_TASK_ID_GMAC, rex_self());

  /* Perform startup init here. Satisfy dependency of other tasks
     provided by this task. Do not linger here, startup latency is
     impacted. Higher priority tasks in the same group will preempt
     this task at this point. Do not rely upon priority alone as the
     sole mechanism to arbitrate resource contention with other tasks. */

  rcinit_handshake_startup(); /* required rcinit handshake */
  /* Optional dog registration here */
#ifndef FEATURE_EXTERNAL_PAGING_GERAN
   /* SOmgr will call this function for Dynamic loading from SO_CREATE */
   gsm_mac_fn_init();
#endif /* FEATURE_EXTERNAL_PAGING_GERAN */   

  gmac_static_init();

  /* Perform remaining init here. This is sequenced by a start
     signal delivered at the end of the sequence group processing. */
  dogRpt = dog_hb_register_rex((rex_sigs_type)GMAC_DOG_RPT_TMR_SIG);

  /* Begin event based processing via REX scheduling. */
  for (;;)
  {
    /* Recind processor context until a relevent signal(s) becomes active. */
    gmac_task_sigs =  rex_wait
                      (
                        (
                          TASK_START_SIG       |
                          TASK_STOP_SIG        |
                          TASK_OFFLINE_SIG     |
                          GMAC_DOG_RPT_TMR_SIG |
                          GPRS_MAC_MASTER_SIG
                        )
                      );

    if ( gmac_task_sigs & TASK_STOP_SIG )
    {
      (void)rex_clr_sigs( rex_self(), TASK_STOP_SIG );
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
    }    
    gmac_process_task_sigs(gmac_task_sigs,dogRpt);
  }

  /* Deregister if leaving the task so that we don't ERR_FATAL */
  dog_hb_deregister(dogRpt);

  /*UNREACHED*/
}

/* Normalise naming */
void gsm_mac_task (dword dummy)
{
   gprs_mac_task(dummy);
}


/*** EOF: don't remove! ***/
