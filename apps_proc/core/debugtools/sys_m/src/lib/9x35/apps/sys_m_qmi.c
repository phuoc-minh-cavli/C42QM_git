/*===========================================================================

           S Y S _ M . C

DESCRIPTION

Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/debugtools/sys_m/src/lib/9x35/apps/sys_m_qmi.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/

#include "sys_m.h"
#include "rex.h"
#include "rcinit.h"



/**
 *
 * @brief sys_m_qmi_task
 *
 * System Monitor main task
 *
 * @param param: Task init parameter
 *
*/
void sys_m_qmi_task(dword param)
{
    rex_sigs_type sigs;
    qmi_csi_os_params os_params;
    qmi_csi_service_handle service_handle = {NULL};

    os_params.tcb = rex_self();
    os_params.sig = SYS_M_QMI_SIG;
    
    service_handle = ssctl_qmi_ser_init(&os_params);
    
    //block for start signal
    rcinit_handshake_startup();    

    //task forever loop
    for (;;)
    {
        sigs = rex_wait(SYS_M_QMI_SIG);
        if (sigs & SYS_M_QMI_SIG)
        {
            rex_clr_sigs(rex_self(), SYS_M_QMI_SIG);
            qmi_csi_handle_event(service_handle, &os_params);
        }

    }
}
