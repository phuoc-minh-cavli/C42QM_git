#ifndef L2_RELEASE_H
#define L2_RELEASE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L 2   S E N D   H E A D E R   F I L E

DESCRIPTION
   Contains functions that send messages to L1 or RR.


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/L2/vcs/l2_release.h_v   1.2   07 Mar 2002 14:18:46   jachan  $
$Header: //components/rel/geran.mpss/5.2.0/gl2/src/l2_release.h#1 $ $DateTime: 2018/02/08 01:15:27 $ $Author: pwbldsvc $

when       who      what, where, why
--------   -------- ---------------------------------------------
10/19/01   JAC       Initial version.

===========================================================================*/
#ifdef FEATURE_GERAN_CS_SUPPORT
/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "rr_l2.h"

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

void       l2_release_link(l2_store_T *, SAPI_T, l2_channel_type_T);
void       l2_handle_disc_reception(l2_store_T *, SAPI_T, l2_channel_type_T);
l2_event_T l2_handle_release_ua_reception(l2_store_T *, SAPI_T, l2_channel_type_T);
l2_event_T l2_handle_release_dm_reception(l2_store_T *, SAPI_T, l2_channel_type_T);
l2_event_T l2_retransmit_disc_command(l2_store_T *, SAPI_T, l2_channel_type_T);
void       l2_send_dm_response(l2_store_T *, SAPI_T, l2_channel_type_T);
#endif /* FEATURE_GERAN_CS_SUPPORT */

#endif
