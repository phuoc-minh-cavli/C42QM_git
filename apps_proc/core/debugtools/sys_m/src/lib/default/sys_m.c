/*===========================================================================

           S Y S _ M . C

DESCRIPTION

Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/debugtools/sys_m/src/lib/default/sys_m.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/

#include "task.h"
#include "rex.h"

#define SYS_M_DUMMY_SIG     0x0100

void sys_m_init(void)
{

}

void sys_m_task(dword param)
{
    rex_sigs_type sigs;
    //block for start signal
    tmc_task_start();
    
    //task forever loop
    for (;;)
    {
        sigs = rex_wait(SYS_M_DUMMY_SIG);
    }
 
}

void sys_m_reset(void)
{
}

void sys_m_shutdown(void)
{
}