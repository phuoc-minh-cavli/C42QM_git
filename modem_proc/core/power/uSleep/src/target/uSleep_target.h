#ifndef _USLEEP_TARGET_H_
#define _USLEEP_TARGET_H_
/*==============================================================================
  FILE:         uSleep_target.h

  OVERVIEW:     Types & definitions target specific requirements for uSleep

  DEPENDENCIES: None
  
                Copyright (c) 2016-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/src/target/uSleep_target.h#2 $
$DateTime: 2019/05/28 10:11:02 $
==============================================================================*/
#include "comdef.h"
#include "uSleep_lpri.h"
#include "rsc.h"
#include "rsc_common.h"
#include "DDIChipInfo.h"


/*==============================================================================
                             TYPE DEFINITIONS
 =============================================================================*/
/* Target config data returned by sleepTarget_initialize() */
typedef struct 
{
  DalChipInfoFamilyType   chip_family;    /* Chip family architecture*/
  DalChipInfoVersionType  chip_ver;       /* Chip architecture version */
  uint32                  l2_mask;        /* Mask for L2 Cache Power Ctl */
  uint32                  tcm_mask;       /* Mask for TCM Power Ctl */
} uSleep_target_config;


/**
 * RPM_master_stats
 *
 * @brief RPM data fields for shutdown & wakeup
 */ 
typedef struct RPM_master_stats_s
{
  /* Bitmask of active cores - core 0 = bit 0 */
  uint32 active_cores;

  /* number of times all cores have power collapsed */
  uint32 num_shutdowns;

  /* timestamp of last shutdown of core */
  uint64 shutdown_req;

  /* timestamp of last master wakeup ind  */
  uint64 wakeup_ind; 
  
  /* timestamp of last bring up req  */   
  uint64 bringup_req;

  /* timestamp of last bring up ack */
  uint64 bringup_ack;

  /* 1=scheduled, 0=rude */
  uint32 wakeup_reason;

  /* time taken (in ticks) for the last W->S transition */
  uint32 last_sleep_transition_duration;

  /* time taken (in ticks) for the last S->W transition */
  uint32 last_wake_transition_duration; 

  uint32 reserved[19];
}RPM_master_stats;
/*==============================================================================
                            FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * uSleepTarget_getCurrentModeLatency
 * 
 * @brief Gets the current modes latency values
 * 
 * @return Currently configuraiton modes latency values
 */
const uSleep_lpr_latency *uSleepTarget_getCurrentModeLatency(void);

/**
 * uSleepTarget_configBasePowerMode
 * 
 * @brief Configures the base power mode once at init (or at every island entry if transitions 
 *        are supported) 
 */
void uSleepTarget_configBasePowerMode(void);

/**
 * uSleepTarget_enterIdle
 * 
 * @brief Waits for the system to become active and performs the 
 *        pre-configured power mode.
 *  
 * @note  Clock gating should be configured before calling this function 
 */
void uSleepTarget_enterIdle(void);

/**
 * uSleepTarget_initialize
 *  
 * @brief Performs any target specific initializations 
 */
void uSleepTarget_initialize(void);

/**
 * uSleepTarget_programWakeupTimer
 *  
 * @brief Programs the wakeup time 
 *  
 * @param qTimerWakeup: Absolute value of the desired wakeup time for the local qTimer 
 * @param AONWakeup:    Absolute value of the desired wakeup time for the AON timer in local qtimer 
 *                      ticks 
 */
void uSleepTarget_programWakeupTimer(uint64 qTimerWakeup, uint64 AONWakeup);

/**
 * uSleepTarget_disableWakeupTimer
 *  
 * @brief Disables the previously programed wakeup time 
 */
void uSleepTarget_disableWakeupTimer(void);

/**
 * uSleepTarget_enterLowPowerMode
 *  
 * @brief Enters a low power mode 
 *  
 * @param mode:       Low Power mode to enter 
 * @param wakeupTime: Absolute value of the desired wakeup time 
 */
void uSleepTarget_enterLowPowerMode(uSleep_LPR *mode, uint64 wakeupTime, boolean reEntrant);

/**
 * uSleepTarget_exitLowPowerMode
 *  
 * @brief Exits the given low power mode 
 */
void uSleepTarget_exitLowPowerMode(uSleep_LPR *mode);

/**
 * uSleepTarget_setPMIHandler
 *  
 * @brief This should be called on the uImage transtion, while still 
 *        in normal mode, to point to a uImage PMI handler function. 
 */
void uSleepTarget_setPMIHandler(void);

/** 
 * uSleepTarget_enableIslandDebug
 * 
 * @brief Allows T32 to remain connected while in island mode
 *  
 * @note This function must be called outside of single threaded mode.
 *        
 * @param enable: Flag to inidate if island should be setup to allow 
 *                debug. 
 */
void uSleepTarget_enableIslandDebug(boolean enable);

/**
 * uSleepTarget_setRpmHandshake
 *
 * @brief Controls the SPM-RPM handshakeing 
 *  
 * @param performHandshake: Boolean value to enable or disable the RPM handshake 
 *                          at power collapse 
 *  
 * @note Assumes the SPM driver has already been initialized 
 *  
 * @return SPM results
 */
rsc_result_t uSleepTarget_setRpmHandshake(boolean performHandshake);

/**
 * uSleepTarget_setSpmLowPowerMode
 *
 * @brief Controls the power mode that SPM will enter
 *  
 * @param coreMode: Power mode to setup
 *  
 * @return SPM results
 */

rsc_result_t uSleepTarget_setRscLowPowerMode(rsc_type type, rsc_low_power_mode_type coreMode);

/**
 * uSleep_target_get_rpm_stat_ptr
 *
 * @brief Gets the memory pointer to the shared memory location
 * of the RPM statistics data 
 *  
 * @return: Pointer to data
 */
void *uSleepTarget_getRPMStatisticsAddress(void);

#endif /* USLEEP_TARGET_H */

