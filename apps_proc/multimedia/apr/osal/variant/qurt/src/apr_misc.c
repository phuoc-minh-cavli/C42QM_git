/*
  Copyright (C) 2009-2010 Qualcomm Technologies Incorporated.
  All rights reserved.
  QUALCOMM Proprietary/GTDR.

  $Header: //components/rel/audio.tx/2.0/apr/osal/variant/qurt/src/apr_misc.c#1 $
  $Author: pwbldsvc $
*/

//#include "rex.h"
#include "apr_errcodes.h"
#include "apr_misc.h"
#include "timetick.h"

APR_INTERNAL int32_t apr_misc_sleep (
  uint64_t time_ns
)
{
//  rex_sleep( ( uint32 ) ( time_ns / 1000000 ) );
    /* Rex API doesn't handle less than a millisecond precision. */

  return APR_EOK;
}

/* Returns time in processor ticks */
APR_INTERNAL uint32_t apr_misc_timeticks ( void )
{
   timetick_type curr_time;
   curr_time = timetick_get();
   return  (uint32_t)curr_time;
}

APR_INTERNAL uint32_t apr_misc_time_usec ( void )
{
   timetick_type curr_time;
   curr_time = timetick_cvt_from_sclk( timetick_get(), T_USEC );
   return  (uint32_t)curr_time;
}

APR_INTERNAL uint32_t apr_misc_get_elapsed_time_usec(uint32_t start)
{
  timetick_type curr_time;
  curr_time = timetick_get_elapsed((timetick_type)start, T_USEC);
  return curr_time;
}

APR_INTERNAL uint32_t apr_misc_get_elapsed_timeticks(uint32_t start)
{
  timetick_type curr_time;
  curr_time = timetick_get_elapsed((timetick_type)start, T_SCLK);
  return curr_time;
}


