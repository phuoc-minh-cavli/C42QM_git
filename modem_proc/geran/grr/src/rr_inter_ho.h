#ifndef RR_INTER_HO_H
#define RR_INTER_HO_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               rr_inter_ho.h

GENERAL DESCRIPTION
   This module contains functions for dealing with intercell handoffs.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  
$Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_inter_ho.h#2 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
05/05/09   sjw     Reworked interfaces between G2G and G2W handover code
08/15/02   Rao     Changed the confusing function names. 
07/20/02   Rao     Added InterRAT ( WCDMA To GSM )  Handover support

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_GERAN_CS_SUPPORT
#include "rr_events.h"
#include "rr_task.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "geran_multi_sim.h"

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

extern rr_event_T rr_inter_cell_handover_control(
  rr_event_T,
  rr_cmd_bdy_type *
   
);

/**
  @brief Check whether an inter-cell handover is in progress.

  @return TRUE if an inter-cell handover is in progress, FALSE otherwise.
*/
extern boolean rr_inter_ho_handover_in_progress( );
extern void rr_set_handover_type(rr_l1_handover_type_T );
extern void send_handover_failure_ota(byte RR_cause  );

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_WCDMA)
extern boolean rr_inter_ho_w2g_starting( );
#endif /* FEATURE_DUAL_SIM && FEATURE_WCDMA */
#endif /* FEATURE_GERAN_CS_SUPPORT */

#endif /* #ifndef RR_INTER_HO_H */

/* EOF */


