#ifndef RR_INIT_H
#define RR_INIT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               rr_init.h

GENERAL DESCRIPTION
   This module contains functions for the rr init function.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_init.h#2 $

===========================================================================*/

#include "rr_defs.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
extern void rr_init_geran_started( );
extern void rr_init_geran_stopped( );
extern void rr_init_modem_online( );
extern void rr_init_modem_offline( );
extern void rr_init_task_start_init(void);
extern void rr_init_recovery_init( );
extern void rr_gprs_initialise( );
extern void discard_all_rr_temporary_data( );

#endif
