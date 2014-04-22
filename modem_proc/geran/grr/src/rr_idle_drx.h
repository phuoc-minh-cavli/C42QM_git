#ifndef RR_IDLE_DRX_H
#define RR_IDLE_DRX_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             rr_idle_drx.h

GENERAL DESCRIPTION
   This module contains functions for dealing with Idle DRX change.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2015 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_conn_establish.h_v   1.1   08 Jul 2002 19:26:28   jachan  $
$Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_idle_drx.h#2 $ $DateTime: 2018/10/10 02:31:27 $ $Author: pwbldsvc $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
26/04/15    TS      Initial Revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_task.h"
#include "rr_events.h"
#include "geran_multi_sim.h"

#ifdef FEATURE_IDLE_DRX_SCALING
/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

extern void rr_idle_drx_init( );
extern boolean rr_idle_drx_start_scaling_suspend_timer( );
extern byte rr_idle_drx_get_extended_dsc( );
extern boolean rr_idle_drx_scaling_in_use( );
extern boolean rr_idle_drx_is_nmeas_off( );
extern void rr_idle_drx_check_dsc(const int dsc_max, const int dsc  );
extern void rr_idle_drx_bs_pa_mfrms_changed(const int old_dsc_max  );
extern void rr_idle_drx_control(rr_event_T drx_event, rr_cmd_bdy_type* msg_ptr  );
#endif /* FEATURE_IDLE_DRX_SCALING */

#endif /* RR_IDLE_DRX_H */

/* EOF */

