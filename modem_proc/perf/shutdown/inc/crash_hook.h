#pragma once
/**
   @file crash_hook.h 
   @author anandj 
   @brief Crash handler for shutdown module in perf.mpss component 
    
   Copyright (c) 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/**
 * @brief RC initialization framework entry point for crash hook module 
 * @warning This *MUST BE* invoked by RCInit in Group 0 *AFTER* Integrity 
 *          checker (if present)
 * @see perf/shutdown/build/shutdown.scons 
 */
extern void perf_shutdown_crash_hook_rcinit (void);
