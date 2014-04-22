#ifndef ERRLOG_ARMV7_H
#define ERRLOG_ARMV7_H
/*===========================================================================

                   L O G  P A C K E T S  F O R  E R R

DESCRIPTION
  This header file contains the definitions of log structure for core dump
 
Copyright (c) 2015   by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/err/src/errlog_armv7.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/07   tbg     Initial revision (pulled code from rexlog.h)

===========================================================================*/


/************************************************************************
 *                        ARCH_COREDUMP_TYPES
 ************************************************************************/

/****************
 *    ARM
 ****************/

typedef enum
{
  ARM_SVC_R0 = 0,
  ARM_SVC_R1,
  ARM_SVC_R2,
  ARM_SVC_R3,
  ARM_SVC_R4,
  ARM_SVC_R5,
  ARM_SVC_R6,
  ARM_SVC_R7,
  ARM_SVC_R8,
  ARM_SVC_R9,
  ARM_SVC_R10,
  ARM_SVC_R11,
  ARM_SVC_R12,
  ARM_SVC_SP,
  ARM_SVC_LR,
  ARM_SVC_SPSR,
  ARM_SVC_PC,
  ARM_SYS_SP,
  ARM_SYS_LR,
  ARM_IRQ_SP,
  ARM_IRQ_LR,
  ARM_IRQ_SPSR,
  ARM_ABT_SP,
  ARM_ABT_LR,
  ARM_ABT_SPSR,
  ARM_UDF_SP,
  ARM_UDF_LR,
  ARM_UDF_SPSR,
  ARM_FIQ_R8,
  ARM_FIQ_R9,
  ARM_FIQ_R10,
  ARM_FIQ_R11,
  ARM_FIQ_R12,
  ARM_FIQ_SP,
  ARM_FIQ_LR,
  ARM_FIQ_SPSR,
  ARM_JVA_SP,
  ARM_JVA_LR,
  ARM_JVA_SPSR,
  ARM_CPSR,
  SIZEOF_ARCH_COREDUMP_REGISTERS
} arch_coredump_register_type;

#define SIZEOF_SVC_REGS 

typedef struct
{
  uint32 regs[SIZEOF_ARCH_COREDUMP_REGISTERS];
} arch_coredump_array_type;

typedef struct
{
  uint32 svc_regs[13]; /* r0-r12 */
  uint32 svc_sp;
  uint32 svc_lr;
  uint32 svc_spsr;
  uint32 pc;
  uint32 sys_sp;
  uint32 sys_lr;
  uint32 irq_sp; 
  uint32 irq_lr;
  uint32 irq_spsr;  
  uint32 abt_sp;
  uint32 abt_lr;
  uint32 abt_spsr;
  uint32 udf_sp;
  uint32 udf_lr;
  uint32 udf_spsr;
  uint32 fiq_r8; 
  uint32 fiq_r9;
  uint32 fiq_r10;
  uint32 fiq_r11;
  uint32 fiq_r12;
  uint32 fiq_sp;
  uint32 fiq_lr;
  uint32 fiq_spsr;
  uint32 jva_sp;
  uint32 jva_lr;
  uint32 jva_spsr;
  uint32 cpsr;
} arch_coredump_field_type;

union arch_coredump_union
{
  uint32 array[SIZEOF_ARCH_COREDUMP_REGISTERS];
  arch_coredump_field_type name;
};


#endif /* ERRLOG_ARMV7_H */
