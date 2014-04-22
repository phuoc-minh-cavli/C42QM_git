/*!
  @file
  ds_log.h

  @brief
  Data service common logging utility module.
*/

/*===========================================================================

  Copyright (c) 2021 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10Feb2021 vpulimi  Created the module. First version of the file.
===========================================================================*/
#ifndef _DS_LOG_H_
#define _DS_LOG_H_


#include "qapi_diag.h"
#include "msg_diag_service.h"


void ds_iot_ulog_print
(
  /*const char* file_name, uint32_t line_number, */
  uint32_t mask,
  uint8_t argnum,   
  const char *fmt, 
  ...
);



void ds_iot_log_print
(
  /*const char* file_name, uint32_t line_number, */
  uint32_t mask,
  uint8_t argnum,   
  const char *fmt, 
  ...
);


/* Data services logging related macros using QShrinkv4.0
 *
 * Use these macros for plain log messages or messages with integer format specifiers.
 *
 * Examples:
 *   ds_iot_log_high("Memory allocation successful\n");
 *   ds_iot_log_high2("Session[%d] - configuration send chunk[%d]\n", sess->index, cfg->max_send_chunk);
 *
 *
 */
#define ds_iot_log_err(format)                                          MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format)
#define ds_iot_log_err1(format, arg1)                                   MSG_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format, arg1)
#define ds_iot_log_err2(format, arg1, arg2)                             MSG_2(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format, arg1, arg2)
#define ds_iot_log_err3(format, arg1, arg2, arg3)                       MSG_3(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format, arg1, arg2, arg3)
#define ds_iot_log_err4(format, arg1, arg2, arg3, arg4)                 MSG_4(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format, arg1, arg2, arg3, arg4)
#define ds_iot_log_err5(format, arg1, arg2, arg3, arg4, arg5)           MSG_5(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format, arg1, arg2, arg3, arg4, arg5)
#define ds_iot_log_err6(format, arg1, arg2, arg3, arg4, arg5, arg6)     MSG_6(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format, arg1, arg2, arg3, arg4, arg5, arg6)



#define ds_iot_log_high(format)                                         MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format)
#define ds_iot_log_high1(format, arg1)                                  MSG_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format, arg1)
#define ds_iot_log_high2(format, arg1, arg2)                            MSG_2(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format, arg1, arg2)
#define ds_iot_log_high3(format, arg1, arg2, arg3)                      MSG_3(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format, arg1, arg2, arg3)
#define ds_iot_log_high4(format, arg1, arg2, arg3, arg4)                MSG_4(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format, arg1, arg2, arg3, arg4)
#define ds_iot_log_high5(format, arg1, arg2, arg3, arg4, arg5)          MSG_5(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format, arg1, arg2, arg3, arg4, arg5)
#define ds_iot_log_high6(format, arg1, arg2, arg3, arg4, arg5, arg6)    MSG_6(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format, arg1, arg2, arg3, arg4, arg5, arg6)


#define ds_iot_log_med(format)                                          MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format)
#define ds_iot_log_med1(format, arg1)                                   MSG_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format, arg1)
#define ds_iot_log_med2(format, arg1, arg2)                             MSG_2(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format, arg1, arg2)
#define ds_iot_log_med3(format, arg1, arg2, arg3)                       MSG_3(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format, arg1, arg2, arg3)
#define ds_iot_log_med4(format, arg1, arg2, arg3, arg4)                 MSG_4(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format, arg1, arg2, arg3, arg4)
#define ds_iot_log_med5(format, arg1, arg2, arg3, arg4, arg5)           MSG_5(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format, arg1, arg2, arg3, arg4, arg5)
#define ds_iot_log_med6(format, arg1, arg2, arg3, arg4, arg5, arg6)     MSG_6(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format, arg1, arg2, arg3, arg4, arg5, arg6)


#define ds_iot_log_low(format)                                          MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format)
#define ds_iot_log_low1(format, arg1)                                   MSG_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format, arg1)
#define ds_iot_log_low2(format, arg1, arg2)                             MSG_2(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format, arg1, arg2)
#define ds_iot_log_low3(format, arg1, arg2, arg3)                       MSG_3(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format, arg1, arg2, arg3)
#define ds_iot_log_low4(format, arg1, arg2, arg3, arg4)                 MSG_4(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format, arg1, arg2, arg3, arg4)
#define ds_iot_log_low5(format, arg1, arg2, arg3, arg4, arg5)           MSG_5(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format, arg1, arg2, arg3, arg4, arg5)
#define ds_iot_log_low6(format, arg1, arg2, arg3, arg4, arg5, arg6)     MSG_6(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format, arg1, arg2, arg3, arg4, arg5, arg6)



/* Data services logging related macros using QShrinkv4.0 along with Ulog
 *
 * Use these macros for plain log messages or messages with integer format specifiers.
 * Ulog printing is controlled by scon flag and can be enable/disable 
 *
 * Examples:
 *   ds_iot_ulog_high("Memory allocation successful\n");
 *   ds_iot_ulog_high2("Session[%d] - configuration send chunk[%d]\n", sess->index, cfg->max_send_chunk);
 *
 *
 */
#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_err(format)                                          MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 0 , format)
#else
#define ds_iot_ulog_err(format)                                          MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_err1(format, arg1)                                   MSG_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format, arg1);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 1 , format, arg1)
#else
#define ds_iot_ulog_err1(format, arg1)                                   MSG_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format, arg1)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_err2(format, arg1, arg2)                             MSG_2(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format, arg1, arg2);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 2 , format, arg1, arg2)
#else
#define ds_iot_ulog_err2(format, arg1, arg2)                             MSG_2(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format, arg1, arg2)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_err3(format, arg1, arg2, arg3)                       MSG_3(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format, arg1, arg2, arg3);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 3 , format, arg1, arg2, arg3)
#else
#define ds_iot_ulog_err3(format, arg1, arg2, arg3)                       MSG_3(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format, arg1, arg2, arg3)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_err4(format, arg1, arg2, arg3, arg4)                 MSG_4(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format, arg1, arg2, arg3, arg4);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 4 , format, arg1, arg2, arg3, arg4)
#else
#define ds_iot_ulog_err4(format, arg1, arg2, arg3, arg4)                 MSG_4(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format, arg1, arg2, arg3, arg4)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_err5(format, arg1, arg2, arg3, arg4, arg5)           MSG_5(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format, arg1, arg2, arg3, arg4, arg5);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 5 , format, arg1, arg2, arg3, arg4, arg5)
#else
#define ds_iot_ulog_err5(format, arg1, arg2, arg3, arg4, arg5)           MSG_5(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format, arg1, arg2, arg3, arg4, arg5)
#endif


#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_err6(format, arg1, arg2, arg3, arg4, arg5, arg6)     MSG_6(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format, arg1, arg2, arg3, arg4, arg5, arg6);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 6 , format, arg1, arg2, arg3, arg4, arg5, arg6)
#else
#define ds_iot_ulog_err6(format, arg1, arg2, arg3, arg4, arg5, arg6)     MSG_6(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR, format, arg1, arg2, arg3, arg4, arg5, arg6)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_high(format)                                         MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 0 , format)
#else
#define ds_iot_ulog_high(format)                                         MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_high1(format, arg1)                                  MSG_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format, arg1);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 1 , format, arg1)
#else
#define ds_iot_ulog_high1(format, arg1)                                  MSG_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format, arg1)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_high2(format, arg1, arg2)                            MSG_2(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format, arg1, arg2);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 2, format, arg1, arg2)
#else
#define ds_iot_ulog_high2(format, arg1, arg2)                            MSG_2(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format, arg1, arg2)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_high3(format, arg1, arg2, arg3)                      MSG_3(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format, arg1, arg2, arg3);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 3 , format, arg1, arg2, arg3)
#else
#define ds_iot_ulog_high3(format, arg1, arg2, arg3)                      MSG_3(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format, arg1, arg2, arg3)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_high4(format, arg1, arg2, arg3, arg4)                MSG_4(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format, arg1, arg2, arg3, arg4);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 4 , format, arg1, arg2, arg3, arg4)
#else
#define ds_iot_ulog_high4(format, arg1, arg2, arg3, arg4)                MSG_4(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format, arg1, arg2, arg3, arg4)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_high5(format, arg1, arg2, arg3, arg4, arg5)          MSG_5(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format, arg1, arg2, arg3, arg4, arg5);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 5 , format, arg1, arg2, arg3, arg4, arg5)
#else
#define ds_iot_ulog_high5(format, arg1, arg2, arg3, arg4, arg5)          MSG_5(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format, arg1, arg2, arg3, arg4, arg5)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_high6(format, arg1, arg2, arg3, arg4, arg5, arg6)    MSG_6(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format, arg1, arg2, arg3, arg4, arg5, arg6);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 6 , format, arg1, arg2, arg3, arg4, arg5, arg6)
#else
#define ds_iot_ulog_high6(format, arg1, arg2, arg3, arg4, arg5, arg6)    MSG_6(MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, format, arg1, arg2, arg3, arg4, arg5, arg6)
#endif



#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_med(format)                                          MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 0 , format)
#else
#define ds_iot_ulog_med(format)                                          MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_med1(format, arg1)                                   MSG_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format, arg1);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 1 , format, arg1)
#else
#define ds_iot_ulog_med1(format, arg1)                                   MSG_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format, arg1)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_med2(format, arg1, arg2)                             MSG_2(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format, arg1, arg2);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 2 , format, arg1, arg2)
#else
#define ds_iot_ulog_med2(format, arg1, arg2)                             MSG_2(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format, arg1, arg2)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_med3(format, arg1, arg2, arg3)                       MSG_3(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format, arg1, arg2, arg3);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 3 , format, arg1, arg2, arg3)
#else
#define ds_iot_ulog_med3(format, arg1, arg2, arg3)                       MSG_3(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format, arg1, arg2, arg3)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_med4(format, arg1, arg2, arg3, arg4)                 MSG_4(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format, arg1, arg2, arg3, arg4);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 4 , format, arg1, arg2, arg3, arg4)
#else
#define ds_iot_ulog_med4(format, arg1, arg2, arg3, arg4)                 MSG_4(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format, arg1, arg2, arg3, arg4)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_med5(format, arg1, arg2, arg3, arg4, arg5)           MSG_5(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format, arg1, arg2, arg3, arg4, arg5);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 5 , format, arg1, arg2, arg3, arg4, arg5)
#else
#define ds_iot_ulog_med5(format, arg1, arg2, arg3, arg4, arg5)           MSG_5(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format, arg1, arg2, arg3, arg4, arg5)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_med6(format, arg1, arg2, arg3, arg4, arg5, arg6)     MSG_6(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format, arg1, arg2, arg3, arg4, arg5, arg6);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 6 , format, arg1, arg2, arg3, arg4, arg5, arg6)
#else
#define ds_iot_ulog_med6(format, arg1, arg2, arg3, arg4, arg5, arg6)     MSG_6(MSG_SSID_LINUX_DATA, MSG_LEGACY_MED, format, arg1, arg2, arg3, arg4, arg5, arg6)
#endif



#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_low(format)                                          MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 0 , format)
#else
#define ds_iot_ulog_low(format)                                          MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_low1(format, arg1)                                   MSG_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format, arg1);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 1 , format, arg1)
#else
#define ds_iot_ulog_low1(format, arg1)                                   MSG_1(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format, arg1)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_low2(format, arg1, arg2)                             MSG_2(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format, arg1, arg2);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 2 , format, arg1, arg2)
#else
#define ds_iot_ulog_low2(format, arg1, arg2)                             MSG_2(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format, arg1, arg2)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_low3(format, arg1, arg2, arg3)                       MSG_3(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format, arg1, arg2, arg3);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 3 , format, arg1, arg2, arg3)
#else
#define ds_iot_ulog_low3(format, arg1, arg2, arg3)                       MSG_3(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format, arg1, arg2, arg3)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_low4(format, arg1, arg2, arg3, arg4)                 MSG_4(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format, arg1, arg2, arg3, arg4);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 4 ,format, arg1, arg2, arg3, arg4)
#else
#define ds_iot_ulog_low4(format, arg1, arg2, arg3, arg4)                 MSG_4(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format, arg1, arg2, arg3, arg4)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_low5(format, arg1, arg2, arg3, arg4, arg5)           MSG_5(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format, arg1, arg2, arg3, arg4, arg5);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 5 ,format, arg1, arg2, arg3, arg4, arg5)
#else
#define ds_iot_ulog_low5(format, arg1, arg2, arg3, arg4, arg5)           MSG_5(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format, arg1, arg2, arg3, arg4, arg5)
#endif

#ifdef TX_DATA_ENABLE_ULOG
#define ds_iot_ulog_low6(format, arg1, arg2, arg3, arg4, arg5, arg6)     MSG_6(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format, arg1, arg2, arg3, arg4, arg5, arg6);\
                                                                         ds_iot_ulog_print(MSG_SSID_LINUX_DATA , 6 , format, arg1, arg2, arg3, arg4, arg5, arg6)
#else
#define ds_iot_ulog_low6(format, arg1, arg2, arg3, arg4, arg5, arg6)     MSG_6(MSG_SSID_LINUX_DATA, MSG_LEGACY_LOW, format, arg1, arg2, arg3, arg4, arg5, arg6)
#endif




/* Data services logging related macros without Qshrinkv4.0
 *
 * Use these macros for log messages with string format specifiers or a combination of string and integer format specifiers.
 *
 * Examples:
 *   ds_iot_log_strf_high("Device assigned IP address [%s]\n", ip_addr);
 *   ds_iot_log_strf_low("Session[%d] - server URL[%s], rqst command[%u]\n", sess->index, hcs_url, sess->hcs_command);
 *
 *
 */


#define ds_iot_log_strf_err(format, ...)           ds_iot_log_print(MSG_LEGACY_ERROR, MSG_NARG(format, ## __VA_ARGS__), format, ##__VA_ARGS__)

#define ds_iot_log_strf_high(format, ...)          ds_iot_log_print(MSG_LEGACY_HIGH,  MSG_NARG(format, ## __VA_ARGS__), format, ##__VA_ARGS__)

#define ds_iot_log_strf_med(format, ...)           ds_iot_log_print(MSG_LEGACY_MED,   MSG_NARG(format, ## __VA_ARGS__), format, ##__VA_ARGS__)

#define ds_iot_log_strf_low(format, ...)           ds_iot_log_print(MSG_LEGACY_LOW,   MSG_NARG(format, ## __VA_ARGS__), format, ##__VA_ARGS__)


/**** ****************************************NEED TO REMOVE REMAINING CODE ******************************************************/


#endif /* _DS_LOG_H_ */
