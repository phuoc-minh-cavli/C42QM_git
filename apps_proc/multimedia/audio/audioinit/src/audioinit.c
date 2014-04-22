/*===========================================================================

                    A U D I O   I N I T   S E R V I C E S

DESCRIPTION
  This header file defines APIs for initializing all audio-related modules.

REFERENCES
  None.

Copyright (C) 2011 - 2012 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/audioinit/src/audioinit.c#3 $
$DateTime: 2019/06/21 00:18:23 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/14   vk     Added VoUSB feature support
06/12/12   jt     Initial revision.
===========================================================================*/

#include "audioinit.h"
#include "csd_debug_msg.h"
#include "csd.h"
#include "csdlite.h"
#include "audio_diag_i.h"

#ifdef FEATURE_AUDIO_RTPD_SERVICE
#include "rtpd_services.h"
#include "rtpd_status.h"
#include "rtpd_debug_msg.h"
#endif


#ifdef FEATURE_APSS_AUDIO_FTM
#include "ftm_audio_diag_dispatch.h"
#endif //FEATURE_APSS_AUDIO_FTM
/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/
void audio_init ( void )
{
  uint32_t rc = CSD_EOK;
#ifdef FEATURE_AUDIO_RTPD_SERVICE
  uint32_t rtpd_rc = RTPD_EOK;
#endif
  
  /* init CSD in case client does not all handles will not be NULL*/
  //rc = csd_init();
  csd_pmic_gpiotwo_on();

  if ( rc == CSD_EOK )
  {
    CSD_MSG_ARG_1(CSD_PRIO_ERROR, "audio_init:csd_init FAILURE %d", rc );
  } 
  else
  {
    CSD_MSG_ARG_0(CSD_PRIO_HIGH,"audio_init:csd_init SUCCESS " );
  }

  if( rc == CSD_EOK )
  {
    //rc = csdlite_init();
    if ( rc )
    {
      CSD_MSG_ARG_1(CSD_PRIO_ERROR, "audio_init:csdlite_init FAILURE %d", rc );
    } 
    else
    {
      CSD_MSG_ARG_0(CSD_PRIO_HIGH,"audio_init:csdlite_init SUCCESS " );
    }
  }

#ifdef FEATURE_AUDIO_RTPD_SERVICE
  rtpd_rc = rtpd_service_init();

  if(rtpd_rc != RTPD_EOK)
  {
    RTPD_MSG( RTPD_PRIO_ERROR, "audio_init:rtpd_init failed, rc = 0x%x", rtpd_rc );
  }
  else
  {
    RTPD_MSG( RTPD_PRIO_HIGH, "audio_init:rtpd_init success, rc = 0x%x", rtpd_rc );
  }
#endif

  ( void )audio_diag_init( );

}
/* Added for Twizy - Deinit Just after Boot -Up  */
void aud_deinit( void )
{
  csd_pmic_gpiotwo_off();
}

