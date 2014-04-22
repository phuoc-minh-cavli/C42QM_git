#ifndef __SHUTDOWN_H__
#define __SHUTDOWN_H__

/*===========================================================================
 * FILE:         shutdown.h
 *
 * DESCRIPTION:  Maximize chances of a clean shutdown right before STM mode
 *
 * Copyright (c) 2015, 2016 Qualcomm Technologies Incorporated.
 ===========================================================================*/

/*==========================================================================

  EDIT HISTORY FOR MODULE

  $Header: //components/rel/perf.mpss/2.1.2.0/shutdown/inc/shutdown.h#1 $

  when       who     what, where, why
  --------   ---     --------------------------------------------------------
  06/30/15   ao      Initial revision
  ===========================================================================*/
 
typedef enum {
    PERF_SHUTDOWN_YES = 0,
    PERF_SHUTDOWN_LIKELY,
    PERF_SHUTDOWN_NO,
    PERF_SHUTDOWN_UNKNOWN
} perf_shutdown_safe_t;

perf_shutdown_safe_t perf_shudown_is_safe(void);

#endif
