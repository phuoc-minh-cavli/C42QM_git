/*===========================================================================

           S Y S _ M . C

DESCRIPTION

Copyright (c) 2011-17 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/debugtools/sys_m/src/sys_m_common.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/

#include "dog.h"
#include "sys_m_internal.h"


#ifdef FEATURE_SMSM
#include "smsm.h"
#endif






void sys_m_spin(void)
{
#ifdef FEATURE_APPS_BREAK
    unsigned long smsm_state;
    smsm_state = smsm_state_get(SMSM_APPS_STATE);

    if (smsm_state & SMSM_MODEM_BREAK)
    {
        INTLOCK();
        while(smsm_state & SMSM_MODEM_BREAK)
        {
            smsm_state = smsm_state_get(SMSM_APPS_STATE);
            dog_kick();
        }
        INTFREE();
    }
#endif
}



