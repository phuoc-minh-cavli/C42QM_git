/**
  @file mhi_pm.c
  @brief
  This file contains the MHI power management implementation.
    

*/
/*
===============================================================================
   
                             Edit History
 
 $Header: 
 
when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/07/14   Dilip   Added changes for Inband Device wake
11/03/14   am      Added time stats for sleep mode
10/21/14   am      Ack d state after changing state
05/27/14   am      SYSERR if D3cold is unexpected
05/23/14   am      Changed nomenclature of function for sleep entry/exit
03/25/13   am      Changed sleep voting function and D3cold handling   
08/15/13   am      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "mhi_pm.h"
#include "mhi_core.h"
#include "mhi_hwacc.h"
#include "mhi_sm.h"

/* GPIO pin# for device wake in the TLMM */
#define MHI_GPIO_DEVICE_WAKE_PINOUT  60

#ifndef MHI_BOOT
/* GPIO handle */
mhi_gpio_handle_t mhi_gpio_device_wake = {NULL};                                           
#endif

#ifndef MHI_BOOT
#ifndef MHI_DEVICEWAKE_DB
/** 
 * @brief      mhi_pm_device_wake_isr
 *
 *             Device Wake ISR
 *              
 * 
 * @param[in]  data - user data
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_pm_device_wake_isr(uint32 data)
{
#ifndef MHI_PM_STATE_TEST 
   DALResult dal_result;
#endif

#ifndef MHI_DUAL_EDGE_GPIO_TRIG_SUPPORTED  
   GPIOIntTriggerType trigger;
#endif

   /* pm test loop directly manipulates mhiCtx.mhi_pm_info.device_wake_gpio_state for now */ 
#ifndef MHI_PM_STATE_TEST 
   dal_result = DalTlmm_GpioIn(mhi_gpio_device_wake.h_mhi_tlmm,
         mhi_gpio_device_wake.mhi_gpio_pin,(DALGpioValueType *)&mhiCtx.mhi_pm_info.device_wake_gpio_state);

   MHI_ASSERT(DAL_SUCCESS == dal_result && mhiCtx.mhi_pm_info.device_wake_gpio_state <= DAL_GPIO_HIGH_VALUE);
#endif
   mhi_osal_debug(MSG_HIGH,"mhi_pm_device_wake_isr: %d",mhiCtx.mhi_pm_info.device_wake_gpio_state);

#ifndef MHI_DUAL_EDGE_GPIO_TRIG_SUPPORTED
   if (LOW == mhiCtx.mhi_pm_info.device_wake_gpio_state)
   {
      trigger = GPIOINT_TRIGGER_HIGH;
   }
   else 
   {
      trigger = GPIOINT_TRIGGER_LOW;
   }

   dal_result = GPIOInt_SetTrigger(
         mhi_gpio_device_wake.h_mhi_gpio_int,
         MHI_GPIO_DEVICE_WAKE_PINOUT, 
         trigger); 

   MHI_ASSERT(DAL_SUCCESS == dal_result);
#endif  

   mhi_sm_set_trig_pending(MHI_SM_TRIG_DEVICE_WAKE_SHIFT,TRUE);
}
#endif  /* MHI_DEVICEWAKE_DB */
#endif /* #ifndef MHI_BOOT */


/**
 * @brief       mhi_pm_init
 *
 *              Initialize the MHI power management software module.
 *
 * @param[in]   *pm_info - pointer to the MHI power module context.
 *
 * @dependencies
 *              Device must be in D0.
 * 
 * @return       
 *              SUCCESS\ERROR  
 * 
 * @sideeffect  None 
 */
uint32 mhi_pm_init(mhi_pm_info_t *pm_info)
{
#ifndef MHI_BOOT
#ifndef MHI_DEVICEWAKE_DB
   DALResult dal_result;
   GPIOIntTriggerType trigger = GPIOINT_TRIGGER_DUAL_EDGE;
#endif
#endif

   mhi_osal_debug(MSG_HIGH,"mhi_pm_init");

   /* Device must be in D0 during init */
   mhiCtx.mhi_pm_info.current_d_state = MHI_LINK_D0;

   /* Sleep mode should not be entered until init is complete */
   mhiCtx.mhi_pm_info.sleep_mode = NONE;

   /* Initialize sleep stats */
   memset(&mhiCtx.mhi_pm_info.sleep_stats,0,sizeof(mhi_time_stats_t)*MAX_SLEEP_MODE);

#ifndef MHI_BOOT
#ifndef MHI_DEVICEWAKE_DB
   dal_result = DAL_DeviceAttach(DALDEVICEID_TLMM,&mhi_gpio_device_wake.h_mhi_tlmm);

   if(DAL_SUCCESS != dal_result || !mhi_gpio_device_wake.h_mhi_tlmm)
   {
      return ERROR;
   }

   dal_result = DAL_DeviceAttach(DALDEVICEID_GPIOINT,&mhi_gpio_device_wake.h_mhi_gpio_int) ;

   if(DAL_SUCCESS != dal_result || !mhi_gpio_device_wake.h_mhi_gpio_int)
   {
      return ERROR;
   }    

   mhi_gpio_device_wake.mhi_gpio_pin = DAL_GPIO_CFG(
         MHI_GPIO_DEVICE_WAKE_PINOUT,
         0,
         DAL_GPIO_INPUT,
         DAL_GPIO_PULL_DOWN,
         DAL_GPIO_2MA); 

   dal_result = DalTlmm_ConfigGpio(
         mhi_gpio_device_wake.h_mhi_tlmm, 
         mhi_gpio_device_wake.mhi_gpio_pin,
         DAL_TLMM_GPIO_ENABLE);

   if(DAL_SUCCESS != dal_result)
   {
      return ERROR;
   }

#ifndef MHI_PM_STATE_TEST
   dal_result = DalTlmm_GpioIn(mhi_gpio_device_wake.h_mhi_tlmm,
         mhi_gpio_device_wake.mhi_gpio_pin,(DALGpioValueType *)&mhiCtx.mhi_pm_info.device_wake_gpio_state);
#else
   mhiCtx.mhi_pm_info.device_wake_gpio_state = TRUE;
#endif

   if (LOW == mhiCtx.mhi_pm_info.device_wake_gpio_state)
   {
#ifndef MHI_DUAL_EDGE_GPIO_TRIG_SUPPORTED 
      trigger = GPIOINT_TRIGGER_HIGH;
#endif
      mhi_link_enable_inactivity_intr(MAX_INACTIVITY);
   }
   else 
   {
#ifndef MHI_DUAL_EDGE_GPIO_TRIG_SUPPORTED 
      trigger = GPIOINT_TRIGGER_LOW;
#endif
      mhi_link_disable_inactivity_intr();
   }    

   /* Register ISR for device_wake signal */
   dal_result = GPIOInt_RegisterIsr(
         mhi_gpio_device_wake.h_mhi_gpio_int,
         MHI_GPIO_DEVICE_WAKE_PINOUT,
         trigger,
         (GPIOINTISR)mhi_pm_device_wake_isr,
         NULL) ;
   if(DAL_SUCCESS != dal_result)
   {
      return ERROR;
   }

   /* Trigger whenever the state of device_wake gpio changes */
   dal_result = GPIOInt_SetTrigger(
         mhi_gpio_device_wake.h_mhi_gpio_int,
         MHI_GPIO_DEVICE_WAKE_PINOUT, 
         trigger); 
   if(DAL_SUCCESS != dal_result)
   {
      return ERROR;
   }
#endif
#else /* #ifndef MHI_BOOT */
   mhiCtx.mhi_pm_info.device_wake_gpio_state = TRUE; /* default init value */
#endif /* #ifndef MHI_BOOT */

   return SUCCESS;

}

/**
 * @brief       mhi_pm_reset
 *
 *              Reset the MHI power manaagment software module.
 *
 * @dependencies
 *              Device must be in D0. 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */  
void mhi_pm_reset(void)
{
   /* Device must be in D0 when MHI is reset  */
   mhiCtx.mhi_pm_info.current_d_state = MHI_LINK_D0;
}

/**
 * @brief       mhi_pm_core_wakeup
 *
 *              API called by MHI core to wake up MHI from low power state.
 *
 * @dependencies
 *              Can be called only after mhi_init 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_pm_core_wakeup(void)
{
  mhi_osal_debug(MSG_HIGH,"mhi_pm_core_wakeup");
  mhi_sm_set_trig_pending(MHI_SM_TRIG_CORE_WAKEUP_SHIFT,FALSE);
}  

/**
 * @brief       mhi_pm_hw_acc_wakeup
 *
 *              API called by the hardware accelerator to wake up from
 *              low power state.
 *              
 * @dependencies
 *              Can be called only after mhi_init  
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_pm_hw_acc_wakeup(void)
{
   mhi_osal_debug(MSG_HIGH,"mhi_pm_hw_acc_wakeup");
   mhi_sm_set_trig_pending(MHI_SM_TRIG_HW_ACC_WAKEUP_SHIFT,TRUE);
}

/**
 * @brief       mhi_pm_process_d_state_change
 *
 *              Processes a change of state device power (D states).
 *              
 * @param[in]   d_oldState - D state to change from              
 * @param[in]   d_newState - D state to change to
 * @param[in]   mhiState - M state
 * @param[in]   wakeup_pending - is there a wake up condition pending
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              SUCCESS - Sucessfully processed D state hange
 *              SYSERR - Error condition encountered 
 * 
 * @sideeffect  D state change may result in link hw being enabled/disabled 
 */
uint32 mhi_pm_process_d_state_change(mhi_link_d_state_t d_oldState, mhi_link_d_state_t d_newState, mhi_state_t mhiState, boolean wakeup_pending)
{
   /* Other than D3cold and D0 all other D state changes should happen in M3 only */
   if (MHI_LINK_D0 != d_newState && MHI_LINK_D3cold != d_newState &&  MHI_STATE_M3 != mhiState) 
      mhi_osal_debug(MSG_FATAL,"mhi_sm_process_d_state_change: Unexpected D state transition \
                     to %d in M state: %d",d_newState,mhiState);

   switch (d_newState)
   {
      case MHI_LINK_D0:
         /* nothing to do */
         mhi_osal_debug(MSG_HIGH,"mhi_pm_process_d_state_change: Change to MHI_LINK_D0 complete");
         break;
      case MHI_LINK_D1:
         /* No trigger defined at this time */
         /* nothing to do */
         mhi_osal_debug(MSG_ERR,"mhi_pm_process_d_state_change: MHI_LINK_D1 Not Supported");
         break;
      case MHI_LINK_D2:
         /* No trigger defined at this time */
         /* nothing to do */
         mhi_osal_debug(MSG_ERR,"mhi_pm_process_d_state_change: MHI_LINK_D2 Not Supported");
         break;
      case MHI_LINK_D3hot:
         mhi_osal_debug(MSG_HIGH,"mhi_pm_process_d_state_change: Change to MHI_LINK_D3hot complete");
         /* Check if wake up is pending */
         if (TRUE == wakeup_pending)
         {
            mhi_pm_wakeup_host();
         }
         break;
      case MHI_LINK_D3cold:
         mhi_osal_debug(MSG_HIGH,"mhi_pm_process_d_state_change: D3cold recieved in M state %d",mhiState);

         if (MHI_LINK_D3hot != d_oldState)
         {
            if (!mhiCtx.mhi_link_info.mhi_link_hw_enabled)
            {
               /* Can happen if MHI was in M2 when PERST was deasserted */
               mhi_pm_exit_sleep_mode();
            }

            /* Set SYSERR*/ 
            mhi_sm_set_mhiState(MHI_STATE_SYSERR);
            
            /* lower log level from FATAL in future */
            mhi_osal_debug(MSG_FATAL,"mhi_pm_process_d_state_change: D3cold unexpected, pending SYSERR!");

            /*Set SYSERR pending flag so that the event can be sent out later */
            mhiCtx.is_syserr_pending = TRUE;

            /* Assert WAKE# to the host immediately if D3cold was signaled unexpectedly */
            mhi_pm_wakeup_host();
         }
         else
         {
            if (TRUE == wakeup_pending)
            {
               /* Assert WAKE# if a wakeup request is pending */
               mhi_pm_wakeup_host();
            }
         }
         
         /* Link is not active, enter sleep mode */
         mhi_pm_enter_sleep_mode(POWER_OFF);
         mhi_osal_debug(MSG_HIGH,"mhi_pm_process_d_state_change: Change to MHI_LINK_D3cold complete");
         break;
      default:
         mhi_osal_debug(MSG_FATAL,"mhi_pm_process_d_state_change: Invalid D state received");
   }

   mhi_link_dstate_ack(d_newState);

   return SUCCESS;
}

/**
 * @brief       mhi_pm_wakeup_host
 *
 *              Calls the link power management API to send a wake up to the 
 *              host. The wake up may involve an interrupt or toggling a GPIO.
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_pm_wakeup_host(void)
{
   /* If device is in low power state, issue wake up */
   if (MHI_LINK_D3cold == mhiCtx.mhi_pm_info.current_d_state)
   {
      /* Issue a link wakeup request */  
      mhi_link_wakeup_req();
      mhi_osal_debug(MSG_HIGH,"mhi_pm_wakeup_host: WAKE# issued in D3cold");
   }
   else
   {
      mhi_osal_debug(MSG_HIGH,"mhi_pm_wakeup_host: Wake up request deferred. \
                      Current D state = %d",mhiCtx.mhi_pm_info.current_d_state);
   }
}

/**
 * @brief       mhi_pm_wakeup_received_fromHost
 *
 *              Received a request from the host to make the device exit low
 *              power mode. Note that if the link reset line is LOW, the
 *              function returns without causing exit from sleep state.
 *
 *
 * @dependencies
 *              Should be called only after mhi_pm_init. 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_pm_wakeup_received_fromHost(void)
{
  /* If reset line is DEASSERTED wakeup from sleep */
  if (DEASSERTED == mhi_link_get_reset_line_state())
     mhi_pm_exit_sleep_mode();
  else
     mhi_osal_debug(MSG_ERR,"mhi_pm_wakeup_received_fromHost: cannot exit sleep RESET is ASSERTED!");
}

/**
 * @brief       mhi_pm_enter_sleep_mode
 *
 *              Enter sleep mode by turning off resources held by
 *              MHI that are sourced from the XO, thus preparing for XO Shutdown.
 *
 * @param[in]   sleep_mode - Level of power savings
 *              
 * @dependencies
 *              mhi_pm_init must be called. Caller should make sure that this is
 *              protected by the mhiSync mutex (i.e. mutex from the mhi context).
 * 
 * @return       
 *              SUCCESS/ERROR  
 * 
 * @sideeffect  None 
 */
uint32 mhi_pm_enter_sleep_mode(mhi_pm_sleep_mode_t sleep_mode)
{   
   uint32 ret=SUCCESS;
   
   if (mhiCtx.mhi_pm_info.sleep_mode == sleep_mode)
   {
      mhi_osal_debug(MSG_HIGH,"mhi_pm_enter_sleep_mode: already in mode %d",sleep_mode);
      return SUCCESS;
   }

   mhi_osal_debug(MSG_HIGH, "mhi_pm_enter_sleep_mode: Entering sleep mode %d",sleep_mode);
   
   if (POWER_OFF == sleep_mode)
      ret=mhi_link_disable();
   else if (LPM == sleep_mode)
   {  
      /* Do not turn off link hw if hw link inactivity timer is not supported */  
      if (mhiCtx.mhi_link_info.hw_link_inact_timer_supported)
         ret=mhi_link_release_hw_resources();
   }
   else
      mhi_osal_debug(MSG_FATAL,"mhi_pm_enter_sleep_mode: Unidentified sleep mode %d",sleep_mode);

   /* Change the sleep_mode only if ret is SUCCESS */
   if (SUCCESS == ret)
   {
      /* Synchronously cause hw accelerator to trigger its own power management */
      mhi_hw_acc_enable_pm();
      
      mhiCtx.mhi_pm_info.sleep_stats[sleep_mode].start_Time_ms=mhi_osal_get_systime_ms();
      mhiCtx.mhi_pm_info.sleep_mode=sleep_mode;
   }      

   return ret;     
}

/**
 * @brief       mhi_pm_exit_sleep_mode
 *
 *              Exit sleep mode by turning on resources held by
 *              MHI that are sourced from the XO.
 *
 *              
 * @dependencies
 *              mhi_pm_init must be called. Caller should make sure that this is
 *              protected by the mhiSync mutex (i.e. mutex from the mhi context).
 * 
 * @return       
 *              SUCCESS/ERROR  
 * 
 * @sideeffect  None 
 */
uint32 mhi_pm_exit_sleep_mode(void)
{
   uint32 ret=SUCCESS;
   mhi_pm_sleep_mode_t sleep_mode=mhiCtx.mhi_pm_info.sleep_mode;
  
   if (NONE == sleep_mode)
   {
      mhi_osal_debug(MSG_HIGH, "mhi_pm_exit_sleep_mode: Nothing to do already exited sleep"); 
   }
   else 
   {
      mhi_osal_debug(MSG_HIGH, "mhi_pm_exit_sleep_mode: Exiting sleep mode %d",sleep_mode);

      if (POWER_OFF == sleep_mode)
         ret=mhi_link_enable();
      else if (LPM == sleep_mode)
         ret=mhi_link_acquire_hw_resources();
      else
         mhi_osal_debug(MSG_FATAL,"mhi_pm_enter_sleep_mode: Unidentified sleep mode %d",sleep_mode);

      /* Change the sleep_mode only if ret is SUCCESS */
      if (SUCCESS == ret)
      {
         /* Request hw accelerator to acquire power resources, synchronous call */
         mhi_hw_acc_disable_pm();

         mhi_util_record_time_stats_ms(&mhiCtx.mhi_pm_info.sleep_stats[sleep_mode]);
         mhiCtx.mhi_pm_info.sleep_mode=NONE;
      }
   }

   return ret;
}

/**
 * @brief       mhi_pm_set_d_state
 *
 *              Set the current D state in the mhi_pm context.
 * 
 * @dependencies
 *              Should be called only after mhi_pm_init. 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_pm_set_d_state(mhi_link_d_state_t d_state)
{
   mhiCtx.mhi_pm_info.current_d_state = d_state;
   mhi_osal_debug(MSG_HIGH,"mhi_pm_set_d_state: %d",d_state);
}

/**
 * @brief       mhi_pm_get_d_state
 *
 *              Get the current D state according to mhi_pm
 * 
 * @dependencies
 *              Should be called only after mhi_pm_init. 
 * 
 * @return       
 *              Current D state as cached by mhi_pm.  
 * 
 * @sideeffect  None 
 */
mhi_link_d_state_t mhi_pm_get_d_state(void)
{
   mhi_osal_debug(MSG_HIGH,"mhi_pm_get_d_state: %d",mhiCtx.mhi_pm_info.current_d_state);
   return mhiCtx.mhi_pm_info.current_d_state;
}

/**
 * @brief       mhi_pm_get_device_wake_gpio_state
 *
 *              Get the current state of the device_wake gpio.
 *              If the GPIO is not supported then get the current state of 
 *              device_wake from the designated doorbell register.  
 * 
 * @dependencies
 *              Should be called only after mhi_pm_init. 
 * 
 * @return       
 *              HIGH\LOW depending on ASSERTION\DEASSERTION of device_wake.  
 * 
 * @sideeffect  None 
 */
boolean mhi_pm_get_device_wake_gpio_state(void)
{
#ifndef MHI_BOOT
   
 #ifdef MHI_DEVICEWAKE_DB
   mhiCtx.mhi_pm_info.device_wake_gpio_state=mhi_mmio_get_chdb(MHI_PM_DEVICE_WAKE_DB_NUM);
 #else
   DALResult dal_result;

   dal_result = DalTlmm_GpioIn(mhi_gpio_device_wake.h_mhi_tlmm,
         mhi_gpio_device_wake.mhi_gpio_pin,(DALGpioValueType *)&mhiCtx.mhi_pm_info.device_wake_gpio_state);
   
   if(DAL_SUCCESS != dal_result)
      mhi_osal_debug(MSG_FATAL,"mhi_pm_get_device_wake_gpio_state: Failed to retrieve device_wake state");
#endif  
#endif /* #ifndef MHI_BOOT */

   mhi_osal_debug(MSG_HIGH,"mhi_pm_get_device_wake_gpio_state: %d",mhiCtx.mhi_pm_info.device_wake_gpio_state);

   return mhiCtx.mhi_pm_info.device_wake_gpio_state;
}

#ifndef MHI_BOOT

/** 
 * @brief      mhi_trigger_device_wake_intr
 *
 *             Artificially trigger device wake for testing
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_trigger_device_wake_intr(void)
{
   DALResult           dal_result;

   dal_result = GPIOInt_TriggerInterrupt(mhi_gpio_device_wake.h_mhi_gpio_int,MHI_GPIO_DEVICE_WAKE_PINOUT);
   MHI_ASSERT(DAL_SUCCESS == dal_result);
}
#endif /* #ifndef MHI_BOOT */

















