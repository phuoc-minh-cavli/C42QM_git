#ifndef RR_CIPHER_H
#define RR_CIPHER_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               rr_cipher.h

GENERAL DESCRIPTION
   This module contains functions for dealing with ciphers.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_cipher.h#2 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#ifdef FEATURE_GERAN_CS_SUPPORT
#include "sys_cnst.h"
#include "environ.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "mm_rr.h"
#include "timers.h"
#include "rr_defs.h"
#include "geran_multi_sim.h"

/*===========================================================================

                         CONSTANT DECLARATIONS

===========================================================================*/
#define RR_CIPHER_KEY_SEQ_NOT_AVAILABLE 0x07
#define OPTIONAL_MOBILE_IDENTITY_IEI    0x17

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

extern rr_event_T rr_cipher_control(rr_event_T, void * );
extern boolean    rr_cipher_mode_supported(cipher_algorithm_T );
extern void       rr_confirm_ciphering_completed( );
extern void       rr_set_ciphering_mode_command_received(boolean );
extern boolean    rr_get_ciphering_mode_command_received( );
extern byte       rr_get_cipher_key_sequence_number( );
extern void       rr_ciphering_has_started(boolean ciphering_started );
extern boolean    rr_has_ciphering_started( );
#endif /* FEATURE_GERAN_CS_SUPPORT */

#endif /* RR_CIPHER_H */
