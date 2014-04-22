#ifndef TASK_H
#define TASK_H

/*===========================================================================

            T A S K    R E S O U R C E S    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations defining global task resources.

Copyright (c) 1990-2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/debugtools/task.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

////////////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////////////

#include "rcevt.h"
#include "rcinit.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* Commenting for threadX compatibility */
#if 0
////////////////////////////////////////////////////////////////////////
// DATA DECLARATIONS
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// This mechanism is utilized to deprecate TASK associated symbols at
// compile time. It facilitates the identification of dead code references.
////////////////////////////////////////////////////////////////////////

#define TASK_DEPRECATED_ATTRIBUTE              __attribute__((deprecated))
#define TASK_DEPRECATED_ATTRIBUTE_TCB          /* __attribute__((deprecated)) */

////////////////////////////////////////////////////////////////////////
// TASK DATA AND NAME DECLARATION
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// The following symbols are considered internal TASK implementation
// details. Do not make assumptions on future implementation details;
// do not utilize the internal APIS.
////////////////////////////////////////////////////////////////////////

#define TASK_REGISTER_SYMBOL(x, y)              static const unsigned long x = y;
#define TASK_DEPRECATED_REGISTER_SYMBOL(x, y)   TASK_DEPRECATED_ATTRIBUTE TASK_REGISTER_SYMBOL(x, y)
#define TASK_INTERNAL_AMSS_MAP_PRIO(x)          (((x) < (AMSS_MAX_TASK_PRI - 1)) ? ((AMSS_MAX_TASK_PRI > (x)) ?  (AMSS_MAX_TASK_PRI - (x)) : 1) : 0)
#define TASK_INTERNAL_STACK_SIZ_PAD             0                                                     /* Stack Size Pad for OS Requirements */
#define TASK_INTERNAL_STACK_ROUND_SIZ           10                                                    /* (2**10) == 1KB */
#define TASK_INTERNAL_STACK_ROUND(x)            (((x) + (1<<TASK_INTERNAL_STACK_ROUND_SIZ) - 1) & ~((1<<TASK_INTERNAL_STACK_ROUND_SIZ) - 1))

#define TASK_INTERNAL_EXTERN(t_name)         extern rex_tcb_type t_name##_tcb TASK_DEPRECATED_ATTRIBUTE_TCB;
#define TASK_INTERNAL_HANDLE(t_name)         (rcinit_lookup_rextask(#t_name))        // Runtime Accessor
#define TASK_INTERNAL_STACK(t_name)          (rcinit_lookup_stack(#t_name))         // Runtime Accessor : Stack should be private
#define TASK_INTERNAL_STKSZ(t_name)          (rcinit_lookup_stksz(#t_name)/sizeof(rex_stack_word_type)) // Runtime Accessor : Stack should be private
#define TASK_INTERNAL_MAIN(t_name)           (rcinit_lookup_entry(#t_name))         // Runtime Accessor
#define TASK_INTERNAL_PRIO(t_name)           (rcinit_lookup_prio(#t_name))          // Runtime Accessor

enum TASK_SIG
{
   TASK_SIG_DEFAULT_START                       = 0x00008000,
   TASK_SIG_DEFAULT_OFFLINE                     = 0x00002000,
   TASK_SIG_DEFAULT_STOP                        = 0x00004000
};

#define TASK_STACKPAD(x)                        TASK_INTERNAL_STACK_ROUND(x)           // Mapping to HLOS Stack Pad
#define TASK_PRIORITY(x)                        TASK_INTERNAL_AMSS_MAP_PRIO(x)         // Mapping to HLOS Priority
#define TASK_NAME(t_name)                       (#t_name)
#define TASK_MAIN(t_name)                       TASK_INTERNAL_MAIN(t_name)             // Runtime Accessor
#define TASK_STACK(t_name)                      TASK_INTERNAL_STACK(t_name)            // Runtime Accessor

////////////////////////////////////////////////////////////////////////
// TASK PUBLIC API
////////////////////////////////////////////////////////////////////////

#define TASK_ARGV_NIL                           0                                      // Indicate NO Task Argument
#define TASK_NODOG_RPT                          -1                                     // Indicate NO Dog Report Value

#define TASK_EXTERN(t_name)                     TASK_INTERNAL_EXTERN(t_name)           // Runtime Accessor
#define TASK_HANDLE(t_name)                     TASK_INTERNAL_HANDLE(t_name)           // Runtime Accessor
#define TASK_PRIO(t_name)                       TASK_INTERNAL_PRIO(t_name)             // Runtime Accessor
#define TASK_STKSZ(t_name)                      TASK_INTERNAL_STKSZ(t_name)            // Runtime Accessor

////////////////////////////////////////////////////////////////////////
// COMMON SIGNALS DEFINITIONS
////////////////////////////////////////////////////////////////////////

// The following signal masks only have context between TASK/TMC and its
// cilents -- every task. They are captured here as part of the TASK
// API that the clients utilize to signal state transition within TMC.

#define TASK_START_SIG                          TASK_SIG_DEFAULT_START
#define TASK_OFFLINE_SIG                        TASK_SIG_DEFAULT_OFFLINE
#define TASK_STOP_SIG                           TASK_SIG_DEFAULT_STOP

// The following signals only have context between the receiving task server
// and the signaling client. These signaling masks are an API declaration between
// the technology team and their clients. The mask should be collected with the
// API that defines the context the signal mask is utilized in.

enum TASK_DEPRECATED_SIG
{
   FS_OP_COMPLETE_SIG                           = 0x40000000,                 // FS NEEDS TO PRIVATIZE THIS CONSTANT TO THEIR DELIVERY
   TASK_SIG                                                                   // list terminator

} ;

////////////////////////////////////////////////////////////////////////
// L4 SPECIFIC DECLARATIONS
////////////////////////////////////////////////////////////////////////

// These constants should be associated with a kernel provided interface;
// Any kernel provided interface should be referenced by TASK and portable.

enum TASK_DEPRECATED_HLOS_PRI
{
   ROOT_PRIORITY_RESERVED     = 246,
   TRAMP_SERVER_PRI           = (ROOT_PRIORITY_RESERVED - 1),
   AMSS_MAX_TASK_PRI          = (REX_MAX_PRIO - 15),                          // Task Required Symbol
   TASK_HLOS_PRI                                                              // list terminator

} ;

////////////////////////////////////////////////////////////////////////
// LEGACY EXTERNAL DEFINITIONS
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// DEPRECATED DECLARATIONS AND TYPES
////////////////////////////////////////////////////////////////////////

// TECH TEAMS NEED TO USE REX_SELF() TO OBTAIN THE HANDLE TO CONTEXT AT RUNTIME.

TASK_EXTERN(diag)
TASK_EXTERN(dog)
TASK_EXTERN(fs)
TASK_EXTERN(timer)

// TECH TEAMS NEED TO USE THE RCINIT_LOOKUP*() FUNCTIONS TO OBTAIN INFORMATION AT RUNTIME.

#define DIAG_PRI                          TASK_PRIO(diag)
#define FS_BENCHMARK_PRI                  TASK_PRIO(fs_benchmark)
#define FS_CPU_HOG_PRI                    TASK_PRIO(fs_cpu_hog)
#define SMD_PRI                           TASK_PRIO(smd)
#define SMD_TEST_PRI                      TASK_PRIO(smd)

#define SLEEP_PRI       TASK_PRIO(sleep)
#define TMC_PRI         0
#define TMC_STACK_SIZ   0

#endif /* #if 0 */

////////////////////////////////////////////////////////////////////////
// DEPRECATED FUNCTIONS
////////////////////////////////////////////////////////////////////////

// TECH TEAMS NEED TO REMOVE REFERENCE TO THE DEPRECATED API.

#include "err.h"
#define task_ack()        ERR_FATAL("TMC/TASK API not present", 0, 0, 0)
#define task_start(x,y,z) ERR_FATAL("TMC/TASK API not present", 0, 0, 0)
#define task_offline()    ERR_FATAL("TMC/TASK API not present", 0, 0, 0)
#define task_stop()       ERR_FATAL("TMC/TASK API not present", 0, 0, 0)

#if defined(__cplusplus)
}
#endif

#endif
