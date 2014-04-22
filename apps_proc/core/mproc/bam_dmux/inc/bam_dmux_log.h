/*=============================================================================

                            BAM DMUX OS

 BAM Data Multiplexer logging

 Copyright  2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/bam_dmux/inc/bam_dmux_log.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/16   rv      Initial version.
===========================================================================*/

#ifndef BAM_DMUX_LOG_H
#define BAM_DMUX_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include <stdarg.h>
#include "stdio.h"
#include "comdef.h"
#include "err.h"
#include "ULogFront.h"

/*=============================================================================

                         MACRO DECLARATIONS

=============================================================================*/
#define BAM_DMUX_LOG_DBG_0(args, fmt, ...) \
  ULogFront_RealTimePrintf(bam_dmux_log_dbg_handle, \
                           2, \
                           BAM_DMUX_LOG_TAG ":%d:0x%X: " fmt, __LINE__, \
                           bam_dmux_os_get_thread_id())

#define BAM_DMUX_LOG_DBG_N(args, fmt, ...) \
  ULogFront_RealTimePrintf(bam_dmux_log_dbg_handle, \
                           args + 2, \
                           BAM_DMUX_LOG_TAG ":%d:0x%X: " fmt, __LINE__, \
                           bam_dmux_os_get_thread_id(), __VA_ARGS__)

#define BAM_DMUX_LOG_DBG_1(args, fmt, ...) BAM_DMUX_LOG_DBG_N(args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_DBG_2(args, fmt, ...) BAM_DMUX_LOG_DBG_N(args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_DBG_3(args, fmt, ...) BAM_DMUX_LOG_DBG_N(args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_DBG_4(args, fmt, ...) BAM_DMUX_LOG_DBG_N(args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_DBG_5(args, fmt, ...) BAM_DMUX_LOG_DBG_N(args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_DBG_6(args, fmt, ...) BAM_DMUX_LOG_DBG_N(args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_DBG_7(args, fmt, ...) BAM_DMUX_LOG_DBG_N(args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_DBG_8(args, fmt, ...) BAM_DMUX_LOG_DBG_N(args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_DBG_9(args, fmt, ...) BAM_DMUX_LOG_DBG_N(args, fmt, __VA_ARGS__)

#define BAM_DMUX_LOG_DBG(args, fmt, ...) BAM_DMUX_LOG_DBG_##args(args, fmt, __VA_ARGS__)

#define BAM_DMUX_LOG_0(level, args, fmt, ...) \
  ULogFront_RealTimePrintf(bam_dmux_log_info_handle, \
                           2, \
                           level ":" BAM_DMUX_LOG_TAG ":%d:0x%X: " fmt, __LINE__, \
                           bam_dmux_os_get_thread_id())

#define BAM_DMUX_LOG_N(level, args, fmt, ...) \
  ULogFront_RealTimePrintf(bam_dmux_log_info_handle, \
                           args + 2, \
                           level ":" BAM_DMUX_LOG_TAG ":%d:0x%X: " fmt, __LINE__, \
                           bam_dmux_os_get_thread_id(), __VA_ARGS__)

#define BAM_DMUX_LOG_1(level, args, fmt, ...) BAM_DMUX_LOG_N(level, args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_2(level, args, fmt, ...) BAM_DMUX_LOG_N(level, args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_3(level, args, fmt, ...) BAM_DMUX_LOG_N(level, args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_4(level, args, fmt, ...) BAM_DMUX_LOG_N(level, args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_5(level, args, fmt, ...) BAM_DMUX_LOG_N(level, args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_6(level, args, fmt, ...) BAM_DMUX_LOG_N(level, args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_7(level, args, fmt, ...) BAM_DMUX_LOG_N(level, args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_8(level, args, fmt, ...) BAM_DMUX_LOG_N(level, args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_9(level, args, fmt, ...) BAM_DMUX_LOG_N(level, args, fmt, __VA_ARGS__)

#define BAM_DMUX_LOG(level, args, fmt, ...) BAM_DMUX_LOG_##args(level, args, fmt, __VA_ARGS__)

#define BAM_DMUX_LOG_INFO(args, fmt, ...) BAM_DMUX_LOG("INFO", args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_WRN(args, fmt, ...)  BAM_DMUX_LOG("WRN", args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_ERR(args, fmt, ...)  BAM_DMUX_LOG("ERR", args, fmt, __VA_ARGS__)
#define BAM_DMUX_LOG_FATAL(args, fmt, ...) \
  do { \
    BAM_DMUX_LOG("FATAL", args, fmt, __VA_ARGS__);\
    ERR_FATAL(BAM_DMUX_LOG_TAG " :%d:0x%X: " fmt, __LINE__, bam_dmux_os_get_thread_id(), 0); \
  } while(0)

#define BAM_DMUX_LOG_RAW_DBG(args, fmt, ...) \
  ULogFront_RealTimePrintf(bam_dmux_log_dbg_handle, args, fmt, __VA_ARGS__)

#define BAM_DMUX_LOG_RAW_INFO(args, fmt, ...) \
  ULogFront_RealTimePrintf(bam_dmux_log_info_handle, args, fmt, __VA_ARGS__)

#define BAM_DMUX_LOG_RAW_WRN(args, fmt, ...) \
  ULogFront_RealTimePrintf(bam_dmux_log_info_handle, args, fmt, __VA_ARGS__)

#define BAM_DMUX_LOG_RAW_ERR(args, fmt, ...) \
  ULogFront_RealTimePrintf(bam_dmux_log_info_handle, args, fmt, __VA_ARGS__)

#define BAM_DMUX_LOG_RAW_FATAL(args, fmt, ...) \
  do { \
    ULogFront_RealTimePrintf(bam_dmux_log_info_handle, args, fmt, __VA_ARGS__); \
    ERR_FATAL(BAM_DMUX_LOG_TAG " :%d:0x%X: %s", __LINE__, bam_dmux_os_get_thread_id(), fmt); \
  } while(0)

/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/

extern uint32 bam_dmux_os_get_thread_id(void);

/*=============================================================================

                     PUBLIC VARIABLE DECLARATIONS

=============================================================================*/
extern ULogHandle bam_dmux_log_dbg_handle;
extern ULogHandle bam_dmux_log_info_handle;

/*=============================================================================

                    PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================
  FUNCTION  bam_dmux_log_init
=============================================================================*/
/**
 * Initializes logging.
 *
 */
/*===========================================================================*/
void bam_dmux_log_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BAM_DMUX_LOG_H */
