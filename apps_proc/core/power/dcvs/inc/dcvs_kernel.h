#ifndef DCVS_KERNEL_H
#define DCVS_KERNEL_H

/******************************************************************//**
 * @file dcvs_kernel.h
 *
 * @brief Kernel (OS) interface header file for Dynamic Clock and
 * Voltage Scaling(DCVS) service.
 *
 * Copyright (C) 2010 - 2011 by Qualcomm Technologies, Inc. All Rights Reserved.
 *
 *********************************************************************/
/*=======================================================================
                        Edit History
$Header: //components/rel/core.tx/6.0/power/dcvs/inc/dcvs_kernel.h#1 $
$DateTime: 2020/01/30 22:49:35 $

when       who     what, where, why
--------   ----    ---------------------------------------------------
04/26/11   ss      Fixing Q6 compiler warnings.
02/18/11   ss      Adding context based processing for DCVS.
03/30/10   ss      Adding interfaces DCVSKernelTicksToMsConvert and 
                   DCVSKernelStartUpDelay.
02/21/09   ss      Removing redundant definitions and interface 
                   declarations.
01/20/10   bjs     Initial check in.
========================================================================*/

#include "DALSys.h"
#include "dcvs_core.h"
#include "qurt_thread.h"

typedef void* DCVSKernelTimerHandle ;

typedef void (*DCVSKernelCallback)(DCVS_CONTEXT* pv);

typedef void (*dcvs_timer_cb)(DCVS_CONTEXT*);


/*********************************************************************
  @brief This function initializes the DCVS Kernel interface.
  Initializes the kernel layer for DCVS.

  @param context: The DCVS data context.

 *********************************************************************/
void DCVSKernelInitialize(DCVS_CONTEXT* context);

/******************************************************************//**
  @brief This function allows firing of delayed callback.
  Callback defined at cb is asynchronously fired after delay_ms 
  milliseconds.

  @param cb: The callback which is invoked after the delay.
  @param delay_ms: The delay in ms.
  @param context: The DCVS data context.

 *********************************************************************/
void DCVSKernelCallbackCreate(DCVSKernelCallback cb, uint32 delay_ms, DCVS_CONTEXT* context);

/******************************************************************//**
 * @brief Kernel interface to query processor utilization.
 * 
 *
 *********************************************************************/
void DCVSKernelIdleInfo(uint32* idle_override);


/**
  @brief DCVSKernelTicksToMsConvert
  
  This function is used to convert system ticks into ms . 

  @param id: the system ticks to be converted. 
  
  @return the ms value for the ticks passed in. 
      

*/
uint32 DCVSKernelTicksToMsConvert(uint32 sysTicks);


/**
  @brief DCVSKernelStartUpDelay
  
  This function is used to define the OS specific delay (in ms) 
  before starting the DCVS algorithm. 
  
  @return the ms value that is used as delay before the start of the
  DCVS algorithm.
      
*/
uint32 DCVSKernelStartUpDelay(void);


/**
   @brief DCVSKernelCriticalSectionEnter - This API is used for
   synchronization within the kernel layer. It is called before 
   entering a critical section. The implementation of this API 
   uses locking based on OS specific basis.
   
   @param  context: The The DCVS data context.

   @return : None.

*/
void DCVSKernelCriticalSectionEnter(DCVS_CONTEXT* context);


/**
   @brief DCVSKernelCriticalSectionExit - This API is used for
   synchronization within the kernel layer. It is called before 
   exiting a critical section.

   @param context: The The DCVS data context.

   @return : None.

*/
void DCVSKernelCriticalSectionExit(DCVS_CONTEXT* context);

/**
   @brief DCVSKernelSleepTickGet - Used to return the accumulated idle time of the system
   since the system is turned on. 
   @param context: The The DCVS data context.
   @return : The accumulated sleep ticks of the system.

*/
uint64 DCVSKernelSleepTickGet(DCVS_CONTEXT* context);

/**
   @brief DCVSKernelSystemTickGet - Used to return the the number of system ticks since the system is 
   turned on.
   @param context: The OAL The DCVS data context.
   @return : The accumulated system ticks.

*/
uint64 DCVSKernelSystemTickGet(DCVS_CONTEXT* context);

/**
   @brief DCVSKernelTimerCreate - This API is used to create a
   DCVS timer . DCVSKernelTimerStart is used to start the timer.

   @param cb: Callback function to be invoked after the timer
              expires.
   @param context: The The DCVS data context. 

   @return : Timer handle to the created DCVS timer.

*/
DCVSKernelTimerHandle DCVSKernelTimerCreate(DCVSKernelCallback cb, DCVS_CONTEXT* context);


/**
   @brief DCVSKernelTimerDestroy - This API is used to delete
   the DCVS timer. The DCVS timer handle would be invalid after 
   this call .
   @param Handle: Timer handle of DCVS timer to be deleted.
   @param context: The The DCVS data context.

   @return : TRUE : Handle succesfully deleted.
             FALSE : Handle could not be deleted.

*/
DALBOOL DCVSKernelTimerDestroy(DCVSKernelTimerHandle Handle, DCVS_CONTEXT* context);

/**
   @brief DCVSKernelTimerStart - Used to start the OS timer used for DCVS
   sampling .
    
   @param Handle : The timer handle used to start a DCVS timer .
   @param time_ms : Time duration information in ms. 
   @param context: The The DCVS data context. 
   @return : 
   TRUE: Timer started succesfully .
   FALSE: Timer couldnt be started. 

*/
DALBOOL DCVSKernelTimerStart(DCVSKernelTimerHandle Handle, uint32 time_ms, DCVS_CONTEXT* context); 

/**
   @brief DCVSKernelTimerStop - Used to stop the OS timer used for DCVS
   sampling .

   @param Handle : The timer handle used to start a DCVS timer .
   @param context: The The DCVS data context. 
   @return : void


*/
void DCVSKernelTimerStop(DCVSKernelTimerHandle Handle, DCVS_CONTEXT* context);

#endif /* DCVS_KERNEL_H */
