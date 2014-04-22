/** 
  @file mhi_sm.c
  @brief
  This file implements the mhi state machine. 
    

*/
/*
===============================================================================
 
                             Edit History
 
 $Header: 

Change revision history maintained in version system
===============================================================================
                   Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "mhi_i.h"
#include "mhi.h"
#include "mhi_sm.h"
#include "mhi_hwacc.h"
#include "mhi_core.h"

/* Variables used by the MHI SM during run time */
struct _mhi_sm_runtime
{
   mhi_state_t mhi_oldState;                /**< Old MHI state */ 
   mhi_state_t mhi_newState;                /**< New MHI state */ 
   mhi_link_d_state_t d_oldState;           /**< Old Device state */
   mhi_link_d_state_t d_newState;           /**< New Device state */ 
   boolean mhi_core_requested_wakeup;       /**< MHI core requested wakeup */ 
   boolean mhi_hw_acc_requested_wakeup;     /**< MHI hw acc requested wakeup */ 
   boolean link_inact_intr_received;        /**< Link inactivity interrupt was received */ 
   boolean device_wake_oldState;            /**< Old state of device wake */
   boolean device_wake_newState;            /**< New state of device wake */ 
   boolean wakeup_received_from_host;       /**< Wakeup received from host */
   boolean link_act_event_received;         /**< link activity detected */ 
};

/* State machine run time */
static mhi_sm_runtime_t sm_rtime;

/* Static function prototypes */
#ifndef MHI_BOOT 
static uint32 mhi_sm_wakeup_from_M2(mhi_sm_runtime_t *sm_Rtime);
static uint32 mhi_sm_wakeup_from_M3(mhi_sm_runtime_t *sm_Rtime);
static void mhi_sm_process_sm_trigs_link_disabled(mhi_sm_runtime_t sm_Rtime);
#endif /* #ifndef MHI_BOOT  */

/*Initialize debug counters */
static void mhi_sm_init_debug_counters(void)
{
#ifndef MHI_BOOT
   memset(&mhiCtx.mhi_sm_info.mhi_sm_debug_counters,0,sizeof(mhi_sm_debug_counters_t));
#endif /* #ifndef MHI_BOOT */   
}

/**
 * @brief       mhi_sm_init
 *
 *              Initialize the MHI state machine.
 *              
 * @param[in]   *mhi_sm_info - pointer to the MHI state machine context.
 *
 * @dependencies
 *              mhi_pm_init 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_sm_init(mhi_sm_info_t *sm_info)
{
   mhi_osal_debug(MSG_HIGH,"mhi_sm_init: Enter");

   mhiCtx.mhi_sm_info.mhiState=MHI_STATE_RESET;

   mhiCtx.mhi_sm_info.wakeup_pending = FALSE;
   mhiCtx.mhi_sm_info.mhi_sm_trigs_pending = 0;
   mhiCtx.mhi_sm_info.sm_rtime_info = &sm_rtime;
   mhiCtx.mhi_sm_info.device_wake = mhi_pm_get_device_wake_gpio_state();
   mhiCtx.mhi_sm_info.link_activity_detected_in_M2=FALSE;
   mhiCtx.mhi_sm_info.d_state = mhi_pm_get_d_state();

   mhi_sm_init_debug_counters();

   /* Initialize the M2 exit timer */
   mhi_osal_timer_init(&mhiCtx.mhi_sm_info.M2_wakeup_timer);

   /* If current state is RESET set to READY otherwise leave the state as RESET, 
    * once mhi_run_state_machine executes it will get set to correct state */
   if(MHI_STATE_RESET == mhi_mmio_get_mhistate())
   {
      /* Set MHI state to ready, so host can start initializing MMIO */ 
      mhi_sm_set_mhiState(MHI_STATE_READY);
   }
   else
   {
      mhi_sm_set_trig_pending(MHI_SM_TRIG_MANUAL_SHIFT, FALSE);
   }
}

/**
 * @brief       mhi_sm_reset
 *
 *              Reset the MHI state machine. 
 *
 * @dependencies
 *              mhi_pm_reset 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */ 
void mhi_sm_reset(void)
{
   /* Reset MHI state machine */
   mhi_sm_set_mhiState(MHI_STATE_RESET);
   mhiCtx.mhi_sm_info.wakeup_pending = FALSE;
   mhiCtx.mhi_sm_info.mhi_sm_trigs_pending = 0;
   mhiCtx.mhi_sm_info.device_wake = mhi_pm_get_device_wake_gpio_state();
   mhiCtx.mhi_sm_info.link_activity_detected_in_M2=FALSE;
   mhiCtx.mhi_sm_info.d_state = mhi_pm_get_d_state();

   mhi_sm_init_debug_counters();
}

/**
 * @brief       mhi_sm_send_state_change_event
 *
 *              Send MHI state change event to the host
 *
 *
 * @param[in]   state - MHI state to set
 *
 * @dependencies
 *              None
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_sm_send_state_change_event(mhi_state_t state)
{
  mhi_ring_element_t event;

  event.stateChgEvent.mhistate = state; 
  event.stateChgEvent.type=MHI_RING_EL_TYPE_MHI_STATE_CHG;
  if (SUCCESS != mhi_core_send_event(PRIMARY_ER,&event,1))
  {
    mhi_core_sys_error();
  }
}

/**
 * @brief       mhi_sm_set_mhiState
 *
 *              Set new mhi state in state machine context and update MHISTATUS reg.
 *              Must be called directly or indirectly from mhi_sm_run_state_machine().
 *              mhi_sm_init() being the only exception.
 *
 *
 * @param[in]   state - MHI state to set
 *
 * @dependencies
 *              None
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */ 
void mhi_sm_set_mhiState(mhi_state_t state)
{
   mhiCtx.mhi_sm_info.mhiState=state; 
   mhi_mmio_set_mhistate(state);
}

/**
 * @brief       mhi_sm_run_state_machine
 *
 *              Initialize the state machine runtime variables. 
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
static __inline void mhi_sm_init_runtime_variables(mhi_sm_runtime_t *sm_Rtime)
{
   sm_Rtime->device_wake_oldState = mhiCtx.mhi_sm_info.device_wake;
   sm_Rtime->device_wake_newState = sm_Rtime->device_wake_oldState;
   sm_Rtime->mhi_oldState = mhiCtx.mhi_sm_info.mhiState;
   sm_Rtime->mhi_newState = sm_Rtime->mhi_oldState;
   sm_Rtime->d_oldState = mhiCtx.mhi_sm_info.d_state;
   sm_Rtime->d_newState = sm_Rtime->d_oldState;
   sm_Rtime->mhi_core_requested_wakeup = FALSE;
   sm_Rtime->mhi_hw_acc_requested_wakeup = FALSE;
   sm_Rtime->link_inact_intr_received = FALSE;
   sm_Rtime->wakeup_received_from_host = FALSE;
   sm_Rtime->link_act_event_received = FALSE;
}

/**
 * @brief       mhi_sm_set_runtime_variables
 *
 *              Sets the appropriate sm runtime variable which prompts action
 *              from the state machine. Also clears notifications which are pending. 
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
static uint32 mhi_sm_set_runtime_variables(mhi_sm_runtime_t *sm_Rtime)
{
   mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: MHI SM trigs pending bitmask: %x",
                                            mhiCtx.mhi_sm_info.mhi_sm_trigs_pending);

   /* Manual trigger */
   if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_MANUAL_SHIFT))
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_MANUAL_SHIFT);
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received sm_trig:%d, \
        manual state machine trigger in M State = %d",MHI_SM_TRIG_MANUAL_SHIFT,sm_Rtime->mhi_oldState);
   }
   /* Check for M state change */
   else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_CTRL_INTR_SHIFT))
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_CTRL_INTR_SHIFT);
      sm_Rtime->mhi_newState=mhi_mmio_get_mhistate();
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received sm_trig:%d, \
        M state change event. New M State = %d",MHI_SM_TRIG_CTRL_INTR_SHIFT,sm_Rtime->mhi_newState);
   }
#ifndef MHI_BOOT   
   /* Check for d state change */
   else if(mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_D_STATE_CHANGE_SHIFT))
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_D_STATE_CHANGE_SHIFT);
      sm_Rtime->d_newState = mhi_pm_get_d_state();
      mhiCtx.mhi_sm_info.d_state = sm_Rtime->d_newState;
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received sm_trig:%d, \
        D state change event. New D State = %d",MHI_SM_TRIG_D_STATE_CHANGE_SHIFT,sm_Rtime->d_newState);
   }   
  /* Check if mhi core originated a wakeup request */
  else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_CORE_WAKEUP_SHIFT)) 
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_CORE_WAKEUP_SHIFT);
      sm_Rtime->mhi_core_requested_wakeup = TRUE;
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received sm_trig:%d, \
        wake up request from MHI CORE",MHI_SM_TRIG_CORE_WAKEUP_SHIFT);
      mhiCtx.mhi_sm_info.mhi_sm_debug_counters.core_wakeup[mhiCtx.mhi_sm_info.mhiState]++;
   }
   /* Check if hardware accelerator originated wakeup request */
   else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_HW_ACC_WAKEUP_SHIFT)) 
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_HW_ACC_WAKEUP_SHIFT);
      sm_Rtime->mhi_hw_acc_requested_wakeup = TRUE;
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received sm_trig:%d, \
        wake up request from HW Acc",MHI_SM_TRIG_HW_ACC_WAKEUP_SHIFT);
      mhiCtx.mhi_sm_info.mhi_sm_debug_counters.hw_acc_wakeup[mhiCtx.mhi_sm_info.mhiState]++;
   }
   /* Check if the state of device wake changed */
   else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_DEVICE_WAKE_SHIFT))
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_DEVICE_WAKE_SHIFT);
      sm_Rtime->device_wake_newState = mhi_pm_get_device_wake_gpio_state();
      mhiCtx.mhi_sm_info.device_wake = sm_Rtime->device_wake_newState;
      mhiCtx.mhi_sm_info.mhi_sm_debug_counters.device_wake_toggles++;
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received sm_trig:%d, \
      device_wake state change",MHI_SM_TRIG_DEVICE_WAKE_SHIFT);
   }
   /* Check if the link inactivity interrupt fired */
   else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_LINK_INACT_TIMER_SHIFT))
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_LINK_INACT_TIMER_SHIFT);
      sm_Rtime->link_inact_intr_received = TRUE;
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received sm_trig:%d, \
        link inactivity interrupt",MHI_SM_TRIG_LINK_INACT_TIMER_SHIFT);
   }
   /* Host sent wakeup request */
   else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_HOST_WAKEUP_RECEIVED_SHIFT))
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_HOST_WAKEUP_RECEIVED_SHIFT);
      sm_Rtime->wakeup_received_from_host = TRUE;
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received sm_trig:%d, \
      wakeup from host PERST De-asserted",MHI_SM_TRIG_HOST_WAKEUP_RECEIVED_SHIFT);
   }
   else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_LINK_UP_SHIFT))
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_LINK_UP_SHIFT);
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables:Received sm_trig:%d, \
      link up event",MHI_SM_TRIG_LINK_UP_SHIFT);
   } 
   else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_LINK_DOWN_SHIFT)) 
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_LINK_DOWN_SHIFT);
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received sm_trig:%d, \
      link down event",MHI_SM_TRIG_LINK_DOWN_SHIFT);
   } 
   else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_LINK_ACTIVITY_SHIFT)) 
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_LINK_ACTIVITY_SHIFT);
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received sm_trig:%d, \
        link activity event",MHI_SM_TRIG_LINK_ACTIVITY_SHIFT);
      sm_Rtime->link_act_event_received=TRUE;
   }
#endif /* #ifndef MHI_BOOT */   
   else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending)
   {
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: sm_trig = %d not handled",mhiCtx.mhi_sm_info.mhi_sm_trigs_pending);
   }
   else
   {
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: No sm_trigs_pending");
   }

   return SUCCESS;
}

/**
 * @brief       mhi_sm_run_state_machine
 *
 *              Run the MHI state machine. 
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_sm_run_state_machine(void)
{
   mhi_osal_debug(MSG_HIGH, "mhi_sm_run_state_machine: Enter");

   /* Acquire mutex */
   mhi_osal_syncenter(mhiCtx.mhi_sm_info.mhiSync);

   /* Check for reset. All states transition to reset, when RESET bit is set in MHICTRL */
   if(mhi_link_is_hw_enabled() && mhi_mmio_is_reset())
   {
      mhi_osal_debug(MSG_HIGH, "mhi_sm_run_state_machine: mhi_reset");
      mhi_core_reset();
   }

   /* Initialize the sm runtime variables */
   mhi_sm_init_runtime_variables(&sm_rtime);

   /* Set the stimulus for the state machine */
   mhi_sm_set_runtime_variables(&sm_rtime);

   /* Check for D state transitions */
   if(sm_rtime.d_newState != sm_rtime.d_oldState)
   {
      if (SUCCESS != mhi_pm_process_d_state_change(sm_rtime.d_oldState, sm_rtime.d_newState, sm_rtime.mhi_oldState,
                                                  mhiCtx.mhi_sm_info.wakeup_pending))
      {
         mhi_osal_debug(MSG_ERR,"mhi_sm_run_state_machine: SYSERR in mhi_pm_process_d_state_change");
         goto err_sync;
      }
      goto exit;
   }
   /* Check if the host set wakeup */
   else if(sm_rtime.wakeup_received_from_host)
   {
      /* Process wakeup from host */
      mhi_pm_wakeup_received_fromHost();

      /* SYSERR if it was pending */
      if (mhiCtx.is_syserr_pending)
      {
         mhi_osal_debug(MSG_ERR,"mhi_sm_run_state_machine: SYSERR pending, calling SYSERR api");
         goto err_sync;
      }
   goto exit;
   } 
    
   switch(sm_rtime.mhi_oldState)
   {
       case MHI_STATE_RESET:
         /* Get the MHI state. If not RESET move to correct state. Required for
          * PCIe bootchain, where MHI is already in M0 by the time AMSS is launched */ 
         if(MHI_STATE_RESET != mhi_mmio_get_mhistate())
         {
            /* Set internal state to current state */
            mhiCtx.mhi_sm_info.mhiState = mhi_mmio_get_mhistate(); 
         }
         
#ifndef MHI_BOOT            
         if(MHI_STATE_M0 == mhi_mmio_get_mhistate())
         {
            mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M0_entry++;
            mhiCtx.mhi_sm_info.mhi_sm_debug_counters.time_spent_in_mhi_state[MHI_STATE_M0].start_Time_ms=mhi_osal_get_systime_ms();
         }
#endif            
         break;
      case MHI_STATE_READY:
         if(MHI_STATE_M0 == sm_rtime.mhi_newState)
         {
            mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: State transition Ready -> M0"); 
            /* Cache channel contexts, event contexts etc. */

            if (SUCCESS != mhi_core_cache_hostconfig())
            {
               mhi_core_sys_error();
            }

            mhi_sm_set_mhiState(MHI_STATE_M0); 
            
#ifndef MHI_BOOT
            mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M0_entry++;
            mhiCtx.mhi_sm_info.mhi_sm_debug_counters.time_spent_in_mhi_state[MHI_STATE_M0].start_Time_ms=mhi_osal_get_systime_ms();
#endif /* #ifndef MHI_BOOT */

            /* Release the mhiSync mutex and reacquire after signalling waiting
             * channels to avoid deadlock */
            mhi_osal_syncleave(mhiCtx.mhi_sm_info.mhiSync);    
            /* Signal any waiting channels */
            mhi_core_process_ch_requestopen();
            mhi_osal_syncenter(mhiCtx.mhi_sm_info.mhiSync);  

            /* Send MHI M0 state set event */
            mhi_sm_send_state_change_event(MHI_STATE_M0);

            /* Send exec env state change notification to host */   
            mhi_core_send_exec_env_change_event(MHI_ENV);
         }
         else
         {
            mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Nothing to do in READY"); 
         }
         break;
#ifndef MHI_BOOT            
      case MHI_STATE_M0:
         if(MHI_STATE_M3 == sm_rtime.mhi_newState)
         {
            mhi_osal_syncleave(&mhiCtx.mhiSync);
            
            /* IPA suspend */
            if (SUCCESS != mhi_hw_acc_suspend(MHI_STATE_M3))
            {
               mhi_osal_debug(MSG_FATAL, "mhi_sm_run_state_machine: HW ACC failed to suspend when going to M3"); 
               goto err_nosync;
            }

            /* Device needs to set all valid channels to suspend in the host context. */
            if (SUCCESS != mhi_core_suspend())
            {
               mhi_osal_debug(MSG_ERR,"mhi_sm_run_state_machine: Failed to suspend");
               goto err_nosync;
            }
            
            mhi_osal_syncenter(&mhiCtx.mhiSync);

            mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: State transition M0 -> M3");
            mhi_sm_set_mhiState(MHI_STATE_M3); 

            /* Update debug counters */
            mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M0_M3_entry++;
            mhiCtx.mhi_sm_info.mhi_sm_debug_counters.time_spent_in_mhi_state[MHI_STATE_M3].start_Time_ms=mhi_osal_get_systime_ms();
            mhi_util_record_time_stats_ms(&mhiCtx.mhi_sm_info.mhi_sm_debug_counters.time_spent_in_mhi_state[MHI_STATE_M0]);
            
            /* Send MHI M3 state event */
            mhi_sm_send_state_change_event(MHI_STATE_M3);

            /* Disable link inactivity intr, not relevant in M3 */
            mhi_link_disable_inactivity_intr(); 

            /* Check if MHI has pending work to be done */ 
            if (mhi_core_is_work_pending())
            {
               mhi_sm_wakeup_from_M3(&sm_rtime);
            }
         }
         else if(sm_rtime.device_wake_newState != sm_rtime.device_wake_oldState)
         {
            /* Device wake toggled, enabl/disable link inactivity intr accordingly */
            if (LOW == sm_rtime.device_wake_newState)
            {
               mhi_link_enable_inactivity_intr(MAX_INACTIVITY); 
            }
            else 
            {
               mhi_link_disable_inactivity_intr();
            }
         }
         else if(sm_rtime.link_inact_intr_received && FALSE == mhiCtx.mhi_link_info.mhi_link_inact_timer_running)  
         {
            if(LOW != sm_rtime.device_wake_newState) 
            {
               mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: link inactivity intr fired but device_wake asserted");
               goto exit;
            }

            if (mhi_core_is_work_pending())
            {
               mhi_osal_debug(MSG_HIGH, "mhi_sm_run_state_machine: Work pending, don't suspend MHI"); 
               /* Do not suspend if there is work pending */
               goto exit;
            }

            mhi_osal_syncleave(&mhiCtx.mhiSync);

            /* IPA suspend */
            if (SUCCESS != mhi_hw_acc_suspend(MHI_STATE_M1))
            {
               mhi_osal_debug(MSG_HIGH, "mhi_sm_run_state_machine: HW ACC has work pending, don't suspend MHI"); 
               /* Do not suspend if there is work pending */
               mhi_osal_syncenter(&mhiCtx.mhiSync); 
               goto exit;
            }   
       
            /* Device needs to set all valid channels to suspend in the host context. */
            if (SUCCESS != mhi_core_suspend())
            {
               mhi_osal_debug(MSG_ERR,"mhi_sm_run_state_machine: Failed to suspend");
               goto err_nosync; 
            }

            mhi_osal_syncenter(&mhiCtx.mhiSync);

            /* If it's been < LINK_SHORTEST_MAX_INACT_TIME_US since link inactivity was armed, assert */
            if (TIME_US_TO_MS(mhiCtx.mhi_link_info.min_inact_expiry_time_us) > (mhi_osal_get_systime_ms() - mhiCtx.mhi_link_info.time_max_link_inact_armed_ms))
               mhi_osal_debug(MSG_FATAL,"Trying to enter M1 < %dms from last arming",TIME_US_TO_MS(mhiCtx.mhi_link_info.min_inact_expiry_time_us));

            mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: State transition M0 -> M1");  
            mhi_sm_set_mhiState(MHI_STATE_M1);

            mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M1_entry++;
            mhiCtx.mhi_sm_info.mhi_sm_debug_counters.time_spent_in_mhi_state[MHI_STATE_M1].start_Time_ms=mhi_osal_get_systime_ms();
            mhi_util_record_time_stats_ms(&mhiCtx.mhi_sm_info.mhi_sm_debug_counters.time_spent_in_mhi_state[MHI_STATE_M0]);

            /* Send MHI M1 state event */
            mhi_sm_send_state_change_event(MHI_STATE_M1);
         }
         else
         {
            mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Nothing to do in M0");
         }
         break;
      case MHI_STATE_M1:
         if (MHI_STATE_M2 == sm_rtime.mhi_newState)
         {
            mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: State transition M1 -> M2"); 
            mhi_sm_set_mhiState(MHI_STATE_M2);  
            mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M2_entry++;
            mhiCtx.mhi_sm_info.mhi_sm_debug_counters.time_spent_in_mhi_state[MHI_STATE_M2].start_Time_ms=mhi_osal_get_systime_ms();             
            mhi_util_record_time_stats_ms(&mhiCtx.mhi_sm_info.mhi_sm_debug_counters.time_spent_in_mhi_state[MHI_STATE_M1]);  
        
            /* Arm link inactivity timer for expiration at the minimum inactivity threshold. On expiry, MHI will attempt to
             * to enter sleep mode */
            mhi_link_enable_inactivity_intr(MIN_INACTIVITY); 

            /* Exit M2 immediately under the following circumstances. New client requests will trigger a wakeup */
            if (TRUE == sm_rtime.device_wake_newState ||
                mhiCtx.mhi_sm_info.wakeup_pending ||
                mhi_core_is_work_pending())
            {
                mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: exit M2 now:: device_wake %d, wakeup_pending %d, core_work_pending %d",
                                         sm_rtime.device_wake_newState,mhiCtx.mhi_sm_info.wakeup_pending,mhiCtx.is_mhi_work_pending);
                if (SUCCESS != mhi_sm_wakeup_from_M2(&sm_rtime))
                {
                   mhi_osal_debug(MSG_ERR,"mhi_sm_run_state_machine: Failed to wakeup from M2");
                   goto err_sync;
                }
            }

            /* Raise priority of worker thread when in M2 state in order to avoid any latencies */
            mhi_osal_set_priority(mhiCtx.mhi_core_tgt_config->mhi_worker_priority_high);
         }
         else if(MHI_STATE_M3 == sm_rtime.mhi_newState)
         {
            /* No need to suspend as we are in M1  */
            mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: State transition M1 -> M3");
            mhi_sm_set_mhiState(MHI_STATE_M3);  
            mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M1_M3_entry++;
            mhiCtx.mhi_sm_info.mhi_sm_debug_counters.time_spent_in_mhi_state[MHI_STATE_M3].start_Time_ms=mhi_osal_get_systime_ms();
            mhi_util_record_time_stats_ms(&mhiCtx.mhi_sm_info.mhi_sm_debug_counters.time_spent_in_mhi_state[MHI_STATE_M1]);
            /* Send MHI M3 event */
            mhi_sm_send_state_change_event(MHI_STATE_M3);
            
            /* Disable link inactivity intr, not relevant in M3 */
            mhi_link_disable_inactivity_intr();

            /* Check if MHI has pending work to be done */ 
            if (mhi_core_is_work_pending() || mhiCtx.mhi_sm_info.wakeup_pending)
            {
               mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Wake up condition detected in M3");
               mhi_sm_wakeup_from_M3(&sm_rtime);
            }
         }
         /* Device wake will be checked when SM attempts to go to M2, otherwise unnecessary wake up may
          * occur due to device wake if SM goes from M1 to M3 instead of M2 */
         else if (sm_rtime.mhi_core_requested_wakeup || sm_rtime.mhi_hw_acc_requested_wakeup)
         { 
            /* Wakeups are acted upon only in M2, D3hot or D3cold states */
            mhiCtx.mhi_sm_info.wakeup_pending = TRUE;
            mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Wake up received in M1");
         }
         else
         {
            mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Nothing to do in M1");
         }
         break; 
      case MHI_STATE_M2:
         if (sm_rtime.device_wake_newState || sm_rtime.mhi_core_requested_wakeup || sm_rtime.mhi_hw_acc_requested_wakeup)
         {             
            if (SUCCESS != mhi_sm_wakeup_from_M2(&sm_rtime))
            {
               mhi_osal_debug(MSG_ERR,"mhi_sm_run_state_machine: Failed to wakeup from M2");
               goto err_sync;
            }
         } 
         else if (sm_rtime.link_inact_intr_received && FALSE == mhiCtx.mhi_link_info.mhi_link_inact_timer_running)
         {
            /* Link is not active, enter LPM */
            if (SUCCESS != mhi_pm_enter_sleep_mode(LPM))
            {
               /* Restart the inactivity timer with min value */
               mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Could not enter sleep mode:LPM"); 
               mhi_link_enable_inactivity_intr(MIN_INACTIVITY);
            }
            else
            {  
               /* Went back to sleep mode after link activity event received in M2.
                * Increment debug counter */
               if (mhiCtx.mhi_sm_info.link_activity_detected_in_M2)
               {
                  mhiCtx.mhi_sm_info.mhi_sm_debug_counters.link_activity_in_M2_without_M0++;
                  mhiCtx.mhi_sm_info.link_activity_detected_in_M2=FALSE;
               }

               /* Exit sleep mode immediately if the MHI interrupt is set */
               if (mhi_osal_is_irq_set(mhiCtx.irqhandle))
               {
                  mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: MHI Interrupt is set in M2"); 
                  /* MHI Interrupt is set, exit LPM */
                  if (SUCCESS != mhi_pm_exit_sleep_mode())
                  {
                     mhi_osal_debug(MSG_ERR,"mhi_sm_run_state_machine: Failed to exit sleep mode");
                     goto err_sync;
                  }
                  
                  /* Set link activity detected flag */
                  mhiCtx.mhi_sm_info.link_activity_detected_in_M2=TRUE;                                  
                  
                  /* Restart the inactivity timer with min value */
                  mhi_link_enable_inactivity_intr(MIN_INACTIVITY);
               }
            }
         }
         else if (sm_rtime.link_act_event_received)
         {
            /* Link activity detected, exit LPM */
            if (SUCCESS != mhi_pm_exit_sleep_mode())
            {
               mhi_osal_debug(MSG_ERR,"mhi_sm_run_state_machine: Failed to exit sleep mode");
               goto err_sync;
            }  

            /* Set link activity detected flag */
            mhiCtx.mhi_sm_info.link_activity_detected_in_M2=TRUE;                                  
            
            /* Restart the inactivity timer with min value */
            mhi_link_enable_inactivity_intr(MIN_INACTIVITY);
         }
         else
         {
             mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Nothing to do in M2");
         }
         break;         
      case MHI_STATE_M3: 
         if(MHI_STATE_M0 == sm_rtime.mhi_newState)
         {
            mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: State transition M3 -> M0"); 
            mhi_sm_set_mhiState(MHI_STATE_M0); 
            mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M0_entry++;
            mhiCtx.mhi_sm_info.mhi_sm_debug_counters.time_spent_in_mhi_state[MHI_STATE_M0].start_Time_ms=mhi_osal_get_systime_ms();
            mhi_util_record_time_stats_ms(&mhiCtx.mhi_sm_info.mhi_sm_debug_counters.time_spent_in_mhi_state[MHI_STATE_M3]);

            /* Send MHI M0 state change event */
            mhi_sm_send_state_change_event(MHI_STATE_M0);
            
            /* resume mhi */
            if (SUCCESS != mhi_core_resume())
            {
               mhi_osal_debug(MSG_ERR,"mhi_sm_run_state_machine: Failed to resume");
               goto err_sync;
            }
           
            /* resume ipa */
            mhi_hw_acc_resume();

            /* Enable link inactivity timer if required */
            if (LOW == sm_rtime.device_wake_newState)
            {
               mhi_link_enable_inactivity_intr(MAX_INACTIVITY); 
            }

            /* Resume procedure complete, clear wake up pending flag */
            mhiCtx.mhi_sm_info.wakeup_pending = FALSE;
         }
         else if (sm_rtime.mhi_core_requested_wakeup || sm_rtime.mhi_hw_acc_requested_wakeup)
         {
            mhi_sm_wakeup_from_M3(&sm_rtime);
         }
         else
         {
            mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Nothing to do in M3");
         }
         break;
#endif /* #ifndef MHI_BOOT */           
      case MHI_STATE_SYSERR:
            mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: MHI_STATE_SYSERR Nothing to do");
         break;   
      default:
            mhi_osal_debug(MSG_FATAL,"mhi_sm_run_state_machine: Unexpected state M state %d",sm_rtime.mhi_oldState);
         return; 
   }

   /* Check for reset. All states transition to reset, when RESET bit is set in MHICTRL */
   if(mhi_link_is_hw_enabled() && mhi_mmio_is_reset())
   {
      mhi_osal_debug(MSG_HIGH, "mhi_sm_run_state_machine: mhi_reset");
      mhi_core_reset();
   }


exit:
   /* Release mutex */
   mhi_osal_syncleave(mhiCtx.mhi_sm_info.mhiSync);
   return;

err_sync:
   /* Release mutex */
   mhi_osal_syncleave(mhiCtx.mhi_sm_info.mhiSync);

err_nosync:
   mhi_core_sys_error();
   return;
}

#ifndef MHI_BOOT 
/**
 * @brief       mhi_sm_update_early_exit_debug_counters
 *
 *              Update counters for early exit from M2/M3
 *
 * @param[in]   early_exit - Struct containing debug counters
 * @param[in]   sm_Rtime - Pending trigger for state machine
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_sm_update_early_exit_debug_counters(mhi_state_early_exit_t *early_exit, mhi_sm_runtime_t *sm_Rtime)
{
   early_exit->num_of_early_exits++;
  
   if (MHI_STATE_M2 == mhiCtx.mhi_sm_info.mhiState)
      if (sm_Rtime->device_wake_newState)
         early_exit->device_wake++;
    
   if (sm_Rtime->mhi_core_requested_wakeup)
      early_exit->core_wake++;

   if (sm_Rtime->mhi_hw_acc_requested_wakeup)
      early_exit->hw_acc_wake++;
      
   if (mhiCtx.is_mhi_work_pending)
      early_exit->mhi_work_pending_flag_set++;
}

/**
 * @brief       mhi_sm_wakeup_from_M2
 *
 *              Handles the wakeup procedure from M2 MHI state.
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
static uint32 mhi_sm_wakeup_from_M2(mhi_sm_runtime_t *sm_Rtime)
{
   /* Update debug counters */
   if (NONE == mhiCtx.mhi_pm_info.sleep_mode)
      mhi_sm_update_early_exit_debug_counters(&mhiCtx.mhi_sm_info.mhi_sm_debug_counters.early_M2_exit,sm_Rtime);

   /* Clear link activity detected flag if set */
   if (mhiCtx.mhi_sm_info.link_activity_detected_in_M2)
      mhiCtx.mhi_sm_info.link_activity_detected_in_M2=FALSE;

   /* Exit sleep mode */
   mhi_pm_exit_sleep_mode();
   
   /* M1 is set just for the record, since it's just a transient state. */
   mhi_osal_debug(MSG_HIGH,"mhi_sm_wakeup_from_M2: State transition M2 -> M1");
   mhi_sm_set_mhiState(MHI_STATE_M1);

   mhi_osal_debug(MSG_HIGH,"mhi_sm_wakeup_from_M2: State transition M1 -> M0");
   mhi_sm_set_mhiState(MHI_STATE_M0); 
   mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M0_entry++;
   mhiCtx.mhi_sm_info.mhi_sm_debug_counters.time_spent_in_mhi_state[MHI_STATE_M0].start_Time_ms=mhi_osal_get_systime_ms();
   mhi_util_record_time_stats_ms(&mhiCtx.mhi_sm_info.mhi_sm_debug_counters.time_spent_in_mhi_state[MHI_STATE_M2]);

   /* Send MHI M0 state set event */
   mhi_sm_send_state_change_event(MHI_STATE_M0);

   /* Disable link inactivity timer when exiting M2. Will be re-enabled when applicable */
   mhi_link_disable_inactivity_intr();
   /* If link inactivity event is pending, it is now stale since we exited M2. Clear it */
   if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_LINK_INACT_TIMER_SHIFT))
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_LINK_INACT_TIMER_SHIFT);
      mhi_osal_debug(MSG_HIGH,"mhi_sm_wakeup_from_M2: Cleared stale link inactivity event");    
   }

   /* Stop the M2 wake up timer */
   mhi_osal_timer_stop(&mhiCtx.mhi_sm_info.M2_wakeup_timer);

   /* resume mhi */
   if (SUCCESS != mhi_core_resume())
   {
      mhi_osal_debug(MSG_ERR,"mhi_sm_wakeup_from_M2: Failed to resume"); 
      return ERROR;
   }
            
   /* resume ipa */
   mhi_hw_acc_resume();

   /* Resume procedure complete, clear wake up pending flag */
   mhiCtx.mhi_sm_info.wakeup_pending = FALSE;

   /* Reset priority */
   mhi_osal_set_priority(mhiCtx.mhi_core_tgt_config->mhi_worker_priority);
  
   return SUCCESS;
}

/**
 * @brief       mhi_sm_wakeup_from_M3
 *
 *              Handles the wake up procedure from M3 if a wake up condition
 *              exists.
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
static uint32 mhi_sm_wakeup_from_M3(mhi_sm_runtime_t *sm_Rtime)
{  
   /* Set flag to register pending wake up */
   if (FALSE == mhiCtx.mhi_sm_info.wakeup_pending)
   {
      mhi_osal_debug(MSG_HIGH,"mhi_sm_wakeup_from_M3: mhi_sm_info.wakeup_pending = TRUE");
      mhiCtx.mhi_sm_info.wakeup_pending = TRUE;
   }

   if (MHI_LINK_D0 == mhiCtx.mhi_pm_info.current_d_state)
      mhi_sm_update_early_exit_debug_counters(&mhiCtx.mhi_sm_info.mhi_sm_debug_counters.early_M3_exit,sm_Rtime);
  
   /* Wake up host, necessary in case device is already in low power state (ex: D3cold) */
   mhi_pm_wakeup_host();

   /* Wake up pending flag will be cleared when the state machine goes back to M0 */
   return MHI_SUCCESS;
}
#endif /* #ifndef MHI_BOOT */

/**
 * @brief       mhi_sm_set_trig_pending
 *
 *              Set a pending MHI state machine trigger notification 
 *
 * @param[in]   trigger - Event notification that needs to be registered with pending
 *                        notifications for the state machine. 
 * @param[in]   signal_worker_thread - TRUE if mhi_core_scheduler needs to be signaled. 
 *
 * @dependencies
 *              mhi_sm_init
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_sm_set_trig_pending(uint32 trigger, boolean signal_worker_thread)
{
   mhi_osal_debug(MSG_HIGH,"mhi_sm_set_trig_pending: %d",trigger);

   mhi_osal_atomic_set_bit(&mhiCtx.mhi_sm_info.mhi_sm_trigs_pending,trigger);
   /* External triggers usually require some processing on the MHI CORE thread.
    * But state machine triggers originating from the MHI worker thread wouldn't
    * want to unnecessarily signal the MHI CORE thread. */
   if (signal_worker_thread)
   {
      mhi_core_signal_worker_thread();
   }
}

/**
 * @brief       mhi_sm_clear_trig_pending
 *
 *              Clear a pending MHI state machine trigger notification 
 *
 * @param[in]   trigger - Event notification that needs to be cleared from the
 *                        memory location containing pending notifications.  
 *
 * @dependencies
 *              mhi_sm_init
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_sm_clear_trig_pending(uint32 trigger)
{
   mhi_osal_debug(MSG_HIGH,"mhi_sm_clear_trig_pending: %d",trigger);

   mhi_osal_atomic_clear_bit(&mhiCtx.mhi_sm_info.mhi_sm_trigs_pending,trigger);
}

/**
 * @brief       mhi_sm_clear_trig_pending
 *
 *              Get status of which state machine event notifications or triggers
 *              are currently pending. 
 *
 * @dependencies
 *              mhi_sm_init
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
mhi_sm_trigs_pending_t mhi_sm_get_trigs_status(void)
{
   return mhiCtx.mhi_sm_info.mhi_sm_trigs_pending;
}

/**
 * @brief       mhi_sm_get_mhistate
 *
 *              Get the current MHI state according to the state machine. This
 *              is for use by external modules.
 *
 * @dependencies
 *              mhi_sm_init
 * 
 * @return       
 *              MHI state  
 * 
 * @sideeffect  None 
 */
mhi_state_t mhi_sm_get_mhistate(void)
{
   mhi_state_t state;

   /* Synchronize with the MHI state machine */
   mhi_osal_syncenter(mhiCtx.mhi_sm_info.mhiSync);
   state = mhiCtx.mhi_sm_info.mhiState;
   mhi_osal_syncleave(mhiCtx.mhi_sm_info.mhiSync);

   return state;
}

/**
 * @brief       mhi_sm_set_syserr_mhistate
 *
 *              Set the MHI state to SYSERR. All other MHI states should be 
 *              handled by run_state_machine.
 *
 * @dependencies
 *              mhi_sm_init
 * 
 * @return       
 *              MHI state  
 * 
 * @sideeffect  None 
 */
void mhi_sm_set_syserr_mhistate(void)
{
   /* Synchronize with the MHI state machine */
   mhi_osal_syncenter(mhiCtx.mhi_sm_info.mhiSync);
   mhiCtx.mhi_sm_info.mhiState = MHI_STATE_SYSERR;
   mhi_mmio_set_mhistate(mhiCtx.mhi_sm_info.mhiState);
   mhi_osal_syncleave(mhiCtx.mhi_sm_info.mhiSync);
}

/**
 * @brief       mhi_sm_M2_wakeup_timer_cb
 *
 *              Callback function for M2 wake up timer
 *
 *
 * @param[in]   time_ms - Time elapsed since the timer started
 * @param[in]   data - User data supplied when the timer was started
 *
 * @dependencies
 *              mhi_sm_init
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_sm_M2_wakeup_timer_cb(int32 time_ms, mhi_timer_user_data data)
{
   if (MHI_STATE_M2 == mhiCtx.mhi_sm_info.mhiState)
      mhi_osal_debug(MSG_FATAL,"mhi_sm_M2_wakeup_timer_cb: Still in M2");
}

/**
 * @brief       mhi_sm_set_device_wake_pending
 *
 *              Sets the devive wake trigger for the state machine and does 
 *              any preprocessing, if required
 * 
 *
 * @param[in]   device_wake_state - State of device wake
 *
 * @dependencies
 *              mhi_sm_init
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_sm_set_device_wake_pending(uint32 device_wake_state)
{
   /* Start M2 wake up timer */ 
   if (MHI_STATE_M2 == mhiCtx.mhi_sm_info.mhiState && HIGH == device_wake_state)
      mhi_osal_timer_start(&mhiCtx.mhi_sm_info.M2_wakeup_timer,mhi_sm_M2_wakeup_timer_cb,NULL,MAX_TIME_FOR_M2_EXIT_MS,0);

   /* Set device wake trigger for state machine */
   mhi_sm_set_trig_pending(MHI_SM_TRIG_DEVICE_WAKE_SHIFT,TRUE);
}




