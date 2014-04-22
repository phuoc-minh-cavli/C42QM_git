/* =========================================================================

DESCRIPTION
  Implementation of stubs for memheap to compile the memheap and amssheap on mba
  image

Copyright (c) 2014,2017, 2019      by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================ */

/* =========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.10/services/utils/src/memheap_mba_stub.c#3 $ $DateTime: 2019/01/24 01:39:00 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
02/03/17  ps     Added stub APIs for optimizations on compiler 7.3.02 
09/15/14  rks    Added stubs for err_SaveFatal2 and err_SaveFatal1
09/19/12  mg     Created original file
============================================================================ */
#include "comdef.h"  
#include "msg_diag_service.h"
#include "qurt.h"

//#ifdef FEATURE_GENERIC_ERR_FATAL_MSG
/* Used to replace all ERR_FATAL message strings.
 * Reduces code size by eliminating other const strings used for errors.
 */
const char err_generic_msg[] = "Error Fatal, parameters: %d %d %d";
const char err_generic_msg_dynamic[] = "Error Fatal, check coredump.err.aux_msg";
//#endif


#if defined (FEATURE_SAVE_DEBUG_TRACE)
void err_Fatal_internal3 (const msg_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3)
{
   return;
}

void err_Fatal_internal2 (const msg_const_type* const_blk, uint32 code1, uint32 code2)
{
   return;
}

void err_Fatal_internal1 (const msg_const_type* const_blk, uint32 code1)
{
  return;
}

void err_Fatal_internal0 (const msg_const_type* const_blk)
{
	return;
}

/* Changes required for instruction optimization for compiler 7.3.02 */
void err_Fatal_internal0_pcrelR0(const msg_const_type* const_blk)
{
   return;
}

void err_Fatal_internal1_pcrelR0(const msg_const_type* const_blk, uint32 code1)
{
  return;
}

void err_Fatal_internal2_pcrelR0(const msg_const_type* const_blk, uint32 code1, uint32 code2)
{
   return;
}

void err_Fatal_internal3_pcrelR0(const msg_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3)
{
   return;
}


#else

void err_Fatal3 (const err_FatalParam_type* const_blk, uint32 code1, uint32 code2, uint32 code3)
{
   return;
}

void err_Fatal2 (const msg_const_type* const_blk, uint32 code1, uint32 code2)
{
   return;
}

void err_Fatal1 (const msg_const_type* const_blk, uint32 code1)
{
   return;
}

void err_Fatal0 (const err_FatalParam_type* const_blk)
{
	return;
}
#endif

void msg_v2_send_3 ( const msg_v2_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2, uint32 xx_arg3)
{
	return;
}

void msg_v3_send_3 ( const msg_v3_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2, uint32 xx_arg3)
{
  return;
}
/* Changes required for instruction optimization for compiler 7.3.02 */
void msg_v2_send_3_pcrelR0( const msg_v2_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2, uint32 xx_arg3)
{
   return;
}

void msg_v3_send_3_pcrelR0( const msg_v3_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2, uint32 xx_arg3)
{
  return;
}

int qurt_thread_get_priority (qurt_thread_t threadid)
{
	return 0;
}
void msg_v4_send_3 ( const msg_v4_const_type * xx_msg_const_ptr, uint32 xx_arg1,uint32 xx_arg2,
    uint32 xx_arg3)
{
	return;
}

void msg_v4_send_3_pcrelR0( const msg_v4_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2, uint32 xx_arg3)
{
  return;
}

int qurt_thread_set_priority (qurt_thread_t threadid, unsigned short newprio)
{
	return 0;
}
