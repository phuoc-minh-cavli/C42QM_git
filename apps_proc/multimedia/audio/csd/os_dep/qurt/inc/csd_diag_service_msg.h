#ifndef _CSD_DIAGSERV_MSG_H
#define _CSD_DIAGSERV_MSG_H

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.


when        who        what, where, why
--------    ---        ------------------------------------------------------
07/04/13   unni/vk     added csd_msg_check_format to get compiler warnings
09/25/12   unni        Added CSD_DIAG_MSG_SPRINTF variable argument logging
04/20/2010 asheeshk    initial draft
09/28/2010 asheeshk    updated to use diag macros instead of dalsys for bmp and 8200

============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#include "msg.h"
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus


/* These SSIDs are in msgcfg.h which is included via msg.h */

/* Priorities Low, Medium, High, Error, Fatal, Debug
   are pulled in from msgcfg.h which is included via msg.h
*/
#define CSD_PRIO_LOW      (MSG_MASK_0)
#define CSD_PRIO_MEDIUM   (MSG_MASK_1)
#define CSD_PRIO_HIGH     (MSG_MASK_2)
#define CSD_PRIO_ERROR    (MSG_MASK_3)
#define CSD_PRIO_FATAL    (MSG_MASK_4)
#define CSD_PRIO_DEBUG    (MSG_MASK_5)

/*
  This is the macro for messages with no parameters with priority.
 */
#define CSD_DIAG_MSG_ARG_0(xx_prio, xx_fmt) \
{                                                       \
    MSG ( MSG_SSID_APPS_CAD_GENERAL, xx_prio, xx_fmt );     \
}

/*
  Macro for messages with 1 parameter and a specific priority
*/
#define CSD_DIAG_MSG_ARG_1(xx_prio, xx_fmt, xx_arg1) \
  {                                                           \
    MSG_1( MSG_SSID_APPS_CAD_GENERAL, xx_prio, xx_fmt, xx_arg1);   \
  }

/*
  Macro for messages with 2 parameters and a specific priority.
*/
#define CSD_DIAG_MSG_ARG_2(xx_prio, xx_fmt, xx_arg1, xx_arg2) \
  {                                                               \
    MSG_2(MSG_SSID_APPS_CAD_GENERAL, xx_prio, xx_fmt, xx_arg1, xx_arg2);  \
  }

/*
  This is the macro for messages with 3 parameters and a specific priority.
*/
#define CSD_DIAG_MSG_ARG_3(xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  {                                                               \
    MSG_3(MSG_SSID_APPS_CAD_GENERAL, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3);  \
  }

/*
  This is the macro for messages with 4 parameters and a specific priority. 
  msg_send_var() uses var arg list supported by the compiler.
*/
#define CSD_DIAG_MSG_ARG_4(xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3,xx_arg4) \
  {                                                               \
    MSG_4(MSG_SSID_APPS_CAD_GENERAL, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4);  \
  }

#define CSD_DIAG_MSG_ARG_5(xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3,xx_arg4,xx_arg5) \
  {                                                               \
    MSG_5(MSG_SSID_APPS_CAD_GENERAL, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4,xx_arg5);  \
  }

#define CSD_DIAG_MSG_ARG_6(xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3,xx_arg4,xx_arg5,xx_arg6) \
  {                                                               \
    MSG_6(MSG_SSID_APPS_CAD_GENERAL, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4,xx_arg5,xx_arg6);  \
  }

#define CSD_DIAG_MSG_ARG_7(xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3,xx_arg4,xx_arg5,xx_arg6,xx_arg7) \
  {                                                               \
    MSG_7(MSG_SSID_APPS_CAD_GENERAL, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4,xx_arg5,xx_arg6,xx_arg7);  \
  }

#define CSD_DIAG_MSG_ARG_8(xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3,xx_arg4,xx_arg5,xx_arg6,xx_arg7,xx_arg8) \
  {                                                               \
    MSG_8(MSG_SSID_APPS_CAD_GENERAL, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4,xx_arg5,xx_arg6,xx_arg7,xx_arg8);  \
  }

#define CSD_DIAG_MSG_ARG_9(xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3,xx_arg4,xx_arg5,xx_arg6,xx_arg7,xx_arg8,xx_arg9) \
  {                                                               \
    MSG_9(MSG_SSID_APPS_CAD_GENERAL, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4,xx_arg5,xx_arg6,xx_arg7,xx_arg8,xx_arg9);  \
  }  
  
/*
  This is the macro for messages with 1 string arguments and a specific priority. 
*/
#define CSD_DIAG_MSG_SPRINTF_ARG_1(xx_prio, xx_fmt, xx_arg1) \
  do { \
      XX_MSG_CONST (MSG_SSID_APPS_CAD_GENERAL, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1)); \
    /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
  /* lint +e717 */ 


/*
  This is the macro for messages with 1 string arguments, 1 decimal arguments 
  and a specific priority. 
*/
#define CSD_DIAG_MSG_SPRINTF_ARG_2(xx_prio, xx_fmt, xx_arg1, xx_arg2) \
  do { \
      XX_MSG_CONST (MSG_SSID_APPS_CAD_GENERAL, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2)); \
    /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
  /* lint +e717 */

/* Check the string format to verify valid the number of arguments.
 * Equivalent to -Wfromat in gcc, armcc pragma check_printf_formats which is not
 * supported in this version */
#pragma __printf_args

/* Empty function to have pragma __pritnf_args verify format and arguments */  
static void csd_msg_check_format(const char * format, ...){
  return;
};


/*
  This is the macro for messages with 1 string arguments, n arguments and a 
  specific priority. 
*/
#define CSD_DIAG_MSG_SPRINTF(xx_prio, xx_fmt, ...) \
  do { \
      XX_MSG_CONST (MSG_SSID_APPS_CAD_GENERAL, xx_prio, xx_fmt); \
      csd_msg_check_format(xx_fmt, ##__VA_ARGS__); \
      msg_sprintf (&xx_msg_const, ##__VA_ARGS__); \
  } while (0)  


/* Class & Function Definations */

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/**
 * Initializes the Diag Interface
 * 
 * @return 0 value on success else failure
 */
int csd_debug_initialize( void );

/**
 * De-Initializez the Diag Interface
 * 
 * @return 0 value on success else failure
 */
int csd_debug_deinitialize( void );

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif//_CSD_DIAGSERV_MSG_H

