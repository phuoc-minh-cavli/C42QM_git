#ifndef _CSD_DBG_MESSAGES_H
#define _CSD_DBG_MESSAGES_H

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.


when        who        what, where, why
--------    ---        ------------------------------------------------------
09/25/2012 unni	       Added CSD_MSG and CSD_MSG_NO_FUNC for variable argument
                        and function name appended logging
04/20/2010 asheeshk    initial draft

============================================================================*/

#include "../inc/csd_diag_service_msg.h"

#define CSD_DIAG_GENERAL           CSD_GENERAL
#define CSD_DIAG_DEFAULT           CSD_DIAG_GENERAL

/* CSD Message Priorities */
#define CSD_DIAG_PRIO_LOW      CSD_PRIO_LOW
#define CSD_DIAG_PRIO_MEDIUM   CSD_PRIO_MEDIUM
#define CSD_DIAG_PRIO_HIGH     CSD_PRIO_HIGH
#define CSD_DIAG_PRIO_ERROR    CSD_PRIO_ERROR
#define CSD_DIAG_PRIO_FATAL    CSD_PRIO_FATAL
#define CSD_DIAG_PRIO_DEBUG    CSD_PRIO_DEBUG

/* This is the macro for messages with no parameters and a specific priority */
#define CSD_MSG_ARG_0(xx_prio, xx_fmt) \
        {                        \
          CSD_DIAG_MSG_ARG_0(xx_prio, xx_fmt); \
        }                        \

/* This is the macro for messages with 1 parameters and a specific priority */
#define CSD_MSG_ARG_1(xx_prio, xx_fmt, xx_arg1) \
        {                        \
          CSD_DIAG_MSG_ARG_1(xx_prio, xx_fmt, xx_arg1); \
        }                        \

/* This is the macro for messages with 2 parameters and a specific priority */
#define CSD_MSG_ARG_2(xx_prio, xx_fmt, xx_arg1, xx_arg2) \
        {                        \
          CSD_DIAG_MSG_ARG_2(xx_prio, xx_fmt, xx_arg1, xx_arg2); \
        }                        \

/* This is the macro for messages with 3 parameters and a specific priority */
#define CSD_MSG_ARG_3(xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
        {                        \
          CSD_DIAG_MSG_ARG_3(xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3); \
        }                        \

/*
  This is the macro for messages with 4 parameters and a specific priority 
  msg_send_var() uses var arg list supported by the compiler.
*/
#define CSD_MSG_ARG_4(xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                      xx_arg4) \
        {                        \
          CSD_DIAG_MSG_ARG_4(xx_prio, xx_fmt, xx_arg1, xx_arg2, \
                          xx_arg3, xx_arg4); \
        }                        \

/*
  This is the macro for messages with 5 parameters and a specific priority 
  msg_send_var() uses var arg list supported by the compiler.
*/
#define CSD_MSG_ARG_5(xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                      xx_arg4,xx_arg5) \
        {                        \
          CSD_DIAG_MSG_ARG_5(xx_prio, xx_fmt, xx_arg1, xx_arg2, \
                          xx_arg3, xx_arg4, xx_arg5); \
        }                        \

/*
  This is the macro for messages with 6 parameters and a specific priority 
  msg_send_var() uses var arg list supported by the compiler.
*/
#define CSD_MSG_ARG_6(xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                      xx_arg4,xx_arg5,xx_arg6) \
        {                        \
          CSD_DIAG_MSG_ARG_6(xx_prio, xx_fmt, xx_arg1, xx_arg2, \
                          xx_arg3, xx_arg4, xx_arg5,xx_arg6); \
        }                        \

/*
  This is the macro for messages with 7 parameters and a specific priority 
  msg_send_var() uses var arg list supported by the compiler.
*/
#define CSD_MSG_ARG_7(xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                      xx_arg4,xx_arg5,xx_arg6,xx_arg7) \
        {                        \
          CSD_DIAG_MSG_ARG_7(xx_prio, xx_fmt, xx_arg1, xx_arg2, \
                          xx_arg3, xx_arg4, xx_arg5,xx_arg6,xx_arg7); \
        }                        \

/*
  This is the macro for messages with 8 parameters and a specific priority 
  msg_send_var() uses var arg list supported by the compiler.
*/
#define CSD_MSG_ARG_8(xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                      xx_arg4,xx_arg5,xx_arg6,xx_arg7,xx_arg8) \
        {                        \
          CSD_DIAG_MSG_ARG_8(xx_prio, xx_fmt, xx_arg1, xx_arg2, \
                          xx_arg3, xx_arg4, xx_arg5,xx_arg6,xx_arg7,xx_arg8); \
        }                        \

/*
  This is the macro for messages with 9 parameters and a specific priority 
  msg_send_var() uses var arg list supported by the compiler.
*/
#define CSD_MSG_ARG_9(xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                      xx_arg4,xx_arg5,xx_arg6,xx_arg7,xx_arg8,xx_arg9) \
        {                        \
          CSD_DIAG_MSG_ARG_9(xx_prio, xx_fmt, xx_arg1, xx_arg2, \
                          xx_arg3, xx_arg4, xx_arg5,xx_arg6,xx_arg7,xx_arg8,xx_arg9); \
        }      
        
/* This is the macro for messages with 1 string arguments and
   a specific priority. 
*/
#define CSD_MSG_SPRINTF_ARG_1(xx_prio,xx_fmt,arg1) \
          CSD_DIAG_MSG_SPRINTF_ARG_1(xx_prio,xx_fmt,arg1)
            
/*
  This is the macro for messages with 2 string argumant and 
  a specific priority. 
*/
#define CSD_MSG_SPRINTF_ARG_2(xx_prio,xx_fmt,arg1,arg2) \
          CSD_DIAG_MSG_SPRINTF_ARG_2(xx_prio,xx_fmt,arg1,arg2)
/*
  This is the macro for messages with varaible arguments and
  a specific priority. 
*/

#define CSD_VA_NUM_ARGS(...) CSD_VA_NUM_ARGS_IMPL(, ##__VA_ARGS__,9,8,7,6,5,4,3,2,1,0)
#define CSD_VA_NUM_ARGS_IMPL(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,N,...) N
#define CSD_TOKENPASTE(x, y)  x ## y
#define CSD_TOKENPASTE2(x, y) CSD_TOKENPASTE(x, y)
#define CSD_MSG_ARG_x(num)  CSD_TOKENPASTE2(CSD_MSG_ARG_, num)

#define CSD_MSG( xx_prio, xx_fmt, ...) \
      CSD_MSG_ARG_x(CSD_VA_NUM_ARGS(__VA_ARGS__))(xx_prio, xx_fmt, ##__VA_ARGS__)

/*
  This is the macro for messages with varaible arguments and
  a specific priority. 
*/
#define CSD_MSG_NO_FUNC( xx_prio, xx_fmt, ...) \
          CSD_DIAG_MSG_SPRINTF( xx_prio, xx_fmt, ##__VA_ARGS__)
       
#endif//_CSD_DBG_MESSAGES

