#ifndef _QSOCKET_REX_H_
#define _QSOCKET_REX_H_
#include "rex.h"
/******************************************************************************
  @file    qsocket_rex.h
  @brief   OS specific extensions for QSocket

  DESCRIPTION
  This is a REX OS specific OS extension to the qsocket interface

  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

 *******************************************************************************/

/*===========================================================================
  CONST  QSOCKET_THREAD_DEFAULT_SIG
===========================================================================*/
/*!
@brief

  The default mask used for signalling with rex threads. If the thread
  wants the interface to use another signal mask it may set it using
  qsocket_thread_set_attr

*/
/*=========================================================================*/
#define QSOCKET_THREAD_DEFAULT_SIG           0x80000000

/*===========================================================================
  FUNCTION  qsocket_thread_set_attr
===========================================================================*/
/*!
@brief

  This function sets signalling attributes required by the socket API
  in order to effectively provide signalling functionality.

  It is safe for a thread to call this function multiple times. If different
  signal masks are provided then the ones used by the interface will change
  on the next call.

  The interface will default to use QSOCKET_THREAD_DEFAULT_SIG if not
  specifically set differently by this function.

@param[in]   sigs      The signal mask which will be used by the socket
                       interface for the current thread.

@return
  0 on success, negative error code on failure.

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int qsocket_thread_set_attr(rex_sigs_type sigs);

/*===========================================================================
  FUNCTION  qsocket_thread_exit
===========================================================================*/
/*!
@brief

  This function cleans up any thread specific information which might
  be stored in a previous call to qsocket_thread_set_attr. 
  All threads which have set thread specific attributes must call 
  this function before exiting to avoid any memory leaks. 

  It is safe to call this function even if no thread specific information
  exists. 

@return
  None

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void qsocket_thread_exit(void);

#endif
