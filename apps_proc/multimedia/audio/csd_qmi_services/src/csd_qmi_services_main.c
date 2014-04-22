/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     M I D D L E W A R E   S E R V I C E S

GENERAL DESCRIPTION
 	This file comprises of all the audio system middleware services related
 	functions and tasks creation.

Copyright(c) 2011 - 2012, 2016 by Qualcomm Technologies, Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*=========================================================================
                     EDIT HISTORY FOR FILE
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
$Header: //components/rel/audio.tx/2.0/audio/csd_qmi_services/src/csd_qmi_services_main.c#1 $
$DateTime: 2018/12/25 22:39:07 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/16    rk     Abstracting OSAL layer
09/16/11    jp     Added FTM registration with Diag init
08/03/11    jp     Initial version.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include <stddef.h>
#include "err.h"
#include "task.h"
#include "csd_os_dependencies.h"
#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "csd_debug_msg.h"
#include "csd.h"
#include "csd_qmi_services_main.h"

qurt_signal_t csd_qmi_service_signal;

extern void *qmi_csd_register_service(qmi_csi_os_params *os_params);

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/
#define QMI_CSD_SVC_WAIT_SIG 0x00000100

/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/
/*
typedef enum audio_qmi_csd_service_thread_state_enum_t
{
  AUDIO_QMI_CSD_SERVICE_THREAD_STATE_ENUM_INIT,
  AUDIO_QMI_CSD_SERVICE_THREAD_STATE_ENUM_READY,
  AUDIO_QMI_CSD_SERVICE_THREAD_STATE_ENUM_EXIT
}
  audio_qmi_csd_service_thread_state_enum_t;

static audio_qmi_csd_service_thread_state_enum_t audio_qmi_csd_service_task_state = AUDIO_QMI_CSD_SERVICE_THREAD_STATE_ENUM_INIT;

*/


/*!===========================================================================
  COMMON INTERNAL ROUTINES   
 =========================================================================== */

/*===========================================================================

FUNCTION audio_qmi_csd_service_task

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS 

===========================================================================*/

void audio_csd_qmi_service_task (uint32 handle)
{
  qmi_csi_os_params os_params;
  void *sp;

  rcinit_handshake_startup();

//  tmc_task_start();

  os_params.signal= &csd_qmi_service_signal;
  os_params.sig = QMI_CSD_SVC_WAIT_SIG;

  qurt_signal_init(&csd_qmi_service_signal);

  sp = qmi_csd_register_service(&os_params);

  if(!sp)
  {
    ERR_FATAL("QMI_CSD:audio_qmi_csd_service_task:Unable to start audio QMI CSD service!", 0,0,0);
  }

  //audio_qmi_csd_service_task_state = AUDIO_QMI_CSD_SERVICE_THREAD_STATE_ENUM_READY;
  /* init CSD in case client does not all handles will not be NULL*/

  while(1)
  {
    qurt_signal_wait( os_params.signal, QMI_CSD_SVC_WAIT_SIG, QURT_SIGNAL_ATTR_WAIT_ANY);
    qurt_signal_clear( os_params.signal, QMI_CSD_SVC_WAIT_SIG);
    qmi_csi_handle_event(sp, &os_params);
    CSD_MSG( CSD_PRIO_LOW, "audio_csd_qmi_service_task: Thread Running" );
  }
  
  //return;
}
