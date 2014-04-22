#ifndef SECAPI_INTERNAL_H
#define SECAPI_INTERNAL_H
/** 
  @file secapi_internal.h
 */
/*===========================================================================
Copyright (c) {2018} Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //components/rel/core.mpss/3.10/securemsm/inc/secapi_internal.h#1 $ 
  $DateTime: 2019/03/17 23:07:12 $ 
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
13-02-18  sahaj    Initial Version (FR#44835)
=============================================================================*/
#include "secerrno.h"

/**
Callback function pointer for SECAPI_GET_RANDOM.
*/
typedef void (*secapi_get_random_cb_fptr_type)( void* );

/**
  Registers a callback that allows the SECRND task to notify the calling 
  task when random data is available.

  In several different scenarios, a task may require random numbers before the 
  SECRND task has been initialized. In such cases, the call to 
  secapi_get_random() returns E_AGAIN. It is the calling task's 
  responsibility to register a callback. This allows the SECRND task to 
  notify the calling task when random data is available. Since the callback 
  function is fired from the SECRND task context, users should not execute 
  any blocking operations that can cause the watchdog to time out. 

  The calling task should not call secapi_get_random() in the callback function.
  
  @param[in] get_random_cb User-defined callback function.
  @param[in] usr_data      User-defined data (can be NULL).

  @return
  E_SUCCESS     - Successful. \n
  E_INVALID_ARG - Callback function pointer is null. \n
  E_NO_MEMORY   - Reached the maximum number of callback function pointers that can be registered. \n 
  E_FAILURE     - SECRND task has already been initialized.

  @dependencies
  None.
*/
extern secerrno_enum_type secapi_get_random_set_cb
(
  secapi_get_random_cb_fptr_type  get_random_cb,
  void*                           usr_data
);

#endif /* SECAPI_INTERNAL_H */
