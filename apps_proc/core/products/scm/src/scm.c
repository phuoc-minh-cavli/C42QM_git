/*===========================================================================
   Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.tx/6.0/products/scm/src/scm.c#1 $
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

extern int tz_syscall(unsigned long addr);
extern int tz_smc(unsigned long, unsigned int, unsigned long, unsigned long);

/**   
   SCM Packet Layout
---------------------- 
|   command header   |
----------------------
|   command buffer   |
----------------------
|   response header  |
----------------------
|   response buffer  |
----------------------
**/

#define SCM_CLASS_REGISTER  (0x2 << 8)
#define SCM_MASK_IRQS       (1 << 5)
#define SCM_ATOMIC(svc, cmd, n) (((((svc) << 10)|((cmd) & 0x3ff)) << 12) | \
                                 SCM_CLASS_REGISTER | \
                                 SCM_MASK_IRQS | \
                                 (n & 0xf))
#define SCM_CREATE_CMD(s, c) ((((s) & 0x3ff) << 10) | ((c) & 0x3ff))

#define SCM_SUCCESS               0
#define SCM_MUTEX_ALLOC_FAIL      1 
#define SCM_UNKNOWN_ERR          -1
#define SCM_OP_NOT_ALLOWED       -2
#define SCM_OP_NOT_AVAIL         -3
#define SCM_OP_NOT_IMPLEMENTED   -4
#define SCM_OP_CANCELLED         -5
#define SCM_MEM_ALLOC_FAIL       -15
#define SCM_UNKOWN_ARG           -16
#define SCM_ARG_OUT_OF_RANGE     -17
#define SCM_BAD_ARG_PTR          -18

boolean g_products_tz_exist= TRUE; 

/**
* Structure for Command Buffer
**/
typedef struct scm_command_s 
{
   uint32 len;               /** Length of command header & buffer **/
   uint32 command_start_off; /** Offset of command buffer **/
   uint32 response_hdr_off;  /** offset of response header **/
   uint32 command_id;        /** command id to execute on command buffer **/
} scm_command_t;

/**
* Structure for Response Buffer
**/   
typedef struct scm_response_s 
{
   uint32 len;                /** Length of response header & buffer **/
   uint32 response_start_off; /** Offset of response buffer **/
   uint32 status;            /** response status (1 on completion) **/
} scm_response_t;

/**
* Qurt Synchronization object for Critical Section
**/
static qurt_mutex_t scm_mutex;
qurt_mutex_t *p_scm_mutex = NULL;

/**Qurt Memory Handle for Command Header**/
static qurt_mem_region_t mem_handle;
static qurt_mem_region_t g_mem_handle;

/**
* set a fixed CMD buffer len to avoid allocating on each sysCall
*/
#define CMD_BUFFER_LEN 200

/* Fixme: these need to move to QURT/TX */
static inline int request_smc(unsigned long addr) 
{
   int ret = tz_syscall(addr);

   return ret;
}

#define SCM_IO_CALL(cmd, ctx, addr, val) tz_smc(cmd, ctx, addr, val)

/**
* physical address mapped for CMD buffer
*/
static unsigned int g_cmd_buffer_phys_add;

/**
* Global COMMAND buffer to be re-used for sysCALLs
*/
static scm_command_t *g_sec_cmd = NULL;

/**
* allocate memory for command & response buffer
*/
static int create_command_buffer(void)
{
  //Allocate physically contiguous memory
  int                    result = 0;
  int                    ret    = 0;
  qurt_mem_region_attr_t attr;

  do
  {
     qurt_mem_region_attr_init(&attr);

     attr.cache_attribs = QURT_MEM_CACHE_OUTER_INNER_UNCACHED;
     attr.mapping_type  = QURT_MEM_MAPPING_VIRTUAL; //This will allocate physically contiguous memory 
     attr.permission    = QURT_PERM_FULL;

     result = qurt_mem_region_create(&g_mem_handle,
                                     CMD_BUFFER_LEN,
                                     qurt_mem_default_pool,
                                     &attr);

     if (result != QURT_EOK)
     {
        ret = -1;
        break;
     }

     result = qurt_mem_region_attr_get(g_mem_handle, &attr);
     if (QURT_EOK == result)
     {
        g_sec_cmd = (scm_command_t *)attr.virtaddr;
        g_cmd_buffer_phys_add = attr.physaddr;
     }
     else
     {
        ret = -1;
     }
  } while(0);

  return ret;
}

/**
* Create Critical Section for serialization since TZ is Single Threaded
**/
static int scm_create_mutex() 
{
   /**
   * Create Synchronization object
   **/
   qurt_mutex_init(&scm_mutex);
   p_scm_mutex = &scm_mutex;

   return 0;
}

/**
 * Allocate command and response headers and buffers
 *
 * @param[in] cmd_size    Command buffer size
 * @param[in] resp_size   Response buffer size
 *
 * @return  A pointer to the command struct
 */   
static scm_command_t *create_command(size_t cmd_size, size_t resp_size, unsigned int *cmd_phys_addr)
{
   qurt_mem_region_attr_t attr;
   scm_command_t          *cmd = NULL;
   size_t                 len = sizeof(scm_command_t) + sizeof(scm_response_t) + cmd_size + resp_size;
   int                    result;

   //Allocate physically contiguous memory
   qurt_mem_region_attr_init(&attr);

   attr.cache_attribs = QURT_MEM_CACHE_OUTER_INNER_UNCACHED;
   attr.mapping_type  = QURT_MEM_MAPPING_VIRTUAL; //This will allocate physically contiguous memory 
   attr.permission    = QURT_PERM_FULL;

   result = qurt_mem_region_create(&mem_handle,
                                   len,
                                   qurt_mem_default_pool,
                                   &attr);

   if (result == QURT_EOK)
   {
      result = qurt_mem_region_attr_get(mem_handle, &attr);
      if (QURT_EOK == result)
      {
         cmd = (scm_command_t *)attr.virtaddr;
         cmd->len = len;
         cmd->command_start_off = sizeof(scm_command_t);
         cmd->response_hdr_off = cmd->command_start_off + cmd_size;
         *cmd_phys_addr = attr.physaddr;
      }
   }
   
   return cmd;
}

/**
 * Get a pointer to the response header based on response offset in cmd
 *
 * @param[in] cmd    Command structure pointer
 *
 * @return  Returns a pointer to the response header of an SCM Packet.
 */
static void *scm_get_response(const scm_command_t *cmd)
{
   return (void *)((uint32)cmd + (uint32)cmd->response_hdr_off);
}

/**
 * Get a pointer to a command buffer
 *
 * @param[in] cmd    Command structure pointer
 *
 * @return  Returns a pointer to the command buffer of an SCM Packet.
 */
static void *scm_get_command_buffer(const scm_command_t *cmd)
{
   return (void*)((uint32)cmd + sizeof(*cmd));
}

/**
 * Get a pointer to a response buffer
 *
 * @param[in] rsp    Response structure pointer
 *
 * @return  Returns a pointer to a response buffer of an SCM Packet.
 */
static void *scm_get_response_buffer(const scm_response_t *rsp)
{
   return (void *)((uint32)rsp + (uint32)rsp->response_start_off);
}

/**
 * Return an Error Code
 *
 * @param[in] err    Error Code
 *
 * @return  Returns an error code.
 */
static int scm_return_value(int err) 
{
   switch (err) {
   case SCM_SUCCESS:
      return E_SUCCESS;
   case SCM_MUTEX_ALLOC_FAIL:
      return -E_FAILURE;
   case SCM_UNKNOWN_ERR:
      return -E_FAILURE;
   case SCM_OP_NOT_ALLOWED:
      return -E_NOT_ALLOWED;
   case SCM_OP_NOT_AVAIL:
      return -E_NOT_AVAILABLE;
   case SCM_OP_NOT_IMPLEMENTED:
      return -E_NOT_SUPPORTED;
   case SCM_OP_CANCELLED:
      return -E_CANCELED;
   case SCM_MEM_ALLOC_FAIL:
      return -E_NO_MEMORY;
   case SCM_UNKOWN_ARG:
      return -E_INVALID_ARG;
   case SCM_ARG_OUT_OF_RANGE:
      return -E_OUT_OF_RANGE;
   case SCM_BAD_ARG_PTR:
      return -E_BAD_ADDRESS;
   }
   return -E_FAILURE;
}

int wdt_scm_call_init(void);

int scm_initialize(void)
{
   int ret = 0; //scm return code

   do
   {
      //Create mutex Object for Critical Section if it doesn't already exist
      if (!p_scm_mutex)
      {
         ret = scm_create_mutex(); //create mutex
         if (ret)
         {
            ret = scm_return_value(SCM_MUTEX_ALLOC_FAIL); //mutex creation failed
            break;
         }
      }

      if(!scm_is_armv8())
      {
      /* initialize global CMD buffer once*/
      ret = create_command_buffer();
      if (ret) 
      {
         ret = scm_return_value(SCM_MEM_ALLOC_FAIL); //mutex creation failed
         break;
      }

      //call wdog scm struct init
      ret = wdt_scm_call_init();
      }
   } while(0);

   return ret; 
}

/**
 * Make an SMC call based on the allocated SCM Packet
 * Uses mutex for synchronization b/w various calls
 *
 * @param[in]  svc_id     service ID
 * @param[in]  cmd_id     command ID
 * @param[in]  cmd_buf    command buffer
 * @param[in]  cmd_len    cmd buffer length
 * @param[out] resp_buf   pointer to response buffer
 * @param[in]  resp_len   response buffer length
 *
 * @return  Returns the status of the smc call (0 for success).
 */
int scm_call(uint32     svc_id, 
             uint32     cmd_id, 
             const void *cmd_buf, 
             size_t     cmd_len,
             void       *resp_buf, 
             size_t     resp_len) 
{
   int ret = E_FAILURE; //scm return code

   //bypass scm_call when tz doesn't exist, this is needed for standalone TN APPS for pre-si work. 
   if (!g_products_tz_exist )
   {
      return scm_return_value(SCM_SUCCESS);
   }

   //Create sync Object for Critical Section if it doesn't already exist
   if (!p_scm_mutex)
   {
      ret = scm_create_mutex(); //create mutex
      if (ret) 
      {
         return scm_return_value(SCM_MUTEX_ALLOC_FAIL); //mutex creation failed
      }
   }

   //Enter Critical Section
   qurt_mutex_lock(p_scm_mutex);

   //make SCM call
   ret = scm_call_STM(svc_id, cmd_id, cmd_buf, cmd_len, resp_buf, resp_len);

   //Exit Critical Section
   qurt_mutex_unlock(p_scm_mutex);

   return ret;
}

/**
 * Make a SCM (mutex free) call to be used directly via error handlers only
 *
 * @param[in]  svc_id     service ID
 * @param[in]  cmd_id     command ID
 * @param[in]  cmd_buf    command buffer
 * @param[in]  cmd_len    cmd buffer length
 * @param[out] resp_buf   pointer to response buffer
 * @param[in]  resp_len   response buffer length
 *
 * @return  Returns the status of the smc call (0 for success).
 */
int scm_call_STM(uint32     svc_id, 
                 uint32     cmd_id, 
                 const void *cmd_buf, 
                 size_t     cmd_len,
                 void       *resp_buf, 
                 size_t     resp_len) 
{  
   int ret; //scm return code
   scm_response_t *rsp;

   if(scm_is_armv8())
   {
      return -E_NOT_ALLOWED;
   }

   if (!g_sec_cmd) 
   {
      //Exit - don't allocate again
      return -E_NO_MEMORY; //allocation failure
   }

   //clear the global command buffer
   memset(g_sec_cmd, 0, CMD_BUFFER_LEN-1);

   /* initialize CMD buffer */
   g_sec_cmd->len = sizeof(scm_command_t) + sizeof(scm_response_t) + cmd_len + resp_len;
   if (CMD_BUFFER_LEN <= g_sec_cmd->len)
   {
     //Exit - don't allocate again
     return -E_NO_MEMORY; //allocation failure
   }

   g_sec_cmd->command_start_off = sizeof(scm_command_t);
   g_sec_cmd->response_hdr_off = g_sec_cmd->command_start_off + cmd_len;
   g_sec_cmd->command_id = (svc_id << 10) | cmd_id;

   if (cmd_buf)
     memscpy(scm_get_command_buffer(g_sec_cmd), (CMD_BUFFER_LEN - sizeof(scm_command_t) -1), cmd_buf, cmd_len);

   //make smc call based on command buffer(done in L4)
   ret = request_smc(g_cmd_buffer_phys_add);

   if (SCM_SUCCESS != ret)
   {  
      //smc call failure
      return scm_return_value(ret);
   }

   rsp = (scm_response_t *)scm_get_response(g_sec_cmd);
   rsp->len = sizeof(scm_response_t) + resp_len;
   rsp->response_start_off = sizeof(scm_response_t);
   rsp->status = 0;

   //response header offset will be > 0 when response is needed
   if (resp_buf && g_sec_cmd->response_hdr_off > 0) 
   {
      //copy response buffer on successful smc call
      memscpy(resp_buf, resp_len, scm_get_response_buffer(rsp), resp_len);
      rsp->status = 1;
   }
#if 0
   else if (!resp_buf && g_sec_cmd->response_hdr_off > 0)
   {  
      //desired buffer to write response is not available.
      ret = -E_FAILURE;
   }
#endif

   return scm_return_value(ret);
}

scm_command_t *wdt_cmd;
unsigned int wdt_cmd_buffer_phys_add;
   
int wdt_scm_call_init(void)
{
   wdt_cmd = create_command(sizeof(tz_config_hw_for_ram_dump_req_t), 4, &wdt_cmd_buffer_phys_add);
   
   if (!wdt_cmd) 
   {
      return -E_NO_MEMORY; //allocation failure
   }

   wdt_cmd->command_id = (TZBSP_SVC_BOOT << 10) | TZBSP_CONFIG_HW_FOR_RAM_DUMP_ID;
   
   return SCM_SUCCESS;
}

int wdt_scm_call(const void *cmd_buf)
{
   int ret = 0;

   //Enter Critical Section
   qurt_mutex_lock(p_scm_mutex);  
   
   ret = wdt_scm_call_STM(cmd_buf);
   
   //Exit Critical Section
   qurt_mutex_unlock(p_scm_mutex);

   return ret;
}

/**
 * Make a WDOG SCM (mutex free) call to be referenced directly via error handlers only
 *
 * @param[in]  cmd_buf    command buffer
 * @return  Returns the status of the smc call (0 for success).
 */
int wdt_scm_call_STM(const void *cmd_buf)
{
   int    ret = 0;

   if (!g_products_tz_exist )
   {
      return scm_return_value(SCM_SUCCESS);
   }

   if(!scm_is_armv8())
   {
      size_t len = sizeof(tz_config_hw_for_ram_dump_req_t);

      memscpy(scm_get_command_buffer(wdt_cmd), (len+4), cmd_buf, len);

      //make smc call based on command buffer(done in L4)
      ret = request_smc(wdt_cmd_buffer_phys_add);

      return scm_return_value(ret);
   }
   else
   {
      scm_desc_t desc;
      desc.args[0] = 1;
      desc.args[1] = 0;
      desc.arginfo = SCM_ARGS(2);

      return scm_call2_STM(SCM_SIP_FNID(TZBSP_SVC_BOOT, TZBSP_CONFIG_HW_FOR_RAM_DUMP_ID), &desc);
   }
}

int scm_io_write(unsigned int address, unsigned int val)
{
   int          ret = 0;
   unsigned int cmd;

   //bypass scm_call when tz doesn't exist, this is needed for standalone TN APPS for pre-si work.
   if (!g_products_tz_exist )
   {
      return scm_return_value(SCM_SUCCESS);
   }

   if (!scm_is_armv8()) 
   {
   cmd = SCM_ATOMIC(TZBSP_SVC_IO, TZBSP_IO_WRITE, 2);

   //Enter Critical Section
   qurt_mutex_lock(p_scm_mutex);  

   ret = SCM_IO_CALL(cmd, (unsigned int)&ret, address, val);

   //Exit Critical Section
   qurt_mutex_unlock(p_scm_mutex);

   return scm_return_value(ret);
   }
   else
   {
      scm_desc_t desc;

      desc.args[0] = address;
      desc.args[1] = val;
      desc.arginfo = SCM_ARGS(2);

      return scm_call2(SCM_SIP_FNID(TZBSP_SVC_IO, TZBSP_IO_WRITE), &desc);
   }
}

int scm_io_write_STM(unsigned int address, unsigned int val)
{
   unsigned int cmd;
   int          ret = 0;

   //bypass scm_call when tz doesn't exist, this is needed for standalone TN APPS for pre-si work.
   if (!g_products_tz_exist )
   {
      return scm_return_value(SCM_SUCCESS);
   }

   if (!scm_is_armv8()) 
   {
   cmd = SCM_ATOMIC(TZBSP_SVC_IO, TZBSP_IO_WRITE, 2);

   ret = SCM_IO_CALL(cmd, (unsigned int)&ret, address, val);

   return scm_return_value(ret);
   }
   else
   {
      scm_desc_t desc;

      desc.args[0] = address;
      desc.args[1] = val;
      desc.arginfo = SCM_ARGS(2);

      return scm_call2_STM(SCM_SIP_FNID(TZBSP_SVC_IO, TZBSP_IO_WRITE), &desc);
   }
}

unsigned int scm_io_read(unsigned int address)
{
   unsigned int val = 0;

   //bypass scm_call when tz doesn't exist, this is needed for standalone TN APPS for pre-si work.
   if (!g_products_tz_exist )
   {
      return 0;
   }

   if (!scm_is_armv8()) 
   {
      unsigned int cmd;
      cmd = SCM_ATOMIC(TZBSP_SVC_IO, TZBSP_IO_READ, 1);

      //Enter Critical Section
      qurt_mutex_lock(p_scm_mutex);  

      val = SCM_IO_CALL(cmd, (unsigned int)&val, address, 0);

      //Exit Critical Section
      qurt_mutex_unlock(p_scm_mutex);
   }
   else
   {
      scm_desc_t desc; 

      desc.args[0] = address;
      desc.arginfo = SCM_ARGS(1);

      (void)scm_call2(SCM_SIP_FNID(TZBSP_SVC_IO, TZBSP_IO_READ), &desc);

      val = desc.ret[0];
   }

   return val;
}

unsigned int scm_io_read_STM(unsigned int address)
{
   unsigned int val = 0;

   //bypass scm_call when tz doesn't exist, this is needed for standalone TN APPS for pre-si work.
   if (!g_products_tz_exist )
   {
      return 0;
   }

   if (!scm_is_armv8()) 
   {
      unsigned int cmd;
      cmd = SCM_ATOMIC(TZBSP_SVC_IO, TZBSP_IO_READ, 1);

      val = SCM_IO_CALL(cmd, (unsigned int)&val, address, 0);
   }
   else
   {
      scm_desc_t desc; 

      desc.args[0] = address;
      desc.arginfo = SCM_ARGS(1);

      (void)scm_call2_STM(SCM_SIP_FNID(TZBSP_SVC_IO, TZBSP_IO_READ), &desc);

      val = desc.ret[0];
   }

   return val;
}



