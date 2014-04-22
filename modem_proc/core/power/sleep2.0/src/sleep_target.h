#ifndef SLEEP_TARGET_H
#define SLEEP_TARGET_H
/*==============================================================================
  FILE:         sleep_target.h
  
  OVERVIEW:     This file provides the externs and declarations needed for
                target specific code

  DEPENDENCIES: None

                Copyright (c) 2012-2014 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.10/power/sleep2.0/src/sleep_target.h#3 $
$DateTime: 2018/12/28 00:36:13 $
==============================================================================*/
#include "sleepi.h"
#include "DALStdDef.h"
#include "DDIChipInfo.h"
#include "rsc_common.h"
#include "rsc.h"

/*==============================================================================
                             TYPE DEFINITIONS
 =============================================================================*/

/**
 * @brief Used to enumerate l2 lprm configuration
 */
typedef enum{
  L2_TCM_RET,       /* Config processor SS & L2 for L2 ret & TCM ret          */
  L2_TCM_NORET,     /* Config processor SS & L2 for L2 non-ret & TCM non-ret  */
  L2_NORET,         /* Config processor SS & L2 for L2 non-ret & TCM ret      */
  L2_NUM_RET_MODES  /* Total number of retention modes                        */
} l2_lprm;

/**
 *  @brief target config data returned by sleepTarget_initialize()
 */
typedef struct 
{
  uint32                  entry_ticks;    /* Overhead Entering Sleep, without
                                             lpr entry time*/
  uint32                  exit_ticks;     /* Overhead Exiting Sleep, without 
                                             lpr exit time*/
  uint32                  init_delay_ms;  /* Delay before sleepOS_performLPM in 
                                             milliseconds*/
  DalChipInfoFamilyType   chip_family;    /* Chip family architecture*/
  DalChipInfoVersionType  chip_ver;       /* Chip architecture version */
  uint32                  l2_mask;        /* Mask for L2 Cache Power Ctl */
  uint32                  tcm_mask;       /* Mask for TCM Power Ctl */
} sleep_target_config;

/*==============================================================================
                             EXTERNAL FUNCTIONS
 =============================================================================*/
/**
 * @brief sleepTarget_initialize
 * 
 * Perform target-specific initialization of the sleep subsystem.
 */
void sleepTarget_initialize(void);

/**
 * @brief sleepTarget_initializeLPR 
 *  
 * Do target-specific initialization for the /sleep/lpr node.
 * This initialization will happen before the node is available
 * to the outside world.
 */
void sleepTarget_initializeLPR(void);

/**
 * @brief sleepTarget_enableSleepTimer
 *  
 * Set the timer match value hardware register to expire after
 * ticks amount of time. 
 * 
 * Function implementaion is defined by type of unerlying processor
 * Q6    : Converts ticks to sclks and programs IDLT
 * Others: Programs the SLEEP QTimer
 *
 * Any software overhead that is required before and after the actual 
 * sleep timer expires will have to be taken into account.
 *
 * @param wakeup_ticks : Absolute time in 19.2MHz ticks when wakeup 
 *                       should be programmed.
 *  
 * @return Actual wakeup time programed
 */
uint64 sleepTarget_enableSleepTimer(uint64 wakeup_ticks);

/**
 * @brief sleepTarget_disableSleepTimer
 *  
 * Disables any Timer Hardware that was programmed to wake-up
 * the processor.
 * 
 * Function implementaion is defined by type of unerlying processor
 * Q6    : Converts ticks to sclks and programs IDLT
 * Others: Programs the SLEEP QTimer
 *
 * 
 */
void sleepTarget_disableSleepTimer(void);

/**
 * @brief sleep function to disable deferrable timers
 *
 */
void sleepTarget_deferTimers(void);

/**
 * @brief sleep function to enable deferrable timers
 *
 */
void sleepTarget_undeferTimers(void);

/**
 * @brief sleep function to enable DOG AUTOKICK
 *
 */
void sleepTarget_enableWatchdog(void);

/**
 * @brief sleep function to disable DOG AUTOKICK
 *
 */
void sleepTarget_disableWatchdog(void);


/**
 * @brief Gets the current CPU family & version
 *  
 * @param chipFamily: Return value with family of chip
 * @param chipVer: Return value with version of chip
 */
void sleepTarget_getCPUArchitecture(DalChipInfoFamilyType  *chip_family, 
                                    DalChipInfoVersionType *chip_ver);

/**
 * sleepTarget_setRPMAssistedMode
 *
 * @brief Sets an indicator if the RPM LPR enter/exit functions have been 
 * executed as part of the current synth mode 
 *  
 * This function should only be called from the RPM LPR functions. 
 *  
 * @param value: TRUE or FALSE to indicate if in RPM assisted mode 
 */
void sleepTarget_setRPMAssistedMode(boolean value);

/**
 * sleepTarget_getRPMAssistedMode
 *
 * @brief Retreives the value previously set by the "set" function that 
 * indicates if an RPM synth mode was selected for entry. 
 *
 * This function is mainly useful to the CPU VDD LPR to determine if we are 
 * in an RPM assised synth mode 
 *  
 * @return: TRUE or FALSE to indicate if we have entered RPM assisted mode
 */
boolean sleepTarget_getRPMAssistedMode(void);

/**
 * sleep_target_get_rpm_stat_ptr
 *
 * @brief Gets the memory pointer to the shared memory location
 * of the RPM statistics data 
 *  
 * @return: Pointer to data
 */
void *sleepTarget_getRPMStatisticsAddress(void);
//All below added by Harshal
/**
 * sleepTarget_performInitialLPRExit
 *
 * @brief Performs any target specfic exit requirements prior to executing 
 *        the LPRM exit routines.  It is expected to call this function as
 *        early as possible in the sleep exit from power collapse routines. 
 */
void sleepTarget_performInitialLPRExit(void);

/** Harshal :
* q6LPMConfig_setRpmhHandshake
*
* @brief Enables or disables the child - parent RSC handshake
*
* @param performHandshake: Requested RSC handshake state
*/
void q6LPMConfig_setRpmHandshake(boolean performHandshake);

/**Harshal:
 * sleepTarget_setRscLowPowerMode
 *
 * @brief Controls the power mode that RSC will enter
 *  
 * @param type:     RSC type to configure (child or parent) 
 * @param coreMode: Power mode to setup
 *  
 * @return RSC results
 */
rsc_result_t sleepTarget_setRscLowPowerMode(rsc_type type, rsc_low_power_mode_type coreMode);

/**
* sleepTarget_getRscTimestamp
*
* @brief Gets the timestamp data of the last RSC power mode execution
*
* @param type:       RSC type to configure (child or parent)
* @param timeStamp:  Timestamp structure poitner to be populated with data
*/
void sleepTarget_getRscTimestamp(rsc_type               type,
                                 rsc_profile_unit       *timeStamp);


/**
 * sleepTarget_setRpmHandshake
 *
 * @brief Controls the RSC-RPM handshakeing
 *
 * @param performHandshake: Boolean value to enable or disable the RPM handshake
 *                          at power collapse
 *
 * @return RSC results
 */
rsc_result_t sleepTarget_setRpmHandshake(boolean performHandshake);




/**
 * sleepTarget_isInterruptPending
 *
 * @brief Checks if a particular interrupt is pending 
 *  
 * @param intNumber: Interrupt vector to check 
 *  
 * @return: TRUE if interrupt is pending, else FALSE
 */
boolean sleepTarget_isInterruptPending(uint32 intNumber);

/**
 * sleepTarget_setDefaultPowerMode
 *
 * @brief Sets a default power mode to enter should the sub-system enter the idle state. 
 *        This mode should be the most basic and quickest to enter.  Typically it will be
 *        clock gating.
 *  
 * @note When un-setting the default mode, the function will take into account any required setting 
 *       changes for the currently selected actual power mode to enter.  The intention of this function
 *       is to only be called from the ATS logic.
 *  
 * @param enable: Boolean to enable or disable the default mode 
 */
void sleepTarget_setDefaultPowerMode(boolean enable);

#endif //SLEEP_TARGET_H

