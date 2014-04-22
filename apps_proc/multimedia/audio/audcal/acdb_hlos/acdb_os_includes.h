/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.
 * All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
 */

#ifndef __ACDB_OS_INCLUDES_H__
#define __ACDB_OS_INCLUDES_H__

#include "mmdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#define WIN32_LIB
#ifndef WIN32_LIB
#include "stringl/stringl.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "msg.h"

#define RTC_PRIO_LOW      (MSG_MASK_0)
#define RTC_PRIO_MEDIUM   (MSG_MASK_1)
#define RTC_PRIO_HIGH     (MSG_MASK_2)
#define RTC_PRIO_ERROR    (MSG_MASK_3)
#define RTC_PRIO_FATAL    (MSG_MASK_4)
#define RTC_PRIO_DEBUG    (MSG_MASK_5)

/* 
  Use DIAG 2.0 APIs MSG_1, MSG_2...to reduce memory usage.
  sprintf is highly unrecommended to use since they are very expensive and
  not Qshrinked
*/   
#define ACDB_MSG_ARG_0(prio,fmt)                                                \
  MSG(MSG_SSID_ACDB_GENERAL,prio,fmt)
  
#define ACDB_MSG_ARG_1(prio,fmt,arg1)                                           \
  MSG_1(MSG_SSID_ACDB_GENERAL,prio,fmt,arg1)

#define ACDB_MSG_ARG_2(prio,fmt,arg1,arg2)                                      \
  MSG_2(MSG_SSID_ACDB_GENERAL,prio,fmt,arg1,arg2)

#define ACDB_MSG_ARG_3(prio,fmt,arg1,arg2,arg3)                                 \
  MSG_3(MSG_SSID_ACDB_GENERAL,prio,fmt,arg1,arg2,arg3)

#define ACDB_MSG_ARG_4(prio,fmt,arg1,arg2,arg3,arg4)                            \
  MSG_4(MSG_SSID_ACDB_GENERAL,prio,fmt,arg1,arg2,arg3,arg4)

#define ACDB_MSG_ARG_5(prio,fmt,arg1,arg2,arg3,arg4,arg5)                       \
  MSG_5(MSG_SSID_ACDB_GENERAL,prio,fmt,arg1,arg2,arg3,arg4,arg5)

#define ACDB_MSG_ARG_6(prio,fmt,arg1,arg2,arg3,arg4,arg5,arg6)                  \
  MSG_6(MSG_SSID_ACDB_GENERAL,prio,fmt,arg1,arg2,arg3,arg4,arg5,arg6)

#define ACDB_MSG_ARG_7(prio,fmt,arg1,arg2,arg3,arg4,arg5,arg6,arg7)             \
  MSG_7(MSG_SSID_ACDB_GENERAL,prio,fmt,arg1,arg2,arg3,arg4,arg5,arg6,arg7)

#define ACDB_MSG_ARG_8(prio,fmt,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8)        \
  MSG_8(MSG_SSID_ACDB_GENERAL,prio,fmt,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8)
  
#define ACDB_MSG_ARG_9(prio,fmt,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9)   \
  MSG_9(MSG_SSID_ACDB_GENERAL,prio,fmt,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9)
  
#define ACDB_VA_NUM_ARGS(...) ACDB_VA_NUM_ARGS_IMPL(, ##__VA_ARGS__,9,8,7,6,5,4,3,2,1,0)
#define ACDB_VA_NUM_ARGS_IMPL(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,N,...) N
#define ACDB_TOKENPASTE(x, y)  x ## y
#define ACDB_TOKENPASTE2(x, y) ACDB_TOKENPASTE(x, y)
#define ACDB_MSG_ARG_x(num)  ACDB_TOKENPASTE2(ACDB_MSG_ARG_, num)
  
#define ACDB_DEBUG_LOG(fmt,...)     \
  ACDB_MSG_ARG_x(ACDB_VA_NUM_ARGS(__VA_ARGS__))(0xFF, fmt, ##__VA_ARGS__)
  
#define RTC_MSG(prio,fmt,...)       \
  ACDB_MSG_ARG_x(ACDB_VA_NUM_ARGS(__VA_ARGS__))(prio, fmt, ##__VA_ARGS__)
  
#ifdef __cplusplus
}
#endif

#define ACDB_MALLOC     malloc
#define ACDB_MEM_FREE   free
#ifdef WIN32_LIB
#define ACDB_MEM_CPY(dst,dst_size,src,src_size)    memcpy(dst,src,src_size)
#else
#define ACDB_MEM_CPY(dst,dst_size,src,src_size)    memscpy((void*)(dst),dst_size,(void*)(src),src_size)
#endif


#endif /* __ACDB_OS_INCLUDES_H__ */
