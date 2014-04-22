/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.tx/6.0/api/products/scm/tz_syscalls.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
05/24/12    mm        Initial version.

===========================================================================*/
#include "syscalls.h"
/**
 * SCM Call to write to HW registers.
 *
 * @param[in] req tz_config_hw_for_ram_dump_req_t 
 *
 * @return  Status of the operation
 */	
int tz_config_hw_for_offline_ram_dump
(
   tz_config_hw_for_ram_dump_req_t *req
);

/**
 * Mutex free SCM Call to write to HW registers.
 * To be referenced directly by error handler only
 *
 * @param[in] req tz_config_hw_for_ram_dump_req_t 
 *
 * @return  Status of the operation
 */
int tz_config_hw_for_offline_ram_dump_STM
(
   tz_config_hw_for_ram_dump_req_t *req
);

/**
 * SCM Call for restoring the security configuration of device.
 *
 * @param[in] req   Device ID to be restore the security configuration 
 *
 * @return  Status of SYSCALL API
 */	
int scm_sec_restore_syscall (scm_device_e_type device);

/**
 * SCM Call for setting DLOAD cookie for RAM dumps on crash.
 *
 * @param[in] void 
 *
 * @return  Status of SYSCALL API
 */	
int tz_config_dload_mode_syscall (sec_dload_mode_buffer_t* req);

/**
 * Non Mutex SCM Call for setting DLOAD cookie for RAM dumps on crash. to be called in context of error handler only.
 *
 * @param[in] void 
 *
 * @return  Status of SYSCALL API
 */	
int tz_config_dload_mode_syscall_STM (sec_dload_mode_buffer_t* req);

