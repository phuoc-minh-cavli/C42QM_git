#ifndef SCM_H
#define SCM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H E A D E R   F O R
                        S C M   D R I V E R

GENERAL DESCRIPTION
  This driver allows an Secure Monitor Call to be made into TrustZone.


EXTERNALIZED FUNCTIONS
  smc_call

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/api/products/scm/scm.h#1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
08/16/12     rs      First version 


===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <stddef.h>

#include "comdef.h"
#include "syscalls.h"

#define SCM_SIP_FNID(s, c)    (((((s) & 0xFF) << 8) | ((c) & 0xFF)) | 0x02000000)
#define SCM_QSEEOS_FNID(s, c) (((((s) & 0xFF) << 8) | ((c) & 0xFF)) | 0x32000000)
#define SCM_SVC_ID(s)         (((s) & 0xFF00) >> 8)

#define SCM_MAX_ARGS    10
#define SCM_MAX_RETURNS 3

/* TODO: for 32-bit only */
#define SCM_FLAG_HLOS   0x0

/* Flags used while setting cold/warm boot addresses */
#define SCM_FLAG_COLDBOOT_CPU1      0x01
#define SCM_FLAG_COLDBOOT_CPU2      0x08
#define SCM_FLAG_COLDBOOT_CPU3      0x20
#define SCM_FLAG_WARMBOOT_CPU1      0x02
#define SCM_FLAG_WARMBOOT_CPU0      0x04
#define SCM_FLAG_WARMBOOT_CPU2      0x10
#define SCM_FLAG_WARMBOOT_CPU3      0x40

typedef enum scm_arg_types {
   SCM_VAL,
   SCM_RO,
   SCM_RW,
   SCM_BUFVAL,
}scm_arg_type_t;

#define SCM_ARGS_IMPL(num, a, b, c, d, e, f, g, h, i, j, ...) (\
                         (((a) & 0xff) << 4) | \
                         (((b) & 0xff) << 6) | \
                         (((c) & 0xff) << 8) | \
                         (((d) & 0xff) << 10) | \
                         (((e) & 0xff) << 12) | \
                         (((f) & 0xff) << 14) | \
                         (((g) & 0xff) << 16) | \
                         (((h) & 0xff) << 18) | \
                         (((i) & 0xff) << 20) | \
                         (((j) & 0xff) << 22) | \
                         (num & 0xffff))

#define SCM_ARGS(...) SCM_ARGS_IMPL(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

/**
 * struct scm_desc
 * @arginfo: Metadata describing the arguments in args[]
 * @args: The array of arguments for the secure syscall
 * @ret: The values returned by the secure syscall
 * @extra_arg_buf: The buffer containing extra arguments (that don't fit in available registers)
 * @x5: The 4rd argument to the secure syscall or physical address of extra_arg_buf
*/
typedef struct scm_desc {
   uint32 arginfo;
   uint64 args[SCM_MAX_ARGS];
   uint64 ret[SCM_MAX_RETURNS];

   /* private */
   void   *extra_arg_buf;
   uint64 x5;
}scm_desc_t;

int scm_initialize(void);

int scm_call(uint32 svc_id, uint32 cmd_id, const void *cmd_buf, size_t cmd_len,
  void *resp_buf, size_t resp_len);
 
/*
 * To be used by error handler only -  not synchronized with mutex
 * should be called from single thread exec env
 */ 
int scm_call_STM(uint32 svc_id, uint32 cmd_id, const void *cmd_buf, size_t cmd_len,
  void *resp_buf, size_t resp_len);
  
int wdt_scm_call(const void *cmd_buf);
int wdt_scm_call_STM(const void *cmd_buf);
int scm_io_write(unsigned int address, unsigned int val);
int scm_io_write_STM(unsigned int address, unsigned int val);

unsigned int scm_io_read(unsigned int address);
unsigned int scm_io_read_STM(unsigned int address);

int scm_call2(uint32 cmd_id, scm_desc_t *desc);
int scm_call2_STM(uint32 cmd_id, scm_desc_t *desc);
int scm_is_armv8(void);
int scm_set_boot_addr(unsigned long addr, unsigned int flags);

#endif /* SCM_H */
