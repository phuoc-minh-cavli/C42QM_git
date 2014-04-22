/**
@file tms_utils_msg.c
@brief This file contains logs related stuff.
*/
/*=============================================================================
Copyright (c) 2017 by Qualcomm Technologies, Inc.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.tx/6.0/debugtools/utils/src/tms_utils_msg.c#1 $
$DateTime: 2020/01/30 22:49:35 $
$Author: pwbldsvc $
===============================================================================*/
#include "tms_utils_msg.h"

/* F3 Flag */
unsigned int tms_diag_f3_flag = 0;

/* ULOG handles */
ULogHandle tms_low_ulog_handle = NULL;
ULogHandle tms_med_ulog_handle = NULL;
ULogHandle tms_high_ulog_handle = NULL;
ULogHandle tms_err_ulog_handle = NULL;

#ifdef TMS_MSG_ULOG

#define TMS_LOW_ULOG_MESSAGE
#define TMS_MED_ULOG_MESSAGE
#define TMS_HIGH_ULOG_MESSAGE
#define TMS_ERR_ULOG_MESSAGE

#ifdef TMS_LOW_ULOG_MESSAGE
#define TMS_LOW_ULOG_BUFFER_SIZE    ( 512 )
#endif  // TMS_LOW_ULOG_MESSAGE

#ifdef TMS_MED_ULOG_MESSAGE
#define TMS_MED_ULOG_BUFFER_SIZE    ( 512 )
#endif  // TMS_MED_ULOG_MESSAGE

#ifdef TMS_HIGH_ULOG_MESSAGE
#define TMS_HIGH_ULOG_BUFFER_SIZE   ( 1024 * 16 )
#endif  // TMS_HIGH_ULOG_MESSAGE

#ifdef TMS_ERR_ULOG_MESSAGE
#define TMS_ERR_ULOG_BUFFER_SIZE    ( 1024 )
#endif  // TMS_ERR_ULOG_MESSAGE

#endif  // TMS_MSG_ULOG

/**
 * @brief tms_utils_msg_init
 *
 * Initialize ULOG handles.
*/
void tms_utils_msg_init( void )
{
#ifdef TMS_MSG_DIAG_F3
  tms_diag_f3_flag = 1;
#endif  // TMS_MSG_DIAG_F3

#ifdef TMS_MSG_ULOG

#ifdef TMS_LOW_ULOG_MESSAGE
  if( NULL == tms_low_ulog_handle )
  {
    /* Initialize tms_low_ulog_handle */
    ULogFront_RealTimeInit( &tms_low_ulog_handle,
                            "TMS_LOW_ULOG",
                            TMS_LOW_ULOG_BUFFER_SIZE,
                            ULOG_MEMORY_LOCAL,
                            ULOG_LOCK_OS );
  }
#endif  // TMS_LOW_ULOG_MESSAGE

#ifdef TMS_MED_ULOG_MESSAGE
    if( NULL == tms_med_ulog_handle )
    {
      /* Initialize tms_med_ulog_handle */
      ULogFront_RealTimeInit( &tms_med_ulog_handle,
                              "TMS_MED_ULOG",
                              TMS_MED_ULOG_BUFFER_SIZE,
                              ULOG_MEMORY_LOCAL,
                              ULOG_LOCK_OS );
    }
#endif  // TMS_MED_ULOG_MESSAGE

#ifdef TMS_HIGH_ULOG_MESSAGE
    if( NULL == tms_high_ulog_handle )
    {
      /* Initialize tms_ulog_high_handle */
      ULogFront_RealTimeInit( &tms_high_ulog_handle,
                              "TMS_HIGH_ULOG",
                              TMS_HIGH_ULOG_BUFFER_SIZE,
                              ULOG_MEMORY_LOCAL,
                              ULOG_LOCK_OS );
    }
#endif  // TMS_HIGH_ULOG_MESSAGE

#ifdef TMS_ERR_ULOG_MESSAGE
    if( NULL == tms_err_ulog_handle )
    {
      /* Initialize tms_err_ulog_handle */
      ULogFront_RealTimeInit( &tms_err_ulog_handle,
                              "TMS_ERR_ULOG",
                              TMS_ERR_ULOG_BUFFER_SIZE,
                              ULOG_MEMORY_LOCAL,
                              ULOG_LOCK_OS );
    }
#endif  // TMS_ERR_ULOG_MESSAGE

#endif  // TMS_MSG_ULOG
}
