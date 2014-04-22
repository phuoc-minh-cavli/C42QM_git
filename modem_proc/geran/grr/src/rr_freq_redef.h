#ifndef RR_FREQ_REDEF_H
#define RR_FREQ_REDEF_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              rr_freq_redef.h

GENERAL DESCRIPTION
   This module contains functions for dealing with frequency redefinitions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_freq_redef.h#2 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_GERAN_CS_SUPPORT
#include "sys_cnst.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "mm_rr.h"
#include "timers.h"
#include "rr_defs.h"
#include "geran_multi_sim.h"

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

rr_event_T rr_handle_frequency_redefinition_msg( void );
rr_event_T rr_handle_frequency_redefinition_ind(void *  );
#endif /* FEATURE_GERAN_CS_SUPPORT */

#endif

