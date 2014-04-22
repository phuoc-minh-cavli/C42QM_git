/*===========================================================================

           S Y S _ M . C

DESCRIPTION

Copyright (c) 2011-17 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/debugtools/sys_m/src/sys_m_qmi.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/

#include "sys_m_internal.h"
#include "qurt.h"
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
    qurt_signal_t signal;
    qmi_csi_os_params os_params_v02;
    qmi_csi_service_handle service_v02_handle;
	uint32 sigs;

	qurt_signal_init(&signal);

    os_params_v02.signal = &signal;
    os_params_v02.sig = SYS_M_QMI_v02_SIG;
    
    service_v02_handle = ssctl_v02_qmi_ser_init(&os_params_v02);
    //block for start signal
    rcinit_handshake_startup();    

    //task forever loop
    for (;;)
    {
        sigs = qurt_signal_wait(&signal, SYS_M_QMI_v02_SIG, QURT_SIGNAL_ATTR_WAIT_ANY);
        if (sigs & SYS_M_QMI_v02_SIG)
        {
            qurt_signal_clear(&signal, SYS_M_QMI_v02_SIG);
            qmi_csi_handle_event(service_v02_handle, &os_params_v02);
        }

    }
}
