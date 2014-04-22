#ifndef __AUDIOINIT_H__
#define __AUDIOINIT_H__

/*===========================================================================

                    A U D I O   I N I T   S E R V I C E S

DESCRIPTION
  This header file defines APIs for initializing all audio-related modules.

REFERENCES
  None.

Copyright (C) 2011 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/audioinit/inc/audioinit.h#2 $
$DateTime: 2019/06/21 00:18:23 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/12   jt     Initial revision.

===========================================================================*/


/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/


/**
 * This is the audioinit function call entry-point.
 *
 * \param[in] index The function call to execute.
 * \param[in] params The parameters of the call.
 * \param[in] size The parameter size.
 *
 * \retval APR_EOK
 *   On success.
 *
 * \remarks
 *   TBD
 */
void audio_init ( void );
void aud_deinit ( void );

#ifdef FEATURE_APSS_AUDIO_MWS_TEST
  extern void audio_mws_diag_service_init( void );
#endif
#endif  /* !__AUDIOINIT_H__ */

