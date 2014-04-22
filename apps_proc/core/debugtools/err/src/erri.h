#ifndef ERRI_H
#define ERRI_H

/*===========================================================================

                    Error Handling Service Internal Header File

Description

Copyright (c) 2009,2018 by Qualcomm Technologies Incorporated.  All Rights Reserved.

$Header: //components/rel/core.tx/6.0/debugtools/err/src/erri.h#1 $

===========================================================================*/


#include "comdef.h"
#include "err.h"
#include "timetick.h"
#include "errlog.h"
 
#if defined(FEATURE_ERR_NV_LOG)
  #define ERR_MAX_LOG 20             /* NV stores 20 error logs */
#else
  #define ERR_MAX_LOG 20             /* This number can change */
#endif


extern coredump_type coredump;

  /* Maximum number of error logs to store per session */
  #ifndef ERR_DATA_MAX_LOGS
  #define ERR_DATA_MAX_LOGS 1
  #endif

  typedef uint64 err_data_flag_type;

  typedef struct {
    err_data_flag_type flag;
	  err_log_type nv_log;
    uint32 length;
  } err_data_log_type;


#ifndef TMS_UTILS_BUF_SZ_ZERO   
#define TMS_UTILS_BUF_SZ_ZERO       0
#endif

  typedef struct {
    err_data_flag_type reset_flag;
    err_data_log_type err_log[ERR_DATA_MAX_LOGS];
  } err_data_type;


/* Magic number that determine whether the data in uninitialized RAM has
 * already been saved to flash or not.
 */
#define ERR_DATA_MAGIC_NUMBER       (uint64)0x5252452041544144ULL
#define ERR_DATA_RESET_MAGIC_NUMBER (uint64)0xABCD06261974EA29ULL

#define ERR_LOOP_DELAY_USEC 10000
#define ERR_CLK_PAUSE_SMALL 500
#define ERR_CLK_PAUSE_KICK 100
/* The following values are based on ERR_LOOP_DELAY_USEC */
/* Actual frequency is loop_freq_value*ERR_LOOP_DELAY_USEC microseconds */
#define ERR_LOOP_DOG_FREQ 5 /* 50 ms */


/* Establish our default action.  This action will be the initial setting
  used if an error occurs before we are able to read NV.  It is also the
  default value returned by err_nv_efs if an NV setting is not available. */
#if defined (FEATURE_AUTO_DLOAD_ON_ERROR)
  #define ERR_DEFAULT_ACTION ERR_DLOAD
#elif defined (FEATURE_AUTO_RESET_ON_ERROR)
  #define ERR_DEFAULT_ACTION ERR_RESET
#else
  #define ERR_DEFAULT_ACTION ERR_NO_ACTION
#endif

boolean err_log_store ( word , const char* , boolean );
void err_nv_log_init ( void );
void err_pause_usec(uint32 usec);

#endif /* ERRI_H */
