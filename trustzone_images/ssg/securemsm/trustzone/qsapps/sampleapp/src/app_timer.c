// Copyright (c) 2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.

#include "IxErrno.h"
#include "cdefs.h"
#include "qsee_log.h"
#include "qsee_sync.h"
#include "qsee_timer.h"
#include <stdint.h>

int run_spin_test(void)
{
   unsigned long long uptime, uptime2;
   /* Test for a small selection of values */
   const uint32_t delay_ms[] = {2, 10, 100};
   size_t ndx;

   C_FOR_ARRAY(ndx, delay_ms){
      if(0 == (uptime = qsee_get_uptime())) {
         qsee_log(QSEE_LOG_MSG_ERROR, "[%d] qsee_get_uptime() failed", __LINE__);
         return -E_FAILURE;
      }

      if(0 != qsee_spin(delay_ms[ndx] * 1000)) {
         qsee_log(QSEE_LOG_MSG_ERROR, "[%d] qsee_spin() failed", __LINE__);
         return -E_FAILURE;
      }

      if(0 == (uptime2 = qsee_get_uptime())) {
         qsee_log(QSEE_LOG_MSG_ERROR, "[%d] qsee_get_uptime() failed", __LINE__);
         return -E_FAILURE;
      }

      // Check that we waited _at least_ the expected delay
      if(uptime2 < uptime + delay_ms[ndx]) {
         qsee_log(QSEE_LOG_MSG_ERROR, "[%d] qsee_spin(%u ms) failed, actual delay = %llu ms", 
                  __LINE__, delay_ms[ndx], uptime2 - uptime);
         return -E_FAILURE;
      }
   }
   return E_SUCCESS;
}