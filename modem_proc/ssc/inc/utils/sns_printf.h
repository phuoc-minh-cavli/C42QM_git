#pragma once
/*=============================================================================
 * @file sns_printf.h
 *
 * Helper functions and macros for submitting debug messages
 *
 * Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_diag_service.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"


/* Note: the following is unfortunately ugly. This needs to include msg.h to
 * get access to diag macros, which includes the difficult "comdef.h" which
 * multiply defines ARR_SIZE.
 * This hack "fixes" the multiple defines */
#undef ARR_SIZE
#include "msg.h"
#ifndef ARR_SIZE
#define ARR_SIZE(a) (sizeof(a)/sizeof((a)[0]))
#endif

#define _MSG_STR_SNS(A) #A
#define MSG_STR_SNS(A) _MSG_STR_SNS(A)

/*=============================================================================
  Macro Definitions
  ===========================================================================*/

#ifndef SNS_DIAG_SSID
/* SNS_DIAG_SSID should be defined in the scons file to pick a particular SSID
 * for this module. If the diag SSID has not been defined, put the messages into
 * the default MSG_SSID_SNS.
 */
#define SNS_DIAG_SSID MSG_SSID_SNS
#endif /* SNS_DIAG_SSID */

/* macros to help count variable number of arguments, max_args=16 */
#define SELECT_NARG(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, N, ...) N
#define NARGS(...) SELECT_NARG(_0, ##__VA_ARGS__, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)


extern void
sns_diag_sensor_printf(sns_diag_service *this,
                       sns_sensor *sensor,
                       sns_message_priority prio,
#if defined(SNS_USES_MSG_API_V4)
                       const void *diag_str, uint32_t line, uint32_t nargs,
#else
                       const char *file, uint32_t line, uint32_t nargs,
#endif
                       const char *format, ...);


extern void
sns_diag_sensor_inst_printf(sns_diag_service *this,
                            sns_sensor_instance *instance,
                            sns_message_priority prio,
#if defined(SNS_USES_MSG_API_V4)
                            const void *diag_str, uint32_t line, uint32_t nargs,
#else
                            const char *file, uint32_t line, uint32_t nargs,
#endif
                            const char *format, ...);


/* Creates a constant string which is placed in big image, for use by diag */
#define SNS_MSG_V2_CONST(xx_ss_id, xx_ss_mask, xx_fmt)                  \
  static const char SNS_bigimg_str[] __attribute__((section(".rodata1")))  \
       = msg_file ":" xx_fmt ;                                          \
  static const msg_v2_const_type xx_msg_v2_const = {                    \
    .desc = { .line = __LINE__, .ss_id = (xx_ss_id), .ss_mask = (xx_ss_mask)},                               \
    .msg = (char*)&SNS_bigimg_str }

/*  Constructs a msg_v4_const_type from the SSID, mask and format-string
*/
#if defined(SNS_USES_MSG_API_V4)
#define SNS_MSG_V4_CONST(xx_ss_id, xx_ss_mask, xx_fmt) \
    static const char QSR_string_ptr[] QSR_4_0_ATTR  \
      = MSG_STR(xx_ss_id) ":" MSG_STR(__LINE__)":" msg_file ":" xx_fmt ; \
    static const msg_v4_const_type xx_msg_v4_const \
      __attribute__((section ("QSR_4_0_MSG.fmt.rodata." MSG_STR(xx_ss_id)"." MSG_STR(xx_ss_mask)"." SECT_EXT),used))= \
      { { xx_ss_mask, (char*)&QSR_string_ptr} }
#endif
/**
 * Print a log message from a Sensor.  Only supports integral arguments.
 *
 * Parameters:
 *   prio: LOW, MED, HIGH, ERROR, FATAL
 *   sensor: sns_sensor
 *   fmt, args: printf style format string and arguments. %s and %f are not supported
 */

#if defined(SNS_USES_MSG_API_V4)
#define SNS_PRINTF(prio, sensor, fmt, ...) do {                         \
    SNS_MSG_V4_CONST(SNS_DIAG_SSID, 1 << (prio), fmt);            \
    sns_diag_sensor_printf(NULL, (sns_sensor*)(sensor),   (prio),            \
                              (void *)(&xx_msg_v4_const), \
                              __LINE__, NARGS(__VA_ARGS__), fmt, ##__VA_ARGS__); \
  } while(0)
#else
#define SNS_PRINTF(prio, sensor, fmt, ...) do {                         \
    SNS_MSG_V2_CONST(SNS_DIAG_SSID, 1 << (SNS_##prio), fmt);            \
    sns_diag_sensor_printf_v2(NULL, (sns_sensor*)(sensor),              \
                              (struct sns_msg_v2_const_type const *)(&xx_msg_v2_const), \
                              NARGS(__VA_ARGS__), ##__VA_ARGS__);       \
  } while(0)
#endif


/**
 * Print a log message from a Sensor Instance.  Only supports integral arguments.
 *
 * Parameters:
 *   prio: LOW, MED, HIGH, ERROR, FATAL
 *   inst: sns_sensor_instance -- must not be NULL
 *   fmt, args: printf style format string and arguments. %s and %f are not supported
 */
#if defined(SNS_USES_MSG_API_V4)
#define SNS_INST_PRINTF(prio, inst, fmt, ...) do {                      \
    SNS_MSG_V4_CONST(SNS_DIAG_SSID, 1 << (prio), fmt);            \
    sns_diag_sensor_inst_printf(NULL, (sns_sensor_instance*)(inst),  (prio), \
                              (void *)(&xx_msg_v4_const), \
                              __LINE__, NARGS(__VA_ARGS__), fmt, ##__VA_ARGS__); \
  } while(0)
#else
#define SNS_INST_PRINTF(prio, inst, fmt, ...) do {                      \
    SNS_MSG_V2_CONST(SNS_DIAG_SSID, 1 << (SNS_##prio), fmt);            \
    sns_diag_sensor_inst_printf_v2(NULL, (sns_sensor_instance*)(inst),  \
                                   (struct sns_msg_v2_const_type const *)(&xx_msg_v2_const), \
                                   NARGS(__VA_ARGS__), ##__VA_ARGS__);  \
  } while(0)
#endif


/**
 * Please use SNS_PRINTF, and do not call this function directly.
 */
extern void sns_diag_sensor_printf_v2(sns_diag_service *, sns_sensor *,
                                      struct sns_msg_v2_const_type const *,
                                      uint32_t, ...);

/**
 * Please use SNS_INST_PRINTF, and do not call this function directly.
 */
extern void sns_diag_sensor_inst_printf_v2(sns_diag_service *, sns_sensor_instance *,
                                           struct sns_msg_v2_const_type const *,
                                           uint32_t, ...);
