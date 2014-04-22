/*===========================================================================
   Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.tx/6.0/products/scm/src/tz_syscalls.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
05/24/12    mm        Initial version.

===========================================================================*/
#include "comdef.h"
#include "msmhwiobase.h"
#include "scm.h"
#include "syscalls.h"
#include "msg.h"
#include "stringl.h"
#include "err.h"

#include "qurt.h"
#include "qurt_memory.h"

#define E_FAILURE 1

#define TCSR_TCSR_REGS_REG_BASE         (CORE_TOP_CSR_BASE + 0x00037000)
#define HWIO_TCSR_BOOT_MISC_DETECT_ADDR (TCSR_TCSR_REGS_REG_BASE + 0x00006100)

#define SEC_RESTORE_MSG_MED(x_fmt, a) \
    MSG_SPRINTF_1 (MSG_SSID_HSUSB, MSG_LEGACY_MED, x_fmt, a)

#define SEC_RESTORE_MSG_ERROR(x_fmt, a) \
    MSG_SPRINTF_1 (MSG_SSID_HSUSB, MSG_LEGACY_ERROR, x_fmt, a)

typedef struct sec_restore_cmd_buffer_s
{
   uint32 device_id;
   uint32 spare;
}sec_restore_cmd_buffer_t;

static qurt_mem_region_t mem_handle;

/**
 * SCM Call to disable SDI on watchdog reset. Used by chipPower routine.
 *
 * @param[in] req Flag to disable SDI on watchdog reset
 *
 * @return  Status of SCM call
 */   
int tz_config_hw_for_offline_ram_dump (tz_config_hw_for_ram_dump_req_t *req)
{
  /* Syscall for Offline Ram Dump Request */
  return wdt_scm_call(req);

}

/**
 * Mutex free SCM Call to clear BOOT Partition. To be referenced directly by error handler only
 *
 * @param[in] req    QFPROM Write multiple rows Diag Packet
 *
 * @return  Status of QFPROM api
 */   
int tz_config_hw_for_offline_ram_dump_STM(tz_config_hw_for_ram_dump_req_t *req)
{
  /* Syscall for Offline Ram Dump Request */
  return wdt_scm_call_STM(req);
}


/**
 * SCM Call for forcing dload mode as requested.
 *
 * @param[in] cmd_buffer   DLOAD cookie value to write into TCSR_BOOT_MISC_DETECT HW register
 *
 * @return  Status of SYSCALL API
 */   
int tz_config_dload_mode_syscall (sec_dload_mode_buffer_t* cmd_buffer)
{
   int scm_result = E_FAILURE;

   scm_result = scm_io_write(HWIO_TCSR_BOOT_MISC_DETECT_ADDR, cmd_buffer->dload_mode_req);

   if (scm_result)
   {
      ERR_FATAL("SCM: error result %u", scm_result, 0, 0);
      scm_result = -E_FAILURE;
   }

   return scm_result;
}

/**
 * Mutex free SCM Call for forcing dload mode as requested - to be used in context if err handler.
 *
 * @param[in] cmd_buffer   DLOAD cookie value to write into TCSR_BOOT_MISC_DETECT HW register
 *
 * @return  Status of SYSCALL API
 */   
int tz_config_dload_mode_syscall_STM (sec_dload_mode_buffer_t* cmd_buffer)
{
   int scm_result = E_FAILURE;

   scm_result = scm_io_write_STM(HWIO_TCSR_BOOT_MISC_DETECT_ADDR, cmd_buffer->dload_mode_req);

   if (scm_result)
   {
      ERR_FATAL("SCM: error result %u", scm_result, 0, 0);
      scm_result = -E_FAILURE;
   }

   return scm_result;
}

#if 0 // not being used as of now
/**
 * SCM Call for restoring the security configuration of device.
 *
 * @param[in] req   Device ID to be restore the security configuration 
 *
 * @return  Status of SYSCALL API
 */   
int scm_sec_restore_syscall (scm_device_e_type device)
{
   int                      ret;
   qurt_mem_region_attr_t   attr;
   uint32                   result = E_FAILURE;
   int                      scm_result = E_FAILURE;
   sec_restore_cmd_buffer_t *cmd;
   size_t                   len = sizeof(sec_restore_cmd_buffer_t);

   /* Create Uncached Region for DEVICE ID and SYSCALL API Status */
   cmd = NULL;

   qurt_mem_region_attr_init(&attr);

   attr.cache_attribs = QURT_MEM_CACHE_OUTER_INNER_UNCACHED;
   attr.mapping_type  = QURT_MEM_MAPPING_VIRTUAL;
   attr.permission    = QURT_PERM_FULL;

   ret = qurt_mem_region_create(&mem_handle,
                                len,
                                qurt_mem_default_pool,
                                &attr);
   if (ret != QURT_EOK)
   {
      ERR_FATAL("SCM: failed to get memory %d, %u", ret, mem_handle, 0);
      return -E_FAILURE;
   }

   ret = qurt_mem_region_attr_get(mem_handle, &attr);

   if (ret == QURT_EOK)
   {
      cmd = (sec_restore_cmd_buffer_t *)attr.virtaddr;
   }

   if (cmd)
   {
      memset(cmd, 0, len);
      cmd->device_id = device;
      cmd->spare = 0;

      scm_result = scm_call(TZ_SVC_MEMORY_PROTECTION, TZ_RESTORE_SEC_CFG_CMD_ID,
                            cmd, sizeof(sec_restore_cmd_buffer_t),
                            &result, sizeof(result)); 
  }

  if (scm_result)
  {
     qurt_mem_region_delete(mem_handle);
     ERR_FATAL("SCM: error result %u, %u", scm_result, result, 0);
     return -E_FAILURE;
  }

  qurt_mem_region_delete(mem_handle);
  return scm_result;
}
#endif
