/*==============================================================================

  HSU Timer Abstraction
  
  GENERAL DESCRIPTION

  EXTERNALIZED FUNCTIONS

  INITALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_timer.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  08/01/12  ac      Initial version
==============================================================================*/

#include "jos_int.h"
#include "hsu_timer.h"
#include "hsu_log.h"
#include "hsu_al_task_int.h"
#include "hsu_qurt_util.h"

static hsu_timer_ctx_type hsu_timer_ctx;
static unsigned long hsu_timer_inlock_flag;

/*============================================================================
  hsu_timer_chk_timer_hnd_validity
==============================================================================*/
/* Function to see if timer ID is valid */
static void hsu_timer_chk_timer_hnd_validity(hsu_timer_handle_type* l_timer_hnd)
{
  if ((NULL == l_timer_hnd) || !(hsu_timer_ctx.timers[*l_timer_hnd].is_allocated))
  {
    HSU_ERR_FATAL("hsu_timer_chk_timer_hnd_validity: Invalid Timer Handle\n", 0, 0, 0);
  }
}

/*============================================================================
  hsu_timer_cb_al_task_queued
==============================================================================*/
static void hsu_timer_cb_al_task_queued(void* timer_cb_data)
{
  hsu_timer_type *timer_ptr = (hsu_timer_type*)timer_cb_data;

  HSU_ASSERT(timer_ptr);
  HSU_ASSERT(timer_ptr->usr_cb);

  hsu_timer_log(timer_ptr);
  
  timer_ptr->usr_cb(timer_ptr->usr_data);
}

/*============================================================================
  hsu_timer_cb
==============================================================================*/
void hsu_timer_cb(timer_cb_data_type data)
{
  hsu_timer_type *timer_ptr = (hsu_timer_type*)data;

  HSU_ASSERT(timer_ptr);
  HSU_ASSERT(timer_ptr->usr_cb);

  if (timer_ptr->feature_mask & HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX)
  {
    hsu_al_task_enqueue_cmd_ex(hsu_timer_cb_al_task_queued, (void *)timer_ptr, TRUE);
  }
  else if (timer_ptr->feature_mask & HSU_TIMER_CB_IN_HSU_AL_TASK_WITHOUT_GIANT_MUTEX)
  {
    hsu_al_task_enqueue_cmd_ex(hsu_timer_cb_al_task_queued, (void *)timer_ptr, FALSE);
  }
  else
  {
    hsu_timer_log(timer_ptr);

    timer_ptr->usr_cb(timer_ptr->usr_data);
  }
}

/*============================================================================
  hsu_timer_init
==============================================================================*/
hsu_timer_handle_type hsu_timer_init
(
  hsu_timer_client_id_type    client_id,
  hsu_timer_feature_bit_field feature_mask, 
  timer_t1_cb_type            usr_cb, 
  timer_cb_data_type          usr_data
)
{
  uint32 i = 0;
  uint32 first_empty_slot_index = 1;
  boolean found_empty_slot = FALSE;
  boolean found_duplicate_entry = FALSE;
  hsu_timer_type *timer_ptr = NULL;
  hsu_timer_handle_type ret_hnd = 0;

  if (!usr_cb)
  {
    HSU_ERR_FATAL("hsu timer usr_cb is NULL", 0, 0, 0);
  }
  
  /* 
   * check whether timer exists AND
   * search the first empty timer slot while performing loop
   */
 INTLOCK_SAVE(hsu_timer_inlock_flag)
  for (i = 1, found_empty_slot = FALSE; i < HSU_TOTAL_NUM_TIMER; ++i)
  {    
    timer_ptr = &(hsu_timer_ctx.timers[i]);

    if (!(timer_ptr->is_allocated))
    {
      first_empty_slot_index = i;
      found_empty_slot = TRUE;
      
      timer_ptr->is_allocated = TRUE;      
      timer_ptr->feature_mask = feature_mask;
      timer_ptr->usr_cb = usr_cb;
      timer_ptr->usr_data = usr_data;
      timer_ptr->client_id = client_id; 
      break;
    }

    if ( (hsu_timer_ctx.timers[i].client_id == client_id)
      && (timer_ptr->usr_cb == usr_cb)
      && (timer_ptr->usr_data == usr_data) )      
    {
      found_duplicate_entry = TRUE;
      ret_hnd = i;
      timer_ptr->duplicate_entry_count++;
      break;
    }
  }
  INTFREE_RESTORE(hsu_timer_inlock_flag)

  if (found_duplicate_entry)
  {
    HSU_MSG_ERROR_1("HSU_TIMER_ALREADY_EXIST on handle %u", ret_hnd);
    return ret_hnd;
  }
  else if (!found_empty_slot)
  {
    HSU_ERR_FATAL("HSU_TIMER_NO_MORE_EMPTY_SLOT_TO_CREATE_TIMER\n", 0, 0, 0);
  }

  /* create a new timer */
  if (!(feature_mask & HSU_TIMER_IS_TIMER_DEFERRABLE))
  {
    /* non-deferrable timer creation */   

    timer_def_osal(
      &(timer_ptr->timer),
      &(timer_ptr->timer_group),
      TIMER_FUNC1_CB_TYPE,
      hsu_timer_cb,
      (timer_cb_data_type)timer_ptr
      );    

    ret_hnd = first_empty_slot_index;
  }
  else
  {
    HSU_ERR_FATAL("HSU_TIMER_FEATURE_UNSUPPORTED\n", 0, 0, 0); /* implementation to be completed for deferrable timers */
  }
  return ret_hnd;
}

/*============================================================================
  hsu_timer_set
==============================================================================*/
void hsu_timer_set(hsu_timer_handle_type *timer_hnd, timetick_type expiry_time, timer_unit_type time_unit)
{
  hsu_timer_type *timer_ptr = NULL;

  hsu_timer_chk_timer_hnd_validity(timer_hnd);
  expiry_time = expiry_time / HSU_EMULATION_TIME_FACTOR;

  timer_ptr = &(hsu_timer_ctx.timers[*timer_hnd]);
  if ((timer_ptr->feature_mask) & HSU_TIMER_DISALLOW_REPRIME_WHILE_TIMER_ACTIVE)
  {
    if (0 != hsu_timer_get_time(timer_hnd, T_MSEC))
    {
      HSU_ERR_FATAL("hsu_timer_set(): timer already in use\n", 0, 0, 0);
    }
  }

  /* Commence timer countdown */
  timer_ptr->timer_val = expiry_time;

  timer_set(
      &(timer_ptr->timer),
      timer_ptr->timer_val,
      0,
      time_unit);
}

/*============================================================================
  hsu_timer_cancel
==============================================================================*/
void hsu_timer_cancel(hsu_timer_handle_type *timer_hnd)
{
  hsu_timer_type *timer_ptr = NULL;

  HSU_ASSERT(timer_hnd != NULL);

  /* Since timer cancelation can be done librarally without the time being 
  ** created, check here and log that timer is being cancelled when it was not
  ** scheduled.
  */
  if (*timer_hnd == HSU_TIMER_UNINITIALIZED_HANDLE)
  {
    HSU_ULOG(NO_MSG, BUS_LOG, "uninitialized_handle");
    return;
  }
  
  hsu_timer_chk_timer_hnd_validity(timer_hnd);

  timer_ptr = &(hsu_timer_ctx.timers[*timer_hnd]);
  if (timer_ptr->timer_val != 0)
  {
    (void)timer_clr(&(timer_ptr->timer), T_NONE);
     timer_ptr->timer_val = 0;
  }
  /* else there is no timer to cancel.  Function is safe to be called multiple times */
}

/*============================================================================
  hsu_timer_get_time
==============================================================================*/
timetick_type hsu_timer_get_time(hsu_timer_handle_type *timer_hnd, timer_unit_type time_unit)
{
  timetick_type ret_time =0;
  hsu_timer_type *timer_ptr = NULL;

  hsu_timer_chk_timer_hnd_validity(timer_hnd);

  timer_ptr = &(hsu_timer_ctx.timers[*timer_hnd]);
  ret_time = timer_get(&(timer_ptr->timer), time_unit);
  return ret_time;
}

/*============================================================================
  hsu_timer_deinit
==============================================================================*/
void hsu_timer_deinit(hsu_timer_handle_type *timer_hnd)
{
  hsu_timer_chk_timer_hnd_validity(timer_hnd);

  /* there is no deregistration in timer APIs */
  /* stub left here for completeness */
}

