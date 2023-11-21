#ifndef RCINIT_POSIX_H
#define RCINIT_POSIX_H
/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_posix.h
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
$Header: //components/rel/core.mpss/3.10/api/debugtools/rcinit_posix.h#1 $
$DateTime: 2018/06/26 03:18:17 $
$Change: 16502877 $
$Author: pwbldsvc $
===============================================================================*/

#include "dll_api.h"                                                             /**< MUST PRECEDE CLIENT DEFINITION OF DLL_API_EXPORT */

#if defined(DLL_API_BUILD_RCINIT)                                                /**< REQUIRED TO BE DECLARED IN TECH AREA SOURCE MODULES (SU/VU VISIBLE) */
#undef  DLL_API_EXPORT_RCINIT                                                    /**< REDEFINITION FOR THIS SPECIFIC MODULE */
#define DLL_API_EXPORT_RCINIT DLL_API_EXPORT_HELPER                              /**< CONTROL OVER DLL SYMBOL VISIBILITY BY EXPORT */
#else
#undef  DLL_API_EXPORT_RCINIT                                                    /**< REDEFINITION FOR THIS SPECIFIC MODULE */
#define DLL_API_EXPORT_RCINIT DLL_API_IMPORT_HELPER                              /**< CONTROL OVER DLL SYMBOL VISIBILITY BY EXPORT */
#endif

#include "pthread.h"
#include "sched.h"

#include "rcxh.h"
#include "rcecb.h"
#include "rcesn_posix.h"
#include "rcevt_posix.h"
#include "rcinit.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/** @addtogroup rcinit_api @{ */

/**
API, Routine maps NHLOS task context identifier to RCINIT_INFO Opaque handle
@param[in] NHLOS task context identifier
@returns
NULL -- Error
RCINIT_INFO -- Opaque handle representing an RCINIT_INFO object
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCINIT_INFO rcinit_lookup_info_posix(pthread_t const tid);

/**
API, Routine maps NHLOS task context identifier to RCINIT_INFO Opaque handle
@param[in] NHLOS task context identifier
@returns
RCINIT_GROUP -- Enumeration represents startup group for NHLOS tid
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_EXPORT_RCINIT RCINIT_GROUP rcinit_lookup_group_posix(pthread_t const tid);

/**
API, Routine maps NHLOS task name to RCINIT_INFO Opaque handle
@param[in] NULL terminated string, string identifier for task context
@returns
pthread_t -- NHLOS task context identifier
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT pthread_t rcinit_lookup_posix(RCINIT_NAME const name);

/**
API, Routine maps RCINIT_INFO Opaque handle to NHLOS task context identifier
@param[in] Opaque handle representing an RCINIT_INFO object
@returns
pthread_t -- NHLOS task context identifier
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT pthread_t rcinit_lookup_posix_info(RCINIT_INFO const info);

// MAP Different NONHLOS Interfaces to RCINIT Priority Concept
// RCINIT priority database is 0 = low, 255 = high

#define RCINIT_MAP_PRIO_POSIX(x)       (x)

/** @} end_addtogroup rcinit_api */

#if defined(__cplusplus)
}
#endif

#endif
