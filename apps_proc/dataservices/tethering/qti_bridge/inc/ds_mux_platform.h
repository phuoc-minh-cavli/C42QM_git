#ifndef DS_MUX_PLATFORM_H
#define DS_MUX_PLATFORM_H
/*===========================================================================

                        D S _  M U X_ P L A T F O R M . H

DESCRIPTION

  Platform/OS specific declration 

Copyright (c) 2016,2021 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/dev/data.mpss/3.4/adubey.data.mpss.3.4.ds_mux_changes/interface/tasks/inc/ds_mux_platform.h#1 
  $ $DateTime: 2021/10/05 22:26:14 $ $Author: pwbldsvc $

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include <stringl/stringl.h>
#include "qurt_signal.h"
#include "qurt_thread.h"
#include "qurt_error.h"
#include "qurt_mutex.h"
#include "qurt_timer.h"
#include "ds_log.h"



#ifdef __cplusplus
extern "C"
{
#endif

typedef  qurt_mutex_t ds_mux_crit_sect_type;

/*===========================================================================

                          PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

                          CRITICAL SECTION FUNCTION DECLARATIONS

===========================================================================*/

/*!
  @brief 
  Initialize a critical section

  @params[in] crit_sect_ptr
  Critical section pointer to be initialized, client just need to pass a 
  structure of type ds_mux_crit_sect_type, memory for any implementation 
  specific data is allocated by platforms. 

  @return
  NONE.

  @notes
  This function should have been called before any calls to
  PS_ENTER_CRIT_SECT() or PS_LEAVE_CRIT_SECT().
  After the critical section usage is complete, client should call
  PS_DESTROY_CRIT_SECTION to release the critical section and its 
  associated memory.
*/
#define DS_MUX_INIT_CRIT_SECTION  ds_mux_init_crit_section
void ds_mux_init_crit_section
(
  qurt_mutex_t*  crit_sect_ptr
);


/*!
  @brief 
  Enter a critical section

  @params[in] crit_sect_ptr
  Critical section to be entered. 

  @return
  NONE.

  @notes
  Critical section should be initialized prior to calling this function. 
*/


#define DS_MUX_ENTER_CRIT_SECTION ds_mux_enter_crit_section
void ds_mux_enter_crit_section
(
  qurt_mutex_t*  crit_sect_ptr
);


/*!
  @brief 
  Leave a critical section

  @params[in] crit_sect_ptr
  Critical section to be left.

  @return
  NONE.

  @notes
  Critical section should be initialized prior to calling this function. 
  If not already in critical section, the behavior is undefined.
*/
#define DS_MUX_LEAVE_CRIT_SECTION ds_mux_leave_crit_section
void ds_mux_leave_crit_section
(
  qurt_mutex_t*  crit_sect_ptr
);

/*!
  @brief 
  Destroy a critical section

  @params[in] crit_sect_ptr
  Critical section to be destroyed.

  @return
  NONE.

  @notes
  Should be only called on initialized critical sections. Otherwise
  behavior is undefined.
*/
#define DS_MUX_DESTROY_CRIT_SECTION ds_mux_destroy_crit_section
void ds_mux_destroy_crit_section
(
  qurt_mutex_t*  crit_sect_ptr
);


/*===========================================================================

                          LOGGING FUNCTION DECLARATIONS

===========================================================================*/

/**
  This macro is used to log recoverable errors with in Common Data code.
*/

#define LOG_MSG_ERROR( fmtString, x, y, z)  \
{  \
  ds_iot_log_err3(fmtString, x, y, z);\
}
#define LOG_MSG_ERROR_3( fmtString, x, y, z)\
{  \
  ds_iot_log_err3(fmtString, x, y, z);  \
}
#define LOG_MSG_ERROR_0( fmtString)   \
{  \
  ds_iot_log_err(fmtString);  \
}
#define LOG_MSG_ERROR_1( fmtString, x)  \
{  \
  ds_iot_log_err1(fmtString, x);  \
}
#define LOG_MSG_ERROR_2( fmtString, x, y)  \
{  \
  ds_iot_log_err2(fmtString, x, y);  \
}
#define LOG_MSG_ERROR_4( fmtString, a, b, c, d)   \
{  \
  ds_iot_log_err4(fmtString, a, b, c, d);   \
}
#define LOG_MSG_ERROR_5( fmtString, a, b, c, d, e)  \
{  \
  ds_iot_log_err5(fmtString, a, b, c, d, e);   \
}
#define LOG_MSG_ERROR_6( fmtString, a, b, c, d, e, f)  \
{  \
  ds_iot_log_err6(fmtString, a, b, c, d, e, f); 	  \
}


#define LOG_MSG_INFO1( fmtString, x, y, z)                            \
{                                                                     \
  ds_iot_log_high3(fmtString, x, y, z);   \
}

#define LOG_MSG_INFO1_3( fmtString, x, y, z)                            \
{                                                                     \
  ds_iot_log_high3(fmtString, x, y, z);                \
}
#define LOG_MSG_INFO1_0( fmtString)                                   \
{                                                                     \
  ds_iot_log_high(fmtString);                       \
}
#define LOG_MSG_INFO1_1( fmtString, x)                                \
{                                                                     \
  ds_iot_log_high1(fmtString, x);                    \
}
#define LOG_MSG_INFO1_2( fmtString, x, y)                             \
{                                                                     \
  ds_iot_log_high2(fmtString, x, y);                 \
}
#define LOG_MSG_INFO1_4( fmtString, a, b, c, d)                       \
{                                                                     \
  ds_iot_log_high4(fmtString, a, b, c, d);           \
}
#define LOG_MSG_INFO1_5( fmtString, a, b, c, d, e)                    \
{                                                                     \
  ds_iot_log_high5(fmtString, a, b, c, d, e);        \
}
#define LOG_MSG_INFO1_6( fmtString, a, b, c, d, e, f)                \
{                                                                     \
  ds_iot_log_high6(fmtString, a, b, c, d, e, f);    \
}
#define   DS_MUX_LOG_MSG_ERROR     LOG_MSG_ERROR
#define   DS_MUX_LOG_MSG_ERROR_0   LOG_MSG_ERROR_0
#define   DS_MUX_LOG_MSG_ERROR_1   LOG_MSG_ERROR_1
#define   DS_MUX_LOG_MSG_ERROR_2   LOG_MSG_ERROR_2
#define   DS_MUX_LOG_MSG_ERROR_3   LOG_MSG_ERROR_3
#define   DS_MUX_LOG_MSG_ERROR_4   LOG_MSG_ERROR_4
#define   DS_MUX_LOG_MSG_ERROR_5   LOG_MSG_ERROR_5
#define   DS_MUX_LOG_MSG_ERROR_6   LOG_MSG_ERROR_6

#define   DS_MUX_LOG_MSG_INFO1    LOG_MSG_INFO1
#define   DS_MUX_LOG_MSG_INFO1_0  LOG_MSG_INFO1_0
#define   DS_MUX_LOG_MSG_INFO1_1  LOG_MSG_INFO1_1
#define   DS_MUX_LOG_MSG_INFO1_2  LOG_MSG_INFO1_2
#define   DS_MUX_LOG_MSG_INFO1_3  LOG_MSG_INFO1_3
#define   DS_MUX_LOG_MSG_INFO1_4  LOG_MSG_INFO1_4
#define   DS_MUX_LOG_MSG_INFO1_5  LOG_MSG_INFO1_5
#define   DS_MUX_LOG_MSG_INFO1_6  LOG_MSG_INFO1_6


#define   DS_MUX_LOG_MSG_INFO2    LOG_MSG_INFO1
#define   DS_MUX_LOG_MSG_INFO2_0  LOG_MSG_INFO1_0
#define   DS_MUX_LOG_MSG_INFO2_1  LOG_MSG_INFO1_1
#define   DS_MUX_LOG_MSG_INFO2_2  LOG_MSG_INFO1_2
#define   DS_MUX_LOG_MSG_INFO2_3  LOG_MSG_INFO1_3
#define   DS_MUX_LOG_MSG_INFO2_4  LOG_MSG_INFO1_4
#define   DS_MUX_LOG_MSG_INFO2_5  LOG_MSG_INFO1_5
#define   DS_MUX_LOG_MSG_INFO2_6  LOG_MSG_INFO1_6

/*===========================================================================

                          TASK UTIL FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================
FUNCTION DS_MUX_FIND_SET_BITS()

DESCRIPTION
  Find the set bits in the dword array and return them in a list.
  Note this function relies on the (little) endianness of the sig_bit_field.
  The buffer variable is assumed to be long enough to handle all the
  set bits.
  
DEPENDENCIES
  None

RETURN VALUE
  the number of set bits.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 ds_mux_find_set_bits(uint32* sig_bit_field, uint32 length, uint32* buffer);



/*===========================================================================

                          MEM ALLOC/DE-ALLOC FUNCTION DECLARATIONS

===========================================================================*/

#define DS_MUX_MEM_ALLOC(buf_ptr,size,typecast_type) \
          PS_SYSTEM_HEAP_MEM_ALLOC(buf_ptr,size,typecast_type)

#define DS_MUX_MEM_FREE(mem_ptr)                     \
          free(mem_ptr)

/*===========================================================================

                          TIMER APIS FUNCTION DECLARATIONS

===========================================================================*/
#define TX_TIMER_INVALID_HANDLE        0
#define  DS_MUX_TIMER_INVALID_HANDLE   TX_TIMER_INVALID_HANDLE

typedef enum
{
  DS_MUX_TIMER_FAILURE = 0,                  /**< Timer operation failed. */
  DS_MUX_TIMER_SUCCESS                 = 1,  /**< Timer operation succeeded. */
} ds_mux_timer_error_type;


typedef  uint32                            ds_mux_timer_handle_type;
typedef  ds_mux_timer_error_type           ds_mux_timer_error_type;

 int ds_mux_timer_alloc
(
  qurt_timer_t* timer_handle,
  qurt_timer_attr_t* timer_attr,
  void (* callback) (void *),
  void *cb_param
);

int ds_mux_timer_start
(
   qurt_timer_t timer_handle,
  qurt_timer_attr_t* timer_attr,
  uint64 time
);

 int ds_mux_timer_free
(
  qurt_timer_t timer_handle
);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* DS_MUX_PLATFORM_H */
