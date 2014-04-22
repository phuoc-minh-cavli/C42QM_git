
/*==============================================================================

                      TMS -> BOOT DLOAD MODE CONTROLS

  DESCRIPTION
    Internal implementation of the boot team's download magic number scheme.
    (if you want a public version talk to the boot team) 
 
    refer to "go/shared_imem" or similar for addresses and details.

  Copyright (c) 2010 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/debugtools/internal/tms_dload/src/tms_dload.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
10/29/14     rks     CR745365:set dload cookie by SCM call
10/10/12     mcg     creating based on dload_entry.c
==============================================================================*/

#include "dog.h"
#include "HALhwio.h"
#include "DDIChipInfo.h"
#include "err.h"
#include "tz_syscalls.h"
#include "scm.h"
#include "qurt.h"

/*=========================================================================
     9x25v1 Sparrow HWIO
     -holds true for 9x35
==========================================================================*/

/*----------------------------------------------------------------------------
 * MODULE: MPM2_PSHOLD
 *--------------------------------------------------------------------------*/

#define MPM2_PSHOLD_REG_BASE                       (MPM2_MPM_BASE      + 0x0000b000)

#define HWIO_MPM2_MPM_PS_HOLD_ADDR                 (MPM2_PSHOLD_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_PS_HOLD_RMSK                        0x1
#define HWIO_MPM2_MPM_PS_HOLD_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PS_HOLD_ADDR, HWIO_MPM2_MPM_PS_HOLD_RMSK)
#define HWIO_MPM2_MPM_PS_HOLD_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PS_HOLD_ADDR, m)
#define HWIO_MPM2_MPM_PS_HOLD_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PS_HOLD_ADDR,v)
#define HWIO_MPM2_MPM_PS_HOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PS_HOLD_ADDR,m,v,HWIO_MPM2_MPM_PS_HOLD_IN)
#define HWIO_MPM2_MPM_PS_HOLD_PSHOLD_BMSK                 0x1
#define HWIO_MPM2_MPM_PS_HOLD_PSHOLD_SHFT                 0x0


#define TMS_DLOAD_MAGIC_NUM  0x10

// Depends upon above...which need to be moved to their own internal HAL layer
#include "msmhwiobase.h"

//configure TZ
tz_config_hw_for_ram_dump_req_t tms_tz_config;

extern void dog_force_bite(void);
extern unsigned int qurt_fiq_lock_save(void);

void tms_dload_write_magic_numbers(void)
{
   sec_dload_mode_buffer_t cmd_buffer = {TMS_DLOAD_MAGIC_NUM, 0x0};
   tz_config_dload_mode_syscall (&cmd_buffer);
}

void tms_dload_write_magic_numbers_STM(void)
{
   sec_dload_mode_buffer_t cmd_buffer = {TMS_DLOAD_MAGIC_NUM, 0x0};
   tz_config_dload_mode_syscall_STM (&cmd_buffer);

}

void tms_dload_clear_magic_numbers(void)
{
   sec_dload_mode_buffer_t cmd_buffer = {0,0};
   tz_config_dload_mode_syscall (&cmd_buffer);
}

void tms_dload_clear_magic_numbers_STM(void)
{
   sec_dload_mode_buffer_t cmd_buffer = {0,0};
   tz_config_dload_mode_syscall_STM (&cmd_buffer);
}

void tms_dload_reset(void)
{
   unsigned int tms_int_flag = 0;
   tms_dload_write_magic_numbers();

   /* disable interrupts */
   INTLOCK_SAVE(tms_int_flag); 

   /* Disable FIQ as well */
   /* Not saving the context since we will not be needing to re-enable it */
   (void)qurt_fiq_lock_save();

   dog_force_bite();
   
   /* Intentionally petting dog */
   dog_force_kick();
   
   /* For supressing compiler warning errors */
   tms_int_flag = tms_int_flag+1;

   /* Halt the execution here */
   while(1){};
}

void tms_dload_reset_STM (void)
{
   unsigned int tms_int_flag = 0;
   tms_dload_write_magic_numbers_STM();

   /* disable interrupts */
   INTLOCK_SAVE(tms_int_flag); 

   /* Disable FIQ as well */
   /* Not saving the context since we will not be needing to re-enable it */
   (void)qurt_fiq_lock_save();

   dog_force_bite();

   /* For supressing compiler warning errors */   
   tms_int_flag = tms_int_flag+1;
}


