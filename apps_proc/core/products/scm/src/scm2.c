/*===========================================================================
   Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.tx/6.0/products/scm/src/scm2.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
08/16/12    rs        Initial version.

===========================================================================*/
#include <stddef.h>

#include "scm.h"
#include "qurt.h"
#include "qurt_memory.h"
#include "IxErrno.h"
#include "stringl.h"

extern int tz_smc2(unsigned long);

#define SCM_SUCCESS           0
#define SCM_ENOMEM            -5
#define SCM_EOPNOTSUPP        -4
#define SCM_EINVAL_ADDR       -3
#define SCM_EINVAL_ARG        -2
#define SCM_ERROR             -1
#define SCM_INTERRUPTED       1
#define SCM_V2_EBUSY          -12
#define SCM_EBUSY             -55

#define SCM_EBUSY_WAIT_MS     30
#define SCM_EBUSY_MAX_RETRY   67

#define SCM_N_EXT_ARGS         7
#define SCM_FIRST_EXT_ARG_IDX  3
#define SCM_SMC_ATOMIC_SYSCALL 31
#define SCM_N_REGISTER_ARGS   (SCM_MAX_ARGS - SCM_N_EXT_ARGS + 1)

#define SCM_SMC64_MASK         0x40000000
#define SCM_SMC_ATOMIC_MASK    0x80000000
#define SCM_IS_CALL_AVAIL_CMD  1

extern boolean g_products_tz_exist; 
extern qurt_mutex_t *p_scm_mutex;
static qurt_mem_region_t g_scm2_mem_handle;
static qurt_mem_region_t *p_scm2_mem_handle = NULL;

/**
 * Return an Error Code
 *
 * @param[in] err    Error Code
 *
 * @return  Returns an error code.
 */
static int scm2_remap_error(int err) 
{
   switch (err) 
   {
     case SCM_SUCCESS:     return E_SUCCESS;
     case SCM_ENOMEM:      return -E_NO_MEMORY;
     case SCM_EOPNOTSUPP:  return -E_NOT_SUPPORTED;
     case SCM_EINVAL_ADDR: return -E_BAD_ADDRESS;
     case SCM_EINVAL_ARG:  return -E_INVALID_ARG;
     case SCM_ERROR:       return -E_FAILURE;
     case SCM_INTERRUPTED: return -E_INTERRUPTED;
     case SCM_EBUSY:       return -E_BUSY;
     case SCM_V2_EBUSY:    return -E_BUSY;
   }

   return err;
}


/* This will be set to specify SMC32 or SMC64 */
static uint32 g_scm_version_mask;

static enum scm_interface_version {
   SCM_UNKNOWN,
   SCM_LEGACY,
   SCM_ARMV8_32,
   SCM_ARMV8_64,
} g_scm_version = SCM_UNKNOWN;

typedef struct scm_extra_arg {
   union {
      uint32 args32[SCM_N_EXT_ARGS];
      uint64 args64[SCM_N_EXT_ARGS];
   }extra_args;
}scm_extra_arg_t;

/*
 * If there are more than SCM_N_REGISTER_ARGS, allocate a buffer and place
 * the additional arguments in it. The extra argument buffer will be
 * pointed to by X5.
 */
static int scm_alloc_extra_arg_buffer(scm_desc_t *desc)
{
   int i, j;
   int result = 0;
   int ret    = 0;
   qurt_mem_region_attr_t attr;
   scm_extra_arg_t        *argbuf = NULL;
   int                    arglen = desc->arginfo & 0xf;
   int                    argbuflen = sizeof(scm_extra_arg_t);

   argbuflen = (argbuflen + 0xfff) & ~0xfff;

   desc->x5 = desc->args[SCM_FIRST_EXT_ARG_IDX];

   if (arglen <= SCM_N_REGISTER_ARGS) 
   {
      desc->extra_arg_buf = NULL;
      return 0;
   }

   qurt_mem_region_attr_init(&attr);

   attr.cache_attribs = QURT_MEM_CACHE_OUTER_INNER_UNCACHED;
   attr.mapping_type  = QURT_MEM_MAPPING_VIRTUAL; //This will allocate physically contiguous memory 
   attr.permission    = QURT_PERM_FULL;

   result = qurt_mem_region_create(&g_scm2_mem_handle,
                                   argbuflen,
                                   qurt_mem_default_pool,
                                   &attr);

   if (result != QURT_EOK)
   {
      return -E_NO_MEMORY;
   }

   result = qurt_mem_region_attr_get(g_scm2_mem_handle, &attr);
   if (QURT_EOK == result)
   {
      desc->x5            = attr.physaddr;
      desc->extra_arg_buf = (void *)attr.virtaddr;
   }
   else
   {
      return -E_NO_MEMORY;
   }

   argbuf = (scm_extra_arg_t *)(desc->extra_arg_buf);

   j = SCM_FIRST_EXT_ARG_IDX;
   if (g_scm_version == SCM_ARMV8_64)
   {
      for (i = 0; i < SCM_N_EXT_ARGS; i++)
      {
         argbuf->extra_args.args64[i] = desc->args[j++];
      }
   }
   else
   {
      for (i = 0; i < SCM_N_EXT_ARGS; i++)
      {
         argbuf->extra_args.args32[i] = desc->args[j++];
      }
   }

   return 0;
}

static int scm_free_extra_arg_buffer(scm_desc_t *desc)
{
   if(desc && desc->extra_arg_buf)
   {
      (void)qurt_mem_region_delete(g_scm2_mem_handle);

      desc->extra_arg_buf = NULL;
   }

   return 0;
}

/*TODO: Supporting only 32-bit targets here */

static int scm_call_armv8_64(uint64 x0, uint64 x1, uint64 x2, uint64 x3, uint64 x4, uint64 x5,
            uint64 *ret1, uint64 *ret2, uint64 *ret3)
{
   return 0;
}

static int scm_call_armv8_32(uint32 w0, uint32 w1, uint32 w2, uint32 w3, uint32 w4, uint32 w5,
            uint64 *ret1, uint64 *ret2, uint64 *ret3)
{
   int ret;

   uint32 args[6];
   uint64 *rets[3];
  
   args[0] = w0;
   args[1] = w1;
   args[2] = w2;
   args[3] = w3;
   args[4] = w4;
   args[5] = w5;

   rets[0] = ret1;
   rets[1] = ret2;
   rets[2] = ret3;

   ret = tz_smc2((unsigned long)args);

   /* After SMC call, the return values are stored in extra[0-2] */
   if (rets[0])
      *(rets[0]) = args[0];
   if (rets[1])
      *(rets[1]) = args[1];
   if (rets[2])
      *(rets[2]) = args[2];

   return ret;
}

int scm_is_armv8(void)
{
   int ret;
   uint64 ret1, x0;

   if (!g_products_tz_exist)
      return 0;

   if (g_scm_version != SCM_UNKNOWN)
      return (g_scm_version == SCM_ARMV8_32) || (g_scm_version == SCM_ARMV8_64);

   /*
    * This is a one time check that runs on the first ever
    * invocation of scm_is_armv8. We might be called in atomic
    * context so no mutexes etc. Also, we can't use the scm_call2
    * or scm_call2_APIs directly since they depend on this init.
    */

   /* First try a SMC64 call */
   g_scm_version = SCM_ARMV8_64;
   ret1 = 0;
   x0 = SCM_SIP_FNID(TZBSP_SVC_INFO, SCM_IS_CALL_AVAIL_CMD) | SCM_SMC_ATOMIC_MASK;

   ret = scm_call_armv8_64(x0 | SCM_SMC64_MASK, SCM_ARGS(1), x0, 0, 0, 0,
              &ret1, NULL, NULL);

   if (ret || !ret1)
   {
      /* Try SMC32 call */
      ret1 = 0;
      ret = scm_call_armv8_32(x0, SCM_ARGS(1), x0, 0, 0, 0,
                 &ret1, NULL, NULL);
      if (ret || !ret1)
         g_scm_version = SCM_LEGACY;
      else
         g_scm_version = SCM_ARMV8_32;
   }
   else
   {
      g_scm_version_mask = SCM_SMC64_MASK;
   }

   return (g_scm_version == SCM_ARMV8_32) ||
          (g_scm_version == SCM_ARMV8_64);
}

static int scm_call2_common(uint64 fn_id, scm_desc_t *desc, int lock)
{
   int         arglen;
   int         ret, retry_count = 0;
   uint64      x0;
   qurt_time_t ticks = 0;

   if (!scm_is_armv8())
      return -E_NOT_SUPPORTED;

   if(!desc)
      return -E_INVALID_ARG;

   if(lock)
   {
      ticks = qurt_timer_convert_time_to_ticks(SCM_EBUSY_WAIT_MS, QURT_TIME_MSEC);
      x0 = fn_id | g_scm_version_mask;
   }
	else
	{
      x0 = fn_id | g_scm_version_mask | SCM_SMC_ATOMIC_MASK;
	}

   arglen = desc->arginfo & 0xF;

   ret = scm_alloc_extra_arg_buffer(desc);
   if (ret)
      return ret;

   do 
   {
      if(lock) 
         qurt_mutex_lock(p_scm_mutex);

      desc->ret[0] = desc->ret[1] = desc->ret[2] = 0;

      if (g_scm_version == SCM_ARMV8_64)
         ret = scm_call_armv8_64(x0, desc->arginfo,
                    desc->args[0], desc->args[1],
                    desc->args[2], desc->x5,
                    &desc->ret[0], &desc->ret[1],
                    &desc->ret[2]);
      else
         ret = scm_call_armv8_32(x0, desc->arginfo,
                    desc->args[0], desc->args[1],
                    desc->args[2], desc->x5,
                    &desc->ret[0], &desc->ret[1],
                    &desc->ret[2]);

      if(lock)
      {
         qurt_mutex_unlock(p_scm_mutex);

         if (ret == SCM_V2_EBUSY)
            qurt_thread_sleep_ext(ticks);
      }
      else
         /* For STM mode, no need to retry */
         break;

   }  while (ret == SCM_V2_EBUSY && (retry_count++ < SCM_EBUSY_MAX_RETRY));

   if (arglen > SCM_N_REGISTER_ARGS)
      scm_free_extra_arg_buffer(desc);

   if (ret < 0)
      return scm2_remap_error(ret);

   return 0;
}

/**
 * scm_call2() - Invoke a syscall in the secure world
 * @fn_id: The function ID for this syscall
 * @desc: Descriptor structure containing arguments and return values
 *
 * Sends a command to the SCM and waits for the command to finish processing.
 * This should *only* be called in pre-emptible context.
 *
 * A note on cache maintenance:
 * Note that any buffers that are expected to be accessed by the secure world
 * must be flushed before invoking scm_call and invalidated in the cache
 * immediately after scm_call returns. An important point that must be noted
 * is that on ARMV8 architectures, invalidation actually also causes a dirty
 * cache line to be cleaned (flushed + unset-dirty-bit). Therefore it is of
 * paramount importance that the buffer be flushed before invoking scm_call2,
 * even if you don't care about the contents of that buffer.
 *
 * Note that cache maintenance on the argument buffer (desc->args) is taken care
 * of by scm_call2; however, callers are responsible for any other cached
 * buffers passed over to the secure world.
*/
int scm_call2(uint32 fn_id, scm_desc_t *desc)
{
   if (!g_products_tz_exist )
      return 0;

   return scm_call2_common(fn_id, desc, 1);
}

int scm_call2_STM(uint32 fn_id, scm_desc_t *desc)
{
   if (!g_products_tz_exist )
      return 0;

   return scm_call2_common(fn_id, desc, 0);
}

/*
 * Set the cold/warm boot address for one of the CPU cores.
 */
int scm_set_boot_addr(unsigned long addr, unsigned int flags)
{
   scm_desc_t desc = {0};

   if (!scm_is_armv8())
      return -E_NOT_SUPPORTED;

   flags = flags | SCM_FLAG_HLOS;

   desc.args[0] = addr;
   desc.args[1] = ~0ULL;
   desc.args[2] = ~0ULL;
   desc.args[3] = ~0ULL;
   desc.args[4] = ~0ULL;
   desc.args[5] = flags;
   desc.arginfo = SCM_ARGS(6);

   return scm_call2_STM(SCM_SIP_FNID(SCM_SVC_BOOT, SCM_BOOT_ADDR_MC), &desc);
}
