#ifndef SYS_M_INTERNAL_H
#define SYS_M_INTERNAL_H
/*===========================================================================

           SYS_M_INTERNAL . H

DESCRIPTION

Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.10/debugtools/api/sys_m_internal.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/



#define SYS_M_ACK                            "ssr:ack"
#define SYS_M_SHUTDOWN                       "ssr:shutdown"
#define SYS_M_SHUTDOWN_ACK                   SYS_M_ACK

//function for error handler to initialize SFR buffer
void *sys_m_init_sfr_buffer(void);

#endif  /* SYS_M_INTERNAL_H */
