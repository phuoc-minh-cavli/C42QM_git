#ifndef LLDB_PORT_H
#define LLDB_PORT_H
#endif
/*======================================================================
                        lldb_port.h 

GENERAL DESCRIPTION
  LLDB macro and API mapping (port) definitions

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2018  by Qualcomm Technologies, Inc.  All Rights Reserved.
 ======================================================================*/
/*======================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 03/12/2018   AJ      Initial Version 
 ======================================================================*/

#define LLDB_SERVER_THREAD_PRIO  18
#define	LLDB_SERVER_THREAD_STACK_SIZE	8*1024

#define MAX_NUM_ARCH_ADDRESS_BYTES 8
#define MAX_RSP_BUF_SIZE 890

#define PT_READ                     4
#define PT_WRITE                    2

#define BKPT_SET_SIGNAL 		0x11DB
#define BKPT_CONTINUE_SIG		0X12DB

typedef unsigned int uint32_t;
//typedef unsigned int uint32;
typedef unsigned char uint8_t;
typedef unsigned int pid_t;
typedef unsigned int addr_t;


#ifndef TX_KERNEL
#define TX_KERNEL
#endif

#ifdef TX_KERNEL

#include "qurt_types.h"
#include "qurt_init.h"
#include "qurt_error.h"
#include "qurt_thread.h"
#include "qurt_timer.h"
//#include "qurt_internal.h"
#include "qurt_system.h"
#include "qurt_memory.h"
#include "qurt_mutex.h"
#include "qurt_pipe.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define lldb_thread_t						qurt_thread_t
#define	lldb_thread_attr_t					qurt_thread_attr_t
#define lldb_thread_create					qurt_thread_create
#define lldb_thread_attr_init				qurt_thread_attr_init
#define lldb_thread_attr_set_name			qurt_thread_attr_set_name
#define lldb_thread_attr_set_priority		qurt_thread_attr_set_priority
#define lldb_thread_attr_set_stack_size		qurt_thread_attr_set_stack_size

#define lldb_signal_t						qurt_signal_t
#define lldb_signal_wait					qurt_signal_wait
#define lldb_signal_set						qurt_signal_set
#define	lldb_signal_init					qurt_signal_init
#define lldb_signal_clear 					qurt_signal_clear

#define lldb_mutex_t 						qurt_mutex_t
#define lldb_mutex_lock						qurt_mutex_lock
#define lldb_mutex_unlock					qurt_mutex_unlock
#define lldb_mutex_init						qurt_mutex_init

#define lldb_pipe_attr_t					qurt_pipe_attr_t
#define lldb_pipe_t							qurt_pipe_t
#define lldb_pipe_attr_init					qurt_pipe_attr_init
#define lldb_pipe_attr_set_element_size   	qurt_pipe_attr_set_element_size
#define lldb_pipe_attr_set_elements   		qurt_pipe_attr_set_elements
#define lldb_pipe_send						qurt_pipe_send
#define lldb_pipe_receive					qurt_pipe_receive
#define lldb_pipe_create					qurt_pipe_create



#define lldb_thread_get_id 					qurt_thread_get_id
#define lldb_thread_sleep 					qurt_thread_sleep

#define LLDB_MODULE_INSTANCE				TXM_MODULE_INSTANCE
#define LLDB_MODULE_PREAMBLE				TXM_MODULE_PREAMBLE
#define LLDB_MAX_MODULES					TXM_MAXIMUM_MODULES


#define LLDB_FALSE							0
#define LLDB_TRUE							1

#define RESP_BUFF_SIZE						1500
#define RX_BUFF_MAX_SIZE					(14*sizeof(int))
#define TX_BUFF_MAX_SIZE					1000
#define LLDB_MAX_CMD_QUEUE_LENGTH			32

#define NUM_GPR_REGS 16
#define MAX_SUPPORTED_SW_BREAKPOINTS 100
#define MAX_MODULE_THREADS		 16


#define STRLEN                   strlen
#define STRCMP                   strcmp
#define STRNCMP                  strncmp
#define STRCPY                   strcpy
#define STRNCPY                  strncpy

#define SNPRINTF 				 snprintf
#define SPRINTF 				 sprintf


#define streq(A,B) !STRNCMP(A, B, STRLEN(B))

#define RPC_INDEX 	15
#define RLR_INDEX 	14
#define RFP_INDEX 	11

#define RSP_INDEX 	13
#define ARM_ACCESS_KRWX_URWX 6UL


#define OPCODE_B_BL_IMM_SHIFT               (25)
#define OPCODE_B_BL_IMM_MASK                (0x7 << OPCODE_B_BL_IMM_SHIFT)
#define OPCODE_B_BL_IMM_VAL                 (0x5) 
#define OP_BLK_TXR_SHIFT                    (20)
#define OP_BLK_TXR_MASK                     (0x3D << OP_BLK_TXR_SHIFT)



#define tolowerK(c)             (c|32)
#define is_decimal_char(c)      ((c >= '0') && (c <= '9'))

typedef void (*fp_entry)(void *);

static inline uint8_t hex2val(char ch)
{
    ch = tolowerK(ch);
    if (is_decimal_char(ch))
    {
        return (ch - '0');
    }
    else if ((ch <= 'f') && (ch >= 'a'))
    {
        return (0xa + ch - 'a');
    }
    else
    {
        return 0;
    }
}

#define WORD_ALIGN(addr)  (addr + (sizeof(uint32_t) - (addr % sizeof(uint32_t))))

#elif defined(Q6_KERNEL)


#endif

