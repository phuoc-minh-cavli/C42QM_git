#ifndef MSG_DIAG_DLL_SERVICE_H
#define MSG_DIAG_DLL_SERVICE_H

/*!
  @ingroup diag_message_service
  @file msg_diag_service.h 
  @brief
  All the declarations and definitions necessary to support the reporting 
  of messages for DLL clients. 
  @note
  Please do NOT include this file directly. Clients 
  need to include msg.h to use Diagnostic Debug Message Service.
  
  @par Some implementation details of Diag Debug Messages:
  The diagnostic extended message packet is defined to be processed as 
  efficiently as possible in the caller's context.  Therefore, the packet 
  is divided into the following logical blocks that are aligned without 
  declaring the structure as PACK:

  
@par
   1. Header
@par
   2. Constant variable length data (format string and filename string, etc).
@par
   3. Arguments ("Variable" variable-length data)
@par
   The data is delivered in this order in the packet to simplify the runtime 
   processing of each message.  All constant data is handled in DIAG task 
   context, leaving the caller's task to only process variable data at runtime.
@par
   The phone will never process the constant data directly, except to copy 
   format and filename strings. The above holds true for all MSG 2.0 macros, 
   except the SPRINTF macros.
*/

/*
Copyright (c) 2015-2018 Qualcomm Technologies,Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*/

/*==========================================================================
                        EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.10/api/services/msg_diag_dll_service.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/12/15   sr      Created file
===========================================================================*/

#include "coreapi_variation.h"

#include "qw.h"                /* for qword */
#include "msg_qsr.h"


#define MSG(xx_ss_id, xx_ss_mask, xx_fmt) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_dll (&xx_msg_v2_const); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
This macro is used if clients want to pass in a time-stamp, @a xx_ts.
In this case, Diagnostic service does NOT introduce timestamp into the message,
but uses the timestamp provided by the client.
*/
#define MSG_TS(xx_ss_id, xx_ss_mask, xx_fmt, xx_ts) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_ts_dll (&xx_msg_const, xx_ts); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0)
                       /* lint +e717 */

/*!
Macro for messages with 1 integer parameter.
*/
#define MSG_1(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_1_dll (&xx_msg_v2_const, (uint32) (xx_arg1)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */


/*!
Macro for messages with 2 integer parameters.
*/
#define MSG_2(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_2_dll (&xx_msg_v2_const, (uint32)(xx_arg1), (uint32)(xx_arg2)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                      /* lint +e717 */

/*!
This is the macro for messages with 3 integer parameters.
*/
#define MSG_3(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_3_dll (&xx_msg_v2_const, (uint32) (xx_arg1), (uint32) (xx_arg2), \
                                 (uint32) (xx_arg3)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
Macro for messages with 3 integer parameters??
*/
#define MSG_3_SAVE(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, do_sav) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_3_dll (&xx_msg_v2_const, (uint32) (xx_arg1), (uint32) (xx_arg2), \
                                 (uint32) (xx_arg3)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
This is the macro for messages with 4 integer parameters. In this case the 
function called needs to have more than 4 parameters so it is going to be a 
slow function call.  So for this case the  msg_send_var() uses var arg list 
supported by the compiler.
*/
#define MSG_4(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_var_dll (&xx_msg_v2_const, (uint32)(4), (uint32) (xx_arg1), \
               (uint32) (xx_arg2), (uint32) (xx_arg3), (uint32) (xx_arg4)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for messages with 5 integer parameters. msg_send_var() 
  uses var arg list supported by the compiler.
*/
#define MSG_5(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_var_dll(&xx_msg_v2_const, (uint32)(5), (uint32)(xx_arg1), \
               (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                  (uint32)(xx_arg5)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for messages with 6 integer parameters. msg_send_var() 
  uses var arg list supported by the compiler.
*/
#define MSG_6(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_var_dll (&xx_msg_v2_const, (uint32)(6), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for messages with 7 integer parameters. msg_send_var() 
  uses var arg list supported by the compiler.
*/
#define MSG_7(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_var_dll (&xx_msg_v2_const, (uint32)(7), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0)                                                             \
                       /* lint +e717 */

/*!
  This is the macro for messages with 8 integer parameters. msg_send_var() 
  uses var arg list supported by the compiler.
*/
#define MSG_8(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_var_dll (&xx_msg_v2_const, (uint32)(8), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for messages with 9 integer parameters. msg_send_var() 
  uses var arg list supported by the compiler.
*/
#define MSG_9(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8, xx_arg9) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_var_dll (&xx_msg_v2_const, (uint32)(9), (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8), \
                                   (uint32)(xx_arg9)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*@}*/ /* End group MSG2.0 macros */


/*!
@ingroup diag_message_service
@name MSG_SPRINTF_* macros
These macros are used for sprintf-style messages. Please use discretion while 
using these macros, because the string expansion happens in the client-context.
As opposed to the MSG_* macros that allow only integer arguments in addition
to a format_string, these macros allow float and string arguments, with the 
added cost.
@par
msg_sprintf() uses var arg list supported by the compiler. These macros are 
to be used when xx_fmt is passed as a literal.
*/
/*@{*/ /* starting group MSG_SPRINTF_* macros */
/*!
  This is the macro for sprintf-style messages with 1 parameter.msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
*/
#define MSG_SPRINTF_1(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for sprintf messages with 2 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
*/
#define MSG_SPRINTF_2(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for sprintf messages with 3 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
*/
#define MSG_SPRINTF_3(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for sprintf messages with 4 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
*/
#define MSG_SPRINTF_4(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */
                                
/*!
  This is the macro for sprintf messages with 5 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
*/
#define MSG_SPRINTF_5(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for sprintf messages with 6 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
*/
#define MSG_SPRINTF_6(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5), \
               (uint32)(xx_arg6)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for sprintf messages with 7 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
*/
#define MSG_SPRINTF_7(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6, \
                                                    xx_arg7 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5), \
               (uint32)(xx_arg6),  (uint32)(xx_arg7)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for sprintf messages with 8 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
*/
#define MSG_SPRINTF_8(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6, \
                                                    xx_arg7, xx_arg8 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5), \
               (uint32)(xx_arg6),  (uint32)(xx_arg7), \
               (uint32)(xx_arg8)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for sprintf messages with 3 parameters. This differs from 
  MSG_SPRINTF_3, in that here xx_fmt can be passed as a variable, instead of
  as a literal.
*/
#define MSG_SPRINTF_FMT_VAR_3(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST_FMT_VAR (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const, (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*@}*/ /* End group MSG_SPRINTF_* macros */

/*---------------------------------------------------------------------------
  The following MACROs are for LEGACY diagnostic messages support.  
---------------------------------------------------------------------------*/

/*!
@name Legacy MSG macros
@ingroup diag_message_service
The following MACROs are for LEGACY (a.k.a MSG 1.0) diagnostic messages support.
The message levels are controlled by selecting the proper macro:
@par
MSG_FATAL  for  fatal
@par
MSG_ERROR for  error
@par
MSG_HIGH  for  high
@par
MSG_MED  for   medium
and
@par
MSG_LOW for  low.
@par
These macros come under "Legacy" subsystem, and do not allow 
filtering by specific functional sub-systems. The further use of these is 
discouraged. Clients are recommended to use the MSG 2.0 macros that allow 
them to specify a sub-system to group their messages.

@param[in]  x_fmt Format string for message (printf style). Note, this parameter
must be a string literal (e.g. "Tuned to %lx"), not a variable. This is because
 the value is compiled into ROM, and can clearly not be a dynamic data type.
@param[in] a     1st parameter for format string
@param[in] b     2nd parameter for format string
@param[in] c     3rd parameter for format string

@dependencies 
  msg_init() must be called prior to these macros.
  This macro, is multiple C expressions, and cannot be used as if it
  is a single expression.

@sideeffects
  On the target hardware, the file and format strings are placed in ROM.
*/
/*@{*/ /* start group Legacy MSG macros */
#define MSG_FATAL(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_FATAL, x_fmt, a, b, c)

#define MSG_ERROR(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_ERROR, x_fmt, a, b, c)

#define MSG_HIGH(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_HIGH, x_fmt, a, b, c)

#define MSG_MED(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_MED, x_fmt, a, b, c)

#define MSG_LOW(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_LOW, x_fmt, a, b, c)

#define MSG_FATAL_NO_SAVE(x_fmt, a, b, c) MSG_FATAL (x_fmt, a, b, c)

#define MSG_ERROR_NO_SAVE(x_fmt, a, b, c) MSG_ERROR (x_fmt, a, b, c)

#define MSG_HIGH_NO_SAVE(x_fmt, a, b, c)  MSG_HIGH (x_fmt, a, b, c)

#define MSG_MED_NO_SAVE(x_fmt, a, b, c)   MSG_MED (x_fmt, a, b, c)

#define MSG_LOW_NO_SAVE(x_fmt, a, b, c)   MSG_LOW (x_fmt, a, b, c)
/*@}*/  /* End group Legacy MSG macros */


/*! 
@cond DOXYGEN_BLOAT
*/
/*
Legacy stop-gap solutions to the need for string transport in MSG services.
note: Please do not use these, these are defined to nothing.
*/
#define MSG_STR_ERROR(x_fmt, s, a, b, c)
#define MSG_STR_HIGH(x_fmt, s, a, b, c)
#define MSG_STR_MED(x_fmt, s, a, b, c)
#define MSG_STR_LOW(x_fmt, s, a, b, c)
/*! 
@endcond
*/

/* -------------------------------------------------------------------------
   Function Definitions
   ------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

/*!
   @ingroup diag_message_service
   @brief 
   Initialize the diagnostic debug message reporting service.

   @dependencies 
   Can only be called by one task. Currently this is called by TMC.

   @sideeffects
   Sets the counts to 0, and throws away any buffered messages. 

   @return
   None
   
   @sa 
   None
*/
   void msg_init (void);

/*!
   @ingroup diag_message_service
   @brief
   This will build a new style diagnostic Message with no parameters. 

   @warning
   Do not call directly; use macro MSG

   @param[in] *xx_msg_const_ptr  static constant constructed using 
   the different xx_msg_const macros

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   msg_send_1, msg_send_2, msg_send_3, msg_send_var
*/
  void msg_send (const msg_const_type * xx_msg_const_ptr);

/*!
   @ingroup diag_message_service
   @brief
   This will build a new style diagnostic Message with 1 parameter. 

   @warning
   Do not call directly; use macro MSG_1.

   @param[in] *xx_msg_const_ptr  static constant constructed using 
   the different xx_msg_const macros
   @param[in] xx_arg1            integer parameter for the message

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   msg_send, msg_send_2, msg_send_3, msg_send_var
*/
  void msg_send_1 (const msg_const_type * xx_msg_const_ptr, uint32 xx_arg1);

/*!
   @ingroup diag_message_service
   @brief
   This will build a new style diagnostic Message with 2 parameters. 

   @warning
   Do not call directly; use macro MSG_2.

   @param[in] *xx_msg_const_ptr  static constant constructed using 
   the different xx_msg_const macros
   @param[in] xx_arg1            integer parameter 1 for the message
   @param[in] xx_arg2            integer parameter 2 for the message

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   msg_send, msg_send_1, msg_send_3, msg_send_var
*/  
  void msg_send_2 (const msg_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2);

/*!
   @ingroup diag_message_service
   @brief
   This will build a new style diagnostic Message with 3 parameters. 

   @warning
   Do not call directly; use macro MSG_3.

   @param[in] *xx_msg_const_ptr  static constant constructed using 
   the different xx_msg_const macros
   @param[in] xx_arg1            integer parameter 1 for the message
   @param[in] xx_arg2            integer parameter 2 for the message
   @param[in] xx_arg3            integer parameter 3 for the message

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   msg_send, msg_send_1, msg_send_2, msg_send_var
*/
  void msg_send_3 (const msg_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2, uint32 xx_arg3);

/*!
@ingroup diag_message_service
   @brief
   This will build a new style diagnostic Message with variable number of(4 to 9) 
   parameters.  

   @warning
   Do not call directly; use macro MSG_4 ..MSG_9.

   @param[in] *xx_msg_const_ptr            static constant constructed using 
   the different xx_msg_const macros
   @param[in] num_args                     number of parameters

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   msg_send, msg_send_1, msg_send_2, msg_send_3
*/
  void msg_send_var (const msg_const_type * xx_msg_const_ptr, uint32 num_args,
    ...);

/*!
@ingroup diag_message_service
  @brief
  This will build a message sprintf diagnostic Message with variable number
  (1 to 8) of parameters.

  @warning
  Do not call directly; use macro MSG_SPRINTF_ (1..8) macros 

  @param[in] *const_blk          static constant constructed using 
  the different xx_msg_const macros
   
   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   None
*/
  void msg_sprintf(const msg_const_type * const_blk,...);

/*!
@ingroup diag_message_service
  @brief
  This function is similar to msg_sprintf function, expect that it takes
  additional parameters : version and void *. 

  @warning
  Do not call directly; use macro MSG_SPRINTF_ (1..8)_EXT macros 

  @param[in] *const_blk          static constant constructed using 
  the different xx_msg_const macros
  Based on the version value, void* can be interpreted as follows-
  Version      Pointer
     0          NULL
     1          uint32*
 
  In future, if there is a need for F3 macros to take additional parameters,
  a new version ID can be added and void* can be interpreted differently
  as per the requirements.
   
   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   None
*/
  void msg_sprintf_ext (uint32 version, void * reserved, uint32 num_args, const msg_const_type * const_blk, ...);
/*!
@ingroup diag_message_service
   @brief
   This will build a message with no parameter, and allow client to pass in 
   own timestamp.

   @warning
   Limited usage: only for Native Debug messages mechanisms like WM RETAILMSG 
   etc that need their timestamps preserved. Do not call directly, use 
   MSG_TS macro.

   @param[in] *const_blk          static constant constructed using 
   the different xx_msg_const macros
   @param[in] timestamp           Client timestamp
   
   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   None
*/
  void msg_send_ts(const msg_const_type *const_blk, uint64 timestamp);

/*!
@ingroup diag_message_service
   @brief
  This function is similar to msg_send_ts function, expect that it takes
  additional parameters : version and void *. 

   @warning
   Limited usage: only for Native Debug messages mechanisms like WM RETAILMSG 
   etc that need their timestamps preserved. Do not call directly, use 
   MSG_TS macro.

   @param[in] *const_blk          static constant constructed using 
   the different xx_msg_const macros
   Based on the version value, void* can be interpreted as follows-
    Version      Pointer
       0          NULL
       1          uint32*
   @param[in] timestamp           Client timestamp
   
   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   None
*/
void msg_send_ts_ext (const msg_const_type * const_blk, uint32 version, void *parameter, uint64 timestamp);

/*!
@ingroup diag_message_service
   @brief
   Saves the F3 message to the RAM buffer.  Also called from the ERR_FATAL
   macro.

   @param[in] *const_blk          static constant constructed using 
   the different xx_msg_const macros
   @param[in] xx_arg1            integer parameter 1
   @param[in] xx_arg2            integer parameter 2
   @param[in] xx_arg3            integer parameter 3
   @param[in] *msg               Message to be stored


   @dependencies
   None

   @return
   None

   @sideeffects
   None

   @sa
   None
*/
void msg_save_3(const msg_const_type* const_blk,
                uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3,
                msg_ext_store_type* msg);
/*!
@ingroup diag_message_service
   @brief
   Sends the desired f3 message using msg_send_3, in addition to invoking 
   err_put_log. This reduces one function call for top level macro.

   @warning
   Do not call directly, use MSG_ERR_LOG macro.

   @param[in] *const_blk          static constant constructed using 
   the different xx_msg_const macros
   @param[in] code1              integer parameter 1
   @param[in] code2              integer parameter 2
   @param[in] code3              integer parameter 3
   
   @dependencies
   None

   @return
   None

   @sideeffects
   None

   @sa
   None
*/
void msg_errlog_3 (const msg_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3);

/*!
@ingroup diag_message_service
   @brief
   Sends the desired f3 message using msg_send_2, in addition to invoking 
   err_put_log. This reduces one function call for top level macro.

   @warning
   Do not call directly, use MSG_ERR_LOG macro.

   @param[in] *const_blk          static constant constructed using 
   the different xx_msg_const macros
   @param[in] code1              integer parameter 1
   @param[in] code2              integer parameter 2
   
   @dependencies
   None

   @return
   None

   @sideeffects
   None

   @sa
   None
*/
void msg_errlog_2 (const msg_const_type* const_blk, uint32 code1, uint32 code2);

/*!
@ingroup diag_message_service
   @brief
   Sends the desired f3 message using msg_send_1, in addition to invoking 
   err_put_log. This reduces one function call for top level macro.

   @warning
   Do not call directly, use MSG_ERR_LOG macro.

   @param[in] *const_blk          static constant constructed using 
   the different xx_msg_const macros
   @param[in] code1              integer parameter 1
   
   @dependencies
   None

   @return
   None

   @sideeffects
   None

   @sa
   None
*/
void msg_errlog_1 (const msg_const_type* const_blk, uint32 code1);

/*!
@ingroup diag_message_service
   @brief
   Sends the desired f3 message using msg_send, in addition to invoking 
   err_put_log. This reduces one function call for top level macro.

   @warning
   Do not call directly, use MSG_ERR_LOG macro.

   @param[in] *const_blk          static constant constructed using 
   the different xx_msg_const macros
   
   @dependencies
   None

   @return
   None

   @sideeffects
   None

   @sa
   None
*/
void msg_errlog_0 (const msg_const_type* const_blk);


/*!
@name MSG_ERR_LOG macro
@ingroup diag_message_service
This macro is currently used by Error services. 
*/
/*!
This constructs the msg_const_type and calls the correct msg_v3_err_log_* functions,
based on the number of arguments to the string, given by @a NbPar.
@a xx_fmt is the format string and @a xx_arg1, @a xx_arg2 and @a xx_arg3 are 
the integer arguments to the string. This macro results in the string being 
sent to diagnostic buffer and into the Error-services buffer.
*/
/*@{*/ /* start group MSG_ERR_LOG macro */
#define MSG_ERR_LOG(NbPar, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
do                                                             \
{                                                              \
 /*lint -e506 -e774*/                                          \
 if (MSG_LEGACY_ERROR & (MSG_BUILD_MASK_MSG_SSID_DFLT))        \
 {                                                             \
   /*lint +e506 +e774*/                                        \
   XX_ERR_MSG_V3_CONST (MSG_SSID_DFLT, MSG_LEGACY_ERROR, xx_fmt);     \
   /*lint -e571 */                                             \
   switch (NbPar)                                         \
   {                                                      \
     case 3:                                               \
       msg_v3_errlog_3 (&xx_err_msg_v3_const, (uint32)(xx_arg1), (uint32)(xx_arg2), (uint32)(xx_arg3)); \
       break;                                                \
     case 2:                                               \
       msg_v3_errlog_2 (&xx_err_msg_v3_const, (uint32)(xx_arg1), (uint32)(xx_arg2));                       \
       break;                                                \
     case 1:                                               \
       msg_v3_errlog_1 (&xx_err_msg_v3_const, (uint32)(xx_arg1));                \
       break;                                                \
     case 0:                                               \
       msg_v3_errlog_0 (&xx_err_msg_v3_const);                                   \
       break;                                                \
     default:                                               \
	   break;                                                 \
   }   \
    /*lint +e571 */                                                                         \
 }                                                                                          \
  /*lint -e717 */                                                                           \
} while (0)                                                                                 \
/* lint +e717 */

/*@{*/ /* start group MSG_ERR_LOG macro */
#define MSG_ERR_LOG_EXT(NbPar, xx_version, xx_parameter, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
do                                                             \
{                                                              \
 /*lint -e506 -e774*/                                          \
 if (MSG_LEGACY_ERROR & (MSG_BUILD_MASK_MSG_SSID_DFLT))        \
 {                                                             \
   /*lint +e506 +e774*/                                        \
   XX_ERR_MSG_V3_CONST (MSG_SSID_DFLT, MSG_LEGACY_ERROR, xx_fmt);     \
   /*lint -e571 */                                             \
   switch (NbPar)                                         \
   {                                                      \
     case 3:                                               \
       msg_v3_errlog_3_ext (&xx_err_msg_v3_const, xx_version, xx_parameter,(uint32)(xx_arg1), (uint32)(xx_arg2), (uint32)(xx_arg3)); \
       break;                                                \
     case 2:                                               \
       msg_v3_errlog_2_ext (&xx_err_msg_v3_const, xx_version, xx_parameter, (uint32)(xx_arg1), (uint32)(xx_arg2));                       \
       break;                                                \
     case 1:                                               \
       msg_v3_errlog_1_ext (&xx_err_msg_v3_const, xx_version, xx_parameter, (uint32)(xx_arg1));                \
       break;                                                \
     case 0:                                               \
       msg_v3_errlog_0_ext (&xx_err_msg_v3_const, xx_version, xx_parameter);                                   \
       break;                                                \
     default:                                               \
	   break;                                                 \
   }   \
    /*lint +e571 */                                                                         \
 }                                                                                          \
  /*lint -e717 */                                                                           \
} while (0)                                                                                 \
/* lint +e717 */

/*@}*/ /* end group MSG_ERR_LOG macro */


#ifdef __cplusplus
}
#endif

#define FEATURE_DIAG_MSG_STATUS

/*!
@ingroup diag_message_service
   @brief
    returns if mask is valid or not

   @warning
    This API is to be used infrequently; in almost all cases clients 
	should use the existing MSG macros without calling this API. 
	Clients should use this API sparingly and only in cases where 
	there is a need to avoid heavy computation that would be wasted 
	if the mask is disabled and the MSG not sent. The client may 
	check multiple masks at once for a given SSID by ORing the 
	individual bit masks together into the ss_mask parameter

   @param[in] ss_id           SSID
   @param[in] ss_mask         sub-system mask.
            
   @dependencies
   None

   @return
   boolean : TRUE : if mask is enabled
             FALSE : if mask is disabled.

   @sideeffects
   None

   @sa
    log_status
*/

boolean msg_status(uint16 ss_id, uint32 ss_mask);


/*!
@ingroup diag_message_service
   @brief
    returns mask and ssid data for a QShrink 3.0 message

   @param[in] ssid_index      index of message to check
   @param[out] msg_ssid       pointer to location to store SSID
   @param[out] msg_mask       pointer to location to store mask
            
   @dependencies
   None

   @return
   boolean : TRUE : if index is valid
             FALSE : if mask is invalid.

   @sideeffects
   None
*/
boolean msg_get_qsr_table_entry(uint16 ssid_index, uint16 *msg_ssid, uint32 *msg_mask);

#endif /* MSG_DIAG_DLL_SERVICE_H */