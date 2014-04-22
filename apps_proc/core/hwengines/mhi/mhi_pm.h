#ifndef MHI_PM_H
#define MHI_PM_H

/** 
  @file mhi_pm.h
  @brief
  This header file defines data structures and public apis associated with mhi power management. 
    

*/
/*
===============================================================================
 
                            Edit History
  
 $Header: 
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/07/14   Dilip   Added changes for Inband Device wake
08/15/13   am      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "mhi_link.h"
#include "mhi_osal.h"
#include "mhi_util.h"

#ifndef MHI_BOOT
#include "DDIGPIOInt.h"
#include "DDITlmm.h"
#endif

/** Sleep mode: determines the level of power saving */
typedef enum 
{
  NONE=0,      /**< HW resources are ON */
  LPM=1,       /**< HW resources are in low power mode, intermediate power savings */
  POWER_OFF=2, /**< HW resources are OFF, maximum power savings */
  MAX_SLEEP_MODE
} mhi_pm_sleep_mode_t;

/** MHI power management related information */
typedef struct _mhi_pm_info
{
  mhi_link_d_state_t current_d_state;           /**< Current D state as last informed by the MHI link */

  volatile uint32 device_wake_gpio_state;       /**< State of device wake GPIO */

  mhi_pm_sleep_mode_t sleep_mode;               /**< MHI sleep mode */

  mhi_time_stats_t sleep_stats[MAX_SLEEP_MODE]; /**< Profile time spent in sleep mode */
} mhi_pm_info_t;
  
#ifndef MHI_BOOT
/** GPIO handle */
typedef struct _mhi_gpio_handle
{
  DalDeviceHandle    *h_mhi_tlmm;
  DalDeviceHandle    *h_mhi_gpio_int;
  DALGpioSignalType   mhi_gpio_pin;
} mhi_gpio_handle_t;
#endif

/**
 * @brief       mhi_pm_init
 *
 *              Initialize the MHI power management software module.
 *
 * @param[in]   *pm_info - pointer to the MHI power module context.
 *
 * @dependencies
 *              Device must be in D0.
 *              Must be called after mhi_link_init
 * 
 * @return       
 *              SUCCESS\ERROR  
 * 
 * @sideeffect  None 
 */
uint32 mhi_pm_init(mhi_pm_info_t *pm_info);

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
void mhi_pm_reset(void);

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
void mhi_pm_hw_acc_wakeup(void);

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
uint32 mhi_pm_process_d_state_change(mhi_link_d_state_t d_oldState, mhi_link_d_state_t d_newState, mhi_state_t mhiState, boolean wakeup_pending);

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
void mhi_pm_wakeup_host(void);

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
void mhi_pm_wakeup_received_fromHost(void);

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
uint32 mhi_pm_enter_sleep_mode(mhi_pm_sleep_mode_t sleep_mode);

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
uint32 mhi_pm_exit_sleep_mode(void);

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
void mhi_pm_set_d_state(mhi_link_d_state_t d_state);

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
mhi_link_d_state_t mhi_pm_get_d_state(void);

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
boolean mhi_pm_get_device_wake_gpio_state(void);

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
void mhi_trigger_device_wake_intr(void);

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
void mhi_pm_core_wakeup(void);

#endif
