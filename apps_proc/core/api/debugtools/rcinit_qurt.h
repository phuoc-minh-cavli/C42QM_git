#ifndef RCINIT_QURT_H
#define RCINIT_QURT_H
/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_qurt.h
@brief This file contains the API for the Run Control Framework, API 2.1
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
$Header: //components/rel/core.tx/6.0/api/debugtools/rcinit_qurt.h#1 $
$DateTime: 2020/01/30 22:49:35 $
$Change: 22400605 $
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

#include "qurt.h"
#ifdef RCINIT_QDI_SUPPORT
#include "qurt_qdi_driver.h"
#endif /* RCINIT_QDI_SUPPORT */

#include "stringl/stringl.h"

#include "rcxh.h"
#include "rcecb.h"
#include "rcevt_qurt.h"
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
RCINIT_INFO rcinit_lookup_info_qurttask(qurt_thread_t tid);

/**
API, Routine maps NHLOS task context identifier to RCINIT_INFO Opaque handle
@param[in] NHLOS task context identifier
@returns
RCINIT_GROUP -- Enumeration represents startup group for NHLOS tid
*/
RCINIT_GROUP rcinit_lookup_group_qurttask(qurt_thread_t tid);

/**
API, Routine maps NHLOS task name to RCINIT_INFO Opaque handle
@param[in] NULL terminated string, string identifier for task context
@returns
qurt_thread_t -- NHLOS task context identifier
*/
qurt_thread_t rcinit_lookup_qurttask(RCINIT_NAME name);

/**
API, Routine maps RCINIT_INFO Opaque handle to NHLOS task context identifier
@param[in] Opaque handle representing an RCINIT_INFO object
@returns
qurt_thread_t -- NHLOS task context identifier
*/
qurt_thread_t rcinit_lookup_qurttask_info(RCINIT_INFO info);

#ifdef RCINIT_QDI_SUPPORT
/**
API, Multi PD Accessors
*/
#define RCINIT_DEVICE_PATH_LEN      32                                           /**< Device Prefix */
#define RCINIT_DEVICE_NAME_LEN      (QURT_MAX_NAME_LEN*2)                        /**< Specialized Device Name */
#define RCINIT_DEVICE_ASID_MAX      32                                           /**< QURT_MAX_ASIDS */

#define RCINIT_DEVICE_METHOD_0      (QDI_PRIVATE+0)                              /**< Specialized Method Call */
#define RCINIT_DEVICE_METHOD_1      (QDI_PRIVATE+1)                              /**< Specialized Method Call */

#define RCINIT_DEVICE               "/rcinit/"                                   /**< Always Use Manifest Constant */

typedef int RCINIT_DEVICE_HANDLE;

typedef enum { RCINIT_DEVICE_STATUS_SUCCESS = 0, RCINIT_DEVICE_STATUS_ERROR = -1 } RCINIT_DEVICE_STATUS;

/**
API, rcinit_device_init
@param[in] RCINIT_NAME NULL Terminated Device Name
@return
RCINIT_DEVICE_HANDLE -- Opaque Device Handle
*/
DLL_API_WARN_UNUSED_RESULT DLL_API_NONNULL((1))
DLL_API_STATIC_INLINE RCINIT_DEVICE_HANDLE rcinit_device_init(RCINIT_NAME const name)
{
   RCINIT_DEVICE_HANDLE rc = RCINIT_DEVICE_STATUS_ERROR;

   if (RCINIT_DEVICE_NAME_LEN > strnlen(name, RCINIT_DEVICE_NAME_LEN))
   {
      char device_path[RCINIT_DEVICE_PATH_LEN + RCINIT_DEVICE_NAME_LEN];

      strlcpy(device_path, RCINIT_DEVICE, sizeof(device_path));
      strlcat(device_path, name, sizeof(device_path));

      rc = qurt_qdi_open(device_path);
   }

   return rc;
}

/**
API, rcinit_device_destroy
@param[in] RCINIT_DEVICE_HANDLE Opaque Device Handle
@return
RCINIT_DEVICE_STATUS -- Error Indication
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_STATIC_INLINE RCINIT_DEVICE_STATUS rcinit_device_destroy(RCINIT_DEVICE_HANDLE const handle)
{
   return (0 <= qurt_qdi_close(handle)) ? RCINIT_DEVICE_STATUS_SUCCESS : RCINIT_DEVICE_STATUS_ERROR;
}

/**
API, rcinit_device_method_0
@param[in] RCINIT_DEVICE_HANDLE Opaque Device Handle
@return
RCINIT_DEVICE_STATUS -- Error Indication
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_STATIC_INLINE RCINIT_DEVICE_STATUS rcinit_device_method_0(RCINIT_DEVICE_HANDLE const handle)
{
   return (0 <= qurt_qdi_handle_invoke(handle, RCINIT_DEVICE_METHOD_0)) ? RCINIT_DEVICE_STATUS_SUCCESS : RCINIT_DEVICE_STATUS_ERROR;
}

/**
API, rcinit_device_method_1
@param[in] RCINIT_DEVICE_HANDLE Opaque Device Handle
@return
RCINIT_DEVICE_STATUS -- Error Indication
*/
DLL_API_WARN_UNUSED_RESULT
DLL_API_STATIC_INLINE RCINIT_DEVICE_STATUS rcinit_device_method_1(RCINIT_DEVICE_HANDLE const handle)
{
   return (0 <= qurt_qdi_handle_invoke(handle, RCINIT_DEVICE_METHOD_1)) ? RCINIT_DEVICE_STATUS_SUCCESS : RCINIT_DEVICE_STATUS_ERROR;
}

#endif /* RCINIT_QDI_SUPPORT */


// MAP Different NONHLOS Interfaces to RCINIT Priority Concept
// RCINIT priority database is 0 = low, 255 = high

#define RCINIT_MAP_PRIO_QURT(x)        (x)

/** @} end_addtogroup rcinit_api */

#if defined(__cplusplus)
}
#endif

#endif
