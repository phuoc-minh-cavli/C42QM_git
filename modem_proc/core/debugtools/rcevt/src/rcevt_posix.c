/** vi: tw=128 ts=3 sw=3 et :
@file rcevt_posix.c
@brief This file contains the API for the Run Control Framework, API 3.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.mpss/3.10/debugtools/rcevt/src/rcevt_posix.c#1 $
$DateTime: 2018/06/26 03:18:17 $
$Change: 16502877 $
$Author: pwbldsvc $
===============================================================================*/

#if !defined(RCINIT_EXCLUDE_KERNEL_POSIX)

#define DLL_API_BUILD_RCINIT

#include "stringl/stringl.h"

#include "rcevt_posix.h"
#include "tms_rcevt.h"

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
#include "tracer.h"
#include "rcinit_tracer_swe.h"
#endif

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
#include "msg.h"
#endif

#if !defined(RCINIT_EXCLUDE_ULOG_SWEVT)
#endif

#if 0

/**
INTERNAL
*/
void rce_mutex_init_posix(rce_mutex_p mutex_p)
{
   if (mutex_p)
   {
      pthread_mutexattr_t attr;

      secure_memset((void*)mutex_p, 0, sizeof(*mutex_p));

      if (pthread_mutexattr_init(&attr)) {;}
      if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL)) {;}
      if (pthread_mutex_init(&mutex_p->mutex_posix, &attr)) {;}
      if (pthread_mutexattr_destroy(&attr)) {;}
   }
}

/**
INTERNAL
*/
void rce_mutex_lock_posix(rce_mutex_p mutex_p)
{
   if (mutex_p)
   {
      if (pthread_mutex_lock(&mutex_p->mutex_posix)) {;}
   }
}

/**
INTERNAL
*/
void rce_mutex_unlock_posix(rce_mutex_p mutex_p)
{
   if (mutex_p)
   {
      if (pthread_mutex_unlock(&mutex_p->mutex_posix)) {;}
   }
}

#endif

#endif
