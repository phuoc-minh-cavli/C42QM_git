#ifndef RFDEVICE_TRX_TIME_PROFILE_H
#define RFDEVICE_TRX_TIME_PROFILE_H

/*!
  @file
  rfdevice_trx_time_profile.h

  @brief
  Diagnostic message services for execution of timing of APIs.
*/

/*==============================================================================

  Copyright (c) 2015-2018 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rfdevice_interface.mpss/1.40/api/rfdevice_trx_time_profile.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
06/15/18   up      Initial Release.
==============================================================================*/

#include "msg.h"
#include "log.h"
#include"rfcommon_time_profile.h"

/* macros used for variable parameter functions */
#define GET_STRING_LITERAL_1(arg1) #arg1":%d "
#define GET_STRING_LITERAL_2(arg1, arg2) #arg1":%d, "#arg2":%d "
#define GET_STRING_LITERAL_3(arg1, arg2, arg3) #arg1":%d, "#arg2":%d, "#arg3":%d "
#define GET_STRING_LITERAL_4(arg1, arg2, arg3, arg4) #arg1":%d, "#arg2":%d, "#arg3":%d, "#arg4":%d "
#define GET_STRING_LITERAL_5(arg1, arg2, arg3, arg4, arg5) #arg1":%d, "#arg2":%d, "#arg3":%d, "#arg4":%d, "#arg5":%d "
#define GET_STRING_LITERAL_6(arg1, arg2, arg3, arg4, arg5, arg6) #arg1":%d, "#arg2":%d, "#arg3":%d, "#arg4":%d, "#arg5":%d, "#arg6":%d "
#define GET_STRING_LITERAL_7(arg1, arg2, arg3, arg4, arg5, arg6, arg7) #arg1":%d, "#arg2":%d, "#arg3":%d, "#arg4":%d, "#arg5":%d, "#arg6":%d, "#arg7":%d "
#define GET_STRING_LITERAL_8(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) #arg1":%d, "#arg2":%d, "#arg3":%d, "#arg4":%d, "#arg5":%d, "#arg6":%d, "#arg7":%d, "#arg8":%d "

#define GET_11TH_ARG(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,NAME,...) NAME
#define GET_9TH_ARG(_1,_2,_3,_4,_5,_6,_7,_8,NAME,...) NAME

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device driver Diag message with variable parameters.

  @param mask
  The message level mask for build and QXDM filtering.

  @param ...
  Variable parameter arguments to be printed
*/
#define RFDEVICE_MSG(log_id, mask, ...) \
  if(mask == RF_ERROR || log_status(log_id))\
    GET_11TH_ARG(__VA_ARGS__, RF_MSG_9, RF_MSG_8, RF_MSG_7,RF_MSG_6,RF_MSG_5,\
                              RF_MSG_4,RF_MSG_3,RF_MSG_2, RF_MSG_1, RF_MSG)(mask, __VA_ARGS__)

#define RFDEVICE_TRX_MSG(mask, ... ) RFDEVICE_MSG(LOG_RFA_DEVICE_TRASCEIVER_C, mask, __VA_ARGS__)
/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device driver Diag message which starts timing profiling and prints parameters.

  @param log_id
  Log mask
 
  @param func_name
  Function name string.

  @param ...
  Variable parameter arguments to be printed
*/
#define RFDEVICE_LOG_START(log_id, func_name, ...) \
	RFDEVICE_MSG(log_id, \
                 RF_MED, \
                 func_name "(): " GET_9TH_ARG(__VA_ARGS__, \
                        GET_STRING_LITERAL_8, GET_STRING_LITERAL_7, GET_STRING_LITERAL_6,\
                        GET_STRING_LITERAL_5, GET_STRING_LITERAL_4, GET_STRING_LITERAL_3,\
                        GET_STRING_LITERAL_2, GET_STRING_LITERAL_1)(__VA_ARGS__),\
                 __VA_ARGS__ );\
    RF_TIME_PROFILE_START(log_id);

#define RFDEVICE_TRX_LOG_START(func_name, ... ) RFDEVICE_LOG_START(LOG_RFA_DEVICE_TRASCEIVER_C, func_name, __VA_ARGS__)

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device driver Diag message which prints message as an Error

  @param func_name
  Function name string.

  @param ...
  Variable parameter arguments to be printed
*/
#define RFDEVICE_TRX_LOG_ERROR(func_name, ...) \
	RFDEVICE_MSG(LOG_RFA_DEVICE_TRASCEIVER_C, \
                 RF_ERROR, \
                 func_name "(): " GET_9TH_ARG(__VA_ARGS__, \
                        GET_STRING_LITERAL_8, GET_STRING_LITERAL_7, GET_STRING_LITERAL_6,\
                        GET_STRING_LITERAL_5, GET_STRING_LITERAL_4, GET_STRING_LITERAL_3,\
                        GET_STRING_LITERAL_2, GET_STRING_LITERAL_1)(__VA_ARGS__),\
                 __VA_ARGS__ );
    
/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device driver Diag message which prints time elapsed and output parameters.

  @param func_name
  Function name string.

  @param msg
  User message to be printed

  @param ...
  Variable parameter arguments to be printed
*/
#define RFDEVICE_LOG_END(log_id, func_name, ...) \
  if(log_status(log_id)) {\
    RFDEVICE_MSG(log_id, \
                 RF_MED, \
                 func_name "(): time taken %dus. " GET_9TH_ARG(__VA_ARGS__, \
                        GET_STRING_LITERAL_8, GET_STRING_LITERAL_7, GET_STRING_LITERAL_6, \
                        GET_STRING_LITERAL_5, GET_STRING_LITERAL_4, GET_STRING_LITERAL_3, \
                        GET_STRING_LITERAL_2, GET_STRING_LITERAL_1)(__VA_ARGS__), \
                 rf_time_get_elapsed( tm_tick, RF_USEC ),__VA_ARGS__ ); \
  }                 

#define RFDEVICE_TRX_LOG_END(func_name, ... ) RFDEVICE_LOG_END(LOG_RFA_DEVICE_TRASCEIVER_C, func_name, __VA_ARGS__)

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Starts timer tick.

  @param tm_tick
  timer tick variable
*/
#define RF_TIME_PROFILE_START(log_id) \
  /*timer variable declaration*/ \
  rf_time_tick_type tm_tick; \
  /*check if logs are enabled*/ \
  if(log_status(log_id)) \
    tm_tick = rf_time_get_tick();

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prints elapsed time.
  
  @param func_name
  Function name string.

  @param msg
  User message to be printed
  
  @param tm_tick
  timer tick variable
*/
#define RF_TIME_PROFILE_END(log_id, msg ) \
    RFDEVICE_MSG(log_id, \
                 RF_MED, \
                 msg": Time elapsed = %d usec", \
                 rf_time_get_elapsed( tm_tick, RF_USEC ));

#endif  /* RFDEVICE_TRX_TIME_PROFILE_H */

