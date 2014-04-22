/*=============================================================================

                            BAM DMUX POWER

 BAM Data Multiplexer power.

 Copyright  2016-2017 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/bam_dmux/src/bam_dmux_power.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/16   rv      Initial version.
===========================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"
#include "stdint.h"
#include "smsm.h"
#include "dsr.h"
#include "bam_dmux_log.h"
#include "bam_dmux_os.h"
#include "bam_dmux_power.h"
#include "bam_dmux_core.h"

/*=============================================================================

                         MACRO DECLARATIONS

=============================================================================*/
/** Power logging */
#undef BAM_DMUX_LOG_TAG
#define BAM_DMUX_LOG_TAG "POWER"

#define BAM_DMUX_POWER_LOG(type, str) \
  bam_dmux_power_log_state(type, str, __LINE__, __FUNCTION__)

#define BAM_DMUX_POWER_LOG_FUNC_START() BAM_DMUX_POWER_LOG(INFO, "Func Start.")
#define BAM_DMUX_POWER_LOG_FUNC_END()   BAM_DMUX_POWER_LOG(INFO, "Func End.")

/** SMSM POWER_CONTROL_ACK Timeout in msec  */
#define BAM_DMUX_POWER_ACK_TIMEOUT       (1000)

/** SMSM POWER_CONTROL ON Timeout in msec  */
#define BAM_DMUX_POWER_ON_TIMEOUT        (5000)

/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/
typedef enum {
  DBG   = 0,
  INFO  = 1,
  WRN   = 2,
  ERR   = 3,
  FATAL = 0xDEAD
} bam_dmux_power_log_type;

/*=============================================================================

                       LOCAL DATA DEFINATION

=============================================================================*/
/* Power vote state */
static boolean                  bam_dmux_power_vote_state;

/* Power control ack pending */
static boolean                  bam_dmux_power_ack_pending;

/* Mutex for A2 power control */
static bam_dmux_os_mutex_type    bam_dmux_power_mutex;

/* SMSM power control ack timer */
static bam_dmux_os_timer_type    bam_dmux_power_ack_timer;

/* SMSM power control on timer */
static bam_dmux_os_timer_type    bam_dmux_power_on_timer;

/* The handle for DSR to handle remote updates. */
static dsr_obj_t                *bam_dmux_power_dsr_handle;

/* Modem SMSM state which handled/handling, this is needed to avoid the 
 * issues with multiple times DSR calls */
static volatile uint32           bam_dmux_power_modem_smsm_state;

/*=============================================================================

                     PUBLIC VARIABLE DECLARATIONS

=============================================================================*/


/*=============================================================================

                     LOCAL FUNCTION DEFINATION

=============================================================================*/

/*=============================================================================
  FUNCTION  bam_dmux_power_log_state
=============================================================================*/
/**
 * Logs the A2 Power control state.
 *
 */
/*===========================================================================*/
static void bam_dmux_power_log_state
(
  bam_dmux_power_log_type log_type,
  const char              *str,
  uint32                  line,
  const char              *func
)
{
  const char *fmt = "%s:POWER:%s@%d:0x%X: %s. vote=%d, ack_pending=%d, " \
                    "SMSM APPS=0x%08X, Modem=0x%08X.";
  uint32 apps_smsm;
  uint32 modem_smsm;

  apps_smsm  = smsm_state_get(SMSM_APPS_STATE);
  modem_smsm = smsm_state_get(SMSM_MODEM_STATE);

  switch (log_type)
  {
    case DBG:
      BAM_DMUX_LOG_RAW_DBG(9, fmt, "DBG", func, line, 
                           bam_dmux_os_get_thread_id(), str,
                           bam_dmux_power_vote_state,
                           bam_dmux_power_ack_pending, 
                           apps_smsm, modem_smsm);
      break;

    case INFO:
      BAM_DMUX_LOG_RAW_INFO(9, fmt, "INFO", func, line, 
                           bam_dmux_os_get_thread_id(), str,
                           bam_dmux_power_vote_state,
                           bam_dmux_power_ack_pending, 
                           apps_smsm, modem_smsm);
      break;
    case WRN:
      BAM_DMUX_LOG_RAW_WRN(9, fmt, "WRN", func, line, 
                           bam_dmux_os_get_thread_id(), str,
                           bam_dmux_power_vote_state,
                           bam_dmux_power_ack_pending, 
                           apps_smsm, modem_smsm);
      break;

    case FATAL:
      BAM_DMUX_LOG_RAW_FATAL(9, fmt, "FATAL", func, line, 
                             bam_dmux_os_get_thread_id(), str,
                             bam_dmux_power_vote_state,
                             bam_dmux_power_ack_pending, 
                             apps_smsm, modem_smsm);
      break;

    case ERR:
    default:
      BAM_DMUX_LOG_RAW_ERR(9, fmt, "ERR", func, line, 
                           bam_dmux_os_get_thread_id(), str,
                           bam_dmux_power_vote_state,
                           bam_dmux_power_ack_pending, 
                           apps_smsm, modem_smsm);
      break;
  }
}

/*=============================================================================
  FUNCTION  bam_dmux_power_update_a2_pc
=============================================================================*/
/**
 * Update the APPS SMSM A2 power control bit and start the ack timer.
 *
 */
/*===========================================================================*/
static void bam_dmux_power_update_a2_pc(boolean vote)
{
  BAM_DMUX_POWER_LOG_FUNC_START();

  if (vote)
  {
    /* Set APPS power control */
    smsm_state_set(SMSM_APPS_STATE, SMSM_A2_POWER_CONTROL);
  }
  else
  {
    /* Clear APPS power control */
    smsm_state_clr(SMSM_APPS_STATE, SMSM_A2_POWER_CONTROL);
  }

  /* Set the ack pending flag */
  bam_dmux_power_ack_pending = TRUE;

  /* Modem should toggling SMSM power control ack within 1 msec,
   * starting timer to monitor */
  bam_dmux_os_timer_set(&bam_dmux_power_ack_timer, BAM_DMUX_POWER_ACK_TIMEOUT);

  BAM_DMUX_POWER_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_power_a2_pc_ack
=============================================================================*/
/**
 * Acknowledges Modem A2 power control update by toggling APPS SMSM A2
 * power control ack bit.
 *
 */
/*===========================================================================*/
static void bam_dmux_power_a2_pc_ack(void)
{
  BAM_DMUX_POWER_LOG_FUNC_START();

  bam_dmux_os_mutex_lock(&bam_dmux_power_mutex);

  /* A2 BAM power events are acknowledged by toggling the SMSM ACK bit */
  if (smsm_state_get(SMSM_APPS_STATE) & SMSM_A2_POWER_CONTROL_ACK)
  {
    smsm_state_clr(SMSM_APPS_STATE, SMSM_A2_POWER_CONTROL_ACK);
  }
  else
  {
    smsm_state_set(SMSM_APPS_STATE, SMSM_A2_POWER_CONTROL_ACK);
  }

  bam_dmux_os_mutex_unlock(&bam_dmux_power_mutex);

  BAM_DMUX_POWER_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_power_a2_pc_ack_cb
=============================================================================*/
/**
 * This function is called when Modem acknowledges APPS A2 power control update.
 *
 */
/*===========================================================================*/
static void bam_dmux_power_a2_pc_ack_cb(void)
{
  boolean notify;
  boolean vote;

  BAM_DMUX_POWER_LOG_FUNC_START();

  bam_dmux_os_mutex_lock(&bam_dmux_power_mutex);

  /* A2 toggling SMSM power control ack, clear the timer */
  bam_dmux_os_timer_clr(&bam_dmux_power_ack_timer);

  /* Notify the Core if is waiting for confirm */
  notify = bam_dmux_power_ack_pending;
  vote   = bam_dmux_power_vote_state;

  if (!bam_dmux_power_ack_pending)
  {
    BAM_DMUX_POWER_LOG(ERR, "BAM DMUX is not waiting for ack.");
  }
  
  /* Clear the ack pending flag */
  bam_dmux_power_ack_pending = FALSE;

  /* Start the Power on timer if APPS vote and A2 is not yet power on */
  if (bam_dmux_power_vote_state && 
      !(bam_dmux_power_modem_smsm_state & SMSM_A2_POWER_CONTROL)) 
  {
    /* Start power on timer */
    bam_dmux_os_timer_set(&bam_dmux_power_on_timer, BAM_DMUX_POWER_ON_TIMEOUT);
  }

  bam_dmux_os_mutex_unlock(&bam_dmux_power_mutex);

  BAM_DMUX_POWER_LOG_FUNC_END();
  
  if (notify)
  {
    /* Notify to the core layer about vote */
    bam_dmux_power_notify(vote ? BAM_DMUX_POWER_EVENT_VOTED : 
                          BAM_DMUX_POWER_EVENT_UNVOTED);
  }
}

/*=============================================================================
  FUNCTION  bam_dmux_power_a2_pc_on_cb
=============================================================================*/
/**
 * This function is called when Modem A2 power control is set.
 * This function calls Core layer to notify the A2 Power state.
 *
 */
/*===========================================================================*/
static void bam_dmux_power_a2_pc_on_cb(void)
{
  BAM_DMUX_POWER_LOG_FUNC_START();

  bam_dmux_os_mutex_lock(&bam_dmux_power_mutex);

  /* Clear the power on timer */
  bam_dmux_os_timer_clr(&bam_dmux_power_on_timer);

  bam_dmux_os_mutex_unlock(&bam_dmux_power_mutex);

  /* Notify to the core layer about power on */
  bam_dmux_power_notify(BAM_DMUX_POWER_EVENT_ON);

  /* Toggle APPS SMSM power control ack to acknowlede A2 */
  bam_dmux_power_a2_pc_ack();

  BAM_DMUX_POWER_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_power_a2_pc_off_cb
=============================================================================*/
/**
 * This function is called when Modem A2 power control is cleared.
 * This function calls Core layer to notify the A2 Power state.
 *
 */
/*===========================================================================*/
static void bam_dmux_power_a2_pc_off_cb(void)
{
  BAM_DMUX_POWER_LOG_FUNC_START();

  bam_dmux_os_mutex_lock(&bam_dmux_power_mutex);

  /* Check if we vote for power down */
  if ((smsm_state_get(SMSM_APPS_STATE) & SMSM_A2_POWER_CONTROL) &&
      (bam_dmux_power_ack_pending == FALSE))
  {
    BAM_DMUX_POWER_LOG(ERR, "BAM DMUX is not voted from Power down, "
        "even though A2 is power down.");
  }

  bam_dmux_os_mutex_unlock(&bam_dmux_power_mutex);

  /* Notify to the core layer about power off */
  bam_dmux_power_notify(BAM_DMUX_POWER_EVENT_OFF);

  /* Toggle APPS SMSM power control ack to acknowlede A2 */
  bam_dmux_power_a2_pc_ack();

  BAM_DMUX_POWER_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_power_dsr_cb
=============================================================================*/
/**
 *  This function is called when the remote processor changes the SMSM bits
 *  related to A2 BAM power collapse.
 *
 * @param[in] payload      State that host was previously in
 *
 */
/*===========================================================================*/
static void bam_dmux_power_dsr_cb(dsr_obj_t *dsr_obj, void *ctxt, void *payload)
{
  uint32 prev;
  uint32 curr;

  BAM_DMUX_POWER_LOG_FUNC_START();

  bam_dmux_os_mutex_lock(&bam_dmux_power_mutex);
  
  curr = smsm_state_get(SMSM_MODEM_STATE);
  curr &= (SMSM_A2_POWER_CONTROL | SMSM_A2_POWER_CONTROL_ACK);

  prev = bam_dmux_power_modem_smsm_state;
  
  /* Check if there is any change */
  if (prev == curr)
  {
    /* There is no change, just return */
    bam_dmux_os_mutex_unlock(&bam_dmux_power_mutex);
    return;
  }
  else
  {
    /* There is change, update and handle new Modem state */
    bam_dmux_power_modem_smsm_state = curr;
    bam_dmux_os_mutex_unlock(&bam_dmux_power_mutex);
  }

  /***************************************************************
   * Check for acknowlede from Modem of the Apps request to
   * Power Control A2 BAM (toggle on the state).
   ***************************************************************/
  if ((curr & SMSM_A2_POWER_CONTROL_ACK) != (prev & SMSM_A2_POWER_CONTROL_ACK))
  {
    bam_dmux_power_a2_pc_ack_cb();
  }

  /* Check for updates in remote power control request */
  if ((curr & SMSM_A2_POWER_CONTROL) != (prev & SMSM_A2_POWER_CONTROL))
  {
    if (curr & SMSM_A2_POWER_CONTROL)
    {
      bam_dmux_power_a2_pc_on_cb();
    }
    else
    {
      bam_dmux_power_a2_pc_off_cb();
    }
  }

  BAM_DMUX_POWER_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_power_smsm_cb
=============================================================================*/
/**
 *  This function is called when the remote processor changes the SMSM bits
 *  related to A2 BAM power collapse.
 *
 * @param[in] host            Which host the state change is for
 * @param[in] prev_state      State that host was previously in
 * @param[in] curr_state      State that host is currently in
 * @param[in] data            Callback data
 *
 */
/*===========================================================================*/
static void bam_dmux_power_smsm_cb
(
  smsm_entry_type  host,
  uint32           prev,
  uint32           curr,
  void            *data
)
{
  BAM_DMUX_POWER_LOG_FUNC_START();
  
  /* Enqueue DSR to process in the DSR thread context, instead of SMSM context */
  dsr_enqueue(bam_dmux_power_dsr_handle, (void*)prev);

  BAM_DMUX_POWER_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_power_ack_timeout_cb
=============================================================================*/
/**
 * A2 Power control acknowledement timedout.
 *
 */
/*===========================================================================*/
static void bam_dmux_power_ack_timeout_cb(void *unused)
{
  boolean fatal = FALSE;

  BAM_DMUX_POWER_LOG_FUNC_START();

  bam_dmux_os_mutex_lock(&bam_dmux_power_mutex);

  if (bam_dmux_power_ack_pending)
  {
    /* Check if ack is already set */
    if ((bam_dmux_power_modem_smsm_state & SMSM_A2_POWER_CONTROL_ACK) != 
        (smsm_state_get(SMSM_MODEM_STATE) & SMSM_A2_POWER_CONTROL_ACK))
    {
      BAM_DMUX_POWER_LOG(INFO, "Looks like Ack is set just before the timedout.");
    }
    else
    {
      if (smsm_state_get(SMSM_APPS_STATE) & SMSM_A2_POWER_CONTROL)
      {
        BAM_DMUX_POWER_LOG(ERR, "A2 power on request ack is timedout.");
      }
      else
      {
        BAM_DMUX_POWER_LOG(ERR, "A2 power off request ack is timedout.");
      }
      fatal = TRUE;
    }
  }
  else
  {
    BAM_DMUX_POWER_LOG(ERR, "Unexpected A2 power ack timedout.");
  }

  bam_dmux_os_mutex_unlock(&bam_dmux_power_mutex);

  /* Notify to the core layer about power error */
  if (fatal)
  {
    bam_dmux_power_notify(BAM_DMUX_POWER_EVENT_FATAL);
  }

  BAM_DMUX_POWER_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_power_on_timeout_cb
=============================================================================*/
/**
 * A2 Power control on timedout.
 *
 */
/*===========================================================================*/
static void bam_dmux_power_on_timeout_cb(void *unused)
{
  boolean fatal = FALSE;

  BAM_DMUX_POWER_LOG_FUNC_START();

  bam_dmux_os_mutex_lock(&bam_dmux_power_mutex);

  if (bam_dmux_power_vote_state &&
      (smsm_state_get(SMSM_APPS_STATE) & SMSM_A2_POWER_CONTROL) &&
      !bam_dmux_power_ack_pending &&
      !(smsm_state_get(SMSM_MODEM_STATE) & SMSM_A2_POWER_CONTROL))
  {
    BAM_DMUX_POWER_LOG(ERR, "A2 power control on timedout.");
    fatal = TRUE;
  }

  bam_dmux_os_mutex_unlock(&bam_dmux_power_mutex);

  /* Notify to the core layer about power error */
  if (fatal)
  {
    bam_dmux_power_notify(BAM_DMUX_POWER_EVENT_FATAL);
  }

  BAM_DMUX_POWER_LOG_FUNC_END();
}

/*=============================================================================

                     PUBLIC FUNCTION DEFINATION

=============================================================================*/

/*=============================================================================
  FUNCTION  bam_dmux_power_init
=============================================================================*/
/**
 * Initialization function. Registers callback function with SMSM to receive
 * the A2 power control updates.
 *
 */
/*===========================================================================*/
void bam_dmux_power_init(void)
{
  int ret;

  BAM_DMUX_POWER_LOG_FUNC_START();

  bam_dmux_os_mutex_init(&bam_dmux_power_mutex);

  bam_dmux_power_vote_state  = FALSE;
  bam_dmux_power_ack_pending = FALSE;

  bam_dmux_os_timer_init(&bam_dmux_power_ack_timer,
                         bam_dmux_power_ack_timeout_cb,
                         NULL);

  bam_dmux_os_timer_init(&bam_dmux_power_on_timer,
                         bam_dmux_power_on_timeout_cb,
                         NULL);

  if (dsr_create(&bam_dmux_power_dsr_handle, 
                 bam_dmux_power_dsr_cb, 
                 NULL, 
                 DSR_PRI_HIGH, 
                 DSR_CONTEXT_ISR) == FALSE)
  {
    BAM_DMUX_LOG_FATAL(0, "dsr_create failed!");
  }

  /**********************************************************************
   * Register for A2 power control bits
   *   - POWER_CONTROL, for A2 requested power collapse
   *   - POWER_CONTROL_ACK, as an acknowledge from A2 for Apps
   *     requested power collapse
   **********************************************************************/
  ret = smsm_cb_register(SMSM_MODEM_STATE,
                         SMSM_A2_POWER_CONTROL | SMSM_A2_POWER_CONTROL_ACK,
                         bam_dmux_power_smsm_cb,
                         NULL);
  if (ret != 1)
  {
    BAM_DMUX_LOG_FATAL(1, "smsm_cb_register failed!, ret=%d.", ret);
  }
  /* Check is power is already set */
  if (SMSM_A2_POWER_CONTROL & smsm_state_get(SMSM_MODEM_STATE))
  {
    dsr_enqueue(bam_dmux_power_dsr_handle, (void*)0);
  }

  BAM_DMUX_POWER_LOG_FUNC_END();
}

/*=============================================================================
  FUNCTION  bam_dmux_power_reset
=============================================================================*/
/**
 * TODO
 */
/*===========================================================================*/
void bam_dmux_power_reset(void)
{
  BAM_DMUX_LOG_FATAL(0, "Function is not supported!");
}

/*=============================================================================
  FUNCTION  bam_dmux_power_vote
=============================================================================*/
/**
 * Votes for A2 Power. Core layer calls this function.
 * Sets SMSM Power Control bit (if not previously set).
 *
 */
/*===========================================================================*/
boolean bam_dmux_power_vote(void)
{
  boolean result = FALSE;
  
  BAM_DMUX_POWER_LOG_FUNC_START();

  bam_dmux_os_mutex_lock(&bam_dmux_power_mutex);

  /* Check if alreay voted or if ack is pending for last vote request */
  if ((bam_dmux_power_vote_state == TRUE) || 
      (bam_dmux_power_ack_pending == TRUE) ||
      (smsm_state_get(SMSM_APPS_STATE) & SMSM_A2_POWER_CONTROL))
  {
    BAM_DMUX_POWER_LOG(WRN, "Power control ack is pending or already voted.");
    goto UNLOCK_EXIT;
  }

  /* Update the state */
  bam_dmux_power_vote_state = TRUE;

  /* Update the A2 Power control */
  bam_dmux_power_update_a2_pc(bam_dmux_power_vote_state);

  /* Successfully voted for A2 power */  
  result = TRUE;

UNLOCK_EXIT:
  bam_dmux_os_mutex_unlock(&bam_dmux_power_mutex);

  BAM_DMUX_POWER_LOG_FUNC_END();
  
  return result;
}

/*=============================================================================
  FUNCTION  bam_dmux_power_unvote
=============================================================================*/
/**
 * Unvotes for A2 Power. Core layer calls this function.
 * Clears SMSM Power Control bit.
 *
 */
/*===========================================================================*/
boolean bam_dmux_power_unvote(void)
{
  boolean result = FALSE;
  
  BAM_DMUX_POWER_LOG_FUNC_START();

  bam_dmux_os_mutex_lock(&bam_dmux_power_mutex);

  /* Check if alreay unvoted or if ack is pending for last unvote request */
  if ((bam_dmux_power_vote_state == FALSE) || 
      (bam_dmux_power_ack_pending == TRUE) ||
      !(smsm_state_get(SMSM_APPS_STATE) & SMSM_A2_POWER_CONTROL))
  {
    BAM_DMUX_POWER_LOG(WRN, "Power control ack is pending or already unvoted.");
    goto UNLOCK_EXIT;
  }

  /* Update the state */
  bam_dmux_power_vote_state = FALSE;

  /* Update the A2 Power control */
  bam_dmux_power_update_a2_pc(bam_dmux_power_vote_state);

  /* Successfully unvoted for A2 power */  
  result = TRUE;

UNLOCK_EXIT:
  bam_dmux_os_mutex_unlock(&bam_dmux_power_mutex);

  BAM_DMUX_POWER_LOG_FUNC_END();
  
  return result;
}
