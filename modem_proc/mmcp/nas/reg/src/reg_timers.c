/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_timers.c_v   1.2   22 May 2002 09:31:32   kabraham  $
$Header: //components/rel/mmcp.mpss/6.1.10/nas/reg/src/reg_timers.c#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/13/02   kwa     Initial release.
05/21/02   kwa     Added watchdog timer.
07/26/02   kwa     Changed REG_TIMERS_PLMN_SEARCH_PERIOD_FACTOR from 2 to 1
                   and REG_TIMERS_PLMN_SEARCH_PERIOD_MAX from 720 to 180.
10/18/02   kwa     Added FEATURE_REG_DEEP_SLEEP.  Moved definition of constants
                   to customer.h.
11/02/02   kwa     Added entered_deep_sleep parameter to
                   reg_timers_start_plmn_search_period_timer.
05/22/03   kwa     Moved REG_TIMERS_PLMN_SEARCH_PERIOD_UNIT inside
                   FEATURE_REG_DEEP_SLEEP to eliminate a lint warning.
09/07/03   kwa     Modified behaviour of the HPLMN Search Period Timer to allow
                   for a faster initial search for a higher priority PLMN after
                   initially camping on a VPLMN.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
02/02/04   kwa     Removed REG_TIMERS_MINUTES_PER_DECIHOUR.
01/24/05   up      Modified function reg_timers_stop_hplmn_search_period_timer
                   to log message when HPLMN timer is stopped.
01/27/05   up      Modified function reg_timers_start_hplmn_search_period_timer
                   and reg_timers_hplmn_search_timer_expiration_callback to
                   set the flag reg_timers_hplmn_search_initiated when timer
                   is expired instead of seting it when it started.
12/21/04   up      Modified function reg_timers_start_hplmn_search_period_timer
                   and reg_timers_hplmn_search_timer_expiration_callback to
                   create a LOG PACKET for start and expiry of HPLMN timer.
02/22/05   up      Updated for Cingular ENS feature.
                   Added Function reg_timers_restart_hplmn_search_period_timer.
03/01/05   up      Updated reg_timers_stop_hplmn_search_period_timer to log
                   F3 msg "Stopping Timer" only if timer is actually running.
03/15/05   up      Updated reg_timers_initialize to register for 
                   reg watchdog_timer with gsdi task.
06/28/05   up      Function reg_timers_start_hplmn_search_timer_10sec is added
                   to start HPLMN timer of 10 sec in connected mode.
07/27/05   up      Currection of the log message in function 
                   reg_timers_start_hplmn_search_timer_10sec.
01/05/06   sn      Updated reg_timers_start_hplmn_search_period_timer to start
                   timer only if it is not running. Also start 2 min timer if
                   previous RPLMN is different than the current RPLMN.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "dog_hb_rex.h"
#include "ms.h"
#include "msg.h"
#include "reg_sim_v.h"
#include "reg_task.h"
#include "reg_task_int.h"
#include "reg_timers.h"
#include "rex.h"
#include "timers_v.h"
#include "reg_log.h"
#include "err.h"
#if !defined(FEATURE_UIM_DISABLE_GSDI_INTERFACE)
#include "gsdi_exp.h"
#include "gsdi_exp_v.h"
#endif
 
#include "reg_nv.h"
#include "timer.h"
#include "mm_v.h"
#include "time_svc.h"
#include "reg_mode.h"
/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/
extern timer_group_type nas_timer_group_non_deferrable;

#define REG_TIMERS_HPLMN_SEARCH_PERIOD_UNIT      DECIHOUR /* 6 seconds */
#define REG_TIMERS_HPLMN_SEARCH_PERIOD_UNIT_A_CIOT      2 /* 2hours step expressed in hours */
#define REG_TIMERS_HPLMN_SEARCH_PERIOD_UNIT_B_CIOT      4 /* 4hours step expressed in hours */

/*
** 3GPP TS 23.122 Section 4.4.3.3.a defines the minimum value for initiating
** the search for a higher priority PLMN to be no less than 2 minutes.
*/
#define REG_TIMERS_INITIAL_HPLMN_SEARCH_PERIOD  (120*SECOND) /* 2 minutes */

#if defined(FEATURE_TRIPLE_SIM)  || defined(FEATURE_SGLTE_DUAL_SIM)
static boolean reg_timers_hplmn_search_initiated_sim[MAX_NAS_STACKS] = { FALSE, FALSE, FALSE};
#define reg_timers_hplmn_search_initiated   reg_timers_hplmn_search_initiated_sim[reg_as_id]

#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
static boolean reg_timers_hplmn_search_initiated_sim[MAX_NAS_STACKS] = { FALSE, FALSE };
#define reg_timers_hplmn_search_initiated   reg_timers_hplmn_search_initiated_sim[reg_as_id]

#else
static boolean reg_timers_hplmn_search_initiated = FALSE;
#endif



#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

static timer_type reg_timers_hplmn_search_period_timer_sim[MAX_NAS_STACKS];
#define reg_timers_hplmn_search_period_timer  reg_timers_hplmn_search_period_timer_sim[reg_as_id]
#else
static timer_type reg_timers_hplmn_search_period_timer;
#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE
*/


#ifdef FEATURE_SGLTE
static rex_timer_type reg_timers_sglte_ps_timer;
#endif

#if defined(FEATURE_SGLTE) || defined(FEATURE_SVLTE_DUAL_SIM)
extern rex_timer_cnt_type mm_get_ps_timer_value(void);
#endif

#if defined FEATURE_TDSCDMA || defined FEATURE_SGLTE
#if defined FEATURE_DUAL_SIM
#if defined(FEATURE_TRIPLE_SIM)  || defined(FEATURE_SGLTE_DUAL_SIM)

static timer_type reg_timers_hplmn_irat_search_period_timer_sim[MAX_NAS_STACKS];
#define reg_timers_hplmn_irat_search_period_timer  reg_timers_hplmn_irat_search_period_timer_sim[reg_as_id]    //LTE_IRAT

static boolean reg_timers_irat_hplmn_search_initiated_sim[MAX_NAS_STACKS] = {FALSE, FALSE,FALSE};
#define reg_timers_irat_hplmn_search_initiated reg_timers_irat_hplmn_search_initiated_sim[reg_as_id]
static boolean reg_timer_long_timer_started_sim[MAX_NAS_STACKS] = {FALSE, FALSE,FALSE};
#define reg_timer_long_timer_started reg_timer_long_timer_started_sim[reg_as_id]
static boolean reg_timer_short_timer_started_sim[MAX_NAS_STACKS] = {FALSE, FALSE,FALSE};
#define reg_timer_short_timer_started reg_timer_short_timer_started_sim[reg_as_id]
#else
static timer_type reg_timers_hplmn_irat_search_period_timer_sim[MAX_NAS_STACKS];
#define reg_timers_hplmn_irat_search_period_timer  reg_timers_hplmn_irat_search_period_timer_sim[reg_as_id]    //LTE_IRAT

static boolean reg_timers_irat_hplmn_search_initiated_sim[MAX_NAS_STACKS] = {FALSE, FALSE};
#define reg_timers_irat_hplmn_search_initiated reg_timers_irat_hplmn_search_initiated_sim[reg_as_id]
static boolean reg_timer_long_timer_started_sim[MAX_NAS_STACKS] = {FALSE, FALSE};
#define reg_timer_long_timer_started reg_timer_long_timer_started_sim[reg_as_id]
static boolean reg_timer_short_timer_started_sim[MAX_NAS_STACKS] = {FALSE, FALSE};
#define reg_timer_short_timer_started reg_timer_short_timer_started_sim[reg_as_id]

#endif
#else
static timer_type reg_timers_hplmn_irat_search_period_timer;
static boolean reg_timers_irat_hplmn_search_initiated = FALSE;
static boolean        reg_timer_long_timer_started;
static boolean        reg_timer_short_timer_started;
#endif
#endif

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
static rex_timer_type reg_timers_validate_sim_timer_sim[MAX_NAS_STACKS];
#define reg_timers_validate_sim_timer  reg_timers_validate_sim_timer_sim[reg_as_id]
#else
static rex_timer_type reg_timers_validate_sim_timer;
#endif

#if defined FEATURE_TDSCDMA || defined FEATURE_SGLTE
#if defined FEATURE_DUAL_SIM 
static timer_type reg_timers_lte_irat_search_period_timer_sim[MAX_NAS_STACKS];
#define reg_timers_lte_irat_search_period_timer  reg_timers_lte_irat_search_period_timer_sim[reg_as_id]  //LTE_IRAT
#else
static timer_type reg_timers_lte_irat_search_period_timer;
#endif
#endif

#if defined FEATURE_TDSCDMA || defined FEATURE_SGLTE
static rex_timer_type reg_timers_backoff_lte_search_timer;
#endif
/* ----------------------------
** Define watchdog report handle
** ---------------------------- */
  #undef  DOG_REG_RPT
  #define DOG_REG_RPT       reg_dog_rpt_var
  static dog_report_type    reg_dog_rpt_var  = 0;      /* Initial Safety */

#ifdef FEATURE_LTE
#if defined FEATURE_DUAL_SIM
static rex_timer_type reg_timers_backoff_fplmn_search_timer_sim[MAX_NAS_STACKS];
#define reg_timers_backoff_fplmn_search_timer reg_timers_backoff_fplmn_search_timer_sim[reg_sub_id]
static rex_timer_type reg_timers_update_lte_cap_timer_sim[MAX_NAS_STACKS];
#define reg_timers_update_lte_cap_timer reg_timers_update_lte_cap_timer_sim[reg_sub_id]
#else
static rex_timer_type reg_timers_backoff_fplmn_search_timer;
static rex_timer_type reg_timers_update_lte_cap_timer;
#endif
#endif

#ifdef FEATURE_DUAL_SIM
static timer_type reg_timers_t3245_timer_sim[MAX_NAS_STACKS];
#define reg_timers_t3245_timer  reg_timers_t3245_timer_sim[reg_sub_id]
#else
static timer_type reg_timers_t3245_timer;
#endif

#ifdef FEATURE_CIOT
static timer_type reg_timers_ciot_irat_search_period_timer;
static timer_type reg_timers_nb1_acq_tier3;
static timer_type reg_timers_nb1_fullband_tier3;//FRACQ
#endif

extern rex_tcb_type* reg_tcb_ptr;


static dword reg_timer_hplmn_timer = REG_TIMERS_MAX_TIMER_VALUE;

/*==============================================================================

                          STATIC FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/
#ifdef FEATURE_CIOT
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif
/*==============================================================================

FUNCTION NAME

  reg_set_ciot_hplmn_timer
 
  Description: This function uses a 23.122 algorithm to calculate
  the hplmn search timer for a device supporting CIoT 

==============================================================================*/
static void reg_set_ciot_hplmn_timer (dword* timer)
{
    byte T = reg_sim_read_hplmn_search_period(); /* Expressed in hours */
    /*
    23.122 T is either in the range 2 hours to 240 hours, using 2 hour steps from 2 hours to 80 hours 
    and 4 hour steps from 84 hours to 240 hours, or it indicates that no periodic attempts shall be made. 
    If no value for T is stored in the SIM, a default value of 72 hours is used. 
    Formula y = 2h x T or y = 4h x T - 80h (43 -> 88, 44 -> 92, 45 -> 96)
    */

    if (T <= 0x28 )
    {
       *timer = T*REG_TIMERS_HPLMN_SEARCH_PERIOD_UNIT_A_CIOT;
    }
    else if (T <= 0x50)
    {        
       *timer = T*REG_TIMERS_HPLMN_SEARCH_PERIOD_UNIT_B_CIOT - 80;
    }
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif
#endif
/*==============================================================================

FUNCTION NAME

  reg_timers_enqueue_timer_command

==============================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif
static void reg_timers_enqueue_timer_command
(
  byte timer_id
)
{
  reg_cmd_type* reg_cmd_p;

  /*
  ** Get a command buffer for the REG task.
  */
  reg_cmd_p = reg_get_cmd_buf();

  if(reg_cmd_p == NULL)
  {
    MSG_FATAL_DS(REG_SUB, "=REG= Unable to allocate the meomory", 0, 0, 0);
  }

  /*
  ** Construct the timer command.
  */
  reg_cmd_p->cmd.timer_expiry.IMH.message_set = MS_TIMER;
  reg_cmd_p->cmd.timer_expiry.IMH.message_id  = MID_TIMER_EXPIRY;

  reg_cmd_p->cmd.timer_expiry.IMH.message_len_lsb =
    (sizeof(timer_expiry_T) - sizeof(IMH_T)) % 256;
  reg_cmd_p->cmd.timer_expiry.IMH.message_len_msb =
    (sizeof(timer_expiry_T) - sizeof(IMH_T)) / 256;

  reg_cmd_p->cmd.timer_expiry.data     = 0;
  reg_cmd_p->cmd.timer_expiry.timer_id = timer_id;

  /*
  ** Put on REG command queue
  */
  reg_put_cmd(reg_cmd_p);
}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif

/*==============================================================================

FUNCTION NAME

  reg_timers_hplmn_search_timer_expiration_callback

==============================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif
static void reg_timers_hplmn_search_timer_expiration_callback
(
  unsigned long dummy
)
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  sys_modem_as_id_e_type as_id;

  if(dummy == REG_HPLMN_TIMER)
  {
    reg_timers_hplmn_search_initiated_sim[SYS_MODEM_AS_ID_1] = TRUE;
    as_id = SYS_MODEM_AS_ID_1;
  }
  else if(dummy == REG_HPLMN_TIMER_SIM_ID_2)
  {
    reg_timers_hplmn_search_initiated_sim[SYS_MODEM_AS_ID_2] = TRUE;
    as_id = SYS_MODEM_AS_ID_2;
  }
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
  else if(dummy == REG_HPLMN_TIMER_SIM_ID_3)
  {
    reg_timers_hplmn_search_initiated_sim[SYS_MODEM_AS_ID_3] = TRUE;
    as_id = SYS_MODEM_AS_ID_3;
  }
#endif
  else
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= Timer expiry: Wrong HPLMN timer value");
    return;
  }
  reg_timers_enqueue_timer_command
  (
    (byte) dummy
  );
#else
  (void) dummy;

  reg_timers_hplmn_search_initiated = TRUE;

  reg_timers_enqueue_timer_command
  (
    REG_HPLMN_TIMER
  );
#endif
  /*
  ** Send a LOG PACKET for HPLMN TIMER EXPIRY Notification.
  */
  reg_log_umts_nas_hplmn_search_timer_expire
  (
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
    as_id
#endif
  );
}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif
#ifdef FEATURE_TDSCDMA
/*==============================================================================

FUNCTION NAME

  reg_timers_hplmn_search_timer_expiration_callback

==============================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif
static void reg_timers_hplmn_irat_search_timer_expiration_callback
(
  unsigned long dummy
)
{
   
      if (dummy == REG_HPLMN_IRAT_TIMER)
      {
#ifdef FEATURE_DUAL_SIM
           reg_timers_irat_hplmn_search_initiated_sim[SYS_MODEM_AS_ID_1] = TRUE;
#else
           reg_timers_irat_hplmn_search_initiated = TRUE;
#endif
           reg_timers_enqueue_timer_command
           (
              REG_HPLMN_IRAT_TIMER
           );
      }  
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
      else if(dummy == REG_HPLMN_IRAT_TIMER_SIM_ID_2)
      {
#ifdef FEATURE_DUAL_SIM
           reg_timers_irat_hplmn_search_initiated_sim[SYS_MODEM_AS_ID_2] = TRUE;
#else
           reg_timers_irat_hplmn_search_initiated = TRUE;
#endif
          reg_timers_enqueue_timer_command
           (
              REG_HPLMN_IRAT_TIMER_SIM_ID_2
           );
      }
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
      else if(dummy == REG_HPLMN_IRAT_TIMER_SIM_ID_3)
      {
         reg_timers_irat_hplmn_search_initiated_sim[SYS_MODEM_AS_ID_3] = TRUE;
         reg_timers_enqueue_timer_command
         (
             REG_HPLMN_IRAT_TIMER_SIM_ID_3
         );
      }
      
#endif

#endif
}


#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif

#if defined FEATURE_TDSCDMA || defined FEATURE_SGLTE
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif

static void reg_timers_lte_irat_search_timer_expiration_callback
(
  unsigned long dummy
)
{
   
      if (dummy == REG_LTE_IRAT_TIMER)
      {
           reg_timers_enqueue_timer_command
           (
              REG_LTE_IRAT_TIMER
           );
#ifdef FEATURE_DUAL_SIM
        reg_timer_long_timer_started_sim[SYS_MODEM_AS_ID_1] = FALSE;
        reg_timer_short_timer_started_sim[SYS_MODEM_AS_ID_1] = FALSE;
#else
        reg_timer_long_timer_started = FALSE;
        reg_timer_short_timer_started = FALSE;
#endif
        
      }
#if defined FEATURE_DUAL_SIM && defined FEATURE_SGLTE
      else if(dummy == REG_LTE_IRAT_TIMER_SIM_ID_2)
      {
          reg_timers_enqueue_timer_command
           (
              REG_LTE_IRAT_TIMER_SIM_ID_2
           );
        reg_timer_long_timer_started_sim[SYS_MODEM_AS_ID_2] = FALSE;
        reg_timer_short_timer_started_sim[SYS_MODEM_AS_ID_2] = FALSE;
      }
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
     else if(dummy == REG_LTE_IRAT_TIMER_SIM_ID_3)
     {
       reg_timers_enqueue_timer_command
       (
          REG_LTE_IRAT_TIMER_SIM_ID_3
       );
      reg_timer_long_timer_started_sim[SYS_MODEM_AS_ID_3] = FALSE;
      reg_timer_short_timer_started_sim[SYS_MODEM_AS_ID_3] = FALSE;
     }

#endif

#endif
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif

static void reg_timers_lte_irat_list_duration_timer_expiration_callback
(
  unsigned long dummy
)
{
   
      if (dummy == REG_LTE_IRAT_TIMER)
      {
           reg_timers_enqueue_timer_command
           (
              REG_BACK_OFF_LTE_SEARCH_TIMER
           );
      }
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif
#endif
#endif


#ifdef FEATURE_LTE

/*==============================================================================

FUNCTION NAME

  reg_timers_backoff_fplmn_search_timer_expiration_callback

==============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif
static void reg_timers_backoff_fplmn_search_timer_expiration_callback
(
  unsigned long cb_param
)
{
  reg_timers_enqueue_timer_command
  (
    (byte)cb_param
  );
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif


/*==============================================================================

FUNCTION NAME

  reg_timers_update_lte_cap_timer_expiration_callback

==============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif
static void reg_timers_update_lte_cap_timer_expiration_callback
(
  unsigned long cb_param
)
{
    reg_timers_enqueue_timer_command
  (
    (byte)cb_param
  );
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif

#endif

#ifdef FEATURE_SGLTE
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif
static void reg_timers_ps_timer_expiration_callback
(
  unsigned long dummy
)
{
   (void) dummy;
   reg_timers_enqueue_timer_command
   (
     REG_SGLTE_PS_TIMER
   );
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif

#endif
/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  reg_timers_t3245_timer_expiration_callback

==============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif
static void reg_timers_t3245_timer_expiration_callback
(
  unsigned long timer
)
{
  reg_timers_enqueue_timer_command
  (
    (byte) timer
  );
}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif

/*==============================================================================

FUNCTION NAME

  reg_timers_validate_sim_timer_expiration_callback

==============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif
static void reg_timers_validate_sim_timer_expiration_callback
(
  unsigned long timer
)
{
  reg_timers_enqueue_timer_command
  (
    (byte) timer
  );
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif

#ifdef FEATURE_CIOT
/*==============================================================================

FUNCTION NAME

  reg_timers_ciot_irat_search_timer_expiration_callback

==============================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif
static void reg_timers_ciot_irat_search_timer_expiration_callback
(
  unsigned long timer
)
{
  reg_timers_enqueue_timer_command
  (
    (byte) timer
  );
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif

/*==============================================================================

FUNCTION NAME

  reg_timers_ciot_nb1_acqdb_tier3_expiration_callback

==============================================================================*/


#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif
static void reg_timers_ciot_nb1_acqdb_tier3_expiration_callback
(
  unsigned long timer
)
{
  reg_timers_enqueue_timer_command
  (
    (byte) timer
  );
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif

/*==============================================================================

FUNCTION NAME

  reg_timers_ciot_nb1_fullband_tier3_expiration_callback

==============================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif
static void reg_timers_ciot_nb1_fullband_tier3_expiration_callback  //FRACQ
(
  unsigned long timer
)
{
  reg_timers_enqueue_timer_command
  (
    (byte) timer
  );
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif
#endif
/*==============================================================================

FUNCTION NAME

  reg_timers_initialize

==============================================================================*/

void reg_timers_initialize
(
  void
)
{
  timer_group_set_deferrable(&nas_timer_group_non_deferrable,FALSE);

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  timer_def(&reg_timers_hplmn_search_period_timer_sim[SYS_MODEM_AS_ID_1], 
            &nas_timer_group_non_deferrable,
            rex_self(),
            0,
            reg_timers_hplmn_search_timer_expiration_callback,
            REG_HPLMN_TIMER);

  timer_def(&reg_timers_hplmn_search_period_timer_sim[SYS_MODEM_AS_ID_2], 
            &nas_timer_group_non_deferrable,
            rex_self(),
            0,
            reg_timers_hplmn_search_timer_expiration_callback,
            REG_HPLMN_TIMER_SIM_ID_2);
#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
  timer_def(&reg_timers_hplmn_search_period_timer_sim[SYS_MODEM_AS_ID_3], 
            &nas_timer_group_non_deferrable,
            rex_self(),
            0,
            reg_timers_hplmn_search_timer_expiration_callback,
            REG_HPLMN_TIMER_SIM_ID_3);
#endif

#else
  timer_def(&reg_timers_hplmn_search_period_timer, 
            &nas_timer_group_non_deferrable,
            rex_self(),
            0,
            reg_timers_hplmn_search_timer_expiration_callback,
            REG_HPLMN_TIMER);
#endif

/*
** This timer is used for IRATs to LTE.
*/
#ifdef FEATURE_TDSCDMA
#if defined FEATURE_DUAL_SIM 
  timer_def(&reg_timers_hplmn_irat_search_period_timer_sim[SYS_MODEM_AS_ID_2], 
            &nas_timer_group_non_deferrable,
            rex_self(),
            0,
            reg_timers_hplmn_irat_search_timer_expiration_callback,
            REG_HPLMN_IRAT_TIMER_SIM_ID_2);
   timer_def(&reg_timers_hplmn_irat_search_period_timer_sim[SYS_MODEM_AS_ID_1], 
            &nas_timer_group_non_deferrable,
            rex_self(),
            0,
            reg_timers_hplmn_irat_search_timer_expiration_callback,
            REG_HPLMN_IRAT_TIMER);
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
   timer_def(
           &reg_timers_lte_irat_search_period_timer_sim[SYS_MODEM_AS_ID_3],
           &nas_timer_group_non_deferrable,
           rex_self(),
           0,
           reg_timers_hplmn_irat_search_timer_expiration_callback,
           REG_HPLMN_IRAT_TIMER_SIM_ID_3
         );
#endif

#else
  timer_def(&reg_timers_hplmn_irat_search_period_timer, 
            &nas_timer_group_non_deferrable,
            rex_self(),
            0,
            reg_timers_hplmn_irat_search_timer_expiration_callback,
            REG_HPLMN_IRAT_TIMER);
#endif

#ifdef FEATURE_TDSCDMA
#if defined FEATURE_DUAL_SIM
  timer_def(&reg_timers_lte_irat_search_period_timer_sim[SYS_MODEM_AS_ID_2], 
            &nas_timer_group_non_deferrable,
            rex_self(),
            0,
            reg_timers_lte_irat_search_timer_expiration_callback,
            REG_LTE_IRAT_TIMER_SIM_ID_2);
   timer_def(&reg_timers_lte_irat_search_period_timer_sim[SYS_MODEM_AS_ID_1], 
            &nas_timer_group_non_deferrable,
            rex_self(),
            0,
            reg_timers_lte_irat_search_timer_expiration_callback,
            REG_LTE_IRAT_TIMER);
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM                           
   timer_def(&reg_timers_lte_irat_search_period_timer_sim[SYS_MODEM_AS_ID_3], 
            &nas_timer_group_non_deferrable,
            rex_self(),
            0,
       reg_timers_lte_irat_search_timer_expiration_callback,
       REG_LTE_IRAT_TIMER_SIM_ID_3
     );
#endif

#else
timer_def(&reg_timers_lte_irat_search_period_timer, 
          &nas_timer_group_non_deferrable,
          rex_self(),
          0,
          reg_timers_lte_irat_search_timer_expiration_callback,
          REG_LTE_IRAT_TIMER);
#endif
#endif

#if defined FEATURE_TDSCDMA || defined FEATURE_SGLTE
timer_def((timer_ptr_type)&reg_timers_backoff_lte_search_timer,
          &nas_timer_group_non_deferrable,
          rex_self(),
          0,
          reg_timers_lte_irat_list_duration_timer_expiration_callback,
          REG_BACK_OFF_LTE_SEARCH_TIMER);

#endif
#endif
#ifdef FEATURE_CIOT
     timer_def(&reg_timers_ciot_irat_search_period_timer, 
            &nas_timer_group_non_deferrable,
            rex_self(),
            0,
            reg_timers_ciot_irat_search_timer_expiration_callback,
            REG_CIOT_IRAT_TIMER);
     timer_def(&reg_timers_nb1_acq_tier3, 
            &nas_timer_group_non_deferrable,
            rex_self(),
            0,
            reg_timers_ciot_nb1_acqdb_tier3_expiration_callback,
            REG_CIOT_NB1_ACQDB_TIER3_TIMER);

      timer_def(&reg_timers_nb1_fullband_tier3, 
            &nas_timer_group_non_deferrable,
            rex_self(),
            0,
            reg_timers_ciot_nb1_fullband_tier3_expiration_callback,
            REG_CIOT_NB1_FULLBAND_TIER3_TIMER);
#endif
  /* Register with DOG HB new API */
  reg_dog_rpt_var = dog_hb_register_rex((rex_sigs_type) REG_DOGRPT_TIMER_SIG);

#ifdef FEATURE_LTE
#ifdef FEATURE_DUAL_SIM
  rex_def_timer_ex
  (
    &reg_timers_backoff_fplmn_search_timer_sim[SYS_MODEM_AS_ID_1],
    reg_timers_backoff_fplmn_search_timer_expiration_callback,
    REG_BACKOFF_FPLMN_TIMER
  );
  rex_def_timer_ex
  (
    &reg_timers_backoff_fplmn_search_timer_sim[SYS_MODEM_AS_ID_2],
    reg_timers_backoff_fplmn_search_timer_expiration_callback,
    REG_BACKOFF_FPLMN_TIMER_SIM_ID_2
  );
  rex_def_timer_ex
  (
    &reg_timers_update_lte_cap_timer_sim[SYS_MODEM_AS_ID_1],
    reg_timers_update_lte_cap_timer_expiration_callback,
    REG_UPDATE_LTE_CAP_TIMER
  );
  rex_def_timer_ex
  (
    &reg_timers_update_lte_cap_timer_sim[SYS_MODEM_AS_ID_2],
    reg_timers_update_lte_cap_timer_expiration_callback,
    REG_UPDATE_LTE_CAP_TIMER_SIM_ID_2
  );
#else
  rex_def_timer_ex
  (
    &reg_timers_backoff_fplmn_search_timer,
    reg_timers_backoff_fplmn_search_timer_expiration_callback,
    REG_BACKOFF_FPLMN_TIMER
  );

  rex_def_timer_ex
  (
    &reg_timers_update_lte_cap_timer,
    reg_timers_update_lte_cap_timer_expiration_callback,
    REG_UPDATE_LTE_CAP_TIMER
  );
#endif
#endif


#ifdef FEATURE_DUAL_SIM
  timer_def(&reg_timers_t3245_timer_sim[SYS_MODEM_AS_ID_1], 
            &nas_timer_group_non_deferrable,
            rex_self(),
            0,
            reg_timers_t3245_timer_expiration_callback,
            REG_T3245_TIMER);

  timer_def(&reg_timers_t3245_timer_sim[SYS_MODEM_AS_ID_2], 
            &nas_timer_group_non_deferrable,
            rex_self(),
            0,
            reg_timers_t3245_timer_expiration_callback,
            REG_T3245_TIMER_SIM_ID_2);
#ifdef FEATURE_TRIPLE_SIM
  timer_def(&reg_timers_t3245_timer_sim[SYS_MODEM_AS_ID_3], 
            &nas_timer_group_non_deferrable,
            rex_self(),
            0,
            reg_timers_t3245_timer_expiration_callback,
            REG_T3245_TIMER_SIM_ID_3);
#endif
#else
  timer_def(&reg_timers_t3245_timer, 
            &nas_timer_group_non_deferrable,
            rex_self(),
            0,
            reg_timers_t3245_timer_expiration_callback,
            REG_T3245_TIMER);
#endif
#ifdef FEATURE_SGLTE
rex_def_timer_ex
(
  &reg_timers_sglte_ps_timer,
  reg_timers_ps_timer_expiration_callback,
  REG_SGLTE_PS_TIMER
);
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  rex_def_timer_ex
  (
    &reg_timers_validate_sim_timer_sim[SYS_MODEM_AS_ID_1],
    reg_timers_validate_sim_timer_expiration_callback,
    REG_VALIDATE_SIM_TIMER
  );

  rex_def_timer_ex
  (
    &reg_timers_validate_sim_timer_sim[SYS_MODEM_AS_ID_2],
    reg_timers_validate_sim_timer_expiration_callback,
    REG_VALIDATE_SIM_TIMER_SIM_ID_2
  );

#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
  rex_def_timer_ex
  (
    &reg_timers_validate_sim_timer_sim[SYS_MODEM_AS_ID_3],
    reg_timers_validate_sim_timer_expiration_callback,
    REG_VALIDATE_SIM_TIMER_SIM_ID_3
  );
#endif
#else
  rex_def_timer_ex
  (
    &reg_timers_validate_sim_timer,
    reg_timers_validate_sim_timer_expiration_callback,
    REG_VALIDATE_SIM_TIMER
  );
#endif
}

/*==============================================================================

FUNCTION NAME

  reg_timers_start_hplmn_search_timer_10sec

// 30 sec timer timer is started when higher priority search is rejected from lower layer.
==============================================================================*/

void reg_timers_start_hplmn_search_timer_10sec
(
  void
)
{
  dword timer_count = 10000;

  (void) timer_set
  (
    &reg_timers_hplmn_search_period_timer,
    timer_count,
    0,
    T_MSEC
  );

  MSG_HIGH_DS_1(REG_SUB, "=REG= Started HPLMN Search Timer (%d seconds)", (word)(timer_count/1000));
}

/*==============================================================================

FUNCTION NAME

  reg_timers_start_hplmn_search_timer_value
 
 DESCRIPTION
  Starts the hplmn timer with a timer passed in ms

==============================================================================*/
void reg_timers_start_hplmn_search_timer_value
(
  dword timeout
)
{  
  (void) timer_set
  (
    &reg_timers_hplmn_search_period_timer,
    timeout,
    0,
    T_MSEC
  );

  MSG_HIGH_DS_1(REG_SUB, "=REG= Started HPLMN Search Timer (%d seconds)", (word)(timeout/1000));
}

/*==============================================================================

FUNCTION NAME

  reg_timers_start_hplmn_search_period_timer

==============================================================================*/

void reg_timers_start_hplmn_search_period_timer
(
  void
)
{
    timer_unit_type unit_type = T_MSEC;
    dword hplmn_search_period_timer = 0;
    dword min_periodic_search_timer;
    dword timer_count;
    boolean print_hours = FALSE;

#ifdef FEATURE_CIOT 
if(!(reg_mode_is_only_req_rat_present_in_rat_pri_list_gsm(reg_mode_rat_pri_list_get())))
{
    /* Read the HPLMN timer value in hours */
    reg_set_ciot_hplmn_timer(&hplmn_search_period_timer);
    
    /* Convert it to minutes */
    hplmn_search_period_timer *= 60;        
    unit_type = T_MIN;
    /* Read the min timer value in minutes */
    min_periodic_search_timer = reg_sim_per_subs_get_min_periodic_search_timer_value((sys_modem_as_id_e_type)reg_sub_id);  
}
else 
#endif
{
   hplmn_search_period_timer = reg_sim_read_hplmn_search_period() * REG_TIMERS_HPLMN_SEARCH_PERIOD_UNIT;
   unit_type = T_MSEC;
   min_periodic_search_timer = reg_sim_per_subs_get_min_periodic_search_timer_value((sys_modem_as_id_e_type)reg_sub_id) * 60000;
}
timer_count = 0;


   /*Use the user defined timer is required*/
  if(reg_timer_hplmn_timer != REG_TIMERS_MAX_TIMER_VALUE)
  {
    timer_count = reg_timer_hplmn_timer;
  }
  else if(hplmn_search_period_timer != 0) 
  {
    /* If hplmn_search_period_timer is less than the min_periodic_search_timer,
    then timer_count will be set to the min_periodic_search_timer. FR 29016 */    
    /* Compare the values that are in the same units */    
    if (hplmn_search_period_timer > min_periodic_search_timer)
    {
        timer_count = hplmn_search_period_timer;
        print_hours = TRUE;
    }
    else
    {
        timer_count = min_periodic_search_timer;
        print_hours = FALSE;
    }
  }

  if(!sys_plmn_match(reg_state_prev_plmn_get(), reg_state_service_plmn_get()))
  {
    reg_timers_stop_hplmn_search_period_timer();
/*IF ffhplmn tag is set to FALSE, then dont perform  fast first hplmn search ref:4.4.3.3.1 23.122*/
/* IF EF is false, follow T after PLMN change*/
#if defined (FEATURE_NAS_REL12 ) || defined (FEATURE_LTE_REL12)
    if(reg_sim_get_fast_hplmn_tag() == FALSE
        && (reg_state_get_hplmn_search_timer_at_power_up() == FALSE)
      )
      {
        reg_timers_hplmn_search_initiated = TRUE; 
      }
#else
/*Pre Rel12 CIOT, no EF control, follow T on PLMN change*/
#ifdef FEATURE_CIOT
    if(reg_state_get_hplmn_search_timer_at_power_up() == FALSE)
  {
    reg_timers_hplmn_search_initiated = TRUE;
  }
#endif
#endif
  }
/*start with T if slow + (non-power up + (ciot || not))*/

  if ((timer_count != 0) && 
      (timer_get(&reg_timers_hplmn_search_period_timer, T_MSEC) == 0))
  {
    /*
    ** If the HPLMN search period timer is being set for the first time
    ** then set it to an initial value which can be less than the value
    ** specified by EF HPLMN on the SIM.
    */

    if (!reg_timers_hplmn_search_initiated)
    {
    /*Here in CIOT timer count is in hrs and REG_TIMERS_INITIAL_HPLMN_SEARCH_PERIOD is in msecs, so taking a minimun doesnt make sense*/
    /*Here we know in CIOT timer_count is always 2 hrs at least, so we take safely say that the minimum is 2 mins which will be entered in ms*/
#ifdef FEATURE_CIOT
       timer_count = REG_TIMERS_INITIAL_HPLMN_SEARCH_PERIOD;    
       unit_type = T_MSEC;
#else
       timer_count = MIN(timer_count, REG_TIMERS_INITIAL_HPLMN_SEARCH_PERIOD);
#endif     
    }

    (void) timer_set
    (
      &reg_timers_hplmn_search_period_timer,
      timer_count,
      0,
      unit_type
    );

#ifdef FEATURE_CIOT
   if(T_MSEC == unit_type)
#endif
   {
    MSG_HIGH_DS_1(REG_SUB, "=REG= Started HPLMN Search Timer (%d minutes)", (word)(timer_count/60000));
    reg_log_umts_nas_hplmn_search_timer_start((dword)(timer_count/60000));
   }
   else
   {   /* The HPLMN timer was set to minutes. Need to print properly */
       if (print_hours)
       {    
         MSG_HIGH_DS_1(REG_SUB, "=REG= Started HPLMN Search Timer (%d hours)", (word)(timer_count/60));
         reg_log_umts_nas_hplmn_search_timer_start((dword)(timer_count));
       }
       else
       {
           MSG_HIGH_DS_1(REG_SUB, "=REG= Started HPLMN Search Timer (%d minutes)", (word)(timer_count));
           reg_log_umts_nas_hplmn_search_timer_start((dword)(timer_count ));
       }
   }
  }
}


/*==============================================================================

FUNCTION NAME

  reg_timers_stop_hplmn_search_period_timer

==============================================================================*/

void reg_timers_stop_hplmn_search_period_timer
(
  void
)
{
  if (timer_get(&reg_timers_hplmn_search_period_timer, T_MSEC))
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Stopped HPLMN Search Timer");
  }
#ifdef FEATURE_CIOT  
   reg_state_reset_psm_alignment();
#endif
  
  reg_timers_hplmn_search_initiated = FALSE;

  (void) timer_clr
  (
    &reg_timers_hplmn_search_period_timer,
    T_MSEC
  );
}

/*==============================================================================

FUNCTION NAME

  reg_timers_restart_hplmn_search_period_timer

==============================================================================*/
boolean reg_timers_restart_hplmn_search_period_timer
(
  void
)
{
   if(timer_get(&reg_timers_hplmn_search_period_timer, T_MSEC))
   {
     MSG_HIGH_DS_0(REG_SUB, "=REG= Restart HPLMN Search Timer");
     (void) timer_clr
     (
       &reg_timers_hplmn_search_period_timer,
       T_MSEC
     );
     reg_timers_start_hplmn_search_period_timer();
     return TRUE;
   }
   else
   {
     MSG_HIGH_DS_0(REG_SUB, "=REG= HPLMN Timer is not restarted : was not running");
   }
   return FALSE;
}

#if defined FEATURE_TDSCDMA || defined FEATURE_SGLTE

void reg_timers_adjust_backoff_lte_search_timer_count
(
  lte_cell_list_type* lte_list_ptr
)
{
  lte_cell_list_type* lte_temp_list_ptr = NULL;
  cell_list_type*  cell_list_temp_ptr = NULL;
  rex_timer_cnt_type curr_timer_count = rex_get_timer(&reg_timers_backoff_lte_search_timer);

  lte_temp_list_ptr = lte_list_ptr;
  
  while(lte_temp_list_ptr != NULL)
  {
    cell_list_temp_ptr = lte_temp_list_ptr->cell_ptr;
    while(cell_list_temp_ptr != NULL)
    {
      cell_list_temp_ptr->timer_count = cell_list_temp_ptr->timer_count + curr_timer_count;
      cell_list_temp_ptr = cell_list_temp_ptr->next_ptr;
    }
    lte_temp_list_ptr = lte_temp_list_ptr->next_ptr;
  }
  
}

/*==============================================================================

FUNCTION NAME

  reg_timers_restart_backoff_fplmn_search_period_timer

==============================================================================*/

void reg_timers_restart_backoff_lte_search_period_timer
(
  rex_timer_cnt_type                     timer
)
{
   if (rex_get_timer(&reg_timers_backoff_lte_search_timer))
   {
     MSG_HIGH_DS_1(REG_SUB,"=REG= LTE IRAT BPLMN : Restart BACKOFF PLMN Search Timer TIMER COUNT %lu", timer);
     (void) rex_clr_timer
     (
       &reg_timers_backoff_lte_search_timer
     );
   }
   else
   {
     MSG_HIGH_DS_1(REG_SUB,"=REG= LTE IRAT BPLMN : Start BACKOFF PLMN Search Timer  TIMER COUNT %lu", timer);
   }
   (void) rex_set_timer
   (
     &reg_timers_backoff_lte_search_timer,
     timer
   );
}

rex_timer_cnt_type reg_timers_get_remaining_lte_duration_timer(void)
{
  return rex_get_timer(&reg_timers_backoff_lte_search_timer);
}


/*==============================================================================

FUNCTION NAME

  reg_timers_stop_backoff_fplmn_search_period_timer

==============================================================================*/

void reg_timers_stop_backoff_lte_search_period_timer
(
  void
)
{
  MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : Cleared BACKOFF PLMN Search Timer");                      
  (void) rex_clr_timer
  (
    &reg_timers_backoff_lte_search_timer
  );
}

void reg_timers_stop_lte_irat_search_period_timer
(
  void
)
{

    if(timer_get(&reg_timers_lte_irat_search_period_timer, T_MSEC))
    {        
        (void) timer_clr
        (
          &reg_timers_lte_irat_search_period_timer,
          T_MSEC
        );
     MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : Stopped LTE IRAT Search Timer");                     
    }
    reg_timer_long_timer_started = FALSE;
    reg_timer_short_timer_started = FALSE;
}

void reg_timers_start_lte_irat_search_period_timer
(
  void
)
{

  boolean found_in_avaialble_list = FALSE;
  boolean found_in_non_avaialble_list = FALSE;

  dword timer_count = (dword)reg_nv_hplmn_irat_search_timer_get();

  reg_state_cell_present_in_list(&found_in_avaialble_list, &found_in_non_avaialble_list);
#ifdef FEATURE_SGLTE
  if( REG_SUB_IS_SGLTE && (found_in_non_avaialble_list == FALSE) && (reg_state_get_rlf_started() 
#ifdef FEATURE_SGLTE_DUAL_SIM
    || (reg_state_get_trm_failure_sglte_main())
#endif 
     ) )
  {
    if(timer_get(&reg_timers_lte_irat_search_period_timer, T_MSEC))
    {
      reg_timers_stop_lte_irat_search_period_timer();
    }
    reg_timers_start_lte_irat_search_period_timer_10sec();
  }
  else 
#endif
  if(found_in_avaialble_list || found_in_non_avaialble_list)
  {
    if(found_in_avaialble_list)
    {
      timer_count = reg_nv_lte_search_config.short_lte_scan*60*1000;
      reg_timer_short_timer_started = TRUE;
    }
    else
    {
      reg_timer_long_timer_started = TRUE;
      timer_count = reg_nv_lte_search_config.long_lte_scan*60*1000;
    }
    if(!timer_get(&reg_timers_lte_irat_search_period_timer, T_MSEC))
    {
      (void) timer_set
      (
        &reg_timers_lte_irat_search_period_timer,
        timer_count,
        0,
        T_MSEC
      );
      MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : Started LTE IRAT Search Timer (%d minutes)",
               (word)(timer_count/60000));
    }
  }
  else
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : lte irat timer is not started as cell is not part of any list");
  }
}

void reg_timers_start_lte_irat_search_period_timer_10sec
(
  void
)
{

  dword timer_count = 10000;

  if(!timer_get(&reg_timers_lte_irat_search_period_timer, T_MSEC))
  {

    (void) timer_set
    (
      &reg_timers_lte_irat_search_period_timer,
      timer_count,
      0,
      T_MSEC
    );
  }
  MSG_HIGH_DS_1(REG_SUB, "=REG= Started LTE IRAT Search Timer(%d seconds)", (word)(timer_count/1000));  
}


/*==============================================================================

FUNCTION NAME

  reg_timer_bplmn_irat_running

==============================================================================*/


boolean reg_timer_bplmn_irat_running                           
(
  void
)
{
  boolean status = FALSE;

  if((timer_get(&reg_timers_hplmn_irat_search_period_timer, T_MSEC)>0) || 
     (timer_get(&reg_timers_lte_irat_search_period_timer, T_MSEC)>0))
  {
    status = TRUE;
  }
  MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : Bplmn times running return_value = %d", status );
  return status;
}

/*==============================================================================

FUNCTION NAME

  reg_timers_start_hplmn_irat_search_period_timer

==============================================================================*/

void reg_timers_start_hplmn_irat_search_period_timer
(
  void
)
{  
  dword timer_count = (dword)reg_nv_hplmn_irat_search_timer_get();

#ifdef FEATURE_SGLTE
  if( REG_SUB_IS_SGLTE && (reg_state_get_rlf_started()
#ifdef FEATURE_SGLTE_DUAL_SIM
    || (reg_state_get_trm_failure_sglte_main())
#endif 
    ) )
  {
    if(timer_get(&reg_timers_hplmn_irat_search_period_timer, T_MSEC))
    {
      reg_timers_stop_hplmn_irat_search_period_timer();
    }
    reg_timers_start_hplmn_irat_search_period_timer_10sec();
    return;
  }
  else 
#endif
  if(!reg_timers_irat_hplmn_search_initiated)
  {
    timer_count = 2*60*1000;
  }
  if(timer_count == REG_TIMERS_MAX_TIMER_VALUE)
  {
    return;
  }
  /* Check if the timer is already running */
  if(!timer_get(&reg_timers_hplmn_irat_search_period_timer, T_MSEC))
  {     
      (void) timer_set
      (
        &reg_timers_hplmn_irat_search_period_timer,
        timer_count,
        0,
        T_MSEC
      );
      MSG_HIGH_DS_1(REG_SUB, "=REG= Started HPLMN IRAT Search Timer (%d minutes)", (word)(timer_count/60000));   
  }
}
/*==============================================================================

FUNCTION NAME

  reg_timers_stop_hplmn_irat_search_period_timer

==============================================================================*/
void reg_timers_stop_hplmn_irat_search_period_timer
(
  void
)
{

    if(timer_get(&reg_timers_hplmn_irat_search_period_timer, T_MSEC))
    {        
        (void) timer_clr
        (
          &reg_timers_hplmn_irat_search_period_timer,
          T_MSEC
        );
        MSG_HIGH_DS_0(REG_SUB, "=REG= Stopped HPLMN IRAT Search Timer");
    }
    reg_timers_irat_hplmn_search_initiated = FALSE;
}

/*==============================================================================

FUNCTION NAME

  reg_timers_stop_all_irat_search_period_timer

==============================================================================*/
void reg_timers_stop_all_irat_search_period_timer
(
  void
)
{
  reg_timers_stop_lte_irat_search_period_timer();
  reg_timers_stop_hplmn_irat_search_period_timer();
}


/*==============================================================================

FUNCTION NAME

  reg_timer_state_hplmn_irat_search_period_timer_10sec

==============================================================================*/
void reg_timers_start_hplmn_irat_search_period_timer_10sec
(
  void
)
{

  dword timer_count = 10000;

  if(!timer_get(&reg_timers_hplmn_irat_search_period_timer, T_MSEC))
  {
    (void) timer_set
    (
      &reg_timers_hplmn_irat_search_period_timer,
      timer_count,
      0,
      T_MSEC
    );
  }
  MSG_HIGH_DS_1(REG_SUB, "=REG= Started HPLMN IRAT Search Timer(%d seconds)", (word)(timer_count/1000));  
}
#endif
#ifdef FEATURE_TDSCDMA
/*==============================================================================

FUNCTION NAME

  reg_timer_restart_hplmn_irat_search_period_timer

==============================================================================*/
void reg_timers_restart_hplmn_irat_search_period_timer
(
  void
)
{
  dword timer_count = (dword)reg_nv_hplmn_irat_search_timer_get();

  (void) timer_clr
  (
    &reg_timers_hplmn_irat_search_period_timer,
    T_MSEC
  );

  if(timer_count == REG_TIMERS_MAX_TIMER_VALUE)
  {
    return;
  }
  
  (void) timer_set
  (
    &reg_timers_hplmn_irat_search_period_timer,
    timer_count,
    0,
    T_MSEC
  );

  MSG_HIGH_DS_1(REG_SUB, "=REG= Started HPLMN IRAT Search Timer (%d minutes)", (word)(timer_count/60000));   
}
#endif
/*==============================================================================

FUNCTION NAME

  reg_timers_pet_watchdog

==============================================================================*/

void reg_timers_pet_watchdog
(
  void
)
{
  /* Report to the Dog HB */
  dog_hb_report(DOG_REG_RPT);
}

#ifdef FEATURE_LTE

/*==============================================================================

FUNCTION NAME

  reg_timers_adjust_backoff_fplmn_list_timer_count

==============================================================================*/

void reg_timers_adjust_backoff_fplmn_list_timer_count
(
  reg_sim_backoff_fplmn_list_info_s_type *backoff_fplmn_list_info_p,
  rex_timer_cnt_type                     diff_timer_count
)
{
  int32 i;
  /*
  ** If timer count is REG_TIMERS_MAX_TIMER_VALUE then the
  ** corresponding plmn is backoff forbidden permanently, So not reducing 
  ** timer count for it.
  */
  for(i = 0; (i < (int32)(backoff_fplmn_list_info_p->length-1)) && 
                 (backoff_fplmn_list_info_p->length <= REG_SIM_BACKOFF_FPLMN_LIST_LENGTH) &&
                 (backoff_fplmn_list_info_p->plmn_timer_list[i].timer_count !=
                 REG_TIMERS_MAX_TIMER_VALUE); i++)
  {
    backoff_fplmn_list_info_p->plmn_timer_list[i].timer_count += diff_timer_count;
  }
}

/*==============================================================================

FUNCTION NAME

  reg_timers_restart_backoff_fplmn_search_period_timer

==============================================================================*/

void reg_timers_restart_backoff_fplmn_search_period_timer
(
  rex_timer_cnt_type                     timer,
  reg_as_id_e_type                       as_id
)
{
#ifdef FEATURE_DUAL_SIM
  rex_timer_type *backoff_timer_ptr = &reg_timers_backoff_fplmn_search_timer_sim[as_id];
#else
   rex_timer_type *backoff_timer_ptr = &reg_timers_backoff_fplmn_search_timer;
   (void)as_id;
#endif
   if (rex_get_timer(backoff_timer_ptr))
   {
     MSG_HIGH_DS_1(as_id, "=REG= Restart BACKOFF PLMN Search Timer TIMER COUNT %lu", timer);
     (void) rex_clr_timer
     (
       backoff_timer_ptr
     );
   }
   else
   {
     MSG_HIGH_DS_1(as_id, "=REG= Start BACKOFF PLMN Search Timer  TIMER COUNT %lu", timer);
   }
   (void) rex_set_timer
   (
     backoff_timer_ptr,
     timer
   );
}

/*==============================================================================

FUNCTION NAME

  reg_timers_start_backoff_fplmn_search_period_timer

==============================================================================*/

void reg_timers_start_backoff_fplmn_search_period_timer
(
  reg_sim_backoff_fplmn_list_info_s_type *backoff_fplmn_list_info_p,
  rex_timer_cnt_type                     timer,
  reg_as_id_e_type                       as_id
)
{
#ifdef FEATURE_DUAL_SIM
    rex_timer_cnt_type curr_timer_count = rex_get_timer(&reg_timers_backoff_fplmn_search_timer_sim[as_id]);
#else
    rex_timer_cnt_type curr_timer_count = rex_get_timer(&reg_timers_backoff_fplmn_search_timer);
#endif
    if(curr_timer_count == 0 && backoff_fplmn_list_info_p->length > 0)
    {
      reg_timers_restart_backoff_fplmn_search_period_timer(timer,as_id);
      backoff_fplmn_list_info_p->plmn_timer_list[backoff_fplmn_list_info_p->length - 1].timer_count= 0;
    }
    else
    {
      if (timer < curr_timer_count)
      {
        reg_timers_adjust_backoff_fplmn_list_timer_count(backoff_fplmn_list_info_p,
                                                                                            (curr_timer_count - timer));
        reg_timers_restart_backoff_fplmn_search_period_timer(timer,as_id);
        backoff_fplmn_list_info_p->plmn_timer_list[backoff_fplmn_list_info_p->length - 1].timer_count = 0;
      }
      else
      {
        backoff_fplmn_list_info_p->plmn_timer_list[backoff_fplmn_list_info_p->length - 1].timer_count = 
                        (timer - curr_timer_count);
      }
    }
}


/*==============================================================================

FUNCTION NAME

  reg_timers_stop_backoff_fplmn_search_period_timer

==============================================================================*/

void reg_timers_stop_backoff_fplmn_search_period_timer
(
  reg_as_id_e_type as_id
)
{
  MSG_HIGH_DS_0(REG_SUB, "=REG= Cleared BACKOFF PLMN Search Timer");
#ifdef FEATURE_DUAL_SIM
  (void) rex_clr_timer
  (
    &reg_timers_backoff_fplmn_search_timer_sim[as_id]
  );
#else
  (void)as_id;
  (void) rex_clr_timer
  (
    &reg_timers_backoff_fplmn_search_timer
  );
#endif
}


/*==============================================================================

FUNCTION NAME

  reg_timers_get_rem_backoff_fplmn_search_timer_value

==============================================================================*/

rex_timer_cnt_type reg_timers_get_rem_backoff_fplmn_search_timer_value
(
  reg_as_id_e_type as_id
)
{
#ifdef FEATURE_DUAL_SIM
   return (rex_get_timer(&reg_timers_backoff_fplmn_search_timer_sim[as_id]));
#else
   (void)as_id;
   return (rex_get_timer(&reg_timers_backoff_fplmn_search_timer));
#endif
}
/*==============================================================================

FUNCTION NAME

  reg_timers_start_update_lte_cap_timer

==============================================================================*/

void reg_timers_start_update_lte_cap_timer
(
  void
)
{
  MSG_HIGH_DS_1(REG_SUB, "=REG= Start UPDATE LTE CAP Timer %d(sec)", (REG_TIMERS_UPDATE_LTE_CAP_VALUE/1000));
  (void) rex_set_timer
   (
     &reg_timers_update_lte_cap_timer,
     REG_TIMERS_UPDATE_LTE_CAP_VALUE
   );
}


/*==============================================================================

FUNCTION NAME

  reg_timers_stop_update_lte_cap_timer

==============================================================================*/

void reg_timers_stop_update_lte_cap_timer
(
  void
)
{
  MSG_HIGH_DS_0(REG_SUB, "=REG= Cleared UPDATE LTE CAP Timer");
  (void) rex_clr_timer
  (
    &reg_timers_update_lte_cap_timer
  );
}


#endif 
#ifdef FEATURE_SGLTE
void reg_timers_start_sglte_ps_timer(void)
{
  rex_timer_cnt_type timer_count = 0;

  timer_count = mm_get_ps_timer_value();

#ifdef FEATURE_SGLTE_DUAL_SIM
  if(reg_state_get_trm_failure_sglte_main())
  {
    timer_count = 15000;
    MSG_HIGH_DS_0(REG_SUB, "=REG= Starting SGLTE PS timer due to RLF on main");
  }
#endif 

  if ((timer_count != 0) && 
      (rex_get_timer(&reg_timers_sglte_ps_timer) == 0))
  {
    /*
    ** If the HPLMN search period timer is being set for the first time
    ** then set it to an initial value which can be less than the value
    ** specified by EF HPLMN on the SIM.
    */
   (void) rex_set_timer
   (
     &reg_timers_sglte_ps_timer,
     timer_count
   );
   MSG_HIGH_DS_1(REG_SUB, "=REG= Started SGLTE PS timer (%d seconds)", (word)(timer_count/1000));
  }

}

/*==============================================================================

FUNCTION NAME

  reg_timers_start_sglte_ps_timer_val

==============================================================================*/

void reg_timers_start_sglte_ps_timer_val(rex_timer_cnt_type timer_count)
{
  if ((timer_count != 0) && 
      (rex_get_timer(&reg_timers_sglte_ps_timer) == 0))
  {
     (void) rex_set_timer
     (
       &reg_timers_sglte_ps_timer,
       timer_count
     );
     MSG_HIGH_DS_1(REG_SUB, "=REG= Started SGLTE PS timer (%d seconds)", (word)(timer_count/1000));            
  }
}

void reg_timers_stop_sglte_ps_timer
(
  void
)
{
  MSG_HIGH_DS_0(REG_SUB, "=REG= Cleared SGLTE PS Search Timer");
  (void) rex_clr_timer
  (
    &reg_timers_sglte_ps_timer
  );
}

#endif 

/*==============================================================================

FUNCTION NAME

  reg_timers_start_t3245_timer

==============================================================================*/

void reg_timers_start_t3245_timer
(
  dword t3245_timer
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
)
{
  if(t3245_timer == REG_T3245_DEFUALT_VALUE)
  {
    t3245_timer = reg_nv_t3245_test_timer_value_get();
    if(t3245_timer == REG_TIMERS_MAX_TIMER_VALUE)
    {
      dword ran_temp;
      mm_secapi_get_random( (uint8 *)&ran_temp, sizeof(dword) );
      
      /*From REl 13 onwards, timer value set to 12 to 24hrs*/
      /*Else follow legacy 24h to 48h*/
#if defined (FEATURE_LTE_REL13) || defined (FEATURE_NAS_REL13)    	
	      t3245_timer = (13 + ((int32)(ran_temp % 11)))* 60 * 60 * 1000; 
#else
	      t3245_timer = (25 + ((int32)(ran_temp % 23)))* 60 * 60 * 1000;
#endif
    }
  }
  (void) timer_set
  (
    &reg_timers_t3245_timer,
    t3245_timer,
    0,
    T_MSEC
  );

  MSG_HIGH_DS_1(REG_SUB, "=REG= Started T3245 Timer (%d seconds)", (dword)(t3245_timer/1000));
}


/*==============================================================================

FUNCTION NAME

  reg_timers_get_rem_t3245_value

==============================================================================*/

rex_timer_cnt_type reg_timers_get_rem_t3245_value
(
  void
)
{
  return (timer_get(&reg_timers_t3245_timer, T_MSEC));
}


/*==============================================================================

FUNCTION NAME

  reg_is_t3245_timer_active

==============================================================================*/

boolean reg_is_t3245_timer_active
(
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type as_id
#endif
)
{
#ifdef FEATURE_DUAL_SIM
  if(timer_get(&reg_timers_t3245_timer_sim[as_id], T_MSEC) > 0)
#else
  if(timer_get(&reg_timers_t3245_timer, T_MSEC) > 0)
#endif
  {
    return TRUE;
  }
  return FALSE;
}


/*==============================================================================

FUNCTION NAME

  reg_timers_stop_t3245

==============================================================================*/

void reg_timers_stop_t3245()
{
#ifdef FEATURE_DUAL_SIM
  if(reg_is_t3245_timer_active((sys_modem_as_id_e_type)reg_sub_id) == TRUE)
#else
  if(reg_is_t3245_timer_active() == TRUE)
#endif
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Stopped T3245 Timer");
    (void) timer_clr
    (
      &reg_timers_t3245_timer,
      T_MSEC
    );
  }
}

/*==============================================================================

FUNCTION NAME

  reg_timers_start_validate_sim_timer

==============================================================================*/

void reg_timers_start_validate_sim_timer
(
  void
)
{
  rex_timer_cnt_type validate_sim_timer = reg_nv_validate_sim_timer_value_get();
  (void) rex_set_timer
  (
    &reg_timers_validate_sim_timer,
    validate_sim_timer
  );

  MSG_HIGH_DS_1(REG_SUB, "=REG= Started validate sim Timer (%d seconds)", 
                         (rex_timer_cnt_type)(validate_sim_timer/1000));

}
#ifdef FEATURE_SGLTE

/*==============================================================================

FUNCTION NAME

  reg_timers_start_validate_sim_timer_per_stack

==============================================================================*/

void reg_timers_start_validate_sim_timer_per_stack
(
  rex_timer_cnt_type validate_sim_timer
)
{
  (void) rex_set_timer
  (
    &reg_timers_validate_sim_timer,
    validate_sim_timer
  );

  MSG_HIGH_DS_1(reg_as_id, "=REG= Started validate sim Timer (%d seconds)",
             (rex_timer_cnt_type)(validate_sim_timer/1000));

}

/*==============================================================================

FUNCTION NAME

  reg_timer_get_validate_sim_timer

==============================================================================*/
rex_timer_cnt_type reg_timer_get_validate_sim_timer(void)
{
  return rex_get_timer(&reg_timers_validate_sim_timer);
}
#endif



/*==============================================================================

FUNCTION NAME

  reg_timers_stop_validate_sim_timer

==============================================================================*/

void reg_timers_stop_validate_sim_timer(void)
{
  if(rex_get_timer(&reg_timers_validate_sim_timer) > 0)
  {
    MSG_HIGH_DS_0(reg_as_id, "=REG= Stopped Validate Sim Timer");
    (void) rex_clr_timer
           (
             &reg_timers_validate_sim_timer
           );
  }
}



/*==============================================================================

FUNCTION NAME

  reg_is_timer_active

==============================================================================*/

boolean reg_is_timer_active
(
  byte timer_id
)
{
  boolean timer_active = FALSE;
  switch(timer_id)
  {
    case REG_HPLMN_TIMER:
      if(timer_get(&reg_timers_hplmn_search_period_timer, T_MSEC) > 0)
      {
        timer_active = TRUE;
      }
      break;
#ifdef FEATURE_TDSCDMA
   case REG_HPLMN_IRAT_TIMER:
      if(timer_get(&reg_timers_hplmn_irat_search_period_timer, T_MSEC) > 0)
      {
        timer_active = TRUE;
      }
      break;
#if defined(FEATURE_DUAL_SIM) 
   case REG_HPLMN_IRAT_TIMER_SIM_ID_2:
      if(timer_get(&reg_timers_hplmn_irat_search_period_timer_sim[SYS_MODEM_AS_ID_2], T_MSEC) > 0)
      {
        timer_active = TRUE;
      }
      break;
#endif

#if defined FEATURE_TDSCDMA || defined FEATURE_SGLTE 
   case REG_LTE_IRAT_TIMER:
      if(timer_get(&reg_timers_lte_irat_search_period_timer, T_MSEC) > 0)
      {
        timer_active = TRUE;
      }
      break;
#if defined(FEATURE_DUAL_SIM) 
   case REG_LTE_IRAT_TIMER_SIM_ID_2:
      if(timer_get(&reg_timers_lte_irat_search_period_timer_sim[SYS_MODEM_AS_ID_2], T_MSEC) > 0)
      {
        timer_active = TRUE;
      }
      break;
#endif
#endif

#if defined FEATURE_TDSCDMA || defined FEATURE_SGLTE    
    case REG_BACK_OFF_LTE_SEARCH_TIMER:
      if(timer_get((timer_ptr_type)&reg_timers_backoff_lte_search_timer, T_MSEC) > 0)
      {
        timer_active = TRUE;
      }
      break;

#endif
#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
    case REG_HPLMN_TIMER_SIM_ID_2:
      if(timer_get(&reg_timers_hplmn_search_period_timer_sim[SYS_MODEM_AS_ID_2], T_MSEC) > 0)
      {
        timer_active = TRUE;
      }
      break;
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
    case REG_HPLMN_TIMER_SIM_ID_3:
      if(timer_get(&reg_timers_hplmn_search_period_timer_sim[SYS_MODEM_AS_ID_3], T_MSEC) > 0)
      {
        timer_active = TRUE;
      }
      break;
#endif
#endif

#ifdef FEATURE_LTE
    case REG_BACKOFF_FPLMN_TIMER:
#ifdef FEATURE_DUAL_SIM
    case REG_BACKOFF_FPLMN_TIMER_SIM_ID_2:
#endif
      if (rex_get_timer(&reg_timers_backoff_fplmn_search_timer) > 0)
      {
        timer_active = TRUE;
      }
      break;

    case REG_UPDATE_LTE_CAP_TIMER:
#ifdef FEATURE_DUAL_SIM
    case REG_UPDATE_LTE_CAP_TIMER_SIM_ID_2:
#endif
      if (rex_get_timer(&reg_timers_update_lte_cap_timer) > 0)
      {
        timer_active = TRUE;
      }
      break;
#endif
#ifdef FEATURE_SGLTE
    case REG_SGLTE_PS_TIMER:
      if(rex_get_timer(&reg_timers_sglte_ps_timer)> 0)
      {
        timer_active = TRUE;
      }
      break;
#endif

    case REG_VALIDATE_SIM_TIMER:
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
    case REG_VALIDATE_SIM_TIMER_SIM_ID_2:
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
    case REG_VALIDATE_SIM_TIMER_SIM_ID_3:
#endif
#endif
      if(rex_get_timer(&reg_timers_validate_sim_timer) > 0)
      {
        timer_active = TRUE;
      }
      break;

    case REG_T3245_TIMER:
#ifdef FEATURE_DUAL_SIM
    case REG_T3245_TIMER_SIM_ID_2:
#ifdef FEATURE_TRIPLE_SIM
    case REG_T3245_TIMER_SIM_ID_3:
#endif
#endif
      if(timer_get(&reg_timers_t3245_timer, T_MSEC) > 0)
      {
        timer_active = TRUE;
      }
      break;
#ifdef FEATURE_CIOT
    case REG_CIOT_IRAT_TIMER:
      if(timer_get(&reg_timers_ciot_irat_search_period_timer,T_MSEC)> 0)
      {
        timer_active = TRUE;
      }
      break;

    case REG_CIOT_NB1_ACQDB_TIER3_TIMER:
      if(timer_get(&reg_timers_nb1_acq_tier3,T_MSEC)> 0)
      {
         MSG_ERROR_DS_1(REG_SUB, "=REG= reg_timers_nb1_acq_tier3 = %d", timer_active);
        timer_active = TRUE;
      }
      break;
    case REG_CIOT_NB1_FULLBAND_TIER3_TIMER:
      if(timer_get(&reg_timers_nb1_fullband_tier3,T_MSEC)> 0)
      {
      MSG_ERROR_DS_1(REG_SUB, "=REG= reg_timers_nb1_fullband_tier3 = %d", timer_active);
        timer_active = TRUE;
      }
      break;
#endif

    default:
      MSG_ERROR_DS_1(REG_SUB, "=REG= Unknown timer expired id = %d", timer_id);
      break;
  }

  return timer_active;

}


/*===========================================================================
FUNCTION NAME 
         reg_timers_set_hplmn_search_timer

DESCRIPTION
  Function to update the HPLMN timer value
  
RETURN VALUE
  None
===========================================================================*/

void reg_timers_set_hplmn_search_timer(uint32 timer_value)
{
  dword  curr_timer;
  byte hplmn_search_period = reg_sim_read_hplmn_search_period();

  dword timer_count = hplmn_search_period * REG_TIMERS_HPLMN_SEARCH_PERIOD_UNIT;

  /* If timer_count is less than the min_periodic_search_timer,
     then timer_count will be set to the min_periodic_search_timer. FR 29016 */
  dword min_periodic_search_timer = reg_sim_per_subs_get_min_periodic_search_timer_value((sys_modem_as_id_e_type)reg_sub_id) * 60000;
  timer_count = MAX(timer_count, min_periodic_search_timer);

  /* If timer_value is equal to ~0 than use SIM read value*/
  if(timer_value  == REG_TIMERS_MAX_TIMER_VALUE)
  {
    reg_timer_hplmn_timer = timer_value;
  }
  else
  {
    reg_timer_hplmn_timer = timer_value*60000; 
    timer_count = reg_timer_hplmn_timer;
  }

  curr_timer = timer_get(&reg_timers_hplmn_search_period_timer, T_MSEC);


  /*If timer is currenctly running and value is more 
    than  new hplmn_search_period then restart the timer
     */
  if(curr_timer > 0 && curr_timer > timer_count )
  {
    reg_timers_stop_hplmn_search_period_timer();
    (void) timer_set
    (
      &reg_timers_hplmn_search_period_timer,
      timer_count,
      0,
      T_MSEC
    );

    MSG_HIGH_DS_1(REG_SUB, "=REG= Started HPLMN Search Timer (%d minutes)", (word)(timer_count/60000));
  }

}


/*===========================================================================
FUNCTION NAME 
         reg_timers_get_hplmn_search_timer

DESCRIPTION
  Function to update the HPLMN timer value
  
RETURN VALUE
  None
===========================================================================*/

uint32 reg_timers_get_hplmn_search_timer(void)
{
  uint32 hplmn_search_period = 0;


  if(reg_timer_hplmn_timer == REG_TIMERS_MAX_TIMER_VALUE)
  {
#ifdef FEATURE_CIOT
    reg_set_ciot_hplmn_timer (&hplmn_search_period);
    /* Express in minutes */
    hplmn_search_period *= 60;
#else
    hplmn_search_period = reg_sim_read_hplmn_search_period()*REG_TIMERS_HPLMN_SEARCH_PERIOD_UNIT/60000;
#endif
  }
  else
  {
    hplmn_search_period = reg_timer_hplmn_timer/60000;
  }

  return hplmn_search_period;
}

#if defined FEATURE_TDSCDMA || defined FEATURE_SGLTE
/*===========================================================================
FUNCTION NAME 
         reg_is_lte_timer_running

DESCRIPTION
  Returns the boolean value to indicate if the LTE timer of a given parameter
  is running
  
RETURN VALUE
  None
===========================================================================*/

boolean reg_is_lte_timer_running(reg_lte_timer_e_type timer)
{

  boolean val = FALSE;

  switch (timer)
  {
    case REG_LTE_LONG_TIMER:
    val = (reg_timer_long_timer_started == TRUE && 
            reg_timer_short_timer_started == FALSE);
    break;

    case REG_LTE_SHORT_TIMER:
    val = (reg_timer_long_timer_started == FALSE && 
            reg_timer_short_timer_started == TRUE);
    break;  
  default:
      break;
  }

  return val;

}
#endif


/*===========================================================================
FUNCTION NAME 
  reg_timers_psm_save_hplmn_search_timer

DESCRIPTION
  This function saves hplmn search timer data into psm_cache
  
RETURN VALUE
  None
===========================================================================*/
void reg_timers_psm_save_hplmn_search_timer(void)
{
  dword remaining_timer_value = 0;
  remaining_timer_value = timer_get(&reg_timers_hplmn_search_period_timer, T_MSEC);

  if(reg_nv_psm_cache_ptr != NULL)
  {
    reg_nv_psm_cache_ptr->hplmn_search_period_timer_saved = FALSE;
    if(remaining_timer_value != 0)
    {
      reg_nv_psm_cache_ptr->hplmn_search_period_timer_rem_duration = remaining_timer_value;

      reg_nv_psm_cache_ptr->hplmn_timer = reg_timer_hplmn_timer;

      reg_nv_psm_cache_ptr->hplmn_search_period_timer_saved = TRUE;
    }
  }
}

/*===========================================================================
FUNCTION NAME 
  reg_timers_psm_restore_hplmn_search_timer

DESCRIPTION
  This function restores hplmn search timer upon PSM exit
  
RETURN VALUE
  None
===========================================================================*/
void reg_timers_psm_restore_hplmn_search_timer(void)
{
  uint64 time_elapsed = 0;
  dword timer_count = 0;

  reg_check_for_null_ptr((void*)reg_nv_psm_cache_ptr);

  if(reg_nv_psm_cache_ptr->hplmn_search_period_timer_saved == TRUE)
  {
    reg_state_prev_plmn_set(reg_nv_psm_cache_ptr->plmn);

    reg_timer_hplmn_timer = reg_nv_psm_cache_ptr->hplmn_timer;


    time_elapsed = time_get_ms_native() - reg_nv_psm_cache_ptr->timers_timestamp;

    /* Run the timer for remaining duration if not expired in PSM sleep else start 10 secs timer */
    if((uint64)reg_nv_psm_cache_ptr->hplmn_search_period_timer_rem_duration > time_elapsed)
    {
      timer_count = (dword)((uint64)reg_nv_psm_cache_ptr->hplmn_search_period_timer_rem_duration - time_elapsed);
      (void) timer_set
      (
        &reg_timers_hplmn_search_period_timer,
        timer_count,
        0,
        T_MSEC
      );

      MSG_HIGH_DS_1(REG_SUB, "=REG= HPLMN Search Timer is running in PSM. Restarting for remaining duration (%d minutes)", (word)(timer_count/60000));
    }
    else
    {
#ifdef FEATURE_CIOT      
      /* Timer has expired during PSM sleep */
      reg_state_set_timer_expired(TRUE);      
#endif
      reg_timers_start_hplmn_search_timer_10sec();
    }
  }
}
#ifdef FEATURE_CIOT
/*===========================================================================
FUNCTION NAME 
  reg_timers_psm_save_ciot_irat_timer

DESCRIPTION
  This function saves ciot irat timer data into psm_cache
  
RETURN VALUE
  None
===========================================================================*/
void reg_timers_psm_save_ciot_irat_timer(void)
{
  dword remaining_timer_value = 0;
  remaining_timer_value = timer_get(&reg_timers_ciot_irat_search_period_timer, T_MSEC);

  if(reg_nv_psm_cache_ptr != NULL)
  {
    reg_nv_psm_cache_ptr->ciot_irat_timer_saved = FALSE;
    if(remaining_timer_value != 0)
    {
      reg_nv_psm_cache_ptr->ciot_irat_timer_rem_duration = remaining_timer_value;      
      reg_nv_psm_cache_ptr->ciot_irat_timer_saved = TRUE;
    }
  }
}



/*===========================================================================
FUNCTION NAME 
  reg_timers_psm_save_ciot_nb1_interleave_timers

DESCRIPTION
  This function saves nb1  data into psm_cache
  
RETURN VALUE
  None
===========================================================================*/
void reg_timers_psm_save_ciot_nb1_interleave_timers(void)
{
  dword remaining_nb1_timer_value = 0;
  dword remaining_fullband_timer_value = 0;
  remaining_nb1_timer_value = timer_get(&reg_timers_nb1_acq_tier3, T_MSEC);
  remaining_fullband_timer_value = timer_get(&reg_timers_nb1_fullband_tier3, T_MSEC);

  if(reg_nv_psm_cache_ptr != NULL)
  {
    reg_nv_psm_cache_ptr->ciot_nb1_acq_tier3_saved = FALSE;
	reg_nv_psm_cache_ptr->ciot_full_fullband_tier3_saved = FALSE;
	reg_nv_psm_cache_ptr->ciot_cm_serv_counter=reg_mode_get_cm_serv_req_count();
    if(remaining_nb1_timer_value != 0)
    {
      reg_nv_psm_cache_ptr->ciot_nb1_acq_tier3_rem_duration = remaining_nb1_timer_value;      
      reg_nv_psm_cache_ptr->ciot_nb1_acq_tier3_saved = TRUE;
    }
	if(remaining_fullband_timer_value != 0)
    {
      reg_nv_psm_cache_ptr->ciot_nb1_fullband_tier3_rem_duration = remaining_fullband_timer_value;      
      reg_nv_psm_cache_ptr->ciot_full_fullband_tier3_saved = TRUE;
    }
  }
}


/*===========================================================================
FUNCTION NAME 
  reg_timers_psm_restore_nb1_interleave_timers

DESCRIPTION
  This function restores NB1 timer upon PSM exit
  
RETURN VALUE
  None
===========================================================================*/
void reg_timers_psm_restore_nb1_interleave_timers(void)
{
  uint64 time_elapsed = 0;
  dword timer_count = 0;

  reg_check_for_null_ptr((void*)reg_nv_psm_cache_ptr);

  if(reg_nv_psm_cache_ptr->ciot_nb1_acq_tier3_saved == TRUE)
  {    
    time_elapsed = time_get_ms_native() - reg_nv_psm_cache_ptr->timers_timestamp;

    /* Run the timer for remaining duration if not expired in PSM sleep else start 10 secs timer */
    if((uint64)reg_nv_psm_cache_ptr->ciot_nb1_acq_tier3_rem_duration > time_elapsed)
    {
      timer_count = (dword)((uint64)reg_nv_psm_cache_ptr->ciot_nb1_acq_tier3_rem_duration - time_elapsed);
      (void) timer_set
      (
        &reg_timers_nb1_acq_tier3,
        timer_count,
        0,
        T_MSEC
      );

      MSG_HIGH_DS_1(REG_SUB, "=REG= NB1 ACQ dB Timer is running in PSM. Restarting for remaining duration (%d minutes)", (word)(timer_count/60000));
    }
    else
    {
       MSG_HIGH_DS_0(REG_SUB, "=REG= NB1 ACQ dB timer expired during PSM");
    }
  }
  
  time_elapsed=0;
  timer_count=0;
  if(reg_nv_psm_cache_ptr->ciot_full_fullband_tier3_saved == TRUE)
  {    
    time_elapsed = time_get_ms_native() - reg_nv_psm_cache_ptr->timers_timestamp;

    /* Run the timer for remaining duration if not expired in PSM sleep else start 10 secs timer */
    if((uint64)reg_nv_psm_cache_ptr->ciot_nb1_fullband_tier3_rem_duration > time_elapsed)
    {
      timer_count = (dword)((uint64)reg_nv_psm_cache_ptr->ciot_nb1_fullband_tier3_rem_duration - time_elapsed);
      (void) timer_set
      (
        &reg_timers_nb1_fullband_tier3,
        timer_count,
        0,
        T_MSEC
      );

      MSG_HIGH_DS_1(REG_SUB, "=REG= NB1 Full Band  Timer is running in PSM. Restarting for remaining duration (%d minutes)", (word)(timer_count/60000));
    }
    else
    {
       MSG_HIGH_DS_0(REG_SUB, "=REG= NB1 Full Band timer expired during PSM");
    }
  }

  reg_mode_set_cm_serv_req_count(reg_nv_psm_cache_ptr->ciot_cm_serv_counter);

  MSG_HIGH_DS_1(REG_SUB, "=REG= CM service counter re-stored = %d)", reg_nv_psm_cache_ptr->ciot_cm_serv_counter);
}




/*===========================================================================
FUNCTION NAME 
  reg_timers_psm_restore_ciot_irat_timer

DESCRIPTION
  This function restores ciot irat timer upon PSM exit
  
RETURN VALUE
  None
===========================================================================*/
void reg_timers_psm_restore_ciot_irat_timer(void)
{
  uint64 time_elapsed = 0;
  dword timer_count = 0;

  reg_check_for_null_ptr((void*)reg_nv_psm_cache_ptr);

  if(reg_nv_psm_cache_ptr->ciot_irat_timer_saved == TRUE)
  {    
    time_elapsed = time_get_ms_native() - reg_nv_psm_cache_ptr->timers_timestamp;

    /* Run the timer for remaining duration if not expired in PSM sleep else start 10 secs timer */
    if((uint64)reg_nv_psm_cache_ptr->ciot_irat_timer_rem_duration > time_elapsed)
    {
      timer_count = (dword)((uint64)reg_nv_psm_cache_ptr->ciot_irat_timer_rem_duration - time_elapsed);
      (void) timer_set
      (
        &reg_timers_ciot_irat_search_period_timer,
        timer_count,
        0,
        T_MSEC
      );

      MSG_HIGH_DS_1(REG_SUB, "=REG= CIOT IRAT Timer is running in PSM. Restarting for remaining duration (%d minutes)", (word)(timer_count/60000));
    }
    else
    {
      /* Timer has expired during PSM sleep */
      reg_state_set_timer_expired(TRUE);
      reg_timers_start_ciot_irat_search_period_timer_10s();
    }
  }
}
/*==============================================================================

FUNCTION NAME

  reg_timers_start_ciot_irat_search_period_timer

==============================================================================*/
boolean reg_timers_start_ciot_irat_search_period_timer
(
  void
)
{  

  ciot_params_s_type params = reg_nv_get_ciot_params();
  
  /* Check if the timer is already running */
  if(!timer_get(&reg_timers_ciot_irat_search_period_timer, T_MSEC))
  {     
      (void) timer_set
      (
        &reg_timers_ciot_irat_search_period_timer,
        params.irat_timer,
        0,
        T_MIN
      );
      MSG_HIGH_DS_1(REG_SUB, "=REG= Started CIOT IRAT Search Timer (%d minutes)", params.irat_timer);   
  }
  else
  {
      return FALSE;
  }
  return TRUE;
}
/*==============================================================================

FUNCTION NAME

  reg_timers_start_ciot_irat_search_period_timer_10s

==============================================================================*/
void reg_timers_start_ciot_irat_search_period_timer_10s
(
  void
)
{  
  /* Check if the timer is already running */
  if(!timer_get(&reg_timers_ciot_irat_search_period_timer, T_MSEC))
  {     
      (void) timer_set
      (
        &reg_timers_ciot_irat_search_period_timer,
        10000,
        0,
        T_MSEC
      );
      MSG_HIGH_DS_0(REG_SUB, "=REG= Started CIOT IRAT Search Timer 10s");
  }
}
/*==============================================================================

FUNCTION NAME

  reg_timers_start_ciot_irat_search_period_timer_value

==============================================================================*/
void reg_timers_start_ciot_irat_search_period_timer_value
(
  dword value
)
{  
  /* Check if the timer is already running */
  if(!timer_get(&reg_timers_ciot_irat_search_period_timer, T_MSEC))
  {     
      (void) timer_set
      (
        &reg_timers_ciot_irat_search_period_timer,
        value,
        0,
        T_MSEC
      );
      MSG_HIGH_DS_1(REG_SUB, "=REG= Started CIOT IRAT Search Timer with value %d", value);
  }
}

#ifdef FEATURE_CIOT
/*==============================================================================

FUNCTION NAME

  reg_timers_get_rem_ciot_irat_value

==============================================================================*/

rex_timer_cnt_type reg_timers_get_rem_ciot_irat_value
(
  void
)
{
  return (timer_get(&reg_timers_ciot_irat_search_period_timer, T_MSEC));
}


/*==============================================================================

FUNCTION NAME

  reg_is_ciot_irat_timer_active

==============================================================================*/

boolean reg_is_ciot_irat_timer_active
(
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type as_id
#endif
)
{
#ifdef FEATURE_DUAL_SIM
  if(timer_get(&reg_timers_ciot_irat_search_period_timer_sim[as_id], T_MSEC) > 0)
#else
  if(timer_get(&reg_timers_ciot_irat_search_period_timer, T_MSEC) > 0)
#endif
  {
    return TRUE;
  }
  return FALSE;
}
#endif
/*==============================================================================

FUNCTION NAME

  reg_timers_stop_ciot_irat_search_period_timer

==============================================================================*/
void reg_timers_stop_ciot_irat_search_period_timer
(
  void
)
{
    MSG_HIGH_DS_0(REG_SUB, "=REG= Stopping CIOT IRAT Search Timer");

    if(timer_get(&reg_timers_ciot_irat_search_period_timer, T_MSEC))
    {        
        (void) timer_clr
        (
          &reg_timers_ciot_irat_search_period_timer,
          T_MSEC
        );
#ifdef FEATURE_CIOT
if(mm_check_gcf_flag_enabled() == TRUE)
{
		reg_nv_ciot_irat_timer_at_power_off = REG_CIOT_IRAT_TIMER_DEFAULT_VALUE; 
}
#endif
        MSG_HIGH_DS_0(REG_SUB, "=REG= Stopped CIOT IRAT Search Timer");
    }    
}

//FRACQ

void reg_timers_start_nb1_tier3_acq_timer
(
  void
)
{
  dword timer_count = reg_nv_nb1_acq_tier3_scan_timer_value_get();

  if(!timer_get(&reg_timers_nb1_acq_tier3, T_MSEC))
  {     
      MSG_HIGH_DS_0(REG_SUB, "=REG= reg_timers_start_nb1_tier3_acq_timer set");
	  (void) timer_set
	  (
	    &reg_timers_nb1_acq_tier3,
	    timer_count,
	    0,
	    T_MSEC
	  );
   }
   MSG_HIGH_DS_1(REG_SUB, "=REG= Started NB1 ACQ dB Tier3 (%d seconds)", (word)(timer_count/1000));
}

void reg_timers_start_nb1_tier3_fullband_timer
(
  void
)
{
  dword timer_count = reg_nv_nb1_fullband_tier3_scan_timer_value_get();

  if(!timer_get(&reg_timers_nb1_fullband_tier3, T_MSEC))
  {   
   MSG_HIGH_DS_0(REG_SUB, "=REG= reg_timers_start_nb1_tier3_fullband_timer set");
    (void) timer_set
    (
      &reg_timers_nb1_fullband_tier3,
      timer_count,
      0,
      T_MSEC
    );
  }

  MSG_HIGH_DS_1(REG_SUB, "=REG= Started NB1 Full band Tier3 (%d seconds)", (word)(timer_count/1000));
}

void reg_timers_stop_nb1_tier3_acq_timer
(
  void
)
{
    MSG_HIGH_DS_0(REG_SUB, "=REG= Stopping ACQ dB tier3 timer");

    if(timer_get(&reg_timers_nb1_acq_tier3, T_MSEC))
    {        
        (void) timer_clr
        (
          &reg_timers_nb1_acq_tier3,
          T_MSEC
        );
        MSG_HIGH_DS_0(REG_SUB, "=REG= Stopping ACQ dB tier3 timer");
    }    
}

void reg_timers_stop_nb1_tier3_fullband_timer
(
  void
)
{
    MSG_HIGH_DS_0(REG_SUB, "=REG= Stopping Full Band Tier3 Timer");

    if(timer_get(&reg_timers_nb1_fullband_tier3, T_MSEC))
    {        
        (void) timer_clr
        (
          &reg_timers_nb1_fullband_tier3,
          T_MSEC
        );
        MSG_HIGH_DS_0(REG_SUB, "=REG= Stopping Full Band Tier3 Timer");
    }    
}

#endif