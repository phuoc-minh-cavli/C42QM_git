/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M A I N   M O D U L E

GENERAL DESCRIPTION
  This module contains the AMSS exception handler

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright(c) 2007-2015 by Qualcomm Technologies, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //components/rel/core.tx/6.0/debugtools/err/src/err_exception_handler.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/15   din     Removed dead code and updated file.
===========================================================================*/


#include <stdio.h>
#include "target.h"     
#include "customer.h"   
#include "comdef.h"
#include "armasm.h"
#include "err.h"
#include "msg.h"
#include "errlog.h"
#include "tms_utils.h"
#include "qurt.h"

#if 0 /* Stubbed for ThreadX port */
#include "okl4/exception.h"
#include "okl4/syscalls.h"
#include <l4/ipc.h>
#include <l4/message.h>
#include <l4/cust/exception.h>
#include <l4/cust/msm_syscall.h>
#endif

typedef enum {
  EXCP_UNKNOWN,
  EXCP_SWI,
  EXCP_UNDEF_INST,
  EXCP_MIS_ALIGN,
  EXCP_PAGE_FAULT,
  EXCP_EXE_FAULT,
  EXCP_DIV_BY_0,
  EXCP_MAX_NUM
} exception_type;

char  exception_info[EXCP_MAX_NUM][ERR_QURT_TASK_NAME_LEN+1+1] = {
  "    :Excep  ",
  "    :No SWI ",
  "    :Undef  ",
  "    :MisAlgn",
  "    :PFault ",
  "    :XFault ",
  "    :DivBy0 ",
};

char              qxdm_dbg_msg[80];

extern coredump_type coredump;
void err_exception_handler( void );

/*===========================================================================
FUNCTION err_exception_handler

DESCRIPTION
  Handle IPC from L4 Kernel when exceptions occur.

===========================================================================*/
/* Variable name changes for QUARTZ_3X */
#define REGS_R14 regs.r14

void err_exception_handler(void)
{
  /* Stubbed for ThreadX port */
  //union arch_coredump_union *p_regs=&coredump.arch.regs;
  //struct okl4_exception_data exception_buf;
  //L4_Word_t fault_address, instruction_addr;
  //arm_irq_context_t regs;


  unsigned int instruction_addr, sp, badva, cause;
  /* Thrad ID of the faulting thread */
  unsigned int falter_thread;
  
  for (;;)
  {
	falter_thread = qurt_exception_wait(&instruction_addr,
		                                &sp,
		                                &badva,
		                                &cause);    

    err_fatal_lock();

	qurt_thread_suspend(falter_thread);

    if(0 > tms_utils_fmt(qxdm_dbg_msg, sizeof(qxdm_dbg_msg),
       "ExIPC: Page Fault @ %lx, and instruction addr: %x, TID: %x", badva, instruction_addr, falter_thread)){;}
    MSG_3(MSG_SSID_TMS, MSG_LEGACY_FATAL,"ExIPC: Page Fault @ %lx, and instruction addr: %x, TID: %x", badva, instruction_addr, falter_thread);

    #if 0 
    /* System call returns register context at the time of crash
      Copy them to coredump */
    L4_MSM_GET_SAVED_REGS(falter_thread, &regs);   

    p_regs->array[ARM_SVC_R0] = regs.r0;
    p_regs->array[ARM_SVC_R1] = regs.r1;
    p_regs->array[ARM_SVC_R2] = regs.r2;
    p_regs->array[ARM_SVC_R3] = regs.r3;
    p_regs->array[ARM_SVC_R4] = regs.r4;
    p_regs->array[ARM_SVC_R5] = regs.r5;
    p_regs->array[ARM_SVC_R6] = regs.r6;
    p_regs->array[ARM_SVC_R7] = regs.r7;
    p_regs->array[ARM_SVC_R8] = regs.r8;
    p_regs->array[ARM_SVC_R9] = regs.r9;
    p_regs->array[ARM_SVC_R10] = regs.r10;
    p_regs->array[ARM_SVC_R11] = regs.r11;
    p_regs->array[ARM_SVC_R12] = regs.r12;
     /* PC and the R13 will be saved above so dont need to save it here */
    p_regs->array[ARM_SYS_LR] = REGS_R14;
    p_regs->array[ARM_CPSR] = regs.cpsr;
	#endif

    err_fatal_jettison_core(instruction_addr, 0, "Exception detected", 0, 0,0);
  }

} /* end of err_exception_handler */

