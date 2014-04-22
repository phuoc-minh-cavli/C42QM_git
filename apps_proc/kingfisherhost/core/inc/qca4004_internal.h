/*****************************************************************************
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/


#ifndef __QCA4004_INTERNAL_H__
#define __QCA4004_INTERNAL_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "qurt.h"

#ifdef QCA4004_DEBUG_PRINT_DISABLE
#define QCA4004_DEBUG_LOG(...)
#else
#include "qcli.h"
#include "qcli_api.h"

extern QCLI_Group_Handle_t qcli_qca4004_driver_handle;
#define QCA4004_DEBUG_LOG(fmt, ...) QCLI_Printf(qcli_qca4004_driver_handle, fmt , ##__VA_ARGS__)
#endif

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/
#define QCA4004_ENTER_CRITICAL()	do { __asm("cpsid i"); } while(0)
#define QCA4004_EXIT_CRITICAL()		do { __asm("cpsie i"); } while(0)

#define QCA4004_LOCK(__lock__)		((qurt_mutex_lock_timed_ext(&(__lock__), QURT_TIME_WAIT_FOREVER)) == QURT_EOK)
#define QCA4004_UNLOCK(__lock__)	do { qurt_mutex_unlock(&(__lock__)); } while(0)

#endif
