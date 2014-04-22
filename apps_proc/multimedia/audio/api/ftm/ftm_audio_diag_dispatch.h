#ifndef FTM_AUDIO_DIAG_DISPATCH_H
#define FTM_AUDIO_DIAG_DISPATCH_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         F T M  A U D I O   D I A G   D I S P A T C H

GENERAL DESCRIPTION
  This module will supply functions to register with
  the diagnostic monitor and dispatch on specific 
  FTM audio commands.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2004-2011 by QUALCOMM Technologies INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/api/ftm/ftm_audio_diag_dispatch.h#1 $
  $DateTime: 2018/12/25 22:39:07 $ 
  $Author: pwbldsvc $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/15/09   zl     Initial revision
===========================================================================*/
#include "DALStdDef.h"

#include "diagpkt.h"


#define FTM_AUDIO_C 14

PACKED void * ftm_audio_dispatch( PACKED void * request, uint16 length );
void ftm_audio_init(void);

#endif /* FTM_AUDIO_DIAG_DISPATCH_H */

