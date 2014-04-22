/*==============================================================================

  HSU BAM State Machine

  GENERAL DESCRIPTION

  EXTERNALIZED FUNCTIONS

  INITALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_no_ecm_bam_sm.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  08/15/13  ac      Initial version
==============================================================================*/

#include "hsu_bam_sm.h"
#include "err.h"
#include "hsu_al_bam_utils.h"
#include "hsu_al_ecm_int.h"
#include "hsu_core_config_int.h"
#include "hsu_os_abstract_bus.h"
#include "hsu_conf_sel_stack_utils_int.h"
#include "hsu_conf_sel_stack_utils.h"
#include "hsu_al_ecm_int.h"
#include "jusb_core.h"
#include "hsu_al_task.h"
#include "hsu_al_task_int.h"
#include "hsu_timer.h"
#include "port_sync.h"
#include "hsu_bus_mode_notifier.h"
#include "hsu_platform.h"
#include "hsu_conf_sel_i.h"
#include "hsu_log.h"
#include "qdss_usb_bam.h"


#define HSU_BAM_SM_WAIT_UL_PIPE_EMPTY_CNT       50
#define HSU_BAM_SM_WAIT_UL_PIPE_EMPTY_TIME_US   20

#define HSU_BAM_SM_FORCE_SHUTDOWN_MAX_TIME      100

//#define HSU_BAM_SM_ENABLE_FORCE_SHUTDOWN_TIMER_CHK
//#define HSU_BAM_SM_UNIT_TEST_SUSPEND_THEN_FORCE_SHUTDOWN

typedef enum
{
  HSU_BAM_SM_DEFERRED = 1,
  HSU_BAM_SM_EXCEPTION,
  HSU_BAM_SM_HANDLED,
  HSU_BAM_SM_SKIP_ENTRY_FUNC_UPON_EXIT,
  HSU_BAM_SM_DISCARD_AND_NO_TRAN,

} hsu_bam_sm_tran_ret_enum; /* transition return values */

// When adding new states,
// must update hsu_generic_log_enum, hsu_event_log_enum, hsu_bam_sm_state_enum (for both BAM and NO BAM state machines)
typedef enum 
{
  FLOW_ENABLED_STATE = 1,
  FLOW_DISABLING_FROM_FORCE_SHUTDOWN,
  FLOW_DISABLED_FROM_FORCE_SHUTDOWN,

  DATA_ACTIVITY_WHILE_IN_FORCE_SHUTDOWN,
  ENUMERATION_WITH_AP,

  FLOW_DISABLING_FROM_SUSPEND,
  FLOW_DISABLED_FROM_SUSPEND,
  DATA_ACTIVITY_WHILE_SUSPENDED,
  BOOTUP_FLOW_DISABLED,

  HSU_BAM_SM_NUM_OF_STATES = BOOTUP_FLOW_DISABLED,

} hsu_bam_sm_state_enum;

typedef struct
{
  hsu_bam_sm_sig_enum inc_sig;
  hsu_bam_sm_state_enum dest_state;
} hsu_bam_sm_trans_def_type; /* describes transition */

/* state machine definitions */
typedef struct
{
  hsu_bam_sm_state_enum state;
  hsu_bam_sm_tran_ret_enum (*entry_func)(hsu_bam_sig_evt_type sig_evt); /* generic entry function when entering a state */
  hsu_bam_sm_tran_ret_enum (*exit_func)(hsu_bam_sig_evt_type sig_evt); /* generic exit function when departing from a state */
  hsu_bam_sm_tran_ret_enum (*filter_func)(hsu_bam_sig_evt_type sig_evt); /* filters out and discards event not relevant to the current state before state machine handles them */

  hsu_bam_sm_trans_def_type transition_def[HSU_BAM_SM_NUM_SIGS]; /* definition describing the transitions per incoming signal */

} hsu_bam_sm_state_mach_def_type;

typedef struct {
  hsu_bam_sm_device_ready_cb device_ready_cb;
  hsu_bam_sm_wakeup_host_cb  wakeup_host_cb;
} hsu_bam_sm_cbs_type;

typedef struct
{
  hsu_bam_sm_state_enum curr_state;
  hsu_bam_sm_state_enum prev_state;

  hsu_timer_handle_type flow_disabling_from_suspend_tmr; /* state specific timer */
  /* for narrow race condition where CB was enqueued to AL task but timer is canceled */
  boolean               flow_disabling_from_suspend_tmr_active;

#ifdef HSU_BAM_SM_ENABLE_FORCE_SHUTDOWN_TIMER_CHK
  hsu_timer_handle_type force_shutdown_tmr; /* timer from beginning of HOST READY FALL to force shutdown */
#endif /* HSU_BAM_SM_ENABLE_FORCE_SHUTDOWN_TIMER_CHK */

  boolean               force_shutdown_due_to_cable_discon;
  hsu_bam_sm_cbs_type   ext_cbs;
  hsu_timer_handle_type host_ready_hi_delay_on_bootup_timer;
  boolean               is_configured_once; // ensure DBM, DPM, IPA and BAM pipes are only configured once
} hsu_bam_sm_ctx_type;

#define HSU_BAM_SM_INVALID_STATE_EVT_ERR_FATAL() \
  HSU_ERR_FATAL("Invalid inputs. state %d, event %d", (uint32)hsu_no_ecm_bam_sm_ctx.curr_state, (uint32)HSU_BAM_GET_EVT(sig_evt), 0);

static void hsu_bam_sm_transition(hsu_bam_sig_evt_type new_sig_evt);
static void hsu_bam_sm_set_device_ready(boolean setting);

#ifdef HSU_BAM_SM_ENABLE_FORCE_SHUTDOWN_TIMER_CHK
static void hsu_bam_sm_force_shutdown_tmr_cb(timer_cb_data_type data);
#endif /* HSU_BAM_SM_ENABLE_FORCE_SHUTDOWN_TIMER_CHK */
static void hsu_bam_sm_bus_mode_notifier_cb(hsu_bus_mode_enum new_mode, void* app_data);

/* entry/exit functions */
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__flow_disabled_from_force_shutdown(hsu_bam_sig_evt_type sig_evt);
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__flow_disabled_from_force_shutdown__exit(hsu_bam_sig_evt_type sig_evt);
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__enumeration_with_ap(hsu_bam_sig_evt_type sig_evt);
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__flow_enabled(hsu_bam_sig_evt_type sig_evt);
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__flow_enabled__exit(hsu_bam_sig_evt_type sig_evt);
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__flow_disabled_from_suspend(hsu_bam_sig_evt_type sig_evt);
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__bootup_flow_disabled(hsu_bam_sig_evt_type sig_evt);

/* filter functions */
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__flow_enabled_filter(hsu_bam_sig_evt_type sig_evt);
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__flow_disabled_from_suspend_filter(hsu_bam_sig_evt_type sig_evt);
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__flow_disabled_from_force_shutdown_filter(hsu_bam_sig_evt_type sig_evt);
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__enumeration_with_ap_filter(hsu_bam_sig_evt_type sig_evt);
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__bootup_flow_disabled_filter(hsu_bam_sig_evt_type sig_evt);

/* state machine definitions */
static hsu_bam_sm_state_mach_def_type hsu_bam_sm_state_mach_def[] = 
{
  {
    /* for error checking purposes */
    (hsu_bam_sm_state_enum)(0),
    NULL,
    NULL,
    NULL,
    {
      (hsu_bam_sm_sig_enum)(0),
      (hsu_bam_sm_state_enum)(0),
    },
  },
  {
    FLOW_ENABLED_STATE,                                /* state */
    hsu_bam_sm__flow_enabled,                          /* entry_func */
    hsu_bam_sm__flow_enabled__exit,                    /* exit_func */ 
    hsu_bam_sm__flow_enabled_filter,                   /* filter_func */
    {
      {
        HSU_AP_SIDEBAND_SIG,                           /* inc_sig */
        FLOW_DISABLED_FROM_FORCE_SHUTDOWN,            /* dest_state */
      },
      {
        HSU_USB_BUS_SIG,                               /* inc_sig */
        FLOW_DISABLED_FROM_SUSPEND,                   /* dest_state */
      },
    },
  },
  {
    FLOW_DISABLING_FROM_FORCE_SHUTDOWN,                /* state */
    NULL,                                              /* entry_func */
    NULL,                                              /* exit_func */ 
    NULL,                                              /* filter_func */
    {
      {
        (hsu_bam_sm_sig_enum)(0),                      /* inc_sig */
        (hsu_bam_sm_state_enum)(0),                    /* dest_state */
      },
    },
  },
  {
    FLOW_DISABLED_FROM_FORCE_SHUTDOWN,                 /* state */
    hsu_bam_sm__flow_disabled_from_force_shutdown,     /* entry_func */
    hsu_bam_sm__flow_disabled_from_force_shutdown__exit,  /* exit_func */ 
    hsu_bam_sm__flow_disabled_from_force_shutdown_filter, /* filter_func */
    {
      {
        HSU_AP_SIDEBAND_SIG,                           /* inc_sig */
        ENUMERATION_WITH_AP,                           /* dest_state */
      },
      {
        HSU_USB_BUS_SIG,                               /* inc_sig */
        FLOW_DISABLED_FROM_FORCE_SHUTDOWN,             /* dest_state */
      },
    },
  },
  {
    DATA_ACTIVITY_WHILE_IN_FORCE_SHUTDOWN,             /* state */
    NULL,                                              /* entry_func */
    NULL,                                              /* exit_func */ 
    NULL,
    {
      {
        (hsu_bam_sm_sig_enum)(0),                      /* inc_sig */
        (hsu_bam_sm_state_enum)(0),                    /* dest_state */
      },
    },
  },
  {
    ENUMERATION_WITH_AP,                               /* state */
    hsu_bam_sm__enumeration_with_ap,                   /* entry_func */
    NULL,                                              /* exit_func */ 
    hsu_bam_sm__enumeration_with_ap_filter,            /* filter_func */
    {
      {
        HSU_AP_SIDEBAND_SIG,                           /* inc_sig */
        FLOW_DISABLED_FROM_FORCE_SHUTDOWN,             /* dest_state */
      },
      {
        HSU_USB_BUS_SIG,                               /* inc_sig */
        FLOW_ENABLED_STATE,                            /* dest_state */
      },
    },
  },
  {
    FLOW_DISABLING_FROM_SUSPEND,                       /* state */
    NULL,                                              /* entry_func */
    NULL,                                              /* exit_func */ 
    NULL,                                              /* filter_func */
    {
      {
        (hsu_bam_sm_sig_enum)(0),                      /* inc_sig */
        (hsu_bam_sm_state_enum)(0),                    /* dest_state */
      },
      {
        (hsu_bam_sm_sig_enum)(0),                      /* inc_sig */
        (hsu_bam_sm_state_enum)(0),                    /* dest_state */
      },
    },
  },
  {
    FLOW_DISABLED_FROM_SUSPEND,                        /* state */
    hsu_bam_sm__flow_disabled_from_suspend,            /* entry_func */
    NULL,                                              /* exit_func */ 
    hsu_bam_sm__flow_disabled_from_suspend_filter,     /* filter_func */
    {
      {
        HSU_AP_SIDEBAND_SIG,                           /* inc_sig */
        FLOW_DISABLED_FROM_FORCE_SHUTDOWN,             /* dest_state */
      },
      {
        HSU_USB_BUS_SIG,                               /* inc_sig */
        FLOW_ENABLED_STATE,                            /* dest_state */
      },
      {
        HSU_FRAMEWORK_SIG,                             /* inc_sig */
        FLOW_DISABLED_FROM_SUSPEND,                    /* dest_state */
      },
    },
  },
  {
    DATA_ACTIVITY_WHILE_SUSPENDED,                     /* state */
    NULL,                                              /* entry_func */
    NULL,                                              /* exit_func */ 
    NULL,                                              /* filter_func */
    {
      {
        (hsu_bam_sm_sig_enum)(0),                      /* inc_sig */
        (hsu_bam_sm_state_enum)(0),                    /* dest_state */
      },
      {
        (hsu_bam_sm_sig_enum)(0),                      /* inc_sig */
        (hsu_bam_sm_state_enum)(0),                    /* dest_state */
      },
    },
  },
  {
    BOOTUP_FLOW_DISABLED,                              /* state */
    hsu_bam_sm__bootup_flow_disabled,                  /* entry_func */
    NULL,                                              /* exit_func */ 
    hsu_bam_sm__bootup_flow_disabled_filter,           /* filter_func */
    {
      {
        HSU_AP_SIDEBAND_SIG,                           /* inc_sig */
        ENUMERATION_WITH_AP,                           /* dest_state */
      },
      {
        HSU_USB_BUS_SIG,                               /* inc_sig */
        BOOTUP_FLOW_DISABLED,                          /* dest_state */
      },
    },
  },
};

static hsu_bam_sm_ctx_type hsu_no_ecm_bam_sm_ctx;

// =========================
// hsu_no_ecm_bam_sm_init
// =========================
void hsu_no_ecm_bam_sm_init(void)
{
  hsu_bus_mode_multi_core_cb_info_struct cb_info;

  hsu_no_ecm_bam_sm_ctx.curr_state = FLOW_DISABLED_FROM_FORCE_SHUTDOWN;
  hsu_no_ecm_bam_sm_ctx.prev_state = FLOW_DISABLED_FROM_FORCE_SHUTDOWN;

#ifdef HSU_BAM_SM_ENABLE_FORCE_SHUTDOWN_TIMER_CHK
  hsu_no_ecm_bam_sm_ctx.force_shutdown_tmr = hsu_timer_init(
      HSU_TIMER_CID__BAM_SM_FORCE_SHUTDOWN_TMR,
      HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX,
      hsu_bam_sm_force_shutdown_tmr_cb,
      (timer_cb_data_type)NULL);
#endif /* HSU_BAM_SM_ENABLE_FORCE_SHUTDOWN_TIMER_CHK */

  cb_info.core_index = HSU_CORE_IDX_PRIMARY;
  cb_info.bus_mode_cb = hsu_bam_sm_bus_mode_notifier_cb;
  cb_info.app_data = NULL;
  hsu_register_bus_mode_multi_core_notif_cb(cb_info);

  /* To avoid warning of param not used in case FEATURE flag is OFF */
  HSU_USE_PARAM(hsu_bam_sm_state_mach_def);
}

// =========================
// hsu_no_ecm_bam_sm_send_sig_evt_qed
// =========================
static void hsu_no_ecm_bam_sm_send_sig_evt_qed(void *arg)
{
  hsu_bam_sm_transition((hsu_bam_sig_evt_type)(uint32)arg);
}

// =========================
// hsu_no_ecm_bam_sm_send_sig_evt
// =========================
/* Queues signal onto AL task */
/* Client must maintain the signal */
void hsu_no_ecm_bam_sm_send_sig_evt(hsu_bam_sig_evt_type sig_evt)
{
  hsu_al_task_enqueue_cmd_ex(hsu_no_ecm_bam_sm_send_sig_evt_qed, (void*)sig_evt, TRUE);
}

// =========================
// hsu_no_ecm_bam_sm_send_sig_evt_imm
// =========================
/* Immediate execution in current task */
static void hsu_no_ecm_bam_sm_send_sig_evt_imm(hsu_bam_sig_evt_type sig_evt)
{
  hsu_bam_sm_transition(sig_evt);
}

// =========================
// hsu_bam_sm_transition
// =========================
/* function to move the state machine forward by one step */
static void hsu_bam_sm_transition(hsu_bam_sig_evt_type new_sig_evt)
{
  uint32 i = 0;
  hsu_bam_sm_state_mach_def_type *src_state_mach_def;
  hsu_bam_sm_state_mach_def_type *dest_state_mach_def;
  hsu_bam_sm_trans_def_type *trans_def_ptr;
  hsu_bam_sm_tran_ret_enum exit_func_ret = HSU_BAM_SM_HANDLED;

  /* obtain pointer to structure describing the current state */
  src_state_mach_def = &hsu_bam_sm_state_mach_def[hsu_no_ecm_bam_sm_ctx.curr_state];

  /* invoke filter function for intermediate events */
  if (src_state_mach_def->filter_func != NULL)
  {
    if (HSU_BAM_SM_DISCARD_AND_NO_TRAN == src_state_mach_def->filter_func(new_sig_evt))
    {
      /* no transition */
      return;
    }
  }

  //+++ destination state
  /* obtain pointer to all the possible transitions leaving the current state (for finding information on destination state via iteration) */
  trans_def_ptr = &(hsu_bam_sm_state_mach_def[hsu_no_ecm_bam_sm_ctx.curr_state].transition_def[0]);

  /* optimize for memory by using search as opposed to lookup table for information on destination state */
  for (i = 0; i < HSU_BAM_SM_NUM_SIGS; i++)
  {
    if (HSU_BAM_GET_SIG(new_sig_evt) == trans_def_ptr->inc_sig)
    {
      break;
    }
    ++trans_def_ptr;
  }
  if (i == HSU_BAM_SM_NUM_SIGS)
  {
    HSU_ERR_FATAL("Malformed hsu_bam_sm_state_mach_def[]. Check table for invalid signals", 0, 0, 0);
  }

  /* obtain pointer to the structure describing the destination state */
  dest_state_mach_def = &hsu_bam_sm_state_mach_def[trans_def_ptr->dest_state];
  //--- destination state

  HSU_ULOG_ENUM2(NO_MSG, BAM_LOG, "", HSU_BAM_SM__STATE_OFFSET2, src_state_mach_def->state,  HSU_BAM_SM__EVENT_OFFSET2, HSU_BAM_GET_EVT(new_sig_evt));

  /* update state such that the destination views the previous state correctly */
  hsu_no_ecm_bam_sm_ctx.prev_state = hsu_no_ecm_bam_sm_ctx.curr_state;
  hsu_no_ecm_bam_sm_ctx.curr_state = dest_state_mach_def->state;

  /* execute exit function due to departure from source state */
  if (src_state_mach_def->exit_func != NULL)
  {
    exit_func_ret = src_state_mach_def->exit_func(new_sig_evt);
    switch (exit_func_ret)
    {
      case HSU_BAM_SM_SKIP_ENTRY_FUNC_UPON_EXIT:
        HSU_ULOG_1(NO_MSG, BAM_LOG, "skip_next_entry_func_due_to_atomic_jump, entry_func 0x%08X", (uint32)dest_state_mach_def->entry_func);
      break;

      case HSU_BAM_SM_HANDLED:
      // Do nothing
      break;

      default:
        HSU_ERR_FATAL("Exit function failed. func: %d", src_state_mach_def->exit_func, 0, 0);
      break;
    }
  }

  /* execute entry function due to entering the destination state */
  if (dest_state_mach_def->entry_func != NULL)
  {
    if (HSU_BAM_SM_SKIP_ENTRY_FUNC_UPON_EXIT == exit_func_ret)
    {
      // To avoid recursion, use the new (override) destination state updated by the exit function
      //   and make the atomic jump
      dest_state_mach_def = &hsu_bam_sm_state_mach_def[hsu_no_ecm_bam_sm_ctx.curr_state];
      HSU_ULOG_ENUM(NO_MSG, BAM_LOG, "new_dest_state", (uint32)hsu_no_ecm_bam_sm_ctx.curr_state, HSU_BAM_SM__STATE_OFFSET2);
    }
    // else do nothing (regular case)

    if (HSU_BAM_SM_HANDLED != dest_state_mach_def->entry_func(new_sig_evt))
    {
      HSU_ERR_FATAL("Entry function failed. func: %d", dest_state_mach_def->entry_func, 0, 0);
    }
  }
  return;
}

// =========================
// hsu_bam_sm__flow_disabled_from_force_shutdown_filter
// =========================
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__flow_disabled_from_force_shutdown_filter(hsu_bam_sig_evt_type sig_evt)
{
  hsu_bam_sm_tran_ret_enum ret = HSU_BAM_SM_HANDLED;
  switch (HSU_BAM_GET_SIG(sig_evt))
  {
    case HSU_USB_BUS_SIG:
    {
      if (HSU_BUS_CONFIGURED_EVT == HSU_BAM_GET_EVT(sig_evt))
      {
        ret = HSU_BAM_SM_HANDLED;
      }
      else
      {
        ret = HSU_BAM_SM_DISCARD_AND_NO_TRAN;
      }
    }
    break;
    case HSU_AP_SIDEBAND_SIG:
    {
      if (HSU_HOST_READY_FALL_EVT == HSU_BAM_GET_EVT(sig_evt))
      {
        ret = HSU_BAM_SM_DISCARD_AND_NO_TRAN;
      }
    }
    break;
    default:
    {
      ret = HSU_BAM_SM_HANDLED;
    }
    break;
  }

  if (HSU_BAM_SM_DISCARD_AND_NO_TRAN == ret)
  {
    HSU_ULOG_ENUM(NO_MSG, BAM_LOG, "discarded_event", HSU_BAM_GET_EVT(sig_evt), HSU_BAM_SM__EVENT_OFFSET2);
  }
  return ret;
}

// =========================
// hsu_bam_sm__flow_disabled_from_force_shutdown
// =========================
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__flow_disabled_from_force_shutdown(hsu_bam_sig_evt_type sig_evt)
{
  hsu_event_log(EVT_FLOW_DISABLED_FROM_FORCE_SHUTDOWN, HSU_CORE_IDX_PRIMARY);
  switch (HSU_BAM_GET_SIG(sig_evt))
  {
    case HSU_AP_SIDEBAND_SIG:
    {
      switch (HSU_BAM_GET_EVT(sig_evt))
      {
        case HSU_HOST_READY_FALL_EVT:
        {
          switch(hsu_no_ecm_bam_sm_ctx.prev_state)
          {
            case FLOW_ENABLED_STATE:
            {
              //hsu_al_ecm_abort_all_sps_pipe(TRUE, TRUE);
#ifdef FEATURE_HS_USB_BAM
              hsu_al_bam_utils_peer_all_pipe_disable(HSU_AL_BAM_UL, HSU_AL_BAM_DL);
              
              /* For each pipe, BAM pipe disable */
              hsu_al_bam_utils_disable_all_pipe(HSU_AL_BAM_UL, HSU_AL_BAM_DL);
#endif
              /* Invoke device ready fall */
              hsu_bam_sm_set_device_ready(FALSE);
    
              /* Turn off BAM clocks */
              /* Move controller to Low Power Mode */
              hsu_os_bus_core_enter_lpm(hsu_hal_os_bus_core_id(HSU_CORE_IDX_PRIMARY), FALSE, FALSE);
    
              #ifdef HSU_BAM_SM_ENABLE_FORCE_SHUTDOWN_TIMER_CHK
              /* stop timer to double check force shutdown completed within allotted time. */
              hsu_timer_cancel(&hsu_no_ecm_bam_sm_ctx.force_shutdown_tmr);
              #endif /* HSU_BAM_SM_ENABLE_FORCE_SHUTDOWN_TIMER_CHK */
            }
            break;
            case FLOW_DISABLED_FROM_SUSPEND:
            {
              /* Bring USB/HSIC core out of low power mode */
              hsu_os_bus_core_exit_lpm(hsu_hal_os_bus_core_id(HSU_CORE_IDX_PRIMARY));
#ifdef FEATURE_HS_USB_BAM
              /* For each pipe invoke ipa_per_pipe_disable() */
              hsu_al_bam_utils_peer_all_pipe_disable(HSU_AL_BAM_UL, HSU_AL_BAM_DL);
              
              /* For each pipe, BAM pipe disable */
              hsu_al_bam_utils_disable_all_pipe(HSU_AL_BAM_UL, HSU_AL_BAM_DL);
#endif
              /* Invoke device ready fall */
              hsu_bam_sm_set_device_ready(FALSE);

              /* Turn off BAM clocks */
              /* Move controller to Low Power Mode */
              hsu_os_bus_core_enter_lpm(hsu_hal_os_bus_core_id(HSU_CORE_IDX_PRIMARY), FALSE, FALSE);

              #ifdef HSU_BAM_SM_ENABLE_FORCE_SHUTDOWN_TIMER_CHK
              /* stop timer to double check force shutdown completed within allotted time. */
              hsu_timer_cancel(&hsu_no_ecm_bam_sm_ctx.force_shutdown_tmr);
              #endif /* HSU_BAM_SM_ENABLE_FORCE_SHUTDOWN_TIMER_CHK */
            }
            break;
            case ENUMERATION_WITH_AP:
            {             
              /* Turn off BAM clocks */
              /* Move controller to Low Power Mode */
              hsu_os_bus_core_enter_lpm(hsu_hal_os_bus_core_id(HSU_CORE_IDX_PRIMARY), FALSE, FALSE);

              /* Invoke device ready fall */
              hsu_bam_sm_set_device_ready(FALSE);

              #ifdef HSU_BAM_SM_ENABLE_FORCE_SHUTDOWN_TIMER_CHK
              /* stop timer to double check force shutdown completed within allotted time. */
              hsu_timer_cancel(&hsu_no_ecm_bam_sm_ctx.force_shutdown_tmr);
              #endif /* HSU_BAM_SM_ENABLE_FORCE_SHUTDOWN_TIMER_CHK */
            }
            break;
            default:
            {
              HSU_BAM_SM_INVALID_STATE_EVT_ERR_FATAL();
            }
            break;
          }
        }
        break;
        
        default:
        {
          HSU_BAM_SM_INVALID_STATE_EVT_ERR_FATAL();
        }
        break;
      }
    }
    break;

    case HSU_USB_BUS_SIG:
    {
      switch (HSU_BAM_GET_EVT(sig_evt))
      {
        case HSU_BUS_RESET_EVT:
        {
          // Handled
        }
        break;

        default:
        {
          HSU_BAM_SM_INVALID_STATE_EVT_ERR_FATAL();
        }
        break;
      }
    }
    break;
    
    default:
    {
      HSU_BAM_SM_INVALID_STATE_EVT_ERR_FATAL();
    }
    break;
  }
  return HSU_BAM_SM_HANDLED;
}

// =========================
// hsu_bam_sm__flow_disabled_from_force_shutdown__exit
// =========================
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__flow_disabled_from_force_shutdown__exit(hsu_bam_sig_evt_type sig_evt)
{
  // +++ Override jump if it deviates from the standard table defined transition
  hsu_bam_sm_tran_ret_enum exit_func_ret = HSU_BAM_SM_EXCEPTION;
  if (HSU_BUS_CONFIGURED_EVT == HSU_BAM_GET_EVT(sig_evt))
  {
    exit_func_ret = HSU_BAM_SM_SKIP_ENTRY_FUNC_UPON_EXIT;
    // Specify the new state for the atomic jump
    hsu_no_ecm_bam_sm_ctx.curr_state = FLOW_ENABLED_STATE;
  }
  else
  {
    exit_func_ret = HSU_BAM_SM_HANDLED;
  }
  // --- Override jump if it deviates from the standard table defined transition
  return exit_func_ret;
}

// =========================
// hsu_bam_sm__enumeration_with_ap_filter
// =========================
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__enumeration_with_ap_filter(hsu_bam_sig_evt_type sig_evt)
{
  hsu_bam_sm_tran_ret_enum ret = HSU_BAM_SM_HANDLED;
  switch (HSU_BAM_GET_SIG(sig_evt))
  {
    case HSU_USB_BUS_SIG:
    {
      switch (HSU_BAM_GET_EVT(sig_evt))
      {
        case HSU_BUS_CONFIGURED_EVT:
        {
          ret = HSU_BAM_SM_HANDLED;
        }
        break;
        default:
        {
          ret = HSU_BAM_SM_DISCARD_AND_NO_TRAN;
        }
        break;
      }
    }
    break;
    default:
    {
      ret = HSU_BAM_SM_HANDLED;
    }
    break;
  }

  if (HSU_BAM_SM_DISCARD_AND_NO_TRAN == ret)
  {
    HSU_ULOG_ENUM(NO_MSG, BAM_LOG, "discarded_event", HSU_BAM_GET_EVT(sig_evt), HSU_BAM_SM__EVENT_OFFSET2);
  }
  return ret;
}

// =========================
// hsu_bam_sm__enumeration_with_ap
// =========================
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__enumeration_with_ap(hsu_bam_sig_evt_type sig_evt)
{
  hsu_event_log(EVT_ENUMERATION_WITH_AP, HSU_CORE_IDX_PRIMARY);
  switch (HSU_BAM_GET_SIG(sig_evt))
  {
    case HSU_AP_SIDEBAND_SIG:
    {
      switch (HSU_BAM_GET_EVT(sig_evt))
      {
        case HSU_HOST_READY_RISE_EVT:
        {
          if (BOOTUP_FLOW_DISABLED == hsu_no_ecm_bam_sm_ctx.prev_state)
          {
            //__Turn on all clock
            //__Bring USB/HSIC out of Low Power Mode
            //__Start HSIC
            //__Device Ready Rise

            /* Turn on all clock */
            /* Bring USB/HSIC out of Low Power Mode */
            hsu_os_bus_core_exit_lpm(hsu_hal_os_bus_core_id(HSU_CORE_IDX_PRIMARY));

            /* Device Ready Rise */
            hsu_bam_sm_set_device_ready(TRUE);
          }
          else
          {
            //__Turn on all clock
            //__Bring USB/HSIC out of Low Power Mode
            //__USB/HSIC restart core
            //__Device Ready Rise

#ifdef HSU_BAM_SM_ENABLE_FORCE_SHUTDOWN_TIMER_CHK
            /* stop timer to double check force shutdown completed within allotted time. */
            hsu_timer_cancel(&hsu_no_ecm_bam_sm_ctx.force_shutdown_tmr);
#endif /* HSU_BAM_SM_ENABLE_FORCE_SHUTDOWN_TIMER_CHK */

            /* Turn on all clock */
            /* Bring USB/HSIC out of Low Power Mode */
            hsu_os_bus_core_exit_lpm(hsu_hal_os_bus_core_id(HSU_CORE_IDX_PRIMARY));

#if 0 //Commenting this out to remove a glitch on D+ during USB connect
            // Disallow enumeration before restart core occurs.
            core_disallow_enumeration(HSU_CORE_IDX_PRIMARY);

            /* Restart core */
            hsu_conf_sel_stack_utils_device_restart_core((void*)hsu_hal_os_bus_core_id(HSU_CORE_IDX_PRIMARY));

            // Allow enumeration after restart core is completed.
            core_allow_enumeration(HSU_CORE_IDX_PRIMARY);
#endif

            /* Device Ready Rise */
            hsu_bam_sm_set_device_ready(TRUE);
          }
        }
        break;
        default:
        {
          HSU_BAM_SM_INVALID_STATE_EVT_ERR_FATAL();
        }
        break;
      }
    }
    break;
    default:
    {
      HSU_BAM_SM_INVALID_STATE_EVT_ERR_FATAL();
    }
    break;
  }
  return HSU_BAM_SM_HANDLED;
}

// =========================
// hsu_bam_sm__standard_seq_to_flow_enable
// =========================
static void hsu_bam_sm__standard_seq_to_flow_enable(void)
{
#ifdef FEATURE_HS_USB_BAM
  hsu_al_bam_utils_peer_all_pipe_enable(HSU_AL_BAM_UL, HSU_AL_BAM_DL);
#endif
  return;
}

// =========================
// hsu_bam_sm__flow_enabled_filter
// =========================
/* Discards new intermediate events that are not relevant at this current state */
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__flow_enabled_filter(hsu_bam_sig_evt_type sig_evt)
{
  hsu_bam_sm_tran_ret_enum ret = HSU_BAM_SM_HANDLED;

  switch(HSU_BAM_GET_SIG(sig_evt))
  {
    case HSU_USB_BUS_SIG:
    {
      switch (HSU_BAM_GET_EVT(sig_evt))
      {
        case HSU_BUS_SUSPEND_EVT:
        case HSU_BUS_RESET_EVT:
        {
          ret = HSU_BAM_SM_HANDLED;
        }
        break;
        default:
        {
          ret = HSU_BAM_SM_DISCARD_AND_NO_TRAN;
        }
        break;
      }
    }
    break;
    default:
    {
      ret = HSU_BAM_SM_HANDLED;
    }
    break;
  }

  if (HSU_BAM_SM_DISCARD_AND_NO_TRAN == ret)
  {
    HSU_ULOG_ENUM(NO_MSG, BAM_LOG, "discarded_event", HSU_BAM_GET_EVT(sig_evt), HSU_BAM_SM__EVENT_OFFSET2);
  }
  return ret;
}

// =========================
// hsu_bam_sm__flow_enabled
// =========================
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__flow_enabled(hsu_bam_sig_evt_type sig_evt)
{
  hsu_event_log(EVT_FLOW_ENABLED_STATE, HSU_CORE_IDX_PRIMARY);
  switch (HSU_BAM_GET_SIG(sig_evt))
  {
    case HSU_USB_BUS_SIG:
    {
      switch (HSU_BAM_GET_EVT(sig_evt))
      {
        case HSU_BUS_CONFIGURED_EVT:
        {
          hsu_os_bus_core_exit_lpm(hsu_hal_os_bus_core_id(HSU_CORE_IDX_PRIMARY));
#ifdef FEATURE_HS_USB_BAM
          if ((ENUMERATION_WITH_AP == hsu_no_ecm_bam_sm_ctx.prev_state))
          {
            if (hsu_no_ecm_bam_sm_ctx.is_configured_once && (FALSE == hsu_al_ecm_is_bam_flow_enabled()))
            {
              // Initialization must have been previously done in hsu_no_ecm_bam_sm_hw_initial_configuration();
              //  thus, skipping the initializations below

              // Note the similarities between HSU_BUS_CONFIGURED_EVT, HSU_BUS_RESUME_EVT sections. 
              // When one is changed, evaluate the other cases as well.

              /* Device enumerated at this point */
              /* For each pipe invoke ipa_per_pipe_disable() */
              hsu_al_bam_utils_peer_all_pipe_disable(HSU_AL_BAM_UL, HSU_AL_BAM_DL);

              /* BAM Reset */
              hsu_al_bam_reset();

              hsu_al_bam_utils_enable_all_pipes(HSU_AL_BAM_UL, HSU_AL_BAM_DL);

              /* Non HW accelerated EP setup */
              hsu_bam_sm__standard_seq_to_flow_enable();

              // Enable QDSS EP
              hsu_al_bam_enable_read_write_infinite_qdss_bam_ep();
            }
            // else the initialization for the very first time 
            //   should be done in hsu_al_ecm.c via hsu_no_ecm_bam_sm_hw_initial_configuration()
          }
          else if (FLOW_DISABLED_FROM_SUSPEND == hsu_no_ecm_bam_sm_ctx.prev_state)
          {
            hsu_bam_sm__standard_seq_to_flow_enable();
            // USB BAM RESET not required here
            hsu_al_bam_utils_enable_all_pipes(HSU_AL_BAM_UL, HSU_AL_BAM_DL);

            // Enable QDSS EP
            hsu_al_bam_enable_read_write_infinite_qdss_bam_ep();
          }
          else if ((FLOW_DISABLED_FROM_FORCE_SHUTDOWN == hsu_no_ecm_bam_sm_ctx.prev_state))
          {
            // For this case, it must have came from a bus reset
            hsu_al_bam_utils_peer_all_pipe_disable(HSU_AL_BAM_UL, HSU_AL_BAM_DL);

            /* BAM Reset */
            hsu_al_bam_reset();

            hsu_al_bam_utils_enable_all_pipes(HSU_AL_BAM_UL, HSU_AL_BAM_DL);

            /* Non HW accelerated EP setup */
            hsu_bam_sm__standard_seq_to_flow_enable();

            // Enable QDSS EP
            hsu_al_bam_enable_read_write_infinite_qdss_bam_ep();
          }
#endif
        }
        break;
        case HSU_BUS_RESUME_EVT:
        {
          hsu_os_bus_core_exit_lpm(hsu_hal_os_bus_core_id(HSU_CORE_IDX_PRIMARY));

          // Note the similarities between HSU_BUS_CONFIGURED_EVT, HSU_BUS_RESUME_EVT sections. 
          // When one is changed, evaluate the other cases as well.
#ifdef FEATURE_HS_USB_BAM
          if (FLOW_DISABLED_FROM_SUSPEND == hsu_no_ecm_bam_sm_ctx.prev_state)
          {
            // Enable QDSS EP
            hsu_al_bam_enable_read_write_infinite_qdss_bam_ep();
          }
#endif
        }
        break;
        case HSU_BUS_SUSPEND_EVT:
        {
          /* Discard */
//          /* Simulate a bus suspend signal to the state machine */
//          hsu_no_ecm_bam_sm_send_sig_evt_imm(HSU_BAM_SET_SIG_EVT(HSU_USB_BUS_SIG, HSU_BUS_SUSPEND_EVT));
        }
        break;
        default:
        {
          HSU_BAM_SM_INVALID_STATE_EVT_ERR_FATAL();
        }
        break;
      }
    }
    break;
    default:
    {
      HSU_BAM_SM_INVALID_STATE_EVT_ERR_FATAL();
    }
    break;
  }
  return HSU_BAM_SM_HANDLED;
}

// =========================
// hsu_bam_sm__flow_enabled__exit
// =========================
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__flow_enabled__exit(hsu_bam_sig_evt_type sig_evt)
{
  // +++ Override jump if it deviates from the standard table defined transition
  hsu_bam_sm_tran_ret_enum exit_func_ret = HSU_BAM_SM_EXCEPTION;
  if (HSU_BUS_RESET_EVT == HSU_BAM_GET_EVT(sig_evt))
  {
    exit_func_ret = HSU_BAM_SM_SKIP_ENTRY_FUNC_UPON_EXIT;
    // Specify the new state for the atomic jump
    hsu_no_ecm_bam_sm_ctx.curr_state = FLOW_DISABLED_FROM_FORCE_SHUTDOWN;
  }
  else
  {
    exit_func_ret = HSU_BAM_SM_HANDLED;
  }
  // --- Override jump if it deviates from the standard table defined transition
  return exit_func_ret;
}

// =========================
// hsu_bam_sm__flow_disabled_from_suspend_filter
// =========================
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__flow_disabled_from_suspend_filter(hsu_bam_sig_evt_type sig_evt)
{
  hsu_bam_sm_tran_ret_enum ret = HSU_BAM_SM_HANDLED;
  switch (HSU_BAM_GET_SIG(sig_evt))
  {
    case HSU_USB_BUS_SIG:
    {
      switch (HSU_BAM_GET_EVT(sig_evt))
      {
        case HSU_BUS_RESUME_EVT:
        case HSU_BUS_CONFIGURED_EVT:
        {
          ret = HSU_BAM_SM_HANDLED;
        }
        break;
        default:
        {
          ret = HSU_BAM_SM_DISCARD_AND_NO_TRAN;
        }
        break;
      }
    }
    break;
    default:
    {
      ret = HSU_BAM_SM_HANDLED;
    }
    break;
  }

  if (HSU_BAM_SM_DISCARD_AND_NO_TRAN == ret)
  {
    HSU_ULOG_ENUM(NO_MSG, BAM_LOG, "discarded_event", HSU_BAM_GET_EVT(sig_evt), HSU_BAM_SM__EVENT_OFFSET2);
  }
  return ret;
}

// =========================
// hsu_bam_sm__flow_disabled_from_suspend
// =========================
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__flow_disabled_from_suspend(hsu_bam_sig_evt_type sig_evt)
{
  hsu_event_log(EVT_FLOW_DISABLED_FROM_SUSPEND, HSU_CORE_IDX_PRIMARY);
  switch (HSU_BAM_GET_SIG(sig_evt))
  {
    case HSU_USB_BUS_SIG:
    {
      switch (HSU_BAM_GET_EVT(sig_evt))
      {
        case HSU_BUS_SUSPEND_EVT:
        {
          //__If no data in DL pipe [no DL] &&
          //  If in for SUSPEND state (will be since operation atomic) &&
          //  If no data activity CB occurred
          //  __Turn off USB/HSIC BAM clks
          //  __Vote for IPA clk gating
          //
          //__Else If found data in DL pipe [DL Active 2]
          //  __request remote wakeup (return execution)

//          /* If found data in DL pipe require remote wakeup */
//          if (!hsu_al_bam_utils_are_DL_pipes_empty())
//          {
//            hsu_al_task_signal_remote_wakeup();
//          }
//          else
//          {
            /* Disable UL and DL EPs*/
            //hsu_al_ecm_abort_all_sps_pipe(TRUE, TRUE);

            hsu_os_delay_us(100);

            hsu_os_bus_schedule_lpm_entry(hsu_hal_os_bus_core_id(HSU_CORE_IDX_PRIMARY));

            // else wakeup is handled immediately as state machine handles the bus resume
//          }
        }
        break;
        default:
        {
          HSU_BAM_SM_INVALID_STATE_EVT_ERR_FATAL();
        }
        break;
      }
    }
    break;
    default:
    {
      HSU_BAM_SM_INVALID_STATE_EVT_ERR_FATAL();
    }
    break;
  }
  return HSU_BAM_SM_HANDLED;
}

// =========================
// hsu_bam_sm__bootup_flow_disabled_filter
// =========================
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__bootup_flow_disabled_filter(hsu_bam_sig_evt_type sig_evt)
{
  hsu_bam_sm_tran_ret_enum ret = HSU_BAM_SM_HANDLED;
  switch (HSU_BAM_GET_SIG(sig_evt))
  {
    case HSU_USB_BUS_SIG:
    {
      ret = HSU_BAM_SM_DISCARD_AND_NO_TRAN;
    }
    break;
    case HSU_AP_SIDEBAND_SIG:
    {
      ret = HSU_BAM_SM_HANDLED;
    }
    break;
    default:
    {
      ret = HSU_BAM_SM_HANDLED;
    }
    break;
  }

  if (HSU_BAM_SM_DISCARD_AND_NO_TRAN == ret)
  {
    HSU_ULOG_ENUM(NO_MSG, BAM_LOG, "discarded_event", HSU_BAM_GET_EVT(sig_evt), HSU_BAM_SM__EVENT_OFFSET2);
  }

  return ret;
}

// =========================
// hsu_bam_sm__bootup_flow_disabled
// =========================
static hsu_bam_sm_tran_ret_enum hsu_bam_sm__bootup_flow_disabled(hsu_bam_sig_evt_type sig_evt)
{
  hsu_event_log(EVT_BOOTUP_FLOW_DISABLED, HSU_CORE_IDX_PRIMARY);
  switch (HSU_BAM_GET_SIG(sig_evt))
  {
    case HSU_AP_SIDEBAND_SIG:
    {
      switch (HSU_BAM_GET_EVT(sig_evt))
      {
        case HSU_HOST_READY_RISE_EVT:
        {
          // do nothing
        }
        break;
        default:
        {
          HSU_BAM_SM_INVALID_STATE_EVT_ERR_FATAL();
        }
        break;
      }
    }
    break;
    default:
    {
      HSU_BAM_SM_INVALID_STATE_EVT_ERR_FATAL();
    }
    break;
  }

  return HSU_BAM_SM_HANDLED;
}

#ifdef HSU_BAM_SM_ENABLE_FORCE_SHUTDOWN_TIMER_CHK
// =========================
// hsu_bam_sm_force_shutdown_tmr_cb
// =========================
static void hsu_bam_sm_force_shutdown_tmr_cb(timer_cb_data_type data)
{
  HSU_ERR_FATAL("Force shutdown procedure exceeding %d ms", HSU_BAM_SM_FORCE_SHUTDOWN_MAX_TIME, 0, 0);
}
#endif /* HSU_BAM_SM_ENABLE_FORCE_SHUTDOWN_TIMER_CHK */

// =========================
// hsu_bam_sm_bus_mode_notifier_cb_qed
// =========================
static void hsu_no_ecm_bam_sm_bus_mode_notifier_cb_qed(void* mode)
{
  hsu_bus_mode_enum new_mode = (hsu_bus_mode_enum) (0 + (uint32)mode); /* extra cast to resolve warnings */

  switch (new_mode)
  {
    case HSU_BUS_MODE_SUSPENDED:
    {
      hsu_no_ecm_bam_sm_send_sig_evt_imm(HSU_BAM_SET_SIG_EVT(HSU_USB_BUS_SIG, HSU_BUS_SUSPEND_EVT));
    }
    break;
    case HSU_BUS_MODE_RESUMED:
    {
      hsu_no_ecm_bam_sm_send_sig_evt_imm(HSU_BAM_SET_SIG_EVT(HSU_USB_BUS_SIG, HSU_BUS_RESUME_EVT));
    }
    break;
    case HSU_BUS_MODE_CONFIGURED:
    {
      hsu_no_ecm_bam_sm_send_sig_evt_imm(HSU_BAM_SET_SIG_EVT(HSU_USB_BUS_SIG, HSU_BUS_CONFIGURED_EVT));
    }
    break;
    case HSU_BUS_MODE_DISCONNECTED:
    {
      hsu_no_ecm_bam_sm_send_sig_evt_imm(HSU_BAM_SET_SIG_EVT(HSU_AP_SIDEBAND_SIG, HSU_HOST_READY_FALL_EVT));
    }
    break;
    case HSU_BUS_MODE_RESET:
    {
      // Special handling to block all bus reset handling until configuration is done once
      if (TRUE == hsu_no_ecm_bam_sm_ctx.is_configured_once)
      {
        hsu_no_ecm_bam_sm_send_sig_evt_imm(HSU_BAM_SET_SIG_EVT(HSU_USB_BUS_SIG, HSU_BUS_RESET_EVT));
      }
    }
    break;
    case HSU_BUS_MODE_CONNECTED:
    {
      hsu_no_ecm_bam_sm_send_sig_evt_imm(HSU_BAM_SET_SIG_EVT(HSU_AP_SIDEBAND_SIG, HSU_HOST_READY_RISE_EVT));
    }
    break;
    //case HSU_BUS_MODE_RESUME_BEGIN:
    case HSU_BUS_MODE_UNCONFIGURED:
    {
      /* Discard */
    }
    break;
    default:
    {
      /* Discard */
    }
    break;
  }
}

// =========================
// hsu_bam_sm_bus_mode_notifier_cb
// =========================
static void hsu_bam_sm_bus_mode_notifier_cb(hsu_bus_mode_enum new_mode, void* app_data)
{
  /* do not need to forward app data */
  hsu_al_task_enqueue_cmd_ex(hsu_no_ecm_bam_sm_bus_mode_notifier_cb_qed, (void*)new_mode, TRUE);
}

// =========================
// hsu_no_ecm_bam_sm_register_cbs
// =========================
/* External API */
void hsu_no_ecm_bam_sm_register_cbs(
    hsu_bam_sm_device_ready_cb device_ready_cb,
    hsu_bam_sm_wakeup_host_cb wakeup_host_cb)
{
  hsu_no_ecm_bam_sm_ctx.ext_cbs.device_ready_cb = device_ready_cb;
  hsu_no_ecm_bam_sm_ctx.ext_cbs.wakeup_host_cb = wakeup_host_cb;
} 

static void hsu_bam_sm_set_device_ready(boolean setting)
{
  if (hsu_no_ecm_bam_sm_ctx.ext_cbs.device_ready_cb)
  {
    hsu_no_ecm_bam_sm_ctx.ext_cbs.device_ready_cb(setting);

    if(setting)
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "hi");
    }
    else
    {
      HSU_ULOG(NO_MSG, BAM_LOG, "lo");
    }
  }
}

// =========================
// hsu_no_ecm_bam_sm_notify_host_ready
// =========================
/* External API */
void hsu_no_ecm_bam_sm_notify_host_ready(boolean ready)
{
  // NOTE: Do not insert disallow enumeration in this function.
  // It will affect subsequent cable connects.  It is better to only place only at beginning of certain
  //   state transitions.
  if(TRUE == ready)
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "bam_sm_start_event_handler");

    // Allow enumeration whenever HOST READY CB occurs.
    core_allow_enumeration(HSU_CORE_IDX_PRIMARY);
    hsu_no_ecm_bam_sm_send_sig_evt(HSU_BAM_SET_SIG_EVT(HSU_AP_SIDEBAND_SIG, HSU_HOST_READY_RISE_EVT));
  }
  else
  {
    hsu_no_ecm_bam_sm_send_sig_evt(HSU_BAM_SET_SIG_EVT(HSU_AP_SIDEBAND_SIG, HSU_HOST_READY_FALL_EVT));
  }
}

// =========================
// hsu_no_ecm_bam_sm_hw_initial_configuration
// =========================
void hsu_no_ecm_bam_sm_hw_initial_configuration(void)
{
  if (FALSE == hsu_no_ecm_bam_sm_ctx.is_configured_once)
  {
 
    //Don't use the reset and enable version since pipes are not configured yet
    //This is absolutely required since BAM Init APIs doesn't initialize to zero properly for the peripheral
    //For QDSS purposes without ECM, this is moved to AL OBEX
    //hsu_al_bam_reset();
    
    hsu_no_ecm_bam_sm_ctx.is_configured_once = TRUE;
  
    #ifdef FEATURE_HS_USB_LOG_UPTO_FIRST_ENUMERATION
    hsu_stop_reg_log();
    hsu_stop_func_log();
    #endif // FEATURE_HS_USB_LOG_UPTO_FIRST_ENUMERATION
  }
}

// =========================
// hsu_no_ecm_bam_sm_is_rx_forward_to_app_allowed
// =========================
boolean hsu_no_ecm_bam_sm_is_rx_forward_to_app_allowed(void)
{
  return TRUE;
}

/* DOCUMENTATION: */
/* List of state machines filters and exceptions */
/* Exception 001: Have to ensure orthogonality of real HOST READY GPIO with USB cable disconnects. */
