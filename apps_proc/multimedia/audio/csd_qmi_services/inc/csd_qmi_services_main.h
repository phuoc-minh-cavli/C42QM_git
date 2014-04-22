#ifndef __AUDIO_CSD_QMI_SERVICES_MAIN_H__
#define __AUDIO_CSD_QMI_SERVICES_MAIN_H__
/*===========================================================================

 A U D I O _ C S D _ Q M I _ S E R V I C E _ M A I N   H E A D E R    F I L E

DESCRIPTION


Copyright(c) 2010 - 2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd_qmi_services/inc/csd_qmi_services_main.h#1 $
$DateTime: 2018/12/25 22:39:07 $ 
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/06/11    akj    Added header files
07/06/11    jp     Initial version.

===========================================================================*/
/****************************************************************************
  INCLUDE HEADER FILES
****************************************************************************/

#include <stddef.h>
#include <string.h>
#include "comdef.h"

#include "msg.h"
#include "err.h"


/****************************************************************************
   DEFINES
****************************************************************************/


/*===========================================================================
 Function Prototype

===========================================================================*/


int32_t audio_qmi_csd_services ( void );

void audio_qmi_csd_service_signal_run ( void );

extern void *qmi_csd_register_service(qmi_csi_os_params *os_params);

#endif /* __AUDIO_CSD_QMI_SERVICES_MAIN_H__*/

