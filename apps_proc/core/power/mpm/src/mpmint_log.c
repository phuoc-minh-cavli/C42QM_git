/*============================================================================
  FILE:         mpmint_log.c
  
  OVERVIEW:     This file implements the functions for logging mpm messages.

  NOTE:         Right now there are no low memory targets that are using vMPM
                driver. If that changes, we may want to restrict the log 
                usage for it.
 
                Copyright (c) 2012 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/

#include <stdarg.h>
#include "mpminti.h"
#include "CoreVerify.h"
#include "ULog.h"
#include "ULogFront.h"
#include "err.h"

/*===========================================================================
 *                            INTERNAL DEFINITIONS
 *===========================================================================*/

/**
 * log handle for mpm logs.
 */
static ULogHandle mpmint_log_handle = NULL;

/**
 * Size of the ULog buffer for mpm log.
 */
static unsigned int mpmint_log_size = 1024;


/*===========================================================================
 *                         GLOBAL FUNCTION DEFINITIONS
 *===========================================================================*/

/*
 * mpmint_log_init
 */
void mpmint_log_init( void )
{
  #if defined(USES_QNX) && defined(QNP_POWERMAN_PROC)
  ULOG_LOCK_TYPE log_lock = ULOG_LOCK_NONE;
  #else
  ULOG_LOCK_TYPE log_lock = ULOG_LOCK_OS;
  #endif

  if( !mpmint_log_handle )
  {
    /* Creating a log handle */
    CORE_VERIFY( ULOG_ERR_INITINCOMPLETE ==
                 ULogFront_RealTimeInit(&mpmint_log_handle, "MPM Driver log", 
                                        0, ULOG_MEMORY_LOCAL, log_lock));

    /* Log header setup */
    ULogCore_HeaderSet(mpmint_log_handle, 
                       "Content-Type: text/mpm-driver-1.0; title=MPM Driver");

    /* Allocating required memory */
    ULogCore_Allocate(mpmint_log_handle, mpmint_log_size);

    /* Enabling the log */
    ULogCore_Enable(mpmint_log_handle);
  }
  else
  {
    ERR_FATAL("Potential multiple initialization of mpm log", 0, 0, 0);
  }
}

/*
 * mpmint_log_message
 */
void mpmint_log_printf( int arg_count, const char *format, ... )
{
  va_list args;

  va_start(args, format);
  ULogFront_RealTimeVprintf(mpmint_log_handle, arg_count, format, args);
  va_end(args);
}
