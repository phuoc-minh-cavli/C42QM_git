#ifndef _USLEEP_OS_H_
#define _USLEEP_OS_H_
/*==============================================================================
  FILE:         uSleep_os.h

  OVERVIEW:     Provides main OS API intface functions and types for uSleep

  DEPENDENCIES: None
 
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.mpss/3.10/power/uSleep/inc/uSleep_os.h#2 $
$DateTime: 2019/05/28 10:11:02 $
==============================================================================*/
#include "comdef.h"

/*==============================================================================
                            DEFINITIONS & TYPES
 =============================================================================*/
/* Enumeration to indicate the Idle Mode Type */
typedef enum
{
  USLEEP_OS_IDLE_MODE_DEFAULT   = 0, /* Configure for single threaded sleep */
  USLEEP_OS_IDLE_MODE_OVERRIDE,      /* Configure for low overhead sleep */
  USLEEP_OS_IDLE_MODE_HOLD_OFF,      /*  Configure for busy wait during idle */ 

  USLEEP_OS_IDLE_MODE_NUM_STATES
} uSleepOS_idleModeType;

/**
 * uSleep_idle_entry_ptr_type
 *
 * @brief Function pointer type for different behaviors when processor goes
 *        idle during uSleep interaction.
 */
typedef uint32 (*uSleep_idle_entry_ptr_type)(void);

/*==============================================================================
                            FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * uSleepOS_configIdleMode
 * 
 * @brief Configures the behavior of the sleep thread for cacheable or uncacheable modes 
 *  
 * @param newIdleMode:  Configured power mode 
 */
void uSleepOS_configIdleMode(uSleepOS_idleModeType  newIdleMode);

/** 
 * uSleepOS_PMIPerformer
 * 
 * @brief Island mode version of the PMI interrupt handler routine.  
 */
void uSleepOS_PMIPerformer(void);

/** 
 * uSleepOS_performLPM
 * 
 * @brief Main uSleep function in island mode that will enter low power modes.
 */
uint32 uSleepOS_performLPM(void);

/** 
 * uSleepOS_performLPMExit
 * 
 * @brief Handles exiting LPR modes 
 */
void uSleepOS_performLPMExit(void);

/** 
 * uSleep_initialize
 * 
 * @brief Main initialization for uSleep
 */
void uSleepOS_initialize(void);

/** 
 * uSleepOS_initializeTask
 * 
 * @brief Initialize the uSleep tasks
 */
void uSleepOS_initializeTask(void);

/**
 * uSleepOS_perform
 * 
 * @brief Entry function for performing low power modes in uSleep.
 *
 * @return Potential error code (unused as of now)
 */
uint32 uSleepOS_perform(void);


/**
 * sleepOS_holdoffNoMode
 *
 * @brief This function simply returns and does not perform any type of low 
 *        power mode. Usually this is during system initialization or in a
 *        situation where we don't want target to enter any low power modes
 *        at all (e.g. debug, bring up). 
 */
uint32 uSleepOS_holdoffNoMode(void);



#endif /* _USLEEP_OS_H_ */

