#ifndef DATA_MSG_H
/*lint -e750*/
#define DATA_MSG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
                       D A T A   S E R V I C E S 
              DIAGNOSTIC MESSAGE 2.0 SERVICE HEADER FILE

GENERAL DESCRIPTION

	All the declarations and definitions necessary to support the reporting 
	of messages for errors and debugging.  This includes support for the 
	extended capabilities as well as the legacy messaging scheme.
  
  Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
  
  /*==================================================================
						  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/utils/inc/data_msg.h#2 $

when       who         what, where, why
--------   -----       ----------------------------------------------------------


===========================================================================*/

/*===========================================================================

                              INCLUDE FILES

===========================================================================*/

/*---------------------------------------------------------------------------
  This will also make available all the Sub-System IDs and the build masks
---------------------------------------------------------------------------*/
#include "msg.h"
#include "event.h"

/*--------------------------------------------------------------------------
ERR_FATAL messages
---------------------------------------------------------------------------*/
#define DS_ERR_FATAL(xx_exp) ERR_FATAL("Assertion " #xx_exp " failed", 0, 0, 0 );

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DS_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DS_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DS_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DS_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DS_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DS_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DS_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DS_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DS_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DS_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DS_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DS_MSG_SPRINTF_2( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DS_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 4 parameters
---------------------------------------------------------------------------*/
#define DS_MSG_SPRINTF_4( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                               xx_arg4) \
  MSG_SPRINTF_4(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
    xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 5 parameters
---------------------------------------------------------------------------*/
#define DS_MSG_SPRINTF_5( xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                      xx_arg4, xx_arg5) \
  MSG_SPRINTF_5(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
    xx_arg4, xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 6 parameters
---------------------------------------------------------------------------*/
#define DS_MSG_SPRINTF_6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                              xx_arg4, xx_arg5, xx_arg6) \
  MSG_SPRINTF_6(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
    xx_arg4, xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 7 parameters
---------------------------------------------------------------------------*/
#define DS_MSG_SPRINTF_7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                          xx_arg4, xx_arg5, xx_arg6, xx_arg7) \
  MSG_SPRINTF_7(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
    xx_arg4, xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------- 
  Wrappers for ERROR_FATAL's and input validation failure messages 
----------------------------------------------------------------------*/
#define DS_ERROR_FATAL(xx_exp) \
  ds_err_fatal_wrapper( __FILENAME__ , __LINE__, xx_exp)
/*---------------------------------------------------------------------- 
  Wrappers for ERROR_ASSERT's and input validation failure messages 
----------------------------------------------------------------------*/
#define DS_ASSERT(val)\
  ds_assert_wrapper( __FILENAME__ , __LINE__, val)

/*---------------------------------------------------------------------- 
  Macros for ERROR_MSGs for DS F3s 
----------------------------------------------------------------------*/
#define DS_ERROR_LOG_0(str) \
  ds_error_log_0_wrapper(__FILENAME__ ,__LINE__, str)

#define DS_ERROR_LOG_1(str, arg1) \
  ds_error_log_1_wrapper(__FILENAME__ ,__LINE__, str, (uint32)arg1)

#define DS_ERROR_LOG_2(str, arg1, arg2) \
  ds_error_log_2_wrapper(__FILENAME__ ,__LINE__, \
  str, (uint32)arg1, (uint32)arg2)

#define DS_ERROR_LOG_3(str, arg1, arg2, arg3) \
  ds_error_log_3_wrapper(__FILENAME__ ,__LINE__, \
  str, (uint32)arg1, (uint32)arg2, (uint32)arg3)

#define DS_ERROR_LOG_4(str, arg1, arg2, arg3, arg4) \
  ds_error_log_4_wrapper(__FILENAME__ ,__LINE__,  \
  str, (uint32)arg1, (uint32)arg2, (uint32)arg3, (uint32)arg4)


//Temporariliy putting this; if design is to have command MACRO, will be modified later
/*===========================================================================
  ATCOP SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_ATCOP_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 4 parameters
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG_SPRINTF_4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
	                                                                     xx_arg4) \
  MSG_SPRINTF_4(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                                       xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 5 parameters
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG_SPRINTF_5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
	                                                            xx_arg4, xx_arg5) \
  MSG_SPRINTF_5(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                              xx_arg4, xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 6 parameters
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG_SPRINTF_6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
	                                                   xx_arg4, xx_arg5, xx_arg6) \
  MSG_SPRINTF_6(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                     xx_arg4, xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 7 parameters
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG_SPRINTF_7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
	                                           xx_arg4, xx_arg5, xx_arg6, xx_arg7) \
  MSG_SPRINTF_7(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                             xx_arg4, xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 8 parameters
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG_SPRINTF_8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
	                                 xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_SPRINTF_8(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                   xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_ATCOP, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  ATCOP SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_ATCOP, DATA_ATCOP_GENERAL, xx_fmt)
  
#define DATA_ATCOP_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_ATCOP, DATA_ATCOP_DEBUG, xx_fmt)
  
#define DATA_ATCOP_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_ATCOP, DATA_ATCOP_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_ATCOP_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_ATCOP, DATA_ATCOP_GENERAL, xx_fmt, xx_arg1)

#define DATA_ATCOP_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_ATCOP, DATA_ATCOP_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_ATCOP_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_ATCOP, DATA_ATCOP_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_ATCOP, DATA_ATCOP_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_ATCOP_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_ATCOP, DATA_ATCOP_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_ATCOP_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_ATCOP, DATA_ATCOP_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_ATCOP_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_ATCOP, DATA_ATCOP_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_ATCOP_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_ATCOP, DATA_ATCOP_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_ATCOP_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_ATCOP, DATA_ATCOP_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)


/*===========================================================================
  DS 3GMGR SS related macros
===========================================================================*/


/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt)  

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_3GMGR_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters 
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters 
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters 
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters 
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS_3GMGR, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  3GMGR SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS_3GMGR, DATA_3GMGR_GENERAL, xx_fmt)
  
#define DATA_3GMGR_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS_3GMGR, DATA_3GMGR_DEBUG, xx_fmt)
  
#define DATA_3GMGR_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS_3GMGR, DATA_3GMGR_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DATA_3GMGR_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_3GMGR, DATA_3GMGR_GENERAL, xx_fmt, xx_arg1)

#define DATA_3GMGR_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_3GMGR, DATA_3GMGR_DEBUG, xx_fmt, xx_arg1)
  
#define DATA_3GMGR_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS_3GMGR, DATA_3GMGR_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_3GMGR, DATA_3GMGR_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_3GMGR_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_3GMGR, DATA_3GMGR_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_3GMGR_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS_3GMGR, DATA_3GMGR_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_3GMGR_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_3GMGR, DATA_3GMGR_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_3GMGR_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_3GMGR, DATA_3GMGR_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_3GMGR_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS_3GMGR, DATA_3GMGR_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  ODL DS messages
===========================================================================*/

#define DS_IOT_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_MODEM_DATA, xx_ss_mask, xx_fmt)  
/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/    
#define DS_IOT_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_MODEM_DATA, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DS_IOT_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_MODEM_DATA, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DS_IOT_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_MODEM_DATA, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DS_IOT_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_MODEM_DATA, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*===========================================================================
  DS APPSRV SS related macros
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG0(xx_ss_mask, xx_fmt) \
  MSG(MSG_SSID_DS, xx_ss_mask, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                               xx_arg5) \
  MSG_5(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5)

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                          xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6)

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                                 xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7)

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                                        xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8)

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
                               xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, \
    xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_FMT_VAR_3(MSG_SSID_DS, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/*===========================================================================
  APPSRV SS related convenience macros. These will substitute the SSID as
  well as the desired sub-group ID.
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro for messages with no parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG0_GEN(xx_fmt) \
  MSG(MSG_SSID_DS, DATA_APPSRV_GENERAL, xx_fmt)

#define DATA_APPSRV_MSG0_DBG(xx_fmt) \
  MSG(MSG_SSID_DS, DATA_APPSRV_DEBUG, xx_fmt)

#define DATA_APPSRV_MSG0_TRC(xx_fmt) \
  MSG(MSG_SSID_DS, DATA_APPSRV_TRACE, xx_fmt)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG1_GEN(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS, DATA_APPSRV_GENERAL, xx_fmt, xx_arg1)

#define DATA_APPSRV_MSG1_DBG(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS, DATA_APPSRV_DEBUG, xx_fmt, xx_arg1)

#define DATA_APPSRV_MSG1_TRC(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DS, DATA_APPSRV_TRACE, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG2_GEN(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS, DATA_APPSRV_GENERAL, xx_fmt, xx_arg1, xx_arg2)

#define DATA_APPSRV_MSG2_DBG(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS, DATA_APPSRV_DEBUG, xx_fmt, xx_arg1, xx_arg2)

#define DATA_APPSRV_MSG2_TRC(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DS, DATA_APPSRV_TRACE, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  Macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define DATA_APPSRV_MSG3_GEN(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS, DATA_APPSRV_GENERAL, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_APPSRV_MSG3_DBG(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS, DATA_APPSRV_DEBUG, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define DATA_APPSRV_MSG3_TRC(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DS, DATA_APPSRV_TRACE, xx_fmt, xx_arg1, xx_arg2, xx_arg3) 



typedef enum
{
  DS_LOCAL_ERR_VAL_NONE = -1,
  DS_LOCAL_ERR_VAL_0 = 0,
  DS_LOCAL_ERR_VAL_1,
  DS_LOCAL_ERR_VAL_2,
  DS_LOCAL_ERR_VAL_3,
  DS_LOCAL_ERR_VAL_4,
  DS_LOCAL_ERR_VAL_5,
  DS_LOCAL_ERR_VAL_6,
  DS_LOCAL_ERR_VAL_7,
  DS_LOCAL_ERR_VAL_8,
  DS_LOCAL_ERR_VAL_9,
  DS_LOCAL_ERR_VAL_10,
  DS_LOCAL_ERR_VAL_11,
  DS_LOCAL_ERR_VAL_12,
  DS_LOCAL_ERR_VAL_13,
  DS_LOCAL_ERR_VAL_14,
  DS_LOCAL_ERR_VAL_15,
  DS_LOCAL_ERR_VAL_16,
  DS_LOCAL_ERR_VAL_17,
  DS_LOCAL_ERR_VAL_18,
  DS_LOCAL_ERR_VAL_19,
  DS_LOCAL_ERR_VAL_20,
  DS_LOCAL_ERR_VAL_21,
  DS_LOCAL_ERR_VAL_22,
  DS_LOCAL_ERR_VAL_23,
  DS_LOCAL_ERR_VAL_24,
  DS_LOCAL_ERR_VAL_25,
  DS_LOCAL_ERR_VAL_26,
  DS_LOCAL_ERR_VAL_27,
  DS_LOCAL_ERR_VAL_28,
  DS_LOCAL_ERR_VAL_29,
  DS_LOCAL_ERR_VAL_30,
  DS_LOCAL_ERR_VAL_31,
  DS_LOCAL_ERR_VAL_32,
  DS_LOCAL_ERR_VAL_33,
  DS_LOCAL_ERR_VAL_34,
  DS_LOCAL_ERR_VAL_35,
  DS_LOCAL_ERR_VAL_36,
  DS_LOCAL_ERR_VAL_37,
  DS_LOCAL_ERR_VAL_38,
  DS_LOCAL_ERR_VAL_39,
  DS_LOCAL_ERR_VAL_40,
  DS_LOCAL_ERR_VAL_41,
  DS_LOCAL_ERR_VAL_42,
  DS_LOCAL_ERR_VAL_43,
  DS_LOCAL_ERR_VAL_44,
  DS_LOCAL_ERR_VAL_45,
  DS_LOCAL_ERR_VAL_46,
  DS_LOCAL_ERR_VAL_47,
  DS_LOCAL_ERR_VAL_48,
  DS_LOCAL_ERR_VAL_49,
} ds_local_err_val_e_type;

/*===========================================================================
            LOCAL DECLARATIONS FOR MODULE
===========================================================================*/

/*===========================================================================

FUNCTION DS_3GPP_ERR_FATAL_WRAPPER

DESCRIPTION
  Wrapper function for ERROR_FATALS
 
DEPENDENCIES 
  None

RETURN VALUE
  None
 
SIDE EFFECTS

===========================================================================*/
void ds_err_fatal_wrapper
(  
  const char    *file_name,
  unsigned int  line_num,
  const char    *str
);

/*===========================================================================

FUNCTION DS_ASSERT_WRAPPER

DESCRIPTION
  Wrapper function for ASSERTION
 
DEPENDENCIES 
  None

RETURN VALUE
  None
 
SIDE EFFECTS

===========================================================================*/
void ds_assert_wrapper
(  
  const char      *file_name,
  unsigned int     line_num,
  int              expression
);

/*===========================================================================

FUNCTION DS_ERROR_LOG_0_WRAPPER

DESCRIPTION
  Wrapper function for reporting error messages
 
DEPENDENCIES 

RETURN VALUE
 
SIDE EFFECTS

===========================================================================*/
void ds_error_log_0_wrapper
(
  const char*   str,
  unsigned int  line_num,  
  const char*   format
);

/*===========================================================================

FUNCTION DS_ERROR_LOG_1_WRAPPER

DESCRIPTION
  Wrapper function for reporting error messages
 
DEPENDENCIES 

RETURN VALUE
 
SIDE EFFECTS

===========================================================================*/
void ds_error_log_1_wrapper
(
  const char*   str,
  unsigned int  line_num,  
  const char*   format,
  uint32        arg1
);

/*===========================================================================

FUNCTION DS_ERROR_LOG_2_WRAPPER

DESCRIPTION
  Wrapper function for reporting error messages
 
DEPENDENCIES 

RETURN VALUE
 
SIDE EFFECTS

===========================================================================*/
void ds_error_log_2_wrapper
(
  const char*   str,
  unsigned int  line_num,  
  const char*   format,
  uint32        arg1,
  uint32        arg2
);

/*===========================================================================

FUNCTION DS_ERROR_LOG_3_WRAPPER

DESCRIPTION
  Wrapper function for reporting error messages
 
DEPENDENCIES 

RETURN VALUE
 
SIDE EFFECTS

===========================================================================*/
void ds_error_log_3_wrapper
(
  const char*   str,
  unsigned int  line_num,  
  const char*   format,
  uint32        arg1,
  uint32        arg2,
  uint32        arg3
);

/*===========================================================================

FUNCTION DS_ERROR_LOG_4_WRAPPER

DESCRIPTION
  Wrapper function for reporting error messages
 
DEPENDENCIES 

RETURN VALUE
 
SIDE EFFECTS

===========================================================================*/
void ds_error_log_4_wrapper
(
  const char*   str,
  unsigned int  line_num,
  const char*   format,
  uint32        arg1,
  uint32        arg2,
  uint32        arg3,
  uint32        arg4
);

#endif  // DATA_MSG_H
